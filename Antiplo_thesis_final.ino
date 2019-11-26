#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define RST_PIN         49         // Configurable, see typical pin layout above
#define SS_PIN          53         // Configurable, see typical pin layout above

const int bt_rx = 10; 
const int bt_tx = 11;

const int gsm_rx = 2;
const int gsm_tx = 3;

String activePhoneNumber = "+639287130270"; //Change this to the target user number

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance
SoftwareSerial bt(bt_rx, bt_tx);
SoftwareSerial gsm(gsm_rx, gsm_tx);

int len = 6; //6 objects , 6 tags/cards
String IDs[6] = {"F9647F98", "A7B72E1B", "B64AED", "47AFAB2C", "B9D2A399", "4258111F"};
String Objs[6] = {"Tag-Object 1", "Tag-Object 2", "Tag-Object 3", "Card-Object 4", "Card-Object 5", "Card-Wallet"};
String detectedCard = "";
String prevCard = "";
bool hasCardOne = false;
String id_one = "";

int ctr = 0;
int prevCtr = 0;
int sendCtr = 0;
unsigned long prevMillis = 0;
unsigned long interval = 2000;

const int led = 5;
const int buzzer = 4;

String btData;
bool hasStarted = false;

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  bt.begin(9600); //for Bluetooth
  gsm.begin(19200); //for GSM
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();   // Init MFRC522
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  //sendTxtAlert("Hello This is a Test");
}

void loop() {
  runSystem();
}


void runSystem() {

  unsigned long currentMillis = millis();

  if ((currentMillis - prevMillis) >= interval) {
    prevMillis = currentMillis;
    prevCtr = ctr;
  }

  if (!rfid.PICC_IsNewCardPresent()) {
    ctr++;
    if (ctr > 1000) ctr = 0;
  }
  else {
    ctr--;
    if (ctr < 0) ctr = 0;
  }

  int diff = prevCtr - ctr;
  int diffAbs = abs(diff);

  if (diffAbs > 1) {
    sendCtr++;
    if (sendCtr < 4) {
      //Dump to Serial Monitor
      Serial.println("No Card");
      String objectTaken = getObjectNameById(prevCard);
      Serial.println("Taken Object: " + objectTaken);
      //send msg to bluetooth
      
      digitalWrite(led, HIGH);
      tone(buzzer, 1000);

      bt.println("Alert! An intruder is trying to take:");
      delay(30);
      bt.println(objectTaken);
      sendTxtAlert("Alert! An intruder is trying to take " + objectTaken);
      Serial.println(F("Alerts are sent!"));
      delay(2000);
    }

  }
  else if (diffAbs != 1 && diffAbs == 0) {
    Serial.println("There's a card");
    digitalWrite(led, LOW);
    noTone(buzzer);
    sendCtr = 0;
  }
  id_one = getIdFromReaderOne();
  if (!id_one.equals("")) {
    Serial.print("ID From Reader 1: ");
    Serial.println(id_one);
    prevCard = id_one;
    reset();
  }
}

String getObjectNameById(String id) {
  int targetObjIndex = -1;
  for (int index = 0; index < len; index++) {
    if (IDs[index] == id) {
      targetObjIndex = index;
      break;
    }
  }
  return Objs[targetObjIndex];
}

void reset() {
  detectedCard = "";
}

String getIdFromReaderOne() {
  if (!rfid.PICC_ReadCardSerial())
  {
    return "";
  }
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    detectedCard.concat(String(rfid.uid.uidByte[i], HEX));
  }
  //rfid.PICC_HaltA();

  detectedCard.toUpperCase();
  return detectedCard;
}

void sendTxtAlert(String sms) {
  gsm.println("AT+CMGF=1");
  delay(800);
  gsm.println("AT+CMGS=\"" +  activePhoneNumber + "\"\r");
  delay(800);
  gsm.println(sms);
  delay(800);
  gsm.println((char)26);// ASCII code of CTRL+Z
  delay(100);
  Serial.println("Message has been sent!");
}
