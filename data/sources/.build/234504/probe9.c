#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for esp_heap_caps.h */
#ifndef STUB_ESP_HEAP_CAPS_H_44AB6F5A
#define STUB_ESP_HEAP_CAPS_H_44AB6F5A
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_heap_caps_h_44ab6f5a;
#endif
/* stub for esp_newlib.h */
#ifndef STUB_ESP_NEWLIB_H_8ADDCDC4
#define STUB_ESP_NEWLIB_H_8ADDCDC4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_newlib_h_8addcdc4;
#endif
/* stub for unity.h */
#ifndef STUB_UNITY_H_F941ADD4
#define STUB_UNITY_H_F941ADD4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_unity_h_f941add4;
#endif
/* stub for unity_test_runner.h */
#ifndef STUB_UNITY_TEST_RUNNER_H_669E14D1
#define STUB_UNITY_TEST_RUNNER_H_669E14D1
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_unity_test_runner_h_669e14d1;
#endif
/* stub for unity_test_utils.h */
#ifndef STUB_UNITY_TEST_UTILS_H_083C0F3B
#define STUB_UNITY_TEST_UTILS_H_083C0F3B
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_unity_test_utils_h_083c0f3b;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


// Some resources are lazy allocated in gpio/rtcio driver, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (100)

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    esp_reent_cleanup();    //clean up some of the newlib's lazy allocations
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    unity_run_menu();
}
