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

#define uart_baudrate 38400

#define UART_START_SIGNAL 0x53

#define CMD_BURN_ID 0x01


#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
 
char st[20];
 
void setup() 
{
  Serial.begin(uart_baudrate);
  SPI.begin();	//Iniciando o SPI
  mfrc522.PCD_Init();	// Iniciando o MFRC522
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}
 
void loop() 
{

  if(Serial.available() > 2){
  	if(Serial.read() == UART_START_SIGNAL){
  		//TODO: implement cmds handler
  	}
  }
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo= "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mensagem : ");
  conteudo.toUpperCase();
} 
 
