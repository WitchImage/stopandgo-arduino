// Pins
const int GREEN_LED = 4;
const int RED_LED = 5;
const int BLUE_LED = 6;
const int WHITE_LED = 7;
const int ECHO1 = 9;
const int TRIGGER1 = 8;
const int ECHO2 = 3;
const int TRIGGER2 = 2;

// Timers constants
const long TRAFFIC_WAIT = 60000;

// Sensor variables
// Interval, prev and currrent in microseconds
float sound_speed = 0.034; // cm/s
long interval = 5;
unsigned long prev = 0;
unsigned long current = 0;
char state = 'P'; // Possible states: R -> Red, G -> Green, P -> Change from red to green, S -> Change from green to red
bool block_state = false; // True when operator remotely stops the traffic
bool should_wait = true;
bool should_pass = true;
int max_distance_cm = 100;

void setup() {
  Serial.begin(9600);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIGGER1, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIGGER2, OUTPUT);
  digitalWrite(TRIGGER1, LOW);
  digitalWrite(TRIGGER2, LOW);

  handleTrafficLights();
}

void loop() {
  handleSerialMessagesReceived();
  handleTrafficLights();
  handleTimer();
  pulseTrigger1();
  pulseTrigger2();
}

void handleSerialMessagesReceived() {
  while (Serial.available()) {
    String data = Serial.readString();
    if (data == "P") {
      state = 'P';
      Serial.println("Dato " + data);
    } else if (data == "R") {
      state = 'R';
    }
  }
}

void handleTrafficLights() {
  if(should_pass) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } else {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  }
}

void handleTimer() {
  if (state == 'P') {
    prev = millis() / 1000;
    state = 'G';
    should_pass = !should_pass;
  }
  current = millis() / 1000;
  Serial.println(String(current - prev));
  if (current - prev > interval) {
    state = 'P';
  }
}

void pulseTrigger1() {
  digitalWrite(TRIGGER1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER1, LOW);

  // measure duration of pulse from ECHO pin
  float duration_us = pulseIn(ECHO1, HIGH);

  // calculate the distance
  float distance_cm = 0.017 * duration_us;

  if (distance_cm > max_distance_cm) {
    digitalWrite(BLUE_LED, LOW);
  } else {
    digitalWrite(BLUE_LED, HIGH);
  }
}

void pulseTrigger2() {
  digitalWrite(TRIGGER2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER2, LOW);

  // measure duration of pulse from ECHO pin
  float duration_us = pulseIn(ECHO2, HIGH);

  // calculate the distance
  float distance_cm = 0.017 * duration_us;

  if (distance_cm > max_distance_cm) {
    digitalWrite(WHITE_LED, LOW);
  } else {
    digitalWrite(WHITE_LED, HIGH);
  }
}