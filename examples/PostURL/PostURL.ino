/* PostURL
 * Example NFC application which posts a single URL to the RF430CL330H
 * NFC Tag controller
 *
 * Written by Eric Brundick 2/4/2015
 */

#include <Wire.h>
#include <RF430CL.h>
#include <NDEF.h>
#include <NDEF_URI.h>

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

  Serial.println("Writing URL object to NFC transceiver-");
  int ndef_size = tiweb.sendTo(nfc);  // Export URI NDEF object to RF430's SRAM

  Serial.print("Configuring NDEF message size (");
    Serial.print(ndef_size);
    Serial.println(" bytes)");
  nfc.setDataLength(ndef_size);

  //Serial.println("SRAM dump-");
  //DumpSRAM();

  Serial.println("Activating NFC transceiver-");
  nfc.enable();
  // NDEF URI object is now live and available over the air!

  Serial.println("Printing URL to Serial port-");
  tiweb.printURI(Serial);  // Test the NDEF_URI printURI() feature
  Serial.println();
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
