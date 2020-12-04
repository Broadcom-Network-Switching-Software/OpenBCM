/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _JER_REGS_H_
#define _JER_REGS_H_


int soc_jer_regs_eci_access_check(int unit);


int soc_jer_regs_blocks_access_check_regs(int unit);


int soc_jer_regs_blocks_access_check_mem(int unit, soc_mem_t test_mem, int block_num);


int soc_jer_regs_blocks_access_check_dma(int unit);


int soc_jer_regs_blocks_access_check(int unit);

int soc_jer_brdc_fsrd_blk_id_set(int unit);

#endif 
