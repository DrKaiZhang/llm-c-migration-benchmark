#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for hal/sdio_slave_periph.h */
#ifndef STUB_HAL_SDIO_SLAVE_PERIPH_H_1CCB491D
#define STUB_HAL_SDIO_SLAVE_PERIPH_H_1CCB491D
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_hal_sdio_slave_periph_h_1ccb491d;
#endif
/* stub for soc/sdio_slave_pins.h */
#ifndef STUB_SOC_SDIO_SLAVE_PINS_H_BDA56BAB
#define STUB_SOC_SDIO_SLAVE_PINS_H_BDA56BAB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_soc_sdio_slave_pins_h_bda56bab;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

const sdio_slave_slot_info_t sdio_slave_slot_info[1]  = {
    {
        .clk_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_CLK,
        .cmd_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_CMD,
        .d0_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D0,
        .d1_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D1,
        .d2_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D2,
        .d3_gpio = SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D3,
        .func = SDIO_SLAVE_SLOT0_FUNC,
    },
};
