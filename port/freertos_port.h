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