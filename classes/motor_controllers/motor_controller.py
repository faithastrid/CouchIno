from typing import Optional

class MotorController(object):
    _output_value: int
    _measured_value: int
    _desired_value: int
    _closed_loop: bool
    _min_cap: int
    _max_cap: int

    def __init__(self, initial_value: int, closed_loop: Optional[bool]=True) -> None:
        self._output_value = initial_value
        self._measured_value = initial_value
        self._desired_value = initial_value
        self._closed_loop = closed_loop

    @property
    def output_value(self) -> int:
        return self._output_value

    @property
    def desired_value(self) -> int:
        return self._desired_value

    @desired_value.setter
    def desired_value(self, desired_value: int) -> None:
        self._desired_value = desired_value

    def time_step_control(self) -> int:
        self._output_value = self._desired_value
        return self._output_value