/* ReadText
 * Example NFC application which waits for an NFC master to post a TEXT NDEF object
 * to the RF430CL330H NFC Tag controller
 *
 * Written by Eric Brundick 2/24/2015
 */

#include <Wire.h>
#include <RF430CL.h>
#include <NDEF.h>
#include <NDEF_TXT.h>

#define RF430CL330H_BOOSTERPACK_RESET_PIN  8
#define RF430CL330H_BOOSTERPACK_IRQ_PIN    12

RF430 nfc(RF430CL330H_BOOSTERPACK_RESET_PIN, RF430CL330H_BOOSTERPACK_IRQ_PIN);
NDEF_TXT msg;
uint8_t buf[512];

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Initializing I2C-");
  Wire.begin();

  Serial.println("Initializing NFC Tag-");
  nfc.begin();

  Serial.println("Activating NFC transceiver-");
  nfc.enable();

  Serial.println("Now waiting for NFC master to post a Text block.");

  msg.setPayloadBuffer(buf, 512);  // Set allocation buffer used to store incoming NDEF data
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

      Serial.println("Assuming data is NDEF TEXT; importing-");
      int ret = msg.import(nfc);
      if (ret < 0) {
        Serial.println("ERROR importing NDEF TEXT data.  SRAM dump:");
        DumpSRAM();
      } else {
        Serial.println("Success!");
        Serial.print("Language code: ");
        Serial.println(msg.getLanguage());
        Serial.println("Text block:");
        Serial.println(msg.getText());
      }
      nfc.flush();
    }
    nfc.enable();
  }
}

void DumpSRAM() {
  uint8_t sram[128];

  nfc.readSRAM(0x0000, sram, 128);
  for (int i=0; i < 128; i++) {
    if (sram[i] < 0x10)
      Serial.print('0');
    Serial.print(sram[i], HEX);
    Serial.print(' ');
    if (i % 9 == 8)
      Serial.println();
  }
  Serial.println();
}
