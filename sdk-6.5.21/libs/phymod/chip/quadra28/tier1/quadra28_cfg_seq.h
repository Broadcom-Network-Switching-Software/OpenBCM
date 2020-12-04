/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.  
 */
 
#ifndef __QUADRA28_CFG_SEQ_H__
#define __QUADRA28_CFG_SEQ_H__

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include "quadra28_types.h"
#include "quadra28_serdes/common/srds_api_enum.h"
/*
 *  Global Variables
 */

/*
 *  Functions
 */

/*
 *  Functions for Manipulating Chip/Port Cfg Descriptors
 */

/*
 * Functions
 */

/*
 *  Functions for Manipulating Chip/Port Cfg Descriptors
 */



#define QUADRA28_IS_DUAL(devid) \
((devid) == QUADRA28_ID_82071 ||\
 (devid) == QUADRA28_ID_82315   \
)

#define QUADRA28_IS_QUAD(devid)\
((devid) == QUADRA28_ID_82071 ||\
 (devid) == QUADRA28_ID_82314 ||\
 (devid) == QUADRA28_ID_82315   \
)

#define QUADRA28_IS_OCTAL(devid)\
((devid) == QUADRA28_ID_82071 ||\
 (devid) == QUADRA28_ID_82070 ||\
 (devid) == QUADRA28_ID_82314 ||\
 (devid) == QUADRA28_ID_82315   \
)

#define QUADRA28_GET_IF_SIDE(_FLAGS, _SIDE)                        \
{                                                                  \
    if(((_FLAGS >> 31) & 0x1) == 0x1) {                            \
        _SIDE = Q28_INTF_SYS;                                      \
    } else {                                                       \
        _SIDE = Q28_INTF_LINE;                                     \
    }                                                              \
}

#define QUADRA28_GET_LOC(_PHY, _SIDE)                              \
{                                                                  \
    if(_PHY->port_loc == phymodPortLocSys) {                       \
        _SIDE = Q28_INTF_SYS;                                      \
    } else {                                                       \
        _SIDE = Q28_INTF_LINE;                                     \
    }                                                              \
}

#define QUADRA28_PMD_ID0                  0xae02
#define QUADRA28_PMD_ID1                  0x5250
#define QUADRA28_82780_CHIP_ID            0x82780
#define QUADRA28_8278F_CHIP_ID            0x8278F
#define QUADRA28_82752_CHIP_ID            0x82752
#define QUADRA28_82758_CHIP_ID            0x82758

#define QUADRA28_NOF_LANES_IN_CORE        4   
#define QUADRA28_PHY_ALL_LANES            0xf 
#define DELAY_1_MS_FROM_US                1000
#define Q28_ALL_LANE                      0xF

/*FOV registers, Since in RDB there is no bit decription
 * we are adding register offset as macro*/
#define Q28_TOP_GP_REG0                      0xC915
#define Q28_TOP_GP_REG1                      0xC916
#define Q28_FOV_TYPE_N0S1                    1
#define Q28_FOV_TYPE_N1S0                    2 

enum LINE_SELECTION{
    Q28_INTF_LINE = 0,
    Q28_INTF_SYS = 1
}; 

#define SIDE_SELECT(pa,r,side){                                                \
    if(Q28_INTF_LINE == side) {                                                \
        PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_SIDE_SELECTIONr(pa,&r));       \
        BCMI_QUADRA28_SIDE_SELECTIONr_SIDE_SELf_SET(r,Q28_INTF_LINE);          \
        PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_WRITE_SIDE_SELECTIONr(pa,r));       \
    } else {                                                                   \
        PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_SIDE_SELECTIONr(pa,&r));       \
        BCMI_QUADRA28_SIDE_SELECTIONr_SIDE_SELf_SET(r,Q28_INTF_SYS);           \
        PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_WRITE_SIDE_SELECTIONr(pa,r));       \
    }                                                                          \
}

#define QUADRA28_EN_BROADCAST(pa,r,enable)  {                               \
    PHYMOD_IF_ERR_RETURN(                                                   \
                 READ_MDIO_BROADCAST_CONTROLr(pa,&r));                      \
    if (1 == enable) {                                                      \
        BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_MCAST_ENf_SET(r,0);      \
        BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(r, 1);     \
    } else {                                                                \
        BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(r,0);      \
    }                                                                       \
    PHYMOD_IF_ERR_RETURN(WRITE_MDIO_BROADCAST_CONTROLr(pa, r));             \
}	   

#define QUADRA28_ADDRESS_LANE(_pa,r, lane_index) {                                  \
    PHYMOD_IF_ERR_RETURN(                                                           \
                 READ_MDIO_BROADCAST_CONTROLr(pa,&r));                              \
    BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(r,0);                  \
    BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_MCAST_ENf_SET(r,1);                  \
    BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_MULTICAST_PHYADf_SET(r, lane_index); \
    PHYMOD_IF_ERR_RETURN(WRITE_MDIO_BROADCAST_CONTROLr(pa, r));                     \
}

#define QUADRA28_IS_PCS_ON(pa,r, status){                 \
    PHYMOD_IF_ERR_RETURN(                                 \
                 BCMI_QUADRA28_READ_PCS_STATUS1r(pa,&r)); \
    *status = BCMI_QUADRA28_PCS_STATUS1r_GET(r);          \
}

#define QUADRA28_SELECT_SIDE(pa, r, _side)                        SIDE_SELECT(pa,r,_side)

#define QUADRA28_RESET_SIDE_BCST_CTRL(pa, r, _side) {              \
    BCMI_QUADRA28_CONFIG_ACCESSr_t  bcast;                         \
    if(Q28_INTF_LINE == _side) {                                   \
        SIDE_SELECT(pa,r,Q28_INTF_LINE);                           \
    } else {                                                       \
        SIDE_SELECT(pa,r,Q28_INTF_SYS);                            \
    }                                                              \
    PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_CONFIG_ACCESSr(pa, &bcast)); \
    BCMI_QUADRA28_CONFIG_ACCESSr_BROADCAST_ENABLEf_SET(bcast,0);         \
    PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_WRITE_CONFIG_ACCESSr(pa, bcast)); \
}

#define Q28_UNUSED_PARAM(x)         (void)(x)

#define Q28_GET_PMD_MODE(m_acc, pmd_mode)                                                  \
    {                                                                                      \
        phymod_interface_t m_intf;                                                         \
        phymod_ref_clk_t m_ref_clk;                                                        \
        uint32_t m_intf_modes;                                                             \
        PHYMOD_IF_ERR_RETURN (                                                             \
          quadra28_get_config_mode(m_acc, &m_intf, &speed, &m_ref_clk, &m_intf_modes));    \
        Q28_UNUSED_PARAM(m_intf);                                                          \
        Q28_UNUSED_PARAM(m_ref_clk);                                                       \
        Q28_UNUSED_PARAM(m_intf_modes);                                                    \
        if (speed <= Q28_SPEED_HG11) {                                                     \
            pmd_mode = Q28_MULTI_PMD;                                                      \
        } else {                                                                           \
            pmd_mode = Q28_SINGLE_PMD;                                                     \
        }                                                                                  \
    }

#define Q28_10G_PHY_ID_WORK_AROUND(speed, acc)     \
                      (void)speed;
/*    if (speed <= Q28_SPEED_HG11) {                 \
        if (acc.lane_mask == 0x2) {                \
            acc.addr += 1;                         \
        }                                          \
        if (acc.lane_mask == 0x4) {                \
            acc.addr += 2;                         \
        }                                          \
        if (acc.lane_mask == 0x8) {                \
            acc.addr += 3;                         \
        }                                          \
    }
*/
#define Q28_GET_DATAPATH(acc, dp){                               \
    BCMI_QUADRA28_GENERAL_PURPOSE_3r_t gpreg_3;                  \
    PHYMOD_MEMSET(&gpreg_3, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_3r_t));            \
    PHYMOD_IF_ERR_RETURN(READ_GENERAL_PURPOSE_3r(acc,&gpreg_3)); \
    dp = (gpreg_3.v[0] & 0x40) >> 6;                                  \
    if (dp == 0) {                                               \
        dp = phymodDatapathNormal;                               \
    } else { dp = phymodDatapathUll; }                           \
}

#define Q28_BIT(X) (1<<X)
#define Q28_GET_SPEED_FROM_REG(reg, speed)                      \
    switch (reg & 0xf) {                                        \
        case 1:                                                 \
            speed = SPEED_1G_OS8;                               \
            break;                                              \
        case 2:                                                 \
            speed = Q28_SPEED_10G;                              \
            break;                                              \
        case 3:                                                 \
            speed = SPEED_1G_OS8P25;                            \
            break;                                              \
        case 4:                                                 \
            speed = Q28_SPEED_40G;                              \
            break;                                              \
        case 7:                                                 \
            speed = Q28_SPEED_42G;                              \
            break;                                              \
        case 8:                                                 \
            speed = Q28_SPEED_11P5G;                            \
            break;                                              \
        case 0xc:                                               \
            speed = Q28_SPEED_20G;                              \
            break;                                              \
        default:                                                \
            speed = Q28_SPEED_10G;                              \
            break;                                              \
    }

typedef struct q28_an_ability_s {
    int fec_ability;
    int pause_ability;
}q28_an_ability_t;

typedef enum {
    Q28_NO_PAUSE = 0,  
    Q28_ASYM_PAUSE,
    Q28_SYMM_PAUSE,
    Q28_ASYM_SYMM_PAUSE,  
    Q28_AN_PAUSE_COUNT
}q28_an_pause_t;

typedef enum {
    Q28_CL73_NO_TECH = 0,
    Q28_CL73_1000BASE_KX = 0x1,
    Q28_CL73_10GBASE_KX4 = 0x2,
    Q28_CL73_10GBASE_KR = 0x4,
    Q28_CL73_40GBASE_KR4 = 0x8,
    Q28_CL73_40GBASE_CR4 = 0x10,
    Q28_CL73_100GBASE_CR10 = 0x20,
    Q28_CL73_100GBASE_KP4 = 0x40,
    Q28_CL73_100GBASE_KR4 = 0x80,
    Q28_CL73_100GBASE_CR4 = 0x100
}q28_cl73_speed_t;

#define QUADRA28_I2C_RAM_START_ADDRESS            0x8007
#define QUADRA28_I2C_MAX_BYTE                     256
#define Q28_MAX_BYTES                             1
typedef enum {
    Q28_I2C_WRITE = 0,
    Q28_I2C_READ
}q28_i2c_acc_t;


int _quadra28_config_line_interface(const phymod_access_t *pa, uint32_t* reg_val, phymod_interface_t intf, uint32_t speed);
int _quadra28_config_sys_interface(const phymod_access_t *pa, uint32_t* reg_val, phymod_interface_t intf);
int _quadra28_config_port_speed(uint32_t speed, uint32_t* reg_val, uint16_t* mode_type);
int quadra28_rom_dload(const phymod_access_t *pa, uint8_t *new_fw, uint32_t fw_length);
int quadra28_firmware_info_get(const phymod_access_t *pa, phymod_core_firmware_info_t *fw_info);
/**   Get Revision ID 
 *    This function retrieves Revision ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return rev_id            Revision ID retrieved from the chip
 */
uint16_t _quadra28_get_rev_id(const phymod_access_t *pa);

/**   Get Chip ID 
 *    This function retrieves Chip ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return chip_id           Chip id retrieved from the chip
 */
uint32_t _quadra28_get_chip_id(const phymod_access_t *pa);

/**   Reset Chip 
 *    This function is used to reset entire chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_chip_reset(const phymod_access_t *pa);	
	
/**   Reset Register 
 *    This function is used to perform register reset 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_register_rst(const phymod_access_t *pa);


/**   Configure PRBS generator 
 *    This function is used to configure PRBS generator with user provided
 *    polynomial and invert data information 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param prbs_mode          User specified polynomial
 *                              0 - PRBS7
 *                              1 - PRBS9
 *                              2 - PRBS11    
 *                              3 - PRBS15
 *                              4 - PRBS23
 *                              5 - PRBS31
 *                              6 - PRBS58
 * 
 *    @param prbs_inv           User specified invert data config
 *                              0 - do not invert PRBS data  
 *                              1 - invert PRBS data  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
/*int quadra28_prbs_config_set(const phymod_access_t *pa,
                        uint32_t flags,
                        enum srds_prbs_polynomial_enum prbs_mode,
                        uint32_t prbs_inv);*/


/**   Enable PRBS generator and PRBS checker 
 *    This function is used to enable or disable PRBS generator and PRBS checker
 *    as requested by the user  
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param enable             Enable or disable as specified by the user
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_enable_set(const phymod_phy_access_t *pa, uint32_t flags, uint32_t enable);

/**   Get Enable status of PRBS generator and PRBS checker 
 *    This function is used to retrieve the enable status of PRBS generator and
 *    PRBS checker
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param *enable            Enable or disable read from chip 
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_enable_get(const phymod_phy_access_t *pa, uint32_t flags, uint32_t *enable);



/**   Get PRBS lock and error status 
 *    This function is used to retrieve PRBS lock, loss of lock and error counts
 *    from the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param lock_status        PRBS lock status denotes PRBS generator and 
 *                              checker are locked to same polynomial data
 *    @param lock_loss          Loss of lock denotes PRBS generator and checker
 *                              are not in sync   
 *    @param error_count        PRBS error count retrieved from chip
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_status_get(const phymod_phy_access_t *pa,
                                uint32_t *lock_status,
                                uint32_t *lock_loss,
                                uint32_t *error_count);

/**   Get lane descriptor 
 *    This function is used to retrieve lane descriptor from package lane 
 *
 *    @param chip_id         chip id number 
 *    @param pa             Pointer to phymod access structure 
 *    @param pkg_lane       Package lane number 
 *
 *    @return pkg_ln_des    Lane descriptor structure contains the info
 *                              about package lane and die lane mapping 
 */
/*const struct _quadra28_PKG_LANE_CFG_S* _quadra28_pkg_ln_des(uint32_t chip_id, const phymod_access_t *pa, int pkg_lane);*/

/**   Get  link status of PHY 
 *    This function is used to retrieve the link status of PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @param link_status        link status of the PHY 
 *                              1 - Up 
 *                              0 - Down 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_chk_phy_link_mode_status(const phymod_access_t *pa,
                                   uint32_t *link_status); 

/**   Set config mode 
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param intf               Interface specified by user 
 *    @param speed              Speed val as specified by user  
 *    @param ref_clk            Reference clock frequency to set 
 *                              the PHY into specified interface 
 *                              and speed
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_set_config_mode(const phymod_phy_access_t *phy,
                          phymod_interface_t intf,
                          uint32_t speed,
                          phymod_ref_clk_t ref_clk, uint32_t datapath, uint16_t retimer);

/**   Get config mode 
 *    This function is used to retrieve the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param intf               Interface type 
 *    @param speed              Speed val retrieved from PHY 
 *    @param ref_clk            Reference clock frequency to set 
 *                              the PHY into specified interface 
 *                              and speed
 *    @param interface_modes    mode which is currently unused
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_get_config_mode(const phymod_access_t *phy,
                       phymod_interface_t *intf,
                       uint32_t *speed,
                       phymod_ref_clk_t *ref_clk,
                       uint32_t *interface_modes);

/**   Configure PRBS generator 
 *    This function is used to configure PRBS generator with user provided
 *    polynomial and invert data information 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param prbs_mode          User specified polynomial
 *                              0 - PRBS7
 *                              1 - PRBS9
 *                              2 - PRBS11    
 *                              3 - PRBS15
 *                              4 - PRBS23
 *                              5 - PRBS31
 *                              6 - PRBS58
 * 
 *    @param prbs_inv           User specified invert data config
 *                              0 - do not invert PRBS data  
 *                              1 - invert PRBS data  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quara28_prbs_config_set(const phymod_phy_access_t *pc,
                        uint32_t flags,
                        uint32_t prbs_mode,
                        uint32_t prbs_inv);

/**   Get PRBS generator configuration 
 *    This function is used to retrieve PRBS generator configuration from 
 *    the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param prbs_mode          Configured Polynomial retrieved from chip 
 *    @param prbs_inv           Configured invert data retrieved from chip
 *                              0 - do not invert PRBS data  
 *                              1 - invert PRBS data  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_prbs_config_get(const phymod_phy_access_t *pa,
                        uint32_t flags,
                        enum srds_prbs_polynomial_enum *prbs_mode,
                        uint32_t *prbs_inv);
						
/**   Get phymod interface type from pma type 
 *    This function is used to retrieve PHYMOD interface type from pma_type
 *
 *    @param pma_type           pma type read from mode register 
 *    @param intf               PHYMOD interface type 
 *    @param speed              Speed val 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _quadra28_get_phymod_interface_type(uint16_t pma_type,
                                     phymod_interface_t *intf,
                                     uint32_t *speed);


/**  PMD lock get 
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param rx_seq_done        RX sew done status
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int quadra28_pmd_lock_get(const phymod_phy_access_t *phy, uint32_t *rx_seq_done); 									 
									 /**   Get pma type from interface and speed 
 *    This function is used to get the pma type from interface and speed. 
 *    This pma type will be programmed to mode register for different PHY modes 
 *
 *    @param intf               PHYMOD interface type 
 *    @param speed              Speed val 
 * 
 *    @return pma_typeselection pma type calculated with interface and speed 
 */
uint16_t _quadra28_get_pma_type_selection(phymod_interface_t intf, uint32_t speed);


int quadra28_tx_rx_polarity_set (const phymod_phy_access_t *phy,
                              uint32_t tx_polarity,
                              uint32_t rx_polarity);


int quadra28_tx_rx_polarity_get (const phymod_phy_access_t *phy,
                              uint32_t *tx_polarity,
                              uint32_t *rx_polarity);

	
int quadra28_link_status(const phymod_phy_access_t *phy, uint32_t *link_status);

int quadra28_reg_read(const phymod_access_t *pa, uint32_t addr, uint32_t  *val);

int quadra28_reg_write(const phymod_access_t *pa, uint32_t addr, uint32_t  val);

int _quadra28_refclk_set(const phymod_access_t *pa, uint32_t ref_clk) ;

int quadra28_get_chip_id(const phymod_access_t *pa);

int quadra28_tx_lane_control_set(const phymod_phy_access_t *phy,
                              phymod_phy_tx_lane_control_t tx_control);

int quadra28_tx_squelch_get(const phymod_phy_access_t *phy,
                               int *tx_squelch);

int quadra28_rx_lane_control_set(const phymod_phy_access_t *phy,
                              phymod_phy_rx_lane_control_t rx_control);


int quadra28_rx_squelch_get(const phymod_phy_access_t *phy, int *rx_squelch);

int quadra28_display_eye_scan(const phymod_phy_access_t *pa);

int quadra28_ber_proj (const phymod_phy_access_t *pa, const phymod_phy_eyescan_options_t* eyescan_options);

int _quadra28_phy_status_dump(const phymod_phy_access_t *phy);

int _quadra28_phy_diagnostics_get(const phymod_phy_access_t *pa, phymod_phy_diagnostics_t* diag);

int quadra28_soft_reset(const phymod_access_t* pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction);

int _quadra28_phy_reset_set(const phymod_phy_access_t *phy,  const phymod_phy_reset_t* reset);

int _quadra28_phy_reset_get(const phymod_phy_access_t *phy,  phymod_phy_reset_t* reset);

int _quadra28_loopback_set(const phymod_phy_access_t *phy, phymod_loopback_mode_t loopback, uint32_t enable);

int _quadra28_loopback_get(const phymod_phy_access_t *phy, phymod_loopback_mode_t loopback, uint32_t* enable);

int _quadra28_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power);

int _quadra28_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power);

int _quadra28_intf_update_wait_check(const phymod_access_t *pa, uint32_t apps_mode_val,
                                     int32_t timeout) ;

int quadra28_channel_select (const phymod_access_t *pa, uint16_t lane);

int _quadra28_pll_seq_restart(const phymod_access_t *pa, uint32_t flag, phymod_sequencer_operation_t operation);

int _quadra28_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx);

int _quadra28_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx);

int _quadra28_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx);

int _quadra28_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx);

int _quadra28_phy_autoneg_ability_set(const phymod_access_t* pa, q28_an_ability_t an_ability);


int _quadra28_phy_autoneg_ability_get(const phymod_access_t* pa, phymod_autoneg_ability_t* an_ability_get_type);

int _quadra28_phy_autoneg_get(const phymod_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done);

int _quadra28_phy_autoneg_remote_ability_get(const phymod_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type);

int _quadra28_phy_autoneg_set(const phymod_access_t* pa, const phymod_autoneg_control_t* an);

int _quadra28_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en);

int _quadra28_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en);

int _quadra28_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status);

int _quadra28_phy_retimer_enable(const phymod_access_t *pa, uint32_t enable);

int _quadra28_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable);

int _quadra28_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable);

int _quadra28_finish_mask_seq(const phymod_access_t *pa);

int _quadra28_set_datapath(const phymod_access_t *pa, int speed, phymod_datapath_t op_datapath);

int _quadra28_phy_retimer_enable_get(const phymod_access_t *pa, uint32_t *enable);

int _quadra28_i2c_read(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr,
                      uint32_t no_of_bytes, uint8_t *read_data);

int _quadra28_i2c_write(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr,
                       uint32_t no_of_bytes, const uint8_t *write_data);

int _quadra28_i2c_rw(const phymod_access_t *pa, uint32_t slv_addr, 
                     uint32_t slave_start_addr, uint16_t xfer_cnt, uint8_t acc_type, uint8_t *data);

int _quadra28_edc_config_set(const phymod_phy_access_t* phy, const phymod_edc_config_t* edc_config);

int _quadra28_edc_config_get(const phymod_phy_access_t* phy, phymod_edc_config_t* edc_config);

int quadra28_before_fw_load(const phymod_core_access_t* core,
                            const phymod_core_init_config_t *init_config, uint32_t chip_id);
int quadra28_bcast_fw_load(const phymod_core_access_t* core,
                           const phymod_core_init_config_t *init_config, 
                           uint8_t *new_fw,
                           uint32_t fw_length);

int quadra28_bcast_after_fw_load(const phymod_core_access_t* core, uint32_t chip_id);
int quadra28_bcast_disable(const phymod_core_access_t* core, uint32_t chip_id);
 
int _quadra28_failover_mode_set(const phymod_phy_access_t* phy, phymod_failover_mode_t failover_mode);
 
int _quadra28_failover_mode_get(const phymod_phy_access_t* phy, phymod_failover_mode_t* failover_mode);
 
int _quadra28_clear_mode_config(phymod_access_t *pa);

int _quadra28_phy_rx_adaptation_resume(const phymod_phy_access_t *phy);

int _quadra28_phy_gpio_config_set(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t gpio_mode);

int _quadra28_phy_gpio_config_get(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t* gpio_mode);

/*Set/Get the output/input value of a PHY GPIO pin*/
int _quadra28_phy_gpio_pin_value_set(const phymod_phy_access_t* phy, int pin_no, int value);

int _quadra28_phy_gpio_pin_value_get(const phymod_phy_access_t* phy, int pin_no, int* value);

#endif /*__QUADRA28_CFG_SEQ_H__*/

