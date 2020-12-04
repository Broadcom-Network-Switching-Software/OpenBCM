/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag_mmu.h
 * Purpose:     
 */

#ifndef   _DIAG_MMU_H_
#define   _DIAG_MMU_H_

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>

/* Functions */
cmd_result_t cmd_dpp_diag_mmu(int unit, args_t* a); 

void cmd_dpp_diag_mmu_usage(int unit);

#endif /* _DIAG_MMU_H_ */
