#include <Wire.h>     // incluye libreria para interfaz I2C
#include <RTClib.h>     // incluye libreria para el manejo del modulo RTC
#include <EEPROM.h>     // para el manejo de retención de estados

RTC_DS3231 rtc;     // crea objeto del tipo RTC_DS3231

//variables
int xhour,xminute,xsecond;
int set_hour, set_minute, set_second;
int lastState;

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
bool changeFormat = false;

void setup() {
  Serial.begin(9600);

  if (! rtc.begin()) {     // si falla la inicializacion del modulo
   Serial.println("Modulo RTC no encontrado !");
   while (1);
  }
  if (rtc.lostPower()) {      // si hay problemas de energia
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

  while(configFlag == false) {

    //para después de haber desconectado el arduino
    
    //recuerda la configuracion del tiempo
    DateTime nowTime = rtc.now();
    //para poder cambiar el tiempo desde el que esta establecido
    xhour = nowTime.hour();
    xminute = nowTime.minute();
    xsecond = nowTime.second();
    //mantener los tiempos configurados
    set_hour = nowTime.hour();
    set_minute = nowTime.minute();
    set_second = nowTime.second();
    //recuerda el estado ya escrito en el eeprom
    lastState = EEPROM.read(0);
    
    //muestrar el tiempo
    printTime();
    
    // ir a modo configuracion
    while (digitalRead(pinButtonConfig) == HIGH) {
      delay(3000);
      if (digitalRead(pinButtonConfig) == HIGH && configFlag == false) {
        Serial.println("modo configuración");
        configFlag = true;    
      }
    }
  }

  //modo configuracion
  while(configFlag == true) {
    //reset de estados 
    hourFlag = false;
    minuteFlag = false;
    secondFlag = false;
    aplicarFlag = false;
    changeFormat = false;

    //aplicar cambios
    while (digitalRead(pinButtonConfig) == HIGH) {
      delay(3000);
      if (digitalRead(pinButtonConfig) == HIGH && configFlag == true) {
        rtc.adjust(DateTime(2011, 11, 11, set_hour, set_minute, set_second));
        Serial.println("cambios aplicados (soltar el botón para imprimir el tiempo)");  
        configFlag = false;
      } 
    }
    //cambiar entre formato 24hs y 12hsAM/PM
    while (digitalRead(pinButtonFormat) == HIGH && changeFormat == false) {
      delay(3000);
      if (digitalRead(pinButtonFormat) == HIGH) {
        if ( lastState == 40 ) {
          Serial.println("formato AM/PM");
          EEPROM.write(0, 39);
          lastState = EEPROM.read(0);
        }
        else {
          Serial.println("formato 24hs");
          EEPROM.write(0, 40);
          lastState = EEPROM.read(0);
        } 
        changeFormat = true;
      } 
    }
    //configurar horas
    while (digitalRead(pinButtonH) == HIGH && hourFlag == false) {
      delay(700);
      if (digitalRead(pinButtonH) == HIGH) {
        xhour ++;
        if (xhour > 23) {
          xhour = 0;
        }
        Serial.println(xhour);
      }
      else {
        set_hour = xhour;
        Serial.print("hora configurada ");
        Serial.println(set_hour);
      }
      hourFlag = true;
    }
    //configurar minutos
    while (digitalRead(pinButtonM) == HIGH && minuteFlag == false) {
      delay(700);
      if (digitalRead(pinButtonM) == HIGH) {
        xminute ++;
        if (xminute >= 60) {
          xminute = 0;
        }
        Serial.println(xminute);
      }
      else {
        set_minute = xminute;
        Serial.print("minuto configurado ");
        Serial.println(set_minute);
      }
      minuteFlag = true; 
    }
    //configurar segundos
    while (digitalRead(pinButtonS) == HIGH && secondFlag == false) {
      delay(700);
      if (digitalRead(pinButtonS) == HIGH) {
        xsecond ++;
        if (xsecond >= 60) {
          xsecond = 0;
        }
        Serial.println(xsecond);
      }
      else {
        set_second = xsecond;
        Serial.print("segundo configurado ");
        Serial.println(set_second);
      }
      secondFlag = true;
    } 
  }
}

void printTime() {
  DateTime nowTime = rtc.now(); 
  if ( lastState == 39 ) {    //esta en formato AM/PM
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
  if ( lastState == 40 ) {    //esta en formato 24hs
    Serial.print(nowTime.hour());
    Serial.print(":"); 
    Serial.print(nowTime.minute());
    Serial.print(":"); 
    Serial.println(nowTime.second());
  }
  //imprimir a cada segundo
  delay(1000);
}
