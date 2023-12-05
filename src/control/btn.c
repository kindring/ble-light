
#include "clock.h"
#include "log.h"
#include "gpio.h"
#include "light.h"

#include "btn.h"


uint8 task_btn_id;
#define KEY_DEMO_ONCE_TIMER      0x0001
#define KEY_DEMO_CYCLE_TIMER     0x0002


// btn 0 add
// btn 1 sub

// 是否在调节亮度
bool btn_mode = false;
// 断案按钮抬起事件 btn
int btn_ind = -1;
int next_btn_ind ;

int temp = 2300;
// int temp_speed = 50;
int light = 0;
// int light_speed = 5;
void temp_add(int temp_speed){
	int next_temp = temp + temp_speed;
	if(next_temp > 6500){
		next_temp = 6500;
	}
	LOG("temp %d ===>> %d\n", temp, next_temp);
	temp = next_temp;
}
void temp_sub(int temp_speed){
	int next_temp = temp - temp_speed;
	if(next_temp < 2300){
		next_temp = 2300;
	}
	LOG("temp %d ===>> %d\n", temp, next_temp);
	temp = next_temp;
}

void light_add(int light_speed){
	int next_light = light + light_speed;
	if(next_light > 100){
		next_light = 100;
	}
	LOG("light %d ===>> %d\n", light, next_light);
	light = next_light;
	light_set(0, light);
	light_set(1, light);
}

void light_sub(int light_speed){
	int next_light = light - light_speed;
	if(next_light < 0){
		next_light = 0;
	}
	LOG("light %d ===>> %d\n", light, next_light);
	light = next_light;
	light_set(0, light);
	light_set(1, light);
}

void btn_mode_switch(){
	btn_mode = !btn_mode;
	LOG("btn_mode change to: %s\n",btn_mode?"light":"temp");
}

btn_all_pressed_t btn_all_pressed[ALL_PRESSED_LEN] = {
	{
		.len = 2,
		.btns = (uint8_t[]){0, 1},
		.callback = btn_mode_switch,
	}
};

void evt_press_release(int i){
	btn_ind = i;
	if(btn_mode)
	{
		btn_mode = false;
		btn_ind = -1;
	}

	// 短按抬起 
	osal_start_timerEx(task_btn_id, EVT_TIME_CHECK, TIME_CHECK_TEMP);
}

// 判断按键是否同时按下
bool btn_is_all_pressed(){
	bool is_all_pressed = false;
	// 遍历所有的同时按下的按键
	for (int i = 0; i < ALL_PRESSED_LEN; i++)
	{
		// 遍历每个同时按下的按键的按键数组
		for (int j = 0; j < btn_all_pressed[i].len; j++)
		{
			// 如果按键数组中有一个按键不是按下状态,则跳过
			if(key_state.key[btn_all_pressed[i].btns[j]].state != HAL_KEY_EVT_PRESS){
				break;
			}
			// 如果按键数组中的所有按键都是按下状态,则执行回调函数
			if(j == btn_all_pressed[i].len - 1){
				btn_all_pressed[i].callback();
				return true;
			}
		}
	}
	return is_all_pressed;
}

static void key_press_evt(uint8_t i,key_evt_t key_evt)
{
	LOG("\nkey index:%d gpio:%d ",i,key_state.key[i].pin);
	switch(key_evt)
	{
		case HAL_KEY_EVT_PRESS:
			LOG("key(press down)\n");		
			
#ifdef HAL_KEY_SUPPORT_LONG_PRESS
			osal_start_timerEx(task_btn_id, KEY_DEMO_LONG_PRESS_EVT, HAL_KEY_LONG_PRESS_TIME);
#endif
			break;
		
		case HAL_KEY_EVT_RELEASE:
			LOG("key(press release)\n");
			evt_press_release(i);
			switch (i)
			{
			case 0:
				light_add(10);
				break;
			case 1:
				light_sub(10);
				break;
			default:
				break;
			}
			break;

#ifdef HAL_KEY_SUPPORT_LONG_PRESS		
		case HAL_KEY_EVT_LONG_RELEASE:
			LOG("key(long press release)\n");
			// 检测是否有同时按下的按键, 只需要在长按事件中检测即可
			// btn_is_all_pressed();
			if(btn_mode){
				btn_mode = false;
				btn_ind = -1;
			}
			break;
#endif
		
		default:
			LOG("unexpect\n");
			break;
	}
}

// key_state 引入

// 初始化按钮
int btn_init(task_id){
    LOG("[btn_init]\n");
    task_btn_id = task_id;
    // 初始化pwm
    for (int i = 0; i < HAL_KEY_NUM; i++)
    {
		hal_gpio_pin_init(btn_pins[i], IE);
        key_state.key[i].pin = btn_pins[i];
        key_state.key[i].state = HAL_KEY_EVT_PRESS;
        key_state.key[i].idle_level = HAL_HIGH_IDLE;
    }
    // 注册同时按下事件
	
    key_state.task_id = task_btn_id;
	key_state.key_callbank = key_press_evt;
	key_init();

    return 1;
}

// 按钮事件处理函数
uint16 Key_ProcessEvent( uint8 task_id, uint16 events )
{
	// LOG("Key_ProcessEvent\n");
	// LOG("key event:%04x\n",events);
	if(task_id != task_btn_id){
		return 0;
	}
	if( events & KEY_DEMO_ONCE_TIMER){		
		//LOG("once timer\n\n");
		osal_start_timerEx( task_btn_id, KEY_DEMO_ONCE_TIMER , 5000);
		return (events ^ KEY_DEMO_ONCE_TIMER);
	}
	if( events & KEY_DEMO_CYCLE_TIMER){		
		//LOG("recycle timer\n\n");
		return (events ^ KEY_DEMO_CYCLE_TIMER);
	}
	
	if( events & HAL_KEY_EVENT){	
		// LOG("HAL_KEY_EVENT \n");											//do not modify,key will use it
		for (uint8 i = 0; i < HAL_KEY_NUM; ++i){
			if ((key_state.temp[i].in_enable == TRUE)||
			(key_state.key[i].state == HAL_STATE_KEY_RELEASE_DEBOUNCE)){
				gpio_key_timer_handler(i);
			}

			
			
		}
		return (events ^ HAL_KEY_EVENT);
	}

#ifdef HAL_KEY_SUPPORT_LONG_PRESS	

	if( events & KEY_DEMO_LONG_PRESS_EVT){
		LOG("KEY_DEMO_LONG_PRESS_EVT\n");
		for (int i = 0; i < HAL_KEY_NUM; ++i){
			if(key_state.key[i].state == HAL_KEY_EVT_PRESS){
				LOG("key:%d gpio:%d	",i,key_state.key[i].pin);
				LOG("key(long press down)\n");
				osal_start_timerEx(task_btn_id, KEY_DEMO_LONG_PRESS_EVT, HAL_KEY_LONG_PRESS_TIME);
				//user app code long press down process 
				if (btn_mode && i != btn_ind && btn_ind != -1){
					// 色温调节
					switch (i)
					{
					case 0:
						temp_add(50);
						break;
					case 1:
						temp_sub(50);
						break;
					default:
						break;
					}
				} else {
				// 长按 但是没有在调节
				switch (i)
				{
				case 0:
					light_add(1);
					break;
				case 1:
					light_sub(1);
					break;
				default:
					break;
				}
			}
		}
		return (events ^ KEY_DEMO_LONG_PRESS_EVT);
	}
#endif

	if( events & EVT_TIME_CHECK){
		LOG("EVT_TIME_CHECK\n");
		
		switch (btn_ind)
		{
		case 0:
			// 如果另一个按钮已经按下,则切换模式
			next_btn_ind = 1;
			break;
		case 1:
			// 如果另一个按钮已经按下,则切换模式
			next_btn_ind = 0;
			break;
		default:
			next_btn_ind = -1;
			break;
		}
		// 如果另一个按钮已经按下,则切换模式
		if(
			next_btn_ind != -1 && (key_state.key[next_btn_ind].state == HAL_KEY_EVT_PRESS || key_state.key[next_btn_ind].state == HAL_KEY_EVT_LONG_PRESS)
		){
			btn_mode = true;
		}
		return (events ^ EVT_TIME_CHECK);
	}
	
	return 0;

}
}