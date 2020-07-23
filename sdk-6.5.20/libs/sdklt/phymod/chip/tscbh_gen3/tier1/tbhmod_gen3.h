/*----------------------------------------------------------------------
 * $Id: tbhmod_gen2.h,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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

#ifndef _tbhmod_gen3_H_
#define _tbhmod_gen3_H_

#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>

#include "tbhmod_gen3_spd_ctrl.h"

#define PHYMOD_ST phymod_access_t

#define TSCBH_GEN3_NOF_LANES_IN_CORE  8
#define TSCBH_GEN3_AUTONEG_SPEED_ID_COUNT 44

#define TBHMOD_GEN3_FEC_NOT_SUPRTD         0
#define TBHMOD_GEN3_FEC_SUPRTD_NOT_REQSTD  1
#define TBHMOD_GEN3_FEC_CL74_SUPRTD_REQSTD 4
#define TBHMOD_GEN3_FEC_CL91_SUPRTD_REQSTD 8

#define TBHMOD_GEN3_CL73_ABILITY_50G_KR1_CR1_POSITION    13
#define TBHMOD_GEN3_CL73_ABILITY_100G_KR2_CR2_POSITION   14
#define TBHMOD_GEN3_CL73_ABILITY_200G_KR4_CR4_POSITION   15

#define TBHMOD_GEN3_VCO_NONE         0x0
#define TBHMOD_GEN3_VCO_20G          0x1
#define TBHMOD_GEN3_VCO_25G          0x2
#define TBHMOD_GEN3_VCO_26G          0x4
#define TBHMOD_GEN3_VCO_INVALID      0x8

#define TSCBH_GEN3_HW_SPEED_ID_TABLE_SIZE   64
#define TSCBH_GEN3_HW_AM_TABLE_SIZE    64
#define TSCBH_GEN3_HW_UM_TABLE_SIZE    64


#define TSCBH_GEN3_ID0                     0x600d
#define TSCBH_GEN3_ID1                     0x8770
#define TSCBH_GEN3_PHY_ALL_LANES           0xff
#define TSCBH_GEN3_TX_TAP_NUM              12
#define TSCBH_GEN3_FORCED_SPEED_ID_OFFSET  56

#define TSCBH_GEN3_PMD_CRC_UCODE_VERIFY 1

#define TSCBH_GEN3_SYNCE_SDM_DIVISOR_20G_VCO        10560
#define TSCBH_GEN3_SYNCE_SDM_DIVISOR_25G_VCO        12800
#define TSCBH_GEN3_SYNCE_SDM_DIVISOR_25GP78125_VCO  13200
#define TSCBH_GEN3_SYNCE_SDM_DIVISOR_26G_VCO        13600

#define TSCBH_GEN3_FEC_OVERRIDE_BIT_SHIFT 0
#define TSCBH_GEN3_FEC_OVERRIDE_MASK  0x1
#define TSCBH_GEN3_PORT_AN_ENABLE_BIT_SHIFT 1
#define TSCBH_GEN3_PORT_AN_ENABLE_MASK  0x2
#define TSCBH_GEN3_PORT_ENABLE_BIT_SHIFT 2
#define TSCBH_GEN3_PORT_ENABLE_MASK  0x4



#define TSCBH_GEN3_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = TSCBH_GEN3_PHY_ALL_LANES; \
    }while(0)

/* So far 4 bits debug mask are used by TBHMOD */
#define TBHMOD_GEN3_DBG_MEM        (1L << 4) /* allocation/object */
#define TBHMOD_GEN3_DBG_REGACC     (1L << 3) /* Print all register accesses */
#define TBHMOD_GEN3_DBG_FUNCVALOUT (1L << 2) /* All values returned by Tier1*/
#define TBHMOD_GEN3_DBG_FUNCVALIN  (1L << 1) /* All values pumped into Tier1*/
#define TBHMOD_GEN3_DBG_FUNC       (1L << 0) /* Every time we enter a  Tier1*/

/* MACROs for AN PAGE definition*/
#define TBHMOD_GEN3_BRCM_OUI        0xAF7
#define TBHMOD_GEN3_BRCM_BAM_CODE   0x3
#define TBHMOD_GEN3_MSA_OUI         0x6A737D /* OUI defined for consortium 25G */
#define TBHMOD_GEN3_MSA_OUI_13to23  0x353
#define TBHMOD_GEN3_MSA_OUI_2to12   0x4DF
#define TBHMOD_GEN3_MSA_OUI_0to1    0x1


/* defines for the PMD over sample value */
#define TBHMOD_GEN3_OS_MODE_1                   0
#define TBHMOD_GEN3_OS_MODE_2                   1
#define TBHMOD_GEN3_OS_MODE_4                   2
#define TBHMOD_GEN3_OS_MODE_2p5                 3
#define TBHMOD_GEN3_OS_MODE_21p25               4
#define TBHMOD_GEN3_OS_MODE_16p5                8
#define TBHMOD_GEN3_OS_MODE_20p625              12


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
#define TBHMOD_GEN3_AN_BASE0_PAGE_PAUSE_MASK   0x3
#define TBHMOD_GEN3_AN_BASE0_PAGE_PAUSE_OFFSET 0xA

#define TBHMOD_GEN3_AN_BASE0_PAGE_NP_MASK   0x1
#define TBHMOD_GEN3_AN_BASE0_PAGE_NP_OFFSET 0xF

#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_10G_KR1_MASK   0x1
#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_10G_KR1_OFFSET 0x7

#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_40G_KR4_MASK   0x1
#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_40G_KR4_OFFSET 0x8

#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_40G_CR4_MASK   0x1
#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_40G_CR4_OFFSET 0x9

#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_100G_KR4_MASK   0x1
#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_100G_KR4_OFFSET 0xC

#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_100G_CR4_MASK   0x1
#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_100G_CR4_OFFSET 0xD

#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_25G_KRS1_CRS1_MASK   0x1
#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_25G_KRS1_CRS1_OFFSET 0xE

#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_25G_KR1_CR1_MASK   0x1
#define TBHMOD_GEN3_AN_BASE1_TECH_ABILITY_25G_KR1_CR1_OFFSET 0xF

#define TBHMOD_GEN3_AN_BASE2_TECH_ABILITY_50G_KR1_CR1_MASK   0x1
#define TBHMOD_GEN3_AN_BASE2_TECH_ABILITY_50G_KR1_CR1_OFFSET 0x2

#define TBHMOD_GEN3_AN_BASE2_TECH_ABILITY_100G_KR2_CR2_MASK   0x1
#define TBHMOD_GEN3_AN_BASE2_TECH_ABILITY_100G_KR2_CR2_OFFSET 0x3

#define TBHMOD_GEN3_AN_BASE2_TECH_ABILITY_200G_KR4_CR4_MASK   0x1
#define TBHMOD_GEN3_AN_BASE2_TECH_ABILITY_200G_KR4_CR4_OFFSET 0x4

#define TBHMOD_GEN3_AN_BASE2_25G_RS_FEC_ABILITY_REQ_MASK   0x1
#define TBHMOD_GEN3_AN_BASE2_25G_RS_FEC_ABILITY_REQ_OFFSET 0xC

#define TBHMOD_GEN3_AN_BASE2_25G_BASE_R_FEC_ABILITY_REQ_MASK   0x1
#define TBHMOD_GEN3_AN_BASE2_25G_BASE_R_FEC_ABILITY_REQ_OFFSET 0xD

#define TBHMOD_GEN3_AN_BASE2_CL74_ABILITY_REQ_SUP_MASK   0x3
#define TBHMOD_GEN3_AN_BASE2_CL74_ABILITY_REQ_SUP_OFFSET 0xE

/* Message Page definitions */

#define TBHMOD_GEN3_AN_MSG_PAGE1_OUI_13to23_MASK 0x7FF
#define TBHMOD_GEN3_AN_MSG_PAGE1_OUI_13to23_OFFSET 0x0

#define TBHMOD_GEN3_AN_MSG_PAGE2_OUI_2to12_MASK   0x7FF
#define TBHMOD_GEN3_AN_MSG_PAGE2_OUI_2to12_OFFSET 0x0

/* Unformatted Page definitions */
#define TBHMOD_GEN3_AN_UF_PAGE0_UD_0to8_MASK 0x1FF
#define TBHMOD_GEN3_AN_UF_PAGE0_UD_0to8_OFFSET 0x0

#define TBHMOD_GEN3_AN_UF_PAGE0_OUI_MASK   0x3
#define TBHMOD_GEN3_AN_UF_PAGE0_OUI_OFFSET 0x9

/* Unformatted Page for CL73BAM only*/
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_20G_KR1_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_20G_KR1_OFFSET 0x2

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_20G_CR1_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_20G_CR1_OFFSET 0x3

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_25G_KR1_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_25G_KR1_OFFSET 0x4

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_25G_CR1_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_25G_CR1_OFFSET 0x5

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_40G_KR2_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_40G_KR2_OFFSET 0x6

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_40G_CR2_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_40G_CR2_OFFSET 0x7

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_KR2_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_KR2_OFFSET 0x8

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_CR2_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_CR2_OFFSET 0x9

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_BRCM_FEC_544_CR2_KR2_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_BRCM_FEC_544_CR2_KR2_OFFSET 0xA

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_BRCM_NO_FEC_CR1_KR1_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_BRCM_NO_FEC_CR1_KR1_OFFSET 0xD

#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_BRCM_FEC_528_CR1_KR1_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_BAM_50G_BRCM_FEC_528_CR1_KR1_OFFSET 0xE

#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_200G_BRCM_NO_FEC_KR4_CR4_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_200G_BRCM_NO_FEC_KR4_CR4_OFFSET 0x3

#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_200G_BRCM_KR4_CR4_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_200G_BRCM_KR4_CR4_OFFSET 0x4

#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_100G_BRCM_KR4_CR4_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_100G_BRCM_KR4_CR4_OFFSET 0x5

#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_100G_BRCM_FEC_528_KR2_CR2_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_100G_BRCM_FEC_528_KR2_CR2_OFFSET 0x7

#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_CL91_REQ_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_CL91_REQ_OFFSET 0xB

#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_CL74_REQ_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_CL74_REQ_OFFSET 0xC

#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_100G_BRCM_NO_FEC_KR2_CR2_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_100G_BRCM_NO_FEC_KR2_CR2_OFFSET 0xD

#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_100G_BRCM_NO_FEC_X4_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_BAM_100G_BRCM_NO_FEC_X4_OFFSET 0xE

/* Unformatted Page for MSA only*/
#define TBHMOD_GEN3_AN_UF_PAGE1_MSA_25G_KR1_ABILITY_MASK   0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_MSA_25G_KR1_ABILITY_OFFSET 0x4

#define TBHMOD_GEN3_AN_UF_PAGE1_MSA_25G_CR1_ABILITY_MASK   0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_MSA_25G_CR1_ABILITY_OFFSET 0x5

#define TBHMOD_GEN3_AN_UF_PAGE1_MSA_50G_KR2_ABILITY_MASK   0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_MSA_50G_KR2_ABILITY_OFFSET 0x8

#define TBHMOD_GEN3_AN_UF_PAGE1_MSA_50G_CR2_ABILITY_MASK   0x1
#define TBHMOD_GEN3_AN_UF_PAGE1_MSA_50G_CR2_ABILITY_OFFSET 0x9

#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_400G_ABILITY_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_400G_ABILITY_OFFSET 0x2

#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_LF1_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_LF1_OFFSET 0x5

#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_LF2_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_LF2_OFFSET 0x6

#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_LF3_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_LF3_OFFSET 0x7

#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_CL91_SUPPORT_MASK   0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_CL91_SUPPORT_OFFSET 0x8

#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_CL74_SUPPORT_MASK   0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_CL74_SUPPORT_OFFSET 0x9

#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_CL91_REQ_MASK   0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_CL91_REQ_OFFSET 0xA

#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_CL74_REQ_MASK   0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_CL74_REQ_OFFSET 0xB

#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_LFR_MASK 0x1
#define TBHMOD_GEN3_AN_UF_PAGE2_MSA_LFR_OFFSET 0xC

/* UI macros definition. */
#define TBHMOD_GEN3_UI_20G_NRZ_OSX1 0x0C698000
#define TBHMOD_GEN3_UI_20G_NRZ_OSX2 0x18D30000
#define TBHMOD_GEN3_UI_25G_PAM4 0x04F70000
#define TBHMOD_GEN3_UI_25G_NRZ_OSX1 0x09EE0000
#define TBHMOD_GEN3_UI_25G_NRZ_OSX2P5 0x18D30000
#define TBHMOD_GEN3_UI_26G_PAM4 0x04D19E00
#define TBHMOD_GEN3_UI_26G_NRZ 0x09A33C00

/* FCLK period macros definition. */
#define TBHMOD_GEN3_FCLK_PERIOD_20G_DIV8 0x634c
#define TBHMOD_GEN3_FCLK_PERIOD_20G_DIV6 0x4a79
#define TBHMOD_GEN3_FCLK_PERIOD_25G 0x4f70
#define TBHMOD_GEN3_FCLK_PERIOD_26G 0x4d19

/* PMD datapath latency. From Blackhawk7 user spec datapath latency table. */
#define TBHMOD_GEN3_PMD_TX_DP_LATENCY_NRZ 167
#define TBHMOD_GEN3_PMD_TX_DP_LATENCY_PAM4 147
#define TBHMOD_GEN3_PMD_RX_DP_LATENCY_NRZ 125
#define TBHMOD_GEN3_PMD_RX_DP_LATENCY_PAM4 85

typedef enum {
    TBHMOD_GEN3_AN_MODE_CL73 = 0,
    TBHMOD_GEN3_AN_MODE_CL73_BAM,
    TBHMOD_GEN3_AN_MODE_NONE,
    TBHMOD_GEN3_AN_MODE_CL73_MSA,
    TBHMOD_GEN3_AN_MODE_MSA,
    TBHMOD_GEN3_AN_MODE_TYPE_COUNT
}tbhmod_gen3_an_type_t;

typedef enum {
    TBHMOD_GEN3_NO_PAUSE = 0,
    TBHMOD_GEN3_SYMM_PAUSE,
    TBHMOD_GEN3_ASYM_PAUSE,
    TBHMOD_GEN3_ASYM_SYMM_PAUSE,
    TBHMOD_GEN3_AN_PAUSE_COUNT
}tbhmod_gen3_an_pause_t;

typedef enum {
    TBHMOD_GEN3_DUAL_PORT  = 0,          /*!< Each channel is one logical port */
    TBHMOD_GEN3_SINGLE_PORT,             /*!< single port mode: 4 channels as one logical port */
    TBHMOD_GEN3_MULTI_MPP_PORT,          /*!< single port mode: 8 channels as one logical port */
    TBHMOD_PORT_MODE_ILLEGAL             /*!< Illegal value (enum boundary) */
} tbhmod_gen3_port_type_t;

typedef enum {
    TBHMOD_GEN3_REF_CLK_156P25MHZ    = 1,
    TBHMOD_GEN3_REF_CLK_312P5MHZ     = 2
} tbhmod_gen3_refclk_t;

typedef struct tbhmod_gen3_an_control_s {
    tbhmod_gen3_an_type_t an_type;
    uint16_t num_lane_adv;
    uint16_t enable;
} tbhmod_gen3_an_control_t;

typedef enum {
    TBHMOD_GEN3_SPD_ZERO            = 0   ,  /*!< Illegal value (enum boundary)   */
    TBHMOD_GEN3_SPD_10G_IEEE_KR1          ,  /*!< 10Gb IEEE KR1                   */
    TBHMOD_GEN3_SPD_20G_BRCM_CR1          ,  /*!< 20Gb BRCM 20G CR1               */
    TBHMOD_GEN3_SPD_20G_BRCM_KR1          ,  /*!< 20Gb BRCM 20G KR1               */
    TBHMOD_GEN3_SPD_25G_IEEE_KS1_CS1      ,  /*!< 25Gb IEEE KS1/CS1               */
    TBHMOD_GEN3_SPD_25G_IEEE_KR1_CR1      ,  /*!< 25Gb IEEE KR1/CR1               */
    TBHMOD_GEN3_SPD_25G_BRCM_CR1          ,  /*!< 25Gb BRCM CR1                   */
    TBHMOD_GEN3_SPD_25G_BRCM_KR1          ,  /*!< 25Gb BRCM KR1                   */
    TBHMOD_GEN3_SPD_25G_BRCM_NO_FEC_KR1_CR1      ,  /*!< 25Gb BRCM NO FEC  KR1/CR1       */
    TBHMOD_GEN3_SPD_25G_BRCM_FEC_528_KR1_CR1     ,  /*!< 25Gb BRCM RS FEC  KR1/CR1       */
    TBHMOD_GEN3_SPD_40G_BRCM_CR2          ,  /*!< 40Gb serial CR2                 */
    TBHMOD_GEN3_SPD_40G_BRCM_KR2          ,  /*!< 40Gb serial CR2                 */
    TBHMOD_GEN3_SPD_40G_IEEE_KR4          ,  /*!< 40Gb serial KR4                 */
    TBHMOD_GEN3_SPD_40G_IEEE_CR4          ,  /*!< 40Gb serial CR4                 */
    TBHMOD_GEN3_SPD_50G_IEEE_KR1_CR1      ,  /*!< 50Gb IEEE KR1/CR1               */
    TBHMOD_GEN3_SPD_50G_BRCM_FEC_528_CR1_KR1      ,  /*!< 50Gb BRCM KR1/CR1               */
    TBHMOD_GEN3_SPD_50G_BRCM_FEC_272_CR1_KR1      ,  /*!< 50Gb BRCM KR1/CR1               */
    TBHMOD_GEN3_SPD_50G_BRCM_CR2_KR2_NO_FEC       ,  /*!< 50Gb BRCM KR2/CR2               */
    TBHMOD_GEN3_SPD_50G_BRCM_CR2_KR2_RS_FEC       ,  /*!< 50Gb BRCM RS528 KR2/Cr2         */
    TBHMOD_GEN3_SPD_50G_BRCM_FEC_544_CR2_KR2      ,  /*!< 50Gb BRCM RS544 KR2/Cr2         */
    TBHMOD_GEN3_SPD_100G_IEEE_KR4         ,  /*!< 100Gb serial XFI FEC RS528      */
    TBHMOD_GEN3_SPD_100G_IEEE_CR4         ,  /*!< 100Gb serial XFI FEC RS528      */
    TBHMOD_GEN3_SPD_100G_BRCM_NO_FEC_X4   ,  /*!< 100Gb serial XFI                */
    TBHMOD_GEN3_SPD_100G_BRCM_KR4_CR4     ,  /*!< 100Gb serial XFI                */
    TBHMOD_GEN3_SPD_CL73_IEEE_20G         ,  /*!< 1G CL73 Auto-neg                */
    TBHMOD_GEN3_SPD_CL73_IEEE_25G         ,  /*!< 1G CL73 Auto-neg                */
    TBHMOD_GEN3_SPD_CL73_IEEE_26G         ,  /*!< 1G CL73 Auto-neg                */
    TBHMOD_GEN3_SPD_100G_IEEE_KR2_CR2     ,  /*!< 100Gb serial IEEE 100G KR2/CR2  */
    TBHMOD_GEN3_SPD_100G_BRCM_NO_FEC_KR2_CR2       ,  /*!< 100Gb serial BRCM NO FEC 100G KR2/CR2  */
    TBHMOD_GEN3_SPD_100G_BRCM_FEC_528_KR2_CR2      ,  /*!< 100Gb serial BRCM RS FEC 100G KR2/CR2  */
    TBHMOD_GEN3_SPD_100G_BRCM_FEC_272_CR2_KR2      ,  /*!< 100Gb serial BRCM FEC 272 100G KR2/CR2 */
    TBHMOD_GEN3_SPD_200G_IEEE_KR4_CR4      ,  /*!< 200G KR4/CR4                    */
    TBHMOD_GEN3_SPD_200G_BRCM_FEC_272_N4   ,  /*!< 200G KR4/CR4 BRCM FEC RS272 1xN */
    TBHMOD_GEN3_SPD_200G_BRCM_FEC_272_CR4_KR4           ,  /*!< 200G KR4/CR4 BRCM FEC RS272 2xN */
    TBHMOD_GEN3_SPD_200G_BRCM_FEC_544_CR8_KR8           ,  /*!< 200G KR8/CR8 BRCM FEC RS544 2xN */
    TBHMOD_GEN3_SPD_200G_BRCM_NO_FEC_KR4_CR4            ,  /*!< 200G KR4/CR4 with NO FEC  */
    TBHMOD_GEN3_SPD_200G_BRCM_KR4_CR4      ,  /*!< 200G KR4/CR4 with FEC544 1xN  */
    TBHMOD_GEN3_SPD_400G_IEEE_X8           ,  /*!< 400G X8 IEEE FEC RS544 2xN  */
    TBHMOD_GEN3_SPD_400G_BRCM_FEC_KR8_CR8               ,  /*!< 400G X8 IEEE FEC RS544 2xN  */
    TBHMOD_GEN3_SPD_400G_BRCM_FEC_272_N8                ,  /*!< 400G X8 IEEE FEC RS272 2xN  */
    TBHMOD_GEN3_SPD_CUSTOM_ENTRY_56                     ,  /*!< Custom Entry at 56          */
    TBHMOD_GEN3_SPD_CUSTOM_ENTRY_57                     ,  /*!< Custom Entry at 57          */
    TBHMOD_GEN3_SPD_CUSTOM_ENTRY_58                     ,  /*!< Custom Entry at 58          */
    TBHMOD_GEN3_SPD_CUSTOM_ENTRY_59                     ,  /*!< Custom Entry at 59          */
    TBHMOD_GEN3_SPD_CUSTOM_ENTRY_60                     ,  /*!< Custom Entry at 60          */
    TBHMOD_GEN3_SPD_CUSTOM_ENTRY_61                     ,  /*!< Custom Entry at 61          */
    TBHMOD_GEN3_SPD_CUSTOM_ENTRY_62                     ,  /*!< Custom Entry at 62          */
    TBHMOD_GEN3_SPD_CUSTOM_ENTRY_63                     ,  /*!< Custom Entry at 63          */
    TBHMOD_GEN3_SPD_150G_FEC_544_2XN_N3                 ,  /*!< RLM 150G N3 FEC RS544 2xN   */
    TBHMOD_GEN3_SPD_300G_FEC_544_2XN_N6                 ,  /*!< RLM 300G N6 FEC RS544 2xN   */
    TBHMOD_GEN3_SPD_350G_FEC_544_2XN_N7                 ,  /*!< RLM 350G N7 FEC RS544 2xN   */
    TBHMOD_GEN3_SPD_ILLEGAL                                /*!< Illegal value (enum boundary)*/
} tbhmod_gen3_spd_intfc_type_t;

typedef enum {
    TBHMOD_GEN3_PLL_MODE_DIV_ZERO = 0, /* Divide value to be determined by API. */
    TBHMOD_GEN3_PLL_MODE_DIV_66         =          (int)0x00000042, /* Divide by 66        */
    TBHMOD_GEN3_PLL_MODE_DIV_80         =          (int)0x00000050, /* Divide by 80        */
    TBHMOD_GEN3_PLL_MODE_DIV_82P5       =          (int)0x80000052, /* Divide by 82.5      */
    TBHMOD_GEN3_PLL_MODE_DIV_85         =          (int)0x00000055, /* Divide by 85        */
    TBHMOD_GEN3_PLL_MODE_DIV_132        =          (int)0x00000084, /* Divide by 132       */
    TBHMOD_GEN3_PLL_MODE_DIV_160        =          (int)0x000000A0, /* Divide by 160       */
    TBHMOD_GEN3_PLL_MODE_DIV_165        =          (int)0x000000A5, /* Divide by 165       */
    TBHMOD_GEN3_PLL_MODE_DIV_170        =          (int)0x000000AA  /* Divide by 170       */
} tbhmod_gen3_pll_mode_type;


typedef enum {
    TBHMOD_GEN3_PORT_MODE_DUAL  = 0,    /* dual port mode.  */
    TBHMOD_GEN3_PORT_MODE_SINGLE,       /* single port mode */
    TBHMOD_GEN3_PORT_MODE_MULTI_MPP,    /* 8 lane port      */
    TBHMOD_GEN3_PORT_MODE_COUNT
} tbhmod_gen3_port_mode_type;

typedef struct tbhmod_gen3_intr_status_s {
    /* Phymod interrupt type phymodIntrXXX. */
    phymod_interrupt_type_t type;
    /* 1-bit error if the intr type is ECC. */
    uint8_t is_1b_err;
    /* 2-bit error if the intr type is ECC. */
    uint8_t is_2b_err;
    /* Indicates the index or address for multi-bit ECC errors. */
    int err_addr;
    /* Indicates interrupt is set for non-ECC interrupt type. */
    uint8_t non_ecc_intr_set;
} tbhmod_gen3_intr_status_t;

extern int tbhmod_gen3_model_num_get(PHYMOD_ST* pc, uint32_t* model_num);
extern int tbhmod_gen3_speed_id_get(PHYMOD_ST* pc, int *speed_id);
extern int tbhmod_gen3_enable_get(PHYMOD_ST* pc, uint32_t* enable);
extern int tbhmod_gen3_disable_set(PHYMOD_ST* pc);
extern int tbhmod_gen3_enable_set(PHYMOD_ST* pc);
extern int tbhmod_gen3_set_an_timers(PHYMOD_ST* pc, phymod_ref_clk_t refclk, uint32_t *pam4_an_timer_value);
extern int tbhmod_gen3_get_pcs_latched_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int tbhmod_gen3_rsfec_cw_type(PHYMOD_ST* pc,int en,int fec_bypass_correction);
extern int tbhmod_gen3_osts_pipeline(PHYMOD_ST* pc, uint32_t en);
extern int tbhmod_gen3_osts_pipeline_get(PHYMOD_ST* pc, uint32_t* en);
extern int tbhmod_gen3_pcs_rx_lane_swap(PHYMOD_ST* pc, int rx_lane_swap);
extern int tbhmod_gen3_pcs_tx_lane_swap(PHYMOD_ST* pc, int tx_lane_swap);
extern int tbhmod_gen3_pcs_single_tx_lane_swap_set(PHYMOD_ST* pc, int tx_physical_lane, int tx_logical_lane);
extern int tbhmod_gen3_pcs_single_rx_lane_swap_set(PHYMOD_ST* pc, int rx_physical_lane, int rx_logical_lane);
extern int tbhmod_gen3_pcs_rx_scramble_idle_en(PHYMOD_ST* pc, int en);
extern int tbhmod_gen3_synce_mode_set(PHYMOD_ST* pc, int stage0_mode, int stage1_mode);
extern int tbhmod_gen3_synce_mode_get(PHYMOD_ST* pc, int* stage0_mode, int* stage1_mode);
extern int tbhmod_gen3_synce_clk_ctrl_set(PHYMOD_ST* pc, uint32_t val);
extern int tbhmod_gen3_synce_clk_ctrl_get(PHYMOD_ST* pc, uint32_t* val);
extern int tbhmod_gen3_resolved_port_mode_get(PHYMOD_ST* pc, uint32_t* port_mode);
extern int tbhmod_gen3_revid_get(PHYMOD_ST* pc, uint32_t* rev_id);
extern int tbhmod_gen3_fec_arch_decode_get(int fec_arch, phymod_fec_type_t* fec_type);
extern int tbhmod_gen3_fec_override_set(PHYMOD_ST* pc, uint32_t enable);
extern int tbhmod_gen3_fec_override_get(PHYMOD_ST* pc, uint32_t* enable);
extern int tbhmod_gen3_fec_correctable_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int tbhmod_gen3_fec_uncorrectable_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int tbhmod_gen3_pmd_reset_seq(PHYMOD_ST* pc);
extern int tbhmod_gen3_set_an_port_mode(PHYMOD_ST* pc, int starting_lane);
extern int tbhmod_gen3_update_port_mode(PHYMOD_ST *pc);
extern int tbhmod_gen3_refclk_set(PHYMOD_ST* pc, tbhmod_gen3_refclk_t ref_clk);
extern int tbhmod_gen3_refclk_get(PHYMOD_ST* pc, tbhmod_gen3_refclk_t* ref_clk);
extern int tbhmod_gen3_pmd_x4_reset(PHYMOD_ST* pc);
extern int tbhmod_gen3_set_sc_speed(PHYMOD_ST* pc, int mapped_speed, int set_sw_speed_change);
extern int tbhmod_gen3_autoneg_control(PHYMOD_ST* pc, tbhmod_gen3_an_control_t *an_control);
extern int tbhmod_gen3_autoneg_control_get(PHYMOD_ST* pc, tbhmod_gen3_an_control_t *an_control, int *an_complete);
extern int tbhmod_gen3_autoneg_status_get(PHYMOD_ST* pc, int *an_en, int *an_done);
extern int tbhmod_gen3_autoneg_fec_status_get(PHYMOD_ST* pc, uint8_t *fec_status);
extern int tbhmod_gen3_autoneg_ability_set(PHYMOD_ST* pc,
                               const phymod_autoneg_advert_abilities_t* autoneg_abilities);
extern int tbhmod_gen3_autoneg_ability_get(PHYMOD_ST* pc, phymod_autoneg_advert_abilities_t* autoneg_abilities);
extern int tbhmod_gen3_autoneg_remote_ability_get(PHYMOD_ST* pc, phymod_autoneg_advert_abilities_t *autoneg_abilities);
extern int tbhmod_gen3_pll_to_vco_get(tbhmod_gen3_refclk_t ref_clock, uint32_t pll, uint32_t *vco);
extern int tbhmod_gen3_pcs_reset_sw_war(const PHYMOD_ST *pc);
extern int tbhmod_gen3_plldiv_lkup_get(PHYMOD_ST* pc, int mapped_speed_id, tbhmod_gen3_refclk_t refclk,  uint32_t *plldiv);
extern int tbhmod_gen3_pmd_rx_lock_override_enable(PHYMOD_ST* pc, uint32_t enable);
extern int tbhmod_gen3_polling_for_sc_done(PHYMOD_ST* pc);
extern int tbhmod_gen3_read_sc_done(PHYMOD_ST* pc);
extern int tbhmod_gen3_read_sc_fsm_status(PHYMOD_ST* pc);
extern int tbhmod_gen3_pcs_clk_blk_en(const PHYMOD_ST* pc, uint32_t en);
extern int tbhmod_gen3_port_start_lane_get(PHYMOD_ST *pc, int *port_starting_lane, int *port_num_lane);
extern int tbhmod_gen3_fec_bypass_indication_set(PHYMOD_ST* pc, uint32_t rsfec_bypass_indication);
extern int tbhmod_gen3_fec_bypass_indication_get(PHYMOD_ST *pc, uint32_t *rsfec_bypass_indication);
extern int tbhmod_gen3_vco_to_pll_lkup(uint32_t vco, tbhmod_gen3_refclk_t refclk, uint32_t* pll_div);
extern int tbhmod_gen3_fec_cobra_enable(PHYMOD_ST *pc, uint32_t enable);
extern int tbhmod_gen3_pcs_ts_config(PHYMOD_ST *pc, int sfd, int rpm);
extern int tbhmod_gen3_pcs_rx_ts_en(PHYMOD_ST* pc, uint32_t en);
extern int tbhmod_gen3_pcs_rx_ts_en_get(PHYMOD_ST* pc, uint32_t* en);
extern int tbhmod_gen3_pcs_rx_deskew_en(PHYMOD_ST *pc, int en);
#ifdef SERDES_API_FLOATING_POINT
extern int tbhmod_gen3_set_pmd_timer_offset(PHYMOD_ST *pc, float ts_clk_period);
#endif
extern int tbhmod_gen3_set_fclk_period (PHYMOD_ST *pc, uint32_t vco, int clk4sync_div);
extern int tbhmod_gen3_pcs_set_1588_ui(PHYMOD_ST *pc, uint32_t vco, int os_mode, int pam4);
extern int tbhmod_gen3_tx_pmd_latency_get(PHYMOD_ST *pc, int *tx_latency);
extern int tbhmod_gen3_tx_pmd_latency_set(PHYMOD_ST *pc, uint32_t vco, int os_mode, int pam4);
extern int tbhmod_gen3_pcs_set_tx_lane_skew_capture(PHYMOD_ST *pc, int tx_skew_en);
extern int tbhmod_gen3_pcs_measure_tx_lane_skew (PHYMOD_ST *pc, uint32_t vco, int os_mode, int pam4, int normalize_to_latest, int *tx_max_skew);
extern int tbhmod_gen3_measure_n_normalize_tx_lane_skew(PHYMOD_ST *pc, uint32_t vco, int os_mode, int is_pam4, int fixed_lane, int *tx_max_skew);
extern int tbhmod_gen3_mod_rx_1588_tbl_val(PHYMOD_ST *pc, int bit_mux_mode, uint32_t vco, int os_mode, int is_pam4, int normalize_to_latest, uint32_t *rx_max_skew, uint32_t *rx_min_skew, uint32_t *skew_per_vl, uint32_t *rx_dsl_sel, uint32_t *rx_psll_sel);
extern int tbhmod_gen3_chk_rx_ts_deskew_valid(PHYMOD_ST *pc, int bit_mux_mode, int *rx_ts_deskew_valid);
extern int tbhmod_gen3_rsfec_symbol_error_counter_get(PHYMOD_ST* pc,
                                          int bit_mux_mode,
                                          int max_count,
                                          int* actual_count,
                                          uint32_t* symb_err_cnt);
extern int tbhmod_gen3_intr_status_get(PHYMOD_ST* pc, tbhmod_gen3_intr_status_t* intr_status);
extern int tbhmod_gen3_tx_ts_info_unpack_tx_ts_tbl_entry(uint32_t *tx_ts_tbl_entry, phymod_ts_fifo_status_t *tx_ts_info);
extern int tbhmod_gen3_rs_fec_hi_ser_get(PHYMOD_ST* pc, uint32_t* hi_ser_lh, uint32_t* hi_ser_live);
extern int tbhmod_gen3_pmd_osmode_set(PHYMOD_ST* pc, int mapped_speed_id, tbhmod_gen3_refclk_t refclk);
extern int tbhmod_gen3_port_an_mode_enable_set(PHYMOD_ST* pc,int enable);
extern int tbhmod_gen3_port_an_mode_enable_get(PHYMOD_ST* pc, int* enable);
extern int tbhmod_gen3_port_cl73_enable_set(PHYMOD_ST* pc, int enable);
extern int tbhmod_gen3_port_cl73_enable_get(PHYMOD_ST* pc, int* enable);
extern int tbhmod_gen3_port_enable_set(PHYMOD_ST* pc, int enable);
extern int tbhmod_gen3_port_enable_get(PHYMOD_ST* pc, int* enable);
extern int tbhmod_gen3_pmd_override_enable_set(PHYMOD_ST* pc, phymod_override_type_t pmd_override_type, uint32_t override_enable, uint32_t override_val);
extern int
tbhmod_gen3_interrupt_enable_set(PHYMOD_ST *pc,
                                 phymod_interrupt_type_t intr_type,
                                 uint32_t enable);
extern int
tbhmod_gen3_interrupt_enable_get(PHYMOD_ST *pc,
                                 phymod_interrupt_type_t intr_type,
                                 uint32_t *enable);
#endif  /*  _tbhmod_gen3_H_ */
