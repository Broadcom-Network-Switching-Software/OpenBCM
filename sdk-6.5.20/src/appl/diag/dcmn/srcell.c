/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        srcell.c
 * Purpose:     Source Route Cell command
 */

#include <appl/diag/dcmn/cmdlist.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#endif

char cmd_dpp_source_routed_cell_usage[] =
    "Usages:\n\t"
    "source_routed_cell [Func name] "
    "[in parameters ordered as in the dpp/fabric.h file]\n\t"
;


