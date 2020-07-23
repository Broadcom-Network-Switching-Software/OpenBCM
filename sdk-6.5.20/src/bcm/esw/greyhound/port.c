/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_GREYHOUND_SUPPORT)
#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/greyhound.h>
#include <bcm_int/esw_dispatch.h>

/*
 * Function:
 *      bcm_gh_port_niv_type_set
 * Description:
 *      Set NIV port type.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      value - (IN) NIV port type 
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_gh_port_niv_type_set(int unit, bcm_port_t port, int value)
{
    bcm_module_t my_modid;
    int mod_port_index;
    uint64 val64;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                DISCARD_IF_VNTAG_NOT_PRESENTf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                DISCARD_IF_VNTAG_PRESENTf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                VNTAG_ACTIONS_IF_PRESENTf, VNTAG_NOOP));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                VNTAG_ACTIONS_IF_NOT_PRESENTf, VNTAG_NOOP));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                TX_DEST_PORT_ENABLEf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                NIV_VIF_LOOKUP_ENABLEf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                NIV_RPF_CHECK_ENABLEf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                NIV_UPLINK_PORTf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                VT_ENABLEf, 0));

    BCM_IF_ERROR_RETURN(bcm_esw_port_control_set(unit, port,
                bcmPortControlDoNotCheckVlan, 0));

    BCM_IF_ERROR_RETURN(READ_EGR_PORT_64r(unit, port, &val64));
    soc_reg64_field32_set(unit, EGR_PORT_64r, &val64, \
                VNTAG_ACTIONS_IF_PRESENTf, VNTAG_NOOP); 
    soc_reg64_field32_set(unit, EGR_PORT_64r, &val64, \
                NIV_PRUNE_ENABLEf, 0); 
    soc_reg64_field32_set(unit, EGR_PORT_64r, &val64, \
                NIV_UPLINK_PORTf, 0); 
    SOC_IF_ERROR_RETURN(WRITE_EGR_PORT_64r(unit, port, val64));


    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit,
                my_modid, port, &mod_port_index));

    switch (value) {
        case BCM_PORT_NIV_TYPE_NONE:
            break;
        case BCM_PORT_NIV_TYPE_SWITCH:
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                            _BCM_CPU_TABS_ETHER, VT_KEY_TYPE_USE_GLPf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, VT_ENABLEf, 1));
            break;

        case BCM_PORT_NIV_TYPE_UPLINK:
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, DISCARD_IF_VNTAG_NOT_PRESENTf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, NIV_VIF_LOOKUP_ENABLEf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, NIV_UPLINK_PORTf, 1));
            BCM_IF_ERROR_RETURN(bcm_esw_port_control_set(unit, port,
                        bcmPortControlDoNotCheckVlan, 1));

            BCM_IF_ERROR_RETURN(READ_EGR_PORT_64r(unit, port, &val64));
            soc_reg64_field32_set(unit, EGR_PORT_64r, &val64, \
                        NIV_UPLINK_PORTf, 1); 
            SOC_IF_ERROR_RETURN(WRITE_EGR_PORT_64r(unit, port, val64));

            break;

        case BCM_PORT_NIV_TYPE_DOWNLINK_ACCESS:
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, DISCARD_IF_VNTAG_PRESENTf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, VNTAG_ACTIONS_IF_NOT_PRESENTf,
                        VNTAG_ADD));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, TX_DEST_PORT_ENABLEf, 1));
            BCM_IF_ERROR_RETURN(bcm_esw_port_control_set(unit, port,
                        bcmPortControlDoNotCheckVlan, 1));

            BCM_IF_ERROR_RETURN(READ_EGR_PORT_64r(unit, port, &val64));
            soc_reg64_field32_set(unit, EGR_PORT_64r, &val64, \
                        VNTAG_ACTIONS_IF_PRESENTf, VNTAG_DELETE); 
            soc_reg64_field32_set(unit, EGR_PORT_64r, &val64, \
                        NIV_PRUNE_ENABLEf, 1); 
            SOC_IF_ERROR_RETURN(WRITE_EGR_PORT_64r(unit, port, val64));

            break;

        case BCM_PORT_NIV_TYPE_DOWNLINK_TRANSIT:
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, DISCARD_IF_VNTAG_NOT_PRESENTf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, TX_DEST_PORT_ENABLEf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, NIV_RPF_CHECK_ENABLEf, 1));
            BCM_IF_ERROR_RETURN(bcm_esw_port_control_set(unit, port,
                        bcmPortControlDoNotCheckVlan, 1));
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh_port_niv_type_get
 * Description:
 *      Get NIV port type.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      value - (OUT) NIV port type 
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_gh_port_niv_type_get(int unit, bcm_port_t port, int *value)
{
    int rv = BCM_E_NONE;
    int vt_enable, uplink, rpf_enable, prune_enable;
    uint64 val64;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit,
                port, VT_ENABLEf, &vt_enable));

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit,
                port, NIV_UPLINK_PORTf, &uplink));

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit,
                port, NIV_RPF_CHECK_ENABLEf, &rpf_enable));

	BCM_IF_ERROR_RETURN(READ_EGR_PORT_64r(unit, port, &val64));
    prune_enable = soc_reg64_field32_get(unit, EGR_PORT_64r, val64, 
                   NIV_PRUNE_ENABLEf);

    if (vt_enable) {
        *value = BCM_PORT_NIV_TYPE_SWITCH;
    } else if (uplink) {
        *value = BCM_PORT_NIV_TYPE_UPLINK;
    } else if (rpf_enable) {
        *value = BCM_PORT_NIV_TYPE_DOWNLINK_TRANSIT;
    } else if (prune_enable) {
        *value = BCM_PORT_NIV_TYPE_DOWNLINK_ACCESS;
    } else {
        *value = BCM_PORT_NIV_TYPE_NONE;
    }

    return rv;
}

/*
 * Function:
 *      bcm_gh_port_extender_type_set
 * Description:
 *      Set Port Extender port type.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      value - (IN) Port type 
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_gh_port_extender_type_set(int unit, bcm_port_t port, int value)
{
    switch (value) {
        case BCM_PORT_EXTENDER_TYPE_NONE:
            BCM_IF_ERROR_RETURN(bcm_gh_port_niv_type_set(unit, port,
                        BCM_PORT_NIV_TYPE_NONE));
            break;
        case BCM_PORT_EXTENDER_TYPE_SWITCH:
            BCM_IF_ERROR_RETURN(bcm_gh_port_niv_type_set(unit, port,
                        BCM_PORT_NIV_TYPE_SWITCH));
            break;
        case BCM_PORT_EXTENDER_TYPE_UPLINK:
            BCM_IF_ERROR_RETURN(bcm_gh_port_niv_type_set(unit, port,
                        BCM_PORT_NIV_TYPE_UPLINK));
            break;
        case BCM_PORT_EXTENDER_TYPE_DOWNLINK_ACCESS:
            BCM_IF_ERROR_RETURN(bcm_gh_port_niv_type_set(unit, port,
                        BCM_PORT_NIV_TYPE_DOWNLINK_ACCESS));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, VNTAG_ACTIONS_IF_NOT_PRESENTf,
                        2));
            break;
        case BCM_PORT_EXTENDER_TYPE_DOWNLINK_TRANSIT:
            BCM_IF_ERROR_RETURN(bcm_gh_port_niv_type_set(unit, port,
                        BCM_PORT_NIV_TYPE_DOWNLINK_TRANSIT));
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh_port_extender_type_get
 * Description:
 *      Get Port Extender port type.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      value - (IN) Port type 
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_gh_port_extender_type_get(int unit, bcm_port_t port, int *value)
{
    int type;

    BCM_IF_ERROR_RETURN(bcm_gh_port_niv_type_get(unit, port, &type));
    switch (type) {
        case BCM_PORT_NIV_TYPE_NONE:
            *value = BCM_PORT_EXTENDER_TYPE_NONE;
            break;
        case BCM_PORT_NIV_TYPE_SWITCH:
            *value = BCM_PORT_EXTENDER_TYPE_SWITCH;
            break;
        case BCM_PORT_NIV_TYPE_UPLINK:
            *value = BCM_PORT_EXTENDER_TYPE_UPLINK;
            break;
        case BCM_PORT_NIV_TYPE_DOWNLINK_ACCESS:
            *value = BCM_PORT_EXTENDER_TYPE_DOWNLINK_ACCESS;
            break;
        case BCM_PORT_NIV_TYPE_DOWNLINK_TRANSIT:
            *value = BCM_PORT_EXTENDER_TYPE_DOWNLINK_TRANSIT;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

#if defined(INCLUDE_L3) 
/*
 * Function:
 *      bcm_gh_port_etag_pcp_de_source_set
 * Description:
 *      Set source of ETAG's PCP and DE fields.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      value - (IN) PCP and DE source
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_gh_port_etag_pcp_de_source_set(int unit, bcm_port_t port, int value)
{
    int source;

    switch (value) {
        case BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG:
            source = 0;
            break;
        case BCM_EXTENDER_PCP_DE_SELECT_INNER_TAG:
            source = 1;
            break;
        case BCM_EXTENDER_PCP_DE_SELECT_DEFAULT:
            source = 2;
            break;
        case BCM_EXTENDER_PCP_DE_SELECT_PHB:
            source = 3;
            break;
        default:
            return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                ETAG_PCP_DE_SOURCEf, source));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh_port_etag_pcp_de_source_get
 * Description:
 *      Get source of ETAG's PCP and DE fields.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      value - (IN) PCP and DE source
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_gh_port_etag_pcp_de_source_get(int unit, bcm_port_t port, int *value)
{
    int source;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, port, ETAG_PCP_DE_SOURCEf,
                &source));

    switch (source) {
        case 0:
            *value = BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG;
            break;
        case 1:
            *value = BCM_EXTENDER_PCP_DE_SELECT_INNER_TAG;
            break;
        case 2:
            *value = BCM_EXTENDER_PCP_DE_SELECT_DEFAULT;
            break;
        case 3:
            *value = BCM_EXTENDER_PCP_DE_SELECT_PHB;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_gh_port_pmq_swrst_release
 * Description:
 *      To release reset state of QSGMII PCS cores in PM4x10Q.
 * Parameters:
 *      unit - (IN) Device number
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
_bcm_gh_port_pmq_swrst_release(int unit)
{
    uint32 rval;
    /*reset ILKN FIFO*/
    BCM_IF_ERROR_RETURN(READ_CHIP_SWRSTr(unit, REG_PORT_ANY, &rval));
    soc_reg_field_set(unit, CHIP_SWRSTr, &rval, FLUSHf, 0x1);
    soc_reg_field_set(unit, CHIP_SWRSTr, &rval, ILKN_BYPASS_RSTNf, 0);
    BCM_IF_ERROR_RETURN(WRITE_CHIP_SWRSTr(unit, REG_PORT_ANY, rval));

    
    BCM_IF_ERROR_RETURN(READ_CHIP_SWRSTr(unit, REG_PORT_ANY, &rval));
    soc_reg_field_set(unit, CHIP_SWRSTr, &rval, ILKN_BYPASS_RSTNf, 0xf);
    soc_reg_field_set(unit, CHIP_SWRSTr, &rval, FLUSHf, 0);
    soc_reg_field_set(unit, CHIP_SWRSTr, &rval, SOFT_RESET_QSGMII_PCSf, 0);
    soc_reg_field_set(unit, CHIP_SWRSTr, &rval, SOFT_RESET_GPORT1f, 0);
    soc_reg_field_set(unit, CHIP_SWRSTr, &rval, SOFT_RESET_GPORT0f, 0);
    BCM_IF_ERROR_RETURN(WRITE_CHIP_SWRSTr(unit, REG_PORT_ANY, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh_port_init
 * Description:
 *      For GH specific port init process.
 * Parameters:
 *      unit - (IN) Device number
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_gh_port_init(int unit)
{
    uint32 rval, pmq_disabled, qmode_en = 0;

    /* special init process for PMQ(PM4x10Q in QSGMII mode) 
     *  - to release PMQ's QSGMII reset state after QSGMII-PCS and UniMAC init.
     */
    BCM_IF_ERROR_RETURN(READ_PGW_CTRL_0r(unit, &rval));
    pmq_disabled = soc_reg_field_get(unit, 
            PGW_CTRL_0r, rval, SW_PM4X10Q_DISABLEf);
    /* ensure the pm4x10Q is enabled and QMODE enabled */
    if (!pmq_disabled) {
        BCM_IF_ERROR_RETURN(READ_CHIP_CONFIGr(unit, REG_PORT_ANY, &rval));
        qmode_en = soc_reg_field_get(unit, CHIP_CONFIGr, rval, QMODEf);
        if (qmode_en) {
            BCM_IF_ERROR_RETURN(_bcm_gh_port_pmq_swrst_release(unit));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh_port_priority_group_mapping_set
 * Purpose:
 *      Assign a Priority Group for the input priority.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      prio       - (IN) input priority.
 *      priority_group  - (IN) priority group ID that the input priority mapped to.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_gh_port_priority_group_mapping_set(int unit, bcm_gport_t gport, int prio, int priority_group)
{
    int rv = BCM_E_UNAVAIL;
    bcm_port_t port;
    uint32 value;
    int field_count;
    soc_field_t  fields_0[]={PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                            PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf};
    soc_field_t  fields_1[]={PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
                            PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf};

    if (soc_feature(unit, soc_feature_priority_flow_control) &&
        soc_feature(unit, soc_feature_gh_style_pfc_config)) {
        /* PFC : 0-7, SAFC : 0-15. Taking the larger range for comparison */
        if ((prio < GH_SAFC_INPUT_PRIORITY_MIN) || 
            (prio > GH_SAFC_INPUT_PRIORITY_MAX)) {
            return BCM_E_PARAM;
        }

        if ((priority_group < GH_PRIOROTY_GROUP_ID_MIN) || 
            (priority_group > GH_PRIOROTY_GROUP_ID_MAX)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_local_get(unit, gport, &port));
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        field_count = COUNTOF(fields_0);
        if (prio < field_count) {
            /* input priority 0~7 */
            BCM_IF_ERROR_RETURN(READ_PG_CTRL0r(unit, port, &value));
            soc_reg_field_set(unit, PG_CTRL0r, &value, 
                                fields_0[prio], priority_group);
            BCM_IF_ERROR_RETURN(WRITE_PG_CTRL0r(unit, port, value));
            return BCM_E_NONE;
        } else {
            /* input priority 8~15 */
            BCM_IF_ERROR_RETURN(READ_PG_CTRL1r(unit, port, &value));
            soc_reg_field_set(unit, PG_CTRL1r, &value, 
                                fields_1[prio-field_count], priority_group);
            BCM_IF_ERROR_RETURN(WRITE_PG_CTRL1r(unit, port, value));
            return BCM_E_NONE;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_gh_port_priority_group_mapping_get
 * Purpose:
 *      Get the assigned Priority Group ID of the input priority.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      prio       - (IN) input priority.
 *      priority_group  - (OUT) priority group ID that the input priority mapped to.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_gh_port_priority_group_mapping_get(int unit, bcm_gport_t gport, int prio, int *priority_group)
{
    int rv = BCM_E_UNAVAIL;
    bcm_port_t port;
    uint32 value;
    int field_count;
    soc_field_t  fields_0[]={PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                            PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf};
    soc_field_t  fields_1[]={PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
                            PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf};

    if (soc_feature(unit, soc_feature_priority_flow_control) &&
        soc_feature(unit, soc_feature_gh_style_pfc_config)) {
        /* PFC : 0-7, SAFC : 0-15. Taking the larger range for comparison */
        if ((prio < GH_SAFC_INPUT_PRIORITY_MIN) || 
            (prio > GH_SAFC_INPUT_PRIORITY_MAX)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_local_get(unit, gport, &port));
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        field_count = COUNTOF(fields_0);
        if (prio < field_count) {
            /* input priority 0~7 */
            BCM_IF_ERROR_RETURN(READ_PG_CTRL0r(unit, port, &value));
            *priority_group = soc_reg_field_get(unit, PG_CTRL0r, value, 
                                                fields_0[prio]);
            return BCM_E_NONE;
        } else {
            /* input priority 8~15 */
            BCM_IF_ERROR_RETURN(READ_PG_CTRL1r(unit, port, &value));
            *priority_group = soc_reg_field_get(unit, PG_CTRL1r, value, 
                                                fields_1[prio-field_count]);
            return BCM_E_NONE;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_gh_port_priority_group_config_set
 * Purpose:
 *      Set the port priority group configuration.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      priority_group - (IN) priority group id.
 *      prigrp_config  - (IN) structure describes port priority group configuration.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_gh_port_priority_group_config_set(int unit, bcm_gport_t gport, int priority_group, bcm_port_priority_group_config_t *prigrp_config)
{
    int rv = BCM_E_UNAVAIL;
    bcm_port_t port;
    uint32 rval;
    uint32 fval;

    if (soc_feature(unit, soc_feature_priority_flow_control) &&
        soc_feature(unit, soc_feature_gh_style_pfc_config)) {
        if ((priority_group < GH_PRIOROTY_GROUP_ID_MIN) || 
            (priority_group > GH_PRIOROTY_GROUP_ID_MAX)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_local_get(unit, gport, &port));
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }

        BCM_IF_ERROR_RETURN(READ_PG_CTRL0r(unit, port, &rval));
        fval = soc_reg_field_get(unit, PG_CTRL0r, rval, PPFC_PG_ENf);
        if (prigrp_config->pfc_transmit_enable) {
            fval |= (1 << priority_group);
        } else {
            fval &= ~(1 << priority_group);
        }
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PPFC_PG_ENf, fval);
        BCM_IF_ERROR_RETURN(WRITE_PG_CTRL0r(unit, port, rval));

        BCM_IF_ERROR_RETURN(READ_SHARED_POOL_CTRLr(unit, port, &rval));

        fval = soc_reg_field_get(unit, SHARED_POOL_CTRLr, rval, SHARED_POOL_XOFF_ENf);
        if (prigrp_config->shared_pool_xoff_enable) {
            fval |= (1 << priority_group);
        } else {
            fval &= ~(1 << priority_group);
        }
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval, SHARED_POOL_XOFF_ENf, fval);

        fval = soc_reg_field_get(unit, SHARED_POOL_CTRLr, rval, SHARED_POOL_DISCARD_ENf);
        if (prigrp_config->shared_pool_discard_enable) {
            fval |= (1 << priority_group);
        } else {
            fval &= ~(1 << priority_group);
        }
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval, SHARED_POOL_DISCARD_ENf, fval);

        BCM_IF_ERROR_RETURN(WRITE_SHARED_POOL_CTRLr(unit, port, rval));

        BCM_IF_ERROR_RETURN(READ_PG2TCr(unit, port, priority_group, &rval));
        soc_reg_field_set(unit, PG2TCr, &rval, 
                  PG_BMPf, prigrp_config->priority_enable_vector_mask);
        BCM_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, priority_group, rval));

        return BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *      bcm_gh_port_priority_group_config_get
 * Purpose:
 *      Get the port priority group configuration.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      priority_group - (IN) priority group id.
 *      prigrp_config  - (OUT) structure describes port priority group configuration.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_gh_port_priority_group_config_get(int unit, bcm_gport_t gport, int priority_group, bcm_port_priority_group_config_t *prigrp_config)
{
    int rv = BCM_E_UNAVAIL;
    bcm_port_t port;
    uint32 rval;
    uint32 fval;

    if (soc_feature(unit, soc_feature_priority_flow_control) &&
        soc_feature(unit, soc_feature_gh_style_pfc_config)) {
        if ((priority_group < GH_PRIOROTY_GROUP_ID_MIN) || 
            (priority_group > GH_PRIOROTY_GROUP_ID_MAX)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_local_get(unit, gport, &port));
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }

        BCM_IF_ERROR_RETURN(READ_PG_CTRL0r(unit, port, &rval));
        fval = soc_reg_field_get(unit, PG_CTRL0r, rval, PPFC_PG_ENf);
        if (fval & (1 << priority_group)) {
            prigrp_config->pfc_transmit_enable = 1;
        } else {
            prigrp_config->pfc_transmit_enable = 0;
        }

        BCM_IF_ERROR_RETURN(READ_SHARED_POOL_CTRLr(unit, port, &rval));

        fval = soc_reg_field_get(unit, SHARED_POOL_CTRLr, rval, SHARED_POOL_XOFF_ENf);
        if (fval & (1 << priority_group)) {
            prigrp_config->shared_pool_xoff_enable = 1;
        } else {
            prigrp_config->shared_pool_xoff_enable = 0;
        }

        fval = soc_reg_field_get(unit, SHARED_POOL_CTRLr, rval, SHARED_POOL_DISCARD_ENf);
        if (fval & (1 << priority_group)) {
            prigrp_config->shared_pool_discard_enable = 1;
        } else {
            prigrp_config->shared_pool_discard_enable = 0;
        }

        BCM_IF_ERROR_RETURN(READ_PG2TCr(unit, port, priority_group, &rval));
        prigrp_config->priority_enable_vector_mask = 
                soc_reg_field_get(unit, PG2TCr, rval, PG_BMPf);

        return BCM_E_NONE;
    }

    return rv;
}

#endif /* BCM_GREYHOUND_SUPPORT */
