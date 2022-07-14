#include <TimerOne.h>

#define RECEIVER_PIN          2
#define TRANSMITTER_PIN_0     6
#define TRANSMITTER_PIN_1     11
#define TRANSMITTER_SELECTOR  12
#define PWM_FREQUENCY         40000   // hz
#define PWM_DURATION          50     // ms
#define SLEEP_TIME            1200    // ms
#define MAX_PULSE_COUNT       8
#define READ_TIMEOUT          1000

#define K                     10 //(L/2*COS_ALPHA)

unsigned long initDownstreamTime, initUpstreamTime, totalDownstreamTime, totalUpstreamTime, initTime, initTimeout;
bool calculationReady = false;
bool readingPulses = false;

bool index = 0;

void setup() {
  // put your setup code here, to run once:
  //Timer1.initialize(12);
  //Timer1.attachInterrupt(ISR_callback);
  pinMode(TRANSMITTER_SELECTOR,OUTPUT);
  pinMode(TRANSMITTER_PIN_0,OUTPUT);
  pinMode(TRANSMITTER_PIN_1,OUTPUT);
  pinMode(RECEIVER_PIN, INPUT);
  digitalWrite(TRANSMITTER_PIN_0, HIGH);
  digitalWrite(TRANSMITTER_PIN_1, HIGH);
  //selectTransmitter(index);
  
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(RECEIVER_PIN), readDownstreamPulses, RISING);
  Serial.println("Iniciando...");
}

void loop() {
  // f = 40 kHz <-> T = 0.25 ms => 8 pulses: 2 us
  sendPulses(index);

  initTimeout = millis();
  while(true) {
    if (calculationReady) {
      Serial.print("Tiempo transductor ");
      Serial.print(index);
      Serial.print(": ");
      Serial.println(totalDownstreamTime);
      break;
    }
    else if (millis() - initTimeout > READ_TIMEOUT) {
      Serial.print("Timeout en transductor ");
      Serial.print(index);
      break;            
    }
  }
  
  //index = !index;
  delay(SLEEP_TIME);
}

void sendPulses(bool index) {
  calculationReady = false;
  readingPulses = false;
  
  selectTransmitter(index);
  readingPulses = true;
  Serial.println("--> Reading pulses!");
  
  initTime = micros();
  if (index) {
    tone(TRANSMITTER_PIN_1, PWM_FREQUENCY);
    delay(PWM_DURATION); //delayMicroseconds(PWM_DURATION);
    noTone(TRANSMITTER_PIN_1);
    //digitalWrite(TRANSMITTER_PIN_1, HIGH);
  }
  else {
    tone(TRANSMITTER_PIN_0, PWM_FREQUENCY);
    delay(PWM_DURATION); //delayMicroseconds(PWM_DURATION);
    noTone(TRANSMITTER_PIN_0);
    //digitalWrite(TRANSMITTER_PIN_0, HIGH);
  }
}

void selectTransmitter(bool index) {
  digitalWrite(TRANSMITTER_SELECTOR, index);
  delay(500);
}

void readDownstreamPulses() {
  if (readingPulses == true && calculationReady == false) {
    totalDownstreamTime = micros() - initTime;
    calculationReady = true;
    readingPulses = false;
  }
  else {
    //Serial.println("Espureo!");
  }
  /*count++;
  if (count == MAX_PULSE_COUNT) {
    // calculate time between transmition and reception
    totalDownstreamTime = millis() - initTime;
    count = 0;
  }*/
}

void calculateVelocity(void) {
  float v;
  v = K*(totalUpstreamTime - totalDownstreamTime)/(totalUpstreamTime*totalDownstreamTime);
  calculationReady = true;
}
