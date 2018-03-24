#include <TimerOne.h>
#include <TimerThree.h>
#include <avr/pgmspace.h>

#define LASER_DIREC_2   A1
#define LASER_DIREC_1   A2
#define MIRROR_DIREC_2  A3
#define MIRROR_DIREC_1  A4

#define INTERRUPT_TO_RAD  0.0628
#define DEG_TO_RAD        0.01745329
#define SUPPLY_VOLTAGE    12
#define ANGLE_MAX         1.047197        // Approx. 60 degrees 
#define ANGLE_MIN         -1.047197       // Approx. -60 degrees 

#define FILTER_COEF         100 
#define SETPOINT_ARRAY_SIZE 101

// Set control frequency 
int controlTime_us = 1000;                  // PID update time in us 
double controlTime_s = controlTime_us/1E6;  // PID update time in s

// Index to keep track of how far through the setpoint array we are 
int setpointIndex = 0; 

// Arrays of setpoints for each shape. Will be overwritten every time the shape changes 
//int setpointArray_laser [SETPOINT_ARRAY_SIZE]; 
int setpointArray_mirror [SETPOINT_ARRAY_SIZE]; 
int *setpointArray_laser; 

// Setpoint Arrays

// Triangle
const int setpoint_laser_triangle [SETPOINT_ARRAY_SIZE] PROGMEM = {-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,
                                                                  -17,-16,-15,-14,-13,-12,-11,-10,-8,-7,
                                                                  -6,-5,-4,-3,-2,0,2,3,4,5,
                                                                   6,7,8,10,11,12,13,14,15,16,
                                                                   17,18,19,20,21,22,23,24,25,26,
                                                                   27,26,25,24,23,22,21,20,19,18,
                                                                   17,16,15,14,13,12,11,10,8,7,
                                                                   6,5,4,3,2,-1,-2,-3,-4,-5,
                                                                   -6,-7,-8,-10,-11,-12,-13,-14,-15,-16,
                                                                   -17,-18,-19,-20,-21,-22,-23,-24,-25,-26,-27}; 

const int setpoint_mirror_triangle [SETPOINT_ARRAY_SIZE] PROGMEM = {46,47,48,49,50,51,52,53,54,55,
                                                                    56,57,58,59,60,61,62,63,63,64,
                                                                    65,66,66,67,67,68,67,67,66,66,
                                                                    65,64,63,63,62,61,60,59,58,57,
                                                                    56,55,54,53,52,51,50,49,48,47,
                                                                    46,46,46,46,46,46,46,46,46,46,
                                                                    46,46,46,46,46,46,46,46,46,46,
                                                                    46,46,46,46,46,46,46,46,46,46,
                                                                    46,46,46,46,46,46,46,46,46,46,
                                                                    46,46,46,46,46,46,46,46,46,46,46}; 
/*************************
        Laser motor
**************************/

// Digital Pins
int motorPin_laser = 9;
int encoderPin_laser = 3;
int encoderPin2_laser = 18;

// Initial displacement
int displacement_laser = 0;

// PID values
double errorTotal_laser = 0; 
double lastError_laser = 0; 
double lastSetpoint_laser = 0; 

double input_laser = 0, output_laser = 0, setpoint_laser = 0;

double kp_laser = 1;
double ki_laser = 2.5;
double kd_laser = 0.10;

/* PID constants from Simulink model */
//double kp_laser = 0.282;
//double ki_laser = 0.94;
//double kd_laser = 0.0188;

/*************************
        Mirror motor
**************************/

// Digital Pins
int motorPin_mirror = 7;
int encoderPin_mirror = 2;
int encoderPin2_mirror = 8;
//int encoderPin2_mirror = 19;        // Will change on new PCB

// Initial displacement
int displacement_mirror = 0;

// PID values
double errorTotal_mirror = 0, lastError_mirror = 0, lastSetpoint_mirror = 0; 

double input_mirror = 0, output_mirror = 0, setpoint_mirror= 0;

// PID constants have not been found yet. Maybe if Jake and Eric BUILD THE FUCKING MOTOR
double kp_mirror = 1;
double ki_mirror = 0; 
double kd_mirror = 0; 

void setup() {

  // Open serial port
  Serial.begin(9600);

  // Test: Read Triangle Data
//  populate_setpoint_arrays(); 

  setpointArray_laser = setpoint_laser_triangle; 

  // Configure Timer 1. PID values will be updated every time this timer interrupts
  Timer1.initialize(controlTime_us);             // Interrupt time in us
  Timer1.attachInterrupt(update_pid);

  // Configure Timer 3. The setpoint for both motors will update every time this timer interrupts
  Timer3.initialize(convert_to_micro_seconds(1));             // Enter time in seconds, will be converted 
  Timer3.attachInterrupt(update_setpoint);

}

void populate_setpoint_arrays () {

  for (int i = 0; i < SETPOINT_ARRAY_SIZE; i++) {
    setpointArray_laser[i] = pgm_read_word(&setpoint_laser_triangle[i]);
    setpointArray_mirror[i] = pgm_read_word(&setpoint_mirror_triangle[i]);
  }
    
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(setpoint_laser);
  Serial.print(","); 
  Serial.println(setpoint_mirror);  

}

int convert_to_micro_seconds (double value) {
  return (int)value*1E6; 
}


