from threading import Thread
from time import sleep
from typing import Optional

from .motor_controllers import (FixedJerkController, IntegralController, LinearController, MotorController, QuickDescentLinearController)
from .game_controllers import (GameController, LogitechGamepadF310)
from .motors import Sabertooth2x60

class CouchBot(object):
    __capped_speed: int
    __zero_cushion: int
    __time_step: float
    __game_controller: GameController
    __left_motor: Sabertooth2x60
    __right_motor: Sabertooth2x60
    __left_motor_controller: MotorController
    __right_motor_controller: MotorController
    __motor_thread: Thread

    def __init__(self,
        port: str,
        baudrate: int,
        initial_motor_value: int,
        left_motor_address: int,
        right_motor_address: int,
        event_device: str,
        max_speed: Optional[int]=100,
        capped_speed: Optional[int]=None,
        zero_cushion: Optional[int]=None,
        motor_controller: Optional[str]=None,
        fixed_jerk: Optional[int]=None,
        integral_coefficient: Optional[float]=None,
        linear_step_size: Optional[int]=None,
        descent_multiplier: Optional[int]=None,
        center: Optional[int]=None,
        closed_loop: Optional[bool]=True,
        time_step: Optional[float]=None
    ) -> None:
        input_functions = {
            1 : self.__left_motor_event,
            5 : self.__right_motor_event
        }
        self.__game_controller = LogitechGamepadF310(event_device=event_device, codes=input_functions)
        self.__left_motor = Sabertooth2x60(port=port, baudrate=baudrate, serial_format="packetized", address=left_motor_address, max_speed=max_speed)
        self.__right_motor = Sabertooth2x60(port=port, baudrate=baudrate, serial_format="packetized", address=right_motor_address, max_speed=max_speed)
        if capped_speed > max_speed:
            raise ValueError
        elif capped_speed == None:
            self.__capped_speed = max_speed
        else:
            self.__capped_speed = capped_speed
        if motor_controller == "fixed_jerk":
            if fixed_jerk == None:
                raise ValueError
            self.__left_motor_controller = FixedJerkController(initial_value=initial_motor_value, fixed_jerk=fixed_jerk, closed_loop=closed_loop)
            self.__right_motor_controller = FixedJerkController(initial_value=initial_motor_value, fixed_jerk=fixed_jerk, closed_loop=closed_loop)
        elif motor_controller == "integral":
            if integral_coefficient == None:
                raise ValueError
            self.__left_motor_controller = IntegralController(initial_value=initial_motor_value, coefficient=integral_coefficient, closed_loop=closed_loop)
            self.__right_motor_controller = IntegralController(initial_value=initial_motor_value, coefficient=integral_coefficient, closed_loop=closed_loop)
        elif motor_controller == "linear":
            if linear_step_size == None:
                raise ValueError
            self.__left_motor_controller = LinearController(initial_value=initial_motor_value, step_size=linear_step_size, closed_loop=closed_loop)
            self.__right_motor_controller = LinearController(initial_value=initial_motor_value, step_size=linear_step_size, closed_loop=closed_loop)
        elif motor_controller == "quick_descent_linear":
            if linear_step_size == None or descent_multiplier == None or center == None:
                raise ValueError
            self.__left_motor_controller = QuickDescentLinearController(initial_value=initial_motor_value, step_size=linear_step_size, descent_multiplier=descent_multiplier, center=center, closed_loop=closed_loop)
            self.__right_motor_controller = QuickDescentLinearController(initial_value=initial_motor_value, step_size=linear_step_size, descent_multiplier=descent_multiplier, center=center, closed_loop=closed_loop)
        elif motor_controller == None:
            self.__left_motor_controller = MotorController(initial_value=initial_motor_value, closed_loop=closed_loop)
            self.__right_motor_controller = MotorController(initial_value=initial_motor_value, closed_loop=closed_loop)
        self.__motor_thread = Thread(target=self.__motor_loop)
        self.__zero_cushion = zero_cushion if zero_cushion != None else 15
        self.__time_step = time_step if time_step != None else 0.1
        self.__motor_thread.start()

    def __left_motor_event(self, value: int) -> None:
        self.__left_motor_controller.desired_value = value

    def __right_motor_event(self, value: int) -> None:
        self.__right_motor_controller.desired_value = value

    def _left_joystick_y(self, value: int) -> None:
        if value < 0:
            value = 0
        elif value > 255:
            value = 255
        if 127 - self.__zero_cushion < value < 127 + self.__zero_cushion:
            self.__left_motor.stop_motors()
        elif value < 127:
            self.__left_motor.set_both_motors_speed(speed=int(self.__capped_speed * (127 - self.__zero_cushion - 1 - value) / (127 - self.__zero_cushion - 1)))
        else:
            self.__left_motor.set_both_motors_speed(speed=int(-self.__capped_speed * (value - 127 + self.__zero_cushion) / (255 - 127 + self.__zero_cushion)))

    def _right_joystick_y(self, value: int) -> None:
        if value < 0:
            value = 0
        elif value > 255:
            value = 255
        if 127 - self.__zero_cushion < value < 127 + self.__zero_cushion:
            self.__right_motor.stop_motors()
        elif value < 127:
            self.__right_motor.set_both_motors_speed(speed=int(self.__capped_speed * (127 - self.__zero_cushion - 1 - value) / (127 - self.__zero_cushion - 1)))
        else:
            self.__right_motor.set_both_motors_speed(speed=int(-self.__capped_speed * (value - 127 + self.__zero_cushion) / (255 - 127 + self.__zero_cushion)))

    def __motor_loop(self):
        while True:
            left_output = self.__left_motor_controller.time_step_control()
            right_output = self.__right_motor_controller.time_step_control()
            self._left_joystick_y(value=left_output)
            self._right_joystick_y(value=right_output)
            sleep(self.__time_step)
