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

RF430 nfc(8, 12);

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
  nfc.setDataPointer(0);
  tiweb.printTo(nfc);
  Serial.print("Configuring NDEF record size ("); Serial.print(nfc.getDataPointer()); Serial.println(" bytes)");
  nfc.setDataLength(nfc.getDataPointer());

  //Serial.println("SRAM dump-");
  //DumpSRAM();

  Serial.println("Activating NFC transceiver-");
  nfc.enable();
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
