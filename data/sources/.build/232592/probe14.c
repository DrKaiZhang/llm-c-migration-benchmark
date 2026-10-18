#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for FreeRTOS.h */
#ifndef STUB_FREERTOS_H_CFCB1879
#define STUB_FREERTOS_H_CFCB1879
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_freertos_h_cfcb1879;
#endif
/* stub for task.h */
#ifndef STUB_TASK_H_3F31FF61
#define STUB_TASK_H_3F31FF61
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_task_h_3f31ff61;
#endif
#endif

/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * license and copyright intentionally withheld to promote copying into user code.
 */


BaseType_t xPortStartScheduler( void )
{
    return pdTRUE;
}

void vPortEndScheduler( void )
{
}

StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
{
    ( void ) pxTopOfStack;
    ( void ) pvParameters;
    ( void ) * pxCode;

    return NULL;
}

void vPortYield( void )
{
    /* Save the current Context */

    /* Switch to the highest priority task that is ready to run. */
    #if ( configNUMBER_OF_CORES == 1 )
    {
        vTaskSwitchContext();
    }
    #else
    {
        vTaskSwitchContext( portGET_CORE_ID() );
    }
    #endif

    /* Start executing the task we have just switched to. */
}

static void prvTickISR( void )
{
    /* Interrupts must have been enabled for the ISR to fire, so we have to
     * save the context with interrupts enabled. */

    #if ( configNUMBER_OF_CORES == 1 )
    {
        /* Maintain the tick count. */
        if( xTaskIncrementTick() != pdFALSE )
        {
            /* Switch to the highest priority task that is ready to run. */
            vTaskSwitchContext();
        }
    }
    #else
    {
        UBaseType_t ulPreviousMask;

        /* Tasks or ISRs running on other cores may still in critical section in
         * multiple cores environment. Incrementing tick needs to performed in
         * critical section. */
        ulPreviousMask = taskENTER_CRITICAL_FROM_ISR();

        /* Maintain the tick count. */
        if( xTaskIncrementTick() != pdFALSE )
        {
            /* Switch to the highest priority task that is ready to run. */
            vTaskSwitchContext( portGET_CORE_ID() );
        }

        taskEXIT_CRITICAL_FROM_ISR( ulPreviousMask );
    }
    #endif /* if ( configNUMBER_OF_CORES == 1 ) */

    /* start executing the new task */
}
