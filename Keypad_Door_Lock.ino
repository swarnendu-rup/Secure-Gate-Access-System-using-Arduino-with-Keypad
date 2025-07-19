/*
🔧 Hardware Wiring:

🖥 LCD (I2C):
-------------
- VCC  -> 5V
- GND  -> GND
- SDA  -> A4 (on Uno) or GPIO21 (on ESP32)
- SCL  -> A5 (on Uno) or GPIO22 (on ESP32)

⌨️ Keypad (4x4):
----------------
- Row Pins (R1–R4): 2, 3, 4, 5
- Column Pins (C1–C4): 6, 7, 10, 11

⚙️ Servo Motor:
---------------
- Signal -> Pin 12
- VCC -> External 5V (⚠️ not from Arduino 5V pin if high torque)
- GND -> Common ground with Arduino

🔴🔵 LEDs:
----------
- Green LED -> Pin 8 (access granted)
- Red LED   -> Pin 9 (access denied)
- Buzzer    -> Pin 9 (access denied)

📌 Notes:
- Use external power if servo causes brownout.
- Pull-up resistors on keypad optional but improve signal reliability.
*/


#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Define LCD with I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define servo motor
Servo gateServo;

// Define LEDs
const int greenLED = 8;
const int redLED = 9;

// Define keypad
const byte ROWS = 4;  // Four rows
const byte COLS = 4;  // Four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};  // Connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 10, 11}; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define password
String password = "78208";
String enteredPassword = "";
bool showPassword = false;

void setup() {
  lcd.begin();   // Initialize LCD
  lcd.backlight();    // Turn on the backlight
  gateServo.attach(12);  // Servo connected to pin 12
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
  lcd.print("Click # for");
  lcd.setCursor(0, 12);
  lcd.print("Instructions");
  gateServo.write(0);  // Initial gate position (closed)
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    if (key == '#') {
      displayInstructions();
    } else if (key == '*') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Click # for");
      lcd.setCursor(0, 12);
      lcd.print("Instructions");
    } else if (key == 'A') {
      showPassword = !showPassword;
      displayPassword();
    } else if (key == 'B') {
      if (enteredPassword.length() > 0) {
        enteredPassword.remove(enteredPassword.length() - 1);
        displayPassword();
      }
    } else if (key == 'C') {
      enteredPassword = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
    } else if (key == 'D') {
      checkPassword();
    } else {
      enteredPassword += key;
      displayPassword();
    }
  }
}

void displayPassword() {
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
  if (showPassword) {
    lcd.print(enteredPassword);  // Show the password in numbers
  } else {
    for (int i = 0; i < enteredPassword.length(); i++) {
      lcd.print('*');  // Display the password as stars
    }
  }
}

void checkPassword() {
  if (enteredPassword == password) {
    correctPassword();
  } else {
    incorrectPassword();
  }
  enteredPassword = "";  // Reset password after checking
}

void correctPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Correct Password");
  lcd.setCursor(0, 1);
  lcd.print("Opening the gate");
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);

  gateServo.write(90);  // Open the gate
  delay(5000);          // Keep the gate open for 5 seconds
  gateServo.write(0);   // Close the gate
  digitalWrite(greenLED, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}

void incorrectPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Incorrect");
  lcd.setCursor(0, 1);
  lcd.print("   Password");
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);

  delay(3000);  // Delay to show message
  digitalWrite(redLED, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}

void displayInstructions() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A: Show/Hide");
  lcd.setCursor(0, 1);
  lcd.print("B: Backspace");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C: Clear");
  lcd.setCursor(0, 1);
  lcd.print("D: Enter");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}