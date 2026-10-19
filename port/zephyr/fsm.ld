/* Collects the FSM_ADD_* tables into flash and defines the bounds fsm.c
 * walks (the same symbols the other ports' linker scripts provide). */
SECTION_PROLOGUE(fsm_area,,SUBALIGN(4))
{
	_fsm_start = .;
	KEEP(*(.fsm_state))
	_fsm_end = .;
	_fsm_trans_start = .;
	KEEP(*(.fsm_trans))
	_fsm_trans_end = .;
	_fsm_evt_start = .;
	KEEP(*(.fsm_evt))
	_fsm_evt_end = .;
} GROUP_ROM_LINK_IN(RAMABLE_REGION, ROMABLE_REGION)
