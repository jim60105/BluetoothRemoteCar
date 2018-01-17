#include  <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11); // RX | TX
#define INPUT_SIZE 8

// defines pins numbers
const int stepPin = 3; 
const int dirPin = 4; 
const int stepPin2 = 5; 
const int dirPin2 = 6; 

void run(int roll, int pitch){
  Serial.print(roll);
  Serial.print('@');
  Serial.println(pitch);
  if(pitch<0){
    digitalWrite(dirPin,LOW); // Enables the motor to move in a particular direction
    digitalWrite(dirPin2,LOW); // Enables the motor to move in a particular direction
    pitch = abs(pitch);
  }else{
    digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
    digitalWrite(dirPin2,HIGH); // Enables the motor to move in a particular direction
  }
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 18; x++) {
    
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(pitch); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(pitch); 
    digitalWrite(stepPin2,HIGH); 
    delayMicroseconds(pitch); 
    digitalWrite(stepPin2,LOW);
    delayMicroseconds(pitch); 
  }
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
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(stepPin2,OUTPUT); 
  pinMode(dirPin2,OUTPUT);
}

String roll,pitch;
int int_roll,int_pitch;
int int_roll_old=3000,int_pitch_old=3000;

void loop() 
{
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available()){
    roll = BTSerial.readStringUntil(',');
    if(roll != ""){
      pitch = BTSerial.readStringUntil(',');
      int_roll=roll.toInt();
      int_pitch=pitch.toInt();

      
      if(int_pitch>0){
        int_pitch = (-200/9)*int_pitch+3000;
        int_pitch_old = (int_pitch*3+int_pitch_old)/4;
        if(int_pitch_old<3000 && int_pitch_old>1000){
          run(int_roll,int_pitch_old);
        }
      }else{
        int_pitch = (-200/9)*int_pitch-3000;
        int_pitch_old = (int_pitch*3+int_pitch_old)/4;
        if(int_pitch_old>-3000 && int_pitch_old<-1000){
          run(int_roll,int_pitch_old); 
        }
      }
    }
  }
  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available()){
    BTSerial.write(Serial.read());
  }
  BTSerial.flush();
  delay(2);
}

