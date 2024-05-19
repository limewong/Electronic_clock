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
  GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����
  GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;		//ָ��Ҫ��ʼ����IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P3, &GPIO_InitStructure);//��ʼ��
	
	// SPI ---------------------------------------------------------------------- OLED
  GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_6;		//ָ��Ҫ��ʼ����IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P1, &GPIO_InitStructure);//��ʼ��
	
  GPIO_InitStructure.Pin  = GPIO_Pin_7;		//ָ��Ҫ��ʼ����IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P4, &GPIO_InitStructure);//��ʼ��
	
  GPIO_InitStructure.Pin  = GPIO_Pin_0;		//ָ��Ҫ��ʼ����IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P5, &GPIO_InitStructure);//��ʼ��
}

void UART_config(void) {
  // >>> �ǵ���� NVIC.c, UART.c, UART_Isr.c <<<
  COMx_InitDefine		COMx_InitStructure;					//�ṹ����
  COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
  COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//ѡ�����ʷ�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
  COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
  COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
  COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
  UART_Configuration(UART1, &COMx_InitStructure);		//��ʼ������1 UART1,UART2,UART3,UART4

  NVIC_UART1_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
  UART1_SW(UART1_SW_P30_P31);		// ����ѡ��, UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

int main() {
  EAXSFR();// ��չ�Ĵ���ʹ��
  GPIO_config();
  UART_config();
	DHT_init();
  EA = 1;	// ����ȫ���ж�

  printf("Init\n");

	OLED_Init();
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
	OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	OLED_Clear();
	
  while(1) {
		
		u8 humidity;
		float temperature;
		char rst = DHT_get_info(&humidity, &temperature);

		if(rst == SUCCESS){
											
			// ��ʾ����
			OLED_Display_GB2312_string(0, 2, "��ǰʪ���ǣ�");
			// ��ʾʪ��
			OLED_ShowNum(88, 2, (int)humidity, 4);
			
			// ��ʾ����		
			OLED_Display_GB2312_string(0, 4, "��ǰ�¶��ǣ�");
			// ��ʾ�¶�
			OLED_ShowNum(88, 4, temperature, 4);
			
		}												
		delay_ms(250);
		delay_ms(250);
						 				
  }
}