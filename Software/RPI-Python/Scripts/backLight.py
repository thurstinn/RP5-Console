import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setup(18, GPIO.OUT)
GPIO.setup(5, GPIO.IN, pull_up_down=GPIO.PUD_UP)

pwm = GPIO.PWM(18, 200) 
mode = 1
pwm.start(75)

def change_mode(channel):
    global mode
    mode += 1
    if mode == 6:
        mode = 1

GPIO.add_event_detect(5, GPIO.FALLING, callback=change_mode, bouncetime=300)

try:
    while True:
        if mode == 1:
            pwm.ChangeDutyCycle(25)
        elif mode == 2:
            pwm.ChangeDutyCycle(10)
        elif mode == 3:
            pwm.ChangeDutyCycle(100)
        elif mode == 4:
            pwm.ChangeDutyCycle(75)
        elif mode == 5:
            pwm.ChangeDutyCycle(50)
        time.sleep(0.1)
except KeyboardInterrupt:
    pwm.stop()
    GPIO.cleanup()        
        
    

