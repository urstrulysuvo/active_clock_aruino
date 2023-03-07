#include<SoftwareSerial.h>
#include<DS3231.h>
#include "MySensor.h"
#include "MyClock.h"
#include<math.h>
#include<SPI.h>
#include<DMD2.h>
#include<fonts/SystemFont5x7.h>
#define SENSOR 4
#define BUZZER 5

SoftDMD dmd(2,1);
DMD_TextBox box(dmd, 2, 4, 60, 7);
//DS3231 *clock;

String input = "\0";
String message = "Welcome To ASH R&D";
String morningMSG = "Good Morning!";
String afternoonMSG = "Good Afternoon!";
String eveningMSG = "Good Evening!";
String breakfastMSG = "It's time for Breakfast";
String lunchMSG = "It's time for Lunch";
String dinnerMSG = "It's time for Dinner";
int a = 0;
bool showGreeting = true;
Sensor *sensor;
Clock *my_clock;

void setup() {
  Serial.begin(9600);
  Serial.println("welcome");
  sensor = new Sensor(SENSOR);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
  my_clock = new Clock(SDA, SCL);
  //clock.begin();
  dmd.setBrightness(20);
  dmd.selectFont(SystemFont5x7);
  dmd.begin(); 
  displayMessage("Hello World!", 1);
  dmd.clearScreen();
}

void displayMessage(String msg, int repeat) {
  dmd.clearScreen();  
  msg = " " + msg + " ";
  int length = msg.length();
  int index = 0, count = 0;
  while(count < repeat) {
    Serial.print(msg[index]);
    box.print(msg[index]);
    delay(400);
    index ++;
    if(index == length) {
      index = 0;
      ++ count;
    }
  }
  dmd.clearScreen();
}

void showTime() {
  Serial.print(clock.getDOWStr());
  Serial.print(" ");
  Serial.print(clock.getDateStr());
  Serial.print(" ");
  Serial.println(clock.getTimeStr());
  delay(1000);
}

void runTime(){
  Time t = clock.getTime();
  int h = t.hour % 12;
  char x = ':',y = ' ';

  if(h == 0){                    
    dmd.drawString(11, 0, "12");
  } else {
    displayTwoDigit(11, 0, h);
  }
  if(a == 0){
    dmd.drawChar(23, 0, x);
    a ++;
  }
  else{
    dmd.drawChar(23, 0, y);
    a --;
  }
  
  displayTwoDigit(28, 0, t.min);
  String weekDay = String(clock.getDOWStr());
  if(t.hour > 11) {
    dmd.drawString(42, 0, "PM");
  }
  else {
    dmd.drawString(42, 0, "AM");
  }
  dmd.drawString(01,9, weekDay);
  displayTwoDigit(22, 9, t.date); 
  dmd.drawChar(33, 9, '.');
  displayTwoDigit(37, 9, t.mon);
  dmd.drawChar(48, 9, '.');
  dmd.drawString(52, 9, String(t.year).substring(2));
  
  if(t.min == 0 && showGreeting) {
    int i = 0;
    while(i ++ < h) {
      digitalWrite(BUZZER, LOW);
      delay(300);
      digitalWrite(BUZZER, HIGH);
      delay(300);
    }
  }
  
  if (t.min % 15 == 0 && showGreeting) {
    greetingMessage(t);
    showGreeting = false;
  }
  if(t.min % 15 == 1)
    showGreeting = true;
}

void greetingMessage(Time time) {
  int h = time.hour;
  int m = time.min;
  if((h == 7 || h == 8) && (m % 30 != 0)) {
      displayMessage(breakfastMSG, 1);
      return;
  }
  if((h == 12 || h == 13) && (m % 30 != 0)) {
      displayMessage(lunchMSG, 1);
      return;
  }
  if((h == 20 || h == 21) && (m % 30 != 0)) {
      displayMessage(dinnerMSG, 1);
      return;
  }
  if(time.hour >= 16) {
    displayMessage(eveningMSG, 1);
    return;
  }
  if (time.hour == 12 && time.min == 0) {
    displayMessage("Good Noon!", 1);
    return;
  }
  if(time.hour >= 12) {
    displayMessage(afternoonMSG, 1);
    return;
  }
  if(time.hour >= 4) {
    displayMessage(morningMSG, 1);
    return;
  }
}

void displayTwoDigit(int posX, int posY, int digit) {
  if(digit < 10){
    dmd.drawString(posX, posY,"0");
    dmd.drawString(posX + 6, posY,String(digit));  
  }
  else{
    dmd.drawString(posX, posY, String(digit));
  }
}

void setupMode() {
  bool isReady = false;
  Serial.println(F("setup mode on"));
  input = "\0";
  while(!isReady) {
    flashDigits(); 
    if(Serial.available() > 0) {
      input = Serial.readStringUntil("\n");
      isReady = true;
    }
  }

  dmd.clearScreen();

  if(input.substring(0, 3) == "msg"){
    message = input.substring(3);
    Serial.println("succeed");
  } else if(input.toInt() == 110) {
    Serial.println("cancel by user");
  } else {
    my_clock->update(input);
    Serial.println("succeed");
  }
  isReady = false;
  input = "\0";
}

void flashDigits(){
  dmd.drawString(0, 9, "...........");
  dmd.drawString(23, 0, ":");  
  if(a == 0){
    dmd.drawString(11,0, "00");
    dmd.drawString(28,0, "00");    
    dmd.drawString(42,0, "00");
    a ++;
  }
  else{
    dmd.drawString(11,0, "  ");
    dmd.drawString(28,0, "  ");   
    dmd.drawString(42,0, "  ");
    a --;
  }  
  delay(200);
}

void loop() {
  if(sensor->isActive()) {
    Serial.println("welcome message");
    displayMessage(message, 1);
  }
  runTime();
  showTime();
  if(Serial.available() > 0) {
    input = Serial.readStringUntil("\n");
  }
  if(input.toInt() == 200) {
    setupMode();
    delay(1000);
  }
}
