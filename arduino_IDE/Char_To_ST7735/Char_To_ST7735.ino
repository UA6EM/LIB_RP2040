
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define SPI01 // Дисплей на SPI1

#if defined(SPI01)        // для RP2040 SPI1
#define TFT_CS        13  // GP13 - CS
#define TFT_RST       14  // GP14 - RESET
#define TFT_DC        15  // GP15 - A0
#define TFT_MISO      12  // GP12 - MISO (MISO, RX)
#define TFT_MOSI      11  // GP11 - SDA  (MOSI, TX)
#define TFT_SCLK      10  // GP10 - SCK

#else                       // для RP2040 SPI0
#define TFT_CS        5   // GP5 - CS
#define TFT_RST       6   // GP6 - RESET
#define TFT_DC        7   // GP7 - A0
#define TFT_MISO      4   // GP4 - MISO (MISO, RX)
#define TFT_MOSI      3   // GP3 - SDA  (MOSI, TX)
#define TFT_SCLK      2   // GP2 - SCK
#endif

// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


bool stops = 0;
void setup() {
  Serial.begin(115200);
  //delay(10000);
  pinMode(29, OUTPUT);
  digitalWrite(29, HIGH); // Led ON

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(1);
  tft.setSPISpeed(50000000);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextWrap(true);
  //tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  tft.setTextSize(1);
  for (int i = 32; i <= 127; i++) {
    tft.print(char(i));
    tft.print(" ");
  }
  while (!Serial.available());
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  for (int i = 128; i <= 255; i++) {
    tft.print(char(i));
    tft.print(" ");
  }
}

  void loop() {
  }
