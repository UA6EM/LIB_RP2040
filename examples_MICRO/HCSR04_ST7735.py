from machine import Pin
import utime
import framebuf
import math
from ST7735 import TFT
from sysfont import sysfont
from machine import SPI,Pin
import time


WIDTH  = 160     # tft display width
HEIGHT = 128     # tft display height

"""
// для RP2040
#define TFT_CS         GP5 - CS 
#define TFT_RST        GP6 - RESET
#define TFT_DC         GP7 - A0
#define TFT_SCK        GP2 - SCK
#define TFT_SDA        GP3 - SDA (MOSI, TX) 
#define TFT_LED        Vout
#define TFT_VCC	       3V3 - 3V3
#define TFT_GND	       GND - GND
"""

spi = SPI(0, baudrate=50000000, polarity=0, phase=0,
          sck=Pin(2), mosi=Pin(3), miso=None)
tft=TFT(spi,7,6,5)   #(CLK, DC, RESET, CS)
tft.initg()
tft.rgb(True)
tft.rotation(3)
tft.fill(TFT.BLACK)
tft.text((38, 30), "CTAPT:", TFT.YELLOW, sysfont, 3, nowrap=True)
utime.sleep(3)
tft.fill(TFT.BLACK)
tft.text((12, 30), "Distance:", TFT.YELLOW, sysfont, 3, nowrap=False)
   
trigger = Pin(15, Pin.OUT)
echo = Pin(14, Pin.IN)

def get_distance():
   trigger.low()
   utime.sleep_us(2)
   trigger.high()
   utime.sleep_us(5)
   trigger.low()
   while echo.value() == 0:
       signaloff = utime.ticks_us()
   while echo.value() == 1:
       signalon = utime.ticks_us()
   timepassed = signalon - signaloff
   distance = (timepassed * 0.0343) / 2
   print("The distance from object is ",distance,"cm")
   return distance
while True:
   #tft.fill(TFT.BLACK);
   ret_val = get_distance()
   #tft.text((12, 30), "Distance:", TFT.YELLOW, sysfont, 3, nowrap=True)
   tft.text((22, 60), str(ret_val) + " cm", TFT.YELLOW, sysfont, 2, nowrap=True)   
   utime.sleep(2)
   tft.text((22, 60), str(ret_val) + " cm", TFT.BLACK, sysfont, 2, nowrap=True) 
