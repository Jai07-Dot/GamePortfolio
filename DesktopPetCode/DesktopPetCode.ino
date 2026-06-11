Jaira Settles 
#include <LiquidCrystal.h>

// 1. LCD Screen Neighborhood (RS=7, E=8, D4=9, D5=10, D6=11, D7=12)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// 2. RGB LED Neighborhood Pins
const int redPin = 6;
const int greenPin = 5;
const int bluePin = 3;

// 3. YOUR NEW PIN ASSIGNMENTS 
const int joystickX = A3;  // VRx moved here!
const int joystickY = A4;  // VRy connected here
const int joystickSW = 2;  // SW Button connected here
const int tempPin = A2;     // Thermistor stays safe on A2

// Custom Pixel Art Matrices
byte normalEye[8] = { B00000, B00000, B01100, B01100, B00000, B00000, B00000, B00000 };
byte madEye[8]    = { B11100, B01100, B00110, B00110, B00000, B00000, B00000, B00000 };
byte cuteEye[8]   = { B00000, B00100, B01010, B10001, B00000, B00000, B00000, B00000 };
byte flatMouth[8] = { B00000, B00000, B00000, B00000, B11111, B00000, B00000, B00000 };
byte sadMouth[8]  = { B00000, B00000, B01110, B10001, B00000, B00000, B00000, B00000 };
byte cuteMouth[8] = { B00000, B00000, B00000, B10001, B01010, B00100, B00000, B00000 };

void setup() {
  lcd.begin(16, 2);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // INPUT_PULLUP turns on an internal resistor for the SW button so it reads cleanly!
  pinMode(joystickSW, INPUT_PULLUP); 

  lcd.createChar(0, normalEye);
  lcd.createChar(1, madEye);
  lcd.createChar(2, cuteEye);
  lcd.createChar(3, flatMouth);
  lcd.createChar(4, sadMouth);
  lcd.createChar(5, cuteMouth);
}

void loop() {
  // Read Thermistor
  int rawTemp = analogRead(tempPin);
  float resistance = (1023.0 / (float)rawTemp) - 1.0;
  resistance = 10000.0 / resistance; 
  float temperature = resistance / 10000.0;
  temperature = log(temperature);
  temperature /= 3950.0; 
  temperature += 1.0 / (25.0 + 273.15);
  temperature = 1.0 / temperature;
  temperature -= 273.15; 
  float fahrenheit = (temperature * 9.0 / 5.0) + 32.0;

  // Read Joystick Pins
  int xVal = analogRead(joystickX);
  int buttonState = digitalRead(joystickSW); // Reads LOW when pressed down

  lcd.clear();

  // State Machine logic using the new A3 readings
  if (xVal < 200) {
    digitalWrite(redPin, HIGH); digitalWrite(greenPin, LOW); digitalWrite(bluePin, LOW);
    lcd.setCursor(4, 0); lcd.write(byte(1)); lcd.setCursor(6, 0); lcd.write(byte(3)); lcd.setCursor(8, 0); lcd.write(byte(1));
    lcd.setCursor(11, 0); lcd.print("MAD!");
  } 
  else if (xVal >= 200 && xVal < 450) {
    digitalWrite(redPin, LOW); digitalWrite(greenPin, LOW); digitalWrite(bluePin, HIGH);
    lcd.setCursor(4, 0); lcd.write(byte(0)); lcd.setCursor(6, 0); lcd.write(byte(4)); lcd.setCursor(8, 0); lcd.write(byte(0));
    lcd.setCursor(11, 0); lcd.print("SAD ");
  }
  else if (xVal > 800) {
    digitalWrite(redPin, HIGH); digitalWrite(greenPin, LOW); digitalWrite(bluePin, HIGH);
    lcd.setCursor(4, 0); lcd.write(byte(2)); lcd.setCursor(6, 0); lcd.write(byte(5)); lcd.setCursor(8, 0); lcd.write(byte(2));
    lcd.setCursor(11, 0); lcd.print("CUTE");
  } 
  else {
    digitalWrite(redPin, HIGH); digitalWrite(greenPin, HIGH); digitalWrite(bluePin, LOW);
    lcd.setCursor(4, 0); lcd.write(byte(0)); lcd.setCursor(6, 0); lcd.write(byte(3)); lcd.setCursor(8, 0); lcd.write(byte(0));
    lcd.setCursor(11, 0); lcd.print("GOOD");
  }

  // Print temperature on line 2
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(fahrenheit, 1);
  lcd.print(" F");

  delay(150);
}