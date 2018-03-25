/***********************************
* Updates PID values for both motors at an interval specified by controlTime_us 
*/

void update_pid() {

  output_laser = calculate_PID(kp_laser, ki_laser, kd_laser, input_laser, setpoint_laser, &errorTotal_laser, &lastError_laser, &lastSetpoint_laser);
  output_mirror = calculate_PID(kp_mirror, ki_mirror, kd_mirror, input_mirror, setpoint_mirror, &errorTotal_mirror, &lastError_mirror, &lastSetpoint_mirror);

  PWM_out(output_laser, motorPin_laser, LASER_DIREC_1, LASER_DIREC_2); 
  PWM_out(output_mirror, motorPin_mirror, MIRROR_DIREC_1, MIRROR_DIREC_2); 
  
}


double calculate_PID (double kp, double ki, double kd, double input, double setpoint, double *errorTotal, double *lastError, double *lastSetpoint) {
  
  double error, derivativeError, errorChange, derivativeFiltered, pid_val, motor_out; 
  bool saturationFlag = 0;

  // P Control
  error = setpoint - input;                 // Calculate Error Term

  // I Control
  *errorTotal += error*controlTime_s;         // Calculate total error for integral term scaled by the change in time since last interrupt                    

  // Prevent integral windup from getting out of control. If saturationFlag is high, the I term will be ignored in the PID calculation
  if (*errorTotal > 255 || *errorTotal < -255)
    saturationFlag = 1; 
  else
    saturationFlag = 0; 

  // D control
  if (setpoint != *lastSetpoint)         // Handle derivative kick by not using the error if the setpoint changed
    derivativeError = -input; 
  else
    derivativeError = error;          

  errorChange = (derivativeError - *lastError)/controlTime_s;                // Difference between error and lastError for derivative term 

//  derivativeFiltered = errorChange*FILTER_COEF/(errorChange + FILTER_COEF);         // Apply low pass filter to derivative term

  // Calculate PID values. If error term has saturated, do not use it. 
  if (saturationFlag)
    pid_val = kp*error + kd*errorChange;                                  // Do not use Ki term if we are saturating
  else
    pid_val = kp*error + ki*errorTotal_laser + kd*errorChange;       

  double output = constrain(pid_val, -SUPPLY_VOLTAGE, SUPPLY_VOLTAGE);                       // Constrain to a value within the power supply 

  *lastError = error; 
  *lastSetpoint = setpoint; 
  
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

void update_setpoint() {
  setpoint_laser = setpointArray_laser[setpointIndex]*DEG_TO_RAD; 
  setpoint_mirror = setpointArray_mirror[setpointIndex]*DEG_TO_RAD; 

  if (setpointIndex >= SETPOINT_ARRAY_SIZE-1)
    setpointIndex = 0; 
  else
    setpointIndex++; 
}

