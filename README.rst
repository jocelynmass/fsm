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

