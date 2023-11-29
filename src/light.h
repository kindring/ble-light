/*
 * @Author: kindring 2460131004@qq.com
 * @Date: 2023-11-20 23:37:29
 * @LastEditors: kindring 2460131004@qq.com
 * @LastEditTime: 2023-11-25 21:37:47
 * @FilePath: \ble-light\src\light.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#ifndef __LIGHT_H__
#define __LIGHT_H__

// 暖光 pwm脚
#define GPIO_WARM P18
// 冷光 pwm脚
#define GPIO_COLD P20

// 风扇 pwm
#define GPIO_FAN P23
// btn +
#define GPIO_BTN_PLUS P26
// btn -
#define GPIO_BTN_MINUS P27


// adc 电源监测脚
#define GPIO_POWER P28

extern int light_init(void);

extern int light_set(uint8_t ch, uint8_t val);

#endif