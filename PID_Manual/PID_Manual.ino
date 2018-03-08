#include <MsTimer2.h>
#include <TimerOne.h>
#include <PID_v1.h>
#define direcPinWhite A1
#define direc2PinWhite A2
#define direcPinRed A3
#define direc2PinRed A4

/*
  #define direcPin A1
  #define direc2Pin A2
*/
#define INTERRUPT_TO_RAD 0.0628
#define SUPPLY_VOLTAGE 6
#define ANGLE_MAX 0.5236
#define ANGLE_MIN -0.5236

int motorPin = 9;
double input = 0, output = 0, setpoint = 0;

// PID Values 
double lastError = 0; 
double errorTotal = 0; 
int lastTime = 0; 
double output_pid = 0; 
double error = 0; 
double lastSetpoint = 0; 
double errorChange = 0;

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
//double kp_white = 1.8;
//double ki_white = 3.2;
//double kd_white = 0.0026;

//double kp_white = 37.6;
//
//double kp_white = 0.422;
//double ki_white = 2.53;
//double kd_white = 0.0169;

//double kp_white = 1;
//double ki_white = 3.5;
//double kd_white = 0.0026;

//double kp_white = 0.1;
//double ki_white = 0.003;
//double kd_white = 0.000001;

double kp_white = 0.09;
double ki_white = 0.000001; 
double kd_white = 0.01; 


void setup() {

  Serial.begin(9600);

  // Configure Timer 1
  Timer1.initialize(100000);             // Interrupt time in us

//    Timer1.initialize(250000);             // Interrupt time in us

  //  Timer1.initialize(2500000);
  Timer1.attachInterrupt(timer_one_ISR);

  // FOR TESTING: initial setpoint
  setpoint = ANGLE_MAX;
  //  setpoint = ;



  /*************************
          White motor
  **************************/
  // Configure digital pins

  pinMode(white_encoderPin, INPUT);
  pinMode(white_motorPin, OUTPUT);
  pinMode(white_encoder2Pin, INPUT);
  pinMode(direcPinWhite, OUTPUT);
  pinMode(direc2PinWhite, OUTPUT);

  // Configure ISR for encoder
  attachInterrupt(digitalPinToInterrupt(white_encoderPin), white_encoder_ISR, FALLING);
}

void loop() {

//  Serial.println(white_displacement); 
//  Serial.println(input_white); 
  compute_PID(); 
  PWM_out(output_pid, direcPinWhite, direc2PinWhite, white_motorPin); 

  //
//
  Serial.print(input_white);
  Serial.print(", ");

  //  Serial.print(input_red);
  //  Serial.print(", ");

  Serial.println(setpoint);

  //  Serial.println(test);

  //  Serial.println(white_displacement);

}

void PWM_out(double pid_out, int direcPin, int direc2Pin, int motorPin) {

  if (pid_out < 0) {
    digitalWrite(direcPin, LOW);
    digitalWrite(direc2Pin, HIGH);
  }
  else
  {
    digitalWrite(direcPin, HIGH);
    digitalWrite(direc2Pin, LOW);
  }

  pid_out = pid_out/INTERRUPT_TO_RAD; 
  
  // Convert voltage to PWM signal
  int pwm_val = round((abs(pid_out) / SUPPLY_VOLTAGE) * 255);
//  Serial.println(pid_out); 
  analogWrite(motorPin, pwm_val);
//  analogWrite(motorPin, pid_out);
}


void white_encoder_ISR() {

  // If the other signal is high on the falling edge of the first signal, we are going forwards
  if (digitalRead(white_encoder2Pin)) {
    white_displacement++;
  }

  // Otherwise, we go backwards
  else {
    white_displacement--;
  }
  
  input_white = white_displacement * INTERRUPT_TO_RAD;
}


// Change target values
void timer_one_ISR() {
  setpoint = -setpoint;
}

void compute_PID() {
  
  int currentTime = micros(); 
  int timeChange = currentTime - lastTime; 
  double derivativeError; 

  error = setpoint - input_white;          // Calculate error

  if (setpoint != lastSetpoint)
    derivativeError = -input_white; 
  else
    derivativeError = error;          

  errorChange = derivativeError - lastError;   // Difference between error and lastError for derivative term 

  errorTotal += error;                      // Total error for integral term 

  double pid_val = kp_white*error + ki_white*errorTotal + kd_white*errorChange/timeChange;       // Calculate PID error

  output_pid = constrain(pid_val, -255, 255);                                    //Constrain to useful value

  lastError = error; 
  lastTime = currentTime; 
  lastSetpoint = setpoint; 
}


