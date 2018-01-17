#include  <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11); // RX | TX

// defines pins numbers
const int stepPinR = 3; //Right
const int dirPinR = 4; 
const int stepPinL = 5; //Left
const int dirPinL = 6; 

String roll,pitch;
int int_roll,int_pitch,loopTimes;
int int_roll_old=3000,int_pitch_old=3000;
int LDelay,RDelay;
void run(int LDelay, int RDelay){

  if(LDelay<0){ //Backward
    digitalWrite(dirPinL,LOW); // Enables the motor to move in a particular direction
    LDelay = abs(LDelay);
  }else{ //Forward
    digitalWrite(dirPinL,HIGH); // Enables the motor to move in a particular direction
  }
  if(RDelay<0){ //Backward
    digitalWrite(dirPinR,LOW); // Enables the motor to move in a particular direction
    RDelay = abs(RDelay);
  }else{ //Forward
    digitalWrite(dirPinR,HIGH); // Enables the motor to move in a particular direction
  }

  loopTimes = (LDelay+RDelay)/(-112)+70;
  
      Serial.print(':');
      Serial.println(loopTimes);
  // 200 pulses for making one full cycle rotation
  //TODO 按照比例loop
  for(int x = 0; x < loopTimes; x++) {
    
    digitalWrite(stepPinR,HIGH); 
    delayMicroseconds(RDelay); 
    digitalWrite(stepPinR,LOW); 
    delayMicroseconds(RDelay); 
    digitalWrite(stepPinL,HIGH); 
    delayMicroseconds(LDelay); 
    digitalWrite(stepPinL,LOW);
    delayMicroseconds(LDelay); 
  }
}

int checkBound(int i){
  if(i>=0){
    if (i>3000){
      i = 3000;
    }else if(i<1000){
      i = 1000;
    }
  }else{
    if (i<-3000){
      i = -3000;
    }else if (LDelay>-1000){
      i = -1000;
    }
  }
  return i;
}

void setup()
{
  Serial.begin(9600);                     // 開啟 Serial port, 通訊速率為 9600 bps
  
  pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(9, HIGH);
  Serial.begin(9600);
  Serial.println("Bluetooth wired connected.");
  Serial.println("Enter AT commands:");
  BTSerial.begin(9600);  // HC-05 default speed in AT command more
  BTSerial.setTimeout(10);
  
  // Sets the two pins as Outputs
  pinMode(stepPinR,OUTPUT); 
  pinMode(dirPinR,OUTPUT);
  pinMode(stepPinL,OUTPUT); 
  pinMode(dirPinL,OUTPUT);
}

void loop() 
{
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available()){
    BTSerial.readStringUntil('[');
    roll = BTSerial.readStringUntil(',');
    if(roll != ""){
      pitch = BTSerial.readStringUntil(']');
      int_roll=roll.toInt();
      int_pitch=pitch.toInt();
      
      if(int_pitch>0){
        int_pitch = (-200/7)*int_pitch+3000;
      }else{
        int_pitch = (-200/7)*int_pitch-3000;
      }
      Serial.print(int_roll);
      Serial.print('@');
      int_roll = int_roll*42;
      Serial.print(int_roll);
      Serial.print('@');
      int_pitch_old = (int_pitch*3+int_pitch_old)/4;
      int_roll_old = (int_roll*3+int_roll_old)/4;
      Serial.print(int_roll);
      Serial.print('@');
      if(int_roll_old>0){
        LDelay = int_pitch_old-int_roll_old;
        RDelay = int_pitch_old+int_roll_old;
      }else{
        LDelay = int_pitch_old+int_roll_old;
        RDelay = int_pitch_old-int_roll_old;
      }Serial.print(int_roll);
      Serial.print('@');
      LDelay = checkBound(LDelay);
      RDelay = checkBound(RDelay);
      
      Serial.print(int_roll);
      Serial.print('@');
      Serial.print(int_pitch);
      Serial.print("  =>  ");
      Serial.print(int_roll_old);
      Serial.print('@');
      Serial.print(int_pitch_old);
      Serial.print("  =>  ");
      Serial.print(LDelay);
      Serial.print('@');
      Serial.print(RDelay);
      run(LDelay,RDelay);
     
    }
  }
  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available()){
    BTSerial.write(Serial.read());
  }
  BTSerial.flush();
  delay(2);
}

