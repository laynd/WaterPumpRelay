/* Water pump regulator project based on Arduino DUE board
 * 
 * Idea is to have a controller with monochrome oled display like: http://www.ebay.com/itm/262202656207?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT
 * with keypad to put in time and adjust duration of pump-running-until-alarm/shutoff  http://www.ebay.com/itm/171124384722?_trksid=p2060353.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT
 * and real time clock module to keep time (because arduno does not have true time clock built in) http://www.ebay.com/itm/181931877954?_trksid=p2060353.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT
 * and audio mini amplifier and sd card reader to store audio file for alarm, keypress sound and etc http://www.ebay.com/itm/181970116642?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT
 * http://www.ebay.com/itm/401063581247?_trksid=p2057872.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT
 * 
 * So technically that's how I see it will be working. There is a relay on pump control that turns on based on water pressure sensor. We will connect 120V relay to pump relay
 * and our 120V relay will be closing loop on arduino board lets say connected from 3.3v to digital pin 5. So when 120v relay is off pin 5 will be 0V when relay is on then 
 * pin 5 will have 3.3V. This way we will know when pump is working. Then just need a trigger variable that will mark the start when pump is activated and store time stamp 
 * then measure how much time passed. If lets say an hour passed and pump is still working then start sounding an alarm. If it goes beyond lets say hour and a half then 
 * there will be arduino controlled relay controlled by digital pin 4 that will cut off power line to pump.  
 * 
 * To reset cut off will need to install pushbutton and program on of keypad buttons. 
 */


#include <SPI.h>
#include <Wire.h>
#include <rtc_clock.h> //clock module lib
#include <Adafruit_GFX.h> // monochrome display related
#include <Adafruit_SSD1306.h> //OLED lib
#include <Keypad.h> //keypad related

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define BAUD 9600  //serial port frequency for monitoring what's happening via arduino soft port serial monitor (tools/serialmonitor)
#define ON LOW     //these are to control relay on/off
#define OFF HIGH
#define RELAY 4    // relay connected on digital pin 4

// OLED screen related
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

// clock module related
RTC_clock rtc_clock(XTAL);
char* daynames[]={"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

// keypad set up
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


//some variables 
char h1, h2, m1, m2, key;
int hn1, hn2, mn1, mn2, hStart, mStart, hStop, mStop, min2GO;
int a=0;
int i=0;
int currentHour=0;
int currentMinute=0;

void setup()   { 
                 
  Serial.begin(BAUD); //initializing serial frequency

  // clock initialization  setting time/date will be set later via keypad
  rtc_clock.init();
  rtc_clock.set_time(10, 29, 0);
  rtc_clock.set_date(2, 2, 2016);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  //clearing display and setting in blank
  display.clearDisplay();
  display.display();
  delay(2000);
  

  
  

 


}


void loop() {
  // just printing time (will remove it later)
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
          rtc_clock.set_time(combiner(hn1,hn2), combiner(mn1,mn2), 0); //combiner() is function of my design just fusing two char into number. might need to look for more elegant way of doing it  
          Serial.println();
          Serial.print(hn1); //printing these into serial just to check what is going on
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
        //nothing there yet
        break;
        case '#':
        //nothing there yet
        break;
           
          
        default:
         break;
      }

      Serial.println(key); //just showing whatbutton was pressed
    }


  
  
}

// here are all kinds of functions some are mine some are leftovers from other pieces of code

int minutes2go(int startHour, int startMin, int stopHour, int stopMin){
  int v1=0;
  int v2=0;
  int v3=0;
  v1=startHour*60+startMin;
  v2=stopHour*60+stopMin;
  return v3=v2-v1;
}

int char2number(char xyz){  //simple way of converting char variable into number 
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

int combiner(int x, int y){ //combining knowing that numbers do not go beyond double digits 
  int comb=0;
  return comb=x*10+y;
}

//below are graphic functions that might be useful for something or might not 

/*

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
*/

