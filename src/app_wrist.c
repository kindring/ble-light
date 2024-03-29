#include <string.h>

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "gatt.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "ota_app_service.h"
#include "gatt_profile_uuid.h"
// #include "wristservice.h"
#include "devinfoservice.h"
#include "peripheral.h"
#include "gapbondmgr.h"

// #include "led_light.h"
#include "kscan.h"
#include "log.h"
#include "ll.h"

#include "adc.h"


#include "light.h"
#include "app_wrist.h"
#include "wrist_service.h"


// GAP connection handle
// 事件处理Id
uint8 AppWrist_TaskID;  
static uint16 gapConnHandle;
static gaprole_States_t gapProfileState = GAPROLE_INIT;

static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = BleName;
// Advertising user-cancelled state
static bool WristAdvCancelled = FALSE;

static uint8 scanData[] =
{
  0xa,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'b',
  'l',
  'e',
  'L',
  'i',
  'g',
  'h',
  't',
  ' ',
};

static uint8 advertData[] = 
{ 
  // flags
  0x02,
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
  // service UUIDs
  0x03,
  GAP_ADTYPE_16BIT_MORE,
  LO_UINT16(HEARTRATE_SERV_UUID),
  HI_UINT16(HEARTRATE_SERV_UUID),
};





/**
 * @fn      appProcOSALMsg
 * 
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
*/
static void appWristProcOSALMsg( osal_event_hdr_t *pMsg );
static void WristGapStateCB( gaprole_States_t newState);

// GAP Role Callbacks
static gapRolesCBs_t WristPeripheralCB =
{
  WristGapStateCB,  // Profile State Change Callbacks
  NULL                            // When a valid RSSI is read from controller
};


static void appWristProcOSALMsg( osal_event_hdr_t *pMsg )
{
    LOG("[fn : appWristProcOSALMsg]  run event is: %d \n", pMsg->event);
}

// Notification from the profile of a state change
static void WristGapStateCB(gaprole_States_t newState)
{
    LOG("app is start gapProfileState: %d WristGapStateCB: %d  \n", gapProfileState,  newState);
    // if connected
    if (newState == GAPROLE_CONNECTED)
    {
        // get connection handle
        GAPRole_GetParameter(GAPROLE_CONNHANDLE, &gapConnHandle);
    }
    // if disconnected
    else if (gapProfileState == GAPROLE_CONNECTED && 
            newState != GAPROLE_CONNECTED)
    {
        uint8 advState = TRUE;
        wristProfile_HandleConnStatusCB( gapConnHandle, LINKDB_STATUS_UPDATE_REMOVED );
        
        if ( newState == GAPROLE_WAITING_AFTER_TIMEOUT )
        {
            // link loss timeout-- use fast advertising
            GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_FAST_ADV_INTERVAL );
            GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_FAST_ADV_INTERVAL );
            GAP_SetParamValue( TGAP_GEN_DISC_ADV_MIN, DEFAULT_FAST_ADV_DURATION );
        }
        else
        {
            // Else use slow advertising
            GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_SLOW_ADV_INTERVAL );
            GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_SLOW_ADV_INTERVAL );
            GAP_SetParamValue( TGAP_GEN_DISC_ADV_MIN, DEFAULT_SLOW_ADV_DURATION );
        }

        // Enable advertising
        GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advState );    
    
    }
    // if advertising stopped
    else if ( gapProfileState == GAPROLE_ADVERTISING && 
            newState == GAPROLE_WAITING )
    {
         // if advertising stopped by user
        if ( WristAdvCancelled )
        {
        WristAdvCancelled = FALSE;
        }
        // if fast advertising switch to slow
        else if ( GAP_GetParamValue( TGAP_GEN_DISC_ADV_INT_MIN ) == DEFAULT_FAST_ADV_INTERVAL )
        {
        uint8 advState = TRUE;
        
        GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_SLOW_ADV_INTERVAL );
        GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_SLOW_ADV_INTERVAL );
        GAP_SetParamValue( TGAP_GEN_DISC_ADV_MIN, DEFAULT_SLOW_ADV_DURATION );
        GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advState );   
        }  
    }
    // if started
  else if (newState == GAPROLE_STARTED)
  {
    // Set the system ID from the bd addr
    uint8 systemId[DEVINFO_SYSTEM_ID_LEN];
    GAPRole_GetParameter(GAPROLE_BD_ADDR, systemId);
    
    // shift three bytes up
    systemId[7] = systemId[5];
    systemId[6] = systemId[4];
    systemId[5] = systemId[3];
    
    // set middle bytes to zero
    systemId[4] = 0;
    systemId[3] = 0;
    
    DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
  }
  
  gapProfileState = newState;
}


static void wristCB(uint8 event, uint8 param_size, uint8* param)
{
  switch(event){

    case WRIST_NOTI_ENABLED:
    
        // if connected start periodic measurement
        if (gapProfileState == GAPROLE_CONNECTED)
        {
        //osal_start_timerEx( AppWrist_TaskID, HEART_PERIODIC_EVT, DEFAULT_HEARTRATE_PERIOD );
        }
        break;
    
    case WRIST_NOTI_DISABLED:
        // stop periodic measurement
        //osal_stop_timerEx( AppWrist_TaskID, HEART_PERIODIC_EVT );
        break;
    
  }
}


static void user_key_1_handler(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	LOG("user key 1 handler:%d %d\n", pin, type);
} 
static void user_key_2_handler(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	LOG("user key 2 handler:%d %d\n", pin, type);
}
static void user_key_3_handler(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	LOG("user key 3 handler:%d %d\n", pin, type);
}
static void user_key_4_handler(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	LOG("user key 4 handler:%d %d\n", pin, type);
}

static void user_key_1_handler_ppp(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	LOG("+++ 1 handler ppp:%d %d\n", pin, type);
} 
static void user_key_2_handler_ppp(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	LOG("+++ 2 handler ppp:%d %d\n", pin, type);
}
static void user_key_3_handler_ppp(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	LOG("+++ 3 handler ppp:%d %d\n", pin, type);
}
static void user_key_4_handler_ppp(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	LOG("+++ 4 handler ppp:%d %d\n", pin, type);
}

static void getPinState(){
	LOG("---------------------------------\n\n\n");
	LOG("P01:%d\n",hal_gpio_read(GPIO_P01));
	LOG("P02:%d\n",hal_gpio_read(GPIO_P02));
	LOG("P03:%d\n",hal_gpio_read(GPIO_P03));
	LOG("P04:%d\n",hal_gpio_read(GPIO_P04));
	LOG("P05:%d\n",hal_gpio_read(GPIO_P05));
	LOG("---------------------------------\n");
}

void test_adc_evt(adc_Evt_t* pev){
    int i = 0;
    LOG("test_adc_evt\n");
    if(pev->type == HAL_ADC_EVT_DATA){
        LOG("HAL_ADC_EVT_DATA\n");
    }
    // typedef struct _adc_Evt_t{
    //     int       type;
    //     adc_CH_t  ch;
    //     uint16_t* data;
    //     uint8_t   size; //word size
    // }adc_Evt_t;
    LOG("adc event type:%d ch:%d\n", pev->type, pev->ch);
    // 输出adc数据
    LOG("[adc data] size:%d value: ", pev->size);
    for(i = 0; i < pev->size; i++){
        LOG("%02x", pev->data[i]);
    }
    LOG("\n");
    uint16 adc_buf[6][64];
    osal_memcpy(adc_buf[pev->ch-2], pev->data, 2*(pev->size));


    // 输出adcbuf数据
    for(i = 0; i < 6; i++){
        LOG("adc_buf[%d]: ", i);
        for(int j = 0; j < 64; j++){
            LOG("%02x", adc_buf[i][j]);
        }
        LOG("\n");
    }

    float value = 0;
    static uint8_t channel_done_flag = 0;
    channel_done_flag |= BIT(pev->ch);
    for(i=2;i<8;i++)
    {
        if(channel_done_flag & BIT(i))
		{
        // is_high_resolution = (adc_cfg.is_high_resolution & BIT(i))?TRUE:FALSE;
            // is_differential_mode = (adc_cfg.is_differential_mode & BIT(i))?TRUE:FALSE;
            LOG("PEV: %d %d %d\n", i, pev->size, pev->data[0]);
            value = hal_adc_value_cal((adc_CH_t)i, adc_buf[i-2], pev->size, TRUE, FALSE);
            // 最终电压值 
            if(i<7)
            {
                LOG("P%d %d mv val: %d mv V: %d",(i+9),(int)(value*1000), (int)(value * 1000 * 4), (int)(value*1000) * 4 * 12);
                // 输出电压值 V 12倍 浮点数
                LOG("[BATT]: %dmV", (int)(value*1000) * 4 * 13);
            } else
            {
                LOG("P%d %d mv ",(20),(int)(value*1000));
            }
        }
    }	
    // getPinState();
}
void test_adc_read(){
   LOG("[test_adc_read] read Read Adc");
   hal_adc_init();
    GPIO_Pin_e controlPin = P34;
   GPIO_Pin_e adcPin = P14;
   adc_Cfg_t adc_cfg = {
        .channel = ADC_BIT(ADC_CH2P_P14),
        .is_continue_mode = FALSE,
        .is_differential_mode = FALSE,
        .is_high_resolution = TRUE,
    };
   
    // 注册adc
    hal_adc_config_channel(adc_cfg, test_adc_evt);
    LOG("hal_adc_config_channel end\n");
    hal_adc_start();
}


void appWristInit( uint8 task_id)
{
    AppWrist_TaskID = task_id;
	LOG("\n\n\nappWristInit\n\n\n");

    // Setup the GAP Peripheral Role Profile
    {
        uint8 initial_advertising_enable = TRUE;
        uint8 advChnMap = GAP_ADVCHAN_37 | GAP_ADVCHAN_38 | GAP_ADVCHAN_39; 

        // By setting this to zero, the device will go into the waiting state after
        // being discoverable for 30.72 second, and will not being advertising again
        // until the enabler is set back to TRUE
        uint16 gapRole_AdvertOffTime = 0;
        
        uint8 enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
        uint16 desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
        uint16 desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
        uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
        uint16 desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;
        uint8 peerPublicAddr[] = {
                0x01,
                0x02,
                0x03,
                0x04,
                0x05,
                0x06
            };

        GAPRole_SetParameter(GAPROLE_ADV_DIRECT_ADDR, sizeof(peerPublicAddr), peerPublicAddr);
        // set adv channel map
        GAPRole_SetParameter(GAPROLE_ADV_CHANNEL_MAP, sizeof(uint8), &advChnMap);        
        
        // Set the GAP Role Parameters
        GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
        GAPRole_SetParameter( GAPROLE_ADVERT_OFF_TIME, sizeof( uint16 ), &gapRole_AdvertOffTime );
        
        GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanData ), scanData );
        GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );
        
        GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
        GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
        GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
        GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
        GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
    }

    // Set the GAP Characteristics
    GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );


    // Set advertising interval
    {
        uint16 advInt = 400;   // actual time = advInt * 625us

        GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MIN, advInt );
        GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MAX, advInt );
        GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, advInt );
        GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, advInt );
    }

    // Initialize GATT attributes
    GGS_AddService( GATT_ALL_SERVICES );         // GAP
    GATTServApp_AddService( GATT_ALL_SERVICES ); // GATT attributes
    DevInfo_AddService( );
    ota_app_AddService();
    // 初始化蓝牙模块
    wristProfile_AddService(wristCB);

    app_datetime_init();

    // Setup a delayed profile startup
    osal_set_event( AppWrist_TaskID, START_DEVICE_EVT );
    // light_init(-1);
    LOG("appWristInit end\n");
    // 初始化按键
    // temp_set(5500, 0, NULL);
    LOG("temp_set end\n");
    // light_set(10, 0, NULL);
    LOG("appWristInit end\n");
    // test_adc_read();
    // 注册定时器. 持续读取数据
    // osal_start_reload_timer( AppWrist_TaskID, TIMER_LIGHT_EVT, 2000);
}

// 事件处理器
uint16 appWristProcEvt( uint8 task_id, uint16 events )
{
    if ( events & SYS_EVENT_MSG )
    {
        uint8 *pMsg;

        if ( (pMsg = osal_msg_receive( AppWrist_TaskID )) != NULL )
        {
        appWristProcOSALMsg( (osal_event_hdr_t *)pMsg );

        // Release the OSAL message
        VOID osal_msg_deallocate( pMsg );
        }

        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if ( events & START_DEVICE_EVT )
    {
        // LOG("START_DEVICE_EVT\n");
        // Start the Device
        VOID GAPRole_StartDevice( &WristPeripheralCB );

        // Register with bond manager after starting device
    //    GAPBondMgr_Register( (gapBondCBs_t *) &WristBondCB );
        
        return ( events ^ START_DEVICE_EVT );
    }

    if ( events & TIMER_LIGHT_EVT )
    {
        LOG("TIMER_LIGHT_EVT\n");
        return ( events ^ TIMER_LIGHT_EVT );
    }
     return 0;
}
