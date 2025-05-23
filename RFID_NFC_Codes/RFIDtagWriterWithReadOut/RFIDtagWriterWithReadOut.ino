//----------------------------------------//
// + This code is made by Marco de Veld + //
//----------------------------------------//

#include <Wire.h>
#include <Adafruit_PN532.h>

// Define I2C pins (or use SPI if that's what you're running)
// These pins work for Arduino Uno, adapt if you're using a different board
#define SDA_PIN 2
#define SCL_PIN 3
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("Looking for PN532...");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN532 board");
    while (1); // halt
  }

  // Configure the board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an NFC tag...");
}

void loop() {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength;

  // Wait for an ISO14443A type card (MIFARE Classic)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an NFC tag!");
    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Define block and key
    uint8_t blockNumber = 1; // Block 1 of sector 0 (be careful not to overwrite block 0 or block 3!)
    uint8_t dataToWrite[16] = {
      'H', 'e', 'l', 'l', 'o', ' ', 'M', 'a',
      'r', 'c', 'o', '!', ' ', ' ', ' ', ' '
    };
    uint8_t keyA[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // Default key A

    // Authenticate the block
    if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, 0, keyA)) {
      Serial.println("Authentication successful!");

      // Write the data to the block
      if (nfc.mifareclassic_WriteDataBlock(blockNumber, dataToWrite)) {
        Serial.println("Data written successfully!");

        // Now read back the block to confirm
        uint8_t dataRead[16];
        if (nfc.mifareclassic_ReadDataBlock(blockNumber, dataRead)) {
          Serial.print("Data in block ");
          Serial.print(blockNumber);
          Serial.print(": ");
          for (int i = 0; i < 16; i++) {
            Serial.write(dataRead[i]); // Print raw characters
          }
          Serial.println();
        } else {
          Serial.println("Failed to read block after writing.");
        }

      } else {
        Serial.println("Failed to write data to block.");
      }

    } else {
      Serial.println("Authentication failed.");
    }

    delay(3000); // Wait before scanning another tag
  }
}

//----------------------------------------//
// + This code is made by Marco de Veld + //
//----------------------------------------//