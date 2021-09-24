//подключение библиотек
#include <Wire.h> //библиотека для i2c
#include <LiquidCrystal_I2C.h> //библиотека для дисплея
#include "DS3231.h" //библиотека для часов
#include <Button.h> //библиотека для кнопки
#include <Adafruit_Sensor.h> //библиотека для датчика жестов
#include <DHT.h> //библиотека для датчика температуры и влажности
#include <DHT_U.h> //библиотека для датчика температуры и влажности
#include <IRremote.h>

#define DHTPIN 3 //определение пина для датчика температуры и влажности
#define DHTTYPE    DHT11     //  определение типа датчика температуры и влажности

Button button1(2); //создание объекта кнопка на пине 2
DS3231 clock; //создание объекта часы
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
DHT_Unified dht(DHTPIN, DHTTYPE); //создание объекта датчика температуры и влажности
IRrecv irrecv(7);

bool h12Flag; //в каком виде показывать время 12-24
bool pmFlag; //показывать или нет am/pm
bool century = false; //????

byte year; //переменная для записи года
byte month; //переменная для записи месяца
byte date; //переменная для записи даты
byte hour; // переменная для записи часа
byte minutes; //переменная для записи минут
byte seconds; // переменная для записи секунд

int btn=0; //переменная для отслеживания пункта меню, всего 5 пунктов (0,1,2,3,4)
bool light = false;
int buttonState = 0; // variable for reading the pushbutton status

int RECV_PIN = 7;
decode_results results;
void setup()
{
  //включение всех датчиков
  lcd.init();
  lcd.init();
  lcd.noBacklight();
  Serial.begin(115200);
  Wire.begin();
  button1.begin();
  dht.begin();
  irrecv.enableIRIn();
}


void loop()
{
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    irrecv.resume(); // Receive the next value
  }
  
  sensors_event_t event; //переменная для получения показаний с датчика температуры и влажности

    if(light==false && results.value==16726215) {
      results.value=0;
      lcd.backlight();
      light=true;
      Serial.print("light:");
      Serial.println(light);
    }
    if(light==true && results.value==16726215) {
      results.value=0;
      lcd.noBacklight();
      light=false;
      Serial.print("light:");
      Serial.println(light);
    }

  if (results.value==16734885) { //переключение меню направо
    results.value=0;
    btn=btn+1;
    Serial.print("btn:");
    Serial.println(btn);
    lcd.clear();
  }
  if (results.value==16716015) { //переключение меню налево
    results.value=0;
    btn=btn-1;
    Serial.print("btn:");
    Serial.println(btn);
    lcd.clear();
  }  
  if (btn==5){ //возврат в первый пункт меню (состояние 0)
    btn=0;
  }
  if (btn==-1) {
    btn=4;
  }
  //обработка пунктов меню
  if (btn==0) {//всё выключено
    digitalWrite(9, LOW); //выключить светодиоды
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    //lcd.noBacklight(); //выключить дисплей
  } else if(btn==1) {//часы
    digitalWrite(9, HIGH);//включить первый светодиод
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    //lcd.backlight(); //включить дисплей
    year = clock.getYear(); //получить информацию от часов
    month = clock.getMonth(century);
    date = clock.getDate();
    hour = clock.getHour(h12Flag, pmFlag);
    minutes = clock.getMinute();
    seconds = clock.getSecond();
    lcd.setCursor(0,1); //пишем время на 2ю строку
    lcd.print(hour);
    lcd.setCursor(2,1);
    lcd.print(":");
    lcd.setCursor(3,1);
    lcd.print(minutes);
    lcd.setCursor(5,1);
    lcd.print(":");
    lcd.setCursor(6,1);
    lcd.print(seconds);  
    lcd.setCursor(0,0);//пишем дату на 1ю строку
    lcd.print(year);
    lcd.setCursor(2,0);
    lcd.print(":");
    lcd.setCursor(3,0);
    lcd.print(month);
    lcd.setCursor(5,0);
    lcd.print(":");
    lcd.setCursor(6,0);
    lcd.print(date);
  } else if(btn==2) {//температура
    digitalWrite(9, LOW);
    digitalWrite(10, HIGH);//включаем второй светодиод
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    dht.temperature().getEvent(&event);//обращение к датчику температуры и влажности
    lcd.setCursor(0,0);
    lcd.print("Temperature:");
    lcd.setCursor(0,1);
    lcd.print(event.temperature);
  } else if(btn==3) {//тут пока пусто
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);//включить третий светодиод
    digitalWrite(12, LOW);
  } else if(btn==4){//влажность
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, HIGH);//включить четвертый светодиод
    dht.humidity().getEvent(&event); //обращение к датчику температуры и влажности
    lcd.setCursor(0,0);
    lcd.print("Humidity:");
    lcd.setCursor(0,1);
    lcd.print(event.relative_humidity);
    lcd.setCursor(8,1);
    lcd.print("%");
  }

}
