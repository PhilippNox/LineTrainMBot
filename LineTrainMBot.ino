#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#include "mBot.h"
#include "MePort.h"
MeBoard myBoard(mBot);
#include "MeRGBLed.h"
#include "MeDCMotor.h"
#include "MeIR.h"
#include "MeUltrasonic.h"
#include "Me7SegmentDisplay.h"

//....................MakeBlock
double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;
MePort linefollower_2(2);
MeRGBLed rgbled_7(7);
MeDCMotor motorLeft(9);
MeDCMotor motorRight(10);
MeUltrasonic ultrasonic_3(3);
Me7SegmentDisplay seg7_4(4);
MeIR ir;
//....................MakeBlock end



 
//....................SETTINGs

//Change In case of an error of the direction of motor movement
boolean const motoLeftInverse = true;    
boolean const motoRightInverse = false;


//..........ModeSetting
boolean const supportState = true;
boolean wallDetection = false;
boolean colorLine = 0;  // if 0 - follow black line, if 1 - follow white line

int mode = -1;  //init state
                //ABOUTE mode - what to do in diffrense situation (depend on state)
                //mode == 0 - left tour
                //mode == 1 - rigth tour
                //mode == 2 - LeftToRigth
                //mode == 3 - RightToLeft
                //mode == 4 - wallLeft
                //mode == 5 - wallRight

int state = 0;  //depent on colorLine
                //ABOUTE state - Linefollower sensor
                //state == 0 - 00 : linefollower sensor
                //state == 1 - 01
                //state == 2 - 10
                //state == 3 - 11


//..........Speed
unsigned int speedOfBot = 127;  //speed of bot
unsigned int deltaSpeed = 12;   //delta of speed change of bot


//..........IRmassage
int irStop      = 21;   //gear
int irSpeedDown = 25;   //down
int irSpeedUp   = 64;   //up
int irWallSensor = 71;  //C
int irColorLine = 70;   //B
int irLeft       = 7;   //left
int irRight     = 9;    //right


//..........Light
int ledBrightness = 15; // value from 0 to 255


//..........Delay 
unsigned long timeNow = 0;        //It should not be changed

unsigned long previousMillis = 0; //It should not be changed
const long intervalMillis = 500;

unsigned long previousButtonPush = 0; //It should not be changed
const long buttonDelay = 500; 

//....................SETTINGs end






void updateState ()
{
 
       if(linefollower_2.dRead1() == colorLine )
        {
          if(linefollower_2.dRead2() == colorLine)
          {
            state = 0;  
          }
          else
          {
            state = 1;
          }
        }
        else
        {
          if(linefollower_2.dRead2() == colorLine)
          {
            state = 2;
          }  
          else
          {
            state = 3;  
          }
        }     

}



void setMode(int modeToSet)
{
    switch(modeToSet)
    {
    case 0 :  //LEFT
      mode = 0;
      rgbled_7.setColor(2,0,ledBrightness,0);
      rgbled_7.setColor(1,0,0,0);
      rgbled_7.show();
      break;

    case 1 :  //RIGHT
       mode = 1;
       rgbled_7.setColor(2,0,0,0);
       rgbled_7.setColor(1,0,ledBrightness,0);
       rgbled_7.show();
       break;

    case 2 : //leftToRight
       mode = 2;
       rgbled_7.setColor(2,0,ledBrightness,0);
       rgbled_7.setColor(1,0,0,ledBrightness);
       rgbled_7.show();
       break;

    case 3 : //rightToLeft
       mode = 3; 
       rgbled_7.setColor(2,0,0,ledBrightness);
       rgbled_7.setColor(1,0,ledBrightness,0);
       rgbled_7.show(); 
       break;


    case 4 : //wallLeft
       mode = 4; 
       rgbled_7.setColor(2,0,ledBrightness,0);
       rgbled_7.setColor(1,ledBrightness,0,0);
       rgbled_7.show(); 
       break;
       
    case 5 : //wallRight
       mode = 5; 
       rgbled_7.setColor(2,ledBrightness,0,0);
       rgbled_7.setColor(1,0,ledBrightness,0);
       rgbled_7.show(); 
       break;

    case -1 :
       mode = -1;
       
       if(colorLine)
       {
         rgbled_7.setColor(2,0,ledBrightness,ledBrightness);
         rgbled_7.setColor(1,0,ledBrightness,ledBrightness);
       }
       else
       {
         rgbled_7.setColor(2,ledBrightness,0,ledBrightness);
         rgbled_7.setColor(1,ledBrightness,0,ledBrightness);
       }
       
       rgbled_7.show();
       break;

     default :    //WTF 
      mode = -1;  
      rgbled_7.setColor(2,255,0,0);
      rgbled_7.setColor(1,255,0,0);
      rgbled_7.show();
     }
}


void updateMode()
{
        timeNow = millis();
       
        if(ir.keyPressed(irStop)) 
        {         
                    setMode(-1); 
                    ir.begin();
        } 
        
        else if(ir.keyPressed(irSpeedDown) && (timeNow-previousButtonPush > buttonDelay))
        {
                    speedOfBot -= deltaSpeed;
                    if(speedOfBot < (deltaSpeed-1)) {speedOfBot = (deltaSpeed-1);}
                    seg7_4.display(speedOfBot);
                    ir.begin(); 
                    previousButtonPush = timeNow;
        }  
                 
        else if(ir.keyPressed(irSpeedUp) && (timeNow-previousButtonPush > buttonDelay))
        {
                    speedOfBot += deltaSpeed;
                    if(speedOfBot > 255) {speedOfBot = 255; }
                    seg7_4.display(speedOfBot);
                    ir.begin(); 
                    previousButtonPush = timeNow;
        } 
                
        else if(ir.keyPressed(irWallSensor) && (timeNow-previousButtonPush > buttonDelay))
        {
             wallDetection = !wallDetection; 
             previousButtonPush = timeNow;    
        } 
        
        else if(ir.keyPressed(irColorLine) && (timeNow-previousButtonPush > buttonDelay))
        {
             colorLine = !colorLine;  
             setMode(mode);   //changeColor of mode -1 ;
             previousButtonPush = timeNow;
        }    
        
        
      

        
        else if (mode == 4) { if(state == 1) { setMode(1); } } //switch to Right  
       
        else if (mode == 5) { if(state == 2) {  setMode(0); } }//switch to Left  

        else if(millis()-previousMillis > intervalMillis)
        {
          previousMillis = millis();
        
          if (wallDetection && ultrasonic_3.distanceCm() < 7)
        
            if (mode == 0 || mode == 2)              //LEFT MODES
            {
              if(state == 3) { setMode(1); } //switch to Right  
              else           { setMode(4); } //switch to wallLeft
            }
            else if (mode == 1 || mode == 3)        //RIGHT MODES
            {
              if(state == 3) {  setMode(0); } //switch to Left
              else           {  setMode(5);} //switch to wallRight   
            }
        }


        
        else if (mode == -1)
        {
                    if      (ir.keyPressed(irLeft))
                    {
                      setMode(0);  
                    }
                    else if (ir.keyPressed(irRight))
                    {
                      setMode(1);
                    }
                    //else if (ir.keyPressed(25)) setMode(-1);
        }  
        else if(mode == 0) //LEFT
        {
                    if(ir.keyPressed(irRight))
                    {
                      setMode(2);
                    }
        }
        else if (mode == 2) //leftToRight
        {
                    if(ir.keyPressed(irLeft)) // cancel of change
                    {
                      setMode(0);  
                    }
                    
                    if(state == 1)  // change done
                    {
                      setMode(1); 
                    }  
        }
        else if (mode == 1) //RIGHT
        {
                    if(ir.keyPressed(irLeft))
                    {
                      setMode(3);
                    }
        }
        else if (mode == 3) //rightToLeft
        {
                    if(ir.keyPressed(irRight)) // cancel of change
                    {
                      setMode(1);
                    }
                    
                    if(state == 2)  // change done
                    {
                      setMode(0);
                    }  
        }

}

void setMotos(int l, int r)
{
  if(motoLeftInverse == false ) { motorLeft.run(l); }   // look global instance
  else                          { motorLeft.run(-l); }

  if(motoRightInverse == false) { motorRight.run(r); }
  else                          { motorRight.run(-r);}

}

void forword (int speedValue)
{
  setMotos(speedValue, speedValue);
}

void poinTurn (int speedValue, bool dir)
{
  double coeff = 0.7;
  if(dir == false) // left
  {
    setMotos(-speedValue*coeff, speedValue*coeff);
  }
  else            // right
  {
    setMotos(speedValue*coeff, -speedValue*coeff);
  }
}

void slalomTurn (int speedValue, bool dir)
{
  double coeff = 0.4
  ;
  if(dir == false) // left
  {
    setMotos(speedValue*coeff, speedValue);  
  }
  else
  {
    setMotos(speedValue, speedValue*coeff);
  }  
}

void stopMotos()  { setMotos(0,0);}


void moveBot(int speedValue)
{
  if(mode == 0) // LEFT
  {
    switch(state)
    {
      case 2 : forword(speedValue); break;
      case 3 : poinTurn(speedValue, true); break;     //right
      case 0 : poinTurn(speedValue, false); break;    //left
      case 1 : slalomTurn(speedValue, false); break;  //left
      default : stopMotos();
    }
  }
  else if (mode == 2) //leftToRight
  {
    switch(state)
    {
      case 2 : slalomTurn(speedValue, true); break;  //right
      case 3 : poinTurn(speedValue, true); break;     //right
      case 0 : slalomTurn(speedValue, true); break;    //right
      case 1 : slalomTurn(speedValue, false); break;  //left
      default : stopMotos();
    }
  }
  else if(mode == 1)  //RIGHT
  {
    switch(state)
    {
      case 1 : forword(speedValue); break;
      case 3 : poinTurn(speedValue, false); break;     //left
      case 0 : poinTurn(speedValue, true); break;    //right
      case 2 : slalomTurn(speedValue, true); break;  //right
      default : stopMotos();
    }  
  }
  else if (mode == 3) //rightToLeft
  {
    switch(state)
    {
      case 2 : slalomTurn(speedValue, true); break;  //rigth
      case 3 : poinTurn(speedValue, false); break;     //left
      case 0 : slalomTurn(speedValue, false); break;    //left
      case 1 : slalomTurn(speedValue, false); break;  //left
      default : stopMotos();
    }
  }
  else if(mode == 4)  //wallLeft
  {
    poinTurn(speedValue, false);     //left
  }
  else if(mode == 5) //wallRight
  {
    poinTurn(speedValue, true);    //right
  }
  else
  {
    stopMotos();
  }
}

void printData()
{
     Serial.print("Mode = ");
     Serial.print(mode);
     Serial.print("\tState = ");
     Serial.println(state);
}



void setup(){
    Serial.begin(9600);
    setMode(-1);
    seg7_4.display(speedOfBot);
}

void loop(){

     ir.loop();
     updateState();
     updateMode();
     moveBot(speedOfBot);
     
     //printData();
     
      
}

