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
    keyboard.on_press_key('w', lambda _: uartBT.write(b'w'))
    keyboard.on_release_key('w', lambda _: uartBT.write(b'x'))
    keyboard.on_press_key('s', lambda _: uartBT.write(b's'))
    keyboard.on_release_key('s', lambda _: uartBT.write(b'x'))


process_keyboard()

while True:
    process_read()
