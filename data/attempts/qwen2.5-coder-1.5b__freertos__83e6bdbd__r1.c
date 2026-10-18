/* model=qwen2.5-coder-1.5b task=freertos__83e6bdbd rep=1 */
#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for 75x_eic.h */
#ifndef STUB_75X_EIC_H_D6D7EB5E
#define STUB_75X_EIC_H_D6D7EB5E
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_75x_eic_h_d6d7eb5e;
#endif
/* stub for 75x_tb.h */
#ifndef STUB_75X_TB_H_26A0939E
#define STUB_75X_TB_H_26A0939E
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_75x_tb_h_26a0939e;
#endif
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
 * FreeRTOS Kernel V10.3.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ST STR75x ARM7
 * port.
 *----------------------------------------------------------*/

/* Library includes. */

/* Scheduler includes. */

/* Constants required to setup the initial stack. */
#define portINITIAL_SPSR				( ( StackType_t ) 0x3f ) /* System mode, THUMB mode, interrupts enabled. */
#define portINSTRUCTION_SIZE			( ( StackType_t ) 4 )

/* Constants required to handle critical sections. */
#define portNO_CRITICAL_N
