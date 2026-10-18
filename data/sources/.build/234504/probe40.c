#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for driver/isp_dma.h */
#ifndef STUB_DRIVER_ISP_DMA_H_A3AB4AFF
#define STUB_DRIVER_ISP_DMA_H_A3AB4AFF
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_driver_isp_dma_h_a3ab4aff;
#endif
/* stub for esp_cache.h */
#ifndef STUB_ESP_CACHE_H_028DA187
#define STUB_ESP_CACHE_H_028DA187
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_cache_h_028da187;
#endif
/* stub for esp_check.h */
#ifndef STUB_ESP_CHECK_H_A6A879A9
#define STUB_ESP_CHECK_H_A6A879A9
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_check_h_a6a879a9;
#endif
/* stub for esp_heap_caps.h */
#ifndef STUB_ESP_HEAP_CAPS_H_44AB6F5A
#define STUB_ESP_HEAP_CAPS_H_44AB6F5A
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_heap_caps_h_44ab6f5a;
#endif
/* stub for esp_log.h */
#ifndef STUB_ESP_LOG_H_E673833F
#define STUB_ESP_LOG_H_E673833F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_log_h_e673833f;
#endif
/* stub for esp_private/dw_gdma.h */
#ifndef STUB_ESP_PRIVATE_DW_GDMA_H_29EE842B
#define STUB_ESP_PRIVATE_DW_GDMA_H_29EE842B
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_private_dw_gdma_h_29ee842b;
#endif
/* stub for esp_private/isp_private.h */
#ifndef STUB_ESP_PRIVATE_ISP_PRIVATE_H_C1B4077E
#define STUB_ESP_PRIVATE_ISP_PRIVATE_H_C1B4077E
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_private_isp_private_h_c1b4077e;
#endif
/* stub for esp_types.h */
#ifndef STUB_ESP_TYPES_H_044F07EB
#define STUB_ESP_TYPES_H_044F07EB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_types_h_044f07eb;
#endif
/* stub for freertos/FreeRTOS.h */
#ifndef STUB_FREERTOS_FREERTOS_H_AC994C92
#define STUB_FREERTOS_FREERTOS_H_AC994C92
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_freertos_freertos_h_ac994c92;
#endif
/* stub for freertos/semphr.h */
#ifndef STUB_FREERTOS_SEMPHR_H_7660B73F
#define STUB_FREERTOS_SEMPHR_H_7660B73F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_freertos_semphr_h_7660b73f;
#endif
/* stub for hal/color_hal.h */
#ifndef STUB_HAL_COLOR_HAL_H_6B110749
#define STUB_HAL_COLOR_HAL_H_6B110749
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_hal_color_hal_h_6b110749;
#endif
/* stub for hal/isp_ll.h */
#ifndef STUB_HAL_ISP_LL_H_8E334912
#define STUB_HAL_ISP_LL_H_8E334912
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_hal_isp_ll_h_8e334912;
#endif
/* stub for hal/mipi_csi_brg_ll.h */
#ifndef STUB_HAL_MIPI_CSI_BRG_LL_H_5C1B645B
#define STUB_HAL_MIPI_CSI_BRG_LL_H_5C1B645B
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_hal_mipi_csi_brg_ll_h_5c1b645b;
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

#include <assert.h>

#define ISP_DMA_IN_BURST_LEN_DFT    8
#define ISP_DMA_OUT_BURST_LEN_DFT   16

struct esp_isp_dma_frame_ctx_t {
    isp_proc_handle_t proc;
    csi_brg_dev_t *csi_brg_hw;
    dw_gdma_channel_handle_t dma_in_chan;
    dw_gdma_channel_handle_t dma_out_chan;
    SemaphoreHandle_t in_done_sem;
    SemaphoreHandle_t out_done_sem;
    dw_gdma_block_transfer_config_t dma_out_trans;
    dw_gdma_block_transfer_config_t dma_in_trans;
    uint32_t input_frame_size_64bit;
    uint32_t output_frame_size_64bit;
};

static const char *TAG = "ISP_DMA";

static bool IRAM_ATTR s_isp_dma_done_cb(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data)
{
    (void)chan;
    (void)event_data;
    BaseType_t high_task_woken = pdFALSE;
    xSemaphoreGiveFromISR(*(SemaphoreHandle_t *)user_data, &high_task_woken);
    return high_task_woken == pdTRUE;
}

static dw_gdma_burst_size_t s_isp_dma_burst_len_to_size(uint32_t burst_len)
{
    switch (burst_len) {
    case 1:  return DW_GDMA_BURST_SIZE_1;
    case 4:  return DW_GDMA_BURST_SIZE_4;
    case 8:  return DW_GDMA_BURST_SIZE_8;
    default: return DW_GDMA_BURST_SIZE_8;
    }
}

static esp_err_t s_isp_dma_sync_cacheable_buffer(void *buffer, size_t size, int flags, const char *buffer_name)
{
    size_t cache_line_size = esp_cache_get_line_size_by_addr(buffer);
    if (cache_line_size == 0) {
        return ESP_OK;
    }

    ESP_RETURN_ON_ERROR(esp_cache_msync(buffer, size, flags), TAG, "sync %s buffer cache failed", buffer_name);
    return ESP_OK;
}

static void s_isp_dma_frame_ctx_destroy(struct esp_isp_dma_frame_ctx_t *ctx)
{
    if (!ctx) {
        return;
    }
    if (ctx->csi_brg_hw) {
        mipi_csi_brg_ll_enable(ctx->csi_brg_hw, false);
    }
    if (ctx->dma_in_chan) {
        dw_gdma_channel_enable_ctrl(ctx->dma_in_chan, false);
        dw_gdma_del_channel(ctx->dma_in_chan);
    }
    if (ctx->dma_out_chan) {
        dw_gdma_channel_enable_ctrl(ctx->dma_out_chan, false);
        dw_gdma_del_channel(ctx->dma_out_chan);
    }
    if (ctx->in_done_sem) {
        vSemaphoreDelete(ctx->in_done_sem);
    }
    if (ctx->out_done_sem) {
        vSemaphoreDelete(ctx->out_done_sem);
    }
    free(ctx);
}

esp_err_t isp_dma_configure_input(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(proc->input_data_source == ISP_INPUT_DATA_SOURCE_DWGDMA, ESP_ERR_INVALID_ARG, TAG, "processor input source is not dwgdma");

    bool valid_format = isp_ll_dma_set_data_type(proc->hal.hw, proc->in_color_format);
    ESP_RETURN_ON_FALSE(valid_format, ESP_ERR_INVALID_ARG, TAG, "dma input only supports RAW8/RAW10/RAW12");

    uint32_t in_bits_per_pixel = color_hal_pixel_format_fourcc_get_bit_depth(proc->in_color_format);
    uint64_t frame_bits = (uint64_t)proc->h_res * proc->v_res * in_bits_per_pixel;
    ESP_RETURN_ON_FALSE((frame_bits % 64) == 0, ESP_ERR_INVALID_ARG, TAG, "frame bits should be 64-bit aligned");

    uint64_t frame_size_in_64bit = frame_bits / 64;
    ESP_RETURN_ON_FALSE(frame_size_in_64bit <= ((1UL << 22) - 1), ESP_ERR_INVALID_ARG, TAG, "frame size exceeds hardware limit");

    /*
     * The input burst length is fixed at ISP_DMA_IN_BURST_LEN_DFT: it must be representable as a
     * DW-GDMA MSIZE (1/4/8) and not exceed the ISP input async FIFO capacity (8 x 64-bit). It also
     * has to equal the input GDMA dst MSIZE (derived from proc->dma_in_burst_len in
     * isp_dma_new_frame_ctx), otherwise the async FIFO overflows.
     */
    isp_ll_dma_set_frame_size(proc->hal.hw, (uint32_t)frame_size_in_64bit);
    isp_ll_dma_set_burst_len(proc->hal.hw, ISP_DMA_IN_BURST_LEN_DFT);
    isp_ll_dma_apply_config(proc->hal.hw);
    proc->dma_in_burst_len = ISP_DMA_IN_BURST_LEN_DFT;

    return ESP_OK;
}

esp_err_t isp_dma_new_frame_ctx(isp_proc_handle_t proc)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(proc->input_data_source == ISP_INPUT_DATA_SOURCE_DWGDMA, ESP_ERR_INVALID_STATE, TAG, "processor input source is not dwgdma");
    ESP_RETURN_ON_FALSE(!proc->dma_frame_ctx, ESP_ERR_INVALID_STATE, TAG, "dma frame context already exists");

    uint32_t output_burst_len = proc->dma_out_burst_len ? proc->dma_out_burst_len : ISP_DMA_OUT_BURST_LEN_DFT;
    ESP_RETURN_ON_FALSE(output_burst_len > 0 && output_burst_len <= 16, ESP_ERR_INVALID_ARG, TAG, "output burst len out of range");

    uint32_t in_bits_per_pixel = color_hal_pixel_format_fourcc_get_bit_depth(proc->in_color_format);
    uint32_t out_bits_per_pixel = color_hal_pixel_format_fourcc_get_bit_depth(proc->out_color_format);
    uint64_t input_frame_bits = (uint64_t)proc->h_res * proc->v_res * in_bits_per_pixel;
    uint64_t output_frame_bits = (uint64_t)proc->h_res * proc->v_res * out_bits_per_pixel;
    ESP_RETURN_ON_FALSE((input_frame_bits % 64) == 0, ESP_ERR_INVALID_ARG, TAG, "input frame bits should be 64-bit aligned");
    ESP_RETURN_ON_FALSE((output_frame_bits % 64) == 0, ESP_ERR_INVALID_ARG, TAG, "output frame bits should be 64-bit aligned");

    struct esp_isp_dma_frame_ctx_t *ctx = heap_caps_calloc(1, sizeof(struct esp_isp_dma_frame_ctx_t), ISP_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(ctx, ESP_ERR_NO_MEM, err, TAG, "no mem for dma frame context");

    ctx->proc = proc;
    ctx->input_frame_size_64bit = input_frame_bits / 64;
    ctx->output_frame_size_64bit = output_frame_bits / 64;

    csi_brg_dev_t *csi_brg_hw = MIPI_CSI_BRG_LL_GET_HW(proc->csi_brg_id);
    ctx->csi_brg_hw = csi_brg_hw;
    mipi_csi_brg_ll_set_intput_data_h_pixel_num(csi_brg_hw, proc->h_res);
    mipi_csi_brg_ll_set_intput_data_v_row_num(csi_brg_hw, proc->v_res);
    mipi_csi_brg_ll_set_burst_len(csi_brg_hw, 512);
    mipi_csi_brg_ll_enable(csi_brg_hw, true);

    ctx->out_done_sem = xSemaphoreCreateBinary();
    ESP_GOTO_ON_FALSE(ctx->out_done_sem, ESP_ERR_NO_MEM, err, TAG, "no mem for output semaphore");
    ctx->in_done_sem = xSemaphoreCreateBinary();
    ESP_GOTO_ON_FALSE(ctx->in_done_sem, ESP_ERR_NO_MEM, err, TAG, "no mem for input semaphore");

    dw_gdma_channel_alloc_config_t dma_out_alloc = {
        .src = {
            .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
            .role = DW_GDMA_ROLE_PERIPH_CSI,
            .handshake_type = DW_GDMA_HANDSHAKE_HW,
            .num_outstanding_requests = 5,
            .status_fetch_addr = MIPI_CSI_BRG_MEM_BASE,
        },
        .dst = {
            .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
            .role = DW_GDMA_ROLE_MEM,
            .handshake_type = DW_GDMA_HANDSHAKE_HW,
            .num_outstanding_requests = 5,
        },
        .flow_controller = DW_GDMA_FLOW_CTRL_SRC,
        .chan_priority = 1,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_new_channel(&dma_out_alloc, &ctx->dma_out_chan), err, TAG, "create output dma channel failed");

    dw_gdma_channel_alloc_config_t dma_in_alloc = {
        .src = {
            .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
            .role = DW_GDMA_ROLE_MEM,
            .handshake_type = DW_GDMA_HANDSHAKE_SW,
            .num_outstanding_requests = 2,
        },
        .dst = {
            .block_transfer_type = DW_GDMA_BLOCK_TRANSFER_CONTIGUOUS,
            .role = DW_GDMA_ROLE_PERIPH_ISP,
            .handshake_type = DW_GDMA_HANDSHAKE_HW,
            .num_outstanding_requests = 1,
        },
        .flow_controller = DW_GDMA_FLOW_CTRL_DST,
        .chan_priority = 1,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_new_channel(&dma_in_alloc, &ctx->dma_in_chan), err, TAG, "create input dma channel failed");

    ctx->dma_out_trans = (dw_gdma_block_transfer_config_t) {
        .src = {
            .addr = MIPI_CSI_BRG_MEM_BASE,
            .width = DW_GDMA_TRANS_WIDTH_64,
            .addr_inc_mode = DW_GDMA_ADDR_INC_MODE_FIXED,
            .burst_size = DW_GDMA_BURST_SIZE_512,
            .axi_burst_len = output_burst_len,
        },
        .dst = {
            .width = DW_GDMA_TRANS_WIDTH_64,
            .addr_inc_mode = DW_GDMA_ADDR_INC_MODE_INCREMENT,
            .burst_size = DW_GDMA_BURST_SIZE_512,
            .axi_burst_len = output_burst_len,
        },
        .size = ctx->output_frame_size_64bit,
    };
    ctx->dma_in_trans = (dw_gdma_block_transfer_config_t) {
        .src = {
            .width = DW_GDMA_TRANS_WIDTH_64,
            .addr_inc_mode = DW_GDMA_ADDR_INC_MODE_INCREMENT,
            .burst_size = DW_GDMA_BURST_SIZE_32,
        },
        .dst = {
            .addr = MIPI_CSI_BRG_MEM_BASE,
            .width = DW_GDMA_TRANS_WIDTH_64,
            .addr_inc_mode = DW_GDMA_ADDR_INC_MODE_FIXED,
            .burst_size = s_isp_dma_burst_len_to_size(proc->dma_in_burst_len),
        },
        .size = ctx->input_frame_size_64bit,
    };

    dw_gdma_event_callbacks_t dma_cbs = {
        .on_full_trans_done = s_isp_dma_done_cb,
    };
    ESP_GOTO_ON_ERROR(dw_gdma_channel_register_event_callbacks(ctx->dma_out_chan, &dma_cbs, &ctx->out_done_sem), err, TAG, "register output dma callback failed");
    ESP_GOTO_ON_ERROR(dw_gdma_channel_register_event_callbacks(ctx->dma_in_chan, &dma_cbs, &ctx->in_done_sem), err, TAG, "register input dma callback failed");

    proc->dma_frame_ctx = ctx;
    return ESP_OK;
err:
    s_isp_dma_frame_ctx_destroy(ctx);
    return ret;
}

void isp_dma_del_frame_ctx(isp_proc_handle_t proc)
{
    if (!proc || !proc->dma_frame_ctx) {
        return;
    }
    s_isp_dma_frame_ctx_destroy(proc->dma_frame_ctx);
    proc->dma_frame_ctx = NULL;
}

esp_err_t esp_isp_dma_process_frame(isp_proc_handle_t proc, void *output_buffer, const void *input_buffer, uint32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(proc && proc->dma_frame_ctx && output_buffer && input_buffer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE((((uintptr_t)output_buffer) % 8) == 0, ESP_ERR_INVALID_ARG, TAG, "output buffer not 8-byte aligned");
    ESP_RETURN_ON_FALSE((((uintptr_t)input_buffer) % 8) == 0, ESP_ERR_INVALID_ARG, TAG, "input buffer not 8-byte aligned");

    esp_isp_dma_frame_ctx_t *ctx = proc->dma_frame_ctx;
    size_t input_frame_size = ctx->input_frame_size_64bit * sizeof(uint64_t);
    size_t output_frame_size = ctx->output_frame_size_64bit * sizeof(uint64_t);

    ESP_RETURN_ON_ERROR(s_isp_dma_sync_cacheable_buffer((void *)input_buffer, input_frame_size,
                                                        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED,
                                                        "input"),
                        TAG, "sync input buffer cache failed");
    /*
     * Discard dirty CPU cache lines before DMA writes the frame. Otherwise a
     * later cache write-back could overwrite data produced by the ISP.
     */
    ESP_RETURN_ON_ERROR(s_isp_dma_sync_cacheable_buffer(output_buffer, output_frame_size,
                                                        ESP_CACHE_MSYNC_FLAG_DIR_M2C, "output"),
                        TAG, "sync output buffer cache failed");

    ctx->dma_out_trans.dst.addr = (uint32_t)output_buffer;
    ctx->dma_in_trans.src.addr = (uint32_t)input_buffer;

    ESP_RETURN_ON_ERROR(dw_gdma_channel_config_transfer(ctx->dma_out_chan, &ctx->dma_out_trans), TAG, "configure output transfer failed");
    ESP_RETURN_ON_ERROR(dw_gdma_channel_config_transfer(ctx->dma_in_chan, &ctx->dma_in_trans), TAG, "configure input transfer failed");
    dw_gdma_channel_enable_ctrl(ctx->dma_out_chan, true);
    dw_gdma_channel_enable_ctrl(ctx->dma_in_chan, true);

    isp_ll_dma_trigger_frame(ctx->proc->hal.hw);

    if (xSemaphoreTake(ctx->in_done_sem, pdMS_TO_TICKS(timeout_ms)) != pdTRUE) {
        ESP_LOGE(TAG, "wait input done timeout");
        return ESP_ERR_TIMEOUT;
    }
    if (xSemaphoreTake(ctx->out_done_sem, pdMS_TO_TICKS(timeout_ms)) != pdTRUE) {
        ESP_LOGE(TAG, "wait output done timeout");
        return ESP_ERR_TIMEOUT;
    }
    ESP_RETURN_ON_ERROR(esp_cache_msync(output_buffer, output_frame_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C),
                        TAG, "sync output buffer failed");
    return ESP_OK;
}
