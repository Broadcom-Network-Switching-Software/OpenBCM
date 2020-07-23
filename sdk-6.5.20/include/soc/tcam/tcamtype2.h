/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     Defines constants describing the Type 2 A20K and A10K chips
 *              for soc/er_tcam.c
 */

/* A20K Interface Regs */
#define A20K_IF_REG_DAR_ADR	0x4000
#define A20K_IF_REG_DRR_ADR	0x4001
#define A20K_IF_REG_DWR_ADR	0x4002

#define A20K_DAR_READ   0x00000800
#define A20K_DRR_READY_MASK 0xc0000000
#define A20K_DRR_READY  0x80000000

#define A20K_IF_REG_RAVA_ADR	0x401F

#define A20K_IF_REG_RAV0_ADR	0x4020
#define A20K_IF_REG_RAV1_ADR	0x4021
#define A20K_IF_REG_RAV2_ADR	0x4022
#define A20K_IF_REG_RAV3_ADR	0x4023

#define A20K_IF_REG_RAVM0_ADR	0x4040
#define A20K_IF_REG_RAVM1_ADR	0x4041
#define A20K_IF_REG_RAVM2_ADR	0x4042
#define A20K_IF_REG_RAVM3_ADR	0x4043


/* A20K System Regs */
#define	A20K_SYS_REG_ICTL_ADR	0x0000
#define	A20K_SYS_REG_IERR_ADR	0x0008
#define	A20K_SYS_REG_IEMR_ADR	0x0009
#define	A20K_SYS_REG_IECR_ADR	0x000A
#define	A20K_SYS_REG_IESR_ADR	0x000B
#define	A20K_SYS_REG_CERR0_ADR	0x000C
#define	A20K_SYS_REG_CERR1_ADR	0x000D
#define	A20K_SYS_REG_CERR2_ADR	0x000E
#define	A20K_SYS_REG_CERR3_ADR	0x000F
#define	A20K_SYS_REG_DCR_ADR	0x0010
#define	A20K_SYS_REG_AIR_ADR	0x0011
#define	A20K_SYS_REG_S32A0_ADR	0x0012
#define	A20K_SYS_REG_S32A1_ADR	0x0013
#define	A20K_SYS_REG_S32B0_ADR	0x0014
#define	A20K_SYS_REG_S32B1_ADR	0x0015
#define	A20K_SYS_REG_SEM_ADR	0x0016
#define	A20K_SYS_REG_MVR_ADR	0x0017
#define	A20K_SYS_REG_DERR_ADR	0x0018
#define	A20K_SYS_REG_DEMR_ADR	0x0019
#define	A20K_SYS_REG_DECR_ADR	0x001A
#define	A20K_SYS_REG_DESR_ADR	0x001B
#define	A20K_SYS_REG_HWCR_ADR	0x0060
#define	A20K_SYS_REG_DLCR_ADR	0x0061
#define	A20K_SYS_REG_INFO_ADR	0x00FF

#define	A20K_SYS_REG_ADM_ADR1(i)	(0x20 + (i) * 2)
#define	A20K_SYS_REG_ADM_ADR0(i)	(0x20 + (i) * 2 + 1)

/* DCR bits */
#define DCR_SOFTREN (1 << 31)
#define DCR_SOFTR   (1 << 30)

/* HWCR bits */
#define HWCR_IOQDR1 (1 << 5)
#define HWCR_IOQDR0 (1 << 4)

/* A20K NSE Regs */

#define A20K_NSE_REG_CMPR_ADR1(i)    (0x00 + (i) * 2)
#define A20K_NSE_REG_CMPR_ADR0(i)    (0x00 + (i) * 2 + 1)
#define	A20K_NSE_REG_CMPR_CNT	0x10

#define A20K_NSE_REG_GMR_07_ADR1(i)    (0x20 + (i) * 2)
#define A20K_NSE_REG_GMR_07_ADR0(i)    (0x20 + (i) * 2 + 1)
#define A20K_NSE_REG_GMR_8F_ADR1(i)    (0x60 + (i) * 2)
#define A20K_NSE_REG_GMR_8F_ADR0(i)    (0x60 + (i) * 2 + 1)
#define	A20K_NSE_REG_GMR_CNT	0x8

#define A20K_NSE_REG_SSR_ADR(i) (0x30 + i)
#define A20K_NSE_REG_SSR_CNT    0x8

#define	A20K_NSE_REG_COMMAND_ADR 0x38
#define	A20K_SET_LRAM_LDEV_MASK	0x180
			
#define	A20K_NSE_REG_HARDWARE_ADR 0x3e
#define	A20K_NSE_REG_PARITY_ADR	0x3f

#define A20K_NSE_REG_CPR_ADR(i) (0x40 + i)
#define A20K_NSE_REG_CPR_CNT    (0x10)

#define A20K_NSE_REG_SRR_ADR1(i)    (0x50 + (i) * 2)
#define A20K_NSE_REG_SRR_ADR0(i)    (0x50 + (i) * 2 + 1)
#define	A20K_NSE_REG_SRR_CNT	0x8

#define	A20K_NSE_REG_BMRX_ADR	0x400
#define	A20K_NSE_REG_BPRX_ADR	0x401
#define	A20K_NSE_REG_BPARX_ADR	0x402

#define	A20K_NSE_REG_BPRA0X_ADR	0x404
#define	A20K_NSE_REG_BPRA1X_ADR	0x405
#define	A20K_NSE_REG_BPRA2X_ADR	0x406
#define	A20K_NSE_REG_BPRA3X_ADR	0x407

/* NSE encoding */
#define TT2_NSE_ADDR(chip, block, reg)  \
    ((reg) | ((block) << 11) | (A20K_TARGET_NSE_REG << 19) | ((chip) << 21))

    /* NSE COMMAND bits */
/* First data quadlet - all zeroes */
/* Second data quadlet */
#define NSE_COMMAND_EMODE   (1 << 31)
#define NSE_COMMAND_LRN (1 << 29)
#define NSE_COMMAND_EN  (1 << 25)
#define NSE_COMMAND_BEN  (1 << 24)

/* Third data quadlet */
#define NSE_COMMAND_TBLSZ_2 (2 << 2)
#define NSE_COMMAND_DEVE    (1 << 1)
#define NSE_COMMAND_SRST    (1 << 0)

/* A20K CMD encoding */
#define	A20K_READ_NSE72_CMD	0x00
#define	A20K_WRITE_NSE72_CMD	0x08

#define	A20K_SEARCH72_CMD	0x18
#define	A20K_SEARCH144_CMD	0x1a


/* A20K encoding of nsize field in ADM sys reg */
#define	A20K_NSIZE_72B	0x0
#define	A20K_NSIZE_144B	0x1
#define ADM_RINFO   27
#define ADM_NSIZE   24


/* A20K encoding of target in quadlet q1 of Read Write NSE command */
#define	A20K_TARGET_NSE_REG		0x3
#define	A20K_TARGET_NSE_DATA_ARRAY	0x0
#define	A20K_TARGET_NSE_MASK_ARRAY	0x1


/* ID pins of A20K and cascaded A10K (in ext_tcam_cypress.v) */
#define A20K_BCST_CHIPID	0x1f
#define A20K_CHIPID		0x0
#define A10K1_CHIPID		0x1
#define A10K2_CHIPID		0x2
#define A10K3_CHIPID		0x3



/* Each A20K (and also each cascaded A10K) have max of 256K entries */
/* Each block has 2K entries */
#define A20K_MAX_ENTRY_NUM		(256*1024)
#define A20K_MAX_ENTRY_NUM_IN_BLK	(2*1024)
#define	A20K_MAX_BLK_NUM		127
#define	A20K_BLK_CNT		0x80


/*************************SEER TABLE CONFIG ENCODING****************
        External Table Configurations:
        0 : L2 = 32K,   LPM - 8K,   MPLS - 8K,  L4 - 4K
        1 : L2 = 512K,  LPM - 8K,   MPLS - 8K,  L4 - 4K
        2 : L2 = 32K,   LPM - 256K, MPLS - 256K,L4 - 4K
        3 : L2 = 32K,   LPM - 8K,   MPLS - 8K,  L4 - 192K
        4 : L2 = 32K,   LPM - 8K,   MPLS - 8K,  L4 - 96K
        5 : L2 = 32K,   LPM - 256K, MPLS - 256K,L4 - 128K
        6 : L2 = 32K,   LPM - 384K, MPLS - 384K,L4 - 64K
       ---------------------------------------------------
       |7 : L2 = 32K,   LPM - 128K, MPLS - 128K,L4 - 64K |
       ---------------------------------------------------
        8 : L2 = 32K,   LPM - 192K, MPLS - 192K,L4 - 32K
        9 : L2 = 32K,   LPM - 448K, MPLS - 448K,L4 - 4K
        10: L2 = 32K,   LPM - 896K, MPLS - 896K,L4 - 4K
        11-15 : not used

        We config the ext table with ACL 64K and LPM 128K
        Total Cam size is (72 X 256K) = 18Mbits
        0 - 128K-1 is ACL
        128K to 256K is LPM
        Associate SRAM is MEM 0 = 36 X 512 K
                          MEM 1 = 36 X 512 K
        64K            Action
        64K            Policing
        64K            Traffic Counter
        32K            Adjacency Portion
        128K           Result
 */

#define ACL_LPM_MAX_CONTEXT     0x7c
#define UD_MACRO_CMD_CONTEXT    0x7e

/*
 * Used when constructing key for searches into ACL table (by fp or by
 * ud_macro_cmd). When ud_macro_cmd=SEARCH with CMD144=1 then we use
 * ACL_GMR0 for search into ACL table 
 */

#define	ACL_ADM0			0x0
#define	ACL_CMPR0			0x0
#define	ACL_GMR0			0x0
#define	ACL_SSR0			0x0


#define	LPM_ADM1			0x1
#define	LPM_CMPR1			0x1

/* To do 64b search in 72b lpm table (by lpm or by ud_macro_cmd) */
#define	LPM_GMR1			0x1

/*
 * To do 72b search in 72b lpm table (only ud initiates these via special
 * ud_macro_cmd:SRCH72)
 */
#define	LPM_GMR2			0x2

/* 
 * used when constructing key for searches into LPM table by lpm. When
 * ud_macro_cmd=SEARCH with CMD144=0 then we use LPM_GMR1 for search
 * into LPM table. When ud_macro_cmd=SEARCH72 with CMD144=0 then we use
 * LPM_GMR2 for search into LPM table 
 */
#define	LPM_SSR1			0x1


/* prog values for cse_cyp_ud_config */

/*
 * bits[19:18] of udcam_wradr form lower 2 bits of 5 bit chipid needed in
 * read write nse command
 */
#define UDQ0_RW_NSE_CHIPID543	0x0

/* 1st quadlet of read_nse, write_nse */
#define UDQ0_RW_NSE_SSR3		0x3

/* 
 * 2nd quadlet of write_nse GMR3 is set to all Ones to allow writing to all
 * bits in data and mask arrays
 */

#define UDQ1_W_NSE_GMR3		0x3

/* Values used when constructing */
/* cypt_rd_nse_reg72, cypt_wr_nse_reg72 tasks */

#define	UD_PT_CMD_PARITY		0x0	/* 1'b0 */
#define	UD_PT_CMD_INDIRECT		0x0	/* 1'b0 */
#define	UD_PT_CMD_SSR			0x0	/* 3'h0 */
#define	UD_PT_CMD_PARALLEL		0x0	/* 1'b0 */
#define	UD_PT_CMD_GMR			0x0	/* 4'h0 */
			/* gmr value during rd wr of cyp nse reg is dont-care */
#define UD_PT_CMD_CONTEXT		0x007f  /* 7'h7F */
			/* use 7F as context number for UD initiated pass-thro commands */
			/* This is not really a cfg in cse, BUT we are just defining it */
			/* different from UD_MACRO_CMD_MGMT_CONTEXT to help in debugging */

#define TT2_NSE_WRITE72 ((1 << 5) | (UD_PT_CMD_CONTEXT << 6) | (A20K_WRITE_NSE72_CMD << 15))
#define TT2_NSE_READ72  ((1 << 5) | (UD_PT_CMD_CONTEXT << 6) | (A20K_READ_NSE72_CMD << 15))
#define TT2_NSE_FETCH72 (UD_PT_CMD_CONTEXT << 6)

#define TT2_NSE_RW_REG_Q0(chip, block, reg)   \
    ((reg) | \
    ((block) << 11) |   \
    (A20K_TARGET_NSE_REG << 19) |   \
    ((chip) << 21))

#define CFG_UDQ0_RW_NSE_SSR3    0x03    /* 3'b */
#define CFG_UDQ1_W_NSE_GMR3 0x03    /* 4'b */

#define TT2_NSE_WR_ARRAY_Q0(chip, target, adr)    \
    ((adr)                             |   \
    ((target) << 19)                   |   \
    ((chip) << 21)                     |   \
    (CFG_UDQ0_RW_NSE_SSR3 << 26))

#define TT2_NSE_WR_ARRAY_Q1(parallel)  \
    ((CFG_UDQ1_W_NSE_GMR3 << 20) | ((parallel) << 29))

#define TT2_NSE_RD_ARRAY_Q0(chip, target, adr)  \
    TT2_NSE_WR_ARRAY_Q0(chip, target, adr)

#define TT2_NSE_RD_ARRAY_Q1(parallel)  \
    TT2_NSE_WR_ARRAY_Q1(parallel)

/* Parity analysis */
#define SOC_ER_PARITY_TCAM_TYPE2_IERR_IBPE      0x001
#define SOC_ER_PARITY_TCAM_TYPE2_IERR_CFOERR    0x002
#define SOC_ER_PARITY_TCAM_TYPE2_IERR_RFOERR    0x004
#define SOC_ER_PARITY_TCAM_TYPE2_IERR_DFOERR    0x008
#define SOC_ER_PARITY_TCAM_TYPE2_IERR_CSERR     0x010
#define SOC_ER_PARITY_TCAM_TYPE2_IERR_ICERR     0x020

#define SOC_ER_PARITY_TCAM_TYPE2_DERR_SRPE      0x001
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_NRPE0     0x002
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_NRPE1     0x004
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_NWPE      0x008
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_NCPE      0x010
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_NFSE      0x020
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_NRTE      0x040
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_SCE       0x080
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_S0RPE0    0x100
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_S0RPE1    0x200
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_S1RPE0    0x400
#define SOC_ER_PARITY_TCAM_TYPE2_DERR_S1RPE1    0x800

#define SOC_ER_PARITY_TCAM_TYPE2_IERR_CONTEXT_MASK      8
#define SOC_ER_PARITY_TCAM_TYPE2_IERR_CONTEXT_SHIFT     0x7f
