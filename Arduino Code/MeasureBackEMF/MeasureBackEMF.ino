#define direcPin A1
#define direc2Pin A2 
#define ARRAY_SIZE 750 

unsigned int runTime[ARRAY_SIZE];
unsigned int interruptCount = 1; 

int encoderPin = 3; 
int motorPin = 5;  

void setup() {
  Serial.begin(9600); 
  digitalWrite(direcPin, HIGH); 
  digitalWrite(direc2Pin, LOW); 
  analogWrite(motorPin, 255); 
  delay(2000); 
  attachInterrupt(digitalPinToInterrupt(encoderPin), store_encoder_data, FALLING);  
  runTime[0] = micros(); 
}

void loop() {
  if (interruptCount >= ARRAY_SIZE-1) {
    detachInterrupt(digitalPinToInterrupt(encoderPin)); 
//    analogWrite(motorPin, 0); 
//    digitalWrite(direcPin, LOW); 
//    digitalWrite(direc2Pin, LOW); 

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
    digitalWrite(direcPin, HIGH); 
    digitalWrite(direc2Pin, LOW); 
    analogWrite(motorPin, 255); 
  }
}

void store_encoder_data(){
  runTime[interruptCount] = micros(); 
  interruptCount++;
}
