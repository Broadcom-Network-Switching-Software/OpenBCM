/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * OOB Flow Control
 * Purpose: API to set different OOB Flow Control and OOB Stats registers.
 */
#include <sal/core/libc.h>

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>

#if defined(BCM_KATANA2_SUPPORT)
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/katana2.h>

#include <bcm/error.h>
#include <bcm/oob.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/katana2.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stat.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#endif

#define BCM_KT2_MMU_NUM_POOL  1
/* Number of Traffic Class */
#define BCM_KT2_NUM_TC  8
/* Number of OOB FC Rx Interface */
#define BCM_KT2_NUM_OOB_FC_RX_INTF   1 
/* Maximum Value of Channel Base Value */
#define BCM_KT2_MAX_CHANNEL_BASE    63

/* Total number of Indexes in OOBFC RX memory */
#define BCM_KT2_MAX_RX_MEM_ENTRY   41 
#define BCM_SB2_MAX_RX_MEM_ENTRY   29 
#define BCM_ML_MAX_RX_MEM_ENTRY    13

typedef struct _bcm_kt2_oob_fc_rx_intf_info_s {
    uint32 channel_base; /* HCFC channel base */
    int enable;          /* Interface enable */
    uint8 num_ports;     /* Number of ports monitored */
    uint8 lookup_base;   /* Look up base in the OOB FC Rx Memory */
} _bcm_kt2_oob_fc_rx_intf_info_t;
    
/*
 * Function:
 *      _bcm_kt2_oob_fc_tx_config_flags_set
 * Purpose:
 *      Set OOB FC Tx enable flags
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      flags - (IN) OOB FC Tx flags
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_oob_fc_tx_config_flags_set(int unit, uint32 flags)
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

    /* pool enable not present */
    if (flags & BCM_OOB_FC_TX_POOL_EN) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (soc_reg64_set(unit, OOBFC_CHANNEL_BASE_64r, 0, 0, rval64));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_oob_fc_tx_config_ipg_set
 * Purpose:
 *      Set OOB FC Tx inter packet gap
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      ipg - (IN) OOB FC Tx inter packet gap
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_oob_fc_tx_config_ipg_set(int unit, uint8 ipg)
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
 *      _bcm_kt2_oob_fc_tx_config_gcs_id_set
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
_bcm_kt2_oob_fc_tx_config_gcs_id_set(int unit,
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
 *      _bcm_kt2_oob_fc_tx_config_egr_mode_set
 * Purpose:
 *      Set OOB FC Tx egress congestion reporting mode
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      mode - (IN) OOB FC Tx egress congestion reporting mode
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_oob_fc_tx_config_egr_mode_set(int unit,
                             bcm_oob_egress_mode_t mode)
{
    uint64 rval64;
    uint64 field64;

    COMPILER_64_ZERO(field64);
    COMPILER_64_ZERO(rval64);
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
 *      _bcm_kt2_oob_fc_tx_config_flags_get
 * Purpose:
 *      Get OOB FC Tx enable status
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_oob_fc_tx_config_flags_get(int unit,
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
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_oob_fc_tx_config_ipg_get
 * Purpose:
 *      Get OOB FC Tx inter packet gap
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_oob_fc_tx_config_ipg_get(int unit,
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
 *      _bcm_kt2_oob_fc_tx_config_gcs_id_get
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
_bcm_kt2_oob_fc_tx_config_gcs_id_get(int unit,
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
 *      _bcm_kt2_oob_fc_tx_config_egr_mode_get
 * Purpose:
 *      Get OOB FC Tx egress congestion reporting mode
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_oob_fc_tx_config_egr_mode_get(int unit,
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
 *      _bcm_kt2_oob_localport_resolve
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
_bcm_kt2_oob_localport_resolve(int unit, bcm_gport_t gport,
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
 *      _bcm_kt2_oob_fc_rx_get_intf_info
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
_bcm_kt2_oob_fc_rx_get_intf_info(int unit, _bcm_kt2_oob_fc_rx_intf_info_t *info,
                            bcm_oob_fc_rx_intf_id_t intf_id)
{
    uint32 rval = 0x0;
    soc_reg_t cfg_reg[BCM_KT2_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFGr};
   
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
 *      _bcm_kt2_oob_fc_rx_disable_intf
 * Purpose:
 *      Disable OOBFC Rx interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      intf_id - (IN) OOBFC Rx Interface ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_oob_fc_rx_disable_intf(int unit,
                           bcm_oob_fc_rx_intf_id_t intf_id)
{
    uint32 rval = 0x0;
    soc_reg_t cfg_reg[BCM_KT2_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFGr};

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
 *      _bcm_kt2_oob_fc_rx_enable_intf
 * Purpose:
 *      Enable OOBFC Rx interface.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      intf_id - (IN) OOBFC Rx Interface ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt2_oob_fc_rx_enable_intf(int unit,
                          bcm_oob_fc_rx_intf_id_t intf_id)
{
    uint32 rval = 0x0;
    soc_reg_t cfg_reg[BCM_KT2_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFGr};

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
 *      _bcm_kt2_oob_fc_rx_intf_get_lookup_base
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
_bcm_kt2_oob_fc_rx_intf_get_lookup_base(int unit,
                                bcm_oob_fc_rx_intf_id_t intf_id,
                                uint32 *lookup_base)
{
    uint32 rval = 0x0;
    soc_reg_t cfg_reg[BCM_KT2_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFGr};

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, cfg_reg[intf_id], 0, 0, &rval));

    *lookup_base = soc_reg_field_get(unit, cfg_reg[intf_id], rval,
                                     LOOKUP_BASEf);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_kt2_oob_fc_tx_config_set
 * Purpose:
 *      Set OOB FC Tx global configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (IN) OOB FC Tx global configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_oob_fc_tx_config_set(int unit,
                            bcm_oob_fc_tx_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if ((config->gcs_id < 0) || (config->gcs_id >= BCM_KT2_MMU_NUM_POOL)) {
        return BCM_E_PARAM;
    }

    if ((config->egr_mode < 0) || (config->egr_mode >= bcmOOBFCTxReportCount)) {
        return BCM_E_PARAM;
    }

    /* Enabling/Disabling FCN, OOBFC Tx Ingress, Egress and Service Pool */ 
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_tx_config_flags_set(unit, config->flags));

    /* Setting Inter packet Gap between two HCFC message */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_tx_config_ipg_set(unit, config->inter_pkt_gap));

    /* Setting service pool id whose congestion state will be
       reported in every HCFC message */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_tx_config_gcs_id_set(unit, config->gcs_id));

    /* Setting Egress mode for reporting OOBFC message. */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_tx_config_egr_mode_set(unit, config->egr_mode));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_oob_fc_tx_config_get
 * Purpose:
 *      Get OOB FC Tx global configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx global configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_oob_fc_tx_config_get(int unit,
                            bcm_oob_fc_tx_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the congestion state reporting status of FCN,
       OOBFC Tx Ingress, Egress and Service Pool */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_tx_config_flags_get(unit, config));

    /* Get the configured Inter Packet Gap between OOBFC Tx message.*/
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_tx_config_ipg_get(unit, config));

    /* Setting service pool id whose congestion state will be
       reported in every HCFC message */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_tx_config_gcs_id_get(unit, config));

    /* Get the configured Egress mode of OOBFC Tx side */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_tx_config_egr_mode_get(unit, config));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_oob_fc_tx_port_control_set
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
_bcm_kt2_oob_fc_tx_port_control_set(int unit, bcm_port_t localport,
                              int status, int dir)
{
    uint64 rval64;
    static const soc_field_t field[2] = {ING_PORT_ENf, ENG_PORT_ENf};
    static const soc_reg_t reg_kt2[2][1] = {
        {OOBFC_ING_PORT_EN_41r},
        {OOBFC_ENG_PORT_EN_41r}};
    static const soc_reg_t reg_sb2[2][1] = {
        {OOBFC_ING_PORT_EN_29r},
        {OOBFC_ENG_PORT_EN_29r}}; 
    int  max_rx_mem_entry = BCM_KT2_MAX_RX_MEM_ENTRY;

    if ((status != 1) && (status != 0)) {
        return BCM_E_PARAM;
    }

    if (dir > 1) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(rval64);

    if (SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit))  {
            max_rx_mem_entry = BCM_ML_MAX_RX_MEM_ENTRY;  
        } else 
#endif
        {
            max_rx_mem_entry = BCM_SB2_MAX_RX_MEM_ENTRY;
        }  
        if ((0 <= localport) && (localport < max_rx_mem_entry)) {
            BCM_IF_ERROR_RETURN
                  (soc_reg64_get(unit, reg_sb2[dir][0], 0, 0, &rval64));
            if (((status == 1) && (!COMPILER_64_BITTEST(rval64, localport))) ||
               ((status == 0) && (COMPILER_64_BITTEST(rval64, localport)))) {
                 if (status == 1) {
                     COMPILER_64_BITSET(rval64, localport);
                 } else {
                     COMPILER_64_BITCLR(rval64, localport);
                 }
                 soc_reg64_field_set(unit, reg_sb2[dir][0], &rval64,
                                field[dir], rval64);
                 BCM_IF_ERROR_RETURN
                        (soc_reg64_set(unit, reg_sb2[dir][0], 0, 0, rval64));
            }
        } else {
            return BCM_E_PARAM;
        }
    } else if (SOC_IS_KATANA2(unit)) {
        if ((0 <= localport) && (localport < max_rx_mem_entry)) { 
            BCM_IF_ERROR_RETURN
                  (soc_reg64_get(unit, reg_kt2[dir][0], 0, 0, &rval64));
            if (((status == 1) && (!COMPILER_64_BITTEST(rval64, localport))) ||
               ((status == 0) && (COMPILER_64_BITTEST(rval64, localport)))) {
                 if (status == 1) {
                      COMPILER_64_BITSET(rval64, localport);
                 } else {
                     COMPILER_64_BITCLR(rval64, localport);
                 }
                 soc_reg64_field_set(unit, reg_kt2[dir][0], &rval64,
                        field[dir], rval64);
                 BCM_IF_ERROR_RETURN 
                       (soc_reg64_set(unit, reg_kt2[dir][0], 0, 0, rval64)); 
            }  
        } else {
            return BCM_E_PARAM;
        }
    } 
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_oob_fc_tx_port_control_get
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
_bcm_kt2_oob_fc_tx_port_control_get(int unit, bcm_port_t localport,
                                 int *status, int dir)
{
    uint64 rval64;
    static const soc_reg_t reg_kt2[2][1] = {
        {OOBFC_ING_PORT_EN_41r},
        {OOBFC_ENG_PORT_EN_41r}};
    static const soc_reg_t reg_sb2[2][1] = {
        {OOBFC_ING_PORT_EN_29r},
        {OOBFC_ENG_PORT_EN_29r}};
    int max_rx_mem_entry = BCM_KT2_MAX_RX_MEM_ENTRY; 
    if (dir > 1) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(rval64);

    if (SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit))  {
            max_rx_mem_entry = BCM_ML_MAX_RX_MEM_ENTRY;
        } else 
#endif
        {
            max_rx_mem_entry = BCM_SB2_MAX_RX_MEM_ENTRY;
        } 
        if ((0 <= localport) && (localport < max_rx_mem_entry)) {
            BCM_IF_ERROR_RETURN
               (soc_reg64_get(unit, reg_sb2[dir][0], 0, 0, &rval64));
        } else {
            return BCM_E_PARAM;
        }
    } else if (SOC_IS_KATANA2(unit)) {
        if ((0 <= localport) && (localport < max_rx_mem_entry)) {
            BCM_IF_ERROR_RETURN   
               (soc_reg64_get(unit, reg_kt2[dir][0], 0, 0, &rval64));
        } else {
            return BCM_E_PARAM;
        }
    }

    if (COMPILER_64_BITTEST(rval64, localport)) {
        *status = 0x1;
    } else {
        *status = 0x0;
    }
    return BCM_E_NONE;
}

/*
* Function:
*      bcm_kt2_oob_fc_tx_info_get
* Purpose:
*      Get OOB FC Tx global information.
* Parameters:
*      unit - (IN) StrataSwitch unit number.
*      info - (OUT) OOB FC Tx global information
* Returns:
*      BCM_E_XXX
*/
int
bcm_kt2_oob_fc_tx_info_get(int unit, bcm_oob_fc_tx_info_t *info)
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

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_oob_fc_rx_config_enable_set
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
_bcm_kt2_oob_fc_rx_config_enable_set(int unit,
                            bcm_oob_fc_rx_intf_id_t intf_id,
                            uint8 channel_base,
                            int array_count,
                            bcm_gport_t *gport_array)
{
    _bcm_kt2_oob_fc_rx_intf_info_t info[BCM_KT2_NUM_OOB_FC_RX_INTF];
    char *sysport_buf, *tc2pri_buf;
    uint32 sysport_list[BCM_KT2_MAX_RX_MEM_ENTRY];
    int rv = BCM_E_INTERNAL;
    void *pentry1, *pentry2;
    uint32 rval = 0x0;
    int mem_wsz = 0;
    int i = 0, total_num_ports = 0;
    int  max_rx_mem_entry = BCM_KT2_MAX_RX_MEM_ENTRY;
    bcm_port_t localport = 0;
    uint64 tc_to_pri_mapping;
    uint32 tc_to_pri_mapping_low = 0x0;
    uint32 tc_to_pri_mapping_high = 0x0;
    soc_reg_t cfg_reg[BCM_KT2_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFGr};
   
    if (SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit))  {
            max_rx_mem_entry = BCM_ML_MAX_RX_MEM_ENTRY;
        } else 
#endif
        {
            max_rx_mem_entry = BCM_SB2_MAX_RX_MEM_ENTRY;
        } 
    } 
    COMPILER_64_ZERO(tc_to_pri_mapping);
   
    sal_memset(info, 0, sizeof(_bcm_kt2_oob_fc_rx_intf_info_t) *
               BCM_KT2_NUM_OOB_FC_RX_INTF);
    sal_memset(sysport_list, 0, sizeof(uint32) *
               BCM_KT2_MAX_RX_MEM_ENTRY);

    /* Backup the per OOBFC Rx interface configuration */
    for (i = 0; i < BCM_KT2_NUM_OOB_FC_RX_INTF; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oob_fc_rx_get_intf_info(unit, &info[i], i));
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
    if ((total_num_ports + array_count) > max_rx_mem_entry) {
        return BCM_E_PARAM;
    }

    /* Read the MMU_CHFC_SYSPORT_MAPPING memory into sysport_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_CHFC_SYSPORT_MAPPINGm);
    sysport_buf = soc_cm_salloc(unit, max_rx_mem_entry * mem_wsz,
                                "oob sysport_buf");
    if (sysport_buf == NULL) {
        return BCM_E_MEMORY;
    }

    if (soc_mem_read_range(unit, MMU_CHFC_SYSPORT_MAPPINGm, MEM_BLOCK_ALL,
                           0, max_rx_mem_entry - 1, sysport_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    /* Read the MMU_INTFO_TC2PRI_MAPPING memory into tc2pri_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_INTFO_TC2PRI_MAPPINGm);
    tc2pri_buf = soc_cm_salloc(unit, max_rx_mem_entry * mem_wsz,
                               "oob tc2pri_buf");
    if (tc2pri_buf == NULL) {
        rv = BCM_E_MEMORY;
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    if (soc_mem_read_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                           0, max_rx_mem_entry - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    /* Creating a list of logical port number from a list of
       modport gport */
    for (i = 0; i < array_count; i++) {
        rv = _bcm_kt2_oob_localport_resolve(unit, gport_array[i],
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
                            0, max_rx_mem_entry - 1, sysport_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    if (soc_mem_write_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                            0, max_rx_mem_entry - 1, tc2pri_buf)) {
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
 *      _bcm_kt2_oob_fc_rx_port_offset_get
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
_bcm_kt2_oob_fc_rx_port_offset_get(int unit,
                         bcm_oob_fc_rx_intf_id_t intf_id,
                         bcm_gport_t gport,
                         uint32 *offset)
{
    bcm_oob_fc_rx_config_t config;
    bcm_gport_t gport_array[BCM_KT2_MAX_RX_MEM_ENTRY] = {0x0};
    int array_count = 0, i = 0;
    int max_rx_mem_entry = BCM_KT2_MAX_RX_MEM_ENTRY;

    if (SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit))  {
            max_rx_mem_entry = BCM_ML_MAX_RX_MEM_ENTRY;
        } else 
#endif
        {
            max_rx_mem_entry = BCM_SB2_MAX_RX_MEM_ENTRY;
        } 
    } 

    /* Get the configured list of gport array
       and array count*/
    BCM_IF_ERROR_RETURN
        (bcm_kt2_oob_fc_rx_config_get(unit, intf_id, &config,
                                      max_rx_mem_entry, gport_array,
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
 *      _bcm_kt2_oob_fc_rx_tc_mapping_get
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
_bcm_kt2_oob_fc_rx_tc_mapping_get(int unit,
                        bcm_oob_fc_rx_intf_id_t intf_id,
                        uint32 tc, uint32 *pri_bmp,
                        uint32 idx)
{
    char *tc2pri_buf;
    uint64 tc2pri;
    void *pentry;
    int mem_wsz = 0;
    int max_rx_mem_entry = BCM_KT2_MAX_RX_MEM_ENTRY;

    if (SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit))  {
            max_rx_mem_entry = BCM_ML_MAX_RX_MEM_ENTRY;
        } else 
#endif
        {
            max_rx_mem_entry = BCM_SB2_MAX_RX_MEM_ENTRY;
        } 
    }
    COMPILER_64_ZERO(tc2pri);

    /* Read the MMU_INTFO_TC2PRI_MAPPING memory into tc2pri_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_INTFO_TC2PRI_MAPPINGm);
    tc2pri_buf = soc_cm_salloc(unit, max_rx_mem_entry * mem_wsz,
                                         "oob tc2pri_buf");
    if (tc2pri_buf == NULL) {
        return BCM_E_MEMORY;
    }

    if (soc_mem_read_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                           0, max_rx_mem_entry - 1, tc2pri_buf)) {
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
 *      _bcm_kt2_oob_fc_rx_tc_mapping_set
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
_bcm_kt2_oob_fc_rx_tc_mapping_set(int unit,
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
    int max_rx_mem_entry = BCM_KT2_MAX_RX_MEM_ENTRY;

    if (SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit))  {
            max_rx_mem_entry = BCM_ML_MAX_RX_MEM_ENTRY;
        } else 
#endif
        {
            max_rx_mem_entry = BCM_SB2_MAX_RX_MEM_ENTRY;
        } 
    }
    COMPILER_64_ZERO(tc2pri);
    COMPILER_64_ZERO(mask64);
    COMPILER_64_ZERO(pri_bmp64);

    /* Read the MMU_INTFO_TC2PRI_MAPPING memory into tc2pri_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_INTFO_TC2PRI_MAPPINGm);
    tc2pri_buf = soc_cm_salloc(unit, max_rx_mem_entry * mem_wsz,
                                         "oob tc2pri_buf");
    if (tc2pri_buf == NULL) {
        return BCM_E_MEMORY;
    }

    if (soc_mem_read_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                           0, max_rx_mem_entry - 1, tc2pri_buf)) {
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
    rv = _bcm_kt2_oob_fc_rx_disable_intf(unit, intf_id);
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
                            0, max_rx_mem_entry - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, tc2pri_buf);
        return rv;
    }

    /* Enable the given OOBFC Rx interface back */
    rv = _bcm_kt2_oob_fc_rx_enable_intf(unit, intf_id);
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
 *      _bcm_kt2_oob_fc_rx_config_disable_set
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
_bcm_kt2_oob_fc_rx_config_disable_set(int unit,
                            bcm_oob_fc_rx_intf_id_t intf_id)
{
    _bcm_kt2_oob_fc_rx_intf_info_t old_info[BCM_KT2_NUM_OOB_FC_RX_INTF];
    _bcm_kt2_oob_fc_rx_intf_info_t new_info[BCM_KT2_NUM_OOB_FC_RX_INTF];
    char *sysport_buf, *tc2pri_buf;
    uint32 *old_sysport_list = NULL;
    uint64 *old_tc2pri_list = NULL;
    uint32 *new_sysport_list = NULL;
    uint64 *new_tc2pri_list = NULL;
    int rv = BCM_E_INTERNAL;
    int mem_wsz = 0;
    int i = 0, j = 0, k = 0;
    void *pentry;
    uint32 rval = 0x0;
    int max_rx_mem_entry = BCM_KT2_MAX_RX_MEM_ENTRY;
    soc_reg_t cfg_reg[BCM_KT2_NUM_OOB_FC_RX_INTF] = {OOBFC_CHIF_CFGr};


    old_sysport_list = soc_cm_salloc(unit, BCM_KT2_MAX_RX_MEM_ENTRY * sizeof(uint32),
                                         "oob old_sysport_list");
    if (old_sysport_list == NULL) {
        return BCM_E_MEMORY;
    }
    old_tc2pri_list = soc_cm_salloc(unit, BCM_KT2_MAX_RX_MEM_ENTRY * sizeof(uint64),
                                         "oob old_tc2pri_list");
    if (old_tc2pri_list == NULL) {
        soc_cm_sfree(unit, old_sysport_list);
        return BCM_E_MEMORY;
    }
    
    new_sysport_list = soc_cm_salloc(unit, BCM_KT2_MAX_RX_MEM_ENTRY * sizeof(uint32),
                                         "oob new_sysport_list");
    if (new_sysport_list == NULL) {
        soc_cm_sfree(unit, old_sysport_list);
        soc_cm_sfree(unit, old_tc2pri_list);
        return BCM_E_MEMORY;
    }

    new_tc2pri_list = soc_cm_salloc(unit, BCM_KT2_MAX_RX_MEM_ENTRY * sizeof(uint64),
                                         "oob new_tc2pri_list");
    if (new_tc2pri_list == NULL) {
        soc_cm_sfree(unit, old_sysport_list);
        soc_cm_sfree(unit, old_tc2pri_list);
        soc_cm_sfree(unit, new_sysport_list);
        return BCM_E_MEMORY;
    }
    if (SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit))  {
            max_rx_mem_entry = BCM_ML_MAX_RX_MEM_ENTRY;
        } else 
#endif
        {
            max_rx_mem_entry = BCM_SB2_MAX_RX_MEM_ENTRY;
        } 
    } 

    sal_memset(old_info, 0, sizeof(_bcm_kt2_oob_fc_rx_intf_info_t) *
               BCM_KT2_NUM_OOB_FC_RX_INTF);
    sal_memset(new_info, 0, sizeof(_bcm_kt2_oob_fc_rx_intf_info_t) *
               BCM_KT2_NUM_OOB_FC_RX_INTF);
    sal_memset(old_sysport_list, 0, sizeof(uint32) *
               BCM_KT2_MAX_RX_MEM_ENTRY);
    sal_memset(new_sysport_list, 0, sizeof(uint32) *
               BCM_KT2_MAX_RX_MEM_ENTRY);
    sal_memset(old_tc2pri_list, 0, sizeof(uint64) *
               BCM_KT2_MAX_RX_MEM_ENTRY);
    sal_memset(new_tc2pri_list, 0, sizeof(uint64) *
               BCM_KT2_MAX_RX_MEM_ENTRY);

    /* Store all the OOBFC Rx interface configuration */
    for (i = 0; i < BCM_KT2_NUM_OOB_FC_RX_INTF; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oob_fc_rx_get_intf_info(unit, &old_info[i], i));
    }

    /* Read the MMU_CHFC_SYSPORT_MAPPING memory into sysport_buf */
    mem_wsz = sizeof(uint32) *
                soc_mem_entry_words(unit, MMU_CHFC_SYSPORT_MAPPINGm);
    sysport_buf = soc_cm_salloc(unit, max_rx_mem_entry * mem_wsz,
                                         "oob sysport_buf");
    if (sysport_buf == NULL) {
        soc_cm_sfree(unit, old_sysport_list);
        soc_cm_sfree(unit, old_tc2pri_list);
        soc_cm_sfree(unit, new_sysport_list);
        soc_cm_sfree(unit, new_tc2pri_list);
        return BCM_E_MEMORY;
    }

    if (soc_mem_read_range(unit, MMU_CHFC_SYSPORT_MAPPINGm, MEM_BLOCK_ALL,
                           0, max_rx_mem_entry - 1, sysport_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, old_sysport_list);
        soc_cm_sfree(unit, old_tc2pri_list);
        soc_cm_sfree(unit, new_sysport_list);
        soc_cm_sfree(unit, new_tc2pri_list);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    /* Store the sysport_buf in old_sysport_list */
    for (i = 0; i < max_rx_mem_entry; i++) {
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
    tc2pri_buf = soc_cm_salloc(unit, max_rx_mem_entry * mem_wsz,
                                         "oob tc2pri_buf");
    if (tc2pri_buf == NULL) {
        rv = BCM_E_MEMORY;
        soc_cm_sfree(unit, old_sysport_list);
        soc_cm_sfree(unit, old_tc2pri_list);
        soc_cm_sfree(unit, new_sysport_list);
        soc_cm_sfree(unit, new_tc2pri_list);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    if (soc_mem_read_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                           0, max_rx_mem_entry - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, old_sysport_list);
        soc_cm_sfree(unit, old_tc2pri_list);
        soc_cm_sfree(unit, new_sysport_list);
        soc_cm_sfree(unit, new_tc2pri_list);
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    /* Store the tc2pri_buf in old_tc2pri_list */
    for (i = 0; i < max_rx_mem_entry; i++) {
        pentry = soc_mem_table_idx_to_pointer(unit,
                                              MMU_INTFO_TC2PRI_MAPPINGm,
                                              void*, tc2pri_buf, i);
        soc_mem_field64_get(unit, MMU_INTFO_TC2PRI_MAPPINGm,
                            pentry, PRI_BMPf, &old_tc2pri_list[i]);
    }

    /* Disable all the OOBFC Rx interface */
    for (i = 0; i < BCM_KT2_NUM_OOB_FC_RX_INTF; i++) {
        if (old_info[i].enable == 1) {
            rv = _bcm_kt2_oob_fc_rx_disable_intf(unit, i);
            if (rv != BCM_E_NONE) {
                soc_cm_sfree(unit, tc2pri_buf);
                soc_cm_sfree(unit, sysport_buf);
                soc_cm_sfree(unit, old_sysport_list);
                soc_cm_sfree(unit, old_tc2pri_list);
                soc_cm_sfree(unit, new_sysport_list);
                soc_cm_sfree(unit, new_tc2pri_list);
                return rv;
            }
        }
    }

    /* Generate new_info */
    /* Genrate new_sysport_list */
    /* Genrate new_tc2pri_list */
    j = 0;
    k = 0;
    for (i = 0; i < BCM_KT2_NUM_OOB_FC_RX_INTF; i++) {
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
    for (i = 0; i < max_rx_mem_entry; i++) {
        pentry = soc_mem_table_idx_to_pointer(unit,
                                              MMU_INTFO_TC2PRI_MAPPINGm,
                                              void*, tc2pri_buf, i);
        soc_mem_field64_set(unit, MMU_INTFO_TC2PRI_MAPPINGm,
                            pentry, PRI_BMPf, new_tc2pri_list[i]);
    }

    if (soc_mem_write_range(unit, MMU_INTFO_TC2PRI_MAPPINGm, MEM_BLOCK_ALL,
                            0, max_rx_mem_entry - 1, tc2pri_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        soc_cm_sfree(unit, old_sysport_list);
        soc_cm_sfree(unit, old_tc2pri_list);
        soc_cm_sfree(unit, new_sysport_list);
        soc_cm_sfree(unit, new_tc2pri_list);
        return rv;
    }

    /* Program new_sysport_list MMU_CHFC_SYSPORT_MAPPING */
    for (i = 0; i < max_rx_mem_entry; i++) {
        pentry = soc_mem_table_idx_to_pointer(unit,
                                              MMU_CHFC_SYSPORT_MAPPINGm,
                                              void*, sysport_buf, i);
        soc_mem_field32_set(unit, MMU_CHFC_SYSPORT_MAPPINGm, pentry,
                            SYS_PORTf, new_sysport_list[i]);
    }

    if (soc_mem_write_range(unit, MMU_CHFC_SYSPORT_MAPPINGm, MEM_BLOCK_ALL,
                            0, max_rx_mem_entry - 1, sysport_buf)) {
        rv = BCM_E_INTERNAL;
        soc_cm_sfree(unit, old_sysport_list);
        soc_cm_sfree(unit, old_tc2pri_list);
        soc_cm_sfree(unit, new_sysport_list);
        soc_cm_sfree(unit, new_tc2pri_list);
        soc_cm_sfree(unit, tc2pri_buf);
        soc_cm_sfree(unit, sysport_buf);
        return rv;
    }

    /* Program all the OOBFC Rx interface configuration register */
    for (i = 0; i < BCM_KT2_NUM_OOB_FC_RX_INTF; i++) {
        rv = soc_reg32_get(unit, cfg_reg[i],
                           0, 0, &rval);
        if (rv != BCM_E_NONE) {
            soc_cm_sfree(unit, old_sysport_list);
            soc_cm_sfree(unit, old_tc2pri_list);
            soc_cm_sfree(unit, new_sysport_list);
            soc_cm_sfree(unit, new_tc2pri_list);
            soc_cm_sfree(unit, tc2pri_buf);
            soc_cm_sfree(unit, sysport_buf);
            return rv;
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
            soc_cm_sfree(unit, old_sysport_list);
            soc_cm_sfree(unit, old_tc2pri_list);
            soc_cm_sfree(unit, new_sysport_list);
            soc_cm_sfree(unit, new_tc2pri_list);
            soc_cm_sfree(unit, tc2pri_buf);
            soc_cm_sfree(unit, sysport_buf);
            return rv;
        }
    }

    rv = BCM_E_NONE;
    soc_cm_sfree(unit, old_sysport_list);
    soc_cm_sfree(unit, old_tc2pri_list);
    soc_cm_sfree(unit, new_sysport_list);
    soc_cm_sfree(unit, new_tc2pri_list);
    soc_cm_sfree(unit, tc2pri_buf);
    soc_cm_sfree(unit, sysport_buf);
    return rv;
}

/*
 * Function:
 *      _bcm_kt2_oob_fc_rx_config_get
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
_bcm_kt2_oob_fc_rx_config_get(int unit,
                 bcm_oob_fc_rx_intf_id_t intf_id,
                 bcm_oob_fc_rx_config_t *config,
                 int array_max,
                 bcm_gport_t *gport_array,
                 int *array_count)
{
    _bcm_kt2_oob_fc_rx_intf_info_t info;
    int rv = BCM_E_INTERNAL;
    char *sysport_buf;
    void *pentry;
    int mem_wsz = 0;
    int i = 0 , j = 0;
    bcm_gport_t gport;
    uint32 new_sysport_list[BCM_KT2_MAX_RX_MEM_ENTRY];
    int max_rx_mem_entry = BCM_KT2_MAX_RX_MEM_ENTRY;

    if (SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit))  {
            max_rx_mem_entry = BCM_ML_MAX_RX_MEM_ENTRY;
        } else 
#endif
        {
            max_rx_mem_entry = BCM_SB2_MAX_RX_MEM_ENTRY;
        } 
    }

    sal_memset(&info, 0, sizeof(_bcm_kt2_oob_fc_rx_intf_info_t));
    sal_memset(new_sysport_list, 0, sizeof(uint32) *
               BCM_KT2_MAX_RX_MEM_ENTRY);
    
    /* Store all the OOBFC Rx interface configuration */
    BCM_IF_ERROR_RETURN
            (_bcm_kt2_oob_fc_rx_get_intf_info(unit, &info, intf_id));

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
        sysport_buf = soc_cm_salloc(unit, max_rx_mem_entry * mem_wsz,
                                             "oob sysport_buf");
        if (sysport_buf == NULL) {
            return BCM_E_MEMORY;
        }

        if (soc_mem_read_range(unit, MMU_CHFC_SYSPORT_MAPPINGm, MEM_BLOCK_ALL,
                               0, max_rx_mem_entry - 1, sysport_buf)) {
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
 *      bcm_kt2_oob_fc_rx_config_set
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
bcm_kt2_oob_fc_rx_config_set(int unit,
                     bcm_oob_fc_rx_intf_id_t intf_id,
                     bcm_oob_fc_rx_config_t *config,
                     int array_count,
                     bcm_gport_t *gport_array)
{
    if ((intf_id < 0) || (intf_id >= BCM_KT2_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if ((config->enable != 0) && (config->enable != 1)) {
        return BCM_E_PARAM;
    }

    if ((config->enable == 1) &&
        ((array_count <= 0) || (gport_array == NULL) ||
        (array_count > BCM_KT2_MAX_RX_MEM_ENTRY))) {
        return BCM_E_PARAM;
    }

    if ((config->enable == 1) &&
        ((config->channel_base >= BCM_KT2_MAX_CHANNEL_BASE))) {
        return BCM_E_PARAM;
    }

    if (config->enable == 1) {
        /* Add entries into MMU_CHFC_SYSPORT_MAPPING memory */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oob_fc_rx_config_enable_set(unit, intf_id,
                                                 config->channel_base,
                                                 array_count, gport_array));
    } else {
        /* Remove entries from MMU_CHFC_SYSPORT_MAPPING memory */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oob_fc_rx_config_disable_set(unit, intf_id));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_oob_fc_rx_config_get
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
bcm_kt2_oob_fc_rx_config_get(int unit,
                     bcm_oob_fc_rx_intf_id_t intf_id,
                     bcm_oob_fc_rx_config_t *config,
                     int array_max,
                     bcm_gport_t *gport_array,
                     int *array_count)
{
    if ((intf_id < 0) || (intf_id >= BCM_KT2_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if ((gport_array == NULL) ||
        (config == NULL) ||
        (array_count == NULL) ||
        (array_max <= 0)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_rx_config_get(unit, intf_id,
                                      config, array_max,
                                      gport_array,
                                      array_count));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_oob_fc_rx_port_tc_mapping_multi_set
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
bcm_kt2_oob_fc_rx_port_tc_mapping_multi_set(int unit,
                                 bcm_oob_fc_rx_intf_id_t intf_id,
                                 bcm_gport_t gport,
                                 int array_count,
                                 uint32 *tc,
                                 uint32 *pri_bmp)
{
    int i = 0;

    if ((intf_id < 0) || (intf_id >= BCM_KT2_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if ((array_count < 1) || (array_count > BCM_KT2_NUM_TC)) {
        return BCM_E_PARAM;
    }

    if ((tc == NULL) || (pri_bmp == NULL)) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < array_count; i++) {
        if ((tc[i] > (BCM_KT2_NUM_TC - 1)) || (pri_bmp[i] > 0xFF)) {
            return BCM_E_PARAM;
        }
    }

    for (i = 0; i < array_count; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_kt2_oob_fc_rx_port_tc_mapping_set(unit, intf_id,
                                                  gport, tc[i],
                                                  pri_bmp[i]));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_oob_fc_rx_port_tc_mapping_multi_get
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
bcm_kt2_oob_fc_rx_port_tc_mapping_multi_get(int unit,
                                     bcm_oob_fc_rx_intf_id_t intf_id,
                                     bcm_gport_t gport,
                                     int array_max,
                                     uint32 *tc,
                                     uint32 *pri_bmp,
                                     int *array_count)
{
    int i = 0;

    if ((intf_id < 0) || (intf_id >= BCM_KT2_NUM_OOB_FC_RX_INTF)) {
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

    if (array_max > BCM_KT2_NUM_TC) {
        *array_count = BCM_KT2_NUM_TC;
    } else {
        *array_count = array_max;
    }

    for (i = 0; i < *array_count; i++) {
        if (tc[i] > (BCM_KT2_NUM_TC - 1)) {
            return BCM_E_PARAM;
        }
    }

    for (i = 0; i < *array_count; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_kt2_oob_fc_rx_port_tc_mapping_get(unit, intf_id,
                                                  gport, tc[i],
                                                  &pri_bmp[i]));
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_kt2_oob_fc_rx_port_tc_mapping_set
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
bcm_kt2_oob_fc_rx_port_tc_mapping_set(int unit,
                                 bcm_oob_fc_rx_intf_id_t intf_id,
                                 bcm_gport_t gport,
                                 uint32 tc,
                                 uint32 pri_bmp)
{
    uint32 idx = 0;
    uint32 offset = 0;
    uint32 lookup_base = 0;

    if ((intf_id < 0) || (intf_id >= BCM_KT2_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if ((tc > (BCM_KT2_NUM_TC - 1)) || (pri_bmp > 0xFF)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_rx_port_offset_get(unit, intf_id,
                                           gport, &offset));

    /* Get lookup base */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_rx_intf_get_lookup_base(unit, intf_id,
                                                &lookup_base));

    idx = offset + lookup_base;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_rx_tc_mapping_set(unit, intf_id, tc,
                                          pri_bmp, idx));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_oob_fc_rx_port_tc_mapping_get
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
bcm_kt2_oob_fc_rx_port_tc_mapping_get(int unit,
                                 bcm_oob_fc_rx_intf_id_t intf_id,
                                 bcm_gport_t gport,
                                 uint32 tc,
                                 uint32 *pri_bmp)
{
    uint32 idx = 0;
    uint32 offset = 0;
    uint32 lookup_base = 0;

    if ((intf_id < 0) || (intf_id >= BCM_KT2_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    if (tc > (BCM_KT2_NUM_TC - 1)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_rx_port_offset_get(unit, intf_id,
                                           gport, &offset));

    /* Get lookup base */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_rx_intf_get_lookup_base(unit, intf_id,
                                                &lookup_base));

    idx = offset + lookup_base;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_rx_tc_mapping_get(unit, intf_id, tc,
                                          pri_bmp, idx));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_oob_fc_rx_port_offset_get
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
bcm_kt2_oob_fc_rx_port_offset_get(int unit,
                             bcm_oob_fc_rx_intf_id_t intf_id,
                             bcm_gport_t gport,
                             uint32 *offset)
{
    if ((intf_id < 0) || (intf_id >= BCM_KT2_NUM_OOB_FC_RX_INTF)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oob_fc_rx_port_offset_get(unit, intf_id,
                                           gport, offset));

    return BCM_E_NONE;
}

#endif /* BCM_KATANA2_SUPPORT */
