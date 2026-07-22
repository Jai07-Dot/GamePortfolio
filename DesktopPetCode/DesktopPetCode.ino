#include <LiquidCrystal.h>

// ============================================================================
// 0. AUTHOR METADATA
// ============================================================================
const char* AUTHOR = "Jaira Settles";
const char* VERSION = "1.0.0";
void initSystem() {
  Serial.begin(9600);
  Serial.print("[");
  Serial.print(AUTHOR);
  Serial.print(" Core Engine v");
  Serial.print(VERSION);
  Serial.println("] Starting...");
}

// ============================================================================
// 1. HARDWARE PIN CONFIGURATION
// ============================================================================
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
const int redPin = 6;
const int greenPin = 5;
const int bluePin = 3;

const int joystickX = A3;   // VRx
const int joystickY = A4;   // VRy
const int joystickSW = 2;   // SW Button
const int tempPin = A2;      // Thermistor
const int lightPin = A1;     // Photocell / Photoresistor

// ============================================================================
// 2. CUSTOM FONT PIXEL MATRICES
// ============================================================================
byte sadMouth[8]  = { B00000, B00000, B01110, B10001, B00000, B00000, B00000, B00000 };
byte uMouth[8]    = { B00000, B00000, B00000, B10001, B10001, B01010, B00100, B00000 }; // Shared "U" and "0" profile
byte openMouth[8] = { B00000, B01110, B11111, B11111, B11111, B01110, B00000, B00000 }; // Feeding open chomp
byte blinkEye[8]  = { B00000, B00000, B00000, B00000, B11111, B00000, B00000, B00000 }; // Standard closed blink line

// Downward Slanted Angry Eyebrow + Eye combos
byte madLeft[8]   = { B11000, B01100, B00110, B00000, B11111, B00000, B00000, B00000 }; 
byte madRight[8]  = { B00011, B00110, B01100, B00000, B11111, B00000, B00000, B00000 }; 

// ============================================================================
// 3. CORE SURVIVAL ENGINE & MEMORY VARIABLES
// ============================================================================
int loveMeter = 50;         
int foodMeter = 50;         
int currentEmotion = 0;      // 0 = HAPPY, 1 = MAD, 2 = SAD, 3 = CUTE

int petCounter = 0;         
int feedCounter = 0;        

bool lastButtonState = HIGH;
bool isolatedAction = false; 
bool manualOverrideActive = false; 
bool wasAsleep = false;      // Tracks light-to-dark transition state changes

// ============================================================================
// 4. ASYNCHRONOUS NON-BLOCKING CLOCKS (SYSTEM TIMERS)
// ============================================================================
unsigned long actionStartTime = 0;
const unsigned long actionTimeout = 1500; 

unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 4000;
const unsigned long blinkDuration = 150;

unsigned long lastDecayTime = 0;
const unsigned long decayInterval = 12000; 

unsigned long lastChompTime = 0;
const unsigned long chompInterval = 200; 
bool mouthIsOpen = false;

// ============================================================================
// 5. INITIALIZATION SETUP
// ============================================================================
void setup() {
  initSystem();
  lcd.begin(16, 2);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(joystickSW, INPUT_PULLUP);

  // Custom Character Index Mapping
  lcd.createChar(0, uMouth);      // Slot 0: Center Nose/Mouth curve
  lcd.createChar(1, sadMouth);    // Slot 1: Sad Mouth Curve
  lcd.createChar(2, madLeft);     // Slot 2: Left Angry Eye
  lcd.createChar(3, madRight);    // Slot 3: Right Angry Eye
  lcd.createChar(4, blinkEye);    // Slot 4: Async Blink Line
  lcd.createChar(6, openMouth);   // Slot 6: Feeding Chomper
}

// ============================================================================
// 6. MAIN SYSTEM LOOP
// ============================================================================
void loop() {
  unsigned long currentTime = millis();

  // Hardware Scan Registries
  int xVal = analogRead(joystickX);
  int yVal = analogRead(joystickY);
  bool currentButtonState = digitalRead(joystickSW);
  int lightLevel = analogRead(lightPin); 

  // Logarithmic Math Conversion Engine for Thermistor (Steinhart-Hart equation)
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

  // Metabolic Metric Decay (Only processes if the environment is lit)
  if (lightLevel >= 200 && (currentTime - lastDecayTime >= decayInterval)) {
    if (loveMeter > 0) loveMeter -= 5;  
    if (foodMeter > 0) foodMeter -= 5;  
    lastDecayTime = currentTime;
  }

  // Dynamic Blinking Engine
  bool isBlinking = false;
  if (currentTime - lastBlinkTime >= blinkInterval) {
    isBlinking = true;
    if (currentTime - lastBlinkTime >= (blinkInterval + blinkDuration)) {
      lastBlinkTime = currentTime;
    }
  }

  // User-Interface Chomp Clock
  if (isolatedAction && feedCounter > 0) {
    if (currentTime - lastChompTime >= chompInterval) {
      mouthIsOpen = !mouthIsOpen;
      lastChompTime = currentTime;
    }
  }

  // Input Interruption: Manual Cycle Button
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    currentEmotion = (currentEmotion + 1) % 4;
    manualOverrideActive = true; 
    isolatedAction = false; 
    delay(50); 
  }
  lastButtonState = currentButtonState;

  // Input Interruption: Joystick Right (Petting Command)
  if (xVal > 800 && !isolatedAction && lightLevel >= 200) {
    petCounter = (petCounter % 3) + 1;
    loveMeter = min(loveMeter + 15, 100); 
    isolatedAction = true;
    manualOverrideActive = false; 
    feedCounter = 0; 
    actionStartTime = currentTime;
    delay(200); 
  }

  // Input Interruption: Joystick Left (Feeding Command)
  if (xVal < 200 && !isolatedAction && lightLevel >= 200) {
    feedCounter = (feedCounter % 3) + 1;
    foodMeter = min(foodMeter + 15, 100); 
    isolatedAction = true;
    manualOverrideActive = false;
    petCounter = 0; 
    actionStartTime = currentTime;
    lastChompTime = currentTime;
    mouthIsOpen = true; 
    delay(200);
  }

  // Interrupt Lockout Timeout Release
  if (isolatedAction && (currentTime - actionStartTime > actionTimeout)) {
    isolatedAction = false;
    petCounter = 0;
    feedCounter = 0;
  }

  // Automated State Threshold Logic Engine
  if (!manualOverrideActive) {
    if (foodMeter <= 25) {
      currentEmotion = 1; 
    } 
    else if (loveMeter <= 25) {
      currentEmotion = 2; 
    } 
    else if (loveMeter >= 75 && foodMeter >= 75) {
      currentEmotion = 3; 
    } 
    else {
      currentEmotion = 0; 
    }
  }

  lcd.clear();

  // ==========================================================================
  // ENVIRONMENT-BASED LIGHT ROUTINE (DISPLAY GATEWAY)
  // ==========================================================================
  if (lightLevel < 200) { 
    // ------------------------------------------------------------------------
    //  SYSTEM SLEEP STATE (Lights out override)
    // ------------------------------------------------------------------------
    digitalWrite(redPin, LOW); digitalWrite(greenPin, LOW); digitalWrite(bluePin, LOW); 
    
    // Output: "- 0 -" with eyes spaced out to match standard facial templates
    lcd.setCursor(3, 0); lcd.print("-");
    lcd.setCursor(5, 0); lcd.print("0"); // FIXED: True text "0" open mouth
    lcd.setCursor(7, 0); lcd.print("-");
    lcd.setCursor(10, 0); lcd.print("zzz");
    
    lcd.setCursor(0, 1);
    lcd.print("SLEEPING...     ");
    
    wasAsleep = true; 
  } 
  else {
    // ------------------------------------------------------------------------
    //  EDGE-TRIGGERED TRANSITION STATE (Fires once upon illumination detection)
    // ------------------------------------------------------------------------
    if (wasAsleep == true) {
      // Full White RGB Initialization Glow
      digitalWrite(redPin, HIGH); digitalWrite(greenPin, HIGH); digitalWrite(bluePin, HIGH); 
      
      // Output: Spaced open-eye yawn profile "o 0 o Yawn"
      lcd.setCursor(3, 0); lcd.print("o");
      lcd.setCursor(5, 0); lcd.print("0"); // FIXED: True text "0" open mouth
      lcd.setCursor(7, 0); lcd.print("o");
      lcd.setCursor(10, 0); lcd.print("Yawn");
      
      lcd.setCursor(0, 1);
      lcd.print("AWAKING...      ");
      
      delay(1500); // UI Hold lock to let display render completely before dropping to standard loops
      wasAsleep = false; 
      lcd.clear();
    }

    // ------------------------------------------------------------------------
    //  STANDARD ACTIVE WAKE STATES 
    // ------------------------------------------------------------------------
    
    // ROW 1 RENDERS (FACE LAYOUT ENGINE)
    if (isolatedAction && petCounter > 0) {
      digitalWrite(redPin, HIGH); digitalWrite(greenPin, HIGH); digitalWrite(bluePin, HIGH); 
      lcd.setCursor(2, 0); lcd.print("^"); 
      lcd.setCursor(4, 0); lcd.print("w"); 
      lcd.setCursor(6, 0); lcd.print("^");
      
      lcd.setCursor(9, 0);
      if (petCounter == 1) lcd.print("LOVE "); 
      if (petCounter == 2) lcd.print("LOV  "); 
      if (petCounter == 3) lcd.print("LO   "); 
    }
    else if (isolatedAction && feedCounter > 0) {
      digitalWrite(redPin, HIGH); digitalWrite(greenPin, HIGH); digitalWrite(bluePin, LOW); 
      lcd.setCursor(2, 0); lcd.print(">"); 
      lcd.setCursor(4, 0); lcd.write(mouthIsOpen ? byte(6) : byte(1)); 
      lcd.setCursor(6, 0); lcd.print("<"); 
      
      lcd.setCursor(9, 0);
      if (feedCounter == 1) lcd.print("NOM    ");
      if (feedCounter == 2) lcd.print("NOM NOM");
      if (feedCounter == 3) lcd.print("3X NOM ");
    }
    else {
      if (currentEmotion == 0) {
        // HAPPY FACE STATE
        digitalWrite(redPin, LOW); digitalWrite(greenPin, HIGH); digitalWrite(bluePin, LOW); 
        lcd.setCursor(4, 0); lcd.print(isBlinking ? "–" : "o"); 
        lcd.setCursor(6, 0); lcd.write(byte(0)); 
        lcd.setCursor(8, 0); lcd.print(isBlinking ? "–" : "o"); 
        lcd.setCursor(10, 0); lcd.print("HAPPY!");
      } 
      else if (currentEmotion == 1) {
        // MAD FACE STATE
        digitalWrite(redPin, HIGH); digitalWrite(greenPin, LOW); digitalWrite(bluePin, LOW); 
        lcd.setCursor(4, 0); lcd.write(isBlinking ? byte(4) : byte(2)); 
        lcd.setCursor(6, 0); lcd.print("_");                             
        lcd.setCursor(8, 0); lcd.write(isBlinking ? byte(4) : byte(3)); 
        lcd.setCursor(11, 0); lcd.print("MAD!");
      }
      else if (currentEmotion == 2) {
        // SAD FACE STATE
        digitalWrite(redPin, LOW); digitalWrite(greenPin, LOW); digitalWrite(bluePin, HIGH); 
        lcd.setCursor(3, 0); lcd.print(isBlinking ? " " : ".");    
        lcd.setCursor(4, 0); lcd.print(isBlinking ? "–" : "o");    
        lcd.setCursor(6, 0); lcd.write(byte(1)); 
        lcd.setCursor(8, 0); lcd.print(isBlinking ? "–" : "o");    
        lcd.setCursor(11, 0); lcd.print("SAD!");
      }
      else if (currentEmotion == 3) {
        // CUTE FACE STATE
        digitalWrite(redPin, HIGH); digitalWrite(greenPin, LOW); digitalWrite(bluePin, HIGH); 
        lcd.setCursor(4, 0); lcd.print(isBlinking ? "–" : "^"); 
        lcd.setCursor(6, 0); lcd.write(byte(0)); 
        lcd.setCursor(8, 0); lcd.print(isBlinking ? "–" : "^"); 
        lcd.setCursor(11, 0); lcd.print("CUTE!");
      }
    }

    // ROW 2 RENDERS (METRIC DISPLAY & ENVIRONMENT TELEMETRY)
    lcd.setCursor(0, 1);
    if (isolatedAction && petCounter > 0) {
      lcd.print("PETTING...      ");
    }
    else if (isolatedAction && feedCounter > 0) {
      lcd.print("FEEDING...      ");
    }
    else if (yVal < 200) {
      lcd.print("Love: "); lcd.print(loveMeter); lcd.print("% ");
      int bars = loveMeter / 20; 
      for(int i=0; i<5; i++) { if(i < bars) lcd.print((char)255); else lcd.print("."); } 
      lcd.print("   ");
    }
    else if (yVal > 800) {
      lcd.print("Food: "); lcd.print(foodMeter); lcd.print("% ");
      int bars = foodMeter / 20;
      for(int i=0; i<5; i++) { if(i < bars) lcd.print((char)255); else lcd.print("."); }
      lcd.print("   ");
    }
    else {
      lcd.print("Temp: "); lcd.print(fahrenheit, 1); lcd.print(" F     ");
    }
  } // End of Active Wake block

  delay(50); // Steady Clock pacing delay
}
