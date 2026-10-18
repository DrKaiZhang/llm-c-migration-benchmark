#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for esp_check.h */
#ifndef STUB_ESP_CHECK_H_A6A879A9
#define STUB_ESP_CHECK_H_A6A879A9
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_check_h_a6a879a9;
#endif
/* stub for esp_macros.h */
#ifndef STUB_ESP_MACROS_H_A1CF3D39
#define STUB_ESP_MACROS_H_A1CF3D39
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_macros_h_a1cf3d39;
#endif
/* stub for esp_pmu.h */
#ifndef STUB_ESP_PMU_H_CE2E78F1
#define STUB_ESP_PMU_H_CE2E78F1
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_pmu_h_ce2e78f1;
#endif
/* stub for esp_private/esp_regdma.h */
#ifndef STUB_ESP_PRIVATE_ESP_REGDMA_H_BDD04471
#define STUB_ESP_PRIVATE_ESP_REGDMA_H_BDD04471
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_private_esp_regdma_h_bdd04471;
#endif
/* stub for esp_private/sleep_retention.h */
#ifndef STUB_ESP_PRIVATE_SLEEP_RETENTION_H_5A8F2F40
#define STUB_ESP_PRIVATE_SLEEP_RETENTION_H_5A8F2F40
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_private_sleep_retention_h_5a8f2f40;
#endif
/* stub for esp_private/startup_internal.h */
#ifndef STUB_ESP_PRIVATE_STARTUP_INTERNAL_H_ECA614B8
#define STUB_ESP_PRIVATE_STARTUP_INTERNAL_H_ECA614B8
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_private_startup_internal_h_eca614b8;
#endif
/* stub for pmu_param.h */
#ifndef STUB_PMU_PARAM_H_19A350A2
#define STUB_PMU_PARAM_H_19A350A2
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_pmu_param_h_19a350a2;
#endif
/* stub for soc/pmu_reg.h */
#ifndef STUB_SOC_PMU_REG_H_0B9A6DAD
#define STUB_SOC_PMU_REG_H_0B9A6DAD
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_soc_pmu_reg_h_0b9a6dad;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */





ESP_LOG_ATTR_TAG(TAG, "sleep_power");

#if SOC_PM_MODEM_LOCK_CLK_WORKAROUND

#define XTALX2_TIE_HIGH_MASK  (PMU_TIE_HIGH_XTALX2_M | PMU_TIE_HIGH_GLOBAL_XTALX2_ICG_M)
#define XTALX2_TIE_HIGH_ON    (PMU_TIE_HIGH_XTALX2 | PMU_TIE_HIGH_GLOBAL_XTALX2_ICG)
#define BBPLL_TIE_HIGH_MASK   (PMU_TIE_HIGH_XPD_BBPLL_M | PMU_TIE_HIGH_XPD_BBPLL_I2C_M | PMU_TIE_HIGH_GLOBAL_BBPLL_ICG_M)
#define BBPLL_TIE_HIGH_ON     (PMU_TIE_HIGH_XPD_BBPLL | PMU_TIE_HIGH_XPD_BBPLL_I2C | PMU_TIE_HIGH_GLOBAL_BBPLL_ICG)

typedef struct {
    void *regdma_desc[2];
} pmu_sleep_power_clock_context_t;

static esp_err_t sleep_power_system_retention_init(void *arg)
{
    const static sleep_retention_entries_config_t power_regs_retention[] = {
        /* During the modem-to-active transition, the MODEM lock keeps the xtalx2, bbpll xpd status unchanged.
         * Therefore, if xtalx2, bbpll is disabled in modem state, we need to set xtalx2, bbpll tie-high to enable it in active state.
         *
         * XTALX2 and BBPLL write values are back-filled before sleep by pmu_sleep_power_clock_config(). */
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(0),  PMU_IMM_HP_CK_POWER_REG,  XTALX2_TIE_HIGH_ON,  XTALX2_TIE_HIGH_MASK,  1,  0),  .owner = ENTRY(2) },
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(1),  PMU_IMM_HP_CK_POWER_REG,  BBPLL_TIE_HIGH_ON,   BBPLL_TIE_HIGH_MASK,   1,  0),  .owner = ENTRY(2) },
    };
    esp_err_t err = sleep_retention_entries_create(power_regs_retention, ARRAY_SIZE(power_regs_retention), REGDMA_LINK_PRI_POWER, SLEEP_RETENTION_MODULE_POWER);

    pmu_sleep_power_clock_context_t *clk = (pmu_sleep_power_clock_context_t *)arg;
    int id_array[ARRAY_SIZE(clk->regdma_desc)] = { REGDMA_POWER_LINK(0), REGDMA_POWER_LINK(1) };
    for (int i = 0; i < ARRAY_SIZE(id_array); i++) {
        void *head = sleep_retention_find_link_by_id(id_array[i]);
        if (head) {
            clk->regdma_desc[i] = head;
        }
    }

    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for clock power retention");
    return ESP_OK;
}

#endif

#define SLEEP_POWER_ANA_WAIT_LINK_BASE          10
#define SLEEP_POWER_ANA_WAIT_LINK_ID(_idx)      REGDMA_POWER_LINK((SLEEP_POWER_ANA_WAIT_LINK_BASE + (_idx)))

typedef struct {
    void *regdma_desc[ANALOG_WAIT_CTRL_NUM];
} pmu_sleep_power_ana_wait_context_t;

static esp_err_t sleep_power_analog_wait_ctrl_init(void *arg)
{
    const static sleep_retention_entries_config_t analog_wait_ctrl_retention[] = {
        /* Sleep to Modem retention has passed hp analog wait duration, so Modem to Sleep retention can reduce this time.
         * So here Sleep to Modem retention set the analog wait duration to about 1.6us, and Modem to Sleep retention set the analog wait duration to origin value. */
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(10),   PMU_SLP_WAKEUP_CNTL7_REG,   (0x20 << PMU_ANA_WAIT_TARGET_S),   PMU_ANA_WAIT_TARGET_M,  1,  0),  .owner = ENTRY(1) },
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(11),   PMU_SLP_WAKEUP_CNTL7_REG,   (PMU_HP_ANALOG_WAIT_TARGET_CYCLES << PMU_ANA_WAIT_TARGET_S),  PMU_ANA_WAIT_TARGET_M,  0,  1),  .owner = ENTRY(1) },
        [2] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(12),   PMU_SLP_WAKEUP_CNTL7_REG,   (PMU_HP_ANALOG_WAIT_TARGET_CYCLES << PMU_ANA_WAIT_TARGET_S),  PMU_ANA_WAIT_TARGET_M,  1,  0),  .owner = ENTRY(2) },
    };
    esp_err_t err = sleep_retention_entries_create(analog_wait_ctrl_retention, ARRAY_SIZE(analog_wait_ctrl_retention), REGDMA_LINK_PRI_POWER, SLEEP_RETENTION_MODULE_POWER);

    pmu_sleep_power_ana_wait_context_t *ana_wait_context = (pmu_sleep_power_ana_wait_context_t *)arg;
    for (int i = 0; i < ANALOG_WAIT_CTRL_NUM; i++) {
        if (!ana_wait_context->regdma_desc[i]) {
            void *head = sleep_retention_find_link_by_id(SLEEP_POWER_ANA_WAIT_LINK_ID(i));
            if (head) {
                ana_wait_context->regdma_desc[i] = head;
            }
        }
    }
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for analog wait ctrl retention");
    return ESP_OK;
}

typedef struct {
#if SOC_PM_MODEM_LOCK_CLK_WORKAROUND
    pmu_sleep_power_clock_context_t clock;
#endif
    pmu_sleep_power_ana_wait_context_t ana_wait;
} pmu_sleep_power_context_t;

static esp_err_t sleep_power_retention_init(void *arg)
{
    pmu_sleep_power_context_t *ctx = (pmu_sleep_power_context_t *)arg;
#if SOC_PM_MODEM_LOCK_CLK_WORKAROUND
    ESP_RETURN_ON_ERROR(sleep_power_system_retention_init(&ctx->clock), TAG, "system retention init failed");
#endif
    ESP_RETURN_ON_ERROR(sleep_power_analog_wait_ctrl_init(&ctx->ana_wait), TAG, "analog wait ctrl retention init failed");
    return ESP_OK;
}

static esp_err_t sleep_power_retention_deinit(void *arg)
{
    pmu_sleep_power_context_t *ctx = (pmu_sleep_power_context_t *)arg;
#if SOC_PM_MODEM_LOCK_CLK_WORKAROUND
    for (int i = 0; i < ARRAY_SIZE(ctx->clock.regdma_desc); i++) {
        ctx->clock.regdma_desc[i] = NULL;
    }
#endif
    for (int i = 0; i < ANALOG_WAIT_CTRL_NUM; i++) {
        ctx->ana_wait.regdma_desc[i] = NULL;
    }
    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(sleep_power_startup_init, SECONDARY, BIT(0), 108)
{
    static DRAM_ATTR pmu_sleep_power_context_t power_context = { 0 };

    sleep_retention_module_init_param_t init_param = {
        .cbs = {
            .create  = { .handle = sleep_power_retention_init,   .arg = &power_context },
            .destroy = { .handle = sleep_power_retention_deinit, .arg = &power_context },
        },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE | SLEEP_RETENTION_MODULE_ATTR_ATTACH
    };

    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_POWER, &init_param);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to init power retention module, err=%d", err);
    } else {
#if PMU_SLEEP_PRIV_ENABLED
        pmu_sleep_data_t *data = (pmu_sleep_data_t *)PMU_instance()->priv;
        data->func[PMU_SLEEP_PRIV_SKIP_MODEM_TO_ACTIVE_ANALOG_WAIT] = &power_context.ana_wait;
#if SOC_PM_MODEM_LOCK_CLK_WORKAROUND
        data->func[PMU_SLEEP_PRIV_MODEM_LOCK_CLK_POWER] = &power_context.clock;
#endif
#endif
    }

    return ESP_OK;
}

void pmu_sleep_power_analog_wait_config(void *data, const uint16_t analog_wait[ANALOG_WAIT_CTRL_NUM])
{
    pmu_sleep_data_t *data_ctx = (pmu_sleep_data_t *)data;
    if (!data_ctx || !data_ctx->func[PMU_SLEEP_PRIV_SKIP_MODEM_TO_ACTIVE_ANALOG_WAIT]) {
        return;
    }

    pmu_sleep_power_ana_wait_context_t *ana_wait_ctx = (pmu_sleep_power_ana_wait_context_t *)data_ctx->func[PMU_SLEEP_PRIV_SKIP_MODEM_TO_ACTIVE_ANALOG_WAIT];
    for (int i = 0; i < ANALOG_WAIT_CTRL_NUM; i++) {
        if (!ana_wait_ctx->regdma_desc[i]) {
            continue;
        }
        regdma_link_set_write_wait_content(ana_wait_ctx->regdma_desc[i], (uint32_t)analog_wait[i] << PMU_ANA_WAIT_TARGET_S, PMU_ANA_WAIT_TARGET_M);
    }
}

#if SOC_PM_MODEM_LOCK_CLK_WORKAROUND
void pmu_sleep_power_clock_config(void *data, const uint32_t config)
{
    pmu_sleep_data_t *datap = (pmu_sleep_data_t *)data;
    if (!datap || !datap->func[PMU_SLEEP_PRIV_MODEM_LOCK_CLK_POWER]) {
        return;
    }

    pmu_sleep_power_clock_context_t *clk = (pmu_sleep_power_clock_context_t *)datap->func[PMU_SLEEP_PRIV_MODEM_LOCK_CLK_POWER];
    if (clk->regdma_desc[0]) {
        regdma_link_set_write_wait_content(clk->regdma_desc[0], config & XTALX2_TIE_HIGH_MASK, XTALX2_TIE_HIGH_MASK);
    }
    if (clk->regdma_desc[1]) {
        regdma_link_set_write_wait_content(clk->regdma_desc[1], config & BBPLL_TIE_HIGH_MASK,  BBPLL_TIE_HIGH_MASK);
    }
}
#endif
