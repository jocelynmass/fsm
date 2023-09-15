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
 
#ifndef __FSM_H__
#define __FSM_H__

#include "port.h"

#define FSM_ANY_STATE		0
#define FSM_INIT_STATE  	1
#define FSM_MBOX_NB			16

struct fsm_trans
{
    const char *name;
    uint32_t from_state;
    uint32_t to_state;
    uint32_t trigger;
	bool return_prev;
};

struct fsm_ctx
{
    uint32_t current_state;
	uint32_t prev_state;
	fsm_queue_t mbox;
	void *arg;
};

struct fsm_evt
{
    const char *name;
    uint32_t trigger;
	int32_t (*cb)(struct fsm_ctx *fsm, uint32_t evt);
};

struct fsm_state
{
    const char *name;
    uint32_t state; 
    int32_t (*event_rcvd)(struct fsm_ctx *fsm, uint32_t evt);
    int32_t (*enter_state)(struct fsm_ctx *fsm, uint32_t evt);
	int32_t (*exit_state)(struct fsm_ctx *fsm, uint32_t evt);
};

#define FSM_ADD_TRANSITION(_name, _from, _to, _trigger) const struct fsm_trans _name##_trans __attribute((section(".fsm_trans"))) = { \
												.name = #_name,                         \
												.from_state = _from,                    \
												.to_state = _to,                        \
												.trigger = _trigger,				    \
												.return_prev = false,					\
												};

#define FSM_ADD_EVT(_name, _trigger, _cb) const struct fsm_evt _name##_evt __attribute((section(".fsm_evt"))) = { \
												.name = #_name,                         \
												.trigger = _trigger,				    \
												.cb = _cb,								\
												};


#define FSM_ADD_RETURN_PREV(_name, _trigger) const struct fsm_trans _name##_trans __attribute((section(".fsm_trans"))) = { \
												.name = #_name,                         \
												.trigger = _trigger,				    \
												.from_state = 0,                    	\
												.to_state = 0,                       	\
												.return_prev = true,					\
												};

#define FSM_ADD_STATE(_name, _state, _evt_cb, _enter_cb, _exit_state) const struct fsm_state _name##_state __attribute((section(".fsm_state"))) = { \
												.name = #_name,                         \
												.state = _state,                        \
												.event_rcvd = _evt_cb,               \
												.enter_state = _enter_cb,	            \
												.exit_state = _exit_state,				\
												};

int32_t fsm_init(struct fsm_ctx *ctx, void *arg);
int32_t fsm_process(struct fsm_ctx *ctx);
int32_t fsm_filter(struct fsm_ctx *ctx, uint32_t event_id);
int32_t fsm_post_event(struct fsm_ctx *ctx, uint32_t event_id);
int32_t fsm_post_event_isr(struct fsm_ctx *ctx, uint32_t event_id);
int32_t fsm_get_current_state(struct fsm_ctx *ctx);
void fsm_update_prev_state(struct fsm_ctx *ctx, uint32_t state);
struct fsm_state *fsm_get_state(struct fsm_ctx *ctx, uint32_t state);

#endif //__FSM_H__