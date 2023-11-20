#include "pwm.h"
#include "gpio.h"

#include "pwm_light.h"

// 申请5个通道的内存
pwm_t pwm_light_chann_list[5];

// 刷新 pwm 引脚
static void pwm_light_reflash(void)
{

    // 遍历 pwm_light_chann_list
    for (int i = 0; i < sizeof(pwm_light_chann_list) / sizeof(pwm_t); i++)
    {
        // 获取 pwm
        pwm_t *pwm = &pwm_light_chann_list[i];

        hal_pwm_close_channel(pwm->pwm_ch);
        hal_pwm_destroy(pwm->pwm_ch);

        hal_pwm_stop();

        hal_gpio_pin_init(pwm->pin, IE);
        hal_gpio_write(pwm->pin, WEAK_PULL_UP);

        hal_pwm_init(pwm->pwm_ch, pwm->div, PWM_CNT_UP, PWM_POLARITY_FALLING);

        hal_pwm_set_count_val(PWM_CH0, pwm->val, pwm->total);

        hal_pwm_open_channel(PWM_CH1, pwm->pin);
    }
    hal_pwm_start();
}

// 创建 pwm
int pwm_light_init(pwm_t *pwm)
{
    // 判断 pwm_ch 是否合法
    if (pwm->pwm_ch < PWM_CH0 || pwm->pwm_ch > PWM_CH4)
    {
        return -1;
    }

    // 判断 pin 是否合法
    if (pwm->pin < GPIO_P00 || pwm->pin > GPIO_P34)
    {
        return -2;
    }

    // 判断 val 是否合法
    if (pwm->val > pwm->total)
    {
        return -3;
    }

    // 判断 step 是否合法
    if (pwm->step > pwm->total)
    {
        return -4;
    }

    // 判断 div 是否合法
    if (pwm->div < PWM_CLK_DIV_2 || pwm->div > PWM_CLK_DIV_128)
    {
        return -5;
    }

    // 判断 pwm_light_chann_list 是否已满
    if (sizeof(pwm_light_chann_list) / sizeof(pwm_t) >= 5)
    {
        return -6;
    }

    // 判断 pwm 是否已存在
    for (int i = 0; i < sizeof(pwm_light_chann_list) / sizeof(pwm_t); i++)
    {
        if (pwm_light_chann_list[i].pwm_ch == pwm->pwm_ch)
        {
            return -7;
        }
    }

    // 添加 pwm
    pwm_light_chann_list[sizeof(pwm_light_chann_list) / sizeof(pwm_t)] = *pwm;

    // 刷新 pwm 引脚
    pwm_light_reflash();

    return 0;
}

int pwm_light_set_val(pwm_t *pwm, uint8_t val)
{
    // 判断 pwm_ch 是否合法
    if (pwm->pwm_ch < PWM_CH0 || pwm->pwm_ch > PWM_CH4)
    {
        return -1;
    }

    // 判断 val 是否合法
    if (val > pwm->total)
    {
        return -2;
    }

    // 设置 pwm
    pwm->val = val;

    // 刷新 pwm 引脚
    pwm_light_reflash();

    return 0;
}

int pwm_light_set_div(pwm_t *pwm, PWM_CLK_DIV_e div)
{
    // 判断 pwm_ch 是否合法
    if (pwm->pwm_ch < PWM_CH0 || pwm->pwm_ch > PWM_CH4)
    {
        return -1;
    }

    // 判断 div 是否合法
    if (div < PWM_CLK_DIV_2 || div > PWM_CLK_DIV_128)
    {
        return -2;
    }

    // 设置 pwm
    pwm->div = div;

    // 刷新 pwm 引脚
    pwm_light_reflash();

    return 0;
}
