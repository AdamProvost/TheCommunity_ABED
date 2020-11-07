#include<LiquidCrystal.h>
LiquidCrystal lcd(6, 7, 8, 9, 10, 11 );

int trigPin = 13;
int echoPin = 12;
int BJTPin = 4;
int ledPin=3;
int i=1;

long duration, distance;

void setup()
{ 

pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);

pinMode(ledPin,OUTPUT);

digitalWrite(BJTPin, HIGH); // Turn on BJT (current can flow)
}

void loop() 
{ 

while(i){
  lcd.print("   ABED   ");
  delay(2000);
  i = 0;
}

lcd.clear();

digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance = (duration/2)/29.1;

if (distance >= 300){
  lcd.print("Out of range...");
  delay(100);
  }
  
if (distance >= 1 && distance < 300)
{ 
lcd.print("Distance: ");
lcd.print(distance);
lcd.print("cm");                      
}


// RB choice math...
/*
 * IB=3IL/hfe=3*100mA/100 =3.0mA
 * 
 * RB = (5V -1.2V)/3.0mA =1.266kOhm 
 * 
 * 
 */
// Braking logic
if (distance >= 1 && distance < 5)
{ 
  lcd.clear();
  digitalWrite(BJTPin, LOW);
  lcd.print("STOP!!!");
  delay(100);                    
} else {
  digitalWrite(BJTPin, HIGH);
}

delay(40);    

}
