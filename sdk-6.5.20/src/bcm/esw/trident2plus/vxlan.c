/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    vxlan.c
 * Purpose: Handle trident2plus specific vlan features.
 */
 
#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/td2_td2p.h>

#include <bcm/error.h>
#include <bcm/types.h>

#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident2plus.h>

 /*
  * Function:
  *      bcm_td2p_share_vp_get
  * Purpose:
  *      Get shared VPs info per VPN
  * Parameters:
  *      unit       - (IN) Device Number
  *      vpn        - (IN) VPN that shared vp belong to
  *      vp_bitmap  - (OUT) shared vp bitmap
  * Returns:
  *      BCM_E_XXX
  */
 int
 bcm_td2p_share_vp_get(int unit, bcm_vpn_t vpn, SHR_BITDCL *vp_bitmap)
 {
     vlan_xlate_entry_t *vent;
     soc_mem_t mem;
     int index_min = 0;
     int index_max = 0;
     uint8 *xlate_buf = NULL;
     int buf_size = 0;
     int i = 0;
     int rv = BCM_E_NONE;
     int vfi = -1;
     int vp = -1;

     if (vpn != BCM_VXLAN_VPN_INVALID) {
         BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
         _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
     }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        mem = VLAN_XLATEm;
    }

     index_min = soc_mem_index_min(unit, mem);
     index_max = soc_mem_index_max(unit, mem);

     buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
     xlate_buf = soc_cm_salloc(unit, buf_size, "VLAN_XLATE buffer");
     if (NULL == xlate_buf) {
         return BCM_E_MEMORY;
     }
     rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, xlate_buf);
     if (rv < 0) {
         soc_cm_sfree(unit, xlate_buf);
         return rv;
     }

     for (i = index_min; i <= index_max; i++) {
         vent = soc_mem_table_idx_to_pointer(unit, mem, void *, xlate_buf, i);
#if defined(BCM_TRIDENT3_SUPPORT)
         if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
             if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_0f) != 3) {
                 continue;
             }
             if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_1f) != 7) {
                 continue;
             }
         } else
#endif
         {
             if (soc_mem_field32_get(unit, mem, vent, VALIDf) == 0) {
                 continue;
             }
         }

         if (soc_mem_field32_get(unit, mem, vent, XLATE__MPLS_ACTIONf) != 0x1) {
             continue;
         }

         if (vfi != -1 && soc_mem_field32_get(unit, mem, vent, XLATE__VFIf) != vfi) {
             continue;
         }

         vp = soc_mem_field32_get(unit, mem, vent, XLATE__SOURCE_VPf);
         SHR_BITSET(vp_bitmap, vp);
     }

     (void)soc_cm_sfree(unit, xlate_buf);

     return BCM_E_NONE;
 }

/*
 * Function:
 *      bcm_td2p_share_vp_delete
 * Purpose:
 *      Delete match criteria of shared VPs
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN that shared vp belong to
 *      vp      - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_share_vp_delete(int unit, bcm_vpn_t vpn, int vp)
{
    vlan_xlate_entry_t *vent;
    soc_mem_t mem;
    int index_min = 0;
    int index_max = 0;
    uint8 *xlate_buf = NULL;
    int buf_size = 0;
    int i = 0;
    int rv = BCM_E_NONE;
    int vfi = -1;
    int source_vp = -1;

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
        _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    } else
#endif
    {
        mem = VLAN_XLATEm;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    xlate_buf = soc_cm_salloc(unit, buf_size, "VLAN_XLATE buffer");
    if (NULL == xlate_buf) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, xlate_buf);
    if (rv < 0) {
        soc_cm_sfree(unit, xlate_buf);
        return rv;
    }

    for (i = index_min; i <= index_max; i++) {
        vent = soc_mem_table_idx_to_pointer(unit, mem, void *, xlate_buf, i);
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_0f) != 3) {
                continue;
            }
            if (soc_mem_field32_get(unit, mem, vent, BASE_VALID_1f) != 7) {
                continue;
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem, vent, VALIDf) == 0) {
                continue;
            }
        }

        if (soc_mem_field32_get(unit, mem, vent, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }

        if (vfi != -1 && soc_mem_field32_get(unit, mem, vent, XLATE__VFIf) != vfi) {
            continue;
        }

        source_vp = soc_mem_field32_get(unit, mem, vent, XLATE__SOURCE_VPf);
        if (vp != source_vp) {
            continue;
        }
        rv = soc_mem_delete_index(unit, mem, MEM_BLOCK_ALL, i);
        if (rv < 0) {
            soc_cm_sfree(unit, xlate_buf);
            return rv;
        }
        bcm_td2_vxlan_port_match_count_adjust(unit, source_vp, -1);
    }

    (void)soc_cm_sfree(unit, xlate_buf);

    return BCM_E_NONE;
}
#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3) */
