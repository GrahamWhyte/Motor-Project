#define direcPin A1
#define direc2Pin A2 
#define ARRAY_SIZE 2500


// Digital Pins
int motorPin = 9;
int encoderPin = 3; 
int encoder2Pin = 10; 

int runtime[ARRAY_SIZE]; 
int interruptCount = 1; 
int motorOffFlag = 0; 
bool interruptFlag = 0; 

void setup() {
  
  Serial.begin(9600); 

  // Configure digital pins
  pinMode(encoderPin, INPUT_PULLUP); 
  pinMode(motorPin, OUTPUT); 
  pinMode(encoder2Pin, INPUT_PULLUP); 
  pinMode(direcPin, OUTPUT);
  pinMode(direc2Pin, OUTPUT);

  // Get motor to full speed, log current 
  digitalWrite(direcPin, HIGH);
  digitalWrite(direc2Pin, LOW); 
  analogWrite(motorPin, 255); 

  // Wait 10s 
  delay(10000); 

  runtime[0] = micros(); 

  // Attach the interrupt
  attachInterrupt(digitalPinToInterrupt(encoderPin), encoder_ISR, FALLING); 
}

void loop() {

  // Get 50 readings at full speed
  if (interruptCount>50 && ~motorOffFlag){
     motorOffFlag = 1; 
     analogWrite(motorPin, 0); // Turn off the motor
  }

  // Send data over serial after loggging all data
  if (interruptCount >= ARRAY_SIZE){
    detachInterrupt(digitalPinToInterrupt(encoderPin)); 
    for (int i = 0; i < ARRAY_SIZE; i++){
      // Signal that python should expect integer data
      Serial.write('i'); 
  
      // Write 8 bits at a time
      Serial.write(highByte(runtime[i])); 
      Serial.write(lowByte(runtime[i])); 
    }

    Serial.write('f'); 
    
  }
// 
}

void encoder_ISR(){
  runtime[interruptCount] = micros(); 
  interruptCount++; 
}

