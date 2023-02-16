from evdev import (InputDevice, list_devices)
from classes import CouchBot

port = "/dev/ttyS0"
baudrate = 9600
initial_motor_value = 127
left_motor_address = 128
right_motor_address = 129
event_device = [InputDevice(path).path for path in list_devices() if InputDevice(path).name == "Logitech Logitech Dual Action"][0]
max_speed = 100
capped_speed = 100
zero_cushion = 15
motor_controller = "linear"
fixed_jerk = None
integral_coefficient = None
linear_step_size = 2
descent_multiplier = None
center = None
closed_loop = False
time_step = 0.01

couch_bot = CouchBot(
    port=port,
    baudrate=baudrate,
    initial_motor_value=initial_motor_value,
    left_motor_address=left_motor_address,
    right_motor_address=right_motor_address,
    event_device=event_device,
    max_speed=max_speed,
    capped_speed=capped_speed,
    zero_cushion=zero_cushion,
    motor_controller=motor_controller,
    fixed_jerk=fixed_jerk,
    integral_coefficient=integral_coefficient,
    linear_step_size=linear_step_size,
    descent_multiplier=descent_multiplier,
    center=center,
    closed_loop=closed_loop,
    time_step=time_step
)
