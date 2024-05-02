#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <gfxfont.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define SerialDebugging true

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 

#include <Wire.h>

const uint8_t   OLED_pin_scl_sck        = 13;
const uint8_t   OLED_pin_sda_mosi       = 11;
const uint8_t   OLED_pin_cs_ss          = 10;
const uint8_t   OLED_pin_res_rst        = 8;
const uint8_t   OLED_pin_dc_rs          = 7;


int upButton = 2;
int downButton = 4;
int selectButton = 5;
const int ledoption1 = 3; //LIGHTS
const int ledoption2 = 6; //FANS
const int ledoption3 = 12;
int menu = 1;
int timer = 1;
int page = 1;
int preset = 1;
int custom = 1;
int run = 1;
int hour = 0;
int minute = 0;
bool lights = false;
bool fans = false;




char timestr[] = "00:00:00";

char timehold[] = "00:00:00";

unsigned long lasttick;

const uint16_t  OLED_Color_Black        = 0x0000;
const uint16_t  OLED_Color_Blue         = 0x001F;
const uint16_t  OLED_Color_Red          = 0xF800;
const uint16_t  OLED_Color_Green        = 0x07E0;
const uint16_t  OLED_Color_Cyan         = 0x07FF;
const uint16_t  OLED_Color_Magenta      = 0xF81F;
const uint16_t  OLED_Color_Yellow       = 0xFFE0;
const uint16_t  OLED_Color_White        = 0xFFFF;

uint16_t        OLED_Text_Color         = OLED_Color_White;
uint16_t        OLED_Background_Color    = OLED_Color_Black;




Adafruit_SSD1351 oled =
  Adafruit_SSD1351(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        &SPI,
        OLED_pin_cs_ss,
        OLED_pin_dc_rs,
        OLED_pin_res_rst
     );

boolean running;

void setup(void) {
  oled.begin();
  oled.setFont(&FreeMono9pt7b);
  oled.fillScreen(OLED_Background_Color);
  oled.setTextColor(OLED_Text_Color);
  oled.setTextSize(1);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  pinMode(ledoption1, OUTPUT);
  pinMode(ledoption2, OUTPUT);
  pinMode(ledoption3, OUTPUT);
  running = false;
  lasttick = micros(); 
  updateMenu();
}

void loop() {
  
  if(running){
    if ((micros() - lasttick) >= 1000000UL) {
      digitalWrite(ledoption1,HIGH);
      digitalWrite(ledoption2,HIGH);
      tickdown(); 
      if (strcmp(timestr,"00:00:00") <= 0) {
        running = false;
        oled.fillScreen(OLED_Background_Color);
        oled.setCursor(0, 10);
        oled.println("Timer\nCompleted");
        delay(1500);
        digitalWrite(ledoption1,LOW);
        digitalWrite(ledoption2,LOW);
        updateMenu();
        strcpy (timestr, timehold);
      }
    }
  }
  
  if (!digitalRead(downButton)){
    menu++;
    DownSelect();
    delay(100);
    while (!digitalRead(downButton));
  }
  if (!digitalRead(upButton)){
    menu--;
    UpSelect();
    delay(100);
    while(!digitalRead(upButton));
  }
  if (!digitalRead(selectButton)){
    MenuSelect();
    delay(100);
    while (!digitalRead(selectButton));
  }
}

void tickdown() {
  oled.fillScreen(OLED_Background_Color);
  oled.setCursor(0, 10);
  oled.println(timestr);
  lasttick += 1000000UL;

  timestr[7]--;
  if (timestr[7]<'0') { timestr[7]+=10; timestr[6]--; }
  if (timestr[6]<'0') { timestr[6]+=6;  timestr[4]--; }
  // minutes
  if (timestr[4]<'0') { timestr[4]+=10; timestr[3]--; }
  if (timestr[3]<'0') { timestr[3]+=6;  timestr[1]--; }
  // hours
  if (timestr[1]<'0') { timestr[1]+=10; timestr[0]--; }
  
  delay(1000);
}

void MenuSelect(){
  switch (page) {
    case 0:
      page = 1;
      break;
    case 1: 
      updateMenu();
      executeAction();
      delay(100);
      break;
    case 2:
      TimerMenu();
      TimerAction();
      delay(100);
      break;
    case 3:
      PresetMenu();
      PresetSelect();
      delay(100);
      break;
    case 4: 
      CustomMenu();
      CustomSelect();
      delay(100);
      break;
    case 5:
      page = 6;
      custom = 2;
      CustomMenu();
      delay(100);
      break;
    case 6:
      page = 4;
      custom = 3;
      CustomMenu();
      delay(100);
      break;
    case 7: 
      break;

  }

}

void UpSelect(){
  switch (page) {
  case 0:
    page = 1;
    break;
  case 1: 
    updateMenu();
    delay(100);
    break;
  case 2:
    timer--;
    TimerMenu();
    delay(100);
    break;
  case 3: 
    preset--;
    PresetMenu();
    delay(100);
    break;
  case 4: 
    custom--;
    CustomMenu();
    delay(100);
    break;
  case 5:
    /*if (hour>0){
      hour--;
      if (hour >= 0 && hour < 9){timestr[1]--;}
      else if (hour == 9){timestr[0]--; timestr[1]+=9;}
      else if (hour > 9 && hour < 19) { timestr[1]-=1; }
      else if (hour == 19) {timestr[0]--; timestr[1]+=9;}
      else if (hour > 19 && hour <= 24) {timestr[1]-=1;}
    } */
    if (hour<24){
      hour++;
      if (hour < 10){timestr[1]++;}
      else if (hour == 10){timestr[0]++; timestr[1]-=9;}
      else if (hour > 10 && hour < 20) { timestr[1]+=1; }
      else if (hour == 20) {timestr[0]++; timestr[1]-=9;}
      else if (hour > 20 && hour <= 24) {timestr[1]+=1;}
    }
    CustomMenu();
    delay(500);
    break;
  case 6:
    /*if (minute>0){
      minute--;
      if (minute >= 0 && minute < 9) {timestr[4]--;}
      else if (minute == 9){timestr[3]--; timestr[4]+=9;}
      else if (minute > 9 && minute < 19) {timestr[4]--;}
      else if (minute == 19){timestr[3]--; timestr[4]+=9;}
      else if (minute > 19 && minute < 29) {timestr[4]--;}
      else if (minute == 29){timestr[3]--; timestr[4]+=9;}
      else if (minute > 29 && minute < 39) {timestr[4]--;}
      else if (minute == 39){timestr[3]--; timestr[4]+=9;}
      else if (minute > 39 && minute < 49) {timestr[4]--;}
      else if (minute == 49){timestr[3]--; timestr[4]+=9;}
      else if (minute > 49 && minute <=59){timestr[4]--;}
    }*/
    if (minute<59){
      minute++;
      if (minute < 10 ) {timestr[4]++;}
      else if (minute == 10){timestr[3]++; timestr[4]-=9;}
      else if (minute > 10 && minute < 20) {timestr[4]++;}
      else if (minute == 20){timestr[3]++; timestr[4]-=9;}
      else if (minute > 20 && minute < 30) {timestr[4]++;}
      else if (minute == 30){timestr[3]++; timestr[4]-=9;}
      else if (minute > 30 && minute < 40) {timestr[4]++;}
      else if (minute == 40){timestr[3]++; timestr[4]-=9;}
      else if (minute > 40 && minute < 50) {timestr[4]++;}
      else if (minute == 50){timestr[3]++; timestr[4]-=9;}
      else if (minute > 50 && minute <=59){timestr[4]++;}
    }
    CustomMenu();
    delay(500);
    break;
  case 7: 
    break;
}}

void DownSelect(){
  switch (page) {
  case 0:
    page = 1;
    break;
  case 1: 
    updateMenu();
    delay(100);
    break;
  case 2:
    timer++;
    TimerMenu();
    delay(100);
    break;
  case 3: 
    preset++;
    PresetMenu();
    delay(100);
    break;
  case 4: 
    custom++;
    CustomMenu();
    delay(100);
    break;
  case 5:
    if (hour>0){
      hour--;
      if (hour >= 0 && hour < 9){timestr[1]--;}
      else if (hour == 9){timestr[0]--; timestr[1]+=9;}
      else if (hour > 9 && hour < 19) { timestr[1]-=1; }
      else if (hour == 19) {timestr[0]--; timestr[1]+=9;}
      else if (hour > 19 && hour <= 24) {timestr[1]-=1;}
    }
    CustomMenu();
    delay(100);
    break;
  case 6:
    if (minute>0){
      minute--;
      if (minute >= 0 && minute < 9) {timestr[4]--;}
      else if (minute == 9){timestr[3]--; timestr[4]+=9;}
      else if (minute > 9 && minute < 19) {timestr[4]--;}
      else if (minute == 19){timestr[3]--; timestr[4]+=9;}
      else if (minute > 19 && minute < 29) {timestr[4]--;}
      else if (minute == 29){timestr[3]--; timestr[4]+=9;}
      else if (minute > 29 && minute < 39) {timestr[4]--;}
      else if (minute == 39){timestr[3]--; timestr[4]+=9;}
      else if (minute > 39 && minute < 49) {timestr[4]--;}
      else if (minute == 49){timestr[3]--; timestr[4]+=9;}
      else if (minute > 49 && minute <=59){timestr[4]--;}
    }
    CustomMenu();
    delay(100);
    break;
  case 7: 
    break;
}}

void updateMenu() {
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Main Menu");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">Start");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" Set Time\n Lights\n Fans");
      break;
    case 2:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Main Menu");
      oled.println(" Start");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">Set Time");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" Lights\n Fans");
      break;
    case 3:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Main Menu");
      oled.println(" Start\n Set Time");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">Lights");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" Fans");
      break;
    case 4:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Main Menu");
      oled.println(" Start\n Set Time\n Lights");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">Fans");
      oled.setFont(&FreeMono9pt7b);
      break;
    case 5:
      menu = 4;
      break;
  }
}

void executeAction() {
  switch (menu) {
    case 1:
      action1();
      break;
    case 2:
      action2();
      break;
    case 3:
      action3();
      break;
    case 4:
      action4();
      break;
  }
}

void action1() { //START
  strcpy(timehold,timestr);
  oled.fillScreen(OLED_Background_Color);
  oled.setCursor(0, 10);
  oled.println("Begin run\nfor:");
  oled.print(hour);
  oled.print(" hour(s)\nand\n");
  oled.print(minute);
  oled.print(" minute(s)");
  delay(500);
  running = true;
}
void action2() { //SET TIME
  page = 2;
  TimerMenu();
}
void action3() { //LIGHTS
  lights = !lights;
  if (lights == true){//LIGHTS INITIALLY OFF
    digitalWrite(ledoption1,HIGH);
    oled.fillScreen(OLED_Background_Color);
    oled.setCursor(0, 10);
    oled.println("Lights ON");
    delay (1500);
  }
  else if (lights == false){
    digitalWrite(ledoption1,LOW);
    oled.fillScreen(OLED_Background_Color);
    oled.setCursor(0, 10);
    oled.println("Lights OFF");
    delay (1500);
  }
  updateMenu();
  page = 1;
}
void action4() { //FANS
  fans = !fans;
  if (fans == true){//FANS INITIALLY OFF
    digitalWrite(ledoption2,HIGH);
    oled.fillScreen(OLED_Background_Color);
    oled.setCursor(0, 10);
    oled.println("Fans ON");
    delay (1500);
  }
  else if (fans == false){//FANS INITIALLY ON
    digitalWrite(ledoption2,LOW);
    oled.fillScreen(OLED_Background_Color);
    oled.setCursor(0, 10);
    oled.println("Fans OFF");
    delay (1500);
  }
  updateMenu();
  page = 1;
}


void TimerMenu(){
  switch(timer){
    case 0:
      timer = 1;
      break;
    case 1:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Timer Menu");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">Preset");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" Custom\n Main Menu");
      break;
    case 2:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Timer Menu\n Preset");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">Custom");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" Main Menu");
      break;
    case 3:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Timer Menu\n Preset\n Custom");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">Main Menu");
      oled.setFont(&FreeMono9pt7b);
      break;
    case 4: 
      timer = 3;
      break;
  }
}

void TimerAction(){
  switch(timer){
    case 1:
      timer1();
      break;
    case 2:
      timer2();
      break;
    case 3: 
      timer3();
      break;
  }
}

void timer1(){
  page = 3;
  PresetMenu();
}

void timer2(){
  page = 4;
  CustomMenu();
}

void timer3(){
  page = 1;
  menu = 1;
  timer = 1;
  digitalWrite(ledoption3, LOW);
  digitalWrite(ledoption2, LOW);
  digitalWrite(ledoption1, LOW);
  updateMenu();
}

void PresetMenu(){
  switch(preset){
    case 0:
      preset = 1;
      break;
    case 1: 
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Presets");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">1 min");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" 10 mins\n 15 mins\n 30 mins\n 1 hour\n Exit");
      break;
    case 2: 
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Presets\n 1 min");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">10 mins");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" 15 mins\n 30 mins\n 1 hour\n Exit");
      break;
    case 3:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Presets\n 1 min\n 10 mins");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">15 mins");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" 30 mins\n 1 hour\n Exit");
      break;
    case 4:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Presets\n 1 min\n 10 mins\n 15 mins");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">30 mins");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" 1 hour\n Exit");
      break;
    case 5:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Presets\n 1 min\n 10 mins\n 15 mins\n 30 mins");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">1 hour");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" Exit");
      break;
    case 6: 
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Presets\n 1 min\n 10 mins\n 15 mins\n 30 mins\n 1 hour");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">Exit");
      oled.setFont(&FreeMono9pt7b);
      break;
  }
}

void PresetSelect(){
  switch(preset){
    case 1:
      hour = 0;
      minute = 1;
      strcpy (timestr, "00:01:00");
      preset = 1;
      timer = 3;
      page = 2;
      TimerMenu();
      break;
    case 2:
      hour = 0;
      minute = 15;
      strcpy (timestr, "00:10:00");
      preset = 1;
      timer = 3;
      page = 2;
      TimerMenu();
      break;
    case 3:
      hour = 0;
      minute = 30;
      strcpy (timestr, "00:15:00");
      preset = 1;
      timer = 3;
      page = 2;
      TimerMenu();
      break;
    case 4:
      hour = 0;
      minute = 45;
      strcpy (timestr, "00:30:00");
      preset = 1;
      timer = 3;
      page = 2;
      TimerMenu();
      break;
    case 5:
      hour = 1;
      minute = 0;
      strcpy (timestr, "01:00:00");
      preset = 1;
      timer = 3;
      page = 2;
      TimerMenu();
      break;
    case 6:
      preset = 1;
      timer = 3;
      page = 2;
      TimerMenu();
      break;
  }

}

void CustomMenu(){
  switch(custom){
    case 0: 
      custom = 1;
      break;
    case 1:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Enter");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">hours");
      oled.setFont(&FreeMono9pt7b);
      oled.println(" minutes");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.setTextSize(2);
      oled.setCursor(0, 85);
      if (hour < 10){
        oled.print("0");
        oled.print(hour);
      }
      else {
        oled.print(hour);
      }
      oled.setFont(&FreeMono9pt7b);
      oled.print(":");
      if (minute < 10){
        oled.print("0");
        oled.print(minute);
      }
      else {
        oled.print(minute);
      }
      oled.setTextSize(1);
      oled.setCursor(0, 100);
      oled.println("\n Done");
      break;
    case 2:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Enter\n hours");
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println(">minutes");
      oled.setFont(&FreeMono9pt7b);
      oled.setTextSize(2);
      oled.setCursor(0, 85);
      if (hour < 10){
        oled.print("0");
        oled.print(hour);
      }
      else {
        oled.print(hour);
      }
      oled.print(":");
      oled.setFont(&FreeMonoBold9pt7b);
      if (minute < 10){
        oled.print("0");
        oled.print(minute);
      }
      else {
        oled.print(minute);
      }
      oled.setFont(&FreeMono9pt7b);
      oled.setTextSize(1);
      oled.setCursor(0, 100);
      oled.println("\n Done");
      break;
    case 3:
      oled.fillScreen(OLED_Background_Color);
      oled.setCursor(0, 10);
      oled.println("Enter\n hours\n minutes");
      oled.setTextSize(2);
      oled.setCursor(0, 85);
      if (hour < 10){
        oled.print("0");
        oled.print(hour);
      }
      else {
        oled.print(hour);
      }
      oled.print(":");
      if (minute < 10){
        oled.print("0");
        oled.print(minute);
      }
      else {
        oled.print(minute);
      }
      oled.setTextSize(1);
      oled.setCursor(0, 100);
      oled.setFont(&FreeMonoBold9pt7b);
      oled.println("\n>Done");
      oled.setFont(&FreeMono9pt7b);
      break;
  }
}

void CustomSelect(){
  switch(custom){
    case 1:
      page = 5;
      CustomMenu();
      delay(500);
      break;
    case 2:
      page = 6;
      CustomMenu();
      delay(500);
      break;
    case 3:
      page = 2;
      menu = 1;
      custom = 1;
      timer = 3;
      preset = 1;
      TimerMenu();
      delay(500);
      break;
  }

}


















