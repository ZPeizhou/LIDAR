/*declaring-------------------------------------------------------------------------------------*/

/* FOR ADJUSTMENT*/
int posThetaGain = 20;       //the rotation of servo motor
float posPhiGain = 11.25;   //the rotation of stepper motor
                            //! make sure it's an int after *256/180 !
int ThetaRANGE = 100;       //the detecting range of Theta VERTICAL (servo motor)
float PhiRANGE = 112.5;     //the detecting range of Phi HORIZONTAL(stepper mootor)
/* FOR ADJUSTMENT*/         //! make sure it's an int after /posPhiGain !

//control of the motors
const int servoPin = 10;      //pin of the servo
float posTheta = 0;          //the position of servo motor
float posPhi = 0;            //the position of stepper motor
int posTheta_initial = 0;    //the initial position of servo
//LIDAR reading
int dist;
int strength;
int check;
int uart[9];
int i;
const int HEADER=0x59;
bool ojbk = 1;               //whether the LIDAR has successfully read a data
String clearReceived;        //used to clear old data
//working status indicator: whether initializing or working
int led1 = 23;               //orange: adjusting stepper motor
int led2 = 25;               //blue: adjusting servo
int led3 = 27;               //red: LIDAR working
int t = 0;
/*-------------------------------------------------------------------------------------declaring*/

/*control functions of motors-------------------------------------------------------------------*/
void servo(int angle)
{  for(int i=0;i<50;i++)
   {  int pulsewidth = (angle * 11) + 500;
      digitalWrite(servoPin, HIGH);   
      delayMicroseconds(pulsewidth);
      digitalWrite(servoPin, LOW);
      delayMicroseconds(20000 - pulsewidth);
   }
   delay(100);
}//homemade function to replace <servo.h> to solve the colision with the LIDAR
void anticlockwise(int num) 
{  for (int count = 0; count < num; count++)
   {  for (int i = 2; i < 6; i++)
      {  digitalWrite(i, HIGH);
         delay(3);
         digitalWrite(i, LOW);}}
}//the control of the stepper motor(anti-clockwise)
void clockwise(int num)
{  for (int count = 0; count < num; count++)
   {  for (int i = 5; i > 1; i--)
      {  digitalWrite(i, HIGH);
         delay(3);
         digitalWrite(i, LOW);}}
}//the control of the stepper motor(clockwise)
void anticlockwise_test(int num) 
{  for (int count = 0; count < num; count++)
   {  for (int i = 2; i < 6; i++)
      {  digitalWrite(i, HIGH);
         delay(20);
         digitalWrite(i, LOW);}}
}//the control of the stepper motor(anti-clockwise) when initialing
void clockwise_test(int num) 
{  for (int count = 0; count < num; count++)
   {  for (int i = 5; i > 1; i--)
      {  digitalWrite(i, HIGH);
         delay(20);
         digitalWrite(i, LOW);}}
}//the control of the stepper motor(clockwise) when initialing
/*-------------------------------------------------------------------control functions of motors*/

/*functions for LIDAR---------------------------------------------------------------------------*/
void LIDAR_read(float posTheta, float posPhi) //the LIDAR takes 3 mearsurements and get an average
{
   while (ojbk)
   {
      if (Serial1.available())//check whether the serial port has data input
      {
         for (int k=1; k<=10; k++)
         {
          clearReceived = String(clearReceived) + String(char(Serial1.read()));
          delay(1);
         }
         clearReceived = "";
         if(Serial1.read()==HEADER)//determine data package frame header 0x59
         {  
            delay(10);
            uart[0]=HEADER;
            if(Serial1.read()==HEADER)//determine datapackage frame header 0x59
            {
               uart[1]=HEADER;
               for(i=2;i<9;i++)//store data to array
               {
                  uart[i]=Serial1.read();
               } 
               check=uart[0]+uart[1]+uart[2]+uart[3]+uart[4]+uart[5]+uart[6]+uart[7];
               if(uart[8]==(check&0xff))//check the received data as per protocols 
               {
                  dist=uart[2]+uart[3]*256;//calculate distance value
                  strength=uart[4]+uart[5]*256;// calculate signal strength value 
                  Serial.print("dist = ");
                  Serial.print(dist);
                  Serial.print("\t");
                  Serial.print("Stre = ");
                  Serial.print(strength);
                  Serial.print("\t");
                  Serial.print("theta = ");
                  Serial.print(posTheta);
                  Serial.print("\t");
                  Serial.print("phi = ");
                  Serial.print(posPhi*180/256); //real Phi angle
                  Serial.print('\n');
                  delay(10);
                  ojbk = 0;
                }
             }
          }
      }
   }       
}
/*---------------------------------------------------------------------------functions for LIDAR*/

/*functions run once----------------------------------------------------------------------------*/
void setup() 
{  
   pinMode(7,INPUT);          //switcher
   pinMode(led1,OUTPUT);      //orange
   pinMode(led2,OUTPUT);      //blue
   pinMode(led3,OUTPUT);      //red
   pinMode(servoPin, OUTPUT); //servo is at PIN6
   for (int i=2;i<6;i++)      //stepper motor
   {pinMode(i,OUTPUT);}
   Serial.begin(9600);        // the arduino system at 9600 bps
   Serial1.begin(11500);   //open the serial port for measurement at 115200 bps
}
/*----------------------------------------------------------------------------functions run once*/

/*main loop-------------------------------------------------------------------------------------*/
void loop()
{
   int n = digitalRead(7); //read of the switcher
   if (n == HIGH) //begin initialization
   {delay(500);t = t+1;n = 0;}
//orange light on: initializing the stepper motor
   while (t==1) 
   {
     digitalWrite(led1,HIGH);n =digitalRead(7);
     if (n == HIGH)
      {delay(500);t = t+1;digitalWrite(led1,LOW);} //exit
     if (analogRead(0) < 500)
      {anticlockwise_test((500-analogRead(0))*5/1023);}
     if (analogRead(0) > 700 )
      {clockwise_test(analogRead(0)*5/1023);}
    }    
//blue light on:  initializing the servo
    while (t == 2) 
    {
      digitalWrite(led2,HIGH);n =digitalRead(7);
      if (n == HIGH)
      {delay(500);t = t+1;digitalWrite(led2,LOW);} //exit
      posTheta_initial = map(analogRead(0),0,1023,0,179);
      servo(posTheta_initial); //set the servo to initial position
      delay(100); //make the beginning of the LIDAR a long press
    }
//LIDAR is functionning
    while (t == 3) 
    { digitalWrite(led3,HIGH);
      delay(1000);
      digitalWrite(led3,LOW);
      PhiRANGE = PhiRANGE*256/180; //convert the Phi angle to stepper motor STEPs
      posPhiGain = posPhiGain*256/180; //convert the Phi angle to stepper motor STEPs
      for (posTheta = 0; posTheta <= ThetaRANGE; posTheta += posThetaGain)
      {
         servo(posTheta+posTheta_initial);
         delay(2000);
         for (posPhi = 0; posPhi < PhiRANGE; posPhi += posPhiGain)          
         {   
            LIDAR_read(posTheta, posPhi);
            delay (200);
            ojbk = 1;
            clockwise(posPhiGain);
         }
         LIDAR_read(posTheta, posPhi);
         delay (200);
         ojbk = 1;
         anticlockwise(PhiRANGE); //put the stepper motor to initial position
       }
       t = 0;//exit
       digitalWrite(led3,LOW);
       PhiRANGE = PhiRANGE*180/256; //convert back
       posPhiGain = posPhiGain*180/256; //convert back
    }
}
/*-------------------------------------------------------------------------------------main loop*/
