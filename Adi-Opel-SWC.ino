/*
 Adapt steering  wheel audio controls from Toyota Corolla to
 Pioneer hud.
 Tested on Corolla G10 with Pioneer MVH-285BT (3.3V @ 10kohms)
  
 Author: Gatul -  November 2020
 
*/

#include<avr/wdt.h> /* Header for watchdog timers in AVR */

//#define DEBUGLOG

#ifdef DEBUGLOG
# define LLOG(...) Serial.print(__VA_ARGS__)
# define LLOGln(...) Serial.println(__VA_ARGS__)
#else
# define LLOG(...)
# define LLOGln(...)
#endif

const int NO_BTN      = 0;
const int MUTE_BTN    = 2;    // pin2 --- 3K5 -- connect to tip
const int SOURCE_BTN  = 3;    // pin3 --- 1K2 -- connect to tip 
const int NEXT_BTN    = 4;    // pin4 --- 8K2 -- connect to tip
const int PREV_BTN    = 5;    // pin5 --- 11K25 -- connect to tip
const int VOLUP_BTN   = 6;    // pin6 --- 16K -- connect to tip
const int VOLDWN_BTN  = 7;    // pin7 --- 24K -- connect to tip

const int PIN_SWCTRL = A0;    // [o] [^] [>] [<] [-] [+] input

const int PIN_LED = LED_BUILTIN;

const unsigned long DEBOUNCER_TIME = 50;

//*****************************************************************************
// Global variables

int oldButton = NO_BTN;
int newButton = oldButton;
int lastButton = NO_BTN;
bool btnPressed = false;

//*****************************************************************************

void setup() {

  pinMode(PIN_LED, OUTPUT);

  pinMode(MUTE_BTN, INPUT);
  digitalWrite(MUTE_BTN, LOW);
  pinMode(SOURCE_BTN, INPUT);
  digitalWrite(SOURCE_BTN, LOW);
  pinMode(NEXT_BTN, INPUT);
  digitalWrite(NEXT_BTN, LOW);
  pinMode(PREV_BTN, INPUT);
  digitalWrite(PREV_BTN, LOW);
  pinMode(VOLUP_BTN, INPUT);
  digitalWrite(VOLUP_BTN, LOW);
  pinMode(VOLDWN_BTN, INPUT);
  digitalWrite(VOLDWN_BTN, LOW);

  //DDRD &= B00000011; // D2 a D7 INPUT
  //PORTD &= B00000111; // D3 a D7 LOW
  //
  //DDRC |= B00111110; // A1 a A5 OUTPUT
  //PORTC &= B11000001; // A1 a A5 LOW

  pinMode(A0, INPUT);

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
  // Values are got from measurement
  if(adcValue >= 100 && adcValue < 210) return VOLDWN_BTN;    // 155
	if(adcValue >= 210 && adcValue < 322) return VOLUP_BTN;     // 264
	if(adcValue >= 322 && adcValue < 438) return MUTE_BTN;      // 379
	if(adcValue >= 438 && adcValue < 558) return NEXT_BTN;      // 497
  if(adcValue >= 558 && adcValue < 681) return PREV_BTN;      // 619
	if(adcValue >= 681 && adcValue < 800) return SOURCE_BTN;    // 742
  
  return NO_BTN;
}

//*****************************************************************************

int get_button() {
  
  
  int analogButton = decode_analog_btn(analogRead(PIN_SWCTRL));
  
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
