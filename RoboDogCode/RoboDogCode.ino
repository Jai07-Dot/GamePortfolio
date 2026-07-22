// =========================================================================
// AUTHOR: Jaira Settles
// PROJECT: Autonomous Robot State Engine
// COPYRIGHT: (c) 2026 Jaira Settles. All Rights Reserved.
// =========================================================================

const String AUTHOR = "Jaira Settles";
const String VERSION = "1.0.0";

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
#define SERVO_PIN        9  

// Hardware Pins (A5 clears the timer conflict, A1 and A3 are fully working!)
#define RED_PIN          A5  
#define GREEN_PIN       A1  
#define BLUE_PIN        A3  
#include <iostream>
#include <string>



// --- DOG STATE ENGINE ---
enum DogMood { FOLLOWING, ZOOMIES, CHASING_TAIL, GUARD, STANDBY };
DogMood currentMood = FOLLOWING;

// --- SERVO RADAR VARIABLES ---
Servo headServo;
unsigned long lastServoMove = 0;
const unsigned long servoInterval = 250; 
int servoPositions[] = {50, 90, 130, 90}; 
int servoIndex = 0;
int lastTargetDirection = 1; 

// --- NON-BLOCKING TIMING VARIABLES ---
unsigned long lastBuzzerAction = 0;
unsigned long durationBuzzer = 0; 
bool buzzerIsOn = false;
unsigned long lastBuzzerToggle = 0;
bool buzzerPinState = HIGH;
int pitchDelayMicros = 2; 

unsigned long lastBarkTimestamp = 0;
const unsigned long barkInterval = 10000; 
unsigned long lastWhineTimestamp = 0;
const unsigned long whineInterval = 2500; 

// AI Tracking Timers & Stabilization Windows
unsigned long lastTransitionTime = 0;
const unsigned long transitionWindow = 250; 
int currentZone = 0; 

unsigned long outOfRangeStart = 0;
unsigned long greenZoneStart = 0;
unsigned long lastSniffTime = 0;
bool sniffToggle = false;

// --- STRATEGY B: SEPARATED INDEPENDENT TIMERS ---
unsigned long zoomiesTimer = 0;             
unsigned long guardDogStandoffTimer = 0;   
unsigned long tailChaseTimer = 0;          
unsigned long lastGuardCheckMillis = 0;    
const unsigned long guardCheckInterval = 100; 

// --- ZOOMIES CONTROLS & VISUAL COUNTDOWN ---
unsigned long zoomiesSubPhaseMillis = 0;
int zoomiesPhaseStep = 0; 
unsigned long countdownStartMillis = 0;
bool isCountingDownToZoomies = false;

void setup() {
  Serial.begin(9600);
  Serial.println("==================================================");
  Serial.println(" Core Engine initialized | Author: " + AUTHOR);
  Serial.println(" Version: " + VERSION);
  Serial.println("==================================================");

  // Motor control pins
  pinMode(RightDirectPin1, OUTPUT); pinMode(RightDirectPin2, OUTPUT);
  pinMode(LeftDirectPin1, OUTPUT);  pinMode(LeftDirectPin2, OUTPUT);
  pinMode(speedPinR, OUTPUT);      pinMode(speedPinL, OUTPUT);
  
  // Ultrasonic Sensor pins
  pinMode(Trig_PIN, OUTPUT);       pinMode(Echo_PIN, INPUT);
  
  // LED Output Setup
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);       
  pinMode(BLUE_PIN, OUTPUT);
  
  // Active-low buzzer setup
  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(BUZZ_PIN, HIGH); 
  
  // Attach Servo to Pin 9 and initialize facing center (90 degrees)
  headServo.attach(SERVO_PIN);
  headServo.write(90);
  
  // Seed random generator
  randomSeed(analogRead(A0));
}

// =========================================================================
// --- CENTRAL MASTER ENGINE CONTROLLER ---
// =========================================================================
void loop() {
  unsigned long currentMillis = millis();
  
  // Keep background sound engines cycling smoothly without lockups
  handleNonBlockingBuzzer(currentMillis);

  // -----------------------------------------------------------------------
  // PRIORITY LOCKOUT ZONE 1: ZOOMIES SEQUENCE 
  // -----------------------------------------------------------------------
  if (isCountingDownToZoomies) {
    handleZoomieCountdown(currentMillis);
    return; 
  }
  
  if (currentMood == ZOOMIES) {
    runIsolatedZoomies(currentMillis);
    return; 
  }

  // -----------------------------------------------------------------------
  // PRIORITY LOCKOUT ZONE 2: GUARD DOG MODE (MAD DOG)
  // -----------------------------------------------------------------------
  if (currentMood == GUARD) {
    runIsolatedGuardDog(currentMillis);
    return; 
  }

  // -----------------------------------------------------------------------
  // STANDARD OPERATION ZONE
  // -----------------------------------------------------------------------
  switch(currentMood) {
    case FOLLOWING:    autoFollow(currentMillis);    break;
    case CHASING_TAIL: runChasingTail(currentMillis); break;
    case STANDBY:      runStandbyMode(currentMillis); break;
    default:           break;
  }
}

// =========================================================================
// --- ISOLATED MOOD LOGIC BLOCKS ---
// =========================================================================

void handleZoomieCountdown(unsigned long currentMillis) {
  // COUNTDOWN TO ZOOMIES: Pure White (All channels ON)
  setRGB(HIGH, HIGH, HIGH);  
  
  if (currentMillis - countdownStartMillis < 1000) {
    go_Backward_Smooth(120, 120); 
  } else {
    stop_Stop(); 
  }
  
  if (currentMillis - countdownStartMillis >= 2500) { 
    isCountingDownToZoomies = false; 
    currentMood = ZOOMIES;
    zoomiesTimer = currentMillis + 3800; 
    zoomiesPhaseStep = 1;
    zoomiesSubPhaseMillis = currentMillis;
    stop_Stop();
  }
}

void runIsolatedZoomies(unsigned long currentMillis) {
  if (currentMillis > zoomiesTimer) { 
    stop_Stop(); 
    setRGB(LOW, LOW, LOW); 
    currentMood = FOLLOWING; 
    currentZone = 0; 
    zoomiesPhaseStep = 0; 
    return; 
  }
  
  // ZOOMIES ACTIVE: Pure White
  setRGB(HIGH, HIGH, HIGH);
  
  if (zoomiesPhaseStep == 1) {
    go_Advance_Smooth(80, 220);
    if (currentMillis - zoomiesSubPhaseMillis >= 1000) {
      zoomiesSubPhaseMillis = currentMillis;
      zoomiesPhaseStep = 2;
    }
  } 
  else if (zoomiesPhaseStep == 2) {
    go_Advance_Smooth(220, 80);
    if (currentMillis - zoomiesSubPhaseMillis >= 1000) {
      zoomiesSubPhaseMillis = currentMillis;
      zoomiesPhaseStep = 3;
    }
  } 
  else if (zoomiesPhaseStep == 3) {
    spin_In_Place(230, 230, true, false); 
    if (currentMillis - zoomiesSubPhaseMillis >= 1800) {
      stop_Stop();
      setRGB(LOW, LOW, LOW);
      currentMood = FOLLOWING; 
      currentZone = 0; 
      zoomiesPhaseStep = 0;
    }
  }
}

void runIsolatedGuardDog(unsigned long currentMillis) {
  if (currentMillis - lastGuardCheckMillis >= guardCheckInterval) {
    lastGuardCheckMillis = currentMillis;
    
    int checkDistance = watch();
    if (checkDistance > 30 && checkDistance != 999) {
      stop_Stop();
      setRGB(LOW, LOW, LOW);
      digitalWrite(BUZZ_PIN, HIGH); 
      currentMood = FOLLOWING; 
      currentZone = 0; 
      return; 
    }
  }
  
  // MAD DOG STATE: Flashing Pure Red
  if ((currentMillis / 100) % 2 == 0) {
    setRGB(HIGH, LOW, LOW);  
  } else {
    setRGB(LOW, LOW, LOW);   
  }
  
  if ((currentMillis / 200) % 2 == 0) {
    go_Advance_Smooth(190, 190); 
  } else {
    spin_In_Place(160, 160, false, false); 
  }
  
  if (currentMillis % 10 < 5) {
    digitalWrite(BUZZ_PIN, LOW);
  } else {
    digitalWrite(BUZZ_PIN, HIGH);
  }
}

// =========================================================================
// --- STANDARD SENSOR TRACKING ENGINE ---
// =========================================================================
void autoFollow(unsigned long currentMillis) {
  if (currentMillis - lastServoMove >= servoInterval) {
    lastServoMove = currentMillis;
    headServo.write(servoPositions[servoIndex]);
    
    if (servoPositions[servoIndex] == 50)       lastTargetDirection = 0; 
    else if (servoPositions[servoIndex] == 90)  lastTargetDirection = 1; 
    else if (servoPositions[servoIndex] == 130) lastTargetDirection = 2; 
    
    servoIndex = (servoIndex + 1) % 4; 
  }

  int distance = watch();
  int targetZone = 0;

  if (distance <= 20)                       targetZone = 1; 
  else if (distance > 20 && distance <= 45) targetZone = 2; 
  else if (distance > 45 && distance < 100) targetZone = 3; 
  else                                      targetZone = 0; 

  if (targetZone != currentZone) {
    if (currentMillis - lastTransitionTime >= transitionWindow) {
      
      if (currentMood == STANDBY && (targetZone == 2 || targetZone == 3)) {
        if (random(0, 2) == 0) {
          isCountingDownToZoomies = true;
          countdownStartMillis = currentMillis;
          stop_Stop(); 
          currentZone = targetZone;
          lastTransitionTime = currentMillis;
          guardDogStandoffTimer = 0; 
          return; 
        }
      }
      
      currentZone = targetZone;
      lastTransitionTime = currentMillis;
    }
  }

  // 1. DANGER ZONE -> Solid Red Standoff Mode
  if (currentZone == 1) { 
    greenZoneStart = 0;
    
    setRGB(HIGH, LOW, LOW); // MADDOG-RED
    stop_Stop();            
    outOfRangeStart = 0;

    if (guardDogStandoffTimer == 0) {
      guardDogStandoffTimer = currentMillis; 
    }
    
    if (currentMillis - guardDogStandoffTimer > 60000) {
      currentMood = GUARD; 
      guardDogStandoffTimer = 0;
      return;
    }
    
    if (currentMillis - lastBarkTimestamp >= barkInterval) {
      triggerLowerBark(50);      
      lastBarkTimestamp = currentMillis;
    }
  } 
  
  // 2. CLOSE FOLLOWING ZONE 
  else if (currentZone == 2) { 
    guardDogStandoffTimer = 0; 
    if (greenZoneStart == 0) greenZoneStart = currentMillis;
    
    if (currentMillis - greenZoneStart > 8000) {
      currentMood = CHASING_TAIL;
      tailChaseTimer = currentMillis + 4000; 
      greenZoneStart = 0;
      stop_Stop();
      return;
    }

    setRGB(LOW, HIGH, LOW); // CLOSE FOLLOW: GREEN
    
    if (lastTargetDirection == 0)      go_Advance_Smooth(80, 150);  
    else if (lastTargetDirection == 2) go_Advance_Smooth(150, 80);  
    else                               go_Advance_Smooth(130, 130); 
    outOfRangeStart = 0;
  } 
  
  // 3. LONG DISTANCE ZONE
  else if (currentZone == 3) { 
    guardDogStandoffTimer = 0; 
    greenZoneStart = 0;
    outOfRangeStart = 0;

    setRGB(LOW, LOW, HIGH); // DISTANCE FOLLOW: BLUE
    
    if (lastTargetDirection == 0)      go_Advance_Smooth(130, 230); 
    else if (lastTargetDirection == 2) go_Advance_Smooth(230, 130); 
    else                               go_Advance_Smooth(210, 210); 
    
    if (currentMillis - lastBarkTimestamp >= barkInterval) {
      triggerLowerBark(50);      
      lastBarkTimestamp = currentMillis;
    }

    if ((currentMillis % 1000 == 0) && random(0, 100) < 5) {
      isCountingDownToZoomies = true;
      countdownStartMillis = currentMillis;
      stop_Stop();
    }
  } 
  
  // 4. OUT OF RANGE / GOING LONELY
  else { 
    guardDogStandoffTimer = 0; 
    greenZoneStart = 0;
    if (outOfRangeStart == 0) outOfRangeStart = currentMillis;
    
    if (currentMillis - outOfRangeStart > 5000) {
      currentMood = STANDBY;
    } else {
      setRGB(LOW, LOW, LOW); 
      stop_Stop();
    }
  }
}

void runChasingTail(unsigned long currentMillis) {
  if (currentMillis > tailChaseTimer) { 
    currentMood = FOLLOWING; 
    currentZone = 0; 
    return; 
  }
  setRGB(LOW, HIGH, LOW); // Matches happy Green follow
  spin_In_Place(220, 220, true, false); 
}

void runStandbyMode(unsigned long currentMillis) {
  headServo.write(90);
  int testDistance = watch();

  if (testDistance < 100) {
    currentMood = FOLLOWING;
    currentZone = 0;
    outOfRangeStart = 0;
    return;
  }

  setRGB(LOW, HIGH, HIGH); // SAD DOG / SNIFF: CYAN

  if (currentMillis - lastWhineTimestamp >= whineInterval) {
    triggerHighWhine(100); 
    lastWhineTimestamp = currentMillis;
  }

  if (currentMillis - lastSniffTime > 700) {
    lastSniffTime = currentMillis;
    sniffToggle = !sniffToggle;
    
    if (sniffToggle) {
      go_Advance_Smooth(100, 0); 
      digitalWrite(BUZZ_PIN, LOW); delayMicroseconds(80); digitalWrite(BUZZ_PIN, HIGH);
    } else {
      go_Advance_Smooth(0, 100); 
      digitalWrite(BUZZ_PIN, LOW); delayMicroseconds(80); digitalWrite(BUZZ_PIN, HIGH);
    }
  }
}

// =========================================================================
// --- HARDWARE SUBSYSTEM DRIVERS ---
// =========================================================================

void go_Advance_Smooth(int leftSpeed, int rightSpeed) {
  digitalWrite(RightDirectPin1, LOW);  digitalWrite(RightDirectPin2, HIGH);
  digitalWrite(LeftDirectPin1, LOW);   digitalWrite(LeftDirectPin2, HIGH);
  analogWrite(speedPinL, leftSpeed);   analogWrite(speedPinR, rightSpeed);
}

void go_Backward_Smooth(int leftSpeed, int rightSpeed) {
  digitalWrite(RightDirectPin1, HIGH); digitalWrite(RightDirectPin2, LOW);
  digitalWrite(LeftDirectPin1, HIGH);  digitalWrite(LeftDirectPin2, LOW);
  analogWrite(speedPinL, leftSpeed);   analogWrite(speedPinR, rightSpeed);
}

void spin_In_Place(int leftSpeed, int rightSpeed, bool leftClockwise, bool rightClockwise) {
  digitalWrite(LeftDirectPin1, leftClockwise ? HIGH : LOW);
  digitalWrite(LeftDirectPin2, leftClockwise ? LOW : HIGH);
  digitalWrite(RightDirectPin1, rightClockwise ? HIGH : LOW);
  digitalWrite(RightDirectPin2, rightClockwise ? LOW : HIGH);
  analogWrite(speedPinL, leftSpeed);
  analogWrite(speedPinR, rightSpeed);
}

void stop_Stop() {
  digitalWrite(RightDirectPin1, LOW); digitalWrite(RightDirectPin2, LOW);
  digitalWrite(LeftDirectPin1, LOW);  digitalWrite(LeftDirectPin2, LOW);
  analogWrite(speedPinL, 0);          analogWrite(speedPinR, 0);
}

void triggerLowerBark(unsigned long durationParam) {
  if (!buzzerIsOn) {
    buzzerIsOn = true;
    durationBuzzer = durationParam; 
    pitchDelayMicros = 4;           
    lastBuzzerAction = millis();
  }
}

void triggerHighWhine(unsigned long durationParam) {
  if (!buzzerIsOn) {
    buzzerIsOn = true;
    durationBuzzer = durationParam; 
    pitchDelayMicros = 1;           
    lastBuzzerAction = millis();
  }
}

void handleNonBlockingBuzzer(unsigned long currentMillis) {
  if (isCountingDownToZoomies) {
    int timeElapsed = currentMillis - countdownStartMillis;
    if ((timeElapsed >= 200 && timeElapsed < 350) || (timeElapsed >= 600 && timeElapsed < 750)) {
      if (currentMillis - lastBuzzerToggle >= 1) { 
        lastBuzzerToggle = currentMillis;
        buzzerPinState = !buzzerPinState;
        digitalWrite(BUZZ_PIN, buzzerPinState);
      }
    } else {
      digitalWrite(BUZZ_PIN, HIGH); 
    }
    return; 
  }

  if (buzzerIsOn) {
    if (currentMillis - lastBuzzerAction >= durationBuzzer) {
      digitalWrite(BUZZ_PIN, HIGH); 
      buzzerIsOn = false;
      lastBuzzerAction = currentMillis; 
    } else {
      if (durationBuzzer == 100 && (currentMillis % 12 == 0)) {
          pitchDelayMicros++; 
          if(pitchDelayMicros > 6) pitchDelayMicros = 6; 
      }
      if (currentMillis - lastBuzzerToggle >= pitchDelayMicros) { 
        lastBuzzerToggle = currentMillis;
        buzzerPinState = !buzzerPinState;
        digitalWrite(BUZZ_PIN, buzzerPinState);
      }
    }
  } else {
    if (currentMood != GUARD) {
      digitalWrite(BUZZ_PIN, HIGH); 
    }
  }
}

int watch() {
  digitalWrite(Trig_PIN, LOW); delayMicroseconds(2);
  digitalWrite(Trig_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(Trig_PIN, LOW);
  
  long durationResult = pulseIn(Echo_PIN, HIGH, 8000); 
  if (durationResult == 0) return 999; 
  return round(durationResult * 0.01657);
}

void setRGB(int r, int g, int b) {
  digitalWrite(RED_PIN, r);
  digitalWrite(GREEN_PIN, g);
  digitalWrite(BLUE_PIN, b);
}
