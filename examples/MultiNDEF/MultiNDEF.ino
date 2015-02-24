/* MultiNDEF
 *
 * Example NFC application which posts two NDEF records simultaneously to the
 * RF430CL330H NFC Tag controller.
 *
 * This example makes use of the two true/false arguments available with .sendTo()
 *  to adjust the presence of MB (Message Begin) and ME (Message End) bits within
 *  each NDEF message's header byte.
 *
 * Note: Not all NFC apps for your phone may support this.  On Android, the "TagWriter" app by NXP did
 * not allow me to see all the records on the tag.  However the "NFC Tools" app by "wakdev" showed me
 * both records.
 *
 * Written by Eric Brundick 2/10/2015
 */

#include <Wire.h>
#include <RF430CL.h>
#include <NDEF.h>
#include <NDEF_URI.h>
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

  Serial.println("Declaring URL object-");
  NDEF_URI tiweb("http://www.ti.com");

  Serial.println("Declaring text description for the URL-");
  NDEF_TXT tidesc("en", "Texas Instruments, Inc.");

  Serial.println("Writing URL object to NFC transceiver-");
  int ndef_size;

  // RECORD 0
  ndef_size = tiweb.sendTo(nfc, true, false);  // This message should set MB, but not ME.

  Serial.println("Writing Text object to NFC transceiver-");
  // RECORD 1
  ndef_size += tidesc.sendTo(nfc, false, true);  // This message should leave MB cleared, but set ME.

  /* If there were any other NDEF records in between these two, they would have both
   * MB and ME cleared ala .sendTo(nfc, false, false)
   *
   * If instead I chose to add one record *after* the text description, I would have to
   * change the "tidesc.sendTo" line to "tidesc.sendTo(nfc, false, false)" and the new
   * record added afterward would use .sendTo(false, true) to terminate the chain of NDEF messages
   * present on the tag.
   */

  Serial.print("Configuring NDEF message size (");
    Serial.print(ndef_size);
    Serial.println(" bytes)");
  nfc.setDataLength(ndef_size);

  //Serial.println("SRAM dump-");
  //DumpSRAM();

  Serial.println("Activating NFC transceiver-");
  nfc.enable();
  // NDEF URI & TEXT objects are now live and available over the air!

  Serial.println("Printing URL to Serial port-");
  tiweb.printURI(Serial);  // Test the NDEF_URI printURI() feature
  Serial.println();

  Serial.println("Printing text description to Serial port-");
  Serial.println(tidesc.getText());
}

void loop() {
  if (nfc.loop()) {
    if (nfc.wasRead()) {
      Serial.println("NDEF tag was read!");
    }
    if (nfc.available()) {
      Serial.print("NFC master has written a new tag! ");
      uint16_t len = nfc.getDataLength();
      Serial.print(len);
      Serial.println(" bytes");
      nfc.flush();  // prevent nfc.available() from returning true again
    }
    nfc.enable();
  }
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
