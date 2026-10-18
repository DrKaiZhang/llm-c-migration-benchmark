#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for ../../../lib/include/audio.h */
#ifndef STUB_LIB_INCLUDE_AUDIO_H_EF8D17ED
#define STUB_LIB_INCLUDE_AUDIO_H_EF8D17ED
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_lib_include_audio_h_ef8d17ed;
#endif
/* stub for bluedroid/init.h */
#ifndef STUB_BLUEDROID_INIT_H_E7BEBC18
#define STUB_BLUEDROID_INIT_H_E7BEBC18
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_bluedroid_init_h_e7bebc18;
#endif
/* stub for bta/bta_gatt_common.h */
#ifndef STUB_BTA_BTA_GATT_COMMON_H_DAA41A4F
#define STUB_BTA_BTA_GATT_COMMON_H_DAA41A4F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_bta_bta_gatt_common_h_daa41a4f;
#endif
/* stub for common/init.h */
#ifndef STUB_COMMON_INIT_H_5580B97A
#define STUB_COMMON_INIT_H_5580B97A
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_common_init_h_5580b97a;
#endif
/* stub for sdkconfig.h */
#ifndef STUB_SDKCONFIG_H_87D33A20
#define STUB_SDKCONFIG_H_87D33A20
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_sdkconfig_h_87d33a20;
#endif
/* stub for zephyr/bluetooth/audio/micp.h */
#ifndef STUB_ZEPHYR_BLUETOOTH_AUDIO_MICP_H_B00FBC6C
#define STUB_ZEPHYR_BLUETOOTH_AUDIO_MICP_H_B00FBC6C
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_zephyr_bluetooth_audio_micp_h_b00fbc6c;
#endif
/* stub for zephyr/bluetooth/audio/vcp.h */
#ifndef STUB_ZEPHYR_BLUETOOTH_AUDIO_VCP_H_268993DB
#define STUB_ZEPHYR_BLUETOOTH_AUDIO_VCP_H_268993DB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_zephyr_bluetooth_audio_vcp_h_268993db;
#endif
/* stub for zephyr/logging/log.h */
#ifndef STUB_ZEPHYR_LOGGING_LOG_H_616B7D2B
#define STUB_ZEPHYR_LOGGING_LOG_H_616B7D2B
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_zephyr_logging_log_h_616b7d2b;
#endif
/* stub for zephyr/sys/util.h */
#ifndef STUB_ZEPHYR_SYS_UTIL_H_EE0CA1FA
#define STUB_ZEPHYR_SYS_UTIL_H_EE0CA1FA
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_zephyr_sys_util_h_ee0ca1fa;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <errno.h>
#include <assert.h>






LOG_MODULE_REGISTER(LEA_BINIT, CONFIG_BT_ISO_LOG_LEVEL);

#if CONFIG_BT_CSIP_SET_MEMBER
#define CSIS_SVC_COUNT      CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT
#else /* CONFIG_BT_CSIP_SET_MEMBER */
#define CSIS_SVC_COUNT      0
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_TBS
#define TBS_BEARER_SVC_COUNT    CONFIG_BT_TBS_BEARER_COUNT
#else /* CONFIG_BT_TBS */
#define TBS_BEARER_SVC_COUNT    0
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_MCS
#define MCS_INSTANCE_SVC_COUNT  CONFIG_BT_MCS_INSTANCE_COUNT
#else /* CONFIG_BT_MCS */
#define MCS_INSTANCE_SVC_COUNT  0
#endif /* CONFIG_BT_MCS */

/* 3 is reserved for other GATT services */
#define TOTAL_SERVICE_COUNT     (3 + \
                                 (IS_ENABLED(CONFIG_BT_ASCS) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_PACS) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_BAP_SCAN_DELEGATOR) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_TMAP) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_GMAP) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_MCS) ? 1 : 0) + \
                                 MCS_INSTANCE_SVC_COUNT + \
                                 (IS_ENABLED(CONFIG_BT_CSIP_SET_MEMBER) ? CSIS_SVC_COUNT : 0) + \
                                 (IS_ENABLED(CONFIG_BT_CAP_ACCEPTOR) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_VCP_VOL_REND) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_MICP_MIC_DEV) ? 1 : 0) + \
                                 CONFIG_BT_VOCS_MAX_INSTANCE_COUNT + \
                                 CONFIG_BT_AICS_MAX_INSTANCE_COUNT + \
                                 (IS_ENABLED(CONFIG_BT_TBS) ? 1 : 0) + \
                                 TBS_BEARER_SVC_COUNT + \
                                 (IS_ENABLED(CONFIG_BT_HAS) ? 1 : 0))

_Static_assert(TOTAL_SERVICE_COUNT <= CONFIG_BT_GATT_MAX_SR_PROFILES, "Too small BT_GATT_MAX_SR_PROFILES");

int bt_le_bluedroid_audio_init(void)
{
    int err = 0;

    BTA_GATT_SetLocalMTU(BLE_AUDIO_ATT_MTU_MIN);

    bt_le_bluedroid_audio_gatts_init();

#if CONFIG_BT_CSIP_SET_MEMBER
    bt_le_bluedroid_csis_state_reset();
#endif /* CONFIG_BT_CSIP_SET_MEMBER */
#if CONFIG_BT_VCP_VOL_REND
    bt_le_bluedroid_vcs_state_reset();
#endif /* CONFIG_BT_VCP_VOL_REND */
#if CONFIG_BT_MICP_MIC_DEV
    bt_le_bluedroid_mics_state_reset();
#endif /* CONFIG_BT_MICP_MIC_DEV */
#if CONFIG_BT_MCS
    bt_le_bluedroid_mcs_state_reset();
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_PACS
    err = bt_le_bluedroid_pacs_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_PACS */

#if (BLE_AUDIO_SVC_DEFERRED_ADD == 0)
#if CONFIG_BT_ASCS
    err = bt_le_bluedroid_ascs_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_ASCS */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
    err = bt_le_bluedroid_bass_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_TMAP
    err = bt_le_bluedroid_tmas_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TMAP */

#if CONFIG_BT_TBS
    err = bt_le_bluedroid_gtbs_init();
    if (err) {
        return err;
    }

    err = bt_le_bluedroid_tbs_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_HAS
    err = bt_le_bluedroid_has_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_HAS */
#endif /* (BLE_AUDIO_SVC_DEFERRED_ADD == 0) */

    return err;
}

void bt_le_bluedroid_audio_deinit(void)
{
    LOG_DBG("[B]AudioDeinit");

    /* The services are removed by the BTA_GATTS_AppDeregister() in the ISO
     * layer's gatt deinit - GATT_Deregister() stops every service the app
     * owns - so only this layer's own hook is dropped here. */
    bt_le_bluedroid_audio_gatts_deinit();
}

#if CONFIG_BT_CSIP_SET_MEMBER
static int bluedroid_gatt_csis_init(struct bt_le_audio_start_info *info,
                                    struct bt_gatt_service **inc_csis_svc)
{
    struct bt_gatt_service *csis_svc[CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT];
    uint8_t count;
    int err;

    if (info == NULL) {
        return 0;
    }

    *inc_csis_svc = NULL;
    count = 0;

    for (size_t i = 0; i < ARRAY_SIZE(info->csis_insts); i++) {
        if (info->csis_insts[i].svc_inst == NULL) {
            continue;
        }

        csis_svc[count] = lib_csip_set_member_svc_get(info->csis_insts[i].svc_inst);
        if (!csis_svc[count]) {
            LOG_ERR("[B]CsisSvcGetFail[%u]", i);
            return -ENODEV;
        }

        if (info->csis_insts[i].included_by_cas) {
            if (*inc_csis_svc == NULL) {
                *inc_csis_svc = csis_svc[count];
            } else {
                /* CAS may include at most one CSIS — caller misconfigured. */
                LOG_ERR("[B]CsisMultiIncByCas");
                return -EINVAL;
            }
        }

        count++;
    }

    err = bt_le_bluedroid_csis_init(csis_svc, count);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_MCS
int bt_le_bluedroid_media_proxy_pl_init(void)
{
    int err;

    err = bt_le_bluedroid_gmcs_init();
    if (err) {
        return err;
    }

    err = bt_le_bluedroid_mcs_init();
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_VCP_VOL_REND
int bt_le_bluedroid_vcp_vol_rend_init(void)
{
    struct bt_vcp_included vcp_included;
    int err;

    memset(&vcp_included, 0, sizeof(vcp_included));

    err = bt_vcp_vol_rend_included_get(&vcp_included);
    if (err) {
        return err;
    }

    err = bt_le_bluedroid_vcs_init(&vcp_included);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
int bt_le_bluedroid_micp_mic_dev_init(void)
{
    struct bt_micp_included micp_included;
    int err;

    memset(&micp_included, 0, sizeof(micp_included));

    err = bt_micp_mic_dev_included_get(&micp_included);
    if (err) {
        return err;
    }

    err = bt_le_bluedroid_mics_init(&micp_included);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_MICP_MIC_DEV */

int bt_le_bluedroid_audio_start(void *info)
{
    struct bt_gatt_service *inc_csis_svc = NULL;
    int err = 0;

#if CONFIG_BT_CSIP_SET_MEMBER
    /* Note:
     * Should add CSIS before CAS in case one CSIS instance
     * is included by CAS.
     */
    err = bluedroid_gatt_csis_init(info, &inc_csis_svc);
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_CAP_ACCEPTOR
    err = bt_le_bluedroid_cas_init(inc_csis_svc);
    if (err) {
        return err;
    }
#else /* CONFIG_BT_CAP_ACCEPTOR */
    ARG_UNUSED(inc_csis_svc);
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if (BLE_AUDIO_SVC_DEFERRED_ADD == 0)
#if CONFIG_BT_MCS
    err = bt_le_bluedroid_media_proxy_pl_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_VCP_VOL_REND
    err = bt_le_bluedroid_vcp_vol_rend_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
    err = bt_le_bluedroid_micp_mic_dev_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MICP_MIC_DEV */
#endif /* (BLE_AUDIO_SVC_DEFERRED_ADD == 0) */

#if CONFIG_BT_ASCS
    err = bt_le_bluedroid_ascs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_ASCS */

#if CONFIG_BT_PACS
    err = bt_le_bluedroid_pacs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_PACS */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
    err = bt_le_bluedroid_bass_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_TMAP
    err = bt_le_bluedroid_tmas_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TMAP */

#if CONFIG_BT_GMAP
    /* GMAS attrs are role-dependent (filled by esp_ble_audio_gmap_register, after
     * audio_init), so create it here in the start phase where it's populated. */
    err = bt_le_bluedroid_gmas_init();
    if (err) {
        return err;
    }

    err = bt_le_bluedroid_gmas_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_GMAP */

#if CONFIG_BT_CSIP_SET_MEMBER
    err = bt_le_bluedroid_csis_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_CAP_ACCEPTOR
    err = bt_le_bluedroid_cas_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if CONFIG_BT_VCP_VOL_REND
    err = bt_le_bluedroid_vcs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
    err = bt_le_bluedroid_mics_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MICP_MIC_DEV */

#if CONFIG_BT_MCS
    err = bt_le_bluedroid_gmcs_start();
    if (err) {
        return err;
    }

    err = bt_le_bluedroid_mcs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_TBS
    err = bt_le_bluedroid_gtbs_start();
    if (err) {
        return err;
    }

    err = bt_le_bluedroid_tbs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_HAS
    err = bt_le_bluedroid_has_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_HAS */

    return err;
}
