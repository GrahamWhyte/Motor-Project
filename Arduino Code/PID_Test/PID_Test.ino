#include <MsTimer2.h>
#include <TimerOne.h>
#include <PID_v1.h>
#define direcPinWhite A1
#define direc2PinWhite A2 
#define direcPinRed A3 
#define direc2PinRed A4

#define direcPin A1 
#define direc2Pin A2

#define INTERRUPT_TO_RAD 0.0628
#define SUPPLY_VOLTAGE 10

int motorPin = 9; 
double input=0, output=0, setpoint=0;
//double kp=1.57, ki=16.8, kd=0.028; 

// PID Constants
// White Motor With Bracket, Encoder, and Laser Diode
//double kp = 0.1656; 
//double ki = 2.76; 
//double kd = 0.0026; 

// Playing with things
//double kp = 1.5; 
//double ki = 3.5; 
//double kd = 0.0026; 


//double kp = 3.3; 
//double ki = 20.62; 
//double kd = 0.165; 

// Defaults 
//double kp = 1; 
//double ki = 0; 
//double kd = 1; 

// Red Motor, Auto-tuned constants
//double kp=1.57, ki=16.8, kd=0.028; 

/*************************
        White motor
**************************/

// Digital Pins
int white_motorPin = 9;
int white_encoderPin = 3; 
int white_encoder2Pin = 10; 

// Initial displacement
int white_displacement = 0;

// PID values
double input_white = 0, output_white = 0, setpoint_white = 0; 
double kp_white = 1;
double ki_white = 3.5;
double kd_white = 0.0026;

// PID Object
PID WhiteMotor(&input_white, &output_white, &setpoint, kp_white, ki_white, kd_white, DIRECT);

// Red Motor

/*************************
        Red motor
**************************/
// Digital Pins
int red_motorPin = 8; 
int red_encoderPin = 2; 
int red_encoder2Pin = 11; 

// Initial Displacement
int red_displacement = 0;

// PID Values
double input_red=0, output_red=0, setpoint_red=0; 
double kp_red = 1;
double ki_red = 3.5;
double kd_red = 0.0026;

// PID Object
PID RedMotor(&input_red, &output_red, &setpoint, kp_red, ki_red, kd_red, DIRECT);


void setup() {

  Serial.begin(9600); 

  // Configure Timer 1 
  Timer1.initialize(250000);             // Interrupt time in us
  Timer1.attachInterrupt(timer_one_ISR);           

  // FOR TESTING: initial setpoint
  setpoint = -1.57; 



  /*************************
          White motor
  **************************/
  // Configure digital pins
  
  pinMode(white_encoderPin, INPUT); 
  pinMode(white_motorPin, OUTPUT);
  pinMode(white_encoder2Pin, INPUT); 
  pinMode(direcPinWhite, OUTPUT);
  pinMode(direc2PinWhite, OUTPUT);

  // Turn PID on
  WhiteMotor.SetMode(AUTOMATIC); 

  // Update every ms
  WhiteMotor.SetSampleTime(1); 

  // Fix output between -255 and 255
  WhiteMotor.SetOutputLimits(-255, 255); 

  // Configure ISR for encoder 
  attachInterrupt(digitalPinToInterrupt(white_encoderPin), white_encoder_ISR, FALLING); 
  
  
  
  /*************************
          Red motor
  **************************/

  // Configure digital pins
  pinMode(red_encoderPin, INPUT); 
  pinMode(red_motorPin, OUTPUT); 
  pinMode(red_encoder2Pin, INPUT);
  pinMode(direcPinRed, OUTPUT);
  pinMode(direc2PinRed, OUTPUT);

  // Turn PID On
  RedMotor.SetMode(AUTOMATIC); 

  // Update Every 1 ms
  RedMotor.SetSampleTime(1); 

  // Update every ms
  RedMotor.SetOutputLimits(-255, 255); 

  // Configure ISR for encoder
  attachInterrupt(digitalPinToInterrupt(red_encoderPin), red_encoder_ISR, FALLING); 
}

void loop() {

  WhiteMotor.Compute();
  PWM_out(output_white, direcPinWhite, direc2PinWhite, white_motorPin); 
  
  RedMotor.Compute();
  PWM_out(output_red, direcPinRed, direc2PinRed, red_motorPin); 
//
  Serial.print(input_white); 
  Serial.print(", "); 
  Serial.print(input_red); 
  Serial.print(", "); 
  Serial.println(setpoint);

//  Serial.println(test); 

//  Serial.println(output);
  
}

void PWM_out(double pid_out, int direcPin, int direc2Pin, int motorPin){
  
  if (pid_out < 0){
    digitalWrite(direcPin, LOW); 
    digitalWrite(direc2Pin, HIGH); 
  }
  else
  {
    digitalWrite(direcPin, HIGH); 
    digitalWrite(direc2Pin, LOW);
  }

  // Convert voltage to PWM signal
  int pwm_val = round((abs(pid_out)/SUPPLY_VOLTAGE)*255); 
  
  analogWrite(motorPin, pwm_val); 
//    analogWrite(motorPin, output); 
}


void white_encoder_ISR(){

  // If the other signal is high on the falling edge of the first signal, we are going forwards
  if (digitalRead(white_encoder2Pin)){
    white_displacement++; 
  }

  // Otherwise, we go backwards
  else {
    white_displacement--;
  }

  input_white = white_displacement * INTERRUPT_TO_RAD; 
}

void red_encoder_ISR(){

  // If the other signal is high on the falling edge of the first signal, we are going forwards
  if (digitalRead(red_encoder2Pin)){
    red_displacement++; 
  }

  // Otherwise, we go backwards
  else {
    red_displacement--;
  }

  input_red = red_displacement * INTERRUPT_TO_RAD; 
}

// Change target values
void timer_one_ISR(){
//  setpoint = setpoint + INTERRUPT_TO_RAD*2; 
  setpoint = -setpoint; 
}


