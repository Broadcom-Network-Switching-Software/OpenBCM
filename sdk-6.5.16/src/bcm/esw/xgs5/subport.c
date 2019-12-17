/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        subport.c
 * Purpose:     XGS5 Subport CoE common driver.
 */

#include <soc/defs.h>

#include <shared/bsl.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT)

#include <soc/drv.h>
#include <soc/scache_dictionary.h>
#include <soc/debug.h>
#include <soc/trident2.h>
#include <soc/td2_td2p.h>
#include <soc/bondoptions.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#define BCMI_MAX_SHORT_VALUE                 65535

/*
 * SUBPORT HW Definition Table
 */
static bcmi_xgs5_subport_coe_hw_defs_t  *bcmi_xgs5_subport_coe_hw_defs[BCM_MAX_NUM_UNITS] = {0};

#define SUBPORT_COE_HW(u_)    (bcmi_xgs5_subport_coe_hw_defs[(u_)])

#define SUBPORT_COE_HW_ING_PORT(u_)  \
    (bcmi_xgs5_subport_coe_hw_defs[(u_)]->igr_port)
#define SUBPORT_COE_HW_EGR_PORT(u_)  \
    (bcmi_xgs5_subport_coe_hw_defs[(u_)]->egr_port)
#define SUBPORT_COE_HW_MODPORT_MAP_SUBPORT(u_)  \
    (bcmi_xgs5_subport_coe_hw_defs[(u_)]->modport_map_subport)
#define SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(u_)  \
    (bcmi_xgs5_subport_coe_hw_defs[(u_)]->subport_tag_sgpp)
#define SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P(u_)  \
    (bcmi_xgs5_subport_coe_hw_defs[(u_)]->egr_subport_tag_dot1p)

#define SUBPORT_COE_HW_ING_PORT_MEM(u_)  \
    (SUBPORT_COE_HW_ING_PORT(u_)->mem)
#define SUBPORT_COE_HW_EGR_PORT_MEM(u_)  \
    (SUBPORT_COE_HW_EGR_PORT(u_)->mem)
#define SUBPORT_COE_HW_MODPORT_MAP_SUBPORT_MEM(u_)  \
    (SUBPORT_COE_HW_MODPORT_MAP_SUBPORT(u_)->mem)
#define SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP_MEM(u_)  \
    (SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(u_)->mem)
#define SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P_MEM(u_) \
    (SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P(u_)->mem)

#define BCM_FAILURE_CLEANUP(rv, label) \
    do { if (SOC_FAILURE(rv)) { goto label; } } while(0)


STATIC int _bcm_subport_coe_port_count[BCM_MAX_NUM_UNITS] = { 0 };

STATIC int
bcmi_xgs5_subtag_subport_init(int unit)
{
    bcm_port_t port;
    int num_vlan = 0, index = 0;
    soc_info_t *si = &SOC_INFO(unit);
    BCM_IF_ERROR_RETURN
        (soc_mem_clear(unit, SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP_MEM(unit),
               COPYNO_ALL, TRUE));
    num_vlan = soc_mem_index_count(unit, VLAN_TABm);

    BCM_PBMP_ITER(si->subtag_allowed_pbm, port) {

        if (port < SOC_MAX_NUM_PORTS) {

            /* SubTag VLAN ID bitmap to keep track of the VLAN ID used per
             * SubTag enabled port.
             */
            if (_bcm_subtag_vlan_id_bitmap[unit][port] == NULL) {

                _bcm_subtag_vlan_id_bitmap[unit][port] =
                    sal_alloc(SHR_BITALLOCSIZE(num_vlan),
                            "_bcm_subtag_vlan_id_bitmap");

                if (_bcm_subtag_vlan_id_bitmap[unit][port] == NULL) {
                    LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR:_bcm_subtag_vlan_id_bitmap "
                                " alloc failed\n")));

                    return BCM_E_MEMORY;
                }
            }
            sal_memset(_bcm_subtag_vlan_id_bitmap[unit][port], 0,
                    SHR_BITALLOCSIZE(num_vlan));
        }
    }

    /* SubTag subport group bitmap to keep track of used SubTag groups */
    if (_bcm_subtag_group_bitmap[unit] == NULL) {

        _bcm_subtag_group_bitmap[unit] = 
             sal_alloc(SHR_BITALLOCSIZE(si->max_subport_coe_groups),
             "subtag_subport_group_bitmap");

        if (_bcm_subtag_group_bitmap[unit] == NULL) {
             LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR:subtag group bitmap "
                                       "alloc failed\n")));

             return BCM_E_MEMORY;
        }
    }

    sal_memset(_bcm_subtag_group_bitmap[unit], 0,
        SHR_BITALLOCSIZE(si->max_subport_coe_groups));

    /* SubTag subport port info per subport group per subport port */
    if (_bcm_subtag_subport_port_info[unit] == NULL) {
        _bcm_subtag_subport_port_info[unit] =
            sal_alloc((si->max_subport_coe_ports *
                       sizeof(_bcm_subtag_subport_port_info_t)),
                       "subtag_subport_port_info");
        if (_bcm_subtag_subport_port_info[unit] == NULL) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: "
                    " subtag_subport_port_info alloc failed\n")));

            return BCM_E_MEMORY;
        }
    }

    for (index = 0; index < si->max_subport_coe_ports; index++) {
        _bcm_subtag_subport_port_info[unit][index].group = BCM_GPORT_INVALID;
        _bcm_subtag_subport_port_info[unit][index].vlan = BCM_VLAN_INVALID;
        _bcm_subtag_subport_port_info[unit][index].subtag_tcam_hw_idx = -1;
        _bcm_subtag_subport_port_info[unit][index].subport_modport = -1; 
        _bcm_subtag_subport_port_info[unit][index].subport_port = -1;
        _bcm_subtag_subport_port_info[unit][index].nh_index = -1;
        _bcm_subtag_subport_port_info[unit][index].valid = 0;
        _bcm_subtag_subport_port_info[unit][index].modport_subport_idx = 0;
        _bcm_subtag_subport_port_info[unit][index].phb_valid = 0;
        _bcm_subtag_subport_port_info[unit][index].priority = 0;
        _bcm_subtag_subport_port_info[unit][index].color = 0;
    }
    /* Set the Ingress/Egress EtherTypes to a specific default */
    BCM_IF_ERROR_RETURN(bcmi_xgs5_subport_coe_ether_type_size_set(
                                unit, bcmSwitchSubportCoEEtherType,
                                BCM_SUBPORT_COE_DEFAULT_ETHERTYPE));
    BCM_IF_ERROR_RETURN(bcmi_xgs5_subport_coe_ether_type_size_set(
                                unit, bcmSwitchSubportEgressTpid,
                                BCM_SUBPORT_COE_DEFAULT_ETHERTYPE)); 
    BCM_IF_ERROR_RETURN(bcmi_xgs5_subport_coe_ether_type_size_set(
                                unit, bcmSwitchSubportEgressWideTpid,
                                BCM_SUBPORT_COE_DEFAULT_ETHERTYPE)); 
    return BCM_E_NONE;
}

STATIC void
bcmi_xgs5_subport_free_resource(int unit)
{
    bcm_port_t port;

    if(_bcm_subport_group_bitmap[unit] != NULL) {
        sal_free(_bcm_subport_group_bitmap[unit]);
        _bcm_subport_group_bitmap[unit] = NULL;
    }

    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {

        if(_bcm_subtag_group_bitmap[unit] != NULL) {
            sal_free(_bcm_subtag_group_bitmap[unit]);
            _bcm_subtag_group_bitmap[unit] = NULL;
        }

        if (_bcm_subtag_subport_port_info[unit] != NULL) {
            sal_free(_bcm_subtag_subport_port_info[unit]);
            _bcm_subtag_subport_port_info[unit] = NULL;
        }

        if (BCM_PBMP_NOT_NULL(SOC_INFO(unit).subtag_allowed_pbm)) {
            BCM_PBMP_ITER(SOC_INFO(unit).subtag_allowed_pbm, port) {
                /* check to fix coverity static overrun error */
                if (port < SOC_MAX_NUM_PORTS) {
                    if (_bcm_subtag_vlan_id_bitmap[unit][port] != NULL) {
                        sal_free(_bcm_subtag_vlan_id_bitmap[unit][port]);
                        _bcm_subtag_vlan_id_bitmap[unit][port] = NULL;
                    }
                }
            }
        }

    } /* end of  soc_feature_hgproxy_subtag_coe */

    if (_bcm_subport_group_subport_port_count[unit] != NULL) {
        sal_free(_bcm_subport_group_subport_port_count[unit]);
        _bcm_subport_group_subport_port_count[unit] = NULL;
    }

    if (_bcm_subport_mutex[unit] != NULL) {
        sal_mutex_destroy(_bcm_subport_mutex[unit]);
        _bcm_subport_mutex[unit] = NULL;
    }

}

/*
 * Function:
 *      bcmi_xgs5_subport_init
 * Purpose:
 *      Initialize the Subport subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *         int  - Result of init
 */
int
bcmi_xgs5_subport_init(int unit,
                       bcm_esw_subport_drv_t *drv,
                       bcmi_xgs5_subport_coe_hw_defs_t *hw_defs)
{
    int rv = BCM_E_NONE;

#ifdef BCM_HGPROXY_COE_SUPPORT 
    soc_info_t *si = &SOC_INFO(unit);
#endif

/* Do this only for the Newer CoE implementations */
#ifdef BCM_HGPROXY_COE_SUPPORT 

    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {

        /* Subport group bitmap to keep track of used subport groups */
        if (_bcm_subport_group_bitmap[unit] == NULL) {

            _bcm_subport_group_bitmap[unit] = 
                sal_alloc(SHR_BITALLOCSIZE(si->max_subport_coe_groups),
                "subport_group_bitmap");

            if (_bcm_subport_group_bitmap[unit] == NULL) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                          (BSL_META_U(unit,"ERROR:subport_init: group bitmap "
                                           "alloc failed\n")));

                bcmi_xgs5_subport_free_resource(unit);

                return BCM_E_MEMORY;
            }

        }

        sal_memset(_bcm_subport_group_bitmap[unit], 0,
            SHR_BITALLOCSIZE(si->max_subport_coe_groups));

        _bcm_subport_group_count[unit] = 0;

        /* Keep Count of subport port created per subport group */
        if (_bcm_subport_group_subport_port_count[unit] == NULL) {
            _bcm_subport_group_subport_port_count[unit] =
                sal_alloc((si->max_subport_coe_groups * sizeof(int)),
                            "subport_group_subport_port_count");
            if (_bcm_subport_group_subport_port_count[unit] == NULL) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                          (BSL_META_U(unit,"ERROR: "
                    "subport_group_subport_port_count alloc failed\n")));

                return BCM_E_MEMORY;
            }
        }

        sal_memset(_bcm_subport_group_subport_port_count[unit], 0,
            (si->max_subport_coe_groups * sizeof(int)));

        /* Subtag CoE subport initialization */

        if(hw_defs) {
           SUBPORT_COE_HW(unit) = hw_defs;
        }
        if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
            BCM_PBMP_NOT_NULL(si->subtag_allowed_pbm)) {
            rv = bcmi_xgs5_subtag_subport_init(unit);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                          (BSL_META_U(unit,"ERROR: SubTag init failed.\n")));

                bcmi_xgs5_subport_free_resource(unit);
                return rv;
            }
        }

        _bcm_subport_coe_port_count[unit] = 0;

        /* Create subport mutex */
        if (NULL == _bcm_subport_mutex[unit]) {
            _bcm_subport_mutex[unit] =
                sal_mutex_create("subtag subport mutex");

            if (_bcm_subport_mutex[unit] == NULL) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                          (BSL_META_U(unit,"ERROR:subtag subport mutex "
                                           "create failed\n")));

                return BCM_E_MEMORY;
            }
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if(SOC_WARM_BOOT(unit)) {
        rv = _bcmi_xgs5_subport_reinit(unit);
        if (rv != BCM_E_NONE) {
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */


#endif

    /* Install Tables */
    /* If a driver is not assigned already, assign the driver */
    if(BCM_ESW_SUBPORT_DRV(unit) == NULL) {
        BCM_ESW_SUBPORT_DRV(unit) = drv;
    }

    return BCM_E_NONE;
}

int _bcmi_xgs5_subport_reinit(int unit)
{
     
    return 0;
}

int _bcm_coe_subtag_subport_port_get(int unit, 
                                     bcm_gport_t subport_port_gport,
                                     bcm_subport_config_t *config)
{
    bcm_gport_t subport_group;
    int i = 0;
    soc_info_t *si = &SOC_INFO(unit);

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if(_bcm_subtag_subport_port_info[unit] == NULL) {
        return BCM_E_NOT_FOUND;
    }

    /* Check if the passed in port is of 'subtag' type */
    if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport_port_gport)) {

        /* Find the entry corresponding to the subport */
        for (i=0; i < si->max_subport_coe_ports; i++) {
            if((_bcm_subtag_subport_port_info[unit][i].valid) && 
                (_bcm_subtag_subport_port_info[unit][i].subport_port == 
                 subport_port_gport)) {
                    /* Entry found, break */
                    break;
                }
        }

        /* Flag error if subport not found */
        if(i == si->max_subport_coe_ports) {
            LOG_VERBOSE(BSL_LS_BCM_SUBPORT,
                        (BSL_META_U(unit, "ERROR:Invalid SubTag subport"
                                          " port gport 0x%x\n"),
                                    subport_port_gport));
            return BCM_E_NOT_FOUND;
        }

        /* Get the group id and ensure it is valid */
        subport_group = _bcm_subtag_subport_port_info[unit][i].group;

        if (!BCM_GPORT_IS_SUBPORT_GROUP(subport_group)) {
            LOG_VERBOSE(BSL_LS_BCM_SUBPORT,
                        (BSL_META_U(unit, "ERROR:Invalid SubTag subport"
                                          " group gport 0x%x\n"),
                subport_port_gport));
            return BCM_E_PARAM;
        }

        /* Fill in needed data */
        config->group = subport_group;
        config->pkt_vlan =
            _bcm_subtag_subport_port_info[unit][i].vlan;  
        /* Put togethe the CoE (mod,port) associated with this subtag */
        config->subport_modport = 
            _bcm_subtag_subport_port_info[unit][i].subport_modport;

        /* Fill in the PHB data */
        if(_bcm_subtag_subport_port_info[unit][i].phb_valid) {
            config->prop_flags |= BCM_SUBPORT_PROPERTY_PHB;
            config->int_pri = 
                _bcm_subtag_subport_port_info[unit][i].priority;
            config->color = 
                _bcm_subtag_subport_port_info[unit][i].color;
        } else {
            config->prop_flags &= ~BCM_SUBPORT_PROPERTY_PHB;
            config->int_pri = 0;
            config->color = 0;
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


int bcmi_xgs5_subport_coe_port_get(int unit, 
                                   bcm_gport_t subport_port_gport,
                                   bcm_subport_config_t *config)
{
    int rv = BCM_E_NONE;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (!BCM_GPORT_IS_SUBPORT_PORT(subport_port_gport)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: gport 0x%x is not "
                                       "subport port gport\n"),
                                  subport_port_gport));                                
            return BCM_E_PARAM;
    }

    _BCM_SUBPORT_COE_LOCK(unit);

    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) && 
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport_port_gport)) {
            rv = _bcm_coe_subtag_subport_port_get(unit, 
                         subport_port_gport, config);
    }

    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_SUBPORT,
                        (BSL_META_U(unit, "ERROR: Subport port 0x%x "
                                          "get failed. rv=%d(%s)\n"),
                                    subport_port_gport, rv, soc_errmsg(rv)));
        _BCM_SUBPORT_COE_UNLOCK(unit);
        return rv;
    }

    _BCM_SUBPORT_COE_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_coe_subtag_subport_default_lport_entry_set
 * Purpose:
 *      Add a deafult lport entry for this Source Trunk Map entry
 * Parameters:
 *      unit   - (IN) Device Number
 *      port   - (IN) Physical port which this coe port belong to
 *      stm_index - (IN) Source Trunk Map Index to update the LPORT at
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_coe_subtag_subport_default_lport_entry_set(int unit, bcm_port_t port,
                                                int stm_index)
{
    source_trunk_map_table_entry_t  stm_entry;
    lport_tab_entry_t               lport_entry;
    void                           *entries[1];
    uint32                          lport_index;
    int                             key_type_value = 0;
    int                             value;
    int                             rv = BCM_E_NONE;

    sal_memcpy(&lport_entry, soc_mem_entry_null(unit, LPORT_TABm),
               soc_mem_entry_words(unit, LPORT_TABm) * sizeof(uint32));

    /* Inherrit the default properties from its cascaded port*/

    /* Enable outer_tpid_enable */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, OUTER_TPID_ENABLEf)) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, OUTER_TPID_ENABLEf, 1);
    }

    /* Switching VLAN. */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, OUTER_TPIDf)) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, OUTER_TPIDf, 0x8100);
    }

    /* Set default  vlan id(pvid) for port. */
    soc_LPORT_TABm_field32_set(unit, &lport_entry, PORT_VIDf, BCM_VLAN_DEFAULT);

    /* Enable mac based vlan classification. */
    soc_LPORT_TABm_field32_set(unit, &lport_entry, MAC_BASED_VID_ENABLEf, 1);

    /* Enable subnet based vlan classification. */
    soc_LPORT_TABm_field32_set(unit, &lport_entry, SUBNET_BASED_VID_ENABLEf, 1);

    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, TRUST_INCOMING_VIDf)) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, TRUST_INCOMING_VIDf, 1);

        /* Set identify mapping for pri/cfi re-mapping */
        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, PRI_MAPPINGf)) {
            soc_LPORT_TABm_field32_set(unit, &lport_entry, PRI_MAPPINGf, 0xfac688);
            soc_LPORT_TABm_field32_set(unit, &lport_entry, CFI_0_MAPPINGf, 0);
            soc_LPORT_TABm_field32_set(unit, &lport_entry, CFI_1_MAPPINGf, 1);
        }

        /* Set identify mapping for ipri/icfi re-mapping */
        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, IPRI_MAPPINGf)) {
            soc_LPORT_TABm_field32_set(unit, &lport_entry, IPRI_MAPPINGf, 0xfac688);
            soc_LPORT_TABm_field32_set(unit, &lport_entry, ICFI_0_MAPPINGf, 0);
            soc_LPORT_TABm_field32_set(unit, &lport_entry, ICFI_1_MAPPINGf, 1);
        }

        /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, CML_FLAGS_NEWf)) {
            soc_LPORT_TABm_field32_set(unit, &lport_entry, CML_FLAGS_NEWf, 0x8);
            soc_LPORT_TABm_field32_set(unit, &lport_entry, CML_FLAGS_MOVEf, 0x8);
        }
        /* Set first VLAN_XLATE key-type to {outer,port}.
         * Set second VLAN_XLATE key-type to {inner,port}.
         */
        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, VT_KEY_TYPEf)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_pt_vtkey_type_value_get(unit,
                         VLXLT_HASH_KEY_TYPE_OVID, &key_type_value));
            soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_KEY_TYPEf,
                                      key_type_value);
        }

        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, VT_KEY_TYPE_USE_GLPf)) {
            soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_KEY_TYPE_USE_GLPf, 1);
        }
        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, VT_PORT_TYPE_SELECTf)) {
            soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_PORT_TYPE_SELECTf, 1);
        }
        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, VT_KEY_TYPE_2f)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_pt_vtkey_type_value_get(unit,
                         VLXLT_HASH_KEY_TYPE_IVID, &key_type_value));
            soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_KEY_TYPE_2f,
                                      key_type_value);
        }
        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, VT_KEY_TYPE_2_USE_GLPf)) {
            soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_KEY_TYPE_2_USE_GLPf, 1);
        }
        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, VT_PORT_TYPE_SELECT_2f)) {
            soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_PORT_TYPE_SELECT_2f, 1);
        }

        /* Trust the outer tag pri/cfi (to be backwards compatible) */
        if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, TRUST_OUTER_DOT1Pf)) {
            soc_LPORT_TABm_field32_set(unit, &lport_entry, TRUST_OUTER_DOT1Pf, 1);
        }
    }

    /* Enable IFP. */
    soc_LPORT_TABm_field32_set(unit, &lport_entry, FILTER_ENABLEf, 1);

    /* Enable VFP. */
    soc_LPORT_TABm_field32_set(unit, &lport_entry, VFP_ENABLEf, 1);

    /* Set port field select index to PFS index max - 1. */
    if(SOC_MEM_FIELD_VALID(unit, LPORT_TABm, FP_PORT_FIELD_SEL_INDEXf)) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, FP_PORT_FIELD_SEL_INDEXf,
                                   (soc_mem_index_max(unit, FP_PORT_FIELD_SELm)
                                    - 1));
    }

    /* Enable V4 L3 termination */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, V4L3_ENABLEf)) {
        BCM_IF_ERROR_RETURN(
         _bcm_esw_port_tab_get(unit, port, V4L3_ENABLEf, &value));

        soc_LPORT_TABm_field32_set(unit, &lport_entry, V4L3_ENABLEf, value);
    }

    /* Enable V6 L3 termination */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, V6L3_ENABLEf)) {
        BCM_IF_ERROR_RETURN(
         _bcm_esw_port_tab_get(unit, port, V6L3_ENABLEf, &value));
        soc_LPORT_TABm_field32_set(unit, &lport_entry, V6L3_ENABLEf, value);
    }

    /* Enable V4 IPMC */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, V4IPMC_ENABLEf)) {
        BCM_IF_ERROR_RETURN(
         _bcm_esw_port_tab_get(unit, port, V4IPMC_ENABLEf, &value));
        soc_LPORT_TABm_field32_set(unit, &lport_entry, V4IPMC_ENABLEf, value);
    }

    /* Enable V6 IPMC */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, V6IPMC_ENABLEf)) {
        BCM_IF_ERROR_RETURN(
         _bcm_esw_port_tab_get(unit, port, V6IPMC_ENABLEf, &value));
        soc_LPORT_TABm_field32_set(unit, &lport_entry, V6IPMC_ENABLEf, value);
    }

    /* IPMC DO VLAN enable */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, IPMC_DO_VLANf)) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_port_tab_get(unit, port, IPMC_DO_VLANf, &value));
        soc_LPORT_TABm_field32_set(unit, &lport_entry, IPMC_DO_VLANf, value);
    }

    /* Enable MPLS */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, MPLS_ENABLEf)) {
        BCM_IF_ERROR_RETURN(
        _bcm_esw_port_tab_get(unit, port, MPLS_ENABLEf, &value));
        soc_LPORT_TABm_field32_set(unit, &lport_entry, MPLS_ENABLEf, value);
    }

    if (SOC_IS_TRIDENT3X(unit)) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, PRI_MAPPINGf, 0xfac688);
        soc_LPORT_TABm_field32_set(unit, &lport_entry, CFI_0_MAPPINGf, 0);
        soc_LPORT_TABm_field32_set(unit, &lport_entry, CFI_1_MAPPINGf, 1);
        soc_LPORT_TABm_field32_set(unit, &lport_entry, IPRI_MAPPINGf, 0xfac688);
        soc_LPORT_TABm_field32_set(unit, &lport_entry, ICFI_0_MAPPINGf, 0);
        soc_LPORT_TABm_field32_set(unit, &lport_entry, ICFI_1_MAPPINGf, 1);
        soc_LPORT_TABm_field32_set(unit, &lport_entry, CML_FLAGS_NEWf, 0x8);
        soc_LPORT_TABm_field32_set(unit, &lport_entry, CML_FLAGS_MOVEf, 0x8);
        soc_LPORT_TABm_field32_set(unit, &lport_entry, TAG_ACTION_PROFILE_PTRf, 0x1);

        BCM_IF_ERROR_RETURN
            (_bcm_esw_pt_vtkey_type_value_get(unit,
                      VLXLT_HASH_KEY_TYPE_OVID, &key_type_value));
        soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_KEY_TYPEf,
                                  key_type_value);

        soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_PORT_TYPE_SELECT_1f, 1);

        BCM_IF_ERROR_RETURN
            (_bcm_esw_pt_vtkey_type_value_get(unit,
                   VLXLT_HASH_KEY_TYPE_IVID, &key_type_value));
        soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_KEY_TYPE_2f,
                                key_type_value);

        soc_LPORT_TABm_field32_set(unit, &lport_entry, VT_PORT_TYPE_SELECT_2f, 1);
    }

    /* Add LPORT default entry */
    entries[0] = &lport_entry;
    BCM_IF_ERROR_RETURN(
        _bcm_lport_ind_profile_entry_add(unit, entries, 1, &lport_index));

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);

    /* Get the current entry programmed at the specified index */
    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                      stm_index, &stm_entry);

    if(BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    /* Update STM LPORT profile index  */
    soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &stm_entry,
                                            LPORT_PROFILE_IDXf,
                                            lport_index);

    if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, OUTER_TPID_ENABLEf)) {
        soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &stm_entry,
                                                 OUTER_TPID_ENABLEf, 1);
    }
    /* Write the data back */
    rv = WRITE_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                       stm_index, &stm_entry);

    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);

    return rv;
}

STATIC int
_bcm_coe_subtag_subport_default_egr_lport_entry_set(int unit, int port)
{
    uint32 values[] = {0, 1, 1, 0};
    soc_field_t fields[] = {
        EN_EFILTERf,
        EM_SRCMOD_CHANGEf,
        EFP_FILTER_ENABLEf,
        EGR_QOS_PROFILE_INDEXf
    };

    BCM_IF_ERROR_RETURN(
        bcm_esw_port_egr_lport_fields_set(
            unit, port, EGR_LPORT_PROFILEm, 4, fields, values));

    if (SOC_MEM_FIELD_VALID(unit, EGR_PORT_1m, OUTER_TPID_ENABLEf)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_egr_lport_field_set(
                unit, port, EGR_PORT_1m, OUTER_TPID_ENABLEf, 1));

    }
    if (SOC_MEM_FIELD_VALID(unit, EGR_LPORT_PROFILEm, VXLT_CTRL_IDf)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_egr_lport_field_set(
                unit, port, EGR_LPORT_PROFILEm, VXLT_CTRL_IDf, 1));

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_coe_subtag_subport_lport_entry_delete
 * Purpose:
 *      Delete the lport profile for this Source Trunk Map entry
 * Parameters:
 *      unit   - (IN) Device Number
 *      stm_index - (IN) Source Trunk Map Index to update the LPORT at
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_coe_subtag_subport_lport_entry_delete(int unit, int stm_index)
{
    source_trunk_map_table_entry_t  stm_entry;
    uint32                          lport_index;

    /* Read the entry in question */
    SOC_IF_ERROR_RETURN
        (READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                      stm_index, &stm_entry)); 

    /* Extract the lport index from the entry */
    lport_index =
        soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &stm_entry,
                                                LPORT_PROFILE_IDXf);

    /* Delete the profile entry for this index*/
    BCM_IF_ERROR_RETURN(
        _bcm_lport_ind_profile_entry_delete(unit, lport_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_coe_subtag_subport_port_delete
 * Purpose:
 *      Add a subport to a subport group
 * Parameters:
 *      unit   - (IN) Device Number
 *      subport_port_gport - (IN) Subport to delete
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_coe_subtag_subport_port_delete(int unit, bcm_gport_t subport)
{
    int rv = BCM_E_NONE, group_id = 0, idx = 0;
    int port = 0, subport_group = -1;
    int subtag_to_subport_port_tab_idx = -1;
    bcm_vlan_t vlan_id;
    soc_mem_t mem;
    subport_tag_sgpp_map_entry_t subport_tag_to_sgpp_port_map_entry;
    soc_info_t *si = &SOC_INFO(unit);

    if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: gport 0x%x is not SubTag "
                                       "subport port type.\n"),
                                  subport));
        return BCM_E_PARAM;
    }

    for (idx=0; idx < si->max_subport_coe_ports; idx++) {
        if(_bcm_subtag_subport_port_info[unit][idx].subport_port ==
            subport) {
            break;
        }
    }

    if(idx >= si->max_subport_coe_ports) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport_gport 0x%x "
                                       "not configured.\n"),
                                  subport));
        return BCM_E_NOT_FOUND;
    }

    if (!_BCM_SUBPORT_COE_SUBTAG_PORT_INFO_VALID_GET(unit, idx)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport port "
                                       "gport 0x%x is invalid\n"),
                                  subport));
        return BCM_E_NOT_FOUND;
    }

    /* Extract the associated group */
    subport_group =
        _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_GROUP_GET(unit, idx);

    /* Extract the associated front panel port */
    port = _BCM_SUBPORT_COE_GROUP_PORT_GET(subport_group);

    /* Get the idx associated with 'SUBPORT_TAG_SGPP_MAPm' entry */
    subtag_to_subport_port_tab_idx =
        _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_TCAM_HW_ID_GET(unit, idx);

    mem = SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP_MEM(unit);
    if (subtag_to_subport_port_tab_idx < 0 ||
        subtag_to_subport_port_tab_idx > soc_mem_index_max(unit,mem)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport port gport "
                                       "0x%x is not found.\n"
                                       "subtag_to_pp_port  tcam idx = %d\n")
                                       ,subport, 
                                        subtag_to_subport_port_tab_idx));
        return BCM_E_NOT_FOUND;
    }

    /* Get the associated VLAN */
    vlan_id = _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_VLAN_GET(unit, idx) &
              0xFFF;

    /* Make sure the VLAN is indeed configured */
    if (!_BCM_COE_SUBTAG_VLAN_ID_USED_GET(unit, port, vlan_id)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport port gport "
                                       "0x%x is not found.\n"
                                        "VLAN id = %d is not set for "
                                        "port %d\n"),
                                        subport, vlan_id, port));
        return BCM_E_NOT_FOUND;
    }

    /*
    * Clean port, VLAN ID to subport mapping
    */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, subtag_to_subport_port_tab_idx,
             &subport_tag_to_sgpp_port_map_entry);
    soc_mem_field32_set(unit, mem,
                        (uint32 *)&subport_tag_to_sgpp_port_map_entry,
                        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->subport_tag,
                        0);
    soc_mem_field32_set(unit, mem,
                        (uint32 *)&subport_tag_to_sgpp_port_map_entry,
                        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->valid, 0);

    /* Clean PHB for the subport */
    soc_mem_field32_set(unit, mem,
                        (uint32 *)&subport_tag_to_sgpp_port_map_entry,
                       SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->phb_enable, 0);
    soc_mem_field32_set(unit, mem,
                        (uint32 *)&subport_tag_to_sgpp_port_map_entry,
                        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->int_pri, 0);
    soc_mem_field32_set(unit, mem, 
                        (uint32 *)&subport_tag_to_sgpp_port_map_entry,
                        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->cng, 0);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, 
                         subtag_to_subport_port_tab_idx,
                         &subport_tag_to_sgpp_port_map_entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport_tag %d to sgpp %d "
                                  "map failed.\n"), vlan_id,
                                             subtag_to_subport_port_tab_idx));
        return rv;
    }

    /*
     * Clean SGPP to Physical port mapping
     */

    mem = SUBPORT_COE_HW_MODPORT_MAP_SUBPORT_MEM(unit);
    rv = soc_mem_field32_modify(unit, mem, 
             _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_MODPORT_IDX_GET(unit, idx),
             SUBPORT_COE_HW_MODPORT_MAP_SUBPORT(unit)->dest, 0);

    rv = soc_mem_field32_modify(unit, mem, 
             _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_MODPORT_IDX_GET(unit, idx),
             SUBPORT_COE_HW_MODPORT_MAP_SUBPORT(unit)->enable, 0);

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                    (BSL_META_U(unit,"ERROR: Cleaning mod_port to subport id "
                                       "%d to physical port "
                                       "map failed.\n"), subport));
        return rv;
    }

#if defined(INCLUDE_L3)
    /* Adjust the encoded offset and delete the allocated next-hop */
    if(_bcm_subtag_subport_port_info[unit][idx].nh_index != -1) {
        BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0,
            _bcm_subtag_subport_port_info[unit][idx].nh_index - 
            BCM_XGS3_DVP_EGRESS_IDX_MIN(unit)));
    }
#endif

    BCM_IF_ERROR_RETURN(_bcm_coe_subtag_subport_lport_entry_delete(unit,
               _bcm_subtag_subport_port_info[unit][idx].modport_subport_idx));

    _BCM_COE_SUBTAG_VLAN_ID_USED_CLR(unit, port, vlan_id);

    group_id = _BCM_SUBPORT_COE_GROUP_ID_GET(subport_group);
    *(_bcm_subport_group_subport_port_count[unit] + group_id) -= 1;

    _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_SET(unit, idx,
                              subtag_to_subport_port_tab_idx,
                              BCM_GPORT_INVALID,
                              BCM_VLAN_INVALID,
                              -1, -1, -1, -1, -1, FALSE, -1);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;
}


int bcmi_xgs5_subport_coe_port_delete(int unit, bcm_gport_t subport)
{
    int rv = BCM_E_NONE;

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (!BCM_GPORT_IS_SUBPORT_PORT(subport)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: gport 0x%x is not "
                                       "subport port gport\n"),
                                       subport));
        return BCM_E_PARAM;
    }

    _BCM_SUBPORT_COE_LOCK(unit);

    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport)) {
        rv = _bcm_coe_subtag_subport_port_delete(unit, subport);
    }

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: Subport port 0x%x "
                                       "delete failed. rv = %d(%s)\n"),
                                        subport, rv, soc_errmsg(rv)));
        _BCM_SUBPORT_COE_UNLOCK(unit);
        return rv;
    }

    _BCM_SUBPORT_COE_UNLOCK(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;

}

/*
 * Function:
 *      _bcm_coe_subtag_subport_group_create
 * Purpose:
 *      Create a SubTag subport group
 * Parameters:
 *      unit        - (IN) Device Number
 *      port        - (IN) Front panel port
 *      group_id    - (IN) Group Id being configured
 *      group  - (OUT) GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_coe_subtag_subport_group_create(int unit, bcm_port_t port,
                                     int group_id, bcm_gport_t *group)
{

    int rv = BCM_E_NONE;

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    _BCM_SUBPORT_COE_SUBTAG_GROUP_SET(*group, port, group_id);
    _BCM_SUBPORT_COE_GROUP_TYPE_CASCADED_SET(*group);

    _BCM_SUBPORT_COE_SUBTAG_GROUP_USED_SET(unit, group_id);
    _bcm_subtag_subport_group_count[unit]++;

    /* Also, set the SUBPORT_TAG_NAMESPACE to be same as the port number */

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
           _BCM_CPU_TABS_NONE, SUBPORT_TAG_NAMESPACEf, port));

    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_subport_coe_group_create
 * Purpose:
 *      Create a subport group
 * Parameters:
 *      unit   - (IN) Device Number
 *      config - (IN) Subport group config information
 *      group  - (OUT) GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_xgs5_subport_coe_group_create(int unit,
                                     bcm_subport_group_config_t *config,
                                     bcm_gport_t *group)
{
    int rv = BCM_E_NONE, group_id = 0, i = 0;
    bcm_port_t port;
    soc_info_t *si = &SOC_INFO(unit);

    /* Ensure the subport module is initialized */
    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (!(config->flags & BCM_SUBPORT_GROUP_TYPE_SUBPORT_TAG)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: The group type has to "
                                       "be SUBPORT_TAG\n")));
        return BCM_E_PARAM;
    }

    /* Validate the passed in front panel port */
    rv = _bcm_esw_port_gport_validate(unit, config->port, &port);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: Invalid gport 0x%x for "
                                       "subport group create.\n"),
                                  config->port));
        return BCM_E_PORT;
    }

    /* Ensure that the physical port can be a cascaded port */
    if ((config->flags & BCM_SUBPORT_GROUP_TYPE_SUBPORT_TAG) &&
        !BCM_PBMP_MEMBER(si->subtag_pbm, port)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubportTag/CoE "
                                       "should be enabled"
                                       " for port %d\n"), port));
            return BCM_E_PORT;
    }

    /* If max groups aready taken up, error out */
    if (_bcm_subport_group_count[unit] == si->max_subport_coe_groups) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: All %d subport "
                                       "groups already used.\n"),
                                       si->max_subport_coe_groups));
        return BCM_E_FULL;
    }

    if (config->flags & BCM_SUBPORT_GROUP_WITH_ID) {
        group_id = BCM_GPORT_SUBPORT_GROUP_GET(*group);
        /* Group id should be within 0 to 512.
        * BCM_GPORT_SUBPORT_GROUP_MASK is 0xffffff (for all chips)
        * but subtag group id is only first 9 bits (8-0). 
        * Check that only subtag group id bits (8-0) are configured*/
        if ((group_id >> _BCM_SUBPORT_COE_GROUP_PORT_SHIFT)
                & _SHR_GPORT_SUBPORT_GROUP_MASK) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport group id = %d "
                                       "should be within 0 to"
                                       " %d\n"), 
                                       group_id, 
                                       si->max_subport_coe_groups-1));
            return BCM_E_PARAM;
        }
        if (_BCM_SUBPORT_COE_GROUP_USED_GET(unit, group_id)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport group id = %d "
                                       "is already used\n"), group_id));
            return BCM_E_EXISTS;
        }
    } else {
        /* If without Id, grab a free group */
        for (i =0; i < si->max_subport_coe_groups; i++) {
            if (!_BCM_SUBPORT_COE_GROUP_USED_GET(unit, i)) {
                group_id = i;
                break;
            }
        }

        if (i == si->max_subport_coe_groups) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: All %d subport groups "
                                       "already used.\n"), i));
            return BCM_E_FULL;
        }
    }

    _BCM_SUBPORT_COE_LOCK(unit);

    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) && 
        (config->flags & BCM_SUBPORT_GROUP_TYPE_SUBPORT_TAG)) {
            rv  =_bcm_coe_subtag_subport_group_create(unit, port, 
                                                      group_id, group);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport group "
                                       "create failed"
                                       " for port %d. rv = %d(%s)\n"),
                                  config->port, rv, soc_errmsg(rv)));
                _BCM_SUBPORT_COE_UNLOCK(unit);
                return rv;
            }
    }

    _BCM_SUBPORT_COE_GROUP_USED_SET(unit, group_id);
    _bcm_subport_group_count[unit]++;

    _BCM_SUBPORT_COE_UNLOCK(unit);


#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_coe_subtag_subport_group_destroy
 * Purpose:
 *      Create a SubTag subport group
 * Parameters:
 *      unit        - (IN) Device Number
 *      port        - (IN) Front panel port
 *      group_id    - (IN) Group Id being configured
 *      group  - (OUT) GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_coe_subtag_subport_group_destroy(int unit, bcm_gport_t group)
{
    int group_id = -1, i = 0;
    int pp_port = 0, rv = BCM_E_NONE;
    int subtag_subport_count = 0;
    soc_info_t *si = &SOC_INFO(unit);

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(group)) {

        group_id = _BCM_SUBPORT_COE_GROUP_ID_GET(group);

        if (!_BCM_SUBPORT_COE_SUBTAG_GROUP_USED_GET(unit, group_id)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport group "
                                       "0x%x not found.\n"), group));
            return BCM_E_NOT_FOUND;
        }

        subtag_subport_count =
            *(_bcm_subport_group_subport_port_count[unit] + group_id);

        for (i=0; i < si->max_subport_coe_ports; i++) {

            if (subtag_subport_count == 0) {
               break;
            }

            if (!_BCM_SUBPORT_COE_SUBTAG_PORT_INFO_VALID_GET(unit, i)) {
               continue;
            }

            if (group !=
               _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_GROUP_GET(unit, i)) {
               continue;
            }

            rv = _bcm_coe_subtag_subport_port_delete(unit, 
                      _bcm_subtag_subport_port_info[unit][i].subport_port);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport group "
                                       "0x%x destroy failed \n"
                                       " to delete subport port (subport group id %d, "
                                        " subport id %d)\n"), group, group_id, pp_port));
                return rv;
            }
            subtag_subport_count--;
        }

        *(_bcm_subport_group_subport_port_count[unit] + group_id) = 0;
        /* delete the subport group */
        _bcm_subport_group_count[unit]--;
        _BCM_SUBPORT_COE_GROUP_USED_CLR(unit, group_id);
        _bcm_subtag_subport_group_count[unit]--;
        _BCM_SUBPORT_COE_SUBTAG_GROUP_USED_CLR(unit, group_id);

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_coe_group_destroy
 * Purpose:
 *      Destroy a subport group and all subport ports attached to it.
 * Parameters:
 *      unit   - (IN) Device Number
 *      group  - (IN) GPORT (generic port) identifier for subport group
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_xgs5_subport_coe_group_destroy(int unit, bcm_gport_t group)
{
    int rv = BCM_E_NONE;
    soc_info_t *si = &SOC_INFO(unit);

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (!BCM_GPORT_IS_SUBPORT_GROUP(group)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: group 0x%x is not subport "
                                       "group gport\n"), group));
        return BCM_E_PARAM;
    }

    _BCM_SUBPORT_COE_LOCK(unit);

    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(group)) {
        if (BCM_PBMP_IS_NULL(si->subtag_pbm)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag ports not "
                                       "configured\n")));
            _BCM_SUBPORT_COE_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        rv = _bcm_coe_subtag_subport_group_destroy(unit, group);
    }

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport group 0x%x "
                                       "destroy failed. rv= %d(%s)\n"),
                                        group, rv, soc_errmsg(rv)));
        _BCM_SUBPORT_COE_UNLOCK(unit);
        return rv;
    }

    _BCM_SUBPORT_COE_UNLOCK(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_coe_subtag_subport_group_get
 * Purpose:
 *      Create a SubTag subport group
 * Parameters:
 *      unit        - (IN) Device Number
 *      group_id    - (IN) Group Id being configured
 *      group  - (OUT) GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_coe_subtag_subport_group_get(int unit, bcm_gport_t group,
                                  bcm_subport_group_config_t *config)
{
    int group_id = -1;

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(group)) {
        group_id = _BCM_SUBPORT_COE_GROUP_ID_GET(group);
        if (!_BCM_SUBPORT_COE_SUBTAG_GROUP_USED_GET(unit, group_id)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport "
                                       "group 0x%x not found.\n"), group));
            return BCM_E_NOT_FOUND;;
        }
        config->flags |= BCM_SUBPORT_GROUP_TYPE_SUBPORT_TAG;
        config->port = _BCM_SUBPORT_COE_GROUP_PORT_GET(group);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_coe_group_get
 * Purpose:
 *      Get a subport group
 * Parameters:
 *      unit   - (IN) Device Number
 *      group  - (IN) GPORT (generic port) identifier
 *      config - (OUT) Subport group config information
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_xgs5_subport_coe_group_get(int unit, bcm_gport_t group,
                                     bcm_subport_group_config_t *config)
{
    int rv = BCM_E_NONE;
    soc_info_t *si = &SOC_INFO(unit);

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_SUBPORT_GROUP(group)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: group 0x%x is not "
                                       "subport group gport\n"),
                                        group));
        return BCM_E_PARAM;
    }

    _BCM_SUBPORT_COE_LOCK(unit);

    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(group)) {
        if (BCM_PBMP_IS_NULL(si->subtag_pbm)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag ports not "
                                       "configured\n")));
            _BCM_SUBPORT_COE_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        rv = _bcm_coe_subtag_subport_group_get(unit, group, config);
    }

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport group get "
                                       "failed for 0x%x, rv = %d(%s)\n"),
                                        group, rv, soc_errmsg(rv)));
        _BCM_SUBPORT_COE_UNLOCK(unit);
        return rv;
    }

    _BCM_SUBPORT_COE_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_coe_subtag_subport_group_traverse
 * Purpose:
 *      Traverse all valid subports for given SubTag subport group
 *      and call the user supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      group     - Subport group GPORT identifier
 *      cb        - User callback function, called once per subport.
 *      user_data - cookie
 */

STATIC int
_bcm_coe_subtag_subport_group_traverse(int unit,
                                   bcm_gport_t group,
                                   bcm_subport_port_traverse_cb cb,
                                   void *user_data)
{

    int rv = BCM_E_NONE, group_id = 0, i = 0;
    int subtag_subport_count = 0;
    bcm_subport_config_t config;
    soc_info_t *si = &SOC_INFO(unit);

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    bcm_subport_config_t_init(&config);

    if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(group)) {
        group_id = _BCM_SUBPORT_COE_GROUP_ID_GET(group);

        if (!_BCM_SUBPORT_COE_SUBTAG_GROUP_USED_GET(unit, group_id)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport group "
                                       "0x%x not found.\n"), group));
            return BCM_E_NOT_FOUND;
        }

        subtag_subport_count =
            *(_bcm_subport_group_subport_port_count[unit] + group_id);


        for (i=0; i < si->max_subport_coe_ports; i++) {

            if (subtag_subport_count == 0) {
               break;
            }

            if (!_BCM_SUBPORT_COE_SUBTAG_PORT_INFO_VALID_GET(unit, i)) {
               continue;
            }

            if (group !=
               _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_GROUP_GET(unit, i)) {
               continue;
            }

            rv = _bcm_coe_subtag_subport_port_get(unit, 
                     _bcm_subtag_subport_port_info[unit][i].subport_port, 
                    &config);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport group "
                                       "0x%x traverse failed \n"
                                       "to get subport port (group id %d, "
                                       "subport id %d)\n"),
                                        group, group_id,
                       _bcm_subtag_subport_port_info[unit][i].subport_port));
                return rv;
            }
            /* coverity[returned_value] */
            rv = cb(unit, 
                    _bcm_subtag_subport_port_info[unit][i].subport_port, 
                    &config, 
                    user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport group 0x%x "
                                       "traverse failed \n"
                                       "to callback for subport "
                                       "port (group id %d, subport id %d)\n"), 
                                       group, group_id, 
                       _bcm_subtag_subport_port_info[unit][i].subport_port));
                return rv;
            }
#endif
            subtag_subport_count--;
        }
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_coe_group_traverse
 * Purpose:
 *      Traverse all valid subports for given subport group and call the user
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      group     - Subport group GPORT identifier
 *      cb        - User callback function, called once per subport.
 *      user_data - cookie
 */

int bcmi_xgs5_subport_coe_group_traverse(int unit, bcm_gport_t group,
                                     bcm_subport_port_traverse_cb cb,
                                     void *user_data)
{
    int rv = BCM_E_NONE;
    soc_info_t *si = &SOC_INFO(unit);

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (!BCM_GPORT_IS_SUBPORT_GROUP(group)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: group 0x%x is not subport "
                                       "group gport\n"), group));
        return BCM_E_PARAM;
    }

    _BCM_SUBPORT_COE_LOCK(unit);

    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(group)) {
        if (BCM_PBMP_IS_NULL(si->subtag_pbm)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag ports not "
                                       "configured\n")));
            _BCM_SUBPORT_COE_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        rv =_bcm_coe_subtag_subport_group_traverse(unit, group, cb, user_data);
    }

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport group traverse failed for "
                                       "0x%x, rv = %d(%s)\n"),
                                        group, rv, soc_errmsg(rv)));
        _BCM_SUBPORT_COE_UNLOCK(unit);
        return rv;
    }

    _BCM_SUBPORT_COE_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_coe_subport_free_entry_find
 * Purpose:
 *      Search for unused entry in the SUBPORT_TAG_SGPP_MAP table
 * Parameters:
 *      unit - Device unit number
 *      index - (OUT) index where found
 * Returns:
 *      BOC_E_NONE      - free entry found
 *      BOC_E_FULL      - no free entries found
 */
STATIC int 
_bcm_coe_subport_free_entry_find(int unit, int *free_index)
{
    int index;
    soc_info_t *si=&SOC_INFO(unit);

    if(_bcm_subtag_subport_port_info[unit] == NULL) {
        return BCM_E_INTERNAL;
    }
    for (index = 0; index < si->max_subport_coe_ports; index++) {
        if (_bcm_subtag_subport_port_info[unit][index].valid == 0) {
            /* Return the first unused entry*/
            *free_index = index;
            return BCM_E_NONE;
        }
    }

    /* If we fall through, the table is full */
    return BCM_E_FULL;
}

/*
 * Function:
 *      _bcm_coe_subport_allocate_modport
 * Purpose:
 *      Search for unused entry from CoE modules configured
 * Parameters:
 *      unit - Device unit number
 *      mod  - (OUT) ModuleId of free module
 *      port - (OUT) Port num of free free port on the module
 * Returns:
 *      BOC_E_NONE      - free entry found
 *      BOC_E_FULL      - no free entries found
 */
STATIC int 
_bcm_coe_subport_allocate_modport(int unit,int *mod, int *port)
{
    int i, j, rv;
    bcm_stk_modid_config_t modIdInfo[TD2P_MAX_COE_MODULES];
    bcm_gport_t coe_system_gport;
    soc_info_t *si=&SOC_INFO(unit);
    SHR_BITDCL *mod_port_bitmap[TD2P_MAX_COE_MODULES] = { NULL };
    int mod_port_used[TD2P_MAX_COE_MODULES] = { 0 };
    int modid, portid;
    unsigned int size;

    if(_bcm_subtag_subport_port_info[unit] == NULL) {
        return BCM_E_INTERNAL;
    }

    /* Initialize the (mod,port) info */
    for(i = 0; i < si->num_coe_modules; i++) {
        modIdInfo[i].modid = modIdInfo[i].num_ports = -1;
        size = SHR_BITALLOCSIZE(TD2P_MAX_SUBPORT_COE_PORT_PER_MOD);
        mod_port_bitmap[i] = 
            sal_alloc(size, "mod_port_bitmap");
        if (mod_port_bitmap[i] == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(mod_port_bitmap[i], 0, size);
    }

    /* Get the configured CoE module-port info */
    _bcm_stk_aux_modport_info_get(unit, BCM_MODID_TYPE_COE, modIdInfo);

    /* Create mod port bitmap */
    for (i = 0; i < si->max_subport_coe_ports; i++){
        if(_BCM_SUBPORT_COE_SUBTAG_PORT_INFO_VALID_GET(unit, i)) {
            coe_system_gport = 
                _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_SUBPORT_MODPORT_GET(unit, i);
            modid = BCM_GPORT_MODPORT_MODID_GET(coe_system_gport);
            portid = BCM_GPORT_MODPORT_PORT_GET(coe_system_gport);
            for (j = 0; j < si->num_coe_modules; j++) {
                if (modid == modIdInfo[j].modid) {
                    mod_port_used[j]++;
                    /* coverity[negative_shift: FALSE] */
                    SHR_BITSET(mod_port_bitmap[j], portid);
                }
            }
        }
    }

    /* Get a unused (mod,port) from the CoE modules */
    rv = BCM_E_FULL;
    for (i = 0; i < si->num_coe_modules; i++) {
        if((modIdInfo[i].modid != BCM_MODID_INVALID) &&
           (modIdInfo[i].num_ports != -1) &&
           (mod_port_used[i] < modIdInfo[i].num_ports)) {
            for(j = 0; j < modIdInfo[i].num_ports; j++) {
                if (!SHR_BITGET(mod_port_bitmap[i],j)){
                    *mod  = modIdInfo[i].modid;
                    *port = j;
                    rv = BCM_E_NONE;
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    for(i = 0; i < si->num_coe_modules; i++) {
        if (mod_port_bitmap[i] != NULL) {
            sal_free(mod_port_bitmap[i]);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_xgs5_subport_coe_mod_port_local
 * Purpose:
 *      Check if a given (mod,port) is local
 * Parameters:
 *      unit   - (IN) Device Number
 *      modid  - (IN) Mod Id value
 *      portid - (IN) Port Id value
 * Returns:
 *      TRUE if local, FALSE otherwise
 */

int _bcm_xgs5_subport_coe_mod_port_local(int unit, int modid, bcm_port_t portid)
{
    int rv;

    /* Check if the passed in (mod,port) is valid on the local device */
    rv = _bcm_stk_aux_modport_valid(unit, BCM_MODID_TYPE_COE, modid, portid);

    return ((rv == BCM_E_NONE) ? TRUE : FALSE);
}


/*
 * Function:
 *      _bcm_xgs5_subport_coe_gport_local
 * Purpose:
 *      Check if a given coe-subport-gport is local
 * Parameters:
 *      unit   - (IN) Device Number
 *      modid  - (IN) Mod Id value
 *      portid - (IN) Port Id value
 * Returns:
 *      TRUE if local, FALSE otherwise
 */

int _bcm_xgs5_subport_coe_gport_local(int unit, bcm_gport_t gport)
{
    int rv;

    /* Check if the passed in (mod,port) is valid on the local device */
    rv = _bcm_stk_aux_modport_valid(unit, BCM_MODID_TYPE_COE,
                                    _BCM_SUBPORT_COE_PORT_ID_MOD_GET(gport),
                                    _BCM_SUBPORT_COE_PORT_ID_PORT_GET(gport));

    if(rv == BCM_E_NONE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * Function:
 *      _bcm_xgs5_subport_coe_mod_local
 * Purpose:
 *      Check if a given mod is a local-coe-module
 * Parameters:
 *      unit   - (IN) Device Number
 *      modid  - (IN) Mod Id value
 * Returns:
 *      TRUE if local, FALSE otherwise
 */

int _bcm_xgs5_subport_coe_mod_local(int unit, int modid)
{
    int rv;

    /* Check if the passed in (mod,port) is valid on the local device */
    rv = _bcm_stk_aux_mod_valid(unit, BCM_MODID_TYPE_COE, modid);

    if(rv == BCM_E_NONE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * Function:
 *      _bcm_coe_subtag_subport_port_add
 * Purpose:
 *      Add a subport to a subport group
 * Parameters:
 *      unit   - (IN) Device Number
 *      config - (IN) Subport config information
 *      gport  - (OUT) Subport_port GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_coe_subtag_subport_port_add(int unit,
                                 bcm_subport_config_t *config,
                                 bcm_gport_t *gport)
{
    int rv = BCM_E_NONE, subtag_index = -1, subtag_hw_index = -1;
    int sp_group_idx = -1, mod_port_index = 0, modid = 0, portid = 0, port;
    uint16 vlan_id = 0, phb_val = 0;
    soc_mem_t mem;
    uint32  subtag_to_sgpp_entry[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.*/
#ifdef BCM_TRIDENT3_SUPPORT
    uint32  subtag_to_sgpp_key[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.*/
#endif
    modport_map_subport_entry_t         sgpp_to_port_entry;
#if defined(INCLUDE_L3)
    bcm_l3_egress_t nh_info;
    egr_l3_next_hop_entry_t egr_nh;
    int nh_index;
#endif
    soc_info_t *si = &SOC_INFO(unit);

    sal_memset(subtag_to_sgpp_entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Ensure subport module is initialized */
    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    /* Ensyre group is of the proper type */
    if (!BCM_GPORT_IS_SUBPORT_GROUP(config->group)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: group 0x%x is not "
                                       "subport group gport\n"),
                                        config->group));
        return BCM_E_PARAM;
    }

    /* Make sure the group is of correct type */
    if (!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(config->group)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport group 0x%x is not "
                                       "SubTag group\n"),
                                        config->group));
        return BCM_E_PARAM;
    }

    /* Get the group Idx */
    sp_group_idx = _BCM_SUBPORT_COE_GROUP_ID_GET(config->group);

    /* Make sure the group exists */
    if (!_BCM_SUBPORT_COE_SUBTAG_GROUP_USED_GET(unit, sp_group_idx)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: group 0x%x does not exist. "
                                       "group id =%d\n"),
                                        config->group, sp_group_idx));
        return BCM_E_NOT_FOUND;
    }

    /* Get the physical port number from the group Id */
    port = _BCM_SUBPORT_COE_GROUP_PORT_GET(config->group);

    /* Check if port is member of pbmp_subtag  */
    if (!SOC_PBMP_MEMBER(si->subtag_pbm, port)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: Port %d is not member "
                                       "of pbmp_subport\n"), port));
        return BCM_E_PORT;
    }

    /* Ensure the port in question is configured to host 'subtended' port */
    if (si->port_num_subport[port] <= 0) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: %d number of subports "
                                       "available for port %d\n"),
                                       si->port_num_subport[port], port));
        return BCM_E_CONFIG;
    }

    /* Check if the number of subports on this physical port
       is already reached at the configured values */
    if(*(_bcm_subport_group_subport_port_count[unit] + sp_group_idx) >= 
        si->port_num_subport[port]) {
        return BCM_E_FULL;
    }

    /* Ensure that the passed in subport_tag is valid */
    if ((config->pkt_vlan & 0xFFF) <= BCM_VLAN_NONE) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: (config->pkt_vlan & 0xFFF) "
                                       "%d is invalid\n"),
                                        (config->pkt_vlan & 0xFFF)));
        return BCM_E_PARAM;
    }

    /* If the PHB is set, check the PRI and Color for sanity */
    if (config->prop_flags & BCM_SUBPORT_PROPERTY_PHB) {
        if (config->int_pri < 0 || config->int_pri > 15) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: int_pri %d is invalid\n"), 
                       config->int_pri));
            return BCM_E_PARAM;
        }
        if ((config->color != bcmColorGreen) &&
             (config->color != bcmColorYellow) &&
             (config->color != bcmColorRed)) {
             LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: color %d is invalid\n"), 
                       config->color));
            return BCM_E_PARAM;
        }
    }

    /* Check if the VLAN passed is valid */
    VLAN_CHK_ID(unit, config->pkt_vlan);

    /* Check if VLAN id is alredy used for the port */
    vlan_id = config->pkt_vlan & 0xFFF; 

    if (_BCM_COE_SUBTAG_VLAN_ID_USED_GET(unit, port, vlan_id)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: VLAN id %d already "
                                       "used for port %d\n"),
                                        vlan_id, port));
        return BCM_E_EXISTS;
    }

    /* If the MODPORT is set in the passed in 'system_gport' we will use the
       (mod,port) which is a part of it, if it is valid. Else allocate one
       from the configured CoE modules */

    /* If a valid gport is supplied use it, else allocate one */
    if(BCM_GPORT_IS_MODPORT(config->subport_modport)) {
        modid = BCM_GPORT_MODPORT_MODID_GET(config->subport_modport);
        portid = BCM_GPORT_MODPORT_PORT_GET(config->subport_modport);
        /* Check if the supplied CoE-(mod,port) is valid */
        if(_bcm_stk_aux_modport_valid(unit, BCM_MODID_TYPE_COE,
                                      modid, portid) != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: Supplied (mod,port) (%d,%d) "
                                       "not valid\n"),
                                        modid, portid));
            return BCM_E_PARAM;
        }

    } else {
        rv = _bcm_coe_subport_allocate_modport(unit, &modid, &portid);
        if(rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: Free CoE (mod,port) "
                                       "not found\n")));
            return rv;
        }
        /* Put together a CoE-MODPORT for subsequent use */
        BCM_GPORT_MODPORT_SET(config->subport_modport, modid, portid);

    }

    /* Find a free entry in SUBPORT_TAG_SGPP_MAPm table */
    mem = SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP_MEM(unit);
    rv = _bcm_coe_subport_free_entry_find(unit, &subtag_index);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: Free entry not found in "
                                  " subtag to sgpp map table\n")));
        return rv;
    }

    if (soc_mem_is_cam(unit, mem)) {
        subtag_hw_index = subtag_index;
         /* Read entry and map VLAN ID (Subtag) to (mod,port) */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, subtag_hw_index,
            &subtag_to_sgpp_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"SUBPORT_TAG_SGPP_MAPm read "
                                       "failed at index %d\n"),
                                       subtag_index));
            return rv;
        }
    /* Make sure the entry allocated is unused*/
        if (soc_mem_field32_get(unit, mem, (uint32 *)&subtag_to_sgpp_entry, 
                SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->valid)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"SUBPORT_TAG_SGPP_MAPm entry "
                                       "at index %d is been used\n"),
                                       subtag_index));
            return BCM_E_FAIL;
        }
    }

    /* Set the TAG, Namespace, (MOD,PORT), VALID and related fields */
    soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry, 
                        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->subport_tag,
                        config->pkt_vlan);

    /* Assign the physical port to the namespace */
    soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry, 
        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->subport_tag_namespace,
        _BCM_SUBPORT_COE_GROUP_PORT_GET(config->group));

    /* Assign the modId and port */

    if (SOC_IS_TRIDENT3X(unit)) {
       /* In Trident3 chip,src port is SGPP(7,0), src modid is SGPP(15,8) */
        soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry,
                        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->src_port, 
                        ((modid << 0x8) | portid));
    } else {
        soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry,
                        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->src_modid, 
                        modid);

        soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry, 
                        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->src_port, 
                        portid);
    }

    /* Set the 'valid' field */
    soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry,
                        SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->valid,
                        1);

    /* Setup the masks */
    if (SOC_MEM_FIELD_VALID(unit, mem,
                  SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->subport_tag_mask)) {
        soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry,
                    SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->subport_tag_mask,
                    0xFFF);
        soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry,
          SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->subport_tag_namespace_mask,
                      0x7F);
    }

    /* Set PHB for the subport */
    if (config->prop_flags & BCM_SUBPORT_PROPERTY_PHB) {
        soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry,
            SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->phb_enable, 1);
        soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry,
            SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->int_pri,
                            config->int_pri);
        soc_mem_field32_set(unit, mem, (uint32 *)&subtag_to_sgpp_entry,
            SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP(unit)->cng,
                           _BCM_COLOR_ENCODING(unit, config->color));
        phb_val = 1;
    }

    /* Write the entry to HW */
    if (soc_mem_is_hashed(unit, mem)) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, &subtag_to_sgpp_entry);
        if (SOC_FAILURE(rv) && rv != SOC_E_EXISTS) {
            return rv;
        }
    } else {
        if (subtag_hw_index >= 0) {
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, subtag_hw_index,
                           &subtag_to_sgpp_entry);
    /* Bail on error */
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                         (BSL_META_U(unit,"ERROR: (VLAN id %d, port %d) "
                                       "to subtag_index %d "
                                       "map failed.\n"),
                                       vlan_id, port, subtag_index));
                return rv;
            }
        }
    }

    /* Map Subtag to Physical port mapping using MODPORT_MAP_SUBPORTm.
     * Note: subtag_index should be released below if failed.
     */

    /* Compute the (mod,port) index */
    rv = _bcm_esw_src_mod_port_table_index_get(unit, modid,
                                               portid, &mod_port_index);
    BCM_FAILURE_CLEANUP(rv, cleanup);

    /* Read the entry at the index and update needed fields */
    mem = SUBPORT_COE_HW_MODPORT_MAP_SUBPORT_MEM(unit);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, mod_port_index,
            &sgpp_to_port_entry);
    BCM_FAILURE_CLEANUP(rv, cleanup);

    /* Set the physical port as the destination */
    soc_mem_field32_set(unit, mem, (uint32 *)&sgpp_to_port_entry,
        SUBPORT_COE_HW_MODPORT_MAP_SUBPORT(unit)->dest, port); 
    /* Set the isTrunk as 0, we change if trunking is added */
    soc_mem_field32_set(unit, mem, (uint32 *)&sgpp_to_port_entry,
        SUBPORT_COE_HW_MODPORT_MAP_SUBPORT(unit)->is_trunk, 0);
    /* Set 1 'enable' the entry  */
    soc_mem_field32_set(unit, mem, (uint32 *)&sgpp_to_port_entry,
        SUBPORT_COE_HW_MODPORT_MAP_SUBPORT(unit)->enable, 1);

    /* Write the entry to HW */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, mod_port_index,
            &sgpp_to_port_entry);
    BCM_FAILURE_CLEANUP(rv, cleanup);

    /* Update the ports SOURCE_TRUNK_MAP entry with a default
       LPORT porperty  */
    rv = _bcm_coe_subtag_subport_default_lport_entry_set(unit, port, 
                                                         mod_port_index);
    BCM_FAILURE_CLEANUP(rv, cleanup);

    /* With the index also update the EGR_GPP_ATTRIBUTES, with the subtag
     * to be used on egress.
     * Note: lport profile should be released below if failed.
     */
    rv = soc_mem_field32_modify(unit, EGR_GPP_ATTRIBUTESm,
                                mod_port_index,
                                SUBPORT_TAGf, vlan_id);

    /* Bail on error */
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: (mod,port) (%d,%d) "
                                       "to physical port %d "
                                       "map failed.\n"), 
                                       modid, portid, port));
        BCM_FAILURE_CLEANUP(rv, cleanup_lport);
    }

    /* We need to reserve a egress-object for this subport */

    /* Allocate a next hop index */
#if defined(INCLUDE_L3)
    bcm_l3_egress_t_init(&nh_info);

    rv = bcm_xgs3_nh_add(unit, _BCM_L3_SHR_MATCH_DISABLE |
                         _BCM_L3_SHR_WRITE_DISABLE, &nh_info, &nh_index);

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: Next hop allocation failed. "
                                  "Coe (mod,port): (%d,%d), physical port %d"), 
                                   modid, portid, port));
        BCM_FAILURE_CLEANUP(rv, cleanup_lport);
    }

    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));

    /* SD-tag entry type */
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            DATA_TYPEf, 2);
    } else {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            ENTRY_TYPEf, 2);
    }

    /* Setup the mod, port associated with the NH entry */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            SD_TAG__HG_MC_DST_MODIDf, modid);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            SD_TAG__HG_MC_DST_PORT_NUMf, portid);

    /* Write EGR_L3_NEXT_HOP entry */
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL, 
                       nh_index, &egr_nh);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: Next hop update failed. "
                                        "Coe (mod,port): (%d,%d), "
                                        "physical port %d"), 
                                        modid, portid, port));
        BCM_FAILURE_CLEANUP(rv, cleanup_nh);
    }

    /* Adjust the offset and assign the nh_index */
    _bcm_subtag_subport_port_info[unit][subtag_index].nh_index = 
        BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + nh_index;
#endif

    /* Update software book keeping.
     * Note: subtag_index, nh_index and software book keeping
     *       should be released below if failed.
     */

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_mem_is_hashed(unit, SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP_MEM(unit))) {
       /* Search the hash table for hw index */
        sal_memset(subtag_to_sgpp_key, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        soc_SUBPORT_ID_TO_SGPP_MAPm_field32_set(unit, &subtag_to_sgpp_key,
                                                BASE_VALIDf, 1);
        soc_SUBPORT_ID_TO_SGPP_MAPm_field32_set(unit, &subtag_to_sgpp_key,
                           SUBPORT_ID_NAMESPACEf,
                          _BCM_SUBPORT_COE_GROUP_PORT_GET(config->group));
        soc_SUBPORT_ID_TO_SGPP_MAPm_field32_set(unit, &subtag_to_sgpp_key,
                           SUBPORT_IDf,
                          config->pkt_vlan);
        rv = (soc_mem_search(unit, SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP_MEM(unit),
                MEM_BLOCK_ANY,
                &subtag_hw_index, &subtag_to_sgpp_key, &subtag_to_sgpp_entry, 0));
        BCM_FAILURE_CLEANUP(rv, cleanup_nh);
    }
#endif
    /* Update bitmap to indicate the subtag/vlan_id is now in use,
       for this port */
    _BCM_COE_SUBTAG_VLAN_ID_USED_SET(unit, port, vlan_id);

    /* Encode our outgoing parameter as GPORT_SUBPORT type and
       fill in the associated (mod,port) */
    _BCM_SUBPORT_COE_PORT_ID_SET(*gport, modid, portid);
    
    /* Set the bit on the outgoing param indicating this is subtag type */
    _BCM_SUBPORT_COE_PORT_TYPE_SET(*gport, _BCM_SUBPORT_COE_TYPE_SUBTAG);

    /* Save the related port info indexed by the subtag_index */
    _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_SET(unit, subtag_index, subtag_hw_index, 
        config->group, config->pkt_vlan, *gport, config->subport_modport,
        phb_val, config->int_pri, config->color, TRUE, mod_port_index);

    /* Increment number of subports on this group */
    *(_bcm_subport_group_subport_port_count[unit] + sp_group_idx) += 1;

    rv = _bcm_coe_subtag_subport_default_egr_lport_entry_set(unit, *gport);
    BCM_FAILURE_CLEANUP(rv, cleanup_nh);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    return rv;

    /* Free/invalidate the resources that have been allocated.*/
cleanup_nh:
#if defined(INCLUDE_L3)
    bcm_xgs3_nh_del(unit, 0, nh_index);
#endif

    _BCM_COE_SUBTAG_VLAN_ID_USED_CLR(unit, port, vlan_id);

    _bcm_subtag_subport_port_info[unit][subtag_index].nh_index = -1;
    *(_bcm_subport_group_subport_port_count[unit] + sp_group_idx) -= 1;

    _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_SET(unit, subtag_index, subtag_hw_index,
                                          BCM_GPORT_INVALID, 
                                          BCM_VLAN_INVALID, 
                                          -1, -1, -1, -1, -1, FALSE, -1);

cleanup_lport:
    _bcm_coe_subtag_subport_lport_entry_delete(unit, mod_port_index);

cleanup:
    /* Free/invalidate the SUBPORT_TAG_SGPP_MAP table entry */
    /* coverity[check_return : FALSE] */
    soc_mem_field32_modify(unit, SUBPORT_COE_HW_SUBPORT_SUBTAG_SGPP_MEM(unit),
                           subtag_hw_index, VALIDf, 0);
    return rv;
}


/*
 * Function:
 *      bcmi_xgs5_subport_coe_port_add
 * Purpose:
 *      Add a subport to a subport group
 * Parameters:
 *      unit   - (IN) Device Number
 *      config - (IN) Subport config information
 *      port   - (OUT) Subport_port GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_xgs5_subport_coe_port_add(int unit, bcm_subport_config_t *config,
                                   bcm_gport_t *port)
{
    int rv = BCM_E_NONE;
    uint16 group_id;
    soc_info_t *si = &SOC_INFO(unit);

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (!BCM_GPORT_IS_SUBPORT_GROUP(config->group)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: config->group 0x%x is not "
                                       "subport group gport\n"),
                                        config->group));
        return BCM_E_PARAM;
    }

    group_id = _BCM_SUBPORT_COE_GROUP_ID_GET(config->group);
    if(!_BCM_SUBPORT_COE_GROUP_USED_GET(unit, group_id)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR:config->group 0x%x "
                                       "is not found\n"), config->group));
        return BCM_E_NOT_FOUND;
    }

    if (_bcm_subport_coe_port_count[unit] == si->max_subport_coe_ports) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR:All %d subport's in use\n"),
                                  si->max_subport_coe_ports));
        return BCM_E_FULL;
    }

    _BCM_SUBPORT_COE_LOCK(unit);

    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(config->group)) {
            if (BCM_PBMP_IS_NULL(si->subtag_pbm)) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag ports not "
                                       "configured\n")));
                _BCM_SUBPORT_COE_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            rv = _bcm_coe_subtag_subport_port_add(unit, config, port);
    }

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: Subport port add failed. "
                                       "rv = %d(%s)\n"),
                                        rv, soc_errmsg(rv)));
        _BCM_SUBPORT_COE_UNLOCK(unit);
        return rv;
    }

    _BCM_SUBPORT_COE_UNLOCK(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;
}

int bcmi_xgs5_subport_coe_port_traverse(int unit,
                                        bcm_subport_port_traverse_cb cb,
                                        void *user_data)
{
    int i = 0, rv;
    bcm_subport_config_t config;
    soc_info_t *si = &SOC_INFO(unit);

    if(_bcm_subtag_subport_port_info[unit] == NULL) {
        return BCM_E_NONE;
    }

    for (i=0; i < si->max_subport_coe_ports; i++) {
        if(_bcm_subtag_subport_port_info[unit][i].valid) {

            bcm_subport_config_t_init(&config);

            rv = _bcm_coe_subtag_subport_port_get(unit, 
                     _bcm_subtag_subport_port_info[unit][i].subport_port, 
                    &config);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport port traverse failed \n"
                                  "to get SubTag subport id %d index: %d\n"), 
                     _bcm_subtag_subport_port_info[unit][i].subport_port, i));
                return rv;
            }

            rv = cb(unit, 
                    _bcm_subtag_subport_port_info[unit][i].subport_port, 
                    &config, 
                    user_data);

            COMPILER_REFERENCE(rv);

#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: subport port traverse failed \n"
                                  "to callback for subport port "
                                  "(subport_port id %d)\n"),
                        _bcm_subtag_subport_port_info[unit][i].subport_port));
                return rv;
            }
#endif
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_coe_subtag_subport_cleanup
 * Purpose:
 *      Delate all valid subport ports and all valid subport groups.
        Also clean up software resources.
 * Parameters:
 *      unit      - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_coe_subtag_subport_cleanup(int unit)
{
    int group_id = 0, rv = BCM_E_NONE, i = 0;
    bcm_port_t port;
    bcm_gport_t subport_group;
    soc_mem_t mem;
    soc_info_t *si = &SOC_INFO(unit);

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

   /* Delete all SubTag subport ports */
    for (i=0; i < si->max_subport_coe_ports; i++) {

        if (!_BCM_SUBPORT_COE_SUBTAG_PORT_INFO_VALID_GET(unit, i)) {
            continue;
        }

        rv = _bcm_coe_subtag_subport_port_delete(unit, 
                  _bcm_subtag_subport_port_info[unit][i].subport_port);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: SubTag subport port "
                                       "delete failed (subport id %d)\n"),
                         _bcm_subtag_subport_port_info[unit][i].subport_port));
            return rv;
        }

        subport_group =
            _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_GROUP_GET(unit, i);

        group_id = _BCM_SUBPORT_COE_GROUP_ID_GET(subport_group);

        if (*(_bcm_subport_group_subport_port_count[unit] + group_id) == 0) {
             /* delete the subport group */
            _bcm_subport_group_count[unit]--;
            _BCM_SUBPORT_COE_GROUP_USED_CLR(unit, group_id);
            _bcm_subtag_subport_group_count[unit]--;
            _BCM_SUBPORT_COE_SUBTAG_GROUP_USED_CLR(unit, group_id);
        }
    }

    /* Unset the 'Cascaded' type on port */
    BCM_PBMP_ITER(SOC_INFO(unit).subtag_pbm, port) {
        mem = SUBPORT_COE_HW_ING_PORT_MEM(unit);
        if (SOC_MEM_FIELD_VALID(unit, mem, 
                                SUBPORT_COE_HW_ING_PORT(unit)->port_type)) {
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify
                (unit, mem, 
                 port, 
                 SUBPORT_COE_HW_ING_PORT(unit)->port_type, 
                            _BCM_COE_PORT_TYPE_ETHERNET));
        }

        mem = SUBPORT_COE_HW_EGR_PORT_MEM(unit);
        if (SOC_MEM_FIELD_VALID(unit, mem, 
                                SUBPORT_COE_HW_EGR_PORT(unit)->port_type)) {
            BCM_IF_ERROR_RETURN(soc_mem_field32_modify(unit, mem, port, 
                                    SUBPORT_COE_HW_EGR_PORT(unit)->port_type,
                                    _BCM_COE_PORT_TYPE_ETHERNET));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_coe_cleanup
 * Purpose:
 *      Delate all valid subport ports and all valid subport groups.
        Also clean up software resources.
 * Parameters:
 *      unit      - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_xgs5_subport_coe_cleanup(int unit)
{

    int rv = BCM_E_NONE;
    soc_info_t *si = &SOC_INFO(unit);

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
        if (soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
            if (BCM_PBMP_NOT_NULL(si->subtag_allowed_pbm)) {
                rv = _bcm_coe_subtag_subport_cleanup(unit);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_SUBPORT,
                          (BSL_META_U(unit,"ERROR: SubTag cleanup failed\n")));
                    return rv;
                }
            }
        }
    }

    bcmi_xgs5_subport_free_resource(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_egr_subtag_dot1p_map_add
 * Purpose:
 *      Map the egress SubTag CoE packet's PHB (internal priority and color) 
 *      to subport tag's 'Priority' and 'Color'.
 * Parameters:
 *      unit      - (IN) Device Number
 *      map       - (IN) qos map
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_xgs5_subport_egr_subtag_dot1p_map_add(int unit,
                                               bcm_qos_map_t *map)
{
    soc_mem_t mem;
    int index;
    uint32              tab_ent[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer. */

    mem = SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P_MEM(unit);

    if (!soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
        return BCM_E_UNAVAIL;
    }

    if (map->etag_pcp > 7 ||
        map->etag_de > 1 ||
        map->int_pri < 0 || map->int_pri > 15 ||
        ((map->color != bcmColorGreen) && 
        (map->color != bcmColorYellow) && 
        (map->color != bcmColorRed))) {
        return BCM_E_PARAM;
    }

    /* EGR_SUBPORT_TAG_DOT1P_MAPm table is indexed with
     * internal priority - 4 bits [5:2] and color - 2 bits[1:0]
     */
    index = ((map->int_pri << 2) | map->color) & soc_mem_index_max(unit, mem);

    sal_memset(tab_ent, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    soc_mem_field32_set(unit, mem, tab_ent,
        SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P(unit)->subport_tag_priority,
        map->etag_pcp);
    if (SOC_IS_TRIDENT3X(unit)) {
        /* TD3 only supports 4-byte SUBPORT-TAG */
        soc_mem_field32_set(unit, mem, tab_ent,
            SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P(unit)->subport_tag_cfi,
            map->etag_de);
    } else {
        soc_mem_field32_set(unit, mem, tab_ent,
           SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P(unit)->subport_tag_color,
            map->color);
        soc_mem_field32_set(unit, mem, tab_ent,
            SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P(unit)->subport_tag_cfi,
            map->etag_de);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                       tab_ent));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_egr_subtag_dot1p_map_delete
 * Purpose:
 *      Delete the map the egress SubTag CoE packet's
 *      PHB (internal priority and color) 
 *      to subport tag's 'Priority' and 'Color'.
 * Parameters:
 *      unit      - (IN) Device Number
 *      map       - (IN) qos map
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_subport_egr_subtag_dot1p_map_delete(int unit,
                                            bcm_qos_map_t *map)
{
    soc_mem_t mem;
    int index;
    uint32              tab_ent[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer. */

    mem = SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P_MEM(unit);

    if (!soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
        return BCM_E_UNAVAIL;
    }

    if (map->etag_pcp > 7 ||
        map->etag_de > 1 ||
        map->int_pri < 0 || map->int_pri > 15 ||
        ((map->color != bcmColorGreen) && 
        (map->color != bcmColorYellow) && 
        (map->color != bcmColorRed))) {
        return BCM_E_PARAM;
    }

    /* EGR_SUBPORT_TAG_DOT1P_MAPm table is indexed with
     * internal priority - 4 bits [5:2] and color - 2 bits[1:0]
     */
    index = ((map->int_pri << 2) | map->color) & soc_mem_index_max(unit, mem);

    sal_memset(tab_ent, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    soc_mem_field32_set(unit, mem, tab_ent,
        SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P(unit)->subport_tag_priority, 0);
    soc_mem_field32_set(unit, mem, tab_ent,
        SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P(unit)->subport_tag_cfi, 0);
    /* Trident3 only support 4-byte SUBPORT-TAG */
    if (!SOC_IS_TRIDENT3X(unit)) {
        soc_mem_field32_set(unit, mem, tab_ent,
        SUBPORT_COE_HW_EGR_SUBPORT_TAG_DOT1P(unit)->subport_tag_color, 0);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                       tab_ent));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_subtag_port_tpid_set
 * Description:
 *      Set the default Tag Protocol ID for a SubTag port.
 * Parameters:
 *      unit  - (IN) Device number
 *      gport - (IN) SubTag group gport (Ignored)
 *      tpid  - (IN) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */



int 
bcmi_xgs5_subport_subtag_port_tpid_set(int unit,
                                       bcm_gport_t gport,
                                       uint16 tpid)
{
    uint32      regval = 0;
    soc_reg_t   reg;

    reg = SUBPORT_TAG_TPIDr;

    if (!(soc_reg_field_valid(unit, reg, TPIDf) || 
          soc_reg_field_valid(unit, reg, ENABLEf))) {
        return BCM_E_UNAVAIL;
    }

    soc_reg_field_set(unit, reg, &regval, TPIDf, tpid);
    soc_reg_field_set(unit, reg, &regval, ENABLEf, 1);

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));

    reg = EGR_SUBPORT_TAG_TPIDr;
    if (!(soc_reg_field_valid(unit, reg, TPID_8BYTE_TAGf) || 
          soc_reg_field_valid(unit, reg, TPID_4BYTE_TAGf))) {
        return BCM_E_UNAVAIL;
    }

    soc_reg_field_set(unit, reg, &regval, TPID_8BYTE_TAGf, tpid);
    soc_reg_field_set(unit, reg, &regval, TPID_4BYTE_TAGf, tpid);

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_subtag_port_tpid_get
 * Description:
 *      Retrieve the default Tag Protocol ID for a SubTag port.
 * Parameters:
 *      unit  - (IN) Device number
 *      gport - (IN)SubTag group gport (Ignored)
 *      tpid  - (OUT) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */

int
bcmi_xgs5_subport_subtag_port_tpid_get(int unit,
                                       bcm_gport_t gport,
                                       uint16 *tpid)
{
    uint32      regval = 0;
    soc_reg_t   reg;

    reg = SUBPORT_TAG_TPIDr;
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &regval));

    if(soc_reg_field_get(unit, reg, regval, VALIDf)) {
        *tpid = soc_reg_field_get(unit, reg, regval, TPIDf);
        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmi_xgs5_subport_subtag_port_tpid_delete
 * Description:
 *      Delete allowed TPID for a SubTag port.
 * Parameters:
 *      unit  - (IN) Device number
 *      gport - (IN) SubTag group gport (Ignored)
 *      tpid  - (IN) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */

int
bcmi_xgs5_subport_subtag_port_tpid_delete(int unit,
                                          bcm_gport_t gport,
                                          uint16 tpid)
{
    uint32      regval = 0;
    soc_reg_t   reg;

    reg = SUBPORT_TAG_TPIDr;
    if (!(soc_reg_field_valid(unit, reg, TPIDf) || 
          soc_reg_field_valid(unit, reg, ENABLEf))) {
        return BCM_E_UNAVAIL;
    }

    soc_reg_field_set(unit, reg, &regval, TPIDf, 0);
    soc_reg_field_set(unit, reg, &regval, ENABLEf, 0);

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));

    reg = EGR_SUBPORT_TAG_TPIDr;
    if (!(soc_reg_field_valid(unit, reg, TPID_8BYTE_TAGf) || 
          soc_reg_field_valid(unit, reg, TPID_4BYTE_TAGf))) {
        return BCM_E_UNAVAIL;
    }

    soc_reg_field_set(unit, reg, &regval, TPID_8BYTE_TAGf, 0);
    soc_reg_field_set(unit, reg, &regval, TPID_4BYTE_TAGf, 0);

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_control_subtag_status_set
 * Description:
 *      Set the SubTag status for a given port.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) port
 *      value - (IN) Enable/Disable status
 * Return Value:
 *      BCM_E_XXX
 * Note: The application need to delete all subport groups/ports, if any,
 *       before disabling the SubTag CoE for given port.
 */

int bcmi_xgs5_port_control_subtag_status_set(int unit,
                                             bcm_port_t port, int value)
{
    int rv = BCM_E_NONE;
    /* int  start = 0, end = 0; */
    uint32 port_type;
    soc_info_t *si = &SOC_INFO(unit);

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (!soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
        return BCM_E_UNAVAIL;
    }

    if (!BCM_PBMP_MEMBER(si->subtag_allowed_pbm, port)) {
        return BCM_E_PORT;
    }
    _BCM_SUBPORT_COE_LOCK(unit);

    port_type = value ? _BCM_COE_PORT_TYPE_CASCADED :
                        _BCM_COE_PORT_TYPE_ETHERNET;

    rv = _bcm_esw_port_tab_set(unit, port,
           _BCM_CPU_TABS_NONE, PORT_TYPEf, port_type);

    if (BCM_FAILURE(rv)) {
        _BCM_SUBPORT_COE_UNLOCK(unit);
        return rv;
    }
    rv = _bcm_esw_egr_port_tab_set(unit, port,
                  PORT_TYPEf, port_type);
    if (BCM_FAILURE(rv)) {
        _BCM_SUBPORT_COE_UNLOCK(unit);
       return rv;
    }


    

    /* start = si->port_subport_base[port] - KT2_MIN_SUBPORT_INDEX;
       end = start + si->port_num_subport[port] - 1;
    */ 
    if (value) {
        BCM_PBMP_PORT_ADD(si->subtag_pbm, port);
        BCM_PBMP_PORT_ADD(si->subtag.bitmap, port);
/****
        SOC_IF_ERROR_RETURN
            (soc_kt2_cosq_port_coe_linkphy_status_set(unit, port, 1));
        SOC_IF_ERROR_RETURN
            (soc_kt2_cosq_s1_range_set(unit, port, start, end, 0));
        SOC_IF_ERROR_RETURN
            (soc_kt2_cosq_repl_map_set(unit, port, start, end, 1));
 */
    } else {
        if (BCM_PBMP_MEMBER(si->subtag_pbm, port)) {
            BCM_PBMP_PORT_REMOVE(si->subtag_pbm, port);
            BCM_PBMP_PORT_REMOVE(si->subtag.bitmap, port);
/****
            SOC_IF_ERROR_RETURN
                (soc_kt2_cosq_port_coe_linkphy_status_set(unit, port, 0));
            SOC_IF_ERROR_RETURN
                (soc_kt2_cosq_s1_range_set(unit, port, 0, 0, 0));
            SOC_IF_ERROR_RETURN
                (soc_kt2_cosq_repl_map_set(unit, port, start, end, 0));
 */ 
        }
    }
    _BCM_SUBPORT_COE_UNLOCK(unit);
    return rv;
}
/*
 * Function:
 *      bcmi_xgs5_port_control_subtag_status_get
 * Description:
 *      Get the SubTag status for a given port.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) port
 *      value - (OUT) Enable/Disable status
 * Return Value:
 *      BCM_E_XXX
 */

int bcmi_xgs5_port_control_subtag_status_get(int unit,
                                             bcm_port_t port, 
                                             int *value)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    soc_info_t *si = &SOC_INFO(unit);
    port_tab_entry_t port_tab_entry;

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    if (!soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
        return BCM_E_UNAVAIL;
    }
    if (!BCM_PBMP_MEMBER(si->subtag_allowed_pbm, port)) {
        return BCM_E_PORT;
    }

    _BCM_SUBPORT_COE_LOCK(unit);

    *value = 0;
    mem = SUBPORT_COE_HW_ING_PORT_MEM(unit);

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port,
            &port_tab_entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR:PORT_TABLEm read failed\n")));
        _BCM_SUBPORT_COE_UNLOCK(unit);
        return rv;
    }
    if ((soc_mem_field32_get(unit, mem, &port_tab_entry,
            PORT_TYPEf) == _BCM_COE_PORT_TYPE_CASCADED)) {
        *value = 1;
    }

    _BCM_SUBPORT_COE_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcmi_xgs5_subport_group_resolve
 * Purpose:
 *      Resolve physical port associated with the subtag subport group
 */

int
_bcmi_xgs5_subport_group_resolve(int unit,
                                bcm_gport_t subport_group_gport,
                                bcm_module_t *modid, bcm_port_t *port,
                                bcm_trunk_t *trunk_id, int *id)

{
    bcm_module_t my_modid;
    uint16 group_id;

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    *modid = -1;
    *port = -1;
    *id = -1;
    *trunk_id = BCM_TRUNK_INVALID;

    if (!BCM_GPORT_IS_SUBPORT_GROUP(subport_group_gport)) {
        LOG_WARN(BSL_LS_BCM_SUBPORT,
                 (BSL_META_U(unit,"gport 0x%x is not subport gport\n"),
                             subport_group_gport));
        return BCM_E_NONE;
    }

    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(subport_group_gport))) {

        group_id = _BCM_SUBPORT_COE_GROUP_ID_GET(subport_group_gport);
        if(!_BCM_SUBPORT_COE_GROUP_USED_GET(unit, group_id)) {
            LOG_WARN(BSL_LS_BCM_SUBPORT,
                     (BSL_META_U(unit,"subport gport 0x%x is not used\n"),
                                 subport_group_gport));
            return BCM_E_NONE;
        }

        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        *modid = my_modid;
        *port = _BCM_SUBPORT_COE_GROUP_PORT_GET(subport_group_gport);
    } else if (soc_feature(unit, soc_feature_subport_enhanced)) {
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
        return _bcm_tr2_subport_group_resolve(unit, subport_group_gport,
                                            modid, port, trunk_id,id);
#endif
    } else if (soc_feature(unit, soc_feature_subport)) {
#if defined (BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
        return _bcm_tr_subport_group_resolve(unit, subport_group_gport,
                                             modid, port, trunk_id, id);
#endif
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcmi_coe_subport_physical_port_get
 * Purpose:
 *      Given a subport-group/subport-port GPORT, return the physical port
 */
int
_bcmi_coe_subport_physical_port_get(int unit, bcm_gport_t subport,
                                    int *local_port)
{
    int i = 0;
    soc_info_t *si = &SOC_INFO(unit);

    if(_bcm_subtag_subport_port_info[unit] == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (BCM_GPORT_IS_SET(subport)) {
        if (BCM_GPORT_IS_SUBPORT_PORT(subport)) {
            if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport)){

                /* Find the entry corresponding to the subport */
                for (i = 0; i < si->max_subport_coe_ports; i++) {
                    if((_bcm_subtag_subport_port_info[unit][i].valid) && 
                       (_bcm_subtag_subport_port_info[unit][i].subport_port == 
                         subport)) {
                            /* Entry found, break */
                            break;
                        }
                }

                /* If not found, return */
                if (i == si->max_subport_coe_ports) {
                    return BCM_E_PORT;
                }

                /* Get the physical port associated with the subport-group in
                   question */

                *local_port = _BCM_SUBPORT_COE_GROUP_PORT_GET(
                              _bcm_subtag_subport_port_info[unit][i].group);
            } else if (BCM_GPORT_IS_SUBPORT_GROUP(subport)) {
                *local_port = _BCM_SUBPORT_COE_GROUP_PORT_GET(subport);
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_coe_subport_mod_port_physical_port_get
 * Purpose:
 *      Given a coe-(mod,port), return the physical port
 */
int
_bcmi_coe_subport_mod_port_physical_port_get(int unit, bcm_module_t modid, bcm_port_t portid,
                                             int *local_port)
{
    int subport_gport;

    /* Encode type and fill in the associated (mod,port) */
    _BCM_SUBPORT_COE_PORT_ID_SET(subport_gport, modid, portid);
    /* Set the bit on the outgoing param indicating this is subtag type */
    _BCM_SUBPORT_COE_PORT_TYPE_SET(subport_gport, _BCM_SUBPORT_COE_TYPE_SUBTAG);

    return _bcmi_coe_subport_physical_port_get(unit, subport_gport, local_port);
}

/*
 * Function:
 *      _bcm_xgs5_subport_port_resolve
 * Purpose:
 *      Resolve subport_port to related (mod,port) 
 */

int
_bcm_xgs5_subport_port_resolve(int unit,
                               bcm_gport_t subport_port_gport,
                               bcm_module_t *modid, bcm_port_t *port,
                               bcm_trunk_t *trunk_id, int *id)
{

#ifdef INCLUDE_L3
    bcm_subport_config_t config;
    int rv = BCM_E_NONE;

    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    *modid = -1;
    *port = -1;
    *id = -1;
    *trunk_id = BCM_TRUNK_INVALID;

    if (!BCM_GPORT_IS_SUBPORT_PORT(subport_port_gport)) {
        LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,"ERROR: gport 0x%x is not subport gport\n"),
                                  subport_port_gport));
        return BCM_E_PARAM;
    }

    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport_port_gport))) {

            bcm_subport_config_t_init(&config);
            rv = _bcm_coe_subtag_subport_port_get(unit, subport_port_gport,
                                                  &config);
            if(rv == BCM_E_NONE) {
                *modid = _BCM_SUBPORT_COE_PORT_ID_MOD_GET(subport_port_gport);
                *port  = _BCM_SUBPORT_COE_PORT_ID_PORT_GET(subport_port_gport);
            }
    } else if (soc_feature(unit, soc_feature_subport_enhanced)) {
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
        return _bcm_tr2_subport_port_resolve(unit,subport_port_gport,
                                             modid, port, trunk_id, id);
#endif /* defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3) */
    } else if (soc_feature(unit, soc_feature_subport)) {
#if defined (BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
        return _bcm_tr_subport_port_resolve(unit, subport_port_gport,
                                            modid, port, trunk_id, id);
#endif
    }

    return rv;

#else
    return BCM_E_UNAVAIL;
#endif
}


/*
 * Function:
 *      bcmi_xgs5_subport_gport_modport_get
 * Purpose:
 *      Given a subport-gport, return teh corresponding mod-port 
 * Parameters:
 *      unit   - (IN) Device Number
 *      subport_gport  - (IN) GPORT (generic port) identifier
 *      module  - (OUT) Module associated with the subport
 *      port  - (OUT) Port associated with the subport  
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_xgs5_subport_gport_modport_get(int unit,
            bcm_gport_t subport_gport, bcm_module_t *module, bcm_port_t *port)
{
    bcm_trunk_t   trunk_id;
    int           id = 0;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_subport_port_resolve(unit, subport_gport,
                            module, port, &trunk_id, &id));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_subport_gport_validate
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
_bcm_xgs5_subport_gport_validate(int unit, bcm_port_t port_in,
                                       bcm_port_t *port_out)
{
    if (BCM_GPORT_IS_SET(port_in)) {
        if (BCM_GPORT_IS_SUBPORT_PORT(port_in) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port_in)) {
                return bcm_esw_port_local_get(unit, port_in, port_out);
        } else {
            return BCM_E_PORT; 
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
*      _bcmi_coe_subport_tcam_idx_get
* Purpose:
*      Given a subport-port GPORT, return its TCAM h/w idx
*/
int
_bcmi_coe_subport_tcam_idx_get(int unit, bcm_gport_t subport,
                                    int *hw_idx)
{
    int i = 0;
    soc_info_t *si = &SOC_INFO(unit);

    if(hw_idx == NULL) {
        return BCM_E_PARAM;
    }

    if(_bcm_subtag_subport_port_info[unit] == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (BCM_GPORT_IS_SET(subport)) {
        if (BCM_GPORT_IS_SUBPORT_PORT(subport)) {
            if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport)){

                /* Find the entry corresponding to the subport */
                for (i = 0; i < si->max_subport_coe_ports; i++) {
                    if((_bcm_subtag_subport_port_info[unit][i].valid) && 
                       (_bcm_subtag_subport_port_info[unit][i].subport_port == 
                         subport)) {
                            /* Entry found, break */
                            break;
                        }
                }

                /* If not found, return error */
                if (i == si->max_subport_coe_ports) {
                    return BCM_E_PORT;
                }

                /* Get the associated hw idx */
                *hw_idx = _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_TCAM_HW_ID_GET(unit, i);
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_PORT;
}

/*
 * Function:
 *      _bcmi_coe_multicast_subport_encap_get
 * Purpose:
 *      Get the Encap ID for NIV virtual port.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      subport_port    - (IN) Multicast group ID.
 *      encap_id        - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcmi_coe_multicast_subport_encap_get(int unit, bcm_gport_t subport_port, 
                                         bcm_if_t *encap_id)
{
    int i = 0;
    soc_info_t *si = &SOC_INFO(unit);

    if(_bcm_subtag_subport_port_info[unit] == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (BCM_GPORT_IS_SET(subport_port)) {
        if (BCM_GPORT_IS_SUBPORT_PORT(subport_port)) {
            if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport_port)){

                /* Find the entry corresponding to the subport */
                for (i=0; i < si->max_subport_coe_ports; i++) {
                    if((_bcm_subtag_subport_port_info[unit][i].valid) && 
                       (_bcm_subtag_subport_port_info[unit][i].subport_port == 
                         subport_port)) {
                            /* Entry found, break */
                            break;
                        }
                }

                /* If not found, return */
                if (i == si->max_subport_coe_ports) {
                    return BCM_E_NOT_FOUND;
                }

                /* Get the encap id */
                *encap_id = _bcm_subtag_subport_port_info[unit][i].nh_index;
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_NONE;
}

int bcmi_xgs5_subport_coe_ether_type_size_set(int unit,
                                              bcm_switch_control_t type,
                                              int arg)
{
    uint32 rval;

    if (arg > BCMI_MAX_SHORT_VALUE || arg < 0) {
        return BCM_E_PARAM;
    }

#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT)
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        if ((type == bcmSwitchSubportCoEEtherType) ||
            (type == bcmSwitchSubportEgressTpid)) {
            /* 0x8100 is not a valid subtag tpid  */
            if (SOC_BOND_INFO_FEATURE_GET(unit,
                    socBondInfoFeatureNoCoeSubtag8100) && (arg == 0x8100)) {
                return BCM_E_PARAM;
            }
        }
    }
#endif /*  BCM_ESW_SUPPORT && BCM_IPROC_SUPPORT */

    switch (type) {
        case bcmSwitchSubportCoEEtherType:
            BCM_IF_ERROR_RETURN(READ_SUBPORT_TAG_TPIDr(unit, &rval));
            soc_reg_field_set(unit, SUBPORT_TAG_TPIDr, &rval, TPIDf,
                              arg);
            /* If the TPID is being set to '0', clear the ENABLEf */
            if(arg) {
                soc_reg_field_set(unit, SUBPORT_TAG_TPIDr, &rval, ENABLEf,
                                  1);
            } else {
                soc_reg_field_set(unit, SUBPORT_TAG_TPIDr, &rval, ENABLEf, 
                                  0);
            }
            SOC_IF_ERROR_RETURN(WRITE_SUBPORT_TAG_TPIDr(unit, rval));
            break;

        case bcmSwitchSubportEgressTpid:
            BCM_IF_ERROR_RETURN(READ_EGR_SUBPORT_TAG_TPIDr(unit, &rval));
            soc_reg_field_set(unit, EGR_SUBPORT_TAG_TPIDr, &rval, TPID_4BYTE_TAGf,
                              arg);
            SOC_IF_ERROR_RETURN(WRITE_EGR_SUBPORT_TAG_TPIDr(unit, rval));
            break;

        case bcmSwitchSubportEgressWideTpid:
            BCM_IF_ERROR_RETURN(READ_EGR_SUBPORT_TAG_TPIDr(unit, &rval));
            soc_reg_field_set(unit, EGR_SUBPORT_TAG_TPIDr, &rval, TPID_8BYTE_TAGf, 
                              arg);
            SOC_IF_ERROR_RETURN(WRITE_EGR_SUBPORT_TAG_TPIDr(unit, rval));
            break;

        default:
            /* Return 'BCM_E_UNAVAIL' by default */
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

int bcmi_xgs5_subport_coe_ether_type_size_get(int unit, 
                                              bcm_switch_control_t type, 
                                              int *arg)
{
    uint32 rval;

    switch (type) {
        case bcmSwitchSubportCoEEtherType:
            BCM_IF_ERROR_RETURN(READ_SUBPORT_TAG_TPIDr(unit, &rval));
            *arg = soc_reg_field_get(unit, SUBPORT_TAG_TPIDr, rval, 
                                     TPIDf);
            break;

        case bcmSwitchSubportEgressTpid:

            BCM_IF_ERROR_RETURN(READ_EGR_SUBPORT_TAG_TPIDr(unit, &rval));
            *arg = soc_reg_field_get(unit, EGR_SUBPORT_TAG_TPIDr, rval, 
                                     TPID_4BYTE_TAGf);
            break;

        case bcmSwitchSubportEgressWideTpid:

            BCM_IF_ERROR_RETURN(READ_EGR_SUBPORT_TAG_TPIDr(unit, &rval));
            *arg = soc_reg_field_get(unit, EGR_SUBPORT_TAG_TPIDr, rval, 
                                     TPID_8BYTE_TAGf);

            break;

        default:
            /* Return 'BCM_E_UNAVAIL' by default */
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

int bcmi_xgs5_subport_subtag_port_override_set(int unit, int port, int arg)
{
    bcm_module_t        modid;
    bcm_port_t          portid;
    bcm_trunk_t         trunk_id;
    int                 id = 0, index = 0, rv = BCM_E_UNAVAIL;
    egr_port_entry_t    egr_port_entry;

    if (arg > BCMI_MAX_SHORT_VALUE || arg < 0) {
        return BCM_E_PARAM;
    }
#if defined(BCM_HGPROXY_COE_SUPPORT)
    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port) &&
        SOC_MEM_FIELD_VALID(unit, EGR_LPORT_PROFILEm, VT_SYS_PORT_OVERRIDEf)) {
        return bcm_esw_port_egr_lport_field_set(
                   unit, port, EGR_LPORT_PROFILEm,
                   VT_SYS_PORT_OVERRIDEf, arg ? 1 : 0);
    }
#endif

    if(BCM_GPORT_IS_SUBPORT_PORT(port)) {
        /* Resolve the subport */
        BCM_IF_ERROR_RETURN(
            _bcm_xgs5_subport_port_resolve(unit, port, &modid, &portid, 
                                           &trunk_id, &id));
        /* Get the associated index */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_src_mod_port_table_index_get(unit, modid, portid,
                                              &index));
    } else {
        index = port;
    }

    
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, EGR_PORTm, MEM_BLOCK_ALL, 
                                     index, &egr_port_entry));

    soc_mem_field32_set(unit, EGR_PORTm, &egr_port_entry,
                        VT_SYS_PORT_OVERRIDEf, arg ? 1 : 0);

    rv = soc_mem_write(unit, EGR_PORTm, MEM_BLOCK_ALL, 
                       index, &egr_port_entry);

    return rv;
}

int bcmi_xgs5_subport_subtag_port_override_get(int unit, int port, int *arg)
{
    bcm_module_t        modid;
    bcm_port_t          portid;
    bcm_trunk_t         trunk_id;
    int                 id = 0, index = 0;
    egr_port_entry_t    egr_port_entry;
 #if defined(BCM_HGPROXY_COE_SUPPORT)
    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port) &&
        SOC_MEM_FIELD_VALID(unit, EGR_LPORT_PROFILEm, VT_SYS_PORT_OVERRIDEf)) {
        return bcm_esw_port_egr_lport_field_get(
                   unit, port, EGR_LPORT_PROFILEm,
                   VT_SYS_PORT_OVERRIDEf, (uint32 *)arg);
    }
 #endif

    if(BCM_GPORT_IS_SUBPORT_PORT(port)) {
        /* Resolve the subport */
        BCM_IF_ERROR_RETURN(
            _bcm_xgs5_subport_port_resolve(unit, port, &modid, &portid, 
                                           &trunk_id, &id));
        /* Get the associated index */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_src_mod_port_table_index_get(unit, modid, portid,
                                              &index));
    } else {
        index = port;
    }

    
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_PORTm, MEM_BLOCK_ALL, 
                                     index, &egr_port_entry));

    *arg = soc_mem_field32_get(unit, EGR_PORTm, &egr_port_entry, 
                               VT_SYS_PORT_OVERRIDEf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_port_learn_set
 * Purpose:
 *      Set the CML bits for a CoE subport.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_subport_port_learn_set(int unit, bcm_gport_t subport_port_id, 
                                  uint32 flags)
{
    int cml = 0, rv = BCM_E_NONE;
    soc_field_t fields[2];
    uint32 values[2];

    /* Ensure the subport module is initialized */
    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    cml = 0;
    if (!(flags & BCM_PORT_LEARN_FWD)) {
       cml |= (1 << 0);
    }
    if (flags & BCM_PORT_LEARN_CPU) {
       cml |= (1 << 1);
    }
    if (flags & BCM_PORT_LEARN_PENDING) {
       cml |= (1 << 2);
    }
    if (flags & BCM_PORT_LEARN_ARL) {
       cml |= (1 << 3);
    }

    /* Setup the fields */
    fields[0] = CML_FLAGS_MOVEf;
    fields[1] = CML_FLAGS_NEWf;
    /* Set the values */
    values[0] = values[1] = cml;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, subport_port_id,
           _BCM_CPU_TABS_NONE, fields[0], values[0]));

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, subport_port_id,
           _BCM_CPU_TABS_NONE, fields[1], values[1]));

    return rv;
}

/*
 * Function:    
 *      bcmi_xgs5_subport_port_learn_get
 * Purpose:
 *      Get the CML bits for a CoE subport.
 * Returns: 
 *      BCM_E_XXX
 */     
int     
bcmi_xgs5_subport_port_learn_get(int unit, bcm_gport_t subport_port_id,
                                  uint32 *flags)
{
    int32 cml = 0;
    
    /* Ensure the subport module is initialized */
    _BCM_SUBPORT_COE_CHECK_INIT(unit);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, subport_port_id,
           CML_FLAGS_NEWf, &cml));

    *flags = 0;
    if (!(cml & (1 << 0))) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (cml & (1 << 1)) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (cml & (1 << 2)) {
       *flags |= BCM_PORT_LEARN_PENDING;
    }
    if (cml & (1 << 3)) {
       *flags |= BCM_PORT_LEARN_ARL;
    }

    return BCM_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT

#define SCACHE_ALLOC_SIZE_INIT(_var_)                                          \
    do {                                                                       \
        (_var_) = 0; (_scache_ofst_) = 0;                                      \
        LOG_VERBOSE(BSL_LS_BCM_SUBPORT,                                        \
            (BSL_META_U(unit,                                                  \
                        "WarmBoot: Scache offset initialized to 0\n")));       \
    } while (0)

#define SCACHE_ALLOC_SIZE_INCR(_var_, _count_)                                 \
    do {                                                                       \
        (_var_) += (_count_); (_scache_ofst_) += (_count_);                    \
        LOG_VERBOSE(BSL_LS_BCM_SUBPORT,                                        \
                    (BSL_META_U(unit,                                          \
                                "WarmBoot: Scache offset incr by %d to %d\n"), \
                                (int)(_count_),  (_scache_ofst_)              \
                               ));                                            \
    } while (0)

#define SCACHE_BYTE_INIT(_scache_ptr_)                                         \
    do {                                                                       \
        _scache_ofst_ = 0;                                                     \
        LOG_VERBOSE(BSL_LS_BCM_SUBPORT,                                        \
                    (BSL_META_U(unit,                                          \
                                "WarmBoot: Scache offset initialized to 0\n")  \
                               ));                                            \
    } while (0)

#define SCACHE_BYTE_COPY(_scache_ptr_, _local_var_, _size_)                   \
    do {                                                                      \
        sal_memcpy(_scache_ptr_, _local_var_, _size_);                        \
        _scache_ptr_ += (_size_);                                             \
        (_scache_ofst_) += (_size_);                                          \
        LOG_VERBOSE(BSL_LS_BCM_SUBPORT,                                       \
                    (BSL_META_U(unit,                                         \
                                "WarmBoot: Scache offset incr by %d to %d\n"),\
                                (int)(_size_),  (_scache_ofst_)              \
                               ));                                           \
    } while (0)

#define SCACHE_BYTE_RESTORE(_scache_ptr_, _local_var_, _size_)                \
    do {                                                                      \
        sal_memcpy(_local_var_, _scache_ptr_, _size_);                        \
        _scache_ptr_ += (_size_);                                             \
        (_scache_ofst_) += (_size_);                                          \
        LOG_VERBOSE(BSL_LS_BCM_SUBPORT,                                       \
                    (BSL_META_U(unit,                                         \
                                "WarmBoot: Scache offset incr by %d to %d\n"),\
                                (int)(_size_),  (_scache_ofst_)              \
                               ));                                           \
    } while (0)

#define SCACHE_BYTE_RESTORE2(_scache_ptr_, _local_var_, _scache_sz_, _local_sz_)\
    do {                                                                      \
        SOC_SCACHE_SIZE_MIN_RECOVER(_scache_ptr_, _local_var_, _scache_sz_, _local_sz_);\
        (_scache_ofst_) += (_scache_sz_);                                     \
        LOG_VERBOSE(BSL_LS_BCM_SUBPORT,                                       \
                    (BSL_META_U(unit,                                         \
                                "WarmBoot: Scache offset incr by %d to %d\n"),\
                                (int)(_scache_sz_),  (_scache_ofst_)          \
                               ));                                            \
    } while (0)

/*
 * Function:
 *      bcmi_xgs5_subport_wb_scache_alloc
 * Purpose:
 *      Allocate scache memory for individual entries to be saved for L2 warm-boot
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      version - (IN) version number
 *      alloc_sz - (OUT) The allocated size
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_subport_wb_scache_alloc(int unit, uint16 version, int *alloc_sz)
{
    int num_vlan = 0;
    soc_info_t *si = &SOC_INFO(unit);
    int incr_sz;
    uint32 _scache_ofst_ = 0;

    SCACHE_ALLOC_SIZE_INIT(*alloc_sz);

    /* Num subport groups (_bcm_subport_group_count) */
    SCACHE_ALLOC_SIZE_INCR(*alloc_sz, sizeof(int));

    /* Subport-group bitmap (_bcm_subport_group_bitmap) */
    SCACHE_ALLOC_SIZE_INCR(*alloc_sz,
        SHR_BITALLOCSIZE(si->max_subport_coe_groups));

    /* Subport-subtag-group bitmap (_bcm_subtag_group_bitmap) */
    SCACHE_ALLOC_SIZE_INCR(*alloc_sz,
        SHR_BITALLOCSIZE(si->max_subport_coe_groups));

    /* Number of Subport ports per group
       (_bcm_subport_group_subport_port_count)*/
    SCACHE_ALLOC_SIZE_INCR(*alloc_sz,
        (si->max_subport_coe_groups * sizeof(int)));

    incr_sz = SOC_SCACHE_SIZE_MIN(sizeof(pbmp_t), 
                                  SHR_BITALLOCSIZE(SOC_MAX_NUM_PORTS));
    /* Bitmap of subtended ports */ 
    SCACHE_ALLOC_SIZE_INCR(*alloc_sz, incr_sz);

    /* VLAN ID array bimap (_bcm_subtag_vlan_id_bitmap) */
    num_vlan = soc_mem_index_count(unit, VLAN_TABm);
    SCACHE_ALLOC_SIZE_INCR(*alloc_sz,
                           SOC_MAX_NUM_PORTS * SHR_BITALLOCSIZE(num_vlan));

    /* Subport port info (_bcm_subtag_subport_port_info_t) */
    SCACHE_ALLOC_SIZE_INCR(*alloc_sz,
        (si->max_subport_coe_ports * sizeof(_bcm_subtag_subport_port_info_t)));


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_wb_sync
 * Purpose:
 *      Sync desired subport data to scache passed in
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      version - (IN) version number
 *      scache_ptr - (IN) Pointer to scache where data will be synced
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_xgs5_subport_wb_sync(int unit, uint16 version, uint8 **scache_ptr)
{
    int value = 0, num_vlan = 0, port = 0;
    soc_info_t *si = &SOC_INFO(unit);
    int copy_sz;
    uint32 _scache_ofst_ = 0;

    SCACHE_BYTE_INIT(*scache_ptr);

    /* If the 'subtag_allowed_pbm' is non-null, it means subport
       config is in effect and we need to sync it to scache */
    if (BCM_PBMP_NOT_NULL(SOC_INFO(unit).subtag_allowed_pbm)) {

        /* Num subport groups (_bcm_subport_group_count) */
        value = _bcm_subport_group_count[unit];
        SCACHE_BYTE_COPY(*scache_ptr, &value, sizeof(int));

        /* Subport-group bitmap (_bcm_subport_group_bitmap) */
        SCACHE_BYTE_COPY(*scache_ptr, _bcm_subport_group_bitmap[unit],
                          SHR_BITALLOCSIZE(si->max_subport_coe_groups));

        /* Subport-group bitmap (_bcm_subtag_group_bitmap) */
        SCACHE_BYTE_COPY(*scache_ptr, _bcm_subtag_group_bitmap[unit],
                          SHR_BITALLOCSIZE(si->max_subport_coe_groups));

        /* Number of Subport ports per group
           (_bcm_subport_group_subport_port_count)*/
        SCACHE_BYTE_COPY(*scache_ptr,
                         _bcm_subport_group_subport_port_count[unit],
                         (si->max_subport_coe_groups * sizeof(int)));

        copy_sz = SOC_SCACHE_SIZE_MIN(sizeof(pbmp_t), 
                                      SHR_BITALLOCSIZE(SOC_MAX_NUM_PORTS));
        /* Bitmap of subtended ports */ 
        /* coverity[overrun-buffer-arg] */
        SCACHE_BYTE_COPY(*scache_ptr, &si->subtag_pbm, copy_sz);

        /* VLAN ID array bimap (_bcm_subtag_vlan_id_bitmap) */
        num_vlan = soc_mem_index_count(unit, VLAN_TABm);

        /* Copy over each configured port's vlan bitmap */
        BCM_PBMP_ITER(si->subtag_pbm, port) {

            SCACHE_BYTE_COPY(*scache_ptr, _bcm_subtag_vlan_id_bitmap[unit][port],
                             SHR_BITALLOCSIZE(num_vlan));
        }

        /* Copy over the subport port info */
        SCACHE_BYTE_COPY(*scache_ptr, _bcm_subtag_subport_port_info[unit],
               (si->max_subport_coe_ports * sizeof(_bcm_subtag_subport_port_info_t)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_subport_wb_recover
 * Purpose:
 *      Recover subport data that was sync'd before.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      version - (IN) version number
 *      scache_ptr - (IN) Pointer to scache where data will be recovered from
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_xgs5_subport_wb_recover(int unit, uint16 version, uint8 **scache_ptr)
{
    int num_vlan = 0, port = 0;
    soc_info_t *si = &SOC_INFO(unit);
    int max_num_ports, pbmp_port_max, scache_sz;
    uint32 _scache_ofst_ = 0;

    SCACHE_BYTE_INIT(*scache_ptr);

    /* If the 'subtag_allowed_pbm' is non-null, it means subport
       config is in effect and we need to sync it to scache */
    if (BCM_PBMP_NOT_NULL(SOC_INFO(unit).subtag_allowed_pbm)) {

        /* Recover SOC_MAX_NUM_PORTS from scache */
        max_num_ports = soc_scache_dictionary_entry_get(unit, 
                                     ssden_SOC_MAX_NUM_PORTS,
                                     SOC_MAX_NUM_PORTS);
        pbmp_port_max = soc_scache_dictionary_entry_get(unit, 
                                     ssden_SOC_PBMP_PORT_MAX,
                                     SOC_PBMP_PORT_MAX);
        /* Num subport groups (_bcm_subport_group_count) */
        SCACHE_BYTE_RESTORE(*scache_ptr, &_bcm_subport_group_count[unit],
                            sizeof(int));

        /* Subport-group bitmap (_bcm_subport_group_bitmap) */
        SCACHE_BYTE_RESTORE(*scache_ptr, _bcm_subport_group_bitmap[unit],
                          SHR_BITALLOCSIZE(si->max_subport_coe_groups));

        /* Subport-group bitmap (_bcm_subtag_group_bitmap) */
        SCACHE_BYTE_RESTORE(*scache_ptr, _bcm_subtag_group_bitmap[unit],
                          SHR_BITALLOCSIZE(si->max_subport_coe_groups));

        /* Number of Subport ports per group
           (_bcm_subport_group_subport_port_count)*/
        /* coverity[overrun-buffer-arg] */
        SCACHE_BYTE_RESTORE(*scache_ptr,
                         _bcm_subport_group_subport_port_count[unit],
                         (si->max_subport_coe_groups * sizeof(int)));

        scache_sz = SOC_SCACHE_SIZE_MIN(SHR_BITALLOCSIZE(pbmp_port_max), 
                                        SHR_BITALLOCSIZE(max_num_ports));
        /* Bitmap of subtended ports */ 
        SCACHE_BYTE_RESTORE2(*scache_ptr, &si->subtag_pbm, 
                             scache_sz, sizeof(pbmp_t));

        sal_memcpy(&si->subtag.bitmap, &si->subtag_pbm, sizeof(pbmp_t));

        /* VLAN ID array bimap (_bcm_subtag_vlan_id_bitmap) */
        num_vlan = soc_mem_index_count(unit, VLAN_TABm);

        /* Copy over each configured port's vlan bitmap */
        BCM_PBMP_ITER(si->subtag_pbm, port) {

            SCACHE_BYTE_RESTORE(*scache_ptr, 
                                _bcm_subtag_vlan_id_bitmap[unit][port],
                                SHR_BITALLOCSIZE(num_vlan));
        }

        /* Copy over the subport port info */
        SCACHE_BYTE_RESTORE(*scache_ptr, _bcm_subtag_subport_port_info[unit],
               (si->max_subport_coe_ports * 
                sizeof(_bcm_subtag_subport_port_info_t)));
    }

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP

/*
 * Function:
 *      _bcm_coe_subport_sw_dump
 * Purpose:
 *      Print Subport module s/w state
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      Void
 */

void
_bcm_coe_subport_sw_dump(int unit)
{
    int idx = 0, port = 0, num_vlan = 0;
    soc_info_t *si = &SOC_INFO(unit);

    LOG_CLI((BSL_META_U(unit,
                "Total Subport Groups: %d\n"),
                _bcm_subport_group_count[unit]));

    LOG_CLI((BSL_META_U(unit,
                        "Subport Groups and ports per group\n")));
    for(idx = 0; idx < si->max_subport_coe_groups; idx++) {
        if(_BCM_SUBPORT_COE_GROUP_USED_GET(unit, idx)) {
           LOG_CLI((BSL_META_U(unit, "%d "), idx));
           LOG_CLI((BSL_META_U(unit, ":%d "),
                    _bcm_subport_group_subport_port_count[unit][idx]));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                "VLAN Ids per port\n")));

    /* Run thru all ports and print their VLAN ID bimap */
    num_vlan = soc_mem_index_count(unit, VLAN_TABm);

    BCM_PBMP_ITER(si->subtag_allowed_pbm, port) {

        LOG_CLI((BSL_META_U(unit, "VLAN List for Port: %d "), port));

        for(idx = 0; idx < num_vlan; idx++) {
            if(_BCM_COE_SUBTAG_VLAN_ID_USED_GET(unit, port, idx)) {
                LOG_CLI((BSL_META_U(unit, "Vlan: %d "), idx));
            }
        }
    }

    /* Run through the subports configured and dump their details */
    LOG_CLI((BSL_META_U(unit,
                        "Subport Ports:\n")));

    for (idx = 0; idx < si->max_subport_coe_ports; idx++) {
        if(_bcm_subtag_subport_port_info[unit][idx].valid) {

            LOG_CLI((BSL_META_U(unit,
                     "Group: 0x%x vlan: %d subtag_tcam_idx: %d "
                     "subport_modport: 0x%x subport_port: 0x%x\n"
                     "nh_index: %d valid: %d modport_subport_idx: 0x%x "
                     "phb_valid: %d priority: %d color: %d\n"), 
                    _bcm_subtag_subport_port_info[unit][idx].group,
                    _bcm_subtag_subport_port_info[unit][idx].vlan,
                    _bcm_subtag_subport_port_info[unit][idx].subtag_tcam_hw_idx,
                    _bcm_subtag_subport_port_info[unit][idx].subport_modport,
                    _bcm_subtag_subport_port_info[unit][idx].subport_port,
                    _bcm_subtag_subport_port_info[unit][idx].nh_index,
                    _bcm_subtag_subport_port_info[unit][idx].valid,
                    _bcm_subtag_subport_port_info[unit][idx].modport_subport_idx,
                    _bcm_subtag_subport_port_info[unit][idx].phb_valid,
                    _bcm_subtag_subport_port_info[unit][idx].priority,
                    _bcm_subtag_subport_port_info[unit][idx].color));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n----\n")));

}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#else  /* BCM_TRIDENT2PLUS_SUPPORT */
int bcm_esw_td2p_subport_not_empty;
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

