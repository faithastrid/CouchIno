from typing import Optional
from .motor_controller import MotorController

class QuickDescentLinearController(MotorController):
    __step_size: int
    __descent_multiplier: int
    __center: int

    def __init__(self, initial_value: int, step_size: int, descent_multiplier: int, center: int, closed_loop: Optional[bool]=True, min_cap: Optional[int]=None, max_cap: Optional[int]=None) -> None:
        super().__init__(initial_value=initial_value, closed_loop=closed_loop)
        self.__step_size = step_size
        self.__descent_multiplier = descent_multiplier
        self.__center = center

    def time_step_control(self) -> int:
        if self._closed_loop:
            if self._measured_value < self._desired_value:
                if self._desired_value > self.__center:
                    self._output_value = self._output_value + self.__step_size
                else:
                    self._output_value = self._output_value + self.__descent_multiplier * self.__step_size
            elif self._measured_value > self._desired_value:
                if self._desired_value < self.__center:
                    self._output_value = self._output_value - self.__step_size
                else:
                    self._output_value = self._output_value - self.__descent_multiplier * self.__step_size
        else:
            if self._output_value < self._desired_value:
                if self._output_value > self.__center:
                    self._output_value = self._output_value + self.__step_size
                else:
                    self._output_value = self._output_value + self.__descent_multiplier * self.__step_size
            elif self._output_value > self._desired_value:
                if self._output_value < self.__center:
                    self._output_value = self._output_value - self.__step_size
                else:
                    self._output_value = self._output_value - self.__descent_multiplier * self.__step_size
        return self._output_value