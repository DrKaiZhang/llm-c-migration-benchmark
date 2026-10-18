#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for bootloader_random.h */
#ifndef STUB_BOOTLOADER_RANDOM_H_002B7CA4
#define STUB_BOOTLOADER_RANDOM_H_002B7CA4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_bootloader_random_h_002b7ca4;
#endif
/* stub for hal/rng_ll.h */
#ifndef STUB_HAL_RNG_LL_H_E7B89B06
#define STUB_HAL_RNG_LL_H_E7B89B06
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_hal_rng_ll_h_e7b89b06;
#endif
/* stub for sdkconfig.h */
#ifndef STUB_SDKCONFIG_H_87D33A20
#define STUB_SDKCONFIG_H_87D33A20
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_sdkconfig_h_87d33a20;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

void bootloader_random_enable(void)
{
    rng_ll_enable();
}

void bootloader_random_disable(void)
{
    rng_ll_disable();
}
