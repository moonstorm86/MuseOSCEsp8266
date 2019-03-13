
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <Wire.h>

char ssid[] = "Nerd Corner";          // your network SSID (name)

const IPAddress ip(192, 168, 1, 249);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

WiFiUDP Udp;
OSCErrorCode error;

const unsigned int localPort = 7000;     // local port to listen for UDP packets (here's where we send the packets)

float value_leftEar = 0;
float value_leftHead = 0;
float value_rightHead = 0;
float value_rightEar = 0;

void setup() {

  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid);
  WiFi.config(ip, gateway, subnet);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif

}

void eeg(OSCMessage &msg) {

  value_leftEar = msg.getFloat(0);
  value_leftHead = msg.getFloat(1);
  value_rightHead = msg.getFloat(2);
  value_rightEar = msg.getFloat(3);

  Serial.println('<' + String(value_leftEar) + ',' + String(value_rightEar) + ',' + String(value_rightHead) + '>');


  //  Serial.print("Left Ear: ");
  //  Serial.print(value_leftEar);
  //  Serial.print(" | ");
  //  Serial.print("Left Forehead: ");
  //  Serial.print(value_leftHead);
  //  Serial.print(" | ");
  //  Serial.print("Right Forehead: ");
  //  Serial.print(value_leftEar);
  //  Serial.print(" | ");
  //  Serial.print("Right Ear: ");
  //  Serial.println(value_rightEar);
}

//Absolute Band Powers
//Absolute band powers are based on the logarithm of the Power Spectral Density of the EEG data for each channel.
//Since it is a logarithm, some of the values will be negative (i.e. when the absolute power is less than 1).
//They are given on a log scale, units are Bels. These values are emitted at 10Hz.


// 1-4Hz, log band power (B)
void delta_absolute(OSCMessage &msg) {
  float value = msg.getFloat(3);
  Serial.print("delta_absolute : ");
  Serial.println(value);
}

// 5-8Hz, log band power (B)
void theta_absolute(OSCMessage &msg) {
  float value = msg.getFloat(3);
  Serial.print("theta_absolute : ");
  Serial.println(value);
}

// 9-13Hz, log band power (B)
void alpha_absolute(OSCMessage &msg) {
  float value = msg.getFloat(3);
  Serial.print("alpha_absolute : ");
  Serial.println(value);
}

// 13-30Hz, log band power (B)
void beta_absolute(OSCMessage &msg) {
  float value = msg.getFloat(3);
  Serial.print("beta_absolute : ");
  Serial.println(value);
}

// 30-50Hz, log band power (B)
void gamma_absolute(OSCMessage &msg) {
  float value = msg.getFloat(3);
  Serial.print("gamma_absolute : ");
  Serial.println(value);
}

void loop() {

  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("Muse-10AB/eeg", eeg);
      msg.dispatch("Muse-10AB/elements/delta_absolute", delta_absolute);
      msg.dispatch("Muse-10AB/elements/theta_absolute", theta_absolute);
      msg.dispatch("Muse-10AB/elements/alpha_absolute", alpha_absolute);
      msg.dispatch("Muse-10AB/elements/beta_absolute", beta_absolute);
      msg.dispatch("Muse-10AB/elements/gamma_absolute", gamma_absolute);
//      msg.dispatch("Muse-10AB/elements/delta_relative", delta_relative);
//      msg.dispatch("Muse-10AB/elements/theta_relative", theta_relative);
//      msg.dispatch("Muse-10AB/elements/alpha_relative", alpha_relative);
//      msg.dispatch("Muse-10AB/elements/beta_relative", beta_relative);
//      msg.dispatch("Muse-10AB/elements/gamma_relative", gamma_relative);

    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}
