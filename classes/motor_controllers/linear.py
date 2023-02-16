from typing import Optional
from .motor_controller import MotorController

class LinearController(MotorController):
    __step_size: int

    def __init__(self, initial_value: int, step_size: int, closed_loop: Optional[bool]=True) -> None:
        super().__init__(initial_value=initial_value, closed_loop=closed_loop)
        self.__step_size = step_size

    def time_step_control(self) -> int:
        if self._closed_loop:
            if self._measured_value < self._desired_value:
                self._output_value = self._output_value + self.__step_size
            elif self._measured_value > self._desired_value:
                self._output_value = self._output_value - self.__step_size
        else:
            if self._output_value < self._desired_value:
                self._output_value = self._output_value + self.__step_size
            elif self._output_value > self._desired_value:
                self._output_value = self._output_value - self.__step_size
        return self._output_value