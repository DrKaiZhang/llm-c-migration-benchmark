#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for cc26x2_cc13x2_prcm.h */
#ifndef STUB_CC26X2_CC13X2_PRCM_H_F8AAC147
#define STUB_CC26X2_CC13X2_PRCM_H_F8AAC147
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_cc26x2_cc13x2_prcm_h_f8aac147;
#endif
/* stub for cpu.h */
#ifndef STUB_CPU_H_852FBE1A
#define STUB_CPU_H_852FBE1A
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_cpu_h_852fbe1a;
#endif
/* stub for kernel_init.h */
#ifndef STUB_KERNEL_INIT_H_00B3E488
#define STUB_KERNEL_INIT_H_00B3E488
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_kernel_init_h_00b3e488;
#endif
/* stub for periph/init.h */
#ifndef STUB_PERIPH_INIT_H_CD539DD5
#define STUB_PERIPH_INIT_H_CD539DD5
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_periph_init_h_cd539dd5;
#endif
/* stub for periph_conf.h */
#ifndef STUB_PERIPH_CONF_H_954257EB
#define STUB_PERIPH_CONF_H_954257EB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_periph_conf_h_954257eb;
#endif
/* stub for stdio_base.h */
#ifndef STUB_STDIO_BASE_H_B7AEBFE4
#define STUB_STDIO_BASE_H_B7AEBFE4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_stdio_base_h_b7aebfe4;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2016 Leon George
 * SPDX-FileCopyrightText: 2018 Anton Gerasimov
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief       implementation of the CPU initialization
 *
 * @author      Leon M. George <leon@georgemail.eu>, Anton Gerasimov <tossel@gmail.com>
 * @}
 */



/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    /* trim device */
    setup_trim_device();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}
