
#include <Wire.h>     // incluye libreria para interfaz I2C
#include <RTClib.h>     // incluye libreria para el manejo del modulo RTC

RTC_DS3231 rtc;     // crea objeto del tipo RTC_DS3231

int xyear,xmonth,xday,xhour,xminute,xsecond;
int set_hour, set_minute, set_second;


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
int a, b;

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
 
  //rtc.adjust(DateTime(__DATE__, __TIME__)); 

  //setting inputs
  pinMode(pinButtonConfig, INPUT);    
  pinMode(pinButtonFormat, INPUT);    
  pinMode(pinButtonH, INPUT);    
  pinMode(pinButtonM, INPUT);    
  pinMode(pinButtonS, INPUT);    

}

void loop() {

  //reloj funcionando MIENTRAS configFlag = false
  while(configFlag == false) {
    
    DateTime nowTime = rtc.now();
    
    xhour = nowTime.hour();
    xminute = nowTime.minute();
    xsecond = nowTime.second();

    
    
    printTime();

   
    // ir a modo configuracion
    while (digitalRead(pinButtonConfig) == HIGH) {
      delay(3000);
      Serial.println("entrando al modo configuracion");
      if (digitalRead(pinButtonConfig) == HIGH) {
        configFlag = true;    
      }
    }

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
      delay(500);
      Serial.println("cambiando formato");
      if (digitalRead(pinButtonFormat) == LOW) {

        Serial.println("cambió");
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
  DateTime nowTime = rtc.now();      // funcion que devuelve fecha y horario en formato
  nowTime.setClockMode(true)

  //Serial.println(formatHours);
  a = nowTime.hour();
  
  //Serial.println(a);
  
  if ( formatHours == true && a > 12 ) {
    a = a - 12;

    // Muestreo
    //Serial.print(nowTime.hour());      // funcion que obtiene la hora de la fecha completa
    Serial.print(a);
    Serial.print(":");       // caracter dos puntos como separador
    Serial.print(nowTime.minute());      // funcion que obtiene los minutos de la fecha completa
    Serial.print(":");       // caracter dos puntos como separador
    Serial.print(nowTime.second());    // funcion que obtiene los segundos de la fecha completa
    Serial.print(" ");
    Serial.println("PM");
    delay(1000);        // demora de 1 segundo
  } else {
    // Muestreo
    Serial.print(nowTime.hour());      // funcion que obtiene la hora de la fecha completa
    Serial.print(":");       // caracter dos puntos como separador
    Serial.print(nowTime.minute());      // funcion que obtiene los minutos de la fecha completa
    Serial.print(":");       // caracter dos puntos como separador
    Serial.println(nowTime.second());    // funcion que obtiene los segundos de la fecha completa
    delay(1000);        // demora de 1 segundo 
 
    /* Muestreo
    Serial.print(set_hour);      // funcion que obtiene la hora de la fecha completa
    Serial.print(":");       // caracter dos puntos como separador
    Serial.print(set_minute);      // funcion que obtiene los minutos de la fecha completa
    Serial.print(":");       // caracter dos puntos como separador
    Serial.println(set_second);    // funcion que obtiene los segundos de la fecha completa
    delay(1000);        // demora de 1 segundo
    */
  }
  
}
