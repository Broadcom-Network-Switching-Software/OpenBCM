/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/defs.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/cmic.h>

#include <sal/core/time.h>
#include <shared/bsl.h>
#include <bcm/port.h>
#include <bcm/tx.h>
#include <bcm/error.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/cosq.h>

#include <soc/katana2.h>
#include <soc/phyreg.h>

#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#include <bcm_int/esw/saber2.h>
#endif

#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#include <bcm_int/esw/metrolite.h>
#endif

#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/virtual.h>
#include <soc/macutil.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/xgs5.h>

#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif
#include <bcm_int/esw/portctrl.h>

static uint32             new_tdm[256] = {0};
extern int mxqblock_max_startaddr[KT2_MAX_MXQBLOCKS];
extern int mxqblock_max_endaddr[KT2_MAX_MXQBLOCKS];
extern int 
bcm_katana2_cosq_reconfigure_flexio(int unit, bcm_port_t port, int lanes);
extern int soc_kt_port_flush(int unit, int port, int enable);
extern int _soc_katana2_mmu_init_helper_dyn(int unit, int port, int ext_mem_enable);
extern int _bcm_saber2_mmu_init_helper_dyn(int unit, int port, int ext_mem_enable);
/*
 * Function Vector
 */
static bcm_esw_port_drv_t bcm_katana2_port_drv = {
    /* fn_drv_init                */ bcmi_katana2_port_fn_drv_init,
    /* resource_set               */ NULL,
    /* resource_get               */ NULL,
    /* resource_multi_set         */ NULL,
    /* resource_speed_set         */ NULL,
    /* resource_traverse          */ NULL,
    /* port_redirect_set          */ bcmi_xgs5_port_redirect_config_set,
    /* port_redirect_get          */ bcmi_xgs5_port_redirect_config_get,
    /* port_enable_set            */ NULL
};

/*
 * Device Specific HW Tables
 */

/* Egr port table */
static bcmi_xgs5_port_redirection_egr_port_table_t
            bcmi_katana2_port_redirection_egr_port_table = {
    /* mem */ 	              EGR_PORTm,
    /* dest_type */           REDIRECTION_TYPEf,
    /* dest_value */          REDIRECTION_DESTINATIONf,
    /* drop_original */       DROP_ORIGINAL_COPYf,
    /* pkt_priority */        REDIRECT_INT_PRIf,
    /* pkt_change_priority */ REDIRECT_INT_PRI_SELf,
    /* pkt_color */           REDIRECT_DROP_PRECEDENCEf,
    /* strength */            INVALIDf,
    /* buffer_priority */     INVALIDf,
    /* action */              INVALIDf,
    /* redir_pkt_source */    INVALIDf,
    /* redir_pkt_truncate */  INVALIDf
};


/* HW Definitions */
static bcmi_xgs5_port_hw_defs_t  bcmi_katana2_port_hw_defs;

/*
 * Function:
 *      bcmi_katana2_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_katana2_port_fn_drv_init(int unit)
{
    /* HW Definition Tables */
    sal_memset(&bcmi_katana2_port_hw_defs, 0,
               sizeof(bcmi_katana2_port_hw_defs));
    bcmi_katana2_port_hw_defs.egr_port =
        &bcmi_katana2_port_redirection_egr_port_table;

    /* Initialize Common XGS5 Port module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_fn_drv_init(unit, &bcm_katana2_port_drv, NULL,
                             &bcmi_katana2_port_hw_defs));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_port_cfg_init
 * Purpose:
 *      Initialize port configuration according to Initial System
 *      Configuration (see init.c)
 * Parameters:
 *      unit       - (IN)StrataSwitch unit number.
 *      port       - (IN)Port number.
 *      vlan_data  - (IN)Initial VLAN data information
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_port_cfg_init(int unit, bcm_port_t port, bcm_vlan_data_t *vlan_data)
{
    port_tab_entry_t ptab;                 /* Port table entry. */
    soc_field_t field_ids[SOC_MAX_MEM_FIELD_WORDS];
    uint32 field_values[SOC_MAX_MEM_FIELD_WORDS];
    uint32 cascaded_type = 0, port_type = 0;
    int field_count = 0;
    int key_type_value = 0;
    bcm_pbmp_t port_pbmp;
    bcm_port_t max_ports = KT2_MAX_LOGICAL_PORTS;

#if defined(BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        max_ports = ML_MAX_LOGICAL_PORTS;
    } else
#endif
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        max_ports = SB2_MAX_LOGICAL_PORTS;
    } 
#endif

    if (port < max_ports) {
        if (IS_ST_PORT(unit, port)) {
            port_type = KT2_PORT_TYPE_HIGIG;
        } else if (IS_LB_PORT(unit, port)) {
            port_type = KT2_PORT_TYPE_LOOPBACK;
        } else if (IS_LP_PORT(unit, port)) {
            port_type = KT2_PORT_TYPE_CASCADED;
            cascaded_type = KT2_CASCADED_TYPE_LINKPHY;
        } else if (IS_SUBTAG_PORT(unit, port)) {
            port_type = KT2_PORT_TYPE_CASCADED;
            cascaded_type = KT2_CASCADED_TYPE_SUBTAG;
        } else {
            port_type = KT2_PORT_TYPE_ETHERNET;
        }

        BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
                   unit, ING_PHYSICAL_PORT_TABLEm, port,
                   PORT_TYPEf,port_type));
        BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
                   unit, ING_PHYSICAL_PORT_TABLEm, port,
                   CASCADED_PORT_TYPEf, cascaded_type));
        BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
                       unit, EGR_PHYSICAL_PORTm, port,
                       PORT_TYPEf,port_type));
        BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
                   unit, ING_PHYSICAL_PORT_TABLEm, port,
                   CASCADED_PORT_TYPEf, cascaded_type));

        if (IS_CPU_PORT(unit,port)) {
            if (SOC_INFO(unit).cpu_hg_index != -1) {
                BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
                       unit, ING_PHYSICAL_PORT_TABLEm,
                       SOC_INFO(unit).cpu_hg_index, PORT_TYPEf, 1));
                BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
                       unit, EGR_PHYSICAL_PORTm, SOC_INFO(unit).cpu_hg_index,
                       PORT_TYPEf, 1));
            }
        }


        /* ingress physical port to pp_port mapping */
        if (port_type != KT2_PORT_TYPE_CASCADED) {
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
                unit, ING_PHYSICAL_PORT_TABLEm, port, PP_PORTf, port));
        } else {
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
                unit, ING_PHYSICAL_PORT_TABLEm, port, PP_PORTf, 
                ((1 << soc_mem_field_length(unit, ING_PHYSICAL_PORT_TABLEm, PP_PORTf)) - 1)));
        }

        /* pp_port to egress physical port mapping */
        BCM_IF_ERROR_RETURN(soc_mem_field32_modify(
           unit, PP_PORT_TO_PHYSICAL_PORT_MAPm, port, DESTINATIONf,port));
    }

    field_ids[field_count] = EN_EFILTERf;
    field_values[field_count] = 1;
    field_count ++;

    /* Initialize egress vlan translation port class with identity mapping */
    BCM_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_ASSIGN(port_pbmp, PBMP_PORT_ALL(unit));
    if (soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &port_pbmp));
    }

    field_ids[field_count] = VT_PORT_GROUP_IDf;
    field_values[field_count] =
        SOC_PBMP_MEMBER(port_pbmp, port) ? port : 0;
    field_count++;

    /* For some devices, directed mirroring will always be 1 */
    if (soc_feature(unit, soc_feature_directed_mirror_only)) {
        field_ids[field_count] = EM_SRCMOD_CHANGEf;
        field_values[field_count] = 1;
        field_count++;
    }

    if (SOC_MEM_IS_VALID(unit, EGR_PORTm)) {
        BCM_IF_ERROR_RETURN 
            (soc_mem_fields32_modify(unit, EGR_PORTm, port, field_count,
                                     field_ids, field_values));
    }

    /* Write PORT_TABm memory */
    sal_memcpy(&ptab, soc_mem_entry_null(unit, PORT_TABm), sizeof (ptab));

    /* Set default  vlan id(pvid) for port. */
    soc_PORT_TABm_field32_set(unit, &ptab, PORT_VIDf, vlan_data->vlan_tag);

    /* Switching VLAN. */
    if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, OUTER_TPIDf)) {
        soc_PORT_TABm_field32_set(unit, &ptab, OUTER_TPIDf, 0x8100);
    }

    /* Enable mac based vlan classification. */
    soc_PORT_TABm_field32_set(unit, &ptab, MAC_BASED_VID_ENABLEf, 1);

    /* Enable subned based vlan classification. */
    soc_PORT_TABm_field32_set(unit, &ptab, SUBNET_BASED_VID_ENABLEf, 1);


    if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, TRUST_INCOMING_VIDf)) {
        soc_PORT_TABm_field32_set(unit, &ptab, TRUST_INCOMING_VIDf, 1);

        /* Set identify mapping for pri/cfi re-mapping */
        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, PRI_MAPPINGf)) {
            soc_PORT_TABm_field32_set(unit, &ptab, PRI_MAPPINGf, 0xfac688);
            soc_PORT_TABm_field32_set(unit, &ptab, CFI_0_MAPPINGf, 0);
            soc_PORT_TABm_field32_set(unit, &ptab, CFI_1_MAPPINGf, 1);
        }
    
        /* Set identify mapping for ipri/icfi re-mapping */
        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, IPRI_MAPPINGf)) {
            soc_PORT_TABm_field32_set(unit, &ptab, IPRI_MAPPINGf, 0xfac688);
            soc_PORT_TABm_field32_set(unit, &ptab, ICFI_0_MAPPINGf, 0);
            soc_PORT_TABm_field32_set(unit, &ptab, ICFI_1_MAPPINGf, 1);
        }
    
        /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, CML_FLAGS_NEWf)) {
            soc_PORT_TABm_field32_set(unit, &ptab, CML_FLAGS_NEWf, 0x8);
            soc_PORT_TABm_field32_set(unit, &ptab, CML_FLAGS_MOVEf, 0x8);
        }
        /* Set first VLAN_XLATE key-type to {outer,port}.
         * Set second VLAN_XLATE key-type to {inner,port}.
         */
        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, VT_KEY_TYPEf)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_pt_vtkey_type_value_get(unit,
                         KT2_VLXLT_HASH_KEY_TYPE_OVID,&key_type_value));
            soc_PORT_TABm_field32_set(unit, &ptab, VT_KEY_TYPEf,
                                      key_type_value);
            BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                          VT_KEY_TYPEf, key_type_value));
        }

        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, VT_KEY_TYPE_USE_GLPf)) {
            soc_PORT_TABm_field32_set(unit, &ptab, VT_KEY_TYPE_USE_GLPf, 1);
            BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                                   VT_KEY_TYPE_USE_GLPf, 1));
        }
        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, VT_PORT_TYPE_SELECTf)) {
            soc_PORT_TABm_field32_set(unit, &ptab, VT_PORT_TYPE_SELECTf, 1);
            BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                                   VT_PORT_TYPE_SELECTf, 1));
        }
        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, VT_KEY_TYPE_2f)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_pt_vtkey_type_value_get(unit,
                         KT2_VLXLT_HASH_KEY_TYPE_IVID,&key_type_value));
            soc_PORT_TABm_field32_set(unit, &ptab, VT_KEY_TYPE_2f,
                                      key_type_value);
            BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                             VT_KEY_TYPE_2f, key_type_value));

        }
        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, VT_KEY_TYPE_2_USE_GLPf)) {
            soc_PORT_TABm_field32_set(unit, &ptab, VT_KEY_TYPE_2_USE_GLPf, 1);
            BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                                   VT_KEY_TYPE_2_USE_GLPf, 1));
        }
        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, VT_PORT_TYPE_SELECT_2f)) {
            soc_PORT_TABm_field32_set(unit, &ptab, VT_PORT_TYPE_SELECT_2f, 1);
            BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                                   VT_PORT_TYPE_SELECT_2f, 1));
        }
    
        /* Trust the outer tag pri/cfi (to be backwards compatible) */
        if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, TRUST_OUTER_DOT1Pf)) {
            soc_PORT_TABm_field32_set(unit, &ptab, TRUST_OUTER_DOT1Pf, 1);
        }
    }

    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_WRITE(unit, PORT_TABm, port, &ptab));

    return (BCM_E_NONE);
}

int
bcm_kt2_port_cfg_init(int unit, bcm_port_t port, bcm_vlan_data_t *vlan_data)
{
#ifdef BCM_KATANA2_SUPPORT
    if(SOC_IS_KATANA2(unit)) {
        return _bcm_kt2_port_cfg_init(unit, port, vlan_data);
    }
#endif /* BCM_KATANA2_SUPPORT */
    return (BCM_E_NONE);
}

int 
_bcm_kt2_port_sw_info_display(int unit, bcm_port_t port) {
    
    int        rv = 0;
    soc_info_t *si = &SOC_INFO(unit);
    int        lanes;
    uint8      mxqblock;
    int        enable;
    int        speed;
    int        link;
    int        autoneg;
    int        discard_mode;
    int        loopback_mode;
    int        master_mode;
    int        encap_mode = BCM_PORT_ENCAP_COUNT;
    char       *discard[] = {"NONE", "ALL", "UNTAG", "TAG"};
    char       *loopback[] = {"NONE", "MAC", "PHY", "RMT", "C57"};
    char       *master[] = {"SLAVE", "MASTER", "AUTO", "NONE"};
    char       *encap[] = {"IEEE", "HIGIG", "B5632", "HIGIG2", "HIGIG-Lite"};
    
    rv += (_bcm_kt2_port_lanes_get(unit, port, &lanes));
    rv += (soc_katana2_get_port_mxqblock(unit,port,&mxqblock));
    rv += (bcm_esw_port_enable_get(unit, port, &enable));
    rv += (bcm_esw_port_autoneg_get(unit, port, &autoneg));
    rv += (bcm_esw_port_speed_get(unit, port, &speed));
    rv += (bcm_esw_port_link_status_get(unit, port, &link));
    rv += (bcm_esw_port_discard_get(unit, port, &discard_mode));
    rv += (bcm_esw_port_loopback_get(unit, port, &loopback_mode));
    rv += (bcm_esw_port_master_get(unit, port, &master_mode));
    rv += (bcm_esw_port_encap_get(unit, port, &encap_mode));

    if (rv < 0) {
        LOG_CLI((BSL_META_U(unit, "\n\n Some parameters not set properly !!")));
    }
    LOG_CLI((BSL_META_U(unit, "\n\n SOFTWARE PORT CONFIGURATION :\n")));
    LOG_CLI((BSL_META_U(unit, "------------------------------------")));
    LOG_CLI((BSL_META_U(unit, "\n Port Name\t\t:\t%s"),si->port_name[port]));
    LOG_CLI((BSL_META_U(unit, "\n MxQBlock ID\t\t:\t%d"), mxqblock));
    LOG_CLI((BSL_META_U(unit, "\n Lane Number\t\t:\t%d "),lanes));
    LOG_CLI((BSL_META_U(unit, "\n Current Speed\t\t:\t%d"), speed));
    LOG_CLI((BSL_META_U(unit, "\n Max speed\t\t:\t%d"),si->port_speed_max[port]));
    LOG_CLI((BSL_META_U(unit, "\n MxQSpeed\t\t:\t%d"),((*mxqspeeds[unit])[mxqblock][lanes-1])));
    LOG_CLI((BSL_META_U(unit, "\n Port status \t\t:\t%s"),(enable ? "ENABLED" : "DISABLED"))); 
    LOG_CLI((BSL_META_U(unit, "\n Link status\t\t:\t%s"),(link ? "UP" : "DOWN")));
    LOG_CLI((BSL_META_U(unit, "\n Autoneg status\t\t:\t%s"),(autoneg ? "TRUE" : "FALSE")));
    LOG_CLI((BSL_META_U(unit, "\n Discard Type\t\t:\t%s"),(discard_mode <= 3 ? discard[discard_mode] : "INVALID")));
    LOG_CLI((BSL_META_U(unit, "\n Loopback Mode\t\t:\t%s"),(loopback_mode <= 4 ? loopback[loopback_mode] : "INVALID")));
    LOG_CLI((BSL_META_U(unit, "\n Master Status\t\t:\t%s"),(master_mode <= 3 ? master[master_mode] : "INVALID")));
    LOG_CLI((BSL_META_U(unit, "\n Encapsulation \t\t:\t%s"),(encap_mode <= 4 ? encap[encap_mode] : "INVALID")));
    LOG_CLI((BSL_META_U(unit, "\n\n***********************************************\n\n")));
    
    return 0;

}

int 
_bcm_kt2_port_hw_info_display(int unit, bcm_port_t port) {

    uint32 rval = 0;
    uint64 rval_64;
    uint8 mxq_port = port;
    int lanes = 0;
    uint8 mxqblock;
    int port0, port1, port2, port3;
    int higig, higig2, xpause, pfc, drain, llfc;
    int core_mode, phy_mode;
    int tx_en, rx_en, line_lpbk, core_lpbk, line_rm_lpbk, core_rm_lpbk;
    int lpbk_leak, rm_lpbk_leak;
    int soft_reset, rs_soft_reset, sw_link, link_status_select; 
    int hdr_mode, sop_crc, speed_mode; 
    int port_enable;
    egr_enable_entry_t egr_en;

    char *phy[] = {"Single", "Dual", "Quad"};
    char *core[] = {"Single", "Dual", "Quad"};
    char *enable[] = {"DISABLED", "ENABLED"};
    char *active[] = {"INACTIVE", "ACTIVE"};
    char *link_sel[] = {"SOFTWARE", "STRAP-PIN"};
    char *hdr[] = {"IEEE", "HG+", "HG2", "CLH", "SCH", "SOP ONLY IEEE"};
    char *speed[] = {"10Mpbps", "100Mbps", "1Gbps", "2.5Gbps", ">=10Gbps"};

    BCM_IF_ERROR_RETURN(_bcm_kt2_port_lanes_get(unit, port, &lanes));
    BCM_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(unit, port, &mxqblock));
    
    LOG_CLI((BSL_META_U(unit, "\n\n HARDWARE PORT CONFIGURATION : \n")));
    LOG_CLI((BSL_META_U(unit, "--------------------------------------\n")));

    /* XPORT_MODE_REG */
    SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
    core_mode = soc_reg_field_get(unit, XPORT_MODE_REGr, rval, CORE_PORT_MODEf);
    phy_mode  = soc_reg_field_get(unit, XPORT_MODE_REGr, rval, PHY_PORT_MODEf);
    LOG_CLI((BSL_META_U(unit, "\n XPORT MODE ")));
    LOG_CLI((BSL_META_U(unit, "\n Core Mode\t:\t%s"), core[core_mode]));
    LOG_CLI((BSL_META_U(unit, "\n Phy Mode\t:\t%s"), phy[phy_mode]));

    /* XPORT PORT ENABLE */
    SOC_IF_ERROR_RETURN(READ_XPORT_PORT_ENABLEr(unit, port, &rval));
    port0 = soc_reg_field_get(unit, XPORT_PORT_ENABLEr, rval, PORT0f);
    port1 = soc_reg_field_get(unit, XPORT_PORT_ENABLEr, rval, PORT1f);
    port2 = soc_reg_field_get(unit, XPORT_PORT_ENABLEr, rval, PORT2f);
    port3 = soc_reg_field_get(unit, XPORT_PORT_ENABLEr, rval, PORT3f);
    LOG_CLI((BSL_META_U(unit, "\n\n XPORT PORT ENABLE ")));
    LOG_CLI((BSL_META_U(unit, "\n MxQBlock\t:\t%d\n Port0\t\t:\t%s\n Port1\t\t:\t%s"
                 "\n Port2\t\t:\t%s\n Port3\t\t:\t%s"), mxqblock,   \
                 enable[port0], enable[port1], enable[port2], enable[port3]));

    
    /* XPORT CONFIG */
    SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, port, &rval));
    higig = soc_reg_field_get(unit, XPORT_CONFIGr, rval, HIGIG_MODEf);
    higig2 = soc_reg_field_get(unit, XPORT_CONFIGr, rval, HIGIG2_MODEf);
    xpause = soc_reg_field_get(unit, XPORT_CONFIGr, rval, XPAUSE_ENf);
    pfc = soc_reg_field_get(unit, XPORT_CONFIGr, rval, PFC_ENABLEf);
    drain = soc_reg_field_get(unit, XPORT_CONFIGr, rval, DRAIN_CONDITION_ENABLEf);
    llfc = soc_reg_field_get(unit, XPORT_CONFIGr, rval, LLFC_ENf);
    LOG_CLI((BSL_META_U(unit, "\n\n XPORT CONFIG ")));
    LOG_CLI((BSL_META_U(unit, "\n HiGig\t\t:\t%s"), enable[higig]));
    LOG_CLI((BSL_META_U(unit, "\n HiGig2\t\t:\t%s"), enable[higig2]));
    LOG_CLI((BSL_META_U(unit, "\n XPause\t\t:\t%s"), enable[xpause]));
    LOG_CLI((BSL_META_U(unit, "\n Priority FC\t:\t%s"), enable[pfc]));
    LOG_CLI((BSL_META_U(unit, "\n Drain Cndtn\t:\t%s"), enable[drain]));
    LOG_CLI((BSL_META_U(unit, "\n Link Level FC\t:\t%s\n\n"), enable[llfc]));

    /* XMAC CTRL */
    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, mxq_port, &rval_64));
    tx_en = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, TX_ENf);
    rx_en = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, RX_ENf);
    line_lpbk = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, LINE_LOCAL_LPBKf);
    core_lpbk = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, CORE_LOCAL_LPBKf);
    line_rm_lpbk = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, LINE_REMOTE_LPBKf);
    core_rm_lpbk = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, CORE_REMOTE_LPBKf);
    soft_reset = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, SOFT_RESETf);
    rs_soft_reset = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, RS_SOFT_RESETf);
    lpbk_leak = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, LOCAL_LPBK_LEAK_ENBf);
    rm_lpbk_leak = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, REMOTE_LPBK_LEAK_ENBf);
    sw_link = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, SW_LINK_STATUSf);
    link_status_select = soc_reg64_field32_get(unit, XMAC_CTRLr, rval_64, LINK_STATUS_SELECTf);
    LOG_CLI((BSL_META_U(unit, "\n\n XMAC CTRL")));
    LOG_CLI((BSL_META_U(unit, "\n Tx\t \t\t:\t%s"), enable[tx_en]));
    LOG_CLI((BSL_META_U(unit, "\n Rx \t\t\t:\t%s"), enable[rx_en]));
    LOG_CLI((BSL_META_U(unit, "\n Soft Reset\t\t:\t%s"),enable[soft_reset]));
    LOG_CLI((BSL_META_U(unit, "\n RS Soft Reset\t\t:\t%s"), enable[rs_soft_reset]));
    LOG_CLI((BSL_META_U(unit, "\n Line Lpbk(Local)\t:\t%s"), enable[line_lpbk]));
    LOG_CLI((BSL_META_U(unit, "\n Core Lpbk(Local)\t:\t%s"), enable[core_lpbk]));
    LOG_CLI((BSL_META_U(unit, "\n Line Lpbk(Remote)\t:\t%s"), enable[line_rm_lpbk]));
    LOG_CLI((BSL_META_U(unit, "\n Core Lpbk(Remote)\t:\t%s"), enable[core_rm_lpbk]));
    LOG_CLI((BSL_META_U(unit, "\n Lpbk Leak(Local)\t:\t%s"), enable[lpbk_leak]));
    LOG_CLI((BSL_META_U(unit, "\n Lpbk Leak(Remote)\t:\t%s"), enable[rm_lpbk_leak]));
    LOG_CLI((BSL_META_U(unit, "\n SW Link Status\t\t:\t%s"), active[sw_link]));
    LOG_CLI((BSL_META_U(unit, "\n Link Status Select\t:\t%s"), link_sel[link_status_select]));


    /* XMAC MODE */
    SOC_IF_ERROR_RETURN(READ_XMAC_MODEr(unit, mxq_port, &rval_64));
    hdr_mode = soc_reg64_field32_get(unit, XMAC_MODEr, rval_64, HDR_MODEf);
    sop_crc  = soc_reg64_field32_get(unit, XMAC_MODEr, rval_64, NO_SOP_FOR_CRC_HGf);
    speed_mode = soc_reg64_field32_get(unit, XMAC_MODEr, rval_64, SPEED_MODEf);
    LOG_CLI((BSL_META_U(unit, "\n\n XMAC MODE")));
    LOG_CLI((BSL_META_U(unit, "\n Header Mode\t\t:\t%s"),hdr[hdr_mode]));
    LOG_CLI((BSL_META_U(unit, "\n SOP for CRC in HG\t:\t%s"), (sop_crc ? "EXCLUDE" : "INCLUDE")));
    LOG_CLI((BSL_META_U(unit, "\n Speed Mode\t\t:\t%s"), speed[speed_mode]));
   

    /* EGR ENABLE */
    SOC_IF_ERROR_RETURN(READ_EGR_ENABLEm(unit, SOC_BLOCK_ANY, port, &egr_en));
    port_enable = soc_EGR_ENABLEm_field32_get(unit, &egr_en, PRT_ENABLEf);
    LOG_CLI((BSL_META_U(unit, "\n\n EGR ENABLE")));
    LOG_CLI((BSL_META_U(unit, "\n Port\t\t:\t%s"),enable[port_enable]));


  
    LOG_CLI((BSL_META_U(unit, "\n\n***************************************************************************************\n")));
    LOG_CLI((BSL_META_U(unit, " NOTE : You can run the following TR Loopback tests to verify sanity of port\n")));
    LOG_CLI((BSL_META_U(unit, " PHY:\n tr 19 pbm=%d\n"),port));
    LOG_CLI((BSL_META_U(unit, " MAC:\n tr 18 pbm=%d\n"),port));
    LOG_CLI((BSL_META_U(unit, "*******************************************************************************************\n\n")));
    LOG_CLI((BSL_META_U(unit, "Basic Port Status :\n\n")));

    return 0;


}

int
_bcm_kt2_port_lanes_set_post_operation(int unit, bcm_port_t port)
{
    soc_error_t        rv = SOC_E_NONE;
#if 0
    uint32             rval = 0;
#endif
    uint8              loop = 0;
    uint8              mxqblock = 0;
    uint8              mxqblock_port = 0;
    int                mode = 0;
    int                new_lanes = 0;
    int                speed=0, max_speed=0; 
    int                port_enable = 0;
    egr_enable_entry_t egr_enable_entry = {{0}};
#if 0
    soc_timeout_t      to = {0};
    sal_usecs_t        timeout_usec = 100000; /* 100ms */
    int                min_polls = 100;
#endif

    sal_memset(&egr_enable_entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, &egr_enable_entry, PRT_ENABLEf, 1);

    SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(
                        unit,port,&mxqblock));
    SOC_IF_ERROR_RETURN(_bcm_kt2_port_lanes_get(
                        unit,port,&new_lanes));

#if 0
    /* Bring-out of Soft-reset relevant MXQPORT(s) */
    /* Delay of 100ms */
    soc_timeout_init(&to, timeout_usec, min_polls);
    if(soc_timeout_check(&to)) {
       LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                             "_bcm_kt2_port_lanes_set_post_operation:TimeOut InternalError\n")));
       return SOC_E_INTERNAL; 
    }
    soc_katana2_mxqblock_reset(unit, mxqblock, 1);
#endif

#ifdef BCM_PORT_DEFAULT_DISABLE
    port_enable = FALSE;
#else
    port_enable = TRUE;
#endif  /* BCM_PORT_DEFAULT_DISABLE */

    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             /* Configuring default settings */
             BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit,mxqblock_port,port_enable));
             BCM_IF_ERROR_RETURN(bcm_esw_port_loopback_set(unit, mxqblock_port, 
                         BCM_PORT_LOOPBACK_NONE)); 
             kt2_tdm_update_flag=0;
             speed = (*mxqspeeds[unit])[mxqblock][new_lanes-1];
             BCM_IF_ERROR_RETURN(bcm_esw_port_speed_max(unit, mxqblock_port,
                                                        &max_speed));
             speed = speed > max_speed ? max_speed : speed;
             rv = bcm_esw_port_speed_set(unit, mxqblock_port ,
                                         speed);
             kt2_tdm_update_flag=1;
             SOC_IF_ERROR_RETURN(rv);
             SOC_IF_ERROR_RETURN(bcm_esw_linkscan_mode_get(
                                 unit, mxqblock_port, &mode));
             if (mode == BCM_LINKSCAN_MODE_NONE) {
                 BCM_IF_ERROR_RETURN(bcm_esw_linkscan_mode_set(
                                     unit,mxqblock_port,BCM_LINKSCAN_MODE_SW));
             }

             BCM_IF_ERROR_RETURN(bcm_esw_port_learn_set(unit, mxqblock_port,
                                 BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
             BCM_IF_ERROR_RETURN(bcm_esw_port_stp_set(
                                 unit, mxqblock_port, BCM_STG_STP_FORWARD));
#if 0
             BCM_IF_ERROR_RETURN(soc_mem_write(
                                 unit, EGR_ENABLEm, MEM_BLOCK_ALL,
                                 mxqblock_port, &egr_enable_entry));
             /* 6. PowerDown MXQBlock/WarpCore */
             READ_XPORT_XGXS_CTRLr(unit, mxqblock_port,&rval);
             soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval,
                               PWRDWNf,0);
             WRITE_XPORT_XGXS_CTRLr(unit, mxqblock_port,rval);
#endif
         }
    }
    return SOC_E_NONE;
}
static 
void soc_katana2_pbmp_one_resync(int unit,soc_ptype_t *ptype)
{
    soc_port_t it_port = 0 ;

    ptype->num = 0; \
    ptype->min = ptype->max = -1;

    PBMP_ITER(ptype->bitmap, it_port) {
              ptype->port[ptype->num++] = it_port;
              if (ptype->min < 0) { \
                  ptype->min = it_port;
              }
              if (it_port > ptype->max) {
                  ptype->max = it_port;
              }
    }
}

void soc_katana2_pbmp_all_resync(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_katana2_pbmp_one_resync(unit,&si->xe);
    soc_katana2_pbmp_one_resync(unit,&si->ether);
    soc_katana2_pbmp_one_resync(unit,&si->mxq);
    soc_katana2_pbmp_one_resync(unit,&si->port);
    soc_katana2_pbmp_one_resync(unit,&si->all);
    soc_katana2_pbmp_one_resync(unit,&si->ge);
    soc_katana2_pbmp_one_resync(unit,&si->hg);
    soc_katana2_pbmp_one_resync(unit,&si->st);
}

void soc_katana2_pbmp_remove(int unit,soc_port_t port)
{
    int         blk = 0;
    soc_info_t *si = &SOC_INFO(unit);

    blk = SOC_DRIVER(unit)->port_info[port].blk;
    SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk],port);

    SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
    SOC_PBMP_PORT_ADD(si->xe.disabled_bitmap, port);

    SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
    SOC_PBMP_PORT_ADD(si->ether.disabled_bitmap, port);

    SOC_PBMP_PORT_REMOVE(si->mxq.bitmap, port);
    SOC_PBMP_PORT_ADD(si->mxq.disabled_bitmap, port);

    SOC_PBMP_PORT_REMOVE(si->port.bitmap, port);
    SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, port);

    SOC_PBMP_PORT_REMOVE(si->all.bitmap, port);
    SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);

    SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
    SOC_PBMP_PORT_ADD(si->ge.disabled_bitmap, port);

    SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
    SOC_PBMP_PORT_ADD(si->hg.disabled_bitmap, port);

    SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
    SOC_PBMP_PORT_ADD(si->st.disabled_bitmap, port);

}
static 
void soc_katana2_pbmp_add(int unit,soc_port_t port,int mxqblock,int speed) {
    int blk=0;
    soc_info_t *si = &SOC_INFO(unit);
    soc_pbmp_t  my_pbmp_xport_xe;

    blk = SOC_DRIVER(unit)->port_info[port].blk;
    SOC_PBMP_PORT_ADD(si->block_bitmap[blk],port);


    if ((mxqblock >=0) && (mxqblock <= 5)) {
         if (speed == 10000) {
             SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
             SOC_PBMP_PORT_REMOVE(si->xe.disabled_bitmap, port);
         } else {
             SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
             SOC_PBMP_PORT_REMOVE(si->ge.disabled_bitmap, port);
         }
         SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->ether.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->mxq.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->mxq.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->port.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->port.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->all.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);
    }
    if ((mxqblock >=6) && (mxqblock <= 9)) {
         SOC_PBMP_CLEAR(my_pbmp_xport_xe);
         my_pbmp_xport_xe = soc_property_get_pbmp_default(unit,
                                                          spn_PBMP_XPORT_XE,
                                                          my_pbmp_xport_xe);
         if (speed >= 13000) {
             if (SOC_PBMP_MEMBER(my_pbmp_xport_xe, port)) {
                 SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
                 SOC_PBMP_PORT_REMOVE(si->xe.disabled_bitmap, port);
             } else {
                 SOC_PBMP_PORT_ADD(si->hg.bitmap, port);
                 SOC_PBMP_PORT_ADD(si->st.bitmap, port);
                 SOC_PBMP_PORT_REMOVE(si->hg.disabled_bitmap, port);
                 SOC_PBMP_PORT_REMOVE(si->st.disabled_bitmap, port);
             }
         } else if (speed == 10000) {
             SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
             SOC_PBMP_PORT_REMOVE(si->xe.disabled_bitmap, port);
         } else {
             SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
             SOC_PBMP_PORT_REMOVE(si->ge.disabled_bitmap, port);
         }
         SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->ether.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->mxq.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->mxq.disabled_bitmap, port);

         SOC_PBMP_PORT_ADD(si->port.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->port.disabled_bitmap, port);
         SOC_PBMP_PORT_ADD(si->all.bitmap, port);
         SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);
    }
    SOC_PORT_TYPE(unit,port)=SOC_BLK_MXQPORT;
    si->port_speed_max[port] = speed;
    if (port == KT2_OLP_PORT) {
        if (SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].bindex != 3) {
            SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].bindex = 3;
            SOC_DRIVER(unit)->port_info[KT2_OLP_PORT].blk = SOC_DRIVER(unit)->port_info[26].blk;
        }
    }
}

static
soc_error_t soc_katana2_port_attach(
            int unit,uint8 mxqblock ,uint8 mxqblock_port)
{
    uint16 phy_addr=0;
    uint16 phy_addr_int=0;
    int    okay;
    bcm_error_t rv = SOC_E_NONE;

    soc_linkscan_pause(unit);
    _katana2_phy_addr_default(unit, mxqblock_port,
                              &phy_addr, &phy_addr_int);
    SOC_IF_ERROR_RETURN
        (soc_phy_cfg_addr_set(unit,mxqblock_port,0, phy_addr));
    SOC_IF_ERROR_RETURN
        (soc_phy_cfg_addr_set(unit,mxqblock_port,SOC_PHY_INTERNAL,
                              phy_addr_int));
    PHY_ADDR(unit, mxqblock_port)     = phy_addr;
    PHY_ADDR_INT(unit, mxqblock_port) = phy_addr_int;
    SOC_IF_ERROR_RETURN(_bcm_port_probe(unit, mxqblock_port, &okay));
    if (!okay) {
        soc_linkscan_continue(unit);
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(_bcm_port_mode_setup(unit, mxqblock_port, FALSE));
    if (IS_HG_PORT(unit, mxqblock_port)) {
          bcm_esw_port_encap_set(unit,mxqblock_port,BCM_PORT_ENCAP_HIGIG);
          /* Set the settings specific to HiGig port.
           * update_port_mode is set to FALSE since port mode update was
           * already done as part of flex port sequence. Also, port mode
           * should be updated only when mxqblock is in hard reset(XMAC_RESET=1)
           * or else we would be having issue in cell count requests in EP.
           */
          _bcm_port_encap_xport_set(unit,mxqblock_port,BCM_PORT_ENCAP_HIGIG,
                                    FALSE);
    } else {
        bcm_esw_port_encap_set(unit,mxqblock_port,BCM_PORT_ENCAP_IEEE);
        /* Set the settings specific to HiGig port.
         * update_port_mode is set to FALSE since port mode update was
         * already done as part of flex port sequence. Also, port mode
         * should be updated only when mxqblock is in hard reset(XMAC_RESET=1)
         * or else we would be having issue in cell count requests in EP.
         */
        _bcm_port_encap_xport_set(unit,mxqblock_port,BCM_PORT_ENCAP_IEEE,
                                  FALSE);
    }
    rv = bcm_esw_port_enable_set(unit,mxqblock_port,1);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                   "Port enable set failed for port[%d]\n"), mxqblock_port));
    }
    rv = _soc_linkscan_hw_port_init(unit, mxqblock_port);
    soc_linkscan_continue(unit);
    return rv;
}
static 
soc_error_t soc_katana2_port_detach(int unit,uint8 mxqblock_port)
{
    bcm_error_t rv = SOC_E_NONE;

    soc_linkscan_pause(unit);
    /* Enabling port before detaching as Flex-IO Unicore GE ports when
     * disabled, not being reset when combined back to XE. This was causing
     * Link up issues at the SERDES level */
    SOC_IF_ERROR_RETURN(soc_phyctrl_enable_set(unit, mxqblock_port, 1));
    SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, mxqblock_port));
    PHY_FLAGS_CLR_ALL(unit, mxqblock_port);

    if (SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), mxqblock_port)) {
        rv = bcm_esw_port_enable_set(unit,mxqblock_port,0);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                       "Port enable set failed for port[%d]\n"),
                                mxqblock_port));
        }
    }
    soc_linkscan_continue(unit);
    return rv;
}
int _bcm_kt2_update_port_mode(int unit,uint8 port,int speed)
{
   uint32 rval = 0;
   uint32 phy_mode = 0;
   uint32 core_mode = 0;
   uint32 wc_10g_21g_sel = 0;
   uint8  mxqblock = 0;
   bcmMxqConnection_t connection_mode;

   SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
   SOC_IF_ERROR_RETURN(soc_katana2_get_phy_port_mode(
                       unit, port, speed, &phy_mode));
   SOC_IF_ERROR_RETURN(soc_katana2_get_core_port_mode(
                       unit, port, &core_mode));
   soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, CORE_PORT_MODEf, core_mode);
   SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock( unit,port,&mxqblock));
   if ((mxqblock == 8) || (mxqblock == 9)) {
        if ((speed == 10000) || (speed == 21000)) {
             wc_10g_21g_sel = 1;
        }
   }
   if ((mxqblock == 6) || (mxqblock == 7)) {
        if (speed == 10000) {
            SOC_IF_ERROR_RETURN(soc_katana2_get_phy_connection_mode(
                                unit,port, mxqblock,&connection_mode));
            /* XFI Mode */
            if (connection_mode == bcmMqxConnectionWarpCore) {
                wc_10g_21g_sel = 1;
            }
        }
   }
   soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, 
                     WC_10G_21G_SELf, wc_10g_21g_sel);
   soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, PHY_PORT_MODEf, phy_mode);
   soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                     PORT_GMII_MII_ENABLEf, (speed >= 10000) ? 0 : 1);
   /* WORK AROUND: KATANA2-1698
      RTL PORT: Hotswap from quad port mode to dual port mode fails 
      occasionally. */
   soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, CORE_PORT_MODEf, 
                    bcmMxqCorePortModeSingle);
   SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
   sal_udelay(100);

   soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, CORE_PORT_MODEf, 
                    core_mode);
   SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
   return BCM_E_NONE;
}

int __bcm_kt2_port_lanes_set(int unit, bcm_port_t port, int lanes)
{
    uint64                xmac_ctrl;
    uint32                port_enable=0;
    int                   length_check = 0;
    epc_link_bmap_entry_t epc_link_bmap_entry={{0}};
    txlp_port_addr_map_table_entry_t txlp_port_addr_map_table_entry={{0}};
    soc_field_t port_enable_field[KT2_MAX_MXQPORTS_PER_BLOCK]=
                                 {PORT0f, PORT1f, PORT2f , PORT3f};

    uint32                start_addr=0;
    uint32                end_addr=0;
    soc_pbmp_t            link_pbmp;
    /* uint32                flush_reg=0; */
    uint32             time_multiplier=1;
    uint64             rval64 ;
    uint32             rval = 0;
    uint32             rval1 = 0;
    uint32             rval2 = 0;
    egr_enable_entry_t egr_enable_entry = {{0}};
    uint32             cell_cnt = 0;
    uint32             try = 0;
    uint32             try_count = 0;
    soc_timeout_t      to = {0};
    sal_usecs_t        timeout_usec = 100000;
    int                min_polls = 100;
    uint32             pfc_enable=0;
    int                pfc_index=0;
    uint8              old_ports[][KT2_MAX_MXQPORTS_PER_BLOCK]=
                                  {{1,0,0,0},{1,0,3,0},{0,0,0,0},{1,2,3,4}};
    uint8              new_ports[][KT2_MAX_MXQPORTS_PER_BLOCK]=
                                  {{1,0,0,0},{1,0,3,0},{0,0,0,0},{1,2,3,4}};
    uint8              new_port_mode[5]={0,1,2,0,3};
    uint32             top_misc_control_1_val = 0;
    soc_field_t        wc_xfi_mode_sel_fld[] =
                       {WC0_8_XFI_MODE_SELf,WC1_8_XFI_MODE_SELf};
    soc_info_t         *si = &SOC_INFO(unit);
    uint8              mxqblock = 0;
    uint8              mxqblock_port = 0;
    uint8              try_loop = 0;
    uint8              loop = 0;
    int                old_lanes = 0;
    int                new_lanes = lanes;
    uint8              xfi_mode[2] = {0,0};
    /* uint8              old_xfi_mode[2] = {0,0}; */
    soc_field_t        port_intf_reset_fld[] = {
                       XQ0_PORT_INTF_RESETf, XQ1_PORT_INTF_RESETf,
                       XQ2_PORT_INTF_RESETf, XQ3_PORT_INTF_RESETf,
                       XQ4_PORT_INTF_RESETf, XQ5_PORT_INTF_RESETf,
                       XQ6_PORT_INTF_RESETf, XQ7_PORT_INTF_RESETf};
    soc_field_t        mmu_intf_reset_fld[] = {
                       XQ0_MMU_INTF_RESETf, XQ1_MMU_INTF_RESETf,
                       XQ2_MMU_INTF_RESETf, XQ3_MMU_INTF_RESETf,
                       XQ4_MMU_INTF_RESETf, XQ5_MMU_INTF_RESETf,
                       XQ6_MMU_INTF_RESETf, XQ7_MMU_INTF_RESETf};
    soc_field_t        new_port_mode_fld[] = {
                       XQ0_NEW_PORT_MODEf, XQ1_NEW_PORT_MODEf,
                       XQ2_NEW_PORT_MODEf, XQ3_NEW_PORT_MODEf,
                       XQ4_NEW_PORT_MODEf, XQ5_NEW_PORT_MODEf,
                       XQ6_NEW_PORT_MODEf, XQ7_NEW_PORT_MODEf};
    uint8              required_tdm_slots = 0;
    uint8              available_tdm_slots = 0;
    uint8              speed_index = 0;
    uint8              speed_value = 0;
    int                port_speed = 0;
    uint8              tdm_ports[4] = {0};
    uint8              index = 0;
    uint32             pos = 0;
    uint32             egr_fifo_depth = 0;
    /* int             cfg_num = 0; */
    int                mxqblock_max_nxtaddr;
    int                init_mode = 0;
    int                threshold, padding = 0;
    int                blk = 0;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (SAL_BOOT_QUICKTURN) {
        time_multiplier=1000;
    }

    if ((new_lanes != 1) && (new_lanes != 2) && (new_lanes != 4)) {
         return BCM_E_PARAM;
    }

    /* cfg_num = soc_property_get(unit, spn_BCM5645X_CONFIG, 0); */

    if (SOC_PORT_TYPE(unit,port) != SOC_BLK_MXQPORT) {
        return SOC_E_CONFIG;
    }
    SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(
                        unit,port,&mxqblock));
    SOC_IF_ERROR_RETURN(_bcm_kt2_port_lanes_get(
                        unit,port,&old_lanes));
    if(old_lanes == new_lanes) {
       LOG_CLI((BSL_META_U(unit, "Port is already in %d lane mode \n "),
                           old_lanes));
       return SOC_E_NONE;
    }

    if ((mxqblock == 8) || (mxqblock == 9)) {
        if (soc_mem_config_set == NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "ATTN:Auto PortGroup Config setting not possible \n")));
        }
    }
    if (SOC_INFO(unit).olp_port[0]) {
        if (port == KT2_OLP_PORT) {
            LOG_CLI((BSL_META_U(unit,
                                "HotSwap is not supported for OLP port \n")));
            return BCM_E_PARAM;
        }
    }
    if (mxqblock == 7 ) {
        if (SOC_INFO(unit).olp_port[0]) {
            LOG_CLI((BSL_META_U(unit,
                                "OLP port is being used so cannot hotswap on MXQ7\n")));
            return BCM_E_PARAM;
        }
    }
    if ((mxqblock==8) || (mxqblock==9)) {
         for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
              if (IS_LP_PORT(unit, (*kt2_mxqblock_ports[unit])[mxqblock][loop])) {
                  LOG_CLI((BSL_META_U(unit,
                                      "HotSwap is not supported for LinkPhy Ports\n")));
                  return BCM_E_PARAM;
              }
         }
    }
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         if (IS_SUBTAG_PORT(unit, (*kt2_mxqblock_ports[unit])[mxqblock][loop])) {
             LOG_CLI((BSL_META_U(unit,
                                 "HotSwap is not supported for SubTag/COE Ports\n")));
             return BCM_E_PARAM;
         }
    }
    if ((mxqblock >= 6 ) && (mxqblock <= 9)) {
        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(
                            unit,&top_misc_control_1_val));
        if ((mxqblock == 6) || (mxqblock == 8)) {
             xfi_mode[0]= soc_reg_field_get(unit, TOP_MISC_CONTROL_1r,
                                            top_misc_control_1_val,
                                            wc_xfi_mode_sel_fld[0]);
        }
        if (xfi_mode[0]) {
            LOG_CLI((BSL_META_U(unit,
                                "XFI Mode enabled. Hot Swapping not possible \n")));
            return SOC_E_UNAVAIL;
        }
        if ((mxqblock == 7) || (mxqblock == 9)) {
             xfi_mode[1]= soc_reg_field_get(unit, TOP_MISC_CONTROL_1r,
                                       top_misc_control_1_val,
                                       wc_xfi_mode_sel_fld[1]);
        }
        if (xfi_mode[1]) {
            LOG_CLI((BSL_META_U(unit,
                                "XFI Mode enabled. Hot Swapping not possible \n")));
            return SOC_E_UNAVAIL;
        }
    }

/*
    if ((new_lanes==4)&& ((mxqblock==8) || (mxqblock==9))) {
         for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
              if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), 
                                   (*kt2_mxqblock_ports[unit])[mxqblock-2][loop])) {
                  LOG_CLI((BSL_META_U(unit,
                                      "MXQBlock%d being used so"
                           " WC cannot be used in XFI mode \n"),mxqblock-2));
                  return SOC_E_CONFIG;
              }
         }
         xfi_mode[mxqblock-8]=1;
         for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
              if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), 
                                   (*kt2_mxqblock_ports[unit])[mxqblock][loop])) {
                  if (!SOC_PBMP_MEMBER((PBMP_XE_ALL(unit)), 
                                        (*kt2_mxqblock_ports[unit])[mxqblock][loop])) {
                      xfi_mode[mxqblock-8]=0;
                      break;
                  }
              }
         }
         if (xfi_mode[mxqblock-8] == 1) {
             LOG_CLI((BSL_META_U(unit,
                                 "NEW XFI MODE ON \n")));
         }
    }
    if ((mxqblock==8) || (mxqblock==9)) {
         old_xfi_mode[mxqblock-8]=1;
         for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
              if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), 
                                   (*kt2_mxqblock_ports[unit])[mxqblock][loop])) {
                  if (!SOC_PBMP_MEMBER((PBMP_XE_ALL(unit)), 
                                   (*kt2_mxqblock_ports[unit])[mxqblock][loop])) {
                      old_xfi_mode[mxqblock-8]=0;
                      break;
                  }
              }
         }
         if (old_xfi_mode[mxqblock-8] == 1) {
             LOG_CLI((BSL_META_U(unit,
                                 "OLD XFI MODE ON \n")));
         }
    }
*/
    switch((*mxqspeeds[unit])[mxqblock][new_lanes-1]) {
    case 1000: speed_index=0;break;
    case 2500: speed_index=2;break;
    case 10000: speed_index=3;break;
    case 13000: speed_index=4;break;
    case 20000:
    case 21000: speed_index=6;break;
    default:    return SOC_E_CONFIG;
    }
    required_tdm_slots = kt2_current_tdm_cycles_info[speed_index].
                         min_tdm_cycles * new_lanes;
    available_tdm_slots = kt2_tdm_pos_info[mxqblock].total_slots;
/*
    if (((mxqblock==8) || (mxqblock==9)) && (xfi_mode[mxqblock-8]==1)) {
          available_tdm_slots += kt2_tdm_pos_info[mxqblock-2].total_slots;
    }
*/
    if (required_tdm_slots > available_tdm_slots) {
        LOG_CLI((BSL_META_U(unit,
                            "TDM feasibility failed required_tdm_slots:%d >"
                            "available_tdm_slots:%d \n"), required_tdm_slots,
                 available_tdm_slots));
        return SOC_E_CONFIG; 
    }
    if (new_lanes == 1) {
        /* 
        if (((mxqblock==8) || (mxqblock==9)) && (old_xfi_mode[mxqblock-8]==1)) {
             tdm_ports[0]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
             tdm_ports[1]=KT2_IDLE;
             tdm_ports[2]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
             tdm_ports[3]=KT2_IDLE;
        } else */ {
             tdm_ports[0]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
             tdm_ports[1]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
             tdm_ports[2]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
             tdm_ports[3]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
        }
    }
    if (new_lanes == 2) {
        /* 
        if (((mxqblock==8) || (mxqblock==9)) && (old_xfi_mode[mxqblock-8]==1)) {
            tdm_ports[0]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
            tdm_ports[1]=(*kt2_mxqblock_ports[unit])[mxqblock][2];
            tdm_ports[2]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
            tdm_ports[3]=(*kt2_mxqblock_ports[unit])[mxqblock][2];
        } else */ {
            tdm_ports[0]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
            tdm_ports[1]=(*kt2_mxqblock_ports[unit])[mxqblock][2];
            tdm_ports[2]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
            tdm_ports[3]=(*kt2_mxqblock_ports[unit])[mxqblock][2];
       }
    }
    if (new_lanes == 4) {
        /* 
        if (((mxqblock==8) || (mxqblock==9)) && (xfi_mode[mxqblock-8]==1)) {
             tdm_ports[0]=(*kt2_mxqblock_ports[unit])[mxqblock-2][0];
             tdm_ports[1]=(*kt2_mxqblock_ports[unit])[mxqblock-2][2];
             tdm_ports[2]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
             tdm_ports[3]=(*kt2_mxqblock_ports[unit])[mxqblock][2];
        } else */ {
             tdm_ports[0]=(*kt2_mxqblock_ports[unit])[mxqblock][0];
             tdm_ports[1]=(*kt2_mxqblock_ports[unit])[mxqblock][1];
             tdm_ports[2]=(*kt2_mxqblock_ports[unit])[mxqblock][2];
             tdm_ports[3]=(*kt2_mxqblock_ports[unit])[mxqblock][3];
        }
    }
    sal_memcpy(&new_tdm[0],&kt2_current_tdm[0],
               kt2_current_tdm_size *sizeof(kt2_current_tdm[0]));
    /*
    if (((mxqblock==8) || (mxqblock==9)) && (xfi_mode[mxqblock-8]==1)) {
        index=0;
       for (loop=0; loop < kt2_tdm_pos_info[mxqblock-2].total_slots; loop++) {
            pos = kt2_tdm_pos_info[mxqblock-2].pos[loop];
                  new_tdm[pos]=tdm_ports[index];
                  index = (index + 1) %2;
             }
        for (loop=0; loop < kt2_tdm_pos_info[mxqblock].total_slots; loop++) {
              pos = kt2_tdm_pos_info[mxqblock].pos[loop];
                  new_tdm[pos]=tdm_ports[index+2];
                  index = (index + 1) %2;
             }

    } else */ {
        for (index=0,loop=0; loop < kt2_tdm_pos_info[mxqblock].total_slots; loop++) {
             pos = kt2_tdm_pos_info[mxqblock].pos[loop];
                      new_tdm[pos]=tdm_ports[index];
                  index = (index + 1) % 4 ;
            }
        }
    /*
    if (((mxqblock==8) || (mxqblock==9)) && (old_xfi_mode[mxqblock-8]==1)) {
          for (loop=0;loop < kt2_tdm_pos_info[mxqblock-2].total_slots; loop++) {
               pos = kt2_tdm_pos_info[mxqblock-2].pos[loop];
                  new_tdm[pos]=KT2_IDLE;
             }
        }
    if (((mxqblock==8) || (mxqblock==9)) && (xfi_mode[mxqblock-8]==1)) {
         index=0;
         for (loop=0; loop < kt2_tdm_pos_info[mxqblock-2].total_slots; loop++) {
              pos = kt2_tdm_pos_info[mxqblock-2].pos[loop];
                  new_tdm[pos]= (*kt2_mxqblock_ports[unit])[mxqblock-2][index];
                  index = (index + 2) %4;
             }
        }
    */
    /*
    kt2_tdm_display(new_tdm,kt2_current_tdm_size,
                    bcm56450_tdm_info[cfg_num].row,
                    bcm56450_tdm_info[cfg_num].col);
     */

    /* Stop Counter Thread for time-being */
    /* SOC_IF_ERROR_RETURN(soc_counter_stop(unit)); */

    sal_memset(&egr_enable_entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, &egr_enable_entry, PRT_ENABLEf, 0);
    BCM_IF_ERROR_RETURN(bcm_esw_linkscan_mode_get(unit, port, &init_mode));
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             /* 1. START: Stop LinkScanning for ports on which flex operation
              * will be performed. This has to be done before performing MMU 
              * Queue flush on each flex port to avoid incorrect updation 
              * of EPC_LINK_BMAP during PHY-MAC sync that happens during link
              * scan mode set */
             bcm_esw_kt2_port_unlock(unit);
             BCM_IF_ERROR_RETURN(bcm_esw_linkscan_mode_set(
                                 unit, mxqblock_port, BCM_LINKSCAN_MODE_NONE));
             bcm_esw_kt2_port_lock(unit);
         }
    }

    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             /* 2. To stop the incoming traffic, disables XMAC in ingress(Rx) 
                   direction. This can be done by disabling RxEn bit in 
                   XMAC_Ctrl register in  MAC XMAC_Ctrl.<old_port>.RxEn= 0 */

             SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(
                                 unit, mxqblock_port, &xmac_ctrl));
             soc_reg64_field32_set(unit, XMAC_CTRLr, &xmac_ctrl, TX_ENf, 1);
             soc_reg64_field32_set(unit, XMAC_CTRLr, &xmac_ctrl, RX_ENf, 0);
             SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(
                                 unit, mxqblock_port, xmac_ctrl));
             sal_udelay((10*time_multiplier));
             /* After disabling XMAC receive path, reset the MAC logic by
              * setting SOFT_RESET=1 in XMAC_CTRL
              * Without the XMAC soft reset only setting XMAC RXEn =0
              * is not taking effect as port flush timeout is seen
              * for bidirectional traffic, when all ports in same block
              * are transmitting and receiving.
              * XMAC disable follows this sequence during MAC disable
              */

             soc_reg64_field32_set(unit, XMAC_CTRLr, &xmac_ctrl,
                                    SOFT_RESETf, 1);
             SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(
                                 unit, mxqblock_port, xmac_ctrl));
             sal_udelay((10*time_multiplier));


             /* 3. Brings the port down. This is done by programming 
                   EPC_LINK_BMAP[x] = 0; (x: port number)

                   This register resides in Sw2 block of ingress pipeline, by 
                   disabling this bit software will block all the new packets 
                   pertaining to this port from entering MMU except for packets
                   utilizing the extended queueing mechanism which may still 
                   be passed to the MMU.

                   Reset appropriate EPC_LINK_BMAP:PORT_BITMAP  */

             BCM_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(
                                 unit, MEM_BLOCK_ANY, 0, &epc_link_bmap_entry));
             soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm,
                          &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
             BCM_PBMP_PORT_REMOVE(link_pbmp,mxqblock_port);
             soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm,
                          &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
             SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(
                                 unit, MEM_BLOCK_ALL, 0, &epc_link_bmap_entry));

/* -------------------------------------------------------------------------- */
/* PortFlushing creating some issue with L0 node movement so moving to Q Flush*/
/* -------------------------------------------------------------------------- */
#ifdef PORT_FLUSH_DEBUG
             /* 4. Issues MMU port flush command 
                Software issues MMU port flush command (up to 4 ports can be 
                flushed in one command). MMU will drop any further enqueue 
                requests for queues within this port and will indicate port 
                flush command complete when all ports specified in the command 
                have drained all their packets from the MMU */

             BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &flush_reg));
             soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, 
                               FLUSH_ACTIVEf,1);
             /* PORT FLUSHING not QUEUE FLUSHING */
             soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, FLUSH_TYPEf,1);
             soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, FLUSH_NUMf,1);
             soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, FLUSH_EXTERNALf,
             (IS_EXT_MEM_PORT(unit, mxqblock_port) ? 1 : 0));
             soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, 
                               FLUSH_ID0f, mxqblock_port);
             BCM_IF_ERROR_RETURN(WRITE_TOQ_FLUSH0r(unit, flush_reg));
#endif


             /* 5. Saves the current configuration of PAUSE/PFC flow control 
                   Then disables the PFC flow control to ensure all pending 
                   packets can flow through MMU.
                   This can be done by programming the following register in
                   MMU for the relevant port:
                   XPORT_TO_MMU_BKP.PRI15_0_BKP = 16d0; */
             BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(
                                 unit, mxqblock_port, &port_speed));
             if (port_speed >= 10000) {
                 if ((mxqblock >= 6) && (mxqblock <= 9)) {
                      switch(mxqblock_port) {
                      case 25: pfc_index=6;break;
                      case 36: pfc_index=7;break;
                      case 26: pfc_index=8;break;
                      case 39: pfc_index=9;break;
                      case 27: pfc_index=10;break;
                      case 33: pfc_index=11;break;
                      case 28: pfc_index=12;break;
                      case 30: pfc_index=13;break;
                      default: return BCM_E_INTERNAL;
                      }
                  } else {
                      pfc_index=mxqblock;
                  }
                  BCM_IF_ERROR_RETURN(READ_XPORT_TO_MMU_BKPr(
                                      unit, pfc_index, &pfc_enable));
                  BCM_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(
                                      unit, pfc_index, 0));
             }

             /* 6. flushes any pending egress packets in MXQPORT, in this 
                   scenario MMU sends out the packets but they are dropped in 
                   the MXQPORT, hence the packets are drained out of MMU but 
                   are not sent out of the chip. This is done by programming 
                   the following: XP_TXFIFO_PKT_DROP_CTL.DROP_EN = 1b1; */

             READ_XP_TXFIFO_PKT_DROP_CTLr(unit, mxqblock_port,&rval);
             soc_reg_field_set(unit, XP_TXFIFO_PKT_DROP_CTLr, &rval, 
                               DROP_ENf,1);
             WRITE_XP_TXFIFO_PKT_DROP_CTLr(unit, mxqblock_port,rval);

#ifndef PORT_FLUSH_DEBUG
             /* Drain cells in mmu/port before cells entering TX FIFO */
             SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, mxqblock_port, TRUE));

#endif

/* -------------------------------------------------  */
/* No more PortFlushing so Confirmation not required  */
/* -------------------------------------------------  */
#ifdef PORT_FLUSH_DEBUG

             /* 7. Waits until all egress port packets are drained. 
                   This is done by making sure MMU is empty for cells destined 
                   for this port and MXQPORT FIFO is empty and no packets are 
                   being sent to MAC.  This is done by checking the following
                   i. MMU Port Flush command has completed by polling 
                        TOQ_FLUSH0.flush_active == 0
                   ii.XP_TXFIFO_CELL_CNT.CELL_CNT == 0

                   SW will have to poll for cell_cnt to be 0 for a few hundred 
                   clocks prior to concluding port TX flush completion */

             try_count=0;
             if (SAL_BOOT_SIMULATION) {
                 BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval));
                 soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_ACTIVEf, 0);
                 BCM_IF_ERROR_RETURN(WRITE_TOQ_FLUSH0r(unit, rval));
             } 
             do {
                 soc_timeout_init(&to, timeout_usec, min_polls);
                 if(soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_U(unit,
                                          "TimeOut InternalError\n")));
                    return BCM_E_INTERNAL;
                 }
                 try_count++;
                 BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval));
                 if (soc_reg_field_get(
                     unit, TOQ_FLUSH0r, rval, FLUSH_ACTIVEf) == 0) {
                     break;
                 }
             } while (try_count != 3);
             if (try_count == 3) {
                 return BCM_E_TIMEOUT;
             }
#endif
             try_count=0;
             if (SAL_BOOT_SIMULATION) {
                 BCM_IF_ERROR_RETURN(READ_XP_TXFIFO_CELL_CNTr(
                                     unit, mxqblock_port, &rval));
                 soc_reg_field_set(unit, XP_TXFIFO_CELL_CNTr,
                                   &rval, CELL_CNTf, 0);
                 BCM_IF_ERROR_RETURN(WRITE_XP_TXFIFO_CELL_CNTr(
                                     unit, mxqblock_port, rval));
             } 
             soc_timeout_init(&to, timeout_usec, min_polls);
             for (try=0; try<100 && try_count < 10 ; try++) {
                 if(soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_U(unit,
                                          "TimeOut InternalError\n")));
                    return SOC_E_INTERNAL; 
                 }
                 BCM_IF_ERROR_RETURN(READ_XP_TXFIFO_CELL_CNTr (
                                     unit,mxqblock_port, &rval));
                 cell_cnt=soc_reg_field_get(unit, XP_TXFIFO_CELL_CNTr,     
                                            rval, CELL_CNTf);
                 if (cell_cnt == 0) {
                     try_count++;
                     break;     
                 }
             } 
             if (try == 100) {
                return SOC_E_TIMEOUT;
             }
    
             /*8. Since we have blocked ingress traffic and outgoing packets 
                  have been dropped in MXQPORT, it is safe to powerdown 
                  Unicore/Warpcore serdes.  This is done by programming the 
                  following:XPORT_XGXS_CTRL.Pwrdwn = 1 */
             /* READ_XPORT_XGXS_CTRLr(unit, mxqblock_port,&rval); */
             rval = 0;
             soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, 
                               LCREF_ENf,1);
             soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, 
                               IDDQf,1);
             soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, 
                               PWRDWNf,1);
             WRITE_XPORT_XGXS_CTRLr(unit, mxqblock_port,rval);

             /*9 Disable the egress cell request generation; this will make 
                 sure that egress does not send any request to MMU. 
                 This is done by clearing EGR_ENABLE(for EDB) /port_enable 
                 (for TXLP) register in Egress */
             /* soc_mem_field32_set(unit, EGR_ENABLEm, 
                                    &egr_enable_entry, PRT_ENABLEf, 0); */
             if (mxqblock <= 7) {
                 BCM_IF_ERROR_RETURN(soc_mem_write(
                                     unit, EGR_ENABLEm, MEM_BLOCK_ALL,
                                     mxqblock_port, &egr_enable_entry));
             } else {
                 SOC_IF_ERROR_RETURN(READ_TXLP_PORT_ENABLEr(
                                     unit, port, &rval));
                 port_enable = soc_reg_field_get(unit, TXLP_PORT_ENABLEr,
                                     rval, PORT_ENABLEf);
                 port_enable &= ~(1 << (loop));
                 soc_reg_field_set(unit, TXLP_PORT_ENABLEr,
                                     &rval, PORT_ENABLEf, port_enable);
                 BCM_IF_ERROR_RETURN(WRITE_TXLP_PORT_ENABLEr(
                                     unit, port, rval));
             } 

             /*10. At this point, the link is down and all pending packets are 
                   drained.  The software then disables the MXQPORT flush.  
                   This is done by programming the followings: 
                   XP_TXFIFO_PKT_DROP_CTL.DROP_EN */
             READ_XP_TXFIFO_PKT_DROP_CTLr(unit, mxqblock_port,&rval);
             soc_reg_field_set(unit, XP_TXFIFO_PKT_DROP_CTLr, &rval, 
                               DROP_ENf,0);
             WRITE_XP_TXFIFO_PKT_DROP_CTLr(unit, mxqblock_port,rval);

             /* 11. Restore PFC */
             if (port_speed >= 10000) {
                 BCM_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(
                                 unit, pfc_index, pfc_enable));
             }
             /* PLEASE NOTE THAT PORT IS STILL DOWN */
            
         }
    }
    /* ReConfigure H/W */
    /* 1. Soft-reset relevant MXQPORT(s) */
    soc_katana2_mxqblock_reset(unit, mxqblock, 0);
    if ((mxqblock==8) || (mxqblock==9)) {
         if (xfi_mode[mxqblock-8]==1) {
             soc_katana2_mxqblock_reset(unit, mxqblock-2, 0);
         }
    }

    /* ReConfigure TDM */
    soc_katana2_reconfigure_tdm(unit,kt2_current_tdm_size,new_tdm); 
    sal_memcpy(&kt2_current_tdm[0],&new_tdm[0],
               kt2_current_tdm_size *sizeof(kt2_current_tdm[0]));

    /* Update local pbmp's */
    soc_linkscan_pause(unit);
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if ((old_ports[old_lanes-1][loop] == 0) &&
             (new_ports[new_lanes-1][loop] == 0)) {
             /* Eithere not used  */
             continue;
         }
         si->port_speed_max[mxqblock_port]=(*mxqspeeds[unit])[mxqblock]
                                                    [new_lanes-1];
         if (old_ports[old_lanes-1][loop] == new_ports[new_lanes-1][loop] ) {
             /* same port was used so only speed change */
             soc_katana2_pbmp_remove(unit,mxqblock_port);
             soc_katana2_pbmp_add(unit,mxqblock_port,mxqblock,
                                  si->port_speed_max[mxqblock_port]);
         }
         if (old_ports[old_lanes-1][loop]  > new_ports[new_lanes-1][loop]) {
             /* This port is not applicable so remove it from pbmp list */
             soc_katana2_pbmp_remove(unit,mxqblock_port);
             continue;
         }
         if (new_ports[new_lanes-1][loop]  > old_ports[old_lanes-1][loop]) {
             /* This port is new so add it in pbmp list first */
             soc_katana2_pbmp_add(unit,mxqblock_port, mxqblock,
                                  si->port_speed_max[mxqblock_port]);
         }
        BCM_IF_ERROR_RETURN(bcm_katana2_cosq_reconfigure_flexio(unit, mxqblock_port, 0));
    }
    if (((mxqblock==8) || (mxqblock==9)) && ((lanes==2)||(lanes==4))) {
        if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                    SOC_BLK_TXLP, &blk, NULL) == SOC_E_NONE) {
            /* Is a TXLP Port */
            si->block_valid[blk] += 1;
            if (si->block_port[blk] < 0) {
                si->block_port[blk] = port;
            }
            if (lanes==2) {
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], (*kt2_mxqblock_ports[unit])[mxqblock][2]);
            }   else if (lanes==4) {
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], (*kt2_mxqblock_ports[unit])[mxqblock][1]);
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], (*kt2_mxqblock_ports[unit])[mxqblock][2]);
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], (*kt2_mxqblock_ports[unit])[mxqblock][3]);
            }
        }
        if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                    SOC_BLK_RXLP, &blk, NULL) == SOC_E_NONE) {
            /* Is a RXLP Port */
            si->block_valid[blk] += 1;
            if (si->block_port[blk] < 0) {
                si->block_port[blk] = port;
            }
            if (lanes==2) {
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], (*kt2_mxqblock_ports[unit])[mxqblock][2]);
            }   else if (lanes==4) {
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], (*kt2_mxqblock_ports[unit])[mxqblock][1]);
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], (*kt2_mxqblock_ports[unit])[mxqblock][2]);
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], (*kt2_mxqblock_ports[unit])[mxqblock][3]);
            }
        }
    }
    soc_katana2_pbmp_all_resync(unit) ;
    soc_esw_dport_init(unit);
    soc_linkscan_continue(unit);
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(_soc_katana2_mmu_reconfigure(unit, mxqblock_port)); 
         }
    }

    mxqblock_max_nxtaddr = mxqblock_max_startaddr[mxqblock];
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             BCM_IF_ERROR_RETURN(READ_DEQ_EFIFO_CFGr(unit,mxqblock_port,&rval));
             if (si->port_speed_max[mxqblock_port] <= 1000) {
                 egr_fifo_depth = (IS_EXT_MEM_PORT(unit, mxqblock_port)) ? 10 : 2;
                 threshold = (IS_EXT_MEM_PORT(unit, mxqblock_port)) ? 8 : 0;
                 padding = 10 - egr_fifo_depth;
             }else if (si->port_speed_max[mxqblock_port] <= 2500) {
                 egr_fifo_depth = (IS_EXT_MEM_PORT(unit, mxqblock_port)) ? 14 : 2;
                 threshold = (IS_EXT_MEM_PORT(unit, mxqblock_port)) ? 12 : 0;
                 padding = 14 - egr_fifo_depth;
             } else if (si->port_speed_max[mxqblock_port] <= 13000) {
                 egr_fifo_depth = (IS_EXT_MEM_PORT(unit, mxqblock_port)) ? 56 : 10; 
                 threshold = (IS_EXT_MEM_PORT(unit, mxqblock_port)) ? 49 : 0;
                 padding = 56 - egr_fifo_depth;
             } else {
                 egr_fifo_depth = (IS_EXT_MEM_PORT(unit, mxqblock_port)) ? 112 : 16;
                 threshold = (IS_EXT_MEM_PORT(unit, mxqblock_port)) ? 85 : 0;
                 padding = 112 - egr_fifo_depth;
             }
             soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                               EGRESS_FIFO_START_ADDRESSf,
                               mxqblock_max_nxtaddr);
             if ((mxqblock_max_nxtaddr + (egr_fifo_depth + padding)) > mxqblock_max_endaddr[mxqblock]) {
                 egr_fifo_depth = (mxqblock_max_endaddr[mxqblock] -
                                   mxqblock_max_nxtaddr)+1;
                 mxqblock_max_nxtaddr = mxqblock_max_endaddr[mxqblock];
             } else {
                 mxqblock_max_nxtaddr += egr_fifo_depth + padding;
             }
             if (egr_fifo_depth == 1) {
                 LOG_CLI((BSL_META_U(unit,
                                     "WARN: EGR_FIFO_DEPTH IS ZERO for port=%d\n"),
                          mxqblock_port));
             }
             if ((IS_EXT_MEM_PORT(unit, mxqblock_port)) && !(PBMP_MEMBER(si->linkphy_pbm, mxqblock_port))) {
                 soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                                   EGRESS_FIFO_XMIT_OVERSPEED_RATE_LIMITERf, 1);
             } else {
                 soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                                   EGRESS_FIFO_XMIT_OVERSPEED_RATE_LIMITERf, 0);
             }
             soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                               EGRESS_FIFO_DEPTHf, egr_fifo_depth);
             soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                               EGRESS_FIFO_XMIT_THRESHOLDf, threshold);
             BCM_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFGr(
                                 unit,mxqblock_port,rval));
         }
    }

    /* WORK-AROUND for port-flushing i.e. need to repeat below step twice */
 for (try_loop = 0; try_loop < 2; try_loop++) { 
    /* EP Reset */
    if (SAL_BOOT_SIMULATION) {
        BCM_IF_ERROR_RETURN(WRITE_DYNAMIC_PORT_RECFG_VECTOR_CFG_31_00r(
                            unit, 0));
        BCM_IF_ERROR_RETURN(WRITE_DYNAMIC_PORT_RECFG_VECTOR_CFG_41_32r(
                            unit, 0));
    }
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             if (mxqblock_port <= 31) {
                 rval1 |= (1 << mxqblock_port);
             } else {
                 rval2 |= (1 << (mxqblock_port-32));
             }
         }
    }
    BCM_IF_ERROR_RETURN(WRITE_DYNAMIC_PORT_RECFG_VECTOR_CFG_31_00r(
                        unit, rval1));
    BCM_IF_ERROR_RETURN(WRITE_DYNAMIC_PORT_RECFG_VECTOR_CFG_41_32r(
                        unit, rval2));
    if (SAL_BOOT_SIMULATION) {
        BCM_IF_ERROR_RETURN(WRITE_DYNAMIC_PORT_RECFG_VECTOR_CFG_31_00r(
                            unit, 0));
        BCM_IF_ERROR_RETURN(WRITE_DYNAMIC_PORT_RECFG_VECTOR_CFG_41_32r(
                            unit, 0));
    }
    try_count = 0;
    do {
       soc_timeout_init(&to, timeout_usec, min_polls);
       if(soc_timeout_check(&to)) {
          LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "TimeOut InternalError\n")));
          return BCM_E_INTERNAL;
       }
       try_count++;
       BCM_IF_ERROR_RETURN(READ_DYNAMIC_PORT_RECFG_VECTOR_CFG_31_00r(
                           unit, &rval1));
       BCM_IF_ERROR_RETURN(READ_DYNAMIC_PORT_RECFG_VECTOR_CFG_41_32r(
                           unit, &rval2));
       if ((rval1 == 0) && (rval2 == 0)) {
            break;
       }
    } while (try_count != 3);
    if (try_count == 3) {
        return BCM_E_TIMEOUT;
    }
 }

    if (mxqblock <= 7) {
        READ_EDATABUF_DBG_PORT_INTF_RESETr(unit, &rval);
        soc_reg_field_set(unit, EDATABUF_DBG_PORT_INTF_RESETr,
                          &rval,  port_intf_reset_fld[mxqblock], 1);
        WRITE_EDATABUF_DBG_PORT_INTF_RESETr(unit, rval);
        READ_EDATABUF_DBG_MMU_INTF_RESETr(unit, &rval);
        soc_reg_field_set(unit, EDATABUF_DBG_MMU_INTF_RESETr,
                          &rval,  mmu_intf_reset_fld[mxqblock], 1);
        soc_reg_field_set(unit, EDATABUF_DBG_MMU_INTF_RESETr,
                          &rval,  new_port_mode_fld[mxqblock], 
                          new_port_mode[new_lanes]);
        WRITE_EDATABUF_DBG_MMU_INTF_RESETr(unit, rval);
    }
    /* mxqblock_port=port; */
    if ((mxqblock >= 8) && (mxqblock <= 9)) {
        WRITE_TXLP_PORT_CREDIT_RESETr(unit, port,0xf);
        for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
             sal_memset(&txlp_port_addr_map_table_entry,0,
                        sizeof(txlp_port_addr_map_table_entry_t));
             mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
             if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
                 /*  5.  As each cell-occupies "4" lines in buffer, [end-start+1] must be a integral
                         multiple of "4". */
                 if (si->port_speed_max[mxqblock_port] <= 2500) {
                     end_addr = start_addr + (( 6 * 4) - 1); /* 6 Cells */
                 } else if (si->port_speed_max[mxqblock_port] <= 10000) {
                     end_addr = start_addr + ((12 * 4) - 1); /* 12 Cells */
                 } else if (si->port_speed_max[mxqblock_port] <= 13000) {
                     end_addr = start_addr + ((16 * 4) - 1); /* 16 Cells */
                 } else if (si->port_speed_max[mxqblock_port] <= 21000) {
                    end_addr = start_addr + ((20 * 4) - 1); /* 20 Cells */
                 }
                 soc_TXLP_PORT_ADDR_MAP_TABLEm_field_set(unit,
                     &txlp_port_addr_map_table_entry,START_ADDRf,&start_addr);
                 soc_TXLP_PORT_ADDR_MAP_TABLEm_field_set(unit,
                     &txlp_port_addr_map_table_entry,END_ADDRf,&end_addr);
                 start_addr = end_addr+1;
             }
             BCM_IF_ERROR_RETURN(WRITE_TXLP_PORT_ADDR_MAP_TABLEm(
                                 unit,SOC_INFO(unit).txlp_block[mxqblock-8],
                                 loop, &txlp_port_addr_map_table_entry));
        }
    }


    /* Begin: Link up sequence */
    sal_udelay((10*time_multiplier));
    /* 1 */
    if (mxqblock <= 7) {
        READ_EDATABUF_DBG_PORT_INTF_RESETr(unit, &rval);
        soc_reg_field_set(unit, EDATABUF_DBG_PORT_INTF_RESETr,
                          &rval,  port_intf_reset_fld[mxqblock], 0);
        WRITE_EDATABUF_DBG_PORT_INTF_RESETr(unit, rval);
        READ_EDATABUF_DBG_MMU_INTF_RESETr(unit, &rval);
        soc_reg_field_set(unit, EDATABUF_DBG_MMU_INTF_RESETr,
                          &rval,  mmu_intf_reset_fld[mxqblock], 0);
        WRITE_EDATABUF_DBG_MMU_INTF_RESETr(unit, rval);
    }
    /* mxqblock_port=port; */
    if ((mxqblock >= 8) && (mxqblock <= 9)) {
        WRITE_TXLP_PORT_CREDIT_RESETr(unit, port,0);
    }
    /* 2 */
    BCM_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(
                        unit, MEM_BLOCK_ANY, 0, &epc_link_bmap_entry));
    soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm,
                        &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             BCM_PBMP_PORT_ADD(link_pbmp,mxqblock_port);
         }
    } 
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm,
                        &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(
                        unit, MEM_BLOCK_ALL, 0, &epc_link_bmap_entry));

    /* Bring out of reset */
    soc_katana2_mxqblock_reset(unit, mxqblock, 1);

    /* XFI Mode ??? */
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             BCM_IF_ERROR_RETURN(_bcm_kt2_update_port_mode(
                                 unit,mxqblock_port,
                                 si->port_speed_max[mxqblock_port]));
             break;
         }
    }
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, mxqblock_port,&rval));
             if (((si->port_speed_max[mxqblock_port]) >= 13000) && 
                  ((si->port_speed_max[mxqblock_port]) != 20000)) {
                  soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf,1);
                  if (soc_feature(unit, soc_feature_higig2) && IS_HL_PORT(unit,port)) { 
                      soc_reg_field_set(unit, XPORT_CONFIGr, &rval, 
                                        HIGIG2_MODEf,1);
                  } 
             } else {
                  soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf,0);
                  soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG2_MODEf,0);
             }
             SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, mxqblock_port,rval));
         } 
    }
    /* mxqblock_port=port; */
    SOC_IF_ERROR_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, port, 0));

    length_check = soc_property_get(unit, spn_MAC_LENGTH_CHECK_ENABLE, 0);

    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(READ_MAC_RSV_MASKr(unit,mxqblock_port, &rval));
             if (length_check) {
                 rval |= 0x20;  /* set bit 5 to enable frame length check */
             } else {
                 rval &= ~0x20; /* clear bit 5 to disable frame length check */
             }
             SOC_IF_ERROR_RETURN(WRITE_MAC_RSV_MASKr(unit,mxqblock_port, rval));
         }
    }

    /* mxqblock_port=port; */
    SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit,port, 0xf));
    SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit,port, 0x0));

    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(READ_XMAC_MODEr(unit,mxqblock_port, &rval64));
             if (si->port_speed_max[mxqblock_port] ==  1000) {
                 speed_value = 2;
             } else if (si->port_speed_max[mxqblock_port] == 2500) {
                 speed_value = 3;
             } else {
                 speed_value = 4;
             }
             soc_reg64_field32_set(unit, XMAC_MODEr, &rval64,
                                   SPEED_MODEf,speed_value);
             if (si->port_speed_max[mxqblock_port] <=  10000) {
                 soc_reg64_field32_set(unit, XMAC_MODEr, &rval64,HDR_MODEf,0);
             } else {
                 soc_reg64_field32_set(unit, XMAC_MODEr, &rval64,HDR_MODEf,2);
             }
             SOC_IF_ERROR_RETURN(WRITE_XMAC_MODEr(unit,mxqblock_port, rval64));
         }
    }

    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(READ_XMAC_RX_CTRLr(
                                 unit,mxqblock_port, &rval64));
             if (si->port_speed_max[mxqblock_port] <=  10000) {
                 soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                                       RUNT_THRESHOLDf, 64);
             } else {
                 soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                                       RUNT_THRESHOLDf, 76);
             }
             soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64, STRIP_CRCf, 0);
             SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_CTRLr(
                                 unit,mxqblock_port, rval64));
         }
    }
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(
                                 unit,mxqblock_port, &rval64));
             if (si->port_speed_max[mxqblock_port] <  2500) {
                 soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &rval64,
                                       TX_64BYTE_BUFFER_ENf,1);
             } else {
                 soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &rval64,
                                       TX_64BYTE_BUFFER_ENf,0);
             }
             soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &rval64, CRC_MODEf,3);
             SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(
                                 unit,mxqblock_port, rval64));
         }
    }
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(READ_XMAC_RX_MAX_SIZEr(
                                 unit,mxqblock_port, &rval64));
             soc_reg64_field32_set(unit, XMAC_RX_MAX_SIZEr, &rval64,
                          RX_MAX_SIZEf,(12*1024));
             SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_MAX_SIZEr(
                                 unit,mxqblock_port, rval64));
         }
    }
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(
                                 unit,mxqblock_port, &rval64));
             soc_reg64_field32_set(unit, XMAC_CTRLr, &rval64, SOFT_RESETf,0);
             soc_reg64_field32_set(unit, XMAC_CTRLr, &rval64, TX_ENf,1);
             soc_reg64_field32_set(unit, XMAC_CTRLr, &rval64, RX_ENf,1);
             SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(
                                 unit,mxqblock_port, rval64));
         }
    }

    /* mxqblock_port=port; */
    SOC_IF_ERROR_RETURN(WRITE_XPORT_ECC_CONTROLr(unit,port, 0xf));

    SOC_IF_ERROR_RETURN(READ_XPORT_PORT_ENABLEr(unit,port, &rval));
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &rval, 
             port_enable_field[(*kt2_port_to_mxqblock_subports[unit])
                               [mxqblock_port-1]],1);
         }
    }
    /* mxqblock_port=port; */
    SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(unit,port, rval));

    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(READ_THDO_PORT_DISABLE_CFG1r(unit,&rval));
             soc_reg_field_set(unit, THDO_PORT_DISABLE_CFG1r, &rval, 
                               PORT_WRf,1);
             soc_reg_field_set(unit, THDO_PORT_DISABLE_CFG1r, &rval, 
                               PORT_WR_TYPEf,0);
             soc_reg_field_set(unit, THDO_PORT_DISABLE_CFG1r, &rval, 
                               PORT_IDf,mxqblock_port);
             SOC_IF_ERROR_RETURN(WRITE_THDO_PORT_DISABLE_CFG1r(unit,rval));
             sal_udelay(10*time_multiplier);
             if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
                  soc_reg_field_set(unit, THDO_PORT_DISABLE_CFG1r, &rval, 
                               PORT_WRf,0);
                  SOC_IF_ERROR_RETURN(WRITE_THDO_PORT_DISABLE_CFG1r(
                                      unit,rval));
             }
             SOC_IF_ERROR_RETURN(READ_THDO_PORT_DISABLE_CFG1r(unit,&rval));
             if (soc_reg_field_get(unit, THDO_PORT_DISABLE_CFG1r, rval,
                                    PORT_WRf) != 0) {
                 return BCM_E_TIMEOUT;
             }
         }
    }
    /* mxqblock_port=port; */
    SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit,port,&rval));
    soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,PWRDWNf,0);
    SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit,port,rval));

    SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit,port,&rval));
    soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,IDDQf,0);
    SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit,port,rval));

    SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit,port,&rval));
    soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,REFSELf,0);
    SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit,port,rval));

    SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit,port,&rval));
    soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,REFDIVf,0);
    SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit,port,rval));

    SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit,port,&rval));
    soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,RSTB_HWf,1);
    SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit,port,rval));

    SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit,port,&rval));
    soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,RSTB_MDIOREGSf,1);
    SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit,port,rval));

    SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit,port,&rval));
    soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,RSTB_PLLf,1);
    SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit,port,rval));

    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(
                                 unit,mxqblock_port,&rval));
             if (si->port_speed_max[mxqblock_port] > 2500) {
                 soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,
                                   TXD10G_FIFO_RSTBf,1);
             } else {
                 soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,
                                   TXD10G_FIFO_RSTBf,0);
             }
             soc_reg_field_set(unit,XPORT_XGXS_CTRLr,&rval,
                               TXD1G_FIFO_RSTBf,0xF);
             SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(
                                 unit,mxqblock_port,rval));
         }
    }

    /* Txd10g_FIFO_RstB_DXGXS1  ?? */

    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
             if (mxqblock <= 7 ) {
                 sal_memset(&egr_enable_entry, 0, sizeof(egr_enable_entry_t));
                 soc_mem_field32_set(unit, EGR_ENABLEm,
                                     &egr_enable_entry,PRT_ENABLEf, 1);
                 SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, 
                                     mxqblock_port, &egr_enable_entry));
             } else {
                 SOC_IF_ERROR_RETURN(READ_TXLP_PORT_ENABLEr(
                                     unit, port, &rval));
                 port_enable = soc_reg_field_get(unit, TXLP_PORT_ENABLEr,
                                     rval, PORT_ENABLEf);
                 port_enable |= (1<< ((*kt2_port_to_mxqblock_subports[unit])
                                      [mxqblock_port-1]));
                 soc_reg_field_set(unit, TXLP_PORT_ENABLEr,
                                     &rval, PORT_ENABLEf, port_enable);
                 SOC_IF_ERROR_RETURN(WRITE_TXLP_PORT_ENABLEr(
                                     unit, port, rval));
             }
         }
    }

    /* End: Link up sequence */

    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if ((old_ports[old_lanes-1][loop] == 0) &&
             (new_ports[new_lanes-1][loop] == 0)) {
             /* Eithere not used  */
             continue;
         }
         if (old_ports[old_lanes-1][loop] == new_ports[new_lanes-1][loop] ) {
             /* same port was used so only speed change */
             SOC_IF_ERROR_RETURN(soc_katana2_port_detach(unit,mxqblock_port));
         }
         if (old_ports[old_lanes-1][loop]  > new_ports[new_lanes-1][loop]) {
             /* This port is not applicable so remove it from pbmp list */
             SOC_IF_ERROR_RETURN(soc_katana2_port_detach(unit,mxqblock_port));
             continue;
         }
         if (new_ports[new_lanes-1][loop]  > old_ports[old_lanes-1][loop]) {
             /* This port is new continue */
             continue;
         }
    }
    for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
         mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
         if ((old_ports[old_lanes-1][loop] == 0) &&
             (new_ports[new_lanes-1][loop] == 0)) {
             /* Eithere not used  */
             continue;
         }
         if (old_ports[old_lanes-1][loop] == new_ports[new_lanes-1][loop] ) {
             /* same port was used so only speed change */
             SOC_IF_ERROR_RETURN(soc_katana2_port_attach(
                                 unit,mxqblock ,mxqblock_port));
         }
         if (old_ports[old_lanes-1][loop]  > new_ports[new_lanes-1][loop]) {
             /* This port is not applicable so remove it from pbmp list */
             continue;
         }
         if (new_ports[new_lanes-1][loop]  > old_ports[old_lanes-1][loop]) {
             /* This port is new so add it in pbmp list first */
             SOC_IF_ERROR_RETURN(soc_katana2_port_attach(
                                 unit,mxqblock ,mxqblock_port));
         }
    }
   
    /* Special Treatment */
/*
    if ((mxqblock==8) || (mxqblock==9)) {
        if (xfi_mode[mxqblock-8]==1) {
            LOG_CLI((BSL_META_U(unit,
                                "4xXFI Mode used  for mxqblock:%d\n"),mxqblock));
        }
        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(
                            unit,&top_misc_control_1_val));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, 
                          &top_misc_control_1_val,
                          wc_xfi_mode_sel_fld[mxqblock-8],xfi_mode[mxqblock-8]);
        SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_1r(
                                unit,top_misc_control_1_val));
    }
*/

    /* 1. Bring-out of Soft-reset XFI mode related MXQPORT(s) */
/*
    if ((mxqblock==8) || (mxqblock==9)) {
         if (xfi_mode[mxqblock-8]==1) {
             soc_timeout_init(&to, timeout_usec, min_polls);
             if(soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "_bcm_kt2_port_lanes_set:TimeOut InternalError\n")));
                return SOC_E_INTERNAL; 
             }
             soc_katana2_mxqblock_reset(unit, mxqblock-2, 1);
         }
    }
*/
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return SOC_E_NONE;

}
int _bcm_kt2_cosq_port_ext_buf_set(int unit, bcm_port_t port, int enable)
{
    int                    mxqblock_port_enable[KT2_MAX_MXQPORTS_PER_BLOCK];
    epc_link_bmap_entry_t  epc_link_bmap_entry;
    soc_pbmp_t             link_pbmp, link_pbmp_bkp;
    uint8                  mxqblock;
    uint8                  mxqblock_port;
    uint8                  loop;
    pbmp_t                 ext_pbmp;

#if defined(BCM_SABER2_SUPPORT)
    int                    okay = 0;
    uint64                 xlmac_ctrl;
#endif

    ext_pbmp = soc_property_get_pbmp(unit, spn_PBMP_EXT_MEM, 0);
    if (soc_feature(unit, soc_feature_mmu_packing)) {
        if (!SOC_PBMP_MEMBER(ext_pbmp, port)){
            return  BCM_E_PORT;
        }
    }

 
    /* Validation on Availability of External Mem */
    if (SOC_DDR3_NUM_MEMORIES(unit) == 0) {
        if (enable) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "_bcm_kt2_cosq_port_ext_buf_set:External Memory Not Available\n")));
            return BCM_E_RESOURCE;
        }
        return BCM_E_NONE;
    }

    /* Disable the port to flush the queues */
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit) && (IS_XL_PORT(unit, port))) {

        /* Stop the incoming traffic, disable XLMAC Rx */
        SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(
                             unit, port, &xlmac_ctrl));
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &xlmac_ctrl, TX_ENf, 1);
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &xlmac_ctrl, RX_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(
                             unit, port, xlmac_ctrl));
        sal_udelay(10);

        BCM_IF_ERROR_RETURN(bcm_esw_port_enable_get(unit, port, &okay));
        BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit, port, 0));

        /* Remove the port from EPC_LINK_BMAP to avoid packets getting queued
         * while switching between buffers */
        BCM_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(
                    unit, MEM_BLOCK_ANY, 0, &epc_link_bmap_entry));
        soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm,
                &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
        BCM_PBMP_PORT_REMOVE(link_pbmp,port);
        soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm,
                &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(
                    unit, MEM_BLOCK_ALL, 0, &epc_link_bmap_entry));
    } else
#endif
    {
#if defined(BCM_SABER2_SUPPORT)
        if(SOC_IS_SABER2(unit)) {
            SOC_IF_ERROR_RETURN(soc_saber2_get_port_block(
                                unit,port,&mxqblock));
        } else
#endif
        {
            SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(
                                unit,port,&mxqblock));
        }

        SOC_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(
                     unit, MEM_BLOCK_ANY, 0, &epc_link_bmap_entry));
        soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm,
                 &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
        SOC_PBMP_ASSIGN(link_pbmp_bkp, link_pbmp);

        for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
#if defined(BCM_SABER2_SUPPORT)
             if(SOC_IS_SABER2(unit)) {
                 mxqblock_port=(*sb2_block_ports[unit])[mxqblock][loop];
             } else
#endif
             {
                 mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
             }
             if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
                 BCM_IF_ERROR_RETURN(bcm_esw_port_enable_get(unit, mxqblock_port,
                                                  &mxqblock_port_enable[loop]));
                 BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit, mxqblock_port,
                                                   0));
                 /* Remove the port from EPC_LINK_BMAP to avoid packets getting
                  * queued while switching between buffers */
                 BCM_PBMP_PORT_REMOVE(link_pbmp, mxqblock_port);

             }
        }
        soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm,
                 &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(
                     unit, MEM_BLOCK_ALL, 0, &epc_link_bmap_entry));
    }
    sal_usleep(200000);
    /* Perform Buffer switching */
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        SOC_IF_ERROR_RETURN(_bcm_saber2_mmu_init_helper_dyn(unit, port, enable));
    } else
#endif
    {
        SOC_IF_ERROR_RETURN(_soc_katana2_mmu_init_helper_dyn(unit, port, enable));
    }
    sal_usleep(200000);
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit) && (IS_XL_PORT(unit, port))) {
        /* Restore EPC_LINK_BMAP with the port */
        BCM_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(
                    unit, MEM_BLOCK_ANY, 0, &epc_link_bmap_entry));
        soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm,
                &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
        BCM_PBMP_PORT_ADD(link_pbmp,port);
        soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm,
                &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(
                    unit, MEM_BLOCK_ALL, 0, &epc_link_bmap_entry));
        sal_usleep(200000);

        BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit, port, okay));
    } else
#endif
    {
        /* Restore EPC_LINK_BMAP with the port */
        SOC_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(
                     unit, MEM_BLOCK_ANY, 0, &epc_link_bmap_entry));
        soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm,
                 &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp_bkp);
        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(
                     unit, MEM_BLOCK_ALL, 0, &epc_link_bmap_entry));
        sal_usleep(200000);

        for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
#if defined(BCM_SABER2_SUPPORT)
             if(SOC_IS_SABER2(unit)) {
                 mxqblock_port=(*sb2_block_ports[unit])[mxqblock][loop];
             } else
#endif
             {
                 mxqblock_port=(*kt2_mxqblock_ports[unit])[mxqblock][loop];
             }
             if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
                 BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit, mxqblock_port,
                                     mxqblock_port_enable[loop]));
             }
        }
    }
    return BCM_E_NONE;
}

int _bcm_kt2_cosq_port_ext_buf_get(int unit, bcm_port_t port, int *enable)
{
    *enable = 0;
    if (SOC_PBMP_MEMBER(PBMP_EXT_MEM (unit), port)) {
        *enable = 1;
    }
    return BCM_E_NONE;

}
int _bcm_kt2_port_lanes_set(int unit, bcm_port_t port, int lanes)
{
   int rv;
   soc_flex_io_operation_status_set(unit, 1);
   rv = __bcm_kt2_port_lanes_set(unit, port, lanes);
   soc_flex_io_operation_status_set(unit, 0);
   return rv;
}
int
_bcm_kt2_port_lanes_get(int unit, bcm_port_t port, int *lanes)
{
    uint8 loop=0;
    uint8 mxqblock=0;

    *lanes=0;
    SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(
                        unit,port,&mxqblock));
    for(loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK; loop++) {
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)),
                             (*kt2_mxqblock_ports[unit])[mxqblock][loop])) {
           (*lanes)++;
        }
    }
    return SOC_E_NONE;
}

int 
_bcm_kt2_port_control_oam_loopkup_with_dvp_set(int unit, bcm_port_t port, 
                                               int val)
{ 
#if defined(INCLUDE_L3)
    int vp = 0;
    egr_dvp_attribute_entry_t dvp_entry;
    /* From the gport get VP index */
    if (BCM_GPORT_IS_MIM_PORT(port)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(port);
    } else if (BCM_GPORT_IS_MPLS_PORT(port)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
    } else if (BCM_GPORT_IS_VLAN_PORT(port)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
    } else {
        return BCM_E_PORT;
    }

    /* Set OAM_KEY3 in EGR_DVP_ATTRIBUTE table entry */
    SOC_IF_ERROR_RETURN
        (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
            OAM_KEY3f, val ? 1 : 0);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry));
    return BCM_E_NONE;
#endif
    return BCM_E_NONE;
}

int 
_bcm_kt2_port_control_oam_loopkup_with_dvp_get(int unit, bcm_port_t port, 
                                               int *val)
{ 
#if defined(INCLUDE_L3)
    int vp = 0;
    egr_dvp_attribute_entry_t dvp_entry;
    /* From the gport get VP index */
    if (BCM_GPORT_IS_MIM_PORT(port)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(port);
    } else if (BCM_GPORT_IS_MPLS_PORT(port)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
    } else if (BCM_GPORT_IS_VLAN_PORT(port)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
    } else {
        return BCM_E_PORT;
    }
    SOC_IF_ERROR_RETURN
        (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    *val = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
            OAM_KEY3f);
#endif
    return BCM_E_NONE;
}

soc_field_t 
    modid_valid_field[KT2_MAX_MODIDS_PER_TRANSLATION_TABLE] =
    { MODID_0_VALIDf,
        MODID_1_VALIDf,
        MODID_2_VALIDf,
        MODID_3_VALIDf};
soc_field_t 
    modid_field[KT2_MAX_MODIDS_PER_TRANSLATION_TABLE] =
    { MODID_0f,
        MODID_1f,
        MODID_2f,
        MODID_3f };
soc_field_t 
    modid_base_port_ptr_field[KT2_MAX_MODIDS_PER_TRANSLATION_TABLE] =
    {   MODID_0_BASE_PP_PORTf ,
        MODID_1_BASE_PP_PORTf ,
        MODID_2_BASE_PP_PORTf ,
        MODID_3_BASE_PP_PORTf } ;
soc_mem_t 
    pp_port_gpp_translation_table[KT2_MAX_PP_PORT_GPP_TRANSLATION_TABLES + 
    KT2_MAX_EGR_PP_PORT_GPP_TRANSLATION_TABLES] = 
    {   PP_PORT_GPP_TRANSLATION_1m,
        PP_PORT_GPP_TRANSLATION_2m,
        PP_PORT_GPP_TRANSLATION_3m,
        PP_PORT_GPP_TRANSLATION_4m,
        EGR_PP_PORT_GPP_TRANSLATION_1m,
        EGR_PP_PORT_GPP_TRANSLATION_2m};

/*
 * Function:
 *      bcm_kt2_modid_set_all
 * Purpose:
 *      Update all the moduleids, valid bits and corresponding port base
 *      pointers in the appropriates tables as per Katana2
 * Parameters:
 *      unit               - (IN)StrataSwitch unit number.
 *      my_mod_list        - (IN) list of module ID's
 *      my_mod_valid_list  - (IN) list of Valid bits corresponding to Module ID
 *      base_ptr_list      - (IN) list of port base ptr corresponding to
 *                                 ModuleID
 * Returns:
 *      BCM_E_XXX
 */

int 
bcm_kt2_modid_set_all(int unit,
        int *my_modid_list, int *my_modid_valid_list ,
        int *base_port_ptr_list) 
{
    int loop = 0;
    int index = 0;

    for (index = 0 ; index < ( KT2_MAX_PP_PORT_GPP_TRANSLATION_TABLES
                + KT2_MAX_EGR_PP_PORT_GPP_TRANSLATION_TABLES ) ; index++) { 
        for (loop = 0 ; loop < KT2_MAX_MODIDS_PER_TRANSLATION_TABLE; loop++) { 
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        pp_port_gpp_translation_table[index], 
                        0, modid_valid_field[loop],
                        my_modid_valid_list[loop]));
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        pp_port_gpp_translation_table[index],
                        0, modid_field[loop],
                        my_modid_list[loop]));
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        pp_port_gpp_translation_table[index], 0, 
                        modid_base_port_ptr_field[loop],
                        base_port_ptr_list[loop]));

        }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_kt2_modid_set
 * Purpose:
 *      Update a given entry with the moduleid, valid bits corresponding port
 *      base pointers in the appropriates tables as per Katana2
 * Parameters:
 *      unit               - (IN)StrataSwitch unit number.
 *      mod_index          - (IN) Index of the module we are setting up
 *      modid_             - (IN) Module ID
 *      modid_valid        - (IN) Valid bit
 *      base_port_ptr      - (IN) Port base pointer
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_kt2_modid_set(int unit, int mod_index, int modid,
                  int modid_valid, int base_port_ptr)
{
    int num_tables = 0;

    /* Sanity check the 'mod_index' value, first index reserved
       for front panel ports */
    if((mod_index < _BCMI_KATANA2_MODULE_2) ||
       (mod_index > (KT2_MAX_MODIDS_PER_TRANSLATION_TABLE-1))) {
        return BCM_E_PARAM;
    }

    /* Loop through relevant tables and set up the entry */
    for (num_tables = 0; num_tables < ( KT2_MAX_PP_PORT_GPP_TRANSLATION_TABLES
                + KT2_MAX_EGR_PP_PORT_GPP_TRANSLATION_TABLES); num_tables++) {
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        pp_port_gpp_translation_table[num_tables],
                        0, modid_valid_field[mod_index], modid_valid));
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        pp_port_gpp_translation_table[num_tables],
                        0, modid_field[mod_index], modid));
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        pp_port_gpp_translation_table[num_tables], 0,
                        modid_base_port_ptr_field[mod_index],
                        base_port_ptr));

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_modid_get_all
 * Purpose:
 *      Get the moduleids, valid bits and corresponding port base pointers
 * Parameters:
 *      unit               - (IN) unit number.
 *      my_mod_list        - (OUT) list of module ID's
 *      my_mod_valid_list  - (OUT) list of Valid bits corresponding to Module ID
 *      base_ptr_list      - (OUT) list of port base ptr corresponding to
 *                                 ModuleID
 * Returns:
 *      BCM_E_XXX
 */

int 
bcm_kt2_modid_get_all(int unit,
        int *my_modid_list, int *my_modid_valid_list ,
        int *base_port_ptr_list) 
{
    int loop = 0;
    soc_mem_t mem = pp_port_gpp_translation_table[0];
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* hw entry  buffer.            */

    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, hw_buf));

    for (loop = 0 ; loop < KT2_MAX_MODIDS_PER_TRANSLATION_TABLE; loop++) { 
        my_modid_valid_list[loop] = soc_mem_field32_get(unit, mem, hw_buf,
                                    modid_valid_field[loop]);
        my_modid_list[loop] = soc_mem_field32_get(unit, mem, hw_buf,
                                    modid_field[loop]);
        base_port_ptr_list[loop] = soc_mem_field32_get(unit, mem, hw_buf,
                                    modid_base_port_ptr_field[loop]);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_modid_get
 * Purpose:
 *      Get the module-id, valid bit and corresponding port base pointer for
 *      a given module index
 * Parameters:
 *      unit            - (IN) unit number.
 *      mod_index       - (IN) Module index to retrieve
 *      modid           - (OUT) Module ID correspoding to mod_index
 *      mod_valid       - (OUT) Valid bits corresponding to mod_index
 *      base_port_ptr   - (OUT) Port base ptr corresponding to mod_index
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_kt2_modid_get(int unit, int mod_index, int *modid, int *modid_valid,
        int *base_port_ptr)
{
    soc_mem_t mem = pp_port_gpp_translation_table[0];
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* hw entry  buffer. */

    /* Sanity check the 'mod_index' value, first index (_BCMI_KATANA2_MODULE_1)
       reserved for front panel ports */
    if((mod_index < _BCMI_KATANA2_MODULE_2) ||
       (mod_index > (KT2_MAX_MODIDS_PER_TRANSLATION_TABLE-1))) {
        return BCM_E_PARAM;
    }

    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, hw_buf));

    *modid = soc_mem_field32_get(unit, mem, hw_buf,
                                modid_valid_field[mod_index]);
    *modid_valid = soc_mem_field32_get(unit, mem, hw_buf,
                                modid_field[mod_index]);
    *base_port_ptr = soc_mem_field32_get(unit, mem, hw_buf,
                                modid_base_port_ptr_field[mod_index]);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_port_gport_validate
 * Description:
 *      Helper funtion to validate port/gport parameter 
 * Parameters:
 *      unit  - (IN) BCM device number
 *      port_in  - (IN) Port / Gport to validate
 *      port_out - (OUT) Port number if valid. 
 * Return Value:
 *      BCM_E_NONE - Port OK 
 *      BCM_E_PORT - Port Invalid
 */
int
_bcm_kt2_port_gport_validate(int unit, bcm_port_t port_in,
                                       bcm_port_t *port_out)
{
    if (BCM_GPORT_IS_SET(port_in)) {
        if (BCM_GPORT_IS_SUBPORT_PORT(port_in)) {
            if (_BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_in) || 
                _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, port_in) ||
                _BCM_KT2_GPORT_IS_GENERAL_SUBPORT_PORT(unit, port_in)) {
                *port_out = _BCM_KT2_SUBPORT_PORT_ID_GET(port_in);
            } else {
                return BCM_E_PORT;
            }
        } else {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, port_in, port_out));
        }
    } else if (SOC_PORT_VALID(unit, port_in)) { 
        *port_out = port_in;
    } else {
        return BCM_E_PORT; 
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_kt2_pp_port_to_modport_get
 * Description:
 *      given a PP_PORT return the modid and port
 * Parameters:
 *      unit  - (IN) BCM device number
 *      pp_port  - (IN) pp_port / Gport
 *      modid    - (OUT) Module ID
 *      port     - (OUT) Port number if valid. 
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_kt2_pp_port_to_modport_get(int unit, bcm_port_t pp_port, int *modid,
                              bcm_port_t *port)
{
    int my_modid_list[KT2_MAX_MODIDS_PER_TRANSLATION_TABLE] = {0};
    int my_modid_valid[KT2_MAX_MODIDS_PER_TRANSLATION_TABLE] = {0};
    int my_modport_base_ptr[KT2_MAX_MODIDS_PER_TRANSLATION_TABLE] = {0};

    *modid = -1;
    *port = -1;

    BCM_IF_ERROR_RETURN(_bcm_kt2_port_gport_validate(unit, pp_port, &pp_port));

    BCM_IF_ERROR_RETURN(
        bcm_kt2_modid_get_all(unit, my_modid_list, my_modid_valid,
                             my_modport_base_ptr));

    if (my_modid_valid[3] && (pp_port >= my_modport_base_ptr[3])) {
        *modid = my_modid_list[3];
        *port = pp_port - my_modport_base_ptr[3];
    } else if (my_modid_valid[2] && (pp_port >= my_modport_base_ptr[2])) {
        *modid = my_modid_list[2];
        *port = pp_port - my_modport_base_ptr[2];
    } else if (my_modid_valid[1] && (pp_port >= my_modport_base_ptr[1])) {
        *modid = my_modid_list[1];
        *port = pp_port - my_modport_base_ptr[1];
    } else {
        *modid = my_modid_list[0];
        *port = pp_port - my_modport_base_ptr[0];
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_kt2_modport_to_pp_port_get
 * Description:
 *      given a modid and port return the pp_port number
 * Parameters:
 *      unit  - (IN) BCM device number
 *      modid    - (IN) Module ID
 *      port     - (IN) Port number if valid. 
 *      pp_port  - (OUT) pp_port / Gport
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_kt2_modport_to_pp_port_get(int unit, int modid, bcm_port_t port,
                            bcm_port_t *pp_port)
{
    int my_modid_list[KT2_MAX_MODIDS_PER_TRANSLATION_TABLE] = {0};
    int my_modid_valid[KT2_MAX_MODIDS_PER_TRANSLATION_TABLE] = {0};
    int my_modport_base_ptr[KT2_MAX_MODIDS_PER_TRANSLATION_TABLE] = {0};

    *pp_port = -1;

    if ((modid > SOC_INFO(unit).modid_max) ||
        (port > SOC_INFO(unit).port_addr_max)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcm_kt2_modid_get_all(unit, my_modid_list, my_modid_valid,
                             my_modport_base_ptr));

    if (my_modid_valid[0] && (modid == my_modid_list[0])) {
        *pp_port = my_modport_base_ptr[0] + port;
    } else if (my_modid_valid[1] && (modid == my_modid_list[1])) {
        *pp_port = my_modport_base_ptr[1] + port;
    } else if(my_modid_valid[2] && (modid == my_modid_list[2])) {
        *pp_port = my_modport_base_ptr[2] + port;
    } else {
        *pp_port = my_modport_base_ptr[3] + port;
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_kt2_mod_is_coe_mod_check
 * Description:
 *      Given a modid, check if it is a CoE modid
 * Parameters:
 *      unit  - (IN) BCM device number
 *      modid    - (IN) Module ID
 * Return Value:
 *      BCM_E_NONE if it is a CoE mod, BCM_E_NOT_FOUND otherwise
 *      a specific BCM_E_XXXX for other general cases
 */
int
_bcm_kt2_mod_is_coe_mod_check(int unit, int modid)
{
    soc_info_t      *si=&SOC_INFO(unit);
    bcm_module_t    my_modid;

    /* If the device is running in the stacking mode, check that
       the module is one of the CoE modules created, else just
       check that it is one greater than the 'local_modid', which
       was the earlier implementation */
    if(si->coe_stacking_mode) {
        return _bcm_stk_aux_mod_valid(unit, BCM_MODID_TYPE_COE, modid);
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
#if defined BCM_METROLITE_SUPPORT
        if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
            if(modid == (my_modid)) {
                return BCM_E_NONE;
            }
        } else 
#endif
        {
            /* Return BCM_E_NONE to indicate the mod was found */
            if(modid == (my_modid + 1)) {
                return BCM_E_NONE;
            }
        }
    }

    /* Return UNAVAIL by default*/
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_kt2_port_mac_set
 * Description:
 *      configure port table mac address 
 * Parameters:
 *      unit  - (IN) BCM device number
 *      port     - (IN) Port number if valid. 
 *      port_mac - (IN) 64 bit mac address
 * Return Value:
 *      BCM_E_XXX
 */
int
 _bcm_kt2_port_mac_set(int unit, bcm_port_t port, uint64 port_mac)
{
    int                index;         /* Entry index.        */
    bcm_module_t       module_id;     /* Module ID           */
    bcm_port_t         port_id;       /* Port ID.            */
    bcm_trunk_t        trunk_id;      /* Trunk ID.           */
    int                local_id;      /* Hardware ID.        */
    port_tab_entry_t   ptab;        
    egr_port_entry_t   egr_port_entry;
    int                rv;

    /* Get index to PORT table from source port */
    if (BCM_GPORT_IS_SET(port)) {
        rv = _bcm_esw_gport_resolve(unit, port,  &module_id,
                &port_id, &trunk_id, &local_id);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
        rv = _bcm_kt2_modport_to_pp_port_get(unit, module_id, port_id,
                &index);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    } else {
        index = port;
    }
    /* Set MAC address in Port table */
    SOC_IF_ERROR_RETURN(
            READ_PORT_TABm(unit, MEM_BLOCK_ANY, index, &ptab));

    soc_mem_field64_set(unit, PORT_TABm, &ptab, MAC_ADDRESSf, port_mac);

    SOC_IF_ERROR_RETURN( WRITE_PORT_TABm(unit,MEM_BLOCK_ALL, index, &ptab));

    SOC_IF_ERROR_RETURN(READ_EGR_PORTm(unit, MEM_BLOCK_ANY, index,
                &egr_port_entry));

    soc_mem_field64_set(unit, EGR_PORTm, &egr_port_entry, 
                            MAC_ADDRESSf, port_mac);
    SOC_IF_ERROR_RETURN(
            WRITE_EGR_PORTm(unit, MEM_BLOCK_ALL, index, &egr_port_entry));
    
    if (SOC_IS_SABER2(unit) && soc_feature(unit, soc_feature_sat) &&
        soc_property_get(unit, spn_SAT_ENABLE, 0)) {

        egr_sat_config_mac_sa_entry_t sat_cfg_entry;

        SOC_IF_ERROR_RETURN(
            READ_EGR_SAT_CONFIG_MAC_SAm(unit, MEM_BLOCK_ANY, index, &sat_cfg_entry));
        
        soc_mem_field64_set(unit, EGR_SAT_CONFIG_MAC_SAm, &sat_cfg_entry, MAC_ADDRESSf, port_mac);

        SOC_IF_ERROR_RETURN( 
            WRITE_EGR_SAT_CONFIG_MAC_SAm(unit, MEM_BLOCK_ALL, index, &sat_cfg_entry));
    }
   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_port_mac_get
 * Description:
 *      configure port table mac address 
 * Parameters:
 *      unit  - (IN) BCM device number
 *      port     - (IN) Port number if valid. 
 *      port_mac - (OUT) 64 bit mac address
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_kt2_port_mac_get(int unit, bcm_port_t port, uint64 *port_mac)
{
    port_tab_entry_t   ptab;   
    int                index;         /* Entry index.        */
    bcm_module_t       module_id;     /* Module ID           */
    bcm_port_t         port_id;       /* Port ID.            */
    bcm_trunk_t        trunk_id;      /* Trunk ID.           */
    int                local_id;      /* Hardware ID.        */
    int                rv;

    /* Get index to PORT table from source port */
    if (BCM_GPORT_IS_SET(port)) {
        rv = _bcm_esw_gport_resolve(unit, port,  &module_id,
                &port_id, &trunk_id, &local_id);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
        rv = _bcm_kt2_modport_to_pp_port_get(unit, module_id, port_id,
                &index);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    } else {
        index = port;
    }

    SOC_IF_ERROR_RETURN(
            READ_PORT_TABm(unit, MEM_BLOCK_ANY, index, &ptab));

    soc_mem_field64_get(unit, PORT_TABm, &ptab, MAC_ADDRESSf, port_mac);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_subport_pbmp_update
 * Description:
 *      given a pbmp, add linkphy/subtag pp_port pbmp and
 *      remove linkphy/subtag port pbmp
 * Parameters:
 *      unit  - (IN) BCM device number
 *      pbmp    - (IN/OUT) port bit map
 */

void
_bcm_kt2_subport_pbmp_update(int unit, bcm_pbmp_t *pbmp)
{
    BCM_PBMP_OR(*pbmp, SOC_INFO(unit).linkphy_pp_port_pbm);
    BCM_PBMP_OR(*pbmp, SOC_INFO(unit).subtag_pp_port_pbm);
    BCM_PBMP_OR(*pbmp, SOC_INFO(unit).general_pp_port_pbm);
    return;
}

/*
 * Function:
 *      _bcm_kt2_flexio_pbmp_update
 * Description:
 *      given a pbmp, add flexio pbmp
 * Parameters:
 *      unit  - (IN) BCM device number
 *      pbmp    - (IN/OUT) port bit map
 */

int
bcm_kt2_flexio_pbmp_update(int unit, bcm_pbmp_t *pbmp)
{
    bcm_port_t first_port, mxqblk_port;
    int        mxqblk, blk_idx;

    if (NULL == pbmp) {
        return BCM_E_PORT;
    }
    for (mxqblk = 0; mxqblk < (KT2_MAX_MXQBLOCKS - 1); mxqblk++) {
        /* Get first port in each MXQ block */
        first_port = (*kt2_mxqblock_ports[unit])[mxqblk][0];
        if (SOC_PORT_TYPE(unit, first_port) == SOC_BLK_MXQPORT) {
            /* Add the mxqport at index 1, 2, & 3 to gven pbmp and update the
            * port_type so that it is treated as valid port*/
            for (blk_idx = 1; blk_idx < KT2_MAX_MXQPORTS_PER_BLOCK; blk_idx++) {
                mxqblk_port = (*kt2_mxqblock_ports[unit])[mxqblk][blk_idx];
                if (mxqblk_port < SOC_INFO(unit).lb_port) {
                    BCM_PBMP_PORT_ADD(*pbmp, mxqblk_port);
                    SOC_PORT_TYPE(unit, mxqblk_port) = SOC_BLK_MXQPORT;
                }
            }
        }
    }
    return BCM_E_NONE;
}

int
_bcm_kt2_flexio_pbmp_update(int unit, bcm_pbmp_t *pbmp)
{

#if defined(BCM_METROLITE_SUPPORT)
    if(SOC_IS_METROLITE(unit)) {
       BCM_IF_ERROR_RETURN(bcm_ml_flexio_pbmp_update(unit, pbmp));
    } else
#endif
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
       BCM_IF_ERROR_RETURN(bcm_sb2_flexio_pbmp_update(unit, pbmp));
    } else
#endif
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(bcm_kt2_flexio_pbmp_update(unit, pbmp));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_source_trunk_map_set
 * Description:
 *      Helper funtion to modify fields in SOURCE_TRUNK_MAP memory.
 * Parameters:
 *      unit  - Device number
 *      port  - Port number
 *      field - Field name within SOURCE_TRUNK_MAP table entry
 *      value - new field value
 * Return Value:
 *      BCM_E_XXX 
 */

int
_bcm_kt2_source_trunk_map_set(int unit, bcm_port_t port, 
                                         soc_field_t field, uint32 value)
{
    bcm_module_t modid;
    int index;

    BCM_IF_ERROR_RETURN(
        _bcm_kt2_pp_port_to_modport_get(unit, port, &modid, &port));

    /* Port sanity check. */
    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return (BCM_E_PORT);
    }

    /* Calculate table index. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_src_mod_port_table_index_get(unit, modid, port, &index));

    return soc_mem_field32_modify(unit,
               SOURCE_TRUNK_MAP_TABLEm, index, field, value);

}

/*
 * Function:
 *      _bcm_kt2_source_trunk_map_get
 * Description:
 *      Helper funtion to get fields in SOURCE_TRUNK_MAP memory.
 * Parameters:
 *      unit  - Device number
 *      port  - Port number
 *      field - Field name within SOURCE_TRUNK_MAP table entry
 *      value - New field value
 * Return Value:
 *      BCM_E_XXX 
 */

int
_bcm_kt2_source_trunk_map_get(int unit, bcm_port_t port, 
                                         soc_field_t field, uint32 *value)
{
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS];
    bcm_module_t modid;
    int index;

    /* Input parameters check */
    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    /* Memory field is valid check. */ 
    if (!SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, field)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_kt2_pp_port_to_modport_get(unit, port, &modid, &port));

    /* Port sanity check. */
    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return (BCM_E_PORT);
    }

    /* Get table index. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_src_mod_port_table_index_get(unit, modid, port, &index));

    if ((index > soc_mem_index_max(unit, SOURCE_TRUNK_MAP_TABLEm)) ||
        (index < soc_mem_index_min(unit, SOURCE_TRUNK_MAP_TABLEm))) {
        return (BCM_E_INTERNAL);
    }

    /* Read table entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY, index, buf));

    /* Read requested field value. */
    *value = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm, buf, field);

    return (BCM_E_NONE);

}

/*
 * Function:
 *      _bcm_kt2_trunk_table_read
 * Purpose:
 *      Read trunk table entry and parse it to common
 *      (bcm_port_cfg_t) structure.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      port       - (IN)Port number.
 *      tid        - (OUT)Trunk group ID.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_trunk_table_read(int unit, bcm_port_t port, int *tid)
{
    int    modid;
    int    src_trk_idx = 0; /*Source Trunk table index.*/
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/

    BCM_IF_ERROR_RETURN(
        _bcm_kt2_pp_port_to_modport_get(unit, port, &modid, &port));

    /* Port sanity check. */
    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return (BCM_E_PORT);
    }

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, modid,
                port, &src_trk_idx));

    /* Read source trunk map table. */
    SOC_IF_ERROR_RETURN (BCM_XGS3_MEM_READ(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                           src_trk_idx, &trunk_map_entry));

    /* Get trunk group id. */
    *tid = 0;
    if (1 == soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                &trunk_map_entry, PORT_TYPEf)) {
        *tid = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                &trunk_map_entry, TGIDf);
    }

    return (BCM_E_NONE); 
}

/*
 * Function:
 *      _bcm_kt2_trunk_table_write
 * Purpose:
 *      Write trunk table entry.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      port       - (IN)Port number.
 *      tid        - (IN)Trunk group ID. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_trunk_table_write(int unit, bcm_port_t port, int tid)
{
    int    rv;
    int    tgid;
    int    port_type;
    int    modid;
    int    src_trk_idx = 0; /*Source Trunk table index.*/
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/

    if (tid != BCM_TRUNK_INVALID) {
        tgid = tid;
        port_type = 1;
    } else {
        tgid = 0;
        port_type = 0;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_kt2_pp_port_to_modport_get(unit, port, &modid, &port));

    /* Port sanity check. */
    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return (BCM_E_PORT);
    }

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, modid,
                port, &src_trk_idx));

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);

    /* Read source trunk map table. */
    rv = BCM_XGS3_MEM_READ(unit, SOURCE_TRUNK_MAP_TABLEm, 
            src_trk_idx, &trunk_map_entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    /* Set trunk group id. */ 
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
            TGIDf, tgid);

    /* Set port is part of trunk flag. */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, &trunk_map_entry,
            PORT_TYPEf, port_type);

    /* Write entry to hw. */
    rv = BCM_XGS3_MEM_WRITE(unit, SOURCE_TRUNK_MAP_TABLEm,
            src_trk_idx, &trunk_map_entry);

    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);

    return rv; 
}

/*
 * Function:
 *     bcm_kt2_port_drain_cells
 * Purpose:
 *     To drain cells associated to the port.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) Port
 * Returns:
 *     BCM_E_XXX
 */
int bcm_kt2_port_drain_cells(int unit, int port)
{
    mac_driver_t *macd;
    int rv = BCM_E_NONE;

    /* Call Port Control module */
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        return bcmi_esw_portctrl_egress_queue_drain(unit, port);
    }
    
    PORT_LOCK(unit);
    rv = soc_mac_probe(unit, port, &macd);

    if (BCM_SUCCESS(rv)) {
        rv = MAC_CONTROL_SET(macd, unit, port, SOC_MAC_CONTROL_EGRESS_DRAIN, 1);

    }
    PORT_UNLOCK(unit);
    return rv;
}

/************ E2EFC CODE START *************************/

#define E2EFC_REMOTE_MODE_VALID(_m_)                                        \
    do {                                                                    \
        if ((_m_) != bcmPortE2efcModeTx && (_m_) != bcmPortE2efcModeRx) {   \
            return BCM_E_UNAVAIL;                                           \
        }                                                                   \
    } while (0)
#define KT2_BYTE2CELL(_a_) ((_a_ + 192) / 192)

#define KT2_CELL2BYTE(_a_) ((_a_) * 192)

#define KT2_E2EFC_PBM2BINDEX(_a_)   \
    do {                            \
        switch (_a_)                \
        {                           \
            case 1:                 \
                (_a_) = 1;          \
                break;              \
            case 2:                 \
                (_a_) = 2;          \
                break;              \
            case 4:                 \
                (_a_) = 3;          \
                break;              \
            case 8:                 \
                (_a_) = 4;          \
                break;              \
            default:                \
                return BCM_E_PARAM; \
        }                           \
    } while (0)

#define KT2_E2EFC_RMOD_NUM          4
#define KT2_E2EFC_MODULE_HDR_LEN    2
#define KT2_PORT_BLOCK_ENT_NUM      4
#define KT2_PORT_BLOCK_INIT_NUM     6

STATIC const soc_field_t e2efc_rmod_id_field[KT2_E2EFC_RMOD_NUM] = {
        RMOD_ID0f, RMOD_ID1f, RMOD_ID2f, RMOD_ID3f};
STATIC const soc_reg_t
e2efc_rmod_id_reg[bcmPortE2efcModeCount][KT2_E2EFC_RMOD_NUM] = {
        {E2EFC_TX_RMODID_0r, E2EFC_TX_RMODID_0r,
        E2EFC_TX_RMODID_1r, E2EFC_TX_RMODID_1r},
        {E2EFC_RX_RMODID_0r, E2EFC_RX_RMODID_0r,
        E2EFC_RX_RMODID_1r, E2EFC_RX_RMODID_1r}};
STATIC const soc_field_t e2efc_rmod_enabled[KT2_E2EFC_RMOD_NUM] = {
        V0f, V1f, V2f, V3f};

STATIC const soc_reg_t
e2efc_module_hdr_reg[KT2_E2EFC_MODULE_HDR_LEN] =
        {XLMAC_E2EFC_MODULE_HDR_0r, XLMAC_E2EFC_MODULE_HDR_1r};

STATIC int
kt2_e2efc_remote_port_num(int unit)
{
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        return 24;
    }
#endif
    return 48;
}

/*
 * Description:
 *      Enable and configure the remote module to apply E2EFC mechanism.
 *      Or disable it.
 * Parameter:
 *      unit           - (IN) unit number
 *      remote_module  - (IN) remote module id
 *      mode           - (IN) selected mode
 *      enable         - (IN) enable or disable selection
 *      e2efc_rmod_cfg - (IN) structure containing the configuration.
 *                       Ignored when enable = 0.
 */
int
bcmi_kt2_port_e2efc_remote_module_enable_set(
    int unit,
    bcm_module_t remote_module,
    bcm_port_e2efc_mode_t mode,
    int enable,
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg)
{
    bcm_port_t local_port;
    uint32 val;
    bcm_module_t remote_module_clear = 0;
    int modid, i, blk, bindex, blk_num, port_mode;

    /* XLMAC Related Parameters */
    uint32 hdr_0_lo, hdr_0_hi, hdr_1_lo, hdr_1_hi;
    uint32 src_mod, dest_mod, src_port;
    int time_unit_sel, time_units, value_a, value_b;
    uint32 val_min, val_max, new_pbm = 0;
#if defined(BCM_SABER2_SUPPORT)
    uint64 val_64;
    int hdr_0 = 0, hdr_1 = 1;
#endif

    E2EFC_REMOTE_MODE_VALID(mode);
    /* Check the remote module id is in the valid range or not */
    if (!SOC_MODID_ADDRESSABLE(unit, remote_module)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid remote module ID.\n")));
        return BCM_E_BADID;
    }

    if (e2efc_rmod_cfg == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Null Remote Module Configuration.\n")));
        return BCM_E_PARAM;
    }

    /* Check the logical port is in the right range  */
    if (e2efc_rmod_cfg->local_port < 0 ||
        e2efc_rmod_cfg->local_port >= MAX_PORT(unit)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Port Range. Should be >= 0 && < %d\n"),
                  MAX_PORT(unit)));
        return BCM_E_PARAM;
    }

    if (enable == 1) {
        /* Search whether the existing remote module ID exist */
        for (i = 0; i < KT2_E2EFC_RMOD_NUM; i++) {
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_reg[mode][i],
                                              REG_PORT_ANY, 0, &val));
            if (remote_module == (soc_reg_field_get(
                                    unit, e2efc_rmod_id_reg[mode][i], val,
                                    e2efc_rmod_id_field[i]))) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                          "Remote Module ID already exists.\n")));
                return BCM_E_EXISTS;
            }
        }
        /* Traverse 4 remote module id and setup the one that is not enabled */
        modid = -1;
        for (i = 0; i < KT2_E2EFC_RMOD_NUM; i++) {
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_reg[mode][i],
                                              REG_PORT_ANY, 0, &val));
            if (!(soc_reg_field_get(unit, e2efc_rmod_id_reg[mode][i], val,
                                    e2efc_rmod_enabled[i]))) {
                soc_reg_field_set(unit, e2efc_rmod_id_reg[mode][i], &val,
                                  e2efc_rmod_id_field[i], remote_module);
                soc_reg_field_set(unit, e2efc_rmod_id_reg[mode][i], &val,
                                  e2efc_rmod_enabled[i], enable);
                BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                                  e2efc_rmod_id_reg[mode][i],
                                                  REG_PORT_ANY, 0, val));
                modid = i;
                break;
            }
        }
        if (modid == -1 ) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                      "E2EFC No Available Remote.Module to Setup"
                      "Maximun 4 Remote Module only.\n")));
            return BCM_E_FULL;
        }
        /* Set up Higig Port and Port Speed Information */
        if (mode == bcmPortE2efcModeTx) {
            /* Higig port setting */
            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(
                                    unit, e2efc_rmod_cfg->local_port,
                                    &local_port));
            if (!IS_HG_PORT(unit, local_port)) {
                LOG_ERROR(
                    BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "Error: unit %d port %d is not a Higig port. "
                                "E2EFC messages can only be transmitted to "
                                "or received from Higig ports.\n"),
                     unit, local_port));
                return BCM_E_PARAM;

            }
            /* Register E2EFC_RMOD_CFG fill in */

            blk = SOC_PORT_BLOCK(unit, local_port);
            bindex = SOC_PORT_BINDEX(unit, local_port);

            blk_num = SOC_BLOCK_INFO(unit, blk).number;

            blk_num = (blk_num >= 6) ? (blk_num - 6) : blk_num ;

            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_RMOD_CFGr,
                                              REG_PORT_ANY, modid, &val));
            if (soc_reg_field_valid(unit, E2EFC_RMOD_CFGr, PORT_GRP_IDf)) {
                soc_reg_field_set(unit, E2EFC_RMOD_CFGr, &val,
                                  PORT_GRP_IDf, blk_num);
            }
            new_pbm |= (1 << bindex);
            soc_reg_field_set(unit, E2EFC_RMOD_CFGr, &val,
                              TX_ENABLE_BMPf, new_pbm);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_RMOD_CFGr,
                                              REG_PORT_ANY, modid, val));
            /* XLMAC Module Header Set up */
            bcm_esw_port_encap_get(unit, local_port, &port_mode);
            /* Higig2 Mode */
            bcm_esw_stk_my_modid_get(unit, (int *)&src_mod);
            if (port_mode == BCM_PORT_ENCAP_HIGIG2) {
                /* default setting K.SOP = 0xfb, MCST = 0(unicast)*/
                hdr_0_hi = 0xfb00000;
                dest_mod = (remote_module & 0xff) << 8;
                hdr_0_hi = hdr_0_hi | dest_mod;
                hdr_0_lo = 0x00000000;
                src_mod = (src_mod & 0xff) << 24;
                src_port = (local_port & 0xff) << 16;
                hdr_0_lo = hdr_0_lo | src_mod | src_port;
            } else {
                /* Higig Mode */
                /* default setting K.SOP = 0xfb, HGI = 8, VID = 1 */
                hdr_0_hi = 0xfb800001;
                src_mod = (src_mod & 0x2f) << 27;
                dest_mod = remote_module & 0xff;
                /* default setting OPCODE = 1, COS = 7 */
                hdr_0_lo = 0x01000701 | src_mod | dest_mod;
            }

            if (!IS_XL_PORT(unit, local_port)) {
                BCM_IF_ERROR_RETURN(soc_reg32_get(unit, XIBP_MH0r, local_port, 0, &val));
                soc_reg_field_set(unit, XIBP_MH0r, &val, MH_BYTES_0_3f, hdr_0_lo);
                BCM_IF_ERROR_RETURN(soc_reg32_set(unit, XIBP_MH0r, local_port, 0, val));

                BCM_IF_ERROR_RETURN(soc_reg32_get(unit, XIBP_MH1r, local_port, 0, &val));
                soc_reg_field_set(unit, XIBP_MH1r, &val, MH_BYTES_4_7f, hdr_0_hi);
                BCM_IF_ERROR_RETURN(soc_reg32_set(unit, XIBP_MH1r, local_port, 0, val));
            }
#if defined(BCM_SABER2_SUPPORT)
            if(SOC_IS_SABER2(unit)) {
               BCM_IF_ERROR_RETURN(
                   soc_reg64_get(unit, e2efc_module_hdr_reg[hdr_0],
                                 local_port, 0, &val_64));
               soc_reg64_field32_set(unit, e2efc_module_hdr_reg[hdr_0],
                                     &val_64, E2EFC_MODULE_HDR_0_LOf, hdr_0_lo);
               soc_reg64_field32_set(unit, e2efc_module_hdr_reg[hdr_0],
                                     &val_64, E2EFC_MODULE_HDR_0_HIf, hdr_0_hi);
               BCM_IF_ERROR_RETURN(
                   soc_reg64_set(unit, e2efc_module_hdr_reg[hdr_0],
                                 local_port, 0, val_64));
            }
#endif
            if (port_mode == BCM_PORT_ENCAP_HIGIG2) {
                /* default setting vlan = 1, opcode = 1 */
                hdr_1_lo = 0x00010100;
                hdr_1_hi = 0x00000000;
            } else {
                hdr_1_lo = 0x00000000;
                hdr_1_hi = 0x00000000;
            }
#if defined(BCM_SABER2_SUPPORT)
            if(SOC_IS_SABER2(unit)) {
               BCM_IF_ERROR_RETURN(
                   soc_reg64_get(unit, e2efc_module_hdr_reg[hdr_1],
                                 local_port, 0, &val_64));
               soc_reg64_field32_set(unit, e2efc_module_hdr_reg[hdr_1],
                                     &val_64, E2EFC_MODULE_HDR_1_LOf, hdr_1_lo);
               soc_reg64_field32_set(unit, e2efc_module_hdr_reg[hdr_1],
                                     &val_64, E2EFC_MODULE_HDR_1_HIf,
                                     hdr_1_hi);
               BCM_IF_ERROR_RETURN(
                   soc_reg64_set(unit, e2efc_module_hdr_reg[hdr_1],
                                 local_port, 0, val_64));
            } else
#endif
            {
               BCM_IF_ERROR_RETURN(soc_reg32_get(unit, XIBP_MH2r, local_port, 0, &val));
               soc_reg_field_set(unit, XIBP_MH2r, &val, MH_BYTES_8_11f, hdr_1_lo);
               BCM_IF_ERROR_RETURN(soc_reg32_set(unit, XIBP_MH2r, local_port, 0, val));

               BCM_IF_ERROR_RETURN(soc_reg32_get(unit, XIBP_MH3r, local_port, 0, &val));
               soc_reg_field_set(unit, XIBP_MH3r, &val, MH_BYTES_12_15f, hdr_1_hi);
               BCM_IF_ERROR_RETURN(soc_reg32_set(unit, XIBP_MH3r, local_port, 0, val));
            }
            /* In the absence of congestion, an E2EFC message is still
            * generated to refresh congestion status when the E2EFC max
            * timer expires. This timer is specified in units of 250ns or
            * 25us, and up to 1023 time units can be specified.
            * The selection of time unit and the number of time units
            * is computed as follows:
             *
             * Let E2EFC message refresh rate = N messages/sec, and N != 0,
             * A = # of 250ns time units = 1 sec / N / 250ns = 4000000 / N,
             * B = # of 25us time units = 1 sec / N / 25us =
             * 40000 / N = A / 100.
             *
             * If A < 1, configure the minimum allowed timer value:
             * time unit = 250ns, # of time units = 1.
             *
             * If A is between 1 and 1023, configure the timer as:
             * time unit = 250ns, # of time units = A.
             *
             * If A > 1023 and B <= 1023, configure the timer as:
             * time unit = 25us, # of time units = B.
             *
             * If B > 1023, configure the maximum allowed timer value:
             * time unit = 25us, # of time units = 1023.
             *
             * Note: If N = 0, # of time units will be set to 0, and E2EFC
             * message refresh is effectively disabled. E2ECC message
             * will be generated only when there is a change in congestion
             * status.
             */

            if (e2efc_rmod_cfg->pkt_per_second == 0) {
                time_unit_sel = 0;
                time_units = 0;
            } else {
                value_a = 4000000 / e2efc_rmod_cfg->pkt_per_second;
                value_b = value_a / 100;
                if (value_a < 1) {
                    time_unit_sel = 0;
                    time_units = 1;
                } else if (value_a <= 1023) {
                    time_unit_sel = 0;
                    time_units = value_a;
                } else if ((value_a > 1023) && (value_b <= 1023)) {
                    time_unit_sel = 1;
                    time_units = value_b;
                } else { /* value_b > 1023 */
                    time_unit_sel = 1;
                    time_units = 1023;
                }
            }
            /* E2EFC meesage trasmit period setup */
            /* For Min TX Timer */
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_MIN_TX_TIMERr,
                                              REG_PORT_ANY, modid, &val_min));
            soc_reg_field_set(unit, E2EFC_MIN_TX_TIMERr, &val_min,
                              LGf, time_unit_sel);
            soc_reg_field_set(unit, E2EFC_MIN_TX_TIMERr, &val_min,
                              TIMERf, time_units);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_MIN_TX_TIMERr,
                                              REG_PORT_ANY, modid, val_min));
            /* For Max TX Timer */
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_MAX_TX_TIMERr,
                                              REG_PORT_ANY, modid, &val_max));
            soc_reg_field_set(unit, E2EFC_MAX_TX_TIMERr, &val_max,
                              LGf, time_unit_sel);
            soc_reg_field_set(unit, E2EFC_MAX_TX_TIMERr, &val_max,
                              TIMERf, time_units);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_MAX_TX_TIMERr,
                                              REG_PORT_ANY, modid, val_max));
        } else {
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                        "E2EFC RX mode not allowed to configure timer"
                        " and XLMAC and HG port setup.\n")));
        }
    } else {
        /* Traverse 4 rmod id and clear the one that match rmod id */
        modid = -1;
        for (i = 0; i < KT2_E2EFC_RMOD_NUM; i++) {
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_reg[mode][i],
                                              REG_PORT_ANY, 0, &val));
            if (remote_module == (soc_reg_field_get(
                                  unit, e2efc_rmod_id_reg[mode][i], val,
                                  e2efc_rmod_id_field[i]))) {
                soc_reg_field_set(unit, e2efc_rmod_id_reg[mode][i], &val,
                                  e2efc_rmod_id_field[i], remote_module_clear);
                soc_reg_field_set(unit, e2efc_rmod_id_reg[mode][i], &val,
                                  e2efc_rmod_enabled[i], 0);
                BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                                  e2efc_rmod_id_reg[mode][i],
                                                  REG_PORT_ANY, 0, val));
                modid = i;
                break;
            }
        }
        if (modid == -1 ) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                      "E2EFC No Existing Remote.Module to Clear"
                      "Maximun 4 Remote Module only.\n")));
            return BCM_E_NOT_FOUND;
        }
    }

    return BCM_E_NONE;
}


/*
 * Description:
 *      Get the enable mode and configuration of the remote module that
 *      applies E2EFC mechanism.
 * Parameter:
 *      unit           - (IN) unit number
 *      remote_module  - (IN) remote module id
 *      mode           - (IN) selected mode
 *      enable         - (OUT) enable or disable selection
 *      e2efc_rmod_cfg - (OUT) structure containing the configuration.
 *                       Only valid if the remote module is enabled.
 */
int
bcmi_kt2_port_e2efc_remote_module_enable_get(
    int unit,
    bcm_module_t remote_module,
    bcm_port_e2efc_mode_t mode,
    int *enable,
    bcm_port_e2efc_remote_module_config_t *e2efc_rmod_cfg)
{
    uint32 val, val_p, val_t;
    int i, time_unit_sel, time_units;
    int blk_num, pbm, phy_port = 0, log_port = 0;

    E2EFC_REMOTE_MODE_VALID(mode);

    /* Check the remote module id is in the valid range or not */
    if (!SOC_MODID_ADDRESSABLE(unit, remote_module)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid remote module ID.\n")));
        return BCM_E_BADID;
    }

    if (enable == NULL || e2efc_rmod_cfg == NULL) {
        return BCM_E_PARAM;
    }

    /* Check the logical port is in the right range  */
    if (e2efc_rmod_cfg->local_port < 0 ||
        e2efc_rmod_cfg->local_port >= MAX_PORT(unit)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Port Range. Should be >= 0 && < %d\n"),
                  MAX_PORT(unit)));
        return BCM_E_PARAM;
    }

    /* if the matched rmod id is not found then the "enable" is zero */
    *enable = 0;
    /*
     * Only Mode TX has the bcm_port_e2efc_remote_module_config_t
     * configuration, so if mode == RX, the get API will check whether the
     * remote_module exists and return enable value.
     */
    /* Traverse 4 rmod id and search the one that match rmod id */
    for (i = 0; i < KT2_E2EFC_RMOD_NUM; i++) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_reg[mode][i],
                                          REG_PORT_ANY, 0, &val));
        if (remote_module == (soc_reg_field_get(unit,
                              e2efc_rmod_id_reg[mode][i], val,
                              e2efc_rmod_id_field[i]))) {
            if (mode == bcmPortE2efcModeTx) {
                BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_RMOD_CFGr,
                                                  REG_PORT_ANY, i, &val_p));
                if (soc_reg_field_valid(unit, E2EFC_RMOD_CFGr, PORT_GRP_IDf)) {

                    blk_num = soc_reg_field_get(unit, E2EFC_RMOD_CFGr, val_p,
                                                PORT_GRP_IDf);
                } else {
                    blk_num = 0;
                }
                pbm = soc_reg_field_get(unit, E2EFC_RMOD_CFGr, val_p,
                                        TX_ENABLE_BMPf);
                KT2_E2EFC_PBM2BINDEX(pbm);
                phy_port = (blk_num * KT2_PORT_BLOCK_ENT_NUM) + pbm
                            + KT2_PORT_BLOCK_INIT_NUM;
                log_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
                e2efc_rmod_cfg->local_port = log_port;
                BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_MIN_TX_TIMERr,
                                                  REG_PORT_ANY, i, &val_t));
                time_unit_sel =
                    soc_reg_field_get(unit, E2EFC_MIN_TX_TIMERr,
                                      val_t, LGf);
                time_units =
                    soc_reg_field_get(unit, E2EFC_MIN_TX_TIMERr,
                                      val_t, TIMERf);
                if (time_units == 0) {
                    e2efc_rmod_cfg->pkt_per_second = 0;
                } else if (time_unit_sel == 1) {
                    /* pkt_per_second = 1 / 25us / time_units */
                    e2efc_rmod_cfg->pkt_per_second = 40000 / time_units;
                } else {
                    /* pkt_per_second = 1 / 250ns / time_units */
                    e2efc_rmod_cfg->pkt_per_second = 4000000 / time_units;
                }
            }
            *enable = 1;
            break;
        }
    }
    return BCM_E_NONE;
}

/* helper function to search the remote module pointer */
int
bcmi_kt2_e2efc_remote_module_ptr_get(
    int unit,
    bcm_module_t remote_module,
    int *module_ptr)
{
    int i, ptr;
    uint32 val;
    bcm_port_e2efc_mode_t mode = bcmPortE2efcModeTx;

    if (module_ptr == NULL) {
        return BCM_E_PARAM;
    }
    /* Check the remote module id is in the valid range or not */
    if (!SOC_MODID_ADDRESSABLE(unit, remote_module)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid remote module ID.\n")));
        return BCM_E_BADID;
    }

    ptr = -1;
    for (i = 0; i < KT2_E2EFC_RMOD_NUM; i++) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_reg[mode][i],
                                          REG_PORT_ANY, 0, &val));
        if (remote_module == (soc_reg_field_get(
                              unit, e2efc_rmod_id_reg[mode][i], val,
                              e2efc_rmod_id_field[i]))) {
            ptr = i;
            break;
        }
    }
    if (ptr == -1) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Remote module ID not found.\n")));
        return BCM_E_NOT_FOUND;
    } else {
        *module_ptr = ptr;
    }
    return BCM_E_NONE;
}

/* helper function to search the remote module id */
int
bcmi_kt2_e2efc_remote_module_id_get(
    int unit,
    int module_ptr,
    bcm_module_t *remote_module)
{
    uint32 val;
    bcm_port_e2efc_mode_t mode = bcmPortE2efcModeTx;

    if (remote_module == NULL) {
        return BCM_E_PARAM;
    }

    if (module_ptr < 0 || module_ptr >= KT2_E2EFC_RMOD_NUM) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid remote module pointer. Should be 0~3.\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, e2efc_rmod_id_reg[mode][module_ptr],
                                      REG_PORT_ANY, 0, &val));
    /* the remote moduel is enabled */
    if (soc_reg_field_get(unit,
                          e2efc_rmod_id_reg[mode][module_ptr], val,
                          e2efc_rmod_enabled[module_ptr])) {
        *remote_module = (soc_reg_field_get(
                            unit, e2efc_rmod_id_reg[mode][module_ptr], val,
                            e2efc_rmod_id_field[module_ptr]));
    } else {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/* helper function to init E2EFC remote port */
int
bcmi_kt2_port_e2efc_remote_port_init(int unit)
{
    int i;
    uint32 val;

    for (i = 0; i < kt2_e2efc_remote_port_num(unit); i++) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, &val));
        soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val, Vf, 0);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, val));
    }

    return BCM_E_NONE;
}
/* Helper function to configure thresholds. */
STATIC int
bcmi_kt2_port_e2efc_configure_thresholds(int unit, int rport_handle_id,
                                         bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg)
{
   uint32  val_off, val_on, val_drop;
   int     set, reset, drop;

   switch(e2efc_rport_cfg->e2efc_resource_type) {
       case BCM_E2EFC_RESOURCE_TYPE_INT_MEM:
            set = KT2_BYTE2CELL(e2efc_rport_cfg->xoff_threshold_bytes);
            reset = KT2_BYTE2CELL(e2efc_rport_cfg->xon_threshold_bytes);
            drop = KT2_BYTE2CELL(e2efc_rport_cfg->drop_threshold_bytes);

            BCM_IF_ERROR_RETURN(READ_E2EFCINT_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            soc_reg_field_set(unit, E2EFCINT_CNT_SET_LIMITr, &val_off, SET_LIMITf, set);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCINT_CNT_SET_LIMITr(unit, rport_handle_id, val_off));

            BCM_IF_ERROR_RETURN(READ_E2EFCINT_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            soc_reg_field_set(unit, E2EFCINT_CNT_RESET_LIMITr, &val_on, RESET_LIMITf, reset);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCINT_CNT_RESET_LIMITr(unit, rport_handle_id, val_on));

            BCM_IF_ERROR_RETURN(READ_E2EFCINT_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            soc_reg_field_set(unit, E2EFCINT_CNT_DISC_LIMITr, &val_drop, DISC_LIMITf, drop);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCINT_CNT_DISC_LIMITr(unit, rport_handle_id, val_drop));
            break;
        case  BCM_E2EFC_RESOURCE_TYPE_QEN:
            set = e2efc_rport_cfg->xoff_threshold_packets;
            reset = e2efc_rport_cfg->xon_threshold_packets;
            drop = e2efc_rport_cfg->drop_threshold_packets;

            BCM_IF_ERROR_RETURN(READ_E2EFCQEN_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            soc_reg_field_set(unit, E2EFCQEN_CNT_SET_LIMITr, &val_off, SET_LIMITf, set);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCQEN_CNT_SET_LIMITr(unit, rport_handle_id, val_off));

            BCM_IF_ERROR_RETURN(READ_E2EFCQEN_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            soc_reg_field_set(unit, E2EFCQEN_CNT_RESET_LIMITr, &val_on, RESET_LIMITf, reset);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCQEN_CNT_RESET_LIMITr(unit, rport_handle_id, val_on));

            BCM_IF_ERROR_RETURN(READ_E2EFCQEN_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            soc_reg_field_set(unit, E2EFCQEN_CNT_DISC_LIMITr, &val_drop, DISC_LIMITf, drop);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCQEN_CNT_DISC_LIMITr(unit, rport_handle_id, val_drop));
            break;
        case BCM_E2EFC_RESOURCE_TYPE_RQE:
            set = e2efc_rport_cfg->xoff_threshold_packets;
            reset = e2efc_rport_cfg->xon_threshold_packets;
            drop = e2efc_rport_cfg->drop_threshold_packets;

            BCM_IF_ERROR_RETURN(READ_E2EFCRQE_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            soc_reg_field_set(unit, E2EFCRQE_CNT_SET_LIMITr, &val_off, SET_LIMITf, set);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCRQE_CNT_SET_LIMITr(unit, rport_handle_id, val_off));

            BCM_IF_ERROR_RETURN(READ_E2EFCRQE_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            soc_reg_field_set(unit, E2EFCRQE_CNT_RESET_LIMITr, &val_on, RESET_LIMITf, reset);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCRQE_CNT_RESET_LIMITr(unit, rport_handle_id, val_on));

            BCM_IF_ERROR_RETURN(READ_E2EFCRQE_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            soc_reg_field_set(unit, E2EFCRQE_CNT_DISC_LIMITr, &val_drop, DISC_LIMITf, drop);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCRQE_CNT_DISC_LIMITr(unit, rport_handle_id, val_drop));
            break;
         case BCM_E2EFC_RESOURCE_TYPE_EXT_MEM:
#if defined(BCM_METROLITE_SUPPORT)
                  if (SOC_IS_METROLITE(unit)) {
                      return BCM_E_PARAM;
                  }
#endif
            set = KT2_BYTE2CELL(e2efc_rport_cfg->xoff_threshold_bytes);
            reset = KT2_BYTE2CELL(e2efc_rport_cfg->xon_threshold_bytes);
            drop = KT2_BYTE2CELL(e2efc_rport_cfg->drop_threshold_bytes);

            BCM_IF_ERROR_RETURN(READ_E2EFCEXT_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            soc_reg_field_set(unit, E2EFCEXT_CNT_SET_LIMITr, &val_off, SET_LIMITf, set);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCEXT_CNT_SET_LIMITr(unit, rport_handle_id, val_off));

            BCM_IF_ERROR_RETURN(READ_E2EFCEXT_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            soc_reg_field_set(unit, E2EFCEXT_CNT_RESET_LIMITr, &val_on, RESET_LIMITf, reset);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCEXT_CNT_RESET_LIMITr(unit, rport_handle_id, val_on));

            BCM_IF_ERROR_RETURN(READ_E2EFCEXT_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            soc_reg_field_set(unit, E2EFCEXT_CNT_DISC_LIMITr, &val_drop, DISC_LIMITf, drop);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCEXT_CNT_DISC_LIMITr(unit, rport_handle_id, val_drop));
            break;
          case BCM_E2EFC_RESOURCE_TYPE_EMA:
#if defined(BCM_METROLITE_SUPPORT)
                  if (SOC_IS_METROLITE(unit)) {
                      return BCM_E_PARAM;
                  }
#endif
            set = KT2_BYTE2CELL(e2efc_rport_cfg->xoff_threshold_bytes);
            reset = KT2_BYTE2CELL(e2efc_rport_cfg->xon_threshold_bytes);
            drop = KT2_BYTE2CELL(e2efc_rport_cfg->drop_threshold_bytes);

            BCM_IF_ERROR_RETURN(READ_E2EFCEMA_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            soc_reg_field_set(unit, E2EFCEMA_CNT_SET_LIMITr, &val_off, SET_LIMITf, set);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCEMA_CNT_SET_LIMITr(unit, rport_handle_id, val_off));

            BCM_IF_ERROR_RETURN(READ_E2EFCEMA_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            soc_reg_field_set(unit, E2EFCEMA_CNT_RESET_LIMITr, &val_on, RESET_LIMITf, reset);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCEMA_CNT_RESET_LIMITr(unit, rport_handle_id, val_on));

            BCM_IF_ERROR_RETURN(READ_E2EFCEMA_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            soc_reg_field_set(unit, E2EFCEMA_CNT_DISC_LIMITr, &val_drop, DISC_LIMITf, drop);
            BCM_IF_ERROR_RETURN(WRITE_E2EFCEMA_CNT_DISC_LIMITr(unit, rport_handle_id, val_drop));
            break;
          default:
            return BCM_E_PARAM;
      }
      return BCM_E_NONE;
}

/* Helper function to get thresholds. */
STATIC int
bcmi_kt2_port_e2efc_get_thresholds(int unit, int rport_handle_id,
                                   bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg)
{
   uint32  val_off, val_on, val_drop;
   int     set, reset, drop;

   switch(e2efc_rport_cfg->e2efc_resource_type) {
       case BCM_E2EFC_RESOURCE_TYPE_INT_MEM:
            BCM_IF_ERROR_RETURN(READ_E2EFCINT_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            set = soc_reg_field_get(unit, E2EFCINT_CNT_SET_LIMITr, val_off, SET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCINT_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            reset = soc_reg_field_get(unit, E2EFCINT_CNT_RESET_LIMITr, val_on, RESET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCINT_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            drop = soc_reg_field_get(unit, E2EFCINT_CNT_DISC_LIMITr, val_drop, DISC_LIMITf);

            e2efc_rport_cfg->xoff_threshold_bytes = KT2_CELL2BYTE(set);
            e2efc_rport_cfg->xon_threshold_bytes = KT2_CELL2BYTE(reset);
            e2efc_rport_cfg->drop_threshold_bytes = KT2_CELL2BYTE(drop);
            break;
        case BCM_E2EFC_RESOURCE_TYPE_QEN:
            BCM_IF_ERROR_RETURN(READ_E2EFCQEN_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            set = soc_reg_field_get(unit, E2EFCQEN_CNT_SET_LIMITr, val_off, SET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCQEN_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            reset = soc_reg_field_get(unit, E2EFCQEN_CNT_RESET_LIMITr, val_on, RESET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCQEN_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            drop = soc_reg_field_get(unit, E2EFCQEN_CNT_DISC_LIMITr, val_drop, DISC_LIMITf);

            e2efc_rport_cfg->xoff_threshold_packets = set;
            e2efc_rport_cfg->xon_threshold_packets = reset;
            e2efc_rport_cfg->drop_threshold_packets = drop;
            break;
        case BCM_E2EFC_RESOURCE_TYPE_RQE:
            BCM_IF_ERROR_RETURN(READ_E2EFCRQE_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            set = soc_reg_field_get(unit, E2EFCRQE_CNT_SET_LIMITr, val_off, SET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCRQE_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            reset = soc_reg_field_get(unit, E2EFCRQE_CNT_RESET_LIMITr, val_on, RESET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCRQE_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            drop = soc_reg_field_get(unit, E2EFCRQE_CNT_DISC_LIMITr, val_drop, DISC_LIMITf);

            e2efc_rport_cfg->xoff_threshold_packets = set;
            e2efc_rport_cfg->xon_threshold_packets = reset;
            e2efc_rport_cfg->drop_threshold_packets = drop;
            break;
         case BCM_E2EFC_RESOURCE_TYPE_EXT_MEM:
#if defined(BCM_METROLITE_SUPPORT)
            if (SOC_IS_METROLITE(unit)) {
                return BCM_E_PARAM;
            }
#endif

            BCM_IF_ERROR_RETURN(READ_E2EFCEXT_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            set = soc_reg_field_get(unit, E2EFCEXT_CNT_SET_LIMITr, val_off, SET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCEXT_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            reset = soc_reg_field_get(unit, E2EFCEXT_CNT_RESET_LIMITr, val_on, RESET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCEXT_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            drop = soc_reg_field_get(unit, E2EFCEXT_CNT_DISC_LIMITr, val_drop, DISC_LIMITf);

            e2efc_rport_cfg->xoff_threshold_bytes = KT2_CELL2BYTE(set);
            e2efc_rport_cfg->xon_threshold_bytes = KT2_CELL2BYTE(reset);
            e2efc_rport_cfg->drop_threshold_bytes = KT2_CELL2BYTE(drop);
            break;
          case BCM_E2EFC_RESOURCE_TYPE_EMA:
#if defined(BCM_METROLITE_SUPPORT)
            if (SOC_IS_METROLITE(unit)) {
                return BCM_E_PARAM;
            }
#endif
            BCM_IF_ERROR_RETURN(READ_E2EFCEMA_CNT_SET_LIMITr(unit, rport_handle_id, &val_off));
            set = soc_reg_field_get(unit, E2EFCEMA_CNT_SET_LIMITr, val_off, SET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCEMA_CNT_RESET_LIMITr(unit, rport_handle_id, &val_on));
            reset = soc_reg_field_get(unit, E2EFCEMA_CNT_RESET_LIMITr, val_on, RESET_LIMITf);
            BCM_IF_ERROR_RETURN(READ_E2EFCEMA_CNT_DISC_LIMITr(unit, rport_handle_id, &val_drop));
            drop = soc_reg_field_get(unit, E2EFCEMA_CNT_DISC_LIMITr, val_drop, DISC_LIMITf);

            e2efc_rport_cfg->xoff_threshold_bytes = KT2_CELL2BYTE(set);
            e2efc_rport_cfg->xon_threshold_bytes = KT2_CELL2BYTE(reset);
            e2efc_rport_cfg->drop_threshold_bytes = KT2_CELL2BYTE(drop);
            break;
          default:
            break;
      }
      return BCM_E_NONE;
}

/*
 * Description:
 *      Add remote module-port and set configuration.
 *      Return a handle id for the bcm_port_e2efc_remote_port_xxx APIs.
 *      The API of remote module-port configuration is only for E2EFC
 *      transmission.
 * Parameter:
 *      unit            - (IN) unit number
 *      e2efc_rport_cfg - (IN) structure containing the configuration
 *      rport_handle_id - (OUT) remote port handle id, to be used for the rest
 *                        of bcm_port_e2efc_remote_port_xxx APIs
 */
int
bcmi_kt2_port_e2efc_remote_port_add(
    int unit,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg,
    int *rport_handle_id)
{
    uint32 val_p;
    int i, enable, rmod_ptr;

    if (e2efc_rport_cfg == NULL || rport_handle_id == NULL) {
        return BCM_E_PARAM;
    }

    /* Check the logical port is in the right range  */
    if (e2efc_rport_cfg->remote_port < 0 ||
        e2efc_rport_cfg->remote_port >= MAX_PORT(unit)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Port Range. Should be >= 0 && < %d\n"),
                  MAX_PORT(unit)));
        return BCM_E_PARAM;
    }

    /* Checking if the duplicated Counter is being set */
    for (i = 0; i < kt2_e2efc_remote_port_num(unit); i++) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, &val_p));
        if ((soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val_p, Vf))) {
            if(bcmi_kt2_e2efc_remote_module_ptr_get(
               unit, e2efc_rport_cfg->remote_module,
               &rmod_ptr) == BCM_E_NONE) {
               if (e2efc_rport_cfg->remote_port ==
                   (soc_reg_field_get(unit, E2EFC_CNT_ATTRr,
                                       val_p, RMT_SRC_PORT_IDf))) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                             (BSL_META_U(unit,
                             "Each remote source port should be"
                             " mapped to only one counter.\n")));
                    return BCM_E_FULL;
              }
            }
        }
    }

    enable = 0;
    for (i = 0; i < kt2_e2efc_remote_port_num(unit); i++) {
        /* check whether the E2EFC_CNT_ATTRr Vf is enabled */
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, &val_p));
        if (!(soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val_p, Vf))) {
            /* Setting remote port, module id and enable counter */
            enable = 1;
            soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p, Vf, enable);
            BCM_IF_ERROR_RETURN(
            bcmi_kt2_e2efc_remote_module_ptr_get(
                unit, e2efc_rport_cfg->remote_module, &rmod_ptr));
            soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                              RMOD_PTRf, rmod_ptr);

            soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                              RMT_SRC_PORT_IDf, e2efc_rport_cfg->remote_port);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_ATTRr,
                                              REG_PORT_ANY, i, val_p));
            BCM_IF_ERROR_RETURN(bcmi_kt2_port_e2efc_configure_thresholds(unit,
                                                         i, e2efc_rport_cfg));
            break;
        }
    }
    if (enable == 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                  "Can't Port Config Because the 48 entries is all full")));
        return BCM_E_FULL;
    }
    *rport_handle_id = i;

    return BCM_E_NONE;
}

/*
 * Description:
 *      Set (modify) configuration of the remote port.
 * Parameter:
 *      unit            - (IN) unit number
 *      rport_handle_id - (IN) remote port handle id
 *      e2efc_rport_cfg - (IN) structure containing the configuration
 */
int
bcmi_kt2_port_e2efc_remote_port_set(
    int unit,
    int rport_handle_id,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg)
{
    uint32 val_p;
    int rmod_ptr;

    /* check whehter the handle id is valid */
    if ( rport_handle_id < 0 || rport_handle_id >= kt2_e2efc_remote_port_num(unit)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Hadle ID.\n")));
        return BCM_E_PARAM;
    }

    if (e2efc_rport_cfg == NULL) {
        return BCM_E_PARAM;
    }

    /* Check the logical port is in the right range  */
    if (e2efc_rport_cfg->remote_port < 0 ||
        e2efc_rport_cfg->remote_port >= MAX_PORT(unit)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Port Range. Should be >= 0 && < %d\n"),
                  MAX_PORT(unit)));
        return BCM_E_PARAM;
    }
    /* check whether the parameter is added or not */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      E2EFC_CNT_ATTRr,
                                      REG_PORT_ANY, rport_handle_id, &val_p));
    if (!(soc_reg_field_get(unit, E2EFC_CNT_ATTRr,
                            val_p, Vf))) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "The port config is not added, thus can't set")));
        return BCM_E_PARAM;
    }

    /* Setting remote port and module id */
    BCM_IF_ERROR_RETURN(
    bcmi_kt2_e2efc_remote_module_ptr_get(
        unit, e2efc_rport_cfg->remote_module, &rmod_ptr));
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p, RMOD_PTRf, rmod_ptr);
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                      RMT_SRC_PORT_IDf, e2efc_rport_cfg->remote_port);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_ATTRr, REG_PORT_ANY,
                                      rport_handle_id, val_p));
    BCM_IF_ERROR_RETURN(bcmi_kt2_port_e2efc_configure_thresholds(
                                  unit, rport_handle_id,
                                  e2efc_rport_cfg));
    return BCM_E_NONE;
}


/*
 * Description:
 *      Get configuration of the remote port.
 *      Return the remote module, remote port and configuration of the
 *      handle id.
 * Parameter:
 *      unit            - (IN) unit number
 *      rport_handle_id - (IN) remote port handle id
 *      e2efc_rport_cfg - (OUT) structure containing the configuration
 */
int
bcmi_kt2_port_e2efc_remote_port_get(
    int unit,
    int rport_handle_id,
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg)
{
    uint32 val_p;
    int rmod_ptr, rmod_id;

    if (e2efc_rport_cfg == NULL) {
        return BCM_E_PARAM;
    }

    /* check whehter the handle id is valid */
    if ( rport_handle_id < 0 || rport_handle_id >= kt2_e2efc_remote_port_num(unit)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Hadle ID.\n")));
        return BCM_E_PARAM;
    }
    /* check whether the parameter is added or not */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      E2EFC_CNT_ATTRr,
                                      REG_PORT_ANY, rport_handle_id, &val_p));
    if (!(soc_reg_field_get(unit, E2EFC_CNT_ATTRr,
                            val_p, Vf))) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                  "The port config is not added, thus can't get")));
        return BCM_E_NOT_FOUND;
    }

    /* Getting remote port and module id */
    rmod_ptr = soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val_p, RMOD_PTRf);
    BCM_IF_ERROR_RETURN(
        bcmi_kt2_e2efc_remote_module_id_get(unit, rmod_ptr, &rmod_id));
    e2efc_rport_cfg->remote_module = rmod_id;
    e2efc_rport_cfg->remote_port =
    soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val_p,
                      RMT_SRC_PORT_IDf);
    BCM_IF_ERROR_RETURN(bcmi_kt2_port_e2efc_get_thresholds(unit,rport_handle_id,
                                                           e2efc_rport_cfg));
    return BCM_E_NONE;
}


/*
 * Description:
 *      Delete remote port.
 * Parameter:
 *      unit            - (IN) unit number
 *      rport_handle_id - (IN) remote port handle id
 */
int
bcmi_kt2_port_e2efc_remote_port_delete(
    int unit,
    int rport_handle_id)
{
    uint32 val_p;
    uint32 clear = 0x00000000;
    bcm_port_e2efc_remote_port_config_t e2efc_rport_cfg_s;
    bcm_port_e2efc_remote_port_config_t *e2efc_rport_cfg = &e2efc_rport_cfg_s;

    /* check whehter the handle id is valid */
    if ( rport_handle_id < 0 || rport_handle_id >= kt2_e2efc_remote_port_num(unit)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                  "Invalid Hadle ID.\n")));
        return BCM_E_PARAM;
    }
    /* check whether the parameter is added or not */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                 REG_PORT_ANY, rport_handle_id, &val_p));
    if (!(soc_reg_field_get(unit, E2EFC_CNT_ATTRr,
                            val_p, Vf))) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                  "The port config is not added, thus can't delete")));
        return BCM_E_NOT_FOUND;
    }

    /* Set to zero remote port and module id */
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                      Vf, clear);
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                      RMOD_PTRf, clear);
    soc_reg_field_set(unit, E2EFC_CNT_ATTRr, &val_p,
                      RMT_SRC_PORT_IDf, clear);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, E2EFC_CNT_ATTRr,
                                      REG_PORT_ANY, rport_handle_id, val_p));

    e2efc_rport_cfg->xoff_threshold_bytes = 0;
    e2efc_rport_cfg->xon_threshold_bytes = 0;
    e2efc_rport_cfg->drop_threshold_bytes = 0;
    e2efc_rport_cfg->xoff_threshold_packets = 0;
    e2efc_rport_cfg->xon_threshold_packets = 0;
    e2efc_rport_cfg->drop_threshold_packets = 0;

    e2efc_rport_cfg->e2efc_resource_type = BCM_E2EFC_RESOURCE_TYPE_INT_MEM;
    BCM_IF_ERROR_RETURN(bcmi_kt2_port_e2efc_configure_thresholds(unit,
                                        rport_handle_id, e2efc_rport_cfg));

    e2efc_rport_cfg->e2efc_resource_type = BCM_E2EFC_RESOURCE_TYPE_QEN;
    BCM_IF_ERROR_RETURN(bcmi_kt2_port_e2efc_configure_thresholds(unit,
                                        rport_handle_id, e2efc_rport_cfg));

    e2efc_rport_cfg->e2efc_resource_type = BCM_E2EFC_RESOURCE_TYPE_RQE;
    BCM_IF_ERROR_RETURN(bcmi_kt2_port_e2efc_configure_thresholds(unit,
                                        rport_handle_id, e2efc_rport_cfg));
#if defined(BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        return BCM_E_NONE;
    }
#endif
    e2efc_rport_cfg->e2efc_resource_type = BCM_E2EFC_RESOURCE_TYPE_EXT_MEM;
    BCM_IF_ERROR_RETURN(bcmi_kt2_port_e2efc_configure_thresholds(unit,
                                        rport_handle_id, e2efc_rport_cfg));

    e2efc_rport_cfg->e2efc_resource_type = BCM_E2EFC_RESOURCE_TYPE_EMA;
    BCM_IF_ERROR_RETURN(bcmi_kt2_port_e2efc_configure_thresholds(unit,
                                        rport_handle_id, e2efc_rport_cfg));
    return BCM_E_NONE;
}
/*
 * Description:
 *      Traverse enabled remote modules that apply E2EFC mechanism.
 * Parameter:
 *      unit      - (IN) unit number
 *      cb        - (IN) call back function to execute
 *      user_data - (IN) pointer to user data
*/
int
bcmi_kt2_port_e2efc_remote_module_traverse(
    int unit,
    bcm_port_e2efc_remote_module_traverse_cb cb,
    void *user_data)
{
    int i, rv, rmod_id;
    bcm_port_e2efc_mode_t mode;
    uint32 val;

    for (i = 0; i < KT2_E2EFC_RMOD_NUM; i++) {
        for (mode = bcmPortE2efcModeTx; mode < bcmPortE2efcModeCount; mode++) {
            soc_reg32_get(unit, e2efc_rmod_id_reg[mode][i],
                          REG_PORT_ANY, 0, &val);
            if (soc_reg_field_get(unit, e2efc_rmod_id_reg[mode][i],
                                  val, e2efc_rmod_enabled[i])) {
                rmod_id = soc_reg_field_get(unit, e2efc_rmod_id_reg[mode][i],
                                            val, e2efc_rmod_id_field[i]);
                rv = cb(unit, rmod_id, mode, user_data);
                if (rv != BCM_E_NONE) {
                    return rv;
                }
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Description:
 *      Traverse remote ports.
 * Parameter:
 *      unit      - (IN) unit number
 *      cb        - (IN) call back function to execute
 *      user_data - (IN) pointer to user data
 */
int
bcmi_kt2_port_e2efc_remote_port_traverse(
    int unit,
    bcm_port_e2efc_remote_port_traverse_cb cb,
    void *user_data)
{
    int i, rv;
    uint32 val;

    for (i = 0; i < kt2_e2efc_remote_port_num(unit); i++) {
        /* check whether the E2EFC_CNT_ATTRr Vf is enabled */
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, E2EFC_CNT_ATTRr,
                                          REG_PORT_ANY, i, &val));
        if (soc_reg_field_get(unit, E2EFC_CNT_ATTRr, val, Vf)) {
            rv = cb(unit, i, user_data);
            if (rv != BCM_E_NONE) {
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_kt2_switch_control_set
 * Description:
 *      Specify general switch behaviors.
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to modify
 *      arg - The value with which to set the parameter
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      For behaviors which are port-specific, all non-stack ports
 *      will be set.
 */

int
bcm_kt2_switch_control_set(int unit,
                           bcm_switch_control_t type,
                           int arg)
{
    switch (type) {
        case bcmSwitchE2EFCReceiveDestMacOui:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_LSr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_DA_LSr,
                                         val, DAf);
                fval = (fval & 0xffffff) | (arg & 0xff) << 24;
                soc_reg_field_set(unit, E2E_IBP_RX_DA_LSr,
                                  &val, DAf, fval);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_DA_LSr(unit, val));
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_MSr(unit, &val));
                fval = (arg >> 8) & 0xffff;
                soc_reg_field_set(unit, E2E_IBP_RX_DA_MSr,
                                  &val, DAf, fval);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_DA_MSr(unit, val));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveDestMacNonOui:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_LSr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_DA_LSr,
                                         val, DAf);
                fval = (fval & 0xff000000) | (arg & 0xffffff);
                soc_reg_field_set(unit, E2E_IBP_RX_DA_LSr,
                                  &val, DAf, fval);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_DA_LSr(unit, val));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveEtherType:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_LENGTH_TYPEr(unit, &val));
                arg = (arg & 0xffff);
                soc_reg_field_set(unit, E2E_IBP_RX_LENGTH_TYPEr,
                                  &val, LENGTH_TYPEf, arg);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_LENGTH_TYPEr(unit, val));
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveOpcode:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_OPCODEr(unit, &val));
                arg = (arg & 0xffff);
                soc_reg_field_set(unit, E2E_IBP_RX_OPCODEr,
                                  &val, OPCODEf, arg);
                SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_OPCODEr(unit, val));
                return BCM_E_NONE;
            }
            break;
        default:
            break;
    }
    return BCM_E_PARAM;
}
/*
 * Function:
 *      bcm_kt2_switch_control_get
 * Description:
 *      Retrieve general switch behaviors.
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      For behaviors which are port-specific, retrieves the setting
 *      for one port arbitrarily chosen from those which support the
 *      setting and are non-stack.
 */

int
bcm_kt2_switch_control_get(int unit,
                           bcm_switch_control_t type,
                           int *arg)
{
    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    switch (type) {
        case bcmSwitchE2EFCReceiveDestMacOui:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, f_lo, f_hi;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_LSr(unit, &val));
                f_lo = soc_reg_field_get(unit, E2E_IBP_RX_DA_LSr,
                                         val, DAf);
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_MSr(unit, &val));
                f_hi = soc_reg_field_get(unit, E2E_IBP_RX_DA_MSr,
                                         val, DAf);
                *arg = (f_hi & 0xffff) << 8 | ((f_lo >> 24) & 0xff);
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveDestMacNonOui:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_DA_LSr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_DA_LSr,
                                         val, DAf);
                *arg = fval & 0Xffffff;
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveEtherType:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_LENGTH_TYPEr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_LENGTH_TYPEr,
                                         val, LENGTH_TYPEf);
                *arg = fval & 0xffff;
                return BCM_E_NONE;
            }
            break;
        case bcmSwitchE2EFCReceiveOpcode:
            if (soc_feature(unit, soc_feature_e2efc)) {
                uint32 val, fval;
                SOC_IF_ERROR_RETURN(READ_E2E_IBP_RX_OPCODEr(unit, &val));
                fval = soc_reg_field_get(unit, E2E_IBP_RX_OPCODEr,
                                         val, OPCODEf);
                *arg = fval & 0xffff;
                return BCM_E_NONE;
            }
            break;


        default:
            break;
    }
    return BCM_E_PARAM;
}
/************ E2EFC CODE END *************************/
#endif /* BCM_KATANA2_SUPPORT */
