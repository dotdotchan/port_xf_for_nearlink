/**
 * @file xtiny_rb.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-02
 *
 * SPDX-FileCopyrightText: 2025 深圳市天工聚创科技有限公司
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/* ==================== [Includes] ========================================== */

#include "xtiny_rb.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================= */

void xtiny_rb_init(struct xtiny_rb *rb, uint8_t *buf, uint32_t size)
{
    if (!rb) {
        return;
    }
    xtiny_rb_reset(rb);
    rb->buf_size       = size;
    rb->p_buf          = buf;
}

void xtiny_rb_reset(struct xtiny_rb *rb)
{
    if (!rb) {
        return;
    }
    rb->head          = 0;
    rb->head_mirror   = 0;
    rb->tail          = 0;
    rb->tail_mirror   = 0;
}

uint32_t xtiny_rb_get_size(const struct xtiny_rb *rb)
{
    if (!XTINY_RB_IS_VALID(rb)) {
        return 0;
    }
    return rb->buf_size;
}

uint32_t xtiny_rb_get_filled(const struct xtiny_rb *rb)
{
    if (!XTINY_RB_IS_VALID(rb)) {
        return 0;
    }
    if (rb->tail == rb->head) {
        return (rb->tail_mirror == rb->head_mirror) ? (0) : (rb->buf_size);
    }
    return (rb->tail > rb->head)
           ? (rb->tail - rb->head)
           : (rb->buf_size - (rb->head - rb->tail));
}

uint32_t xtiny_rb_get_empty(const struct xtiny_rb *rb)
{
    if (!XTINY_RB_IS_VALID(rb)) {
        return 0;
    }
    return rb->buf_size - xtiny_rb_get_filled(rb);
}

bool xtiny_rb_is_empty(const struct xtiny_rb *rb)
{
    if (!XTINY_RB_IS_VALID(rb)) {
        return 0;
    }
    return XTINY_RB_IS_EMPTY(rb);
}

bool xtiny_rb_is_full(const struct xtiny_rb *rb)
{
    if (!XTINY_RB_IS_VALID(rb)) {
        return 0;
    }
    return XTINY_RB_IS_FULL(rb);
}

/**
 * @brief 获取当前读索引到缓冲区末尾的连续可读字节数
 *
 * @param rb 环形缓冲区指针
 * @return uint32_t 连续可读字节数（缓冲区无效或为空时返回0）
 */
uint32_t xtiny_rb_get_continuous_read_space(const struct xtiny_rb *rb)
{
    if (!XTINY_RB_IS_VALID(rb)) {
        return 0;
    }
    /* 快照，避免并发修改导致计算不一致 */
    uint32_t head      = rb->head;
    uint32_t tail      = rb->tail;
    uint32_t head_mirr = rb->head_mirror;
    uint32_t tail_mirr = rb->tail_mirror;
    uint32_t filled;
    if (tail == head) {
        filled = (tail_mirr == head_mirr) ? 0u : rb->buf_size;
    } else if (tail > head) {
        filled = tail - head;
    } else {
        filled = rb->buf_size - (head - tail);
    }
    if (filled == 0) {
        return 0;
    }
    uint32_t contiguous = rb->buf_size - head;
    return (filled < contiguous) ? filled : contiguous;
}

/**
 * @brief 获取当前写索引到缓冲区末尾的连续可写字节数
 *
 * @param rb 环形缓冲区指针
 * @param b_force 是否强制模式（1=强制，0=非强制）
 * @return uint32_t 连续可写字节数（缓冲区无效或为满时返回0）
 */
uint32_t xtiny_rb_get_continuous_write_space(const struct xtiny_rb *rb, uint8_t b_force)
{
    if (!XTINY_RB_IS_VALID(rb)) {
        return 0;
    }
    /* 快照，避免并发修改导致计算不一致 */
    uint32_t head      = rb->head;
    uint32_t tail      = rb->tail;
    uint32_t head_mirr = rb->head_mirror;
    uint32_t tail_mirr = rb->tail_mirror;
    uint32_t contiguous = rb->buf_size - tail;
    if (b_force) {
        return contiguous;
    }
    uint32_t filled;
    if (tail == head) {
        filled = (tail_mirr == head_mirr) ? 0u : rb->buf_size;
    } else if (tail > head) {
        filled = tail - head;
    } else {
        filled = rb->buf_size - (head - tail);
    }
    uint32_t empty = rb->buf_size - filled;
    if (empty == 0) {
        return 0;
    }
    return (empty < contiguous) ? empty : contiguous;
}

uint32_t xtiny_rb_write(
    struct xtiny_rb *rb, const uint8_t *src, uint32_t size, uint8_t b_force)
{
    const uint8_t *p_src = (const uint8_t *)src;
    uint32_t empty_sz;
    uint32_t first;
    if ((!XTINY_RB_IS_VALID(rb)) || (0 == size)) {
        return 0;
    }
    empty_sz = xtiny_rb_get_empty(rb);
    if (b_force) {
        if (size > rb->buf_size) {
            if (src) {
                p_src += (size - rb->buf_size);
            }
            size = rb->buf_size;
        }
    } else {
        if (empty_sz == 0) {
            return 0;
        }
        if (size > empty_sz) {
            size = empty_sz;
        }
    }
    first = rb->buf_size - rb->tail;
    if (first > size) {
        if (src) {
            memcpy(rb->p_buf + rb->tail, p_src, size);
        }
        rb->tail += size;
        if ((b_force) && (size > empty_sz)) {
            rb->head = rb->tail;
        }
    } else {
        uint32_t second = size - first;
        if (src) {
            memcpy(rb->p_buf + rb->tail, p_src, first);
            memcpy(rb->p_buf, p_src + first, second);
        }
        rb->tail_mirror ^= 1;
        rb->tail = second;
        if ((b_force) && (size > empty_sz)) {
            if (rb->tail <= rb->head) {
                rb->head_mirror ^= 1;
            }
            rb->head = rb->tail;
        }
    }
    return size;
}

uint32_t xtiny_rb_read(struct xtiny_rb *rb, uint8_t *dst, uint32_t size)
{
    uint32_t filled;
    uint32_t first;
    if ((!XTINY_RB_IS_VALID(rb)) || (0 == size)) {
        return 0;
    }
    filled = xtiny_rb_get_filled(rb);
    if (0 == filled) {
        return 0;
    }
    if (size > filled) {
        size = filled;
    }
    first = rb->buf_size - rb->head;
    if (first > size) {
        if (dst) {
            memcpy(dst, rb->p_buf + rb->head, size);
        }
        rb->head += size;
    } else {
        uint32_t second = size - first;
        if (dst) {
            memcpy(dst, rb->p_buf + rb->head, first);
            memcpy((char *)dst + first, rb->p_buf, second);
        }
        rb->head_mirror ^= 1;
        rb->head = second;
    }
    return size;
}

/* ==================== [Static Functions] ================================== */
