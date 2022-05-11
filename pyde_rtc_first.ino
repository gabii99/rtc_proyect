
#include <Wire.h>     // incluye libreria para interfaz I2C
#include <RTClib.h>     // incluye libreria para el manejo del modulo RTC
#include <EEPROM.h>

RTC_DS3231 rtc;     // crea objeto del tipo RTC_DS3231

int xyear,xmonth,xday,xhour,xminute,xsecond;
int set_hour, set_minute, set_second;
int lastState;

/*
DateTime nowTime = rtc.now();
int set_hour = nowTime.hour();
int set_minute = nowTime.minute();
int set_second = nowTime.second();
*/
//pin set
const int pinButtonConfig = 2;
const int pinButtonFormat = 3;
const int pinButtonH = 4;
const int pinButtonM = 5;
const int pinButtonS = 6;

//flags
bool configFlag = false;
bool hourFlag = false;
bool minuteFlag = false;
bool secondFlag = false;
bool aplicarFlag = false;
bool formatHours = false;
bool changeFormat = false;

bool eepromFormat = false;

void setup() {
  Serial.begin(9600);

  if (! rtc.begin()) {     // si falla la inicializacion del modulo
   Serial.println("Modulo RTC no encontrado !");
   while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  //setting inputs
  pinMode(pinButtonConfig, INPUT);    
  pinMode(pinButtonFormat, INPUT);    
  pinMode(pinButtonH, INPUT);    
  pinMode(pinButtonM, INPUT);    
  pinMode(pinButtonS, INPUT);    

  //rtc.adjust(DateTime(__DATE__, __TIME__));

}

void loop() {

  //reloj funcionando MIENTRAS configFlag = false
  while(configFlag == false) {
    
    //recuerda la configuracion del tiempo
    DateTime nowTime = rtc.now();

    xhour = nowTime.hour();
    xminute = nowTime.minute();
    xsecond = nowTime.second();
   
    set_hour = nowTime.hour();
    set_minute = nowTime.minute();
    set_second = nowTime.second();

    //recuerda el estado ya escrito en el eeprom
    lastState = EEPROM.read(0);
   
    // ir a modo configuracion
    while (digitalRead(pinButtonConfig) == HIGH) {
      delay(3000);
      Serial.println("entrando al modo configuracion");
      if (digitalRead(pinButtonConfig) == HIGH) {
         
        configFlag = true;    
      }
    }

    //print al final para comprobar primero alguna interrupción
    printTime();

  }


  //modo configuracion
  while(configFlag == true) {
    delay(300);
    Serial.println("estoy dentro");

    //reset de estados 
    hourFlag = false;
    minuteFlag = false;
    secondFlag = false;
    aplicarFlag = false;
    changeFormat = false;
    //formatHours = false;
    
    //cambiar entre formato 24hs y 12hsAM/PM
    while (digitalRead(pinButtonFormat) == HIGH && changeFormat == false) {
      delay(1000);
      if (digitalRead(pinButtonFormat) == HIGH) {

        Serial.println("cambió");
        
        if ( eepromFormat == false ) {
          eepromFormat = true;  
          EEPROM.write(0, 39);
          lastState = EEPROM.read(0);
        }
        else {
          eepromFormat = false;
          EEPROM.write(0, 40);
          lastState = EEPROM.read(0);
        }

        Serial.println(eepromFormat);
        
        //registro algo
        if (formatHours == false) {
          formatHours = true;
        }
        else {
          formatHours = false;
        }
        
        changeFormat = true; 
      }
      
    }

    //configurar horas formato 24hs
    while (digitalRead(pinButtonH) == HIGH && hourFlag == false && formatHours == false) {
      delay(400);
      
      if (digitalRead(pinButtonH) == LOW) {
        
        set_hour = xhour - 1;
        Serial.print("hora configurada ");
        Serial.println(set_hour);
        
        hourFlag = true;
      }
      
      if (xhour == 24) {
        xhour = 0;
      }
      Serial.println(xhour);
      xhour ++;
      
    }
    //configurar horas formato 12hsAM/PM
    while (digitalRead(pinButtonH) == HIGH && hourFlag == false && formatHours == true) {
      delay(400);
      
      if (digitalRead(pinButtonH) == LOW) {
        
        set_hour = xhour - 1;
        Serial.print("hora configurada ");
        Serial.println(set_hour);
        
        hourFlag = true;
      }
      
      if (xhour > 12) {
        xhour = 0;
      }
      Serial.println(xhour);
      xhour ++;
      
    }
    
    //configurar minutos
    while (digitalRead(pinButtonM) == HIGH && minuteFlag == false) {
      delay(400);
      
      if (digitalRead(pinButtonM) == LOW) {
        
        set_minute = xminute - 1;
        Serial.print("hora configurada ");
        Serial.println(set_minute);
        
        minuteFlag = true;
      }
      
      if (xminute == 60) {
        xminute = 0;
      }
      Serial.println(xminute);
      xminute ++;
      
    }
    //configurar segundos
    while (digitalRead(pinButtonS) == HIGH && secondFlag == false) {
      delay(400);
      
      if (digitalRead(pinButtonS) == LOW) {
        
        set_second = xsecond - 1;
        Serial.print("hora configurada ");
        Serial.println(set_second);
        
        secondFlag = true;
      }
      
      if (xsecond == 60) {
        xsecond = 0;
      }
      Serial.println(xsecond);
      xsecond ++;
      
    }
    
    //aplicar cambios
    while (digitalRead(pinButtonConfig) == HIGH && aplicarFlag == false) {
      delay(3000);
      Serial.println("saliendo al modo configuracion");
      if (digitalRead(pinButtonConfig) == HIGH) {

        rtc.adjust(DateTime(2020, 12, 4, set_hour, set_minute, set_second));

        configFlag = false; 
        aplicarFlag = true;
      }
      
    }
    
  }

}

void printTime() {
  DateTime nowTime = rtc.now(); 

  if ( lastState == 39 ) {    //eepromFormat == true
    Serial.print(nowTime.twelveHour());
    Serial.print(":"); 
    Serial.print(nowTime.minute());
    Serial.print(":"); 
    Serial.print(nowTime.second());
    Serial.print(" ");
    if (nowTime.isPM() == 0){
       Serial.println("AM");
    }
    else{
      Serial.println("PM");
    }
  }
  if ( lastState == 40 ) {    //eepromFormat == false
    Serial.print(nowTime.hour());
    Serial.print(":"); 
    Serial.print(nowTime.minute());
    Serial.print(":"); 
    Serial.print(nowTime.second());
    Serial.println(" ");  
  }
  
  delay(1000);
}
