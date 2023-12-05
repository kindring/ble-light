
#include "clock.h"
#include "log.h"
#include "gpio.h"
#include "light.h"

#include "btn.h"


uint8 task_btn_id;
#define KEY_DEMO_ONCE_TIMER      0x0001
#define KEY_DEMO_CYCLE_TIMER     0x0002

int last_release_btn_ind = -1;

void evt_press_release(int i){
	// 短按抬起 
	osal_start_timerEx(task_btn_id, BTN_EVT_TIME_CHECK, TIME_CHECK_TEMP * 3);
	// 500ms 后检测是否
	last_release_btn_ind = i;
}


static void key_press_evt(uint8_t i,key_evt_t key_evt)
{
	LOG("\nkey index:%d gpio:%d ",i,key_state.key[i].pin);
	switch(key_evt)
	{
		case HAL_KEY_EVT_PRESS:
			LOG("key(press down)\n");	
			if (last_release_btn_ind != -1)
			{
				if (last_release_btn_ind == i)	
				{
					// 按下后再次按下
					
				}
			}else {
				osal_stop_timeEx(task_btn_id, BTN_EVT_TIME_CHECK)
			}
			
#ifdef HAL_KEY_SUPPORT_LONG_PRESS
			osal_start_timerEx(task_btn_id, KEY_DEMO_LONG_PRESS_EVT, HAL_KEY_LONG_PRESS_TIME);
#endif
			break;
		
		case HAL_KEY_EVT_RELEASE:
			LOG("key(press release)\n");
			evt_press_release(i);
			break;

#ifdef HAL_KEY_SUPPORT_LONG_PRESS		
		case HAL_KEY_EVT_LONG_RELEASE:
			LOG("key(long press release)\n");
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
	if(task_id != task_btn_id){
		return 0;
	}
	if( events & KEY_DEMO_ONCE_TIMER){		
		LOG("once timer\n\n");
		osal_start_timerEx( task_btn_id, KEY_DEMO_ONCE_TIMER , 5000);
		return (events ^ KEY_DEMO_ONCE_TIMER);
	}
	if( events & KEY_DEMO_CYCLE_TIMER){		
		LOG("recycle timer\n\n");
		return (events ^ KEY_DEMO_CYCLE_TIMER);
	}
	
	if( events & HAL_KEY_EVENT){	
		LOG("HAL_KEY_EVENT \n");											//do not modify,key will use it
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
		// LOG("KEY_DEMO_LONG_PRESS_EVT\n");
		for (int i = 0; i <= HAL_KEY_NUM; i++){
			// LOG("%d --- state [%d]",i, key_state.key[i].state);
			if(key_state.key[i].state == HAL_KEY_EVT_PRESS)
			{
				osal_start_timerEx(task_btn_id, KEY_DEMO_LONG_PRESS_EVT, HAL_KEY_LONG_PRESS_TIME);
			}
		}
		return (events ^ KEY_DEMO_LONG_PRESS_EVT);
	}
#endif

	if( events & EVT_TIME_CHECK){
		LOG("EVT_TIME_CHECK\n");
		return (events ^ EVT_TIME_CHECK);
	}
	
	return 0;

}
