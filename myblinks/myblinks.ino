// The code that blinks my name in morse code.

// ============================================================================
// AUTHOR METADATA
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

void setup() {
  initSystem();
  pinMode(LED_BUILTIN, OUTPUT);
}
// timing for the blinks
int dotTime = 200;
  int dashTime = 600;
  int letterSpace = 800;
void dot() {
digitalWrite(LED_BUILTIN, HIGH);
delay(dotTime);
digitalWrite(LED_BUILTIN, LOW);
delay(dotTime);
 
   }
void dash() {
 digitalWrite(LED_BUILTIN, HIGH);
delay(dashTime);
digitalWrite(LED_BUILTIN, LOW);
delay(dashTime);
 
   }
void loop() {
//J(.---)
dot(); dash(); dash(); dash();
delay(letterSpace);
//A(.-)
dot(); dash();
 //I(..)
 dot(); dot();
 delay(letterSpace);
 //R(.-.)
 dot(); dash(); dot();
 delay(letterSpace);
 //A(.-)
 dot(); dash();
 delay(2000); //pause before repeating
   
}
