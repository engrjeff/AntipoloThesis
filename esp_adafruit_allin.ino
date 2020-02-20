/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "SegoviaWiFi"
#define WLAN_PASS       "SegoviaFamily"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "JeffSegovia"
#define AIO_KEY         "aio_weJr88vxA2djFKC1QeQxHlCwlD6K"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish ir_sensor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ir_state");

// Setup a feed called 'trash_servo' for subscribing to changes.
Adafruit_MQTT_Subscribe trash_servo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/trash_servo");

Servo trashServo;

//Pin Definition
const int ir_pin = D1;
const int servo_pin = D2;
const int led_pin = D3;
bool is_full = false;
bool previous_state = false;
int open_angle = 30;
int close_angle = 130;
/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  pinMode(ir_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  trashServo.attach(servo_pin);
  trashServo.write(close_angle);
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&trash_servo);
}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();


  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(500))) {
    if (subscription == &trash_servo) {
      Serial.print(F("Got: "));
      Serial.println((char *)trash_servo.lastread);
      char * lastRead = (char *)trash_servo.lastread;
      //check value
      if (strcmp(lastRead, "OPEN") == 0) {
        trashServo.write(open_angle);
      }
      else if (strcmp(lastRead, "CLOSE") == 0) {
        trashServo.write(close_angle);
      }
    }
  }


  // Now we can publish stuff!
  Serial.print(F("\nSending IR value "));
  is_full = digitalRead(ir_pin);
  is_full = !is_full;
  digitalWrite(led_pin, is_full);
  if (previous_state != is_full) {
    String ir_state = "";
    if (is_full == true) {
      ir_state = "The trash can is full.";
    }
    else {
      ir_state = "The trash can is NOT full.";
    }
    Serial.print(ir_state);
    Serial.print("...");

    unsigned int len = ir_state.length();
    char state[len];
    ir_state.toCharArray(state, len);
    if (! ir_sensor.publish(state)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }
  }

  previous_state = is_full;


  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
    if(! mqtt.ping()) {
    mqtt.disconnect();
    }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
