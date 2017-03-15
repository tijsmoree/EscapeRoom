#include <Key.h>
#include <Keypad.h>

// KEYPAD
const byte rows = 4;
const byte cols = 3;
char keys[rows][cols] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}};
byte rowPins[rows] = {6, 3, 2, 4};
byte colPins[cols] = {9, 10, 8};
Keypad pad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);


// PINS
const byte speaker = 5;
const byte finBut = 11;
const byte phone = 12;
const byte led = 13;


// NUMBERS
char numberActual[] = "7822000";
char numberReset[] = "abcdefg";
char numberGuess[] = "abcdefg";
int n = String(numberActual).length();
int i = 0;
boolean right = false;


// INITIAL SETUP
void setup() {
  Serial.begin(9600);
  
  pinMode(phone, INPUT);
  pinMode(led, OUTPUT);
  pinMode(speaker, OUTPUT);
  pinMode(finBut, INPUT_PULLUP);
}


// LOOPING
void loop() {
  // GET PHONE NUMBER
  char key = pad.getKey();
  if(!right) {
    if(digitalRead(phone)) { // not dialing
      String(numberReset).toCharArray(numberGuess, n);
      i = 0;
      noTone(speaker);
    } else if(key) {
      if(i < n) {
        numberGuess[i++] = key;
        
        tone(speaker, 425);
        delay(200);
        noTone(speaker);
      }
    } else if(i == 0) {
      tone(speaker, 425);
    }
  } else {
    noTone(speaker);
  }

  // CHECK PHONE NUMBER
  if(!right and i == n) {
    if(String(numberGuess).equals(String(numberActual))) {
      right = true;
      digitalWrite(led, HIGH);
      coin();

      Serial.write(5);
    } else {
      error();

      Serial.write(4);
      Serial.print(String(numberGuess));
    }
    i++;
  }

  // BUTTON PRESSED
  if(digitalRead(finBut) and right) {
    digitalWrite(led, LOW);
    Serial.write(1);
  }
}


// RIGHT TUNE
void coin() {
  delay(200);
  tone(speaker, 982);
  delay(100);
  tone(speaker, 1314);
  delay(500);
  noTone(speaker);
}


// WRONG TUNE
void error() {
  delay(200);
  tone(speaker, 130);
  delay(400);
  noTone(speaker);
}
