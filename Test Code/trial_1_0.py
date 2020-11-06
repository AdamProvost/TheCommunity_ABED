##############################
#
#	This program will run your RoboCar continuously straight forward until
#	it is within 20cm of an obstacle (so make sure you have some room when running),
#	it will then send the signal to stop all motors and will display LED colors 
#	in accordance to obstance emergency (Green = safe, oragne = warning, red = danger)
#
#
#	I know his isn't the power intervention method we talked about,
#	but it's a demonstration of connecting the motor, ultrasonic sensor, 
#	and LEDs into one working program
#
#	When running this program the file must be included in the /Freenove/Code/Server
#	directory to run correctly; and the command to run should be:
#
#			sudo python trial_1_0.py Run
#
##############################

# importing libraries
import time
from Led import *
from Motor import *
from Ultrasonic import *
from servo import *

#creating local instances
led=Led()
PWM=Motor()
ultrasonic=Ultrasonic()
servo=Servo()

# testing method
def test_interfacing():
    try:
        while True:
            servo.setServoPwm('0',100)		# set servos to face forward
            servo.setServoPwm('1',120)
            PWM.setMotorModel(-1000,-1000,-1000,-1000)  # start motors in Forward direction
            print ("The car is moving forward")
            data = ultrasonic.get_distance()    # capture distance data
            print("distance = " + str(data) + "cm")		# print current distance
            if (data >= 40):
                print("minimal danger")		# its in safe distance
                led.ledIndex(0x01,0,255,0)      # display green LEDs
                led.ledIndex(0x02,0,255,0)    
                led.ledIndex(0x04,0,255,0)    
                led.ledIndex(0x08,0,255,0)     
                led.ledIndex(0x10,0,255,0)  
                led.ledIndex(0x20,0,255,0)  
                led.ledIndex(0x40,0,255,0)    
                led.ledIndex(0x80,0,255,0)  
            elif (data < 40 and data >= 20):	# in range for warning
                print("moderate danger")
                led.ledIndex(0x01,255,125,0)      # display orange LEDs
                led.ledIndex(0x02,255,125,0)    
                led.ledIndex(0x04,255,125,0)    
                led.ledIndex(0x08,255,125,0)     
                led.ledIndex(0x10,255,125,0)    
                led.ledIndex(0x20,255,125,0)      
                led.ledIndex(0x40,255,125,0)    
                led.ledIndex(0x80,255,125,0)  
            elif (data < 20):
                print("immediate danger")		# its less than 20cm away
                PWM.setMotorModel(0,0,0,0)		# stop wheels/motors
                led.ledIndex(0x01,255,0,0)      # display Red LEDs
                led.ledIndex(0x02,255,0,0)    
                led.ledIndex(0x04,255,0,0)    
                led.ledIndex(0x08,255,0,0)      
                led.ledIndex(0x10,255,0,0)    
                led.ledIndex(0x20,255,0,0)      
                led.ledIndex(0x40,255,0,0)    
                led.ledIndex(0x80,255,0,0)
            time.sleep(1)	# sleep for 1ms for consistent Ultrasonic readings
    except KeyboardInterrupt:
        led.colorWipe(led.strip, Color(0,0,0))  #turn off the lights
        PWM.setMotorModel(0,0,0,0)		# stop motors before quitting
        print("\n\n Ending Program \n\n")	# print end


          
# Main program logic follows:
if __name__ == '__main__':

    print ('Program is starting ... ')
    import sys
    
    if sys.argv[1] == 'Run':	# specify Run when executing this program (other 
    								# implementations can be added later)
        test_interfacing()		# break to test method