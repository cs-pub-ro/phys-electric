#!/usr/bin/env python3

import serial
import asyncio
import serial.tools.list_ports
import argparse
import os

async def get_input():
    loop = asyncio.get_event_loop()
    return await loop.run_in_executor(None, input)

async def read_data(serial_port, stop_event):
    while not stop_event.is_set():
        if serial_port.in_waiting > 0: # TODO maybe find better solution here
            message = serial_port.read(serial_port.in_waiting).decode()
            print(f"Data received: {message}")
            # send data to client
        await asyncio.sleep(0.1) # yield control

async def send_data(serial_port, stop_event):
    while not stop_event.is_set():
        message = await get_input()
        # get data from tcp socket
        # send data to pico
        if message.lower() == 'exit':
            stop_event.set()
            break
        else:
            print(f"Data sent: {message}")
            serial_port.write(message.encode())

async def main(serial_port_name, baudrate):
    try:
        serial_port = serial.Serial(serial_port_name, baudrate, timeout=1)
    except Exception as e:
        print(f"Serial port failed: {e}")
        print("Available ports:")
        for i in serial.tools.list_ports.comports():
            print(i.device)
        return

    stop_event = asyncio.Event()

    read_task = asyncio.create_task(read_data(serial_port, stop_event))
    send_task = asyncio.create_task(send_data(serial_port, stop_event))

    await send_task
    read_task.cancel()
    try:
        await read_task
    except asyncio.CancelledError:
        # print("read_task cancelled")
        pass
    finally:
        serial_port.close()


if __name__ == "__main__":
    default_baudrate = 9600
    if os.name == 'nt':  # Windows
        default_port = 'COM3'
    else:  # not Windows
        default_port = '/dev/ttyUSB0'
    parser = argparse.ArgumentParser()
    parser.add_argument('port', type=str, nargs='?', default=default_port, help='The name of the serial port (default COM3 on Windows and /dev/ttyUSB0 on other OS)')
    parser.add_argument('baudrate', type=int, nargs='?', default=default_baudrate, help='The baud rate for the serial communication (default 9600)')
    parser.add_argument('--list_ports', default=False, action='store_true', help='Lists ports')

    args = parser.parse_args()
    serial_port_name = args.port
    baud_rate = args.baudrate
    list_ports = args.list_ports
    if list_ports:
        for i in serial.tools.list_ports.comports():
            print(i.device)
    else:
        asyncio.run(main(serial_port_name, baud_rate))
