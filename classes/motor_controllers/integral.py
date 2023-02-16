from typing import Optional
from .motor_controller import MotorController

class IntegralController(MotorController):
    __coefficient: float
    __error_sum: int

    def __init__(self, initial_value: int, coefficient: float, closed_loop: Optional[bool]=True) -> None:
        super().__init__(initial_value=initial_value, closed_loop=closed_loop)
        self.__coefficient = coefficient

    def time_step_control(self) -> int:
        if self._closed_loop:
            self.__error_sum = self.__error_sum + self.__coefficient * (self._measured_value - self._desired_value)
            self._output_value = self._output_value + self.__error_sum
        else:
            self.__error_sum = self.__error_sum + self.__coefficient * (self._output_value - self._desired_value)
            self._output_value = self._output_value + self.__error_sum
        return self._output_value