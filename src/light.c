
#include "pwm.h"
#include "gpio.h"

#include "OSAL.h"
#include "log.h"

#include "pwm_light.h"

#include "light.h"

// 配置 
light_data_t light_data = {5500, 0};

int light_init(){
    LOG("[light_init]\n");
    // 初始化pwm
    int ret = 0;
    // if()
    ret = pwm_light_init(WARM_CH, GPIO_WARM, 1000, 1000, 5, PWM_CLK_DIV_16);

    if(ret != 0){
        LOG("[light_init] pwm_light_init warm failed %d \n", ret);
        return ret;
    }

    ret = pwm_light_init(COLD_CH, GPIO_COLD, 1000, 1000, 5, PWM_CLK_DIV_16);

    if(ret != 0){
        LOG("[light_init] pwm_light_init cold failed  %d \n", ret);
        return ret;
    }

    return ret;
}

int light_ch_set(uint8_t ch, uint16_t val){
    LOG("[light_set] set ch%d val to %d \n", ch, val);
    int ret = pwm_light_set_val(ch, val);
    return ret;
}




// 计算 冷暖光的亮度值
int comLightVal(){
    int light_val = light_data.light, temp_val = light_data.temp;
    // 根据色温与亮度来计算冷暖灯光对应的亮度值
    // 亮度值范围 0~100 翻 100倍
    // 色温范围 2500~6500
    // 暖色温最大亮度值 = 亮度值 * (色温 - 2500) / (6500 - 2500)
    // 亮度调整偏移后的暖色温亮度值 = 暖色温最大亮度值 + 亮度值 * 2500 / (6500 - 2500)
    int tmp_warm_val = 1000 * (temp_val - 2500) / (6500 - 2500);
    int tmp_cold_val = 1000 - tmp_warm_val;
    int warm_val = light_val * tmp_warm_val / 50;
    int cold_val = light_val * tmp_cold_val / 50;
    LOG("[comLightVal] warm_val %d, cold_val %d \n", warm_val, cold_val);
    light_ch_set(WARM_CH, warm_val);
    light_ch_set(COLD_CH, cold_val);
    return 0;
}

// 亮度调节
int light_set(uint8_t val){
    LOG("[light_set] set light val to %d \n", val);
    // 亮度值为 0~100
    light_data.light = val;
    comLightVal();
    return 0;
}

// 色温调节
int temp_set(int temp){
    LOG("[temp_set] set temp val to %d \n", temp);
    light_data.temp = temp;
    comLightVal();
    return 0;
}

// js code
// function comLightVal(light_val, temp_val){
//     // 根据色温与亮度来计算冷暖灯光对应的亮度值
//     // 亮度值范围 0~100
//     // 色温范围 2500~6500
//     // 暖色温最大亮度值 = 亮度值 * (色温 - 2500) / (6500 - 2500)
//     // 亮度调整偏移后的暖色温亮度值 = 暖色温最大亮度值 + 亮度值 * 2500 / (6500 - 2500)
//     let tmp_warm_val = 100 * (temp_val - 2500) / (6500 - 2500);
//     let tmp_cold_val = 100 - tmp_warm_val;
//     let warm_val = light_val * tmp_warm_val / 100;
//     let cold_val = light_val * tmp_cold_val / 100;
//     console.log(`[comLightVal] warm_val: ${warm_val} , cold_val: ${cold_val}\n`);
// }
