#ifndef __DHT_H__
#define __DHT_H__

#include "config.h"

#define DHT 	P46
/**
 * @brief ��ʼ��
 * 
 */
void DHT_init();

/**
 * @brief ��ȡ��ʪ��
 * 
 * @param humidity      ʪ��
 * @param temperature   �¶�
 * @return char �Ƿ�ɹ���ȡ������ 0:�ɹ� -1:ʧ��
 */
char DHT_get_info(u8* humidity, float* temperature);

#endif