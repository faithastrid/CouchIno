from gpiozero import DigitalOutputDevice
from serial import Serial
from typing import Optional, Union

# TODO: add 50 us wait btwn pin going high and changing motor speed

class Sabertooth2x60(object):
    __serial: Serial
    __serial_format: int
    __select_pin: DigitalOutputDevice
    __address: int
    __max_speed: int
    __motor_one_speed: int
    __motor_two_speed: int

    def __init__(
        self,
        port: str,
        baudrate: int,
        serial_format: Optional[str]=None,
        select_pin: Optional[Union[int, str]]=None,
        address: Optional[int]=None,
        max_speed: Optional[int]=None
    ) -> None:
        if baudrate not in [2400, 9600, 19200, 38400, 115200]:
            raise ValueError
        self.__serial = Serial(port=port, baudrate=baudrate, timeout=1, write_timeout=1)
        if serial_format == "standard" or serial_format == None:
            self.__serial_format = 0
        elif serial_format == "slave_select":
            self.__serial_format = 1
            if select_pin == None:
                raise ValueError
            self.__select_pin = DigitalOutputDevice(pin=select_pin, active_high=True, initial_value=False)
        elif serial_format == "packetized":
            self.__serial_format = 2
            if address == None or not (0 <= address <= 255):
                raise ValueError
            self.__address = address
        else:
            raise ValueError
        self.__max_speed = int(max_speed) if max_speed != None else 100
        self.__motor_one_speed = 0
        self.__motor_two_speed = 0

    @property
    def max_speed(self) -> int:
        return self.__max_speed

    @property
    def motor_one_speed(self) -> int:
        return self.__motor_one_speed

    @property
    def motor_two_speed(self) -> int:
        return self.__motor_two_speed

    def set_motor_one_speed(self, speed: int) -> bool:
        # try:
            if not (abs(speed) <= self.__max_speed):
                raise ValueError
            if self.__serial_format == 0:
                success = self.__set_motor_one_speed_standard(speed=speed)
            elif self.__serial_format == 1:
                success = self.__set_motor_one_speed_slave_select(speed=speed)
            elif self.__serial_format == 2:
                success = self.__set_motor_one_speed_packetized(speed=speed)
            else:
                raise ValueError
            self.__motor_one_speed = speed
            return success
        # except Exception as e:
        #     print(f"Error {e} occurred")
        #     return False

    def __set_motor_one_speed_standard(self, speed: int) -> bool:
        self.__serial.write(data=int((64 if speed < 0 else 63) * speed / self.__max_speed + 64))
        return True

    def __set_motor_one_speed_slave_select(self, speed: int) -> bool:
        # TODO: wait 50 us
        self.__select_pin.on()
        self.__serial.write(data=int((64 if speed < 0 else 63) * speed / self.__max_speed + 64))
        # TODO: wait 50 us
        self.__select_pin.off()
        return True

    def __set_motor_one_speed_packetized(self, speed: int) -> bool:
        command = 0 if speed >= 0 else 1
        formatted_speed = int(127 * abs(speed) / self.__max_speed)
        packet = [self.__address, command, formatted_speed, (self.__address + command + formatted_speed) & 127]
        self.__serial.write(data=bytearray(packet))
        return True

    def set_motor_two_speed(self, speed: int) -> bool:
        # try:
            if not (abs(speed) <= self.__max_speed):
                raise ValueError
            if self.__serial_format == 0:
                success = self.__set_motor_two_speed_standard(speed=speed)
            elif self.__serial_format == 1:
                success = self.__set_motor_two_speed_slave_select(speed=speed)
            elif self.__serial_format == 2:
                success = self.__set_motor_two_speed_packetized(speed=speed)
            else:
                raise ValueError
            self.__motor_two_speed = speed
            return success
        # except Exception as e:
        #     print(f"Error {e} occurred")
        #     return False

    def __set_motor_two_speed_standard(self, speed: int) -> bool:
        self.__serial.write(data=int((64 if speed < 0 else 63) * speed / self.__max_speed + 192))
        return True

    def __set_motor_two_speed_slave_select(self, speed: int) -> bool:
        # TODO: wait 50 us
        self.__select_pin.on()
        self.__serial.write(data=int((64 if speed < 0 else 63) * speed / self.__max_speed + 192))
        # TODO: wait 50 us
        self.__select_pin.off()
        return True

    def __set_motor_two_speed_packetized(self, speed: int) -> bool:
        command = 4 if speed >= 0 else 5
        formatted_speed = int(127 * abs(speed) / self.__max_speed)
        packet = [self.__address, command, formatted_speed, (self.__address + command + formatted_speed) & 127]
        self.__serial.write(data=bytearray(packet))
        return True

    def set_both_motors_speed(self, speed: int) -> bool:
        # try:
            if not (abs(speed) <= self.__max_speed):
                raise ValueError
            if self.__serial_format == 0:
                success = self.__set_both_motors_speed_standard(speed=speed)
            elif self.__serial_format == 1:
                success = self.__set_both_motors_speed_slave_select(speed=speed)
            elif self.__serial_format == 2:
                success = self.__set_both_motors_speed_packetized(speed=speed)
            else:
                raise ValueError
            self.__motor_one_speed = speed
            self.__motor_two_speed = speed
            return success
        # except Exception as e:
        #     print(f"Error {e} occurred")
        #     return False

    def __set_both_motors_speed_standard(self, speed: int) -> bool:
        self.__serial.write(data=int((64 if speed < 0 else 63) * speed / self.__max_speed + 64))
        self.__serial.write(data=int((64 if speed < 0 else 63) * speed / self.__max_speed + 192))
        return True

    def __set_both_motors_speed_slave_select(self, speed: int) -> bool:
        # TODO: wait 50 us
        self.__select_pin.on()
        self.__serial.write(data=int((64 if speed < 0 else 63) * speed / self.__max_speed + 64))
        self.__serial.write(data=int((64 if speed < 0 else 63) * speed / self.__max_speed + 192))
        # TODO: wait 50 us
        self.__select_pin.off()

    def __set_both_motors_speed_packetized(self, speed: int) -> bool:
        motor_one_command = 0 if speed >= 0 else 1
        motor_two_command = 4 if speed >= 0 else 5
        motor_one_formatted_speed = int(127 * abs(speed) / self.__max_speed)
        motor_two_formatted_speed = int(127 * abs(speed) / self.__max_speed)
        motor_one_packet = [self.__address, motor_one_command, motor_one_formatted_speed, (self.__address + motor_one_command + motor_one_formatted_speed) & 127]
        motor_two_packet = [self.__address, motor_two_command, motor_two_formatted_speed, (self.__address + motor_two_command + motor_two_formatted_speed) & 127]
        self.__serial.write(data=bytearray(motor_one_packet))
        self.__serial.write(data=bytearray(motor_two_packet))
        return True

    def stop_motors(self) -> bool:
        # try:
            if self.__serial_format == 0:
                success = self.__stop_motors_standard()
            elif self.__serial_format == 1:
                success = self.__stop_motors_slave_select()
            elif self.__serial_format == 2:
                success = self.__stop_motors_packetized()
            else:
                raise ValueError
            self.__motor_one_speed = 0
            self.__motor_two_speed = 0
            return success
        # except Exception as e:
        #     print(f"Error {e} occurred")
        #     return False

    def __stop_motors_standard(self) -> bool:
        self.__serial.write(0)
        return True

    def __stop_motors_slave_select(self) -> bool:
        # TODO: wait 50 us
        self.__select_pin.on()
        self.__serial.write(0)
        # TODO: wait 50 us
        self.__select_pin.off()
        return True

    def __stop_motors_packetized(self) -> bool:
        motor_one_packet = [self.__address, 0, 0, (self.__address) & 127]
        motor_two_packet = [self.__address, 4, 0, (self.__address + 4) & 127]
        self.__serial.write(data=bytearray(motor_one_packet))
        self.__serial.write(data=bytearray(motor_two_packet))
        return True

    def kill(self) -> bool:
        # try:
            self.stop_motors()
            self.__serial.close()
            return True
        # except Exception as e:
        #     print(f"Error {e} occurred")
        #     return False