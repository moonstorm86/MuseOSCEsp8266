This project pulls data from the Muse 2014 headband down to an ESP8266 using OSC. 

The second part of the project uses the parsed OSC packets with UDP and sends them over serial (RX/TX) to an Arduino Uno. On the Arduino a small sonic synthesizer is built with the Mozzi sound library. The idea here is to evaluate aspects of the data being pushed over the Arduino and use them as variables in sound synthesis thus augmenting the sonic output based on weighing the types of brain activity.

Significant limitations regarding the output occurred because there are problems sending this type of data through this method of capture. The device did function at times but remains very unreliable. 

I will be continuing this project in the near future. Since this project represents a proof of concept with significant contstraints due to the types of technology we were allowed to use in class, I will transition to a more robust tech stack for further exploration. 
