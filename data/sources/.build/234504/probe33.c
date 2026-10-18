#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for esp_efuse.h */
#ifndef STUB_ESP_EFUSE_H_04D83204
#define STUB_ESP_EFUSE_H_04D83204
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_efuse_h_04d83204;
#endif
/* stub for esp_efuse_table.h */
#ifndef STUB_ESP_EFUSE_TABLE_H_4F2B14DC
#define STUB_ESP_EFUSE_TABLE_H_4F2B14DC
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_efuse_table_h_4f2b14dc;
#endif
/* stub for esp_efuse_utility.h */
#ifndef STUB_ESP_EFUSE_UTILITY_H_953FD311
#define STUB_ESP_EFUSE_UTILITY_H_953FD311
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_efuse_utility_h_953fd311;
#endif
/* stub for esp_err.h */
#ifndef STUB_ESP_ERR_H_33ACA4FB
#define STUB_ESP_ERR_H_33ACA4FB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_err_h_33aca4fb;
#endif
/* stub for esp_log.h */
#ifndef STUB_ESP_LOG_H_E673833F
#define STUB_ESP_LOG_H_E673833F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_log_h_e673833f;
#endif
/* stub for esp_types.h */
#ifndef STUB_ESP_TYPES_H_044F07EB
#define STUB_ESP_TYPES_H_044F07EB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_types_h_044f07eb;
#endif
/* stub for soc/efuse_periph.h */
#ifndef STUB_SOC_EFUSE_PERIPH_H_F3B4CB15
#define STUB_SOC_EFUSE_PERIPH_H_F3B4CB15
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_soc_efuse_periph_h_f3b4cb15;
#endif
/* stub for sys/param.h */
#ifndef STUB_SYS_PARAM_H_F6E092E7
#define STUB_SYS_PARAM_H_F6E092E7
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_sys_param_h_f6e092e7;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdlib.h"
#include "assert.h"

ESP_LOG_ATTR_TAG(TAG, "efuse");

// Contains functions that provide access to efuse fields which are often used in IDF.

// Returns chip package from efuse
uint32_t esp_efuse_get_pkg_ver(void)
{
    uint32_t pkg_ver = 0;
#ifdef EFUSE_PKG_VERSION
    esp_efuse_read_field_blob(ESP_EFUSE_PKG_VERSION, &pkg_ver, ESP_EFUSE_PKG_VERSION[0]->bit_count);
#endif
    return pkg_ver;
}


esp_err_t esp_efuse_set_rom_log_scheme(esp_efuse_rom_log_scheme_t log_scheme)
{
    int cur_log_scheme = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_UART_PRINT_CONTROL, &cur_log_scheme, 2);
    if (!cur_log_scheme) { // not burned yet
        return esp_efuse_write_field_blob(ESP_EFUSE_UART_PRINT_CONTROL, &log_scheme, 2);
    } else {
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t esp_efuse_disable_rom_download_mode(void)
{
    return esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MODE);
}

esp_err_t esp_efuse_enable_rom_secure_download_mode(void)
{
    if (!esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MODE)) {
        return esp_efuse_write_field_bit(ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD);
    }
    return ESP_OK;
}
