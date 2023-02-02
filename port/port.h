#ifndef __FSM_PORT_H__
#define __FSM_PORT_H__

#ifdef FSM_FREERTOS_PORT 
#include "freertos_port.h"
#endif

static inline int fsm_queue_receive(fsm_queue_t queue, void *data);
static inline int fsm_queue_send(fsm_queue_t queue, void *data, bool isr);
static inline fsm_queue_t fsm_queue_new(fsm_queue_def_t *def);

#endif //__FSM_PORT_H__