#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for esp_attr.h */
#ifndef STUB_ESP_ATTR_H_48D79341
#define STUB_ESP_ATTR_H_48D79341
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_attr_h_48d79341;
#endif
/* stub for esp_intr_alloc.h */
#ifndef STUB_ESP_INTR_ALLOC_H_B8B14EE3
#define STUB_ESP_INTR_ALLOC_H_B8B14EE3
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_intr_alloc_h_b8b14ee3;
#endif
/* stub for esp_log.h */
#ifndef STUB_ESP_LOG_H_E673833F
#define STUB_ESP_LOG_H_E673833F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_log_h_e673833f;
#endif
/* stub for esp_private/cache_err_int.h */
#ifndef STUB_ESP_PRIVATE_CACHE_ERR_INT_H_5F82C579
#define STUB_ESP_PRIVATE_CACHE_ERR_INT_H_5F82C579
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_private_cache_err_int_h_5f82c579;
#endif
/* stub for esp_rom_sys.h */
#ifndef STUB_ESP_ROM_SYS_H_8C4057DF
#define STUB_ESP_ROM_SYS_H_8C4057DF
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_rom_sys_h_8c4057df;
#endif
/* stub for hal/cache_ll.h */
#ifndef STUB_HAL_CACHE_LL_H_87E5CADF
#define STUB_HAL_CACHE_LL_H_87E5CADF
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_hal_cache_ll_h_87e5cadf;
#endif
/* stub for riscv/interrupt.h */
#ifndef STUB_RISCV_INTERRUPT_H_4BBDBD15
#define STUB_RISCV_INTERRUPT_H_4BBDBD15
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_riscv_interrupt_h_4bbdbd15;
#endif
/* stub for soc/periph_defs.h */
#ifndef STUB_SOC_PERIPH_DEFS_H_06F2C51F
#define STUB_SOC_PERIPH_DEFS_H_06F2C51F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_soc_periph_defs_h_06f2c51f;
#endif
/* stub for soc/soc.h */
#ifndef STUB_SOC_SOC_H_8A24CA57
#define STUB_SOC_SOC_H_8A24CA57
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_soc_soc_h_8a24ca57;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 The cache has an interrupt that can be raised as soon as an access to a cached
 region (flash) is done without the cache being enabled. We use that here
 to panic the CPU, which from a debugging perspective is better than grabbing bad
 data from the bus.
*/

ESP_LOG_ATTR_TAG(TAG, "CACHE_ERR");

const char cache_error_msg[] = "Cache access error";

void esp_cache_err_get_panic_info(esp_cache_err_info_t *err_info)
{
    if (err_info == NULL) {
        return;
    }
    const uint32_t access_err_status = cache_ll_l1_get_access_error_intr_status(0, CACHE_LL_L1_ACCESS_EVENT_MASK);

    /* Return the error string if a cache error is active */
    err_info->err_str = access_err_status ? cache_error_msg : NULL;
}

bool esp_cache_err_has_active_err(void)
{
    return cache_ll_l1_get_access_error_intr_status(0, CACHE_LL_L1_ACCESS_EVENT_MASK);
}

void esp_cache_err_clear_active_err(void)
{
    cache_ll_l1_clear_access_error_intr(0, CACHE_LL_L1_ACCESS_EVENT_MASK);
}

void esp_cache_err_int_init(void)
{
    const uint32_t core_id = 0;

    /* Disable cache interrupts if enabled. */
    ESP_INTR_DISABLE(ETS_CACHEERR_INUM);

    /**
     *  Bind all cache errors to ETS_CACHEERR_INUM interrupt. we will deal with
     * them in handler by different types
     *
     * On ESP32C5 boards, the cache is a shared one but buses are still
     * distinct. So, we have an bus0 and a bus1 sharing the same cache.
     * This error can occur if a bus performs a request but the cache
     * is disabled.
     */
    esp_rom_route_intr_matrix(core_id, ETS_CACHE_INTR_SOURCE, ETS_CACHEERR_INUM);

    /* Set the type and priority to cache error interrupts. */
    esprv_int_set_type(ETS_CACHEERR_INUM, INTR_TYPE_LEVEL);
    esprv_int_set_priority(ETS_CACHEERR_INUM, SOC_INTERRUPT_LEVEL_MEDIUM);

    ESP_DRAM_LOGV(TAG, "access error intr clr & ena mask is: 0x%x", CACHE_LL_L1_ACCESS_EVENT_MASK);
    /**
     * Here we
     * 1. enable the cache fail tracer to take cache error interrupt into effect.
     * 2. clear potential cache error interrupt raw bits
     * 3. enable cache error interrupt en bits
     */
    cache_ll_l1_enable_fail_tracer(0, true);
    cache_ll_l1_clear_access_error_intr(0, CACHE_LL_L1_ACCESS_EVENT_MASK);
    cache_ll_l1_enable_access_error_intr(0, CACHE_LL_L1_ACCESS_EVENT_MASK);

    /* Enable the interrupts for cache error. */
    ESP_INTR_ENABLE(ETS_CACHEERR_INUM);
}

int esp_cache_err_get_cpuid(void)
{
    return 0;
}
