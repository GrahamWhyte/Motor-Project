#define direcPin A1
#define direc2Pin A2 
#define ARRAY_SIZE 2500

unsigned int runTime[ARRAY_SIZE];
unsigned int interruptCount = 1; 

int encoderPin = 3; 
int motorPin = 9;  

int motorOffFlag = 0; 

void setup() {
  Serial.begin(9600); 
  digitalWrite(direcPin, HIGH); 
  digitalWrite(direc2Pin, LOW); 
  analogWrite(motorPin, 255); 
  delay(5000); 
  attachInterrupt(digitalPinToInterrupt(encoderPin), store_encoder_data, FALLING);  
  runTime[0] = micros(); 
}

void loop() {

  if (~motorOffFlag){
    analogWrite(direcPin, 255); 
    analogWrite(direc2Pin, 0); 
    analogWrite(motorPin, 255); 
  }
  
  if (interruptCount >= ARRAY_SIZE-1) {
    motorOffFlag = 1; 
    detachInterrupt(digitalPinToInterrupt(encoderPin)); 
    analogWrite(motorPin, 0); 
    digitalWrite(direcPin, LOW); 
    digitalWrite(direc2Pin, LOW); 

    for (int i = 0; i<ARRAY_SIZE; i++){
      // Signal that python should expect integer data
      Serial.write('i'); 
  
      // Write 8 bits at a time
      Serial.write(highByte(runTime[i])); 
      Serial.write(lowByte(runTime[i])); 
    }
    Serial.write('f'); 
  }

}

void store_encoder_data(){
  runTime[interruptCount] = micros(); 
  interruptCount++;
}
