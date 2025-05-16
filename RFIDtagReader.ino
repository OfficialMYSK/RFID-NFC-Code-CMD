//----------------------------------------//
// + This code is made by Marco de Veld + //
//----------------------------------------//

#include <Wire.h>
#include <Adafruit_PN532.h>

// Define IRQ and RESET (same pins you use!)
#define PN532_IRQ   (2)
#define PN532_RESET (3)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// Default key for Mifare Classic tags
uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

void setup(void) {
  Serial.begin(115200); // Use this Buad value in your serial monitor
  Serial.println("NFC Reader Initialized");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }

  nfc.SAMConfig();
  Serial.println("Waiting for an NFC card...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
  uint8_t uidLength;

  // Look for a tag
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Card detected!");

    // Show UID
    Serial.print("UID Length: "); Serial.println(uidLength, DEC);
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Authenticate block 4
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 1, 0, keya);

    if (success) {
      Serial.println("Block 1 authentication successful");

      uint8_t data[16]; // Block size is always 16 bytes

      // Read the block data
      success = nfc.mifareclassic_ReadDataBlock(1, data);

      if (success) {
        Serial.print("Data in block 1: ");

        // Print raw bytes
        for (uint8_t i = 0; i < 16; i++) {
          Serial.print((char)data[i]);
        }

        Serial.println("");
      } else {
        Serial.println("Failed to read block 1");
      }
    } else {
      Serial.println("Authentication failed for block 1");
    }

    Serial.println("Waiting for next card...");
    delay(2000); // Debounce time before scanning again
  }
}

//----------------------------------------//
// + This code is made by Marco de Veld + //
//----------------------------------------//
