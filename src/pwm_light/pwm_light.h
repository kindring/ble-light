#ifndef __PWM_LIGHT_H__
#define __PWM_LIGHT_H__

#include "gpio.h"

// 判断通道是否合法 0 - max_pwm_light
#define CHECK_PWM_CH if(ch < 0 || ch > max_pwm_light){return -1;}
   
#define CHECK_PWM_PIN if(pin < P0 || pin > P34){return -1;}


// pwm 结构体
typedef struct pwm_t {
    PWMN_e pwm_ch;// pwm通道
    GPIO_Pin_e pin;// gpio引脚
    uint8_t val;// 当前值 15
    uint8_t total;// 总量 0-100
    uint8_t step;// 步长 5
    PWM_CLK_DIV_e div;// pwm时钟分频
} pwm_t;

// pwm结构体数组 不定长 最大5个
extern pwm_t pwm_light_list[];


/**
 * @brief 获取pwm
 * @param uint8_t ch 通道
 * @return pwm_t* pwm结构体
*/
extern pwm_t *pwm_light_get(uint8_t ch);

/**
 * @brief 初始化pwm
 * @param uint8_t ch 通道
 * @param GPIO_Pin_e pin 引脚编号
 * @param int val 值
 * @param uint8_t total 总量
 * @param uint8_t step 步长
 * @param PWM_CLK_DIV_e div 分频
 * @return 0 成功 -1 失败
*/
extern int pwm_light_init(uint8_t ch, GPIO_Pin_e pin, int val, uint8_t total, uint8_t step, PWM_CLK_DIV_e div);

/**
 * @brief 设置pwm总量
 * @param uint8_t ch 通道
 * @param int total 总量
 * @return 0 成功 -1 失败
*/
extern int pwm_light_set_total(uint8_t ch, int total )

/**
 * @brief 设置pwm引脚
 * @param uint8_t ch 通道
 * @param GPIO_Pin_e pin 引脚编号
 * @return 0 成功 -1 失败
*/
extern int pwm_light_set_pin(uint8_t ch, GPIO_Pin_e pin);


/**
 * @brief 设置pwm值
 * @param uint8_t ch 通道
 * @param val 新值
 * @return 0 成功 -1 失败
*/
extern int pwm_light_set_val(uint8_t ch , uint8_t val);

/**
 * @brief 设置pwm频率
 * @param uint8_t ch 通道
 * @param div 目标频率
*/
extern int pwm_light_set_div(uint8_t ch , PWM_CLK_DIV_e div);


#endif