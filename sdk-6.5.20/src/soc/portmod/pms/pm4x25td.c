/*
 *
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_common.h>
#include <soc/mcm/memregs.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X25TD_SUPPORT

#include <soc/portmod/clmac.h>
#include <soc/portmod/pm4x25.h>
#include <soc/portmod/pm4x25_shared.h>

#define PORTMOD_MAX_LINK_WAIT_TIMEOUT      10000
#define PORTMOD_WAIT_TIMEOUT_PER_ITER   1000

#define PM4X25TD_EGR_1588_TIMESTAMP_MODE_48BIT (0)
#define PM4X25TD_EGR_1588_TIMESTAMP_MODE_32BIT (1)

/*
 * Function Prototypes
 */
STATIC
void pm4x25td_port_mac_link_up_wait(int unit, int port, pm_info_t pm_info);

int pm4x25td_port_tsc_reset_set(int unit, int port, int in_reset)
{
    static int rst_flag = 0;
    SOC_INIT_FUNC_DEFS;

    if (!in_reset && !rst_flag) {
        _SOC_IF_ERR_EXIT(soc_tsc_xgxs_reset(unit, port, 0));
        _SOC_IF_ERR_EXIT(soc_tsc_xgxs_reset(unit, port, 1));
        _SOC_IF_ERR_EXIT(soc_tsc_xgxs_reset(unit, port, 2));
        _SOC_IF_ERR_EXIT(soc_tsc_xgxs_reset(unit, port, 3));
        rst_flag = 1;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_lag_failover_status_toggle (int unit, soc_port_t port, pm_info_t pm_info)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    pm4x25td_port_mac_link_up_wait(unit, port, pm_info);
    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_CPORT_LAG_FAILOVER_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_LAG_FAILOVER_CONFIGr(unit, port, rval));
    soc_reg_field_set(unit, CPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 0);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_LAG_FAILOVER_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}


int pm4x25td_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    int rv;
    rv = READ_PGW_CNTMAXSIZEr(unit, port, (uint32_t *)val);
    return rv;
}

int pm4x25td_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    int rv;
    rv = WRITE_PGW_CNTMAXSIZEr(unit, port, (uint32_t)val);
    return rv;
}

int pm4x25td_port_modid_set (int unit, int port, pm_info_t pm_info, int value)
{
    uint32_t rval, modid;
    int      flen;
    SOC_INIT_FUNC_DEFS;

    flen = soc_reg_field_length(unit, CPORT_CONFIGr, MY_MODIDf)? value : 0;
    modid =  (value < (1 <<  flen))? value : 0;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_CONFIGr, &rval, MY_MODIDf, modid);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_higig2_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_CONFIGr, &rval, HIGIG2_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_higig2_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    *(mode) = soc_reg_field_get(unit, CPORT_CONFIGr, rval, HIGIG2_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_higig_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_CONFIGr, &rval, HIGIG_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_higig_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    *(mode) = soc_reg_field_get(unit, CPORT_CONFIGr, rval, HIGIG_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_config_port_type_set (int unit, int port, pm_info_t pm_info, int type)
{

    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_CONFIGr, &rval, PORT_TYPEf, type);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_config_port_type_get (int unit, int port, pm_info_t pm_info, int *type)
{

    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    *(type) = soc_reg_field_get(unit, CPORT_CONFIGr, rval, PORT_TYPEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_led_chain_config (int unit, int port, pm_info_t *pm, 
                          int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(WRITE_CPORT_LED_CHAIN_CONFIGr (unit, port, value));

exit:
    SOC_FUNC_RETURN;
}


int _pm4x25td_pm_enable(int unit, int port, pm_info_t pm_info, int port_index, const portmod_port_add_info_t* add_info, int enable)
{
    uint32 reg_val;
    uint32 rsv_mask;
    int  rv;
    SOC_INIT_FUNC_DEFS;

    if(enable) {
        /* Power Save */
        _SOC_IF_ERR_EXIT(READ_CPORT_POWER_SAVEr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_POWER_SAVEr, &reg_val, CPORT_COREf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_POWER_SAVEr(unit, port, reg_val));


        /* Bring MAC OOR */
        _SOC_IF_ERR_EXIT(READ_CPORT_MAC_CONTROLr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_MAC_CONTROLr, &reg_val, CMAC_RESETf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_MAC_CONTROLr(unit, port, reg_val));

        if (add_info->interface_config.interface == SOC_PORT_IF_CAUI) {
            rv = WRITE_CPORT_MODE_REGr(unit, port, 1); 
            _SOC_IF_ERR_EXIT(rv);
        } else {
            rv = WRITE_CPORT_MODE_REGr(unit, port, 0); 
            _SOC_IF_ERR_EXIT(rv);
        }
    
        /* RSV Mask */
        rsv_mask = 0;
        SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
        SHR_BITSET(&rsv_mask, 4); /* CRC error */
        SHR_BITSET(&rsv_mask, 6); /* Truncated/Frame out of Range */
        SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/
        _SOC_IF_ERR_EXIT(WRITE_PGW_MAC_RSV_MASKr(unit, port, rsv_mask));

        if (PM_4x25_INFO(pm_info)->nof_phys[0]> 0) {  
            /* Get Serdes OOR */
            _SOC_IF_ERR_EXIT(pm4x25td_port_tsc_reset_set(unit, port, 1));
            _SOC_IF_ERR_EXIT(pm4x25td_port_tsc_reset_set(unit, port, 0));
        }
    } else { /* disable */

        rv = WRITE_CPORT_MODE_REGr(unit, port, 0); 
        _SOC_IF_ERR_EXIT(rv);

        /* Put Serdes in reset*/
        _SOC_IF_ERR_EXIT(pm4x25td_port_tsc_reset_set(unit, port, 1));

        /* put MAC in reset */
        _SOC_IF_ERR_EXIT(READ_CPORT_MAC_CONTROLr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_MAC_CONTROLr, &reg_val, CMAC_RESETf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_MAC_CONTROLr(unit, port, reg_val));

        /* Turn on Power Save */
        _SOC_IF_ERR_EXIT(READ_CPORT_POWER_SAVEr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_POWER_SAVEr, &reg_val, CPORT_COREf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_POWER_SAVEr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x25td_pm_port_init(int unit, int port, int internal_port,
                         const portmod_port_add_info_t* add_info, int enable)
{
    int rv;
    uint32 reg_val, flags;
    SOC_INIT_FUNC_DEFS;

    if(enable) {

        /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_CPORT_SOFT_RESETr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_SOFT_RESETr, &reg_val, CPORT_COREf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, reg_val));

        soc_reg_field_set(unit, CPORT_SOFT_RESETr, &reg_val, CPORT_COREf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, reg_val));

        /* Port enable */
        _SOC_IF_ERR_EXIT(READ_CPORT_ENABLE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_ENABLE_REGr, &reg_val, PORT0f, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_ENABLE_REGr(unit, port, reg_val));
      
        /* Init MAC */
        flags = 0;
        if(PORTMOD_PORT_ADD_F_RX_SRIP_CRC_GET(add_info)) {
            flags |= CLMAC_INIT_F_RX_STRIP_CRC;
        }

        if(PORTMOD_PORT_ADD_F_TX_APPEND_CRC_GET(add_info)) {
            flags |= CLMAC_INIT_F_TX_APPEND_CRC;
        }

        if(PORTMOD_PORT_ADD_F_TX_REPLACE_CRC_GET(add_info)) {
            flags |= CLMAC_INIT_F_TX_REPLACE_CRC;
        }

        if(PORTMOD_PORT_ADD_F_TX_PASS_THROUGH_CRC_GET(add_info)) {
            flags |= CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE;
        }

        flags |= CLMAC_INIT_F_IPG_CHECK_DISABLE;

        if(PHYMOD_INTF_MODES_HIGIG_GET(&(add_info->interface_config))) {
            flags |= CLMAC_INIT_F_IS_HIGIG;
        }

        rv = clmac_init(unit, port, flags);
        _SOC_IF_ERR_EXIT(rv);

        /* LSS */
        _SOC_IF_ERR_EXIT(READ_CPORT_FAULT_LINK_STATUSr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_FAULT_LINK_STATUSr, &reg_val,
                          REMOTE_FAULTf, 1);
        soc_reg_field_set(unit, CPORT_FAULT_LINK_STATUSr, &reg_val,
                          LOCAL_FAULTf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_FAULT_LINK_STATUSr(unit, port, reg_val)); 


        /* Counter MAX size */ 
        _SOC_IF_ERR_EXIT(READ_PGW_CNTMAXSIZEr(unit, port, &reg_val));
        soc_reg_field_set(unit, PGW_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, 1518);
        _SOC_IF_ERR_EXIT(WRITE_PGW_CNTMAXSIZEr(unit, port, reg_val));

    } else {
        /* Port disable */
        _SOC_IF_ERR_EXIT(READ_CPORT_ENABLE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_ENABLE_REGr, &reg_val ,PORT0f, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_ENABLE_REGr(unit, port, reg_val));

         /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_CPORT_SOFT_RESETr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_SOFT_RESETr, &reg_val, CPORT_COREf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{
    uint32 reg_val, old_val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(READ_CPORT_SOFT_RESETr(unit, port, &reg_val));
    old_val = reg_val;
    soc_reg_field_set(unit, CPORT_SOFT_RESETr, &reg_val, CPORT_COREf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, reg_val));
    _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, old_val));

exit:
    SOC_FUNC_RETURN;

}

int pm4x25td_ext_phy_attach_to_pm(int unit, pm_info_t pm_info, const phymod_core_access_t* ext_phy_access, uint32 first_phy_lane)
{
    /* empty happy function */
    return SOC_E_NONE;
}


int pm4x25td_ext_phy_detach_from_pm(int unit, pm_info_t pm_info, phymod_core_access_t* ext_phy_access)
{
    /* empty happy function */
    return SOC_E_NONE;
}

int pm4x25td_xphy_lane_attach_to_pm(int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_connection)
{
    /* empty happy function. */
    return SOC_E_NONE;
}


int pm4x25td_xphy_lane_detach_from_pm(int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_connection)
{
    /* empty happy function. */
    return SOC_E_NONE;
}

int pm4x25td_port_pgw_reconfig(int unit, int port, pm_info_t pm_info, 
                               const portmod_port_mode_info_t *pmode, int phy_port, int flags)
{
    int rv;
    uint32 reg_val;

    SOC_INIT_FUNC_DEFS;

    if (PORTMOD_PORT_PGW_MAC_RESET == flags) {
        rv = clmac_discard_set(unit, port, 1);
        /* 1.a st SOFT_RESET field in CLMAC_CTRL for new ports that use XLMAC */
        rv = clmac_soft_reset_set(unit, port, 1);
        _SOC_IF_ERR_EXIT(rv);

        _SOC_IF_ERR_EXIT(READ_CPORT_MAC_CONTROLr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_MAC_CONTROLr, &reg_val, CMAC_RESETf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_MAC_CONTROLr(unit, port, reg_val));

        /* 1.a set CPORT_CORE field in CPORT_SOFT_RESET for all new ports */
        _SOC_IF_ERR_EXIT(READ_CPORT_SOFT_RESETr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_SOFT_RESETr, &reg_val, CPORT_COREf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, reg_val));

        /* Port enable */
        _SOC_IF_ERR_EXIT(READ_CPORT_ENABLE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_ENABLE_REGr, &reg_val, PORT0f, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_ENABLE_REGr(unit, port, reg_val));
        /* cport_power_save.cport_core = 0; */
        _SOC_IF_ERR_EXIT(READ_CPORT_POWER_SAVEr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_POWER_SAVEr, &reg_val, CPORT_COREf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_POWER_SAVEr(unit, port, reg_val));
    } else if (PORTMOD_PORT_PGW_MAC_UNRESET == flags) {
        /* cport_soft_reset.cport_core = 0; */
        _SOC_IF_ERR_EXIT(READ_CPORT_SOFT_RESETr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_SOFT_RESETr, &reg_val, CPORT_COREf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, reg_val));

        /* cport_cport_enable.port0 = 1; */
        _SOC_IF_ERR_EXIT(READ_CPORT_ENABLE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_ENABLE_REGr, &reg_val, PORT0f, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_ENABLE_REGr(unit, port, reg_val));

        /* cport_cport_mac_control.cmac_reset = 0; */
        _SOC_IF_ERR_EXIT(READ_CPORT_MAC_CONTROLr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_MAC_CONTROLr, &reg_val, CMAC_RESETf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_MAC_CONTROLr(unit, port, reg_val));

        rv = clmac_discard_set(unit, port, 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = clmac_rx_enable_set(unit, port, 1);
        _SOC_IF_ERR_EXIT(rv);
        rv = clmac_tx_enable_set(unit, port, 1);
        _SOC_IF_ERR_EXIT(rv);
        rv = clmac_soft_reset_set(unit, port, 0);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_pfc_config_set (int unit, int port,pm_info_t pm_info,
                                const portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    if (pfc_cfg->classes != 8) {
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT(clmac_pfc_config_set(unit, port, pfc_cfg));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_pfc_config_get (int unit, int port,pm_info_t pm_info,
                                portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    pfc_cfg->classes = 8;
    _SOC_IF_ERR_EXIT(clmac_pfc_config_get(unit, port, pfc_cfg));
exit:
    SOC_FUNC_RETURN;
}

STATIC
void pm4x25td_port_mac_link_up_wait(int unit, int port, pm_info_t pm_info)
{
    
    int i = 0, link = 0, rv = SOC_E_NONE;
    uint32_t rval;

    /* Link status to IPIPE is updated by H/W and driven based on both the
     * rising edge of XLPORT_LAG_FAILOVER_CONFIG.LINK_STATUS_UP and
     * actual link up status from Serdes.
     * In some loopback scenarios it may take longer time to see Serdes
     * link up status.
     */

    do {
        rv = READ_CPORT_XGXS0_LN0_STATUS0_REGr(unit, port, &rval);

        if (SOC_FAILURE(rv)) {
            break;
        }

        link = soc_reg_field_get(unit, CPORT_XGXS0_LN0_STATUS0_REGr,
                                 rval, LINK_STATUSf);
        if (link) {
            break;
        }
        sal_udelay(PORTMOD_WAIT_TIMEOUT_PER_ITER);
    } while((++i * PORTMOD_WAIT_TIMEOUT_PER_ITER) <
             PORTMOD_MAX_LINK_WAIT_TIMEOUT);

    return;
}

int pm4x25td_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info,
                            portmod_egr_1588_timestamp_config_t timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    SOC_INIT_FUNC_DEFS;

    if (timestamp_config.timestamp_mode == portmodTimestampMode32bit) {
        timestamp_mode = PM4X25TD_EGR_1588_TIMESTAMP_MODE_32BIT;
    } else if (timestamp_config.timestamp_mode == portmodTimestampMode48bit) {
        timestamp_mode = PM4X25TD_EGR_1588_TIMESTAMP_MODE_48BIT;
    } else {
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(READ_CPORT_MODE_REGr(unit, port, &reg_val));

    soc_reg_field_set(unit, CPORT_MODE_REGr, &reg_val,
                      EGR_1588_TIMESTAMPING_MODEf, timestamp_mode);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_MODE_REGr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info,
                           portmod_egr_1588_timestamp_config_t* timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_MODE_REGr(unit, port, &reg_val));
    timestamp_mode = soc_reg_field_get(unit, CPORT_MODE_REGr, reg_val,
                                       EGR_1588_TIMESTAMPING_MODEf);

    if (timestamp_mode == PM4X25TD_EGR_1588_TIMESTAMP_MODE_32BIT) {
        timestamp_config->timestamp_mode = portmodTimestampMode32bit;
    } else {
        timestamp_config->timestamp_mode = portmodTimestampMode48bit;
    }

    timestamp_config->cmic_48_overr_en = 0;

exit:
    SOC_FUNC_RETURN;
}
#endif /* PORTMOD_PM4X25TD_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
