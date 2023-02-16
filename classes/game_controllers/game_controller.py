from evdev import InputDevice, KeyEvent, ecodes, categorize
from threading import Thread
from typing import (Callable, Dict)

class GameController(object):
    __gamepad: InputDevice
    __codes: Dict[int, Callable]
    __event_thread: Thread

    def __init__(self,
        event_device: str,
        codes: Dict[int, Callable]
    ) -> None:
        self.__gamepad = InputDevice(event_device)
        self.__codes = codes
        self.__event_thread = Thread(target=self.__run_loop, daemon=True)
        self.start()

    @property
    def codes(self) -> Dict[str, int]:
        return self.__codes

    @codes.setter
    def scancodes(self, codes: Dict[int, Callable]) -> None:
        self.__codes = codes

    def __run_loop(self) -> None:
        try:
            for event in self.__gamepad.read_loop():
                if self.__codes.get(event.code) != None:
                    self.__codes.get(event.code)(event.value)
        except Exception as e:
            print(f"Error {e} occurred.")

    def start(self) -> None:
        self.__event_thread.start()