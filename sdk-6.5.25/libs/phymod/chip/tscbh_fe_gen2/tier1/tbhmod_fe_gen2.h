/*----------------------------------------------------------------------
 * $Id: tbhmod_fe_gen2.h,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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

#ifndef _tbhmod_fe_gen2_H_
#define _tbhmod_fe_gen2_H_

#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include <tscbh/tier1/tbhmod.h>

#define PHYMOD_ST phymod_access_t


/* this file contains the initial code based on gen2,
   needs to be updated according to FE GEN2's changelist.
*/
#define TSCBH_FE_GEN2_NOF_LANES_IN_CORE       8
#define TSCBH_FE_GEN2_NRZ_MAX_PER_LANE_SPEED 28125
#define TSCBH_FE_GEN2_AUTONEG_SPEED_ID_COUNT 44
#define TSCBH_FE_GEN2_PLL_NUM_PER_CORE       2

/*! \enum tbhmod_spd_intfc_type_t 

All legal speed-interface combination are encapsulated in this enum

There are several speed and interface combinations allowed for a logical PHY
port. Names and speeds are self explanatory.

Speed and interface selection is combined because we don't want the speeds
to be incompatible with interface.

*/

typedef enum {
    TBHMOD_FE_GEN2_SPD_ZERO            = 0 ,  /*!< Illegal value (enum boundary)  */
    TBHMOD_FE_GEN2_SPD_10G_IEEE_KR1          ,  /*!< 10Gb IEEE KR1                   */
    TBHMOD_FE_GEN2_SPD_25G_IEEE_KS1_CS1    ,  /*!< 25Gb IEEE KS1/CS1              */
    TBHMOD_FE_GEN2_SPD_25G_IEEE_KR1_CR1    ,  /*!< 25Gb IEEE KR1/CR1              */
    TBHMOD_FE_GEN2_SPD_40G_IEEE_KR4        ,  /*!< 40Gb serial KR4                */
    TBHMOD_FE_GEN2_SPD_40G_IEEE_CR4        ,  /*!< 40Gb serial CR4                */
    TBHMOD_FE_GEN2_SPD_50G_IEEE_KR1_CR1    ,  /*!< 50Gb IEEE KR1/CR1              */
    TBHMOD_FE_GEN2_SPD_100G_IEEE_KR4       ,  /*!< 100Gb serial XFI FEC RS528     */
    TBHMOD_FE_GEN2_SPD_100G_IEEE_CR4       ,  /*!< 100Gb serial XFI FEC RS528     */
    TBHMOD_FE_GEN2_SPD_100G_IEEE_KR2_CR2   ,  /*!< 100Gb serial IEEE 100G KR2/CR2 */
    TBHMOD_FE_GEN2_SPD_200G_IEEE_KR4_CR4   ,  /*!< 200G KR4/CR4                   */
    TBHMOD_FE_GEN2_SPD_400G_IEEE_X8        ,  /*!< 400G X8 IEEE FEC RS544 2xN     */
    TBHMOD_FE_GEN2_SPD_CL73_IEEE_20GVCO    ,  /*!< 1G CL73 Auto-neg               */
    TBHMOD_FE_GEN2_SPD_CL73_IEEE_25GVCO    ,  /*!< 1G CL73 Auto-neg               */
    TBHMOD_FE_GEN2_SPD_CL73_IEEE_26GVCO    ,  /*!< 1G CL73 Auto-neg               */
    TBHMOD_FE_GEN2_SPD_20G_BRCM_CR1        ,  /*!< 20Gb BRCM 20G CR1              */
    TBHMOD_FE_GEN2_SPD_20G_BRCM_KR1        ,  /*!< 20Gb BRCM 20G KR1              */
    TBHMOD_FE_GEN2_SPD_25G_BRCM_CR1        ,  /*!< 25Gb BRCM CR1                  */
    TBHMOD_FE_GEN2_SPD_25G_BRCM_KR1        ,  /*!< 25Gb BRCM KR1                  */
    TBHMOD_FE_GEN2_SPD_40G_BRCM_CR2        ,  /*!< 40Gb serial CR2                */
    TBHMOD_FE_GEN2_SPD_40G_BRCM_KR2        ,  /*!< 40Gb serial CR2                */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_CR2_KR2_NO_FEC       ,  /*!< 50Gb BRCM KR2/CR2            */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_CR2_KR2_RS_FEC       ,  /*!< 50Gb BRCM RS528 KR2/Cr2      */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_CR4                  ,  /*!< 50Gb BRCM CR4                */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_KR4                  ,  /*!< 50Gb BRCM KR4                */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_544_CR2_KR2      ,  /*!< 50Gb BRCM RS544 KR2/CR2      */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_544_CR1_KR1      ,  /*!< 50Gb BRCM RS544 KR1/CR1      */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_NO_FEC_CR1_KR1       ,  /*!< 50Gb BRCM NO FEC KR1/CR1     */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_528_CR1_KR1      ,  /*!< 50Gb BRCM RS528 KR1/CR1      */
    TBHMOD_FE_GEN2_SPD_400G_BRCM_NO_FEC_KR8_CR8      ,  /*!< 400Gb BRCM NO FEC KR8/CR8    */
    TBHMOD_FE_GEN2_SPD_400G_BRCM_FEC_KR8_CR8         ,  /*!< 400G X8 IEEE FEC RS544 2xN   */
    TBHMOD_FE_GEN2_SPD_200G_BRCM_NO_FEC_KR4_CR4      ,  /*!< 200G KR4/CR4 with NO FEC     */
    TBHMOD_FE_GEN2_SPD_200G_BRCM_KR4_CR4             ,  /*!< 200G KR4/CR4 with FEC544 1xN */
    TBHMOD_FE_GEN2_SPD_100G_BRCM_KR4_CR4             ,  /*!< 100Gb serial XFI             */
    TBHMOD_FE_GEN2_SPD_100G_BRCM_FEC_528_KR2_CR2     ,  /*!< 100Gb serial BRCM RS FEC 100G KR2/CR2 */
    TBHMOD_FE_GEN2_SPD_100G_BRCM_NO_FEC_KR2_CR2      ,  /*!< 100Gb serial BRCM NO FEC 100G KR2/CR2 */
    TBHMOD_FE_GEN2_SPD_100G_BRCM_NO_FEC_X4           ,  /*!< 100Gb serial XFI                */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_272_CR1_KR1      ,  /*!< 50Gb BRCM KR1/CR1               */
    TBHMOD_FE_GEN2_SPD_100G_BRCM_FEC_272_CR2_KR2     ,  /*!< 100Gb serial BRCM FEC 272 100G KR2/CR2 */
    TBHMOD_FE_GEN2_SPD_200G_BRCM_FEC_272_CR4_KR4     ,  /*!< 200G KR4/CR4 BRCM FEC RS272 2xN */
    TBHMOD_FE_GEN2_SPD_25G_BRCM_NO_FEC_KR1_CR1       ,  /*!< 25Gb BRCM NO FEC  KR1/CR1       */
    TBHMOD_FE_GEN2_SPD_25G_BRCM_FEC_BASE_R_KR1_CR1   ,  /*!< 25Gb BRCM BASE R KR1/CR1        */
    TBHMOD_FE_GEN2_SPD_25G_BRCM_FEC_528_KR1_CR1      ,  /*!< 25Gb BRCM RS FEC  KR1/CR1       */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_CR2_KR2_RS_FEC_FLEXE ,  /*!< 50G KR2/CR2 BRCM FEC RS528 for FLEXE     */
    TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_544_CR2_KR2_FLEXE,  /*!< 50G KR2/CR2 BRCM FEC RS544 1xN for FLEXE */
    TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_56               ,  /*!< Custom Entry at 56          */
    TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_57               ,  /*!< Custom Entry at 57          */
    TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_58               ,  /*!< Custom Entry at 58          */
    TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_59               ,  /*!< Custom Entry at 59          */
    TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_60               ,  /*!< Custom Entry at 60          */
    TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_61               ,  /*!< Custom Entry at 61          */
    TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_62               ,  /*!< Custom Entry at 62          */
    TBHMOD_FE_GEN2_SPD_CUSTOM_ENTRY_63               ,  /*!< Custom Entry at 63          */
    TBHMOD_FE_GEN2_SPD_10G_IEEE_FEC_BASE_R_N1        ,  /*!< 10G IEEE FEC BASE_R         */
    TBHMOD_FE_GEN2_SPD_20G_BRCM_FEC_BASE_R_N1        ,  /*!< 20Gx1 BRCM FEC BASE_R       */
    TBHMOD_FE_GEN2_SPD_40G_IEEE_FEC_BASE_R_N4        ,  /*!< 40Gx4 IEEE FEC BASE_R       */
    TBHMOD_FE_GEN2_SPD_150G_BRCM_N3                 ,  /*!< RLM 150G N3 FEC RS544 2xN    */
    TBHMOD_FE_GEN2_SPD_200G_BRCM_FEC_272_N4         ,  /*!< 200G KR4/CR4 BRCM FEC RS272 1xN */
    TBHMOD_FE_GEN2_SPD_300G_BRCM_N6                 ,  /*!< RLM 300G N6 FEC RS544 2xN       */
    TBHMOD_FE_GEN2_SPD_350G_BRCM_N7                 ,  /*!< RLM 350G N7 FEC RS544 2xN       */
    TBHMOD_FE_GEN2_SPD_400G_BRCM_FEC_272_N8         ,  /*!< 400G X8 IEEE FEC RS272 2xN      */
    TBHMOD_FE_GEN2_SPD_200G_BRCM_FEC_544_CR8_KR8    ,  /*!< 200G KR8/CR8 BRCM FEC RS544 2xN */
    TBHMOD_FE_GEN2_SPD_12P12G_BRCM_KR1              ,  /*!< 12G KR1                         */
    TBHMOD_FE_GEN2_SPD_ILLEGAL                          /*!< Illegal value (enum boundary)  */
} tbhmod_fe_gen2_spd_intfc_type_t;

typedef enum {
    TBHMOD_FE_GEN2_PLL_MODE_DIV_ZERO = 0, /* Divide value to be determined by API. */
    TBHMOD_FE_GEN2_PLL_MODE_DIV_66   =     (int)0x00000042, /* Divide by 66        */
    TBHMOD_FE_GEN2_PLL_MODE_DIV_80   =     (int)0x00000050, /* Divide by 80        */
    TBHMOD_FE_GEN2_PLL_MODE_DIV_82P5 =     (int)0x80000052, /* Divide by 82.5      */
    TBHMOD_FE_GEN2_PLL_MODE_DIV_85   =     (int)0x00000055, /* Divide by 85        */
    TBHMOD_FE_GEN2_PLL_MODE_DIV_132  =     (int)0x00000084, /* Divide by 132       */
    TBHMOD_FE_GEN2_PLL_MODE_DIV_160  =     (int)0x000000A0, /* Divide by 160       */
    TBHMOD_FE_GEN2_PLL_MODE_DIV_165  =     (int)0x000000A5, /* Divide by 165       */
    TBHMOD_FE_GEN2_PLL_MODE_DIV_170  =     (int)0x000000AA  /* Divide by 170       */
} tbhmod_fe_gen2_pll_mode_type;

/* defines for the PMD over sample value */
#define TBHMOD_FE_GEN2_OS_MODE_1                   0
#define TBHMOD_FE_GEN2_OS_MODE_2                   1
#define TBHMOD_FE_GEN2_OS_MODE_4                   2
#define TBHMOD_FE_GEN2_OS_MODE_2p5                 3
#define TBHMOD_FE_GEN2_OS_MODE_21p25               4
#define TBHMOD_FE_GEN2_OS_MODE_16p5                8
#define TBHMOD_FE_GEN2_OS_MODE_20p625              12

#define TSCBH_FE_GEN2_FEC_OVERRIDE_BIT_SHIFT 0
#define TSCBH_FE_GEN2_FEC_OVERRIDE_MASK  0x1

extern int tbhmod_fe_gen2_pcs_ts_en(PHYMOD_ST* pc, int en, int sfd, int rpm);
extern int tbhmod_fe_gen2_pcs_rx_ts_en(PHYMOD_ST* pc, uint32_t en);
extern int tbhmod_fe_gen2_set_pmd_timer_offset(PHYMOD_ST* pc, int ts_clk_period);
extern int tbhmod_fe_gen2_pcs_set_1588_ui (PHYMOD_ST* pc, uint32_t vco, uint32_t vco1, int os_mode, int clk4sync_div, int pam4);
extern int tbhmod_fe_gen2_1588_pmd_latency(PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4);
extern int tbhmod_fe_gen2_update_tx_pmd_latency (PHYMOD_ST* pc, uint32_t latency_adj, int normalize_to_latest);
extern int tbhmod_fe_gen2_pcs_set_tx_lane_skew_capture (PHYMOD_ST* pc, int tx_skew_en);
extern int tbhmod_fe_gen2_pcs_measure_tx_lane_skew (PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4, int normalize_to_latest, int *tx_max_skew);
extern int tbhmod_fe_gen2_pcs_mod_rx_1588_tbl_val(PHYMOD_ST* pc, int an_en, int bit_mux_mode, uint32_t vco, int os_mode, int pam4, int tbl_ln, int normalize_to_latest, int rx_skew_adjust_enb, int *table , int *rx_max_skew); 
extern int tbhmod_fe_gen2_pcs_ts_deskew_valid(PHYMOD_ST* pc, int bit_mux_mode, int *rx_ts_deskew_valid);
extern int tbhmod_fe_gen2_ts_offset_rx_set(PHYMOD_ST* pc, uint32_t vco, int os_mode, int pam4, int tbl_ln, uint32_t *table);
extern int tbhmod_fe_gen2_pmd_osmode_set(PHYMOD_ST* pc, int mapped_speed_id, tbhmod_refclk_t refclk);
extern int tbhmod_fe_gen2_tx_ts_info_unpack_tx_ts_tbl_entry(uint32_t *tx_ts_tbl_entry, phymod_ts_fifo_status_t *tx_ts_info);
extern int tbhmod_fe_gen2_fec_override_set(PHYMOD_ST* pc, uint32_t enable);
extern int tbhmod_fe_gen2_fec_override_get(PHYMOD_ST* pc, uint32_t* enable);
extern int tbhmod_fe_gen2_fec_align_status_get(PHYMOD_ST* pc, uint32_t *fec_status_live);
extern int tbhmod_fe_gen2_pcs_reset_sw_war(const PHYMOD_ST* pc);
extern int tbhmod_fe_gen2_pcs_clk_blk_en(const PHYMOD_ST* pc, uint32_t en);
extern int tbhmod_fe_gen2_fec_three_cw_bad_state_get(PHYMOD_ST* pc, uint32_t *state);
extern int tbhmod_fe_gen2_1588_ts_valid_get(PHYMOD_ST* pc, uint16_t* ts_valid);
extern int tbhmod_fe_gen2_fec_corruption_period_set(PHYMOD_ST* pc, uint32_t period);
extern int tbhmod_fe_gen2_fec_corruption_period_get(PHYMOD_ST* pc, uint32_t *period);
extern int tbhmod_fe_gen2_pcs_tx_lane_swap_get(PHYMOD_ST *pc, uint32_t *tx_swap);
extern int tbhmod_fe_gen2_pcs_rx_lane_swap_get(PHYMOD_ST *pc, uint32_t *rx_swap);
extern int tbhmod_fe_gen2_fec_degraded_ctrl_enable_set(PHYMOD_ST *pc,
                                                       uint8_t enable);
extern int tbhmod_fe_gen2_fec_degraded_ctrl_enable_get(PHYMOD_ST *pc,
                                                       uint8_t *enable);
extern int tbhmod_fe_gen2_fec_degraded_ctrl_activate_threshold_set(PHYMOD_ST *pc,
                                                       uint32_t threshold);
extern int tbhmod_fe_gen2_fec_degraded_ctrl_activate_threshold_get(PHYMOD_ST *pc,
                                                       uint32_t *threshold);
extern int tbhmod_fe_gen2_fec_degraded_ctrl_deactivate_threshold_set(PHYMOD_ST *pc,
                                                       uint32_t threshold);
extern int tbhmod_fe_gen2_fec_degraded_ctrl_deactivate_threshold_get(PHYMOD_ST *pc,
                                                       uint32_t *threshold);
extern int tbhmod_fe_gen2_fec_degraded_ctrl_interval_set(PHYMOD_ST *pc,
                                                         uint32_t interval);
extern int tbhmod_fe_gen2_fec_degraded_ctrl_interval_get(PHYMOD_ST *pc,
                                                         uint32_t *interval);
extern int tbhmod_fe_gen2_fec_degraded_status_get(PHYMOD_ST *pc,
                                           phymod_fec_degraded_ser_status_t* status);
#endif  /*  _tbhmod_fe_gen2_H_ */
