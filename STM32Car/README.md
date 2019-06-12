# STM32麦轮小车

## 电脑蓝牙控制

### 蓝牙连接

​	首先打开电脑蓝牙，小车接电，连接小车蓝牙`BT04-A`，输入密码`1234`。连接之后，可以看作是两个串口通过一条隐形的线连在了一起，这时的蓝牙通讯问题就转变为了串口通讯问题。

​	然后，通过电脑设置的蓝牙选项中可以看到对应`BT04-A`的传出端口为`COM5`（不同电脑端口选择可能会不一样）。

### 蓝牙通讯协议

​	由于通过已知的资料，仅知道蓝牙通讯协议的波特率为`9600`，所以通过下位机的代码反向了解蓝牙通讯协议的指令结构。

​	在Minibalance.c中，对STM32各模块进行初始化的部分可以看到，串口2的波特率为9600，所以接下来只需要分析串口2传输数据的代码。

```c
uart2_init(36,9600);            //=====串口2初始化
```

​	在usart.c中，找到了功能为串口2接收中断的函数USART2_IRQHandler()。

​	对于指令的第一位数据，如果为0x4B，则会进入原地转向模式；如果为0x49或0x4A，则会进入弧线形转向模式（未进行实验，也有可能是整体平移式运动）。

```c
if(Usart_Receive==0x4B) Turn_Flag=1;
else if(Usart_Receive==0x49||Usart_Receive==0x4A) Turn_Flag=0;
```

​	对于指令的第二位数据，在原地转向模式中，如果为0x43，则原地逆时针旋转；如果为0x47，则原地顺时针旋转；如果为0x41，则前进；如果为0x45，则后退；如果为其他输入，则停止运动。

```c
if(Usart_Receive==0x43) Flag_Left=0,Flag_Right=1;    
else if(Usart_Receive==0x47) Flag_Left=1,Flag_Right=0;
else Flag_Left=0,Flag_Right=0;

if(Usart_Receive==0x41||Usart_Receive==0x45)Flag_Direction=Usart_Receive-0x40;
else Flag_Direction=0;
```

​	对于指令的第二位数据，在弧线形转向模式中，如果为0x41~0x48或0~8，对应方向为前、右前、右、右后、后、左后、左和左前八个方向；如果为其他输入，则停止运动。

```c
if(Usart_Receive>=0x41&&Usart_Receive<=0x48) Flag_Direction=Usart_Receive-0x40;
else if(Usart_Receive<=8) Flag_Direction=Usart_Receive;	
else Flag_Direction=0;
```

​	至此，我们已经了解了蓝牙传输协议的指令内容，可以开始串口传输的代码编写。

### 指令传输

​	首先打开串口`COM5`，设置波特率为9600。

```python
import serial
serialPort="COM5"
baudRate=9600
ser=serial.Serial(serialPort,baudRate,timeout=0.5)  
```

​	然后通过键盘控制小车的运动。停止命令为b'\x4B\x59'。

```python
import msvcrt
key1 = ord(msvcrt.getch())
if key1 == 13: #Enter退出
    break  
elif key1 == 32: #空格键
    stop()
```

​	由于我们通过上下左右键控制小车运动，而msvcrt模块捕捉键盘信号时，如果输入为方向键，则会得到两个字符，且第一个字符为224。所以当key1为224时，进行key2的读取。

```python
elif key1 == 224:
    key2 = ord(msvcrt.getch())
    if key2 == 72: go_up()
    elif key2 == 80: go_down()
    elif key2 == 75: go_left()
    elif key2 == 77: go_right()
```

​	前进、后退、左转、右转的命令分别为：b'\x4B\x41'，b'\x4B\x45'，b'\x4B\x43'，b'\x4B\x47'。这里以go_up()为例。

```python
def go_up():
    data = b'\x4B\x41'
    ser.write(data)
```

​	最后，退出后关闭串口。

```python
ser.close()
```

### 小车速度控制

​	在原有基础上添加对小车的速度控制。如果对小车进行CAN或串口控制的话，已有的通讯协议已经可以满足对小车速度的控制要求，但由于我们使用的是蓝牙传输，而在原有的设定中，蓝牙控制无法改变速度，所以我们需要对下位机代码进行修改。

#### 下位机代码修改

​	在control.c中，可以看到，在确定了移动方向之后，小车将以参数step作为加速度，而当小车速度绝对值达到临界值RC_Velocity将不再增加或减少。所以，如果要改变小车的运行速度，改变RC_Velocity的值就可以了。

​	对蓝牙数据读取函数USART2_IRQHandler()进行修改，这里选择0x31~0x35作为小车速度的控制命令，使其速度分为5个档位，最小速度为6，最大速度为30（原代码的默认速度）。并且将Turn_Flag设置为2，使得小车在接收到速度更改命令使不会改变原有的运动方向状态。

```c
else if(Usart_Receive>=0x31&&Usart_Receive<=0x35) 
{
    Turn_Flag=2;
    RC_Velocity=6*(Usart_Receive-0x30);
}
```

#### 蓝牙控制代码修改

​	将小车的速度控制按键设为键盘上的12345，当键盘检测到1~5的输入时，就发送速度控制命令。

```python
elif key1>=49 and key1<=53:
    speed_level(key1)
```

​	1，2，3，4，5档的速度命令分别为：b'\x31'，b'\x32'，b'\x33'，b'\x34'，b'\x35'。

```python
def speed_level(level):
    level=level-48
    if level == 1:
        data = b'\x31'
        ser.write(data)
    ...
```

### 小车下载程序

​	电脑使用Keil打开project并进行修改和编译。

​	在Project-Options for Target-Output中勾选Creat HEX File使得工程在编译结束后生产hex文件进行烧录。

​	编译成功后OBJ文件夹中新的hex文件将覆盖原有的hex文件。

​	电脑安装USB转TTL模块CH340G的驱动（需要在小车与电脑接线后安装，否则无法安装成功）。

​	小车和电脑接线使用MicroUSB手机数据线（华为的充电线就是）。

​	打开MCUISP烧录软件，根据开发手册进行设置，并将联机下载时的程序文件设为刚刚编译成功后产生的hex文件。开始编程，烧录成功。:grin: