#! /usr/bin/env python
#coding=utf-8

import numpy as np
import serial
import msvcrt

def go_right():
    data = b'\x4B\x47'
    ser.write(data)

def go_left():
    data = b'\x4B\x43'
    ser.write(data)

def go_up():
    data = b'\x4B\x41'
    ser.write(data)

def go_down():
    data = b'\x4B\x45'
    ser.write(data)

def stop():
    data = b'\x4B\x59'
    ser.write(data)

def speed_level(level):
    level=level-48
    if level == 1:
        data = b'\x31'
        ser.write(data)
    elif level == 2:
        data = b'\x32'
        ser.write(data)      
    elif level == 3:
        data = b'\x33'
        ser.write(data) 
    elif level == 4:
        data = b'\x34'
        ser.write(data)
    elif level == 5:
        data = b'\x35'
        ser.write(data) 
    

if __name__ == '__main__':
    # 打开串口
    serialPort="COM5"
    baudRate=9600
    ser=serial.Serial(serialPort,baudRate,timeout=0.5)    
    # 开始控制
    while True:
        key1 = ord(msvcrt.getch())
        if key1 == 13: #Enter退出
            break  
        elif key1 == 32: #空格键
            stop()
        elif key1>=49 and key1<=53: #速度控制
            speed_level(key1)
        elif key1 == 224: #方向控制
            key2 = ord(msvcrt.getch())
            if key2 == 72:
                go_up()
            elif key2 == 80:
                go_down()
            elif key2 == 75:
                go_left()
            elif key2 == 77:
                go_right()
    #关闭串口
    ser.close()

    
    