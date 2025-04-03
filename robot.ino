// robot code
#include <TM1637Display.h>

#define CLK 2  // Clock pin
#define DIO 4  // Data pin



#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
#define PLAYBACK_PIN 3  // Pin connected to the playback module
#define TRIG_PIN A5  // Pin connected to the ultrasonic sensor's trig
#define ECHO_PIN A4  // Pin connected to the ultrasonic sensor's echo

unsigned char carSpeed = 90;
unsigned char carSpeed1 = 136;
bool isPoweredOn = true;  // State variable to check if the robot should be running
const int thresholdDistance = 20;  // Distance threshold to detect obstacles
unsigned long startTime;
unsigned long runTime = 4000;  // Time to move forward (in milliseconds)
unsigned long stopTime = 0;  // Time spent stopped due to obstacle
unsigned long remainingTime = 0;  // Remaining time after obstacle
char t ; //for bluuettoth controll


TM1637Display display(CLK, DIO);

long readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}


void forward() { 
  if (!isPoweredOn) return;
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  //Serial.println("Forward");
}
void back() {
  if (!isPoweredOn) return;
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  //Serial.println("Back");
}

void left() {
  if (!isPoweredOn) return;
  analogWrite(ENA, carSpeed1);
  analogWrite(ENB, carSpeed1);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); 
  //Serial.println("Left");
}

void right() {
  if (!isPoweredOn) return;
  analogWrite(ENA, carSpeed1);
  analogWrite(ENB, carSpeed1);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  //Serial.println("Right");
}

void stop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Stop!");
}

void powerOff() {
  stop();
  isPoweredOn = false;
  Serial.println("Power Off");
}

void setup() {
  Serial.begin(9600);
  // Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(PLAYBACK_PIN, OUTPUT);  // Set playback module pin as output
  pinMode(TRIG_PIN, OUTPUT);      // Set TRIG pin as output
  pinMode(ECHO_PIN, INPUT);       // Set ECHO pin as input
  display.setBrightness(0x0f);
  stop();
  display.showNumberDec(0000);
  delay(7000);
}

void announceMessage() {
  if (true) {
    digitalWrite(PLAYBACK_PIN, HIGH);  // Trigger the playback module
    delay(1000);  // Duration to ensure the message is played
    digitalWrite(PLAYBACK_PIN, LOW);   // Stop the playback
    Serial.println("Buzzer");
    // messageAnnounced = true;  // Ensure the message is only announced once
  }
}

void moveWithObstacleCheck(void (*movementFunc)(), unsigned long duration) {
  startTime = millis();
  unsigned long elapsedTime = 0;
  unsigned long totalElapsedTime = 0;
  bool obstacleDetected = false;

  while (totalElapsedTime < duration) {
    long distance = readUltrasonicDistance();

    if (distance < thresholdDistance) {
      stop();
      Serial.println("Stopped! Obstacle detected within 20 cm.");
      // add beep sound
      elapsedTime = millis() - startTime;
      totalElapsedTime += elapsedTime;
      unsigned long remainingTime = duration - totalElapsedTime;
      Serial.print("Remaining time: ");
      Serial.print(remainingTime);
      Serial.println(" milliseconds.");
      obstacleDetected = true;

      while (distance < thresholdDistance) {
        // add beep sound
        announceMessage();
        distance = readUltrasonicDistance();
        delay(10);
      }

      obstacleDetected = false;
      startTime = millis(); // Reset start time after obstacle is cleared
    }

    if (!obstacleDetected) {
      movementFunc();
      elapsedTime = millis() - startTime;
      totalElapsedTime += elapsedTime;
      startTime = millis(); // Reset start time after each iteration
    }
  }
  stop();
}

void loop() {
  int distance = readUltrasonicDistance();
  if (isPoweredOn) {
    if (distance < thresholdDistance) {  // Stop if an obstacle is detected within 20 cm
      stop();
      while (distance < thresholdDistance) {
        announceMessage();         // add beep sound
        distance = readUltrasonicDistance();
        delay(10);
      }
    }
    else {
      // bluetooth controlled directions //yellow towards adapter
      if (Serial.available() ) {
        while(true){
          t = Serial.read();
          delay(1000);
          if(t == '1'|| t == '2'|| t == '3'|| t == '4'|| t == '5'|| t == '6'){
              break;
          }
        }
        // Serial.println("yes");
      }
      if (t == '1') {
        t = '0';
        display.showNumberDec(0001);
        moveWithObstacleCheck(forward, 1000);
        stop();
        // Send '1' to the receiver Arduino
        Serial.println("1");
        delay(1000);
        Serial.println("1");
        delay(1000);
        //Wait for "door closed" message from the receiver Arduino
        while (true) {
          if (Serial.available() > 0) {
            String message = Serial.readStringUntil('\n');
            message = "hi";
            Serial.println("1");
            Serial.println("no" + t);
            Serial.println(message);
            if (message == "door closed") {
              break;
            }
          }
        }
        // Perform additional instructions after receiving "door closed"
        moveWithObstacleCheck(back, 1000);
        stop();
        delay(700);
      }
 
      else if(t == '2'){   
        t = '0';    //move reverse (all motors rotate in reverse direction)
      display.showNumberDec(0002);
      delay(15000); // Wait for 15 seconds
      moveWithObstacleCheck(forward, 1000);  // Move forward for runTime milliseconds
        stop();
        delay(2000);
        moveWithObstacleCheck(right,2000);
        moveWithObstacleCheck(forward, 1000);  // Move forward for runTime milliseconds
        stop();
        delay(7000);
        //reverse instructions
        moveWithObstacleCheck(right, 4000);
        moveWithObstacleCheck(forward, 1000);
        stop();
        delay(2000);
        moveWithObstacleCheck(left,2000);
        stop();
        delay(2000);
        moveWithObstacleCheck(forward, 1000);
      }
 
      else if(t == '3'){   
        t = '0';   //turn right (left side motors rotate in forward direction, right side motors doesn't rotate)
        display.showNumberDec(3);
        delay(1000); // Wait for 1 second
        moveWithObstacleCheck(forward, 1000);  // Move forward for runTime milliseconds
        stop();
        delay(100);
        
      }
 
      else if(t == '4'){      //turn left (right side motors rotate in forward direction, left side motors doesn't rotate)
      t = '0'; 
      display.showNumberDec(4);
        delay(1000); // Wait for 1 second
        moveWithObstacleCheck(back, 1000);  // Move forward for runTime milliseconds
        stop();
        delay(700);
      }
      
      else if(t == '5'){      //turn left (right side motors rotate in forward direction, left side motors doesn't rotate)
      t = '0'; 
      display.showNumberDec(5);
        delay(1000); // Wait for 1 second
        moveWithObstacleCheck(left, 1000);  // Move forward for runTime milliseconds
        stop();
        delay(700);
      }

      else if(t == '6'){      //turn left (right side motors rotate in forward direction, left side motors doesn't rotate)
      t = '0'; 
      display.showNumberDec(6);
        delay(1000); // Wait for 1 second
        moveWithObstacleCheck(right, 1000);  // Move forward for runTime milliseconds
        stop();
        delay(700);
      }
    }
  }
}