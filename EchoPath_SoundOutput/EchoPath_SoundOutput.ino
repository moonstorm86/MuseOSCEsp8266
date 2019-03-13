#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <mozzi_midi.h>
#include <mozzi_rand.h>
#include <mozzi_fixmath.h> // for Q16n16 fixed-point fractional number type
#include <AutoMap.h>

const int MIN_F = 20;
const int MAX_F = 150;

AutoMap kMapF(0, 1023, MIN_F, MAX_F);

//#define THERMISTOR_PIN 1
//#define LDR_PIN 2
//#define NUM_VOICES 8
//#define THRESHOLD 10

//const char INPUT_PIN = 0; // set the input for the knob to analog pin 0

// to convey the volume level from updateControl() to updateAudio()
//byte volume;

//============

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing


// variables to hold the parsed data
char messageFromPC[numChars] = {0};

int brainSignal1  = 0;
int brainSignal2 = 0; // temp
int brainSignal3 = 0; // ldr

boolean newData = false;

//============

// harmonics
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos0(COS8192_DATA);

// duplicates but slightly off frequency for adding to originals
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7b(COS8192_DATA);

//============

// base pitch frequencies in 24n8 fixed int format (for speed later)
Q16n16 f1, f2, f3, f4, f5, f6; //,f7;

Q16n16 variation() {
  // 32 random bits & with 524287 (b111 1111 1111 1111 1111)
  // gives between 0-8 in Q16n16 format
  return  (Q16n16) (xorshift96() & 524287UL);
}

//============

void setup() {

  startMozzi(64);
  // select base frequencies using mtof (midi to freq) and fixed-point numbers
  f1 = Q16n16_mtof(Q16n0_to_Q16n16(48));
  f2 = Q16n16_mtof(Q16n0_to_Q16n16(74));
  f3 = Q16n16_mtof(Q16n0_to_Q16n16(64));
  f4 = Q16n16_mtof(Q16n0_to_Q16n16(77));
  f5 = Q16n16_mtof(Q16n0_to_Q16n16(67));
  f6 = Q16n16_mtof(Q16n0_to_Q16n16(57));
  //  f7 = Q16n16_mtof(Q16n0_to_Q16n16(60));

  // set Oscils with chosen frequencies
  aCos1.setFreq_Q16n16(f1);
  aCos2.setFreq_Q16n16(f2);
  aCos3.setFreq_Q16n16(f3);
  aCos4.setFreq_Q16n16(f4);
  aCos5.setFreq_Q16n16(f5);
  aCos6.setFreq_Q16n16(f6);
  //  aCos7.setFreq_Q16n16(f7);

  // set frequencies of duplicate oscillators
  aCos1b.setFreq_Q16n16(f1 + variation());
  aCos2b.setFreq_Q16n16(f2 + variation());
  aCos3b.setFreq_Q16n16(f3 + variation());
  aCos4b.setFreq_Q16n16(f4 + variation());
  aCos5b.setFreq_Q16n16(f5 + variation());
  aCos6b.setFreq_Q16n16(f6 + variation());
  //  aCos7b.setFreq_Q16n16(f7 + variation());

  Serial.begin(115200);

  Serial.println("This demo expects 3 pieces of data - text, an integer and a floating point value");
  Serial.println("Enter data in this style <HelloWorld, 12, 24.7>  ");

}

//============

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

//============

void updateControl() {

  recvWithEndMarker();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
//    showParsedData();
    newData = false;
  }
  showParsedData();

  int adjust1 = mozziAnalogRead(brainSignal1) + 1;
  adjust1 = kMapF(adjust1);
  Serial.println(adjust1);

  int adjust2 = mozziAnalogRead(brainSignal2);
  adjust2 = kMapF(adjust2);
  Serial.println(adjust2);

  // todo: choose a nice scale or progression and make a table for it
  // or add a very slow gliss for f1-f7, like shephard tones

  // change frequencies of the b oscillators
  switch (lowByte(xorshift96()) & 7) { // 7 is 0111

    case 0:
      aCos1b.setFreq_Q16n16(f1 + variation() * adjust1);
      break;

    case 1:
      aCos2b.setFreq_Q16n16(f2 + variation());
      break;

    case 2:
      aCos3b.setFreq_Q16n16(f3 + variation());
      break;

    case 3:
      aCos4b.setFreq_Q16n16(f4 + variation() * adjust2);
      break;

    case 4:
      aCos5b.setFreq_Q16n16(f5 + variation());
      break;

    case 5:
      aCos6b.setFreq_Q16n16(f6 + variation());
      break;
      /*
        case 6:
         aCos7b.setFreq_Q16n16(f7+variation());
        break;
      */
  }

  //  delay(1000);

}

//============

void parseData() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index
  char delimiters[] = ",";

  strtokIndx = strtok(tempChars, delimiters); // this continues where the previous call left off
  strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, delimiters); // this continues where the previous call left off
  brainSignal1 = atoi(strtokIndx);     // convert this part to an integer
  Serial.print("Message 1: ");
  Serial.println(brainSignal1);

  strtokIndx = strtok(NULL, "\n"); // this continues where the previous call left off
  brainSignal2 = atoi(strtokIndx);     // convert this part to an integer
  Serial.print("Message 2: ");
  Serial.println(brainSignal2);

}

//============

void showParsedData() {

  Serial.print("Message 1: ");
  Serial.println(brainSignal1);
  Serial.print("Message 2: ");
  Serial.println(brainSignal2);
  Serial.print("Message 3: ");
  Serial.println(brainSignal3);

}

int updateAudio() {

  int asig =
    aCos1.next() + aCos1b.next() +
    aCos2.next() + aCos2b.next() +
    aCos3.next() + aCos3b.next() +
    aCos4.next() + aCos4b.next() +
    aCos5.next() + aCos5b.next() +
    aCos6.next() + aCos6b.next();// +
  //  aCos7.next() + aCos7b.next();


  return asig >> 3;
}


void loop() {
  audioHook();
}
