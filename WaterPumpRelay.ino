
#include <SPI.h>
#include <Wire.h>
#include <rtc_clock.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define BAUD 9600
#define ON LOW
#define OFF HIGH
#define RELAY 4


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

RTC_clock rtc_clock(XTAL);
char* daynames[]={"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

// keypad initialization
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {12, 13, 10, 9}; //connect to the column pinouts of the keypad pin 11 switched to 13 bc of 11 set to play audio
// initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// keypad initialization

char h1, h2, m1, m2, key;
int hn1, hn2, mn1, mn2, hStart, mStart, hStop, mStop, min2GO;
int a=0;
int i=0;
int currentHour=0;
int currentMinute=0;

void setup()   {                
  Serial.begin(BAUD);
  rtc_clock.init();
  rtc_clock.set_time(10, 29, 9);
  rtc_clock.set_date(22, 10, 2012);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  
  display.display();
  delay(2000);
  display.clearDisplay();

  
  

 


}


void loop() {
  // time related
  Serial.print(rtc_clock.get_hours());
  Serial.print(":");
  Serial.print(rtc_clock.get_minutes());
  Serial.print(":");
  Serial.println(rtc_clock.get_seconds());
  Serial.print(" ");
  Serial.print(daynames[rtc_clock.get_day_of_week()-1]);
  Serial.print(": ");
  Serial.print(rtc_clock.get_days());
  Serial.print(".");
  Serial.print(rtc_clock.get_months());
  Serial.print(".");
  Serial.println(rtc_clock.get_years());

  
 key = keypad.waitForKey(); // reading keypad
    if(key)  // Check for a valid key.
    {
      switch (key)
      {
        case 'A': // adjusting time if time is incorrect in time module
          Serial.print("A");
          h1=keypad.waitForKey(); Serial.print(h1); hn1=char2number(h1);
          h2=keypad.waitForKey(); Serial.print(h2); hn2=char2number(h2);
          m1=keypad.waitForKey(); Serial.print(m1); mn1=char2number(m1);
          m2=keypad.waitForKey(); Serial.print(m2); mn2=char2number(m2);
          //setTime(combiner(hn1,hn2), combiner(mn1,mn2)); need to change it 
          Serial.println();
          Serial.print(hn1);
          Serial.print(hn2);
          Serial.print(mn1);
          Serial.print(mn2);
          Serial.println();
          Serial.print(combiner(hn1,hn2));
          Serial.println();
          Serial.print(combiner(mn1,mn2));
          Serial.println();
          break;
        case 'B': // setting time till sounding alarm
          Serial.print("B");
          h1=keypad.waitForKey(); Serial.print(h1); hn1=char2number(h1);
          h2=keypad.waitForKey(); Serial.print(h2); hn2=char2number(h2);
          m1=keypad.waitForKey(); Serial.print(m1); mn1=char2number(m1);
          m2=keypad.waitForKey(); Serial.print(m2); mn2=char2number(m2);
          hStart=combiner(hn1, hn2);
          mStart=combiner(mn1, mn2);
          Serial.println();
          Serial.print("Hours Till Alarm: ");
          Serial.print(hStart);
          Serial.print("Minutes Till Alarm: ");
          Serial.print(mStart);
          Serial.println();
          
          break;
        case 'C': // setting time till automatic pump shut off
          Serial.print("C");
          h1=keypad.waitForKey(); Serial.print(h1); hn1=char2number(h1);
          h2=keypad.waitForKey(); Serial.print(h2); hn2=char2number(h2);
          m1=keypad.waitForKey(); Serial.print(m1); mn1=char2number(m1);
          m2=keypad.waitForKey(); Serial.print(m2); mn2=char2number(mn2);
          hStop=combiner(hn1, hn2);
          mStop=combiner(mn1, mn2);
          Serial.println();
          Serial.print("Hours Till Shutdown: ");
          Serial.print(hStart);
          Serial.print("Minutes Till ShutDown: ");
          Serial.print(mStart);
          Serial.println();
          break;
          
        case 'D': // reset system turn on water pump 
          Serial.print("System Reset");
          h1='D';
          while (h1=='D'){
           h1=keypad.getKey();
           Serial.print(h1);
          }
          Serial.print("&");
          break;
        
        case '*':
        //startPlayback(R2D2, sizeof(R2D2));
        break;
        case '#':
        //startPlayback(POPSOUND, sizeof(POPSOUND));
        break;
           
          
        default:
         break;
      }

      Serial.println(key);
    }


  
  
}



int minutes2go(int startHour, int startMin, int stopHour, int stopMin){
  int v1=0;
  int v2=0;
  int v3=0;
  v1=startHour*60+startMin;
  v2=stopHour*60+stopMin;
  return v3=v2-v1;
}

int char2number(char xyz){
  int a1=0;
  switch (xyz)
      {
        case '1': a1=1; break;
        case '2': a1=2; break;
        case '3': a1=3; break;
        case '4': a1=4; break;
        case '5': a1=5; break;
        case '6': a1=6; break;
        case '7': a1=7; break;
        case '8': a1=8; break;
        case '9': a1=9; break;
        case '0': a1=0; break;
        default: a1=0;  break;
      }
      return a1;
}

int combiner(int x, int y){
  int comb=0;
  return comb=x*10+y;
}





void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  display.display();
}






void testdrawroundrect(void) {
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, WHITE);
    display.display();
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}
   
void testdrawrect(void) {
  for (int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
  }
}

void testdrawline() {  
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
  }
  delay(250);
}


