/**
 * @file xtiny_rb.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-02
 *
 * SPDX-FileCopyrightText: 2025 深圳市天工聚创科技有限公司
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __XTINY_RB_H__
#define __XTINY_RB_H__

/* ==================== [Includes] ========================================== */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XTINY_RB_FORCE          1
#define XTINY_RB_NO_FORCE       0

/* ==================== [Typedefs] ========================================== */

struct xtiny_rb {
    volatile uint32_t   head        : (sizeof(uint32_t) * 8) - 1;
    volatile uint32_t   head_mirror : 1;
    volatile uint32_t   tail        : (sizeof(uint32_t) * 8) - 1;
    volatile uint32_t   tail_mirror : 1;
    uint32_t            buf_size;
    uint8_t            *p_buf;
};

/* ==================== [Global Prototypes] ================================= */

void xtiny_rb_init(struct xtiny_rb *rb, uint8_t *buf, uint32_t size);
void xtiny_rb_reset(struct xtiny_rb *rb);

bool xtiny_rb_is_empty(const struct xtiny_rb *rb);
bool xtiny_rb_is_full(const struct xtiny_rb *rb);

uint32_t xtiny_rb_get_continuous_read_space(const struct xtiny_rb *rb);
uint32_t xtiny_rb_get_continuous_write_space(const struct xtiny_rb *rb, uint8_t b_force);

uint32_t xtiny_rb_get_size(const struct xtiny_rb *rb);
uint32_t xtiny_rb_get_filled(const struct xtiny_rb *rb);
uint32_t xtiny_rb_get_empty(const struct xtiny_rb *rb);

/* src 为 NULL 时不实际写入，只改变 head 和 tail */
uint32_t xtiny_rb_write(struct xtiny_rb *rb, const uint8_t *src, uint32_t size, uint8_t b_force);
/* dst 为 NULL 时移除，不复制 */
uint32_t xtiny_rb_read(struct xtiny_rb *rb, uint8_t *dst, uint32_t size);

/* ==================== [Macros] ============================================ */

#define XTINY_RB_IS_VALID(rb)       ((rb) && ((rb)->p_buf) && ((rb)->buf_size > 0))

#define XTINY_RB_IS_EMPTY(rb)       (((rb)->tail == (rb)->head) && ((rb)->tail_mirror == (rb)->head_mirror))
#define XTINY_RB_IS_FULL(rb)        (((rb)->tail == (rb)->head) && ((rb)->tail_mirror != (rb)->head_mirror))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XTINY_RB_H__
