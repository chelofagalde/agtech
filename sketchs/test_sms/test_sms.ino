#include <SoftwareSerial.h>

SoftwareSerial shieldSerial(7, 8);

void setup()
{
  shieldSerial.begin(115200); //Comunicación serial con el shield
  Serial.begin(115200); //Serial para imprimir por pantalla
  delay(500);

  Serial.println("Turn on the shield, initializing GSM...");

  //Espero a que se registre en la red, debe recibir 2 strings con status:
  int dataToReceive = 2;
  while (dataToReceive > 0) {
    if (shieldSerial.available() > 0) {
      String data = shieldSerial.readString();
      Serial.println(data);
      dataToReceive--;
    }
  }

  //Envio un par de veces un ping:
  for (int i = 0; i < 10; i++) {
    sendATCommand("AT");
    printAllDataFromShield();
  }

  Serial.println("Setting basic configs...");
  setSMSCharset();
  setTextMode();
  getSignalQuality();

  delay(500);

  Serial.println("Setup DONE. Starting loop...");
}

void loop() {
  if (Serial.available()) {
    switch (Serial.read())
    {
      case 't':
        Serial.println("Sending sms...");
        sendSMS();
        break;
      case 'q':
        Serial.println("Getting signal quality...");
        getSignalQuality();
        break;
      case 'p':
        printAllDataFromShield();
        break;
    }
  }

  delay(100);
}

//-------------------------------------------------------
// Funciones de alto nivel para interactuar con el shield
//-------------------------------------------------------

void setSMSCharset() {
  sendATCommand("AT+CSCS=\"UCS2\"");
  printAllDataFromShield();
}

void setTextMode() {
  sendATCommand("AT+CMGF=1");    //Because we want to send the SMS in text mode
  printAllDataFromShield();
}

void sendSMS() {
  sendATCommand("AT+CMGS=\"+5491121729848\"");//send sms message, be careful need to add a country code before the cellphone number
  sendATCommand("sms de prueba");//the content of the message
  sendATCommand("\x1A");//ctrl-z
  printAllDataFromShield();
}

void getSignalQuality() {
  sendATCommand("AT+CSQ");
  printAllDataFromShield();
}

//-------------------------------------------------------
// Helpers
//-------------------------------------------------------

void sendATCommand(String ATCommand) {
  shieldSerial.print(ATCommand);
  delay(100);
  shieldSerial.print("\r");
  delay(100);
}

int getDataFromShield() {
  if (shieldSerial.available() > 0) {
    return shieldSerial.read();
  } else {
    return -1;
  }
}

void printDataToSerial(int data) {
  Serial.write(data);
  delay(100);
}

void printAllDataFromShield() {
  while (shieldSerial.available() > 0) {
    Serial.write(shieldSerial.read());
    delay(100);
  }
}
