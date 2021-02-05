//Reads Analog voltage at two terminals of Wheatstone Bridge (Vc, Vd), Varies Resistance in digital potentiometer, 
//Reads resistance on digital potentiometer, Calculates Rx when Vc=Vd
//Mandira Marambe

//--------------------------------------------------------INITIATIONS, ASSIGNMENTS & SETUP-----------------------------------------------------//

#include <LiquidCrystal.h>                       //Initiate LCD library
#include <Wire.h>                                //Initiate Wire library

#define Addr 0x2C 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);           

void setup() {
  
  //Initiate lcd, I2C and Serial monitor:
  lcd.begin(2, 16);
  Serial.begin(9600); 
  Wire.begin();                                 // join i2c bus (address optional for master)

  
}

byte val = 128;                                 //Or any byte value to be set as the lowest
float RxList[]= {0.0,0.0};                      //Rx List to use for obtaining factor
  

void loop() {
  
//----------------------------------------------------------------VOLTAGES ON LCD--------------------------------------------------------------//
  
  //Read analog pins A0, A1 as integers:
  int sensorValue = analogRead(A0); 
  int sensorValue2= analogRead(A1);
  
  // Convert the analog readings to a voltage (0 - 5V):
  float voltage = (float) sensorValue * (5.0 / 1023.0);  //Voltage at Vc between R1 and R2
  float v2 = (float) sensorValue2 * (5.0 / 1023.0);      //Voltage at Vd between R3[Variable] and Rx[Sensor]

   
  //Print voltage, v2 on lcd: 
  lcd.setCursor(1, 1);
  lcd.print("Vc (A0)  ");
  lcd.print(voltage); 
  lcd.setCursor(1, 0);
  lcd.print("Vd (A1)  ");
  lcd.print(v2);
  
  //Delay to allow stabilise reading:
  delay(2);

  //-----------------------------------------------------------------VARY R3--------------------------------------------------------------------//

  Wire.beginTransmission(Addr);        // transmit to device #44 (0x2c)  // device address is specified in datasheet
  Wire.write(byte(0x00));              // sends instruction byte
  Wire.write(val);                     // sends potentiometer value byte
  Wire.endTransmission();              // stop transmitting

  val++;                              // increment value
  if (val == 256) {                   // if reached 256th position (max)
    val = 128;                        // start over from lowest value
  }
  
  //-----------------------------------------------------RECEIVE RESISTANCE R3 AT Vc=Vd------------------------------------------------------//
   
   unsigned int data;
   // Request 1 byte of data
   Wire.requestFrom(Addr, 1);
  
   // Read 1 byte of data
   if (Wire.available() == 1)
   {
     data = Wire.read();
     }
  
    //Calculate and scale resistence and voltage from other pin
   float R3 = (data / 256.0 ) * 10.0;

   // Check if Vc is in acceptable range of Vd and print corresponding R3
   float r1= v2-0.01;
   float r2= v2+0.01;
   if (r1<= voltage && voltage <= r2){
       lcd.clear();
       lcd.setCursor(1, 1);
       lcd.print("R3 : ");
       lcd.print(R3);
       lcd.println(" K");
       
//-----------------------------------------------------------------CALCULATE RX----------------------------------------------------------------//

   //Calculate and output Rx and length measurement on lcd
       float R2= 4.7 ;                     // 4.7 K resistor
       float R1= 1.0;                      // 1K resistor
       float Rx= R3*R1;
       
       lcd.clear();
       lcd.setCursor(1, 0);
       lcd.print("Rx : ");
       lcd.print(Rx);
       lcd.println(" K");
       delay(1000);

 //-------------------------------------------------------------ELONGATION  PERCENTAGE------------------------------------------------------------//      

         if (RxList[0]==0){                                      //If Rx initial has not been recorded yet
            RxList[0]= Rx;
            lcd.clear();
            lcd.setCursor(1, 1);
            lcd.print("Stretch Now ");                            //delay to stretch 
            delay (5000);
               
         }
         else {
            RxList[1]= Rx;                                       //If Rx initial has been recorded,store Rx on stretch for facor calculation 
    
          //Calculate elongation percentage from Rx change factor when List[0] and List[1] are both occupied
          
            float factor = RxList[1]/RxList[0];                   //Get factor
            float elongationPercentage= (60*factor)/9;            //Map to elongation % (from predetermined equation-- y=9x/60; y= Elongation)
            lcd.clear();
            lcd.setCursor(1, 1);
            lcd.print("E : ");
            lcd.print(elongationPercentage);
            lcd.println(" %");
            delay(5000);    
       }   
   }
//-----------------------------------------------------------------E-N-D--------------------------------------------------------------------------//   
}   
