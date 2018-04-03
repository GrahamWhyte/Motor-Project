/**************************
* laser_encoder_ISR_1
* 
* Interrupt Service Routine that triggers on every falling edge of the laser motor slot detector A
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
******************************/
void laser_encoder_ISR_1() {

  // Rising Edge 
  if (digitalRead(encoderPin_laser)){
    if (digitalRead(encoderPin2_laser))
      displacement_laser--;  
    else 
      displacement_laser++; 
  }

  // Falling Edge
   if (!digitalRead(encoderPin_laser)){
    if (digitalRead(encoderPin2_laser))
      displacement_laser++; 
    else 
      displacement_laser--; 
  }

  // Convert the displacement (in interrupts) to radians
  input_laser = displacement_laser * INTERRUPT_TO_DEG;
}

/**************************
* laser_encoder_ISR_2
* 
* Interrupt Service Routine that triggers on every falling edge of the laser motor slot detector B
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
******************************/
void laser_encoder_ISR_2() {

  // Rising Edge 
  if (digitalRead(encoderPin2_laser)){
    if (digitalRead(encoderPin_laser))
      displacement_laser++;  
    else 
      displacement_laser--; 
  }

  // Falling Edge
   if (!digitalRead(encoderPin2_laser)){
    if (digitalRead(encoderPin_laser))
      displacement_laser--; 
    else 
      displacement_laser++; 
  }
}

/**************************
* mirror_encoder_ISR_1
* 
* Interrupt Service Routine that triggers on every falling edge of the mirror motor slot detector A
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
******************************/
void mirror_encoder_ISR_1() {

  // Rising Edge 
  if (digitalRead(encoderPin_mirror)){
    if (digitalRead(encoderPin2_mirror))
      displacement_mirror--;  
    else 
      displacement_mirror++; 
  }

  // Falling Edge
   if (!digitalRead(encoderPin_mirror)){
    if (digitalRead(encoderPin2_mirror))
      displacement_mirror++; 
    else 
      displacement_mirror--; 
  }
  input_mirror = displacement_mirror * INTERRUPT_TO_DEG;
}

/**************************
* mirror_encoder_ISR_2
* 
* Interrupt Service Routine that triggers on every falling edge of the mirror motor slot B
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
******************************/
void mirror_encoder_ISR_2() {

  // Rising Edge 
  if (digitalRead(encoderPin2_mirror)){
    if (digitalRead(encoderPin_mirror))
      displacement_mirror++;  
    else 
      displacement_mirror--; 
  }

  // Falling Edge
   if (!digitalRead(encoderPin2_mirror)){
    if (digitalRead(encoderPin_mirror))
      displacement_mirror--; 
    else 
      displacement_mirror++; 
  }

  // Convert the displacement (in interrupts) to radians
  input_mirror = displacement_mirror * INTERRUPT_TO_DEG;
}


