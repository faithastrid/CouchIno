from threading import Thread
from time import sleep
from classes import Sabertooth2x60

class DriveTest(Thread):

    def run(self) -> None:
        port = "/dev/ttyS0"
        baudrate = 9600

        left_motor = Sabertooth2x60(port=port, baudrate=baudrate, serial_format="packetized", address=128, max_speed=100)
        right_motor = Sabertooth2x60(port=port, baudrate=baudrate, serial_format="packetized", address=129, max_speed=100)

        left_motor.set_motor_one_speed(50)
        sleep(2)
        left_motor.set_motor_one_speed(0)

        left_motor.set_motor_two_speed(-50)
        sleep(2)
        left_motor.set_motor_two_speed(0)

        right_motor.set_motor_one_speed(50)
        sleep(2)
        right_motor.set_motor_one_speed(0)
        
        right_motor.set_motor_two_speed(-50)
        sleep(2)
        right_motor.set_motor_two_speed(0)

        left_motor.set_both_motors_speed(50)
        right_motor.set_both_motors_speed(-50)
        sleep(2)

        left_motor.stop_motors()
        right_motor.stop_motors()

