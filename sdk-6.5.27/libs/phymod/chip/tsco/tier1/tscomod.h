/*----------------------------------------------------------------------
 * $Id: tscomod.h,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators
 *----------------------------------------------------------------------*/
/*
 * 
 * $Copyright:
 * All Rights Reserved.$
 */

#ifndef _tscomod_H_
#define _tscomod_H_

#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>

#include "tscomod_spd_ctrl.h"

#define PHYMOD_ST phymod_access_t

#define TSCOMOD_NOF_LANES_IN_CORE  8
#define TSCOMOD_AUTONEG_SPEED_ID_COUNT 48

#define TSCOMOD_FEC_NOT_SUPRTD         0
#define TSCOMOD_FEC_SUPRTD_NOT_REQSTD  1
#define TSCOMOD_FEC_CL74_SUPRTD_REQSTD 4
#define TSCOMOD_FEC_CL91_SUPRTD_REQSTD 8

#define TSCOMOD_CL73_ABILITY_50G_KR1_CR1_POSITION    13
#define TSCOMOD_CL73_ABILITY_100G_KR2_CR2_POSITION   14
#define TSCOMOD_CL73_ABILITY_200G_KR4_CR4_POSITION   15

#define TSCOMOD_VCO_NONE         0x0
#define TSCOMOD_VCO_41G          0x1
#define TSCOMOD_VCO_51G          0x2
#define TSCOMOD_VCO_53G          0x4
#define TSCOMOD_VCO_INVALID      0x8

#define TSCOMOD_HW_SPEED_ID_TABLE_SIZE   64
#define TSCOMOD_HW_AM_TABLE_SIZE    64
#define TSCOMOD_HW_UM_TABLE_SIZE    64


#define TSCOMOD_ID0                     0x600d
#define TSCOMOD_ID1                     0x8770
#define TSCOMOD_PHY_ALL_LANES           0xff
#define TSCOMOD_TX_TAP_NUM              12
#define TSCOMOD_FORCED_SPEED_ID_OFFSET  56

#define TSCOMOD_PMD_CRC_UCODE_VERIFY 1

#define TSCOMOD_SYNCE_SDM_DIVISOR_10G_PER_LANE        5280
#define TSCOMOD_SYNCE_SDM_DIVISOR_20G_PER_LANE        10560
#define TSCOMOD_SYNCE_SDM_DIVISOR_25G_PER_LANE        13200
#define TSCOMOD_SYNCE_SDM_DIVISOR_51G_VCO_PAM4        13200
#define TSCOMOD_SYNCE_SDM_DIVISOR_53G_VCO_PAM4        13600

#define TSCOMOD_FEC_OVERRIDE_BIT_SHIFT 0
#define TSCOMOD_FEC_OVERRIDE_MASK  0x1
#define TSCOMOD_PORT_AN_ENABLE_BIT_SHIFT 1
#define TSCOMOD_PORT_AN_ENABLE_MASK  0x2
#define TSCOMOD_PORT_ENABLE_BIT_SHIFT 2
#define TSCOMOD_PORT_ENABLE_MASK  0x4

#define TSCOMOD_IEEE_CL22_REG_ADDR        0x0003
#define TSCOMOD_PCS_REG_START_ADDR        0x9000
#define TSCOMOD_PCS_ONE_COPY_REG          0x9270
#define TSCOMOD_PCS_FOUR_COPY_REG_BLOCK1  0xc010
#define TSCOMOD_PCS_FOUR_COPY_REG_BLOCK2  0xc150
#define TSCOMOD_PCS_FOUR_COPY_REG_BLOCK3  0xc170
#define TSCOMOD_PCS_FOUR_COPY_REG_BLOCK4  0xc210


#define TSCOMOD_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = TSCOMOD_PHY_ALL_LANES; \
    }while(0)

/* So far 4 bits debug mask are used by TSCOOD */
#define TSCOMOD_DBG_MEM        (1L << 4) /* allocation/object */
#define TSCOMOD_DBG_REGACC     (1L << 3) /* Print all register accesses */
#define TSCOMOD_DBG_FUNCVALOUT (1L << 2) /* All values returned by Tier1*/
#define TSCOMOD_DBG_FUNCVALIN  (1L << 1) /* All values pumped into Tier1*/
#define TSCOMOD_DBG_FUNC       (1L << 0) /* Every time we enter a  Tier1*/

/* MACROs for AN PAGE definition*/
#define TSCOMOD_BRCM_OUI        0xAF7
#define TSCOMOD_BRCM_BAM_CODE   0x3
#define TSCOMOD_MSA_OUI         0x6A737D /* OUI defined for consortium 25G */
#define TSCOMOD_MSA_OUI_13to23  0x353
#define TSCOMOD_MSA_OUI_2to12   0x4DF
#define TSCOMOD_MSA_OUI_0to1    0x1


/* defines for the PMD over sample value */
#define TSCOMOD_OS_MODE_1                   0x0
#define TSCOMOD_OS_MODE_2                   0x1
#define TSCOMOD_OS_MODE_4                   0x2
#define TSCOMOD_OS_MODE_5                   0x3
#define TSCOMOD_OS_MODE_8                   0x9
#define TSCOMOD_OS_MODE_16                  0x12
#define TSCOMOD_OS_MODE_32                  0x1a
#define TSCOMOD_OS_MODE_64                  0x22
#define TSCOMOD_OS_MODE_33                  0x11
#define TSCOMOD_OS_MODE_41p25               0x19
#define TSCOMOD_OS_MODE_42p5                0x21
#define TSCOMOD_OS_MODE_UNINITIALIZED       0x3f


/* Below macros are defined based on IEEE and MSA 25G spec
 * for detailed information Please refer to the
 * IEEE 802.3by section 73.6 Link codeword encoding
 * MSA Spec section 3.2.5 Auto-negotiation Figure 10
 */

/* 50GBASE_KR_CR, 100GBASE_KR2_CR2, 200GBASE_KR4_CR4
 * bit position is proposed in 802.3cb, but not finilized.
 * The corresponding Macro value may need revisit.
 */

/* Base page definitions */
#define TSCOMOD_AN_BASE0_PAGE_PAUSE_MASK   0x3
#define TSCOMOD_AN_BASE0_PAGE_PAUSE_OFFSET 0xA

#define TSCOMOD_AN_BASE0_PAGE_NP_MASK   0x1
#define TSCOMOD_AN_BASE0_PAGE_NP_OFFSET 0xF

#define TSCOMOD_AN_BASE1_TECH_ABILITY_10G_KR1_MASK   0x1
#define TSCOMOD_AN_BASE1_TECH_ABILITY_10G_KR1_OFFSET 0x7

#define TSCOMOD_AN_BASE1_TECH_ABILITY_40G_KR4_MASK   0x1
#define TSCOMOD_AN_BASE1_TECH_ABILITY_40G_KR4_OFFSET 0x8

#define TSCOMOD_AN_BASE1_TECH_ABILITY_40G_CR4_MASK   0x1
#define TSCOMOD_AN_BASE1_TECH_ABILITY_40G_CR4_OFFSET 0x9

#define TSCOMOD_AN_BASE1_TECH_ABILITY_100G_KR4_MASK   0x1
#define TSCOMOD_AN_BASE1_TECH_ABILITY_100G_KR4_OFFSET 0xC

#define TSCOMOD_AN_BASE1_TECH_ABILITY_100G_CR4_MASK   0x1
#define TSCOMOD_AN_BASE1_TECH_ABILITY_100G_CR4_OFFSET 0xD

#define TSCOMOD_AN_BASE1_TECH_ABILITY_25G_KRS1_CRS1_MASK   0x1
#define TSCOMOD_AN_BASE1_TECH_ABILITY_25G_KRS1_CRS1_OFFSET 0xE

#define TSCOMOD_AN_BASE1_TECH_ABILITY_25G_KR1_CR1_MASK   0x1
#define TSCOMOD_AN_BASE1_TECH_ABILITY_25G_KR1_CR1_OFFSET 0xF

#define TSCOMOD_AN_BASE2_TECH_ABILITY_50G_KR1_CR1_MASK   0x1
#define TSCOMOD_AN_BASE2_TECH_ABILITY_50G_KR1_CR1_OFFSET 0x2

#define TSCOMOD_AN_BASE2_TECH_ABILITY_100G_KR2_CR2_MASK   0x1
#define TSCOMOD_AN_BASE2_TECH_ABILITY_100G_KR2_CR2_OFFSET 0x3

#define TSCOMOD_AN_BASE2_TECH_ABILITY_200G_KR4_CR4_MASK   0x1
#define TSCOMOD_AN_BASE2_TECH_ABILITY_200G_KR4_CR4_OFFSET 0x4

#define TSCOMOD_AN_BASE2_TECH_ABILITY_100G_KR1_CR1_MASK   0x1
#define TSCOMOD_AN_BASE2_TECH_ABILITY_100G_KR1_CR1_OFFSET 0x5

#define TSCOMOD_AN_BASE2_TECH_ABILITY_200G_KR2_CR2_MASK   0x1
#define TSCOMOD_AN_BASE2_TECH_ABILITY_200G_KR2_CR2_OFFSET 0x6

#define TSCOMOD_AN_BASE2_TECH_ABILITY_400G_KR4_CR4_MASK   0x1
#define TSCOMOD_AN_BASE2_TECH_ABILITY_400G_KR4_CR4_OFFSET 0x7

#define TSCOMOD_AN_BASE2_25G_RS_FEC_ABILITY_REQ_MASK   0x1
#define TSCOMOD_AN_BASE2_25G_RS_FEC_ABILITY_REQ_OFFSET 0xC

#define TSCOMOD_AN_BASE2_25G_BASE_R_FEC_ABILITY_REQ_MASK   0x1
#define TSCOMOD_AN_BASE2_25G_BASE_R_FEC_ABILITY_REQ_OFFSET 0xD

#define TSCOMOD_AN_BASE2_CL74_ABILITY_REQ_SUP_MASK   0x3
#define TSCOMOD_AN_BASE2_CL74_ABILITY_REQ_SUP_OFFSET 0xE

#define TSCOMOD_AN_BASE3_RS_FEC_544_2XN_MASK   0x1
#define TSCOMOD_AN_BASE3_RS_FEC_544_2XN_OFFSET 0xb


/* Message Page definitions */

#define TSCOMOD_AN_MSG_PAGE1_OUI_13to23_MASK 0x7FF
#define TSCOMOD_AN_MSG_PAGE1_OUI_13to23_OFFSET 0x0

#define TSCOMOD_AN_MSG_PAGE2_OUI_2to12_MASK   0x7FF
#define TSCOMOD_AN_MSG_PAGE2_OUI_2to12_OFFSET 0x0

/* Unformatted Page definitions */
#define TSCOMOD_AN_UF_PAGE0_UD_0to8_MASK 0x1FF
#define TSCOMOD_AN_UF_PAGE0_UD_0to8_OFFSET 0x0

#define TSCOMOD_AN_UF_PAGE0_OUI_MASK   0x3
#define TSCOMOD_AN_UF_PAGE0_OUI_OFFSET 0x9

/* Unformatted Page for CL73BAM only*/
#define TSCOMOD_AN_UF_PAGE1_BAM_20G_KR1_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_20G_KR1_OFFSET 0x2

#define TSCOMOD_AN_UF_PAGE1_BAM_20G_CR1_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_20G_CR1_OFFSET 0x3

#define TSCOMOD_AN_UF_PAGE1_BAM_25G_KR1_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_25G_KR1_OFFSET 0x4

#define TSCOMOD_AN_UF_PAGE1_BAM_25G_CR1_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_25G_CR1_OFFSET 0x5

#define TSCOMOD_AN_UF_PAGE1_BAM_40G_KR2_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_40G_KR2_OFFSET 0x6

#define TSCOMOD_AN_UF_PAGE1_BAM_40G_CR2_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_40G_CR2_OFFSET 0x7

#define TSCOMOD_AN_UF_PAGE1_BAM_50G_KR2_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_50G_KR2_OFFSET 0x8

#define TSCOMOD_AN_UF_PAGE1_BAM_50G_CR2_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_50G_CR2_OFFSET 0x9

#define TSCOMOD_AN_UF_PAGE1_BAM_50G_BRCM_FEC_544_CR2_KR2_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_50G_BRCM_FEC_544_CR2_KR2_OFFSET 0xA

#define TSCOMOD_AN_UF_PAGE1_BAM_100G_BRCM_CR1_KR1_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_100G_BRCM_CR1_KR1_OFFSET 0xD

#define TSCOMOD_AN_UF_PAGE1_BAM_50G_BRCM_FEC_528_CR1_KR1_MASK 0x1
#define TSCOMOD_AN_UF_PAGE1_BAM_50G_BRCM_FEC_528_CR1_KR1_OFFSET 0xE

#define TSCOMOD_AN_UF_PAGE2_BAM_200G_BRCM_NO_FEC_KR4_CR4_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_BAM_200G_BRCM_NO_FEC_KR4_CR4_OFFSET 0x3

#define TSCOMOD_AN_UF_PAGE2_BAM_200G_BRCM_KR4_CR4_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_BAM_200G_BRCM_KR4_CR4_OFFSET 0x4

#define TSCOMOD_AN_UF_PAGE2_BAM_100G_BRCM_KR4_CR4_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_BAM_100G_BRCM_KR4_CR4_OFFSET 0x5

#define TSCOMOD_AN_UF_PAGE2_BAM_100G_BRCM_FEC_528_KR2_CR2_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_BAM_100G_BRCM_FEC_528_KR2_CR2_OFFSET 0x7

#define TSCOMOD_AN_UF_PAGE2_BAM_CL91_REQ_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_BAM_CL91_REQ_OFFSET 0xB

#define TSCOMOD_AN_UF_PAGE2_BAM_CL74_REQ_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_BAM_CL74_REQ_OFFSET 0xC

#define TSCOMOD_AN_UF_PAGE2_BAM_100G_BRCM_NO_FEC_KR2_CR2_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_BAM_100G_BRCM_NO_FEC_KR2_CR2_OFFSET 0xD

#define TSCOMOD_AN_UF_PAGE2_BAM_100G_BRCM_NO_FEC_X4_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_BAM_100G_BRCM_NO_FEC_X4_OFFSET 0xE

/* Unformatted Page for MSA only*/
#define TSCOMOD_AN_UF_PAGE1_MSA_25G_KR1_ABILITY_MASK   0x1
#define TSCOMOD_AN_UF_PAGE1_MSA_25G_KR1_ABILITY_OFFSET 0x4

#define TSCOMOD_AN_UF_PAGE1_MSA_25G_CR1_ABILITY_MASK   0x1
#define TSCOMOD_AN_UF_PAGE1_MSA_25G_CR1_ABILITY_OFFSET 0x5

#define TSCOMOD_AN_UF_PAGE1_MSA_50G_KR2_ABILITY_MASK   0x1
#define TSCOMOD_AN_UF_PAGE1_MSA_50G_KR2_ABILITY_OFFSET 0x8

#define TSCOMOD_AN_UF_PAGE1_MSA_50G_CR2_ABILITY_MASK   0x1
#define TSCOMOD_AN_UF_PAGE1_MSA_50G_CR2_ABILITY_OFFSET 0x9

#define TSCOMOD_AN_UF_PAGE2_MSA_400G_ABILITY_MASK   0x1
#define TSCOMOD_AN_UF_PAGE2_MSA_400G_ABILITY_OFFSET 0x2

#define TSCOMOD_AN_UF_PAGE2_MSA_LF1_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_MSA_LF1_OFFSET 0x5

#define TSCOMOD_AN_UF_PAGE2_MSA_LF2_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_MSA_LF2_OFFSET 0x6

#define TSCOMOD_AN_UF_PAGE2_MSA_LF3_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_MSA_LF3_OFFSET 0x7

#define TSCOMOD_AN_UF_PAGE2_MSA_CL91_SUPPORT_MASK   0x1
#define TSCOMOD_AN_UF_PAGE2_MSA_CL91_SUPPORT_OFFSET 0x8

#define TSCOMOD_AN_UF_PAGE2_MSA_CL74_SUPPORT_MASK   0x1
#define TSCOMOD_AN_UF_PAGE2_MSA_CL74_SUPPORT_OFFSET 0x9

#define TSCOMOD_AN_UF_PAGE2_MSA_CL91_REQ_MASK   0x1
#define TSCOMOD_AN_UF_PAGE2_MSA_CL91_REQ_OFFSET 0xA

#define TSCOMOD_AN_UF_PAGE2_MSA_CL74_REQ_MASK   0x1
#define TSCOMOD_AN_UF_PAGE2_MSA_CL74_REQ_OFFSET 0xB

#define TSCOMOD_AN_UF_PAGE2_MSA_LFR_MASK 0x1
#define TSCOMOD_AN_UF_PAGE2_MSA_LFR_OFFSET 0xC

/* UI macros definition. */
#define TSCOMOD_UI_41G_NRZ_OSX2         0x0C698000
#define TSCOMOD_UI_41G_NRZ_OSX4         0x18D30000

#define TSCOMOD_UI_51G_PAM4_OSX2        0x04F70000
#define TSCOMOD_UI_51G_PAM4_OSX1        0x027B8000
#define TSCOMOD_UI_51G_NRZ_OSX5         0x18D30000
#define TSCOMOD_UI_51G_NRZ_OSX2         0x09EE0000
#define TSCOMOD_UI_51G_NRZ_OSX1         0x04F70000

#define TSCOMOD_UI_53G_PAM4_OSX2        0x04D19E00
#define TSCOMOD_UI_53G_PAM4_OSX1        0x0268CF00
#define TSCOMOD_UI_53G_NRZ_OSX2         0x09A33C00
#define TSCOMOD_UI_53G_NRZ_OSX1         0x04D19E00

/* FCLK period macros definition. */
#define TSCOMOD_FCLK_PERIOD_41G_DIV8 0x634c
#define TSCOMOD_FCLK_PERIOD_41G_DIV6 0x4a79
#define TSCOMOD_FCLK_PERIOD_51G 0x4f70
#define TSCOMOD_FCLK_PERIOD_53G 0x4d19

/* PMD datapath latency. From Osprey user spec v.2.0  datapath latency table. */
#define TSCOMOD_PMD_TX_DP_LATENCY_NRZ                       344
#define TSCOMOD_PMD_TX_DP_LATENCY_PAM4                      344

#define TSCOMOD_PMD_RX_DP_LATENCY_NRZ_OS1                   126
#define TSCOMOD_PMD_RX_DP_LATENCY_NRZ_NON_OS1               106
#define TSCOMOD_PMD_RX_DP_LATENCY_PAM4_OS1                  126
#define TSCOMOD_PMD_RX_DP_LATENCY_PAM4_NON_OS1              86

typedef enum {
    TSCOMOD_AN_MODE_CL73 = 0,
    TSCOMOD_AN_MODE_CL73_BAM,
    TSCOMOD_AN_MODE_NONE,
    TSCOMOD_AN_MODE_CL73_MSA,
    TSCOMOD_AN_MODE_MSA,
    TSCOMOD_AN_MODE_TYPE_COUNT
}tscomod_an_type_t;

typedef enum {
    TSCOMOD_NO_PAUSE = 0,
    TSCOMOD_SYMM_PAUSE,
    TSCOMOD_ASYM_PAUSE,
    TSCOMOD_ASYM_SYMM_PAUSE,
    TSCOMOD_AN_PAUSE_COUNT
}tscomod_an_pause_t;

typedef enum {
    TSCOMOD_AN_TIMER_NONE = 0,
    TSCOMOD_AN_FAIL_INHIBIT_TIMER_LT_PAM4
}tscomod_an_timer_t;

typedef enum {
    TSCOMOD_DUAL_PORT  = 0,          /*!< Dual Ports in current MPP, each port can use 1 lane only */
    TSCOMOD_SINGLE_PORT,             /*!< Single Port in current MPP */
    TSCOMOD_MULTI_MPP_PORT,          /*!< Multi-MPP port, single port per PM, using both 'MPPs' */
    TBHMOD_PORT_MODE_ILLEGAL         /*!< Illegal value (enum boundary) */
} tscomod_port_type_t;

typedef enum {
    TSCOMOD_REF_CLK_156P25MHZ    = 1,
    TSCOMOD_REF_CLK_312P5MHZ     = 2,
    TSCOMOD_REF_CLK_COUNT
} tscomod_refclk_t;

typedef struct tscomod_an_control_s {
    tscomod_an_type_t an_type;
    uint16_t num_lane_adv;
    uint16_t enable;
} tscomod_an_control_t;

typedef enum {
    TSCOMOD_SPD_ZERO            = 0   ,  /*!< Illegal value (enum boundary)   */
    TSCOMOD_SPD_10G_IEEE_KR1          ,  /*!< 10Gb IEEE KR1                   */
    TSCOMOD_SPD_20G_BRCM_CR1          ,  /*!< 20Gb BRCM 20G CR1               */
    TSCOMOD_SPD_20G_BRCM_KR1          ,  /*!< 20Gb BRCM 20G KR1               */
    TSCOMOD_SPD_25G_IEEE_KS1_CS1      ,  /*!< 25Gb IEEE KS1/CS1               */
    TSCOMOD_SPD_25G_IEEE_KR1_CR1      ,  /*!< 25Gb IEEE KR1/CR1               */
    TSCOMOD_SPD_25G_BRCM_CR1          ,  /*!< 25Gb BRCM CR1                   */
    TSCOMOD_SPD_25G_BRCM_KR1          ,  /*!< 25Gb BRCM KR1                   */
    TSCOMOD_SPD_25G_BRCM_NO_FEC_KR1_CR1      ,  /*!< 25Gb BRCM NO FEC  KR1/CR1       */
    TSCOMOD_SPD_25G_BRCM_FEC_528_KR1_CR1     ,  /*!< 25Gb BRCM RS FEC  KR1/CR1       */
    TSCOMOD_SPD_40G_BRCM_CR2          ,  /*!< 40Gb serial CR2                 */
    TSCOMOD_SPD_40G_BRCM_KR2          ,  /*!< 40Gb serial CR2                 */
    TSCOMOD_SPD_40G_IEEE_KR4          ,  /*!< 40Gb serial KR4                 */
    TSCOMOD_SPD_40G_IEEE_CR4          ,  /*!< 40Gb serial CR4                 */
    TSCOMOD_SPD_50G_IEEE_KR1_CR1      ,  /*!< 50Gb IEEE KR1/CR1               */
    TSCOMOD_SPD_50G_BRCM_FEC_528_CR1_KR1      ,  /*!< 50Gb BRCM KR1/CR1               */
    TSCOMOD_SPD_50G_BRCM_FEC_272_CR1_KR1      ,  /*!< 50Gb BRCM KR1/CR1               */
    TSCOMOD_SPD_50G_BRCM_CR2_KR2_NO_FEC       ,  /*!< 50Gb BRCM KR2/CR2               */
    TSCOMOD_SPD_50G_BRCM_CR2_KR2_RS_FEC       ,  /*!< 50Gb BRCM RS528 KR2/Cr2         */
    TSCOMOD_SPD_50G_BRCM_FEC_544_CR2_KR2      ,  /*!< 50Gb BRCM RS544 KR2/Cr2         */
    TSCOMOD_SPD_100G_IEEE_KR4         ,  /*!< 100Gb serial XFI FEC RS528      */
    TSCOMOD_SPD_100G_IEEE_CR4         ,  /*!< 100Gb serial XFI FEC RS528      */
    TSCOMOD_SPD_100G_BRCM_NO_FEC_X4   ,  /*!< 100Gb serial XFI                */
    TSCOMOD_SPD_100G_BRCM_KR4_CR4     ,  /*!< 100Gb serial XFI                */
    TSCOMOD_SPD_CL73_IEEE_41G         ,  /*!< 1G CL73 Auto-neg                */
    TSCOMOD_SPD_CL73_IEEE_51G         ,  /*!< 1G CL73 Auto-neg                */
    TSCOMOD_SPD_CL73_IEEE_53G         ,  /*!< 1G CL73 Auto-neg                */
    TSCOMOD_SPD_100G_IEEE_KR2_CR2     ,  /*!< 100Gb serial IEEE 100G KR2/CR2  */
    TSCOMOD_SPD_100G_BRCM_NO_FEC_KR2_CR2       ,  /*!< 100Gb serial BRCM NO FEC 100G KR2/CR2  */
    TSCOMOD_SPD_100G_BRCM_FEC_528_KR2_CR2      ,  /*!< 100Gb serial BRCM RS FEC 100G KR2/CR2  */
    TSCOMOD_SPD_100G_BRCM_FEC_272_CR2_KR2      ,  /*!< 100Gb serial BRCM FEC 272 100G KR2/CR2 */
    TSCOMOD_SPD_100G_IEEE_KR1_CR1_OPT  ,  /*!< 100G PAM4 FEC                   */
    TSCOMOD_SPD_100G_IEEE_KR1_CR1      ,  /*!< 100G PAM4 FEC 544 2XN           */
    TSCOMOD_SPD_100G_BRCM_KR1_CR1      ,  /*!< 100G PAM4 FEC 544 1XN           */
    TSCOMOD_SPD_100G_BRCM_FEC_272_KR1_CR1      ,  /*!< 100G PAM4 FEC 272 1XN           */
    TSCOMOD_SPD_200G_IEEE_KR4_CR4      ,  /*!< 200G KR4/CR4                    */
    TSCOMOD_SPD_200G_BRCM_FEC_272_N4   ,  /*!< 200G KR4/CR4 BRCM FEC RS272 1xN */
    TSCOMOD_SPD_200G_BRCM_FEC_272_CR4_KR4           ,  /*!< 200G KR4/CR4 BRCM FEC RS272 2xN */
    TSCOMOD_SPD_200G_BRCM_FEC_544_CR8_KR8           ,  /*!< 200G KR8/CR8 BRCM FEC RS544 2xN */
    TSCOMOD_SPD_200G_BRCM_NO_FEC_KR4_CR4            ,  /*!< 200G KR4/CR4 with NO FEC  */
    TSCOMOD_SPD_200G_BRCM_KR4_CR4      ,  /*!< 200G KR4/CR4 with FEC544 1xN  */
    TSCOMOD_SPD_200G_IEEE_KR2_CR2      ,  /*!< 200G IEEE FEC 544 2XN         */
    TSCOMOD_SPD_200G_BRCM_FEC_272_KR2_CR2  ,  /*!< 200G FEC 272 2XN              */
    TSCOMOD_SPD_200G_BRCM_FEC_544_KR2_CR2  ,  /*!< 200G FEC 544 1XN              */
    TSCOMOD_SPD_200G_BRCM_FEC_272_N2       ,  /*!< 200G FEC 272 1XN              */
    TSCOMOD_SPD_400G_IEEE_X8           ,  /*!< 400G X8 IEEE FEC RS544 2xN    */
    TSCOMOD_SPD_400G_BRCM_FEC_KR8_CR8               ,  /*!< 400G X8 IEEE FEC RS544 2xN  */
    TSCOMOD_SPD_400G_BRCM_FEC_272_N8                ,  /*!< 400G X8 IEEE FEC RS272 2xN  */
    TSCOMOD_SPD_400G_IEEE_KR4_CR4                   ,  /*!< 400G X4 IEEE FEC RS544 2xN  */
    TSCOMOD_SPD_400G_BRCM_FEC_272_KR4_CR4           ,  /*!< 400G X4 BRCM FEC RS272 2xN  */
    TSCOMOD_SPD_CUSTOM_ENTRY_56                     ,  /*!< Custom Entry at 56          */
    TSCOMOD_SPD_CUSTOM_ENTRY_57                     ,  /*!< Custom Entry at 57          */
    TSCOMOD_SPD_CUSTOM_ENTRY_58                     ,  /*!< Custom Entry at 58          */
    TSCOMOD_SPD_CUSTOM_ENTRY_59                     ,  /*!< Custom Entry at 59          */
    TSCOMOD_SPD_CUSTOM_ENTRY_60                     ,  /*!< Custom Entry at 60          */
    TSCOMOD_SPD_CUSTOM_ENTRY_61                     ,  /*!< Custom Entry at 61          */
    TSCOMOD_SPD_CUSTOM_ENTRY_62                     ,  /*!< Custom Entry at 62          */
    TSCOMOD_SPD_CUSTOM_ENTRY_63                     ,  /*!< Custom Entry at 63          */
    TSCOMOD_SPD_150G_FEC_544_2XN_N3                 ,  /*!< RLM 150G N3 FEC RS544 2xN   */
    TSCOMOD_SPD_300G_FEC_544_2XN_N6                 ,  /*!< RLM 300G N6 FEC RS544 2xN   */
    TSCOMOD_SPD_350G_FEC_544_2XN_N7                 ,  /*!< RLM 350G N7 FEC RS544 2xN   */
    TSCOMOD_SPD_ILLEGAL                                /*!< Illegal value (enum boundary)*/
} tscomod_spd_intfc_type_t;

typedef enum {
    TSCOMOD_PLL_MODE_DIV_ZERO = 0, /* Divide value to be determined by API. */
    TSCOMOD_PLL_MODE_DIV_132        =          (int)0x00000084, /* Divide by 132       */
    TSCOMOD_PLL_MODE_DIV_165        =          (int)0x000000A5, /* Divide by 165       */
    TSCOMOD_PLL_MODE_DIV_170        =          (int)0x000000AA  /* Divide by 170       */
} tscomod_pll_mode_type;

typedef struct tscomod_intr_status_s {
    phymod_interrupt_type_t type;
    uint8_t is_1b_err;
    uint8_t is_2b_err;
    int err_addr;
    /* Indicates interrupt is set for non-ECC interrupt type. */
    uint8_t non_ecc_intr_set;
} tscomod_intr_status_t;

typedef enum {
    TSCOMOD_RS_FEC_CW_TYPE_544  = 0,        /* CW type 544  */
    TSCOMOD_RS_FEC_CW_TYPE_272,             /* CW type 272 */
    TSCOMOD_RS_FEC_CW_TYPE_COUNT
} tscomod_rs_fec_cw_type;

#define TSCOMOD_NOF_LANES_IN_PML        4
#define TSCOMOD_NOF_LANES_IN_MPP        2

#define TSCOMOD_PML_NUM(lane)           (lane / TSCOMOD_NOF_LANES_IN_PML)
#define TSCOMOD_PML_LANE(lane)          (lane % TSCOMOD_NOF_LANES_IN_PML)
#define TSCOMOD_PML_LANE_MASK(lane)     (0x01 << (TSCOMOD_PML_NUM(lane) * TSCOMOD_NOF_LANES_IN_PML))

#define TSCOMOD_MPP_NUM(lane)           ((lane % TSCOMOD_NOF_LANES_IN_PML)/TSCOMOD_NOF_LANES_IN_MPP)
#define TSCOMOD_MPP_LANE(lane)          (lane % TSCOMOD_NOF_LANES_IN_MPP)
#define TSCOMOD_MPP_LANE_MASK(lane)     (TSCOMOD_PML_LANE_MASK(lane)<<(TSCOMOD_MPP_NUM(lane)*TSCOMOD_NOF_LANES_IN_MPP))

/* Bit error mask to generate FEC error. */
typedef struct tscomod_fec_error_mask_s {
    uint32_t error_mask_bit_31_0;     /* Error mask bit 31-0. */
    uint32_t error_mask_bit_63_32;    /* Error mask bit 63-32. */
    uint16_t error_mask_bit_79_64;    /* Error mask bit 79-64. */
    uint32_t error_injection_freq;    /* Repeat the error injection every N-th codeword. */
} tscomod_fec_error_mask_t;

#define TSCO_TS_OFFSET_SOP          0
#define TSCO_TS_OFFSET_SFD          7
#define TSCO_TS_OFFSET_MAC_DA       8
#define TSCO_TS_RPM_OFFSET          4

extern int tscomod_model_num_get(PHYMOD_ST* pc, uint32_t* model_num);
extern int tscomod_speed_id_get(PHYMOD_ST* pc, int *speed_id);
extern int tscomod_enable_get(PHYMOD_ST* pc, uint32_t* enable);
extern int tscomod_disable_set(PHYMOD_ST* pc);
extern int tscomod_enable_set(PHYMOD_ST* pc);
extern int tscomod_set_an_timers(PHYMOD_ST* pc, phymod_ref_clk_t refclk,
                                 uint32_t *nrz_an_timer_value,
                                 uint32_t *pam4_an_timer_value);
extern int tscomod_get_pcs_latched_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int tscomod_rsfec_cw_type_set(PHYMOD_ST* pc, tscomod_rs_fec_cw_type cw_type, int fec_bypass_correction);
extern int tscomod_osts_pipeline(PHYMOD_ST* pc, uint32_t en);
extern int tscomod_osts_pipeline_get(PHYMOD_ST* pc, uint32_t* en);
extern int tscomod_pcs_rx_lane_swap(PHYMOD_ST* pc, int rx_lane_swap);
extern int tscomod_pcs_rx_mux_select(PHYMOD_ST* pc, int rx_lane_swap);
extern int tscomod_pcs_tx_m1_lane_swap(PHYMOD_ST* pc, int tx_lane_swap);
extern int tscomod_pcs_tx_m0_lane_swap(PHYMOD_ST* pc, int tx_lane_swap);
extern int tscomod_pcs_tx_mux_select(PHYMOD_ST* pc, int tx_lane_swap);
extern int tscomod_pcs_rx_scramble_idle_en(PHYMOD_ST* pc, int en);
extern int tscomod_synce_mode_set(PHYMOD_ST* pc, int stage0_mode, int stage1_mode);
extern int tscomod_synce_mode_get(PHYMOD_ST* pc, int* stage0_mode, int* stage1_mode);
extern int tscomod_synce_clk_ctrl_set(PHYMOD_ST* pc, uint32_t val);
extern int tscomod_synce_clk_ctrl_get(PHYMOD_ST* pc, uint32_t* val);
extern int tscomod_resolved_port_mode_get(PHYMOD_ST* pc, uint32_t* port_mode);
extern int tscomod_revid_get(PHYMOD_ST* pc, uint32_t* rev_id);
extern int tscomod_fec_arch_decode_get(int fec_arch, phymod_fec_type_t* fec_type);
extern int tscomod_fec_align_status_get(PHYMOD_ST* pc, uint32_t* fec_align_live);
extern int tscomod_fec_override_set(PHYMOD_ST* pc, uint32_t enable);
extern int tscomod_fec_override_get(PHYMOD_ST* pc, uint32_t* enable);
extern int tscomod_fec_correctable_counter_get(PHYMOD_ST* pc, phymod_phy_speed_config_t* speed_config, uint32_t* count);
extern int tscomod_fec_uncorrectable_counter_get(PHYMOD_ST* pc, phymod_phy_speed_config_t* speed_config,uint32_t* count);
extern int tscomod_pmd_reset_seq(PHYMOD_ST* pc);
extern int tscomod_set_an_port_mode(PHYMOD_ST* pc, int starting_lane);
extern int tscomod_update_port_mode(PHYMOD_ST *pc, uint32_t data_rate);
extern int tscomod_port_multi_mpp_mode_set(PHYMOD_ST *pc, uint32_t enable);
extern int tscomod_refclk_set(PHYMOD_ST* pc, tscomod_refclk_t ref_clk);
extern int tscomod_refclk_get(PHYMOD_ST* pc, tscomod_refclk_t* ref_clk);
extern int tscomod_pmd_x4_reset(PHYMOD_ST* pc);
extern int tscomod_set_sc_speed(PHYMOD_ST* pc, int mapped_speed, int set_sw_speed_change);
extern int tscomod_autoneg_control(PHYMOD_ST* pc, tscomod_an_control_t *an_control);
extern int tscomod_autoneg_control_get(PHYMOD_ST* pc, tscomod_an_control_t *an_control, int *an_complete);
extern int tscomod_autoneg_status_get(PHYMOD_ST* pc, int *an_en, int *an_done);
extern int tscomod_autoneg_fec_status_get(PHYMOD_ST* pc, uint8_t *fec_status);
extern int tscomod_autoneg_ability_set(PHYMOD_ST* pc,
                               const phymod_autoneg_advert_abilities_t* autoneg_abilities);
extern int tscomod_autoneg_ability_base_100g_fec_sel_set(PHYMOD_ST* pc,
                               const phymod_autoneg_advert_abilities_t* autoneg_abilities);
extern int tscomod_autoneg_ability_get(PHYMOD_ST* pc, phymod_autoneg_advert_abilities_t* autoneg_abilities);
extern int tscomod_autoneg_remote_ability_get(PHYMOD_ST* pc, phymod_autoneg_advert_abilities_t *autoneg_abilities);
extern int tscomod_pll_to_vco_get(tscomod_refclk_t ref_clock, uint32_t pll, uint32_t *vco);
extern int tscomod_pcs_reset_sw_war(const PHYMOD_ST *pc);
extern int tscomod_plldiv_lkup_get(PHYMOD_ST* pc, int mapped_speed_id, tscomod_refclk_t refclk,  uint32_t *plldiv);
extern int tscomod_pmd_rx_lock_override_enable(PHYMOD_ST* pc, uint32_t enable);
extern int tscomod_polling_for_sc_done(PHYMOD_ST* pc);
extern int tscomod_read_sc_done(PHYMOD_ST* pc);
extern int tscomod_read_sc_fsm_status(PHYMOD_ST* pc);
extern int tscomod_pcs_clk_blk_en(const PHYMOD_ST* pc, uint32_t en);
extern int tscomod_port_start_lane_get(PHYMOD_ST *pc, int *port_starting_lane, int *port_num_lane);
extern int tscomod_fec_bypass_indication_set(PHYMOD_ST* pc, uint32_t rsfec_bypass_indication);
extern int tscomod_fec_bypass_indication_get(PHYMOD_ST *pc, uint32_t *rsfec_bypass_indication);
extern int tscomod_vco_to_pll_lkup(uint32_t vco, tscomod_refclk_t refclk, uint32_t* pll_div);
extern int tscomod_fec_cobra_enable(PHYMOD_ST *pc, uint32_t enable);
extern int tscomod_pcs_ts_config(PHYMOD_ST *pc, int ts_offset, int base_addr_adjust);
extern int tscomod_pcs_rx_ts_en(PHYMOD_ST* pc, uint32_t en);
extern int tscomod_pcs_rx_ts_en_get(PHYMOD_ST* pc, uint32_t* en);
extern int tscomod_pcs_1588_ts_offset_set(PHYMOD_ST *pc, uint32_t ns_offset, uint32_t sub_ns_offset);
extern int tscomod_pcs_1588_ts_offset_get(PHYMOD_ST *pc, uint32_t *ns_offset, uint32_t *sub_ns_offset);
extern int tscomod_ts_offset_rx_set(PHYMOD_ST* pc, int tbl_ln, uint32_t *table);
extern int tscomod_pcs_rx_deskew_en(PHYMOD_ST *pc, int en);
#ifdef SERDES_API_FLOATING_POINT
extern int tscomod_set_pmd_timer_offset(PHYMOD_ST *pc, float ts_clk_period);
#endif
extern int tscomod_set_fclk_period (PHYMOD_ST *pc, uint32_t vco, int clk4sync_div);
extern int tscomod_pcs_set_1588_ui(PHYMOD_ST *pc, uint32_t vco, int os_mode, int pam4);
extern int tscomod_tx_pmd_latency_get(PHYMOD_ST *pc, int *tx_latency);
extern int tscomod_tx_pmd_latency_set(PHYMOD_ST *pc, uint32_t vco, int os_mode, int pam4);
extern int tscomod_pcs_set_tx_lane_skew_capture(PHYMOD_ST *pc, int tx_skew_en);
extern int tscomod_pcs_measure_tx_lane_skew (PHYMOD_ST *pc, uint32_t vco, int os_mode, int pam4,
                                             int pma_width_multiplier, int normalize_to_latest, int *tx_max_skew);
extern int tscomod_measure_n_normalize_tx_lane_skew(PHYMOD_ST *pc, uint32_t vco, int os_mode, int is_pam4, int fixed_lane, int *tx_max_skew);
extern int tscomod_mod_rx_1588_tbl_val(PHYMOD_ST *pc, int bit_mux_mode, uint32_t vco, int os_mode, int is_pam4, int normalize_to_latest, uint32_t *rx_max_skew, uint32_t *rx_min_skew, uint32_t *skew_per_vl, uint32_t *rx_dsl_sel, uint32_t *rx_psll_sel);
extern int tscomod_chk_rx_ts_deskew_valid(PHYMOD_ST *pc, int bit_mux_mode, int *rx_ts_deskew_valid);
extern int tscomod_rsfec_symbol_error_counter_get(PHYMOD_ST* pc,
                                          int bit_mux_mode,
                                          int max_count,
                                          int* actual_count,
                                          uint32_t* symb_err_cnt);
extern int tscomod_intr_status_get(PHYMOD_ST* pc, tscomod_intr_status_t* intr_status);
extern int tscomod_tx_ts_info_unpack_tx_ts_tbl_entry(uint32_t *tx_ts_tbl_entry, phymod_ts_fifo_status_t *tx_ts_info);
extern int tscomod_rs_fec_hi_ser_get(PHYMOD_ST* pc, uint32_t* hi_ser_lh, uint32_t* hi_ser_live);
extern int tscomod_pmd_osmode_set(PHYMOD_ST* pc, int mapped_speed_id, tscomod_refclk_t refclk);
extern int tscomod_port_an_mode_enable_set(PHYMOD_ST* pc,int enable);
extern int tscomod_port_an_mode_enable_get(PHYMOD_ST* pc, int* enable);
extern int tscomod_port_cl73_enable_set(PHYMOD_ST* pc, int enable);
extern int tscomod_port_cl73_enable_get(PHYMOD_ST* pc, int* enable);
extern int tscomod_port_enable_set(PHYMOD_ST* pc, int enable);
extern int tscomod_port_enable_get(PHYMOD_ST* pc, int* enable);
extern int tscomod_pcs_reg_num_copy_get(uint32_t reg_addr, int *num_copy);
extern int tscomod_pmd_override_enable_set(PHYMOD_ST* pc,
                                        phymod_override_type_t pmd_override_type,
                                        uint32_t override_enable,
                                        uint32_t override_val);
extern int tscomod_pmd_tx_pcs_delay_cnt_set(PHYMOD_ST* pc, uint32_t delay_cnt);
extern int
tscomod_interrupt_enable_set(PHYMOD_ST *pc,
                             phymod_interrupt_type_t intr_type,
                             uint32_t enable);
extern int
tscomod_interrupt_enable_get(PHYMOD_ST *pc,
                             phymod_interrupt_type_t intr_type,
                             uint32_t *enable);
extern int tscomod_1588_ts_valid_get(PHYMOD_ST* pc, uint16_t* ts_valid);
extern int tscomod_fec_error_inject_set(const PHYMOD_ST *pc,
                                        uint16_t error_control_map,
                                        tscomod_fec_error_mask_t bit_error_mask);
extern int tscomod_fec_error_inject_get(const PHYMOD_ST *pc,
                                        uint16_t* error_control_map,
                                        tscomod_fec_error_mask_t* bit_error_mask);

extern int tscomod_autoneg_ability_400g_8lane_get(PHYMOD_ST* pc, uint32_t *enabled);
extern int tscomod_autoneg_ability_400g_4lane_get(PHYMOD_ST* pc, uint32_t *enabled);

extern int tscomod_an_link_fail_inhibit_timer_set(PHYMOD_ST* pc,
                                                  tscomod_an_timer_t timer_type,
                                                  uint32_t period);
extern int tscomod_an_link_fail_inhibit_timer_get(PHYMOD_ST* pc,
                                                  tscomod_an_timer_t timer_type,
                                                  uint32_t* period);
extern int tscomod_fec_error_bits_counter_get (PHYMOD_ST* pc, phymod_phy_speed_config_t* speed_config,uint32_t* count);

#endif  /*  _tscomod_H_ */
