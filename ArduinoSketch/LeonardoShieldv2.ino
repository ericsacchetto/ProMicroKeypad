#include <Keypad.h>
#include "HID-Project.h"


// Rotary Encoder Inputs
#define Clock A1
#define Data A2
#define Push A0
int counter = 0;
int currentStateClock;
int lastStateClock;
String currentDir ="";
unsigned long lastButtonPress = 0;

#define NUM_LED_COLS (3)            // Number of LED columns (+, anode)
#define NUM_LED_ROWS (3)            // Number of LED rows (-, cathode)

const byte ROWS = 3;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  };

byte rowPins[ROWS] = {2, 3, 4}; 
byte colPins[COLS] = {5, 6, 7}; 
static const uint8_t ledRowPins[NUM_LED_ROWS] = {8, 9, 10};     // Pins connected to LED rows (-)
static const uint8_t ledColPins[NUM_LED_COLS] = {16, 14, 15};   // Pins connected to LED cols (+)
static bool LED_buffer[NUM_LED_COLS][NUM_LED_ROWS];             // Keeps track of LED states

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
  Consumer.begin();
  Keyboard.begin();

  setupLEDPins();

  pinMode(Clock,INPUT_PULLUP);
  pinMode(Data,INPUT_PULLUP);
  pinMode(Push, INPUT_PULLUP);
  lastStateClock = digitalRead(Clock);

  for (uint8_t i = 0; i < NUM_LED_COLS; i++)
  {
    for (uint8_t j = 0; j < NUM_LED_ROWS; j++)
    {
      LED_buffer[i][j] = 0;
    }
  }
}

static void setupLEDPins()
{
  uint8_t i;
  // LED drive rows - written LOW when active, HIGH otherwise
  for (i = 0; i < NUM_LED_ROWS; i++)
  {
    pinMode(ledRowPins[i], OUTPUT);
    digitalWrite(ledRowPins[i], HIGH);
  }

  // LED select columns - Write HIGH to turn an LED on.
  for (i = 0; i < NUM_LED_COLS; i++)
  {
    pinMode(ledColPins[i], OUTPUT);
    digitalWrite(ledColPins[i], LOW);
  }
}

void LightLED(int c, int r)
{
  digitalWrite(ledColPins[c], HIGH);
  digitalWrite(ledRowPins[r], LOW);
  delay(500);
}

void OpenApp(String appAddress)
{
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_ESC);
  delay(100);
  Keyboard.releaseAll();
  Keyboard.print(appAddress);
  delay(100);
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();
}
  
void loop(){
  char customKey = customKeypad.getKey();
  currentStateClock = digitalRead(Clock);

  if (currentStateClock != lastStateClock  && currentStateClock == 1)
  {
    if (digitalRead(Data) != currentStateClock)
    {
      counter --;
      Consumer.write(MEDIA_VOLUME_DOWN);
    } 
    else 
    {
      counter ++;
      Consumer.write(MEDIA_VOLUME_UP);
    }
  }
  lastStateClock = currentStateClock;

  int btnState = digitalRead(Push);

  if (btnState == LOW) 
  {
    if (millis() - lastButtonPress > 50) {
      Consumer.write(MEDIA_VOLUME_MUTE);
    }
    lastButtonPress = millis();
  }

    
  switch (customKey) 
  {
  case '1':  
    Consumer.write(MEDIA_PREVIOUS);
    LightLED(0,0);
    break;
  case '2':
    Consumer.write(MEDIA_PLAY_PAUSE);
    LightLED(1,0);
    break;
  case '3':
    Consumer.write(MEDIA_NEXT);
    LightLED(2,0);
    break;
  case '4':
    Keyboard.write(KEY_PRINTSCREEN);
    LightLED(0,1);
    break;
  case '5':
    Keyboard.press(KEY_LEFT_WINDOWS);
    Keyboard.press(KEY_L);
    Keyboard.releaseAll();
    LightLED(1,1);
    break;
  case '6':
    Keyboard.write(KEY_MENU);
    LightLED(2,1);
    break;
  case '7':
    OpenApp("Your App 1");  //replace with your favorite app
    LightLED(0,2);
    break;
  case '8':
    OpenApp("Your App 2");  //replace with your second favorite app
    LightLED(1,2);
    break;
  case '9':
    OpenApp("spotify");
    LightLED(2,2);
    break;
  }
  setupLEDPins();
}
