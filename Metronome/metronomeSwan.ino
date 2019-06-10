#include <SPI.h>
#include <Wire.h>

//---------------------------------------------------------------

#define BUZZER 6      // buzzer pin
#define MIN_BPM 20      // minimum bpm value
#define MAX_BPM 240     // maximum bpm value
#define POT A0          // pot analog pin

//---------------------------------------------------------------

int bpm;                // bpm value
//---------------------------------------------------------------

void setup() {
  
  pinMode(BUZZER, OUTPUT);  // buzer pin as output
  pinMode(POT,INPUT);
  Serial.begin(9600);
}

//---------------------------------------------------------------

void loop() {

    bpm = map(analogRead(POT), 0, 1023, MIN_BPM, MAX_BPM);  
    tone(BUZZER, 2000);       // does a "tick" for...
    delay(6000 / bpm);        // 10% of T (where T is the time between two BPSs)
    noTone(BUZZER);           // silence for...
    delay(54000 / bpm);       // 90% of T
    Serial.println("BPM: ");
    Serial.println(bpm);    

}
