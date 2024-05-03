#include <Servo.h>

// Define servo objects
Servo servo;

// Ultrasonic Module pins
const int trigPin = 13;  // Trig pin of ultrasonic sensor
const int echoPin = 4;   // Echo pin of ultrasonic sensor
// Servo motor that aims ultrasonic sensor
const int right_sensor = 2;  // Right IR sensor pin
const int left_sensor = 3;   // Left IR sensor pin

// Motor A, Arduino and L298N connections
int enA = 5;   // ENABLE pin for Motor A to provide PWM
int in1 = 11;  // IN1 pin for Motor A for direction
int in2 = 12;  // IN2 pin for Motor A for direction

// Motor B, Arduino and L298N connections
int enB = 6;  // ENABLE pin for Motor B to provide PWM
int in3 = 7;   // IN3 pin for Motor B for direction
int in4 = 8;   // IN4 pin for Motor B for direction

const int BT_RX = 0;  // Bluetooth RX pin
const int BT_TX = 1;  // Bluetooth TX pin

char recieved = " ";  // Variable to store received character
int p;  // Unused variable
int dis;  // Distance variable for ultrasonic sensor
long duration;  // Duration variable for ultrasonic sensor
int drop = 0;  // Variable to store drop count
int obj = 0;   // Variable to store collision count

void setup() {
  Serial.begin(9600);  // Serial initializing for Debugging

  // Specify pinModes for enA, in1, in2, enB, in3, and in4, Bluetooth, IR sensors, Ultrasonic sensor
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

  servo.attach(9);  // Attach servo to pin 9
  // Add a short delay to stabilize the motor driver
  delay(100);
}

// Function to handle collision avoidance
void collision(){ 
  int diss;
  diss = ultrasonicsensor();  // Read distance from ultrasonic sensor
  if (diss < 20){  // If obstacle detected within 20cm
    stop_motion();  // Stop the robot
    delay(200);
    move_backward();  // Move backward
    forwardright();  // Slightly turn right
    diss = ultrasonicsensor();  // Read distance again
    if (diss < 20){  // If obstacle still detected
      stop_motion();  // Stop the robot
      obj++;  // Increment collision count
      delay(200);
      move_backward();  // Move backward
      forwardleft();  // Turn left
      forwardleft();  // Turn left again to avoid obstacle
    }
    else{
      move_forward();  // If no obstacle, move forward
    }
  }
  else{
    move_forward();  // If no obstacle, move forward
  }
}

// Function to move backward
void move_backward() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);  // Motor A: Clockwise
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);   // Motor B: Clockwise
  analogWrite(enA, 220);    // Motor A: 50% duty cycle
  analogWrite(enB, 128);    // Motor B: 50% duty cycle
  delay(200);  // Delay for stability
}

// Function to move forward
void move_forward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  analogWrite(enA, 220);    // Motor A: 50% duty cycle
  analogWrite(enB, 128);    // Motor B: 50% duty cycle
  delay(200);  // Delay for stability
}

// Function to move forward and slightly turn right
void forwardright() {
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  analogWrite(enA, 220);    // Motor A: 50% duty cycle
  analogWrite(enB, 0);
  delay(200);  // Delay for stability
}

// Function to move forward and slightly turn left
void forwardleft() {
  digitalWrite(in1, HIGH);
  digitalWrite(in4, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  analogWrite(enB, 128);
  analogWrite(enA, 0);      // Motor A: 50% duty cycle
  delay(200);  // Delay for stability
}

// Function to stop motion
void stop_motion() {
  digitalWrite(in1, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  analogWrite(enA, 0);      // Motor A: 50% duty cycle
  analogWrite(enB, 0);      // Motor B: 50% duty cycle
  delay(100);  // Delay for stability
}

// Function to read distance from ultrasonic sensor
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

// Function to follow line
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

  // Check if serial data is available
  if (Serial.available()>0) {
    recieved = Serial.read();  // Read the character received
    int dist;
    Serial.print(recieved);  // Print the received character
    // Execute corresponding action based on received character
    if (recieved == 'd') {  // If 'd' received, print drop count
      Serial.print("\nDrop count: ");
      Serial.print(drop);
      Serial.print('\n');
    }
    if (recieved == 'o') {  // If 'o' received, print collision count
      Serial.print("\ncollision count: ");
      Serial.print(obj);
      Serial.print('\n');
    }
    if (recieved == 'f') {  // If 'f' received, move forward
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
    if (recieved == 'b') {  // If 'b' received, move backward
      Serial.print('0');
      move_backward();
      recieved = 's';
    }
    if (recieved == 'l') {  // If 'l' received, move forward and slightly turn right
      forwardright();
      recieved = 's';
    }
    if (recieved == 'r') {  // If 'r' received, move forward and slightly turn left
      forwardleft();
      recieved = 's';
    }
    if (recieved == 's') {  // If 's' received, stop motion
      stop_motion();
      recieved = 's';
    }
    if (recieved == 'c'){  // If 'c' received, perform collision avoidance
      collision();
      recieved = 'c';
    }
    if (recieved == 'i'){  // If 'i' received, perform line following
      line_follow();
    }
    if (recieved == 'u'){  // If 'u' received, perform dropping action
      servo.write(0);
      delay(50);
      servo.write(180);
      delay(150);
      servo.write(0);
      drop++;
    }
  }
}
