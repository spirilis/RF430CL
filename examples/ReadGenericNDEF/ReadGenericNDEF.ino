/* ReadGenericNDEF
 * Example NFC application which waits for an NFC master to post any type of NDEF object
 * to the RF430CL330H NFC Tag controller
 *
 * Written by Eric Brundick 2/25/2015
 */

#include <Wire.h>
#include <RF430CL.h>
#include <NDEF.h>

#define RF430CL330H_BOOSTERPACK_RESET_PIN  8
#define RF430CL330H_BOOSTERPACK_IRQ_PIN    12

RF430 nfc(RF430CL330H_BOOSTERPACK_RESET_PIN, RF430CL330H_BOOSTERPACK_IRQ_PIN);
NDEF msg;  // Generic NDEF object
uint8_t buf[512];
char typebuf[64], idbuf[64];

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Initializing I2C-");
  Wire.begin();

  Serial.println("Initializing NFC Tag-");
  nfc.begin();

  Serial.println("Activating NFC transceiver-");
  nfc.enable();

  Serial.println("Now waiting for NFC master to post an NDEF.");

  msg.setPayloadBuffer(buf, 512);  // Set allocation buffer used to store incoming NDEF data
  msg.setTypeBuffer(typebuf, 64);
  msg.setIDBuffer(idbuf, 64);
}

void loop() {
  if (nfc.loop()) {
    // If nfc.loop() returns true, nfc.disable() is implicitly run...
    if (nfc.isError()) {
      Serial.println("NFC transceiver reports its SRAM contents do not contain valid NDEF data.");
      DumpSRAM();
    }
    if (nfc.wasRead()) {
      Serial.println("NDEF tag was read!");
    }
    if (nfc.available()) {
      Serial.print("NFC master has written a new tag! ");
      uint16_t len = nfc.getDataLength();
      Serial.print(len);
      Serial.println(" bytes");

      int ret = msg.import(nfc);
      if (ret < 0) {
        Serial.println("ERROR importing NDEF data.  SRAM dump:");
        DumpSRAM();
      } else {
        Serial.print("Success!  NDEF Type: TNF=");
        Serial.print(msg.getTNF(), HEX);
        Serial.print("; type=");
        Serial.print(msg.getType());
        Serial.print("; id=");
        Serial.println(msg.getID());
        Serial.print("Payload: ");
        Serial.write(msg.getPayload(), msg.getPayloadLength());
        Serial.println();
      }
      nfc.flush();
    }
    nfc.enable();
  }
}

void DumpSRAM() {
  uint8_t sram[512];

  nfc.readSRAM(0x0000, sram, 512);
  for (int i=0; i < 512; i++) {
    if (sram[i] < 0x10)
      Serial.print('0');
    Serial.print(sram[i], HEX);
    Serial.print(' ');
    if (i % 9 == 8)
      Serial.println();
  }
  Serial.println();
}
