from threading import Thread
from classes import Sabertooth2x60

class KillTest(Thread):

    def run(self) -> None:
        port = "/dev/ttyS0"
        baudrate = 9600

        left_motor = Sabertooth2x60(port=port, baudrate=baudrate, serial_format="packetized", address=128, max_speed=100)
        right_motor = Sabertooth2x60(port=port, baudrate=baudrate, serial_format="packetized", address=129, max_speed=100)

        left_motor.stop_motors()
        right_motor.stop_motors()