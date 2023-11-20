#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "OSAL_PwrMgr.h"
#include "osal_snv.h"

#include "ll_sleep.h"


#include "uart.h"

int app_main(void)
{

    /* Initialize the operating system */
    osal_init_system();

    osal_pwrmgr_device( PWRMGR_BATTERY );

    /* Start OSAL */
    osal_start_system(); // No Return from here
    return 0;
}

