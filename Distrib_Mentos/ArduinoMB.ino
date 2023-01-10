#include <SPI.h>
#include "LiquidCrystal_I2C.h"
#include "RFID.h"
#include "DHT.h"
#include "Arduino.h"

#define vent A1
#define resist A0

#define cap_prod_fruit 2
#define cap_prod_menthe 3
#define cap_mot_fruit 4
#define cap_mot_menthe 5
#define bout_fruit 6
#define bout_menthe 7
#define mot_fruit A3
#define mot_menthe A2
#define SS_PIN 10 
#define RST_PIN 9         
RFID rfid(SS_PIN, RST_PIN);
String rfidCard;
LiquidCrystal_I2C lcd(0x27,20,4);

DHT dht(8, DHT11);

float t;
float h;

int epuifruit;
int epuimenthe;

String eFruit;
String eMenthe;

String hum1; //setup envoi vers nodered
String temp1;

//String datar;

String serialResponse = "";
char sz[50] ; //"Here; is some; sample;100;data;1.414;1020";
String msg1;
String msg2;
int i=0;
int len;


void setup() 
{
  Serial.begin(9600);

  digitalWrite(vent, HIGH);
  pinMode(vent, OUTPUT);
  digitalWrite(resist,HIGH);
  pinMode(resist, OUTPUT);
  
  dht.begin();
  

  SPI.begin();
  rfid.init();
  lcd.begin();
  lcd.backlight(); //open the backlight
  lcd.print("Initialisation...");
  pinMode(cap_prod_fruit, INPUT);
  pinMode(cap_prod_menthe, INPUT);
  pinMode(cap_mot_fruit, INPUT);
  pinMode(cap_mot_menthe, INPUT);
  pinMode(bout_fruit, INPUT_PULLUP);
  pinMode(bout_menthe, INPUT_PULLUP);
  pinMode(mot_fruit, OUTPUT);
  pinMode(mot_menthe, OUTPUT);

  do
  {
    digitalWrite(mot_fruit, LOW);
  }
  while(digitalRead(cap_mot_fruit) != LOW);
  digitalWrite(mot_fruit, HIGH);
  do
  {
    digitalWrite(mot_menthe, LOW);
  }
  while(digitalRead(cap_mot_menthe) != LOW);
  digitalWrite(mot_menthe, HIGH);
}

void loop() 
{
  lcd.clear();
  lcd.print("Scan your badge...");

  epuifruit = digitalRead(cap_prod_fruit);
  epuimenthe = digitalRead(cap_prod_menthe);
  


  if(rfid.isCard()) {
    if(rfid.readCardSerial()) {
        rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
        Serial.println(rfidCard);
        if(rfidCard == "61 214 217 66") {
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print("Access accepted");
          lcd.setCursor(0,2);
          lcd.print("Choix Mentos");
          while(digitalRead(bout_fruit) == HIGH && digitalRead(bout_menthe) == HIGH){
            Serial.println("Appuie bouton");
          }
          //Choix Mentos
          //Fruit
          //if(digitalRead(bout_fruit) == LOW){
          if(digitalRead(bout_fruit) == LOW && digitalRead(cap_prod_fruit) == HIGH){
            digitalWrite(mot_fruit, LOW);
            delay(1000);
            do{
              digitalWrite(mot_fruit, LOW);
            }while(digitalRead(cap_mot_fruit) != LOW);
            digitalWrite(mot_fruit, HIGH);
            lcd.clear();
          }
          else if(digitalRead(bout_fruit) == LOW && digitalRead(cap_prod_fruit) == LOW){
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("Fruit epuiser");
            delay(3000);
            lcd.clear();
          }
          else{
            Serial.println("Attente");
          }
          //Menthe
          //if(digitalRead(bout_menthe) == LOW){
          if(digitalRead(bout_menthe) == LOW && digitalRead(cap_prod_menthe) == HIGH){
            digitalWrite(mot_menthe, LOW);
            delay(1000);
            do{
              digitalWrite(mot_menthe, LOW);
            }while(digitalRead(cap_mot_menthe) != LOW);
            digitalWrite(mot_menthe, HIGH);
            lcd.clear();
          }
          else if(digitalRead(bout_menthe) == LOW && digitalRead(cap_prod_menthe) == LOW){
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("Menthe epuiser");
            delay(3000);
            lcd.clear();
          }
          else{
            Serial.println("Attente");
          }
        }
        else{
           lcd.clear();
           lcd.setCursor(0,1);
           lcd.print("Access denied");
           lcd.setCursor(0,2);
           lcd.print("Try again");
           delay(3000);
           lcd.clear();
           lcd.print("Scan your badge...");
         }
         }
   rfid.halt();

  

  }

  
  float h = dht.readHumidity();       //lecture capteur temp et hum
  float t = dht.readTemperature();
    
  hum1 = String(int(h));                   
  temp1 = String(int(t));


  if (epuifruit == 0)
  {
     eFruit = "Empty";
  }
  else
  {
     eFruit = "Full";
  }
  if (epuimenthe == 0)
  {
    eMenthe = "Empty";
  }
  else
  {
    eMenthe = "Full";
  }

  Serial.println(hum1 +"\t"+ temp1 +"\t"+ eFruit +"\t"+ eMenthe);
  delay(2);
     
    

  if ( Serial.available()) 
  {
    serialResponse = Serial.readStringUntil("");
i=0;
    // Convert from String Object to String.
    char buf[sizeof(sz)];
    serialResponse.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    while ((str = strtok_r(p, ";", &p)) != NULL) // delimiter is the semicolon
    {
    len = sizeof(str);
    if (i==0)
    {
    msg1=String(str);
    }
    else if (i==1)
    {
    msg2=String(str);
    }
    //
      //Serial.println(str);
      //Serial.println(out1);
      //
      i=i+1;
      
    }
      Serial.println(i);
      Serial.println(msg1);
      Serial.println(msg2);
  

  int tempmax=msg1.toInt();
  int tempmin=msg2.toInt();

  Serial.println(tempmax);
  Serial.println(tempmin);  

    if (t > tempmax)
    {
      digitalWrite(vent, LOW);
    }
    else
    {
      digitalWrite(vent, HIGH);
    }
    if (t < tempmin)
    {
      digitalWrite(resist, LOW);
    }
    else
    {
      digitalWrite(resist, HIGH);
    }

  }
}

