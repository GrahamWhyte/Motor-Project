unsigned int test = 4; 
int acknowledge = 0; 
char arduinoStartSignal = 'a'; 
char inputSignal; 

void setup() {

  // Baud rate of 9600
  Serial.begin(9600); 

  // Send start signal to Python script
  Serial.write(arduinoStartSignal); 

  // Wait for response from python
  while (Serial.available() == 0){}
  char test = Serial.read(); 
  Serial.print(test); 
  delay(1000000000); 
}

void loop() {
  for (int i = 0; i<300; i++){

    // Signal that python should expect integer data
    Serial.write('i'); 

    // Write 8 bits at a time
    Serial.write(highByte(i)); 
    Serial.write(lowByte(i)); 
  }

  // Send signal to write to text file
  Serial.write('f'); 
}
