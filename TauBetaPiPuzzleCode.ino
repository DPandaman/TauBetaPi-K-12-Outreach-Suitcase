#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup (I2C address might vary, often 0x27 or 0x3F. Adjust if necessary)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;
#define BUZZER 10

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
bool state = false;

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

String code = "000000000000"; // Start with "0000" as the initial code
String correct = "123412341234";

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Welcome message
  lcd.setCursor(0, 0);
  lcd.print("Enter Code:");
  lcd.setCursor(0, 1);
  lcd.print("------------");
}

void loop() {
  char customKey = customKeypad.getKey();

  if (customKey) {
    Serial.println(customKey);

    tone(BUZZER, 50);
    delay(100);
    noTone(BUZZER);

    if (customKey == '*') {
      // Reset everything
      code = "000000000000"; // Reset code to "0000"
      digitalWrite(LED_BUILTIN, LOW);
      state = false;

      // Update LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Code:");
      lcd.setCursor(0, 1);
      lcd.print("------------");
    } else if (isdigit(customKey)) { // Check if the key is a digit
      // Rotate code to keep only the last 4 digits
      code = code.substring(1) + customKey;

      // Display updated code on LCD
      lcd.setCursor(0, 1);
      lcd.print(code);

      // Check if the last 4 digits match the correct code
      if (code.equals(correct)) {
        if (!state) {
          digitalWrite(LED_BUILTIN, HIGH);
          state = true;

          // Success message
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Access Granted");
        } else {
          digitalWrite(LED_BUILTIN, LOW);
          state = false;

          // Locked message
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Access Locked");
        }

        delay(2000); // Hold message for 2 seconds
        code = "------------"; // Reset the code to "0000" after matching

        // Reset LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter Code:");
        lcd.setCursor(0, 1);
        lcd.print("------------");
      }
    }
  }
}