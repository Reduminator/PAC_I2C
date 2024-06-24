#include <Wire.h>      // libreria de comunicacion por I2C
#include <LCD.h>      // libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>    // libreria para LCD por I2C
#include <AHT20.h>
#include <RTClib.h>   // incluye libreria para el manejo del modulo RTC


LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7
RTC_DS3231 rtc;     // crea objeto del tipo RTC_DS3231
AHT20 aht20; // Instanciamos el objeto del tipo ATH20

void setup() {
  Serial.begin(9600);    // inicializa comunicacion serie a 9600 bps
  
  lcd.setBacklightPin(3,POSITIVE);  // puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH);   // habilita iluminacion posterior de LCD
  lcd.begin(16, 2);     // 16 columnas por 2 lineas para LCD 1602A
  lcd.clear();      // limpia pantalla

  if (! rtc.begin()) {       // si falla la inicializacion del modulo
  Serial.println("Modulo RTC no encontrado !");  // muestra mensaje de error
  while (1);         // bucle infinito que detiene ejecucion del programa
  }
  
  rtc.adjust(DateTime(__DATE__, __TIME__));  // funcion que permite establecer fecha y horario
             // al momento de la compilacion. Comentar esta linea
            // y volver a subir para normal operacion
  
  if(aht20.begin() == false){
    Serial.println("AHT20 not detected. Please check wiring. Freezing.");
  }
}
void loop() {

  //Obtener temperatura y humedad del ATH20
  int tempC_aht20 = aht20.getTemperature();
  int humidity_aht20 = aht20.getHumidity();

  //Obtener fecha y hora del RTC
  DateTime fecha = rtc.now();      // funcion que devuelve fecha y horario en formato
                                   // DateTime y asigna a variable fecha
  int dia = fecha.day();
  int mes = fecha.month();
  int anio = fecha.year();
  int hora = fecha.hour();
  int minut = fecha.minute();

  // Mostrar fecha y hora en LCD
  lcd.setCursor(0, 0);    // ubica cursor en columna 0 y linea 0
  lcd.print(dia);
  lcd.setCursor(2, 0);
  lcd.print("/");
  lcd.setCursor(3, 0);
  lcd.print(mes);
  lcd.setCursor(5, 0);
  lcd.print("/");
  lcd.setCursor(6, 0);
  lcd.print(anio);
  
  lcd.setCursor(11, 0);
  lcd.print(hora);
  lcd.setCursor(13, 0);
  lcd.print(":");
  lcd.setCursor(14, 0);
  lcd.print(minut);

  //Mostrar temperatura y humedad en LCD
  lcd.setCursor(0, 1);
  lcd.print(tempC_aht20);
  lcd.setCursor(2, 1);
  lcd.print("C");
  lcd.setCursor(4, 1);
  lcd.print(humidity_aht20);
  lcd.setCursor(7, 1);
  lcd.print("% RH");
}
