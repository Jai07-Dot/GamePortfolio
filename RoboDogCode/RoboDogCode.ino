//Jaira Settles
#include <Servo.h>

// --- PIN CONFIGURATIONS ---
#define RightDirectPin1 12
#define RightDirectPin2 11
#define speedPinR        3
#define LeftDirectPin1   7
#define LeftDirectPin2   8
#define speedPinL        6
#define Echo_PIN         2
#define Trig_PIN        10
#define BUZZ_PIN        13

// Verified Hardware Pins
#define GREEN_PIN       A1  
#define BLUE_PIN        A3  

// --- NON-BLOCKING TIMING VARIABLES ---
unsigned long lastBuzzerAction = 0;
unsigned long durationBuzzer = 0; 
bool buzzerIsOn = false;
unsigned long lastBuzzerToggle = 0;
bool buzzerPinState = HIGH;
int whinePitchModifier = 2; 

unsigned long lastBarkTimestamp = 0;
const unsigned long barkInterval = 2000; 
unsigned long lastWhineTimestamp = 0;
const unsigned long whineInterval = 2500; 

// --- NEW: SMOOTH TRANSITION TIMING WINDOW VARIABLES ---
unsigned long lastTransitionTime = 0;
const unsigned long transitionWindow = 200; // 200ms safety window between motor changes
int currentZone = 0; // 0=Rest, 1=Danger, 2=Walk, 3=Run

void setup() {
  // Motor control pins
  pinMode(RightDirectPin1, OUTPUT); pinMode(RightDirectPin2, OUTPUT);
  pinMode(LeftDirectPin1, OUTPUT);  pinMode(LeftDirectPin2, OUTPUT);
  pinMode(speedPinR, OUTPUT);      pinMode(speedPinL, OUTPUT);
  
  // Ultrasonic Sensor pins
  pinMode(Trig_PIN, OUTPUT);       pinMode(Echo_PIN, INPUT);
  
  // Force the correct analog pins into digital output mode
  pinMode(GREEN_PIN, OUTPUT);       
  pinMode(BLUE_PIN, OUTPUT);
  
  // Active-low buzzer setup (starts off/silent)
  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(BUZZ_PIN, HIGH); 
  
  // Debug Serial Monitor
  Serial.begin(9600);
}

void loop() {
  // 1. Constantly runs the buzzer frequency generator in the background
  handleNonBlockingBuzzer(millis());

  // 2. Pure standalone smart pet behaviors
  autoFollow();
}

// --- BEHAVIOR ENGINE (REVERSED FOR BACK RADAR WITH STABILIZATION WINDOW) ---
void autoFollow() {
  int distance = watch();
  unsigned long currentMillis = millis();
  int targetZone = 0;

  // Map the distance to a target zone ID
  if (distance <= 20) {
    targetZone = 1; // Danger / Cyan
  } else if (distance > 20 && distance <= 45) {
    targetZone = 2; // Walk / Green
  } else if (distance > 45 && distance < 100) {
    targetZone = 3; // Run / Blue
  } else {
    targetZone = 0; // Out of Range / Rest
  }

  // Only allow a zone change if the 200ms stabilization window has passed
  if (targetZone != currentZone) {
    if (currentMillis - lastTransitionTime >= transitionWindow) {
      currentZone = targetZone;
      lastTransitionTime = currentMillis; // Reset the window timer
    }
  }

  // --- EXECUTE STABILIZED ZONE ACTIONS ---
  switch (currentZone) {
    
    case 1: // DANGER ZONE (CYAN): Too close! Stop completely.
      setRGB(HIGH, HIGH);     // Cyan / Teal
      stop_Stop();            // Stop moving completely
      
      if (currentMillis - lastBarkTimestamp >= barkInterval) {
        triggerBark(50);      // Quick alert chirp
        lastBarkTimestamp = currentMillis;
      }
      break;

    case 2: // CLOSE FOLLOWING ZONE (PURE GREEN): Walk toward it.
      setRGB(HIGH, LOW);      // Pure Green
      go_Walk();              // Walk smoothly toward the trigger
      break;

    case 3: // LONG DISTANCE FOLLOWING ZONE (PURE BLUE): Run to catch up.
      setRGB(LOW, HIGH);      // Pure Blue
      go_Run();               // Run swiftly toward the trigger
      
      if (currentMillis - lastBarkTimestamp >= barkInterval) {
        triggerBark(50);      // Eager catching-up chirp
        lastBarkTimestamp = currentMillis;
      }
      break;

    default: // OUT OF RANGE / NO READ (LONELY WHINE)
      setRGB(LOW, LOW);       // Out of sight: No color / LEDs Off
      stop_Stop();            // Park safely
      
      if (currentMillis - lastWhineTimestamp >= whineInterval) {
        triggerLonelyWhine(200); // 200ms sad yip/whimper
        lastWhineTimestamp = currentMillis;
      }
      break;
  }
}

// --- HARDWARE UTILITY FUNCTIONS ---

// Ultrasonic ping calculation
int watch() {
  digitalWrite(Trig_PIN, LOW); delayMicroseconds(2);
  digitalWrite(Trig_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(Trig_PIN, LOW);
  
  long durationResult = pulseIn(Echo_PIN, HIGH, 8000); 
  if (durationResult == 0) return 999; 
  return round(durationResult * 0.01657);
}

// Pure digital switching to pins A1 and A3
void setRGB(int g, int b) {
  digitalWrite(GREEN_PIN, g);
  digitalWrite(BLUE_PIN, b);
}

// Walk Function (Directions swapped: HIGH/LOW flipped to reverse movement)
void go_Walk() {
  digitalWrite(RightDirectPin1, LOW);  digitalWrite(RightDirectPin2, HIGH);
  digitalWrite(LeftDirectPin1, LOW);   digitalWrite(LeftDirectPin2, HIGH);
  analogWrite(speedPinL, 130);         analogWrite(speedPinR, 130); 
}

// Run Function (Directions swapped: HIGH/LOW flipped to reverse movement)
void go_Run() {
  digitalWrite(RightDirectPin1, LOW);  digitalWrite(RightDirectPin2, HIGH);
  digitalWrite(LeftDirectPin1, LOW);   digitalWrite(LeftDirectPin2, HIGH);
  analogWrite(speedPinL, 210);         analogWrite(speedPinR, 210); 
}

// Full Stop
void stop_Stop() {
  digitalWrite(RightDirectPin1, LOW); digitalWrite(RightDirectPin2, LOW);
  digitalWrite(LeftDirectPin1, LOW);  digitalWrite(LeftDirectPin2, LOW);
  analogWrite(speedPinL, 0);          analogWrite(speedPinR, 0);
}

// --- ASYNCHRONOUS BUZZER CONTROLLERS ---
void triggerBark(unsigned long durationParam) {
  if (!buzzerIsOn) {
    buzzerIsOn = true;
    durationBuzzer = durationParam;
    whinePitchModifier = 2; 
    lastBuzzerAction = millis();
  }
}

void triggerLonelyWhine(unsigned long durationParam) {
  if (!buzzerIsOn) {
    buzzerIsOn = true;
    durationBuzzer = durationParam;
    whinePitchModifier = 1; 
    lastBuzzerAction = millis();
  }
}

void handleNonBlockingBuzzer(unsigned long currentMillis) {
  if (buzzerIsOn) {
    if (currentMillis - lastBuzzerAction >= durationBuzzer) {
      digitalWrite(BUZZ_PIN, HIGH); 
      buzzerIsOn = false;
      lastBuzzerAction = currentMillis; 
    } else {
      if (durationBuzzer > 100 && (currentMillis % 15 == 0)) {
         whinePitchModifier++; 
         if(whinePitchModifier > 12) whinePitchModifier = 12; 
      }

      if (currentMillis - lastBuzzerToggle >= whinePitchModifier) { 
        lastBuzzerToggle = currentMillis;
        buzzerPinState = !buzzerPinState;
        digitalWrite(BUZZ_PIN, buzzerPinState);
      }
    }
  } else {
    digitalWrite(BUZZ_PIN, HIGH); 
  }
}