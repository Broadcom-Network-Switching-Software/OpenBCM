/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>

#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <bcm/error.h>
#include <bcm/proxy.h>
#include <bcm/debug.h>
#include <bcm/types.h>
#include <bcm/l3.h>

#include <bcm_int/esw/port.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/proxy.h>
#include <bcm_int/esw/vlan.h>


/*
 * Function:
 *      _bcm_td2_proxy_flags_to_shr
 * Purpose:
 *      Translate proxy specific flags to shared (table management) flags. 
 * Parameters:
 *    proxy_flags   - (IN)  Proxy flags. 
 *    shr_flags  - (OUT) Shared flags. 
 * Returns:
 *    BCM_E_XXX
 */
STATIC INLINE int
_bcm_td2_proxy_flags_to_shr(uint32 proxy_flags, uint32 *shr_flags)
{
    uint32 flag;

    if (NULL == shr_flags) {
        return (BCM_E_PARAM);
    }

    if ((proxy_flags == 0) ||  (proxy_flags & (BCM_PROXY_WITH_ID | BCM_PROXY_REPLACE))) {
        flag = 0;
    } else {
        return BCM_E_PARAM;
    }

    if (proxy_flags & BCM_PROXY_REPLACE) {
       flag |= _BCM_L3_SHR_UPDATE;
    }

    if (proxy_flags & BCM_PROXY_WITH_ID) {
       flag |= _BCM_L3_SHR_WITH_ID;
    }

    *shr_flags = flag;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td2_proxy_flags_valid
 * Purpose:
 *      Check Validity of Proxy Flags
 * Parameters:
 *    proxy_flags   - (IN)  Proxy flags. 
 * Returns:
 *    BCM_E_XXX
 */
STATIC INLINE int
_bcm_td2_proxy_flags_valid(uint32 proxy_flags)
{
    if ((proxy_flags == 0) || 
             (proxy_flags & (BCM_PROXY_PACKET_MODIFY | BCM_PROXY_RSVD_VLAN_ADD |
                                       BCM_PROXY_LEARN_DISABLE | BCM_PROXY_L3_FORCE |
                                       BCM_PROXY_PACKET_DROP))) {
        return (BCM_E_NONE);
    }
    return (BCM_E_PARAM);
}


/*
 * Function:
 *      bcm_td2_proxy_egress_create
 * Purpose:
 *      Create Proxy Egress forwarding object.
 * Parameters:
 *      unit    - (IN)  bcm device.
 *      flags   - (IN)  BCM_L3_REPLACE: replace existing.
 *                      BCM_L3_WITH_ID: intf argument is given.
 *      proxy_egr     - (IN) Egress forwarding destination.
 *      proxy_intf_id    - (OUT) L3 interface id pointing to Egress object.
 *                      This is an IN argument if either BCM_L3_REPLACE
 *                      or BCM_L3_WITH_ID are given in flags.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_proxy_egress_create(int unit, uint32 flags, bcm_proxy_egress_t *proxy_egr, 
                          bcm_if_t *proxy_intf_id)
{
    int err_code = BCM_E_UNAVAIL; /* used for L3_IF_ERROR_CLEANUP_ELSE_RETURN */
    int index = -1;                 /* Next hop index.     */ 
    uint32  shr_flags;              /* Table shared flags. */

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if ((NULL == proxy_egr) || (NULL == proxy_intf_id)) {
        return (BCM_E_PARAM);
    }

    /* Verify egress object range if BCM_PROXY_WITH_ID flag is set. */ 
    if (flags & BCM_PROXY_WITH_ID) {
        if (!BCM_XGS3_PROXY_EGRESS_IDX_VALID(unit, *proxy_intf_id)) {
             return (BCM_E_PARAM);
        }
        if (BCM_XGS3_PROXY_EGRESS_IDX_VALID(unit, *proxy_intf_id)) {
            index = *proxy_intf_id - BCM_XGS3_PROXY_EGRESS_IDX_MIN(unit);
        }
    }

    /* Check validity of Proxy_egr flags */ 
    BCM_IF_ERROR_RETURN(_bcm_td2_proxy_flags_valid(proxy_egr->flags));

    /* Convert api flags to shared table management flags. */ 
    BCM_IF_ERROR_RETURN(_bcm_td2_proxy_flags_to_shr(flags, &shr_flags));

    /* Disable matching entry lookup. */
    shr_flags |= _BCM_L3_SHR_MATCH_DISABLE;

    /* Create next hop entry. */
    err_code = bcm_xgs3_proxy_nh_add(unit, shr_flags, proxy_egr, &index);

    if (err_code == BCM_E_NONE) {
       /* Return BCM_XGS3_PROXY_EGRESS_IDX_MIN(unit) + nh_index for Proxy object*/
       *proxy_intf_id = BCM_XGS3_PROXY_EGRESS_IDX_MIN(unit) + index; 
    } 
    return err_code;
}

/*
 * Function:
 *      bcm_td2_proxy_egress_destroy
 * Purpose:
 *      Destroy Proxy Egress forwarding object.
 * Parameters:
 *      unit    - (IN) bcm device.
 *      intf    - (IN) L3 interface id pointing to Egress object.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_proxy_egress_destroy(int unit, bcm_if_t proxy_intf_id)
{
    int nh_idx=0;               /* Next hop index.          */
    int rv;                   /* Operation return status. */

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if (!BCM_XGS3_PROXY_EGRESS_IDX_VALID(unit, proxy_intf_id)) {
        return (BCM_E_PARAM);
    }

    /* Calculate next hop index. */
    if (BCM_XGS3_PROXY_EGRESS_IDX_VALID(unit, proxy_intf_id)) {
        nh_idx = proxy_intf_id - BCM_XGS3_PROXY_EGRESS_IDX_MIN(unit);
    }

    /* If entry is used by hosts/routes or ecmp objects - reject deletion */
    if(1 < BCM_XGS3_L3_ENT_REF_CNT
       (BCM_XGS3_L3_TBL_PTR(unit, next_hop), nh_idx)) {
        return (BCM_E_BUSY);
    }

    /* Delete next hop entry. */
    rv = bcm_xgs3_nh_del(unit, 0, nh_idx);
    return (rv);
}


/*
 * Function:
 *      bcm_td2_proxy_egress_get
 * Purpose:
 *      Get Proxy Egress forwarding object.
 * Parameters:
 *      unit    - (IN) bcm device.
 *      intf    - (IN) L3 interface id pointing to Egress object.
 *      egr     - (OUT) Egress forwarding destination.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_proxy_egress_get(int unit, bcm_if_t proxy_intf_id, bcm_proxy_egress_t *proxy_egr) 
{
    int offset=0;

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if ((NULL == proxy_egr) || (!BCM_XGS3_PROXY_EGRESS_IDX_VALID(unit, proxy_intf_id))) {
        return (BCM_E_PARAM);
    }
    if (BCM_XGS3_PROXY_EGRESS_IDX_VALID(unit, proxy_intf_id)) {
        offset = BCM_XGS3_PROXY_EGRESS_IDX_MIN(unit);
    } 

    /* Fill next hop entry info. */
    BCM_IF_ERROR_RETURN(bcm_xgs3_proxy_nh_get(unit, (proxy_intf_id - offset), proxy_egr));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2_proxy_nh_add
 * Purpose:
 *      Routine set proxy next hop entry in the chip.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      idx       - (IN)Allocated entry index.
 *      buf       - (IN)Next hop entry data.
 *      info      - (IN)Additional info
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_td2_proxy_nh_add(int unit, int idx, void *buf, void *info)
{
    ing_l3_next_hop_entry_t in_nh_entry;   /* Buffer to read ingress nh entry. */
    egr_l3_next_hop_entry_t eg_nh_entry;   /* Buffer to read egress nh entry.  */
    bcm_proxy_egress_t *proxy_nh_entry;    /* Next hop entry passed buffer.    */
    soc_mem_t mem;                         /* Next hop table memory.           */
    soc_field_t acc_type = ENTRY_TYPEf;    /* Field in egress next hop table   */
    uint32 drop = 0;
    uint32 glp = 0;

    /* Input parameters check */
    if (NULL == buf) {
        return (BCM_E_PARAM);
    }
    proxy_nh_entry = (bcm_proxy_egress_t *) buf;

    /* Zero buffers. */
    sal_memset(&in_nh_entry, 0, sizeof(ing_l3_next_hop_entry_t));

    glp = (SOC_GPORT_MODPORT_MODID_GET(proxy_nh_entry->gport) << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS |
           SOC_GPORT_MODPORT_PORT_GET(proxy_nh_entry->gport));
    mem = INITIAL_ING_L3_NEXT_HOPm;
    /* Read ingress next hop entry. */
    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, &in_nh_entry));
    /* Set module id to initial ingress l3 next hop entry. */
    if (soc_feature(unit, soc_feature_generic_dest)) {
        soc_mem_field32_dest_set(unit, mem, &in_nh_entry, DESTINATIONf, SOC_MEM_FIF_DEST_DGPP, glp);
    } else {
        soc_mem_field32_set(unit, mem, &in_nh_entry, MODULE_IDf,
                            SOC_GPORT_MODPORT_MODID_GET(proxy_nh_entry->gport));
        soc_mem_field32_set(unit, mem, &in_nh_entry, Tf, 0);
        soc_mem_field32_set(unit, mem, &in_nh_entry, PORT_NUMf,
                            SOC_GPORT_MODPORT_PORT_GET(proxy_nh_entry->gport));
    }
    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_WRITE(unit, mem, idx, &in_nh_entry));

    /* Zero buffers. */
    sal_memset(&in_nh_entry, 0, sizeof(ing_l3_next_hop_entry_t));

    mem = ING_L3_NEXT_HOPm;
    /* Read ingress next hop entry. */
    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, &in_nh_entry));
    if (soc_feature(unit, soc_feature_generic_dest)) {
        soc_mem_field32_dest_set(unit, mem, &in_nh_entry, DESTINATIONf, SOC_MEM_FIF_DEST_DGPP, glp);
    } else {
        soc_mem_field32_set(unit, mem, &in_nh_entry, MODULE_IDf,
                            SOC_GPORT_MODPORT_MODID_GET(proxy_nh_entry->gport));
        soc_mem_field32_set(unit, mem, &in_nh_entry, Tf, 0);
        soc_mem_field32_set(unit, mem, &in_nh_entry, PORT_NUMf,
                            SOC_GPORT_MODPORT_PORT_GET(proxy_nh_entry->gport));
    }
    if (soc_mem_field_valid(unit, mem, MTU_SIZEf)) {
        soc_mem_field32_set(unit, mem, &in_nh_entry, MTU_SIZEf, 0x3FFF);
    }
    drop = proxy_nh_entry->flags &  BCM_PROXY_PACKET_DROP;
    soc_mem_field32_set(unit, mem, &in_nh_entry, DROPf, drop);
    soc_mem_field32_set(unit, mem, &in_nh_entry, ENTRY_TYPEf, 0x2);

    /* Write ingress next hop entry. */
    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_WRITE(unit, mem, idx, &in_nh_entry));


    /* Zero buffers. */
    sal_memset(&eg_nh_entry, 0, sizeof(egr_l3_next_hop_entry_t));


    mem = EGR_L3_NEXT_HOPm;
    /* Read egress next hop entry. */
    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, &eg_nh_entry));

            if (soc_feature(unit, soc_feature_mem_access_data_type)) {
                acc_type = DATA_TYPEf;
            }
            soc_mem_field32_set(unit, mem, &eg_nh_entry,
                             acc_type, _BCM_PROXY_EGR_NEXT_HOP_SDTAG_VIEW);
            drop = proxy_nh_entry->flags &  BCM_PROXY_PACKET_DROP;
            soc_mem_field32_set(unit, mem, &eg_nh_entry,
                    SD_TAG__BC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, mem, &eg_nh_entry,
                    SD_TAG__UUC_DROPf, drop ? 1 : 0);
            soc_mem_field32_set(unit, mem, &eg_nh_entry,
                    SD_TAG__UMC_DROPf, drop ? 1 : 0);

            if (proxy_nh_entry->flags &  BCM_PROXY_LEARN_DISABLE) {
                 soc_mem_field32_set(unit, mem, &eg_nh_entry,
                        SD_TAG__HG_LEARN_OVERRIDEf, 0x1);
            }
            if (proxy_nh_entry->flags &  BCM_PROXY_PACKET_MODIFY) {
                 soc_mem_field32_set(unit, mem, &eg_nh_entry,
                        SD_TAG__HG_MODIFY_ENABLEf, 0x1);
            }
            if (proxy_nh_entry->flags &  BCM_PROXY_RSVD_VLAN_ADD) {
                 soc_mem_field32_set(unit, mem, &eg_nh_entry,
                        SD_TAG__HG_ADD_SYS_RSVD_VIDf, 0x1);
            }
            if (proxy_nh_entry->flags &  BCM_PROXY_L3_FORCE) {
                 soc_mem_field32_set(unit, mem, &eg_nh_entry, 
                        SD_TAG__HG_L3_OVERRIDEf, 0x1);
            }
            soc_mem_field32_set(unit, mem, &eg_nh_entry,
                        SD_TAG__HG_MC_DST_MODIDf,
                        SOC_GPORT_MODPORT_MODID_GET(proxy_nh_entry->gport));
            soc_mem_field32_set(unit, mem, &eg_nh_entry,
                        SD_TAG__HG_MC_DST_PORT_NUMf,
                        SOC_GPORT_MODPORT_PORT_GET(proxy_nh_entry->gport));

            /* HG_HDR  must be PPD0 */
            soc_mem_field32_set(unit, mem, &eg_nh_entry,
                    SD_TAG__HG_HDR_SELf, 0);
            soc_mem_field32_set(unit, mem, &eg_nh_entry,
                     SD_TAG__HG_CHANGE_DESTINATIONf, 0x1);

            /* Delete VNTAG */
            soc_mem_field32_set(unit, mem, &eg_nh_entry,
                     SD_TAG__VNTAG_ACTIONSf, 0x3);

    /* Write egress next hop entry. */
    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_WRITE(unit, mem, idx, &eg_nh_entry));

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_td2_proxy_nh_get
 * Purpose:
 *        Read the Proxy NextHop table info given index.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      idx       - (IN) Table index to read. 
 *      nh_entry  - (OUT) Next hop entry info. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td2_proxy_nh_get(int unit, int idx, bcm_proxy_egress_t *proxy_nh_entry)
{
    int rv = BCM_E_NONE;
    egr_l3_next_hop_entry_t  egr_entry;        /* Egress next hop entry.     */
    bcm_module_t  modid_in, modid_out;
    bcm_port_t       port_in, port_out;
    soc_mem_t       mem;

    /* Input parameters check */
    if (NULL == proxy_nh_entry) {
        return (BCM_E_PARAM);
    }

    sal_memset(&egr_entry, 0, sizeof(egr_l3_next_hop_entry_t));
    (void) bcm_proxy_egress_t_init(proxy_nh_entry);

    /*  Read egress next hop entry. */
    mem = EGR_L3_NEXT_HOPm;
    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, &egr_entry));

    modid_in = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_entry, SD_TAG__HG_MC_DST_MODIDf);
    port_in = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_entry, SD_TAG__HG_MC_DST_PORT_NUMf);
    rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    modid_in, port_in, &modid_out, &port_out);

    if (BCM_SUCCESS(rv)) {
        SOC_GPORT_MODPORT_SET(proxy_nh_entry->gport, modid_out, port_out);
    }

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_entry, SD_TAG__HG_L3_OVERRIDEf)) {
         proxy_nh_entry->flags |=  BCM_PROXY_L3_FORCE;
    }

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_entry, SD_TAG__HG_ADD_SYS_RSVD_VIDf)) {
         proxy_nh_entry->flags |=  BCM_PROXY_RSVD_VLAN_ADD;
    }

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_entry, SD_TAG__HG_MODIFY_ENABLEf)) {
         proxy_nh_entry->flags |=  BCM_PROXY_PACKET_MODIFY;
    }

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_entry, SD_TAG__HG_LEARN_OVERRIDEf)) {
         proxy_nh_entry->flags |=  BCM_PROXY_LEARN_DISABLE;
    }

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_entry, SD_TAG__BC_DROPf) ||
         soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_entry, SD_TAG__UUC_DROPf) ||
         soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_entry, SD_TAG__UMC_DROPf)) {
            proxy_nh_entry->flags |=  BCM_PROXY_PACKET_DROP;
    }


    return (BCM_E_NONE);
}

/*
 * Purpose:
 *      Traverse all valid Proxy egress entries and call the
 * 	 supplied callback routine.
 * Parameters:
 *     unit   - Device Number
 *     cb  - User callback function.
 *     user_data - cookie
 */

int
bcm_td2_proxy_egress_traverse(int unit, bcm_proxy_egress_traverse_cb  cb,
                                                        void *user_data)
{
    bcm_proxy_egress_t proxy_nh_entry;     /* Next hop entry info.           */
    int idx;
    bcm_if_t proxy_if_id;
    char *egr_tbl_ptr;            /* Dma egress nh table pointer.   */
    uint32 *egr_entry_ptr = NULL; /* Egress next hop entry pinter.  */ 
    int rv = BCM_E_NONE;          /* Operation return status.       */
    soc_mem_t mem;                      /* Table location memory.           */
    bcm_module_t  modid_in, modid_out;
    bcm_port_t       port_in, port_out;

     /* If no callback provided, we are done.  */ 
     if (NULL == cb) { 
          return BCM_E_NONE; 
     } 
     mem = EGR_L3_NEXT_HOPm;

    /*  Read egress next hop entry. */
    rv = bcm_xgs3_l3_tbl_dma(unit, mem,
                             sizeof(egr_l3_next_hop_entry_t), "egr_nh_tbl",
                             &egr_tbl_ptr, NULL);
    if (rv < 0) {
        return rv;
    }

    for (idx = 0; idx < BCM_XGS3_L3_NH_TBL_SIZE(unit); idx++) {
        /* Skip unused entries. */
        if (!BCM_XGS3_L3_ENT_REF_CNT (BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                                      idx)) {
            continue;
        }

        /*  Read egress next hop entry. */
        egr_entry_ptr = 
            soc_mem_table_idx_to_pointer(unit, EGR_L3_NEXT_HOPm,
                                         uint32 *, egr_tbl_ptr, idx);

         if (!soc_mem_field32_get(unit, mem, egr_entry_ptr, SD_TAG__HG_CHANGE_DESTINATIONf)) {
            continue;
         }
         bcm_proxy_egress_t_init(&proxy_nh_entry);

         proxy_if_id = idx + BCM_XGS3_PROXY_EGRESS_IDX_MIN(unit);
         modid_in = soc_mem_field32_get(unit, mem, egr_entry_ptr, SD_TAG__HG_MC_DST_MODIDf);
         port_in = soc_mem_field32_get(unit, mem,  egr_entry_ptr, SD_TAG__HG_MC_DST_PORT_NUMf);
         rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    modid_in, port_in, &modid_out, &port_out);

         if (BCM_SUCCESS(rv)) {
            SOC_GPORT_MODPORT_SET(proxy_nh_entry.gport, modid_out, port_out);
         }

         if (soc_mem_field32_get(unit, mem,  egr_entry_ptr, SD_TAG__HG_L3_OVERRIDEf)) {
             proxy_nh_entry.flags |=  BCM_PROXY_L3_FORCE;
         }

         if (soc_mem_field32_get(unit, mem,  egr_entry_ptr, SD_TAG__HG_ADD_SYS_RSVD_VIDf)) {
             proxy_nh_entry.flags |=  BCM_PROXY_RSVD_VLAN_ADD;
         }

         if (soc_mem_field32_get(unit, mem, egr_entry_ptr, SD_TAG__HG_MODIFY_ENABLEf)) {
             proxy_nh_entry.flags |=  BCM_PROXY_PACKET_MODIFY;
         }

         if (soc_mem_field32_get(unit, mem, egr_entry_ptr, SD_TAG__HG_LEARN_OVERRIDEf)) {
             proxy_nh_entry.flags |=  BCM_PROXY_LEARN_DISABLE;
         }

         if (soc_mem_field32_get(unit, mem, egr_entry_ptr, SD_TAG__BC_DROPf) ||
              soc_mem_field32_get(unit, mem, egr_entry_ptr, SD_TAG__UUC_DROPf) ||
              soc_mem_field32_get(unit, mem, egr_entry_ptr, SD_TAG__UMC_DROPf)) {
             proxy_nh_entry.flags |=  BCM_PROXY_PACKET_DROP;
         }

         /* Execute operation routine if any. */
         if (cb) {
            rv = (cb)(unit, proxy_if_id, (void *)&proxy_nh_entry, (void *) user_data);
            if (rv < 0) {
                break;
            }
         }
    }
    soc_cm_sfree(unit, egr_tbl_ptr);
    return (rv);

}

#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

