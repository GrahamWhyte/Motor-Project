#include <TimerOne.h>

// Direction Pins
#define WhiteDirecPin   A1
#define WhiteDirecPin2  A2
#define RedDirecPin     A3
#define RedDirecPin2    A4

// Assorted Constants
#define INTERRUPT_TO_RAD 0.0628
#define SUPPLY_VOLTAGE 8
#define ANGLE_MAX 0.6
#define ANGLE_MIN -0.6

/*************************
        White motor
**************************/

// Digital Pins
int white_motorPin = 9;
int white_encoderPin = 3;
int white_encoderPin2 = 10;

// Initial displacement
int white_displacement = 0;

// PID values
double white_errorTotal = 0; 
double white_lastError = 0; 
int white_lastTime = 0; 
double white_lastSetpoint = 0; 

double input_white = 0, output_white = 0, setpoint_white = 0;

double kp_white = 1.4;
double ki_white = 0.0005; 
double kd_white = 0.00001; 

/*************************
        Red motor
**************************/

// Digital Pins
int red_motorPin = 7;
int red_encoderPin = 2;
int red_encoderPin2 = 8;

// Initial displacement
int red_displacement = 0;

// PID values
double red_errorTotal = 0; 
double red_lastError = 0; 
int red_lastTime = 0; 
double red_lastSetpoint = 0; 

double input_red= 0, output_red = 0, setpoint_red= 0;

double kp_red = 0.5;
double ki_red = 0; 
double kd_red = 0; 


/*************************
* Setup
* 
* Initializes Timer1, Configures all pins, Attaches interrupts to encoder pins, initializes setpoints
* 
**************************/
void setup() {

  Serial.begin(9600);

  // Configure Timer 1
  Timer1.initialize(100000);             // Interrupt time in us
  Timer1.attachInterrupt(timer_one_ISR);

  // FOR TESTING: initial setpoint
  setpoint_white = ANGLE_MAX;
  setpoint_red = ANGLE_MAX; 

  /*************************
          White motor
  **************************/
  // Configure digital pins

  pinMode(white_motorPin, OUTPUT);
  pinMode(white_encoderPin, INPUT);
  pinMode(white_encoderPin2, INPUT);
  pinMode(WhiteDirecPin, OUTPUT);
  pinMode(WhiteDirecPin, OUTPUT);

  // Configure ISR for white encoder
  attachInterrupt(digitalPinToInterrupt(white_encoderPin), white_encoder_ISR, FALLING);

  /*************************
          Red motor
  **************************/
  // Configure digital pins
  pinMode(red_motorPin, OUTPUT);
  pinMode(red_encoderPin, INPUT);
  pinMode(red_encoderPin2, INPUT);
  pinMode(RedDirecPin, OUTPUT);
  pinMode(RedDirecPin, OUTPUT);

  // Configure ISR for red encoder
  attachInterrupt(digitalPinToInterrupt(red_encoderPin), red_encoder_ISR, FALLING);
}

/********************************
* loop
* 
* Main loop of code, calls compute_PID and PWM_out functions 
* All plotting on the serial monitor should go here 
* 
********************************/
void loop() {

  compute_PID_white_motor(); 
  PWM_out(output_white, WhiteDirecPin, WhiteDirecPin2, white_motorPin); 

  compute_PID_red_motor(); 
  PWM_out(output_red, RedDirecPin, RedDirecPin2, red_motorPin); 
  
}

/*************************
* PWM_out
* Inputs: PID value of desired motor, pins related to desired motor
* Outputs: None
* 
* Converts output of PID controller to direction and power for motor. Both motors use this function
* 
***************************/
void PWM_out(double pid_out, int direcPin, int direc2Pin, int motorPin) {

  // Determine direction
  if (pid_out < 0) {
    analogWrite(direcPin, 0);
    analogWrite(direc2Pin, 255);
  }
  else
  {
    analogWrite(direcPin, 255);
    analogWrite(direc2Pin, 0);
  }
  
  // Convert voltage to PWM signal
  int pwm_val = round((abs(pid_out) / SUPPLY_VOLTAGE) * 255);
  analogWrite(motorPin, pwm_val);
}

/**************************
* white_encoder_ISR
* 
* Interrupt Service Routine that triggers on every falling edge of the white motor
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
* 
******************************/
void white_encoder_ISR() {

  // If the other signal is high on the falling edge of the first signal, we are going forwards
  if (digitalRead(white_encoderPin2)) {
    white_displacement++;
  }

  // Otherwise, we go backwards
  else {
    white_displacement--;
  }

  // Convert the displacement (in interrupts) to radians
  input_white = white_displacement * INTERRUPT_TO_RAD;
}


/**************************
* red_encoder_ISR
* 
* Interrupt Service Routine that triggers on every falling edge of the red motor
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
* 
**************************/
void red_encoder_ISR() {

  // If the other signal is high on the falling edge of the first signal, we are going forwards
  if (digitalRead(red_encoderPin2)) {
    red_displacement++;
  }

  // Otherwise, we go backwards
  else {
    red_displacement--;
  }

  // Convert the displacement (in interrupts) to radians
  input_red = red_displacement * INTERRUPT_TO_RAD;
}


/****************************
* timer_one_ISR
* 
* Interrupt services routine that triggers every time timer 1 ticks 
* Inverts setpoint to draw a line
* 
*****************************/
void timer_one_ISR() {
  setpoint_white = -setpoint_white;
  setpoint_red = -setpoint_red;
}

/*****************************
* compute_PID_white_motor
* 
* Computes error, total error, and change in error since last reading for the white motor
* Scales error by PID constants 
* 
******************************/
void compute_PID_white_motor() {

  double error, derivativeError, errorChange; 
  int currentTime = micros(); 
  int timeChange = currentTime - white_lastTime; 

  error = setpoint_white - input_white;          // Calculate error

  if (setpoint_white != white_lastSetpoint)
    derivativeError = -input_white; 
  else
    derivativeError = error;          

  errorChange = derivativeError - white_lastError;   // Difference between error and lastError for derivative term 

  white_errorTotal += error;                      // Total error for integral term 

  double pid_val = kp_white*error + ki_white*white_errorTotal + kd_white*errorChange/timeChange;       // Calculate PID error

  output_white = constrain(pid_val, -255, 255);                                    //Constrain to useful value

  white_lastError = error; 
  white_lastTime = currentTime; 
  white_lastSetpoint = setpoint_white; 

}

/*****************************
* compute_PID_red_motor
* 
* Computes error, total error, and change in error since last reading for the red motor
* Scales error by PID constants 
* 
*****************************/
void compute_PID_red_motor() {
  
  double error, derivativeError, errorChange;   
  int currentTime = micros(); 
  int timeChange = currentTime - red_lastTime; 

  error = setpoint_red - input_red;          // Calculate error

  if (setpoint_red != red_lastSetpoint)
    derivativeError = -input_red; 
  else
    derivativeError = error;          

  errorChange = derivativeError - red_lastError;   // Difference between error and lastError for derivative term 

  red_errorTotal += error;                      // Total error for integral term 

  double pid_val = kp_red*error + ki_red*red_errorTotal + kd_red*errorChange/timeChange;       // Calculate PID error

  output_red = constrain(pid_val, -255, 255);                                    //Constrain to useful value

  red_lastError = error; 
  red_lastTime = currentTime; 
  red_lastSetpoint = setpoint_red; 
}


