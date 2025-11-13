#include "EasyMFRC522.h"
#include <LiquidCrystal.h>

int MAX_STRING_SIZE = 100;
int BLOCK = 1;
int openTime = 2000;
int button = 22;
int LCD_RS = 12;
int LCD_ENABLE = 11;
int LCD_D4 = 10;
int LCD_D5 = 9;
int LCD_D6 = 8;
int LCD_D7 = 7;
int displayTime = 3000;

bool waitForMasterCard = false;
bool waitForBlankCard = false;

EasyMFRC522 rfidReader(53, 5);
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

String proccessIncomingData(String input){
  String result = "";
  for (unsigned int i = 0; i < input.lenght(); i++){
    if(input[i] != '"') {
      result += input[i];
    }
  }
  return result;
}

void screen(String row1, String row2){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(row1);
  lcd.setCursor(0, 1);
  lcd.print(row2);
}

void writeOPEN(){
  char writeStringBuffer[MAX_STRING_SIZE];
  strcpy(writeStringBuffer, "OPEN");
  int result;
  int stringSize = strlen(writeStringBuffer)
  result = rfidReader.writeFile(BLOCK, "myLabel", (byte*)writeStringBuffer, stringSize+1);
  screen("KLONOZAS", "SIKERES");
  delay(displayTime);
  waitForMasterCard = false;
  waitForBlankCard = false;
  screen("OLVASD LE A", "KARTYAD!");
}

void checkForButton(){
  if(digitalRead(button)){
    waitForMasterCard = true;
    screen("OLVASD LE A", "MASTERT!");
  }
}

int OPEN(){
  Serial.println("Belepes engedelyezve!");
  screen("BELEPES", "ENGEDELYEZVE!");
  digitalWrite(3, HIGH);
  delay(openTime);
  digitalWrite(3, LOW);
  screen("OLVASD LE A", "KARTYAD!");
}

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(20000);
  rfidReader.init();
  lcd.begin(16, 2);

  pinMode(3, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(13, HIGH);
  digitalWrite(14, HIGH);

  screen("OLVASD LE A", "KARTYAD!");
}

void loop(){
  bool readState = false;
  do {
    readState = rfidReader.detectTag();
    delay(50);
    if(!waitForMasterCard){
      checkForButton();
    }
  } while(!readState);

  int result;
  char stringBuffer[MAX_STRING_SIZE];
  result = rfidReader.readFile(BLOCK, "myLabel", (byte*)stringBuffer, MAX_STRING_SIZE);
  stringBuffer[MAX_STRING_SIZE-1] = 0;

  bool skipUnknownMsg = false;

  if(result >= 0) {
    bool skipErrMsg = false;
    bool skipEntrance = false;
    String data;
    data = proccessIncomingData(stringBuffer);

    if(waitForMasterCard){
      skipEntrance = true;
      if(data != "MASTER"){
        waitForMasterCard = false;
        skipErrMsg = true;
        skipUnknownMsg = true;
        screen("OLVASD LE A", "KARTYAD!");
      }
    } else if(waitForBlankCard){
      if(data != "MASTER"){
        skipEntrance = true;
        skipErrMsg = true;
        skipUnknownMsg = true;
        writeOPEN();
      } else {
        waitForMasterCard = false;
        waitForBlankCard = false;
        screen("KLONOZAS", "SIKERTELEN");
        delay(displayTime);
        skipEntrance = true;
        screen("OLVASD LE A", "KARTYAD!");
      }
    }

    if(data == "OPEN" || data == "MASTER"){
      if(waitForMasterCard){
        skipEntrance = true;
        if(data == "MASTER"){
          waitForMasterCard = false;
          waitForBlankCard = true;
          screen("MASOLAS...", "UJ KARTYA:");
        }
      } else {
        if(!skipEntrance && !waitForMasterCard && !waitForBlankCard){
          OPEN();
        }
      }
    } else {
      if(!skipErrMsg){
        Serial.println("Belepes megtagadva!");
        screen("BELEPES", "MEGTAGADVA!");
        delay(openTime);
        screen("OLVASD LE A", "KARTYAD!");
      }
    }
  } else if(success){
    if(!skipUnknownMsg){
      screen("ISMERETLEN", "KARTYA!");
      delay(openTime);
    }
    screen("OLVASD LE A", "KARTYAT!");
  }
  rfidReader.unselectMifareTag();
  delay(100);
}
