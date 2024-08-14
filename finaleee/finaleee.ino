
/* ------------------------------------------------------------------------
 * Created by: Tauseef Ahmad
 * Created on: 23 Feb, 2022
 *  
 * Tutorial: https://youtu.be/aNjkNmHRx4o
 * ------------------------------------------------------------------------
 * Download Resources
 * ------------------------------------------------------------------------
 * Preferences--> Aditional boards Manager URLs : 
 * For ESP8266 and NodeMCU - Version 3.0.2
 * http://arduino.esp8266.com/stable/package_esp8266com_index.json
 * ------------------------------------------------------------------------*/

#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Servo.h>
//-----------------------------------------
#define RST_PIN D3
#define SS_PIN D4
#define led D2
//-----------------------------------------
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

Servo servo;
//-----------------------------------------
/* Be aware of Sector Trailer Blocks */
int blockNum = 2;
/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];
//-----------------------------------------
String card_holder_name;
const String sheet_url = "https://script.google.com/macros/s/AKfycbwZ9lGj63Mq7MKl-wBg4ubBY7YQxcqu2pjMbCXomZBfr8zqZ_l09e3h-Aw-Zw7Rp98/exec?name=";
//-----------------------------------------
// Fingerprint for demo URL, expires on ‎Monday, ‎May ‎2, ‎2022 7:20:58 AM, needs to be updated well before this date
// const uint8_t fingerprint[32] = {0xf1, 0xcd, 0x76, 0x69, 0xb2, 0xe4, 0x95, 0xa4, 0x6e, 0xed, 0x69, 0x27, 0xa4, 0x37, 0x7e, 0x7c, 0x82, 0x14, 0x08, 0x03, 0xc6, 0x00, 0xbd, 0x3a, 0xde, 0x79, 0x72, 0xd8, 0xba, 0xde, 0x49, 0xea};
// const uint8_t fingerprint[32] = {0x05, 0x69, 0xa7, 0x2a, 0x23, 0xea, 0x22, 0x34, 0x56, 0x25, 0x15, 0xd4, 0x77, 0xc9, 0x30, 0x56, 0x96, 0x3c, 0xd3, 0xcc, 0xff, 0xb7, 0xf4, 0x3e, 0x5c, 0xfb, 0xca, 0x61, 0xf2, 0x59, 0x74, 0x8c};
//9a 87 9b 82 e9 19 7e 63 8a db 67 ed a7 09 d9 2f 30 de e7 3c
// const uint8_t fingerprint[32] = {05 69 a7 2a 23 ea 22 34 56 25 15 d4 77 c9 30 56 96 3c d3 cc ff b7 f4 3e 5c fb ca 61 f2 59 74 8c}
// #define fingerprint "05 69 a7 2a 23 ea 22 34 56 25 15 d4 77 c9 30 56 96 3c d3 cc ff b7 f4 3e 5c fb ca 61 f2 59 74 8c"
// #define fingerprint "f1 cd 76 69 b2 e4 95 a4 6e ed 69 27 a4 37 7e 7c 82 14 08 03 c6 00 bd 3a de 79 72 d8 ba de 49 ea"
//-----------------------------------------
#define WIFI_SSID "miran"
#define WIFI_PASSWORD "        "
//-----------------------------------------
String reg[2] = { "miran", "zeeshan" };

unsigned long tcrr = 0;
unsigned long tpre = 0;


/****************************************************************************************************
 * setup() function
 ****************************************************************************************************/
void setup() {
  //--------------------------------------------------
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  //Serial.setDebugOutput(true);
  //--------------------------------------------------
  //WiFi Connectivity
  // Serial.println();
  // Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    // Serial.print(".");
    delay(200);
  }
  // Serial.println("");
  // Serial.println("WiFi connected.");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());
  // Serial.println();
  //--------------------------------------------------
  /* Set led as OUTPUT */
  pinMode(led, OUTPUT);
  //--------------------------------------------------
  /* Initialize SPI bus */
  SPI.begin();
  servo.attach(D8);
  //--------------------------------------------------
}




/****************************************************************************************************
 * loop() function
 ****************************************************************************************************/
void loop() {
  tcrr = millis();
  if (tcrr - tpre >= 5000) {
    door_mv(false);
    digitalWrite(led, LOW);
    delay(500);
    digitalWrite(led, HIGH);
  }
  //--------------------------------------------------
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if (!mfrc522.PICC_IsNewCardPresent()) { return; }
  /* Select one of the cards */
  if (!mfrc522.PICC_ReadCardSerial()) { return; }
  /* Read data from the same block */
  //--------------------------------------------------
  // Serial.println();
  // Serial.println(F("Reading last data from RFID..."));
  ReadDataFromBlock(blockNum, readBlockData);
  /* If you want to print the full memory dump, uncomment the next line */
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  /* Print the data read from block */
  // Serial.println();
  // Serial.print(F("Last data in RFID:"));
  // Serial.print(blockNum);
  // Serial.print(F(" --> "));
  for (int j = 0; j < 16; j++) {
    Serial.write(readBlockData[j]);
  }
  // Serial.println();
  //--------------------------------------------------
  // digitalWrite(led, HIGH);
  // delay(200);
  // digitalWrite(led, LOW);
  // delay(200);
  // digitalWrite(led, HIGH);
  // delay(200);
  // digitalWrite(led, LOW);
  //--------------------------------------------------

  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  if (WiFi.status() == WL_CONNECTED) {
    //-------------------------------------------------------------------------------
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    //-------------------------------------------------------------------------------
    // client->setFingerprint(fingerprint);
    // Or, if you want to ignore the SSL certificate
    //then use the following line instead:
    client->setInsecure();
    //-----------------------------------------------------------------
    String name_ts = String((char*)readBlockData);
    card_holder_name = sheet_url + String((char*)readBlockData);
    card_holder_name.trim();
    Serial.println(name_ts);

    if (name_ts == reg[0] || name_ts == reg[1]) {
      tpre = millis();
      door_mv(true);
      digitalWrite(led, HIGH);
      delay(500);
      digitalWrite(led, LOW);
      //-----------------------------------------------------------------
      HTTPClient https;
      // Serial.print(F("[HTTPS] begin...\n"));
      //-----------------------------------------------------------------

      //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
      if (https.begin(*client, (String)card_holder_name)) {
        //-----------------------------------------------------------------
        // HTTP
        // Serial.print(F("[HTTPS] GET...\n"));
        // start connection and send HTTP header
        int httpCode = https.GET();
        //-----------------------------------------------------------------
        // httpCode will be negative on error
        if (httpCode > 0) {
          Serial.println(card_holder_name);
          // HTTP header has been send and Server response header has been handled
          // Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          // file found at server
        }
        //-----------------------------------------------------------------
        else
          // {Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());}
          //-----------------------------------------------------------------
          https.end();
        delay(1000);
      }
      //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
      else {
        // Serial.printf("[HTTPS} Unable to connect\n");
      }
      //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    }
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}




/****************************************************************************************************
 * ReadDataFromBlock() function
 ****************************************************************************************************/
void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  //----------------------------------------------------------------------------
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  //----------------------------------------------------------------------------
  /* Authenticating the desired data block for Read access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  //----------------------------------------------------------------------------s
  if (status != MFRC522::STATUS_OK) {
    //  Serial.print("Authentication failed for Read: ");
    //  Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //----------------------------------------------------------------------------
  else {
    // Serial.println("Authentication success");
  }
  //----------------------------------------------------------------------------
  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    // Serial.print("Reading failed: ");
    // Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //----------------------------------------------------------------------------
  else {
    // Serial.println("Block was read successfully");
  }
  //----------------------------------------------------------------------------
}

void door_mv(bool cond) {
  if (cond) {
    servo.write(0);
  } else {
    servo.write(180);
  }
}
