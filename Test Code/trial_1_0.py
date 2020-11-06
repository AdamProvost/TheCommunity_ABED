import time
from Led import *
from Motor import *
from Ultrasonic import *
from servo import *

led=Led()
PWM=Motor()
ultrasonic=Ultrasonic()
servo=Servo()

def blink_at_dist():
    try:
        while True:
            servo.setServoPwm('0',100)
            servo.setServoPwm('1',120)
            PWM.setMotorModel(-1000,-1000,-1000,-1000)       #Forward
            print ("The car is moving forward")
            data = ultrasonic.get_distance()    # capture distance data
            print("distance = " + str(data) + "cm")
            if (data >= 40):
                print("minimal danger")
                led.ledIndex(0x01,0,255,0)      #green
                led.ledIndex(0x02,0,255,0)    
                led.ledIndex(0x04,0,255,0)    
                led.ledIndex(0x08,0,255,0)     
                led.ledIndex(0x10,0,255,0)  
                led.ledIndex(0x20,0,255,0)  
                led.ledIndex(0x40,0,255,0)    
                led.ledIndex(0x80,0,255,0)  
            elif (data < 40 and data >= 20):
                print("moderate danger")
                led.ledIndex(0x01,255,125,0)      #orange
                led.ledIndex(0x02,255,125,0)    
                led.ledIndex(0x04,255,125,0)    
                led.ledIndex(0x08,255,125,0)     
                led.ledIndex(0x10,255,125,0)    
                led.ledIndex(0x20,255,125,0)      
                led.ledIndex(0x40,255,125,0)    
                led.ledIndex(0x80,255,125,0)  
            elif (data < 20):
                print("immediate danger")
                PWM.setMotorModel(0,0,0,0)
                led.ledIndex(0x01,255,0,0)      #Red
                led.ledIndex(0x02,255,0,0)    
                led.ledIndex(0x04,255,0,0)    
                led.ledIndex(0x08,255,0,0)      
                led.ledIndex(0x10,255,0,0)    
                led.ledIndex(0x20,255,0,0)      
                led.ledIndex(0x40,255,0,0)    
                led.ledIndex(0x80,255,0,0)
            #led.colorWipe(led.strip, Color(0,0,0))  #turn off the lights
            time.sleep(1)
    except KeyboardInterrupt:
        led.colorWipe(led.strip, Color(0,0,0))  #turn off the lights
        PWM.setMotorModel(0,0,0,0)
        print("\n\n Ending Program \n\n")


          
# Main program logic follows:
if __name__ == '__main__':

    print ('Program is starting ... ')
    import sys
    
    if sys.argv[1] == 'Run':
        blink_at_dist()