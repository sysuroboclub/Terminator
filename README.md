# Terminator
This is an intergrated robotics project for undergraduated students going on right now.

## 注意事项：
  1. 电路板不要和水、金属板接触，以免短路烧坏主板；
  2. 电池电量过低需要充电，以免过放；
  3. 接线不要接反、接错，以免短路。

## 学习建议
  1. 搜索 - 善用百度、谷歌搜索，网络上有大量关于Arduino和树莓派现成的教程可供参考。阅读经验可以快速；
  2. 交流 - 积极在群里反馈，交流问题。

## 项目介绍
  ### 语音问答
  * 设备：音响、[树莓派](https://www.raspberrypi.org/)和[麦克风阵列](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.48722e8dSL9P1T&id=548667422899&_u=k1nv1uovbbae)
  * 任务列表
    1. 学会使用树莓派、了解树莓派的接口；
    2. 接收人声作为麦克风阵列的输入，在显示屏上打印出对应的文字（如将人发出的“今天天气怎么样？”显示出来）；
    3. 利用音响作为输出，读出屏幕上的文字（如“今天是晴天”，需要查找现成的第三方API）；
    ……
    
  ### 环境感知
  * 设备：Kinect体感器、高清摄像头
  * 环境：[Kinect SDK](https://www.microsoft.com/en-us/download/details.aspx?id=44561)
  * 任务列表
    1. 用Kinect体感器建立宿舍的3D地图（了解SLAM）；
    2. 用高清摄像头完成识别人脸的任务（了解“计算机视觉”和“人脸识别”）；
    ……
    
  ### 机械臂控制
  * 设备：[6自由度机械臂](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.48722e8dSL9P1T&id=555742998021&_u=k1nv1uov2538)
  * 任务列表
    1. 用遥控器控制机械臂抓取桌面上的橡皮，并放到盒子里；
    2. 编写一套程序，在无人干预的情况下，让机械臂完成上一个任务；
    ……
    
  ### STM32麦轮小车
  * 蓝牙名称：`BT04-A`; 密码: `1234`
  * 操作方法：安装[安卓手机APP](https://github.com/sysuroboclub/Terminator/raw/master/software/minibalance_v5.2(%E5%8F%AF%E4%BB%A5%E9%81%A5%E6%8E%A7%E5%92%8C%E7%9B%91%E6%8E%A7%E6%94%AF%E6%8C%81android9.0).apk)
    
## 参考资料
  1. 机械臂 - [微云下载](https://share.weiyun.com/5NON5eJ) (配套电池：用手机适配器充电；4小时充满电)
  2. 麦轮小车 - [微云下载](https://share.weiyun.com/5FbUiIc)
  3. PS2无线手柄 - [微云下载](https://share.weiyun.com/5YLfcvz)
