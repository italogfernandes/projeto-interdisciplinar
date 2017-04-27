/* ------------------------------------------------------------------------------
* FEDERAL UNIVERSITY OF UBERLANDIA
* Faculty of Electrical Engineering
* Uberlândia, Brazil
* ------------------------------------------------------------------------------
* Author: Ítalo G S Fernandes.
* contact: italogsfernandes@gmail.com
* URLs: https://github.com/italogfernandes
* ------------------------------------------------------------------------------
* Description: This code communicates a RFID module with an android smartphone
* by passing any read value to the android app using bluetooth.
* ------------------------------------------------------------------------------
* RFID 	-	UNO		-  DUE
* SDA		-	10		-	10
* SCK		-	13		-	ICSP-3
* MOSI	-	11		-	ICSP-4
* MISO	-	12		-	ICSP-1
* IRQ		-	Not connected
* GND		-	GND		-	GND
* RST		-	9		-	9
* 3.3		-	3.3V	-	3.3V
*/


#include <SPI.h>
#include <MFRC522.h>

#define uart_baudrate 115200

#define UART_START_SIGNAL 0x53

#define CMD_RFID_READ "CMDREAD"
#define CMD_RFID_WRITE "CMDWRITE"

//Pinos reset e slave select
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
void setup()
{
    Serial.begin(uart_baudrate);
    SPI.begin();	//Iniciando o SPI
    rfid.PCD_Init();	// Iniciando o MFRC522
    mensageminicial();
    //Prepara chave - padrao de fabrica = FFFFFFFFFFFFh
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}
String serialOp; // Variable for receiving commands from serial
void loop(){
    if(Serial.available() > 0){
        serialOp = Serial.readString();
        if(serialOp == CMD_RFID_READ){
            Serial.println("Modo leitura selecionado");
            delay(3000); //NOTE: why they use this delay?
            modo_leitura();
        }
        else if(serialOp == CMD_RFID_WRITE){
            Serial.println("Modo gravacao selecionado");
            delay(3000); //NOTE: why they use this delay?
            modo_gravacao();
        }
    }
}

void modo_leitura() {
  mensagem_inicial_cartao();
  //Aguarda cartao
  while ( ! rfid.PICC_IsNewCardPresent()){
    delay(100);
  }
  if ( ! rfid.PICC_ReadCardSerial()){
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag : ");
  String conteudo = "";
  byte letra;
  for (byte i = 0; i < rfid.uid.size; i++){
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    conteudo.concat(String(rfid.uid.uidByte[i]<0x10 ? " 0" : " "));
    conteudo.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println();

  //Obtem os dados do setor 1, bloco 4 = Nome
  byte sector         = 1;
  byte blockAddr      = 4;
  byte trailerBlock   = 7;
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  //Autenticacao usando chave A
  status=rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                  trailerBlock, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  status = rfid.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
  }
  //Mostra os dados do nome no Serial Monitor
  for (byte i = 1; i < 16; i++) {
    Serial.print(char(buffer[i]));
  }
  Serial.println();

  //Obtem os dados do setor 0, bloco 1 = Sobrenome
  sector         = 0;
  blockAddr      = 1;
  trailerBlock   = 3;

  //Autenticacao usando chave A
  status=rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                  trailerBlock, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  status = rfid.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK){
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
  }
  //Mostra os dados do sobrenome no Serial Monitor
  for (byte i = 0; i < 16; i++)
  {
    Serial.print(char(buffer[i]));
  }
  Serial.println();

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  delay(3000);
  mensageminicial();
}

void modo_gravacao(){
  mensagem_inicial_cartao();
  //Aguarda cartao
  while ( ! rfid.PICC_IsNewCardPresent()) {
    delay(100);
  }
  if ( ! rfid.PICC_ReadCardSerial())    return;

  //Mostra UID na serial
  Serial.print(F("UID do Cartao: "));    //Dump UID
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  //Mostra o tipo do cartao
  Serial.print(F("\nTipo do PICC: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  byte buffer[34];
  byte block;
  MFRC522::StatusCode status;
  byte len;

  Serial.setTimeout(20000L) ;
  Serial.println(F("Digite o sobrenome,em seguida o caractere #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ;
  for (byte i = len; i < 30; i++) buffer[i] = ' ';

  block = 1;
  //Serial.println(F("Autenticacao usando chave A..."));
  status=rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  //Grava no bloco 1
  status = rfid.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  block = 2;
  //Serial.println(F("Autenticacao usando chave A..."));
  status=rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(rfid.GetStatusCodeName(status));
      return;
    }

  Serial.println(F("Digite o nome, em seguida o caractere #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 20) ;
  for (byte i = len; i < 20; i++) buffer[i] = ' ';

  block = 4;
  //Serial.println(F("Autenticacao usando chave A..."));
    status=rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                      block, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(rfid.GetStatusCodeName(status));
      return;
  }

  //Grava no bloco 4
  status = rfid.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  block = 5;
  //Serial.println(F("Authenticating using key A..."));
  status=rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  //Grava no bloco 5
  status = rfid.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    //return;
  }
  else
  {
    Serial.println(F("Dados gravados com sucesso!"));
  }

  rfid.PICC_HaltA(); // Halt PICC
  rfid.PCD_StopCrypto1();  // Stop encryption on PCD
  delay(5000);
  mensageminicial();
}

void mensageminicial(){
  Serial.println("\nSelecione o modo leitura ou gravacao...");
  Serial.println(CMD_RFID_READ);
  Serial.println(CMD_RFID_WRITE);
  Serial.println();
}

void mensagem_inicial_cartao(){
  Serial.println("Aproxime o seu cartao do leitor...");
}
