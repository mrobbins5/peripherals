#ifndef __DEBOUNCE_H__
#define __DEBOUNCE_H__

#include <stdint.h>
#include "driver_defines.h"
#include "gpio_port.h"




//*****************************************************************************
//DEBOUNCE//
//*****************************************************************************
void debounce_wait(void) ;

bool sw_io_debounce_fsm(void);


//Debounce the button to prevent double presses
bool sw1_debounce_fsm(void);

//Debounce the button to prevent double presses
bool sw2_debounce_fsm(void);
#endif