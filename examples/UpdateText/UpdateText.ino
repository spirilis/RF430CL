/* UpdateText
 *
 * Example NFC application which posts a simple phrase to the RF430CL330H
 *  NFC Tag controller, then updates the text by appending hashmarks every time
 *  the user presses PUSH1.
 *
 * This sketch illustrates how you can update an NDEF object (NDEF_TXT in this example),
 *  then the process required for rewriting the NFC tag to reflect the new contents.
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

// Need to declare these global so they are accessible by loop() as well as setup().
NDEF_TXT txt("en");
uint8_t txtbuf[129];

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Initializing I2C-");
  Wire.begin();

  Serial.println("Initializing NFC Tag-");
  nfc.begin();

  txt.setPayloadBuffer(txtbuf, 128); /* <-- Payload buffer contents always get NUL-terminated, so be sure
                                      * your allocated buffer is 1 byte longer than the stated maximum size.
                                      */

  Serial.println("Setting & appending values to buffer-");
  txt.setText("My NFC tag- ");

  Serial.println("Writing Text object to NFC transceiver-");
  int ndef_size = txt.sendTo(nfc);  // Export Text NDEF object to RF430's SRAM

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
  Serial.println(txt.getText());
  
  Serial.println("Configuring PUSH1 to update NFC text record when pressed-");
  pinMode(PUSH1, INPUT_PULLUP);
  attachInterrupt(PUSH1, triggerTextUpdate, FALLING);
}

// Flag which tells loop() whether it needs to update the NFC tag
volatile boolean update_trigger = false;

void loop() {
  delay(100);
  
  if (update_trigger) {
    txt.print('#');  // Add a hashmark each time PUSH1 is pressed!
    nfc.disable();  // Disable RF before updating SRAM contents
    nfc.setDataPointer(0);  // We're rewriting the NFC tag's SRAM
    size_t ndef_size = txt.sendTo(nfc);  // Write new NDEF data
    nfc.setDataLength(ndef_size);
    nfc.enable();  // Make it live!
    Serial.println("Tag updated.");

    update_trigger = false;  // Detrigger
    //Serial.println("New SRAM contents-");
    //DumpSRAM();
  }
}

/* Runs every time PUSH1 is pressed, just signals to the main loop() that
 * it should carry out an update to the NFC tag.
 *
 * It is NOT RECOMMENDED to perform any sort of NFC updates inside an interrupt
 * service routine, as the NFC tag I/O requires I2C, which itself requires interrupts
 * that won't run when the CPU is already executing within interrupt context.
 */
void triggerTextUpdate()
{
  update_trigger = true;
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
