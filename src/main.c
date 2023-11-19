


#include "rf_phy_driver.h"
#include "global_config.h"
#include "jump_function.h"

#include "ll_debug.h"
#include "ll_hw_drv.h"
#include "ll_sleep.h"

//#include "touch_key.h"
#include "uart.h"
#include "gpio.h"
#include "clock.h"
#include "common.h"
#include "log.h"
#include "adc.h"
#include "pwrmgr.h"
#include "string.h"
// #include "phy_console.h"

extern void  ble_main(void);

extern void hal_rom_code_ini(void);
extern int app_main(void);
extern void init_config(void);

extern  uint32_t pclk;

#define     LARGE_HEAP_SIZE  6*1024
uint8       g_largeHeap[LARGE_HEAP_SIZE];
volatile uint8 g_clk32K_config;

static void rf_wakeup_handler(void){
  NVIC_SetPriority((IRQn_Type)BB_IRQ, IRQ_PRIO_REALTIME);
  NVIC_SetPriority((IRQn_Type)CP_TIMER_IRQ, IRQ_PRIO_HIGH);
}


static void hal_init(void)
{

// 用于初始化系统。 它可能根据“g_system_clk”的值设置系统时钟，“g_system_clk”是提供给函数的参数。 系统时钟可配置为使用特定频率或源，例如 32MHz 的 DLL（延迟锁定环）或 16MHz 的外部晶体。  
  hal_system_init(g_system_clk);//(SYS_CLK_DLL_32M);//SYS_CLK_XTAL_16M);   //system init

  //hal_rtc_clock_config(CLK_32K_XTAL);//CLK_32K_RCOSC)CLK_32K_XTAL;
// 配置实时时钟
  hal_rtc_clock_config(g_clk32K_config);
  
//  用于配置各种 SRAM（静态随机存取存储器）块的保留。 这可能确保特定 SRAM 块在低功耗模式或睡眠状态期间保留其内容。
  hal_pwrmgr_RAM_retention(RET_SRAM0|RET_SRAM1|RET_SRAM2|RET_SRAM3|RET_SRAM4);
  
  //LOG_INIT();
//   初始化gpio模块
  hal_gpio_init();
//   初始化 ADC（模数转换器）模块
  hal_adc_init();
	
//   console_init(s_cmd_list, cons_callback);
	
  LOG("all driver init OK!\n");

}

static void hal_rfphy_init(void)
{
    //============config the txPower
    g_rfPhyTxPower  = RF_PHY_TX_POWER_0DBM ;
    //============config BLE_PHY TYPE
    g_rfPhyPktFmt   = PKT_FMT_BLE1M;
    //============config RF Frequency Offset
    g_rfPhyFreqOffSet   =RF_PHY_FREQ_FOFF_00KHZ;

    hal_rom_code_ini();
    
    //Quick Boot setting and 
     *(volatile uint32_t *) 0x4000f01c = 0x0000004;       //  3'b1xx: 62.5us.  control bits for main digital part reset wait time after power up charge pump. 

    //========= low currernt setting IO init
    //========= pull all io to gnd by default
    *(volatile uint32_t *) 0x4000f008 = 0x36db6db6;//P00 - P09 pull down
    *(volatile uint32_t *) 0x4000f00c = 0x36db6db6;//P10 - P19 pull down
    *(volatile uint32_t *) 0x4000f010 = 0x36db6db6;//P20 - P29 pull down
    *(volatile uint32_t *) 0x4000f014 = 0xb0c3edb6;//P30 - P34 pull donw

    //========= UART RX Pull up
    hal_gpio_pull_set(P10,WEAK_PULL_UP);    

    DCDC_CONFIG_SETTING(0x0d);


    NVIC_SetPriority((IRQn_Type)BB_IRQ, IRQ_PRIO_REALTIME);
    NVIC_SetPriority((IRQn_Type)CP_TIMER_IRQ, IRQ_PRIO_HIGH);

    hal_pwrmgr_register(MOD_USR0, NULL, rf_wakeup_handler);

}


int  main(void)  
{     
    g_system_clk = SYS_CLK_DLL_48M;
    g_clk32K_config = CLK_32K_XTAL;//CLK_32K_XTAL,CLK_32K_RCOSC
    
    osal_mem_set_heap((osalMemHdr_t *)g_largeHeap, LARGE_HEAP_SIZE);
	
    init_config();

    hal_pwrmgr_init();

    hal_rfphy_init();  
    							
    hal_init();

    app_main();	

}
