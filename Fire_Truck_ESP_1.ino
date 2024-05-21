#define BLYNK_TEMPLATE_ID "TMPL6AeLHr5Uk" 
#define BLYNK_TEMPLATE_NAME "IoT Smart Car"
#define BLYNK_AUTH_TOKEN "7HH45BxX0ntawWd6ab5vfiUKxIddy3oA"
#include <HTTPClient.h>
#include<string.h>
#include <stdlib.h>
#include<ESP32Servo.h>

Servo HTurret; //name of horizontal servo motor, init
Servo VTurret; //name of vertical servo motor, init

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Greenwich-Guest 2.4G";//;
char pass[] = "greenwichvn@123";

bool fetch_blynk_state = true;  //true or false

//#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h> 
#include <SPI.h>
#include <MFRC522.h>

#define enA 25
#define in1 26  
#define in2 27
#define in3 14
#define in4 12
#define enB 33
#define echo 35
#define trigger 32  
//---------RFID
#define SS_PIN 5
#define RST_PIN 0
MFRC522 mfrc522(SS_PIN, RST_PIN);
//---------5 IC to detect line
#define ir1 15
#define ir2 2
#define ir3 16
#define ir4 4
#define ir5 17
//---------servo
#define servoPin1 22 // pin of horizontal servo motor
#define servoPin2 21 // pin of vertical servo motor
#define relayPin 13 // pin to on/off motor

int pos = 0;
int right = 90; //the farthest the motor will rotate, to the right
int Vpos = 160;
int Hpos = 90;

int Set=15;
int distance_F; 
int value_sensor1();
int value_sensor2();
int value_sensor3();
int value_sensor4();
int value_sensor5();
int value_sensor6();
void initWiFi(); // initialization wifi
void checkBlynkStatus();
void check_progress(int location, int status[]);// Wait for extinguishing process comple
void moveToLocation(int location);// Move to a house that location point to
void system(int status[], int *ptr_status);// Direct the car to the nearest house fire
void Move(); // Move the car
void water_spray(int fl);// water spray until extinguishing the fire
void Return_first_pos();
void floor(int fl);
void check_floor(int status);
void forward();//car go forward
void backward();//car go backward
void left();// Turn left
void left2();// Turn left more strong
void Right();//Turn right
void Right2();//Turn right more strong
void stop();// Stop the car
void followline();//car find line
void move_black_line(); //
void avoid_obstacles(); //Avoid obstacles
void updateStatus(); // Update status of all house
char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;
HTTPClient http;
String UID_Tag="";

void RFID_Read(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {  
    return;
  }
  Serial.print("UID: ");   
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  { 
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  UID_Tag = content.substring(1);
  mfrc522.PICC_HaltA();  
  mfrc522.PCD_StopCrypto1();
  Serial.println(""); 
}

void forward(){ 
analogWrite(enA, 120); 
analogWrite(enB, 120); 
digitalWrite(in1, LOW); 
digitalWrite(in2, HIGH); 
digitalWrite(in3, HIGH); 
digitalWrite(in4, LOW); 
}

void backward(){ 
digitalWrite(in1, HIGH);
digitalWrite(in2, LOW);
digitalWrite(in3, LOW);
digitalWrite(in4, HIGH);
analogWrite(enA, 150); 
analogWrite(enB, 150);
}

void left(){ 
digitalWrite(in1, LOW); 
digitalWrite(in2, HIGH);
digitalWrite(in3, LOW); 
digitalWrite(in4, LOW);
analogWrite(enA, 125); 
analogWrite(enB, 125);
}

void left2(){
digitalWrite(in1, LOW);
digitalWrite(in2, HIGH);
digitalWrite(in3, LOW); 
digitalWrite(in4, HIGH);
analogWrite(enA, 115); 
analogWrite(enB, 115);
}
void Right(){ 
digitalWrite(in1, LOW); 
digitalWrite(in2, LOW);
digitalWrite(in3, HIGH); 
digitalWrite(in4, LOW); 
analogWrite(enA, 125); 
analogWrite(enB, 125);
}
void Right2(){ 
digitalWrite(in1, HIGH);
digitalWrite(in2, LOW);
digitalWrite(in3, HIGH); 
digitalWrite(in4, LOW);
analogWrite(enA, 125); 
analogWrite(enB, 125);
}

void stop(){
digitalWrite(in1, LOW);
digitalWrite(in2, LOW);
digitalWrite(in3, LOW); 
digitalWrite(in4, LOW);
}

void followline(){
analogWrite(enA, 85); 
analogWrite(enB, 85); 
digitalWrite(in1, LOW); 
digitalWrite(in2, HIGH); 
digitalWrite(in3, HIGH); 
digitalWrite(in4, LOW); 
}


void move_black_line() {
    int s1 = digitalRead(ir1); 
    int s2 = digitalRead(ir2); 
    int s3 = digitalRead(ir3);  
    int s4 = digitalRead(ir4);  
    int s5 = digitalRead(ir5);

    if (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 0)
      Right2();
    else if (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 0 && s5 == 0)
      Right2();
    else if (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 0 && s5 == 1)
      Right();
    else if (s1 == 1 && s2 == 1 && s3 == 0 && s4 == 0 && s5 == 1)
      Right();
    else if (s1 == 1 && s2 == 1 && s3 == 0 && s4 == 1 && s5 == 1)
      forward();
    else if (s1 == 1 && s2 == 0 && s3 == 0 && s4 == 1 && s5 == 1)
      left();
    else if (s1 == 1 && s2 == 0 && s3 == 1 && s4 == 1 && s5 == 1)
      left();
    else if (s1 == 0 && s2 == 0 && s3 == 1 && s4 == 1 && s5 == 1)
      left2();
    else if (s1 == 0 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 1)
      left2();
    else if (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 1)
      followline();
}
void distance(){
  distance_F = Ultrasonic_read();
  Serial.print("D F=");Serial.println(distance_F);
}
void avoid_obstacles(){
  backward();
  delay(330);
  left2();
  delay(530);
  forward();
  delay(600);
  left2();
  delay(100);
  Right2();
  delay(300);
  forward();
  delay(135);
  Right2();
  delay(210);
  left2();
  delay(45);
  Right2();
  delay(400);
}


int value_sensor1()
{
  http.begin("https://blynk.cloud/external/api/get?token=7HH45BxX0ntawWd6ab5vfiUKxIddy3oA&V1");
  String payload;
  int httpcode=http.GET();
  int value;
  if (httpcode>0)
  {
    payload=http.getString();
    // Serial.println(httpcode);
    // value=atoi(pay);
    // Serial.println(payload);
  }
  value=payload.toInt();
  //Serial.println(value);
  return value;
}

int value_sensor2()
{
  http.begin("https://blynk.cloud/external/api/get?token=7HH45BxX0ntawWd6ab5vfiUKxIddy3oA&V2");
  String payload;
  int httpcode=http.GET();
  int value;
  if (httpcode>0)
  {
    payload=http.getString();
    // Serial.println(httpcode);
    // value=atoi(pay);
    // Serial.println(payload);
  }
  value=payload.toInt();
  //Serial.println(value);
  return value;
}

int value_sensor3()
{
  http.begin("https://blynk.cloud/external/api/get?token=7HH45BxX0ntawWd6ab5vfiUKxIddy3oA&V3");
  String payload;
  int httpcode=http.GET();
  int value;
  if (httpcode>0)
  {
    payload=http.getString();
    // Serial.println(httpcode);
    // value=atoi(pay);
    // Serial.println(payload);
  }
  value=payload.toInt();
  //Serial.println(value);
  return value;
}

int value_sensor4()
{
  http.begin("https://blynk.cloud/external/api/get?token=7HH45BxX0ntawWd6ab5vfiUKxIddy3oA&V4");
  String payload;
  int httpcode=http.GET();
  int value;
  if (httpcode>0)
  {
    payload=http.getString();
    // Serial.println(httpcode);
    // value=atoi(pay);
    // Serial.println(payload);
  }
  value=payload.toInt();
  //Serial.println(value);
  return value;
}

int value_sensor5()
{
  http.begin("https://blynk.cloud/external/api/get?token=7HH45BxX0ntawWd6ab5vfiUKxIddy3oA&V5");
  String payload;
  int httpcode=http.GET();
  int value;
  if (httpcode>0)
  {
    payload=http.getString();
    // Serial.println(httpcode);
    // value=atoi(pay);
    // Serial.println(payload);
  }
  value=payload.toInt();
  //Serial.println(value);
  return value;
}

int value_sensor6()
{
  http.begin("https://blynk.cloud/external/api/get?token=7HH45BxX0ntawWd6ab5vfiUKxIddy3oA&V6");
  String payload;
  int httpcode=http.GET();
  int value;
  if (httpcode>0)
  {
    payload=http.getString();
    // Serial.println(httpcode);
    // value=atoi(pay);
    // Serial.println(payload);
  }
  value=payload.toInt();
  //Serial.println(value);
  return value;
}
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
void checkBlynkStatus()
{ // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
    // Serial.println(ss);
}
void moveToLocation(int location)
{
    if(location == 1)
    {
        while(UID_Tag != "E4 85 EA BB")// VE TRAM
        {
          Serial.println("VEtram");
          RFID_Read();
          Move();
        }
        stop();
    }
      
    else if(location == 2) // Den nha 1
    {
        while(UID_Tag != "D3 15 31 08")
        {
          Serial.println("House 2");
          RFID_Read();
          Move();
        }
        stop();
    }
    else if(location == 3) // Den nha 2
    {
      while(UID_Tag != "24 6F F6 BB")
          {
            Serial.println("House 2");
            RFID_Read();
            Move();
          }
          stop();
    }

    else if(location == 4) // Den nha 3
    {
      while(UID_Tag != "22 A2 A9 21")
        {
          RFID_Read();
          Serial.print("House 2");
          Move();
        }
        stop();
    }
}
int checkLocation(int *ptr, int status[]) 
{
    if(ptr == &status[1] || ptr == &status[2])
        return 2;
    else if(ptr == &status[3] || ptr == &status[4])
        return 3;
    else if(ptr == &status[5] || ptr == &status[6])
        return 4;
}

void system(int status[], int *ptr_status) 
{
    int house_fire = 7;
    for(int i =0; i < 7; i++)
    {
        if(status[i] == 1)
        {
            while(&status[i] != ptr_status)
            {
                if(&status[i] > ptr_status) ptr_status++;
                else if((&status[i] < ptr_status)) ptr_status--;
            }

            moveToLocation(checkLocation(ptr_status, status));
            check_progress(checkLocation(ptr_status, status), status);
            i = 0;
            updateStatus();
        }
            house_fire--;
    }
    if(!house_fire)//If there is no fire house anymore, the car will turn back to the station
        moveToLocation(1);
}
void check_progress(int location, int status[]) //  Wait for extinguishing process comple
{
    if(location == 2)
    {
        if(status[1])
        {
          floor(1);
          while(status[1])
          {
            water_spray(1);
            status[1] = value_sensor1();
          }
          check_floor(value_sensor2());
        }
            
        else if(status[2])
          {
            floor(2);
            while(status[2])
          {
            water_spray(2);
            status[2] = value_sensor2();
          }
          check_floor(value_sensor1());
            }
    }

    else if(location == 3)
    {
        if(status[3])
        {  
          floor(1);
          while(status[3])
          {
            water_spray(1);
            status[3] = value_sensor3();
          }
          check_floor(value_sensor4());
        }
        else if(status[4])
        {
          floor(2);
          while(status[4])
          {
            water_spray(2);
            status[4] = value_sensor4();
          }
          check_floor(value_sensor3());
        }
    }

    else if(location == 4)
    {
        if(status[5])
        {
          floor(1);
          while(status[5])
          {
            water_spray(1);
            status[5] = value_sensor5();
          }
          check_floor(value_sensor6());
        }
        else if(status[6])
        {
          floor(2);
          while(status[6])
          {
            water_spray(2);
            status[6] = value_sensor6();
          }
          check_floor(value_sensor5());
        }
    }
    
}

void water_spray(int fl)
{
    
    if(fl==1)
    {
    digitalWrite(relayPin,HIGH);
    VTurret.write(100);
    delay(150);
    HTurret.write(80);
    delay(150);
    VTurret.write(70);
    delay(150);
    HTurret.write(50);
    delay(150);
    }else{
    digitalWrite(relayPin,HIGH);
    VTurret.write(100);
    delay(150);
    HTurret.write(120);
    delay(150);
    VTurret.write(80);
    delay(150);
    HTurret.write(90);
    delay(150);
    }
}

void Return_first_pos(){
  digitalWrite(relayPin,LOW);
  delay(500);
  VTurret.write(right);
  delay(1000);
  HTurret.write(Hpos);
  delay(1000);
  VTurret.write(Vpos);
  delay(1000);
}

void floor(int fl){
  if(fl == 1)
  {
  digitalWrite(relayPin,LOW);
  VTurret.write(right);
  delay(1000);
  HTurret.write(60);
  delay(1000);
  }      
  else
  {
  digitalWrite(relayPin,LOW);
  VTurret.write(right);
  delay(1000);
  HTurret.write(120);
  delay(1000);
  }
}
void check_floor(int status)
{
  if(!status)
    Return_first_pos();
}
void setup()
{
  Serial.begin(9600);
  //During Starting all Relays should TURN OFF
  //Blynk.begin(auth, ssid, pass);
  initWiFi();
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  Blynk.config(auth);
  SPI.begin();
  delay(1000);      
  mfrc522.PCD_Init();  

pinMode(ir1, INPUT);
pinMode(ir2, INPUT);
pinMode(ir3, INPUT);
pinMode(ir4, INPUT);
pinMode(ir5, INPUT);

pinMode(echo, INPUT );
pinMode(trigger, OUTPUT);

pinMode(enA, OUTPUT); 
pinMode(in1, OUTPUT); 
pinMode(in2, OUTPUT); 
pinMode(in3, OUTPUT);   
pinMode(in4, OUTPUT); 
pinMode(enB, OUTPUT); 


  HTurret.attach(servoPin1);
  VTurret.attach(servoPin2);
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin,LOW);

  analogWrite(enA, 180); 
  analogWrite(enB, 180);
  
}
int Ultrasonic_read(){
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  int time = pulseIn (echo, HIGH);
  return time / 58.2;
}
int status[] = {0, value_sensor1(), value_sensor2(), value_sensor3(), value_sensor4(), value_sensor5(), value_sensor6()}; // Decleared the list to store each status of house
int *ptr_status = status;
void updateStatus()
{
  status[1] = value_sensor1();
  status[2] = value_sensor2();
  status[3] = value_sensor3();
  status[4] = value_sensor4();
  status[5] = value_sensor5();
  status[6] = value_sensor6();
}
void Move()
{
  int s1 = digitalRead(ir1); 
  int s2 = digitalRead(ir2); 
  int s3 = digitalRead(ir3);  
  int s4 = digitalRead(ir4);  
  int s5 = digitalRead(ir5);
  distance();
  move_black_line();
  if(distance_F < 13)
  {
    avoid_obstacles();
  }
}
void loop()
{
  Blynk.run(); 
  timer.run(); // Initiates SimpleTimer
  updateStatus();
  system(status, ptr_status);
}
