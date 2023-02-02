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

#ifndef __FREERTOS_FSM_PORT_H__
#define __FREERTOS_FSM_PORT_H__

#include "queue.h"

typedef struct fsm_queue_def_t
{
    uint32_t len;
    uint32_t obj_size;
}fsm_queue_def_t;

typedef QueueHandle_t fsm_queue_t;

#define FSM_QUEUE_DEF(_name, _len, _obj_size)  \
    fsm_queue_def_t _name = { .len = _len, .obj_size = _obj_size }; \

static inline int fsm_queue_receive(fsm_queue_t queue, void *data)
{
    return xQueueReceive(queue, data, portMAX_DELAY) == pdTRUE ? 0 : -1;
}

static inline int fsm_queue_send(fsm_queue_t queue, void *data, bool isr)
{
    if(isr){
        return xQueueSendToBackFromISR(queue, data, NULL) == pdTRUE ? 0 : -1;
    }else{
        return xQueueSendToBack(queue, data, portMAX_DELAY) == pdTRUE ? 0 : -1;
    }
}

static inline fsm_queue_t fsm_queue_new(fsm_queue_def_t *def)
{
    return xQueueCreate(def->len, def->obj_size);
}


#endif //__FREERTOS_FSM_PORT_H__