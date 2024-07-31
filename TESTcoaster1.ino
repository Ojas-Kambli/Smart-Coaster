/**********************************************************************************
 *  TITLE: Blynk + Manual Button + IR + DHT11 control 4 Relays using ESP32 (Real time feedback + no WiFi control)
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/VNeT5QgH-IM
 *  Related Blog : https://iotcircuithub.com/esp32-projects/
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP32 : https://github.com/espressif/arduino-esp32
 *
 *  Download the libraries 
 *  Blynk Library (1.0.1):  https://github.com/blynkkk/blynk-library
 *  AceButton Library (1.9.2): https://github.com/bxparks/AceButton
 *  IRremote Library (3.6.1): https://github.com/Arduino-IRremote/Arduino-IRremote
 *  DHT Library (1.4.3): https://github.com/adafruit/DHT-sensor-library
 **********************************************************************************/

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID "TMPL3mfyUjynE"
#define BLYNK_TEMPLATE_NAME "Coaster"
#define BLYNK_AUTH_TOKEN "u4igYWANQiNlxZ2YvPmgImOix8kI7epS"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Coaster";
char pass[] = "coaster@123";

bool fetch_blynk_state = true;  //true or false

//#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>  
#include <AceButton.h>
#include <LiquidCrystal_I2C.h>
using namespace ace_button;

#define DHTPIN              16 //D16  pin connected with DHT
#define IR_Sensor         35 // D35 (IR receiver pin)

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301


  
// define the GPIO connected with Relays and switches
#define RelayPin1 23  //D23
#define RelayPin2 22  //D22
#define RelayPin3 21  //D21
#define RelayPin4 19  //D19

#define SwitchPin1 13  //D13
#define SwitchPin2 12  //D12
#define SwitchPin3 14  //D14
#define SwitchPin4 27  //D27

#define wifiLed   2   //D2

//Change the virtual pins according the rooms
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2
#define VPIN_BUTTON_3    V3 
#define VPIN_BUTTON_4    V4

#define VPIN_BUTTON_C    V5
#define VPIN_TEMPERATURE V6
#define VPIN_HUMIDITY    V7

#define Buzzer_pin 25
LiquidCrystal_I2C lcd(0x27,16,2);
// Relay State
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW; //Define integer to remember the toggle state for relay 3
bool toggleState_4 = LOW; //Define integer to remember the toggle state for relay 4
int IR_Reading = LOW;
int wifiFlag = 0;
float temperature1 = 0;
float humidity1   = 0;
float temp1=5;
float temp2=40;
char auth[] = BLYNK_AUTH_TOKEN;

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);

void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);

BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);

// When App button is pushed - switch the state

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  IR_Reading = digitalRead(IR_Sensor);
  if(IR_Reading == LOW)
  {
    digitalWrite(RelayPin1, !toggleState_1);
  }
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  IR_Reading = digitalRead(IR_Sensor);
  if(IR_Reading == LOW)
  {
    digitalWrite(RelayPin2, !toggleState_2);
  }
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  IR_Reading = digitalRead(IR_Sensor);
  if(IR_Reading == LOW)
  {
    digitalWrite(RelayPin3, !toggleState_3);
  }
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  toggleState_4 = param.asInt();
  IR_Reading = digitalRead(IR_Sensor);
  if(IR_Reading == LOW)
  {
    digitalWrite(RelayPin4, !toggleState_4);
  }
}

BLYNK_WRITE(VPIN_BUTTON_C) {
  all_SwitchOff();
}

void all_SwitchOff(){
  toggleState_1 = 0; digitalWrite(RelayPin1, HIGH); Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1); delay(100);
  toggleState_2 = 0; digitalWrite(RelayPin2, HIGH); Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2); delay(100);
  toggleState_3 = 0; digitalWrite(RelayPin3, HIGH); Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3); delay(100);
  toggleState_4 = 0; digitalWrite(RelayPin4, HIGH); Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4); delay(100);
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity1);
  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature1);
}

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    wifiFlag = 0;
    if (!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
    }
    digitalWrite(wifiLed, HIGH);
    //Serial.println("Blynk Connected");
  }
}

BLYNK_CONNECTED() {
  // Request the latest state from the server
  if (fetch_blynk_state){
    Blynk.syncVirtual(VPIN_BUTTON_1);
    Blynk.syncVirtual(VPIN_BUTTON_2);
    Blynk.syncVirtual(VPIN_BUTTON_3);
    Blynk.syncVirtual(VPIN_BUTTON_4);
  }
    Blynk.syncVirtual(VPIN_TEMPERATURE);
    Blynk.syncVirtual(VPIN_HUMIDITY);
}

void readSensor(){
  
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
    humidity1 = h;
    temperature1 = t;
   // Serial.println(temperature1);
   // Serial.println(humidity1);
  }  
}

void sendSensor()
{
  readSensor();
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  if (temperature1>=temp2 || temperature1<=temp1)
  {
    tone(Buzzer_pin, 1000); // Send 1KHz sound signal...
    delay(1000);         // ...for 1 sec
    noTone(Buzzer_pin);     // Stop sound...
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Temperature");
  lcd.setCursor(2,1);
  lcd.print(String(temperature1)+String(" Celcius"))
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity1);
  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature1);
}

void setup()
{
  Serial.begin(9600);
  while (!Serial) 
  {
    ; 
  }
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Temperature");
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, !toggleState_1);
  digitalWrite(RelayPin2, !toggleState_2);
  digitalWrite(RelayPin3, !toggleState_3);
  digitalWrite(RelayPin4, !toggleState_4);

  pinMode(IR_Sensor, INPUT);
  pinMode(Buzzer_pin, OUTPUT);
  dht.begin();    // Enabling DHT sensor

  digitalWrite(wifiLed, LOW);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  config3.setEventHandler(button3Handler);
  config4.setEventHandler(button4Handler);
  
  button1.init(SwitchPin1);
  button2.init(SwitchPin2);
  button3.init(SwitchPin3);
  button4.init(SwitchPin4);

  //Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  timer.setInterval(1000L, sendSensor); // Sending Sensor Data to Blynk Cloud every 1 second
  Blynk.config(auth);
  delay(1000);
  
  if (!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
  }
}

void loop()
{ 
  //ir_remote(); //IR remote Control
  
  Blynk.run();
  timer.run(); // Initiates SimpleTimer

  button1.check();
  button2.check();
  button3.check();
  button4.check();
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventReleased:
      IR_Reading=digitalRead(IR_Sensor);
      if(IR_Reading==LOW)
      {
        digitalWrite(RelayPin1, toggleState_1);
        toggleState_1 = !toggleState_1;
        Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
      }
      break;
  }
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  switch (eventType) {
    case AceButton::kEventReleased:
      IR_Reading=digitalRead(IR_Sensor);
      if(IR_Reading==LOW)
      {
        digitalWrite(RelayPin2, toggleState_2);
        toggleState_2 = !toggleState_2;
        Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
      }
      break;
  }
}
void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT3");
  switch (eventType) {
    case AceButton::kEventReleased:
      IR_Reading=digitalRead(IR_Sensor);
      if(IR_Reading==LOW)
      {
        digitalWrite(RelayPin3, toggleState_3);
        toggleState_3 = !toggleState_3;
        Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
      }
      break;
  }
}
void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT4");
  switch (eventType) {
    case AceButton::kEventReleased:
      IR_Reading=digitalRead(IR_Sensor);
      if(IR_Reading==LOW)
      {
        digitalWrite(RelayPin4, toggleState_4);
        toggleState_4 = !toggleState_4;
        Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
      }
      break;
  }
}
