#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define SPI01

#if defined(SPI01)          // для RP2040 SPI1
#define TFT_CS        13  // GP13 - CS 
#define TFT_RST       14  // GP14 - RESET
#define TFT_DC        15  // GP15 - A0
#define TFT_MOSI      11  // GP11 - SDA (MOSI, TX)
#define TFT_SCLK      10  // GP10 - SCK 

#else                       // для RP2040 SPI0
#define TFT_CS        5   // GP5 - CS 
#define TFT_RST       6   // GP6 - RESET
#define TFT_DC        7   // GP7 - A0
#define TFT_MOSI      3   // GP3 - SDA (MOSI, TX)
#define TFT_SCLK      2   // GP2 - SCK
#endif

// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// DS18B20
#include <OneWire.h>
#define Pin_VccDS 20  // DS18B20 - VDD (+3.3V) R 4.7k -[]-
#define Pin_SigDS 21  // DS18B20 - Data        R 4.7k ----

OneWire  ds(Pin_SigDS);
byte addr[8];    // = {0x28, 0xEC, 0xF0, 0xD5, 0x04, 0x00, 0x00, 0xF2};

volatile float temperature;
uint32_t timing;
uint32_t t1;
uint32_t t2;

// HC-SR04
const int trigPin = 17;
const int echoPin = 16;

//define sound speed in cm/uS
#define SOUND_SPEED 0.035
#define CM_TO_INCH 0.393701

int32_t duration;
float distanceCm;
float distanceInch;


// Chip INT Temperature
int16_t chipTemperature;



void getDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED / 2;

  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  //  Serial.print("Distance (inch): ");
  //  Serial.println(distanceInch);
}


void getTemperature()
{
  int16_t temp;
  t1 = micros();
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);                       // Считывание значения с датчика
  temp = (ds.read() | ds.read() << 8);  // Принимаем два байта температуры
  temperature = (float)temp / 16.0;     // round((float)temp / 16.0);
  ds.reset();
  ds.write(0xCC);                       // Обращение ко всем датчикам
  ds.write(0x44);                       // Команда на конвертацию
  t2 = micros();
}


void setup(void) {
  Serial.begin(115200);
  Serial.print(F("Hello! ST77xx TFT Test"));

  // HC-SR04
  pinMode(trigPin, OUTPUT);   // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);    // Sets the echoPin as an Input

  // TFT LED
  pinMode(29, OUTPUT);
  digitalWrite(29, HIGH); // Led ON
  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(1);
  tft.setSPISpeed(20000000);
  Serial.println(F("Initialized"));
  uint32_t times = millis();
  times = millis() - times;
  Serial.println(times, DEC);
  Serial.println("done");
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Temp = ");

  // DS18B20
  pinMode(Pin_VccDS, OUTPUT);
  digitalWrite(Pin_VccDS, HIGH); // DS_VCC ON

label:
  ds.reset();      // сброс шины
  ds.search(addr); // определение адреса датчика
  if (OneWire::crc8(addr, 7) != addr[7])
  { //проверка массива адреса на срц
    Serial.println F("CRC ADRESS DS18B20 is not valid!");
    goto label;
  }
  ds.reset();      // сброс шины
  ds.select(addr); // выставить адрес
  ds.write(0x4E);  // разрешение записать конфиг
  ds.write(0x7F);  // Th контроль температуры макс 128грд
  ds.write(0xFF);  // Tl контроль температуры мин -128грд
  ds.write(0x60);  // 0x60 12-бит разрешение, 0x00 -9бит разрешение
  ds.write(0x7F);  // точность 0,5гр = 1F; 0,25гр = 3F; 0,125гр = 5F; 0,0625гр = 7F;

  // запросим температуру, считывать будем по таймеру
  ds.reset();
  ds.write(0xCC); // Обращение ко всем датчикам
  ds.write(0x44); // Команда на конвертацию
  timing = millis();

}// ******* END SETUP *******



void loop() {

  if (millis() - timing > 1000)
  {
    timing = millis();
    getTemperature();
    Serial.print("  Temperature = ");
    Serial.print(temperature, 3);

    getDistance();
    chipTemperature = analogReadTemp();


    tft.setCursor(78, 30);
    tft.setTextSize(2);
    tft.setCursor(78, 30);
    tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
    tft.print(temperature, 1);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.fillCircle(134, 30, 2, ST77XX_WHITE);
    tft.setCursor(140, 30);
    tft.print("C");

    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.setCursor(0, 60);
    tft.print("Dist = ");
    tft.println(distanceCm);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

    tft.setCursor(0, 90);
    tft.print("Chip = ");
    tft.println(chipTemperature, DEC);
    tft.fillCircle(114, 90, 2, ST77XX_WHITE);
    tft.setCursor(120, 90);
    tft.print("C");


  }
}
