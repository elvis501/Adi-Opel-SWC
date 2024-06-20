/*
 Adapt steering  wheel audio controls from Toyota Corolla to
 Pioneer hud.
 Tested on Corolla G10 with Pioneer MVH-285BT (3.3V @ 10kohms)
  
 Author: Gatul -  November 2020
 
*/

#include<avr/wdt.h> /* Header for watchdog timers in AVR */

#define DEBUGLOG

#ifdef DEBUGLOG
# define LLOG(...) Serial.print(__VA_ARGS__)
# define LLOGln(...) Serial.println(__VA_ARGS__)
#else
# define LLOG(...)
# define LLOGln(...)
#endif

const int NO_BTN      = 0;
const int MUTE_BTN    = 2;    // 3.5 Ohm
const int SOURCE_BTN  = 3;    // 1K2 
const int NEXT_BTN    = 4;    // 8K2
const int PREV_BTN    = 5;    // 11K25
const int VOLUP_BTN   = 6;    // 16K
const int VOLDWN_BTN  = 7;    // 24K

const int PIN_SWCTRL = A0;    // [-][+][o][>][<][^] input

const int PIN_LED = LED_BUILTIN;

const unsigned long DEBOUNCER_TIME = 50;

//*****************************************************************************
// Variables

int oldButton = NO_BTN;
int newButton = oldButton;
int lastButton = NO_BTN;
bool btnPressed = false;

//*****************************************************************************

void setup() {

  pinMode(PIN_LED, OUTPUT);

  pinMode(MUTE_BTN, INPUT);
  pinMode(SOURCE_BTN, INPUT);
  pinMode(NEXT_BTN, INPUT);
  pinMode(PREV_BTN, INPUT);
  pinMode(VOLUP_BTN, INPUT);
  pinMode(VOLDWN_BTN, INPUT);

  //DDRD &= B00000011; // D2 a D7 INPUT
  //PORTD &= B00000111; // D3 a D7 LOW
// para reducir ruido ADC...
  //DDRC |= B00111110; // A1 a A5 OUTPUT
  //PORTC &= B11000001; // A1 a A5 LOW

  pinMode(A0, INPUT/*_PULLUP*/); // or INPUT

  pinMode(A1, OUTPUT);
  digitalWrite(A1, LOW);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, LOW);
  pinMode(A3, OUTPUT);
  digitalWrite(A3, LOW);
  pinMode(A4, OUTPUT);
  digitalWrite(A4, LOW);
  pinMode(A6, OUTPUT);
  digitalWrite(A6, LOW);
  pinMode(A7, OUTPUT);
  digitalWrite(A7, LOW);

#ifdef DEBUGLOG
    Serial.begin(115200);
#endif
  wdt_disable();  /* Disable the watchdog and wait for more than 2 seconds */
  delay(3000);
  // Just to know which program is running on my Arduino
  LLOGln(F("START " __FILE__ " from " __DATE__));
  
  wdt_enable(WDTO_2S);  /* Enable the watchdog with a timeout of 2 seconds */
}

//*****************************************************************************

void loop() {

  newButton = get_button();
  
  if(newButton != oldButton) {
    send_remote_command(newButton, oldButton);
    oldButton = newButton;
    LLOGln( oldButton );
  }

  wdt_reset();  // Reset the watchdog
  delay(10);    // 10ms
}

//*****************************************************************************

int decode_analog_btn(int adcValue){
  //LLOGln(adcValue);

  if(adcValue <= 130) return NEXT_BTN;
	if(adcValue >= 145 && adcValue <= 225) return PREV_BTN;
	if(adcValue >= 285 && adcValue <= 365) return VOLUP_BTN;
	if(adcValue >= 525 && adcValue <= 605) return VOLDWN_BTN;  
  return NO_BTN;
}

//*****************************************************************************

int get_button() {
  
  
  int analogButton = decode_analog_btn(analogRead(PIN_SWCTRL));
  //int digitalButton = digitalRead(PIN_MODEBTN);

  
  //digitalWrite(PIN_LED, analogButton * 32 );
  
  if( analogButton == NO_BTN ) {
  
    btnPressed = false;
    lastButton = NO_BTN;
  
  } else {
    if( !btnPressed)  {
      btnPressed = true;

      delay(DEBOUNCER_TIME);
      
      if( decode_analog_btn(analogRead(PIN_SWCTRL)) == analogButton ) {
          lastButton = analogButton;
      }

    } 
  }
  
  //digitalWrite(PIN_LED, analogButton * 32 );
  
  return lastButton;
}

//*****************************************************************************

void send_remote_command(int newBtn, int oldBtn) {

  byte ledStatus = HIGH;
  if(newBtn == NO_BTN) {
    pinMode(oldBtn, INPUT);
    ledStatus = LOW;
  }
  else pinMode(newBtn, OUTPUT);
  
  digitalWrite(PIN_LED, ledStatus);
}

//*****************************************************************************
