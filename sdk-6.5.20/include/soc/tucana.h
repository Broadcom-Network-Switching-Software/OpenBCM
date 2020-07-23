/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tucana.h
 */

#ifndef _SOC_TUCANA_H_
#define _SOC_TUCANA_H_

#include <soc/drv.h>


extern int soc_tucana_misc_init(int);
extern int soc_tucana_mmu_init(int);
extern int soc_tucana_age_timer_get(int, int *, int *);
extern int soc_tucana_age_timer_max_get(int, int *);
extern int soc_tucana_age_timer_set(int, int, int);
extern int soc_tucana_stat_get(int, soc_port_t, int, uint64*);

extern int soc_tucana_arl_parity_error(int unit, int blk, int mem);
extern int soc_tucana_mmu_parity_error(int unit, int blk, uint32 info);
extern int soc_tucana_mmu_crc_error(int unit, uint32 addr);
extern int soc_tucana_pdll_lock_loss(int unit, uint32 info);
extern int soc_tucana_num_cells(int unit, int *num_cells);

#define SOC_TUCANA_CRC_ADDR_MASK       0xffff
#define SOC_TUCANA_CRC_MEM_BITS_SHIFT  16
#define SOC_TUCANA_CRC_MEM_BITS_MASK   0x3

#define SOC_TUCANA_PARITY_XQ           0x001
#define SOC_TUCANA_PARITY_EBUF_HG      0x002
#define SOC_TUCANA_PARITY_EBUF         0x004
#define SOC_TUCANA_PARITY_CELLASMB     0x008
#define SOC_TUCANA_PARITY_RELMGR       0x010
#define SOC_TUCANA_PARITY_CCC          0x020
#define SOC_TUCANA_PARITY_CCPTR        0x040
#define SOC_TUCANA_PARITY_CG1M1_FIFO   0x080
#define SOC_TUCANA_PARITY_CG1M0_FIFO   0x100
#define SOC_TUCANA_PARITY_CG0M1_FIFO   0x200
#define SOC_TUCANA_PARITY_CG0M0_FIFO   0x400

#define SOC_TUCANA_MEM_FAIL_DLL 0x2
#define SOC_TUCANA_MEM_FAIL_PLL 0x1

/* Needed for MMU cell calculations.  This is a fixed HW value. */
#define SOC_TUCANA_MMU_PTRS_PER_PTR_BLOCK       125

extern soc_functions_t soc_tucana_drv_funs;

/*
 * IPORT Mode Definitions.
 *     These indicate the polarity of the IPORT_MODE bit in the
 *     rule and mask tables.
 * SOC_IPORT_MODE_SPECIFIC  -  The port field may be compared like
 *     any other field.  It indicates a speicific port (or all ones
 *     to indicate all ports in the block).  Note that if the
 *     IPORT_MASK in the mask is 0, then the IPORT field must be
 *     zero for proper operation; the IPORT field is ignored in the
 *     match in this case.
 * SOC_IPORT_MODE_ARBITRARY_BMAP -   Arbitrary bitmaps supported;
 *     special comparison and sorting is required.
 */
#define SOC_IPORT_MODE_SPECIFIC           0
#define SOC_IPORT_MODE_ARBITRARY_BMAP     1

/* Values for the port type field in the port table in tucana */

#define SOC_TUCANA_PORT_TYPE_DEFAULT 0
#define SOC_TUCANA_PORT_TYPE_TRUNK 1
#define SOC_TUCANA_PORT_TYPE_STACKING 2
#define SOC_TUCANA_PORT_TYPE_RESERVED 3

#endif	/* !_SOC_TUCANA_H_ */
