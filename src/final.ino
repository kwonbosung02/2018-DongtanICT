#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#include <Wire.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define WIFI_SSID "WIFI ID HERE"
#define WIFI_PASSWORD "WIFIPW HERE"

#define FIREBASE_HOST "FIREBASEURL HERE"
#define FIREBASE_AUTH "FIREBASE PASSWD HERE"

uint8_t putChar1[8] = {0xf0,0xf0,0x1f,0x4,0x1f,0x0,0x1f,0x21};
uint8_t putChar2[8] = {0xa,0x1e,0xa,0xa,0x17,0x16,0x16,0x16};
uint8_t putChar3[8] = {0x15,0x15,0x1d,0x15,0x1d,0xe,0x11,0xe};
uint8_t putChar4[8] = {0x4,0xa,0x11,0x0,0x1f,0x4,0x4,0x4};
uint8_t putChar5[8] = {0x0,0xa,0x16,0x17,0x16,0x16,0xa,0x0};
uint8_t putChar6[8] = {0x0,0x9,0x15,0x15,0x15,0x15,0x9,0x0};
uint8_t putChar7[8] = {0x0,0x1d,0x11,0x11,0x11,0x11,0x1d,0x0};

int timer = 0;

int sit_pin = 14;
int exit_pin = 12;

void printLog(String msg){
  lcd.setCursor(5,0);
  lcd.print(msg);
  lcd.print("           ");
}

void printId(String msg){
  lcd.setCursor(5,1);
  lcd.print(msg);
  lcd.print("           ");
}

void setup() {
  Serial.begin(9600);
  
  Wire.begin(2,0);
  
  lcd.init();   
  lcd.backlight();

  lcd.createChar(0, putChar1); lcd.createChar(1, putChar2); 
  lcd.createChar(2, putChar3); lcd.createChar(3, putChar4); 
  lcd.createChar(4, putChar5); lcd.createChar(5, putChar6);
  lcd.createChar(6, putChar7); 

  lcd.setCursor(0,0);
  lcd.print((char)0);lcd.print((char)1); lcd.print((char)2);lcd.print((char)3);
  
  lcd.setCursor(0,1);
  lcd.print((char)4); lcd.print((char)5);lcd.print((char)6); lcd.print(":");

  printLog("Wait...");

  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("connecting");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  printLog("Connected");

  
  Serial.println();
  Serial.print("connected");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  timer = millis();//설정
}

bool prevSit = 0;
bool prevExit = 0;

long lastDetected = 0;
//-------------------------------------
void loop() {
  switchCheck();
  
  String userId = Firebase.getString("/seat/test1/userId");
  bool isReserved = Firebase.getBool("/seat/test1/isReserved");

  if(isReserved){
    printId(userId);
  } else {
    printId("No User");
  }
  //-----------------------------------------------
  long offlineTime = millis() - lastDetected;
  Serial.println(offlineTime);
  //---------------------------------------------------
  if(!prevSit && offlineTime > 1000*10){
    Firebase.setBool("/seat/test1/kick", true);
    
  }


  if (Firebase.failed()) {
      Serial.print("Firebase Error: ");
      Serial.println(Firebase.error());
  }



  
}

void switchCheck(){
  bool currSit = digitalRead(sit_pin);
  bool currExit = digitalRead(exit_pin);
  if(currSit != prevSit){
    prevSit = currSit;

    if(currSit){
      sit(currSit);
    } else {
      sit(currSit);
    }
  }
  
  if(currExit != prevExit){
    prevExit = currExit;
    
    if(currExit){
      exit();
    } else {
      
    }
  }
}

void sit(bool b){
  if(b){
    Firebase.setBool("/seat/test1/isPeopleDetected", b);
  } else 
  {
    lastDetected = millis();
    Firebase.setBool("/seat/test1/isPeopleDetected", b);
    Firebase.setInt("/seat/test1/lastDetected", lastDetected);
  //  Firebase.setInt("/seat/test1/offlineTime",millis());//-------------
  }
  
}

void exit(){
  Firebase.setBool("/seat/test1/isPeopleDetected", false);
  Firebase.setBool("/seat/test1/isReserved", false);
  Firebase.setBool("/seat/test1/kick", false);
 
  Firebase.setInt("/seat/test1/lastDetected", 0);
  Firebase.setString("/seat/test1/seatName", "A1");
  Firebase.setString("/seat/test1/userId", "");

}

