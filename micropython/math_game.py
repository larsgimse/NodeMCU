import random
import time
from machine import Pin

led_green = Pin(14, Pin.OUT)
led_red = Pin(12, Pin.OUT)

while True:
    led_green.off()
    led_red.off()
    number_1 = random.randint(2,12)
    number_2 = random.randint(2,12)
    answer = int(input("What is " + str(number_1) + " * " + str(number_2) + "? "))
    if answer == number_1 * number_2:
        print("Hurray!")
        led_green.on()
        time.sleep(2)
    else:
        print("Wrong! Answer is " + str(number_1 * number_2))
        led_red.on()
        time.sleep(2)
