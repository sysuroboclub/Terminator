#include <Servo.h> 
#include <SSD1306.h>
#include <DATASCOPE.h>      //这是PC端上位机的库文件
#include <MsTimer2.h>        //定时中断
#include <PS2X_lib.h>  
//////////PS2引脚//////////////////
#define PS2_DAT        16  //14    
#define PS2_CMD        17  //15
#define PS2_SEL        18  //16
#define PS2_CLK        19  //17
////////OLED显示屏引脚///////////
#define OLED_DC 4  
//#define OLED_CS 5//CS直接接地
#define OLED_CLK 13
#define OLED_MOSI 12
#define OLED_RESET 2
#define KEY 14
SSD1306 oled(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, 0);
int Battery_Voltage;  //电池电压
DATASCOPE data;//实例化一个 上位机 对象，对象名称为 data
Servo myservo1,myservo2,myservo3,myservo4,myservo5,myservo6;  //创建一个舵机控制对象
PS2X ps2x; // create PS2 Controller Class
#define l0 0.105f  //机械臂的参数
#define l1 0.078f
#define l2 0.055f
float Target1=100,Target2=90,Target3=82,Target4=86,Target5=90,Target6=90;  //6个舵机的目标值
float Position1=90,Position2=90,Position3=90,Position4=90,Position5=90,Position6=90;   //实际位置值(估算得到)  
float Velocity1,Velocity2,Velocity3,Velocity4,Velocity5,Velocity6;     //舵机速度变量
float  Position_KP=0.9,Position_KI=0,Position_KD=0.3;  //位置控制PID参数
float TargetX=2000,TargetY=0,Target_Beta=-45,Target_Alpha=0,Target_Gamma=0;               //电机目标值
char Flag_Way;  //控制方式 坐标控制还是单独控制
float Speed=1;  //控制舵机运动的速度
int error = 0;   //PS2使用的一个变量
int control_number = 0;
void (* resetFunc) (void) = 0;// Reset func 
/**************************************************************************
函数功能：数学模型
入口参数：末端执行器位姿态
返回  值：无
**************************************************************************/
u8 Kinematic_Analysis(float x,float y,float Beta,float Alpha,float Gamma)
{  
        float m,n,k,a,b,c,theta1,theta2,theta3,s1ps2;
        static float Last_TargetX,Last_TargetY;
        m=l2*cos(Alpha)-x;   //中间变量
        n=l2*sin(Alpha)-y;   //中间变量
        k=(l1*l1-l0*l0-m*m-n*n)/2/l0;//中间变量
        a=m*m+n*n;             //解一元二次方程
        b=-2*n*k;
        c=k*k-m*m;
        if(b*b-4*a*c<=0)        { 
        TargetX=Last_TargetX;   
        TargetY=Last_TargetY;   
        return 0; //防止出现非实数解
        }
        theta1=(-b+sqrt(b*b-4*a*c))/2/a;  //得到一元二次方程的解，只取其中一个，另外一个解是(-b+sqrt(b*b-4*a*c))/2/a
        theta1=asin(theta1)*180/PI;       //弧度换成角度
        if(theta1>90)theta1=90;           //控制舵机的最大角度±90°
        if(theta1<-90)theta1=-90;
        k=(l0*l0-l1*l1-m*m-n*n)/2/l1;     //过程系数
        a=m*m+n*n;                        //解一元二次方程
        b=-2*n*k;
        c=k*k-m*m;
        if(b*b-4*a*c<=0)  { 
        TargetX=Last_TargetX;   //使用上一次的值
        TargetY=Last_TargetY;   //使用上一次的值
        return 0;         //防止出现非实数解
        }
        s1ps2=(-b-sqrt(b*b-4*a*c))/2/a;      //得到一元二次方程的解，只取其中一个，另外一个解是(-b+sqrt(b*b-4*a*c))/2/a
        s1ps2=asin(s1ps2)*180/PI;            //弧度换成角度
        if(s1ps2>90)theta2=90;    //限制最大角度  
        if(s1ps2<-90)theta2=-90;     
        theta2=s1ps2-theta1;
        if(theta2>90)theta2=90;      //控制舵机的最大角度±90°
        if(theta2<-90)theta2=-90;    //控制舵机的最大角度±90°
        theta3=Alpha*180/PI-theta1-theta2;   //求关节3角度
        if(theta3>90)theta3=90;
        if(theta3<-90)theta3=-90;     //控制舵机的最大角度±90°
        Target1   = 90-(Beta);   //作用到输出  
        Target2   = 90-(theta1-90); 
        Target3   = 90+(theta2);
        Target4   = 90+(theta3);  
        Target6   = 111-(Gamma);   //6自由度额外增加的自由度  
        Last_TargetX=TargetX;  //保存历史值
        Last_TargetY=TargetY;
        return 0;
}
/**************************************************************************
函数功能：虚拟示波器往上位机发送数据 作者：平衡小车之家
入口参数：无
返回  值：无
**************************************************************************/
void DataScope(void)
{
  int i,Send_Count;
  data.DataScope_Get_Channel_Data(Target1, 1);//显示第1个数据
  data.DataScope_Get_Channel_Data(Target2, 2);//显示第2个数据
  data.DataScope_Get_Channel_Data(Target3, 3);//显示第3个数据
  data.DataScope_Get_Channel_Data(Target4, 4);//显示第4个数据
  data.DataScope_Get_Channel_Data(Target5, 5);//显示第5个数据
  data.DataScope_Get_Channel_Data(Target6, 6);//显示第6个数据
  Send_Count = data.DataScope_Data_Generate(6);   
  for ( i = 0 ; i < Send_Count; i++)
  {
    Serial.write(DataScope_OutPut_Buffer[i]);  
  }
  delay(50);  //上位机必须严格控制发送时序
}

/**************************************************************************
函数功能：求次方的函数
入口参数：m,n
返回  值：m的n次幂
**************************************************************************/
uint32_t oled_pow(uint8_t m,uint8_t n)
{
  uint32_t result=1;  
  while(n--)result*=m;    
  return result;
} 
/**************************************************************************
函数功能：显示变量
入口参数：x:x坐标   y:行  num：显示的变量 len ：变量的长度
返回  值：无
**************************************************************************/
void OLED_ShowNumber(uint8_t x,uint8_t y,uint32_t num,uint8_t len)
{           
  u8 t,temp;  
  u8 enshow=0;               
  for(t=0;t<len;t++)  {
    temp=(num/oled_pow(10,len-t-1))%10;
    oled.drawchar(x+6*t,y,temp+'0');
  }  
} 
/**************************************************************************
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM
根据位置式离散PID公式 
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
pwm代表输出
**************************************************************************/
int Position_PID1(float Encoder,float Target){    //舵机1的PID控制器
   static float Bias,Pwm,Integral_bias,Last_Bias;
   Bias=Encoder-Target;                                  //计算偏差
   Integral_bias+=Bias;                                  //求出偏差的积分
   Pwm=Position_KP*Bias+Position_KI*Integral_bias/100+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
   Last_Bias=Bias;                                       //保存上一次偏差 
   return Pwm;                                           //增量输出
}
int Position_PID2(float Encoder,float Target){    //舵机2的PID控制器
   static float Bias,Pwm,Integral_bias,Last_Bias;
   Bias=Encoder-Target;                                  //计算偏差
   Integral_bias+=Bias;                                  //求出偏差的积分
   Pwm=Position_KP*Bias+Position_KI*Integral_bias/100+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
   Last_Bias=Bias;                                       //保存上一次偏差 
   return Pwm;                                           //增量输出
}
int Position_PID3(float Encoder,float Target){     //舵机3的PID控制器
   static float Bias,Pwm,Integral_bias,Last_Bias;
   Bias=Encoder-Target;                                  //计算偏差
   Integral_bias+=Bias;                                  //求出偏差的积分
   Pwm=Position_KP*Bias+Position_KI*Integral_bias/100+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
   Last_Bias=Bias;                                       //保存上一次偏差 
   return Pwm;                                           //增量输出
}
int Position_PID4(float Encoder,float Target){      //舵机4的PID控制器
   static float Bias,Pwm,Integral_bias,Last_Bias;
   Bias=Encoder-Target;                                  //计算偏差
   Integral_bias+=Bias;                                  //求出偏差的积分
   Pwm=Position_KP*Bias+Position_KI*Integral_bias/100+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
   Last_Bias=Bias;                                       //保存上一次偏差 
   return Pwm;                                           //增量输出
}
int Position_PID5(float Encoder,float Target){    //舵机5的PID控制器
   static float Bias,Pwm,Integral_bias,Last_Bias;
   Bias=Encoder-Target;                                  //计算偏差
   Integral_bias+=Bias;                                  //求出偏差的积分
   Pwm=Position_KP*Bias+Position_KI*Integral_bias/100+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
   Last_Bias=Bias;                                       //保存上一次偏差 
   return Pwm;                                           //增量输出
}
int Position_PID6(float Encoder,float Target){    //舵机6的PID控制器
   static float Bias,Pwm,Integral_bias,Last_Bias;
   Bias=Encoder-Target;                                  //计算偏差
   Integral_bias+=Bias;                                  //求出偏差的积分
   Pwm=Position_KP*Bias+Position_KI*Integral_bias/100+Position_KD*(Bias-Last_Bias);       //位置式PID控制器
   Last_Bias=Bias;                                       //保存上一次偏差 
   return Pwm;                                           //增量输出
}
 void Control_servo(float velocity1,float velocity2,float velocity3,float velocity4,float velocity5,float velocity6)
{
    Position1+=velocity1;       //速度的积分，得到舵机的位置
    Position2+=velocity2;       //速度的积分，得到舵机的位置
    Position3+=velocity3;       //速度的积分，得到舵机的位置
    Position4+=velocity4;       //速度的积分，得到舵机的位置
    Position5+=velocity5;       //速度的积分，得到舵机的位置
    Position6+=velocity6;       //速度的积分，得到舵机的位置
    myservo1.write(Position1);        // 指定舵机转向的角度
    myservo2.write(Position2);        // 指定舵机转向的角度
    myservo3.write(Position3);        // 指定舵机转向的角度
    myservo4.write(Position4);        // 指定舵机转向的角度
    myservo5.write(Position5);        // 指定舵机转向的角度
    myservo6.write(Position6);        // 指定舵机转向的角度 
}
/**************************************************************************
my control part
**************************************************************************/
void my_control(float Step){
  if(control_number == 0){
    if(Target4 > 0) Target4-=Step;
    if(Target2 < 155) Target2+=Step;
    else control_number = 1;
    }
  if(control_number == 1){
    if(Target5 < 120) Target5 += Step;
    else control_number = 2;
    }
  if(control_number == 2){
    if(Target4 < 90) Target4+=Step;
    else if(Target2 < 180) Target2+=Step;
    else control_number = 3;
    }
  if(control_number == 3){
    if(Target3 > 45) {Target3-=Step;Target4+=Step;}
    else if(Target5 > 90) Target5-=Step;
    else control_number = 4;
    }
  if(control_number == 4){
    if(Target3 > 0){Target3-=Step;Target4+=Step;}
    if(Target2 > 90){Target2-=Step;}
    if(Target3==0 & Target2<=90) control_number = 5;
    }
  if(control_number == 5){
    Target3 = 90;
    Target4 = 90;
    }
  //else TargetX-=Step;

  if(Speed<=1) Speed=1;
  if(Speed>=15) Speed=15;
  }
/**************************************************************************
函数功能：PS2手柄接收控制函数
入口参数：无
返回  值：无
**************************************************************************/
void PS2_RX(float Step)
{
      ps2x.read_gamepad(false, 0); //read controller and set large motor to spin at 'vibrate' speed
      if(Flag_Way==1)    //坐标控制
    { 
       if(ps2x.Button(PSB_TRIANGLE))   Target_Alpha-=Step; 
       if(ps2x.Button(PSB_CROSS))      Target_Alpha+=Step;
       if(ps2x.Button(PSB_CIRCLE))     Target5+=Step*2000;
       if(ps2x.Button(PSB_SQUARE))     Target5-=Step*2000; //机械爪的张合
       if(ps2x.Button(PSB_PAD_UP))     TargetX+=Step;   //X轴
       if(ps2x.Button(PSB_PAD_DOWN))   TargetX-=Step;   //X轴
       if(ps2x.Button(PSB_PAD_RIGHT))  Target_Beta+=Step*2000;  //云台
       if(ps2x.Button(PSB_PAD_LEFT))   Target_Beta-=Step*2000; 
       if(ps2x.Button(PSB_R1))         TargetY+=Step;   //Y轴
       if(ps2x.Button(PSB_R2))         TargetY-=Step;   //Y轴
       if(ps2x.Button(PSB_SELECT))     Target_Gamma-=Step*2000; 
       if(ps2x.Button(PSB_START))      Target_Gamma+=Step*2000;
    }
    else  //单独控制
    { 
       if(ps2x.Button(PSB_TRIANGLE))  Target4-=Step;
       if(ps2x.Button(PSB_CROSS))     Target4+=Step;
       if(ps2x.Button(PSB_CIRCLE))    Target5+=Step;   //爪子
       if(ps2x.Button(PSB_SQUARE))    Target5-=Step;   //爪子
       if(ps2x.Button(PSB_PAD_UP))    Target3-=Step;
       if(ps2x.Button(PSB_PAD_DOWN))  Target3+=Step;
       if(ps2x.Button(PSB_PAD_RIGHT)) Target1-=Step;  //云台
       if(ps2x.Button(PSB_PAD_LEFT))  Target1+=Step;  //云台
       if(ps2x.Button(PSB_R1))        Target2-=Step;
       if(ps2x.Button(PSB_R2))        Target2+=Step;
       if(ps2x.Button(PSB_SELECT))    Target6+=Step;  //额外增加的舵机
       if(ps2x.Button(PSB_START))     Target6-=Step;  //额外增加的舵机
    }
    if(ps2x.Button(PSB_L1))          Speed+=0.1;  //速度控制
    else  if(ps2x.Button(PSB_L2))    Speed-=0.1;
    if(Speed<=1)Speed=1;  //限制最小速度
    if(Speed>=15)Speed=15; //限制最大速度
   
}
/**************************************************************************
函数功能：按键扫描
入口参数：无
返回  值：按键状态 0：无动作 1：单击
**************************************************************************/
unsigned char My_click (void) {
    static byte flag_key = 1; //按键按松开标志
    if (flag_key && (digitalRead(KEY) == 0))   { //如果发生单击事件
    flag_key = 0;
    if (digitalRead(KEY) == 0)  return 1;    //
  }
  else if (digitalRead(KEY) == 1)     flag_key = 1;
  return 0;//无按键按下
}
/**************************************************************************
函数功能：目标赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void){  
    int Amplitude_H=180, Amplitude_L=0;       //舵机脉宽极限值
    if(Target1<Amplitude_L)  Target1=Amplitude_L; 
    if(Target1>Amplitude_H)  Target1=Amplitude_H; 
    if(Target2<Amplitude_L)  Target2=Amplitude_L; 
    if(Target2>Amplitude_H)  Target2=Amplitude_H; 
    if(Target3<Amplitude_L)  Target3=Amplitude_L; 
    if(Target3>Amplitude_H)  Target3=Amplitude_H; 
    if(Target4<Amplitude_L)  Target4=Amplitude_L; 
    if(Target4>Amplitude_H)  Target4=Amplitude_H; 
    if(Target5<Amplitude_L)  Target5=Amplitude_L; 
    if(Target5>Amplitude_H)  Target5=Amplitude_H; 
    if(Target6<Amplitude_L)  Target6=Amplitude_L; 
    if(Target6>Amplitude_H)  Target6=Amplitude_H;   
}
/**************************************************************************
函数功能：5ms控制函数 核心代码 作者：平衡小车之家
入口参数：无
返回  值：无
**************************************************************************/
void control(){  
  static float Voltage_All,Voltage_Count;//电压采样相关变量
  char Key,Temp;//临时变量
  float Temp2; //采集电压的临时变量
    static char count;   
    sei();        //全局中断开启
    Key=My_click();//扫描按键变化  
    if(Key==1)  Flag_Way=!Flag_Way; //单击控制机械臂的状态
    if(Battery_Voltage>680)        //===如果电池电压不存在异常
    {
      my_control(Speed);
      //Temp=Kinematic_Analysis(TargetX,TargetY,Target_Beta,Target_Alpha,Target_Gamma);
      /*************************************************************************************
      if(Flag_Way==1)    {   //进行运动学分析之后的控制
       PS2_RX((float)Speed/3000);    //PS2遥控坐标
       Temp=Kinematic_Analysis(TargetX,TargetY,Target_Beta,Target_Alpha,Target_Gamma);  //运动学分析
      }
      else          PS2_RX(Speed);    //接收PS2遥控
      ***************************************************************************************/
      Xianfu_Pwm();   //限制目标值
      Velocity1=-Position_PID1(Position1,Target1);//舵机PID控制
      Velocity2=-Position_PID2(Position2,Target2);//舵机PID控制
      Velocity3=-Position_PID3(Position3,Target3);//舵机PID控制
      Velocity4=-Position_PID4(Position4,Target4);//舵机PID控制
      Velocity5=-Position_PID5(Position5,Target5);//舵机PID控制
      Velocity6=-Position_PID6(Position6,Target6);//舵机PID控制
      Control_servo(Velocity1,Velocity2,Velocity3,Velocity4,Velocity5,Velocity6); //控制6个舵机   
     }
   Temp2 = analogRead(1);  //采集一下电池电压
   Voltage_Count++;       //平均值计数器
   Voltage_All+=Temp2;     //多次采样累积
   if(Voltage_Count==40) Battery_Voltage=Voltage_All*0.5371/4,Voltage_All=0,Voltage_Count=0;//求平均值
 }
 /**************************************************************************
函数功能：初始化 相当于STM32里面的Main函数 作者：平衡小车之家
入口参数：无
返回  值：无
**************************************************************************/
void setup()   {                
   oled.ssd1306_init(SSD1306_SWITCHCAPVCC);
   oled.clear();   // clears the screen and buffer
   Serial.begin(128000);           //开启串口
   delay(200);                      //延时等待初始化完成
   myservo1.attach(10);             //初始化各个舵机
   myservo2.attach(9);
   myservo3.attach(8);
   myservo4.attach(7);  
   myservo5.attach(6);
   myservo6.attach(5);
   MsTimer2::set(50, control);       //使用Timer2设置定时中断
   MsTimer2::start();               //中断使能
   //error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);//初始化PS2手柄
}
/**************************************************************************
函数功能：主循环程序体
入口参数：无
返回  值：无
**************************************************************************/
void loop()   {                   
  oled.drawstring(00, 0, "VOLTAGE:");
  oled.drawstring(71, 0, ".");
  oled.drawstring(93, 0, "V");
  OLED_ShowNumber(58, 0, Battery_Voltage / 100, 2);
  OLED_ShowNumber(81, 0, Battery_Voltage % 100, 2);      
  OLED_ShowNumber(0, 1,Target1,3);    OLED_ShowNumber(40, 1,Position1,3);  
  OLED_ShowNumber(0, 2,Target2,3);    OLED_ShowNumber(40, 2,Position2,3); 
  OLED_ShowNumber(0, 3,Target3,3);    OLED_ShowNumber(40, 3,Position3,3); 
  OLED_ShowNumber(0, 4,Target4,3);    OLED_ShowNumber(40, 4,Position4,3);  
  OLED_ShowNumber(0, 5,Target5,3);    OLED_ShowNumber(40, 5,Position5,3);   
  OLED_ShowNumber(0, 6,Target6,3);    OLED_ShowNumber(40, 6,Position6,3);  
  if( Velocity1<0)  oled.drawstring(70,1,"-"),  OLED_ShowNumber(80,1,-Velocity1,3);    else  oled.drawstring(70,1,"+"), OLED_ShowNumber(80,1, Velocity1,3);
  if( Velocity2<0)  oled.drawstring(70,2,"-"),  OLED_ShowNumber(80,2,-Velocity2,3);    else  oled.drawstring(70,2,"+"), OLED_ShowNumber(80,2, Velocity2,3);
  if( Velocity3<0)  oled.drawstring(70,3,"-"),  OLED_ShowNumber(80,3,-Velocity3,3);    else  oled.drawstring(70,3,"+"), OLED_ShowNumber(80,3, Velocity3,3);
  if( Velocity4<0)  oled.drawstring(70,4,"-"),  OLED_ShowNumber(80,4,-Velocity4,3);    else  oled.drawstring(70,4,"+"), OLED_ShowNumber(80,4, Velocity4,3);
  if( Velocity5<0)  oled.drawstring(70,5,"-"),  OLED_ShowNumber(80,5,-Velocity5,3);    else  oled.drawstring(70,5,"+"), OLED_ShowNumber(80,5, Velocity5,3);
  if( Velocity6<0)  oled.drawstring(70,6,"-"),  OLED_ShowNumber(80,6,-Velocity6,3);    else  oled.drawstring(70,6,"+"), OLED_ShowNumber(80,6, Velocity6,3);
    oled.drawstring(0, 7, "SPEED:");   OLED_ShowNumber(35, 7,Speed,2);    oled.drawstring(60, 7, "MODE:");
  if(Flag_Way==0) oled.drawstring(90, 7, "ABC");
  else            oled.drawstring(90, 7, "XYZ");
//////=============刷新=======================//
 oled.display();
 DataScope();
}
