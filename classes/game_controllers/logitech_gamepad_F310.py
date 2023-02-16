from typing import (Callable, Dict)
from .game_controller import GameController

LOGITECH_GAMEPAD_F310_CODES = [288, 289, 290, 291, 16, 17, 298, 3, 5, 299, 0, 1, 292, 293, 294, 295, 296, 297]

class LogitechGamepadF310(GameController):
    def __init__(self, event_device: str, codes: Dict[int, Callable]) -> None:
        for code in codes.keys():
            if code not in LOGITECH_GAMEPAD_F310_CODES:
                raise ValueError
        super().__init__(event_device=event_device, codes=codes)