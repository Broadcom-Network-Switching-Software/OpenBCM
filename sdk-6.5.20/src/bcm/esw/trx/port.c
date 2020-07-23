/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Port function implementations
 */


#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRX_SUPPORT)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>

#if  defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif

#include <bcm/port.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/trill.h>

#include <bcm_int/esw_dispatch.h>
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/subport.h>
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/virtual.h>
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      _bcm_trx_port_cml_hw2flags
 * Description:
 *      Converts a HW value to CML flags 
 * Parameters:
 *      unit         - (IN) Device number
 *      val          - (IN) TRX HW value 
 *      flags        - (OUT) CML flags BCM_PORT_LEARN_*
 * Return Value:
 *      BCM_E_XXX
 */
int _bcm_trx_port_cml_hw2flags(int unit, uint32 val, uint32 *flags)
{
    uint32  lflags = 0;

    if (NULL == flags) {
        return BCM_E_PARAM;
    }

    if (!(val & (1 << 0))) {
       lflags |= BCM_PORT_LEARN_FWD;
    }
    if (val & (1 << 1)) {
       lflags |= BCM_PORT_LEARN_CPU;
    }
    if (val & (1 << 2)) {
       lflags |= BCM_PORT_LEARN_PENDING;
    }
    if (val & (1 << 3)) {
       lflags |= BCM_PORT_LEARN_ARL;
    }

    *flags = lflags;
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_trx_port_cml_flags2hw
 * Description:
 *      Converts CML flags to a HW value.
 * Parameters:
 *      unit         - (IN) Device number
 *      flags        - (IN) CML flags BCM_PORT_LEARN_*
 *      val          - (OUT) TRX HW value
 * Return Value:
 *      BCM_E_XXX
 */
int _bcm_trx_port_cml_flags2hw(int unit, uint32 flags, uint32 *val)
{
    uint32 hw_val = 0;

    if (NULL == val) {
        return BCM_E_PARAM;
    }
    if (!(flags & BCM_PORT_LEARN_FWD)) {
       hw_val |= (1 << 0);
    }
    if (flags & BCM_PORT_LEARN_CPU) {
       hw_val |= (1 << 1);
    }
    if (flags & BCM_PORT_LEARN_PENDING) {
       hw_val |= (1 << 2);
    }
    if (flags & BCM_PORT_LEARN_ARL) {
       hw_val |= (1 << 3);
    }

    *val = hw_val;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_vp_tpid_add
 * Description:
 *      Add allowed TPID for a virtual port.
 * Parameters:
 *      unit         - (IN) Device number
 *      vport        - (IN) Virtual Port identifier (MPLS ot MIM)
 *      tpid         - (IN) Tag Protocol ID
 *      color_select - (IN) Color mode for TPID
 * Return Value:
 *      BCM_E_XXX
 */
int _bcm_trx_vp_tpid_add(int unit, bcm_gport_t vport, uint16 tpid, int color_select)
{
    int                 vp, rv, tpid_idx=0, tpid_enable=0, islocal;
    source_vp_entry_t   svp;
    uint32              ena_f, cfi_cng, egr_vlan_ctrl;
    bcm_port_t          port;
    bcm_module_t        modid, mymodid;
    bcm_trunk_t         tgid;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    egr_vlan_control_1_entry_t entry;
#endif
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    int vp_lag_vp;
#endif
#if defined(BCM_TRX_SUPPORT)
    /*
     * TRX devices only support OUTER_CFI to be mapped to CNG
     */
    if (color_select == BCM_COLOR_INNER_CFI && SOC_IS_TRX(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, vport, &modid, &port, &tgid, &vp));

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        if (tgid != -1) {
            rv = _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tgid, &vp_lag_vp);
            if (BCM_SUCCESS(rv) &&
                _bcm_vp_used_get(unit, vp_lag_vp, _bcmVpTypeVpLag)) {
                vp = vp_lag_vp;
            }
        }
    }
#endif

    if (-1 == vp) { 
        return BCM_E_PORT;
    }
    if (tgid == -1) {
        BCM_IF_ERROR_RETURN( _bcm_esw_modid_is_local(unit, modid, &islocal));
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mymodid));
        if (!islocal)  {
            if (soc_feature(unit, soc_feature_subtag_coe) ||
                soc_feature(unit, soc_feature_linkphy_coe)) {
            #ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit) &&
                    (_bcm_kt2_mod_is_coe_mod_check(unit, modid) == BCM_E_NONE)) {
                        BCM_IF_ERROR_RETURN(_bcm_kt2_modport_to_pp_port_get(unit,
                            modid, port, &port));
                } else {
                    return BCM_E_PORT;
                }
            #endif
            }
        } else {
            while (mymodid < modid) {
                port += 32;
                modid -= 1;
            }
        }
    }

    _bcm_fb2_outer_tpid_tab_lock(unit);

    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }
    
    /* See if the entry already exist */
    rv = _bcm_fb2_outer_tpid_lkup(unit, tpid, &tpid_idx);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
        uint32 svp_attrs_1[SOC_MAX_MEM_WORDS];
        uint32 tpid_enable = 0;

        BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit,
                    MEM_BLOCK_ANY, vp, svp_attrs_1));
        soc_SVP_ATTRS_1m_field_get(unit,
                &svp_attrs_1, TPID_ENABLEf, (uint32 *)&tpid_enable);

        ena_f = tpid_enable;
    } else
#endif
    {
        ena_f = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
    }

    if ((BCM_E_NOT_FOUND == rv) || !(ena_f & (1 << tpid_idx)) ) {
        rv = _bcm_fb2_outer_tpid_entry_add(unit, tpid, &tpid_idx);
        if (BCM_FAILURE(rv)) {
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return rv;
        }

    }
#if (defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)) \
    && defined(INCLUDE_L3)
    /* Add TPID for Trill Virtual Port */
    if ((SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit))
         && (BCM_GPORT_IS_TRILL_PORT(vport)) 
         && (3 == soc_SOURCE_VPm_field32_get(unit, &svp, TPID_SOURCEf))) {
            rv = bcm_td_trill_tpid_add(unit, vport, tpid_idx);
            if (BCM_FAILURE(rv)) {
                rv = _bcm_fb2_outer_tpid_entry_delete(unit,tpid_idx);
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                return (rv);
            }
    } else
#endif
    {
        tpid_enable = (1 << tpid_idx);
        soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 1);
        ena_f |= tpid_enable;
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
            rv = soc_mem_field32_modify(unit, SVP_ATTRS_1m,
                    vp, TPID_ENABLEf, ena_f);
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, &svp, TPID_ENABLEf, ena_f);
        }

        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);

        if (BCM_FAILURE(rv)) {
            rv = _bcm_fb2_outer_tpid_entry_delete(unit,tpid_idx);
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return rv;
        }
    }

    if ((tgid == -1) && islocal && SOC_PORT_VALID(unit, port)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
            if (soc_feature(unit, soc_feature_egr_all_profile)) {
                BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_get(unit,
                    port, EGR_VLAN_CONTROL_1m, CFI_AS_CNGf, &cfi_cng));
            } else {
                rv = READ_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, port, &entry);
                cfi_cng = soc_EGR_VLAN_CONTROL_1m_field32_get(unit, &entry,
                                CFI_AS_CNGf);
            }
        } else
#endif
        {
            rv = READ_EGR_VLAN_CONTROL_1r(unit, port, &egr_vlan_ctrl);
            cfi_cng = soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r,
                            egr_vlan_ctrl, CFI_AS_CNGf);
        }
        if (BCM_FAILURE(rv)) {
            rv = _bcm_fb2_outer_tpid_entry_delete(unit,tpid_idx);
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return rv;
        }

        /* Update color selection per TPID per port */
        switch (color_select) {
          case BCM_COLOR_PRIORITY:
              cfi_cng &= ~tpid_enable;
              break;
          case BCM_COLOR_INNER_CFI:
              cfi_cng = 0x1;
              break;
          case BCM_COLOR_OUTER_CFI:
              cfi_cng |= tpid_enable;
              break;
          default:
              /* Already checked color_select param */
              /* Should never get here              */ 
              break;
        }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
            if (soc_feature(unit, soc_feature_egr_all_profile)) {
                BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_set(unit,
                    port, EGR_VLAN_CONTROL_1m, CFI_AS_CNGf, cfi_cng));
            } else {
                soc_EGR_VLAN_CONTROL_1m_field32_set(unit, &entry,
                        CFI_AS_CNGf, cfi_cng);
                rv = WRITE_EGR_VLAN_CONTROL_1m(unit,
                        MEM_BLOCK_ANY, port, &entry);
            }
        } else
#endif
        {
            soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &egr_vlan_ctrl,
                    CFI_AS_CNGf, cfi_cng);
            rv = WRITE_EGR_VLAN_CONTROL_1r(unit, port, egr_vlan_ctrl);
        }
        if (BCM_FAILURE(rv)) {
            rv = _bcm_fb2_outer_tpid_entry_delete(unit,tpid_idx);
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return rv;
        }
    }
    
    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trx_vp_tpid_get_all
 * Description:
 *      Retrieve a list of TPIDs available for the given vport.
 * Parameters:
 *      unit         - (IN) Device number
 *      vport        - (IN) Virtual Port identifier (MPLS ot MIM)
 *      size         - (IN) size of the buffer arrays
 *      tpid_array   - (OUT) tpid buffer array
 *      color_array  - (OUT) color buffer array
 *      count        - (OUT) actual number of tpids/colors retrieved
 * Return Value:
 *      BCM_E_XXX
 */
int _bcm_trx_vp_tpid_get_all(int unit, bcm_gport_t vport, int size,
                uint16 *tpid_array, int *color_array, int *count)
{
    int                 vp, rv;
    source_vp_entry_t   svp;
    uint32              tpid_enable=0;
    bcm_port_t          port;
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int cnt;
    uint32 tpid;
    int num_entries;
    int index;

    rv = BCM_E_NONE;
    cnt = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, vport, &modid, &port, &tgid, &vp));

    if (-1 == vp) {
        return BCM_E_PORT;
    }

    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
        uint32 svp_attrs_1[SOC_MAX_MEM_WORDS];

        BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit,
                    MEM_BLOCK_ANY, vp, svp_attrs_1));
        soc_SVP_ATTRS_1m_field_get(unit,
                &svp_attrs_1, TPID_ENABLEf, (uint32 *)&tpid_enable);

        /* max tpid entries limited by width of this field */
        num_entries = soc_mem_field_length(unit,SVP_ATTRS_1m,TPID_ENABLEf);
    } else
#endif
    {
        tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
        /* max tpid entries limited by width of this field */
        num_entries = soc_mem_field_length(unit,SOURCE_VPm,TPID_ENABLEf);
    }

    if (size == 0) {
        for (index = 0; index < num_entries; index++) {
            if (tpid_enable & (1 << index)) {
                cnt++;
            }
        }
        *count = cnt;
        return BCM_E_NONE;
    }
   
    for (index = 0; index < num_entries; index++) {
        if (cnt < size) {
            if (tpid_enable & (1 << index)) {
                rv = READ_EGR_OUTER_TPIDr(unit, index, &tpid);
                if (BCM_FAILURE(rv)) {
                    return (rv);
                }
                tpid_array[cnt] = (uint16)tpid;
                cnt++;
            }
        }
    }
    *count = cnt;

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vp_tpid_delete
 * Description:
 *      Delete allowed TPID for a virtual port.
 * Parameters:
 *      unit  - (IN) Device number
 *      vport - (IN) Virtual Port identifier (WLAN or MIM)
 *      tpid  - (IN) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int _bcm_trx_vp_tpid_delete(int unit, bcm_gport_t vport, uint16 tpid)
{
    int                 vp, rv, tpid_idx;
    source_vp_entry_t   svp;
    uint32              ena_f;
    bcm_port_t          port;
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    int vp_lag_vp;
#endif

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, vport, &modid, &port, &tgid, &vp));
    
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        if (tgid != -1) {
            rv = _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tgid, &vp_lag_vp);
            if (BCM_SUCCESS(rv) &&
                _bcm_vp_used_get(unit, vp_lag_vp, _bcmVpTypeVpLag)) {
                vp = vp_lag_vp;
            }
        }
    }
#endif

    if (-1 == vp) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
        uint32 svp_attrs_1[SOC_MAX_MEM_WORDS];
        uint32 tpid_enable = 0;
        BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit,
                            MEM_BLOCK_ANY, vp, svp_attrs_1));
        tpid_enable = soc_mem_field32_get(unit, SVP_ATTRS_1m, &svp_attrs_1, TPID_ENABLEf);
        ena_f = tpid_enable;
    } else
#endif
    {
        ena_f = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
    }
    _bcm_fb2_outer_tpid_tab_lock(unit);

    rv = _bcm_fb2_outer_tpid_lkup(unit, tpid, &tpid_idx);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return (rv);
    }

#if (defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)) \
    && defined(INCLUDE_L3)
    /* Add TPID for Trill Virtual Port */
    if ((SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit))
         && (BCM_GPORT_IS_TRILL_PORT(vport))
         && (3 == soc_SOURCE_VPm_field32_get(unit, &svp, TPID_SOURCEf))) {
            rv = bcm_td_trill_tpid_delete(unit, vport, tpid_idx);
            if (BCM_FAILURE(rv)) {
                rv = _bcm_fb2_outer_tpid_entry_delete(unit,tpid_idx);
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                return (rv);
            }
    } else
#endif
    {
        if (ena_f & (1 << tpid_idx)) {
            ena_f &= ~(1 << tpid_idx);
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
                rv = soc_mem_field32_modify(unit, SVP_ATTRS_1m,
                        vp, TPID_ENABLEf, ena_f);
            } else
#endif
            {
                soc_SOURCE_VPm_field32_set(unit, &svp, TPID_ENABLEf, ena_f);
            }
            if (!ena_f) {
                soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 0);
            }
            rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);

            if (BCM_FAILURE(rv)) {
                rv = _bcm_fb2_outer_tpid_entry_delete(unit,tpid_idx);
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                return rv;
            }
        } else {
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return BCM_E_NOT_FOUND;
        }
    }

    rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_trx_vp_tpid_delete_all
 * Description:
 *      Delete all allowed TPID for a virtual port.
 * Parameters:
 *      unit  - (IN) Device number
 *      vport - (IN) Virtual Port identifier
 * Return Value:
 *      BCM_E_XXX
 */
int _bcm_trx_vp_tpid_delete_all(int unit, bcm_gport_t vport)
{
    int                 vp, rv = BCM_E_NONE, tpid_idx;
    source_vp_entry_t   svp;
    uint32              ena_f;
    bcm_port_t          port;
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    int vp_lag_vp;
#endif

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, vport, &modid, &port, &tgid, &vp));
    
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        if (tgid != -1) {
            rv = _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tgid, &vp_lag_vp);
            if (BCM_SUCCESS(rv) &&
                _bcm_vp_used_get(unit, vp_lag_vp, _bcmVpTypeVpLag)) {
                vp = vp_lag_vp;
            }
        }
    }
#endif

    if (-1 == vp) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
        uint32 svp_attrs_1[SOC_MAX_MEM_WORDS];
        uint32 tpid_enable = 0;
        BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit,
                            MEM_BLOCK_ANY, vp, svp_attrs_1));
        tpid_enable = soc_mem_field32_get(unit, SVP_ATTRS_1m, &svp_attrs_1, TPID_ENABLEf);
        ena_f = tpid_enable;
    } else
#endif
    {
        ena_f = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
    }
    /* If was not set - do nothing */
    if (!ena_f) {
        return BCM_E_NONE;
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 0);
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
        rv = soc_mem_field32_modify(unit, SVP_ATTRS_1m,
                                    vp, TPID_ENABLEf, 0);
    } else
#endif
    {
        soc_SOURCE_VPm_field32_set(unit, &svp, TPID_ENABLEf, 0);
    }
    BCM_IF_ERROR_RETURN(WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp));

    /* Perform SW delition only after HW was updated */
    _bcm_fb2_outer_tpid_tab_lock(unit);
    tpid_idx = 0;
    while (ena_f) {
        if (ena_f & 1) {
            rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                return (rv);
            }
        }
        ena_f = (ena_f >> 1);
        tpid_idx++;
    }
    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_trx_vp_tpid_set
 * Description:
 *      Set the default Tag Protocol ID for a virtual gport.
 * Parameters:
 *      unit  - Device number
 *      vport - Virtual Port identifier
 *      tpid  - Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int _bcm_trx_vp_tpid_set(int unit, bcm_gport_t vport, uint16 tpid)
{
    int                 vp, rv, tpid_idx, old_idx, islocal;
    source_vp_entry_t   svp;
    uint32              ena_f, evc;
    bcm_port_t          port;
    bcm_module_t        modid, mymodid;
    bcm_trunk_t         tgid;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    egr_vlan_control_1_entry_t vlan_control;
#endif
    int pp_port_flag = 0;
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    int vp_lag_vp;
#endif
    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, vport, &modid, &port, &tgid, &vp));

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        if (tgid != -1) {
            rv = _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tgid, &vp_lag_vp);
            if (BCM_SUCCESS(rv) &&
                _bcm_vp_used_get(unit, vp_lag_vp, _bcmVpTypeVpLag)) {
                vp = vp_lag_vp;
            }
        }
    }
#endif

    if (-1 == vp) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
        uint32 svp_attrs_1[SOC_MAX_MEM_WORDS];
        uint32 tpid_enable = 0;

        BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit,
                            MEM_BLOCK_ANY, vp, svp_attrs_1));
        tpid_enable = soc_mem_field32_get(unit, SVP_ATTRS_1m, &svp_attrs_1, TPID_ENABLEf);
        ena_f = tpid_enable;
    } else
#endif
    {
        ena_f = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
    }

    old_idx = 0;
    if (tgid == -1) {
        BCM_IF_ERROR_RETURN( _bcm_esw_modid_is_local(unit, modid, &islocal));
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mymodid));

        if (!islocal)  {
            if (soc_feature(unit, soc_feature_subtag_coe) ||
                soc_feature(unit, soc_feature_linkphy_coe)) {
            #ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit) &&
                    (_bcm_kt2_mod_is_coe_mod_check(unit, modid) == BCM_E_NONE)) {
                    BCM_IF_ERROR_RETURN(_bcm_kt2_modport_to_pp_port_get(unit,
                        modid, port, &port));
                    pp_port_flag = 1;
                } else {
                    return BCM_E_PORT;
                }
            #endif
            }
        } else {
            while (mymodid < modid) {
                port += 32;
                modid -= 1;
            }
        }
        
        if (islocal || (!islocal && pp_port_flag)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                if (soc_feature(unit, soc_feature_egr_all_profile)) {
                    BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_get(unit,
                        port, EGR_VLAN_CONTROL_1m, OUTER_TPID_INDEXf,
                        (void *)&old_idx));
                } else {
                    BCM_IF_ERROR_RETURN(READ_EGR_VLAN_CONTROL_1m(unit,
                                MEM_BLOCK_ANY, port, &vlan_control));
                    old_idx = soc_EGR_VLAN_CONTROL_1m_field32_get(unit,
                                &vlan_control, OUTER_TPID_INDEXf);
                }
            } else
#endif
            {
                BCM_IF_ERROR_RETURN(READ_EGR_VLAN_CONTROL_1r(unit, port, &evc));
                old_idx =  soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r,
                                             evc, OUTER_TPID_INDEXf);
            }
        }
    }

    _bcm_fb2_outer_tpid_tab_lock(unit);
    if (tgid == -1 && islocal) {
        /* Delete the TPID referenced by EGR_VLAN_CONTROL_1r */
        rv = _bcm_fb2_outer_tpid_entry_delete(unit, old_idx);
        if (BCM_FAILURE(rv)) {
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return (rv);
        }
    }

    /* Delete the TPID referenced by SOURCE_VP table */
    tpid_idx = 0;
    while (ena_f) {
        if (ena_f & 1) {
            rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                return (rv);
            }
        }
        ena_f = ena_f >> 1;
        tpid_idx++;
    }

    /* Add TPID */
    rv = _bcm_fb2_outer_tpid_entry_add(unit, tpid, &tpid_idx);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return (rv);
    }

    if (tgid == -1 && islocal) {
        /* Add TPID reference for EGR_VLAN_CONTROL_1. 
         * The first insertion and second insertion will return
         * the same status because we are adding the same TPID value.
         */
        rv = _bcm_fb2_outer_tpid_entry_add(unit, tpid, &tpid_idx);
        if (BCM_FAILURE(rv)) {
            _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return (rv);
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
            if (soc_feature(unit, soc_feature_egr_all_profile)) {
                BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_set(unit,
                    port, EGR_VLAN_CONTROL_1m, OUTER_TPID_INDEXf, tpid_idx));
            } else {
                soc_EGR_VLAN_CONTROL_1m_field32_set(unit, &vlan_control,
                        OUTER_TPID_INDEXf, tpid_idx);
                rv = WRITE_EGR_VLAN_CONTROL_1m(unit,
                            MEM_BLOCK_ANY, port, &vlan_control);
            }
        } else
#endif
        {
            soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &evc,
                              OUTER_TPID_INDEXf, tpid_idx);
            rv = WRITE_EGR_VLAN_CONTROL_1r(unit, port, evc);
        }
        if (BCM_FAILURE(rv)) {
            _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
            _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return (rv);
        }
    }
#if (defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)) \
    && defined(INCLUDE_L3)
    /* Set TPID for Trill Virtual Port */
    if ((SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit))
         && (BCM_GPORT_IS_TRILL_PORT(vport))
         && (3 == soc_SOURCE_VPm_field32_get(unit, &svp, TPID_SOURCEf))) {
            rv = bcm_td_trill_tpid_set(unit, vport, tpid_idx);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                return (rv);
            }
    } else
#endif
    {
        ena_f = 1 << tpid_idx;
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
            rv = soc_mem_field32_modify(unit, SVP_ATTRS_1m,
                    vp, TPID_ENABLEf, ena_f);
        } else
#endif
        {
            soc_SOURCE_VPm_field32_set(unit, &svp, TPID_ENABLEf, ena_f);
        }
        soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 1);
 
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp); 
        if (BCM_FAILURE(rv)) {
            _bcm_fb2_outer_tpid_entry_delete(unit, tpid_idx);
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return (rv);
        }
    }

    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trx_vp_tpid_get
 * Description:
 *      Get the default Tag Protocol ID for a virtual port.
 * Parameters:
 *      unit  - Device number
 *      vport - Virtual Port identifier
 *      tpid  - Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int _bcm_trx_vp_tpid_get(int unit, bcm_gport_t vport, uint16 *tpid)
{
    int                 tpid_idx, vp, islocal;
    uint32              evc;
    bcm_port_t          port;
    bcm_module_t        modid, mymodid;
    bcm_trunk_t         tgid;
    source_vp_entry_t   svp;
    uint32              ena_f; 
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    int rv;
    int vp_lag_vp;
#endif

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, vport, &modid, &port, &tgid, &vp));

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_lag)) {
        if (tgid != -1) {
            rv = _bcm_esw_trunk_tid_to_vp_lag_vp(unit, tgid, &vp_lag_vp);
            if (BCM_SUCCESS(rv) &&
                _bcm_vp_used_get(unit, vp_lag_vp, _bcmVpTypeVpLag)) {
                vp = vp_lag_vp;
            }
        }
    }
#endif

    if (-1 == vp) { 
        return BCM_E_PORT;
    }
    if (tgid == -1) {
        BCM_IF_ERROR_RETURN( _bcm_esw_modid_is_local(unit, modid, &islocal));
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mymodid));
        if (!islocal)  {
            if (soc_feature(unit, soc_feature_subtag_coe) ||
                soc_feature(unit, soc_feature_linkphy_coe)) {
            #ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit) &&
                    (_bcm_kt2_mod_is_coe_mod_check(unit, modid) == BCM_E_NONE)) {
                    BCM_IF_ERROR_RETURN(_bcm_kt2_modport_to_pp_port_get(unit,
                        modid, port, &port));
                } else {
                    return BCM_E_PORT;
                }
            #endif
            }
        } else {
            while (mymodid < modid) {
                port += 32;
                modid -= 1;
            }
        }
    }

#if (defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)) \
    && defined(INCLUDE_L3)
    if ((SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit))
         && (BCM_GPORT_IS_TRILL_PORT(vport))) {
        int rv;
        rv = bcm_td_trill_tpid_get(unit, vport, &tpid_idx);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }   
    } else
#endif
    {
        if (tgid == -1 && islocal) {
            if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                if (soc_feature(unit, soc_feature_egr_all_profile)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_port_egr_lport_field_get(
                        unit, port, EGR_VLAN_CONTROL_1m,
                        OUTER_TPID_INDEXf, (uint32 *)&tpid_idx));
                } else {
                    egr_vlan_control_1_entry_t vlan_control;
                    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_VLAN_CONTROL_1m,
                                         MEM_BLOCK_ANY, port, &vlan_control));
                    soc_mem_field_get(unit, EGR_VLAN_CONTROL_1m,
                                      (uint32*) &vlan_control, OUTER_TPID_INDEXf,
                                      (uint32 *) &tpid_idx);
                }
            } else {
                SOC_IF_ERROR_RETURN(READ_EGR_VLAN_CONTROL_1r(unit, port, &evc));
                tpid_idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r,
                                      evc, OUTER_TPID_INDEXf);
            }
        } else {
            BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_IS_VALID(unit, SVP_ATTRS_1m)) {
                uint32 svp_attrs_1[SOC_MAX_MEM_WORDS];
                uint32 tpid_enable = 0;

                BCM_IF_ERROR_RETURN(READ_SVP_ATTRS_1m(unit,
                            MEM_BLOCK_ANY, vp, svp_attrs_1));
                soc_SVP_ATTRS_1m_field_get(unit,
                        &svp_attrs_1, TPID_ENABLEf, (uint32 *)&tpid_enable);

                ena_f = tpid_enable;
            } else
#endif
            {
                ena_f = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
            }
            tpid_idx = -1;
            while (ena_f) {
                tpid_idx++;
                if (ena_f & 1) {
                    break;
                }
                ena_f = ena_f >> 1;
            }
            if (tpid_idx == -1) {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    _bcm_fb2_outer_tpid_entry_get(unit, tpid, tpid_idx);

    return (BCM_E_NONE);
}



/*
 * Function:
 *      _bcm_trx_egr_src_port_outer_tpid_set
 * Description:
 *      Enable/Disable  outer tpid on all egress ports 
 * Parameters:
 *      unit - Device number
 *      tpid_index - Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_trx_egr_src_port_outer_tpid_set(int unit, int tpid_index, int enable)
{
    uint32      egr_src_port;
    uint32      tpid_enable;
    uint32      new_tpid_enable;
    bcm_port_t  port;
    soc_reg_t   reg;
    soc_pbmp_t  temp_pbmp;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    egr_port_1_entry_t  pe;
#endif

    if (SOC_IS_TD_TT(unit) || SOC_IS_HURRICANE(unit) || SOC_IS_TRIUMPH3(unit) 
        || SOC_IS_KATANAX(unit) || SOC_IS_HURRICANE2(unit)) {
        reg = EGR_PORT_1r;
    } else {
        reg = EGR_SRC_PORTr;
    }

    BCM_PBMP_CLEAR(temp_pbmp);
    BCM_PBMP_ASSIGN(temp_pbmp, PBMP_E_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update (unit, &temp_pbmp);
    }
#endif

    PBMP_ITER(temp_pbmp, port) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_td2p_style_egr_outer_tpid)) {
            BCM_IF_ERROR_RETURN(READ_EGR_PORT_1m(unit, MEM_BLOCK_ANY,
                        port, &pe));
            tpid_enable = soc_mem_field32_get(unit, EGR_PORT_1m, &pe,
                    OUTER_TPID_ENABLEf);
        } else 
#endif
        {
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &egr_src_port));
            tpid_enable = soc_reg_field_get(unit, reg,
                    egr_src_port, OUTER_TPID_ENABLEf);
        }

        if (enable) {
            new_tpid_enable = (tpid_enable | (1 << tpid_index));
        } else {
            new_tpid_enable = (tpid_enable & ~(1 << tpid_index));
        } 

        if (new_tpid_enable != tpid_enable) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_td2p_style_egr_outer_tpid)) {
                soc_mem_field32_set(unit, EGR_PORT_1m, &pe, 
                        OUTER_TPID_ENABLEf, new_tpid_enable);
                BCM_IF_ERROR_RETURN(WRITE_EGR_PORT_1m(unit, 
                            MEM_BLOCK_ANY, port, &pe));
            } else 
#endif
            {
                soc_reg_field_set(unit, reg, &egr_src_port,
                        OUTER_TPID_ENABLEf, new_tpid_enable);
                BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, egr_src_port));
            }
        }
    } 
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_source_trunk_map_set
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
_bcm_trx_source_trunk_map_set(int unit, bcm_port_t port, 
                              soc_field_t field, uint32 value)
{
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
    int index, trunk_id = -1, id = -1;

#if defined BCM_HGPROXY_COE_SUPPORT
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        /* Resolve the port */
        SOC_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &mod_out, 
                                   &port_out, &trunk_id, &id));
    } else
#endif
    {
        if (BCM_GPORT_IS_SET(port) &&
            ((soc_feature(unit, soc_feature_shg_support_remote_port) &&
             (field == CLASS_IDf)) ||
            (soc_feature(unit, soc_feature_stm_tpid_support)))) {
            SOC_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &mod_out,
                                       &port_out, &trunk_id, &id));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
            /* Port sanity check. */
            if (SOC_PORT_ADDRESSABLE(unit, port)) {
                BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mod_out));
                port_out = port;
            } else {
                return (BCM_E_PORT);
            }
        }
    }
    /* Calculate table index. */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, 
        mod_out, port_out, &index));

    return soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                  index, field, value);
}

/*
 * Function:
 *      _bcm_esw_source_trunk_map_get
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
_bcm_trx_source_trunk_map_get(int unit, bcm_port_t port, 
                              soc_field_t field, uint32 *value)
{
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS];
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
    int index, trunk_id = -1, id = -1;

    /* Input parameters check */
    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    /* Memory field is valid check. */ 
    if (!SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, field)) {
        return (BCM_E_UNAVAIL);
    }

#if defined BCM_HGPROXY_COE_SUPPORT
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        /* Resolve the port */
        SOC_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &mod_out, 
                                   &port_out, &trunk_id, &id));
    } else
#endif
    {
        if (BCM_GPORT_IS_SET(port) &&
            ((soc_feature(unit, soc_feature_shg_support_remote_port) &&
             (field == CLASS_IDf)) ||
            (soc_feature(unit, soc_feature_stm_tpid_support)))) {
            SOC_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &mod_out,
                                       &port_out, &trunk_id, &id));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
            /* Port sanity check. */
            if (SOC_PORT_ADDRESSABLE(unit, port)) {
                BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &mod_out));
                port_out = port;
            } else {
                return (BCM_E_PORT);
            }
        }
    }

    

    /* Get table index. */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit,
        mod_out, port_out, &index));

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
 *     _bcm_trx_port_force_vlan_set
 * Description:
 *     To set the force vlan attribute of a port
 * Parameters:
 *     unit        device number
 *     port        port number
 *     vlan        vlan identifier
 *                 (0 - 4095) - use this VLAN id if egress packet is tagged
 *     pkt_prio    egress packet priority (-1, 0..7)
 *                 any negative priority value disable the priority
 *                 override if the egress packet is tagged
 *     flags       bit fields
 *                 BCM_PORT_FORCE_VLAN_ENABLE - enable force vlan on this
 *                                              port
 *                 BCM_PORT_FORCE_VLAN_UNTAG - egress untagged when force
 *                                             vlan is enabled on this port
 *
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_trx_port_force_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vlan,
                            int pkt_prio, uint32 flags)
{
    bcm_port_cfg_t pcfg;
    uint32 reg_val;
    egr_pvlan_eport_control_entry_t entry;
    int local_port = port;

    sal_memset(&entry, 0, sizeof(entry));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
       if ((soc_feature(unit, soc_feature_channelized_switching)) &&
           BCM_GPORT_IS_SET(port) &&
           _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

           /* Get channel number for subport*/
           BCM_IF_ERROR_RETURN(
               _bcm_coe_subtag_subport_port_subport_num_get(unit,
               port, NULL, &local_port));
       }
#endif

    BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_port_cfg_get(unit, port,
                                                             &pcfg));
    reg_val = 0;

    if (flags & BCM_PORT_FORCE_VLAN_ENABLE) {
        if (!BCM_VLAN_VALID(vlan)) {
            return BCM_E_PARAM;
        }

        if (pkt_prio > 7) {
            return BCM_E_PARAM;
        }

        pcfg.pc_pvlan_enable = 1;
        if(!soc_feature(unit, soc_feature_egr_pvlan_eport_control_is_mem)) {
            soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                              PVLAN_ENABLEf, 1);
        } else {
            soc_mem_field32_set(unit, EGR_PVLAN_EPORT_CONTROLm, &entry,
                              PVLAN_ENABLEf, 1);
        }
        if (!(flags & BCM_PORT_FORCE_VLAN_UNTAG)) {
            if(!soc_feature(unit, soc_feature_egr_pvlan_eport_control_is_mem)) {
                soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                                  PVLAN_PVIDf, vlan);
            } else {
                soc_mem_field32_set(unit, EGR_PVLAN_EPORT_CONTROLm, &entry,
                                  PVLAN_PVIDf, vlan);
            }
            if (pkt_prio >= 0) {
                if(!soc_feature(unit, soc_feature_egr_pvlan_eport_control_is_mem)) {
                    soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                                      PVLAN_PRIf, pkt_prio);
                    soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                                      PVLAN_RPEf, 1);
                } else {
                    soc_mem_field32_set(unit, EGR_PVLAN_EPORT_CONTROLm, &entry,
                                      PVLAN_PRIf, pkt_prio);
                    soc_mem_field32_set(unit, EGR_PVLAN_EPORT_CONTROLm, &entry,
                                      PVLAN_RPEf, 1);
                }
            }
        } else {
            if(!soc_feature(unit, soc_feature_egr_pvlan_eport_control_is_mem)) {
                soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                                  PVLAN_UNTAGf, 1);
            } else {
                soc_mem_field32_set(unit, EGR_PVLAN_EPORT_CONTROLm, &reg_val,
                                  PVLAN_UNTAGf, 1);
            }
        }
    } else {
        pcfg.pc_pvlan_enable = 0;
    }

    BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));
    if(!soc_feature(unit, soc_feature_egr_pvlan_eport_control_is_mem)) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_PVLAN_EPORT_CONTROLr(unit, local_port, reg_val));
    } else {
        SOC_IF_ERROR_RETURN(WRITE_EGR_PVLAN_EPORT_CONTROLm(unit, -1, local_port, &entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_trx_port_force_vlan_get
 * Description:
 *     To get the force vlan attribute of a port
 * Parameters:
 *     unit        device number
 *     port        port number
 *     vlan        pointer to vlan identifier
 *                 (0 - 4095) - use this VLAN id if egress packet is tagged
 *                 valid only when BCM_PORT_FORCE_VLAN_ENABLE is set and
 *                 BCM_PORT_FORCE_VLAN_UNTAG is clear
 *     pkt_prio    egress packet priority (-1, 0 - 7)
 *                 valid only when BCM_PORT_FORCE_VLAN_ENABLE is set and
 *                 BCM_PORT_FORCE_VLAN_UNTAG is clear
 *     flags       bit fields
 *                 BCM_PORT_FORCE_VLAN_ENABLE - enable force vlan on this
 *                                              port
 *                 BCM_PORT_FORCE_VLAN_UNTAG - egress untagged when force
 *                                             vlan is enabled on this port
 *
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_trx_port_force_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vlan,
                            int *pkt_prio, uint32 *flags)
{
    bcm_port_cfg_t pcfg;
    uint32 reg_val;
    egr_pvlan_eport_control_entry_t entry;
    int local_port = port;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (!vlan || !pkt_prio || !flags) {
        return BCM_E_PARAM;
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
       if ((soc_feature(unit, soc_feature_channelized_switching)) &&
           BCM_GPORT_IS_SET(port) &&
           _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

           /* Get channel number for subport */
           BCM_IF_ERROR_RETURN(
               _bcm_coe_subtag_subport_port_subport_num_get(unit,
               port, NULL, &local_port));
       }
#endif

    BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_port_cfg_get(unit, port,
                                                             &pcfg));

    *flags = 0;
    if (pcfg.pc_pvlan_enable) {
        *flags |= BCM_PORT_FORCE_VLAN_ENABLE;

        if (!soc_feature(unit, soc_feature_egr_pvlan_eport_control_is_mem)) {
            SOC_IF_ERROR_RETURN(READ_EGR_PVLAN_EPORT_CONTROLr(unit, local_port,
                                                              &reg_val));
            if(!soc_reg_field_get(unit, EGR_PVLAN_EPORT_CONTROLr, reg_val,
                               PVLAN_UNTAGf)) {
                *vlan = soc_reg_field_get(unit, EGR_PVLAN_EPORT_CONTROLr,
                                          reg_val, PVLAN_PVIDf);
                if (soc_reg_field_get(unit, EGR_PVLAN_EPORT_CONTROLr, reg_val,
                                      PVLAN_RPEf)) {
                    *pkt_prio = soc_reg_field_get(unit,
                                                  EGR_PVLAN_EPORT_CONTROLr,
                                                  reg_val, PVLAN_PRIf);
                } else {
                    *pkt_prio = -1;
                }
            } else {
                *flags |= BCM_PORT_FORCE_VLAN_UNTAG;
            }
        } else {
            sal_memset(&entry, 0, sizeof(entry));
            SOC_IF_ERROR_RETURN(READ_EGR_PVLAN_EPORT_CONTROLm(unit, -1, local_port,
                                                              &entry));
            if(!soc_mem_field32_get(unit, EGR_PVLAN_EPORT_CONTROLm, &entry,
                               PVLAN_UNTAGf)) {
                *vlan = soc_mem_field32_get(unit, EGR_PVLAN_EPORT_CONTROLm,
                                          &entry, PVLAN_PVIDf);
                if (soc_mem_field32_get(unit, EGR_PVLAN_EPORT_CONTROLm, &entry,
                                      PVLAN_RPEf)) {
                    *pkt_prio = soc_mem_field32_get(unit,
                                                  EGR_PVLAN_EPORT_CONTROLm,
                                                  &entry, PVLAN_PRIf);
                } else {
                    *pkt_prio = -1;
                }
            } else {
                *flags |= BCM_PORT_FORCE_VLAN_UNTAG;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trx_port_dtag_mode_set
 * Description:
 *      Set the double-tagging mode of a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - Double-tagging mode, one of:
 *              BCM_PORT_DTAG_MODE_NONE            No double tagging
 *              BCM_PORT_DTAG_MODE_INTERNAL        Service Provider port
 *              BCM_PORT_DTAG_MODE_EXTERNAL        Customer port
 *              BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG  Remove customer tag
 *              BCM_PORT_DTAG_ADD_EXTERNAL_TAG     Add customer tag
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      BCM_PORT_DTAG_MODE_INTERNAL is for service provider ports.
 *              A tag will be added if the packet does not already
 *              have the internal TPID (see bcm_port_tpid_set below).
 *              Internally this sets DT_MODE and clears IGNORE_TAG.
 *      BCM_PORT_DTAG_MODE_EXTERNAL is for customer ports.
 *              The service provider TPID will always be added
 *              (see bcm_port_tpid_set below).
 *              Internally this sets DT_MODE and sets IGNORE_TAG.
 */

int
_bcm_trx_port_dtag_mode_set(int unit, bcm_port_t port, int mode)
{
    bcm_vlan_action_set_t action;
    _bcm_port_info_t      *pinfo;
    int                   color_mode;
#if defined(BCM_TRX_SUPPORT)
    int                   prev_mode;
    int                   index;
#endif    
    int                   rv;

    if (!IS_HG_PORT(unit, port)) {
        /* Modify default ingress actions */
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_default_action_get(unit, port, &action));
        action.dt_outer      = bcmVlanActionNone;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_inner      = bcmVlanActionNone;
        action.dt_inner_prio = bcmVlanActionNone;
        action.ot_outer      = bcmVlanActionNone;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.ot_inner      = bcmVlanActionNone;
        action.it_outer      = bcmVlanActionAdd;
        action.it_inner      = bcmVlanActionNone;
        action.it_inner_prio = bcmVlanActionNone;
        action.ut_outer      = bcmVlanActionAdd;
        if (mode & BCM_PORT_DTAG_ADD_EXTERNAL_TAG) {
            action.ut_inner = bcmVlanActionAdd;
        } else {
            action.ut_inner = bcmVlanActionNone;
        }
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_KATANA2(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            rv = _bcm_trx_vlan_port_default_action_profile_set(unit, port, &action);
        } else
#endif
        {
            rv = _bcm_trx_vlan_port_default_action_set(unit, port, &action);
        }
        BCM_IF_ERROR_RETURN(rv);

        /* Modify default egress actions */
        rv = _bcm_trx_vlan_port_egress_default_action_get(unit, port, &action);
        BCM_IF_ERROR_RETURN(rv);

        action.ot_inner      = bcmVlanActionNone;
        if (mode & BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG) {
            action.dt_inner = bcmVlanActionDelete;
            action.dt_inner_prio = bcmVlanActionDelete;
        } else {
            action.dt_inner = bcmVlanActionNone;
            action.dt_inner_prio = bcmVlanActionNone;
        }
        action.dt_outer      = bcmVlanActionNone;
        action.dt_outer_prio = bcmVlanActionNone;
        action.ot_outer      = bcmVlanActionNone;
        action.ot_outer_prio = bcmVlanActionNone;
        rv = _bcm_trx_vlan_port_egress_default_action_set(unit, port, &action);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = _bcm_port_info_get(unit, port, &pinfo);
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_TRX_SUPPORT)
    prev_mode = pinfo->dtag_mode;
#endif    
    pinfo->dtag_mode = mode;
    mode &= BCM_PORT_DTAG_MODE_INTERNAL | BCM_PORT_DTAG_MODE_EXTERNAL;

#if defined(BCM_TRX_SUPPORT)
    /* return if previous mode and current mode are same */
    if(prev_mode & mode) {
        return  BCM_E_NONE;
    }
#endif
    if (mode == BCM_PORT_DTAG_MODE_NONE) {
        /* Set the default outer TPID. */
        rv = bcm_esw_port_tpid_set(unit, port, 
                                   _bcm_fb2_outer_tpid_default_get(unit));
        BCM_IF_ERROR_RETURN(rv);
    } else if (mode == BCM_PORT_DTAG_MODE_INTERNAL) {
        /* Add the default outer TPID. */
        rv = bcm_esw_switch_control_port_get(unit, port, 
                                             bcmSwitchColorSelect, &color_mode);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcm_esw_port_tpid_add(unit, port, _bcm_fb2_outer_tpid_default_get(unit),
                                   color_mode);
        BCM_IF_ERROR_RETURN(rv);

    } else if (mode == BCM_PORT_DTAG_MODE_EXTERNAL) {
        /* Disable all outer TPIDs. */
        BCM_IF_ERROR_RETURN (bcm_esw_port_tpid_delete_all(unit, port));
    }
#if defined(BCM_TRX_SUPPORT)
    /*  Increase the ref count of the default tpid if the mode
     *  is changing to internal/none from external */
    if (prev_mode & BCM_PORT_DTAG_MODE_EXTERNAL) { 
          rv = _bcm_fb2_outer_tpid_lkup(unit, _bcm_fb2_outer_tpid_default_get(unit),
                                        &index);
          BCM_IF_ERROR_RETURN(rv);
        
          rv = _bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1);
          BCM_IF_ERROR_RETURN(rv);
    }
#endif    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trx_port_control_egress_class_select_set
 * Description:
 *      Helper funtion to modify fields in FP_I2E_CLASSID_SELECT/IFP_I2E_CLASSID_SELECTm memory.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) Port number
 *      value - (IN) new field value
 * Return Value:
 *      BCM_E_XXX 
 */
int
_bcm_trx_port_control_egress_class_select_set(int unit, bcm_port_t port, 
                                              uint32 value)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;

    if (SOC_MEM_IS_VALID(unit, FP_I2E_CLASSID_SELECTm)) {
        mem = FP_I2E_CLASSID_SELECTm;
    }
    else if (SOC_MEM_IS_VALID(unit, IFP_I2E_CLASSID_SELECTm)) {
        /* For TOMAHAWK IFP_I2E_CLASSID_SELECTm is used */
        mem = IFP_I2E_CLASSID_SELECTm;
    }
    else {
        return SOC_E_UNAVAIL;
    }

    /* Check for valid input for I2E_CLASSID_SELECT*/
    /* coverity[unsigned_compare] : FALSE */
    if ((value < bcmPortEgressClassSelectNone) ||
        (value >= bcmPortEgressClassSelectCount)) {
        return (BCM_E_PARAM);
    }

    switch (value) {
       case bcmPortEgressClassSelectFieldIngress:
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TRIDENT2(unit)) {
               value = 0xf;  /* IFD_CLASS_ID bit */
            } else  
#endif /* BCM_TRIDENT2_SUPPORT */
               return BCM_E_UNAVAIL;
            break;
       default:
            break;
    } 

    /* Port sanity check. */
    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return (BCM_E_PORT);
    }

#ifdef BCM_TRIDENT3_SUPPORT
    /* In TD3, IFP_I2E_CLASSID_SELECT is indexed by    *
     * LPORT_PROFILE_TABLE.MISC_PORT_PROFILE_INDEX.    *
     *                                                 *
     * The LPORT_PROFILE_TABLE is indexed from the     *
     * SOURCE_TRUNK_MAP_TABLE Lport_profile_idx field. */
     
    if (SOC_IS_TRIDENT3X(unit) && (mem == IFP_I2E_CLASSID_SELECTm)) {
        int src_index;
        uint32 src_ent[SOC_MAX_MEM_FIELD_WORDS];
        int src_tbl_is_stm = 1;
        soc_mem_t src_mem = SOURCE_TRUNK_MAP_TABLEm;

        lport_tab_entry_t lport_entry;
        uint32 lport_index = 0;

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_lport_prof_src_check(unit, _BCM_ESW_PT_PORT,
            &src_tbl_is_stm));

        if (src_tbl_is_stm) {
            int modid = 0;
            BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
            BCM_IF_ERROR_RETURN
                (_bcm_esw_src_mod_port_table_index_get(unit, modid, port,
                                                       &src_index));
        } else {
            src_mem = ING_DEVICE_PORTm;
            src_index = port;
        }

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, src_mem, MEM_BLOCK_ANY, src_index, src_ent));
        lport_index = soc_mem_field32_get(unit, src_mem,
                                          src_ent, LPORT_PROFILE_IDXf);

        BCM_IF_ERROR_RETURN(READ_LPORT_TABm(unit, MEM_BLOCK_ANY, 
                                       lport_index, &lport_entry));

        soc_LPORT_TABm_field32_set(unit, &lport_entry, 
                       MISC_PORT_PROFILE_INDEXf, port);

        BCM_IF_ERROR_RETURN(WRITE_LPORT_TABm(unit, MEM_BLOCK_ANY, 
                                     lport_index, &lport_entry));
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    /* Memory field is valid check. */
    if (!SOC_MEM_FIELD_VALID(unit, mem, PORT_SELf)) {
        return (BCM_E_UNAVAIL);
    }

    rv = soc_mem_field32_modify(unit, mem, port, PORT_SELf, value);

    return rv;
}

/*
 * Function:
 *      _bcm_trx_port_control_egress_class_select_get
 * Description:
 *      Helper funtion to get fields in FP_I2E_CLASSID_SELECT/IFP_I2E_CLASSID_SELECTm memory.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) Port number
 *      value - (OUT) New field value
 * Return Value:
 *      BCM_E_XXX 
 */
int
_bcm_trx_port_control_egress_class_select_get(int unit, bcm_port_t port, 
                                              uint32 *value)
{
    fp_i2e_classid_select_entry_t fp_ent;
    ifp_i2e_classid_select_entry_t ifp_ent;
    soc_mem_t mem;
    void *ptr_ent;

    if (SOC_MEM_IS_VALID(unit, FP_I2E_CLASSID_SELECTm)) {
        mem = FP_I2E_CLASSID_SELECTm;
        ptr_ent = (void*) &fp_ent;
    }
    else if (SOC_MEM_IS_VALID(unit, IFP_I2E_CLASSID_SELECTm)) {
        /* For TOMAHAWK IFP_I2E_CLASSID_SELECTm is used */
        mem = IFP_I2E_CLASSID_SELECTm;
        ptr_ent = (void*) &ifp_ent;
    }
    else {
        return SOC_E_UNAVAIL;
    }

    /* Memory field is valid check. */
    if (!SOC_MEM_FIELD_VALID(unit, mem, PORT_SELf)) {
        return (BCM_E_UNAVAIL);
    }

    /* Port sanity check. */
    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return (BCM_E_PORT);
    }

    /* Read table entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, ptr_ent));

    /* Read requested field value. */
    *value = soc_mem_field32_get(unit, mem, ptr_ent, PORT_SELf);
    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_trx_port_control_higig_class_select_set
 * Description:
 *      Helper funtion to modify fields in FP_HG_CLASSID_SELECT memory.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) Port number
 *      value - (IN) new field value
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_trx_port_control_higig_class_select_set(int unit, bcm_port_t port,
                                              uint32 value)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;

    /* Check for valid input for I2E_CLASSID_SELECT*/
    /* coverity[unsigned_compare] : FALSE */
    if ((value < bcmPortFieldHiGigClassSelectNone) ||
        (value >= bcmPortFieldHiGigClassSelectCount)) {
        return (BCM_E_PARAM);
    }

    /* Value 2 and 3 are reserved for TD2/TD2+/TH/APACHE chips*/
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        if ((value == bcmPortFieldHiGigClassSelectL3Egress) ||
            (value == bcmPortFieldHiGigClassSelectL3EgressIntf)) {
            return (BCM_E_PARAM);
        }
    }
#endif

    if (SOC_IS_TOMAHAWKX(unit)) {
        mem = IFP_HG_CLASSID_SELECTm;
    } else {
        mem = FP_HG_CLASSID_SELECTm;
    }

    /* Memory field is valid check. */
    if (!SOC_MEM_FIELD_VALID(unit, mem, PORT_SELf)) {
        return (BCM_E_UNAVAIL);
    }

    /* Port sanity check. */
    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return (BCM_E_PORT);
    }

    rv = soc_mem_field32_modify(unit, mem, port, PORT_SELf, value);

    return rv;
}

/*
 * Function:
 *      _bcm_trx_port_control_higig_class_select_get
 * Description:
 *      Helper funtion to get fields in FP_HG_CLASSID_SELECT memory.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) Port number
 *      value - (OUT) New field value
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_trx_port_control_higig_class_select_get(int unit, bcm_port_t port, 
                                              uint32 *value)
{
    soc_mem_t mem;

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        ifp_hg_classid_select_entry_t ent;
        mem = IFP_HG_CLASSID_SELECTm;

        /* Memory field is valid check. */
        if (!SOC_MEM_FIELD_VALID(unit, mem, PORT_SELf)) {
            return (BCM_E_UNAVAIL);
        }

        /* Port sanity check. */
        if (!SOC_PORT_ADDRESSABLE(unit, port)) {
            return (BCM_E_PORT);
        }

        /* Read table entry. */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &ent));

        /* Read requested field value. */
        *value = soc_mem_field32_get(unit, mem, &ent, PORT_SELf);
    } else 
#endif
    {
        fp_hg_classid_select_entry_t ent;
        mem = FP_HG_CLASSID_SELECTm;

        /* Memory field is valid check. */
        if (!SOC_MEM_FIELD_VALID(unit, mem, PORT_SELf)) {
            return (BCM_E_UNAVAIL);
        }

        /* Port sanity check. */
        if (!SOC_PORT_ADDRESSABLE(unit, port)) {
            return (BCM_E_PORT);
        }

        /* Read table entry. */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &ent));

        /* Read requested field value. */
        *value = soc_mem_field32_get(unit, mem, &ent, PORT_SELf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trx_source_trunk_map_lport_all_set
 * Purpose:
 *      Set the LPORT profile index reference for all entries
 *      in the Source Trunk Map Table.
 *      Helper routine.
 * Parameters:
 *      unit             - (IN) Device number
 *      lport_index      - (IN) LPORT Index
 *      stm_num_entries  - (OUT) Returns number of entries in table
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_trx_source_table_lport_all_set(int unit, uint32 lport_index,
                                   int *src_num_entries)
{
    int rv = BCM_E_NONE;
    int i, m;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int pipe;
    soc_info_t *si;
    source_trunk_map_table_entry_t stm_ent;
    int stm_entry_per_pipe;
#endif 
    soc_mem_t src_mem[2] = {SOURCE_TRUNK_MAP_TABLEm, INVALIDm};
    uint32 *src_entry;
    int index_min, index_max;
    uint32 *stm_buf;

#ifdef BCM_TOMAHAWK3_SUPPORT
    /* This code is added to support the non-contiguous pipes (0, 1, 6,7 half chip or
       bond option 0, 1, 4, 5) in TH3. Since this memory is addr_split in TH3, we cannot
       use mem_read/write_range as only certain blocks of entries are valid. With this
       check of checking if the pipe is active we make sure that only the entries which
       are valid are accessed
    */
    if (SOC_IS_TOMAHAWK3(unit)) {
        
        si = &SOC_INFO(unit);
        index_min = soc_mem_index_min(unit, src_mem[0]);
        index_max = soc_mem_index_max(unit, src_mem[0]);
        *src_num_entries = index_max - index_min + 1;
        stm_entry_per_pipe = *src_num_entries / _TH3_PIPES_PER_DEV;
        for (i = 0; i < *src_num_entries; i++) {
            pipe = i / stm_entry_per_pipe;
            if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
                continue;
            }
            SOC_IF_ERROR_RETURN(
                READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                       i, &stm_ent));

            soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &stm_ent,
                                               LPORT_PROFILE_IDXf,
                                               lport_index);

            SOC_IF_ERROR_RETURN(WRITE_SOURCE_TRUNK_MAP_TABLEm(unit,MEM_BLOCK_ALL,
                                               i, &stm_ent));
        }
        return BCM_E_NONE;
    }
    else
#endif
    {
        if (soc_mem_is_valid(unit, ING_DEVICE_PORTm)) {
            src_mem[1] = ING_DEVICE_PORTm;
        }

        for (m = 0; m < 2; m++) {
            if (src_mem[m] == INVALIDm) {
                continue;
            }

            index_min = soc_mem_index_min(unit, src_mem[m]);
            index_max = soc_mem_index_max(unit, src_mem[m]);
            *src_num_entries = index_max - index_min + 1;
            stm_buf = soc_cm_salloc(unit,
                                    (*src_num_entries) *
                                    (WORDS2BYTES(soc_mem_entry_words(unit, src_mem[m]))),
                                    "Source Trunk Map table buffer");
            if (stm_buf == NULL) {
                return BCM_E_MEMORY;
            }

            soc_mem_lock(unit, src_mem[m]);
            rv = soc_mem_read_range(unit, src_mem[m], MEM_BLOCK_ANY,
                                    index_min, index_max,
                                    stm_buf);
            if (SOC_SUCCESS(rv)) {
                for (i = 0; i < *src_num_entries; i++) {
                    src_entry =
                        soc_mem_table_idx_to_pointer(unit, src_mem[m], void *, stm_buf, i);
                    soc_mem_field32_set(unit, src_mem[m], src_entry, LPORT_PROFILE_IDXf,
                                        lport_index);
                }
                rv = soc_mem_write_range(unit, src_mem[m], MEM_BLOCK_ANY,
                                        index_min, index_max,
                                         stm_buf);
            }

            soc_mem_unlock(unit, src_mem[m]);
            soc_cm_sfree(unit, stm_buf);

            if (BCM_FAILURE(rv)) {
                break;
            }
        }

    }
    return rv;
}

/*
 * Function:
 *      _bcm_trx_lport_tab_default_entry_index_get
 * Purpose:
 *      Return the default LPORT profile entry index.
 * Parameters:
 *      unit  - (IN) Device number
 * Returns:
 *      Default LPORT profile entry index.
 */
int
_bcm_trx_lport_tab_default_entry_index_get(int unit)
{
    return soc_mem_index_min(unit, LPORT_TABm);
}

/*
 * Function:
 *      _bcm_trx_egr_lport_tab_default_entry_index_get
 * Purpose:
 *      Return the default LPORT profile entry index.
 * Parameters:
 *      unit  - (IN) Device number
 * Returns:
 *      Default LPORT profile entry index.
 */
int
_bcm_trx_egr_lport_tab_default_entry_index_get(int unit)
{
    return soc_mem_index_min(unit, EGR_LPORT_PROFILEm);
}

/*
 * Function:
 *      _bcm_trx_rtag7_tab_default_entry_index_get
 * Purpose:
 *      Return the default RTAG7 profile entry index.
 * Parameters:
 *      unit  - (IN) Device number
 * Returns:
 *      Default LPORT profile entry index.
 */
int
_bcm_trx_rtag7_tab_default_entry_index_get(int unit)
{
    return soc_mem_index_min(unit, RTAG7_PORT_BASED_HASHm);
}

/*
 * Function:
 *      _bcm_trx_lport_tab_default_entry_add
 * Purpose:
 *      Internal function for initializing common resource management
 *      Shared by Virtual-port based  modules
 * Parameters:
 *      unit    -  (IN) Device number.
 *      prof    -  (IN) LPORT memory profile. 
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_trx_lport_tab_default_entry_add(int unit, soc_profile_mem_t *prof) 
{
    int rv = BCM_E_NONE;
    soc_info_t *si;
    lport_tab_entry_t lport_entry;
    rtag7_port_based_hash_entry_t rtag7_entry;
    uint32 lport_index;
    void *entries[2];
    bcm_module_t my_modid=0;
    int stab_num_entries = 0;
    int i;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int entries_per_pipe;
#endif

    /* Input parameters check. */
    if (NULL == prof) {
        return (BCM_E_PARAM);
    }

    si = &SOC_INFO(unit);

    sal_memcpy(&lport_entry, soc_mem_entry_null(unit, LPORT_TABm),
               soc_mem_entry_words(unit, LPORT_TABm) * sizeof(uint32)); 

    if (SOC_MEM_IS_VALID(unit, RTAG7_PORT_BASED_HASHm)) {
        sal_memcpy(&rtag7_entry,
                   soc_mem_entry_null(unit, RTAG7_PORT_BASED_HASHm),
                   soc_mem_entry_words(unit, RTAG7_PORT_BASED_HASHm) *
                   sizeof(uint32));
    }

    /* Create a default LPORT profile entry */
    rv = bcm_esw_stk_my_modid_get(unit, &my_modid);
    if (rv < 0) {
        return rv;
    }

    /* Enable IFP. */
    soc_LPORT_TABm_field32_set(unit, &lport_entry, FILTER_ENABLEf, 1);

    /* Enable VFP. */
    if (!(SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit))) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, VFP_ENABLEf, 1);
    }

    /* Set port filed select index to PFS index max - 1. */
    if(SOC_MEM_FIELD_VALID(unit, LPORT_TABm, FP_PORT_FIELD_SEL_INDEXf)) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, FP_PORT_FIELD_SEL_INDEXf,
                                   (soc_mem_index_max(unit, FP_PORT_FIELD_SELm)
                                    - 1));
    }

    /* Enable MPLS */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, MPLS_ENABLEf)) {
         soc_LPORT_TABm_field32_set(unit, &lport_entry, MPLS_ENABLEf, 1);
    }

    /* Enable TRILL */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, TRILL_ENABLEf)) {
         soc_LPORT_TABm_field32_set(unit, &lport_entry, TRILL_ENABLEf, 1);
    }
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, ALLOW_TRILL_FRAMESf)) {
         soc_LPORT_TABm_field32_set(unit, &lport_entry, ALLOW_TRILL_FRAMESf, 1);
    }
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, ALLOW_NON_TRILL_FRAMESf)) {
         soc_LPORT_TABm_field32_set(unit, &lport_entry,
                                    ALLOW_NON_TRILL_FRAMESf, 1);
    }
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, COPY_CORE_IS_IS_TO_CPUf)) {
         soc_LPORT_TABm_field32_set(unit, &lport_entry,
                                    COPY_CORE_IS_IS_TO_CPUf, 1);
    }
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, RTAG7_HASH_CFG_SEL_TRILL_ECMPf)) {
         soc_LPORT_TABm_field32_set(unit, &lport_entry,
                                    RTAG7_HASH_CFG_SEL_TRILL_ECMPf, 1);
    }

    /* Enable OAM */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, OAM_ENABLEf)) {
         soc_LPORT_TABm_field32_set(unit, &lport_entry, OAM_ENABLEf, 1);
    }

    /* Enable MIM */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, MIM_TERM_ENABLEf)) {
         soc_LPORT_TABm_field32_set(unit, &lport_entry, MIM_TERM_ENABLEf, 1);
    }
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, MIM_MC_TERM_ENABLEf)) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, MIM_MC_TERM_ENABLEf, 1);
    } 
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, SRC_SYS_PORT_IDf)) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, SRC_SYS_PORT_IDf,
                                   si->lb_port);
    }

    if(SOC_MEM_FIELD_VALID(unit, LPORT_TABm, MY_MODIDf)) {
        soc_LPORT_TABm_field32_set(unit, &lport_entry, MY_MODIDf, my_modid);
    }

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit) && soc_feature(unit, soc_feature_xflow_macsec)) {
        if(SOC_MEM_FIELD_VALID(unit, LPORT_TABm, PORT_VIDf)) {
            soc_LPORT_TABm_field32_set(unit, &lport_entry, PORT_VIDf,
                                       BCM_VLAN_DEFAULT);
            if(SOC_MEM_FIELD_VALID(unit, LPORT_TABm, OUTER_TPID_ENABLEf)) {
                soc_LPORT_TABm_field32_set(unit, &lport_entry,
                                           OUTER_TPID_ENABLEf, 1);
            }
        }
    }
#endif

    /* Add LPORT default entry */
    entries[0] = &lport_entry; 
    entries[1] = &rtag7_entry;

    /* Note that when 'soc_feature_separate_ing_lport_rtag7_profile' is TRUE
       we only set the LPORT index and not RTAG7, but this one call will suffice
       since the entries[0] will point to 'lport' entry, the entries[1] which
       points to RTAG7 will only be used in the case when the mentioned
       feature is FALSE, wherein the 'prof' passed in would be acting on an
       older profiling scheme */

    /* Tomahawk 3 made this memory on a per pipe unique, so we're going to add
     * a default entry into index 0 of each pipe
     */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit) && SOC_MEM_UNIQUE_ACC(unit, LPORT_TABm)) {
        for (i = 0; i < _TH3_PIPES_PER_DEV; i++) {
            rv = soc_profile_mem_add_unique(unit, prof, entries, 1, i, &lport_index);
        }
    } else
#endif
    {
        rv = soc_profile_mem_add(unit, prof, entries, 1, &lport_index);
    }

    if (lport_index != _bcm_trx_lport_tab_default_entry_index_get(unit)) {
        /* Something went horribly wrong. */
        return (BCM_E_INTERNAL);
    }

    /* Update all entries in SOURCE_TRUNK_MAP to use LPORT default entry */
    BCM_IF_ERROR_RETURN
        (bcm_trx_source_table_lport_all_set(unit, lport_index,
                                            &stab_num_entries));

    /* Update reference count (keep 1 extra count) */
    for (i = 0; i < stab_num_entries; i++) {
        if (soc_feature(unit, soc_feature_separate_ing_lport_rtag7_profile)) {
            BCM_IF_ERROR_RETURN
                (_bcm_lport_ind_profile_mem_reference(unit, lport_index, 1));
        } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
            /* This adds a reference count to the first index of the per pipe
             * memory:
             * example:
             * LPORT_TAB_PIPE0 index 0's ref_count == 21
             * LPORT_TAB_PIPE1 index 0's ref_count == 21
             */
            if (SOC_IS_TOMAHAWK3(unit) && SOC_MEM_UNIQUE_ACC(unit, LPORT_TABm)) {
                /* Fetches the # of entries per pipe.  This is so that
                 * the function call below will increase the ref_count on a per
                 * pipe basis
                 */
                entries_per_pipe = SOC_MEM_SIZE(unit, LPORT_TABm);
                BCM_IF_ERROR_RETURN
                    (_bcm_lport_profile_mem_reference_unique(
                                    unit, lport_index, 1, i/entries_per_pipe));
            } else
#endif
            {
                BCM_IF_ERROR_RETURN
                    (_bcm_lport_profile_mem_reference(unit, lport_index, 1));
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_egr_lport_tab_default_entry_add
 * Purpose:
 *      Internal function for initializing common resource management
 *      Shared by Virtual-port based  modules
 * Parameters:
 *      unit    -  (IN) Device number.
 *      prof    -  (IN) LPORT memory profile. 
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_trx_egr_lport_tab_default_entry_add(int unit, soc_profile_mem_t *prof) 
{
    int rv = BCM_E_NONE;
    int index = 0;
    uint32 egr_lport_index;
    void *entries[9];
    egr_lport_profile_entry_t    egr_lport_profile_entry;
    egr_vlan_control_1_entry_t   egr_vlan_control_1_entry;
    egr_vlan_control_2_entry_t   egr_vlan_control_2_entry;
    egr_vlan_control_3_entry_t   egr_vlan_control_3_entry;
    egr_ipmc_cfg2_entry_t        egr_ipmc_cfg2_entry;
    egr_mtu_entry_t                egr_mtu_entry;
    egr_port_1_entry_t            egr_port_1_entry;
    egr_counter_control_entry_t  egr_counter_control_entry;
    egr_shaping_control_entry_t  egr_shaping_control_entry;

    /* Input parameters check. */
    if (NULL == prof) {
        return (BCM_E_PARAM);
    }

    if (SOC_MEM_IS_VALID(unit, EGR_LPORT_PROFILEm)) {
        sal_memcpy(&egr_lport_profile_entry,
                   soc_mem_entry_null(unit, EGR_LPORT_PROFILEm),
                   soc_mem_entry_words(unit, EGR_LPORT_PROFILEm) *
                   sizeof(uint32));
        entries[index++] = &egr_lport_profile_entry;
    }

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_CONTROL_1m)) {
        sal_memcpy(&egr_vlan_control_1_entry,
                   soc_mem_entry_null(unit, EGR_VLAN_CONTROL_1m),
                   soc_mem_entry_words(unit, EGR_VLAN_CONTROL_1m) *
                   sizeof(uint32));
        entries[index++] = &egr_vlan_control_1_entry;
    }

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_CONTROL_2m)) {
        sal_memcpy(&egr_vlan_control_2_entry,
                   soc_mem_entry_null(unit, EGR_VLAN_CONTROL_2m),
                   soc_mem_entry_words(unit, EGR_VLAN_CONTROL_2m) *
                   sizeof(uint32));
        entries[index++] = &egr_vlan_control_2_entry;
    }

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_CONTROL_3m)) {
        sal_memcpy(&egr_vlan_control_3_entry,
                   soc_mem_entry_null(unit, EGR_VLAN_CONTROL_3m),
                   soc_mem_entry_words(unit, EGR_VLAN_CONTROL_3m) *
                   sizeof(uint32));
        entries[index++] = &egr_vlan_control_3_entry;
    }

    if (SOC_MEM_IS_VALID(unit, EGR_IPMC_CFG2m)) {
        sal_memcpy(&egr_ipmc_cfg2_entry,
                   soc_mem_entry_null(unit, EGR_IPMC_CFG2m),
                   soc_mem_entry_words(unit, EGR_IPMC_CFG2m) *
                   sizeof(uint32));
        entries[index++] = &egr_ipmc_cfg2_entry;
    }

    if (SOC_MEM_IS_VALID(unit, EGR_MTUm)) {
        sal_memcpy(&egr_mtu_entry,
                   soc_mem_entry_null(unit, EGR_MTUm),
                   soc_mem_entry_words(unit, EGR_MTUm) *
                   sizeof(uint32));
        entries[index++] = &egr_mtu_entry;
    }

    if (SOC_MEM_IS_VALID(unit, EGR_PORT_1m)) {
        sal_memcpy(&egr_port_1_entry,
                   soc_mem_entry_null(unit, EGR_PORT_1m),
                   soc_mem_entry_words(unit, EGR_PORT_1m) *
                   sizeof(uint32));
        entries[index++] = &egr_port_1_entry;
    }

    if (soc_feature(unit, soc_feature_egr_all_profile) == 0) {
        if (SOC_MEM_IS_VALID(unit, EGR_COUNTER_CONTROLm)) {
            sal_memcpy(&egr_counter_control_entry,
                       soc_mem_entry_null(unit, EGR_COUNTER_CONTROLm),
                       soc_mem_entry_words(unit, EGR_COUNTER_CONTROLm) *
                       sizeof(uint32));
            entries[index++] = &egr_counter_control_entry;
        }

        if (SOC_MEM_IS_VALID(unit, EGR_SHAPING_CONTROLm)) {
            sal_memcpy(&egr_shaping_control_entry,
                       soc_mem_entry_null(unit, EGR_SHAPING_CONTROLm),
                       soc_mem_entry_words(unit, EGR_SHAPING_CONTROLm) *
                       sizeof(uint32));
            entries[index++] = &egr_shaping_control_entry;
        }
    }

    if (index > 0) {
        BCM_IF_ERROR_RETURN(
            soc_profile_mem_add(unit, prof, entries, 1, &egr_lport_index));
        if (egr_lport_index !=
            _bcm_trx_egr_lport_tab_default_entry_index_get(unit)) {
            /* Something went horribly wrong. */
            rv = BCM_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_rtag7_tab_default_entry_add
 * Purpose:
 *      Internal function for initializing RTAG7 default
 *      entry
 * Parameters:
 *      unit    -  (IN) Device number.
 *      prof    -  (IN) LPORT memory profile. 
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_trx_rtag7_tab_default_entry_add(int unit, soc_profile_mem_t *prof) 
{
    rtag7_port_based_hash_entry_t rtag7_entry;
    void *entries[1];
    uint32 rtag7_index;
    int stm_num_entries = 0, i;

    /* Input parameters check. */
    if (NULL == prof) {
        return (BCM_E_PARAM);
    }

    if (SOC_MEM_IS_VALID(unit, RTAG7_PORT_BASED_HASHm)) {
        sal_memcpy(&rtag7_entry,
                   soc_mem_entry_null(unit, RTAG7_PORT_BASED_HASHm),
                   soc_mem_entry_words(unit, RTAG7_PORT_BASED_HASHm) *
                   sizeof(uint32));
    }

    entries[0] = &rtag7_entry;

    BCM_IF_ERROR_RETURN(
        soc_profile_mem_add(unit, prof, entries, 1, &rtag7_index));

    if (rtag7_index != _bcm_trx_rtag7_tab_default_entry_index_get(unit)) {
        /* Something went horribly wrong. */
        return (BCM_E_INTERNAL);
    }
    if (SOC_IS_TRIDENT3X(unit)) {
        /* Update reference count (keep 1 extra count) */
        stm_num_entries = soc_mem_index_max(unit,
                              SOURCE_TRUNK_MAP_TABLEm) - soc_mem_index_min(unit,
                              SOURCE_TRUNK_MAP_TABLEm) + 1;
        for (i = 0; i < stm_num_entries; i++) {
            BCM_IF_ERROR_RETURN
                   (_bcm_rtag7_ind_profile_mem_reference(unit, rtag7_index, 1));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *           _bcm_trx_gport_to_nhi_set
 * Purpose:
*            set EGR_PORT_TO_NHI_MAPPINGr or EGR_DGPP_TO_NHIm 
 * Parameters:
 *           unit -  (IN) Device number.
 *           gport -  (IN) modport gport.
 *           nh_index -  (IN) nexthop index.
 * Returns:
 *           BCM_E_XXX
 */
int
_bcm_trx_gport_to_nhi_set(int unit, int gport, int nh_index)
{
    int rv = 0;
    int port;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    int                              modid;
    uint32                           dgpp = 0;
    uint32                           modbase = 0;
    egr_dgpp_to_nhi_modbase_entry_t  mem_val;
#endif
    if (nh_index < 0) {
        return BCM_E_PARAM;
    }
    if (!BCM_GPORT_IS_MODPORT(gport)) {
        return BCM_E_PARAM;
    }

    port = BCM_GPORT_MODPORT_PORT_GET(gport);

    if (!soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
        rv = soc_reg_field32_modify(unit, EGR_PORT_TO_NHI_MAPPINGr,
                                    port, NEXT_HOP_INDEXf, nh_index);
    } 
#ifdef BCM_TRIDENT2PLUS_SUPPORT    
    else {
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        BCM_IF_ERROR_RETURN(
            READ_EGR_DGPP_TO_NHI_MODBASEm(unit, MEM_BLOCK_ANY, modid, &mem_val));
        modbase = soc_mem_field32_get(unit, EGR_DGPP_TO_NHI_MODBASEm,
                                      &mem_val, BASEf);
        dgpp = modbase + port;
        rv = soc_mem_field32_modify(unit, EGR_DGPP_TO_NHIm,
                                    dgpp, NEXT_HOP_INDEXf, nh_index);
    }
#endif    
    return rv;
}

/*
 * Function:
 *           _bcm_trx_gport_to_nhi_get
 * Purpose:
*            get EGR_PORT_TO_NHI_MAPPINGr or EGR_DGPP_TO_NHIm 
 * Parameters:
 *           unit -  (IN) Device number.
 *           gport -  (IN) modport gport.
 *           nh_index -  (OUT) nexthop index ptr.
 * Returns:
 *           BCM_E_XXX
 */

int
_bcm_trx_gport_to_nhi_get(int unit, int gport, int* nh_index)
{
    uint32 reg_val=0;
    int port;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    int                                modid;
    uint32                             modbase = 0;
    uint32                             dgpp = 0;
    egr_dgpp_to_nhi_modbase_entry_t    mem_val1;
    egr_dgpp_to_nhi_entry_t            mem_val2;
#endif
    if (!BCM_GPORT_IS_MODPORT(gport)) {
        return BCM_E_PARAM;
    }

    port = BCM_GPORT_MODPORT_PORT_GET(gport);
    
    if (!soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
       BCM_IF_ERROR_RETURN(
           READ_EGR_PORT_TO_NHI_MAPPINGr(unit, port, &reg_val));
       *nh_index = soc_reg_field_get(unit, EGR_PORT_TO_NHI_MAPPINGr, reg_val, 
                                     NEXT_HOP_INDEXf);
    } 
#ifdef BCM_TRIDENT2PLUS_SUPPORT    
    else {
        /* TD2+ */
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        BCM_IF_ERROR_RETURN(
            READ_EGR_DGPP_TO_NHI_MODBASEm(unit, MEM_BLOCK_ANY, modid, &mem_val1));
        modbase = soc_mem_field32_get(unit, EGR_DGPP_TO_NHI_MODBASEm,
                                      &mem_val1, BASEf);
        dgpp = modbase + port;
        BCM_IF_ERROR_RETURN(
            READ_EGR_DGPP_TO_NHIm(unit, MEM_BLOCK_ANY, dgpp, &mem_val2));
        *nh_index = soc_mem_field32_get(unit, EGR_DGPP_TO_NHIm, &mem_val2, 
                                        NEXT_HOP_INDEXf);
    }
#endif    
    return BCM_E_NONE;
}
#else /* BCM_TRX_SUPPORT */
typedef int bcm_esw_trx_port_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRX_SUPPORT */

