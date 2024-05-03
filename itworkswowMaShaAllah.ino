#include <Servo.h>
//#include <Wire.h>
Servo servo;
Servo servo1;

// Ultrasonic Module pins
const int trigPin = 13;  // 10 microsecond high pulse causes chirp, wait 50 us
const int echoPin = 4;   // Width of high pulse indicates distance
// Servo motor that aims ultrasonic sensor
//const int servoPin = 6; // PWM output for hobby servo
const int right_sensor = 2;
const int left_sensor = 3;

// Motor A, Arduino and L298N connections
int enA = 5;   // ENABLE pin for Motor A to provide PWM (connected to Arduino digital pin 6)
int in1 = 11;  // IN1 pin for Motor A for dfirection (connected to Arduino digital pin 7)
int in2 = 12;  // IN2 pin for Motor A for direction (connected to Arduino digital pin 8)

// Motor B, Arduino and L298N connections
int enB = 6;  // ENABLE pin for Motor B to provide PWM (connected to Arduino digital pin 5)
int in3 = 7;   // IN3 pin for Motor B for direction (connected to Arduino digital pin 9)
int in4 = 8;   // IN4 pin for Motor B for direction (connected to Arduino digital pin 10)

const int BT_RX = 0;
const int BT_TX = 1;


char recieved = " ";
int p;
int dis;
long duration;
int drop = 0;
int obj = 0;

void setup() {
  Serial.begin(9600);  // Serial initializing for Debugging
  // Specify pinModes for enA, in1, in2, enB, in3, and in4, Bluetooth,IR sensors, Ultrasonic sensor
  pinMode(right_sensor, INPUT);
  pinMode(left_sensor, INPUT);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(BT_RX, INPUT);
  pinMode(BT_TX, OUTPUT);

   pinMode(BT_RX, INPUT);
  pinMode(BT_TX, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  // Start with 50% duty cycle
  analogWrite(enA, 128);  // Motor A: 50% duty cycle
  analogWrite(enB, 128);  // Motor B: 50% duty cycle

  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  // Motor A: Clockwise
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);  // Motor B: Clockwise

  servo.attach(9);
  // Add a short delay to stabilize the motor driver
  delay(100);
}

void collision(){ 
  int diss;
  diss = ultrasonicsensor();
  if (diss < 20){
    stop_motion();
    delay(200);
    move_backward();
    forwardright();
    diss = ultrasonicsensor();
    if (diss < 20){
      stop_motion();
      obj++;
      delay(200);
      move_backward();
      forwardleft();
      forwardleft();
    }
    else{
      move_forward();
    }
  }
  else{
    move_forward();
  }
}

void move_backward() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);  // Motor A: Clockwise
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enA, 220);  // Motor A: 50% duty cycle
  analogWrite(enB, 128);

  delay(200);
}

void move_forward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  analogWrite(enA, 220);  // Motor A: 50% duty cycle
  analogWrite(enB, 128);

  delay(200);
}

void forwardright() {
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);

  analogWrite(enA, 220);  // Motor A: 50% duty cycle
  analogWrite(enB, 0);

  delay(200);
}

void forwardleft() {
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);

  analogWrite(enB, 128);
  analogWrite(enA, 0);  // Motor A: 50% duty cycle
 

  delay(200);
}

void stop_motion() {
  digitalWrite(in1, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);

  analogWrite(enA, 0);  // Motor A: 50% duty cycle
  analogWrite(enB, 0);

  delay(100);
}


int ultrasonicsensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(15);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(15);
  duration = pulseIn(echoPin, HIGH);
  dis = duration * 0.034 / 2;
  Serial.print("distance is ");
  Serial.print(dis);
  return dis;
}

void line_follow() {
  int left_value = 0;
  int right_value = 0;
  int dist_i;
  int count = 0;
  while(right_value != 1 && left_value!=1){
  
   dist_i = ultrasonicsensor();
      if(dist_i < 10){
        stop_motion();
        obj++;
      }
      else{
        delay(200);
    int left_value = digitalRead(left_sensor);
    int right_value = digitalRead(right_sensor);
    int count=0;
    Serial.print(left_value);
    Serial.print(right_value);

    // Simple line following logic
    if (left_value == 0 && right_value == 0) { 
      if(count >= 3){
        stop_motion();
        break;
      }
      else{
        analogWrite(enA, 60*3);  // Motor A: 50% duty cycle
        analogWrite(enB, 50*3);
        digitalWrite(in1, HIGH);
        digitalWrite(in4, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        delay(100);
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW); 
        delay(50);
        count++;
      }
    } 

    else if (left_value == HIGH && right_value==LOW) { // Left sensor on line, turn right
      analogWrite(enA, 50*3);
      analogWrite(enB, 0);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW); // Slow left motor
      delay(100);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW); 
      delay(50);
    }

    else if (right_value == HIGH && left_value==LOW) { // Right sensor on line, turn left
      Serial.print("abdhh (01)");
      analogWrite(enB, 50*3);
      analogWrite(enA, 0);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH); // Slow right motor
      delay(100);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW); 
      delay(50);
      
    } 
    else if (right_value == 1 && left_value==1){
      stop_motion();
      servo.write(0);
      delay(50);
      servo.write(180);
      delay(150);
      servo.write(0);
      drop++;
      break;
    }
    }
  }
  stop_motion();
}

void loop() {
  servo.write(0);
  analogWrite(enA, 128);  // Motor A: 50% duty cycle
  analogWrite(enB, 128);  // Motor B: 50% duty cycle

  if (Serial.available()>0) {
    recieved = Serial.read();
    int dist;
    Serial.print(recieved);
    if (recieved == 'd') {
      Serial.print("\nDrop count: ");
      Serial.print(drop);
      Serial.print('\n');
    }
    if (recieved == 'o') {
      Serial.print("\ncollision count: ");
      Serial.print(obj);
      Serial.print('\n');
    }
    if (recieved == 'f') {
      dist = ultrasonicsensor();
      if(dist < 20){
        stop_motion();
        obj++;
      }
      else{
          move_forward();
      }
      recieved = 's';
    }
    if (recieved == 'b') {
      Serial.print('0');
      move_backward();
      recieved = 's';
    }
    if (recieved == 'l') {
      forwardright();
      recieved = 's';
    }
    if (recieved == 'r') {
      forwardleft();
      recieved = 's';
    }
    if (recieved == 's') {
      stop_motion();
      recieved = 's';
    }
    if (recieved == 'c'){
      collision();
      recieved = 'c';
      }
    if (recieved == 'i'){
    line_follow();
    }
    if (recieved == 'u'){
      servo.write(0);
      delay(50);
      servo.write(180);
      delay(150);
      servo.write(0);
      drop++;
    }
  }
}