#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Parts adjustment
const int I2C_ADDR = 0x27;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
const int BUTTON = 7;
const int BUZZER = 8;

// Variables
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);
String morse_text = "";
String display_text = "";
char last_char;
int buttonState;
int lastButtonState;
unsigned long startTime;
unsigned long lastSignalTime;
unsigned long duration;
unsigned long work_duration;
const unsigned long dotDuration = 350;

// Function declarations
void decode();
void printtolcd();

void setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello");
  Serial.begin(9600);
}

void loop()
{
  buttonState = !digitalRead(BUTTON);
  if (buttonState != lastButtonState)
  {
    if (buttonState == HIGH)
    {
      Serial.print("Pressed\n");
      startTime = millis();
      tone(BUZZER, 1500);
    }
    else
    {
      duration = millis() - startTime;
      morse_text += (duration < dotDuration) ? "." : "-";
      Serial.print("Current morse: " + morse_text + "\n");
      lastSignalTime = millis();
      noTone(BUZZER);
      delay(20);
    }
  }

  if (morse_text != "")
  {

    if (dotDuration > (millis() - lastSignalTime))
    {
      // Space bewteen signals
    }
    else if ((3 * dotDuration) > (millis() - lastSignalTime) && buttonState != HIGH)
    {
      Serial.print("Decoding\n");
      last_char = 'f';
      decode();
    }
  }
  else
  {
    if ((millis() - lastSignalTime) > (7 * dotDuration))
    {
      if (last_char == ' ')
      {
      }
      else
      {
        if (display_text.length() != 0)
        {
          display_text += " ";
          last_char = ' ';
        }
      }
    }
  }

  lastButtonState = buttonState;
}

void decode()
{
  static String morse[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....",
                           "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
                           ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "!"};
  int i = 0;
  while (morse[i] != "!")
  {
    if (morse[i] == morse_text)
    {
      char some = char('A' + i);
      display_text += some;
      printtolcd();
      Serial.print("Morse text: " + morse_text);
      Serial.print("\n");
      morse_text = "";
      Serial.print(char('A' + i));
      Serial.print("\n");
      Serial.print("Display text: " + display_text);
      break;
    }
    i++;
  }
  if (morse[i] == "!")
  {
    Serial.println("");
    Serial.println("This code does not exist!");
    morse_text = "";
  }
}

void printtolcd()
{
  lcd.clear();
  if (display_text.length() > 32)
  {
    display_text = display_text.substring(32);
  }

  if (display_text.length() > 16)
  {
    lcd.setCursor(0, 0);
    lcd.print(display_text.substring(0, 16));

    lcd.setCursor(0, 1);
    lcd.print(display_text.substring(16));
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print(display_text);
  }
}
