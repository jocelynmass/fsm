/*
 * Copyright (c) 2017-2023 Jocelyn Masserot.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimers.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimers in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of Jocelyn Masserot, nor the names of its contributors
 *     may be used to endorse or promote products derived from this Software
 *     without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * WITH THE SOFTWARE.
 */

#ifndef __ZEPHYR_FSM_PORT_H__
#define __ZEPHYR_FSM_PORT_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

typedef struct fsm_queue_def_t
{
    struct k_msgq *msgq;
    char *buf;
    uint32_t len;
    uint32_t obj_size;
}fsm_queue_def_t;

typedef struct k_msgq* fsm_queue_t;

/* Static storage: fsm_init() expands this inside the function */
#define FSM_QUEUE_DEF(_name, _len, _obj_size)  \
    static struct k_msgq _name##_##msgq; \
    static char __aligned(4) _name##_##buf[(_len) * (_obj_size)]; \
    static fsm_queue_def_t _name = { .msgq = &_name##_##msgq, .buf = _name##_##buf, .len = _len, .obj_size = _obj_size }; \

#define log_err(_tag, _fmt, ...) printk("[" _tag "] " _fmt, ##__VA_ARGS__)

static inline int fsm_queue_receive(fsm_queue_t queue, void *data)
{
    return k_msgq_get(queue, data, K_FOREVER) == 0 ? 0 : -1;
}

/* From an ISR (or to avoid blocking) the event is dropped if the queue is full */
static inline int fsm_queue_send(fsm_queue_t queue, void *data, bool isr)
{
    return k_msgq_put(queue, data, isr ? K_NO_WAIT : K_FOREVER) == 0 ? 0 : -1;
}

static inline fsm_queue_t fsm_queue_new(fsm_queue_def_t *def)
{
    k_msgq_init(def->msgq, def->buf, def->obj_size, def->len);

    return def->msgq;
}

#endif //__ZEPHYR_FSM_PORT_H__
