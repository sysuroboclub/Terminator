#include "stm32f10x.h"
#include "sys.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Left,Flag_Right,Flag_Direction=0;   //����ң����صı���
u8 Flag_Stop=1,Flag_Show=0;                 //ֹͣ��־λ�� ��ʾ��־λ Ĭ��ֹͣ ��ʾ��
int Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //���������������
long int Position_A,Position_B,Position_C,Position_D,Rate_A,Rate_B,Rate_C,Rate_D; //PID������ر���
int Encoder_A_EXTI;                       //ͨ���ⲿ�ж϶�ȡ�ı���������                       
long int Motor_A,Motor_B,Motor_C,Motor_D;        //���PWM����
long int Target_A,Target_B,Target_C,Target_D;     //���Ŀ��ֵ
int Voltage;                             //��ص�ѹ������صı���
float Show_Data_Mb;                      //ȫ����ʾ������������ʾ��Ҫ�鿴������                         
u8 delay_50,delay_flag;                          //��ʱ��ر���
u8 Run_Flag=0;  												//����ң����ر���������״̬��־λ
u8 rxbuf[8],Urxbuf[8],CAN_ON_Flag=0,Usart_ON_Flag=0,PS2_ON_Flag=0,Usart_Flag,PID_Send,Flash_Send;  //CAN�ʹ��ڿ�����ر���
u8 txbuf[8],txbuf2[8],Turn_Flag;             //CAN������ر���
float Pitch,Roll,Yaw,Move_X,Move_Y,Move_Z;   //����ǶȺ�XYZ��Ŀ���ٶ�
u16 PID_Parameter[10],Flash_Parameter[10];  //Flash�������
float	Position_KP=40,Position_KI=0,Position_KD=40;  //λ�ÿ���PID����
float Velocity_KP=10,Velocity_KI=10;	          //�ٶȿ���PID����
int RC_Velocity=30;
int RC_Position=3000;         //����ң�ص��ٶȺ�λ��ֵ
int PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY;  
int main(void)
  { 
		delay_init();	    	            //=====��ʱ������ʼ��	
		JTAG_Set(SWD_ENABLE);           //=====��SWD�ӿ� �������������SWD�ӿڵ���
		LED_Init();                     //=====��ʼ���� LED ���ӵ�Ӳ���ӿ�
		KEY_Init();                     //=====������ʼ��
		MY_NVIC_PriorityGroupConfig(2);	//=====�����жϷ���
		if(MODE==0)Run_Flag=1;          //=====�����Ĺ����У�����ģʽѡ�񿪹�ȷ������λ��ģʽ�����ٶ�ģʽ
		else Run_Flag=0;                //=====�����Ĺ����У�����ģʽѡ�񿪹�ȷ������λ��ģʽ�����ٶ�ģʽ
		OLED_Init();                    //=====OLED��ʼ��
		uart_init(128000);              //=====����1��ʼ��
		uart2_init(9600);               //=====����2��ʼ��
		uart3_init(115200);             //=====����3��ʼ�� 
		Encoder_Init_TIM2();            //=====�������ӿ�
		Encoder_Init_TIM3();            //=====�������ӿ�
		Encoder_Init_TIM4();            //=====��ʼ��������C
		Encoder_Init_TIM5();            //=====��ʼ��������D
		Adc_Init();                     //=====adc��ʼ��
		IIC_Init();                     //=====IIC��ʼ��
		MPU6050_initialize();           //=====MPU6050��ʼ��	
		DMP_Init();                     //=====��ʼ��DMP     
		MiniBalance_PWM_Init(7199,0);   //=====��ʼ��PWM 10KHZ�������������
		PS2_Init();											//=====ps2�����˿ڳ�ʼ��
		PS2_SetInit();		 							//=====ps2���ó�ʼ��,���á����̵�ģʽ������ѡ���Ƿ�����޸�
		MiniBalance_EXTI_Init();         //=====MPU6050 5ms��ʱ�жϳ�ʼ��
		CAN1_Mode_Init(1,2,3,6,0);      //=====CAN��ʼ��	
    while(1)
	   {
	    if(Flag_Show==0)           //ʹ��MiniBalance APP��OLED��ʾ��
			{
				APP_Show();	              
				oled_show();             //===��ʾ����
	    }
    	else                       //ʹ��MiniBalance��λ�� ��λ��ʹ�õ�ʱ����Ҫ�ϸ��ʱ�򣬹ʴ�ʱ�ر�app��ز��ֺ�OLED��ʾ��
  		{
				DataScope();             //����MiniBalance��λ��
			}
     	 CAN1_SEND();             //CAN����		
      USART_TX();               //���ڷ���						
			PS2_LX=PS2_AnologData(PSS_LX);
			PS2_LY=PS2_AnologData(PSS_LY);
			PS2_RX=PS2_AnologData(PSS_RX);
			PS2_RY=PS2_AnologData(PSS_RY);
			PS2_KEY=PS2_DataKey();
			delay_flag=1;	
			delay_50=0;
			while(delay_flag);	       //ͨ��MPU6050��INT�ж�ʵ�ֵ�50ms��׼��ʱ				
	  } 
}

