
#include "EasyMFRC522.h"
#include <LiquidCrystal.h>


#define MAX_STRING_SIZE 100  
#define BLOCK 1          

int openTime = 2000;

EasyMFRC522 rfidReader(53, 5); 

int button = 22;

int LCD_RS = 12;
int LCD_ENABLE = 11;
int LCD_D4 = 10;
int LCD_D5 = 9;
int LCD_D6 = 8;
int LCD_D7 = 7;

LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

String removeQuotes(String input) {
  String result = "";
  for (unsigned int i = 0; i < input.length(); i++) {
    if (input[i] != '"') { 
      result += input[i];
    }
  }
  return result;
}


void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(9600);
  Serial.setTimeout(20000);
  rfidReader.init(); 

  pinMode(3, OUTPUT);
 
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);

  pinMode(button, INPUT);
  

  lcd.begin(16,2);
  readYourCard();
}



void loop() {

  

  bool success;
  do {
    success = rfidReader.detectTag();    
    delay(50);
    checkForButton();
  } while (!success);

  
  int result;
  char stringBuffer[MAX_STRING_SIZE];

  result = rfidReader.readFile(BLOCK, "mylabel", (byte*)stringBuffer, MAX_STRING_SIZE);

  stringBuffer[MAX_STRING_SIZE-1] = 0;

  if (result >= 0) {
    if(removeQuotes(stringBuffer) == "OPEN") {
      OPEN();
    } else {
      Serial.println("Belepes megtagadva!");
      lcd.setCursor(0,0);
      accessDenied();
      delay(openTime);
      lcd.setCursor(0,0);
      readYourCard();
    }
  } else if(success) {
    unknownCard();
    delay(openTime);
    readYourCard();
  }


  rfidReader.unselectMifareTag();
  delay(100);
}

void checkForButton(){
  int pressed = digitalRead(button);

  if(pressed) {

  }
}

void OPEN(){
  Serial.println("Belepes engedelyezve!");
  lcd.setCursor(0,0);
  accessGranted();

  digitalWrite(3, HIGH);
  delay(openTime);
  digitalWrite(3, LOW);
  lcd.setCursor(0,0);
  readYourCard();
}

void readYourCard(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("OLVASD LE A");
  lcd.setCursor(0, 1);
  lcd.print("KARTYAD!");
}

void accessGranted(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BELEPES");
  lcd.setCursor(0,1);
  lcd.print("ENGEDELYEZVE!");
}

void accessDenied(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BELEPES");
  lcd.setCursor(0,1);
  lcd.print("MEGTAGADVA!");
}

void unknownCard(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ISMERETLEN");
  lcd.setCursor(0,1);
  lcd.print("KARTYA!");
}


