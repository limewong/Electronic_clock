#ifndef __DHT_H__
#define __DHT_H__

#include "config.h"

#define DHT 	P46
/**
 * @brief 初始化
 * 
 */
void DHT_init();

/**
 * @brief 获取温湿度
 * 
 * @param humidity      湿度
 * @param temperature   温度
 * @return char 是否成功获取到数据 0:成功 -1:失败
 */
char DHT_get_info(u8* humidity, float* temperature);

#endif