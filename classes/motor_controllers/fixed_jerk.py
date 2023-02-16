from typing import Optional
from .motor_controller import MotorController

class FixedJerkController(MotorController):
    __fixed_jerk: int
    __acceleration: int

    def __init__(self, initial_value: int, fixed_jerk: int, closed_loop: Optional[bool]=True) -> None:
        super().__init__(initial_value=initial_value, closed_loop=closed_loop)
        self.__fixed_jerk = fixed_jerk

    def time_step_control(self) -> int:
        if self._closed_loop:
            if self._desired_value > self._measured_value:
                self.__acceleration = self.__acceleration + self.__fixed_jerk
            elif self._desired_value < self._measured_value:
                self.__acceleration = self.__acceleration - self.__fixed_jerk
            self._output_value = self._output_value + self.__acceleration
        else:
            if self._desired_value > self._output_value:
                self.__acceleration = self.__acceleration + self.__fixed_jerk
            elif self._desired_value < self._output_value:
                self.__acceleration = self.__acceleration - self.__fixed_jerk
            self._output_value = self._output_value + self.__acceleration
        return self._output_value