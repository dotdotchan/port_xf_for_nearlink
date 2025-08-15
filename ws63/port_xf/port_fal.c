/**
 * @file port_fal.c
 * @author dotc (dotchan@qq.com)
 * @brief 
 * @date 2025-06-10
 * 
 * @Copyright (c) 2025, CorAL. All rights reserved.
 */

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"

#if CONFIG_XF_FAL_ENABLE

#include "xf_fal.h"
#include "xf_init.h"

#include "sfc.h"
#include "soc_osal.h"
#include "partition.h"

#include "port_flash_config.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_fal"

/**
 * @brief 目前暂时只固定描述及使用 分区 0x30: rsv0 for customer，
 * 更多见 SDK 文件 "param_sector.json" （或类似的分区表描述文件）
 */
#define PARTITION_ID 0x30
#define FLASH_4K_MASK 0xFFF

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static xf_err_t safe_flash_page_erase(uint32_t offset, size_t size);
static xf_err_t safe_flash_byte_write(uint32_t offset, const uint8_t *data, uint32_t blen);
static xf_err_t safe_flash_byte_read(uint32_t offset, uint8_t *buff, uint32_t blen);

static xf_err_t internal_flash_init(void);
static xf_err_t internal_flash_deinit(void);
static xf_err_t internal_flash_read(size_t src_offset, void *dst, size_t size);
static xf_err_t internal_flash_write(size_t dst_offset, const void *src, size_t size);
static xf_err_t internal_flash_erase(size_t offset, size_t size);

/* ==================== [Static Variables] ================================== */

static xf_fal_flash_dev_t sc_internal_flash_dev = {
    .name           = INTERNAL_FLASH_NAME,
    .addr           = INTERNAL_FLASH_START_ADDR,
    .len            = INTERNAL_FLASH_LEN,
    .sector_size    = SECTOR_SIZE,
    .page_size      = PAGE_SIZE,
    .io_size        = IO_SIZE,
    .ops.init       = internal_flash_init,
    .ops.deinit     = internal_flash_deinit,
    .ops.read       = internal_flash_read,
    .ops.write      = internal_flash_write,
    .ops.erase      = internal_flash_erase,
};

/* 
    TODO 默认分区表在这里。
 */
static const xf_fal_partition_t internal_flash_partition_table[] = {
    {CONFIG_XF_FAL_DEFAULT_PARTITION_NAME,     INTERNAL_FLASH_NAME, 0, 128 * PAGE_SIZE },
};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

/* ==================== [Static Functions] ================================== */

static int port_xf_fal(void)
{
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_ID, &info);
    if (ret_val != ERRCODE_SUCC || info.part_info.addr_info.size == 0 ||
        (info.part_info.addr_info.addr & FLASH_4K_MASK) != 0 ||
        (info.part_info.addr_info.size & FLASH_4K_MASK) != 0) {
        return XF_ERR_INVALID_CHECK;
    }
    sc_internal_flash_dev.addr = info.part_info.addr_info.addr;
    sc_internal_flash_dev.len = info.part_info.addr_info.size;

    xf_fal_register_flash_device(&sc_internal_flash_dev);
    xf_fal_register_partition_table(
        internal_flash_partition_table,
        ARRAY_SIZE(internal_flash_partition_table));
    return XF_OK;
}
XF_INIT_EXPORT_PREV(port_xf_fal);

static xf_err_t internal_flash_init(void)
{
    return XF_OK;
}

static xf_err_t internal_flash_deinit(void)
{
    return XF_OK;
}

static xf_err_t internal_flash_read(size_t src_offset, void *dst, size_t size)
{
    xf_err_t xf_ret = XF_OK;
    /* 检查 src_offset 是否在 flash 范围内 */
    if (src_offset + size >= sc_internal_flash_dev.len) {
        XF_LOGE(TAG, "out of range");
        XF_LOGE(TAG, "offset:%d\tsize:%d\tflash_len:%d",
                (int)src_offset, (int)size, (int)sc_internal_flash_dev.len);
        return XF_FAIL;
    }

    errcode_t ret = uapi_sfc_reg_read(src_offset + sc_internal_flash_dev.addr, (uint8_t *)dst, size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return XF_OK;
}

static xf_err_t internal_flash_write(size_t dst_offset, const void *src, size_t size)
{
    xf_err_t xf_ret = XF_OK;
    /* 检查 dst_offset 是否在 flash 范围内 */
    if (dst_offset + size >= sc_internal_flash_dev.len) {
        XF_LOGE(TAG, "out of range");
        XF_LOGE(TAG, "offset:%d\tsize:%d\tflash_len:%d",
                (int)dst_offset, (int)size, (int)sc_internal_flash_dev.len);
        return XF_FAIL;
    }
    errcode_t ret = uapi_sfc_reg_write(dst_offset + sc_internal_flash_dev.addr, (uint8_t *)src, size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return XF_OK;
}

static xf_err_t internal_flash_erase(size_t offset, size_t size)
{
    xf_err_t xf_ret = XF_OK;
    XF_LOGI(TAG, ">>off:%d, size:%d", offset, size);
    if (offset + size > sc_internal_flash_dev.len) {
        return XF_FAIL;
    }
    // xf_ret = safe_flash_page_erase(offset + sc_internal_flash_dev.addr, size);

    errcode_t ret = uapi_sfc_reg_erase(offset + sc_internal_flash_dev.addr, size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return XF_OK;
}

#endif /* CONFIG_XF_FAL_ENABLE */
