/* AddText
 * Example NFC application which posts a long sentence to the RF430CL330H
 * NFC Tag controller
 *
 * Written by Eric Brundick 2/10/2015
 */

#include <Wire.h>
#include <RF430CL.h>
#include <NDEF.h>
#include <NDEF_TXT.h>

#define RF430CL330H_BOOSTERPACK_RESET_PIN  8
#define RF430CL330H_BOOSTERPACK_IRQ_PIN    12

RF430 nfc(RF430CL330H_BOOSTERPACK_RESET_PIN, RF430CL330H_BOOSTERPACK_IRQ_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Initializing I2C-");
  Wire.begin();

  Serial.println("Initializing NFC Tag-");
  nfc.begin();

  Serial.println("Declaring Text object with 128-byte buffer-");
  NDEF_TXT t("en");  // NDEF_TXT object language = English
  uint8_t textbuf[129];
  t.setPayloadBuffer(textbuf, 128); /* <-- Payload buffer contents always get NUL-terminated, so be sure
                                     * your allocated buffer is 1 byte longer than the stated maximum size.
                                     */

  Serial.println("Setting & appending values to buffer-");
  t.setText("Mary had a little lamb.\r\n");  // Could have used .println() here, it'd work the same!
  t.println("Little lamb, little lamb!");
  t.println("Mary had a little lamb,");
  t.println("his face was white as snow!");
  /* ^ NDEF_TXT implements write(), and the base NDEF class extends Print, which means you
   * may use .print(), .println() et al natively on it!  But only if .setPayloadBuffer() has been
   * run to give the object a general-purpose buffer to store written text.
   */

  Serial.println("Writing Text object to NFC transceiver-");
  int ndef_size = t.sendTo(nfc);  // Export Text NDEF object to RF430's SRAM

  Serial.print("Configuring NDEF message size (");
    Serial.print(ndef_size);
    Serial.println(" bytes)");
  nfc.setDataLength(ndef_size);

  //Serial.println("SRAM dump-");
  //DumpSRAM();

  Serial.println("Activating NFC transceiver-");
  nfc.enable();
  // NDEF Text object is now live and available over the air!

  Serial.println("Printing Text to Serial port-");
  Serial.println(t.getText());
}

void loop() {
  delay(1000);
}

//void DumpSRAM() {
//  uint8_t sram[128];
//
//  nfc.readSRAM(0x0000, sram, 128);
//  for (int i=0; i < 128; i++) {
//    if (sram[i] < 0x10)
//      Serial.print('0');
//    Serial.print(sram[i], HEX);
//    Serial.print(' ');
//    if (i % 9 == 8)
//      Serial.println();
//  }
//  Serial.println();
//}
// ^ You can uncomment this and the DumpSRAM() lines in setup() for an intimate look
// at the RF430CL330H's SRAM contents after the data's been written.
