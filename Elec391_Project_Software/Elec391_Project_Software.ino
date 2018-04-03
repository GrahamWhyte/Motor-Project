#include <TimerOne.h>
#include <TimerThree.h>
#include <avr/pgmspace.h>

#define LASER_DIREC_2   A1
#define LASER_DIREC_1   A2
#define MIRROR_DIREC_2  A3
#define MIRROR_DIREC_1  A4

#define INTERRUPT_TO_RAD  0.0628
#define INTERRUPT_TO_DEG  0.9
#define SUPPLY_VOLTAGE    14
#define ANGLE_MAX         1.047197        // Approx. 60 degrees 
#define ANGLE_MIN         -1.047197       // Approx. -60 degrees 

//#define FILTER_COEF         4099
#define FILTER_COEF         4007
#define SETPOINT_ARRAY_SIZE 101
#define TERMINATION_VAL -1000

//const int LASER_ARRAY_SIZE = 27; 
//const int MIRROR_ARRAY_SIZE = 1; 

// Set control frequency 
int controlTime_us = 1000;                  // PID update time in us 
double controlTime_s = controlTime_us/1E6;  // PID update time in s

// Index to keep track of how far through the setpoint array we are 
int setpointIndex = 0; 
bool setpointDir= true; 


// Timer 1 flag
bool t1Flag = false; 

// Arrays of setpoints for each shape. Will be overwritten every time the shape changes 
double setpointArray_laser [SETPOINT_ARRAY_SIZE]; 
double setpointArray_mirror [SETPOINT_ARRAY_SIZE]; 

// Character to recieve from Serial. Will determine which shape to draw
char shapeSignal = ""; 
char circleSignal = 'c'; 
char triangleSignal = 'l'; 
char squareSignal = 's'; 
char lineSignal = 'l'; 

// Setpoint Arrays

// Line
//const double setpoint_laser_line [SETPOINT_ARRAY_SIZE] PROGMEM = {46.8, 43.2, 39.6, 36, 32.4, 28.8, 25.2, 21.6, 18, 14.4, 
//                                                               10.8, 7.2, 3.6, 0, -3.6, -7.2, -10.8, -14.4, -18, -21.6,
//                                                               -25.2, -28.8, -32.4, -36, -39.6, -43.2, -46.8, TERMINATION_VAL}; 
//
//const double setpoint_laser_line [SETPOINT_ARRAY_SIZE] PROGMEM = {25.2, 21.6, 18, 14.4, 
//                                                               10.8, 7.2, 3.6, 0, -3.6, -7.2, -10.8, -14.4, -18, -21.6,
//                                                               -25.2, TERMINATION_VAL}; 
//const double setpoint_mirror_line [SETPOINT_ARRAY_SIZE] PROGMEM = {61.2, 61.2, 61.2, 61.2,
//                                                                  61.2, 61.2, 61.2, 61.2, 61.2, 61.2, 61.2, 61.2, 61.2, 61.2,
//                                                                  61.2, TERMINATION_VAL}; 


const double setpoint_laser_line [SETPOINT_ARRAY_SIZE] PROGMEM = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                                                  0, 0, 0, 0,
                                                                  TERMINATION_VAL}; 
                                                                  
//const double setpoint_mirror_line [SETPOINT_ARRAY_SIZE] PROGMEM = {46.8, 43.2, 39.6, 36, 32.4, 28.8, 25.2, 21.6, 18, 14.4, 
//                                                                   10.8, 7.2, 3.6, 0, -3.6, -7.2, -10.8, -14.4, -18, -21.6,
//                                                                   -25.2, -28.8, -32.4, -36, -39.6, -43.2, -46.8, TERMINATION_VAL}; 

const double setpoint_mirror_line [SETPOINT_ARRAY_SIZE] PROGMEM = {21.6, 25.2, 28.8, 32.4, 36, 39.6, 43.2, 46.8, 50.4, 54, 
                                                                    57.6, 61.2, 64.8, 68.4, 
                                                                    TERMINATION_VAL}; 
                                                                   
//


//const double setpoint_mirror_line[MIRROR_ARRAY_SIZE] PROGMEM = {54}; 
//const double setpoint_mirror_line [27] PROGMEM = {46.8, 54, 54, 54, 54, 54, 54, 54, 54, 54,
//                                                  54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 
//                                                  54, 54, 54}; 


//const double setpoint_laser_line [SETPOINT_ARRAY_SIZE] PROGMEM = {21.6, 25.2, 28.8, 32.4, 36, 39.6, 43.2, 46.8, 50.4, 54, 57.6, 61.2, 64.8, 68.4}; 
//const double setpoint_mirror_line [SETPOINT_ARRAY_SIZE] PROGMEM = {21.6, 25.2, 28.8, 32.4, 36, 39.6, 43.2, 46.8, 50.4, 54, 57.6, 61.2, 64.8, 68.4}; 

//const double setpoint_mirror_line [SETPOINT_ARRAY_SIZE] PROGMEM = {36, 39.6, 43.2, 46.8, 43.2, 39.6}; 

// Square
//const double setpoint_laser_square [SETPOINT_ARRAY_SIZE] PROGMEM = {46.8, 46.8, 46.8, 0, -46.8, -46.8, -46.8, 0, 46.8, TERMINATION_VAL}; 
//
//const double setpoint_mirror_square [SETPOINT_ARRAY_SIZE] PROGMEM = {21.6, 46.8, 68.4, 68.4, 68.4, 46.8, 21.6, 21.6, 21.6, TERMINATION_VAL}; 


const double setpoint_laser_square [SETPOINT_ARRAY_SIZE] PROGMEM = {18,14.4,10.8,7.2,3.6,0,-3.6,-7.2,-10.8,-14.4,-18,-18,
                                                                    -18,-18,-18,-18,-18,-18,-18,-18,-14.4,-10.8,-7.2,-3.6,
                                                                    0,3.6,7.2,10.8,14.4,18,18,18,18,18,18,18,18,18, TERMINATION_VAL}; 
                                                                    
const double setpoint_mirror_square [SETPOINT_ARRAY_SIZE] PROGMEM = {32.4,32.4,32.4,32.4,32.4,32.4,32.4,32.4,32.4,32.4,
                                                                     32.4,32.4,36,39.6,43.2,46.8,50.4,54,57.6,57.6,57.6,
                                                                     57.6,57.6,57.6,57.6,57.6,57.6,57.6,57.6,57.6,57.6,53,
                                                                     50.4,46.8,43.2,39.6,36,32.4, TERMINATION_VAL}; 

//// Triangle
const double setpoint_laser_triangle [SETPOINT_ARRAY_SIZE] PROGMEM = {
                                                                    -27,-26,-25,-24,-23,-22,-21,-20,-19,-18,
                                                                    -17,-16,-15,-14,-13,-12,-11,-10,-8,-7,
                                                                    -6,-5,-4,-3,-2,0,2,3,4,5,
                                                                     6,7,8,10,11,12,13,14,15,16,
                                                                     17,18,19,20,21,22,23,24,25,26,
                                                                     27,26,25,24,23,22,21,20,19,18,
                                                                     17,16,15,14,13,12,11,10,8,7,
                                                                     6,5,4,3,2,-1,-2,-3,-4,-5,
                                                                     -6,-7,-8,-10,-11,-12,-13,-14,-15,-16,
                                                                     -17,-18,-19,-20,-21,-22,-23,-24,-25,-26, TERMINATION_VAL
                                                                   };                                                             

const double setpoint_mirror_triangle [SETPOINT_ARRAY_SIZE] PROGMEM = {
                                                                      46,47,48,49,50,51,52,53,54,55,
                                                                      56,57,58,59,60,61,62,63,63,64,
                                                                      65,66,66,67,67,68,67,67,66,66,
                                                                      65,64,63,63,62,61,60,59,58,57,
                                                                      56,55,54,53,52,51,50,49,48,47,
                                                                      46,46,46,46,46,46,46,46,46,46,
                                                                      46,46,46,46,46,46,46,46,46,46,
                                                                      46,46,46,46,46,46,46,46,46,46,
                                                                      46,46,46,46,46,46,46,46,46,46,
                                                                      46,46,46,46,46,46,46,46,46,46,TERMINATION_VAL
                                                                    }; 



// Circle
const double setpoint_laser_circle [SETPOINT_ARRAY_SIZE] PROGMEM =   {
                                                                    -45,-44,-43,-41,-40,-39,-37,-36,-34,-33,
                                                                    -31,-29,-27,-26,-24,-22,-20,-18,-16,-13,
                                                                    -11,-9,-7,-5,-2,0,2,5,7,9,
                                                                    11,13,16,18,20,22,24,26,27,29,
                                                                    31,33,34,36,37,39,40,41,43,44,
                                                                    45,44,43,41,40,39,37,36,34,33,
                                                                    31,29,27,26,24,22,20,18,16,13,
                                                                    11,9,7,5,2,0,-2,-5,-7,-9,
                                                                    -11,-13,-16,-18,-20,-22,-24,-26,-27,-29,
                                                                    -31,-33,-34,-36,-37,-39,-40,-41,-43,-44, TERMINATION_VAL
                                                                  }; 

const double setpoint_mirror_circle [SETPOINT_ARRAY_SIZE] PROGMEM = {
                                                                  45,53,56,58,59,60,62,62,63,64,
                                                                  64,65,65,66,66,66,67,67,67,67,
                                                                  67,67,67,67,67,68,67,67,67,67,
                                                                  67,67,67,67,67,66,66,66,65,65,
                                                                  64,64,63,62,62,60,59,58,56,53,
                                                                  45,37,34,32,31,30,28,28,27,26,
                                                                  26,25,25,24,24,24,23,23,23,23,
                                                                  23,23,23,23,23,23,23,23,23,23,
                                                                  23,23,23,23,23,24,24,24,25,25,
                                                                  26,26,27,28,28,30,31,32,34,37, TERMINATION_VAL
                                                                }; 

const double setpoint_laser_zero [SETPOINT_ARRAY_SIZE] PROGMEM = {0, TERMINATION_VAL}; 
const double setpoint_mirror_zero [SETPOINT_ARRAY_SIZE] PROGMEM = {0, TERMINATION_VAL}; 

/*************************
        Laser motor
**************************/

// Digital Pins
int motorPin_laser = 9;
int encoderPin_laser = 3;
int encoderPin2_laser = 18;

int homingPin_laser = 20; 

// Initial displacement
int displacement_laser = 0;

// PID values
double errorTotal_laser = 0; 
double lastError_laser = 0; 
double lastSetpoint_laser = 0; 
double lastTime_laser = 0; 

double input_laser = 0, output_laser = 0, setpoint_laser = 0;

double kp_laser = 0.89618;
double ki_laser = 1.0035;
double kd_laser = 0.19066;

//double kp_laser = 0.4;
//double ki_laser = 0.0966;
//double kd_laser = 0.2936;

/* PID constants from Simulink model */
//double kp_laser = 1;
//double ki_laser = 0;
//double kd_laser = 0;

/*************************
        Mirror motor
**************************/

// Digital Pins
int motorPin_mirror = 7;
int encoderPin_mirror = 2;
//int encoderPin2_mirror = 8;
int encoderPin2_mirror = 19;        // Will change on new PCB

int homingPin_mirror = 21; 

// Initial displacement
int displacement_mirror = 0;

// PID values
double errorTotal_mirror = 0, lastError_mirror = 0, lastSetpoint_mirror = 0, lastTime_mirror = 0; 

double input_mirror = 0, output_mirror = 0, setpoint_mirror= 0;

//double kp_mirror = 0.93058;
//double ki_mirror = 0.73872; 
//double kd_mirror = 0.22632; 

//double kp_mirror = 0.71722;
//double ki_mirror = 0.19652; 
//double kd_mirror = 0.2502; 

double kp_mirror = 0.89618;
double ki_mirror = 1.0035;
double kd_mirror = 0.19066;

void setup() {

 // Open serial port
  Serial.begin(9600);
  
  // Configure pins for Laser motor
  pinMode(motorPin_laser, OUTPUT);
  pinMode(encoderPin_laser, INPUT);
  pinMode(encoderPin2_laser, INPUT);
  pinMode(LASER_DIREC_1, OUTPUT);
  pinMode(LASER_DIREC_2, OUTPUT);
  pinMode(homingPin_laser, INPUT); 

  // Configure ISR for mirror motor encoder slots
  attachInterrupt(digitalPinToInterrupt(encoderPin_laser), laser_encoder_ISR_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin2_laser), laser_encoder_ISR_2, CHANGE);
  
  // Configure pins for Mirror motor
  pinMode(motorPin_mirror, OUTPUT);
  pinMode(encoderPin_mirror, INPUT);
  pinMode(encoderPin2_mirror, INPUT);
  pinMode(MIRROR_DIREC_1, OUTPUT);
  pinMode(MIRROR_DIREC_2, OUTPUT);
  pinMode(homingPin_mirror, INPUT); 


//  Serial.println("Homing Begin"); 
//  homing(); 
//  Serial.println("Homing Done"); 
//  

  // Configure ISR for mirror motor encoder slots
  attachInterrupt(digitalPinToInterrupt(encoderPin_mirror), mirror_encoder_ISR_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin2_mirror), mirror_encoder_ISR_2, CHANGE);
  
  // Configure Timer 1. PID values will be updated every time this timer interrupts
  Timer1.initialize(controlTime_us);             // Interrupt time in us
  Timer1.attachInterrupt(update_pid);

  // Configure Timer 3. The setpoint for both motors will update every time this timer interrupts
  Timer3.initialize(convert_to_micro_seconds(0.01));             // Enter time in seconds, will be converted 
  Timer3.attachInterrupt(update_setpoint);


  /*
   * Uncomment this function if we're not sure if the circuit is working. Make sure it is commented out 
   * before actually running the program
  */
//   testMotor(); 
//  
  // Homing


  
  // Initially draw a triangle
  populate_setpoint_arrays(setpoint_laser_square, setpoint_mirror_square); 
//  readShape('t'); 

//  delay(1); 

}

void loop() {
//  if (Serial.available()>0) {
//    shapeSignal = (char)Serial.read();
////    Serial.println(shapeSignal); 
//    readShape(shapeSignal);
//  }
//  Serial.println(shapeSignal); 

  
  /*
   * Debugging serial prints 
   * Probably won't work when GUI is implemented
   */
//  Serial.print(input_laser);
//  Serial.print(","); 
//  Serial.println(setpoint_laser);  
  Serial.print(input_mirror);
  Serial.print(", "); 
  Serial.println(setpoint_mirror);
//  Serial.println(lastTime_laser); 
//
//  sendFloat(setpoint_mirror);  
//  sendFloat(setpoint_laser);
  
//  Serial.println(setpoint_mirror); 


//  if (t1Flag) 
//    update_pid(); 
}

long convert_to_micro_seconds (double value) {
  long output = round(value*1E6); 
  return output; 
}

void homing() {

  // Rotate until slot detector hits gap

  while(digitalRead(homingPin_laser)) {
    analogWrite(motorPin_laser, 78);   
    analogWrite(LASER_DIREC_2, 255);
    analogWrite(LASER_DIREC_1, 0); 
  }
  while(digitalRead(homingPin_laser)) {
    analogWrite(motorPin_laser, 78);   
    analogWrite(LASER_DIREC_2, 255);
    analogWrite(LASER_DIREC_1, 0); 
  }
  displacement_laser = 0;
  analogWrite(motorPin_laser, 0);
  Serial.println("Laser Motor Done"); 

  while(digitalRead(homingPin_mirror)) {
    analogWrite(motorPin_mirror, 40);   
    analogWrite(MIRROR_DIREC_2, 255);
    analogWrite(MIRROR_DIREC_1, 0); 
  }
  analogWrite(motorPin_mirror, 0);


  // Zero displacement at homing position for both motors

  displacement_mirror = 0; 
}

void populate_setpoint_arrays (double *laser_shape, double *mirror_shape) {

  for (int i = 0; i < SETPOINT_ARRAY_SIZE; i++) {
    setpointArray_laser[i] = pgm_read_float(&laser_shape[i]);
    setpointArray_mirror[i] = pgm_read_float(&mirror_shape[i]);
  }  
}


void readShape(char shape) {
  double *laser_array;
  double *mirror_array;  

//  shape = 'c'; 

//  if (shape != 's') {
//    laser_array = setpoint_laser_circle; 
//    mirror_array = setpoint_mirror_circle; 
//  }
//
//  else 
//  {
//    laser_array = setpoint_laser_square; 
//    mirror_array = setpoint_mirror_square; 
//  }
//    

  switch (shape) {
    
    case 'c': 
      laser_array = setpoint_laser_circle; 
      mirror_array = setpoint_mirror_circle; 
      break; 

    case 's':
      laser_array = setpoint_laser_square; 
      mirror_array = setpoint_mirror_square; 
      break; 

    case 't':
      laser_array = setpoint_laser_triangle; 
      mirror_array = setpoint_mirror_triangle; 
      break; 

    case 'l':
      laser_array = setpoint_laser_line; 
      mirror_array = setpoint_mirror_line; 
      break;

    // Make sure nothing weird happens if an unexpected signal comes through
    default: 
      laser_array = laser_array; 
      mirror_array = mirror_array; 
      break; 
  }

  populate_setpoint_arrays(laser_array, mirror_array); 
}


// Test to make sure current driver circuit is working
// Should not be called during normal operation
void testMotor () {
  while (true) {
    analogWrite(motorPin_laser, 150);   
    analogWrite(LASER_DIREC_2, 255);
    analogWrite(LASER_DIREC_1, 0); 
    analogWrite(motorPin_mirror, 150); 
    analogWrite(MIRROR_DIREC_2, 255);
    analogWrite(MIRROR_DIREC_1, 0); 
  }
}

void sendFloat(double val) {
  byte *bytes = (byte *)&val; 
  Serial.write(bytes, sizeof(val));
}


