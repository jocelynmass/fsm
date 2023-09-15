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
#include "fsm.h"
#include "port.h"

#define FSM_TAG             "FSM"

extern unsigned long _fsm_start;   /* start address for the .fsm_state section. defined in linker script */
extern unsigned long _fsm_end;     /* end address for the .fsm_state section. defined in linker script */
extern unsigned long _fsm_trans_start;   /* start address for the .fsm_trans section. defined in linker script */
extern unsigned long _fsm_trans_end;     /* end address for the .fsm_trans section. defined in linker script */
extern unsigned long _fsm_evt_start;   /* start address for the .fsm_evt section. defined in linker script */
extern unsigned long _fsm_evt_end;     /* end address for the .fsm_evt section. defined in linker script */

struct fsm_state *fsm_get_state(struct fsm_ctx *ctx, uint32_t state)
{
    struct fsm_state *fsm;
    
    for(fsm = (struct fsm_state *)&_fsm_start ; fsm < (struct fsm_state *)&_fsm_end ; fsm++){
        if(fsm->state == state){
            return fsm;
        }
    }

    return NULL;
}

static struct fsm_evt *fsm_get_free_event(struct fsm_ctx *ctx, uint32_t evt)
{
    struct fsm_evt *event;
    
    for(event = (struct fsm_evt *)&_fsm_evt_start ; event < (struct fsm_evt *)&_fsm_evt_end ; event++)
    {   
        if(evt == event->trigger){
            return event;
        }
    }

    return NULL;
}

static int32_t fsm_get_new_state_id(struct fsm_ctx *ctx, uint32_t evt)
{
    struct fsm_trans *trans;
    
    for(trans = (struct fsm_trans *)&_fsm_trans_start ; trans < (struct fsm_trans *)&_fsm_trans_end ; trans++)
    {   
        if((evt == trans->trigger) && trans->return_prev){
            return ctx->prev_state;
        }else{
            if(((ctx->current_state == trans->from_state) || (trans->from_state == FSM_ANY_STATE)) && evt == trans->trigger)
            {
                if((ctx->current_state == trans->to_state)){
                    return -1;
                }
                return trans->to_state;
            }
        }
    }

    return -1;
}

static bool fsm_handle_free_event(struct fsm_ctx *ctx, uint32_t evt)
{
    struct fsm_evt *event;
    
    event = fsm_get_free_event(ctx, evt);

    if(event){
        event->cb(ctx, evt);
        return true;
    }

    return false;
}

int32_t fsm_init(struct fsm_ctx *ctx, void *arg)
{
    struct fsm_state *fsm;

    ctx->current_state = FSM_INIT_STATE;
    ctx->arg = arg;
    fsm = fsm_get_state(ctx, ctx->current_state);
    if(fsm == NULL)
    {
        log_err(FSM_TAG, "no initial state found\n");
        return -1;
    }

    FSM_QUEUE_DEF(fsm_q, FSM_MBOX_NB, sizeof(uint32_t));
    ctx->mbox = fsm_queue_new(&fsm_q);

    return fsm->enter_state(ctx, 0);
}

int32_t fsm_filter(struct fsm_ctx *ctx, uint32_t event_id)
{
    struct fsm_state *state;

    if(fsm_get_free_event(ctx, event_id) != NULL){
        // A free event is an event that is independent 
        // to the state the firmware is currently in
        return 0;
    }

    if(fsm_get_new_state_id(ctx, event_id) != -1){
        return 0;
    }

    state = fsm_get_state(ctx, ctx->current_state);
    if(state->event_rcvd){
        state->event_rcvd(ctx, event_id);
    }

    return -1;
}

int32_t fsm_post_event(struct fsm_ctx *ctx, uint32_t event_id)
{
    return fsm_queue_send(ctx->mbox, &event_id, false);
}

int32_t fsm_post_event_isr(struct fsm_ctx *ctx, uint32_t event_id)
{
    return fsm_queue_send(ctx->mbox, &event_id, true);
}

int32_t fsm_process(struct fsm_ctx *ctx)
{
    uint32_t new_state;
    uint32_t event_id;
    struct fsm_state *state;

    while(1)
    {
        if(fsm_queue_receive(ctx->mbox, &event_id) == 0)
        {
            if(fsm_handle_free_event(ctx, event_id) == false){
                new_state = fsm_get_new_state_id(ctx, event_id);

                if(new_state == -1){
                    continue;
                }

                state = fsm_get_state(ctx, ctx->current_state);
                // Exit current state
                if(state->exit_state)
                {
                    state->exit_state(ctx, event_id);
                }
                
                ctx->prev_state = ctx->current_state;

                state = fsm_get_state(ctx, new_state);
                // Enter new state
                if((state != NULL) && state->enter_state)
                {
                    ctx->current_state = new_state;
                    state->enter_state(ctx, event_id);
                }
            }
        }   
    }

    return -1;
}

int32_t fsm_get_current_state(struct fsm_ctx *ctx)
{
    return ctx->current_state;
}

void fsm_update_prev_state(struct fsm_ctx *ctx, uint32_t state)
{
    ctx->prev_state = state;
}