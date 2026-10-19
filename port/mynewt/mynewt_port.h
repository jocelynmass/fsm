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

#ifndef __MYNEWT_FSM_PORT_H__
#define __MYNEWT_FSM_PORT_H__

#include "os/os.h"

typedef struct fsm_queue_def_t
{
    uint32_t len;
    uint32_t obj_size;
    void *buf;
    void *evbuf;
}fsm_queue_def_t;

typedef struct _fsm_queue_t
{
    fsm_queue_def_t *def;
    struct os_mempool mpool;
    struct os_mempool epool;
    struct os_eventq evq;
}_fsm_queue_t;

typedef _fsm_queue_t* fsm_queue_t;

#define FSM_QUEUE_DEF(_name, _len, _obj_size)  \
    static uint32_t _name##_##buf[_len]; \
    static struct os_event _name##_##evbuf[_len];\
    static fsm_queue_def_t _name = { .len = _len, .obj_size = _obj_size, .buf = _name##_##buf, .evbuf = _name##_##evbuf }; \


static inline int fsm_queue_receive(fsm_queue_t queue, void *data)
{
    struct os_event* ev;

    ev = os_eventq_get(&queue->evq);

    memcpy(data, ev->ev_arg, queue->def->obj_size);

    os_memblock_put(&queue->mpool, ev->ev_arg); // put back mem block
    os_memblock_put(&queue->epool, ev);         // put back ev block

    return 0;
}

static inline int fsm_queue_send(fsm_queue_t queue, void *data, bool isr)
{
    void *edata = NULL;
    struct os_event *ev = NULL;

    edata = os_memblock_get(&queue->mpool);
    if(edata == NULL){
        return -1;
    }
    memcpy(edata, data, queue->def->obj_size);

    ev = (struct os_event*) os_memblock_get(&queue->epool);
    if(ev == NULL){
        os_memblock_put(&queue->mpool, edata);
        return -1;
    }

    memset(ev, 0, sizeof(struct os_event));
    ev->ev_arg = edata;

    os_eventq_put(&queue->evq, ev);

    return 0;
}

static inline fsm_queue_t fsm_queue_new(fsm_queue_def_t *def)
{
    static _fsm_queue_t q;

    memset(&q, 0, sizeof(_fsm_queue_t));
    
    q.def = def;
    if(os_mempool_init(&q.mpool, def->len, def->obj_size, def->buf, "fsm_q")){
        return NULL;
    }

    if(os_mempool_init(&q.epool, def->len, sizeof(struct os_event), def->evbuf, "fsm_evq")){
        return NULL;
    }

    os_eventq_init(&q.evq);

    return (fsm_queue_t)&q;
}


#endif //__MYNEWT_FSM_PORT_H__