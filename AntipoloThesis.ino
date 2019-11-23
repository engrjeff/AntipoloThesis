#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         49         // Configurable, see typical pin layout above
#define SS_PIN          53         // Configurable, see typical pin layout above
#define SS_PIN_2          47         // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522 rfid2(SS_PIN_2, RST_PIN);  // Create MFRC522 instance

String IDs[2] = {"F9647F98", "A7B72E1B"};
String Objs[2] = {"Object 1", "Object 2"};
String detectedCard = "";
String detectedCard2 = "";
String prevCard = "";
String prevCard2 = "";
bool hasCardOne = false;
String id_one = "";

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  //while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();   // Init MFRC522
  rfid2.PCD_Init();   // Init
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  runSystem();
}


void runSystem() {

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! rfid.PICC_ReadCardSerial())
  {
    Serial.print("previous: ");
    Serial.println(prevCard);
    String objectName = getObjectNameById(prevCard);
    Serial.print(F("Object Taken: "));
    Serial.println(objectName);
    digitalWrite(LED_BUILTIN, HIGH);
    //send sms
    //buzzer
    //bluetooth
    //return;
  }
  else {

    id_one = getIdFromReaderOne();
    if (!id_one.equals("")) {
      Serial.print(F("Detected Card: "));
      Serial.println(id_one);
      prevCard = id_one;
      digitalWrite(LED_BUILTIN, LOW);
      reset();
    }
  }
}

String getObjectNameById(String id) {
  int targetObjIndex = -1;
  for (int index = 0; index < 2; index++) {
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

  for (byte i = 0; i < rfid.uid.size; i++)
  {
    detectedCard.concat(String(rfid.uid.uidByte[i], HEX));
  }
  //rfid.PICC_HaltA();

  detectedCard.toUpperCase();
  return detectedCard;
}
