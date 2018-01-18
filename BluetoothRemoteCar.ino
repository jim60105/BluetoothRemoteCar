#include  <SoftwareSerial.h>
#include <Math.h>
SoftwareSerial BTSerial(10, 11); // RX | TX: Notice that they're opposite from the HM-10 Pins

// defines pins numbers
const int stepPinR = 3; //Right
const int dirPinR = 4; 
const int stepPinL = 5; //Left
const int dirPinL = 6; 

String roll,pitch;
int int_roll,int_pitch,loopTimes,x;
int int_roll_old=3000,int_pitch_old=3000;
int counterL=0,counterR=0;
double LDelay,RDelay,aver;
double spaceL,spaceR;
void run(){
  //  Set the direction of steppers
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

  //  Main loop
  loopTimes = 15;
  
  spaceL=(LDelay/(LDelay+RDelay));
  spaceR=(RDelay/(LDelay+RDelay));
  counterL = counterR = 0;
  aver = (LDelay+RDelay)/2;
  
  for(x = 1; x <= loopTimes; x++) {
    while(x>spaceL*counterR||x>spaceR*counterL){
      if(x>spaceL*counterR){
        move(stepPinR,aver);
        counterR++;
      }
      if(x>spaceR*counterL){  
        move(stepPinL,aver);
        counterL++;
      }
    }
  }
}

//  Action the steps
void move(int stepPin,double Delay){
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(Delay); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(Delay); 
}

//  My stepper works in delay near abs(1000~3000)
int checkBound(int i){
  if(i>=0){ //foward
    if (i>3000){
      i = 3000;
    }else if(i<1000){
      i = 1000;
    }
  }else{  //backward
    if (i<-3000){
      i = -3000;
    }else if(i>-1000){
      i = -1000;
    }
  }
  return i;
}

void setup()
{
  Serial.begin(9600);
  
  pinMode(9, OUTPUT);   // This pin will pull the HC-05 pin HIGH to switch module to AT mode
  digitalWrite(9, HIGH);// I'm not exactly sure is it nessary for HM-10, but it works with this active.
  Serial.begin(9600);
  Serial.println("Bluetooth wired connected.");
  Serial.println("Enter AT commands:");
  BTSerial.begin(9600);  // HM-10 default speed in AT command more
  BTSerial.setTimeout(10);
  
  // Sets the two pins as Outputs
  pinMode(stepPinR,OUTPUT); 
  pinMode(dirPinR,OUTPUT);
  pinMode(stepPinL,OUTPUT); 
  pinMode(dirPinL,OUTPUT);
}

void loop() 
{
  // Keep reading from HM-10
  if (BTSerial.available()){
    //  Input data shoud be a String like: "[-37,50]"
    BTSerial.readStringUntil('[');
    roll = BTSerial.readStringUntil(',');
    if(roll != ""){
      pitch = BTSerial.readStringUntil(']');
      int_roll=roll.toInt();
      int_pitch=pitch.toInt();

      //  Set pitch angle 1~90=>3000~1000 && -90~0=>-1000~-3000
      //  This will change degrees to delay times, so we should inverse the range here.
      //  My stepper works in delay near abs(1000~3000)
      if(int_pitch>0){
        int_pitch = (-200/11)*int_pitch+3000;
      }else{
        int_pitch = (-200/11)*int_pitch-3000;
      }
      int_roll = int_roll*42;
      
      //  You can do some smooth here:
    //int_pitch_old = (int_pitch*3+int_pitch_old)/4;
    //int_roll_old = (int_roll*3+int_roll_old)/4;
      int_pitch_old = int_pitch;
      int_roll_old = int_roll;

      //  Make turn
      if(int_pitch_old>0){
        LDelay = int_pitch_old+int_roll_old;
        RDelay = int_pitch_old-int_roll_old;
      }else{
        LDelay = int_pitch_old-int_roll_old;
        RDelay = int_pitch_old+int_roll_old;
      }

      //  checkBound
      LDelay = checkBound(LDelay);
      RDelay = checkBound(RDelay);

      run();
    }
  }
  
  //  Keep reading from Arduino Serial Monitor and send to HM-10
  //  Only works in AT mode
  if (Serial.available()){
    BTSerial.write(Serial.read());
  }

  //  Flush out datas in BTSerial. In order to try to avoid lagging. 
  //  I'm not quite sure is this useful.
  BTSerial.flush();
  delay(2);
}

