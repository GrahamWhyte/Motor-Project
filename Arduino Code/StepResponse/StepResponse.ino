#define direcPin A1
#define direc2Pin A2 
#define ARRAY_SIZE 3500 

unsigned int runTime[ARRAY_SIZE];
unsigned int interruptCount = 1; 

int encoderPin = 3; 
int motorPin = 9;  

bool interruptFlag = 0; 

void setup() {
  Serial.begin(9600); 
  attachInterrupt(digitalPinToInterrupt(encoderPin), store_encoder_data, FALLING);  
  runTime[0] = micros(); 
}

void loop() {
  if (interruptCount >= ARRAY_SIZE-1) {
    detachInterrupt(digitalPinToInterrupt(encoderPin)); 
    analogWrite(motorPin, 0); 
    analogWrite(direcPin, 0); 
    analogWrite(direc2Pin, 0); 

  for (int i = 0; i<ARRAY_SIZE; i++){

    // Signal that python should expect integer data
    Serial.write('i'); 

    // Write 8 bits at a time
    Serial.write(highByte(runTime[i])); 
    Serial.write(lowByte(runTime[i])); 
  }

  Serial.write('f'); 

  }

  else{
    digitalWrite(direcPin, 255); 
    digitalWrite(direc2Pin, 0); 
    analogWrite(motorPin, 255); 
  }
}

void store_encoder_data(){

  // Log the first 1000 interrupts
  if (interruptCount <= 1000) {
    runTime[interruptCount] = micros(); 
    interruptCount++; 
  }

  // After 1000 interrupts have passed, log every 2nd interrupt
  else
    if (interruptFlag) {
      runTime[interruptCount] = micros(); 
      interruptCount++; 
      interruptFlag = !interruptFlag; 
    }
    else
      interruptFlag = !interruptFlag; 
}
