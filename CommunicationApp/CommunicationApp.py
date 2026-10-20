# simple uart send/receive app without sophisticated GUI,just to be able to
# enhance it with some gui one day
# currently based on:
# https://forbot.pl/forum/topic/17581-python-i-komunikacja-uart/

# python3.12 -m venv .venv
# source .venv/bin/activate
# python -m pip install keyboard pyserial
# sudo .venv/bin/python CommunicationApp/CommunicationApp.py

import keyboard
import serial
import select
import sys

uartDebug = serial.Serial("/dev/ttyACM1", 115200, timeout=0.1)
uartBT = serial.Serial("/dev/rfcomm0", 9600, timeout=0.1)


def process_read():
    data = uartDebug.readline()
    if data:
        data = data.decode()
        print(data)


# TODO - async reading and writing, so could do it independently
# TODO - wrap into classes
# TODO - write gui for terminal read and controlling terminal

def process_keyboard():
    pressed_keys = []
    current_command = b'x'

    def update_command():
        nonlocal current_command
        if 'w' in pressed_keys and 'a' in pressed_keys:
            command = b'L'
        elif 'w' in pressed_keys and 'd' in pressed_keys:
            command = b'R'
        elif 'w' in pressed_keys:
            command = b'w'
        elif 's' in pressed_keys:
            command = b's'
        else:
            command = b'x'

        if command != current_command:
            uartBT.write(command)
            current_command = command

    def on_press(key):
        if key not in pressed_keys:
            pressed_keys.append(key)
        update_command()

    def on_release(key):
        if key in pressed_keys:
            pressed_keys.remove(key)
        update_command()

    for key in ('w', 'a', 'd', 's'):
        keyboard.on_press_key(key, lambda _, key=key: on_press(key))
        keyboard.on_release_key(key, lambda _, key=key: on_release(key))


process_keyboard()

while True:
    process_read()
