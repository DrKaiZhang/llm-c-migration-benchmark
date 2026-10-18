#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for esp_attr.h */
#ifndef STUB_ESP_ATTR_H_48D79341
#define STUB_ESP_ATTR_H_48D79341
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_attr_h_48d79341;
#endif
/* stub for esp_log.h */
#ifndef STUB_ESP_LOG_H_E673833F
#define STUB_ESP_LOG_H_E673833F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_log_h_e673833f;
#endif
/* stub for esp_private/critical_section.h */
#ifndef STUB_ESP_PRIVATE_CRITICAL_SECTION_H_44F41F83
#define STUB_ESP_PRIVATE_CRITICAL_SECTION_H_44F41F83
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_private_critical_section_h_44f41f83;
#endif
/* stub for esp_private/esp_modem_clock.h */
#ifndef STUB_ESP_PRIVATE_ESP_MODEM_CLOCK_H_C68582EB
#define STUB_ESP_PRIVATE_ESP_MODEM_CLOCK_H_C68582EB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_private_esp_modem_clock_h_c68582eb;
#endif
/* stub for esp_private/periph_ctrl.h */
#ifndef STUB_ESP_PRIVATE_PERIPH_CTRL_H_DD9A2AE7
#define STUB_ESP_PRIVATE_PERIPH_CTRL_H_DD9A2AE7
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_private_periph_ctrl_h_dd9a2ae7;
#endif
/* stub for freertos/FreeRTOS.h */
#ifndef STUB_FREERTOS_FREERTOS_H_AC994C92
#define STUB_FREERTOS_FREERTOS_H_AC994C92
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_freertos_freertos_h_ac994c92;
#endif
/* stub for hal/clk_gate_ll.h */
#ifndef STUB_HAL_CLK_GATE_LL_H_FCF31343
#define STUB_HAL_CLK_GATE_LL_H_FCF31343
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_hal_clk_gate_ll_h_fcf31343;
#endif
/* stub for soc/soc_caps.h */
#ifndef STUB_SOC_SOC_CAPS_H_20B28EC4
#define STUB_SOC_SOC_CAPS_H_20B28EC4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_soc_soc_caps_h_20b28ec4;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef __PERIPH_CTRL_ALLOW_LEGACY_API
#endif

#if SOC_MODEM_CLOCK_IS_INDEPENDENT && SOC_MODEM_CLOCK_SUPPORTED
#endif

/// @brief For simplicity and backward compatible, we are using the same spin lock for both bus clock on/off and reset
/// @note  We may want to split them into two spin locks in the future
static portMUX_TYPE __attribute__((unused)) periph_spinlock = portMUX_INITIALIZER_UNLOCKED;

static uint8_t ref_counts[PERIPH_MODULE_MAX] = {0};

void periph_rcc_enter(void)
{
    esp_os_enter_critical_safe(&periph_spinlock);
}

void periph_rcc_exit(void)
{
    esp_os_exit_critical_safe(&periph_spinlock);
}

uint8_t periph_rcc_acquire_enter(shared_periph_module_t periph)
{
    periph_rcc_enter();
    return ref_counts[periph];
}

void periph_rcc_acquire_exit(shared_periph_module_t periph, uint8_t ref_count)
{
    ref_counts[periph] = ++ref_count;
    periph_rcc_exit();
}

uint8_t periph_rcc_release_enter(shared_periph_module_t periph)
{
    periph_rcc_enter();
    return ref_counts[periph] - 1;
}

void periph_rcc_release_exit(shared_periph_module_t periph, uint8_t ref_count)
{
    ref_counts[periph] = ref_count;
    periph_rcc_exit();
}

void periph_module_enable(shared_periph_module_t periph)
{
#ifdef __PERIPH_CTRL_ALLOW_LEGACY_API
    assert(periph < PERIPH_MODULE_MAX);
    esp_os_enter_critical_safe(&periph_spinlock);
    if (ref_counts[periph] == 0) {
        periph_ll_enable_clk_clear_rst(periph);
    }
    ref_counts[periph]++;
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}

void periph_module_disable(shared_periph_module_t periph)
{
#ifdef __PERIPH_CTRL_ALLOW_LEGACY_API
    assert(periph < PERIPH_MODULE_MAX);
    esp_os_enter_critical_safe(&periph_spinlock);
    ref_counts[periph]--;
    if (ref_counts[periph] == 0) {
        periph_ll_disable_clk_set_rst(periph);
    }
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}

void periph_module_reset(shared_periph_module_t periph)
{
#ifdef __PERIPH_CTRL_ALLOW_LEGACY_API
    assert(periph < PERIPH_MODULE_MAX);
    esp_os_enter_critical_safe(&periph_spinlock);
    periph_ll_reset(periph);
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}

#if !SOC_IEEE802154_BLE_ONLY
#if SOC_BT_SUPPORTED || SOC_WIFI_SUPPORTED || SOC_IEEE802154_SUPPORTED
IRAM_ATTR void wifi_bt_common_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_PHY_MODULE);
#else
    esp_os_enter_critical_safe(&periph_spinlock);
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_enable_clk();
    }
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]++;
#if CONFIG_IDF_TARGET_ESP32C2
    if (ref_counts[PERIPH_COEX_MODULE] == 0) {
        periph_ll_coex_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_COEX_MODULE]++;
#endif
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}

IRAM_ATTR void wifi_bt_common_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_PHY_MODULE);
#else
    esp_os_enter_critical_safe(&periph_spinlock);
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]--;
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_disable_clk();
    }
#if CONFIG_IDF_TARGET_ESP32C2
    ref_counts[PERIPH_COEX_MODULE]--;
    if (ref_counts[PERIPH_COEX_MODULE] == 0) {
        periph_ll_coex_module_disable_clk_set_rst();
    }
#endif
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}
#endif  //#if SOC_BT_SUPPORTED || SOC_WIFI_SUPPORTED
#endif  //#if !SOC_IEEE802154_BLE_ONLY

#if CONFIG_ESP_WIFI_ENABLED
void wifi_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_WIFI_MODULE);
#else
    esp_os_enter_critical_safe(&periph_spinlock);
    if (ref_counts[PERIPH_WIFI_MODULE] == 0) {
        periph_ll_wifi_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_WIFI_MODULE]++;
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}

void wifi_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_WIFI_MODULE);
#else
    esp_os_enter_critical_safe(&periph_spinlock);
    ref_counts[PERIPH_WIFI_MODULE]--;
    if (ref_counts[PERIPH_WIFI_MODULE] == 0) {
        periph_ll_wifi_module_disable_clk_set_rst();
    }
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}
#endif // CONFIG_ESP_WIFI_ENABLED

#if SOC_BT_SUPPORTED || SOC_WIFI_SUPPORTED || SOC_IEEE802154_SUPPORTED
// PERIPH_WIFI_BT_COMMON_MODULE is enabled outside
IRAM_ATTR void phy_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_PHY_CALIBRATION_MODULE);
#else
    esp_os_enter_critical_safe(&periph_spinlock);
#if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED
    periph_ll_phy_calibration_module_enable_clk_clear_rst();
    if (ref_counts[PERIPH_RNG_MODULE] == 0) {
        periph_ll_enable_clk_clear_rst(PERIPH_RNG_MODULE);
    }
    ref_counts[PERIPH_RNG_MODULE]++;
#endif
#if SOC_WIFI_SUPPORTED
    if (ref_counts[PERIPH_WIFI_MODULE] == 0) {
        periph_ll_wifi_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_WIFI_MODULE]++;
#endif
#if SOC_BT_SUPPORTED
    if (ref_counts[PERIPH_BT_MODULE] == 0) {
        periph_ll_enable_clk_clear_rst(PERIPH_BT_MODULE);
    }
    ref_counts[PERIPH_BT_MODULE]++;
#endif
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}

// PERIPH_WIFI_BT_COMMON_MODULE is disabled outside
IRAM_ATTR void phy_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_PHY_CALIBRATION_MODULE);
#else
    esp_os_enter_critical_safe(&periph_spinlock);
#if SOC_BT_SUPPORTED
    ref_counts[PERIPH_BT_MODULE]--;
    if (ref_counts[PERIPH_BT_MODULE] == 0) {
        periph_ll_disable_clk_set_rst(PERIPH_BT_MODULE);
    }
#endif
#if SOC_WIFI_SUPPORTED
    ref_counts[PERIPH_WIFI_MODULE]--;
    if (ref_counts[PERIPH_WIFI_MODULE] == 0) {
        periph_ll_wifi_module_disable_clk_set_rst();
    }
#endif
#if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED
    // Do not disable PHY clock and RNG clock
    ref_counts[PERIPH_RNG_MODULE]--;
#endif
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}

IRAM_ATTR bool phy_module_has_clock_bits(uint32_t mask)
{
    uint32_t val = 0;
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    val = modem_clock_module_bits_get(PERIPH_PHY_MODULE);
#else
#if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED
    val = DPORT_REG_READ(periph_ll_get_clk_en_reg(PERIPH_WIFI_BT_COMMON_MODULE));
#else
    return true;
#endif
#endif
    if ((val & mask) != mask) {
        ESP_LOGW("periph_ctrl", "phy module clock bits 0x%x, required 0x%x", val, mask);
        return false;
    }
    return true;
}

IRAM_ATTR void coex_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_COEX_MODULE);
#else
    esp_os_enter_critical_safe(&periph_spinlock);
#if CONFIG_IDF_TARGET_ESP32C2
    if (ref_counts[PERIPH_COEX_MODULE] == 0) {
        periph_ll_coex_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_COEX_MODULE]++;
#endif
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}

IRAM_ATTR void coex_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_COEX_MODULE);
#else
    esp_os_enter_critical_safe(&periph_spinlock);
#if CONFIG_IDF_TARGET_ESP32C2
    ref_counts[PERIPH_COEX_MODULE]--;
    if (ref_counts[PERIPH_COEX_MODULE] == 0) {
        periph_ll_coex_module_disable_clk_set_rst();
    }
#endif
    esp_os_exit_critical_safe(&periph_spinlock);
#endif
}
#endif  //#if SOC_BT_SUPPORTED || SOC_WIFI_SUPPORTED || SOC_IEEE802154_SUPPORTED
