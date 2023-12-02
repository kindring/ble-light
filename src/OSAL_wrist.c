#include "OSAL.h"
#include "OSAL_Tasks.h"


/* HCI */
#include "hci_tl.h"

/* LL */
#include "ll.h"


/* L2CAP */
#include "l2cap.h"

/* gap */
#include "gap.h"
#include "gapgattserver.h"
#include "gapbondmgr.h"

/* GATT */
#include "gatt.h"

#include "gattservapp.h"

/* Profiles */
#include "peripheral.h"

/* btn */
#include "btn.h"

/* Application */
#include "app_wrist.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */

// The order in this table must be identical to the task initialization calls below in osalInitTask.
const pTaskEventHandlerFn tasksArr[] =
{
  LL_ProcessEvent,
  HCI_ProcessEvent,
  L2CAP_ProcessEvent,
  SM_ProcessEvent,
  GAP_ProcessEvent,
  GATT_ProcessEvent,
  GAPRole_ProcessEvent,
  GATTServApp_ProcessEvent,
  Key_ProcessEvent,
  appWristProcEvt,
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16 *tasksEvents;

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalInitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void osalInitTasks( void )
{
  uint8 taskID = 0;

  tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
  osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));

  /* LL Task */
  LL_Init( taskID++ );

  /* HCI Task */
  HCI_Init( taskID++ );

  /* L2CAP Task */
  L2CAP_Init( taskID++ );

  /* SM Task */
  SM_Init( taskID++ );

  /* GAP Task */
  GAP_Init( taskID++ );

  /* GATT Task */
  GATT_Init( taskID++ );

  /* Profiles */
  GAPRole_Init( taskID++ );

  GATTServApp_Init( taskID++ );

  /** 初始化按钮 事件*/
  btn_init(taskID++);

  /* Application */
  appWristInit( taskID );
}

/*********************************************************************
*********************************************************************/
