#include <MsTimer2.h>
#include <TimerOne.h>
#include <PID_v1.h>
#define direcPin A0
#define direc2Pin A2 

#define MOTOR_THRESHOLD 0.1

double displacement, output, setpoint;
//double kp=1.57, ki=16.8, kd=0.028; 

// PID Constants
double kp = 1.57; 
double ki = 16.8; 
double kd = 0.028; 

// Digital Pins
int motorPin = 9;
int encoderPin = 3; 
int encoder2Pin = 10; 

double test = 0; 

//double displacement = 0; 

double setpointArray[8] = {30000, 20000, 40000, 1300, 23000, 10000, 3000, 12000}; 
int setpointIndex = 0; 

PID testPID(&displacement, &output, &setpoint, kp, ki, kd, DIRECT);

void setup() {

  Serial.begin(9600); 

  // Configure digital pins
  pinMode(encoderPin, INPUT); 
  pinMode(motorPin, OUTPUT); 
  pinMode(encoder2Pin, INPUT); 
  pinMode(direcPin, OUTPUT);
  pinMode(direc2Pin, OUTPUT);

  // Set PID mode
  testPID.SetMode(AUTOMATIC); 

  // Configure ISR for encoder 
  attachInterrupt(digitalPinToInterrupt(encoderPin), encoder_ISR, FALLING); 

  // Configure Timer 1 
  Timer1.initialize(10000); 
  Timer1.attachInterrupt(timer_one_ISR); // Update every 1us              

  // Configure Timer 2
  MsTimer2::set(10000, timer_two_ISR); // 10s period
  MsTimer2::start();

  // FOR TESTING: initial setpoint
  setpoint = 10000; 
  
//  delay(1000); 
}

void loop() {
  
//  testPID.Compute();
//  
//  // If we've gone too far:
//  if (displacement>=setpoint){
//    digitalWrite(direcPin, LOW); 
//    digitalWrite(direc2Pin, HIGH); 
//  }
//
//  // If we haven't gone far enough, go forward
//  else if (displacement < setpoint){
//    digitalWrite(direcPin, HIGH); 
//    digitalWrite(direc2Pin, LOW);  
//  }

//  analogWrite(motorPin, output); 
  analogWrite(motorPin, 255); 
  Serial.print(displacement); 
  Serial.print(", "); 
  Serial.println(setpoint); 
//  Serial.println(test); 
  
}

void encoder_ISR(){

  // If the other signal is high on the falling edge of the first signal, we are going forwards
  if (digitalRead(encoder2Pin)){
    displacement++; 
  }

  // Otherwise, we go backwards
  else {
    displacement--;
  }
  
}

// Re-compute PID values
void timer_one_ISR(){
  
  testPID.Compute();
  
  // If we've gone too far:
  if (displacement>setpoint){
    digitalWrite(direcPin, LOW); 
    digitalWrite(direc2Pin, HIGH); 
  }

  // If we haven't gone far enough, go forward
  else {
    digitalWrite(direcPin, HIGH); 
    digitalWrite(direc2Pin, LOW);  
  }
   

}

void timer_two_ISR() {
  setpoint = setpointArray[setpointIndex]; 
  if (setpointIndex < 8)
    setpointIndex++; 
  else
    setpointIndex = 0; 
}


