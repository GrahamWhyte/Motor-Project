void timer_one_ISR() {
  t1Flag = true; 
}

/***********************************
* Updates PID values for both motors at an interval specified by controlTime_us 
*/





void update_pid() {

//  digitalWrite(13, LOW); 
  t1Flag = false; 
  
  double kp = 0;
    
  if(laser_direction == 0)
    kp = kp_laser - LASER_MOD;
  else
    kp = kp_laser;
  
  output_laser = calculate_PID(kp, ki_laser, kd_laser, input_laser, setpoint_laser, &errorTotal_laser, &lastError_laser, &lastSetpoint_laser, &lastTime_laser);
  output_mirror = calculate_PID(kp_mirror, ki_mirror, kd_mirror, input_mirror, setpoint_mirror, &errorTotal_mirror, &lastError_mirror, &lastSetpoint_mirror, &lastTime_mirror);

  PWM_out(output_laser, motorPin_laser, LASER_DIREC_1, LASER_DIREC_2); 
  PWM_out(output_mirror, motorPin_mirror, MIRROR_DIREC_1, MIRROR_DIREC_2); 

//  digitalWrite(13, HIGH); 
}


double calculate_PID (double kp, double ki, double kd, double input, double setpoint, double *errorTotal, double *lastError, double *lastSetpoint, double *lastTime) {
  
  double error, derivativeError, errorChange, derivativeFiltered, pid_val, motor_out, currentTime, timeChange; 
  bool saturationFlag = 0;
//
  currentTime = micros()/1E6; 
  timeChange = currentTime - *lastTime; 
//  
  // P Control
  error = setpoint - input;                 // Calculate Error Term

  // I Control
  *errorTotal += error*timeChange;         // Calculate total error for integral term scaled by the change in time since last interrupt                    

  // Prevent integral windup from getting out of control. If saturationFlag is high, the I term will be ignored in the PID calculation
  if (*errorTotal > 255 || *errorTotal < -255)
    saturationFlag = 1; 
  else
    saturationFlag = 0; 
//
//  // D control
//  if (setpoint != *lastSetpoint)         // Handle derivative kick by not using the error if the setpoint changed
//    derivativeError = -input; 
//  else
  derivativeError = error;          

  errorChange = (derivativeError - *lastError)/timeChange;                // Difference between error and lastError for derivative term 

  derivativeFiltered = errorChange*FILTER_COEF/(errorChange + FILTER_COEF);         // Apply low pass filter to derivative term

  // Calculate PID values. If error term has saturated, do not use it. 
  if (saturationFlag)
    pid_val = kp*error + kd*derivativeFiltered;                                  // Do not use Ki term if we are saturating
  else
    pid_val = kp*error + ki*(*errorTotal) + kd*derivativeFiltered;       

  double output = constrain(pid_val, -SUPPLY_VOLTAGE, SUPPLY_VOLTAGE);                       // Constrain to a value within the power supply 

//  output_buffer[]
  
  *lastError = error; 
  *lastSetpoint = setpoint; 
  *lastTime = currentTime; 
  
  return output; 
}

// NOTE: WILL NEED TO TAKE ANOTHER LOOK AT LOGIC
void PWM_out(double pidVal, int motorPin, int direcPin1, int direcPin2) {
  
  // Determine direction
  if (pidVal < 0) {
    analogWrite(direcPin1, 0);        // Must write PWM signal instead of digital signal to analog pins
    analogWrite(direcPin2, 255);
  }
  else
  {
    analogWrite(direcPin1, 255);
    analogWrite(direcPin2, 0);
  }
  
  // Convert output voltage to PWM signal
  int pwmVal = round(abs(pidVal*255/SUPPLY_VOLTAGE));

  analogWrite(motorPin, pwmVal);
  
}

//void update_setpoint() {
//  setpoint_laser = setpointArray_laser[setpointIndex]; 
//  setpoint_mirror = setpointArray_mirror[setpointIndex]; 
//
////  if (setpointDir){
////    if (setpointIndex >= SETPOINT_ARRAY_SIZE-1){
////        setpointDir = false; 
////        setpointIndex--; 
////    }
////    else
////      setpointIndex++; 
////  }
////  
////  if (!setpointDir) {
////    if (setpointIndex<= 0) {
////        setpointDir= true; 
////        setpointIndex++; 
////    }
////    else 
////      setpointIndex--; 
////  }
//
////
//  if (setpointIndex >= SETPOINT_ARRAY_SIZE-1)
//    setpointIndex = 0; 
//  else
//    setpointIndex++; 
//}

void update_setpoint() {
  setpoint_laser = setpointArray_laser[setpointIndex]; 
  setpoint_mirror = setpointArray_mirror[setpointIndex]; 

//  if (setpointArray_laser[setpointIndex+1] == TERMINATION_VAL || setpointArray_mirror[setpointIndex+1] == TERMINATION_VAL) {
//    setpoint_laser = setpointArray_laser[setpointIndex]; 
//    setpoint_mirror = setpointArray_mirror[setpointIndex]; 
//    setpointDir = false; 
//  }
//
//  if (setpointIndex == 0) {
//    setpoint_laser = setpointArray_laser[setpointIndex]; 
//    setpoint_mirror = setpointArray_mirror[setpointIndex]; 
//    setpointDir = true; 
//  }
//
//  if (setpointDir) {
//    setpointIndex++; 
//  }
//  else {
//    setpointIndex--; 
//  }

  if (setpointArray_laser[setpointIndex+1]== TERMINATION_VAL)
    setpointIndex = 0; 
  else
    setpointIndex++; 
}




