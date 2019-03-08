# 语言问答

### 设备

[树莓派](https://www.raspberrypi.org/)、[麦克风阵列](http://wiki.seeedstudio.com/cn/ReSpeaker_Mic_Array_v2.0/)、蓝牙音箱、



### 任务列表

#### 1.  树莓派入门

##### 1.1 树莓派拆箱

树莓派可以视作一块微型的电脑主板，我们需要插上一块安装了操作系统的SD卡，就是一台迷你电脑了。首先，我们需要对SD卡格式化，官网推荐了[这个](https://www.sdcard.org)，然后从树莓派官网上下载操作系统，官方推荐的是Raspbian，当然也可以ubuntu甚至windows。这里我们选择的是[Raspbian](https://www.raspberrypi.org/downloads/raspbian/)，然后需要一个工具将操作系统镜像写入SD卡中，这是使用的是[Eicher](https://www.balena.io/etcher/)，安装指导[如下](https://www.raspberrypi.org/documentation/installation/installing-images/README.md)。

##### 1.2登录树莓派

接下来需要登录树莓派，采用无显示屏的登录方法，在上一步的SD卡安装好操作系统后，插入电脑，直接在root目录下，创建ssh和 wpa_supplicant.conf 文件，ssh是一个空白文件，提醒树莓派启动ssh服务，wpa_supplicant.conf用于配置无线网络连接，内容如下：

> country=CN
> ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
> update_config=1
>
> network={
> ssid="WiFi-A"
> psk="12345678"
> key_mgmt=WPA-PSK
> priority=1
> }

其中ssid为名字，psk为密码，key_mgmt为加密方式，priotiry为连接优先级。

然后树莓派通电，就能连上设置的wifi了（比如电脑的热点），然后获取树莓派的IP地址，在windows上使用SSH服务需要[putty](https://www.putty.org)，启动填入树莓派的IP地址，点击open，然后会弹出命令框，需要输入树莓派的账户和密码，默认是pi和raspberry。登录后就可以通过终端来操作树莓派了。

详见http://shumeipai.nxez.com/2017/09/13/raspberry-pi-network-configuration-before-boot.html

##### 1.3 使用微软 远程桌面连接 登录树莓派

使用ssh方法无法看到树莓派的图形界面，第一次通过ssh登录后，下载tightvncserver和xrdp并启动。然后就可以通过远程桌面连接登录了。

> sudo apt-get install tightvncserver
>
> sudo apt-get install xrdp
>
> sudo /etc/init.d/xrdp restart

win+R输入mstsc弹出连接窗口，输入树莓派IP地址就可以登录了。

##### 1.4 (选用)使用网线直连树莓派的方法上网

用网线连接树莓派和电脑，电脑连接wifi，并在网络适配器里将无线连接设置为共享，树莓派就可以通过网线共享电脑的无线网了。然后在命令行界面输入arp -a，接口192.168.137.1下的第一个IP地址就为树莓派的IP地址，有了IP地址就可用ssh或远程桌面的方法登录树莓派了。

详见http://shumeipai.nxez.com/2013/10/15/raspberry-pi-and-a-network-cable-directly-connected-laptop.html



#### 2. 使用麦克风阵列录音

##### 2.1 连接麦克风阵列

在windows上，需要下载USB驱动，推荐使用[Zadig](https://zadig.akeo.ie/)，具体指导[如下](http://wiki.seeedstudio.com/cn/ReSpeaker_Mic_Array_v2.0/#_8)，

在linux上，无需安装驱动，接上树莓派就可以使用了。

##### 2.2 录音

Python函数库依赖：pyaudio

可通过以下命令安装该库：

> ```python
> sudo pip install pyaudio
> ```

首先运行_get_index.py_，在输出窗口查看__ReSpeaker__的设备ID（Input Device id），将文件_record.py_中的变量__RESPEAKER_INDEX__的值修改为前述ID，然后运行_record.py_进行录音，音频文件保存为_output.wav_。在[这个网站](http://wiki.seeedstudio.com/cn/ReSpeaker_Mic_Array_v2.0/)上有详细教程。



#### 3. 语音与文本互相转换

##### 3.1 前提

本实验通过调用百度AI开放平台的API实现语音转换文本以及文本转语音。

Python函数库依赖：aip

可通过以下命令安装该库：

```python
sudo pip install baidu-aip
```

从[百度AI开放平台](https://ai.baidu.com/tech/speech)上注册一个账号后，在语音技术模块处获得__APP_ID__、__API_KEY__以及__SECRET_KEY__，将以上ID和口令填入_sound2text.py_和_text2sound.py_中对应位置。

##### 3.2 将语音转换为文本输出

将要转换成文本的语音文件_audio.wav_（时长不超过1分钟）放在_sound2text.py_相同目录下，再运行_sound2text.py_即可在输出窗口查看语音转换后的文本。

##### 3.3 将文本转换成语音输出

将要转换成语音的文本字符串作为_text2sound.py_中变量__text__的值，再运行_text2sound.py_即可在该目录下查看到文本转换而成的音频文件_audio.mp3_。



#### 4. 使用音箱将文本输出为语音

树莓派支持HDMI线和3.5mm音频线传输音频数据，我们的音箱只支持USB连接，所以可以使用USB-HDMI转换线，或者通过蓝牙连接，我们选择了后者。推荐使用图形界面来操作，因为可能会遇到奇怪的问题。

十分重要！！！成功连接蓝牙音箱后，需要右键点击桌面右上角音量键，将输出设备改成音箱，否则音箱无声音输出。



#### 温馨提示

实际操作中，会发现很多没有提及的问题，这里不一一列出，遇到再自行百度吧。

