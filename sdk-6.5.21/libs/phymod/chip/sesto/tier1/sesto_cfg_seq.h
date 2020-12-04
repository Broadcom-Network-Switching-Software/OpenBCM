/*----------------------------------------------------------------------
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : sesto_cfg_seq.h
 * Description: c functions implementing Tier1
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef SESTO_CFG_SEQ_H 
#define SESTO_CFG_SEQ_H
#include <phymod/phymod_acc.h>
#include "sesto_serdes/common/srds_api_err_code.h"

#define SESTO_DEV_PMA_PMD                  1
#define SESTO_DEV_AN                       7
#define SESTO_M0_EEPROM_PAGE_SIZE          64
#define SESTO_FW_DLOAD_RETRY_CNT           50000
#define SESTO_ENABLE                       1
#define SESTO_DISABLE                      0
#define SESTO_SPD_100G                     100000
#define SESTO_SPD_106G                     106000
#define SESTO_SPD_1G                       1000
#define SESTO_SPD_10G                      10000
#define SESTO_SPD_11G                      11000
#define SESTO_SPD_20G                      20000
#define SESTO_SPD_21G                      21000
#define SESTO_SPD_40G                      40000
#define SESTO_SPD_42G                      42000
#define SESTO_MAX_FALCON_LANE              4
#define SESTO_MAX_MERLIN_LANE              10
#define SESTO_DISABLE                      0
#define SESTO_ENABLE                       1
#define SESTO_M0_INTR_GRP                  0 
#define SESTO_MM_INTR_GRP                  1
#define SESTO_DF_INTR_GRP                  2 
#define SESTO_DEMUX_INTR_GRP               3 
#define SESTO_MUX_INTR_GRP                 4 
#define SESTO_FW_ALREADY_DOWNLOADED        0xFAD
#define SESTO_PMD_ID0                      0xAE02
#define SESTO_PMD_ID1                      0x5290
#define SESTO_CHIP_ID_82764                0x82764
#define SESTO_CHIP_ID_82792                0x82792
#define SESTO_CHIP_ID_82790                0x82790
#define SESTO_CHIP_ID_82796                0x82796
#define SESTO_MAX_INTRS_SUPPORT           8 

typedef enum {
	MSGOUT_DONTCARE = 0x0000,
	MSGOUT_GET_CNT	= 0x8888,
	MSGOUT_GET_LSB	= 0xABCD,
	MSGOUT_GET_MSB	= 0x4321,
	MSGOUT_GET_2B	= 0xEEEE,
	MSGOUT_GET_B	= 0xF00D,
	MSGOUT_ERR		= 0x0BAD,
	MSGOUT_NEXT		= 0x2222, 
	MSGOUT_NOT_DWNLD     = 0x0101, 
	MSGOUT_DWNLD_ALREADY = 0x0202,
	MSGOUT_DWNLD_DONE    = 0x0303,
	MSGOUT_PRGRM_DONE    = 0x0404
} SESTO_MSGOUT_E;

typedef enum {
    SESTO_MERLIN_CORE = 0,
    SESTO_FALCON_CORE = 1
}SESTO_CORE_TYPE;

typedef struct {
    uint16_t pass_thru;
    uint16_t gearbox_100g_inverse_mode;
    uint16_t BCM84793_capablity;
    SESTO_CORE_TYPE passthru_sys_side_core;
    uint16_t reserved; 
}SESTO_DEVICE_AUX_MODE_T;

typedef enum {
    SESTO_SLICE_UNICAST = 0,
    SESTO_SLICE_MULTICAST = 1,
    SESTO_SLICE_BROADCAST = 2,
    SESTO_CAST_INVALID = 3
}SESTO_SLICE_CAST_TYPE;

typedef enum {
    SESTO_IF_LINE = 0,
    SESTO_IF_SYS = 1
}SESTO_IF_SIDE;

typedef enum {
    SESTO_AN_NONE = 0,
    SESTO_AN_CL73 = 1,
    SESTO_AN_PROP = 2
}SESTO_AN_MODE;

typedef enum {
  SESTO_PLL_MODE_64  = 0x0, /*4'b0000*/ 
  SESTO_PLL_MODE_66  = 0x1, /*4'b0001*/ 
  SESTO_PLL_MODE_80  = 0x2, /*4'b0010*/ 
  SESTO_PLL_MODE_128 = 0x3, /*4'b0011*/ 
  SESTO_PLL_MODE_132 = 0x4, /*4'b0100*/ 
  SESTO_PLL_MODE_140 = 0x5, /*4'b0101*/ 
  SESTO_PLL_MODE_160 = 0x6, /*4'b0110*/ 
  SESTO_PLL_MODE_165 = 0x7, /*4'b0111*/ 
  SESTO_PLL_MODE_168 = 0x8, /*4'b1000*/ 
  SESTO_PLL_MODE_170 = 0x9, /*4'b1001*/ 
  SESTO_PLL_MODE_175 = 0xa, /*4'b1010*/ 
  SESTO_PLL_MODE_180 = 0xb, /*4'b1011*/ 
  SESTO_PLL_MODE_184 = 0xc, /*4'b1100*/ 
  SESTO_PLL_MODE_200 = 0xd, /*4'b1101*/ 
  SESTO_PLL_MODE_224 = 0xe, /*4'b1110*/ 
  SESTO_PLL_MODE_264 = 0xf, /*4'b1111*/ 
  /*Sesto Specific*/
  SESTO_PLL_MODE_33  = 0x10,
  SESTO_PLL_MODE_32  = 0x11,
  SESTO_PLL_MODE_16  = 0x12,
  SESTO_PLL_MODE_82P5= 0x13,
  SESTO_PLL_MODE_40  = 0x14,
  SESTO_PLL_MODE_70  = 0x15,
  SESTO_PLL_MODE_INVALID = -1
} SESTO_PLL_MODE_E;

typedef enum  {
  SESTO_INT_M0_SLV_SYSRESET_REQ = 0,
  SESTO_INT_M0_MST_SYSRESET_REQ,
  SESTO_INT_M0_SLV_LOCKUP,
  SESTO_INT_M0_MST_LOCKUP,
  SESTO_INT_M0_SLV_MISC_INTR,
  SESTO_INT_M0_MST_MISC_INTR,
  SESTO_INT_M0_SLV_MSGOUT_INTR,
  SESTO_INT_M0_MST_MSGOUT_INTR,
  SESTO_INT_M0_SLV_DED,
  SESTO_INT_M0_SLV_SEC,
  SESTO_INT_M0_MST_DED,
  SESTO_INT_M0_MST_SEC,
  SESTO_INT_MODULE2_INTRB_LOW,
  SESTO_INT_MODULE2_INTRB_HIGH,
  SESTO_INT_MODULE1_INTRB_LOW,
  SESTO_INT_MODULE1_INTRB_HIGH,
  SESTO_INT_MUX_PMD_LOCK_LOST,
  SESTO_INT_MUX_PMD_LOCK_FOUND,
  SESTO_INT_MUX_SIGDET_LOST,
  SESTO_INT_MUX_SIGDET_FOUND,
  SESTO_INT_PLL_10G_LOCK_LOST,
  SESTO_INT_PLL_10G_LOCK_FOUND,
  SESTO_INT_MERLIN_PMI_ARB_WDOG_EXP,
  SESTO_INT_MERLIN_PMI_M0_WDOG_EXP,
  SESTO_INT_DEMUX_PMD_LOCK_LOST,
  SESTO_INT_DEMUX_PMD_LOCK_FOUND,
  SESTO_INT_DEMUX_SIGDET_LOST,
  SESTO_INT_DEMUX_SIGDET_FOUND,
  SESTO_INT_PLL_25G_LOCK_LOST,
  SESTO_INT_PLL_25G_LOCK_FOUND,
  SESTO_INT_FALCON_PMD_ARB_WDOG_EXP,
  SESTO_INT_FALCON_PMD_M0_WDOG_EXP,
  SESTO_INT_CL91_RX_ALIGN_LOST,
  SESTO_INT_CL91_RX_ALIGN_FOUND,
  SESTO_INT_DEMUX_ONEG_INBAND_MSG_LOST,
  SESTO_INT_DEMUX_ONEG_INBAND_MSG_FOUND,
  SESTO_INT_DEMUX_PCS_MON_LOCK_LOST,
  SESTO_INT_DEMUX_PCS_MON_LOCK_FOUND,
  SESTO_INT_DEMUX_FIFOERR,
  SESTO_INT_DEMUX_CL73_AN_RESTARTED,
  SESTO_INT_DEMUX_CL73_AN_COMPLETE,
  SESTO_INT_CL91_TX_ALIGN_LOST,
  SESTO_INT_CL91_TX_ALIGN_FOUND,
  SESTO_INT_MUX_ONEG_INBAND_MSG_LOST,
  SESTO_INT_MUX_ONEG_INBAND_MSG_FOUND,
  SESTO_INT_MUX_PCS_MON_LOCK_LOST,
  SESTO_INT_MUX_PCS_MON_LOCK_FOUND,
  SESTO_INT_MUX_FIFOERR,
  SESTO_INT_MUX_CL73_AN_RESTARTED,
  SESTO_INT_MUX_CL73_AN_COMPLETE,
  SESTO_INT_MUX_CL73_AN_RESTARTED_P2,
  SESTO_INT_MUX_CL73_AN_RESTARTED_P1,
  SESTO_INT_MUX_CL73_AN_COMPLETE_P2,
  SESTO_INT_MUX_CL73_AN_COMPLETE_P1
} SESTO_INTR_TYPE_E;

#define SESTO_IF_ERR_RETURN(A)                                                  \
    do {                                                                        \
        if ((rv = (A)) != PHYMOD_E_NONE)                                        \
        {  goto ERR; }                                                          \
    } while (0)

#define SESTO_RETURN_WITH_ERR(A, B)                                             \
    do {                                                                        \
        PHYMOD_DEBUG_ERROR(B);                                                  \
        rv = (A);                                                               \
        { goto ERR; }                                                           \
    } while (0)

#define READ_SESTO_PMA_PMD_REG(acc, addr, value)                                 \
    do {                                                                         \
        uint32_t read_tmp;                                                       \
        SESTO_IF_ERR_RETURN                                                      \
          (PHYMOD_BUS_READ(acc, ((SESTO_DEV_PMA_PMD << 16) | addr), &read_tmp)); \
        value = (read_tmp & 0xffff);                                             \
      } while (0);

#define WRITE_SESTO_PMA_PMD_REG(acc, addr, value)                                \
        SESTO_IF_ERR_RETURN                                                      \
	        (PHYMOD_BUS_WRITE(acc, (SESTO_DEV_PMA_PMD << 16) | addr, value));

#define READ_SESTO_AN_REG(acc, addr, value)                                      \
    do {                                                                         \
        uint32_t an_read_tmp;                                                    \
        SESTO_IF_ERR_RETURN                                                      \
          (PHYMOD_BUS_READ(acc, ((SESTO_DEV_AN << 16) | addr), &an_read_tmp));   \
        value = (an_read_tmp & 0xffff);                                          \
      } while (0);

#define WRITE_SESTO_AN_REG(acc, addr, value)                                     \
        SESTO_IF_ERR_RETURN                                                      \
	        (PHYMOD_BUS_WRITE(acc, (SESTO_DEV_AN << 16) | addr, value));

#define SESTO_CHIP_FIELD_WRITE(acc, reg_name, field_name, value)                 \
    do {                                                                         \
        reg_name##_TYPE_T cfw_tmp;                                               \
        READ_SESTO_PMA_PMD_REG(acc, reg_name##_ADR, cfw_tmp.data);               \
        cfw_tmp.fields.field_name = value;                                       \
        WRITE_SESTO_PMA_PMD_REG(acc, reg_name##_ADR, cfw_tmp.data);              \
      } while (0);

#define SESTO_CHIP_FIELD_READ(acc, reg_name, field_name, value)                  \
    do {                                                                         \
        reg_name##_TYPE_T cfr_tmp;                                               \
        READ_SESTO_PMA_PMD_REG(acc, reg_name##_ADR, cfr_tmp.data);               \
        value = cfr_tmp.fields.field_name;                                       \
      } while (0);

#define SESTO_GET_CORE(_C, _LT, _ST)                                                                                 \
    {                                                                                                                \
        SESTO_DEVICE_AUX_MODE_T  *aux_mode_m;                                                                        \
        aux_mode_m = (SESTO_DEVICE_AUX_MODE_T*)_C.device_aux_modes;                                                  \
        if (aux_mode_m->pass_thru)    {                                                                              \
            _LT = (aux_mode_m->passthru_sys_side_core == SESTO_FALCON_CORE) ? SESTO_MERLIN_CORE : SESTO_FALCON_CORE; \
        } else if (_C.data_rate == SESTO_SPD_100G || _C.data_rate == SESTO_SPD_106G) {                               \
            _LT = (aux_mode_m->gearbox_100g_inverse_mode) ? SESTO_MERLIN_CORE : SESTO_FALCON_CORE ;                  \
        } else {                                                                                                     \
            _LT = (aux_mode_m->passthru_sys_side_core == SESTO_FALCON_CORE) ? SESTO_MERLIN_CORE : SESTO_FALCON_CORE; \
        }                                                                                                            \
        _ST = (_LT == SESTO_MERLIN_CORE) ? SESTO_FALCON_CORE : SESTO_MERLIN_CORE;                                    \
    }

#define SESTO_IS_MULTI_BROAD_CAST_SET(_C)   (_C == SESTO_SLICE_MULTICAST || _C == SESTO_SLICE_BROADCAST)
#define SESTO_GET_INTF_SIDE(phy, if_side)   (if_side = (phy->port_loc == phymodPortLocSys) ? SESTO_IF_SYS : SESTO_IF_LINE)
#define SESTO_GET_CORE_SLICE_INFO(_ACC, _IP, _CON, _C_V, _M_V)           \
    do {                                                                 \
        SESTO_CORE_TYPE line_ip_type, sys_ip_type;                       \
        uint16_t intf_side = 0;                                          \
        SESTO_GET_INTF_SIDE(_ACC, intf_side);                            \
        SESTO_GET_CORE(_CON, line_ip_type, sys_ip_type);                 \
        _IP = (intf_side == SESTO_IF_LINE) ? line_ip_type : sys_ip_type; \
        _sesto_lane_cast_get(_ACC, _IP, _C_V, _M_V);                     \
    }while(0);

#define SESTO_GET_IP(_ACC, _CON, _IP)                                    \
    do {                                                                 \
        SESTO_CORE_TYPE line_ip_type, sys_ip_type;                       \
        uint16_t intf_side = 0;                                          \
        SESTO_GET_INTF_SIDE(_ACC, intf_side);                            \
        SESTO_GET_CORE(_CON, line_ip_type, sys_ip_type);                 \
        _IP = (intf_side == SESTO_IF_LINE) ? line_ip_type : sys_ip_type; \
    }while(0);

#define SESTO_RESET_SLICE(_P, _DT)                                         \
    if (_DT == SESTO_DEV_PMA_PMD) {                                        \
        SESTO_IF_ERR_RETURN                                                      \
	        (PHYMOD_BUS_WRITE(_P, (SESTO_DEV_PMA_PMD << 16) | DEV1_SLICE_SLICE_ADR ,0x0)); \
    } else {                                                               \
        SESTO_IF_ERR_RETURN                                                      \
	        (PHYMOD_BUS_WRITE(_P, (SESTO_DEV_AN << 16) | DEV7_SLICE_SLICE_ADR , 0x0));  \
    }

#define SESTO_GET_PORT_FROM_MODE(_C, _PRT, aux_mode)                                                               \
    do {                                                                                                           \
        if (_C->data_rate == SESTO_SPD_100G || _C->data_rate == SESTO_SPD_106G) {                                  \
            _PRT = 1;                                                                                              \
        } else if ((_C->data_rate == SESTO_SPD_40G || _C->data_rate == SESTO_SPD_42G) && aux_mode->pass_thru) {    \
            _PRT = 1;                                                                                              \
        } else if ((_C->data_rate == SESTO_SPD_40G || _C->data_rate == SESTO_SPD_42G) && (!aux_mode->pass_thru)){  \
            _PRT = 2;                                                                                              \
        } else if (_C->data_rate == SESTO_SPD_20G || _C->data_rate == SESTO_SPD_21G) {                             \
            _PRT = 2;                                                                                              \
        } else if (_C->data_rate == SESTO_SPD_1G || _C->data_rate == SESTO_SPD_10G                                 \
                                                 || _C->data_rate == SESTO_SPD_11G) {                              \
            _PRT = 4;                                                                                              \
        }                                                                                                          \
    }while(0);

typedef enum {
    SESTO_CL73_NO_TECH = 0,
    SESTO_CL73_1000BASE_KX = 0x1,
    SESTO_CL73_10GBASE_KX4 = 0x2,
    SESTO_CL73_10GBASE_KR = 0x4,
    SESTO_CL73_40GBASE_KR4 = 0x8,
    SESTO_CL73_40GBASE_CR4 = 0x10,
    SESTO_CL73_100GBASE_CR10 = 0x20,
    SESTO_CL73_100GBASE_KP4 = 0x40,
    SESTO_CL73_100GBASE_KR4 = 0x80,
    SESTO_CL73_100GBASE_CR4 = 0x100
} SESTO_CL73_SPEED_T;

typedef enum {
    SESTO_NO_PAUSE = 0,
    SESTO_ASYM_PAUSE,
    SESTO_SYMM_PAUSE,
    SESTO_ASYM_SYMM_PAUSE,
    SESTO_AN_PAUSE_COUNT
} SESTO_AN_PAUSE_T;

/* Module controller I2C master commands */
typedef enum {
  SESTO_FLUSH   = 0,
  SESTO_RANDOM_ADDRESS_READ,
  SESTO_CURRENT_ADDRESS_READ,
  SESTO_WRITE
} SESTO_I2CM_CMD_E;

typedef struct SESTO_AN_ADV_ABILITY_S{
  SESTO_CL73_SPEED_T an_base_speed;
  SESTO_AN_PAUSE_T an_pause;
  uint16_t an_fec;
  uint16_t an_cl72;
} sesto_an_adv_ability_t;

typedef struct sesto_an_ability_s {
  sesto_an_adv_ability_t cl73_adv; /*includes cl73 related */
  uint32_t an_master_lane; /* Master lane belongs to port. For 10 lane port[0-9], for 4 lane port [0-3], for 2 lane port [0-1]*/
} sesto_an_ability_t;

#define SESTO_FALCON 
#define SESTO_MERLIN 

int sesto_get_chipid(const phymod_access_t *pa, uint32_t *chipid, uint32_t *rev);

int _sesto_wait_mst_msgout(const phymod_access_t *pa,
                            SESTO_MSGOUT_E exp_message,
                            int poll_time);

int _sesto_core_init(const phymod_core_access_t* core,
                     const phymod_core_init_config_t* init_config);

int _sesto_set_slice_reg(const phymod_access_t* pm_acc, SESTO_SLICE_CAST_TYPE cast_type,
                          uint16_t ip, uint16_t dev_type, uint16_t mcast_val, uint16_t lane);
 
void _sesto_lane_cast_get(const phymod_phy_access_t *phy, SESTO_CORE_TYPE ip,
                          uint16_t *cast_type, uint16_t *mcast_val); 

int _sesto_phy_interface_config_get(const phymod_phy_access_t *phy,
                                   uint32_t flags, phymod_phy_inf_config_t *config);

int _sesto_rx_pmd_lock_get(const phymod_phy_access_t *phy, uint32_t* rx_pmd_locked);

int _sesto_config_dut_mode_reg(const phymod_phy_access_t *phy,
                               const phymod_phy_inf_config_t* config);

int _sesto_udms_config(const phymod_phy_access_t *phy,
                       const phymod_phy_inf_config_t* config,
                       SESTO_AN_MODE mode);

int _sesto_phy_interface_config_set(const phymod_phy_access_t *phy,
                                uint32_t flags, const phymod_phy_inf_config_t* config);

int sesto_is_lane_mask_set(const phymod_phy_access_t *phy, uint16_t prt,
                           const phymod_phy_inf_config_t *config);

int _sesto_config_dut_mode_reg(const phymod_phy_access_t *phy,
                               const phymod_phy_inf_config_t* config);

int _sesto_udms_config(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config,
                        SESTO_AN_MODE mode);

int _sesto_interface_set(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config);

int _sesto_phy_interface_config_set(const phymod_phy_access_t* pa, uint32_t flags,
                                    const phymod_phy_inf_config_t* config);

int _sesto_phy_interface_config_get(const phymod_phy_access_t *phy, uint32_t flags,
                                    phymod_phy_inf_config_t *config);

int _sesto_phy_pcs_link_get(const phymod_phy_access_t *phy, uint32_t *pcs_link);

int _sesto_configure_ref_clock(const phymod_access_t *pa, phymod_ref_clk_t ref_clk);

int _sesto_get_pll_modes(phymod_ref_clk_t ref_clk, const phymod_phy_inf_config_t *config, 
                         uint16_t *fal_pll_mode, uint16_t *mer_pll_mode);

int _sesto_map_mer_pll_div(SESTO_PLL_MODE_E pll_mode);

int _sesto_config_pll_div(const phymod_access_t *pa, const phymod_phy_inf_config_t *config);

int _sesto_core_dp_rstb(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t cfg);

int _sesto_core_reset_set(const phymod_access_t *pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction);

int _sesto_get_pcs_link_status(const phymod_phy_access_t *phy, uint32_t *link_sts, uint32_t* lock_lost_lh, uint32_t* error_count);

int _sesto_pcs_link_monitor_enable_set(const phymod_phy_access_t *phy, uint16_t en_dis);

int _sesto_pcs_link_monitor_enable_get(const phymod_phy_access_t *phy, uint32_t *get_pcs);

int _sesto_fw_enable(const phymod_access_t *pa, uint16_t en_dis) ;

int _sesto_get_ieee_intf(const phymod_phy_access_t *phy, uint16_t ip, const phymod_phy_inf_config_t *config, uint16_t *intf);

int _sesto_set_ieee_intf(const phymod_phy_access_t *phy, uint16_t intf, const phymod_phy_inf_config_t *config, uint16_t ip);

int _sesto_loopback_set(const phymod_phy_access_t *phy, phymod_loopback_mode_t loopback, uint32_t enable);

int _sesto_loopback_get(const phymod_phy_access_t *phy, phymod_loopback_mode_t loopback, uint32_t *enable);

int _sesto_tx_rx_polarity_set(const phymod_phy_access_t *phy, uint32_t tx_polarity, uint32_t rx_polarity);

int _sesto_tx_rx_polarity_get(const phymod_phy_access_t *phy, uint32_t *tx_polarity, uint32_t *rx_polarity);

int _sesto_tx_set(const phymod_phy_access_t *phy, const phymod_tx_t* tx);

int _sesto_tx_get(const phymod_phy_access_t *phy, phymod_tx_t* tx);

int _sesto_rx_set(const phymod_phy_access_t *phy, const phymod_rx_t* rx);

int _sesto_rx_get(const phymod_phy_access_t *phy, phymod_rx_t* rx);

int _sesto_tx_power_set(const phymod_phy_access_t *phy, uint32_t power_tx);

int _sesto_rx_power_set(const phymod_phy_access_t *phy, uint32_t power_rx);

int _sesto_tx_rx_power_get(const phymod_phy_access_t *phy, phymod_phy_power_t* power);

int _sesto_force_tx_training_set(const phymod_phy_access_t *phy, uint32_t enable);

int _sesto_force_tx_training_get(const phymod_phy_access_t *phy, uint32_t *enable);

int _sesto_force_tx_training_status_get(const phymod_phy_access_t *phy, phymod_cl72_status_t* status);

int _sesto_phy_reset_set(const phymod_phy_access_t *phy, const phymod_phy_reset_t* reset);

int _sesto_phy_reset_get(const phymod_phy_access_t *phy, phymod_phy_reset_t* reset);

int _sesto_phy_status_dump(const phymod_phy_access_t *phy);

int _sesto_tx_lane_control_set(const phymod_phy_access_t *phy,  phymod_phy_tx_lane_control_t tx_control);

int _sesto_rx_lane_control_set(const phymod_phy_access_t *phy,  phymod_phy_rx_lane_control_t rx_control);

int _sesto_tx_lane_control_get(const phymod_phy_access_t *phy,  phymod_phy_tx_lane_control_t *tx_control);

int _sesto_rx_lane_control_get(const phymod_phy_access_t *phy,  phymod_phy_rx_lane_control_t *rx_control);

int _sesto_ext_intr_status_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* intr_status);

int _sesto_ext_intr_enable_set(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t enable);

int _sesto_ext_intr_enable_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* enable);

int _sesto_ext_intr_status_clear(const phymod_phy_access_t *phy, uint32_t intr_type);

int _sesto_pll_sequencer_restart(const phymod_core_access_t *core, phymod_sequencer_operation_t operation);

int _sesto_fec_enable_set(const phymod_phy_access_t *phy, uint32_t enable);

int _sesto_fec_enable_get(const phymod_phy_access_t *phy, uint32_t* enable);

int _sesto_config_hcd_link_sts(const phymod_access_t *pa, phymod_phy_inf_config_t config, uint16_t ip);

int _sesto_autoneg_ability_set(const phymod_phy_access_t *phy, sesto_an_ability_t *an_ability);

int _sesto_autoneg_set(const phymod_phy_access_t *phy, const phymod_autoneg_control_t* an);

int _sesto_autoneg_get(const phymod_phy_access_t *phy, phymod_autoneg_control_t* an, uint32_t *an_done);

int _sesto_autoneg_ability_get(const phymod_phy_access_t *phy, sesto_an_ability_t *an_ablity);

int _sesto_autoneg_remote_ability_get(const phymod_phy_access_t *phy, phymod_autoneg_ability_t* an_ability_get);

int _sesto_pcs_link_monitor_enable_get(const phymod_phy_access_t *phy, uint32_t *get_pcs);

int _sesto_pcs_link_monitor_enable_set(const phymod_phy_access_t *phy, uint16_t en_dis);

int _sesto_port_from_lane_map_get(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* cfg, uint16_t *port);

int _sesto_gpio_config_set(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t gpio_mode);

int _sesto_gpio_config_get(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t* gpio_mode);

int _sesto_gpio_pin_value_set(const phymod_access_t *pa, int pin_no, int value);

int _sesto_gpio_pin_value_get(const phymod_access_t *pa, int pin_no, int* value);

int _sesto_module_write(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr, uint32_t no_of_bytes, const uint8_t *write_data);

int _sesto_module_read(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr, uint32_t no_of_bytes, uint8_t *read_data);

int _sesto_set_module_command(const phymod_access_t *pa, uint16_t xfer_addr, uint32_t slv_addr, unsigned char xfer_cnt, SESTO_I2CM_CMD_E cmd);

int _sesto_phy_repeater_mode_set(const phymod_phy_access_t *phy, phymod_operation_mode_t repeater_mode);

int _sesto_core_cfg_polarity_set(const phymod_phy_access_t *phy, uint32_t tx_polarity, uint32_t rx_polarity);

int _sesto_core_cfg_tx_set(const phymod_phy_access_t *phy,const phymod_tx_t* tx);

int _sesto_rx_adaptation_resume(const phymod_phy_access_t* phy);

err_code_t _sesto_merlin_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range);

err_code_t _sesto_falcon_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range);

int _sesto_firmware_lane_config_set(const phymod_phy_access_t* pa, phymod_firmware_lane_config_t fw_config);

int _sesto_firmware_lane_config_get(const phymod_phy_access_t* pa, phymod_firmware_lane_config_t* fw_config);

int _sesto_phy_short_channel_mode_set(const phymod_phy_access_t *phy, uint32_t enable);

int _sesto_phy_short_channel_mode_get(const phymod_phy_access_t *phy, uint32_t *enable, uint32_t *status);

int _sesto_phy_eye_margin_est_get(const phymod_phy_access_t *phy, int *left_eye_mUI, int *right_eye_mUI, int *upper_eye_mV, int *lower_eye_mV);
#endif /* SESTO_CFG_SEQ_H */
