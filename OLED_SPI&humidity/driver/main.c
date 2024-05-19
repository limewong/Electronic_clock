#include "config.h"
#include "UART.h"
#include "GPIO.h"
#include "NVIC.h"
#include "Switch.h"
#include "Delay.h"
#include "oled.h"
#include "bmp.h"
#include "DHT.h"

void GPIO_config(void) {
  GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
  GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;		//指定要初始化的IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P3, &GPIO_InitStructure);//初始化
	
	// SPI ---------------------------------------------------------------------- OLED
  GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_6;		//指定要初始化的IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P1, &GPIO_InitStructure);//初始化
	
  GPIO_InitStructure.Pin  = GPIO_Pin_7;		//指定要初始化的IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P4, &GPIO_InitStructure);//初始化
	
  GPIO_InitStructure.Pin  = GPIO_Pin_0;		//指定要初始化的IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P5, &GPIO_InitStructure);//初始化
}

void UART_config(void) {
  // >>> 记得添加 NVIC.c, UART.c, UART_Isr.c <<<
  COMx_InitDefine		COMx_InitStructure;					//结构定义
  COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
  COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
  COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
  COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
  COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
  UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4

  NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
  UART1_SW(UART1_SW_P30_P31);		// 引脚选择, UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

int main() {
  EAXSFR();// 扩展寄存器使能
  GPIO_config();
  UART_config();
	DHT_init();
  EA = 1;	// 开启全局中断

  printf("Init\n");

	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
	OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	OLED_Clear();
	
  while(1) {
		
		u8 humidity;
		float temperature;
		char rst = DHT_get_info(&humidity, &temperature);

		if(rst == SUCCESS){
											
			// 显示汉字
			OLED_Display_GB2312_string(0, 2, "当前湿度是：");
			// 显示湿度
			OLED_ShowNum(88, 2, (int)humidity, 4);
			
			// 显示汉字		
			OLED_Display_GB2312_string(0, 4, "当前温度是：");
			// 显示温度
			OLED_ShowNum(88, 4, temperature, 4);
			
		}												
		delay_ms(250);
		delay_ms(250);
						 				
  }
}