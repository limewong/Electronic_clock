#include "DHT.h"
#include "GPIO.h"
#include "delay.h"

static void GPIO_config(void) {
  GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
  GPIO_InitStructure.Pin  = GPIO_Pin_6;		//指定要初始化的IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P4, &GPIO_InitStructure);//初始化
}
/**
 * @brief 初始化
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
等待DHT电平变化
************************/
#define wait_level_change(level, min, max, desc) 				                        			\
		cnt = 0;                                                                        	\ 
		while(DHT == level){														    															\ 																																			
			delay_1us();                                                                        \ 
			cnt++;                                                                              \ 
		};                                                    																\ 
		if(cnt < min || cnt > max){                                                             \ 
			/*不符合响应时间*/                                                                   \ 
			printf("err: [%d]us, 不满足%s时间 [%d,%d]\n", (int)cnt, desc, (int)min, (int)max);     \
			return -1;                                                                          \ 
		}                                                                                       \ 

// 不要随便打日志
char on_read(u8 *dat){ // 由外部传入一个数组（指针），函数执行完毕，此数组被填充好数据
	
	u16 cnt = 0; 					// 计时器，每+1，过了1us, 一定要初始化为0
//	u8 dat[5] = {0x00};		// 5byte * 8bit = 40bit
	char i, j;
	// 读取DHT11数据
	// printf("read\n");
	
	// 1. 主机起始信号拉低 20ms
	DHT = 0;
	delay_ms(20);
	DHT = 1;
	
	// 2. 等待主机释放总线
	cnt = 0;
	while(DHT == 1 && cnt < 40){
		delay_1us();
		cnt++;
	}
	if(cnt < 8 || cnt > 35){
		// 不符合响应时间
		printf("err: 时间[%d], 主机释放总线[%d,%d]\n", (int)cnt,(int)10, (int)35);
		return -1;
	}
	
	// 3. 响应低电平时间, 83us [78,88]us, 当前是0，直到为1，结束循环
	wait_level_change(0, 78, 88, "Resp响应低电平");
	
	// 4. 响应高电平时间，87us [80,92]us, 当前是1，直到为0，结束循环
	wait_level_change(1, 80, 92, "Resp响应高电平");
	
	// 解析40bit数据，（5byte * 8bit）
	
	// 外循环1次，一个byte字节被填充
	for(i = 0; i < 5; i++){
		// 内循环1次，一个bit位被填充
		for(j = 7; j >= 0; j--){ // 7, 6, 5, 4, 3, 2, 1, 0
			// 5. 数据：低电平时间 54us [50,58]us, 当前是0，直到为1，结束循环
			wait_level_change(0, 45, 58, "Data信号低电平");
	
			// 6. 数据：高电平时间 ,当前是1，直到为0，结束循环
			// 信号0:  24us [23, 27]us
			// 信号1:  71us [68, 74]us
			wait_level_change(1, 22, 75, "Data信号高电平");
			
			// 例如：收到数据 1001 1011 
			// 0b 0 0 0 0 - 0 0 0 0 默认值
			// 0b 1 0 0 0 - 0 0 0 0  j == 7
			// 0b 1 0 0 0 - 0 0 0 0  j == 6
			// 0b 1 0 0 0 - 0 0 0 0  j == 5
			// 0b 1 0 0 1 - 0 0 0 0  j == 4
			// 0b 1 0 0 1 - 1 0 0 0  j == 3
			// 0b 1 0 0 1 - 1 0 0 0  j == 2
			// 0b 1 0 0 1 - 1 0 1 0  j == 1
			// 0b 1 0 0 1 - 1 0 1 1  j == 0
			
			// 判定高电平信号是0还是1
			if(cnt > 50){ // 高电平时间较长
				// 是高电平，设置为1 （不设置，默认就是0）
				dat[i] |= (1 << j);
			}
		}
	}
	
	DHT = 1;// 主机释放总线
	
	// for(i = 0; i < 5; i++){
	// 	printf("%d ", (int)dat[i]);
	// }
	// printf("\n");
	
	// printf("last_cnt: %d\n", (int)cnt);
	
	// 校验数据: 前四个字节的和等于最后一个字节, 不等则返回-1
	if((dat[0] + dat[1] + dat[2] + dat[3]) != dat[4]){
		printf("check failure!\n");
		return -1;
	}
    
	// 0:成功, -1:失败
	return 0;
}

/**
 * @brief 获取温湿度
 * 
 * @param humidity      湿度
 * @param temperature   温度
 * @return char 是否成功获取到数据 0:成功 -1:失败
 */
char DHT_get_info(u8* humidity, float* temperature){
	u8 dat[5] = {0x00};		// 5byte * 8bit = 40bit
	char rst = on_read(dat);
	// printf("rst: %d\n", (int)rst);
	
	if(rst != SUCCESS){
		return rst;
	}
	
	// 直接取湿度的整数部分，不考虑小数部分
	*humidity = dat[0];
	// 取出温度的整数部分 + 小数部分*0.1(低7位)
	*temperature = dat[2] + ((dat[3] & 0x7F) * 0.1);
	// 温度小数部分最高位: 0表示正温度，1表示负温度
	if (dat[3] >> 7){ 		// 最高位是1  或 dat[3] & (1 << 7)
		*temperature *= -1; // 对原数据正负号取反
	}
	// printf("humidity: %d %% temperature: %.2f ℃ \n", (int)*humidity, *temperature);
	
	return 0;
}