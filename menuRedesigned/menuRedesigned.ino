
#include "U8glib.h"
#include <Keypad.h>

U8GLIB_SSD1306_128X64 u8g(4, 5, 6, 7); // SW SPI Com: SCK = 4, MOSI = 5, CS = 6, A0 = 7
#define KEY_NONE 0
#define KEY_PREV 1
#define KEY_NEXT 2
#define KEY_SELECT 3
#define KEY_BACK 4

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
byte rowPins[ROWS] = {36, 34, 32, 30}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {28, 26, 24, 22}; //connect to the column pinouts of the keypad 
// initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// keypad initialization
char h1, h2, m1, m2, key;

//char uiKeyPrev = "A";
//char uiKeyNext = "B";
//char uiKeySelect = "C";
//char uiKeyBack = "D"

uint8_t uiKeyCodeFirst = KEY_NONE;
uint8_t uiKeyCodeSecond = KEY_NONE;
uint8_t uiKeyCode = KEY_NONE;


void uiSetup(void) {
  // configure input keys 
  
  
}

void uiStep(void) {
  uiKeyCodeSecond = uiKeyCodeFirst;
  
  if(key){
    switch (key){
      case 'A':
      uiKeyCodeFirst = KEY_PREV;

      break;
      case 'B':
      uiKeyCodeFirst = KEY_NEXT;

      break;
      case 'C':
      uiKeyCodeFirst = KEY_SELECT;

      break;
      case 'D':
      uiKeyCodeFirst = KEY_BACK;

      break;
      default:
      uiKeyCodeFirst = KEY_NONE;
      break;
    }
  }
  
  
  if ( uiKeyCodeSecond == uiKeyCodeFirst )
    uiKeyCode = uiKeyCodeFirst;
  else
    uiKeyCode = KEY_NONE;
}


#define MENU_ITEMS 4
char *menu_strings[MENU_ITEMS] = { "Set Time", "Set Till Alarm", "Set Till ShutOff", "Reset System" };

uint8_t menu_current = 0;
uint8_t menu_redraw_required = 0;
uint8_t last_key_code = KEY_NONE;


void drawMenu(void) {
  uint8_t i, h;
  u8g_uint_t w, d;
  //u8g.u8g_SetFontPosBaseline(10); //will it work?... 
  u8g.setFont(u8g_font_6x13);
  u8g.setFontRefHeightText();
  u8g.setFontPosCenter();
  
  h = u8g.getFontAscent()-u8g.getFontDescent();
  w = u8g.getWidth();
  for( i = 0; i < MENU_ITEMS; i++ ) {
    d = (w-u8g.getStrWidth(menu_strings[i]))/2;
    u8g.setDefaultForegroundColor();
    if ( i == menu_current ) {
      u8g.drawBox(0, i*h+1, w, h);
      u8g.setDefaultBackgroundColor();
    }
    u8g.drawStr(d, i*h, menu_strings[i]);
  }
}

void updateMenu(void) {
  if ( uiKeyCode != KEY_NONE && last_key_code == uiKeyCode ) {
    return;
  }
  last_key_code = uiKeyCode;
  
  switch ( uiKeyCode ) {
    case KEY_NEXT:
      menu_current++;
      if ( menu_current >= MENU_ITEMS )
        menu_current = 0;
      menu_redraw_required = 1;
      break;
    case KEY_PREV:
      if ( menu_current == 0 )
        menu_current = MENU_ITEMS;
      menu_current--;
      menu_redraw_required = 1;
      break;
  }
}


void setup() {
 
  //u8g.setVirtualScreenDimension(128, 64);
  uiSetup();                                // setup key detection and debounce algorithm
  menu_redraw_required = 1;     // force initial redraw
}

void loop() {  

  key=keypad.getKey();
  uiStep();                                     // check for key press
    
  if (  menu_redraw_required != 0 ) {
    u8g.firstPage();
    do  {
      drawMenu();
    } while( u8g.nextPage() );
    menu_redraw_required = 0;
  }

  updateMenu();                            // update menu bar
  
}
