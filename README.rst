=====
fsm
=====

Simple Finite-State Machine that can be used with embedded systems

Setup
------------

To work with your project, two dedicated section have to be added to your linker script. One to store the different state and one other to store the possible state transitions:

.. code-block:: c

  .fsm_state :
  {
      . = ALIGN(4);
      _fsm_start = .;
      KEEP(*(.fsm_state))           /* fsm state */
      . = ALIGN(4);
      _fsm_end = . ;
  } >FLASH

  .fsm_trans :
  {
      . = ALIGN(4);
      _fsm_trans_start = .;
      KEEP(*(.fsm_trans))           /* fsm transition*/
      . = ALIGN(4);
      _fsm_trans_end = . ;
  } >FLASH

Example
------------

In this example, we'll implement the following state machine:

.. code-block:: c

  #include "fsm.h"

  static struct fsm_ctx g_fsm;

  #define EVT1    0x01
  #define EVT2    0x02
  #define EVT3    0x03


  // Custom function to send app event to the FSM
  static void app_send_event(uint8_t evt)
  {
      // Check if the event needs to be proceed by the state machine
      if(fsm_filter(&g_fsm, evt) == 0)
    {
      if(mcu_in_isr())
      {
        fsm_post_event_isr(&g_fsm, evt);
      }else{
        fsm_post_event(&g_fsm, evt);
      }
    }
  }

  static int32_t init_state_check(struct fsm_ctx *fsm, uint32_t state)
  {
      // Called by the state machine to check if the app allows the state change
      // return -1 to prevent the FSM to move to the state
      printf("init_state_check\n");
      return 0;
  }

  static int32_t init_exit_state(struct fsm_ctx *fsm, uint32_t evt)
  {
      // Called by the state machine when the state exits
      printf("init_exit_state\n");
      return 0;
  }

  static int32_t init_enter_state(struct fsm_ctx *fsm, uint32_t evt)
  {
      // Called by the state machine when the state enters
      printf("init_enter_state\n");

      app_send_event(EVT1);
  }

  FSM_ADD_STATE(init, FSM_INIT_STATE, init_state_check, init_enter_state, init_exit_state);

  static int32_t state1_state_check(struct fsm_ctx *fsm, uint32_t state)
  {
      // Called by the state machine to check if the app allows the state change
      // return -1 to prevent the FSM to move to the state
      printf("state1_state_check\n");
      return 0;
  }

  static int32_t state1_exit_state(struct fsm_ctx *fsm, uint32_t evt)
  {
      // Called by the state machine when the state exits
      printf("state1_exit_state\n");
      return 0;
  }

  static int32_t state1_enter_state(struct fsm_ctx *fsm, uint32_t evt)
  {
      // Called by the state machine when the state enters
      printf("state1_enter_state\n");
      delay_ms(1000);
      app_send_event(EVT2);
  }

  FSM_ADD_STATE(state1, FSM_STATE1, state1_state_check, state1_exit_state, state1_enter_state);


  static int32_t state2_state_check(struct fsm_ctx *fsm, uint32_t state)
  {
      // Called by the state machine to check if the app allows the state change
      // return -1 to prevent the FSM to move to the state
      printf("state2_state_check\n");
      return 0;
  }

  static int32_t state2_exit_state(struct fsm_ctx *fsm, uint32_t evt)
  {
      // Called by the state machine when the state exits
      printf("state2_exit_state\n");
      return 0;
  }

  static int32_t state2_enter_state(struct fsm_ctx *fsm, uint32_t evt)
  {
      // Called by the state machine when the state enters
      printf("state2_enter_state\n");
      delay_ms(1000);
      app_send_event(EVT3);
  }

  FSM_ADD_STATE(state2, FSM_STATE2, state2_state_check, state2_exit_state, state2_enter_state);

  FSM_ADD_TRANSITION(init_to_state1, FSM_INIT_STATE, FSM_STATE1, EVT1);
  FSM_ADD_TRANSITION(state1_to_state2, FSM_STATE1, FSM_STATE2, EVT2);
  FSM_ADD_TRANSITION(state2_to_state1, FSM_STATE2, FSM_STATE1, EVT3);

  void main(void)
  {   
      printf("main\n");

      fsm_init(&g_fsm, NULL);
      fsm_process(&g_fsm);
  }
