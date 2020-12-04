/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DPP_INTERRUPTS_HANDLER_H_
#define _DPP_INTERRUPTS_HANDLER_H_

#include <soc/dpp/ARAD/arad_interrupts.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>

/*
* fill common parameters 
*/
int arad_interrupt_handler_init_cmn_param(int unit, interrupt_common_params_t* interrupt_common_params);

#endif /*__DPP_INTERRUPTS_HANDLER_H_ */
