#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for esp_rom_sys.h */
#ifndef STUB_ESP_ROM_SYS_H_8C4057DF
#define STUB_ESP_ROM_SYS_H_8C4057DF
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_rom_sys_h_8c4057df;
#endif
/* stub for sdkconfig.h */
#ifndef STUB_SDKCONFIG_H_87D33A20
#define STUB_SDKCONFIG_H_87D33A20
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_sdkconfig_h_87d33a20;
#endif
/* stub for unity.h */
#ifndef STUB_UNITY_H_F941ADD4
#define STUB_UNITY_H_F941ADD4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_unity_h_f941add4;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

static int my_printf(const char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    int len = esp_rom_vprintf(fmt, list);
    va_end(list);
    return len;
}

TEST_CASE("Test esp_rom_vprintf", "[rom][vprintf]")
{
    int len = esp_rom_printf("esp_rom_printf function works %d %d %d\n", 1, 2, 3);
    TEST_ASSERT_EQUAL_INT(36, len);
    len = my_printf("esp_rom_vprintf function works %d %d %d\n", 1, 2, 3);
    TEST_ASSERT_EQUAL_INT(37, len);
}
