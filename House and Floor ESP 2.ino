#define BLYNK_TEMPLATE_ID "TMPL6AeLHr5Uk"
#define BLYNK_TEMPLATE_NAME "IoT Smart Car"
#define BLYNK_AUTH_TOKEN "7HH45BxX0ntawWd6ab5vfiUKxIddy3oA"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Mastermind";
char pass[] = "2001zone";

bool fetch_blynk_state = true;  //true or false

//#define BLYNK_PRINT Serial
#include <WiFi.h>

#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// #include <ESP8266WiFi.h>
// #include <WiFiClient.h>
// #include <BlynkSimpleEsp8266.h>

// define the GPIO connected with Sensor
#define Di_Sensor1   19
#define Di_Sensor2   18
#define Di_Sensor3   5
#define Di_Sensor4   17
#define Di_Sensor5   21
#define Di_Sensor6   22
// #define Di_Sensor1   16
// #define Di_Sensor2   5
// #define Di_Sensor3   4
// #define Di_Sensor4   0
// #define Di_Sensor5   2
// #define Di_Sensor6   15

#define wifiLed   2   //D2

//Change the virtual pins according the rooms
#define VIR_SENSOR_1    V1
#define VIR_SENSOR_2    V2
#define VIR_SENSOR_3    V3
#define VIR_SENSOR_4    V4
#define VIR_SENSOR_5    V5
#define VIR_SENSOR_6    V6

void check_sensor();

int wifiFlag = 0;

char auth[] = BLYNK_AUTH_TOKEN;

void check_sensor()
{
  int ss1;
  int ss2;
  int ss3;
  int ss4;
  int ss5;
  int ss6;

  if(digitalRead(Di_Sensor1) == LOW)
  {
    ss1=1;
  }
  else ss1=0;

  if(digitalRead(Di_Sensor2) == LOW)
  {
    ss2=1;
  }
  else ss2=0;

  if(digitalRead(Di_Sensor3) == LOW)
  {
    ss3=1;
  }
  else ss3=0;

  if(digitalRead(Di_Sensor4) == LOW)
  {
    ss4=1;
  }
  else ss4=0;

  if(digitalRead(Di_Sensor5) == LOW)
  {
    ss5=1;
  }
  else ss5=0;

  if(digitalRead(Di_Sensor6) == LOW)
  {
    ss6=1;
  }
  else ss6=0;

  Blynk.virtualWrite(VIR_SENSOR_1,   ss1);
  Serial.println(ss1);
  Blynk.virtualWrite(VIR_SENSOR_2,   ss2);
  Blynk.virtualWrite(VIR_SENSOR_3,   ss3);
  Blynk.virtualWrite(VIR_SENSOR_4,   ss4);
  Blynk.virtualWrite(VIR_SENSOR_5,   ss5);
  Blynk.virtualWrite(VIR_SENSOR_6,   ss6);

}


BlynkTimer timer;

// When App button is pushed - switch the state

void checkBlynkStatus()
{ // called every 3 seconds by SimpleTimer
 
  bool isconnected = Blynk.connected();
  if (isconnected == false)
  {
    wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true)
  {
    wifiFlag = 0;
    digitalWrite(wifiLed, HIGH);
    Serial.println("Blynk Connected");
  }
  check_sensor();
}
 
 
void setup()
{
  Serial.begin(9600);
 
  pinMode(Di_Sensor1, INPUT);
  pinMode(Di_Sensor2, INPUT);
  pinMode(Di_Sensor3, INPUT);
  pinMode(Di_Sensor4, INPUT);
  pinMode(Di_Sensor5, INPUT);
  pinMode(Di_Sensor6, INPUT);

 
  //pinMode(wifiLed, OUTPUT);
 
  //During Starting all Relays should TURN OFF
  //digitalWrite(wifiLed, LOW);
  //Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  Blynk.config(auth);
  delay(1000);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates SimpleTimer
  // Serial.println(analogRead(Di_Sensor1));

}
 