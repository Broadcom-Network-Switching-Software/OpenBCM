/*----------------------------------------------------------------------
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : viper_pmd_cfg_seq.h
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/
/*
 *  $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * 
*/


#ifndef VIPER_PMD_CFG_SEQ_H 
#define VIPER_PMD_CFG_SEQ_H

typedef enum {
    VIPER_PRBS_POLYNOMIAL_7 = 0,
    VIPER_PRBS_POLYNOMIAL_15,
    VIPER_PRBS_POLYNOMIAL_23,
    VIPER_PRBS_POLYNOMIAL_31,
    VIPER_PRBS_POLYNOMIAL_9,
    VIPER_PRBS_POLYNOMIAL_10,
    VIPER_PRBS_POLYNOMIAL_11,
    VIPER_PRBS_POLYNOMIAL_TYPE_COUNT
} viper_prbs_poly_t;

typedef enum {
    VIPER_CL37_SGMII_10M = 0,
    VIPER_CL37_SGMII_100M,
    VIPER_CL37_SGMII_1000M,
    VIPER_CL37_SGMII_SPEED_COUNT
}viper_cl37_sgmii_speed_t;

typedef enum {
    VIPER_NO_PAUSE = 0,
    VIPER_SYMM_PAUSE,
    VIPER_ASYM_PAUSE,
    VIPER_ASYM_SYMM_PAUSE,
    VIPER_AN_PAUSE_COUNT
}viper_an_pause_t;

typedef enum {
    VIPER_SPD_10_SGMII = 0           ,  /*!< 10Mb SGMII (serial)             */
    VIPER_SPD_100_SGMII              ,  /*!< 100Mb SGMII (serial)            */
    VIPER_SPD_1000_SGMII             ,  /*!< 1Gb SGMII (serial)              */
    VIPER_SPD_100_FX                 ,  /*!< 100Mb fiber                     */
    VIPER_SPD_1000_X1                ,  /*!< 1Gb   fiber                     */
    VIPER_SPD_2500                   ,  /*!< 2.5Gb  based on 1000BASE-X      */
    VIPER_SPD_10000_CX4              ,  /*!< 10Gb CX4                        */
    VIPER_SPD_10000                  ,  /*!< 10Gb XAUI                       */
    VIPER_SPD_ILLEGAL                   /*!< Illegal value (enum boundary)   */
} viper_spd_intfc_type_t;

typedef enum {
    VIPER_SPD_10M = 0,        
    VIPER_SPD_100M,         
    VIPER_SPD_1G,         
    VIPER_SPD_2p5G,         
    VIPER_SPD_5G_X4,         
    VIPER_SPD_6G_X4,         
    VIPER_SPD_10G_HiG,         
    VIPER_SPD_10G_CX4,         
    VIPER_SPD_12G_HiG,         
    VIPER_SPD_12p5G_X4,         
    VIPER_SPD_13G_X4,         
    VIPER_SPD_15G_X4,         
    VIPER_SPD_16G_X4         
} viper_actual_speed_id_t;

typedef enum {
    VIPER_DUPLEX_HALF = 0x0,
    VIPER_DUPLEX_FULL,
    VIPER_DUPLEX_COUNT
}viper_duplex_mode_t;

#define  VIPER_OP_XGXS               0x0
#define  VIPER_OP_XGXS_NCC           0x1
#define  VIPER_OP_INDLANE_OS8        0x4 
#define  VIPER_OP_INDLANE_OS5        0x5
#define  VIPER_OP_INDLANE_OS4        0x6
#define  VIPER_OP_PCI                0x7
#define  VIPER_OP_XGXS_NLQ           0x8
#define  VIPER_OP_XGXS_NLQNCC        0x9
#define  VIPER_OP_P_BYPASS           0xA 
#define  VIPER_OP_P_BYPASS_NDSK      0xB 
#define  VIPER_OP_COMBO_CORE_MODE    0xC 
#define  VIPER_OP_CLOCKS_OFF         0xF

/* XGXS Control Mode defines */
#define VIPER_XGXS_MODE_XGXS		0x00
#define VIPER_XGXS_MODE_XGXS_nCC	0x01
#define VIPER_XGXS_MODE_INDLANE_OS8	0x04
#define VIPER_XGXS_MODE_INDLANE_OS5	0x05
#define VIPER_XGXS_MODE_INDLANE_OS4	0x06
#define VIPER_XGXS_MODE_PCI		0x07
#define VIPER_XGXS_MODE_XGXS_nLQ	0x08
#define VIPER_XGXS_MODE_XGXS_nLQnCC	0x09
#define VIPER_XGXS_MODE_PBYPASS		0x0A
#define VIPER_XGXS_MODE_PBYPASS_nDSK	0x0B
#define VIPER_XGXS_MODE_COMBO_CORE	0x0C
#define VIPER_XGXS_MODE_CLOCKS_OFF	0x0F

/* DIGITAL5 MISC8 Mode defines */
#define VIPER_MISC8_OSDR_MODE_OSX2      0x01
#define VIPER_MISC8_OSDR_MODE_OSX4      0x02
#define VIPER_MISC8_OSDR_MODE_OSX5      0x03

/* DIGITAL MISC1 Register CLOCK / Speed Defines */
#define VIPER_MISC1_CLK_25M		0
#define VIPER_MISC1_CLK_100M		1
#define VIPER_MISC1_CLK_125M		2
#define VIPER_MISC1_CLK_156p25M		3
#define VIPER_MISC1_CLK_187p5M		4
#define VIPER_MISC1_CLK_161p25M		5
#define VIPER_MISC1_CLK_50M		6
#define VIPER_MISC1_CLK_106p25M		7

/* speed definitiosn if force_speed_b5 = 1 */
#define VIPER_MISC1_10G_DXGXS		0x00
#define VIPER_MISC1_10p5G_HiG_DXGXS	0x01
#define VIPER_MISC1_10p5G_DXGXS		0x02
#define VIPER_MISC1_12p773G_HiG_DXGXS	0x03
#define VIPER_MISC1_12p773G_DXGXS	0x04
#define VIPER_MISC1_10G_XFI		0x05
#define VIPER_MISC1_40G_X4		0x06
#define VIPER_MISC1_20G_HiG_DXGXS	0x07
#define VIPER_MISC1_20G_DXGXS		0x08
#define VIPER_MISC1_10G_SFI		0x09
#define VIPER_MISC1_31p5G		0x0a
#define VIPER_MISC1_32p7G		0x0b
#define VIPER_MISC1_20G_SCR		0x0c
#define VIPER_MISC1_10G_HiG_DXGXS_SCR	0x0d
#define VIPER_MISC1_10G_DXGXS_SCR	0x0e
#define VIPER_MISC1_12G_R2		0x0f
#define VIPER_MISC1_10G_X2		0x10
#define VIPER_MISC1_40G_KR4		0x11
#define VIPER_MISC1_40G_CR4		0x12
#define VIPER_MISC1_100G_CR10		0x13
#define VIPER_MISC1_5G_HiG_DXGXS	0x14
#define VIPER_MISC1_5G_DXGXS		0x15
#define VIPER_MISC1_15p75_HiG_DXGXS	0x16

/* speed definitiosn if force_speed_b5 = 0 */
#define VIPER_MISC1_2500BRCM_X1		0x10
#define VIPER_MISC1_5000BRCM_X4		0x11
#define VIPER_MISC1_6000BRCM_X4		0x12
#define VIPER_MISC1_10GHiGig_X4		0x13
#define VIPER_MISC1_10GBASE_CX4		0x14
#define VIPER_MISC1_12GHiGig_X4		0x15
#define VIPER_MISC1_12p5GhiGig_X4	0x16
#define VIPER_MISC1_13GHiGig_X4		0x17
#define VIPER_MISC1_15GHiGig_X4		0x18
#define VIPER_MISC1_16GHiGig_X4		0x19
#define VIPER_MISC1_5000BRCM_X1		0x1a
#define VIPER_MISC1_6363BRCM_X1		0x1b
#define VIPER_MISC1_20GHiGig_X4		0x1c
#define VIPER_MISC1_21GHiGig_X4		0x1d
#define VIPER_MISC1_25p45GHiGig_X4	0x1e
#define VIPER_MISC1_10G_HiG_DXGXS	0x1f

/* Adjusts the C in the RC delay in the frequency doubler*/
#define VIPER_EN_CAP_50fF     0x1
#define VIPER_EN_CAP_100fF    0x2
#define VIPER_EN_CAP_150fF    0x3
#define VIPER_EN_CAP_200fF    0x4
#define VIPER_EN_CAP_250fF    0x5
#define VIPER_EN_CAP_300fF    0x6
#define VIPER_EN_CAP_350fF    0x7

typedef enum {
    VIPER_AN_MODE_CL37 = 0,
    VIPER_AN_MODE_SGMII,
    VIPER_AN_MODE_NONE,
    VIPER_AN_MODE_TYPE_COUNT
}viper_an_mode_type_t;

typedef enum {
    VIPER_AN_PORT_MODE_QUAD = 0,    /* 4 port on the core */
    VIPER_AN_PORT_MODE_SINGLE,      /* 1 port using all 4 lane */
    VIPER_AN_PORT_MODE_TYPE_COUNT
}an_port_mode_type_t;


typedef struct viper_an_adv_ability_s{
  viper_cl37_sgmii_speed_t cl37_sgmii_speed;
  viper_an_pause_t an_pause;
}viper_an_adv_ability_t;

typedef struct viper_prbs_status_s {
    uint32_t prbs_lock;      
    uint32_t prbs_lock_loss; 
    uint32_t error_count; 
} viper_prbs_status_t;

#define viper_an_ability_t viper_an_adv_ability_t

#define MODEL_XGXS_10G_028 0x8
#define MODEL_SGMIIPLUS2X4 0xf

#define VIPER_CORE_IS_XGXS_10G_028(pa)  (viper_core_model_get(pa) ==  MODEL_XGXS_10G_028)
#define VIPER_CORE_IS_SGMIIPLUS2_X4(pa) (viper_core_model_get(pa) ==  MODEL_SGMIIPLUS2X4)

extern int viper_core_model_get(const PHYMOD_ST *pa);

extern int viper_pmd_force_ana_signal_detect(PHYMOD_ST *pa, int enable);   

extern int viper_prbs_lane_poly_get     (PHYMOD_ST *pa, uint8_t ln, viper_prbs_poly_t *prbs_poly);
extern int viper_prbs_lane_poly_set     (PHYMOD_ST *pa, uint8_t ln, viper_prbs_poly_t  prbs_poly);

extern int viper_prbs_lane_inv_data_get (PHYMOD_ST *pa, uint8_t ln, uint32_t  *inv_data);
extern int viper_prbs_lane_inv_data_set (PHYMOD_ST *pa, uint8_t ln, uint32_t   inv_data);
extern int viper_prbs_enable_get        (PHYMOD_ST *pa, uint8_t ln, uint32_t  *enable);
extern int viper_prbs_enable_set        (PHYMOD_ST *pa, uint8_t ln, uint32_t   enable);

extern int viper_tx_lane_reset          (PHYMOD_ST *pa, uint32_t  enable);   
extern int viper_rx_lane_reset          (PHYMOD_ST *pa, uint32_t  enable);   
extern int viper_mii_gloop_get          (PHYMOD_ST *pa, uint32_t *enable);   
extern int viper_mii_gloop_set          (PHYMOD_ST *pa, uint32_t  enable);   
extern int viper_mdio_reset             (PHYMOD_ST *pa, uint32_t  enable);   
extern int viper_pll_reset              (const PHYMOD_ST *pa, uint32_t  enable);   

extern int viper_sgmii_master_aneg_100M (const PHYMOD_ST *pa, uint8_t speed);
extern int viper_sgmii_slave_aneg_speed (const PHYMOD_ST *pa);
extern int viper_pll_disable_forced_10G (PHYMOD_ST *pa);
extern int viper_pll_enable_forced_10G  (PHYMOD_ST *pa);
extern int viper_fiber_force_10G_CX4    (PHYMOD_ST *pa);
extern int viper_fiber_force_100FX      (PHYMOD_ST *pa);
extern int viper_fiber_force_2p5G       (PHYMOD_ST *pa);
extern int viper_sgmii_force_100m       (PHYMOD_ST *pa);
extern int viper_sgmii_force_10m        (PHYMOD_ST *pa);
extern int viper_fiber_force_10G        (PHYMOD_ST *pa);
extern int viper_sgmii_aneg_10M         (const PHYMOD_ST *pa);
extern int viper_sgmii_force_1g         (PHYMOD_ST *pa);
extern int viper_fiber_force_1G         (PHYMOD_ST *pa);
extern int viper_fiber_AN_1G            (const PHYMOD_ST *pa);
extern int viper_lpi_enable_set         (const PHYMOD_ST *pa, uint32_t enable);
extern int viper_lpi_enable_get         (const PHYMOD_ST *pa, uint32_t *enable);
extern int viper_pll_disable            (const PHYMOD_ST *pa);
extern int viper_pll_enable             (const PHYMOD_ST *pa);
extern int viper_lane_reset             (const PHYMOD_ST *pa);

extern int viper_tx_pol_set             (PHYMOD_ST *pa, uint8_t val);
extern int viper_rx_pol_set             (PHYMOD_ST *pa, uint8_t val);

extern int viper_tx_pol_get             (PHYMOD_ST *pa, uint32_t *val);
extern int viper_rx_pol_get             (PHYMOD_ST *pa, uint32_t *val);

extern int viper_global_loopback_set    (const PHYMOD_ST *pa, uint8_t enable);
extern int viper_global_loopback_get    (const PHYMOD_ST *pa, uint32_t *lpbk); 
extern int viper_gloop10g_set           (const PHYMOD_ST *pa, uint8_t enable);
extern int viper_gloop10g_get           (const PHYMOD_ST *pa, uint32_t *lpbk); 
extern int viper_10g_global_loopback_set (const PHYMOD_ST *pa, uint8_t enable);
extern int viper_10g_global_loopback_get (const PHYMOD_ST *pa, uint32_t *enable);
extern int viper_get_link_status        (const PHYMOD_ST *pc, uint32_t *link);
extern int viper_get_link_status_10G    (const PHYMOD_ST *pc, uint32_t *link);
extern int viper_pmd_lock_get           (const PHYMOD_ST *pc, uint32_t* lockStatus);
extern int viper_prbs                   (PHYMOD_ST *pa, uint8_t prbs_mode);

extern int viper_remote_loopback_set    (const PHYMOD_ST *pa, viper_actual_speed_id_t speed_id, uint8_t enable);
extern int viper_pll_lock_speed_up      (PHYMOD_ST *pa);
extern int viper_forced_speed_up        (PHYMOD_ST *pa);
extern int viper_an_speed_up            (PHYMOD_ST *pa);

extern int viper_revid_read   (const PHYMOD_ST *pa, uint32_t *revid); 
extern int viper_autoneg_get  (const PHYMOD_ST *pa, phymod_autoneg_control_t  *an, uint32_t *an_done);
extern int viper_tx_lane_swap (PHYMOD_ST *pa, uint32_t ln_map);
extern int viper_rx_lane_swap (PHYMOD_ST *pa, uint32_t ln_map);
extern int viper_tx_lane_swap_get (const PHYMOD_ST *pa, uint32_t *tx_lane_map);
extern int viper_rx_lane_swap_get (const PHYMOD_ST *pa, uint32_t *tx_lane_map);

extern int viper_autoneg_status_get      (const PHYMOD_ST *pa, phymod_autoneg_status_t *status);
extern int viper_tsc_tx_pi_freq_override (const PHYMOD_ST *pa, uint8_t enable, int16_t freq_override_val); 
extern int viper_pll_sequencer_control(const PHYMOD_ST *pa, int enable);
extern int viper_tx_afe_post_set(const PHYMOD_ST *pc, uint8_t enable, uint8_t post);
extern int viper_sgmii_mode_get (const PHYMOD_ST *pa, uint16_t *sgmii_mode);
extern int viper_set_spd_intf(PHYMOD_ST *pa, viper_spd_intfc_type_t type);
extern int viper_actual_speed_get(const PHYMOD_ST* pa, viper_actual_speed_id_t *speed_id);
extern int viper_speed_id_interface_config_get(const phymod_phy_access_t* phy, int speed_id, phymod_phy_inf_config_t* config);
extern int viper_autoneg_set(const PHYMOD_ST* pa, const phymod_autoneg_control_t* an);
extern int viper_lane_power_set(PHYMOD_ST *pa, uint16_t data, uint16_t mask);
extern int viper_phy_enable_set(const PHYMOD_ST *pa, int enable, int combo);
extern int viper_phy_enable_get(const PHYMOD_ST *pa, int *enable);
extern int viper_xgxs_sel(const PHYMOD_ST *pa, int select);
extern int viper_prbs_status_get (PHYMOD_ST *pa, viper_prbs_status_t *status);
extern int viper_prbs_generator(PHYMOD_ST *pa, uint8_t os_mode);
extern int viper_speed_ctrl_reset(PHYMOD_ST *pa);
extern int viper_multimmds_set(PHYMOD_ST *pa, int enable);
extern int viper_autoneg_local_ability_set(const PHYMOD_ST *pa, viper_an_adv_ability_t *ability);
extern int viper_autoneg_local_ability_get(const PHYMOD_ST *pa, viper_an_adv_ability_t *ability);
extern int viper_autoneg_remote_ability_get(const PHYMOD_ST *pa, viper_an_adv_ability_t *ability);
extern int viper_remote_loopback_get (const PHYMOD_ST *pa, viper_actual_speed_id_t speed_id, uint32_t *lpbk);
extern int viper_10G_CX4_compliance_set(const PHYMOD_ST *pa, uint16_t value);
extern int viper_signal_detect_set (PHYMOD_ST *pa);
extern int viper_signal_invert_set (PHYMOD_ST *pa);
extern int viper_100FX_global_loopback_set (const PHYMOD_ST *pa, uint8_t enable);
extern int viper_100FX_global_loopback_get (const PHYMOD_ST *pa, uint32_t *lpbk);
extern int viper_fiber_force_100FX_get (const PHYMOD_ST *pa, uint32_t *fx_mode);
extern int viper_rx_large_swing_set(PHYMOD_ST *pa, uint32_t enable);
extern int viper_xgxs_sel_enable(PHYMOD_ST *pa, uint32_t enable);
extern int viper_diag_disp(PHYMOD_ST *pa, char* cmd_str);
extern int viper_diag_topology(PHYMOD_ST *pa);
extern int viper_diag_speed(PHYMOD_ST *pa);
extern int viper_diag_link_status(PHYMOD_ST *pa);
extern int viper_diag_autoneg(PHYMOD_ST *pa);
extern int viper_diag_tx_rx_control(PHYMOD_ST *pa);
extern int viper_tx_post_set(PHYMOD_ST *pa, uint32_t enable, uint32_t value);
extern int viper_tx_post_get(PHYMOD_ST *pa, uint32_t *enable, uint32_t *value);
extern int viper_tx_main_set(PHYMOD_ST *pa, uint32_t value);
extern int viper_tx_main_get(PHYMOD_ST *pa, uint32_t *value);
extern int viper_rx_pf_set(PHYMOD_ST *pa, uint32_t value);
extern int viper_rx_pf_get(PHYMOD_ST *pa, uint32_t *value);
extern int viper_cal_th_set(const PHYMOD_ST *pa, uint32_t value);
extern int _viper_speed_get(PHYMOD_ST* pa, viper_actual_speed_id_t *speed_id);
extern int viper_linkdown_transmit_set(const PHYMOD_ST *pa, uint32_t enable);
extern int viper_linkdown_transmit_get(const PHYMOD_ST *pa, uint32_t* enable);
extern int viper_duplex_set(const PHYMOD_ST *pa, viper_duplex_mode_t duplex);
extern int viper_duplex_get(const PHYMOD_ST *pa, viper_duplex_mode_t* duplex);
extern int viper_sigdet_threshold_set(const PHYMOD_ST *pa, uint32_t data);
extern int viper_phy_hw_reset(const PHYMOD_ST *pa);
extern int viper_signal_detect_get (PHYMOD_ST *pa, uint32_t *sd_en);
extern int viper_signal_invert_get (PHYMOD_ST *pa, uint32_t *invert_en);
extern int viper_rx_large_swing_get(PHYMOD_ST *pa, uint32_t *enable);
extern int viper_sigdet_threshold_get(const PHYMOD_ST *pa, uint32_t *data);
#endif /* VIPER_PMD_CFG_SEQ_H */
