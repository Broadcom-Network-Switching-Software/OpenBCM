/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * OOB Flow Control and OOB Stats
 * Purpose: API to set different OOB Flow Control and OOB Stats registers.
 */

#include <sal/core/libc.h>

#include <shared/bsl.h>
#include <soc/defs.h>

#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/tomahawk.h>

#include <bcm/error.h>
#include <bcm/oob.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/tomahawk.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stat.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Number of Traffic Class */
#define _TH_NUM_TC  8
/* Number of OOB FC Rx Interface */
#define _TH_NUM_OOB_FC_RX_INTF    4
/* Maximum Value of Channel Base Value */
#define _TH_MAX_CHANNEL_BASE    63

/* Total number of Indexes in OOBFC RX memory */
#define _TH_MAX_RX_MEM_ENTRY    96

/* Maximum Value of OOB Stats Config_ID */
#define _TH_MAX_STATS_CONFIG_ID 3

/* Total number of Indexes in OOB Stats Service Pool List */
#define _TH_MAX_IDX_STATS_POOL_LIST    8

/* Total number of Indexes in OOB Stats UC Queue List */
#define _TH_MAX_IDX_STATS_QUEUE_LIST     1320

/* Total number of UC Queue per port. */
#define _TH_NUM_UCAST_QUEUE_PER_PORT 10

/* Total number of pipes per slice */
#define _TH_NUM_PIPE_PER_SLICE       2

/* 0xF is the end of reporting of OOB Pool Stats */
#define _TH_OOB_STATS_END_POOL_REPORT   0xF

/* 0x7FF is the end of reporting of OOB Queue Stats */
#define _TH_OOB_STATS_END_QUEUE_REPORT   0x7FF

typedef struct _bcm_th_oob_fc_rx_intf_info_s {
    uint32 channel_base; /* HCFC channel base */
    int enable;          /* Interface enable */
    uint8 num_ports;     /* Number of ports monitored */
    uint8 lookup_base;   /* Look up base in the OOB FC Rx Memory */
} _bcm_th_oob_fc_rx_intf_info_t;
    
/*
 * Function:
 *      _bcm_th_oob_fc_tx_config_flags_set
 * Purpose:
 *      Set OOB FC Tx enable flags
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      flags - (IN) OOB FC Tx flags
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_tx_config_flags_set(int unit, uint32 flags)
{
    uint32 rval = 0;
    uint64 rval64;
    uint64 field64;

    COMPILER_64_ZERO(rval64);
    COMPILER_64_ZERO(field64);

    if (flags & BCM_OOB_FC_TX_FCN_EN) {
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, INTFO_FCN_ENr, 0, 0, &rval));
        soc_reg_field_set(unit, INTFO_FCN_ENr, &rval, ENf, 0x1);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, INTFO_FCN_ENr, 0, 0, rval));
    } else {
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, INTFO_FCN_ENr, 0, 0, &rval));
        soc_reg_field_set(unit, INTFO_FCN_ENr, &rval, ENf, 0x0);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, INTFO_FCN_ENr, 0, 0, rval));
    }

    BCM_IF_ERROR_RETURN
        (soc_reg64_get(unit, OOBFC_CHANNEL_BASE_64r,
                       0, 0, &rval64));
    if (flags & BCM_OOB_FC_TX_ING_EN) {
        COMPILER_64_SET(field64, 0, 1);
    }
    soc_reg64_field_set(unit, OOBFC_CHANNEL_BASE_64r, &rval64,
                        ING_ENf, field64);

    COMPILER_64_ZERO(field64);
    if (flags & BCM_OOB_FC_TX_EGR_EN) {
        COMPILER_64_SET(field64, 0, 1);
    }
    soc_reg64_field_set(unit, OOBFC_CHANNEL_BASE_64r, &rval64,
                        ENG_ENf, field64);

    COMPILER_64_ZERO(field64);
    if (flags & BCM_OOB_FC_TX_POOL_EN) {
        COMPILER_64_SET(field64, 0, 1);
    }
    if (SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWKPLUS(unit)) {
        soc_reg64_field_set(unit, OOBFC_CHANNEL_BASE_64r, &rval64,
                            MC_ENf, field64);
    } else {
        soc_reg64_field_set(unit, OOBFC_CHANNEL_BASE_64r, &rval64,
                            POOL_ENf, field64);
    }

    BCM_IF_ERROR_RETURN
        (soc_reg64_set(unit, OOBFC_CHANNEL_BASE_64r, 0, 0, rval64));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_config_ipg_set
 * Purpose:
 *      Set OOB FC Tx inter packet gap
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      ipg - (IN) OOB FC Tx inter packet gap
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_tx_config_ipg_set(int unit, uint8 ipg)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, OOBFC_TX_IDLEr, 0, 0, &rval));
    soc_reg_field_set(unit, OOBFC_TX_IDLEr, &rval,
                      IDLE_GAPf, ipg);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, OOBFC_TX_IDLEr, 0, 0, rval));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_config_gcs_id_set
 * Purpose:
 *      Set OOB FC Tx global congestion reporting
 *      service pool id
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      id - (IN) OOB FC Tx global congestion service pool id
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_tx_config_gcs_id_set(int unit,
                             bcm_service_pool_id_t id)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, OOBFC_GCSr, 0, 0, &rval));
    soc_reg_field_set(unit, OOBFC_GCSr, &rval,
                      GCS_IDf, id);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, OOBFC_GCSr, 0, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_config_egr_mode_set
 * Purpose:
 *      Set OOB FC Tx egress congestion reporting mode
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      mode - (IN) OOB FC Tx egress congestion reporting mode
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_tx_config_egr_mode_set(int unit,
                             bcm_oob_egress_mode_t mode)
{
    uint64 rval64;
    uint64 field64;

    COMPILER_64_ZERO(field64);

    BCM_IF_ERROR_RETURN
        (soc_reg64_get(unit, OOBFC_CHANNEL_BASE_64r,
                       0, 0, &rval64));
    COMPILER_64_SET(field64, 0, mode);
    soc_reg64_field_set(unit, OOBFC_CHANNEL_BASE_64r, &rval64,
                        ENG_MODEf, field64);
    BCM_IF_ERROR_RETURN
        (soc_reg64_set(unit, OOBFC_CHANNEL_BASE_64r,
                       0, 0, rval64));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_config_flags_get
 * Purpose:
 *      Get OOB FC Tx enable status
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_tx_config_flags_get(int unit,
                              bcm_oob_fc_tx_config_t *config)
{
    uint32 rval = 0;
    uint64 rval64;
    uint64 regval64;

    COMPILER_64_ZERO(rval64);
    COMPILER_64_ZERO(regval64);

    config->flags = 0x0;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, INTFO_FCN_ENr, 0, 0, &rval));

    if (soc_reg_field_get(unit, INTFO_FCN_ENr, rval, ENf)) {
        config->flags |= BCM_OOB_FC_TX_FCN_EN;
    }

    BCM_IF_ERROR_RETURN
        (soc_reg64_get(unit, OOBFC_CHANNEL_BASE_64r,
                       0, 0, &rval64));

    regval64 = soc_reg64_field_get(unit, OOBFC_CHANNEL_BASE_64r,
                            rval64, ING_ENf);
    if (!COMPILER_64_IS_ZERO(regval64)) {
        config->flags |= BCM_OOB_FC_TX_ING_EN;
    }

    regval64 = soc_reg64_field_get(unit, OOBFC_CHANNEL_BASE_64r,
                            rval64, ENG_ENf);
    if (!COMPILER_64_IS_ZERO(regval64)) {
        config->flags |= BCM_OOB_FC_TX_EGR_EN;
    }

    if (SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWKPLUS(unit)) {
        regval64 = soc_reg64_field_get(unit, OOBFC_CHANNEL_BASE_64r,
                                rval64, MC_ENf);
    } else {
        regval64 = soc_reg64_field_get(unit, OOBFC_CHANNEL_BASE_64r,
                                rval64, POOL_ENf);
    }

    if (!COMPILER_64_IS_ZERO(regval64)) {
        config->flags |= BCM_OOB_FC_TX_POOL_EN;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_config_ipg_get
 * Purpose:
 *      Get OOB FC Tx inter packet gap
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_tx_config_ipg_get(int unit,
                            bcm_oob_fc_tx_config_t *config)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, OOBFC_TX_IDLEr, 0, 0, &rval));

    config->inter_pkt_gap = soc_reg_field_get(unit, OOBFC_TX_IDLEr,
                                              rval, IDLE_GAPf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_config_gcs_id_get
 * Purpose:
 *      Get OOB FC Tx global congestion reporting
 *      service pool id
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_tx_config_gcs_id_get(int unit,
                              bcm_oob_fc_tx_config_t *config)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, OOBFC_GCSr, 0, 0, &rval));

    config->gcs_id = soc_reg_field_get(unit, OOBFC_GCSr, rval,
                                       GCS_IDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_config_egr_mode_get
 * Purpose:
 *      Get OOB FC Tx egress congestion reporting mode
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_tx_config_egr_mode_get(int unit,
                                  bcm_oob_fc_tx_config_t *config)
{
    uint64 rval64;

    BCM_IF_ERROR_RETURN
        (soc_reg64_get(unit, OOBFC_CHANNEL_BASE_64r,
                       0, 0, &rval64));

    config->egr_mode = soc_reg64_field32_get(unit, OOBFC_CHANNEL_BASE_64r,
                                             rval64, ENG_MODEf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_localport_resolve
 * Purpose:
 *      Get the logical port from modport gport
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      gport - (IN) Modport gport.
 *      local_port - (OUT) Logical port number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_localport_resolve(int unit, bcm_gport_t gport,
                        bcm_port_t *local_port)
{
    bcm_module_t module;
    bcm_port_t port;
    bcm_trunk_t trunk;
    int id, result;

    if (!BCM_GPORT_IS_SET(gport)) {
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
        *local_port = gport;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &module, &port, &trunk, &id));

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, module, &result));
    if (result == FALSE) {
        return BCM_E_PARAM;
    }

    *local_port = port;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_get_intf_info
 * Purpose:
 *      Get the per OOBFC Rx interface info structure
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      info - (OUT) Per OOBFC RX interface info structure.
 *      intf_id - (IN) OOBFC Rx Interface ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_get_intf_info(int unit, _bcm_th_oob_fc_rx_intf_info_t *info,
                            bcm_oob_fc_rx_intf_id_t intf_id)
{
    uint32 rval = 0x0;
    soc_reg_t cfg_reg[_TH_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFG0r,
                                                 OOBFC_CHIF_CFG1r,
                                                 OOBFC_CHIF_CFG2r,
                                                 OOBFC_CHIF_CFG3r};

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, cfg_reg[intf_id], 0, 0, &rval));
        
    info->enable = soc_reg_field_get(unit, cfg_reg[intf_id], rval,
                                     OOBFC_CH_ENf);
    if (info->enable == 1) {
        info->num_ports = soc_reg_field_get(unit, cfg_reg[intf_id], rval,
                                            CH_PORT_NUMf);
        info->lookup_base = soc_reg_field_get(unit, cfg_reg[intf_id], rval,
                                              LOOKUP_BASEf);
        info->channel_base = soc_reg_field_get(unit, cfg_reg[intf_id], rval,
                                               OOBFC_CH_BASEf);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_disable_intf
 * Purpose:
 *      Disable OOBFC Rx interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      intf_id - (IN) OOBFC Rx Interface ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_disable_intf(int unit,
                           bcm_oob_fc_rx_intf_id_t intf_id)
{
    uint32 rval = 0x0;
    soc_reg_t cfg_reg[_TH_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFG0r,
                                                 OOBFC_CHIF_CFG1r,
                                                 OOBFC_CHIF_CFG2r,
                                                 OOBFC_CHIF_CFG3r};

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, cfg_reg[intf_id], 0, 0, &rval));
    soc_reg_field_set(unit, cfg_reg[intf_id], &rval,
                      OOBFC_CH_ENf, 0);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, cfg_reg[intf_id], 0, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_enable_intf
 * Purpose:
 *      Enable OOBFC Rx interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      intf_id - (IN) OOBFC Rx Interface ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_enable_intf(int unit,
                          bcm_oob_fc_rx_intf_id_t intf_id)
{
    uint32 rval = 0x0;
    soc_reg_t cfg_reg[_TH_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFG0r,
                                                 OOBFC_CHIF_CFG1r,
                                                 OOBFC_CHIF_CFG2r,
                                                 OOBFC_CHIF_CFG3r};

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, cfg_reg[intf_id], 0, 0, &rval));

    soc_reg_field_set(unit, cfg_reg[intf_id], &rval,
                      OOBFC_CH_ENf, 1);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, cfg_reg[intf_id], 0, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_intf_get_lookup_base
 * Purpose:
 *      Get the look base in OOBFC Rx Memory for
 *      given OOBFC Rx interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      intf_id - (IN) OOBFC Rx Interface ID.
 *      look_base - (OUT) Lookup base in OOBFC Rx Memory
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_intf_get_lookup_base(int unit,
                                bcm_oob_fc_rx_intf_id_t intf_id,
                                uint32 *lookup_base)
{
    uint32 rval = 0x0;
    soc_reg_t cfg_reg[_TH_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFG0r,
                                                 OOBFC_CHIF_CFG1r,
                                                 OOBFC_CHIF_CFG2r,
                                                 OOBFC_CHIF_CFG3r};

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, cfg_reg[intf_id], 0, 0, &rval));

    *lookup_base = soc_reg_field_get(unit, cfg_reg[intf_id], rval,
                                     LOOKUP_BASEf);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th_oob_fc_tx_config_set
 * Purpose:
 *      Set OOB FC Tx global configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (IN) OOB FC Tx global configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_oob_fc_tx_config_set(int unit,
                            bcm_oob_fc_tx_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if ((config->gcs_id < 0) && (config->gcs_id >= _TH_MMU_NUM_POOL)) {
        return BCM_E_PARAM;
    }

    if ((config->egr_mode < 0) && (config->egr_mode >= bcmOOBFCTxReportCount)) {
        return BCM_E_PARAM;
    }

    /* Enabling/Disabling FCN, OOBFC Tx Ingress, Egress and Service Pool */ 
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_tx_config_flags_set(unit, config->flags));

    /* Setting Inter packet Gap between two HCFC message */
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_tx_config_ipg_set(unit, config->inter_pkt_gap));

    /* Setting service pool id whose congestion state will be
       reported in every HCFC message */
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_tx_config_gcs_id_set(unit, config->gcs_id));

    /* Setting Egress mode for reporting OOBFC message. */
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_tx_config_egr_mode_set(unit, config->egr_mode));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_fc_tx_config_get
 * Purpose:
 *      Get OOB FC Tx global configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx global configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_oob_fc_tx_config_get(int unit,
                            bcm_oob_fc_tx_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the congestion state reporting status of FCN,
       OOBFC Tx Ingress, Egress and Service Pool */
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_tx_config_flags_get(unit, config));

    /* Get the configured Inter Packet Gap between OOBFC Tx message.*/
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_tx_config_ipg_get(unit, config));

    /* Setting service pool id whose congestion state will be
       reported in every HCFC message */
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_tx_config_gcs_id_get(unit, config));

    /* Get the configured Egress mode of OOBFC Tx side */
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_tx_config_egr_mode_get(unit, config));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_port_control_set
 * Purpose:
 *      Set OOB FC Tx Port configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      gport - (IN) Modport Gport
 *      status - (IN) Status of Congestion State reporting 
 *      dir - (IN) Direction 0 = Ingreess, 1 = Egress
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_oob_fc_tx_port_control_set(int unit, bcm_port_t localport,
                              int status, int dir)
{
    uint32 rval = 0;
    uint64 rval64;
    static const soc_field_t field[2] = {ING_PORT_ENf, ENG_PORT_ENf};
    static const soc_reg_t reg[2][3] = {
        {OOBFC_ING_PORT_EN0_64r, OOBFC_ING_PORT_EN1_64r, OOBFC_ING_PORT_EN2r},
        {OOBFC_ENG_PORT_EN0_64r, OOBFC_ENG_PORT_EN1_64r, OOBFC_ENG_PORT_EN2r}};

    if ((status != 1) && (status != 0)) {
        return BCM_E_PARAM;
    }

    if (dir > 1) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(rval64);

    if ((localport >= 0) && (localport < 64)) {
        /* For logical port 0-63 */
        if (dir == 1) {
            /* Setting per egress port QSEL only for the first time when
               enable/disable congestion state reporting of OOBFC */
            BCM_IF_ERROR_RETURN
                (soc_reg64_get(unit, OOBFC_ENG_PORT_QSEL0_64r,
                               0, 0, &rval64));
            if (!COMPILER_64_BITTEST(rval64, localport)) {
                COMPILER_64_BITSET(rval64, localport);
                soc_reg64_field_set(unit, OOBFC_ENG_PORT_QSEL0_64r, &rval64,
                                    ENG_PORT_QSELf, rval64);
                BCM_IF_ERROR_RETURN
                    (soc_reg64_set(unit, OOBFC_ENG_PORT_QSEL0_64r,
                                   0, 0, rval64));
            }
        }

        BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg[dir][0], 0, 0, &rval64));
        if (((status == 1) && (!COMPILER_64_BITTEST(rval64, localport))) ||
            ((status == 0) && (COMPILER_64_BITTEST(rval64, localport)))) {
            if (status == 1) {
                COMPILER_64_BITSET(rval64, localport);
            } else {
                COMPILER_64_BITCLR(rval64, localport);
            }
            soc_reg64_field_set(unit, reg[dir][0], &rval64,
                                field[dir], rval64);
            BCM_IF_ERROR_RETURN
                (soc_reg64_set(unit, reg[dir][0], 0, 0, rval64));
        }
    } else if ((localport >= 64) && (localport < 128)) {
        /* For logical port 64-127 */
        if (dir == 1) {
            /* Setting per egress port QSEL only for the first time when
               enable/disable congestion state reporting of OOBFC */
            BCM_IF_ERROR_RETURN
                (soc_reg64_get(unit, OOBFC_ENG_PORT_QSEL1_64r,
                               0, 0, &rval64));
            if (!COMPILER_64_BITTEST(rval64, (localport % 64))) {
                COMPILER_64_BITSET(rval64, (localport % 64));
                soc_reg64_field_set(unit, OOBFC_ENG_PORT_QSEL1_64r, &rval64,
                                    ENG_PORT_QSELf, rval64);
                BCM_IF_ERROR_RETURN
                    (soc_reg64_set(unit, OOBFC_ENG_PORT_QSEL1_64r,
                                   0, 0, rval64));
            }
        }

        BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg[dir][1], 0, 0, &rval64));
        if (((status == 1) && (!COMPILER_64_BITTEST(rval64, (localport % 64)))) ||
            ((status == 0) && (COMPILER_64_BITTEST(rval64, (localport % 64))))) {
            if (status == 1) {
                COMPILER_64_BITSET(rval64, (localport % 64));
            } else {
                COMPILER_64_BITCLR(rval64, (localport % 64));
            }
            soc_reg64_field_set(unit, reg[dir][1], &rval64,
                                field[dir], rval64);
            BCM_IF_ERROR_RETURN
                (soc_reg64_set(unit, reg[dir][1], 0, 0, rval64));
        }
    } else if ((localport >= 128) && (localport < 136)) {
        /* For logical port 128-135 */
        if (dir == 1) {
            /* Setting per egress port QSEL only for the first time when
               enable/disable congestion state reporting of OOBFC */
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, OOBFC_ENG_PORT_QSEL2r,
                               0, 0, &rval));
            if (!(rval & (0x1 << (localport % 128)))) {
                rval |= 0x1 << (localport % 128);
                rval &= 0xFF;
                soc_reg_field_set(unit, OOBFC_ENG_PORT_QSEL2r, &rval,
                                    ENG_PORT_QSELf, rval);
                BCM_IF_ERROR_RETURN
                    (soc_reg32_set(unit, OOBFC_ENG_PORT_QSEL2r,
                                   0, 0, rval));
            }
        }

        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg[dir][2], 0, 0, &rval));
        if (((status == 1) && (!(rval & (0x1 << (localport % 128))))) ||
            ((status == 0) && ((rval & (0x1 << (localport % 128)))))) {
            if (status == 1) {
                rval |= 0x1 << (localport % 128);
                rval &= 0xFF;
            } else {
                rval &= ~(0x1 << (localport % 128));
                rval &= 0xFF;
            }
            soc_reg_field_set(unit, reg[dir][2], &rval, field[dir], rval);

            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg[dir][2], 0, 0, rval));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_port_control_get
 * Purpose:
 *      Get OOB FC Tx side Port configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      gport - (IN) Modport Gport
 *      status - (OUT) Status of Congestion State reporting 
 *      dir - (IN) Direction 0 = Ingreess, 1 = Egress
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_oob_fc_tx_port_control_get(int unit, bcm_port_t localport,
                                 int *status, int dir)
{
    uint32 rval = 0;
    uint64 rval64;
    static const soc_reg_t reg[2][3] = {
        {OOBFC_ING_PORT_EN0_64r, OOBFC_ING_PORT_EN1_64r, OOBFC_ING_PORT_EN2r},
        {OOBFC_ENG_PORT_EN0_64r, OOBFC_ENG_PORT_EN1_64r, OOBFC_ENG_PORT_EN2r}};

    if (dir > 1) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(rval64);

    if ((localport >= 0) && (localport < 64)) {
        /* For logical port 0-63 */
        BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg[dir][0], 0, 0, &rval64));
        if (COMPILER_64_BITTEST(rval64, localport)) {
            *status = 0x1;
        } else {
            *status = 0x0;
        }
    } else if ((localport >= 64) && (localport < 128)) {
        /* For logical port 64-127 */
        BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg[dir][1], 0, 0, &rval64));

        if (COMPILER_64_BITTEST(rval64, (localport % 64))) {
            *status = 0x1;
        } else {
            *status = 0x0;
        }
    } else if ((localport >= 128) && (localport < 136)) {
        /* For logical port 128-135 */
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg[dir][2], 0, 0, &rval));
        if (rval & (0x1 << (localport % 128))) {
            *status = 0x1;
        } else {
            *status = 0x0;
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
* Function:
*      bcm_th_oob_fc_tx_info_get
* Purpose:
*      Get OOB FC Tx global information.
* Parameters:
*      unit - (IN) StrataSwitch unit number.
*      info - (OUT) OOB FC Tx global information
* Returns:
*      BCM_E_XXX
*/
int
bcm_th_oob_fc_tx_info_get(int unit, bcm_oob_fc_tx_info_t *info)
{
    uint64 rval64;

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_reg64_get(unit, OOBFC_CHANNEL_BASE_64r,
                       0, 0, &rval64));

    /* Get the Channel base value for HCFC messages containing
       congestion state of different resources */
    /* Ingress Ports */
    info->ing_base =  (uint8)soc_reg64_field32_get(unit, OOBFC_CHANNEL_BASE_64r,
                                                   rval64, ING_BASEf);

    /* Egress Unicast Queue */
    info->ucast_base =  (uint8)soc_reg64_field32_get(unit, OOBFC_CHANNEL_BASE_64r,
                                                     rval64, ENGU_BASEf);

    /* Egress Multicast Queue */
    info->mcast_base =  (uint8)soc_reg64_field32_get(unit, OOBFC_CHANNEL_BASE_64r,
                                                     rval64, ENGM_BASEf);

    /* Egress Cos Queue */
    info->cos_base =  (uint8)soc_reg64_field32_get(unit, OOBFC_CHANNEL_BASE_64r,
                                                   rval64, ENGG_BASEf);

    /* Ingress and Egress Service Pool Congestion */
    if (SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWKPLUS(unit)) {
        info->pool_base =  (uint8)soc_reg64_field32_get(unit, OOBFC_CHANNEL_BASE_64r,
                                                      rval64, MC_BASEf);
    } else {
        info->pool_base =  (uint8)soc_reg64_field32_get(unit, OOBFC_CHANNEL_BASE_64r,
                                                      rval64, POOL_BASEf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_config_enable_set
 * Purpose:
 *      Enable and configure the given OOB FC Rx Interface
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      channel_base - (IN) HCFC channel base
 *      array_count - (IN) Number of gports in the gport array
 *      gport_array - (IN) Array of modport type gport
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_config_enable_set(int unit,
                            bcm_oob_fc_rx_intf_id_t intf_id,
                            uint8 channel_base,
                            int array_count,
                            bcm_gport_t *gport_array)
{
    _bcm_th_oob_fc_rx_intf_info_t info[_TH_NUM_OOB_FC_RX_INTF];
    char *sysport_buf, *tc2pri_buf;
    uint32 sysport_list[_TH_MAX_RX_MEM_ENTRY];
    int rv = BCM_E_INTERNAL;
    void *pentry1, *pentry2;
    uint32 rval = 0x0;
    int mem_wsz = 0;
    int i = 0, total_num_ports = 0;
    bcm_port_t localport;
    uint64 tc_to_pri_mapping;
    uint32 tc_to_pri_mapping_low = 0x0;
    uint32 tc_to_pri_mapping_high = 0x0;
    soc_reg_t cfg_reg[_TH_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFG0r,
                                                 OOBFC_CHIF_CFG1r,
                                                 OOBFC_CHIF_CFG2r,
                                                 OOBFC_CHIF_CFG3r};

    COMPILER_64_ZERO(tc_to_pri_mapping);

    sal_memset(info, 0, sizeof(_bcm_th_oob_fc_rx_intf_info_t) *
               _TH_NUM_OOB_FC_RX_INTF);
    sal_memset(sysport_list, 0, sizeof(uint32) *
               _TH_MAX_RX_MEM_ENTRY);

    /* Backup the per OOBFC Rx interface configuration */
    for (i = 0; i < _TH_NUM_OOB_FC_RX_INTF; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_th_oob_fc_rx_get_intf_info(unit, &info[i], i));
        /* Compute the total number of ports which is
           already configured */
        if (info[i].enable == 1) {
            total_num_ports += info[i].num_ports;
        }
    }

    /* Return error if trying to enable already enabled
       OOBFC Rx Interface */
    if (info[intf_id].enable == 1) {
        return BCM_E_PARAM;
    }

    /* Return error if there is no room in memory to accomodate
       the list of ports which need to programmed in memory */
    if ((total_num_ports + array_count) > _TH_MAX_RX_MEM_ENTRY) {
        return BCM_E_PARAM;
    }

    /* Read the MMU_CHFC_SYSPORT_MAPPING memory into sysport_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_CHFC_SYSPORT_MAPPINGm);
    sysport_buf = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                "oob sysport_buf");
    if (sysport_buf == NULL) {
        return BCM_E_MEMORY;
    }

    if (soc_mem_read_range(unit, MMU_CHFC_SYSPORT_MAPPINGm, MEM_BLOCK_ALL,
                           0, _TH_MAX_RX_MEM_ENTRY - 1, sysport_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    /* Read the MMU_INTFO_TC2PRI_MAPPING memory into tc2pri_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_INTFO_TC2PRI_MAPPINGm);
    tc2pri_buf = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                               "oob tc2pri_buf");
    if (tc2pri_buf == NULL) {
        rv = BCM_E_MEMORY;
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    if (soc_mem_read_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                           0, _TH_MAX_RX_MEM_ENTRY - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    /* Creating a list of logical port number from a list of
       modport gport */
    for (i = 0; i < array_count; i++) {
        rv = _bcm_th_oob_localport_resolve(unit, gport_array[i],
                                           &localport);
        if (rv != BCM_E_NONE) {
            soc_cm_sfree(unit, tc2pri_buf);
            soc_cm_sfree(unit, sysport_buf);
            return rv;
        }
        sysport_list[i] = localport;
    }

    /* Fill the info structure for the given interface */
    info[intf_id].enable = 1;
    info[intf_id].channel_base = channel_base;
    info[intf_id].lookup_base = total_num_ports;
    info[intf_id].num_ports = array_count;

    /* Program sysport_list in MMU_CHFC_SYSPORT_MAPPING */
    /* Program default value into MMU_INTFO_TC2PRI_MAPPING */
    for (i = 0; i < array_count; i++) {
        pentry1 = soc_mem_table_idx_to_pointer(unit,
                                               MMU_CHFC_SYSPORT_MAPPINGm,
                                               void*, sysport_buf,
                                               (total_num_ports + i));
        pentry2 = soc_mem_table_idx_to_pointer(unit,
                                               MMU_INTFO_TC2PRI_MAPPINGm,
                                               void*, tc2pri_buf,
                                               (total_num_ports + i));
        soc_mem_field32_set(unit, MMU_CHFC_SYSPORT_MAPPINGm, pentry1,
                            SYS_PORTf, sysport_list[i]);
        /* TC to PRI Mapping Default Value is unity mapped
           TC=7 mapped PRI=7 0x80 (bits 63-56)
           TC=6 mapped PRI=6 0x40 (bits 55-48)
           TC=5 mapped PRI=5 0x20 (bits 47-40) 
           TC=4 mapped PRI=4 0x10 (bits 39-32)
           TC=3 mapped PRI=3 0x08 (bits 31-24)
           TC=2 mapped PRI=2 0x04 (bits 23-16)
           TC=1 mapped PRI=1 0x02 (bits 15-8)
           TC=0 mapped PRI=0 0x01 (bits 7-0)
         */
        tc_to_pri_mapping_high |= 0x80 << 24;
        tc_to_pri_mapping_high |= 0x40 << 16;
        tc_to_pri_mapping_high |= 0x20 << 8;
        tc_to_pri_mapping_high |= 0x10;
        tc_to_pri_mapping_low |= 0x08 << 24;
        tc_to_pri_mapping_low |= 0x04 << 16;
        tc_to_pri_mapping_low |= 0x02 << 8;
        tc_to_pri_mapping_low |= 0x01;

        COMPILER_64_SET(tc_to_pri_mapping, tc_to_pri_mapping_high,
                        tc_to_pri_mapping_low);

        soc_mem_field64_set(unit, MMU_INTFO_TC2PRI_MAPPINGm,
                            pentry2, PRI_BMPf, tc_to_pri_mapping);
    }

    if (soc_mem_write_range(unit, MMU_CHFC_SYSPORT_MAPPINGm, MEM_BLOCK_ALL,
                            0, _TH_MAX_RX_MEM_ENTRY - 1, sysport_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    if (soc_mem_write_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                            0, _TH_MAX_RX_MEM_ENTRY - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    /* Program the given interface configuration register.*/
    rv = soc_reg32_get(unit, cfg_reg[intf_id],
                       0, 0, &rval);
    if (rv != BCM_E_NONE) {
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }
    soc_reg_field_set(unit, cfg_reg[intf_id], &rval,
                      OOBFC_CH_ENf, info[intf_id].enable);
    soc_reg_field_set(unit, cfg_reg[intf_id], &rval,
                      OOBFC_CH_BASEf, info[intf_id].channel_base);
    soc_reg_field_set(unit, cfg_reg[intf_id], &rval,
                      LOOKUP_BASEf, info[intf_id].lookup_base);
    soc_reg_field_set(unit, cfg_reg[intf_id], &rval,
                      CH_PORT_NUMf, info[intf_id].num_ports);
    rv = soc_reg32_set(unit, cfg_reg[intf_id],
                       0, 0, rval);
    if (rv != BCM_E_NONE) {
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    rv = BCM_E_NONE;
    soc_cm_sfree(unit, tc2pri_buf);
    soc_cm_sfree(unit, sysport_buf);
    return rv;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_port_offset_get
 * Purpose:
 *      Get the channel offset value for a given port
 *      in the HCFC message received on a given OOB FC
 *      Rx Interface
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport gport
 *      offset - (OUT) Channel offset value
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_port_offset_get(int unit,
                         bcm_oob_fc_rx_intf_id_t intf_id,
                         bcm_gport_t gport,
                         uint32 *offset)
{
    bcm_oob_fc_rx_config_t config;
    bcm_gport_t gport_array[_TH_MAX_RX_MEM_ENTRY] = {0x0};
    int array_count = 0, i = 0;

    /* Get the configured list of gport array
       and array count*/
    BCM_IF_ERROR_RETURN
        (bcm_th_oob_fc_rx_config_get(unit, intf_id, &config,
                                     _TH_MAX_RX_MEM_ENTRY, gport_array,
                                     &array_count));

    if (config.enable == 0) {
        return BCM_E_PARAM;
    }

    /* Get the Channel offset of the port in HCFC message */
    for (i = 0; i < array_count; i++) {
        if (gport == gport_array[i]) {
            break;
        }
    }

    /* Return error if gport is not there in the list */
    if (i == array_count) {
        return BCM_E_NOT_FOUND;
    }

    *offset = i;
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_tc_mapping_get
 * Purpose:
 *      Get the traffic class to priority
 *      mapping for given OOB FC Rx Interface at
 *      given idx in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      tc - (IN) Traffic Class array[0-7]
 *      pri_bmp - (OUT) Priority Bitmap[0-7]
 *      idx - (IN) Index in TC to PRI mapping memory
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_tc_mapping_get(int unit,
                        bcm_oob_fc_rx_intf_id_t intf_id,
                        uint32 tc, uint32 *pri_bmp,
                        uint32 idx)
{
    char *tc2pri_buf;
    uint64 tc2pri;
    void *pentry;
    int mem_wsz = 0;

    COMPILER_64_ZERO(tc2pri);

    /* Read the MMU_INTFO_TC2PRI_MAPPING memory into tc2pri_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_INTFO_TC2PRI_MAPPINGm);
    tc2pri_buf = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                         "oob tc2pri_buf");
    if (tc2pri_buf == NULL) {
        return BCM_E_MEMORY;
    }

    if (soc_mem_read_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                           0, _TH_MAX_RX_MEM_ENTRY - 1, tc2pri_buf)) {
        soc_cm_sfree(unit, tc2pri_buf);
        return BCM_E_INTERNAL;
    }

    /* Get the pointer to the given index. */
    pentry = soc_mem_table_idx_to_pointer(unit,
                                          MMU_INTFO_TC2PRI_MAPPINGm,
                                          void*, tc2pri_buf, idx);

    soc_mem_field64_get(unit, MMU_INTFO_TC2PRI_MAPPINGm,
                        pentry, PRI_BMPf, &tc2pri);

    /* Get tc2pri value */
    COMPILER_64_SHR(tc2pri, (8 * tc));
    *pri_bmp = COMPILER_64_LO(tc2pri) & 0xFF;

    soc_cm_sfree(unit, tc2pri_buf);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_tc_mapping_set
 * Purpose:
 *      Set the traffic class to priority
 *      mapping for given OOB FC Rx Interface at
 *      given idx in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      tc - (IN) Traffic Class array[0-7]
 *      pri_bmp - (IN) Priority Bitmap[0-7]
 *      idx - (IN) Index in TC to PRI mapping memory
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_tc_mapping_set(int unit,
                          bcm_oob_fc_rx_intf_id_t intf_id, uint32 tc,
                          uint32 pri_bmp, uint32 idx)
{
    char *tc2pri_buf;
    uint64 tc2pri;
    uint64 mask64;
    uint64 pri_bmp64;
    int rv = BCM_E_INTERNAL;
    void *pentry;
    int mem_wsz = 0;

    COMPILER_64_ZERO(tc2pri);
    COMPILER_64_ZERO(mask64);
    COMPILER_64_ZERO(pri_bmp64);

    /* Read the MMU_INTFO_TC2PRI_MAPPING memory into tc2pri_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_INTFO_TC2PRI_MAPPINGm);
    tc2pri_buf = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                         "oob tc2pri_buf");
    if (tc2pri_buf == NULL) {
        return BCM_E_MEMORY;
    }

    if (soc_mem_read_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                           0, _TH_MAX_RX_MEM_ENTRY - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, tc2pri_buf);
        return rv;
    }

    /* Get the pointer to the given index. */
    pentry = soc_mem_table_idx_to_pointer(unit,
                                          MMU_INTFO_TC2PRI_MAPPINGm,
                                          void*, tc2pri_buf, idx);

    soc_mem_field64_get(unit, MMU_INTFO_TC2PRI_MAPPINGm,
                        pentry, PRI_BMPf, &tc2pri);


    /* Disable the given OOBFC Rx interface */
    rv = _bcm_th_oob_fc_rx_disable_intf(unit, intf_id);
    if (rv != BCM_E_NONE) {
        soc_cm_sfree(unit, tc2pri_buf);
        return rv;
    }

    /* Update tc2pri value */
    COMPILER_64_SET(pri_bmp64, 0, pri_bmp);
    COMPILER_64_SHL(pri_bmp64, (tc * 8));
    COMPILER_64_MASK_CREATE(mask64, 8, (tc * 8));
    COMPILER_64_NOT(mask64);
    COMPILER_64_AND(tc2pri, mask64);
    COMPILER_64_OR(tc2pri, pri_bmp64);

    /* Program tc2pri MMU_INTFO_TC2PRI_MAPPING */
    soc_mem_field64_set(unit, MMU_INTFO_TC2PRI_MAPPINGm,
                        pentry, PRI_BMPf, tc2pri);

    if (soc_mem_write_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                            0, _TH_MAX_RX_MEM_ENTRY - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, tc2pri_buf);
        return rv;
    }

    /* Enable the given OOBFC Rx interface back */
    rv = _bcm_th_oob_fc_rx_enable_intf(unit, intf_id);
    if (rv != BCM_E_NONE) {
        soc_cm_sfree(unit, tc2pri_buf);
        return rv;
    }

    rv = BCM_E_NONE;
    soc_cm_sfree(unit, tc2pri_buf);
    return rv;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_config_disable_set
 * Purpose:
 *      Disable and reset the configuration of the
 *      given OOB FC Rx Interface
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_config_disable_set(int unit,
                            bcm_oob_fc_rx_intf_id_t intf_id)
{
    _bcm_th_oob_fc_rx_intf_info_t old_info[_TH_NUM_OOB_FC_RX_INTF];
    _bcm_th_oob_fc_rx_intf_info_t new_info[_TH_NUM_OOB_FC_RX_INTF];
    char *sysport_buf = NULL, *tc2pri_buf = NULL;
    uint32 *old_sysport_list = NULL;
    uint64 *old_tc2pri_list = NULL;
    uint32 *new_sysport_list = NULL;
    uint64 *new_tc2pri_list = NULL;
    int rv = BCM_E_INTERNAL;
    int mem_wsz = 0;
    int i = 0, j = 0, k = 0;
    void *pentry;
    uint32 rval = 0x0;
    soc_reg_t cfg_reg[_TH_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFG0r,
                                                 OOBFC_CHIF_CFG1r,
                                                 OOBFC_CHIF_CFG2r,
                                                 OOBFC_CHIF_CFG3r};

    sal_memset(old_info, 0, sizeof(_bcm_th_oob_fc_rx_intf_info_t) *
               _TH_NUM_OOB_FC_RX_INTF);
    sal_memset(new_info, 0, sizeof(_bcm_th_oob_fc_rx_intf_info_t) *
               _TH_NUM_OOB_FC_RX_INTF);

    mem_wsz = sizeof(uint32);
    old_sysport_list = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                         "oob old_sysport_list");
    if (old_sysport_list == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    mem_wsz = sizeof(uint64);
    old_tc2pri_list = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                         "oob old_tc2pri_list");
    if (old_tc2pri_list == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    mem_wsz = sizeof(uint32);
    new_sysport_list = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                         "oob new_sysport_list");
    if (new_sysport_list == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    mem_wsz = sizeof(uint64);
    new_tc2pri_list = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                         "oob new_tc2pri_list");
    if (new_tc2pri_list == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    sal_memset(old_sysport_list, 0, sizeof(uint32) *
               _TH_MAX_RX_MEM_ENTRY);
    sal_memset(new_sysport_list, 0, sizeof(uint32) *
               _TH_MAX_RX_MEM_ENTRY);
    sal_memset(old_tc2pri_list, 0, sizeof(uint64) *
               _TH_MAX_RX_MEM_ENTRY);
    sal_memset(new_tc2pri_list, 0, sizeof(uint64) *
               _TH_MAX_RX_MEM_ENTRY);

    /* Store all the OOBFC Rx interface configuration */
    for (i = 0; i < _TH_NUM_OOB_FC_RX_INTF; i++) {
        rv = _bcm_th_oob_fc_rx_get_intf_info(unit, &old_info[i], i);
        if (rv != BCM_E_NONE) {
            goto cleanup;
        }
    }

    /* Read the MMU_CHFC_SYSPORT_MAPPING memory into sysport_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_CHFC_SYSPORT_MAPPINGm);
    sysport_buf = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                         "oob sysport_buf");
    if (sysport_buf == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    if (soc_mem_read_range(unit, MMU_CHFC_SYSPORT_MAPPINGm, MEM_BLOCK_ALL,
                           0, _TH_MAX_RX_MEM_ENTRY - 1, sysport_buf)) {
        rv = BCM_E_INTERNAL;
        goto cleanup;
    }

    /* Store the sysport_buf in old_sysport_list */
    for (i = 0; i < _TH_MAX_RX_MEM_ENTRY; i++) {
        pentry = soc_mem_table_idx_to_pointer(unit,
                                              MMU_CHFC_SYSPORT_MAPPINGm,
                                              void*, sysport_buf, i);
        old_sysport_list[i] = soc_mem_field32_get(unit,
                                                  MMU_CHFC_SYSPORT_MAPPINGm,
                                                  pentry, SYS_PORTf);
    }

    /* Read the MMU_INTFO_TC2PRI_MAPPING memory into tc2pri_list */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_INTFO_TC2PRI_MAPPINGm);
    tc2pri_buf = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                         "oob tc2pri_buf");
    if (tc2pri_buf == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    if (soc_mem_read_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                           0, _TH_MAX_RX_MEM_ENTRY - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        goto cleanup;
    }

    /* Store the tc2pri_buf in old_tc2pri_list */
    for (i = 0; i < _TH_MAX_RX_MEM_ENTRY; i++) {
        pentry = soc_mem_table_idx_to_pointer(unit,
                                              MMU_INTFO_TC2PRI_MAPPINGm,
                                              void*, tc2pri_buf, i);
        soc_mem_field64_get(unit, MMU_INTFO_TC2PRI_MAPPINGm,
                            pentry, PRI_BMPf, &old_tc2pri_list[i]);
    }

    /* Disable all the OOBFC Rx interface */
    for (i = 0; i < _TH_NUM_OOB_FC_RX_INTF; i++) {
        if (old_info[i].enable == 1) {
            rv = _bcm_th_oob_fc_rx_disable_intf(unit, i);
            if (rv != BCM_E_NONE) {
                goto cleanup;
            }
        }
    }

    /* Generate new_info */
    /* Genrate new_sysport_list */
    /* Genrate new_tc2pri_list */
    j = 0;
    k = 0;
    for (i = 0; i < _TH_NUM_OOB_FC_RX_INTF; i++) {
        if (i != intf_id) {
            new_info[i].enable = old_info[i].enable;
            new_info[i].channel_base = old_info[i].channel_base;
            new_info[i].num_ports = old_info[i].num_ports;
            if (new_info[i].enable) {
                new_info[i].lookup_base = j;
            } else {
                new_info[i].lookup_base = 0x0;
            }
            for (k = 0; k < new_info[i].num_ports; k++) {
                new_sysport_list[j] = old_sysport_list[old_info[i].lookup_base + k];
                new_tc2pri_list[j] = old_tc2pri_list[old_info[i].lookup_base + k];
                j++;
            }
        } else {
            new_info[i].enable = 0;
            new_info[i].channel_base = 0;
            new_info[i].lookup_base = 0;
            new_info[i].num_ports = 0;
        }
    }

    /* Program new_tc2pri_list in MMU_INTFO_TC2PRI_MAPPING */
    for (i = 0; i < _TH_MAX_RX_MEM_ENTRY; i++) {
        pentry = soc_mem_table_idx_to_pointer(unit,
                                              MMU_INTFO_TC2PRI_MAPPINGm,
                                              void*, tc2pri_buf, i);
        soc_mem_field64_set(unit, MMU_INTFO_TC2PRI_MAPPINGm,
                            pentry, PRI_BMPf, new_tc2pri_list[i]);
    }

    if (soc_mem_write_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                            0, _TH_MAX_RX_MEM_ENTRY - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        goto cleanup;
    }

    /* Program new_sysport_list MMU_CHFC_SYSPORT_MAPPING */
    for (i = 0; i < _TH_MAX_RX_MEM_ENTRY; i++) {
        pentry = soc_mem_table_idx_to_pointer(unit,
                                              MMU_CHFC_SYSPORT_MAPPINGm,
                                              void*, sysport_buf, i);
        soc_mem_field32_set(unit, MMU_CHFC_SYSPORT_MAPPINGm, pentry,
                            SYS_PORTf, new_sysport_list[i]);
    }

    if (soc_mem_write_range(unit, MMU_CHFC_SYSPORT_MAPPINGm, MEM_BLOCK_ALL,
                            0, _TH_MAX_RX_MEM_ENTRY - 1, sysport_buf)) {
        rv = BCM_E_INTERNAL;
        goto cleanup;
    }

    /* Program all the OOBFC Rx interface configuration register */
    for (i = 0; i < _TH_NUM_OOB_FC_RX_INTF; i++) {
        rv = soc_reg32_get(unit, cfg_reg[i],
                           0, 0, &rval);
        if (rv != BCM_E_NONE) {
            goto cleanup;
        }
        soc_reg_field_set(unit, cfg_reg[i], &rval,
                          OOBFC_CH_ENf, new_info[i].enable);
        soc_reg_field_set(unit, cfg_reg[i], &rval,
                          OOBFC_CH_BASEf, new_info[i].channel_base);
        soc_reg_field_set(unit, cfg_reg[i], &rval,
                          LOOKUP_BASEf, new_info[i].lookup_base);
        soc_reg_field_set(unit, cfg_reg[i], &rval,
                          CH_PORT_NUMf, new_info[i].num_ports);
        rv = soc_reg32_set(unit, cfg_reg[i],
                           0, 0, rval);
        if (rv != BCM_E_NONE) {
            goto cleanup;
        }
    }

    rv = BCM_E_NONE;
cleanup:
    if (old_sysport_list != NULL)
        soc_cm_sfree(unit, old_sysport_list);
    if (old_tc2pri_list != NULL)
        soc_cm_sfree(unit, old_tc2pri_list);
    if (new_sysport_list != NULL)
        soc_cm_sfree(unit, new_sysport_list);
    if (new_tc2pri_list != NULL)
        soc_cm_sfree(unit, new_tc2pri_list);
    if (tc2pri_buf != NULL)
        soc_cm_sfree(unit, tc2pri_buf);
    if (sysport_buf != NULL)
        soc_cm_sfree(unit, sysport_buf);

    return rv;
}

/*
 * Function:
 *      _bcm_th_oob_fc_rx_config_get
 * Purpose:
 *      Get the configuration and status
 *      each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      config - (OUT) OOB FC Rx Interface Config
 *      array_max - (IN) Maximum Lenght of Gport array passed
 *      gport_array - (OUT) Array of modport type gport
 *      array_count - (OUT) Number of gports returned in the gport array
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_fc_rx_config_get(int unit,
                 bcm_oob_fc_rx_intf_id_t intf_id,
                 bcm_oob_fc_rx_config_t *config,
                 int array_max,
                 bcm_gport_t *gport_array,
                 int *array_count)
{
    _bcm_th_oob_fc_rx_intf_info_t info;
    int rv = BCM_E_INTERNAL;
    char *sysport_buf;
    void *pentry;
    int mem_wsz = 0;
    int i = 0 , j = 0;
    bcm_gport_t gport;
    uint32 new_sysport_list[_TH_MAX_RX_MEM_ENTRY];

    sal_memset(&info, 0, sizeof(_bcm_th_oob_fc_rx_intf_info_t));
    sal_memset(new_sysport_list, 0, sizeof(uint32) *
               _TH_MAX_RX_MEM_ENTRY);
    
    /* Store all the OOBFC Rx interface configuration */
    BCM_IF_ERROR_RETURN
            (_bcm_th_oob_fc_rx_get_intf_info(unit, &info, intf_id));

    /* If given OOBFC Rx Interface is not enabled then
       return empty gport array */
    if (info.enable == 0) {
        sal_memset(config, 0, sizeof(bcm_oob_fc_rx_config_t));
        sal_memset(gport_array, 0, sizeof(bcm_gport_t) * array_max);
        *array_count = 0;
        return BCM_E_NONE;
    } else {
        config->enable = info.enable;
        config->channel_base = info.channel_base;
        /* Read the MMU_CHFC_SYSPORT_MAPPING memory into sysport_buf */
        mem_wsz = sizeof(uint32) *
                    soc_mem_entry_words(unit, MMU_CHFC_SYSPORT_MAPPINGm);
        sysport_buf = soc_cm_salloc(unit, _TH_MAX_RX_MEM_ENTRY * mem_wsz,
                                             "oob sysport_buf");
        if (sysport_buf == NULL) {
            return BCM_E_MEMORY;
        }

        if (soc_mem_read_range(unit, MMU_CHFC_SYSPORT_MAPPINGm, MEM_BLOCK_ALL,
                               0, _TH_MAX_RX_MEM_ENTRY - 1, sysport_buf)) {
            rv = BCM_E_INTERNAL;
            soc_cm_sfree(unit, sysport_buf);
            return rv;
        }

        if (array_max >= info.num_ports) {
            *array_count = info.num_ports;
        } else {
            *array_count = array_max;
        }

        /* Store the required number of indexes into a sysport_list */
        for (i = info.lookup_base, j = 0;
             i < (info.lookup_base + *array_count); i++, j++) {
            pentry = soc_mem_table_idx_to_pointer(unit,
                                                  MMU_CHFC_SYSPORT_MAPPINGm,
                                                  void*, sysport_buf, i);
            new_sysport_list[j] = soc_mem_field32_get(unit,
                                                      MMU_CHFC_SYSPORT_MAPPINGm,
                                                      pentry, SYS_PORTf);
        }

        /* Convert the sysport list into a modport gport list */
        for (i = 0; i < *array_count; i++) {
            rv = bcm_esw_port_gport_get(unit, new_sysport_list[i],
                                        &gport);
            if (rv != BCM_E_NONE) {
                soc_cm_sfree(unit, sysport_buf);
                return rv;
            }
            gport_array[i] = gport;
        }

    }

    rv = BCM_E_NONE;
    soc_cm_sfree(unit, sysport_buf);
    return rv;
}

/*
 * Function:
 *      bcm_th_oob_fc_rx_config_set
 * Purpose:
 *      Set the configuration and enable 
 *      each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      config - (IN) OOB FC Rx Interface Config
 *      array_count - (IN) Number of gports in the gport array
 *      gport_array - (IN) Array of modport type gport
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_oob_fc_rx_config_set(int unit,
                     bcm_oob_fc_rx_intf_id_t intf_id,
                     bcm_oob_fc_rx_config_t *config,
                     int array_count,
                     bcm_gport_t *gport_array)
{
    if ((intf_id < 0) || (intf_id >= _TH_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if ((config->enable != 0) && (config->enable != 1)) {
        return BCM_E_PARAM;
    }

    if ((config->enable == 1) &&
        ((array_count == 0) || (gport_array == NULL) ||
        (array_count > _TH_MAX_RX_MEM_ENTRY))) {
        return BCM_E_PARAM;
    }

    if ((config->enable == 1) &&
        ((config->channel_base > _TH_MAX_CHANNEL_BASE))) {
        return BCM_E_PARAM;
    }

    if (config->enable == 1) {
        /* Add entries into MMU_CHFC_SYSPORT_MAPPING memory */
        BCM_IF_ERROR_RETURN
            (_bcm_th_oob_fc_rx_config_enable_set(unit, intf_id,
                                                 config->channel_base,
                                                 array_count, gport_array));
    } else {
        /* Remove entries from MMU_CHFC_SYSPORT_MAPPING memory */
        BCM_IF_ERROR_RETURN
            (_bcm_th_oob_fc_rx_config_disable_set(unit, intf_id));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_fc_rx_config_get
 * Purpose:
 *      Get the configuration and status
 *      each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      config - (OUT) OOB FC Rx Interface Config
 *      array_max - (IN) Maximum Lenght of Gport array passed
 *      gport_array - (OUT) Array of modport type gport
 *      array_count - (OUT) Number of gports returned in the gport array
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_oob_fc_rx_config_get(int unit,
                     bcm_oob_fc_rx_intf_id_t intf_id,
                     bcm_oob_fc_rx_config_t *config,
                     int array_max,
                     bcm_gport_t *gport_array,
                     int *array_count)
{
    if ((intf_id < 0) || (intf_id >= _TH_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if ((gport_array == NULL) ||
        (config == NULL) ||
        (array_count == NULL)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_rx_config_get(unit, intf_id,
                                      config, array_max,
                                      gport_array,
                                      array_count));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_fc_rx_port_tc_mapping_multi_set
 * Purpose:
 *      Set the per port traffic class to priority
 *      mapping for each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      array_count - (IN) Number of elements in Traffic Class array
 *      tc - (IN) Traffic Class array[0-7]
 *      pri_bmp - (IN) Priority Bitmap Array[0-7]
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_oob_fc_rx_port_tc_mapping_multi_set(int unit,
                                 bcm_oob_fc_rx_intf_id_t intf_id,
                                 bcm_gport_t gport,
                                 int array_count,
                                 uint32 *tc,
                                 uint32 *pri_bmp)
{
    int i = 0;

    if ((intf_id < 0) || (intf_id >= _TH_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if ((array_count < 1) || (array_count > _TH_NUM_TC)) {
        return BCM_E_PARAM;
    }

    if ((tc == NULL) || (pri_bmp == NULL)) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < array_count; i++) {
        if ((tc[i] > (_TH_NUM_TC - 1)) || (pri_bmp[i] > 0xFF)) {
            return BCM_E_PARAM;
        }
    }

    for (i = 0; i < array_count; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_th_oob_fc_rx_port_tc_mapping_set(unit, intf_id,
                                                  gport, tc[i],
                                                  pri_bmp[i]));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_fc_rx_port_tc_mapping_multi_get
 * Purpose:
 *      Get the per port traffic class to priority
 *      mapping for each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      array_max - (IN) Maximum Length tc and pri_bmp array
 *      tc - (OUT) Traffic Class array[0-7]
 *      pri_bmp - (OUT) Priority Bitmap Array[0-7]
 *      array_count - (OUT) Number of elements in Traffic Class array
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_oob_fc_rx_port_tc_mapping_multi_get(int unit,
                                     bcm_oob_fc_rx_intf_id_t intf_id,
                                     bcm_gport_t gport,
                                     int array_max,
                                     uint32 *tc,
                                     uint32 *pri_bmp,
                                     int *array_count)
{
    int i = 0;

    if ((intf_id < 0) || (intf_id >= _TH_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if (array_max < 1) {
        return BCM_E_PARAM;
    }

    if ((array_count == NULL) ||
        (tc == NULL) ||
        (pri_bmp == NULL)) {
        return BCM_E_PARAM;
    }

    if (array_max > _TH_NUM_TC) {
        *array_count = _TH_NUM_TC;
    } else {
        *array_count = array_max;
    }

    for (i = 0; i < *array_count; i++) {
        if (tc[i] > (_TH_NUM_TC - 1)) {
            return BCM_E_PARAM;
        }
    }

    for (i = 0; i < *array_count; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_th_oob_fc_rx_port_tc_mapping_get(unit, intf_id,
                                                  gport, tc[i],
                                                  &pri_bmp[i]));
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th_oob_fc_rx_port_tc_mapping_set
 * Purpose:
 *      Set the per port traffic class to priority
 *      mapping for each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      tc - (IN) Traffic Class array[0-7]
 *      pri_bmp - (IN) Priority Bitmap[0-7]
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_oob_fc_rx_port_tc_mapping_set(int unit,
                                 bcm_oob_fc_rx_intf_id_t intf_id,
                                 bcm_gport_t gport,
                                 uint32 tc,
                                 uint32 pri_bmp)
{
    uint32 idx = 0;
    uint32 offset = 0;
    uint32 lookup_base = 0;

    if ((intf_id < 0) || (intf_id >= _TH_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if ((tc > (_TH_NUM_TC - 1)) || (pri_bmp > 0xFF)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_rx_port_offset_get(unit, intf_id,
                                           gport, &offset));

    /* Get lookup base */
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_rx_intf_get_lookup_base(unit, intf_id,
                                                &lookup_base));

    idx = offset + lookup_base;

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_rx_tc_mapping_set(unit, intf_id, tc,
                                          pri_bmp, idx));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_fc_rx_port_tc_mapping_get
 * Purpose:
 *      Set the per port traffic class to priority
 *      mapping for each OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      tc - (IN) Traffic Class array[0-7]
 *      pri_bmp - (OUT) Priority Bitmap[0-7]
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_oob_fc_rx_port_tc_mapping_get(int unit,
                                 bcm_oob_fc_rx_intf_id_t intf_id,
                                 bcm_gport_t gport,
                                 uint32 tc,
                                 uint32 *pri_bmp)
{
    uint32 idx = 0;
    uint32 offset = 0;
    uint32 lookup_base = 0;

    if ((intf_id < 0) || (intf_id >= _TH_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if (tc > (_TH_NUM_TC - 1)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_rx_port_offset_get(unit, intf_id,
                                           gport, &offset));

    /* Get lookup base */
    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_rx_intf_get_lookup_base(unit, intf_id,
                                                &lookup_base));

    idx = offset + lookup_base;

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_rx_tc_mapping_get(unit, intf_id, tc,
                                          pri_bmp, idx));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_fc_rx_port_offset_get
 * Purpose:
 *      Get the Channel offset value for a given port
 *      on given OOB FC Rx Interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      intf_id - (IN) OOB FC Rx Interface Number
 *      gport - (IN) Modport type gport
 *      offset - (OUT) Channel offset value in HCFC message
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_oob_fc_rx_port_offset_get(int unit,
                             bcm_oob_fc_rx_intf_id_t intf_id,
                             bcm_gport_t gport,
                             uint32 *offset)
{
    if ((intf_id < 0) || (intf_id >= _TH_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_fc_rx_port_offset_get(unit, intf_id,
                                           gport, offset));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_oob_stats_config_flags_set
 * Purpose:
 *      Set the enable control for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      flags - (IN) OOB Stats Enable controls
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_config_flags_set(int unit, uint32 flags)
{
    uint32 rval = 0;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, STATS_INTF_REPORT_ENr, 0, 0, &rval));

    soc_reg_field_set(unit, STATS_INTF_REPORT_ENr, &rval, TIMESTAMP_ENf,
                      ((flags & BCM_OOB_STATS_TIMESTAMP_EN) ? 0x1 : 0x0));

    soc_reg_field_set(unit, STATS_INTF_REPORT_ENr, &rval, INST_SERV_POOL_SIZE_ENf,
                      ((flags & BCM_OOB_STATS_POOL_EN) ? 0x1 : 0x0));

    soc_reg_field_set(unit, STATS_INTF_REPORT_ENr, &rval, INST_UC_QSIZE_ENf,
                      ((flags & BCM_OOB_STATS_QUEUE_EN) ? 0x1 : 0x0));

    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, STATS_INTF_REPORT_ENr, 0, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_stats_config_ipg_set
 * Purpose:
 *      Set the inter packet gap for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      ipg - (IN) OOB Stats inter packet gap(clock cycles)
 *                 can range from 0-255
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_config_ipg_set(int unit, uint8 ipg)
{
    uint32 rval = 0;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, OOB_STATS_TX_IDLEr, 0, 0, &rval));
    soc_reg_field_set(unit, OOB_STATS_TX_IDLEr, &rval,
                      IDLE_GAPf, ipg);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, OOB_STATS_TX_IDLEr, 0, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_stats_config_cfg_id_set
 * Purpose:
 *      Set the inter packet gap for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      cfg_id - (IN) OOB Stats config ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_config_cfg_id_set(int unit, uint8 cfg_id)
{
    uint32 rval = 0;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, STATS_INTF_CONFIG_IDr, 0, 0, &rval));
    soc_reg_field_set(unit, STATS_INTF_CONFIG_IDr, &rval,
                      CONFIG_IDf, cfg_id);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, STATS_INTF_CONFIG_IDr, 0, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_stats_config_flags_get
 * Purpose:
 *      Get the enable control for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      config - (OUT) OOB Stats Configuration structure
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_config_flags_get(int unit,
                        bcm_oob_stats_config_t *config)
{
    uint32 rval = 0;

    config->flags = 0x0;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, STATS_INTF_REPORT_ENr, 0, 0, &rval));

    if (soc_reg_field_get(unit, STATS_INTF_REPORT_ENr,
                          rval, TIMESTAMP_ENf)) {
        config->flags |= BCM_OOB_STATS_TIMESTAMP_EN;
    }

    if (soc_reg_field_get(unit, STATS_INTF_REPORT_ENr,
                          rval, INST_SERV_POOL_SIZE_ENf)) {
        config->flags |= BCM_OOB_STATS_POOL_EN;
    }

    if (soc_reg_field_get(unit, STATS_INTF_REPORT_ENr,
                          rval, INST_UC_QSIZE_ENf)) {
        config->flags |= BCM_OOB_STATS_QUEUE_EN;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_stats_config_ipg_get
 * Purpose:
 *      Get the inter packet gap for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      config - (OUT) OOB Stats Configuration structure
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_config_ipg_get(int unit,
                        bcm_oob_stats_config_t *config)
{
    uint32 rval = 0;

    config->inter_pkt_gap = 0x0;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, OOB_STATS_TX_IDLEr, 0, 0, &rval));

    config->inter_pkt_gap = soc_reg_field_get(unit, OOB_STATS_TX_IDLEr,
                                              rval, IDLE_GAPf);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_stats_config_cfg_id_get
 * Purpose:
 *      Get the Config ID for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      config - (OUT) OOB Stats Configuration structure
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_config_cfg_id_get(int unit,
                        bcm_oob_stats_config_t *config)
{
    uint32 rval = 0;

    config->config_id = 0x0;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, STATS_INTF_CONFIG_IDr, 0, 0, &rval));

    config->config_id = soc_reg_field_get(unit, STATS_INTF_CONFIG_IDr,
                                          rval, CONFIG_IDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_stats_pool_mapping_set
 * Purpose:
 *      Set the service pool id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which service pool
 *               in the service pool list needs to be added
 *      spid - (IN) Specifies the service pool for which the corresponding pool
 *             size should be reported. Mapping as follows:
 *                 0-3: Report stats for ingress service pool ID 0-3
 *                 4-7: Report stats for egress service pool ID 0-3
 *                 15: End reporting round
 *             Reset Value: 0xF
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_pool_mapping_set(int unit,
                        int offset, uint32 spid)
{
    uint32 entry[SOC_MAX_MEM_WORDS];

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);

    /* Read the STATS_INTF_SERVPOOL_LIST memory into entry buffer */
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, STATS_INTF_SERVPOOL_LISTm, MEM_BLOCK_ALL,
                      offset, entry));

    soc_mem_field32_set(unit, STATS_INTF_SERVPOOL_LISTm, entry,
                        SPIDf, spid);

    /* Write the entry buffer into STATS_INTF_SERVPOOL_LIST memory */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, STATS_INTF_SERVPOOL_LISTm, MEM_BLOCK_ALL,
                      offset, entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_stats_pool_mapping_get
 * Purpose:
 *      Get the service pool id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which service pool
 *               in the service pool list needs to be added
 *      spid - (OUT) Specifies the service pool for which the corresponding pool
 *             size should be reported. Mapping as follows:
 *                 0-3: Report stats for ingress service pool ID 0-3
 *                 4-7: Report stats for egress service pool ID 0-3
 *                 15: End reporting round
 *             Reset Value: 0xF
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_pool_mapping_get(int unit,
                        int offset, uint32 *spid)
{
    uint32 entry[SOC_MAX_MEM_WORDS];

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);

    /* Read the STATS_INTF_SERVPOOL_LIST memory entry buffer */
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, STATS_INTF_SERVPOOL_LISTm, MEM_BLOCK_ALL,
                      offset, entry));

    *spid = soc_mem_field32_get(unit, STATS_INTF_SERVPOOL_LISTm, entry,
                                SPIDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_stats_queue_mapping_set
 * Purpose:
 *      Set the queue id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the queue ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which Queue Id
 *               in the queue list needs to be added
 *      slice_id - (IN) 0: Specifies logical queue ID is for Slice R
 *                      1: Specifies logical queue ID is for Slice S
 *      queue_id - (IN) logical queue ID
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_queue_mapping_set(int unit,
                        int offset, int slice_id, int queue_id)
{
    uint32 entry[SOC_MAX_MEM_WORDS];

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);

    /* Read the STATS_INTF_QUEUE_LIST memory entry buffer */
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, STATS_INTF_QUEUE_LISTm, MEM_BLOCK_ALL,
                      offset, entry));

    soc_mem_field32_set(unit, STATS_INTF_QUEUE_LISTm, entry,
                        SLICE_IDf, slice_id);
    soc_mem_field32_set(unit, STATS_INTF_QUEUE_LISTm, entry,
                        LOGICAL_QUEUE_IDf, queue_id);

    /* Write the entry buffer into STATS_INTF_QUEUE_LIST memory */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, STATS_INTF_QUEUE_LISTm, MEM_BLOCK_ALL,
                      offset, entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_stats_queue_mapping_get
 * Purpose:
 *      Get the UC queue id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the UC queue ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which Queue Id
 *               in the queue list needs to be added
 *      gport - (OUT) UC Queue gport
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_oob_stats_queue_mapping_get(int unit, int offset,
                         bcm_gport_t *gport)
{
    int logical_queue_id = -1;
    int slice_id = -1;
    bcm_cos_t cosq = -1;
    soc_info_t *si;
    int mmu_port = 1, phy_port = -1, logical_port = -1;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int q_offset = 0;
    bcm_port_t port_offset = -1;

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);

    /* Read the STATS_INTF_QUEUE_LIST memory entry buffer */
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, STATS_INTF_QUEUE_LISTm, MEM_BLOCK_ALL,
                      offset, entry));

    logical_queue_id = soc_mem_field32_get(unit, STATS_INTF_QUEUE_LISTm, entry,
                                           LOGICAL_QUEUE_IDf);

    slice_id = soc_mem_field32_get(unit, STATS_INTF_QUEUE_LISTm, entry,
                                  SLICE_IDf);

    if (logical_queue_id == _TH_OOB_STATS_END_QUEUE_REPORT) {
        *gport = _TH_OOB_STATS_END_QUEUE_REPORT;
    } else {
        si = &SOC_INFO(unit);

        if (si == NULL) {
            return BCM_E_INTERNAL;
        }

        /*
         * Each Pipe has 33 Ports and each port has 10 Unicast
         * Queues. So total of 330 Logical Queues per Pipe.
         * Within one slice X and Y Pipes has Logical Queue IDs
         * as below:
         * 0-329: Logical UC Queue on X Pipe of Slice
         * 1024-1353: Logical UC Queue on Y Pipe of Slice
         */
        if (logical_queue_id >= 1024) {
            q_offset = 1024;
            port_offset = SOC_TH_MMU_PORT_STRIDE;
        } else {
            q_offset = 0;
            port_offset = 0;
        }
        cosq = (logical_queue_id - q_offset) % _TH_NUM_UCAST_QUEUE_PER_PORT;
        mmu_port = (slice_id * SOC_TH_MMU_PORT_STRIDE * _TH_NUM_PIPE_PER_SLICE)
                    + ((logical_queue_id - q_offset) / _TH_NUM_UCAST_QUEUE_PER_PORT) +
                    port_offset;

        phy_port = si->port_m2p_mapping[mmu_port];
        logical_port = si->port_p2l_mapping[phy_port];
        BCM_IF_ERROR_RETURN
            (_bcm_th_cosq_port_cos_resolve(unit, logical_port, cosq,
                                           _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                           gport));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_stats_config_set
 * Purpose:
 *      Set the global configuration setting
 *      for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      config - (IN) OOB Stats Configuration structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_config_set(int unit,
                bcm_oob_stats_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if (config->config_id > _TH_MAX_STATS_CONFIG_ID) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_config_flags_set(unit, config->flags));

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_config_ipg_set(unit,
                                          config->inter_pkt_gap));

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_config_cfg_id_set(unit,
                                             config->config_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_stats_config_get
 * Purpose:
 *      Get the global configuration setting
 *      for OOB STATS.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      config - (OUT) OOB Stats Configuration structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_config_get(int unit,
                bcm_oob_stats_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_config_flags_get(unit, config));

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_config_ipg_get(unit, config));

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_config_cfg_id_get(unit, config));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th_oob_stats_pool_mapping_multi_set
 * Purpose:
 *      Set the service pool id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      array_count - (IN) Number of elements in array
 *      offset_array - (IN) Indexes at which service pool
 *                      in the service pool list needs to be added
 *      dir_array - (IN) Direction array
 *      pool_array - (IN) Service Pool ID array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_pool_mapping_multi_set(int unit,
                        int array_count, int *offset_array,
                        uint8 *dir_array, bcm_service_pool_id_t *pool_array)
{
    int idx = 0;

    if ((array_count < 0) || (array_count > _TH_MAX_IDX_STATS_POOL_LIST)) {
        return BCM_E_PARAM;
    }

    if ((offset_array == NULL) ||
        (dir_array == NULL) ||
        (pool_array == NULL)) {
        return BCM_E_PARAM;
    }

    for (idx = 0; idx < array_count; idx++) {
        if ((offset_array[idx] < 0) &&
            (offset_array[idx] >= _TH_MAX_IDX_STATS_POOL_LIST)) {
            return BCM_E_PARAM;
        }
    }

    for (idx = 0; idx < array_count; idx++) {
        BCM_IF_ERROR_RETURN
            (bcm_th_oob_stats_pool_mapping_set(unit, offset_array[idx],
                                               dir_array[idx],
                                               pool_array[idx]));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_stats_pool_mapping_multi_get
 * Purpose:
 *      Get the service pool id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      array_max - (IN) Number of elements in array
 *      offset_array - (IN) Indexes at which service pool
 *                      in the service pool list needs to be added
 *      dir_array - (OUT) Direction array
 *      pool_array - (OUT) Service Pool ID array
 *      array_count - (OUT) Number of elements returned in the array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_pool_mapping_multi_get(int unit,
                        int array_max, int *offset_array, uint8 *dir_array,
                        bcm_service_pool_id_t *pool_array, int *array_count)
{
    int idx = 0;

    if ((offset_array == NULL) ||
        (dir_array == NULL) ||
        (pool_array == NULL) ||
        (array_count == NULL)) {
            return BCM_E_PARAM;
    }

    if ((array_max >= 0) && (array_max <= _TH_MAX_IDX_STATS_POOL_LIST)) {
        *array_count = array_max;
    } else if (array_max > _TH_MAX_IDX_STATS_POOL_LIST) {
        *array_count = _TH_MAX_IDX_STATS_POOL_LIST;
    } else {
        return BCM_E_PARAM;
    }

    for (idx = 0; idx < *array_count; idx++) {
        if ((offset_array[idx] < 0) &&
            (offset_array[idx] >= _TH_MAX_IDX_STATS_POOL_LIST)) {
            return BCM_E_PARAM;
        }
    }

    for (idx = 0; idx < *array_count; idx++) {
        BCM_IF_ERROR_RETURN
            (bcm_th_oob_stats_pool_mapping_get(unit, offset_array[idx],
                                               &dir_array[idx],
                                               &pool_array[idx]));

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_stats_pool_mapping_set
 * Purpose:
 *      Set the service pool id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which service pool
 *               in the service pool list needs to be added
 *      dir - (IN) Direction
 *      pool - (IN) Service Pool ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_pool_mapping_set(int unit,
                        int offset, uint8 dir,
                        bcm_service_pool_id_t pool)
{
    uint32 spid = 0x0;

    if (((pool < 0) || (pool >= _TH_MMU_NUM_POOL)) &&
        (pool != _TH_OOB_STATS_END_POOL_REPORT)) {
        return BCM_E_PARAM;
    }

    if ((offset < 0) || (offset >= _TH_MAX_IDX_STATS_POOL_LIST)) {
        return BCM_E_PARAM;
    }

    if ((dir != BCM_OOB_STATS_INGRESS) &&
        (dir != BCM_OOB_STATS_EGRESS)) {
        if (pool != _TH_OOB_STATS_END_POOL_REPORT) {
            return BCM_E_PARAM;
        }
    }

    if ((dir == BCM_OOB_STATS_EGRESS) &&
        (pool != _TH_OOB_STATS_END_POOL_REPORT)) {
        /* Egress Service Pool spid value [4, ..., 7]*/
        spid = pool | 0x4;
    } else {
        /* Ingress Service Pool spid value [0, ..., 3]*/
        spid = pool;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_pool_mapping_set(unit, offset, spid));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_stats_pool_mapping_get
 * Purpose:
 *      Get the service pool id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which service pool
 *               in the service pool list needs to be added
 *      dir - (OUT) Direction
 *      pool - (OUT) Service Pool ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_pool_mapping_get(int unit,
                        int offset, uint8 *dir,
                        bcm_service_pool_id_t *pool)
{
    uint32 spid = 0x0;

    if ((offset < 0) || (offset >= _TH_MAX_IDX_STATS_POOL_LIST)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_pool_mapping_get(unit, offset, &spid));

    if (spid == _TH_OOB_STATS_END_POOL_REPORT) {
        *pool = _TH_OOB_STATS_END_POOL_REPORT;
    } else {
        /* Ingress Service Pool spid value [0, ..., 3]*/
        /* Egress Service Pool spid value [4, ..., 7]*/
        *pool = spid & 0x3;
        if (spid & 0x4) {
            *dir = BCM_OOB_STATS_EGRESS;
        } else {
            *dir = BCM_OOB_STATS_INGRESS;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_stats_queue_mapping_multi_set
 * Purpose:
 *      Set the queue id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      array_count - (IN) Number of elements in array
 *      offset_array - (IN) Indexes at which Queue Id
 *                      in the queue list needs to be added
 *      gport_array - (IN) Queue gport array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_queue_mapping_multi_set(int unit,
                        int array_count, int *offset_array,
                        bcm_gport_t *gport_array)
{
    int idx = 0;

    if ((array_count < 0) || (array_count > _TH_MAX_IDX_STATS_QUEUE_LIST)) {
        return BCM_E_PARAM;
    }

    if ((offset_array == NULL) ||
        (gport_array == NULL)) {
        return BCM_E_PARAM;
    }

    for (idx = 0; idx < array_count; idx++) {
        if ((offset_array[idx] < 0) &&
            (offset_array[idx] >= _TH_MAX_IDX_STATS_QUEUE_LIST)) {
            return BCM_E_PARAM;
        }
    }

    for (idx = 0; idx < array_count; idx++) {
        BCM_IF_ERROR_RETURN
            (bcm_th_oob_stats_queue_mapping_set(unit, offset_array[idx],
                                                gport_array[idx]));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_stats_queue_mapping_multi_get
 * Purpose:
 *      Get the queue id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the queue ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      array_max - (IN) Maximum number of elements in array
 *      offset_array - (IN) Indexes at which Queue Id
 *                      in the queue list needs to be added
 *      gport_array - (OUT) Queue gport array
 *      array_count - (OUT) Number of elements in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_queue_mapping_multi_get(int unit,
                        int array_max, int *offset_array,
                        bcm_gport_t *gport_array, int *array_count)
{
    int idx = 0;

    if ((offset_array == NULL) ||
        (gport_array == NULL) ||
        (array_count == NULL)) {
        return BCM_E_PARAM;
    }

    if ((array_max >= 0) && (array_max <= _TH_MAX_IDX_STATS_QUEUE_LIST)) {
        *array_count = array_max;
    } else if (array_max > _TH_MAX_IDX_STATS_QUEUE_LIST) {
        *array_count = _TH_MAX_IDX_STATS_QUEUE_LIST;
    } else {
        return BCM_E_PARAM;
    }

    for (idx = 0; idx < *array_count; idx++) {
        BCM_IF_ERROR_RETURN
            (bcm_th_oob_stats_queue_mapping_get(unit, offset_array[idx],
                                                &gport_array[idx]));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th_oob_stats_queue_mapping_set
 * Purpose:
 *      Set the queue id at a given index in the memory
 *      so that OOB Stats will be reported in the same order
 *      as the service pool ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which Queue Id
 *               in the queue list needs to be added
 *      gport - (IN) Queue gport
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_queue_mapping_set(int unit,
                        int offset, bcm_gport_t gport)
{
    bcm_port_t local_port = -1;
    int logical_queue_id = -1;
    int hw_index = -1;
    int mmu_port, phy_port;
    soc_info_t *si;
    int slice_id = 1; /* Default value is 1 */
    int pipe = -1;
    int q_offset = 0;

    if ((offset < 0) || (offset >= _TH_MAX_IDX_STATS_QUEUE_LIST)) {
        return BCM_E_PARAM;
    }

    if (gport == _TH_OOB_STATS_END_QUEUE_REPORT) {
        logical_queue_id = _TH_OOB_STATS_END_QUEUE_REPORT;
    } else {
        si = &SOC_INFO(unit);

        if (si == NULL) {
            return BCM_E_INTERNAL;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_th_cosq_index_resolve(unit, gport, -1,
                                        _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                        &local_port, &hw_index, NULL));

        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /*
          Calculating Logical_Queue_Id per Slice.
          Each Slice Control services a pair of Pipes.
          Slice R = Pipe 0,1
          Slice S = Pipe 2,3
          0-329: Logical UC Queue on X Pipe of Slice
          1024-1353: Logical UC Queue on Y Pipe of Slice
        */
        pipe = si->port_pipe[local_port];
        if (pipe & 0x1) {
            q_offset = 1024;
        } else {
            q_offset = 0;
        }
        logical_queue_id = q_offset + ((mmu_port %
                           SOC_TH_MMU_PORT_STRIDE) *
                           _TH_NUM_UCAST_QUEUE_PER_PORT) +
                           (hw_index % _TH_NUM_UCAST_QUEUE_PER_PORT);

        slice_id = pipe / _TH_NUM_PIPE_PER_SLICE;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_queue_mapping_set(unit, offset, slice_id,
                                             logical_queue_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_oob_stats_queue_mapping_get
 * Purpose:
 *      Get the queue id at a given index in the memory.
 *      OOB Stats will be reported in the same order
 *      as the queue ids are programmed in the memory.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      offset - (IN) Index at which Queue Id
 *               in the queue list needs to be added
 *      gport - (OUT) Queue gport
 * Returns:
 *      BCM_E_XXX
 */
int bcm_th_oob_stats_queue_mapping_get(int unit,
                        int offset, bcm_gport_t *gport)
{
    if ((offset < 0) || (offset >= _TH_MAX_IDX_STATS_QUEUE_LIST)) {
        return BCM_E_PARAM;
    }

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th_oob_stats_queue_mapping_get(unit, offset, gport));

    return BCM_E_NONE;
}

#endif /* BCM_TOMAHAWK_SUPPORT */
