/* Hello World - NFC edition
 *
 * Designed for the RF430CL330H BoosterPack with Energia
 */

#include <RF430CL.h>
#include <Wire.h>

#define RF430CL330H_BOOSTERPACK_RESET_PIN  8
#define RF430CL330H_BOOSTERPACK_IRQ_PIN    12

RF430 nfc(RF430CL330H_BOOSTERPACK_RESET_PIN, RF430CL330H_BOOSTERPACK_IRQ_PIN);

const uint8_t ndef_data_HelloWorld[] = {
  0xD1,           // MB (Message Begin), SR (Short Record), TNF = 1
  0x01, 16,       // Type Length = 0x01, Payload Length
  'T',            // Type = T (Text)
  0x02,           // 1st payload byte - UTF-8, IANA country code = 2 characters
  'e', 'n',       // 'e', 'n' (2-character IANA country code for text language)
  'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'  // Text!
};

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Initializing I2C-");
  Wire.begin();

  Serial.println("Initializing RF430CL330H-");
  nfc.begin();

  Serial.println("Writing Hello World NDEF data-");
  nfc.write(ndef_data_HelloWorld, sizeof(ndef_data_HelloWorld));

  Serial.print("Writing NDEF size ("); Serial.print(sizeof(ndef_data_HelloWorld)); Serial.println(" bytes)-");
  nfc.setDataLength(sizeof(ndef_data_HelloWorld));

  Serial.println("RF enable-");
  nfc.enable();  // Activate RF
}

void loop() {
  uint16_t i;


  if (!digitalRead(RF430CL330H_BOOSTERPACK_IRQ_PIN)) {  // IRQ line active?
    nfc.disable();  // Must turn off RF before accessing registers or SRAM

    i = nfc.readReg(RF430_REG_INT_FLAG);
    if (i) {
      Interpret_RF430_Interrupt_Flag(i);
      nfc.writeReg(RF430_REG_INT_FLAG, i);
    } else {
      Serial.println('.');
    }

    nfc.enable();
  }

  delay(100);
}

void Interpret_RF430_Interrupt_Flag(uint16_t flg)
{
  Serial.print("RF430 Interrupt Flags: ");

  if (flg & RF430_INT_END_OF_READ) Serial.print("END_OF_READ ");
  if (flg & RF430_INT_END_OF_WRITE) Serial.print("END_OF_WRITE ");
  if (flg & RF430_INT_CRC_COMPLETED) Serial.print("CRC_COMPLETED ");
  if (flg & RF430_INT_BIP8_ERROR) Serial.print("BIP-8_ERROR ");
  if (flg & RF430_INT_NDEF_ERROR) Serial.print("NDEF_FORMAT_ERROR ");
  if (flg & RF430_INT_GENERIC_ERROR) Serial.print("GENERIC_ERROR ");
  Serial.println();
}
