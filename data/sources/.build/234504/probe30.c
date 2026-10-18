#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for esp_check.h */
#ifndef STUB_ESP_CHECK_H_A6A879A9
#define STUB_ESP_CHECK_H_A6A879A9
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_check_h_a6a879a9;
#endif
/* stub for esp_event.h */
#ifndef STUB_ESP_EVENT_H_5915D679
#define STUB_ESP_EVENT_H_5915D679
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_event_h_5915d679;
#endif
/* stub for esp_log.h */
#ifndef STUB_ESP_LOG_H_E673833F
#define STUB_ESP_LOG_H_E673833F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_log_h_e673833f;
#endif
/* stub for esp_netif.h */
#ifndef STUB_ESP_NETIF_H_D1AC7B61
#define STUB_ESP_NETIF_H_D1AC7B61
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_netif_h_d1ac7b61;
#endif
/* stub for esp_netif_sntp.h */
#ifndef STUB_ESP_NETIF_SNTP_H_82B35F15
#define STUB_ESP_NETIF_SNTP_H_82B35F15
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_netif_sntp_h_82b35f15;
#endif
/* stub for esp_sntp.h */
#ifndef STUB_ESP_SNTP_H_BDD21509
#define STUB_ESP_SNTP_H_BDD21509
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_sntp_h_bdd21509;
#endif
/* stub for freertos/FreeRTOS.h */
#ifndef STUB_FREERTOS_FREERTOS_H_AC994C92
#define STUB_FREERTOS_FREERTOS_H_AC994C92
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_freertos_freertos_h_ac994c92;
#endif
/* stub for freertos/event_groups.h */
#ifndef STUB_FREERTOS_EVENT_GROUPS_H_48E99D91
#define STUB_FREERTOS_EVENT_GROUPS_H_48E99D91
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_freertos_event_groups_h_48e99d91;
#endif
/* stub for lwip/apps/sntp.h */
#ifndef STUB_LWIP_APPS_SNTP_H_06F04914
#define STUB_LWIP_APPS_SNTP_H_06F04914
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_lwip_apps_sntp_h_06f04914;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


// Remove compat macro and include lwip API
#undef SNTP_OPMODE_POLL

static const char *TAG = "esp_netif_sntp";

ESP_EVENT_DEFINE_BASE(NETIF_SNTP_EVENT);

typedef struct sntp_storage {
    esp_sntp_time_cb_t sync_cb;
    SemaphoreHandle_t sync_sem;
    ip_event_t ip_event_to_renew;
    size_t index_of_first_server;
    size_t num_of_servers;
    char* servers[];
} sntp_storage_t;

static sntp_storage_t *s_storage = NULL;

static void sync_time_cb(struct timeval *tv)
{
    if (s_storage && s_storage->sync_sem) {
        xSemaphoreGive(s_storage->sync_sem);
    }
    if (s_storage && s_storage->sync_cb) {
        s_storage->sync_cb(tv);
    }
    // Post event with the timeval payload
    esp_netif_sntp_time_sync_t evt = {0};
    if (tv) {
        evt.tv = *tv;
    }
    if (esp_event_post(NETIF_SNTP_EVENT, NETIF_SNTP_TIME_SYNC, &evt, sizeof(evt), 0) != ESP_OK) {
        ESP_LOGE(TAG, "Error posting SNTP time sync");
    }
}

static esp_err_t sntp_init_api(void *ctx)
{
    esp_err_t ret = ESP_OK;

    esp_sntp_config_t * config = ctx;
    ESP_GOTO_ON_FALSE(config->num_of_servers <= SNTP_MAX_SERVERS, ESP_ERR_INVALID_ARG, err, TAG, "Tried to configure more servers than enabled in lwip. Please update CONFIG_SNTP_MAX_SERVERS");

    sntp_set_time_sync_notification_cb(sync_time_cb);
    sntp_setoperatingmode(SNTP_OPMODE_POLL);

    for (int i = 0; i < config->num_of_servers; ++i) {
        sntp_setservername(i, config->servers[i]);
    }

    if (config->server_from_dhcp) {
#if LWIP_DHCP_GET_NTP_SRV
        sntp_servermode_dhcp(1);
#else
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "Tried to configure SNTP server from DHCP, while disabled. Please enable CONFIG_LWIP_DHCP_GET_NTP_SRV");
#endif
    }

    if (config->smooth_sync) {
        sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    }

    if (config->start) {
        sntp_init();
    }
err:
    return ret;
}

static esp_err_t renew_servers_api(void *ctx)
{
    if (s_storage && s_storage->num_of_servers) {
        for (int i = 0; i < s_storage->num_of_servers; ++i) {
            sntp_setservername(i + s_storage->index_of_first_server, s_storage->servers[i]);
        }
    }
    return ESP_OK;
}

void esp_netif_sntp_renew_servers(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_netif_tcpip_exec(renew_servers_api, NULL);
}

esp_err_t esp_netif_sntp_init(const esp_sntp_config_t * config)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(s_storage == NULL, ESP_ERR_INVALID_STATE, TAG, "esp_netif_sntp already initialized");
    s_storage = calloc(1, sizeof(sntp_storage_t) + // allocate space for servers only if we are supposed to refresh the settings
                          (config->renew_servers_after_new_IP ? config->num_of_servers * sizeof(char*) : 0));
    ESP_GOTO_ON_FALSE(s_storage != NULL, ESP_ERR_NO_MEM, err, TAG, "Failed to allocate SNTP storage");
    if (config->wait_for_sync) {
        s_storage->sync_sem = xSemaphoreCreateBinary();
        ESP_GOTO_ON_FALSE(s_storage->sync_sem != NULL, ESP_ERR_NO_MEM, err, TAG, "Failed to SNTP sync semaphore");
    }
    if (config->renew_servers_after_new_IP && config->num_of_servers > 0) {
        s_storage->num_of_servers = config->num_of_servers;
        s_storage->index_of_first_server = config->index_of_first_server;
        // store the server names, as we'd have to refresh the config after IP event
        for (int i = 0; i < config->num_of_servers; ++i) {
            s_storage->servers[i] = strdup(config->servers[i]);
        }

        ESP_GOTO_ON_ERROR(esp_event_handler_register(IP_EVENT, config->ip_event_to_renew, esp_netif_sntp_renew_servers, NULL),
                          err, TAG, "Failed to register IP event");
        s_storage->ip_event_to_renew = config->ip_event_to_renew;

    }
    if (config->sync_cb) {
        s_storage->sync_cb = config->sync_cb;
    }

    ESP_GOTO_ON_ERROR(esp_netif_tcpip_exec(sntp_init_api, (void*)config), err, TAG, "Failed initialize SNTP service");

    return ret;

err:
    esp_netif_sntp_deinit();
    return ret;
}

static esp_err_t sntp_stop_api(void *ctx)
{
    sntp_stop();
    return ESP_OK;
}

void esp_netif_sntp_deinit(void)
{
    if (s_storage) {
        sntp_storage_t *storage = s_storage;
        s_storage = NULL;
        sntp_set_time_sync_notification_cb(NULL);
        esp_netif_tcpip_exec(sntp_stop_api, NULL);
        if (storage->num_of_servers) {
            for (int i = 0; i < storage->num_of_servers; ++i) {
                free(storage->servers[i]);
            }
            esp_event_handler_unregister(IP_EVENT, storage->ip_event_to_renew, esp_netif_sntp_renew_servers);
        }
        if (storage->sync_sem) {
            vSemaphoreDelete(storage->sync_sem);
        }
        free(storage);

    }
}

esp_err_t esp_netif_sntp_sync_wait(TickType_t tout)
{
    if (s_storage == NULL || s_storage->sync_sem == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (xQueueSemaphoreTake(s_storage->sync_sem, tout) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    if (sntp_get_sync_mode() == SNTP_SYNC_MODE_SMOOTH &&
        sntp_get_sync_status() == SNTP_SYNC_STATUS_IN_PROGRESS) {
        return ESP_ERR_NOT_FINISHED;
    }
    return ESP_OK;
}

static esp_err_t sntp_start_api(void* ctx)
{
    sntp_stop();
    sntp_init();
    return ESP_OK;
}

esp_err_t esp_netif_sntp_start(void)
{
    return esp_netif_tcpip_exec(sntp_start_api, NULL);
}

esp_err_t esp_netif_sntp_reachability(unsigned int index, unsigned int *reachability)
{
    if (index >= SNTP_MAX_SERVERS || reachability == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_storage == NULL || sntp_enabled() == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    *reachability = sntp_getreachability(index);
    return ESP_OK;
}
