#include <TimerOne.h>
#include <TimerThree.h>
#include <avr/pgmspace.h>

#define LASER_DIREC_2   A1
#define LASER_DIREC_1   A2
#define MIRROR_DIREC_2  A3
#define MIRROR_DIREC_1  A4

#define LASERSETSIZE 31
#define MIRRORSETSIZE 12

#define INTERRUPT_TO_DEG  0.9
#define SUPPLY_VOLTAGE    14
#define ANGLE_MAX         1.047197        // Approx. 60 degrees 
#define ANGLE_MIN         -1.047197       // Approx. -60 degrees 

//#define FILTER_COEF         4099
#define FILTER_COEF         4007
#define SETPOINT_ARRAY_SIZE 101
#define TERMINATION_VAL -1000

//Kp Laser Correction
#define  LASER_MOD 0.375

bool laser_direction;

union {
  float floatVal; 
  byte floatBuf[4]; 
} floatData; 

union {
  int intVal; 
  byte intBuf[2]; 
} intData; 

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
volatile double setpointArray_laser [SETPOINT_ARRAY_SIZE]; 
volatile double setpointArray_mirror [SETPOINT_ARRAY_SIZE]; 

// Character to recieve from Serial. Will determine which shape to draw
char shapeSignal = ""; 
char circleSignal = 'c'; 
char triangleSignal = 'l'; 
char squareSignal = 's'; 
char lineSignal = 'l'; 

// Setpoint Arrays
const double setpoint_laser_line [SETPOINT_ARRAY_SIZE] PROGMEM = {25.2, 21.6, 18, 14.4, 
                                                               10.8, 7.2, 3.6, 0, -3.6, -7.2, -10.8, -14.4, -18, -21.6,
                                                               -25.2, TERMINATION_VAL}; 
const double setpoint_mirror_line [SETPOINT_ARRAY_SIZE] PROGMEM = {61.2, 61.2, 61.2, 61.2,
                                                                  61.2, 61.2, 61.2, 61.2, 61.2, 61.2, 61.2, 61.2, 61.2, 61.2,
                                                                  61.2, TERMINATION_VAL};
                                                                  
const double setpoint_laser_circle [SETPOINT_ARRAY_SIZE] PROGMEM = {18,14.4,10.8,7.2,3.6,7.2,10.8,14.4, TERMINATION_VAL}; 
const double setpoint_mirror_circle [SETPOINT_ARRAY_SIZE] PROGMEM = {43.2,46.8,50.4,46.8,43.2,39.6,36,39.6, TERMINATION_VAL};

const double setpoint_laser_square [SETPOINT_ARRAY_SIZE] PROGMEM = {  0,
                                                                    18,18,18,
                                                                       0,
                                                                    -18,-18,-18,
                                                                   TERMINATION_VAL}; 
const double setpoint_mirror_square [SETPOINT_ARRAY_SIZE] PROGMEM = {43.2,
                                                                     43.2,52.4,57.6,
                                                                     57.6,
                                                                     57.6, 52.4,43.2,
                                                                      TERMINATION_VAL};

const double setpoint_laser_triangle [SETPOINT_ARRAY_SIZE] PROGMEM = {0,3.6,7.2,10.8,7.2,3.6,0,-3.6,-7.2,-10.8,-7.2,-3.6,TERMINATION_VAL}; 
const double setpoint_mirror_triangle [SETPOINT_ARRAY_SIZE] PROGMEM = {43.2,43.2,43.2,43.2,46.8,50.4,54,50.4,46.8,43.2,43.2,43.2,TERMINATION_VAL};                                                                                                                     
const double setpoint_laser_zero [SETPOINT_ARRAY_SIZE] PROGMEM = {0, TERMINATION_VAL}; 
const double setpoint_mirror_zero [SETPOINT_ARRAY_SIZE] PROGMEM = {0, TERMINATION_VAL}; 

const double setpoint_laser_list[LASERSETSIZE] = {-54,-50.4,-46.8,-43.2,-39.6,-36,-32.4,-28.8,-25.2,-21.6,-18,-14.4,-10.8,-7.2,-3.6,0,3.6,7.2,10.8,14.4,18,21.6,25.2,28.8,32.4,36,39.6,43.2,46.8,50.4,54};
const double setpoint_mirror_list[MIRRORSETSIZE] = {28.8,32.4,36,39.6,43.2,46.8,50.4,54,57.6,61.2,64.8,68.4};
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


double kp_laser = 0.8618;
double ki_laser = 1.0035;
double kd_laser = 0.19066;


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


double kp_mirror = 1.2;
double ki_mirror = 0.120;
double kd_mirror = 0.161;

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

//
//  Serial.println("Homing Begin"); 
//  homing(); 
//  Serial.println("Homing Done"); 
//  

  // Configure ISR for mirror motor encoder slots
  attachInterrupt(digitalPinToInterrupt(encoderPin_mirror), mirror_encoder_ISR_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin2_mirror), mirror_encoder_ISR_2, CHANGE);
  
  // Configure Timer 1. PID values will be updated every time this timer interrupts
  Timer1.initialize(controlTime_us);             // Interrupt time in us
  Timer1.attachInterrupt(timer_one_ISR);

  // Configure Timer 3. The setpoint for both motors will update every time this timer interrupts
  Timer3.initialize(convert_to_micro_seconds(0.05));             // Enter time in seconds, will be converted 
  Timer3.attachInterrupt(update_setpoint);


  /*
   * Uncomment this function if we're not sure if the circuit is working. Make sure it is commented out 
   * before actually running the program
  */
//   testMotor(); 
//  
  // Homing


  
  // Initially draw a square
  populate_setpoint_arrays(setpoint_laser_line, setpoint_mirror_line); 
//  readShape('t'); 

  // Wait 500ms for shit to get not fucked
  delay(500); 

}



bool commandFlag = false; 
bool lenFlag = false; 
bool dataFlag = false; 

int dataIndex = 0; 
int command; 
int len; 
byte *data; 
byte *buff;

double *laser_array; 
double *mirror_array; 

  
//char recievedChar = 'g'; 
void loop() {
  // Make sure at least 3 bytes are available
  if (Serial.available()>0) {
    readShape(); 
  }

//   if (Serial.available()) {
//      Serial.readBytes(buff, 1); 
//      if (!commandFlag) {
//        command = (int)buff[0];
//        commandFlag = true;
//      } else if (!lenFlag) {
//        len = (int)buff[0];
//        lenFlag = true;
//      } else if (!dataFlag) {
//        data[dataIndex] = (byte)buff[0];
//        dataIndex++;
//        if (dataIndex >= len) {
//          dataFlag = true;
////          Serial.write((byte)command);
////          Serial.write((byte)len);
////          for (int i = 0; i < len; i++) {
////            Serial.write(data[i]);
////          }
//        }
//      } else {
//        commandFlag = false;
//        lenFlag = false;
//        dataFlag = false;
//        
//        if (command == 0) {  
//      
//        switch (data[0]) {
//        
//          case 'c': 
//            laser_array = setpoint_laser_circle; 
//            mirror_array = setpoint_mirror_circle; 
//            break; 
//      
//          case 's':
//            laser_array = setpoint_laser_square; 
//            mirror_array = setpoint_mirror_square; 
//            break; 
//      
//          case 't':
//            laser_array = setpoint_laser_triangle; 
//            mirror_array = setpoint_mirror_triangle; 
//            break; 
//      
//          case 'l':
//            laser_array = setpoint_laser_line; 
//            mirror_array = setpoint_mirror_line; 
//            break;
//    
//        // Make sure nothing weird happens if an unexpected signal comes through
//        default: 
//          laser_array = setpoint_laser_line; 
//          mirror_array = setpoint_mirror_line; 
//          break; 
//    
//      }
//      populate_setpoint_arrays(laser_array, mirror_array); 
//  }
//        
//        dataIndex = 0;
//      }
//   }
  
  /*
   * Debugging serial prints 
   * Probably won't work when GUI is implemented
//   */
//  Serial.print(input_laser);
//  Serial.print(","); 
//  Serial.println(setpoint_laser);  
//  Serial.print(input_mirror);
//  Serial.print(", "); 
//  Serial.println(setpoint_mirror);
//  Serial.println(lastTime_laser); 
//

  
//  Serial.println(setpoint_mirror); 


  if (t1Flag) 
    update_pid(); 
}

long convert_to_micro_seconds (double value) {
  long output = round(value*1E6); 
  return output; 
}

void homing() {

  // Rotate until slot detector hits gap
  while(digitalRead(homingPin_mirror)) {
    analogWrite(motorPin_mirror, 100);   
    analogWrite(MIRROR_DIREC_2, 255);
    analogWrite(MIRROR_DIREC_1, 0); 
  }
  analogWrite(motorPin_mirror, 0);
  displacement_mirror = 0; 

  while(digitalRead(homingPin_laser)) {
    analogWrite(motorPin_laser, 100);   
    analogWrite(LASER_DIREC_2, 255);
    analogWrite(LASER_DIREC_1, 0); 
  }
  
  displacement_laser = 0;
//  analogWrite(motorPin_laser, 0);
//  Serial.println("Laser Motor Done"); 

}

void populate_setpoint_arrays (double *laser_shape, double *mirror_shape) {

  for (int i = 0; i < SETPOINT_ARRAY_SIZE; i++) {
    setpointArray_laser[i] = pgm_read_float(&laser_shape[i]);
    setpointArray_mirror[i] = pgm_read_float(&mirror_shape[i]);

//    if (setpointArray_laser[i] != TERMINATION_VAL){
//      Serial.write('m'); 
//      sendFloat(setpointArray_mirror[i]);  
//      Serial.write('l');
//      sendFloat(setpointArray_laser[i]);
//    }
  }  
}


void readShape() {
  double *laser_array;
  double *mirror_array;  

  int *arrayOfFun; 
  char option = 'q'; 

  byte *test; 

  static bool commandFlag = false; 
  static bool lenFlag = false; 
  static bool dataFlag = false; 

  static int dataIndex = 0; 
  static int command; 
  static int len; 
  static int data; 

  long newSpeed = 0; 

  Serial.readBytes(test, 1); 
//  Serial.write(test[0]);

  if (!commandFlag) {
    command = test[0]; 
    commandFlag = true;
  }
  else if (!lenFlag) {
    len = test[0];  
    lenFlag = true; 
  }
  else if (!dataFlag) {
    data = test[0]; 
    dataFlag = true;
//    if (dataIndex < len) {
//      data[dataIndex] = test[0];
//      dataIndex++; 
//    }
//    else {
//      dataFlag = true; 
//    }
  }

  if (dataFlag){
    
    if (command == 0) {  
      
        switch (data) {
        
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
          laser_array = setpoint_laser_line; 
          mirror_array = setpoint_mirror_line; 
          break; 
    
      }
      populate_setpoint_arrays(laser_array, mirror_array); 
  }

  /* Command = 1: Change update speed
   * Convert 4 byte array into float and update timer
   */
//  if (command == 1){
//
//    for (int i = 0; i<4; i++){
//      newSpeed = (newSpeed<<8)|data[i]; 
//    }
//    
//    Timer3.setPeriod(convert_to_micro_seconds((newSpeed/100))); 
//  }
//
//  if (command == 2){
//    Serial.println("New XData"); 
//    for (int i = 0; i < 4*len; i++) {
//       
//    }
//  }

  // Reset Flags
  commandFlag = false; 
  lenFlag = false; 
  dataFlag = false; 
  dataIndex = 0; 
  command = 0; 
  len = 0; 
  data = 0;
//  for (int i = 0; i<4; i++){
//    data[i] = 0; 
//  }
 }

}


// Test to make sure current driver circuit is working
// Should not be called during normal operation
void testMotor () {
  while (true) {
    analogWrite(motorPin_laser, 150);   
    analogWrite(LASER_DIREC_2, 255);
    analogWrite(LASER_DIREC_1, 0); 
    analogWrite(motorPin_mirror, 75); 
    analogWrite(MIRROR_DIREC_2, 255);
    analogWrite(MIRROR_DIREC_1, 0); 
  }
}

void sendFloat(double val) {
  byte *bytes = (byte *)&val; 
  Serial.write(bytes, sizeof(val));
}

