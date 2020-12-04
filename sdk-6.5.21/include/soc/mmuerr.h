/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_MMUERR_H
#define _SOC_MMUERR_H

extern int soc_mmu_error_init(int unit);
extern int soc_mmu_error_done(int unit);
extern int soc_mmu_error_all(int unit);

/* MMU Error Status register decodings */
#define SOC_ERR_HAND_MMU_XQ_PARITY   0x00000001   /* Notify */
#define SOC_ERR_HAND_MMU_LLA_PARITY  0x00000002   /* Fatal */
#define SOC_ERR_HAND_MMU_PP_SBE      0x00000004   /* Notify */
#define SOC_ERR_HAND_MMU_PP_DBE      0x00000008   /* Notify */
#define SOC_ERR_HAND_MMU_UPK_PARITY  0x00000010   /* Fatal */
#define SOC_ERR_HAND_MMU_ING_PARITY  0x00000020   /* Fatal */
#define SOC_ERR_HAND_MMU_EGR_PARITY  0x00000040   /* Fatal */
#define SOC_ERR_HAND_MMU_ALL         0x0000003f   /* Mask */
#define SOC_ERR_HAND_MMU_ALL_H15     0x0000007f   /* Mask */

#define SOC_MMU_ERR_CLEAR_ALL        0x000001ff   /* Mask */

#define SOC_MMU_ERR_ING_SHIFT        21
#define SOC_MMU_ERR_ING_MASK         0x7

#define SOC_MMU_ERR_ING_ING_BUF      1
#define SOC_MMU_ERR_ING_ING_VLAN     2
#define SOC_MMU_ERR_ING_ING_MC       4

/* Statistics tracking for each port's MMU */
typedef struct soc_mmu_error_s {
    int pp_sbe_blocks_init;
    int pp_dbe_blocks_init;
    int pp_sbe_cells_init;
    int pp_dbe_cells_init;
    int pp_sbe_blocks;
    int pp_dbe_blocks;
    int pp_sbe_cells;
    int pp_dbe_cells;
    int xq_parity;
    int lla_parity;
    int upk_parity;
    int ing_parity;
    int egr_parity;
} soc_mmu_error_t;

#endif  /* !_SOC_MMUERR_H */


