/* HelloWorld - NFC edition
 * Written for the TI RF430CL330H with Energia
 *
 * Showcases Low-Power Sleep modes in Energia and IRQ-driven wakeup of
 * the MCU based on RF430CL330H IRQs.
 *
 * 2/25/2015 Eric Brundick
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

  Serial.println("Hello World - NFC edition!");
  Serial.println("Here we will post the text \"Hello World\" to the RF430CL330H.");

  Serial.println("Initializing I2C and RF430CL330H-");
  Wire.begin();  // Initialize I2C subsystem
  nfc.begin();   // Format RF430CL330H, prepare for data
  // Register interrupt to wake MCU when RF430CL330H INTO (IRQ) line triggers
  attachInterrupt(RF430CL330H_BOOSTERPACK_IRQ_PIN, wake_up, FALLING);

  Serial.println("Creating NFC NDEF_TXT object-");
  NDEF_TXT helloWorld("en", "Hello World");  // English

  Serial.println("Posting to RF430CL330H-");
  size_t ndef_size;

  ndef_size = helloWorld.sendTo(nfc);  // Write NDEF data to NFC device memory
  nfc.setDataLength(ndef_size);  // Inform NFC device memory how much data is there

  Serial.println("Activating RF430CL330H RF link-");
  nfc.enable();  // Now we're live!
}

void loop() {
  if (nfc.loop()) {
    if (nfc.wasRead()) {
      Serial.println("Something has read the NFC device!");
    }
    if (nfc.available()) {
      Serial.println("Something has re-written the NFC device!");
      nfc.flush();
    }
    nfc.enable();  // If nfc.loop() returns true, it will have disabled the RF link as a side-effect.
  }

  Serial.println("<low power sleep>");
  Serial.flush();  // wait for unsent UART data to flush out before going into low-power sleep
  suspend();
  Serial.println("<wake up>");
}

void wake_up()
{
  wakeup();  // Signal to Energia to wake the MCU upon IRQ exit
}
