#include <SPI.h>
#include <MFRC522.h>

//-----------------------------------------
constexpr uint8_t RST_PIN = D3;
constexpr uint8_t SS_PIN = D4;
//-----------------------------------------
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;        
//-----------------------------------------
MFRC522::StatusCode status;
//-----------------------------------------

void setup() 
{
  Serial.begin(9600);
  SPI.begin();
  Serial.println("Scan a MIFARE 1K Tag to hard format...");
}

void loop()
{
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++){
    key.keyByte[i] = 0xFF;
  }
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println("**Card Detected**");
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++){
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  hardFormatRFID();
}

void hardFormatRFID()
{
  Serial.println("Hard Formatting RFID...");

  // Overwrite all blocks with blank data
  byte blankData[16] = {0x00};
  for (int blockNum = 0; blockNum < 64; blockNum++) { // Assuming 64 blocks for MIFARE 1K
    // Authenticate and write data to each block
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK){
      Serial.print("Authentication failed for block ");
      Serial.print(blockNum);
      Serial.print(": ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    status = mfrc522.MIFARE_Write(blockNum, blankData, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("Writing to Block ");
      Serial.print(blockNum);
      Serial.print(" failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
  }

  Serial.println("RFID hard formatted successfully");
}
