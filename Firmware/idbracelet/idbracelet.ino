/**
   ----------------------------------------------------------------------------
   FEDERAL UNIVERSITY OF UBERLANDIA
   Faculty of Electrical Engineering
   Uberlândia, Brazil
   ----------------------------------------------------------------------------
   Authors: Italo G S Fernandes, Ana Carolina Torres Cresto
   contact: italogsfernandes@gmail.com, anatcresto@gmail.com
   URL: https://github.com/italogfernandes/projeto-interdisciplinar
   ------------------------------------------------------------------------------
   Description:

   This code has a description in the presentation 'semana 8', please read.

   BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).

   Pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
  ----------------------------------------------------------------------------
  Especial Thanks to MiguelBalboa for the library used to control the rfid module.
*/

#include <SPI.h>
#include <MFRC522.h>

//LEDS Red e Yellow
#define LED_R 4
#define LED_Y 3

//Mensagens
#define ERROR_MSG(x) Serial.print("$E" + String(x) + "\n")
#define LOG_MSG(x) Serial.print("$L" + String(x) + "\n")

//RFID
constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

//Protocolo
uint32_t actual_patient_id;
uint8_t id_values[4];
bool estado_writing = 0;

void dump_byte_array(byte *buffer_rfid, byte bufferSize);
void ler_paciente_id();
void write_id_to_rfid();
void recebe_comando_bt();

void setup() {
  //GPIO
  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  //Bluetooth - Serial
  Serial.begin(9600); // Initialize serial communications with the PC
  //RFID - SPI
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  //Mensagens de LOG iniciais
  LOG_MSG("Scan a MIFARE Classic PICC to demonstrate read and write.");
  LOG_MSG("Using key (for A and B):");
  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
  LOG_MSG("BEWARE: Data will be written to the PICC, in sector #1");

  LOG_MSG("Setup completed.");
  digitalWrite(LED_Y, 1); digitalWrite(LED_R, 0); delay(500); digitalWrite(LED_Y, 0); digitalWrite(LED_R, 1);
  digitalWrite(LED_Y, 1); digitalWrite(LED_R, 0); delay(500); digitalWrite(LED_Y, 0); digitalWrite(LED_R, 1);
  digitalWrite(LED_Y, 0); digitalWrite(LED_R, 0);
}

void loop() {
  //Se existe um comando via Bluetooth
  if (Serial.available() > 0) {
    delay(5); //wait all the data in buffer
    recebe_comando_bt();
  }
  // Look for new cards and Select one of the cards
  if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (estado_writing) {
      write_id_to_rfid();
      estado_writing = 0;
    } else {
      ler_paciente_id();
    }
  }
  digitalWrite(LED_R, estado_writing);
  digitalWrite(LED_Y, !estado_writing);

}


void recebe_comando_bt() {
  if (Serial.read() == '$') {
    switch (Serial.read()) {
      case 'W':
        id_values[0] = Serial.read();
        id_values[1] = Serial.read();
        id_values[2] = Serial.read();
        id_values[3] = Serial.read();
        if (Serial.read() == '\n') {
          estado_writing = 1;
        }
        break;
    }
  }
}

void write_id_to_rfid() {

  // Show some details of the PICC (that is: the tag/card)
  LOG_MSG("Card UID:");
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  LOG_MSG("PICC type: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  LOG_MSG(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    ERROR_MSG("This sample only works with MIFARE Classic cards.");
    return;
  }

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7
  byte sector         = 2;
  byte blockAddr      = 4;
  byte trailerBlock   = 7;
  MFRC522::StatusCode status;
  byte buffer_rfid[18];
  byte size_buffer_rfid = sizeof(buffer_rfid);

  // Authenticate using key A
  LOG_MSG("Authenticating using key A...");
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    ERROR_MSG("PCD_Authenticate() failed: ");
    ERROR_MSG(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write data to the block
  LOG_MSG("Writing data into block "); LOG_MSG(blockAddr);
  LOG_MSG(" ...");
  dump_byte_array(id_values, 4);
  buffer_rfid[0] = id_values[0]; buffer_rfid[1] = id_values[1]; buffer_rfid[2] = id_values[2]; buffer_rfid[3] = id_values[3];
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, buffer_rfid, 16);
  if (status != MFRC522::STATUS_OK) {
    ERROR_MSG("MIFARE_Write() failed: ");
    ERROR_MSG(mfrc522.GetStatusCodeName(status));
  }
  // Read data from the block (again, should now be what we have written)
  LOG_MSG("Reading data from block "); LOG_MSG(blockAddr);
  LOG_MSG(" ...");
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer_rfid, &size_buffer_rfid);
  if (status != MFRC522::STATUS_OK) {
    ERROR_MSG("MIFARE_Read() failed: ");
    ERROR_MSG(mfrc522.GetStatusCodeName(status));
  }
  LOG_MSG("Data in block "); LOG_MSG(blockAddr); LOG_MSG(":");
  dump_byte_array(buffer_rfid, 4);
  if(buffer_rfid[0] == id_values[0] && buffer_rfid[1] == id_values[1] && buffer_rfid[2] == id_values[2] && buffer_rfid[3] == id_values[3]){
    LOG_MSG("TUDO OK :)");
  } else {
    ERROR_MSG("Não vai dar não.");
  }

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}


void ler_paciente_id() {
  // Show some details of the PICC (that is: the tag/card)
  LOG_MSG("Card UID:");
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  LOG_MSG("PICC type: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  LOG_MSG(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    ERROR_MSG("This sample only works with MIFARE Classic cards.");
    return;
  }

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7
  byte sector         = 2;
  byte blockAddr      = 4;
  byte trailerBlock   = 7;
  MFRC522::StatusCode status;
  byte buffer_rfid[18];
  byte size_buffer_rfid = sizeof(buffer_rfid);

  // Authenticate using key A
  LOG_MSG("Authenticating using key A...");
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    ERROR_MSG("PCD_Authenticate() failed: ");
    ERROR_MSG(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Show the whole sector as it currently is
  //LOG_MSG("Current data in sector:");
  //Serial.print("$L");
  //mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  //Serial.print("\n");

  // Read data from the block
  LOG_MSG("Reading data from block "); LOG_MSG(blockAddr);
  LOG_MSG(" ...");
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer_rfid, &size_buffer_rfid);
  if (status != MFRC522::STATUS_OK) {
    ERROR_MSG("MIFARE_Read() failed: ");
    ERROR_MSG(mfrc522.GetStatusCodeName(status));
  }
  LOG_MSG(("Data in block " + String(blockAddr) + ":"));
  dump_byte_array(buffer_rfid, 16);
  LOG_MSG(("Data in block " + String(blockAddr) + ":" ));
  Serial.print("$L");
  for (int i = 0 ; i < 18; i++) {
    Serial.print((char)(buffer_rfid[i]));
  }
  Serial.print("\n");


  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();

  //SENDING Pacote de leitura
  Serial.print("\r\n");
  Serial.print("\n");
  Serial.print("$R");
  Serial.write(buffer_rfid[0]);
  Serial.write(buffer_rfid[1]);
  Serial.write(buffer_rfid[2]);
  Serial.write(buffer_rfid[3]);
  Serial.print("\n");
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer_rfid, byte bufferSize) {
  Serial.print("$L");
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer_rfid[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer_rfid[i], HEX);
  }
  Serial.print("\n");
}

