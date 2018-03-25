/**************************
* laser_encoder_ISR_1
* 
* Interrupt Service Routine that triggers on every falling edge of the laser motor slot detector A
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
******************************/
void laser_encoder_ISR_1() {

  // If the other signal is high on the falling edge of the first signal, we are going backwards
  if (digitalRead(encoderPin2_laser)) {
    displacement_laser--;
  }

  // Otherwise, we go forwards
  else {
    displacement_laser++;
  }

  // Convert the displacement (in interrupts) to radians
  input_laser = displacement_laser * INTERRUPT_TO_RAD;
}

/**************************
* laser_encoder_ISR_2
* 
* Interrupt Service Routine that triggers on every falling edge of the laser motor slot detector B
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
******************************/
void laser_encoder_ISR_2() {

  // If the other signal is high on the falling edge of the first signal, we are going forwards
  if (digitalRead(encoderPin_laser)) {
    displacement_laser++;
  }

  // Otherwise, we go backwards
  else {
    displacement_laser--;
  }

  // Convert the displacement (in interrupts) to radians
  input_laser = displacement_laser * INTERRUPT_TO_RAD;
}

/**************************
* mirror_encoder_ISR_1
* 
* Interrupt Service Routine that triggers on every falling edge of the mirror motor slot detector A
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
******************************/
void mirror_encoder_ISR_1() {

  // If the other signal is high on the falling edge of the first signal, we are going backwards
  if (digitalRead(encoderPin2_mirror)) {
    displacement_mirror--;
  }

  // Otherwise, we go forwards
  else {
    displacement_mirror++;
  }

  // Convert the displacement (in interrupts) to radians
  input_mirror = displacement_mirror * INTERRUPT_TO_RAD;
}

/**************************
* mirror_encoder_ISR_2
* 
* Interrupt Service Routine that triggers on every falling edge of the mirror motor slot B
* Determines direction and increments or decrements displacement accordingly 
* Converts displacement into radians
******************************/
void mirror_encoder_ISR_2() {

  // If the other signal is high on the falling edge of the first signal, we are going forwards
  if (digitalRead(encoderPin_mirror)) {
    displacement_mirror++;
  }

  // Otherwise, we go backwards
  else {
    displacement_mirror--;
  }

  // Convert the displacement (in interrupts) to radians
  input_mirror = displacement_mirror * INTERRUPT_TO_RAD;
}


