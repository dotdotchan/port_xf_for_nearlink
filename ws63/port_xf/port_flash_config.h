/**
 * @file port_flash_config.h
 * @author dotc (dotchan@qq.com)
 * @brief 
 * @date 2025-06-10
 * 
 * @Copyright (c) 2025, CorAL. All rights reserved.
 */

#ifndef __PORT_FLASH_CONFIG_H__
#define __PORT_FLASH_CONFIG_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "xf_fal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define INTERNAL_FLASH_NAME                 CONFIG_XF_FAL_DEFAULT_FLASH_DEVICE_NAME

#define INTERNAL_FLASH_SECTOR_SIZE          (4 * 1024)
#define INTERNAL_FLASH_PAGE_SIZE            (256)
#define INTERNAL_FLASH_IO_SIZE              (4)

#define SECTOR_SIZE                         INTERNAL_FLASH_SECTOR_SIZE
#define PAGE_SIZE                           INTERNAL_FLASH_PAGE_SIZE
#define IO_SIZE                             INTERNAL_FLASH_IO_SIZE

#define INTERNAL_FLASH_START_ADDR           (0)
#define INTERNAL_FLASH_LEN                  (0 * 1024)

#define START_ADDR                          INTERNAL_FLASH_START_ADDR

#define USER_PART                           CONFIG_XF_FAL_DEFAULT_PARTITION_NAME

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __PORT_FLASH_CONFIG_H__ */
