
/* FOR ADJUSTMENT*/
int posThetaGain = 1;      //the rotation of servo motor
float posPhiGain = 1;   //the rotation of stepper motor
                            //! make sure it's an int after *256/180 !
int ThetaRANGE = 40;       //the detecting range of Theta VERTICAL (servo motor)
float PhiRANGE = 128;     //the detecting range of Phi HORIZONTAL(stepper mootor)
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
int led3 = 27;               //red: LIDAR working

void setup() 
{
   pinMode(servoPin, OUTPUT); //servo is at PIN6
   for (int i=2;i<6;i++) //stepper motor
   {pinMode(i,OUTPUT);}
   Serial.begin(9600); //set the Baud rate of Arduino and computer serial port
   Serial1.begin(115200); //set the Baud rate of LiDAR and Arduino serial port
}

void anticlockwise(int num) 
{  for (int count = 0; count < num; count++)
   {  for (int i = 2; i < 6; i++)
      {  digitalWrite(i, HIGH);
         delay(3);
         digitalWrite(i, LOW);}}
}
void clockwise(int num)
{  for (int count = 0; count < num; count++)
   {  for (int i = 5; i > 1; i--)
      {  digitalWrite(i, HIGH);
         delay(3);
         digitalWrite(i, LOW);}}
}
void servo(int angle)
{  for(int i=0;i<50;i++)
   {  int pulsewidth = (angle * 11) + 500;
      digitalWrite(servoPin, HIGH);   
      delayMicroseconds(pulsewidth);
      digitalWrite(servoPin, LOW);
      delayMicroseconds(20000 - pulsewidth);
   }
   delay(100);
}

void LIDAR_read(float posTheta, float posPhi)
{
   while(ojbk)
   {
      if (Serial1.available())  //check whether the serial port has data input
      {
         for (int k=1; k<=15; k++)
         {
          clearReceived = String(clearReceived) + String(char(Serial1.read()));
          delay(1);
         }
         clearReceived = "";
         if(Serial1.read()==HEADER)  //determine data package frame header 0x59
         {
            delay(10);
            uart[0]=HEADER;
            if(Serial1.read()==HEADER)  //determine datapackage frame header 0x59
            {
               uart[1]=HEADER;
               for(i=2;i<9;i++)  //store data to array
               {
                  uart[i]=Serial1.read();
               }
               check=uart[0]+uart[1]+uart[2]+uart[3]+uart[4]+uart[5]+uart[6]+uart[7];
               if(uart[8]==(check&0xff))  //check the received data as per protocols
               {if (uart[2]+uart[3]*256 < 60) 
               {
                  dist=uart[2]+uart[3]*256;  //calculate distance value
                  strength=uart[4]+uart[5]*256;  //calculate signal strength value 
                  //Serial.print("dist = ");
                  Serial.println(dist);  //output LiDAR testsdistance value 
                  //Serial.print('\t');
                  //Serial.print("strength = ");
                  //Serial.print(strength);  //output signal strength value
                  //Serial.print("\t");
                  //Serial.print("theta = ");
                  Serial.println(posTheta);
                  //Serial.print("\t");
                  //Serial.print("phi = ");
                  Serial.println(posPhi*180/256); //convert the steps into Phi angle
                  //Serial.print('\n');
                  
                  delay(10);
                  ojbk = 0;//ojbk = 0 means the LIDAR has output a successful read
                }
                }
             }
           }
        }
     }
 }

void loop() 
{  
   digitalWrite(led3,HIGH);
   //PhiRANGE = PhiRANGE*256/180; //convert the Phi angle to stepper motor STEPs
   //posPhiGain = posPhiGain*256/180;
   for (posTheta = 0; posTheta <= ThetaRANGE; posTheta += posThetaGain)
   {
      posTheta_initial = map(analogRead(0),0,1023,0,179);
      servo(posTheta_initial-20+posTheta);
      delay(2000);
      for (posPhi = 0; posPhi < PhiRANGE; posPhi += posPhiGain)          
      {
         LIDAR_read(posTheta, posPhi);
         delay(200);
         clockwise(posPhiGain);
         delay(2000);
         ojbk = 1;
      }
      LIDAR_read(posTheta, posPhi);
      ojbk = 1;
      delay(200);
      anticlockwise(PhiRANGE);
      delay (200);
   }
   //PhiRANGE = PhiRANGE*180/256; //convert back
   //posPhiGain = posPhiGain*180/256; //convert back
}     
