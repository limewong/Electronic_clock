#include "DHT.h"
#include "GPIO.h"
#include "delay.h"

static void GPIO_config(void) {
  GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����
  GPIO_InitStructure.Pin  = GPIO_Pin_6;		//ָ��Ҫ��ʼ����IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P4, &GPIO_InitStructure);//��ʼ��
}
/**
 * @brief ��ʼ��
 * 
 */
void DHT_init(){
    GPIO_config();
}

void Delay1us(void)	//@24.000MHz
{
	unsigned char data i;

	i = 6;
	while (--i);
}

void delay_1us(){
	NOP8();
}

/***********************
�ȴ�DHT��ƽ�仯
************************/
#define wait_level_change(level, min, max, desc) 				                        			\
		cnt = 0;                                                                        	\ 
		while(DHT == level){														    															\ 																																			
			delay_1us();                                                                        \ 
			cnt++;                                                                              \ 
		};                                                    																\ 
		if(cnt < min || cnt > max){                                                             \ 
			/*��������Ӧʱ��*/                                                                   \ 
			printf("err: [%d]us, ������%sʱ�� [%d,%d]\n", (int)cnt, desc, (int)min, (int)max);     \
			return -1;                                                                          \ 
		}                                                                                       \ 

// ��Ҫ������־
char on_read(u8 *dat){ // ���ⲿ����һ�����飨ָ�룩������ִ����ϣ������鱻��������
	
	u16 cnt = 0; 					// ��ʱ����ÿ+1������1us, һ��Ҫ��ʼ��Ϊ0
//	u8 dat[5] = {0x00};		// 5byte * 8bit = 40bit
	char i, j;
	// ��ȡDHT11����
	// printf("read\n");
	
	// 1. ������ʼ�ź����� 20ms
	DHT = 0;
	delay_ms(20);
	DHT = 1;
	
	// 2. �ȴ������ͷ�����
	cnt = 0;
	while(DHT == 1 && cnt < 40){
		delay_1us();
		cnt++;
	}
	if(cnt < 8 || cnt > 35){
		// ��������Ӧʱ��
		printf("err: ʱ��[%d], �����ͷ�����[%d,%d]\n", (int)cnt,(int)10, (int)35);
		return -1;
	}
	
	// 3. ��Ӧ�͵�ƽʱ��, 83us [78,88]us, ��ǰ��0��ֱ��Ϊ1������ѭ��
	wait_level_change(0, 78, 88, "Resp��Ӧ�͵�ƽ");
	
	// 4. ��Ӧ�ߵ�ƽʱ�䣬87us [80,92]us, ��ǰ��1��ֱ��Ϊ0������ѭ��
	wait_level_change(1, 80, 92, "Resp��Ӧ�ߵ�ƽ");
	
	// ����40bit���ݣ���5byte * 8bit��
	
	// ��ѭ��1�Σ�һ��byte�ֽڱ����
	for(i = 0; i < 5; i++){
		// ��ѭ��1�Σ�һ��bitλ�����
		for(j = 7; j >= 0; j--){ // 7, 6, 5, 4, 3, 2, 1, 0
			// 5. ���ݣ��͵�ƽʱ�� 54us [50,58]us, ��ǰ��0��ֱ��Ϊ1������ѭ��
			wait_level_change(0, 45, 58, "Data�źŵ͵�ƽ");
	
			// 6. ���ݣ��ߵ�ƽʱ�� ,��ǰ��1��ֱ��Ϊ0������ѭ��
			// �ź�0:  24us [23, 27]us
			// �ź�1:  71us [68, 74]us
			wait_level_change(1, 22, 75, "Data�źŸߵ�ƽ");
			
			// ���磺�յ����� 1001 1011 
			// 0b 0 0 0 0 - 0 0 0 0 Ĭ��ֵ
			// 0b 1 0 0 0 - 0 0 0 0  j == 7
			// 0b 1 0 0 0 - 0 0 0 0  j == 6
			// 0b 1 0 0 0 - 0 0 0 0  j == 5
			// 0b 1 0 0 1 - 0 0 0 0  j == 4
			// 0b 1 0 0 1 - 1 0 0 0  j == 3
			// 0b 1 0 0 1 - 1 0 0 0  j == 2
			// 0b 1 0 0 1 - 1 0 1 0  j == 1
			// 0b 1 0 0 1 - 1 0 1 1  j == 0
			
			// �ж��ߵ�ƽ�ź���0����1
			if(cnt > 50){ // �ߵ�ƽʱ��ϳ�
				// �Ǹߵ�ƽ������Ϊ1 �������ã�Ĭ�Ͼ���0��
				dat[i] |= (1 << j);
			}
		}
	}
	
	DHT = 1;// �����ͷ�����
	
	// for(i = 0; i < 5; i++){
	// 	printf("%d ", (int)dat[i]);
	// }
	// printf("\n");
	
	// printf("last_cnt: %d\n", (int)cnt);
	
	// У������: ǰ�ĸ��ֽڵĺ͵������һ���ֽ�, �����򷵻�-1
	if((dat[0] + dat[1] + dat[2] + dat[3]) != dat[4]){
		printf("check failure!\n");
		return -1;
	}
    
	// 0:�ɹ�, -1:ʧ��
	return 0;
}

/**
 * @brief ��ȡ��ʪ��
 * 
 * @param humidity      ʪ��
 * @param temperature   �¶�
 * @return char �Ƿ�ɹ���ȡ������ 0:�ɹ� -1:ʧ��
 */
char DHT_get_info(u8* humidity, float* temperature){
	u8 dat[5] = {0x00};		// 5byte * 8bit = 40bit
	char rst = on_read(dat);
	// printf("rst: %d\n", (int)rst);
	
	if(rst != SUCCESS){
		return rst;
	}
	
	// ֱ��ȡʪ�ȵ��������֣�������С������
	*humidity = dat[0];
	// ȡ���¶ȵ��������� + С������*0.1(��7λ)
	*temperature = dat[2] + ((dat[3] & 0x7F) * 0.1);
	// �¶�С���������λ: 0��ʾ���¶ȣ�1��ʾ���¶�
	if (dat[3] >> 7){ 		// ���λ��1  �� dat[3] & (1 << 7)
		*temperature *= -1; // ��ԭ����������ȡ��
	}
	// printf("humidity: %d %% temperature: %.2f �� \n", (int)*humidity, *temperature);
	
	return 0;
}