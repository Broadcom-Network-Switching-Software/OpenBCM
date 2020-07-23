/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l3.c
 * Purpose: Triumph3 L3 function implementations
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(INCLUDE_L3) && defined(BCM_TRIUMPH3_SUPPORT) 
#include <soc/drv.h>
#include <bcm/error.h>

#include <bcm/l3.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/lpmv6.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/tomahawk.h>

#if defined(BCM_TRX_SUPPORT) 
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/qos.h>
#endif /* BCM_TRIDENT_SUPPORT*/
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT_SUPPORT*/

#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */

#define _BCM_TR3_NUM_ESM_HOST_TABLES  (4)
#define _BCM_TR3_L3_MEM_BANKS_ALL     (-1)
#define _BCM_TR3_HOST_ENTRY_NOT_FOUND (-1)

/* L3 ESM host table state */
static int *_bcm_tr3_esm_host_tbl_freeidx_ptr
            [_BCM_TR3_NUM_ESM_HOST_TABLES][SOC_MAX_NUM_DEVICES];
#define _TR3_ESM_HOST_TBL_STATE(_u_, _tbl_) \
        (_bcm_tr3_esm_host_tbl_freeidx_ptr[(_tbl_)][(_u_)])
#define _TR3_ESM_HOST_TBL_FREE_ENTRY(_u_, _tbl_) \
        (*_bcm_tr3_esm_host_tbl_freeidx_ptr[(_tbl_)][(_u_)])

extern _bcm_defip_pair128_table_t *l3_defip_pair128[BCM_MAX_NUM_UNITS];
/* ECMP dynamic load balancing internal state */

typedef struct _tr3_ecmp_dlb_nh_membership_s {
    int member_id; /* ECMP DLB member ID assigned to next hop index */
    int group;  /* ECMP DLB group this member belongs to */
    struct _tr3_ecmp_dlb_nh_membership_s *next;
} _tr3_ecmp_dlb_nh_membership_t;

typedef struct _tr3_ecmp_dlb_nh_info_s {
    int valid;
    int nh_index; /* next hop index */
    _tr3_ecmp_dlb_nh_membership_t *nh_membership_list;
                  /* Linked list of ECMP DLB member IDs assigned to this
                   * next hop index and the ECMP DLB groups the member IDs
                   * belong to.
                   */ 
} _tr3_ecmp_dlb_nh_info_t;

typedef struct _tr3_ecmp_dlb_bookkeeping_s {
    int ecmp_dlb_nh_info_size; /* Size of ecmp_dlb_nh_info array */
    _tr3_ecmp_dlb_nh_info_t *ecmp_dlb_nh_info;
    SHR_BITDCL *ecmp_dlb_id_used_bitmap;
    SHR_BITDCL *ecmp_dlb_flowset_block_bitmap; /* Each block corresponds to
                                                 512 entries */
    SHR_BITDCL *ecmp_dlb_member_id_used_bitmap;
    int ecmp_dlb_sample_rate;
    int ecmp_dlb_tx_load_min_th;
    int ecmp_dlb_tx_load_max_th;
    int ecmp_dlb_qsize_min_th;
    int ecmp_dlb_qsize_max_th;
    int ecmp_dlb_exp_load_min_th;
    int ecmp_dlb_exp_load_max_th;
    int ecmp_dlb_imbalance_min_th;
    int ecmp_dlb_imbalance_max_th;
    uint8 *ecmp_dlb_load_weight;
    soc_profile_mem_t *ecmp_dlb_quality_map_profile;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint8 recovered_from_scache; /* Indicates if the following members of this
                                    structure have been recovered from scache:
                                    ecmp_dlb_sample_rate,
                                    ecmp_dlb_tx_load_min_th,
                                    ecmp_dlb_tx_load_max_th,
                                    ecmp_dlb_qsize_min_th,
                                    ecmp_dlb_qsize_max_th,
                                    ecmp_dlb_exp_load_min_th,
                                    ecmp_dlb_exp_load_max_th,
                                    ecmp_dlb_imbalance_min_th,
                                    ecmp_dlb_imbalance_max_th,
                                    ecmp_dlb_load_weight */
#endif /* BCM_WARM_BOOT_SUPPORT */
} _tr3_ecmp_dlb_bookkeeping_t;

static _tr3_ecmp_dlb_bookkeeping_t *_tr3_ecmp_dlb_bk[BCM_MAX_NUM_UNITS];

#define ECMP_DLB_INFO(_unit_) (_tr3_ecmp_dlb_bk[_unit_])
#define ECMP_DLB_NH_INFO(_unit_, _idx_) \
           (_tr3_ecmp_dlb_bk[_unit_]->ecmp_dlb_nh_info[_idx_])

#define _BCM_ECMP_DLB_ID_USED_GET(_u_, _idx_) \
    SHR_BITGET(ECMP_DLB_INFO(_u_)->ecmp_dlb_id_used_bitmap, _idx_)
#define _BCM_ECMP_DLB_ID_USED_SET(_u_, _idx_) \
    SHR_BITSET(ECMP_DLB_INFO(_u_)->ecmp_dlb_id_used_bitmap, _idx_)
#define _BCM_ECMP_DLB_ID_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(ECMP_DLB_INFO(_u_)->ecmp_dlb_id_used_bitmap, _idx_)

#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_GET(_u_, _idx_) \
    SHR_BITGET(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_SET(_u_, _idx_) \
    SHR_BITSET(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_SET_RANGE(_u_, _idx_, _count_) \
    SHR_BITSET_RANGE(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_, _count_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_CLR_RANGE(_u_, _idx_, _count_) \
    SHR_BITCLR_RANGE(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_, _count_)
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_TEST_RANGE(_u_, _idx_, _count_, _result_) \
    SHR_BITTEST_RANGE(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_, _count_, _result_)

#define _BCM_ECMP_DLB_MEMBER_ID_USED_GET(_u_, _idx_) \
    SHR_BITGET(ECMP_DLB_INFO(_u_)->ecmp_dlb_member_id_used_bitmap, _idx_)
#define _BCM_ECMP_DLB_MEMBER_ID_USED_SET(_u_, _idx_) \
    SHR_BITSET(ECMP_DLB_INFO(_u_)->ecmp_dlb_member_id_used_bitmap, _idx_)
#define _BCM_ECMP_DLB_MEMBER_ID_USED_CLR(_u_, _idx_) \
    SHR_BITCLR(ECMP_DLB_INFO(_u_)->ecmp_dlb_member_id_used_bitmap, _idx_)

/*----- STATIC FUNCS ----- */
/*
 * Function:
 *      _bcm_tr3_l3_ent_init
 * Purpose:
 *      Tr3 helper routine used to init l3 host entry buffer
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN/OUT) l3 entry  lookup key & search result.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
STATIC int
_bcm_tr3_l3_ent_init(int unit, soc_mem_t mem, 
                      _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{
    int esm;
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32 *buf_p;                /* HW buffer address.          */ 

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    esm = BCM_TR3_MEM_IS_ESM(unit, mem);

    /* Zero destination buffer. */
    buf_p = (uint32 *)l3x_entry;
    sal_memset(buf_p, 0, BCM_L3_MEM_ENT_SIZE(unit, mem)); 

    if (esm) {
        if (ipv6) {
            soc_mem_ip6_addr_set(unit, mem, buf_p, IP_ADDRf, l3cfg->l3c_ip6, 0);
        } else {
            soc_mem_field32_set(unit, mem, buf_p, IP_ADDRf, l3cfg->l3c_ip_addr);
        }
        soc_mem_field32_set(unit, mem, buf_p, VRF_IDf, l3cfg->l3c_vrf);
        /* ESM VALID bit is set/unset in EXT_TCAM_VBIT table in
         * the ESM add/del function  */
    } else {
        if (ipv6) { /* ipv6 entry */
            soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6UC__IP_ADDR_LWR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
            soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6UC__IP_ADDR_UPR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
            soc_mem_field32_set(unit, mem, buf_p, IPV6UC__VRF_IDf,
                                                  l3cfg->l3c_vrf);
            soc_mem_field32_set(unit, mem, buf_p, VALID_0f, 1); 
            soc_mem_field32_set(unit, mem, buf_p, VALID_1f, 1); 
            if (BCM_XGS3_L3_MEM(unit, v4_2) == mem) {
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_0f, 2);
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_1f, 2);
            } else if (BCM_XGS3_L3_MEM(unit, v6_4) == mem) { 
                /* ipv6 extended host entry */
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_0f, 3);
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_1f, 3);
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_2f, 3);
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_3f, 3);
                soc_mem_field32_set(unit, mem, buf_p, VALID_0f, 1); 
                soc_mem_field32_set(unit, mem, buf_p, VALID_1f, 1); 
                soc_mem_field32_set(unit, mem, buf_p, VALID_2f, 1); 
                soc_mem_field32_set(unit, mem, buf_p, VALID_3f, 1); 
            } else {
                return BCM_E_NOT_FOUND;
            }
        } else { /* ipv4 entry */
            soc_mem_field32_set(unit, mem, buf_p, IPV4UC__IP_ADDRf,
                                               l3cfg->l3c_ip_addr);
            soc_mem_field32_set(unit, mem, buf_p, IPV4UC__VRF_IDf,
                                                  l3cfg->l3c_vrf);
            if (BCM_XGS3_L3_MEM(unit, v4) == mem) {
                /* ipv4 unicast */
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, 0); 
                soc_mem_field32_set(unit, mem, buf_p, VALIDf, 1); 
            } else if (BCM_XGS3_L3_MEM(unit, v4_2) == mem) { 
                /* ipv4 extended host entry */
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_0f, 1);
                soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_1f, 1);
                soc_mem_field32_set(unit, mem, buf_p, VALID_1f, 1); 
                soc_mem_field32_set(unit, mem, buf_p, VALID_0f, 1); 
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_ipmc_ent_init
 * Purpose:
 *      Set GROUP/SOURCE/VID/IMPC flag in the entry.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      l3x_entry - (IN/OUT) IPMC entry to fill. 
 *      l3cfg     - (IN) Api IPMC data. 
 * Returns:
 *    BCM_E_XXX
 */
STATIC void
_bcm_tr3_l3_ipmc_ent_init(int unit, uint32 *buf_p,
                             _bcm_l3_cfg_t *l3cfg)
{
    soc_mem_t mem;                     /* IPMC table memory.    */
    int ipv6;                          /* IPv6 entry indicator. */
    int idx;                           /* Iteration index.      */
    soc_field_t v4typef[] = { KEY_TYPE_0f, KEY_TYPE_1f };
    soc_field_t v6typef[] = { KEY_TYPE_0f, KEY_TYPE_1f, 
                              KEY_TYPE_2f, KEY_TYPE_3f };
    soc_field_t validf[] = { VALID_0f, VALID_1f, VALID_2f, VALID_3f };

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    mem =  (ipv6) ? L3_ENTRY_4m : L3_ENTRY_2m;

    sal_memset(buf_p, 0, BCM_L3_MEM_ENT_SIZE(unit, mem));

    if (ipv6) {
        /* Set group address. */
        soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6MC__GROUP_IP_ADDR_LWR_96f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_96BIT);
        l3cfg->l3c_ip6[0] = 0x0;    /* Don't write ff entry already mcast. */
        soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6MC__GROUP_IP_ADDR_UPR_24f, 
                             l3cfg->l3c_ip6, SOC_MEM_IP6_BITS_119_96);
        l3cfg->l3c_ip6[0] = 0xff;    /* Restore The entry  */

        /* Set source  address. */
        soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6MC__SOURCE_IP_ADDR_LWR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_LOWER_ONLY);
        soc_mem_ip6_addr_set(unit, mem, buf_p, IPV6MC__SOURCE_IP_ADDR_UPR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_UPPER_ONLY);

        for (idx = 0; idx < 4; idx++) {
            /* Set key type */
            soc_mem_field32_set(unit, mem, buf_p, v6typef[idx], 
                                SOC_MEM_KEY_L3_ENTRY_4_IPV6MC_IPV6_MULTICAST);

            /* Set entry valid bit. */
            soc_mem_field32_set(unit, mem, buf_p, validf[idx], 1);

        }

        /* Set vlan id or L3_IIF. */
        if (l3cfg->l3c_vid < 4096) {
            soc_mem_field32_set(unit, mem, buf_p, IPV6MC__VLAN_IDf, l3cfg->l3c_vid);
        } else {
            soc_mem_field32_set(unit, mem, buf_p, IPV6MC__L3_IIFf, l3cfg->l3c_vid);
        }

        /* Set virtual router id. */
        soc_mem_field32_set(unit, mem, buf_p, IPV6MC__VRF_IDf, l3cfg->l3c_vrf);

    } else {
        /* Set group id. */
        soc_mem_field32_set(unit, mem, buf_p, IPV4MC__GROUP_IP_ADDRf,
                            l3cfg->l3c_ip_addr);

        /* Set source address. */
        soc_mem_field32_set(unit, mem, buf_p, IPV4MC__SOURCE_IP_ADDRf,
                            l3cfg->l3c_src_ip_addr);

        for (idx = 0; idx < 2; idx++) {
            /* Set key type */
            soc_mem_field32_set(unit, mem, buf_p, v4typef[idx], 
                                SOC_MEM_KEY_L3_ENTRY_2_IPV4MC_IPV4_MULTICAST);

            /* Set entry valid bit. */
            soc_mem_field32_set(unit, mem, buf_p, validf[idx], 1);
        }

        /* Set vlan id or L3_IIF. */
        if (l3cfg->l3c_vid < 4096) {
            soc_mem_field32_set(unit, mem, buf_p, IPV4MC__VLAN_IDf, l3cfg->l3c_vid);
        } else {
            soc_mem_field32_set(unit, mem, buf_p, IPV4MC__L3_IIFf, l3cfg->l3c_vid);
        }

        /* Set virtual router id. */
        soc_mem_field32_set(unit, mem, buf_p, IPV4MC__VRF_IDf, l3cfg->l3c_vrf);
    }

    return;
}

/*
 * Function:
 *      _bcm_tr3_l3_ent_parse
 * Purpose:
 *      Tr3 helper routine used to parse hw l3 entry to api format.
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN/OUT) l3 entry key & parse result.
 *      nh_idx    - (IN/OUT) Next hop index. 
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
STATIC int
_bcm_tr3_l3_ent_parse(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                                        int *nh_idx, void *l3x_entry)
{
    int idx;                      /* Iteration index.            */
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32 *buf_p;                /* HW buffer address.          */ 
    _bcm_l3_fields_t *fld;        /* L3 table common fields.     */
    int embedded_nh = FALSE;
    soc_field_t ism_hitf[] = {HIT_0f, HIT_1f, HIT_2f};
    soc_field_t esm_hitf[] = {SRC_HITf, DST_HITf};
    int esm = FALSE;
    uint32 glp;
    
    buf_p = (uint32 *)l3x_entry;
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Set table fields */
    if (BCM_TR3_MEM_IS_ESM(unit, mem)) {
        BCM_TR3_ESM_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
        esm = TRUE;
    } else {
        BCM_TR3_ISM_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);  
    }

    /* Check if embedded NH table is being used */
    embedded_nh = (mem == L3_ENTRY_2m && !ipv6) || 
                  (mem == L3_ENTRY_4m && ipv6)  ||
                  (mem == EXT_IPV4_UCAST_WIDEm) ||
                  (mem == EXT_IPV6_128_UCAST_WIDEm);

    /* Reset entry flags */
    l3cfg->l3c_flags = (ipv6) ? BCM_L3_IP6 : 0;

    /* Set hit bit. */
    if (esm) {
        /* For ESM Memory */
        if (soc_mem_field32_get(unit, mem, buf_p, esm_hitf[0])) {
            l3cfg->l3c_flags |= BCM_L3_S_HIT;
        }
        if (soc_mem_field32_get(unit, mem, buf_p, esm_hitf[1])) {
            l3cfg->l3c_flags |= BCM_L3_D_HIT;
        }
    } else {
        /* For ISM Memory */
        for (idx = 0; idx < COUNTOF(ism_hitf); idx++) {
            if (soc_mem_field32_get(unit, mem, buf_p, ism_hitf[idx])) {
                l3cfg->l3c_flags |= BCM_L3_HIT;
            }
        }
    }

    /* Get priority override flag. */
    if (soc_mem_field32_get(unit, mem, buf_p, fld->rpe)) {
        l3cfg->l3c_flags |= BCM_L3_RPE;
    }

    /* Get destination discard flag. */
    if (SOC_MEM_FIELD_VALID(unit, mem, fld->dst_discard)) {
        if (soc_mem_field32_get(unit, mem, buf_p, fld->dst_discard)) {
            l3cfg->l3c_flags |= BCM_L3_DST_DISCARD;
        }
    }

    /* Get local addr bit. */
    if (SOC_MEM_FIELD_VALID(unit, mem, fld->local_addr)) {
        if (soc_mem_field32_get(unit, mem, buf_p, fld->local_addr)) {
            l3cfg->l3c_flags |= BCM_L3_HOST_LOCAL;
        }
    }

    /* Get classification group id. */
    l3cfg->l3c_lookup_class = 
        soc_mem_field32_get(unit, mem, buf_p, fld->class_id);

    /* Get priority. */
    l3cfg->l3c_prio = soc_mem_field32_get(unit, mem, buf_p, fld->priority);

    /* Get virtual router id. */
    l3cfg->l3c_vrf = soc_mem_field32_get(unit, mem, buf_p, fld->vrf);

    /* Get bfd_enable bit. */
    if (SOC_MEM_FIELD_VALID(unit, mem, fld->bfd_enable)) {
        l3cfg->l3c_bfd_enable
            = soc_mem_field32_get(unit, mem, buf_p, fld->bfd_enable);
    }

    if (embedded_nh) {
        uint8 port_mask_len = 0, modid_mask_len = 0;
        uint32 temp = 0;
        
        if (nh_idx) {
            *nh_idx = BCM_XGS3_L3_INVALID_INDEX; /* Embedded NH */
        }
        l3cfg->l3c_intf = soc_mem_field32_get(unit, mem, buf_p,
                                           fld->l3_intf);
        soc_mem_mac_addr_get(unit, mem, buf_p, fld->mac_addr,
 		             l3cfg->l3c_mac_addr);
        glp = soc_mem_field32_get(unit, mem, buf_p, fld->glp);
        /* Determine length of port mask */
        temp = SOC_PORT_ADDR_MAX(unit);
        while (0 != temp) {
            port_mask_len++;
            temp >>= 1;
        }
        /* Determine length of modid mask */
        temp = SOC_MODID_MAX(unit);
        while (0 != temp) {
            modid_mask_len++;
            temp >>= 1;
        }
        
        if (glp & (1 << (port_mask_len + modid_mask_len))) {
            l3cfg->l3c_flags |= BCM_L3_TGID;
            l3cfg->l3c_port_tgid = glp & BCM_TGID_PORT_TRUNK_MASK(unit);
            l3cfg->l3c_modid = 0;
        } else {
            l3cfg->l3c_port_tgid = glp & SOC_PORT_ADDR_MAX(unit);
            l3cfg->l3c_modid = (glp >> port_mask_len)
                & SOC_MODID_MAX(unit);
        }
    } else {
        /* Get next hop info. */
        if (nh_idx) {
            *nh_idx = soc_mem_field32_get(unit, mem, buf_p, fld->nh_idx);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_clear_hit
 * Purpose:
 *      Clear hit bit on l3 entry
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      mem       - (IN)L3 table memory.
 *      l3cfg     - (IN)l3 entry info. 
 *      l3x_entry - (IN)l3 entry filled hw buffer.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_clear_hit(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                                   void *l3x_entry, int l3_entry_idx)
{
    uint32 *buf_p;                /* HW buffer address.       */ 
    soc_field_t hitf[] = { HIT_0f, HIT_1f, HIT_2f};
    int i;

    /* Input parameters check */
    if ((NULL == l3cfg) || (NULL == l3x_entry)) {
        return (BCM_E_PARAM);
    }

    /* Init memory pointers. */ 
    buf_p = (uint32 *)l3x_entry;

    /* If entry was not hit  there is nothing to clear */
    if (!(l3cfg->l3c_flags & BCM_L3_HIT)) {
        return (BCM_E_NONE);
    }

    /* Clear hit bits */
    for (i = 0; i < COUNTOF(hitf); i++) {
        soc_mem_field32_set(unit, mem, buf_p, hitf[i], 0);
    }

    /* Write entry back to hw. */
    return BCM_XGS3_MEM_WRITE(unit, mem, l3_entry_idx, buf_p);
}

/*
 * Function:
 *      _bcm_tr3_l3_ism_add
 * Purpose:
 *      Add and entry to Tr3 ISM L3 host table.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      l3cfg  - (IN) L3 entry info.
 *      nh_idx - (IN) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_ism_add(int unit, _bcm_l3_cfg_t *l3cfg, int nh_idx)
{
    int idx;                  /* Iteration index.        */
    int ipv6;
    uint32 *bufp;             /* Hardware buffer ptr     */
    soc_mem_t mem;            /* L3 table memory.        */  
    int rv = BCM_E_NONE;      /* Operation status.       */
    _bcm_l3_fields_t *fld;    /* L3 table common fields. */
    int embedded_nh = FALSE;
    uint32 glp, port_id, modid;
    _bcm_l3_intf_cfg_t l3_intf;
    l3_entry_1_entry_t l3v4_entry;    
    l3_entry_2_entry_t l3v4v6_entry;
    l3_entry_4_entry_t l3v6_ext_entry;
    soc_field_t hitf[] = {HIT_0f, HIT_1f, HIT_2f};

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Check if embedded NH table is being used */
    embedded_nh = BCM_TR3_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg->l3c_intf, nh_idx);

    /* Get table memory. */
    BCM_TR3_ISM_L3_HOST_TABLE_MEM(unit, l3cfg->l3c_intf, ipv6, mem, nh_idx);
    
    /* Set table fields */
    BCM_TR3_ISM_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);

    /* Assign entry buf based on table being used */
    BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem, bufp,
                                  l3v4_entry,
                                  l3v4v6_entry,
                                  l3v6_ext_entry);

    /* Prepare ISM host entry for addition. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem, l3cfg, bufp));

    /* Set hit bit. */
    for (idx = 0; idx < COUNTOF(hitf); idx++) {
        if (l3cfg->l3c_flags & BCM_L3_HIT) {
            soc_mem_field32_set(unit, mem, bufp, hitf[idx], 1);
        }
    }

    /* Set priority override bit. */
    if (l3cfg->l3c_flags & BCM_L3_RPE) {
        soc_mem_field32_set(unit, mem, bufp, fld->rpe, 1);
    }

    /* Set destination discard bit. */
    if (l3cfg->l3c_flags & BCM_L3_DST_DISCARD) {
        soc_mem_field32_set(unit, mem, bufp, fld->dst_discard, 1);
    }

    /* Set local addr bit. */
    if (l3cfg->l3c_flags & BCM_L3_HOST_LOCAL) {
        soc_mem_field32_set(unit, mem, bufp, fld->local_addr, 1);
    }

    /* Get bfd_enable bit. */
    if (SOC_MEM_FIELD_VALID(unit, mem, fld->bfd_enable)) {
        if (l3cfg->l3c_bfd_enable) {
            soc_mem_field32_set(unit, mem, bufp, fld->bfd_enable, 1);
        }
    }
    /* Set classification group id. */
    soc_mem_field32_set(unit, mem, bufp, fld->class_id, 
                        l3cfg->l3c_lookup_class);
    /*  Set priority. */
    soc_mem_field32_set(unit, mem, bufp, fld->priority, l3cfg->l3c_prio);

    if (embedded_nh) {
        sal_memset(&l3_intf, 0, sizeof(_bcm_l3_intf_cfg_t));
        BCM_XGS3_L3_INTF_IDX_SET(l3_intf, l3cfg->l3c_intf);

        BCM_XGS3_L3_MODULE_LOCK(unit);
        rv = BCM_XGS3_L3_HWCALL_EXEC(unit, if_get) (unit, &l3_intf);
        BCM_XGS3_L3_MODULE_UNLOCK(unit);
        if (rv >= 0) {
            soc_mem_field32_set(unit, mem, bufp, fld->l3_oif, l3_intf.l3i_vid);
        }

        if (l3cfg->l3c_flags & BCM_L3_TGID) {
            port_id = (BCM_TGID_PORT_TRUNK_MASK(unit) & l3cfg->l3c_port_tgid);
            glp = (1 << (SOC_TRUNK_BIT_POS(unit))) |port_id;
        } else {
            port_id = (_BCM_TR3_L3_PORT_MASK & l3cfg->l3c_port_tgid);
            modid   = (_BCM_TR3_L3_MODID_MASK & l3cfg->l3c_modid) <<
                                                  _BCM_TR3_L3_MODID_SHIFT;
            glp = modid | port_id;
        }

        soc_mem_mac_addr_set(unit, mem, bufp,
                             fld->mac_addr, l3cfg->l3c_mac_addr);
        soc_mem_field32_set(unit, mem, bufp, fld->l3_intf, l3cfg->l3c_intf);
        soc_mem_field32_set(unit, mem, bufp, fld->glp, glp);
    } else {
        /* Set next hop index. */
        soc_mem_field32_set(unit, mem, bufp, fld->nh_idx, nh_idx);
    }

    /* Insert in ISM table */
    rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, bufp);

    /* Handle host entry 'replace' actions */
    if ((BCM_E_EXISTS == rv) && (l3cfg->l3c_flags & BCM_L3_REPLACE)) {
        rv = BCM_E_NONE;
    }
    
    /* Write status check. */
    if ((rv >= 0) && (BCM_XGS3_L3_INVALID_INDEX == l3cfg->l3c_hw_index)) {
        (ipv6) ?  BCM_XGS3_L3_IP6_CNT(unit)++ : BCM_XGS3_L3_IP4_CNT(unit)++;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_esm_add
 * Purpose:
 *      Add and entry to Tr3 ESM L3 host table.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      l3cfg  - (IN) L3 entry info.
 *      nh_idx - (IN) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_esm_add(int unit, _bcm_l3_cfg_t *l3cfg, int nh_idx)
{
    int ipv6;
    uint32 *bufp;             /* Hardware buffer ptr */
    soc_mem_t mem;            /* ESM L3 table memory. */  
    soc_mem_t mem_data;       /* ESM L3 table memory - data only */  
    int rv = BCM_E_NONE;      /* Operation status.       */
    _bcm_l3_fields_t *fld;    /* L3 table common fields. */
    int tbl, tbl_free_entry;                 
    int embedded_nh = FALSE;
    uint32 glp, port_id, modid;
    _bcm_l3_intf_cfg_t l3_intf;
    ext_ipv4_ucast_entry_t          l3v4_entry;
    ext_ipv4_ucast_wide_entry_t     l3v4wide_entry;
    ext_ipv6_128_ucast_entry_t      l3v6_entry;
    ext_ipv6_128_ucast_wide_entry_t l3v6wide_entry;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Check if embedded NH table is being used */
    embedded_nh = BCM_TR3_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg->l3c_intf, nh_idx);

    /* Get table memory. */
    BCM_TR3_ESM_L3_HOST_TABLE_MEM(unit, l3cfg->l3c_intf, ipv6, mem, nh_idx);
    
    /* Set table fields */
    BCM_TR3_ESM_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);

    /* Assign entry buf based on table being used */
    BCM_TR3_ESM_L3_HOST_ENTRY_BUF(ipv6, mem, bufp,
                                  l3v4_entry,
                                  l3v4wide_entry,
                                  l3v6_entry,
                                  l3v6wide_entry);

    /* Prepare ESM host entry for addition. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem, l3cfg, bufp));

    /* Set hit bit. */
    if (l3cfg->l3c_flags & BCM_L3_HIT) {
        soc_mem_field32_set(unit, mem, bufp, fld->hit, 1);
    }

    /* Set priority override bit. */
    if (l3cfg->l3c_flags & BCM_L3_RPE) {
        soc_mem_field32_set(unit, mem, bufp, fld->rpe, 1);
    }

    /* Set destination discard bit. */
    if (l3cfg->l3c_flags & BCM_L3_DST_DISCARD) {
        if (SOC_MEM_FIELD_VALID(unit, mem, fld->dst_discard)) {
            soc_mem_field32_set(unit, mem, bufp, fld->dst_discard, 1);
        }
    }

    /* Set local addr bit. */
    if (l3cfg->l3c_flags & BCM_L3_HOST_LOCAL) {
        if (SOC_MEM_FIELD_VALID(unit, mem, fld->local_addr)) {
            soc_mem_field32_set(unit, mem, bufp, fld->local_addr, 1);
        }
    }

    /* Set classification group id. */
    soc_mem_field32_set(unit, mem, bufp, fld->class_id, 
                        l3cfg->l3c_lookup_class);
    /*  Set priority. */
    soc_mem_field32_set(unit, mem, bufp, fld->priority, l3cfg->l3c_prio);

    if (embedded_nh) {
        sal_memset(&l3_intf, 0, sizeof(_bcm_l3_intf_cfg_t));
        BCM_XGS3_L3_INTF_IDX_SET(l3_intf, l3cfg->l3c_intf);

        BCM_XGS3_L3_MODULE_LOCK(unit);
        rv = BCM_XGS3_L3_HWCALL_EXEC(unit, if_get) (unit, &l3_intf);
        BCM_XGS3_L3_MODULE_UNLOCK(unit);
        if (rv >= 0) {
            soc_mem_field32_set(unit, mem, bufp, fld->l3_oif, l3_intf.l3i_vid);
        }

        if (l3cfg->l3c_flags & BCM_L3_TGID) {
            port_id = (BCM_TGID_PORT_TRUNK_MASK(unit) & l3cfg->l3c_port_tgid);
            glp = (1 << (SOC_TRUNK_BIT_POS(unit)))  | port_id;
        } else {
            port_id = (_BCM_TR3_L3_PORT_MASK & l3cfg->l3c_port_tgid);
            modid   = (_BCM_TR3_L3_MODID_MASK & l3cfg->l3c_modid) <<
                                                  _BCM_TR3_L3_MODID_SHIFT;
            glp = modid | port_id;
        }

        soc_mem_mac_addr_set(unit, mem, bufp,
                             fld->mac_addr, l3cfg->l3c_mac_addr);
        soc_mem_field32_set(unit, mem, bufp, fld->l3_intf, l3cfg->l3c_intf);
        soc_mem_field32_set(unit, mem, bufp, fld->glp, glp);
    } else {
        /* Set next hop index. */
        soc_mem_field32_set(unit, mem, bufp, fld->nh_idx, nh_idx);
    }

    /* Write to ESM table */
    BCM_TR3_ESM_TBL_ENUMERATE(mem, tbl);

    /* Perform lookup */
    rv = soc_tr3_search_ext_mem(unit, &mem, bufp, &tbl_free_entry);
    if (BCM_E_NONE == rv) { /* Entry exists */
        if (l3cfg->l3c_flags & BCM_L3_REPLACE) {
            l3cfg->l3c_hw_index = tbl_free_entry;
        } else {
            return rv;
        }
    } else if (BCM_E_NOT_FOUND == rv) {
        tbl_free_entry = _TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl);
        rv = BCM_E_NONE;

    } else {
        return rv;
    }
    
    if (tbl_free_entry <= soc_mem_index_max(unit, mem)) {
        BCM_IF_ERROR_RETURN(soc_mem_write( unit, mem, MEM_BLOCK_ALL,
                              tbl_free_entry, bufp));
        
        /* Point to next free entry in the table */
        _TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl) += 1;
        tbl_free_entry = _TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl);

        /* Set invalid entry marker */
        if (tbl_free_entry <= soc_mem_index_max(unit, mem)) {
            switch(mem) {
              case EXT_IPV4_UCASTm:
                mem_data = EXT_L3_UCAST_DATA_IPV4m;
                break;
              case EXT_IPV4_UCAST_WIDEm:
                mem_data = EXT_L3_UCAST_DATA_WIDE_IPV4m;
                break;
              case EXT_IPV6_128_UCASTm:
                mem_data = EXT_L3_UCAST_DATA_IPV6_128m;
                break;
              case EXT_IPV6_128_UCAST_WIDEm:
                mem_data = EXT_L3_UCAST_DATA_WIDE_IPV6_128m;
                break;
              default:
                return BCM_E_INTERNAL; /* should never happen */
            }
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem_data, MEM_BLOCK_ANY,
                         tbl_free_entry, bufp));
            soc_mem_field32_set(unit, mem_data, bufp, CLASS_IDf,
                                      _TR3_ESM_HOST_ENTRY_INVALID_MARKER);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem_data, MEM_BLOCK_ALL, 
                                 tbl_free_entry, bufp));
        }
  
    } else {
        rv = (BCM_E_FULL);
    }
    
    /* Write status check. */
    if ((rv >= 0) && (BCM_XGS3_L3_INVALID_INDEX == l3cfg->l3c_hw_index)) {
        (ipv6) ?  BCM_XGS3_L3_IP6_CNT(unit)++ : BCM_XGS3_L3_IP4_CNT(unit)++;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_ism_get
 * Purpose:
 *      Get an entry from Tr3 ISM L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 *      embd     - (OUT) If TRUE, entry was found as embedded NH.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_ism_get(int unit, _bcm_l3_cfg_t *l3cfg, int *nh_idx, int *embd)
{
    int ipv6;                                   /* IPv6 entry indicator.    */
    int clear_hit;                              /* Clear hit bit.           */
    int rv = BCM_E_NONE;                        /* Operation return status. */
    soc_mem_t mem, mem_ext;                     /* L3 table memories        */
    uint32 *buf_key, *buf_entry;                /* Key and entry buffer ptrs*/
    l3_entry_1_entry_t l3v4_key, l3v4_entry;    /* Lookup key and entry     */
    l3_entry_2_entry_t l3v4v6_key, l3v4v6_entry;     /* Lookup key and entry*/
    l3_entry_4_entry_t l3v6_ext_key, l3v6_ext_entry; /* Lookup key and entry*/

    /* Indexed NH entry */
    *embd = _BCM_TR3_HOST_ENTRY_NOT_FOUND;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Preserve clear_hit value. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    /* Table memories */
    if (ipv6) {
        mem = BCM_XGS3_L3_MEM(unit, v6); mem_ext = BCM_XGS3_L3_MEM(unit, v6_4);
    } else {
        mem = BCM_XGS3_L3_MEM(unit, v4); mem_ext = BCM_XGS3_L3_MEM(unit, v4_2);
    }

    /* Assign entry-key buf based on table being used */
    BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem, buf_key,
                                        l3v4_key,
                                        l3v4v6_key,
                                        l3v6_ext_key);

    /* Assign entry buf based on table being used */
    BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                        l3v4_entry,
                                        l3v4v6_entry,
                                        l3v6_ext_entry);
    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem, l3cfg, buf_key));

    /* Perform lookup */
    rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY,
                                _BCM_TR3_L3_MEM_BANKS_ALL,
                                buf_key, buf_entry, &l3cfg->l3c_hw_index);
    if (BCM_SUCCESS(rv)) {
        /* Indexed NH entry */
        *embd = FALSE;
        /* Extract entry info. */
        BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_parse(unit, mem, l3cfg,
                                                  nh_idx, buf_entry));
        /* Clear the HIT bit */
        if (clear_hit) {
            BCM_IF_ERROR_RETURN(_bcm_tr3_l3_clear_hit(unit, mem,
                                       l3cfg, buf_entry, l3cfg->l3c_hw_index));
        }
    } else if ((BCM_E_NOT_FOUND == rv) &&
               (soc_feature(unit, soc_feature_l3_extended_host_entry))) {
        /* Search in the extended tables only if the extended host entry
         * feature is supported and if the entry was not found in the 
         * regular host entry */

        /* Assign entry-key buf based on table being used */
        BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_key,
                                            l3v4_key,
                                            l3v4v6_key,
                                            l3v6_ext_key);

        /* Assign entry buf based on table being used */
        BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_entry,
                                            l3v4_entry,
                                            l3v4v6_entry,
                                            l3v6_ext_entry);

        /* Prepare lookup key. */
        BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem_ext, l3cfg, buf_key));

        /* Perform lookup hw. */
        rv = soc_mem_generic_lookup(unit, mem_ext, MEM_BLOCK_ANY,
                                    _BCM_TR3_L3_MEM_BANKS_ALL,
                                    buf_key, buf_entry, &l3cfg->l3c_hw_index);

        if (BCM_SUCCESS(rv)) {

            /* Embedded NH entry */
            *embd = TRUE;
            /* Extract entry info. */
            BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_parse(unit, mem_ext, l3cfg,
                                                          nh_idx, buf_entry));
            /* Clear the HIT bit */
            if (clear_hit) {
                BCM_IF_ERROR_RETURN(_bcm_tr3_l3_clear_hit(unit, mem_ext,
                                              l3cfg, buf_entry, l3cfg->l3c_hw_index));
            }
        }
    } 
     
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_esm_get
 * Purpose:
 *      Get an entry from Tr3 ESM L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 *      embd     - (OUT) If TRUE, entry was found as embedded NH.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_esm_get(int unit, _bcm_l3_cfg_t *l3cfg, int *nh_idx, int *embd)
{
    int ipv6;                                   /* IPv6 entry indicator.    */
    int clear_hit;                              /* Clear hit bit.           */
    int rv = BCM_E_NONE;                        /* Operation return status. */
    soc_mem_t mem;                              /* L3 table memories        */
    uint32 *buf_key;                            /* Key and entry buffer ptrs*/
    ext_ipv4_ucast_entry_t          l3v4_key;
    ext_ipv4_ucast_wide_entry_t     l3v4wide_key;
    ext_ipv6_128_ucast_entry_t      l3v6_key;
    ext_ipv6_128_ucast_wide_entry_t l3v6wide_key;

    /* Indexed NH entry */
    *embd = _BCM_TR3_HOST_ENTRY_NOT_FOUND;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Preserve clear_hit value. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    /* Table memories */
    if (ipv6) {
        mem = BCM_XGS3_L3_MEM(unit, v6_esm);
    } else {
        mem = BCM_XGS3_L3_MEM(unit, v4_esm);
    }

    /* Assign entry-key buf based on table being used */
    BCM_TR3_ESM_L3_HOST_ENTRY_BUF(ipv6, mem, buf_key,
                                        l3v4_key,
                                        l3v4wide_key,
                                        l3v6_key,
                                        l3v6wide_key);

    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem, l3cfg, buf_key));

    /* Perform lookup */
    rv = soc_tr3_search_ext_mem(unit, &mem, buf_key, &l3cfg->l3c_hw_index);

    if (BCM_SUCCESS(rv)) {
        /* Set to true if the entry was found in a wide host entry */
        *embd = ((mem == BCM_XGS3_L3_MEM(unit, v6_esm_wide)) ||
                 (mem == BCM_XGS3_L3_MEM(unit, v4_esm_wide))) ? TRUE : FALSE;

        if ((*embd) &&
            (!soc_feature(unit, soc_feature_l3_extended_host_entry))) {
            /* Flag error if the entry was found in the extended host table
             * but the extended host entries are not supported
             * NOTE: This should never occur, as the 'host add' operation
             * should take care of this. */  
            return BCM_E_PARAM;
        }        

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                            l3cfg->l3c_hw_index, buf_key));
        /* Extract entry info. */
        BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_parse(unit, mem, l3cfg,
                                                  nh_idx, buf_key));
        /* Clear the HIT bit */
        if (clear_hit) {
            BCM_IF_ERROR_RETURN(_bcm_tr3_l3_clear_hit(unit, mem,
                                       l3cfg, buf_key, l3cfg->l3c_hw_index));
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_ism_del
 * Purpose:
 *      Delete an entry from Tr3 ISM L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_ism_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    int ipv6;                          /* IPv6 entry indicator.*/
    soc_mem_t mem, mem_ext;            /* L3 table memory.     */  
    int rv = BCM_E_NONE;               /* Operation status.    */
    uint32 *buf_entry;                 /* Entry buffer ptrs    */
    l3_entry_1_entry_t l3v4_entry;     /* Lookup entry */
    l3_entry_2_entry_t l3v4v6_entry;   /* Lookup entry */
    l3_entry_4_entry_t l3v6_ext_entry; /* Lookup entry */

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    if (ipv6) {
        mem = BCM_XGS3_L3_MEM(unit, v6); mem_ext = BCM_XGS3_L3_MEM(unit, v6_4);
    } else {
        mem = BCM_XGS3_L3_MEM(unit, v4); mem_ext = BCM_XGS3_L3_MEM(unit, v4_2);
    }

    /* Assign entry-key buf based on table being used */
    BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                        l3v4_entry,
                                        l3v4v6_entry,
                                        l3v6_ext_entry);
    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem, l3cfg, buf_entry));

    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void*) buf_entry);

    if ((BCM_E_NOT_FOUND == rv) &&
        (soc_feature(unit, soc_feature_l3_extended_host_entry))) { 
        /* Delete from  the extended tables only if the extended host entry
         * feature is supported and if the entry was not found in the 
         * regular host entry */

        /* Assign entry-key buf based on table being used */
        BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_entry,
                                            l3v4_entry,
                                            l3v4v6_entry,
                                            l3v6_ext_entry);
        /* Prepare lookup key. */
        BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem_ext, l3cfg, buf_entry));
    
        rv = soc_mem_delete(unit, mem_ext, MEM_BLOCK_ANY, (void*) buf_entry);
    }

    /* Write status check. */
    if (BCM_SUCCESS(rv)) {
        (ipv6) ?  BCM_XGS3_L3_IP6_CNT(unit)-- : BCM_XGS3_L3_IP4_CNT(unit)--;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_esm_del
 * Purpose:
 *      Delete an entry from Tr3 ESM L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_esm_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    int tbl;                  /* Table enumeration    */
    int ipv6;                 /* IPv6 entry indicator.*/
    int l3_entry_idx;         /* Index of L3 host entry  */
    void *null_entry;         /* Null entry for delete opearation */
    int rv = BCM_E_NONE;      /* Operation status.    */
    soc_mem_t mem;            /* L3 table memory.     */  
    soc_mem_t mem_data;       /* L3 table memory - associated data */  
    uint32 *buf_key, *buf_entry = NULL; /* Key and entry buffer ptrs*/
    ext_ipv4_ucast_entry_t          l3v4_entry;    
    ext_ipv4_ucast_wide_entry_t     l3v4wide_entry;
    ext_ipv6_128_ucast_entry_t      l3v6_entry; 
    ext_ipv6_128_ucast_wide_entry_t l3v6wide_entry;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    if (ipv6) {
        mem = BCM_XGS3_L3_MEM(unit, v6_esm);
    } else {
        mem = BCM_XGS3_L3_MEM(unit, v4_esm); 
    }

    /* Assign entry-key buf based on table being used */
    BCM_TR3_ESM_L3_HOST_ENTRY_BUF(ipv6, mem, buf_key,
                                        l3v4_entry,
                                        l3v4wide_entry,
                                        l3v6_entry,
                                        l3v6wide_entry);

    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem, l3cfg, buf_key));

    /* Lookup TCAM with the key provided */
    rv = soc_tr3_search_ext_mem(unit, &mem, buf_key, &l3_entry_idx);
    if (BCM_SUCCESS(rv)) {

        if ((!soc_feature(unit, soc_feature_l3_extended_host_entry)) &&
            ((mem == EXT_IPV4_UCAST_WIDEm) || (mem == EXT_IPV6_128_UCAST_WIDEm))) {
            /* Flag error if the entry was found in the extended host table
             * of ESM but the extended host entries are not supported
             * NOTE: This should never occur, as the 'host add' operation
             * should take care of this. */  
            return BCM_E_PARAM;
        }

        /* Write null entry at l3_entry_idx, clear vbit */
        null_entry = soc_mem_entry_null(unit, mem);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                         l3_entry_idx, null_entry));
        soc_tr3_set_vbit(unit, mem, l3_entry_idx, 0);

        /* Move the last valid entry in place of deleted entry to keep
         * entries contiguous - this works because host entry hit is 
         * an exact match, not LPM */
        BCM_TR3_ESM_TBL_ENUMERATE(mem, tbl);
        BCM_TR3_ESM_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                            l3v4_entry,
                                            l3v4wide_entry,
                                            l3v6_entry,
                                            l3v6wide_entry);
        /* Read last entry */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                            (_TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl) - 1),
                            buf_entry));
        /* Write last entry to index of entry that was deleted */
        /* This entry is now valid */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                            l3_entry_idx, buf_entry));

        /* The last entry is now invalid because it has been moved in place 
         * of the deleted entry. Write invalid entry marker here */
        soc_tr3_set_vbit(unit, mem,
                         (_TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl) - 1), 0);
        switch(mem) {
          case EXT_IPV4_UCASTm:
              mem_data = EXT_L3_UCAST_DATA_IPV4m;
            break;
          case EXT_IPV4_UCAST_WIDEm:
              mem_data = EXT_L3_UCAST_DATA_WIDE_IPV4m;
            break;
          case EXT_IPV6_128_UCASTm:
              mem_data = EXT_L3_UCAST_DATA_IPV6_128m;
            break;
          case EXT_IPV6_128_UCAST_WIDEm:
              mem_data = EXT_L3_UCAST_DATA_WIDE_IPV6_128m;
            break;
          default:
              mem_data = INVALIDm;
            break;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem_data, MEM_BLOCK_ANY,
                     (_TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl)-1),
                     buf_entry));
        soc_mem_field32_set(unit, mem_data, buf_entry, CLASS_IDf,
                                            _TR3_ESM_HOST_ENTRY_INVALID_MARKER);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem_data, MEM_BLOCK_ALL,
                                  (_TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl) - 1),
                                  buf_entry));
        
        /* Update free entry pointer */
        _TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl) -= 1;
    }

    /* Write status check. */
    if (BCM_SUCCESS(rv)) {
        (ipv6) ?  BCM_XGS3_L3_IP6_CNT(unit)-- : BCM_XGS3_L3_IP4_CNT(unit)--;
    }

    return rv;
}

STATIC INLINE int
_bcm_tr3_ip_key_to_l3cfg(int unit, bcm_l3_key_t *ipkey, _bcm_l3_cfg_t *l3cfg)
{
    int ipv6;           /* IPV6 key.                     */

    /* Input parameters check */
    if ((NULL == ipkey) || (NULL == l3cfg)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination buffer first. */
    sal_memset(l3cfg, 0, sizeof(_bcm_l3_cfg_t));

    ipv6 = (ipkey->l3k_flags & BCM_L3_IP6);

    /* Set vrf id. */
    l3cfg->l3c_vrf = ipkey->l3k_vrf;
    l3cfg->l3c_vid = ipkey->l3k_vid;

    if (ipv6) {
        if (BCM_IP6_MULTICAST(ipkey->l3k_ip6_addr)) {
            /* Copy ipv6 group, source & vid. */
            sal_memcpy(l3cfg->l3c_ip6, ipkey->l3k_ip6_addr, sizeof(bcm_ip6_t));
            sal_memcpy(l3cfg->l3c_sip6, ipkey->l3k_sip6_addr,
                       sizeof(bcm_ip6_t));
            l3cfg->l3c_vid = ipkey->l3k_vid;
            l3cfg->l3c_flags = (BCM_L3_IP6 | BCM_L3_IPMC);
        } else {
            /* Copy ipv6 address. */
            sal_memcpy(l3cfg->l3c_ip6, ipkey->l3k_ip6_addr, sizeof(bcm_ip6_t));
            l3cfg->l3c_flags = BCM_L3_IP6;
        }
    } else {
        if (BCM_IP4_MULTICAST(ipkey->l3k_ip_addr)) {
            /* Copy ipv4 mcast group, source & vid. */
            l3cfg->l3c_ip_addr = ipkey->l3k_ip_addr;
            l3cfg->l3c_src_ip_addr = ipkey->l3k_sip_addr;
            l3cfg->l3c_vid = ipkey->l3k_vid;
            l3cfg->l3c_flags = BCM_L3_IPMC;
        } else {
            /* Copy ipv4 address . */
            l3cfg->l3c_ip_addr = ipkey->l3k_ip_addr;
        }
    }
    return (BCM_E_NONE);
}

STATIC INLINE int
_bcm_tr3_l3cfg_to_ipkey(int unit, bcm_l3_key_t *ipkey, _bcm_l3_cfg_t *l3cfg)
{
    /* Input parameters check */
    if ((NULL == ipkey) || (NULL == l3cfg)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination buffer first. */
    sal_memset(ipkey, 0, sizeof(bcm_l3_key_t));

    ipkey->l3k_vrf = l3cfg->l3c_vrf;
    ipkey->l3k_vid = l3cfg->l3c_vid;

    if (l3cfg->l3c_flags & BCM_L3_IP6) {
        if (l3cfg->l3c_flags & BCM_L3_IPMC) {
            /* Copy ipv6 group, source & vid. */
            sal_memcpy(ipkey->l3k_ip6_addr, l3cfg->l3c_ip6, sizeof(bcm_ip6_t));
            sal_memcpy(ipkey->l3k_sip6_addr, l3cfg->l3c_sip6,
                       sizeof(bcm_ip6_t));
            ipkey->l3k_vid = l3cfg->l3c_vid;
        } else {
            /* Copy ipv6 address. */
            sal_memcpy(ipkey->l3k_ip6_addr, l3cfg->l3c_ip6, sizeof(bcm_ip6_t));
        }
    } else {
        if (l3cfg->l3c_flags & BCM_L3_IPMC) {
            /* Copy ipv4 mcast group, source & vid. */
            ipkey->l3k_ip_addr = l3cfg->l3c_ip_addr;
            ipkey->l3k_sip_addr = l3cfg->l3c_src_ip_addr;
            ipkey->l3k_vid = l3cfg->l3c_vid;
        } else {
            /* Copy ipv4 address . */
            ipkey->l3k_ip_addr = l3cfg->l3c_ip_addr;
        }
    }
    /* Store entry flags. */
    ipkey->l3k_flags = l3cfg->l3c_flags;
    return (BCM_E_NONE);
}

int
bcm_tr3_l3_conflict_get(int unit, bcm_l3_key_t *ipkey, bcm_l3_key_t *cf_array,
                        int cf_max, int *cf_count)
{
    _bcm_l3_cfg_t l3cfg;
    uint32 *bufk, *bufp;
    soc_mem_t mem;
    int nh_idx, index, rv = BCM_E_NONE;
    _bcm_l3_fields_t *fld;
    l3_entry_1_entry_t l3v4_key, l3v4_entry;    
    l3_entry_2_entry_t l3v4v6_key, l3v4v6_entry;
    l3_entry_4_entry_t l3v6_ext_key, l3v6_ext_entry;
    _soc_ism_mem_banks_t mem_banks;
    uint8 i, bidx, num_ent;
    uint32 result, bucket_index, *keyt;
    uint32 l3_entry[SOC_MAX_MEM_WORDS];
    int ipmc, ipv6, embedded_nh;
    uint32 key_type_1[] = {SOC_MEM_KEY_L3_ENTRY_1_IPV4UC_IPV4_UNICAST, -1, -1};
    uint32 key_type_2[] = {SOC_MEM_KEY_L3_ENTRY_2_IPV4UC_IPV4_UNICAST, 
                           SOC_MEM_KEY_L3_ENTRY_2_IPV4MC_IPV4_MULTICAST,
                           SOC_MEM_KEY_L3_ENTRY_2_IPV6UC_IPV6_UNICAST};
    uint32 key_type_4[] = {SOC_MEM_KEY_L3_ENTRY_4_IPV6UC_IPV6_UNICAST,
                           SOC_MEM_KEY_L3_ENTRY_4_IPV6MC_IPV6_MULTICAST, -1};

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if ((NULL == ipkey) || (NULL == cf_count) || 
        (NULL == cf_array) || (cf_max <= 0)) {
        return (BCM_E_PARAM);
    }

    /* The wider entry may be specified or not */
    embedded_nh = ipkey->l3k_flags & BCM_L3_DEREFERENCED_NEXTHOP;
    nh_idx = (embedded_nh) ? BCM_XGS3_L3_INVALID_INDEX : 0;

    /* Translate lookup key to l3cfg format. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_ip_key_to_l3cfg(unit, ipkey, &l3cfg));
    
    /* Get entry type. */
    ipv6 = (l3cfg.l3c_flags & BCM_L3_IP6);
    ipmc = (l3cfg.l3c_flags & BCM_L3_IPMC);

    /* Get table memory. */
    BCM_TR3_ISM_L3_HOST_TABLE_MEM(unit, l3cfg.l3c_intf, ipv6, mem, nh_idx);
    
    if (!embedded_nh) {
        /* Look for the entry in wider tables */
        if (ipv6) {
            mem = BCM_XGS3_L3_MEM(unit, v6_4);
        } else {
            mem = BCM_XGS3_L3_MEM(unit, v4_2);
        }
        /* Assign entry-key buf based on table being used */
        BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem, bufk, l3v4_key,
                                      l3v4v6_key, l3v6_ext_key);
        
        /* Assign entry buf based on table being used */
        BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem, bufp, l3v4_entry,
                                      l3v4v6_entry, l3v6_ext_entry);
        /* Prepare lookup key. */
        if (ipmc) {
            _bcm_tr3_l3_ipmc_ent_init(unit, bufk, &l3cfg);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem, &l3cfg, bufk));
        }
        
        /* Perform lookup */
        rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY,
                                    _BCM_TR3_L3_MEM_BANKS_ALL,
                                    bufk, bufp, &l3cfg.l3c_hw_index);
        if (BCM_SUCCESS(rv)) {
            embedded_nh = BCM_L3_DEREFERENCED_NEXTHOP;
        }
    }
    
    /* Bump up the size conditionally */
    mem = (embedded_nh || ipmc) ? (ipv6 ? L3_ENTRY_4m : L3_ENTRY_2m) :
                                  (ipv6 ? L3_ENTRY_2m : L3_ENTRY_1m);

    /* Set the key type field array */
    keyt = (embedded_nh || ipmc) ? (ipv6 ? key_type_4 : key_type_2) : 
                                   (ipv6 ? key_type_2 : key_type_1);
    
    /* Set table fields */
    BCM_TR3_ISM_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);

    /* Assign entry buf based on table being used */
    BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem, bufp, l3v4_entry,
                                  l3v4v6_entry, l3v6_ext_entry);

    /* Prepare ISM host entry */
    if (ipmc) {
        _bcm_tr3_l3_ipmc_ent_init(unit, bufp, &l3cfg);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l3_ent_init(unit, mem, &l3cfg, bufp));
    }
        
    *cf_count = 0;

    rv = soc_ism_get_banks_for_mem(unit, mem, mem_banks.banks, 
                                   mem_banks.bank_size, &mem_banks.count);
    if (SOC_FAILURE(rv)) {
        return BCM_E_INTERNAL;
    }
    if (mem_banks.count == 0) {
        return BCM_E_NOT_FOUND;
    }
    for (bidx = 0; bidx < mem_banks.count; bidx++) {
        rv = soc_generic_hash(unit, mem, bufp, (uint32)1 << mem_banks.banks[bidx],
                              0, &index, &result, &bucket_index, &num_ent);
        if (SOC_FAILURE(rv)) {
            return BCM_E_INTERNAL;
        }
        for (i = 0; (i < num_ent) && (*cf_count < cf_max); i++) {
            rv = soc_mem_read(unit, mem, COPYNO_ALL, index + i, l3_entry);
            if (SOC_FAILURE(rv)) {
                return BCM_E_MEMORY;
            }
            if (soc_mem_field32_get(unit, mem, &l3_entry, fld->valid) &&
                ((soc_mem_field32_get(unit, mem, &l3_entry, fld->key_type) == keyt[0]) ||
                 (soc_mem_field32_get(unit, mem, &l3_entry, fld->key_type) == keyt[1]) ||
                 (soc_mem_field32_get(unit, mem, &l3_entry, fld->key_type) == keyt[2]))) {
                switch(soc_mem_field32_get(unit, mem, &l3_entry, fld->key_type)) {
                case SOC_MEM_KEY_L3_ENTRY_1_IPV4UC_IPV4_UNICAST:
                      l3cfg.l3c_flags = 0;
                      break;
                case SOC_MEM_KEY_L3_ENTRY_2_IPV4UC_IPV4_UNICAST:
                      l3cfg.l3c_flags = BCM_L3_DEREFERENCED_NEXTHOP;
                      break;
                case SOC_MEM_KEY_L3_ENTRY_2_IPV4MC_IPV4_MULTICAST:
                      l3cfg.l3c_flags = BCM_L3_IPMC;
                      break;
                case SOC_MEM_KEY_L3_ENTRY_2_IPV6UC_IPV6_UNICAST:
                      l3cfg.l3c_flags = BCM_L3_IP6;
                      break;
                case SOC_MEM_KEY_L3_ENTRY_4_IPV6UC_IPV6_UNICAST:
                      l3cfg.l3c_flags = BCM_L3_IP6 | BCM_L3_DEREFERENCED_NEXTHOP;
                      break;
                case SOC_MEM_KEY_L3_ENTRY_4_IPV6MC_IPV6_MULTICAST:
                      l3cfg.l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC;
                      break;
                default:
                      break;
                }

                rv = _bcm_tr3_l3_get_by_idx(unit, NULL, index + i,
                                                                &l3cfg, &nh_idx);
                if (BCM_FAILURE(rv)) {
                    continue;
                }

                BCM_IF_ERROR_RETURN
                    (_bcm_tr3_l3cfg_to_ipkey(unit, cf_array + (*cf_count), &l3cfg));
                if ((++(*cf_count)) >= cf_max) {
                    break;
                }
            }
        }
    } 
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_get_host_ent_by_idx
 * Purpose:
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      dma_ptr  - (IN)Table pointer in dma. 
 *      soc_mem_ - (IN)Table memory.
 *      idx      - (IN)Index to read.
 *      l3cfg    - (OUT)l3 entry search result.
 *      nh_index - (OUT)Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_get_host_ent_by_idx(int unit, void *dma_ptr, soc_mem_t mem,
                                int idx, _bcm_l3_cfg_t *l3cfg, int *nh_idx)
{
    uint32 ipv6;                       /* IPv6 entry indicator.   */
    int key_type;
    int clear_hit;                     /* Clear hit bit flag.     */
    int esm = TRUE;
    _bcm_l3_fields_t *fld;             /* L3 table common fields. */
    uint32 *buf_entry;                 /* Key and entry buffer ptrs*/
    l3_entry_1_entry_t l3v4_entry;     /* Lookup entry     */
    l3_entry_2_entry_t l3v4v6_entry;   /* Lookup entry*/
    l3_entry_4_entry_t l3v6_ext_entry; /* Lookup entry*/
    ext_ipv4_ucast_entry_t          l3v4esm_entry;    
    ext_ipv4_ucast_wide_entry_t     l3v4wide_entry;
    ext_ipv6_128_ucast_entry_t      l3v6_entry; 
    ext_ipv6_128_ucast_wide_entry_t l3v6wide_entry;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    /* Get clear hit flag. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    if ((BCM_XGS3_L3_MEM(unit, v4) == mem)   ||
        (BCM_XGS3_L3_MEM(unit, v4_2) == mem) ||
        (BCM_XGS3_L3_MEM(unit, v6_4) == mem)) {
        esm = FALSE;
        BCM_TR3_ISM_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
        BCM_TR3_ISM_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                            l3v4_entry,
                                            l3v4v6_entry,
                                            l3v6_ext_entry);
    } else {
        esm = TRUE;
        BCM_TR3_ESM_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
        BCM_TR3_ESM_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                            l3v4esm_entry,
                                            l3v4wide_entry,
                                            l3v6_entry,
                                            l3v6wide_entry);
    }
    
    if (NULL == dma_ptr) { /* Read from hardware. */
        /* Zero buffers. */
        sal_memset(buf_entry, 0, BCM_L3_MEM_ENT_SIZE(unit, mem));

        /* Read entry from hw. */
        BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, buf_entry)); 
    } else { /* Read from dma. */
        buf_entry = soc_mem_table_idx_to_pointer(unit, mem,
                                   uint32 *, dma_ptr, idx);
    }

    if (esm) {
        if (((BCM_XGS3_L3_MEM(unit, v6_esm)) == mem) || 
            ((BCM_XGS3_L3_MEM(unit, v6_esm_wide)) == mem)) {
            l3cfg->l3c_flags = BCM_L3_IP6;
            ipv6 = TRUE;
        }
        
        if (ipv6) {
            soc_mem_ip6_addr_get(unit, mem, buf_entry,
                                 IP_ADDRf, l3cfg->l3c_ip6, 0);
        }

    } else {
        if(!soc_mem_field32_get(unit, mem, buf_entry, fld->valid)) {
            return (BCM_E_NOT_FOUND);
        }
  
        key_type = soc_mem_field32_get(unit, mem, buf_entry, fld->key_type);

        if (BCM_XGS3_L3_MEM(unit, v4) == mem) {
            if (key_type != SOC_MEM_KEY_L3_ENTRY_1_IPV4UC_IPV4_UNICAST) {
                return (BCM_E_NOT_FOUND);
            }
            l3cfg->l3c_flags = 0;
        } else if (BCM_XGS3_L3_MEM(unit, v4_2) == mem) {
            if (key_type == SOC_MEM_KEY_L3_ENTRY_2_IPV4UC_IPV4_UNICAST) {
                l3cfg->l3c_flags = 0;
            } else if (key_type ==
                    SOC_MEM_KEY_L3_ENTRY_2_IPV4MC_IPV4_MULTICAST) {
                l3cfg->l3c_flags = BCM_L3_IPMC;
            } else if (key_type ==
                    SOC_MEM_KEY_L3_ENTRY_2_IPV6UC_IPV6_UNICAST) {
                l3cfg->l3c_flags = BCM_L3_IP6;
            } else {
                return (BCM_E_NOT_FOUND);
            }
        } else if (BCM_XGS3_L3_MEM(unit, v6_4) == mem) {
            if (key_type == SOC_MEM_KEY_L3_ENTRY_4_IPV6UC_IPV6_UNICAST) {
                l3cfg->l3c_flags = BCM_L3_IP6;
            } else if (key_type == 
                    SOC_MEM_KEY_L3_ENTRY_4_IPV6MC_IPV6_MULTICAST) {
                l3cfg->l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC;
            } else {
                return (BCM_E_NOT_FOUND);
            }
        }

        /* Ignore protocol mismatch & multicast entries. */
        if ((ipv6  != (l3cfg->l3c_flags & BCM_L3_IP6)) ||
            (l3cfg->l3c_flags & BCM_L3_IPMC)) {
            return (BCM_E_NONE);
        }

        if (ipv6) {
            /* Extract host info from the entry. */
            soc_mem_ip6_addr_get(unit, mem, buf_entry,
                                 IPV6UC__IP_ADDR_LWR_64f, l3cfg->l3c_ip6,
                                 SOC_MEM_IP6_LOWER_ONLY);

            soc_mem_ip6_addr_get(unit, mem, buf_entry,
                                 IPV6UC__IP_ADDR_UPR_64f, l3cfg->l3c_ip6,
                                 SOC_MEM_IP6_UPPER_ONLY);
        }
    }

    /* Set index to l3cfg. */
    l3cfg->l3c_hw_index = idx;

    if (!ipv6) {
        l3cfg->l3c_ip_addr = 
            soc_mem_field32_get(unit, mem, buf_entry, fld->ip4);
    }

    /* Parse entry data. */
    BCM_IF_ERROR_RETURN(
        _bcm_tr3_l3_ent_parse(unit, mem, l3cfg, nh_idx, buf_entry));

    /* Clear the HIT bit */
    if (clear_hit) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l3_clear_hit(unit, mem, l3cfg,
                                           (void *)buf_entry, idx));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr3_l3_get_tbl_info
 * Purpose:
 *      Get an entry from Tr3 ISM/ESM L3 host table
 *      and also the info where it was found.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 *      esm      - (OUT) If TRUE, entry found in ESM. 
 *      embd     - (OUT) If TRUE, entry is embedded NH entry.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_get_tbl_info(int unit, _bcm_l3_cfg_t *l3cfg,
                          int *nh_idx, int *esm, int *embd)
{
    int rv;
    *esm  = _BCM_TR3_HOST_ENTRY_NOT_FOUND;
    *embd = _BCM_TR3_HOST_ENTRY_NOT_FOUND;

    /* Search ISM tables first */
    rv = _bcm_tr3_l3_ism_get(unit, l3cfg, nh_idx, embd);
    if (BCM_E_NOT_FOUND == rv && BCM_TR3_ESM_HOST_TBL_PRESENT(unit)) {       
        /* Not found in ISM, search ESM */
        rv = _bcm_tr3_l3_esm_get(unit, l3cfg, nh_idx, embd);
        if (BCM_SUCCESS(rv)) {
            *esm = TRUE;
        }
        return rv;
    } else if (BCM_FAILURE(rv)) { 
        /* ISM search failed */
        return rv;
    } else {                      
        /* Found match in ISM */
        *esm = FALSE;
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      _bcm_tr3_l3_ipmc_ent_parse
 * Purpose:
 *      Service routine used to parse hw l3 ipmc entry to api format.
 * Parameters:
 *      unit      - (IN)SOC unit number. 
 *      l3cfg     - (IN/OUT)l3 entry parsing destination buf.
 *      l3x_entry - (IN/OUT)hw buffer.
 * Returns:
 *      void
 */
STATIC void
_bcm_tr3_l3_ipmc_ent_parse(int unit, _bcm_l3_cfg_t *l3cfg,
                          l3_entry_4_entry_t *l3x_entry)
{
    soc_mem_t mem;                     /* IPMC table memory.    */
    uint32 *buf_p;                     /* HW buffer address.    */
    int ipv6;                          /* IPv6 entry indicator. */
    int idx;                           /* Iteration index.      */
    soc_field_t hitf[] = {HIT_0f, HIT_1f, HIT_2f};
    soc_field_t rpe, dst_discard, vrf_id, l3mc_idx, class_id, pri;

    buf_p = (uint32 *)l3x_entry;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory and table fields */
    if (ipv6) {
        mem         = L3_ENTRY_4m;
        pri         = IPV6MC__PRIf;
        rpe         = IPV6MC__RPEf;
        vrf_id      = IPV6MC__VRF_IDf;
        l3mc_idx    = IPV6MC__L3MC_INDEXf;
        class_id    = IPV6MC__CLASS_IDf;
        dst_discard = IPV6MC__DST_DISCARDf;
    } else {
        mem         = L3_ENTRY_2m;
        pri         = IPV4MC__PRIf;
        rpe         = IPV4MC__RPEf;
        vrf_id      = IPV4MC__VRF_IDf;
        l3mc_idx    = IPV4MC__L3MC_INDEXf;
        class_id    = IPV4MC__CLASS_IDf;
        dst_discard = IPV4MC__DST_DISCARDf;
    }

    /* Mark entry as multicast & clear rest of the flags. */
    l3cfg->l3c_flags = BCM_L3_IPMC;
    if (ipv6) {
       l3cfg->l3c_flags |= BCM_L3_IP6;
    }

    /* Read hit value. */
    if(soc_mem_field32_get(unit, mem, buf_p, HIT_0f)) {
        l3cfg->l3c_flags |= BCM_L3_HIT;
    } else if (ipv6) {
        /* Get hit bit. */
        for (idx = 0; idx < 3; idx++) {
            if(soc_mem_field32_get(unit, mem, buf_p, hitf[idx])) { 
                l3cfg->l3c_flags |= BCM_L3_HIT;
                break;
            }
        }
    }

    /* Set ipv6 group address to multicast. */
    if (ipv6) {
        l3cfg->l3c_ip6[0] = 0xff;   /* Set entry ip to mcast address. */
    }

    /* Read priority override */
    if(soc_mem_field32_get(unit, mem, buf_p, rpe)) { 
        l3cfg->l3c_flags |= BCM_L3_RPE;
    }

    /* Read destination discard bit. */
    if(soc_mem_field32_get(unit, mem, buf_p, dst_discard)) { 
        l3cfg->l3c_flags |= BCM_L3_DST_DISCARD;
    }

    /* Read Virtual Router Id. */
    l3cfg->l3c_vrf = soc_mem_field32_get(unit, mem, buf_p, vrf_id);

    /* Pointer to ipmc replication table. */
    l3cfg->l3c_ipmc_ptr = soc_mem_field32_get(unit, mem, buf_p, l3mc_idx);

    /* Classification lookup class id. */
    l3cfg->l3c_lookup_class = soc_mem_field32_get(unit, mem, buf_p, class_id);

    /* Read priority value. */
    l3cfg->l3c_prio = soc_mem_field32_get(unit, mem, buf_p, pri);
    return;
}

/*
 * Function:
 *      _bcm_tr3_l3_defip_mem_get
 * Purpose:
 *      Resolve route table memory for a given route entry.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      flags      - (IN)IPv6/IPv4 route.
 *      plen       - (IN)Prefix length.
 *      mem        - (OUT)Route table memory.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l3_defip_mem_get(int unit, uint32 flags, int plen, soc_mem_t *mem)
{
    /* default value */
    *mem = L3_DEFIPm;

    if (flags & BCM_L3_IP6) {
        if (plen > 64) {
            *mem = (BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV6_128_DEFIPm)) ?\
                    EXT_IPV6_128_DEFIPm : L3_DEFIP_PAIR_128m;
        } else {
            *mem = (BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV6_64_DEFIPm)) ?\
                    EXT_IPV6_64_DEFIPm :
                   ((BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV6_128_DEFIPm)) ?\
                    EXT_IPV6_128_DEFIPm : L3_DEFIPm);
        }
    } else {
        *mem = (BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV4_DEFIPm)) ?\
                EXT_IPV4_DEFIPm : L3_DEFIPm ;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_defip_pair128_init
 * Purpose:
 *      Initialize sw image for L3_DEFIP_PAIR_128 internal tcam.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_defip_pair128_init(int unit)
{
    int rv;    
    int mem_sz;   
    int defip_config;
    int ipv6_128_depth;
    int ranger_device;    /* Ranger specific config */

    /* De-allocate any existing structures. */
    if (BCM_DEFIP_PAIR128(unit) != NULL) {
        rv = _bcm_defip_pair128_deinit(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Allocate cam control structure. */
    mem_sz = sizeof(_bcm_defip_pair128_table_t);
    BCM_DEFIP_PAIR128(unit) = sal_alloc(mem_sz, "tr3_l3_defip_pair128");
    if (BCM_DEFIP_PAIR128(unit) == NULL) {
        return (BCM_E_MEMORY);
    }

    /* Reset cam control structure. */
    sal_memset(BCM_DEFIP_PAIR128(unit), 0, mem_sz);

    ranger_device = FALSE;
    if (soc_feature(unit, soc_feature_l3_256_defip_table)) {
        ranger_device = TRUE;
    }

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE,
                                        ((ranger_device) ? 0 : 1));
    ipv6_128_depth =  SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);

    if (ranger_device ) { 
        if (defip_config) {
            ipv6_128_depth = _BCM_HX4_RANGER_DEFIP_TCAM_DEPTH;
        } else { /* tcams are not paired - no ipv6/65-ipv6/128 routes */
            ipv6_128_depth      = 0;
        }
    }

    BCM_DEFIP_PAIR128_TOTAL(unit) = ipv6_128_depth;
    BCM_DEFIP_PAIR128_URPF_OFFSET(unit) = 0;
    BCM_DEFIP_PAIR128_IDX_MAX(unit) = ipv6_128_depth - 1;
    BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit) = ipv6_128_depth;
    if (ipv6_128_depth) {
        mem_sz = ipv6_128_depth * sizeof(_bcm_defip_pair128_entry_t);
        BCM_DEFIP_PAIR128_ARR(unit) = 
                                sal_alloc(mem_sz, "tr3_l3_defip_pair128_entry_array");
        if (BCM_DEFIP_PAIR128_ARR(unit) == NULL) {
            BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
            return (BCM_E_MEMORY);
        }
        sal_memset(BCM_DEFIP_PAIR128_ARR(unit), 0, mem_sz);
    }

    rv = _bcm_defip_pair128_field_cache_init(unit);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
        return rv;
    }
    
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr3_l3_host_stat_get_table_info
 * Description:
 *      Provides relevant flex table information(table-name,index with
 *      direction)  for given L3 Host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port             - (IN) GPORT ID
 *      num_of_tables    - (OUT) Number of flex counter tables
 *      table_info       - (OUT) Flex counter tables information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC bcm_error_t
_bcm_tr3_l3_host_stat_get_table_info(
                   int                        unit,
                   bcm_l3_host_t              *info,
                   uint32                     *num_of_tables,
                   bcm_stat_flex_table_info_t *table_info)
{
    int rv, embd;
    soc_mem_t mem;          
    _bcm_l3_cfg_t l3cfg;     

    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    if (!(soc_feature(unit, soc_feature_l3_extended_host_entry))) {
        return BCM_E_UNAVAIL;
    }

    mem = INVALIDm;
    /* Vrf is in device supported range check. */
    if ((info->l3a_vrf > SOC_VRF_MAX(unit)) || 
        (info->l3a_vrf < BCM_L3_VRF_DEFAULT)) {
        return (BCM_E_PARAM);
    }
  
    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
    l3cfg.l3c_flags = info->l3a_flags;
    l3cfg.l3c_vrf = info->l3a_vrf;

    if (info->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(l3cfg.l3c_ip6, info->l3a_ip6_addr, BCM_IP6_ADDRLEN);
    } else {
        l3cfg.l3c_ip_addr = info->l3a_ip_addr;
    }
   
    rv = _bcm_tr3_l3_ism_get(unit, &l3cfg, NULL, &embd);
    if (BCM_SUCCESS(rv)) { /* Entry found in ISM */
        mem = (l3cfg.l3c_flags & BCM_L3_IP6) ? L3_ENTRY_4m :
                                               L3_ENTRY_2m;       
    }

    if (BCM_E_NOT_FOUND == rv && BCM_TR3_ESM_HOST_TBL_PRESENT(unit)) {   
        /* Not found in ISM, search ESM */
        rv = _bcm_tr3_l3_esm_get(unit, &l3cfg, NULL, &embd);
        if (BCM_SUCCESS(rv)) { /* Entry found in ESM */
            mem = (l3cfg.l3c_flags & BCM_L3_IP6) ?
                         EXT_IPV6_128_UCAST_WIDEm :
                         EXT_IPV4_UCAST_WIDEm;       
        }
    } 

    if (BCM_SUCCESS(rv)) {
        table_info[*num_of_tables].table = mem;
        table_info[*num_of_tables].index = l3cfg.l3c_hw_index;
        table_info[*num_of_tables].direction = bcmStatFlexDirectionIngress;
        (*num_of_tables)++;
    }
    
    return rv;
}

/*----- END STATIC FUNCS ----- */

/*
 * Function:
 *      _bcm_tr3_l3_add
 * Purpose:
 *      Add and entry to Tr3 ISM/ESM L3 host table.
 *      The decision to add to ISM/ESM is based on 
 *      switch control(s) and ESM/ISM table full conditions.
 *      conditions.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      l3cfg  - (IN) L3 entry info.
 *      nh_idx - (IN) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_add(int unit, _bcm_l3_cfg_t *l3cfg, int nh_idx)
{
    int rv;

    /* Access ISM before ESM */
    rv = _bcm_tr3_l3_ism_add(unit, l3cfg, nh_idx);
    if (BCM_E_FULL == rv && BCM_TR3_ESM_HOST_TBL_PRESENT(unit)) {       
        if (BCM_L3_RPE_SET(l3cfg->l3c_flags)) {
            if (l3cfg->l3c_lookup_class > SOC_ADDR_CLASS_MAX(unit)) {
                return (BCM_E_PARAM);
            }
        } else {
            l3cfg->l3c_prio = (l3cfg->l3c_lookup_class & 0x3C0) >> 6;
            l3cfg->l3c_lookup_class = l3cfg->l3c_lookup_class & 0x3F;
        }

        /* ISM host table full, try to add to ESM */
        return (_bcm_tr3_l3_esm_add(unit, l3cfg, nh_idx));
    } else if (BCM_FAILURE(rv)) { 
        /* ISM host add failed */
        return rv;
    } else {                      
        /* ISM host add successful */
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      _bcm_tr3_l3_get
 * Purpose:
 *      Get an entry from Tr3 ISM/ESM L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_get(int unit, _bcm_l3_cfg_t *l3cfg, int *nh_idx)
{
    int rv;
    int embd = _BCM_TR3_HOST_ENTRY_NOT_FOUND;

    /* Search ISM tables first */
    rv = _bcm_tr3_l3_ism_get(unit, l3cfg, nh_idx, &embd);
    if (BCM_E_NOT_FOUND == rv && BCM_TR3_ESM_HOST_TBL_PRESENT(unit)) {   
        /* Not found in ISM, search ESM */
        BCM_IF_ERROR_RETURN((_bcm_tr3_l3_esm_get(unit, l3cfg, nh_idx, &embd)));
        if (!BCM_L3_RPE_SET(l3cfg->l3c_flags)) {
            l3cfg->l3c_lookup_class = (((l3cfg->l3c_prio & 0xF) << 6) |
                                       (l3cfg->l3c_lookup_class & 0x3F));
        }

        return BCM_E_NONE;
    } else if (BCM_FAILURE(rv)) { 
        /* ISM search failed */
        return rv;
    } else {                      
        /* Found match in ISM */
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      _bcm_tr3_l3_del
 * Purpose:
 *      Del an entry from Tr3 ISM/ESM L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    int rv;

    /* Attempt to delete entry from ISM tables first */
    rv = _bcm_tr3_l3_ism_del(unit, l3cfg);
    if ((BCM_E_NOT_FOUND == rv) && BCM_TR3_ESM_HOST_TBL_PRESENT(unit)) { 
        rv = _bcm_tr3_l3_esm_del(unit, l3cfg);
        if (BCM_E_NOT_FOUND == rv) {
            rv = BCM_E_NONE; /* Ignore if entry to del is not found */
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_traverse
 * Purpose:
 *      Walk over range of entries in l3 host table.
 * Parameters:
 *      unit - SOC unit number.
 *      flags - L3 entry flags to match during traverse.  
 *      start - First index to read. 
 *      end - Last index to read.
 *      cb  - Caller notification callback.
 *      user_data - User cookie, which should be returned in cb. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_traverse(int unit, int flags, uint32 start, uint32 end,
                      bcm_l3_host_traverse_cb cb, void *user_data)
{
    int rv = BCM_E_NONE;
    int ipv6;                   /* Protocol IPv6/IPv4          */
    int nh_idx;                 /* Next hop index.                 */
    int total = 0;              /* Total number of entries walked. */
    int table_size;             /* Number of entries in the table. */
    int table_ent_size;         /* Table entry size. */
    bcm_l3_host_t info;         /* Callback info buffer.           */
    _bcm_l3_cfg_t l3cfg;        /* HW entry info.                  */
    uint8 *l3_tbl_buf = NULL;    /* Table dma pointer.              */
    int tbl, idx, idx_min, idx_max;  /* Min and Max iteration index */
    int chunk_size, num_chunks, chunk_index;
    int num_tables = _BCM_TR3_NUM_ESM_HOST_TABLES/2;
    soc_mem_t mem[_BCM_TR3_NUM_ESM_HOST_TABLES] =
              {INVALIDm, INVALIDm, INVALIDm, INVALIDm};
    int host_table_entry = 0;

    /* If no callback provided, we are done.  */
    if (NULL == cb) {
        return (BCM_E_NONE);
    }

    ipv6 = (flags & BCM_L3_IP6) ? TRUE : FALSE;
    /* If table is empty -> there is nothing to read. */
    if (ipv6 && (!BCM_XGS3_L3_IP6_CNT(unit))) {
        return (BCM_E_NONE);
    }
    if (!ipv6 && (!BCM_XGS3_L3_IP4_CNT(unit))) {
        return (BCM_E_NONE);
    }

    if (ipv6) {
        mem[0] = BCM_XGS3_L3_MEM(unit, v6); 
        if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
            mem[1] = BCM_XGS3_L3_MEM(unit, v6_4); 
            num_tables++;
        }
    } else {
        mem[0] = BCM_XGS3_L3_MEM(unit, v4); 
        if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
            mem[1] = BCM_XGS3_L3_MEM(unit, v4_2); 
            num_tables++;
        }
    }

    for(tbl = 0; tbl < num_tables; tbl++) {
  
        if (INVALIDm == mem[tbl]) {
            continue;
        }
        if (soc_mem_index_count(unit, mem[tbl]) == 0) {
            continue;
        }

        chunk_size = 1024;
        table_size = soc_mem_index_count(unit, mem[tbl]);
        table_ent_size = BCM_L3_MEM_ENT_SIZE(unit, mem[tbl]);
        num_chunks = soc_mem_index_count(unit, mem[tbl]) / chunk_size;
        if (soc_mem_index_count(unit, mem[tbl]) % chunk_size) {
            num_chunks++;
        }

        l3_tbl_buf = soc_cm_salloc(unit, table_ent_size * chunk_size,
                                           "l3_tbl_chunk buffer");
        if (NULL == l3_tbl_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        
        for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {
            idx_min = chunk_index * chunk_size;
            idx_max = idx_min + chunk_size - 1;
            if (idx_max > soc_mem_index_max(unit, mem[tbl])) {
                idx_max = soc_mem_index_max(unit, mem[tbl]);
            }

            /* Reset allocated buffer. */
            sal_memset(l3_tbl_buf, 0, (table_ent_size * chunk_size));

            rv = soc_mem_read_range(unit, mem[tbl], MEM_BLOCK_ANY,
                    idx_min, idx_max, l3_tbl_buf);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            /* Input index`s sanity. */
            if (start > (uint32)table_size || start > end) {
                if(l3_tbl_buf) {
                    soc_cm_sfree(unit, l3_tbl_buf);
                    l3_tbl_buf = NULL;
                }
                return (BCM_E_NOT_FOUND);
            }

            /* Iterate over all the entries - show matching ones. */
            for (idx = idx_min; idx <= idx_max; idx++) {
                /* Reset buffer before read. */
                sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
                l3cfg.l3c_flags = flags;

                /* Get entry from  hw. */
                rv = _bcm_tr3_l3_get_host_ent_by_idx(unit, l3_tbl_buf, mem[tbl],
                                           (idx % chunk_size), &l3cfg, &nh_idx);

                /* Check for read errors & invalid entries. */
                if (rv < 0) {
                    if (rv != BCM_E_NOT_FOUND) {
                        break;
                    }
                    continue;
                }
    
                /* Check read entry flags & update index accordingly. */
                if (BCM_L3_CMP_EQUAL !=
                    _bcm_xgs3_trvrs_flags_cmp(unit, flags, l3cfg.l3c_flags, &idx)) {
                    continue;
                }
    
                /* Valid entry found -> increment total table_sizeer */
                total++;
                if ((uint32)total < start) {
                    continue;
                }
    
                /* Don't read beyond last required index. */
                if ((uint32)total > end) {
                    break;
                }
    
                /* Get next hop info. */
                rv = _bcm_xgs3_l3_get_nh_info(unit, &l3cfg, nh_idx);
                if (rv < 0) {
                    continue;
                }
    
                /* Fill host info. */
                _bcm_xgs3_host_ent_init(unit, &l3cfg, TRUE, &info);
                rv = (*cb) (unit, total, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                    break;
                }
#endif
            }
        }
        if(l3_tbl_buf) {
            soc_cm_sfree(unit, l3_tbl_buf);
            l3_tbl_buf = NULL;
        }
    }

    if (BCM_TR3_ESM_HOST_TBL_PRESENT(unit)) {
        idx = 0;
        num_tables =  1;

        if (ipv6) {
            mem[0] = BCM_XGS3_L3_MEM(unit, v6_esm); 
            if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
                mem[1] = BCM_XGS3_L3_MEM(unit, v6_esm_wide); 
                num_tables++;
            }
        } else {
            mem[0] = BCM_XGS3_L3_MEM(unit, v4_esm); 
            if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
                mem[1] = BCM_XGS3_L3_MEM(unit, v4_esm_wide); 
                num_tables++;
            }
        }

        for(tbl = 0; tbl < num_tables; tbl++) {
      
            if (INVALIDm == mem[tbl]) {
                continue;
            }
            if (soc_mem_index_count(unit, mem[tbl]) == 0) {
                continue;
            }

            table_size = soc_mem_index_count(unit, mem[tbl]);
            table_ent_size = BCM_L3_MEM_ENT_SIZE(unit, mem[tbl]);
            chunk_size = 1024;
            num_chunks = soc_mem_index_count(unit, mem[tbl]) / chunk_size;
            if (soc_mem_index_count(unit, mem[tbl]) % chunk_size) {
                num_chunks++;
            }

            l3_tbl_buf = soc_cm_salloc(unit, table_ent_size * chunk_size,
                                               "l3esm_tbl_chunk buffer");
            if (NULL == l3_tbl_buf) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            host_table_entry = _TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl+(2 * ipv6));
        
            for (chunk_index = num_chunks - 1; chunk_index >= 0; chunk_index--) {
                idx_min = chunk_index * chunk_size;
                idx_max = idx_min + chunk_size - 1;
                if (idx_max > soc_mem_index_max(unit, mem[tbl])) {
                    idx_max = soc_mem_index_max(unit, mem[tbl]);
                }

                if (idx_min > host_table_entry) {
                    continue;
                }

                /* Reset allocated buffer. */
                sal_memset(l3_tbl_buf, 0, (table_ent_size * chunk_size));

                rv = soc_mem_read_range(unit, mem[tbl], MEM_BLOCK_ANY,
                        idx_min, idx_max, l3_tbl_buf);
                if (SOC_FAILURE(rv)) {
                    goto cleanup;
                }

                /* Input index`s sanity. */
                if (start > (uint32)table_size || start > end) {
                    if(l3_tbl_buf) {
                        soc_cm_sfree(unit, l3_tbl_buf);
                        l3_tbl_buf = NULL;
                    }
                    return (BCM_E_NOT_FOUND);
                }

                /* Iterate over all the entries - show matching ones. */
                for (idx = idx_min; idx <= idx_max; idx++) {
                    if (idx < host_table_entry) {
                        /* Reset buffer before read. */
                        sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
                        l3cfg.l3c_flags = flags;
        
                        /* Get entry from  hw. */
                        rv = _bcm_tr3_l3_get_host_ent_by_idx(unit, l3_tbl_buf,
                                 mem[tbl], (idx % chunk_size), &l3cfg, &nh_idx);
    
                        /* Check for read errors & invalid entries. */
                        if (rv < 0) {
                            if (rv != BCM_E_NOT_FOUND) {
                                break;
                            }
                            continue;
                        }
        
                        /* Check read entry flags & update index accordingly. */
                        if (BCM_L3_CMP_EQUAL !=
                            _bcm_xgs3_trvrs_flags_cmp(unit, flags,
                                          l3cfg.l3c_flags, &idx)) {
                            continue;
                        }
        
                        /* Valid entry found -> increment total table_sizeer */
                        total++;
                        if ((uint32)total < start) {
                            continue;
                        }
        
                        /* Don't read beyond last required index. */
                        if ((uint32)total > end) {
                            break;
                        }
        
                        /* Get next hop info. */
                        rv = _bcm_xgs3_l3_get_nh_info(unit, &l3cfg, nh_idx);
                        if (rv < 0) {
                            break;
                        }
    
                        /* Fill host info. */
                        _bcm_xgs3_host_ent_init(unit, &l3cfg, TRUE, &info);
                        rv = (*cb) (unit, total, &info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                           break;
                        }
#endif
                    }
                }
            }
            if(l3_tbl_buf) {
                soc_cm_sfree(unit, l3_tbl_buf);
                l3_tbl_buf = NULL;
            }
        }
    }

    /* Reset last read status. */
    if (BCM_E_NOT_FOUND == rv) {
        rv = BCM_E_NONE;
    }

cleanup:
    if (l3_tbl_buf) {
        soc_cm_sfree(unit, l3_tbl_buf);
    }

    return (rv);

}

/*
 * Function:
 *      bcm_tr3_l3_replace
 * Purpose:
 *      Replace an entry in TR3 L3 host table.
 * Parameters:
 *      unit -  (IN)SOC unit number.
 *      l3cfg - (IN)L3 entry information.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l3_replace(int unit, _bcm_l3_cfg_t *l3cfg)
{
    
    _bcm_l3_cfg_t entry;        /* Original l3 entry.             */
    int nh_idx_old;             /* Original entry next hop index. */
    int nh_idx_new;             /* New allocated next hop index   */
    int rv = BCM_E_UNAVAIL;     /* Operation return status.       */
    int esm_old, embd_old, embd_new;

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if (NULL == l3cfg) {
        return (BCM_E_PARAM); 
    }

    /* Set lookup key. */
    entry = *l3cfg;

    esm_old  = _BCM_TR3_HOST_ENTRY_NOT_FOUND;
    embd_old = _BCM_TR3_HOST_ENTRY_NOT_FOUND;

    if (BCM_XGS3_L3_MCAST_ENTRY(l3cfg)) {
        /* Check if identical entry exits. */
        if (BCM_XGS3_L3_HWCALL_CHECK(unit, ipmc_get)) {
            BCM_XGS3_L3_MODULE_LOCK(unit);
            rv = BCM_XGS3_L3_HWCALL_EXEC(unit, ipmc_get) (unit, &entry);
            BCM_XGS3_L3_MODULE_UNLOCK(unit);
        }
        if (rv < 0) {
            return rv;
        }

        /* Write mcast entry to hw. */
        l3cfg->l3c_hw_index = entry.l3c_hw_index;       /*Replacement index & flag. */
        if (BCM_XGS3_L3_HWCALL_CHECK(unit, ipmc_add)) {
            BCM_XGS3_L3_MODULE_LOCK(unit);
            rv = BCM_XGS3_L3_HWCALL_EXEC(unit, ipmc_add) (unit, l3cfg);
            BCM_XGS3_L3_MODULE_UNLOCK(unit);
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else {
        if (!(BCM_XGS3_L3_EGRESS_MODE_ISSET(unit))) {
            /* Trunk id validation. */
            BCM_XGS3_L3_IF_INVALID_TGID_RETURN(unit, l3cfg->l3c_flags,
                                               l3cfg->l3c_port_tgid);
        }

        /* Check if identical entry exits. */
        rv = _bcm_tr3_l3_get_tbl_info(unit, &entry, &nh_idx_old, &esm_old, &embd_old);

        if ((BCM_E_NOT_FOUND == rv) || (BCM_E_DISABLED == rv)) {
            return  bcm_xgs3_host_as_route(unit, l3cfg, BCM_XGS3_L3_OP_ADD, rv);
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }
        l3cfg->l3c_bfd_enable = entry.l3c_bfd_enable;
         
        if (BCM_GPORT_IS_BLACK_HOLE(l3cfg->l3c_port_tgid)) {
             nh_idx_new = 0;
        } else { /* If new entry is in indexed NH table */
            /* Get next hop index. */
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_nh_init_add(unit, l3cfg, NULL, &nh_idx_new));
        }

        embd_new = BCM_TR3_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg->l3c_intf, nh_idx_new);
        if (embd_old != embd_new) {
            rv = bcm_xgs3_l3_del(unit, &entry);
            l3cfg->l3c_hw_index = BCM_XGS3_L3_INVALID_INDEX;
        } else {
            /*Replacement index & flag. */
            l3cfg->l3c_hw_index = entry.l3c_hw_index; 
        }        
        if (BCM_XGS3_L3_HWCALL_CHECK(unit, l3_add)) {
            BCM_XGS3_L3_MODULE_LOCK(unit);
            rv = BCM_XGS3_L3_HWCALL_EXEC(unit, l3_add)(unit, l3cfg, nh_idx_new);
            BCM_XGS3_L3_MODULE_UNLOCK(unit);
        } else {
            rv = BCM_E_UNAVAIL;
        }

        /* Do best effort clean_up if needed. */
        if (BCM_FAILURE(rv)) {
            bcm_xgs3_nh_del(unit, 0, nh_idx_new);
        }

        /* Free original next hop index. */
        if (embd_old == embd_new) {
            BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_idx_old));
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l3_del_intf
 * Purpose:
 *      Delete all the entries in Tr3 L3 host table with
 *      NH matching a certain interface.
 * Parameters:
 *      unit   - (IN)SOC unit number.
 *      l3cfg  - (IN)Pointer to memory for l3 table related information.
 *      negate - (IN)0 means interface match; 1 means not match
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l3_del_intf(int unit, _bcm_l3_cfg_t *l3cfg, int negate)
{
    int rv;                             /* Operation return status.      */
    int tmp_rv;                         /* First error occured.          */
    bcm_if_t intf;                      /* Deleted interface id.         */
    int nh_index;                       /* Next hop index.               */
    bcm_l3_egress_t egr;                /* Egress object.                */
    _bcm_if_del_pattern_t pattern;      /* Interface & negate combination 
                                           to be used for every l3 entry
                                           comparision.                  */

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check */
    if (NULL == l3cfg) {
        return (BCM_E_PARAM);
    }

    intf = l3cfg->l3c_intf;

    /* Check l3 switching mode. */ 
    if (BCM_XGS3_L3_EGRESS_MODE_ISSET(unit)) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf) || 
            BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
                nh_index = intf - BCM_XGS3_EGRESS_IDX_MIN(unit); 
            } else {
                nh_index = intf - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            }
            /* Get egress object information. */
            BCM_IF_ERROR_RETURN(bcm_xgs3_nh_get(unit, nh_index, &egr));
            /* Use egress object interface for iteration */
            intf = egr.intf;
        }
    }

    pattern.l3_intf = intf;
    pattern.negate = negate;

    /* Delete all ipv4 entries matching the interface. */
    tmp_rv = _bcm_tr3_l3_del_match(unit, 0, (void *)&pattern,
                                    _bcm_xgs3_l3_intf_cmp, NULL, NULL);

    /* Delete all ipv6 entries matching the interface. */
    rv = _bcm_tr3_l3_del_match(unit, BCM_L3_IP6, (void *)&pattern,
                                _bcm_xgs3_l3_intf_cmp, NULL, NULL);

    return (tmp_rv < 0) ? tmp_rv : rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_get_by_idx
 * Purpose:
 *      Get an entry from L3 table by index.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      dma_ptr  - (IN)Table pointer in dma. 
 *      idx      - (IN)Index to read.
 *      l3cfg    - (OUT)l3 entry search result.
 *      nh_index - (OUT)Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_get_by_idx(int unit, void *dma_ptr, int idx,
                        _bcm_l3_cfg_t *l3cfg, int *nh_idx)
{
    int v6;
    int embeded_nh;
    soc_mem_t mem;
    int rv = BCM_E_UNAVAIL;     /* Operation return code. */

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    if (l3cfg->l3c_flags & BCM_L3_IPMC) {
        /* Get multicast entry. */
        rv = _bcm_tr3_l3_ipmc_get_by_idx(unit, NULL, idx, l3cfg);
    } else {
        /* Get unicast entry. */
        v6 = (l3cfg->l3c_flags & BCM_L3_IP6); 
        embeded_nh = (l3cfg->l3c_flags & BCM_L3_DEREFERENCED_NEXTHOP); 
        mem = (v6) ? 
              ((embeded_nh) ? 
              BCM_XGS3_L3_MEM(unit, v6_4) : BCM_XGS3_L3_MEM(unit, v6)) :
              ((embeded_nh) ?  
              BCM_XGS3_L3_MEM(unit, v4_2) : BCM_XGS3_L3_MEM(unit, v4)); 
        rv = _bcm_tr3_l3_get_host_ent_by_idx(unit, NULL, mem, idx, l3cfg, nh_idx);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_del_match
 * Purpose:
 *      Delete an entry in L3 table matches a certain rule.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      flags.    - (IN)Generic l3 flags. 
 *      pattern   - (IN)Comparison match argurment.
 *      cmp_func  - (IN)Comparison function.
 *      notify_cb - (IN)Delete notification callback. 
 *      user_data - (IN)User provided cookie for callback.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_del_match(int unit, int flags, void *pattern,
                       bcm_xgs3_ent_op_cb cmp_func,
                       bcm_l3_host_traverse_cb notify_cb, void *user_data)
{
    int rv;
    int ipv6;             /* IPv6/IPv4 lookup flag. */
    int nh_idx;           /* Next hop index.        */
    int tbl, idx;         /* Iteration indices.     */
    int cmp_result;       /* Compare against pattern result. */
    bcm_l3_host_t info;   /* Host cb buffer.        */
    _bcm_l3_cfg_t l3cfg;  /* Hw entry info.         */
    int idx_max;
    int num_tables = 1;
    int table_ent_size;
    int table_size;
    char *l3_tbl_ptr = NULL;

    soc_mem_t mem[_BCM_TR3_NUM_ESM_HOST_TABLES] =
              {INVALIDm, INVALIDm, INVALIDm, INVALIDm};

    /* Check Protocol. */
    ipv6 = (flags & BCM_L3_IP6) ? TRUE : FALSE;

    if (ipv6) {
        if (BCM_XGS3_L3_IP6_CNT(unit) == 0) {
            /* No Ipv6 entries. Nothing to delete */
            return BCM_E_NONE;
        }
        mem[0] = BCM_XGS3_L3_MEM(unit, v6); 
        if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
            mem[1] = BCM_XGS3_L3_MEM(unit, v6_4); 
            num_tables++;
        }
    } else {
        if (BCM_XGS3_L3_IP4_CNT(unit) == 0) {
            /* No Ipv4 entries. Nothing to delete */
            return BCM_E_NONE;
        }
        mem[0] = BCM_XGS3_L3_MEM(unit, v4); 
        if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
            mem[1] = BCM_XGS3_L3_MEM(unit, v4_2); 
            num_tables++;
        }
    }

    for (tbl = 0; tbl < num_tables; tbl++) {
        if (INVALIDm == mem[tbl]) {
            continue;
        }
        idx_max = soc_mem_index_max(unit, mem[tbl]);
        table_ent_size = BCM_L3_MEM_ENT_SIZE(unit, mem[tbl]);
        BCM_IF_ERROR_RETURN(
                bcm_xgs3_l3_tbl_dma(unit, mem[tbl], table_ent_size,
                    "l3_tbl", &l3_tbl_ptr, &table_size));
        
        for (idx = 0; idx <= idx_max; idx++) {

            /* Set protocol. */
            l3cfg.l3c_flags = flags;

            rv = _bcm_tr3_l3_get_host_ent_by_idx(unit, l3_tbl_ptr, mem[tbl], idx,
                                                      &l3cfg, &nh_idx);
            /* Check for read errors & invalid entries. */
            if (rv < 0) {
                if (rv != BCM_E_NOT_FOUND) {
                    if (l3_tbl_ptr) {
                        soc_cm_sfree(unit, l3_tbl_ptr);
                    }
                    return rv;
                }
                continue;
            }

            /* Check read entry flags & update index accordingly. */
            if (BCM_L3_CMP_EQUAL !=
                _bcm_xgs3_trvrs_flags_cmp(unit, flags, l3cfg.l3c_flags, &idx)) {
                continue;
            }

            /* If compare function provided match the entry against pattern */
            if (cmp_func) {
                rv = (*cmp_func) (unit, pattern, (void *)&l3cfg,
                                                 (void *)&nh_idx, &cmp_result);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, l3_tbl_ptr);
                    return rv;
                }
                /* If entry doesn't match the pattern don't delete it. */
                if (BCM_L3_CMP_EQUAL != cmp_result) {
                    continue;
                }
            }

            /* Entry matches the rule -> delete it. */
            rv = _bcm_tr3_l3_ism_del(unit, &l3cfg);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, l3_tbl_ptr);
                return rv;
            }

            /* Release next hop index. */
            rv = bcm_xgs3_nh_del(unit, 0, nh_idx);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, l3_tbl_ptr);
                return rv;
            }
   
            /* Check if notification required. */
            if (notify_cb) {
                /* Fill host info. */
                _bcm_xgs3_host_ent_init(unit, &l3cfg, FALSE, &info);
                /* Call notification callback. */
                (*notify_cb) (unit, idx, &info, user_data);
            }
        }

        if (l3_tbl_ptr) {
            soc_cm_sfree(unit, l3_tbl_ptr);
            l3_tbl_ptr = NULL;
        }
    }

    if (BCM_TR3_ESM_HOST_TBL_PRESENT(unit)) {
        idx = 0;
        num_tables =  1;
        if (ipv6) {
            mem[0] = BCM_XGS3_L3_MEM(unit, v6_esm); 
            if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
                mem[1] = BCM_XGS3_L3_MEM(unit, v6_esm_wide); 
                num_tables++;
            }
        } else {
            mem[0] = BCM_XGS3_L3_MEM(unit, v4_esm); 
            if (soc_feature(unit, soc_feature_l3_extended_host_entry)) {
                mem[1] = BCM_XGS3_L3_MEM(unit, v4_esm_wide); 
                num_tables++;
            }
        }

        for (tbl = 0; tbl < num_tables; tbl++) {
            idx = 0; /* reset idx for each table */
            if (INVALIDm == mem[tbl]) {
                continue;
            }

            while (idx < _TR3_ESM_HOST_TBL_FREE_ENTRY(unit, (tbl + (2 * ipv6)))) {

                /* Set protocol. */
                l3cfg.l3c_flags = flags;

                rv = _bcm_tr3_l3_get_host_ent_by_idx(unit, NULL, mem[tbl], idx,
                                                          &l3cfg, &nh_idx);
                /* Check for read errors & invalid entries. */
                if (rv < 0) {
                    if (rv != BCM_E_NOT_FOUND) {
                        return rv;
                    }
                    idx++;
                    continue;
                }
                /* Check read entry flags & update index accordingly. */
                if (BCM_L3_CMP_EQUAL !=
                    _bcm_xgs3_trvrs_flags_cmp(unit, flags, l3cfg.l3c_flags, &idx)) {
                    idx++;
                    continue;
                }

                /* If compare function provided match the entry against pattern */
                if (cmp_func) {
                    BCM_IF_ERROR_RETURN((*cmp_func) (unit, pattern, (void *)&l3cfg,
                                                     (void *)&nh_idx, &cmp_result));
                    /* If entry doesn't match the pattern don't delete it. */
                    if (BCM_L3_CMP_EQUAL != cmp_result) {
                        idx++;
                        continue;
                    }
                }

                /* Entry matches the rule -> delete it. */
                BCM_IF_ERROR_RETURN(_bcm_tr3_l3_esm_del(unit, &l3cfg));

                /* Release next hop index. */
                BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_idx));
   
                /* Check if notification required. */
                if (notify_cb) {
                    /* Fill host info. */
                    _bcm_xgs3_host_ent_init(unit, &l3cfg, FALSE, &info);
                    /* Call notification callback. */
                    (*notify_cb) (unit, idx, &info, user_data);
                }
            }
        } /* per esm table */
    } /* esm */

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr3_esm_host_tbl_init
 * Purpose:
 *      Initialize TR3 external host table sw state. 
 * Parameters:
 *      unit        - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_esm_host_tbl_init(int unit)
{
    int tbl;
    int v6 = 0;
    uint32 *esm_entry;
    
    soc_mem_t mem[_BCM_TR3_NUM_ESM_HOST_TABLES];
    ext_ipv4_ucast_entry_t          l3v4_entry;
    ext_ipv4_ucast_wide_entry_t     l3v4wide_entry;
    ext_ipv6_128_ucast_entry_t      l3v6_entry;
    ext_ipv6_128_ucast_wide_entry_t l3v6wide_entry;

    mem[0] = EXT_L3_UCAST_DATA_IPV4m;
    mem[1] = EXT_L3_UCAST_DATA_WIDE_IPV4m;
    mem[2] = EXT_L3_UCAST_DATA_IPV6_128m;
    mem[3] = EXT_L3_UCAST_DATA_WIDE_IPV6_128m;

    for (tbl = 0; tbl < _BCM_TR3_NUM_ESM_HOST_TABLES; tbl++) {
        if (_TR3_ESM_HOST_TBL_STATE(unit, tbl) != NULL) {
            sal_free(_TR3_ESM_HOST_TBL_STATE(unit, tbl));
            _TR3_ESM_HOST_TBL_STATE(unit, tbl) = NULL;
        }
        
        _TR3_ESM_HOST_TBL_STATE(unit, tbl) = (int*)
                         sal_alloc(sizeof (int), "ESM host free idx info");

        if (NULL == _TR3_ESM_HOST_TBL_STATE(unit, tbl)) {
            return (BCM_E_MEMORY);
        }

        if (soc_mem_index_count(unit, mem[tbl])) {
            /* ESM table exists and its size is configured */
            if ((mem[tbl] == EXT_L3_UCAST_DATA_IPV6_128m) ||
                (mem[tbl] == EXT_L3_UCAST_DATA_WIDE_IPV6_128m)) {
                v6 = 1;
            }

            BCM_TR3_ESM_L3_HOST_ENTRY_BUF(v6, mem[tbl], esm_entry,
                                          l3v4_entry,
                                          l3v4wide_entry,
                                          l3v6_entry,
                                          l3v6wide_entry);
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem[tbl],
                                MEM_BLOCK_ANY, 0, esm_entry));

            soc_mem_field32_set(unit, mem[tbl], esm_entry, CLASS_IDf,
                                  _TR3_ESM_HOST_ENTRY_INVALID_MARKER);
    
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem[tbl],
                                MEM_BLOCK_ALL, 0, esm_entry));
        }
        _TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl) = 0;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr3_esm_host_tbl_deinit
 * Purpose:
 *      Free the TR3 external host table sw state. 
 * Parameters:
 *      unit        - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_esm_host_tbl_deinit(int unit)
{
    int tbl;

    for (tbl = 0; tbl < _BCM_TR3_NUM_ESM_HOST_TABLES; tbl++) {
        if (_TR3_ESM_HOST_TBL_STATE(unit, tbl) != NULL) {
            sal_free(_TR3_ESM_HOST_TBL_STATE(unit, tbl));
            _TR3_ESM_HOST_TBL_STATE(unit, tbl) = NULL;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_hx4_l3_defip_init
 * Purpose:
 *      Configure HX4 internal route table as per soc properties.
 *      (tcam pairing for IPv4, 64/128 bit IPV6 prefixes)
 * Parameters:
 *      unit     - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_hx4_l3_defip_init(int unit)
{
    int defip_config;
    int ipv6_64_depth = 0;
    uint32 defip_key_sel_0 = 0;
    uint32 defip_key_sel_1 = 0;
    int mem_v4, mem_v6, mem_v6_128; /* Route table memories */
    int ranger_device;              /* Ranger specific config */
    int num_ipv6_128b_entries;
    int tcam_pair_count = 0;
    int tcam_idx = 0;
    int start_index = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    ranger_device = FALSE;
    if (soc_feature(unit, soc_feature_l3_256_defip_table)) {
        ranger_device = TRUE;
    }

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE,
                                        ((ranger_device) ? 0 : 1));
    num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);
    BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit) = num_ipv6_128b_entries;


    if (num_ipv6_128b_entries) {
        tcam_pair_count = (num_ipv6_128b_entries / tcam_depth) +
                          ((num_ipv6_128b_entries % tcam_depth) ? 1 : 0);
    } else {
        tcam_pair_count = 0;
    }

    if (tcam_pair_count > (SOC_CONTROL(unit)->l3_defip_max_tcams / 2)) {
        tcam_pair_count = (SOC_CONTROL(unit)->l3_defip_max_tcams / 2);
    }

    if (!defip_config) {
        if (ranger_device) {
            ipv6_64_depth = _BCM_HX4_RANGER_DEFIP_TCAM_DEPTH;
        } else {
            ipv6_64_depth = SOC_CONTROL(unit)->l3_defip_max_tcams * tcam_depth; 
        }
    } else {
        for (tcam_idx = 0; tcam_idx < tcam_pair_count; tcam_idx++) {
            defip_key_sel_0 |= (0x01 << tcam_idx);
        }

        ipv6_64_depth =  soc_mem_index_count(unit, L3_DEFIPm);
        if (ranger_device) {
            defip_key_sel_0 = 0x3;
            ipv6_64_depth = 0;
        }
    }

    /* LPM config regs. */
    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SEL_0r(unit, defip_key_sel_0));
    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SEL_1r(unit, defip_key_sel_1));

    /* Get memory for IPv4 entries. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_defip_mem_get(unit, 0, 0, &mem_v4));

    /* Initialize IPv4 entries lookup engine. */
    BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));

    /* Get memory for IPv6 entries. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_l3_defip_mem_get(unit, BCM_L3_IP6, 0, &mem_v6));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v4 != mem_v6) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));
    }

    /* Overriding defaults set during lpm init */
    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm128_init(unit));
        ipv6_64_depth -= (tcam_pair_count * tcam_depth * 2);
        start_index = (tcam_pair_count * tcam_depth * 2);
    } else {
        start_index = 0;
    }

    SOC_LPM_STATE_FENT(unit, (MAX_PFX_INDEX(unit)))  = ipv6_64_depth;
    SOC_LPM_STATE_START(unit, (MAX_PFX_INDEX(unit))) = start_index;
    SOC_LPM_STATE_END(unit, (MAX_PFX_INDEX(unit)))   = start_index - 1;

    BCM_IF_ERROR_RETURN(soc_fb_lpm_state_config(unit, ipv6_64_depth, 
                start_index));

    /* Get memory for IPv6 entries with prefix length > 64. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_l3_defip_mem_get(unit, BCM_L3_IP6,
                                  BCM_XGS3_L3_IPV6_PREFIX_LEN,
                                  &mem_v6_128));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v6 != mem_v6_128) {
        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
            return soc_fb_lpm128_init(unit);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_tr3_defip_pair128_init(unit));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_defip_init
 * Purpose:
 *      Configure TR3 internal route table as per soc properties.
 *      (tcam pairing for IPv4, 64/128 bit IPV6 prefixes)
 * Parameters:
 *      unit     - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_defip_init(int unit)
{
    int defip_config;
    int index_min, index_max;
    uint32 defip_key_sel_val = 0;
    int mem_v4, mem_v6, mem_v6_128; /* Route table memories */
    int ipv6_64_depth = 0;
    int rangerplus_device;          /* Ranger plus specific config */
    int num_ipv6_128b_entries;
    int start_index = 0;
    int tcam_pair_count = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    if (soc_feature(unit, soc_feature_l3_expanded_defip_table)) {
        return _bcm_hx4_l3_defip_init(unit);/* HX4, Ranger */
    }

    rangerplus_device = FALSE;
    if (soc_feature(unit, soc_feature_l3_256_defip_table)) {
        rangerplus_device = TRUE;
    }

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE,
                                       (rangerplus_device) ? 0 : 1);
    num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);
    BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit) = num_ipv6_128b_entries;


    if (num_ipv6_128b_entries) {
        tcam_pair_count = (num_ipv6_128b_entries / tcam_depth) +
                          ((num_ipv6_128b_entries % tcam_depth) ? 1 : 0);
    } else {
        tcam_pair_count = 0;
    }

    if ((rangerplus_device == TRUE) || 
        soc_feature(unit, soc_feature_l3_reduced_defip_table)) {
        if (!defip_config) {
            if (soc_feature(unit, soc_feature_l3_reduced_defip_table)) {
                ipv6_64_depth = 6 * _BCM_TR3_DEFIP_TCAM_DEPTH;
            } else {
                ipv6_64_depth = 8 * _BCM_TR3_DEFIP_TCAM_DEPTH;
                if (rangerplus_device) {
                    ipv6_64_depth = _BCM_HX4_RANGER_DEFIP_TCAM_DEPTH;
                }
            }
        } else if (soc_feature(unit, soc_feature_l3_reduced_defip_table)) {
            switch (tcam_pair_count) {
                case 1:
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr,
                                      &defip_key_sel_val, V6_KEY_SEL_CAM0_1f,
                                      0x1);
                break;

                case 2:
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr,
                                      &defip_key_sel_val, V6_KEY_SEL_CAM0_1f,
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr,
                                      &defip_key_sel_val, V6_KEY_SEL_CAM2_3f,
                                      0x1);
                break;

                case 3:
                default:
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr,
                                      &defip_key_sel_val, V6_KEY_SEL_CAM0_1f,
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr,
                                      &defip_key_sel_val, V6_KEY_SEL_CAM2_3f,
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr,
                                      &defip_key_sel_val, V6_KEY_SEL_CAM4_5f,
                                      0x1);
                break;

            }
            ipv6_64_depth = soc_mem_index_count(unit, L3_DEFIPm);
        } else {

            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                               V6_KEY_SEL_CAM0_1f, 0x1);
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                               V6_KEY_SEL_CAM2_3f, 0x1);

            ipv6_64_depth = 4 * _BCM_TR3_DEFIP_TCAM_DEPTH;
            if (rangerplus_device) {
                ipv6_64_depth = 0;
            }
        }
    } else { /* Tr3 case */
        switch (tcam_pair_count) {
            case 1:
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                                  V6_KEY_SEL_CAM0_1f, 0x1);
                break;

            case 2:
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                                  V6_KEY_SEL_CAM0_1f, 0x1);
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val,
                                  V6_KEY_SEL_CAM2_3f, 0x1);
                break;

            case 3:
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                  &defip_key_sel_val, V6_KEY_SEL_CAM0_1f, 
                                  0x1);
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                  &defip_key_sel_val, V6_KEY_SEL_CAM2_3f, 
                                  0x1);
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                  &defip_key_sel_val, V6_KEY_SEL_CAM4_5f, 
                                  0x1);
                break;

            case 4:
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                  &defip_key_sel_val, V6_KEY_SEL_CAM0_1f, 
                                  0x1);
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                  &defip_key_sel_val, V6_KEY_SEL_CAM2_3f, 
                                  0x1);
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                  &defip_key_sel_val, V6_KEY_SEL_CAM4_5f, 
                                  0x1);
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                  &defip_key_sel_val, V6_KEY_SEL_CAM6_7f, 
                                  0x1);
                break;

            default:
                break;
        }
        ipv6_64_depth =  soc_mem_index_count(unit, L3_DEFIPm); 
    }

    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(unit, defip_key_sel_val));

    /* Get memory for IPv4 entries. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_defip_mem_get(unit, 0, 0, &mem_v4));

    /* Initialize IPv4 entries lookup engine. */
    if (BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV4_DEFIPm)) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_ext_lpm_init(unit, mem_v4)); 
    } else {
        BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));
    }

    /* Get memory for IPv6 entries. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_l3_defip_mem_get(unit, BCM_L3_IP6, 0, &mem_v6));

    /* Initialize IPv6 entries lookup engine. */
    if (BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV6_64_DEFIPm) ||
        BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV6_128_DEFIPm)) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_ext_lpm_init(unit, mem_v6));
    } else {
        if (mem_v4 != mem_v6) {
            BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));
        }
        /* Overriding defaults set during lpm init */
        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
            BCM_IF_ERROR_RETURN(soc_fb_lpm128_init(unit));
            ipv6_64_depth -= (tcam_pair_count * tcam_depth * 2);
            start_index = (tcam_pair_count * tcam_depth * 2);
        } else {
            start_index = 0;
        }

        SOC_LPM_STATE_START(unit, (MAX_PFX_INDEX(unit))) = start_index;
        SOC_LPM_STATE_END(unit, (MAX_PFX_INDEX(unit))) = start_index - 1;
        SOC_LPM_STATE_FENT(unit, (MAX_PFX_INDEX(unit))) = ipv6_64_depth;
        BCM_IF_ERROR_RETURN(soc_fb_lpm_state_config(unit, ipv6_64_depth, 
                                                    start_index));
    }

    /* Get memory for IPv6 entries with prefix length > 64. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_l3_defip_mem_get(unit, BCM_L3_IP6,
                                  BCM_XGS3_L3_IPV6_PREFIX_LEN,
                                  &mem_v6_128));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v6 != mem_v6_128) {
        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
            return soc_fb_lpm128_init(unit);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_tr3_defip_pair128_init(unit));
        }
    }

    /* Update defip size in l3 book keeping if ESM present */
    if (soc_feature(unit, soc_feature_esm_support) && (L3_DEFIPm != mem_v4)) {
        index_min = soc_mem_index_min(unit, mem_v4);
        index_max = soc_mem_index_max(unit, mem_v4);
        BCM_XGS3_L3_DEFIP_TBL_SIZE(unit) = index_max - index_min + 1;
    }
    if (!SOC_LPM_STAT_INIT_CHECK(unit)) {
        if (soc_feature(unit, soc_feature_l3_shared_defip_table)) {
            if (soc_fb_lpm_stat_init(unit) < 0) {
       /* COVERITY
        * soc_fb_lpm_deinit is called in error condition.
        * so irrespective of its return value we are going to release lpm lock
        * and return E_INTERNAL. Hence we can ignore the return value
        */
        /* coverity[check_return] */
                soc_fb_lpm_deinit(unit);
                return SOC_E_INTERNAL;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_defip_deinit
 * Purpose:
 *      De-initialize route table for triumph3 devices.
 * Parameters:
 *      unit - (IN)  SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_tr3_l3_defip_deinit(int unit)
{
    soc_mem_t mem_v4;      /* IPv4 Route table memory.             */
    soc_mem_t mem_v6;      /* IPv6 Route table memory.             */
    soc_mem_t mem_v6_128;  /* IPv6 full prefix route table memory. */

    /* Get memory for IPv4 entries. */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_defip_mem_get(unit, 0, 0, &mem_v4));

    /* Initialize IPv4 entries lookup engine. */
    if (BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV4_DEFIPm)) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_ext_lpm_deinit(unit, mem_v4));
    } else {
        BCM_IF_ERROR_RETURN(soc_fb_lpm_deinit(unit));
    }

    /* Get memory for IPv6 entries. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_l3_defip_mem_get(unit, BCM_L3_IP6, 0, &mem_v6));

    /* Initialize IPv6 entries lookup engine. */
    if (BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV6_64_DEFIPm) ||
        BCM_TR3_ESM_LPM_TBL_PRESENT(unit, EXT_IPV6_128_DEFIPm)) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_ext_lpm_deinit(unit, mem_v6));
    } else {
        if (mem_v4 != mem_v6) {
            BCM_IF_ERROR_RETURN(soc_fb_lpm_deinit(unit));
        }
    } 

    /* Get memory for IPv6 entries with prefix length > 64. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_l3_defip_mem_get(unit, BCM_L3_IP6,
                                  BCM_XGS3_L3_IPV6_PREFIX_LEN,
                                  &mem_v6_128));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v6 != mem_v6_128) {
         if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
             BCM_IF_ERROR_RETURN(soc_fb_lpm128_deinit(unit));
         } else {
             BCM_IF_ERROR_RETURN(_bcm_defip_pair128_deinit(unit));
         }
    }
    return (BCM_E_NONE);

}

/*
 * Function:
 *      _bcm_tr3_l3_lpm_get
 * Purpose:
 *      Get an entry from the route table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_lpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    soc_mem_t mem = L3_DEFIPm;
    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit);

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                              lpm_cfg->defip_sub_len, &mem));

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        if (mem == L3_DEFIPm || mem == L3_DEFIP_PAIR_128m) {
            return _bcm_l3_scaled_lpm_get(unit, lpm_cfg, nh_ecmp_idx);
        }
    }

    switch (mem) {
        case EXT_IPV4_DEFIPm:
        case EXT_IPV6_64_DEFIPm:
        case EXT_IPV6_128_DEFIPm:
            return _bcm_tr3_ext_lpm_match(unit, lpm_cfg, nh_ecmp_idx);
        case L3_DEFIP_PAIR_128m:
            if (max_v6_entries > 0) {
                return _bcm_l3_defip_pair128_get(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;
        default:
            if (soc_mem_index_count(unit, L3_DEFIPm) > 0) {
                return _bcm_fb_lpm_get(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;
    }

    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *      _bcm_tr3_l3_lpm_add
 * Purpose:
 *      Add an entry to TR3 route table table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_lpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx)
{
    soc_mem_t mem = L3_DEFIPm;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                              lpm_cfg->defip_sub_len, &mem));

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        if (mem == L3_DEFIPm || mem == L3_DEFIP_PAIR_128m) {
            return _bcm_l3_scaled_lpm_add(unit, lpm_cfg, nh_ecmp_idx);
        }
    }

    switch (mem) {
        case EXT_IPV4_DEFIPm:
        case EXT_IPV6_64_DEFIPm:
        case EXT_IPV6_128_DEFIPm:
            if (BCM_L3_RPE_SET(lpm_cfg->defip_flags)) {
                if (lpm_cfg->defip_lookup_class > SOC_ADDR_CLASS_MAX(unit)) {
                    return (BCM_E_PARAM);
                }
            } else {
                lpm_cfg->defip_prio = (lpm_cfg->defip_lookup_class & 0x3C0) >> 6;
                lpm_cfg->defip_lookup_class = lpm_cfg->defip_lookup_class & 0x3F;
            }

            return _bcm_tr3_ext_lpm_add(unit, lpm_cfg, nh_ecmp_idx);
        case L3_DEFIP_PAIR_128m:
            if (soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) > 0) {
                return _bcm_l3_defip_pair128_add(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;
        default:
            if (soc_mem_index_count(unit, L3_DEFIPm) > 0) {
                return _bcm_fb_lpm_add(unit, lpm_cfg, nh_ecmp_idx);
            }
            break;
    }

    return (BCM_E_FULL);
}

/*
 * Function:
 *      _bcm_tr3_l3_lpm_del
 * Purpose:
 *      Delete an entry from the route table.
 * Parameters:
 *      unit    - (IN)SOC unit number.
 *      lpm_cfg - (IN)Buffer to fill defip information. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_lpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg)
{
    soc_mem_t mem = L3_DEFIPm;
    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit);

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_defip_mem_get(unit, lpm_cfg->defip_flags,
                                              lpm_cfg->defip_sub_len, &mem));

    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        if (mem == L3_DEFIPm || mem == L3_DEFIP_PAIR_128m) {
            return _bcm_l3_scaled_lpm_del(unit, lpm_cfg);
        }
    }

    switch (mem) {
        case EXT_IPV4_DEFIPm:
        case EXT_IPV6_64_DEFIPm:
        case EXT_IPV6_128_DEFIPm:
            return _bcm_tr3_ext_lpm_delete(unit, lpm_cfg);
        case L3_DEFIP_PAIR_128m:
            if (max_v6_entries > 0) {
                return _bcm_l3_defip_pair128_del(unit, lpm_cfg);
            }
            break;    
        default: 
            if (soc_mem_index_count(unit, L3_DEFIPm) > 0) {
                return _bcm_fb_lpm_del(unit, lpm_cfg);
            }
            break;    
    }

    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *      _bcm_tr3_l3_lpm_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_lpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    soc_mem_t mem = L3_DEFIPm;  /* Route table memory.      */
    int rv1 = BCM_E_NONE;
    int rv2 = BCM_E_NONE;
    uint32 max_v6_entries = BCM_XGS3_L3_IP6_MAX_128B_ENTRIES(unit);

    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_defip_mem_get(unit, trv_data->flags, 0, &mem));

    switch (mem) {
        case EXT_IPV4_DEFIPm:
        case EXT_IPV6_64_DEFIPm:
        case EXT_IPV6_128_DEFIPm:
            return  _bcm_tr3_defip_traverse(unit, trv_data);
        default: 
            if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                return _bcm_l3_scaled_lpm_update_match(unit, trv_data);
            }

            if ((trv_data->flags & BCM_L3_IP6) && (max_v6_entries > 0)) {
                rv2 = _bcm_l3_defip_pair128_update_match(unit, trv_data);
            }

            if (soc_mem_index_count(unit, L3_DEFIPm) > 0) {
                rv1 = _bcm_fb_lpm_update_match(unit, trv_data);
            }
            BCM_IF_ERROR_RETURN(rv1);
            BCM_IF_ERROR_RETURN(rv2);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_fs_l3_defip_urpf_enable
 * Purpose:
 *      Configure HX4 internal route table for URPF mode
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      enable   - (IN)Enable/disable URPF.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fs_l3_defip_urpf_enable(int unit, int enable)
{
    int defip_config;
    int ipv6_64_depth = 0;
    int ipv6_128_depth = 0;
    int ipv6_128_dip_offset = 0;
    uint32 defip_key_sel_0 = 0;
    uint32 defip_key_sel_1 = 0;
    uint32 num_ipv6_128b_entries = 0;
    int    tcam_pair_count = 0;
    uint32 tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int    tcam_idx = 0;
    int start_index = 0;

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);
    num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);

    if (num_ipv6_128b_entries) {
        tcam_pair_count = (num_ipv6_128b_entries / tcam_depth) +
                          ((num_ipv6_128b_entries % tcam_depth) ? 1 : 0);
    } else {
        tcam_pair_count = 0;
    }

    if (tcam_pair_count > (SOC_CONTROL(unit)->l3_defip_max_tcams / 2)) {
        tcam_pair_count = (SOC_CONTROL(unit)->l3_defip_max_tcams / 2);
    }

    if (enable) {

        /* Ranger device does not support URPF */
        defip_key_sel_1 = 0xFF00;
        if (defip_config) {
            switch (tcam_pair_count) {
                case 0:
                    ipv6_64_depth = 8 * tcam_depth;
                    start_index = 0;
                    break;

                case 1:
                case 2:
                    defip_key_sel_0 = 0x11;
                    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                        ipv6_64_depth = 6 * tcam_depth;
                        start_index = 2 * tcam_depth;
                    }
                    break;
                case 3:
                case 4:
                    defip_key_sel_0 = 0x33;
                    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                        ipv6_64_depth = 4 * tcam_depth;
                        start_index = 4 * tcam_depth;
                    }
                    break;
                case 5:
                case 6:
                    defip_key_sel_0 = 0x77;
                    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                        ipv6_64_depth = 2 * tcam_depth;
                        start_index = 6 * tcam_depth;
                    }
                    break;
                case 7:
                case 8:
                default:
                    defip_key_sel_0 = 0xFF;
                    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                        ipv6_64_depth = 0;
                        start_index = 8 * tcam_depth;
                    }
                    break;
            }

            if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm) / 2;
                ipv6_128_dip_offset = num_ipv6_128b_entries / 2;
                ipv6_128_depth      = num_ipv6_128b_entries / 2;
            }
        } else {
            ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm) / 2;
            ipv6_128_dip_offset = 0;
            ipv6_128_depth      = 0;
        }
    } else { /* Disable uRPF */
        if (defip_config) {
            for (tcam_idx = 0; tcam_idx < tcam_pair_count; tcam_idx++) {
                defip_key_sel_0 |= (0x01 << tcam_idx);
            }

            ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm);
            ipv6_128_dip_offset = num_ipv6_128b_entries;
            ipv6_128_depth      = num_ipv6_128b_entries;
            if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                ipv6_64_depth -= (tcam_pair_count * tcam_depth * 2);
                start_index = (tcam_pair_count * tcam_depth * 2);
            }
        } else {
            ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm);
            ipv6_128_dip_offset = 0;
            ipv6_128_depth      = 0;
        }

    }

    SOC_LPM_STATE_FENT(unit, (MAX_PFX_INDEX(unit)))  = ipv6_64_depth;

    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        BCM_DEFIP_PAIR128_URPF_OFFSET(unit)    = ipv6_128_dip_offset;
        BCM_DEFIP_PAIR128_IDX_MAX(unit)        = ipv6_128_depth - 1;
        BCM_DEFIP_PAIR128_TOTAL(unit)          = ipv6_128_depth;
    }

    SOC_LPM_STATE_START(unit, (MAX_PFX_INDEX(unit))) = start_index;
    SOC_LPM_STATE_END(unit, (MAX_PFX_INDEX(unit)))   = start_index-1;
    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SEL_0r(unit, defip_key_sel_0));
    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SEL_1r(unit, defip_key_sel_1));

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_hx4_l3_defip_urpf_enable
 * Purpose:
 *      Configure HX4 internal route table for URPF mode
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      enable   - (IN)Enable/disable URPF.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_hx4_l3_defip_urpf_enable(int unit, int enable)
{
    int defip_config;
    int ipv6_64_depth = 0;
    int ipv6_128_depth = 0;
    int ipv6_128_dip_offset = 0;
    uint32 defip_key_sel_0 = 0;
    uint32 defip_key_sel_1 = 0;
    uint32 num_ipv6_128b_entries = 0;
    int    tcam_pair_count = 0;
    uint32 tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int    tcam_idx = 0;
    int start_index = 0;

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);
    num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);

    if (num_ipv6_128b_entries) {
        tcam_pair_count = (num_ipv6_128b_entries / tcam_depth) +
                          ((num_ipv6_128b_entries % tcam_depth) ? 1 : 0);
    } else {
        tcam_pair_count = 0;
    }

    if (tcam_pair_count > (SOC_CONTROL(unit)->l3_defip_max_tcams / 2)) {
        tcam_pair_count = (SOC_CONTROL(unit)->l3_defip_max_tcams / 2);
    }

    if (enable) {

        /* Ranger device does not support URPF */
        defip_key_sel_1 = 0xF0;
        if (SOC_CONTROL(unit)->l3_defip_max_tcams == 4) {
            defip_key_sel_1 = 0xC;
        }

        if (defip_config) {
            switch (tcam_pair_count) {
                case 0:
                    ipv6_64_depth = 4 * tcam_depth;
                    start_index = 0;
                break;

                case 1:
                case 2:
                   defip_key_sel_0 = 0x5; /* 0x0101 */

                   if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                       ipv6_64_depth = 2 * tcam_depth;
                       start_index = 2 * tcam_depth;

                       if (SOC_CONTROL(unit)->l3_defip_max_tcams == 4) {
                           defip_key_sel_0 = 0x3;
                       }
                   }
                   break;

                case 3:
                case 4:
                default:
                   defip_key_sel_0 = 0xF; /* 0x1111 */

                   if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                       ipv6_64_depth = 0;
                       start_index = 4 * tcam_depth;
                   }
                   break;
            }

            if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm) / 2;
                ipv6_128_dip_offset = num_ipv6_128b_entries / 2;
                ipv6_128_depth      = num_ipv6_128b_entries / 2;
            }
        } else {
            ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm) / 2;
            ipv6_128_dip_offset = 0;
            ipv6_128_depth      = 0;
        }
    } else { /* Disable uRPF */
        if (defip_config) {
            for (tcam_idx = 0; tcam_idx < tcam_pair_count; tcam_idx++) {
                defip_key_sel_0 |= (0x01 << tcam_idx);
            }

            ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm);
            ipv6_128_dip_offset = num_ipv6_128b_entries;
            ipv6_128_depth      = num_ipv6_128b_entries;
            if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                ipv6_64_depth -= (tcam_pair_count * tcam_depth * 2);
                start_index = (tcam_pair_count * tcam_depth * 2);
            }
        } else {
            ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm);
            ipv6_128_dip_offset = 0;
            ipv6_128_depth      = 0;
        }

    }

    SOC_LPM_STATE_FENT(unit, (MAX_PFX_INDEX(unit)))  = ipv6_64_depth;
    
    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        BCM_DEFIP_PAIR128_URPF_OFFSET(unit)    = ipv6_128_dip_offset;
        BCM_DEFIP_PAIR128_IDX_MAX(unit)        = ipv6_128_depth - 1;
        BCM_DEFIP_PAIR128_TOTAL(unit)          = ipv6_128_depth;
    }

    SOC_LPM_STATE_START(unit, (MAX_PFX_INDEX(unit))) = start_index;
    SOC_LPM_STATE_END(unit, (MAX_PFX_INDEX(unit)))   = start_index-1;
    BCM_IF_ERROR_RETURN(soc_fb_lpm_state_config(unit, ipv6_64_depth, 
                start_index));

    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SEL_0r(unit, defip_key_sel_0));
    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SEL_1r(unit, defip_key_sel_1));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_l3_expanded_defip_urpf_enable
 * Purpose:
 *      Configure HX4 internal route table for URPF mode
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      enable   - (IN)Enable/disable URPF.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l3_expanded_defip_urpf_enable(int unit, int enable)
{
    if (soc_feature(unit, soc_feature_l3_32k_defip_table)) {
        /* Firescout family - 16 tcams */
        return _bcm_fs_l3_defip_urpf_enable(unit, enable);
    } else {
        /* Helix4 family - 8 tcams */
        return _bcm_hx4_l3_defip_urpf_enable(unit, enable);
    }
}

/*
 * Function:
 *      _bcm_tr3_l3_defip_urpf_enable
 * Purpose:
 *      Configure TR3 internal route table for URPF mode
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      enable   - (IN)Enable/disable URPF.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_defip_urpf_enable(int unit, int enable)
{
    int defip_config;
    uint32 defip_key_sel_val = 0;
    int ipv6_64_depth = 0;
    int ipv6_128_dip_offset = 0, ipv6_128_depth = 0;
    int num_ipv6_128b_entries;
    int tcam_pair_count = 0;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int start_index = 0;

    if (soc_feature(unit, soc_feature_l3_expanded_defip_table)) {/* HX4 */
        return _bcm_l3_expanded_defip_urpf_enable(unit, enable);
    }

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);
    num_ipv6_128b_entries =  SOC_L3_DEFIP_MAX_128B_ENTRIES(unit);

    if (num_ipv6_128b_entries) {
        tcam_pair_count = (num_ipv6_128b_entries / tcam_depth) +
                          ((num_ipv6_128b_entries % tcam_depth) ? 1 : 0);
    } else {
        tcam_pair_count = 0;
    }
    
    if (enable) { 
        if (soc_feature(unit, soc_feature_l3_reduced_defip_table)) {/*Apollo2*/
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                              URPF_LOOKUP_CAM3f, 0x1);
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                              URPF_LOOKUP_CAM4f, 0x1);
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                              URPF_LOOKUP_CAM5f, 0x1);

            if (defip_config) {
                switch (tcam_pair_count) {
                    case 0:
                        ipv6_64_depth       = 3 * _BCM_TR3_DEFIP_TCAM_DEPTH;
                        ipv6_128_depth      = 0;
                        ipv6_128_dip_offset = 0;
                    break;

                    default:
                        soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                                V6_KEY_SEL_CAM0_1f, 0x1);
                        soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                               V6_KEY_SEL_CAM4_5f, 0x1);
                        ipv6_128_dip_offset = num_ipv6_128b_entries / 2;
                        ipv6_128_depth      = num_ipv6_128b_entries / 2;
                        ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm) / 2;
                    break;
                }
            } else {
                ipv6_64_depth       = 3 * _BCM_TR3_DEFIP_TCAM_DEPTH;
                ipv6_128_dip_offset = 0;
                ipv6_128_depth      = 0;
            }        
        } else { /* TR3 */
            /* All CAMs in URPF mode */
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                              URPF_LOOKUP_CAM4f, 0x1);
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                              URPF_LOOKUP_CAM5f, 0x1);
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                              URPF_LOOKUP_CAM6f, 0x1);
            soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                              URPF_LOOKUP_CAM7f, 0x1);
    
            switch (tcam_pair_count) {
                case 0:
                    /* No pairing of tcams */
                    ipv6_64_depth = 4 * tcam_depth;
                    break;
    
              case 1:
                case 2:
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                       V6_KEY_SEL_CAM0_1f, 0x1);
                soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                       V6_KEY_SEL_CAM4_5f, 0x1);
                if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                    ipv6_64_depth = 2 * tcam_depth;
                    start_index = 2 * tcam_depth;
                }
                break;

              case 3:
              case 4:
              default: 
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM0_1f, 
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM2_3f, 
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM4_5f, 
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM6_7f, 
                                      0x1);
                    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                        ipv6_64_depth = 0;
                        start_index = 4 * tcam_depth;
                    }

                break;
            }

            if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                ipv6_128_dip_offset = num_ipv6_128b_entries / 2;
                ipv6_128_depth      = num_ipv6_128b_entries / 2;
                ipv6_64_depth =  soc_mem_index_count(unit, L3_DEFIPm) / 2;
            }
        }
    } else { /* Disable urpf */
        defip_key_sel_val = 0; /* reset */
        if (soc_feature(unit, soc_feature_l3_reduced_defip_table)) {/*Apollo2*/
            if (defip_config) {
                switch (tcam_pair_count) {
                  case 0:
                  break;   

                  case 1:
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                           V6_KEY_SEL_CAM0_1f, 0x1);
                  break;   

                  case 2:

                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                           V6_KEY_SEL_CAM0_1f, 0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                           V6_KEY_SEL_CAM2_3f, 0x1);
                  break;

                  case 3:
                  default:
                     soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                            V6_KEY_SEL_CAM0_1f, 0x1);
                     soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                            V6_KEY_SEL_CAM2_3f, 0x1);
                     soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, &defip_key_sel_val, 
                                            V6_KEY_SEL_CAM4_5f, 0x1);
                }
                ipv6_64_depth       = soc_mem_index_count(unit, L3_DEFIPm);
                ipv6_128_dip_offset = 0;
                ipv6_128_depth      = num_ipv6_128b_entries;
            } else {
                ipv6_64_depth       = 6 * _BCM_TR3_DEFIP_TCAM_DEPTH;
                ipv6_128_dip_offset = 0;
                ipv6_128_depth      = 0;
            }
        } else {
            switch (tcam_pair_count) {
                case 1:
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM0_1f, 
                                      0x1);
                break;

                case 2:
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM0_1f, 
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM2_3f, 
                                      0x1);
                break;

                case 3:
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM0_1f, 
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM2_3f, 
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM4_5f, 
                                      0x1);
                break;

                case 4:
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM0_1f, 
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM2_3f, 
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM4_5f, 
                                      0x1);
                    soc_reg_field_set(unit, L3_DEFIP_KEY_SELr, 
                                      &defip_key_sel_val, V6_KEY_SEL_CAM6_7f, 
                                      0x1);
                break;   

                default:
                    break;
    }
    
            ipv6_64_depth =  soc_mem_index_count(unit, L3_DEFIPm);
            ipv6_128_dip_offset = 0;
            ipv6_128_depth      = num_ipv6_128b_entries;
            if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                ipv6_64_depth -= (tcam_pair_count * tcam_depth * 2);
                start_index = (tcam_pair_count * tcam_depth * 2);
            }
        }
    }

    if (!soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        if (BCM_DEFIP_PAIR128_INIT_CHECK(unit)) {
            BCM_DEFIP_PAIR128_URPF_OFFSET(unit)    = ipv6_128_dip_offset;
            BCM_DEFIP_PAIR128_IDX_MAX(unit)        = ipv6_128_depth - 1;
            BCM_DEFIP_PAIR128_TOTAL(unit)          = ipv6_128_depth;
        }
    }

    if (SOC_LPM_INIT_CHECK(unit)) {
        SOC_LPM_STATE_FENT(unit, (MAX_PFX_INDEX(unit)))  = ipv6_64_depth;
        SOC_LPM_STATE_START(unit, MAX_PFX_INDEX(unit)) = start_index;
        SOC_LPM_STATE_END(unit, MAX_PFX_INDEX(unit))   = start_index - 1;
        BCM_IF_ERROR_RETURN(soc_fb_lpm_state_config(unit, ipv6_64_depth,
                    start_index));
    }

    BCM_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(unit, defip_key_sel_val));
    SOC_IF_ERROR_RETURN(soc_fb_lpm_stat_init(unit));
    if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
        SOC_IF_ERROR_RETURN(soc_fb_lpm128_stat_init(unit));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_mtu_profile_index_get
 * Purpose:
 *      Get TR3 L3 interface MTU profile table index
 *      given the MTU value to set.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      mtu       - (IN)MTU value to set.
 *      index     - (OUT)MTU profile index.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_tr3_mtu_profile_index_get(int unit, uint32 mtu, uint32 *index) 
{
    ing_l3_next_hop_attribute_1_entry_t mtu_value_buf;
    uint32  *mtu_value_buf_p; /* Attribute table buff address */
    void *null_entry = soc_mem_entry_null(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1m);
    void *entries[1];

    if (NULL == index) {
        return BCM_E_PARAM;
    }

    /* Reset the buffer to default value. */
    mtu_value_buf_p = (uint32 *)&mtu_value_buf;
    sal_memcpy(mtu_value_buf_p, null_entry,
               sizeof(ing_l3_next_hop_attribute_1_entry_t));

    soc_mem_field32_set(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1m,
                        mtu_value_buf_p, MTU_SIZEf, mtu);

    entries[0] = mtu_value_buf_p;
    BCM_IF_ERROR_RETURN(_bcm_ing_l3_nh_attrib_entry_add(unit, entries, 1, 
                                             (uint32 *)index));
    return BCM_E_NONE;
} 

/*
 * Function:
 *      _bcm_tr3_l3_intf_mtu_get
 * Purpose:
 *      Get TR3 L3 interface MTU value. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      intf_info - (IN)Interface information.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_intf_mtu_get(int unit, _bcm_l3_intf_cfg_t *intf_info)
{
    /* Buffer for mtu idx */
    ing_l3_next_hop_attribute_1_index_entry_t mtu_idx_value_buf;  
    /* Buffer for mtu profile table entry */
    ing_l3_next_hop_attribute_1_entry_t mtu_value_buf;  
    uint32  *mtu_idx_value_buf_p;          /* Write buffer address */
    uint32  *mtu_value_buf_p;          /* Write buffer address */
    int  mtu_index, mtu_profile_index;
    int  interface_map_mode = 0;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    /* Zero the buffer. */
    mtu_idx_value_buf_p = (uint32 *)&mtu_idx_value_buf;
    sal_memset(mtu_idx_value_buf_p, 0, sizeof(ing_l3_next_hop_attribute_1_index_entry_t));

    mtu_value_buf_p = (uint32 *)&mtu_value_buf;
    sal_memset(mtu_value_buf_p, 0, sizeof(ing_l3_next_hop_attribute_1_entry_t));

    if (!BCM_XGS3_L3_INTF_VLAN_SPLIT_EGRESS_MODE_ISSET(unit)) {
        if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
            BCM_IF_ERROR_RETURN(
                bcm_xgs3_l3_ingress_intf_map_get(unit, &interface_map_mode));
        }
        if (!interface_map_mode) {
            mtu_index = intf_info->l3i_vid;
        } else {
            mtu_index = intf_info->l3i_index;
        }
        BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, 
                                              ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm,
                                              mtu_index, mtu_idx_value_buf_p));
    } else {
    /* Read mtu table entry by index. */
        BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, 
                                              ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm,
                                              intf_info->l3i_index, 
                                              mtu_idx_value_buf_p));
    }
    mtu_profile_index = 
        soc_mem_field32_get(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm,
                            mtu_idx_value_buf_p, INDEXf);

    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1m,
                                          mtu_profile_index, mtu_value_buf_p));
    intf_info->l3i_mtu = 
        soc_mem_field32_get(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1m,
                            mtu_value_buf_p, MTU_SIZEf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_intf_mtu_set
 * Purpose:
 *      Set TR3 L3 interface MTU value. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      intf_info - (IN)Interface information.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_intf_mtu_set(int unit, _bcm_l3_intf_cfg_t *intf_info)
{
    /* Buffer for mtu idx */
    ing_l3_next_hop_attribute_1_index_entry_t mtu_idx_value_buf; 
    void *null_idx_entry =
                 soc_mem_entry_null(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm);
    int  mtu_index, mtu_profile_index, interface_map_mode = 0;
    uint32 mtu = _BCM_TR3_L3_DEFAULT_MTU_SIZE; /* Default mtu size */

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    /* coverity[dead_error_begin : FALSE] */
    if(!SOC_MEM_FIELD32_VALUE_FIT(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1m, 
                                  MTU_SIZEf, intf_info->l3i_mtu)) {
        return (BCM_E_PARAM);
    }

    /* Set mtu. */
    if (intf_info->l3i_mtu) {
        mtu = intf_info->l3i_mtu;
    }
    
    /* Get mtu profile table index for the mtu */
    BCM_IF_ERROR_RETURN(
     _bcm_tr3_mtu_profile_index_get(unit, mtu,
                                    (uint32 *)&mtu_profile_index));

    sal_memcpy(&mtu_idx_value_buf, null_idx_entry,
               sizeof(ing_l3_next_hop_attribute_1_index_entry_t));
    soc_mem_field32_set(unit, ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm,
                        &mtu_idx_value_buf, INDEXf, mtu_profile_index);
    if (!BCM_XGS3_L3_INTF_VLAN_SPLIT_EGRESS_MODE_ISSET(unit)) {
        if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
            BCM_IF_ERROR_RETURN(
                bcm_xgs3_l3_ingress_intf_map_get(unit, &interface_map_mode));
        }
        if (!interface_map_mode) {
            mtu_index = intf_info->l3i_vid;
        } else {
            mtu_index = intf_info->l3i_index;
        }
        WRITE_ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm(unit, MEM_BLOCK_ALL,
                                                 mtu_index, &mtu_idx_value_buf);
    } else {
        WRITE_ING_L3_NEXT_HOP_ATTRIBUTE_1_INDEXm(unit, MEM_BLOCK_ALL,
                                                 intf_info->l3i_index, 
                                                 &mtu_idx_value_buf);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_ecmp_grp_get
 * Purpose:
 *      Get ecmp group next hop members by index.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      ecmp_grp - (IN) Ecmp group id to read. 
 *      ecmp_count - (IN) Maximum number of entries to read.
 *      nh_idx     - (OUT) Next hop indexes. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_ecmp_grp_get(int unit, int ecmp_grp, int ecmp_group_size, int *nh_idx)
{
    int idx;
    void *ecmp_null_entry;
    void *ecmp_grp_null_entry;
    ecmp_entry_t ecmp_entry;
    ecmp_count_entry_t ecmp_grp_entry;
    int ecmp_idx, num_ecmp_entries;

    /* Input parameters sanity check. */
    if ((NULL == nh_idx) || (ecmp_group_size < 1)) {
         return (BCM_E_PARAM);
    }

    ecmp_grp_null_entry = soc_mem_entry_null(unit, L3_ECMP_COUNTm);
    sal_memcpy(&ecmp_grp_entry, ecmp_grp_null_entry, sizeof(ecmp_count_entry_t));

    /* Read ECMP group table entry indexed by group ID */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMP_COUNTm,
                        MEM_BLOCK_ANY, ecmp_grp, &ecmp_grp_entry));

    /* Base ptr & path count of the ECMP group in the ECMP entry table */
    ecmp_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, BASE_PTRf);
    num_ecmp_entries = soc_mem_field32_get(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, COUNTf);
    num_ecmp_entries++; /* Count is zero based */ 

    sal_memset(nh_idx, 0, ecmp_group_size * sizeof(int));
    ecmp_null_entry = soc_mem_entry_null(unit, L3_ECMPm);
    sal_memcpy(&ecmp_entry, ecmp_null_entry, sizeof(ecmp_entry_t));

    /* Read the NH index of each member of the ECMP group */    
    for (idx = 0; idx < num_ecmp_entries; idx++) {
    
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMPm,
                            MEM_BLOCK_ANY, (ecmp_idx + idx), &ecmp_entry));
        nh_idx[idx] = soc_mem_field32_get(unit, L3_ECMPm,
                                          &ecmp_entry, NEXT_HOP_INDEXf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_ecmp_grp_add
 * Purpose:
 *      Add ecmp group next hop members, or reset ecmp group entry.  
 *      NOTE: Function always writes all the entries in ecmp group.
 *            If there is not enough nh indexes - next hops written
 *            in cycle. 
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      ecmp_grp   - (IN)ecmp group id to write.
 *      buf        - (IN)Next hop indexes or NULL for entry reset.
 *      info       - (IN)ECMP additional info
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_ecmp_grp_add(int unit, int ecmp_grp, void *buf, void *info)
{
    uint32 reg_val, value;
    _bcm_l3_tbl_op_t data;
    int *nh_idx = NULL;
    int nh_cycle_idx;
    int idx, ecmp_idx, entry_type;
    void *ecmp_null_entry, *initial_ecmp_null_entry;
    void *ecmp_grp_null_entry, *initial_ecmp_grp_null_entry;
    ecmp_entry_t ecmp_entry;
    ing_l3_next_hop_entry_t ing_nh;
    ecmp_count_entry_t ecmp_grp_entry;
    initial_l3_ecmp_entry_t initial_ecmp_entry;
    initial_l3_ecmp_group_entry_t initial_ecmp_grp_entry;
    int l3_ecmp_oif_flds[8] = {  L3_OIF_0f, L3_OIF_1f, L3_OIF_2f, L3_OIF_3f, 
                                 L3_OIF_4f, L3_OIF_5f, L3_OIF_6f, L3_OIF_7f }; 
    int l3_ecmp_oif_type_flds[8] = {  L3_OIF_0_TYPEf, L3_OIF_1_TYPEf, 
                             L3_OIF_2_TYPEf, L3_OIF_3_TYPEf, L3_OIF_4_TYPEf, 
                             L3_OIF_5_TYPEf, L3_OIF_6_TYPEf, L3_OIF_7_TYPEf };
    ecmp_count_entry_t old_ecmp_count_entry;
    int dlb_id, group_enable;
    int rv;
    int ecmp_table_incr = 0;
    _bcm_l3_ecmp_group_info_t *ecmp_info = info;
    _bcm_l3_ecmp_group_buffer_t *ecmp_buffer = buf;

    if (NULL == ecmp_buffer || NULL == ecmp_info ||
        NULL == ecmp_buffer->group_buffer) {
        return (BCM_E_PARAM);
    }

    nh_idx = ecmp_buffer->group_buffer;

    /* Null ECMP and ECMP group table entries */
    ecmp_null_entry = soc_mem_entry_null(unit, L3_ECMPm);
    ecmp_grp_null_entry = soc_mem_entry_null(unit, L3_ECMP_COUNTm);
    initial_ecmp_null_entry = soc_mem_entry_null(unit, INITIAL_L3_ECMPm);
    initial_ecmp_grp_null_entry = soc_mem_entry_null(unit, INITIAL_L3_ECMP_GROUPm);
    sal_memcpy(&ecmp_grp_entry, ecmp_grp_null_entry, sizeof(ecmp_count_entry_t));
    sal_memcpy(&initial_ecmp_grp_entry, initial_ecmp_grp_null_entry,
               sizeof(initial_l3_ecmp_group_entry_t));

    if (BCM_XGS3_L3_ENT_REF_CNT(BCM_XGS3_L3_TBL_PTR(unit,
                                ecmp_grp), ecmp_grp)) {
        /* Group has already exists, get base ptr from group table */ 
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMP_COUNTm,
                            MEM_BLOCK_ANY, ecmp_grp, &ecmp_grp_entry));
        if (soc_feature(unit, soc_feature_l3_ecmp_1k_groups)) {
            ecmp_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, BASE_PTRf);
        } else {
            ecmp_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, BASE_PTR_0f);
        }
    } else {  
        /* Get index to the first slot in the ECMP table
         * that can accomodate max_paths */
        data.width = ecmp_info->max_paths;
        data.tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, ecmp); 
        data.oper_flags = _BCM_L3_SHR_TABLE_TRAVERSE_CONTROL; 
        data.entry_index = -1;

        rv = _bcm_xgs3_tbl_free_idx_get(unit, &data);

        if (BCM_E_FULL == rv) {
            /* Defragment ECMP table */
            BCM_IF_ERROR_RETURN(bcm_tr2_l3_ecmp_defragment_no_lock(unit, 0));

            /* Attempt to get free index again */
            BCM_IF_ERROR_RETURN (_bcm_xgs3_tbl_free_idx_get(unit, &data));

        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        ecmp_idx = data.entry_index;
        BCM_XGS3_L3_ENT_REF_CNT_INC(data.tbl_ptr, ecmp_idx, ecmp_info->max_paths);
        ecmp_table_incr = 1;
    }

    sal_memcpy(&ecmp_grp_entry, ecmp_grp_null_entry, sizeof(ecmp_count_entry_t));

    for (idx = 0, nh_cycle_idx = 0; idx < ecmp_info->max_paths; idx++, nh_cycle_idx++) {
        sal_memcpy(&ecmp_entry, ecmp_null_entry, sizeof(ecmp_entry_t));
        sal_memcpy(&initial_ecmp_entry, initial_ecmp_null_entry,
                   sizeof(initial_l3_ecmp_entry_t));

        /* If this is the last nhop then program black-hole. */
        if ( (!idx) && (!nh_idx[nh_cycle_idx]) ) {
              nh_cycle_idx = 0;
        } else  if (!nh_idx[nh_cycle_idx]) {
            break;
        }

        /* Write NHs to ECMP entry table */
        soc_mem_field32_set(unit, L3_ECMPm, &ecmp_entry, 
                            NEXT_HOP_INDEXf, nh_idx[nh_cycle_idx]);
        rv = soc_mem_write(unit, L3_ECMPm, MEM_BLOCK_ALL,
                                         (ecmp_idx + idx), &ecmp_entry);
        if (BCM_FAILURE(rv)) {
            if (ecmp_table_incr == 1) {
                BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                            ecmp_idx, ecmp_info->max_paths);
            }
            return rv;
        } 

        /* Write NHs to initial ECMP entry table */
        soc_mem_field32_set(unit, INITIAL_L3_ECMPm, &initial_ecmp_entry, 
                            NEXT_HOP_INDEXf, nh_idx[idx]);
        rv = soc_mem_write(unit, INITIAL_L3_ECMPm, MEM_BLOCK_ALL,
                                         (ecmp_idx + idx), &initial_ecmp_entry);
        if (BCM_FAILURE(rv)) {
            if (ecmp_table_incr == 1) {
                BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                            ecmp_idx, ecmp_info->max_paths);
            }
            return rv;
        } 

        if (soc_feature(unit, soc_feature_urpf)) {
            /* Check if URPF is enabled on device */
            BCM_IF_ERROR_RETURN(
                READ_L3_DEFIP_RPF_CONTROLr(unit, &reg_val));
            if (reg_val) {
                if (idx < 8) {
                    BCM_IF_ERROR_RETURN (soc_mem_read(unit, 
                         ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                         nh_idx[idx], &ing_nh));

                    entry_type = 
                      soc_ING_L3_NEXT_HOPm_field32_get(unit, 
                           &ing_nh, ENTRY_TYPEf);

                    if (entry_type == 0x0) {
                        /* 
                         * if l3_oif is valid then read it than vlan.
                         * vlan and l3_oifs are overlaid fields.
                         */
                        if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm,
                                                L3_OIFf)) {
                            value = soc_ING_L3_NEXT_HOPm_field32_get(
                                        unit, &ing_nh, L3_OIFf);
                        } else {
                            value = soc_ING_L3_NEXT_HOPm_field32_get(
                                        unit, &ing_nh, VLAN_IDf);
                        }

                        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, 
                                       l3_ecmp_oif_type_flds[idx], entry_type);
                        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, 
                                            l3_ecmp_oif_flds[idx], value);
                    } else if (entry_type == 0x1) {
                        value  = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, L3_OIFf);
                        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, 
                                       l3_ecmp_oif_type_flds[idx], entry_type);
                        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, 
                                       l3_ecmp_oif_flds[idx], value);
                    }
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, URPF_COUNTf, idx);
                }else {
                    /* Inorder to avoid TRAP_TO_CPU, urpf_mode on L3_IIF/PORT must be set 
                       to STRICT_MODE / LOOSE_MODE */
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, ECMP_GT8f , 1);
                }
            }
        }
    }

    /* Write ECMP group count and base index to group table entry 
     * indexed by group index */
    if (!idx) {
        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, COUNTf, idx);
        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_ecmp_grp_entry, COUNTf, idx);
    } else {
        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, COUNTf, idx-1);
        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_ecmp_grp_entry, COUNTf, idx-1);
    }
    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                        &ecmp_grp_entry, BASE_PTRf, ecmp_idx);
    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                        &initial_ecmp_grp_entry, BASE_PTRf, ecmp_idx);

    if (soc_feature(unit, soc_feature_ecmp_dlb)) {
        /* Preserve the old values of ECMP DLB related fields */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMP_COUNTm, MEM_BLOCK_ANY,
                    ecmp_grp, &old_ecmp_count_entry));
        dlb_id = soc_mem_field32_get(unit, L3_ECMP_COUNTm, &old_ecmp_count_entry,
                DLB_IDf);
        group_enable = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                &old_ecmp_count_entry, GROUP_ENABLEf);
        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, DLB_IDf,
                dlb_id);
        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, GROUP_ENABLEf,
                group_enable);
    }

    rv = soc_mem_write(unit, L3_ECMP_COUNTm, MEM_BLOCK_ALL,
                  ecmp_grp, &ecmp_grp_entry);
    if (BCM_FAILURE(rv)) {
        if (ecmp_table_incr == 1) {
            BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                        ecmp_idx, ecmp_info->max_paths);
        }
        return rv;
    }

    rv = soc_mem_write(unit, INITIAL_L3_ECMP_GROUPm, MEM_BLOCK_ALL,
                  ecmp_grp, &initial_ecmp_grp_entry);

    if (BCM_FAILURE(rv)) {
        if (ecmp_table_incr == 1) {
            BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                        ecmp_idx, ecmp_info->max_paths);
        }
        return rv;
    }

    /* Save the max possible paths for this ECMP group in s/w */ 
    BCM_XGS3_L3_MAX_PATHS_PERGROUP_PTR(unit)[ecmp_grp] = ecmp_info->max_paths;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_ecmp_grp_del
 * Purpose:
 *      Reset ecmp group next hop members
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      ecmp_grp   - (IN)ecmp group id to write.
 *      info       - (IN)ECMP additional info
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_ecmp_grp_del(int unit, int ecmp_grp, void *info)
{
    int idx, ecmp_idx;
    _bcm_l3_tbl_op_t data;
    void *ecmp_null_entry, *initial_ecmp_null_entry;
    void *ecmp_grp_null_entry, *initial_ecmp_grp_null_entry;
    ecmp_entry_t ecmp_entry;
    ecmp_count_entry_t ecmp_grp_entry;
    initial_l3_ecmp_entry_t initial_ecmp_entry;
    initial_l3_ecmp_group_entry_t initial_ecmp_grp_entry;
    ecmp_count_entry_t old_ecmp_count_entry;
    int dlb_id, group_enable;

    if (NULL == info) {
        return BCM_E_INTERNAL;
    }

    /* Null ECMP and ECMP group table entries */
    ecmp_null_entry = soc_mem_entry_null(unit, L3_ECMPm);
    ecmp_grp_null_entry = soc_mem_entry_null(unit, L3_ECMP_COUNTm);
    initial_ecmp_null_entry = soc_mem_entry_null(unit, INITIAL_L3_ECMPm);
    initial_ecmp_grp_null_entry = soc_mem_entry_null(unit, INITIAL_L3_ECMP_GROUPm);
    sal_memcpy(&ecmp_entry, ecmp_null_entry, sizeof(ecmp_entry_t));
    sal_memcpy(&ecmp_grp_entry, ecmp_grp_null_entry, sizeof(ecmp_count_entry_t));
    sal_memcpy(&initial_ecmp_entry, initial_ecmp_null_entry,
               sizeof(initial_l3_ecmp_entry_t));
    sal_memcpy(&initial_ecmp_grp_entry, initial_ecmp_grp_null_entry,
               sizeof(initial_l3_ecmp_group_entry_t));

    /* Get the base entry index of the group */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMP_COUNTm,
                                     MEM_BLOCK_ANY, ecmp_grp, &ecmp_grp_entry));
    ecmp_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, BASE_PTRf);

    /* Write null entries in the ECMP entry tables per group member
     * starting from the base group entry */
    for (idx = 0; idx < ((_bcm_l3_ecmp_group_info_t*)info)->max_paths; idx++) {
         /* ECMP entry table */
         BCM_IF_ERROR_RETURN(soc_mem_write(unit, L3_ECMPm, MEM_BLOCK_ALL, 
                                          (ecmp_idx + idx), &ecmp_entry));
         /* Initial ECMP entry table */
         BCM_IF_ERROR_RETURN(soc_mem_write(unit, INITIAL_L3_ECMPm, MEM_BLOCK_ALL, 
                                          (ecmp_idx + idx), &initial_ecmp_entry));
    }

    /* Decrement ref count for the entries in ecmp table
     * Ref count for ecmp_group table is decremented in common table del func. */
    data.tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, ecmp); 
    BCM_XGS3_L3_ENT_REF_CNT_DEC(data.tbl_ptr, ecmp_idx,
                                ((_bcm_l3_ecmp_group_info_t*)info)->max_paths);

    /* Reset group entry */
    sal_memcpy(&ecmp_grp_entry, ecmp_grp_null_entry, sizeof(ecmp_count_entry_t));

    if (soc_feature(unit, soc_feature_ecmp_dlb)) {
        /* Preserve the old values of ECMP DLB related fields */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMP_COUNTm, MEM_BLOCK_ANY,
                    ecmp_grp, &old_ecmp_count_entry));
        dlb_id = soc_mem_field32_get(unit, L3_ECMP_COUNTm, &old_ecmp_count_entry,
                DLB_IDf);
        group_enable = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                &old_ecmp_count_entry, GROUP_ENABLEf);
        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, DLB_IDf,
                dlb_id);
        soc_mem_field32_set(unit, L3_ECMP_COUNTm, &ecmp_grp_entry, GROUP_ENABLEf,
                group_enable);
    }

    /* Write null entries in the ECMP group tables indexed by group id */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, L3_ECMP_COUNTm, MEM_BLOCK_ALL,
                                      ecmp_grp, &ecmp_grp_entry));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, INITIAL_L3_ECMP_GROUPm, MEM_BLOCK_ALL,
                                      ecmp_grp, &initial_ecmp_grp_entry));

    /* Reset the max paths of the deleted group */
    BCM_XGS3_L3_MAX_PATHS_PERGROUP_PTR(unit)[ecmp_grp] = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_tnl_term_add
 * Purpose:
 *      Add tunnel termination entry to the hw.
 * Parameters:
 *      unit     - (IN) BCM device number. 
 *      tnl_info - (IN) Tunnel terminator parameters.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_tnl_term_add(int unit, uint32 *entry_ptr, bcm_tunnel_terminator_t *tnl_info)
{
    bcm_module_t mod_in, mod_out, my_mod;
    bcm_port_t port_in, port_out;
    _bcm_l3_ingress_intf_t iif;
    int tunnel, wlan;
    int rv;

    /* Program remote port */
    if ((tnl_info->type == bcmTunnelTypeWlanWtpToAc) || 
        (tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
        (tnl_info->type == bcmTunnelTypeWlanWtpToAc6) ||
        (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) {
        wlan = 1;
    } else {
        wlan = 0;
    }
    if (wlan) {
        if (tnl_info->flags & BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE) {
            if (!BCM_GPORT_IS_MODPORT(tnl_info->remote_port)) {
                return BCM_E_PARAM;
            }
            mod_in = BCM_GPORT_MODPORT_MODID_GET(tnl_info->remote_port);
            port_in = BCM_GPORT_MODPORT_PORT_GET(tnl_info->remote_port); 
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in, 
                                        port_in, &mod_out, &port_out));
            if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
                return (BCM_E_BADID);
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
                return (BCM_E_PORT);
            }
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, REMOTE_TERM_GPPf,
                                       (mod_out << 6) | port_out);
        } else {
            /* Send to the local loopback port */
            rv = bcm_esw_stk_my_modid_get(unit, &my_mod);
            BCM_IF_ERROR_RETURN(rv);
            port_in = AXP_PORT(unit,SOC_AXP_NLF_WLAN_DECAP);
            mod_in = my_mod;
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in, 
                                        port_in, &mod_out, &port_out);
            BCM_IF_ERROR_RETURN(rv);

            soc_L3_TUNNELm_field32_set(unit, entry_ptr, REMOTE_TERM_GPPf,
                                       (mod_out << 6) | port_out);
            /* Locally Terminated */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, 
                                       LOCAL_TUNNEL_TERMINATIONf, 1 );
        }
        /* Program tunnel id */
        if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
            if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
                return BCM_E_PARAM;
            }
            if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_IDf)) {
                tunnel = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, TUNNEL_IDf,
                                           tunnel);
            }
        }

        /* Program HA/FA for Ac2Ac tunnels */
        if (((tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
            (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) &&
            (tnl_info->flags &  BCM_TUNNEL_TERM_WLAN_SET_ROAM)) {
            _bcm_tr3_wlan_tunnel_set_roam(unit, tnl_info->tunnel_id);    
        }
    }
    if ((tnl_info->type == bcmTunnelTypeAutoMulticast) ||
        (tnl_info->type == bcmTunnelTypeAutoMulticast6)) {
        /* Program L3_IIFm */
        if(SOC_MEM_FIELD_VALID(unit, L3_IIFm, IPMC_L3_IIFf)) {
            sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
            iif.intf_id = tnl_info->vlan;

            rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
            BCM_IF_ERROR_RETURN(rv);
            iif.vrf = tnl_info->vrf;
            rv = _bcm_tr_l3_ingress_interface_set(unit, &iif, NULL, NULL);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Program tunnel id */
        if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
            if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
                return BCM_E_PARAM;
            }
            if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_IDf)) {
                tunnel = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, TUNNEL_IDf,
                                           tunnel);
            }
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr3_nh_update_match
 * Purpose:
 *      Update/Show/Delete all entries in nh table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_nh_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    bcm_l3_egress_t nh_entry;     /* Next hop entry info.           */
    uint32 *ing_entry_ptr;        /* Ingress next hop entry pointer.*/ 
    char *ing_tbl_ptr;            /* Dma table pointer.             */
    int cmp_result;               /* Test routine result.           */
    soc_mem_t mem;                /* Next hop memory.               */
    soc_mem_t mem_egr_nh;                /* Next hop memory.               */
    int idx;                      /* Iteration index.               */
    char *egr_tbl_buf;            /* Dma egress nh table pointer.   */
    uint32 *egr_entry_ptr = NULL; /* Egress next hop entry pinter.  */ 
    int rv = BCM_E_NONE;          /* Operation return status.       */
    int table_ent_size;           /* Table entry size. */
    int idx_min, idx_max, chunk_size, num_chunks, chunk_index;
    int entry_type;

    /* Get next table memory. */
    mem = BCM_XGS3_L3_MEM(unit, nh);
    mem_egr_nh = EGR_L3_NEXT_HOPm;

    /* Table DMA the nhtable to software copy */
    BCM_IF_ERROR_RETURN 
        (bcm_xgs3_l3_tbl_dma(unit, mem, BCM_XGS3_L3_ENT_SZ(unit, nh), "nh_tbl",
                             &ing_tbl_ptr, NULL));

    chunk_size = 1024;
    table_ent_size = BCM_L3_MEM_ENT_SIZE(unit, mem_egr_nh);
    num_chunks = soc_mem_index_count(unit, mem_egr_nh) / chunk_size;
    if (soc_mem_index_count(unit, mem_egr_nh) % chunk_size) {
        num_chunks++;
    }

    egr_tbl_buf = soc_cm_salloc(unit, table_ent_size * chunk_size,
                                       "egr_tbl_chunk buffer");
    if (NULL == egr_tbl_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
        
    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {
        idx_min = chunk_index * chunk_size;
        idx_max = idx_min + chunk_size - 1;
        if (idx_max > soc_mem_index_max(unit, mem_egr_nh)) {
            idx_max = soc_mem_index_max(unit, mem_egr_nh);
        }

        /* Reset allocated buffer. */
        sal_memset(egr_tbl_buf, 0, (table_ent_size * chunk_size));

        rv = soc_mem_read_range(unit, mem_egr_nh, MEM_BLOCK_ANY,
                idx_min, idx_max, egr_tbl_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Iterate over all the entries - show matching ones. */
        for (idx = idx_min; idx <= idx_max; idx++) {
            /* Skip unused entries. */
            if (!BCM_XGS3_L3_ENT_REF_CNT (BCM_XGS3_L3_TBL_PTR(unit,
                                                   next_hop), idx)) {
                continue;
            }

            /* Skip trap to CPU entry internally installed entry. */
            if (BCM_XGS3_L3_L2CPU_NH_IDX(unit) == idx) {
                continue;
            }

            /* Skip the internally installed BLACK_HOLE entry */
            if (BCM_XGS3_L3_BLACK_HOLE_NH_IDX(unit) == idx) {
                continue;
            }

            /* Calculate entry ofset. */
            ing_entry_ptr =
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *, ing_tbl_ptr, idx);

            /*  Read egress next hop entry. */
            egr_entry_ptr = 
                soc_mem_table_idx_to_pointer(unit, mem_egr_nh,
                                     uint32 *, egr_tbl_buf, (idx % chunk_size));
            entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                    egr_entry_ptr, ENTRY_TYPEf);
            if (entry_type == 0 || entry_type == 1 || entry_type == 7 ||
                (entry_type == 4 && soc_feature(unit, soc_feature_wlan))) {
                /* Valid entry types for L3 egress object are L3UC, MPLS,
                 * WLAN, and L3MC.
                 */
            } else {
                continue;
            }

            /* coverity[var_deref_model : FALSE] */
            _bcm_xgs3_nh_entry_parse(unit, ing_entry_ptr, egr_entry_ptr, NULL,  idx,
                                     &nh_entry, NULL);
    
            if (soc_feature(unit, soc_feature_ecmp_dlb)) {
                bcm_tr3_l3_egress_dlb_attr_get(unit, idx, &nh_entry);
            }

            /* Execute operation routine if any. */
            if (trv_data->op_cb) {
                rv = (*trv_data->op_cb)(unit, (void *)trv_data,
                                        (void *)&nh_entry,(void *)&idx, &cmp_result);
                if (rv < 0) {
#ifdef BCM_CB_ABORT_ON_ERR
                    if(SOC_CB_ABORT_ON_ERR(unit)) {
                        break;
                    }
#endif
                }
            }
        }
    }

cleanup:
    if (ing_tbl_ptr) {
        soc_cm_sfree(unit, ing_tbl_ptr);
    }

    if (egr_tbl_buf) {
        soc_cm_sfree(unit, egr_tbl_buf);
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_tr3_l3_tnl_term_entry_init
 * Purpose:
 *      Initialize soc tunnel terminator entry key portion.
 * Parameters:
 *      unit     - (IN)  BCM device number. 
 *      tnl_info - (IN)  BCM buffer with tunnel info.
 *      entry    - (OUT) SOC buffer with key filled in.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_tnl_term_entry_init(int unit, bcm_tunnel_terminator_t *tnl_info,
                                soc_tunnel_term_t *entry)
{
    int       idx;                /* Entry iteration index.     */
    int       idx_max;            /* Entry widht.               */
    uint32    *entry_ptr;         /* Filled entry pointer.      */
    _bcm_tnl_term_type_t tnl_type;/* Tunnel type.               */
    int       rv;                 /* Operation return status.   */

    /* Input parameters check. */
    if ((NULL == tnl_info) || (NULL == entry)) {
        return (BCM_E_PARAM);
    }

    /* Get tunnel type & sub_type. */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_l3_set_tnl_term_type(unit, tnl_info, &tnl_type));

    /* Reset destination structure. */
    sal_memset(entry, 0, sizeof(soc_tunnel_term_t));

    /* Set Destination/Source pair. */
    entry_ptr = (uint32 *)&entry->entry_arr[0];
    if (tnl_type.tnl_outer_hdr_ipv6) {
        /* Apply mask on source address. */
        rv = bcm_xgs3_l3_mask6_apply(tnl_info->sip6_mask, tnl_info->sip6);
        BCM_IF_ERROR_RETURN(rv);

        /* SIP [0-63] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[0], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP [64-127] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[1], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP [0-63] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[2], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP [64-127] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[3], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_UPPER_ONLY);

        /* SIP MASK [0-63] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[0], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP MASK [64-127] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[1], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP MASK [0-63] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[2], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP MASK [64-127] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[3], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_UPPER_ONLY);
    } else {
        tnl_info->sip &= tnl_info->sip_mask;

        /* Set destination ip. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, DIPf, tnl_info->dip);

        /* Set source ip. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, SIPf, tnl_info->sip);

        /* Set destination subnet mask. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, DIP_MASKf,
                                   tnl_info->dip_mask);

        /* Set source subnet mask. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, SIP_MASKf,
                                   tnl_info->sip_mask);
    }

    /* Resolve number of entries hw entry occupies. */
    idx_max = (tnl_type.tnl_outer_hdr_ipv6) ? SOC_TNL_TERM_IPV6_ENTRY_WIDTH : \
              SOC_TNL_TERM_IPV4_ENTRY_WIDTH;  
    
    for (idx = 0; idx < idx_max; idx++) {
        entry_ptr = (uint32 *)&entry->entry_arr[idx];

        /* Set valid bit. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, VALIDf, 1);

        /* Set tunnel subtype. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, SUB_TUNNEL_TYPEf,
                                   tnl_type.tnl_sub_type);

        /* Set tunnel type. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, TUNNEL_TYPEf,
                                   tnl_type.tnl_auto);

        /* Set entry mode. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, KEY_TYPEf,
                               tnl_type.tnl_outer_hdr_ipv6);
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, KEY_TYPE_MASKf, 1);

        if (0 == idx) {
            /* Set the PROTOCOL field */
            if ((tnl_info->type == bcmTunnelTypeIpAnyIn4) || 
                (tnl_info->type == bcmTunnelTypeIpAnyIn6))
            {
              /* Set PROTOCOL and PROTOCOL_MASK field to zero for IpAnyInx tunnel type*/
               soc_L3_TUNNELm_field32_set(unit, entry_ptr, PROTOCOLf, 0x0);
               soc_L3_TUNNELm_field32_set(unit, entry_ptr, PROTOCOL_MASKf, 0x0);
            } else { 
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, PROTOCOLf,
                                           tnl_type.tnl_protocol);
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, 
                                           PROTOCOL_MASKf, 0xff);
            }
        }
        
        if ((tnl_info->type == bcmTunnelTypeWlanWtpToAc) ||
            (tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
            (tnl_info->type == bcmTunnelTypeWlanWtpToAc6) ||
            (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) { 

            /* Set the L4 ports - WLAN/AMT tunnels */
            if (0 == idx) {
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, L4_DEST_PORTf,
                                           tnl_info->udp_dst_port);

                soc_L3_TUNNELm_field32_set(unit, entry_ptr,
                                           L4_DEST_PORT_MASKf, 0xffff);

                soc_L3_TUNNELm_field32_set(unit, entry_ptr, L4_SRC_PORTf,
                                           tnl_info->udp_src_port);
    
                soc_L3_TUNNELm_field32_set(unit, entry_ptr,
                                           L4_SRC_PORT_MASKf, 0xffff);
            }

            /* Set UDP tunnel type. */
            if (soc_mem_field_valid(unit, L3_TUNNELm, UDP_TUNNEL_TYPEf)) {
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, UDP_TUNNEL_TYPEf,
                                           tnl_type.tnl_udp_type);
            }
            /* Ignore UDP checksum */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr,
                                       UDP_CHECKSUM_CONTROLf, 0x0);
        } else if (tnl_info->type == bcmTunnelTypeAutoMulticast) {

            /* Set UDP tunnel type. */
            if (soc_mem_field_valid(unit, L3_TUNNELm, UDP_TUNNEL_TYPEf)) {
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, UDP_TUNNEL_TYPEf,
                                           tnl_type.tnl_udp_type);
            }

            soc_L3_TUNNELm_field32_set(unit, entry_ptr,
                                       UDP_CHECKSUM_CONTROLf, 0x0);
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, CTRL_PKTS_TO_CPUf,
                       0x1);
        } else if (tnl_info->type == bcmTunnelTypeAutoMulticast6) {

            /* Set UDP tunnel type. */
            if (soc_mem_field_valid(unit, L3_TUNNELm, UDP_TUNNEL_TYPEf)) {
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, UDP_TUNNEL_TYPEf,
                                           tnl_type.tnl_udp_type);
            }
            soc_L3_TUNNELm_field32_set(unit, entry_ptr,
                                                UDP_CHECKSUM_CONTROLf, 0x0);
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, CTRL_PKTS_TO_CPUf, 0x1);
        }

        /* Save vlan id for ipmc lookup.*/
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, L3_IIFf, tnl_info->vlan);

        /* Set GRE payload */
        if (tnl_type.tnl_gre) {
            /* GRE IPv6 payload is allowed. */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, GRE_PAYLOAD_IPV6f,
                                       tnl_type.tnl_gre_v6_payload);

            /* GRE IPv6 payload is allowed. */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, GRE_PAYLOAD_IPV4f,
                                       tnl_type.tnl_gre_v4_payload);
        }
    }
    return (BCM_E_NONE);

}

/*
 * Function:
 *      _bcm_tr3_l3_tnl_term_entry_parse
 * Purpose:
 *      Parse tunnel terminator entry portion.
 * Parameters:
 *      unit     - (IN)  BCM device number. 
 *      entry    - (IN)  SOC buffer with tunne information.  
 *      tnl_info - (OUT) BCM buffer with tunnel info.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_tnl_term_entry_parse(int unit, soc_tunnel_term_t *entry,
                                  bcm_tunnel_terminator_t *tnl_info)
{
    _bcm_tnl_term_type_t tnl_type;     /* Tunnel type information.   */
    uint32 *entry_ptr;                 /* Filled entry pointer.      */
    int tunnel_id;                     /* Tunnel ID */
    int remote_port;                   /* Remote port */
    bcm_module_t mod, mod_out, my_mod; /* Module IDs */
    bcm_port_t port, port_out;         /* Physical ports */

    /* Input parameters check. */
    if ((NULL == tnl_info) || (NULL == entry)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination structure. */
    bcm_tunnel_terminator_t_init(tnl_info);
    sal_memset(&tnl_type, 0, sizeof(_bcm_tnl_term_type_t));

    entry_ptr = (uint32 *)&entry->entry_arr[0];

    /* Get valid bit. */
    if (!soc_L3_TUNNELm_field32_get(unit, entry_ptr, VALIDf)) {
        return (BCM_E_NOT_FOUND);
    }

    tnl_type.tnl_outer_hdr_ipv6 =
        (soc_L3_TUNNELm_field32_get(unit, entry_ptr, KEY_TYPEf) == 0x1) ? 1 : 0;
     
    /* Get Destination/Source pair. */
    if (tnl_type.tnl_outer_hdr_ipv6) {
        /* SIP [0-63] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[0], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP [64-127] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[1], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP [0-63] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[2], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP [64-127] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[3], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_UPPER_ONLY);

        /* SIP MASK [0-63] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[0], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP MASK [64-127] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[1], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP MASK [0-63] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[2], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP MASK [64-127] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[3], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_UPPER_ONLY);

    }  else {
        /* Get destination ip. */
        tnl_info->dip = soc_L3_TUNNELm_field32_get(unit, entry_ptr, DIPf);

        /* Get source ip. */
        tnl_info->sip = soc_L3_TUNNELm_field32_get(unit, entry_ptr, SIPf);

        /* Destination subnet mask. */
        tnl_info->dip_mask = BCM_XGS3_L3_IP4_FULL_MASK;

        /* Source subnet mask. */
        tnl_info->sip_mask = soc_L3_TUNNELm_field32_get(unit, entry_ptr, 
                                                        SIP_MASKf);
    }

    /* Get tunnel subtype. */
    tnl_type.tnl_sub_type = 
        soc_L3_TUNNELm_field32_get(unit, entry_ptr, SUB_TUNNEL_TYPEf);

    /* Get UDP tunnel type. */
    if (soc_mem_field_valid(unit, L3_TUNNELm, UDP_TUNNEL_TYPEf)) {
        tnl_type.tnl_udp_type =
            soc_L3_TUNNELm_field32_get(unit, entry_ptr, UDP_TUNNEL_TYPEf);
    }

    /* Get tunnel type. */
    tnl_type.tnl_auto = 
        soc_L3_TUNNELm_field32_get(unit, entry_ptr, TUNNEL_TYPEf);

    /* Copy DSCP from outer header flag. */
    if (soc_L3_TUNNELm_field32_get(unit, entry_ptr, USE_OUTER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
    }
    /* Copy TTL from outer header flag. */
    if (soc_L3_TUNNELm_field32_get(unit, entry_ptr, USE_OUTER_HDR_TTLf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_TTL;
    }
    /* Keep inner header DSCP flag. */
    if (soc_L3_TUNNELm_field32_get(unit, entry_ptr,
                                   DONOT_CHANGE_INNER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
    }

    soc_mem_pbmp_field_get(unit, L3_TUNNELm, entry_ptr, ALLOWED_PORT_BITMAPf,
                           &tnl_info->pbmp);

    /* Tunnel or IPMC lookup vlan id */
    tnl_info->vlan = soc_L3_TUNNELm_field32_get(unit, entry_ptr, IINTFf);

    /*  Get trust dscp per tunnel */ 
    if (soc_L3_TUNNELm_field32_get(unit, entry_ptr, USE_OUTER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_DSCP_TRUST;
    }
    
    /* Get Tunnel class id for VFP match */
    if(SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_CLASS_IDf)) {                 
        tnl_info->tunnel_class = soc_L3_TUNNELm_field32_get(unit, entry_ptr, 
                                    TUNNEL_CLASS_IDf);
    }

    /* Get the protocol field and make some decisions */
    tnl_type.tnl_protocol = soc_L3_TUNNELm_field32_get(unit, entry_ptr, 
                                                       PROTOCOLf);
    switch (tnl_type.tnl_protocol) {
        case 0x2F:
            tnl_type.tnl_gre = 1;
            break;
        case 0x67:
            tnl_type.tnl_pim_sm = 1;
        default:
            break;
    }
    /* Get gre IPv4 payload allowed. */
    tnl_type.tnl_gre_v4_payload = 
        soc_L3_TUNNELm_field32_get(unit, entry_ptr, GRE_PAYLOAD_IPV4f); 

    /* Get gre IPv6 payload allowed. */
    tnl_type.tnl_gre_v6_payload = 
        soc_L3_TUNNELm_field32_get(unit, entry_ptr, GRE_PAYLOAD_IPV6f);

    /* Get the L4 data */
    if (soc_mem_field_valid (unit, L3_TUNNELm, L4_SRC_PORTf)) {
        tnl_info->udp_src_port = soc_L3_TUNNELm_field32_get(unit, entry_ptr, 
                                                        L4_SRC_PORTf);
    }
    if (soc_mem_field_valid (unit, L3_TUNNELm, L4_DEST_PORTf)) {
        tnl_info->udp_dst_port = soc_L3_TUNNELm_field32_get(unit, entry_ptr, 
                                                        L4_DEST_PORTf);
    }
    /* Get the tunnel ID */
    if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_IDf)) {
        tunnel_id = soc_L3_TUNNELm_field32_get(unit, entry_ptr, TUNNEL_IDf);
        if (tunnel_id) {
            BCM_GPORT_TUNNEL_ID_SET(tnl_info->tunnel_id, tunnel_id);
            tnl_info->flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
        }
    }

    /* Get tunnel type, sub_type and protocol. */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_l3_get_tnl_term_type(unit, tnl_info, &tnl_type));
    /* Get the remote port member */
    if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, REMOTE_TERM_GPPf)) {
        remote_port = soc_L3_TUNNELm_field32_get(unit, entry_ptr,
                                             REMOTE_TERM_GPPf);
        mod = (remote_port >> 6) & 0x7F;
        port = remote_port & 0x3F;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod, port, &mod_out, &port_out));
        BCM_GPORT_MODPORT_SET(tnl_info->remote_port, mod_out, port_out);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_mod));
        if (mod != my_mod) {
            if ((tnl_info->type == bcmTunnelTypeWlanWtpToAc) ||
                (tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
                (tnl_info->type == bcmTunnelTypeWlanWtpToAc6) ||
                (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) {
                tnl_info->flags |= BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE;
            }
        }
    }
    if ((tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
        (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) {
        if (_bcm_tr3_wlan_tunnel_get_roam(unit, tnl_info->tunnel_id)) {
            tnl_info->flags |= BCM_TUNNEL_TERM_WLAN_SET_ROAM;
        }
     }
    /* Get IIF if l3 ingress mode is not set */
    if (!BCM_XGS3_L3_INGRESS_MODE_ISSET(unit) &&
        BCM_XGS3_L3_ING_IF_TBL_SIZE(unit) && (tnl_info->vlan > BCM_VLAN_MAX)) {
        _bcm_l3_ingress_intf_t iif;
        sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
        iif.intf_id = tnl_info->vlan;
        BCM_IF_ERROR_RETURN(_bcm_tr_l3_ingress_interface_get(unit, NULL, &iif));

        tnl_info->vrf = iif.vrf;
        tnl_info->if_class = iif.if_class;
        tnl_info->qos_map_id = iif.qos_map_id;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr3_l3_ipmc_get
 * Purpose:
 *      Get TR3 L3 multicast entry.
 * Parameters:
 *      unit  - (IN)SOC unit number.
 *      l3cfg - (IN/OUT)Group/Source key & Get result buffer.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_tr3_l3_ipmc_get(int unit, _bcm_l3_cfg_t *l3cfg)
{
    l3_entry_4_entry_t l3x_key;    /* Lookup key buffer.    */
    l3_entry_4_entry_t l3x_entry;  /* Search result buffer. */
    int clear_hit;                              /* Clear hit flag.       */
    soc_mem_t mem;                              /* IPMC table memory.    */
    int ipv6;                                   /* IPv6 entry indicator. */
    int rv;                                     /* Return value.         */
    int l3_entry_idx;                           /* Entry index           */

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    mem =  (ipv6) ? L3_ENTRY_4m : L3_ENTRY_2m;

    /*  Zero buffers. */
    sal_memcpy(&l3x_key, soc_mem_entry_null(unit, mem), 
                   soc_mem_entry_words(unit,mem) * 4);
    sal_memcpy(&l3x_entry, soc_mem_entry_null(unit, mem), 
                   soc_mem_entry_words(unit,mem) * 4);

    /* Check if clear hit bit is required. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    /* Lookup Key preparation. */
    _bcm_tr3_l3_ipmc_ent_init(unit, (uint32 *)&l3x_key, l3cfg);

    /* Perform hw lookup. */
    MEM_LOCK(unit, mem);
    rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY,
                                _BCM_TR3_L3_MEM_BANKS_ALL,
                                &l3x_key, &l3x_entry, &l3_entry_idx);
    l3cfg->l3c_hw_index = l3_entry_idx;

    MEM_UNLOCK(unit, mem);
    BCM_XGS3_LKUP_IF_ERROR_RETURN(rv, BCM_E_NOT_FOUND);

    /* Extract buffer information. */
    _bcm_tr3_l3_ipmc_ent_parse(unit, l3cfg, &l3x_entry);

    /* Clear the HIT bit */
    if (clear_hit) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l3_clear_hit(unit, mem, l3cfg, &l3x_entry,
                                                  l3_entry_idx));
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_ipmc_get_by_idx
 * Purpose:
 *      Get l3 multicast entry by entry index.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      dma_ptr  - (IN)Table pointer in dma. 
 *      idx       - (IN)Index to read. 
 *      l3cfg     - (IN/OUT)Entry data.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_tr3_l3_ipmc_get_by_idx(int unit, void *dma_ptr,
                           int idx, _bcm_l3_cfg_t *l3cfg)
{
    l3_entry_4_entry_t l3_entry_v6;    /* Read buffer.            */
    l3_entry_2_entry_t l3_entry_v4;    /* Read buffer.            */
    void *buf_p;                      /* Hardware buffer ptr     */
    soc_mem_t mem;                     /* IPMC table memory.      */
    uint32 ipv6;                       /* IPv6 entry indicator.   */
    int clear_hit;                     /* Clear hit bit indicator.*/
    int key_type;
    soc_field_t l3iif;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    if (ipv6) {
        mem   = L3_ENTRY_4m;
        buf_p = (l3_entry_4_entry_t *)&l3_entry_v6;
        l3iif = IPV6MC__L3_IIFf;
    } else {
        mem   = L3_ENTRY_2m;
        buf_p = (l3_entry_2_entry_t *)&l3_entry_v4;
        l3iif = IPV4MC__L3_IIFf;
    }

    /* Check if clear hit is required. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    if (NULL == dma_ptr) {             /* Read from hardware. */
        /* Zero buffers. */
        sal_memcpy(buf_p, soc_mem_entry_null(unit, mem), 
                     soc_mem_entry_words(unit,mem) * 4);

        /* Read entry from hw. */
        BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, buf_p));
    } else {                    /* Read from dma. */
        if (ipv6) {
            buf_p = soc_mem_table_idx_to_pointer(unit, mem,
                     l3_entry_4_entry_t *, dma_ptr, idx);
        } else {
            buf_p = soc_mem_table_idx_to_pointer(unit, mem,
                     l3_entry_2_entry_t *, dma_ptr, idx);
        }
    }

    /* Ignore invalid entries. */
    if (!soc_mem_field32_get(unit, mem, buf_p, VALID_0f)) {
        return (BCM_E_NOT_FOUND);
    }

    key_type = soc_mem_field32_get(unit, mem, buf_p, KEY_TYPE_0f);

    switch (key_type) {
      case SOC_MEM_KEY_L3_ENTRY_2_IPV4MC_IPV4_MULTICAST:
          l3cfg->l3c_flags = BCM_L3_IPMC;
          break;
      case SOC_MEM_KEY_L3_ENTRY_2_IPV6UC_IPV6_UNICAST:
      case SOC_MEM_KEY_L3_ENTRY_4_IPV6UC_IPV6_UNICAST:
          l3cfg->l3c_flags = BCM_L3_IP6;
          break;
      case SOC_MEM_KEY_L3_ENTRY_4_IPV6MC_IPV6_MULTICAST:
          l3cfg->l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC;
          break;
      default:
          /* Catch all other key types, set flags to zero so that the 
           * following flag check is effective for all key types */
          l3cfg->l3c_flags = 0;  
          break;
    }

    /* Ignore protocol mismatch & multicast entries. */
    if ((ipv6  != (l3cfg->l3c_flags & BCM_L3_IP6)) ||
        (!(l3cfg->l3c_flags & BCM_L3_IPMC))) {
        return (BCM_E_NOT_FOUND);
    }

    /* Set index to l3cfg. */
    l3cfg->l3c_hw_index = idx;

    if (ipv6) {
        /* Get group address. */
        soc_mem_ip6_addr_get(unit, mem, buf_p, IPV6MC__GROUP_IP_ADDR_LWR_96f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_96BIT);

        soc_mem_ip6_addr_get(unit, mem, buf_p, IPV6MC__GROUP_IP_ADDR_UPR_24f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_BITS_119_96);

        /* Get source  address. */
        soc_mem_ip6_addr_get(unit, mem, buf_p, IPV6MC__SOURCE_IP_ADDR_LWR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_LOWER_ONLY);
        soc_mem_ip6_addr_get(unit, mem, buf_p, IPV6MC__SOURCE_IP_ADDR_UPR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_UPPER_ONLY);

        l3cfg->l3c_ip6[0] = 0xff;    /* Set entry to multicast*/
    } else {
        /* Get group id. */
        l3cfg->l3c_ip_addr =
            soc_mem_field32_get(unit, mem, buf_p, IPV4MC__GROUP_IP_ADDRf);

        /* Get source address. */
        l3cfg->l3c_src_ip_addr =
            soc_mem_field32_get(unit, mem,  buf_p, IPV4MC__SOURCE_IP_ADDRf);
    }

    /* Get L3_IIF or vlan id. */
    l3cfg->l3c_vid = soc_mem_field32_get(unit, mem, buf_p, l3iif);

    /* Parse entry data. */
    _bcm_tr3_l3_ipmc_ent_parse(unit, l3cfg, buf_p);

    /* Clear the HIT bit */
    if (clear_hit) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l3_clear_hit(unit, mem, l3cfg, buf_p,
                                                     l3cfg->l3c_hw_index));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr3_l3_ipmc_add
 * Purpose:
 *      Add TR3 L3 multicast entry.
 * Parameters:
 *      unit  - (IN)SOC unit number.
 *      l3cfg - (IN/OUT)Group/Source key.
 * Returns:
 *    BCM_E_XXX
 */
int
 _bcm_tr3_l3_ipmc_add(int unit, _bcm_l3_cfg_t *l3cfg)
{
    l3_entry_4_entry_t l3x_entry;  /* Write entry buffer.   */
    soc_mem_t mem;                              /* IPMC table memory.    */
    uint32 *buf_p;                              /* HW buffer address.    */
    int ipv6;                                   /* IPv6 entry indicator. */
    int idx;                                    /* Iteration index.      */
    int rv;                                     /* Return value.         */
    soc_field_t hitf[] = { HIT_0f, HIT_1f, HIT_2f};
    soc_field_t rpe, dst_discard, vrf_id, l3mc_idx, class_id, pri;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory and table fields */
    if (ipv6) {
        mem         = L3_ENTRY_4m;
        pri         = IPV6MC__PRIf;
        rpe         = IPV6MC__RPEf;
        vrf_id      = IPV6MC__VRF_IDf;
        l3mc_idx    = IPV6MC__L3MC_INDEXf;
        class_id    = IPV6MC__CLASS_IDf;
        dst_discard = IPV6MC__DST_DISCARDf;
    } else {
        mem         = L3_ENTRY_2m;
        pri         = IPV4MC__PRIf;
        rpe         = IPV4MC__RPEf;
        vrf_id      = IPV4MC__VRF_IDf;
        l3mc_idx    = IPV4MC__L3MC_INDEXf;
        class_id    = IPV4MC__CLASS_IDf;
        dst_discard = IPV4MC__DST_DISCARDf;
    }

    /*  Zero buffers. */
    buf_p = (uint32 *)&l3x_entry; 
    sal_memcpy(buf_p, soc_mem_entry_null(unit, mem), 
                   soc_mem_entry_words(unit,mem) * 4);

   /* Prepare entry to write. */
    _bcm_tr3_l3_ipmc_ent_init(unit, (uint32 *)&l3x_entry, l3cfg);

    /* Set priority override bit. */
    if (l3cfg->l3c_flags & BCM_L3_RPE) {
        soc_mem_field32_set(unit, mem, buf_p, rpe, 1);
    }

    /* Set destination discard. */
    if (l3cfg->l3c_flags & BCM_L3_DST_DISCARD) {
        soc_mem_field32_set(unit, mem, buf_p, dst_discard, 1);
    }
	
    /* Virtual router id. */
    soc_mem_field32_set(unit, mem, buf_p, vrf_id, l3cfg->l3c_vrf);

    /* Set priority. */
    soc_mem_field32_set(unit, mem, buf_p, pri, l3cfg->l3c_prio);

    /* Pointer to ipmc table. */
    soc_mem_field32_set(unit, mem, buf_p, l3mc_idx, l3cfg->l3c_ipmc_ptr);

    /* Classification lookup class id. */
    soc_mem_field32_set(unit, mem, buf_p, class_id, l3cfg->l3c_lookup_class);

    for (idx = 0; idx < COUNTOF(hitf); idx++) {
        /* Set hit bit. */
        if (l3cfg->l3c_flags & BCM_L3_HIT) {
            soc_mem_field32_set(unit, mem, buf_p, hitf[idx], 1);
        }
    }

    /* Write entry to the hw. */
    MEM_LOCK(unit, mem);
    /* Handle replacement. */
    if (BCM_XGS3_L3_INVALID_INDEX != l3cfg->l3c_hw_index) {
        rv = BCM_XGS3_MEM_WRITE(unit, mem, l3cfg->l3c_hw_index, buf_p);
    } else {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, (void *)buf_p);
    }

    /* Increment number of ipmc routes. */
    if (BCM_SUCCESS(rv) && (BCM_XGS3_L3_INVALID_INDEX == l3cfg->l3c_hw_index)) {
        (ipv6) ? BCM_XGS3_L3_IP6_IPMC_CNT(unit)++ : \
                 BCM_XGS3_L3_IP4_IPMC_CNT(unit)++;
    }
    /*    coverity[overrun-local : FALSE]    */
    MEM_UNLOCK(unit, mem);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_ipmc_del
 * Purpose:
 *      Delete TR3 L3 multicast entry.
 * Parameters:
 *      unit  - (IN)SOC unit number.
 *      l3cfg - (IN)Group/Source deletion key.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_tr3_l3_ipmc_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    l3_entry_4_entry_t l3x_entry;  /* Delete buffer.          */
    soc_mem_t mem;                              /* IPMC table memory.      */
    int ipv6;                                   /* IPv6 entry indicator.   */
    int rv;                                     /* Operation return value. */

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    mem =  (ipv6) ? L3_ENTRY_4m : L3_ENTRY_2m;

    /*  Zero entry buffer. */
    sal_memcpy(&l3x_entry, soc_mem_entry_null(unit, mem), 
               soc_mem_entry_words(unit,mem) * 4);


    /* Key preparation. */
    _bcm_tr3_l3_ipmc_ent_init(unit, (uint32 *)&l3x_entry, l3cfg);

    /* Delete the entry from hw. */
    MEM_LOCK(unit, mem);

    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void *)&l3x_entry);

    /* Decrement number of ipmc routes. */
    if (BCM_SUCCESS(rv)) {
        (ipv6) ? BCM_XGS3_L3_IP6_IPMC_CNT(unit)-- : \
            BCM_XGS3_L3_IP4_IPMC_CNT(unit)--;
    }
    MEM_UNLOCK(unit, mem);
    return rv;
}

/*----- ECMP DLB FUNCS ----- */

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_member_id_alloc
 * Purpose:
 *      Get a free ECMP DLB Member ID and mark it used.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      member_id - (OUT) Allocated Member ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_member_id_alloc(int unit, int *member_id)
{
    int vla;
    int i;
    int max_members;

    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                        bcmSwitchEcmpDynamicAccountingSelect, &vla));
    if (vla) {
        max_members = soc_mem_index_count(unit, DLB_ECMP_MEMBER_ATTRIBUTEm);
    } else {
        /* In physical link accounting, the number of members is limited to
         * the number of physical ports.
         */
        max_members = soc_reg_field_length(unit, DLB_ECMP_MEMBER_HW_STATE_64r,
                BITMAPf);
    }

    for (i = 0; i < max_members; i++) {
        if (!_BCM_ECMP_DLB_MEMBER_ID_USED_GET(unit, i)) {
            _BCM_ECMP_DLB_MEMBER_ID_USED_SET(unit, i);
            *member_id = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_member_id_free
 * Purpose:
 *      Free an ECMP DLB Member ID.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      member_id - (IN) Member ID to be freed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_member_id_free(int unit, int member_id)
{
    if (member_id < 0 || member_id > soc_mem_index_max(unit,
                DLB_ECMP_MEMBER_ATTRIBUTEm)) {
        return BCM_E_PARAM;
    }

    _BCM_ECMP_DLB_MEMBER_ID_USED_CLR(unit, member_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_quality_assign
 * Purpose:
 *      Assign quality based on loading and queue size.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 *      tx_load_percent - (IN) Percent weight of loading in determing quality.
 *                             The remaining percentage is the weight of queue
 *                             size.
 *      entry_arr - (IN) Array of 64 quality map table entries.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int 
_bcm_tr3_ecmp_dlb_quality_assign(int unit, int tx_load_percent,
        uint32 *entry_arr)
{
    int quantized_tx_load;
    int quantized_qsize;
    int quality;
    int entry_index;

    if (entry_arr == NULL) {
        return BCM_E_PARAM;
    }

    for (quantized_tx_load = 0; quantized_tx_load < 8; quantized_tx_load++) {
        for (quantized_qsize = 0; quantized_qsize < 8; quantized_qsize++) {
            quality = (quantized_tx_load * tx_load_percent +
                    quantized_qsize * (100 - tx_load_percent)) / 100;
            entry_index = (quantized_tx_load << 3) + quantized_qsize;
            soc_DLB_ECMP_QUALITY_MAPPINGm_field32_set(unit,
                    &entry_arr[entry_index], QUALITYf, quality);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_member_quality_map_set
 * Purpose:
 *      Set per-member ECMP DLB quality mapping table.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      member_id - (IN) Member ID.
 *      tx_load_percent - (IN) Percent weight of loading in determing quality.
 *                             The remaining percentage is the weight of queue
 *                             size.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_member_quality_map_set(int unit, int member_id,
        int tx_load_percent)
{
    soc_profile_mem_t *profile;
    soc_mem_t mem;
    int entry_bytes;
    int entries_per_profile;
    int alloc_size;
    uint32 *entry_arr;
    int rv = BCM_E_NONE;
    void *entries;
    uint32 base_index;
    uint32 old_base_index;
    dlb_ecmp_quality_control_entry_t quality_control_entry;

    profile = ECMP_DLB_INFO(unit)->ecmp_dlb_quality_map_profile;

    entries_per_profile = 64;
    entry_bytes = sizeof(dlb_ecmp_quality_mapping_entry_t);
    alloc_size = entries_per_profile * entry_bytes;
    entry_arr = sal_alloc(alloc_size, "ECMP DLB Quality Map entries");
    if (entry_arr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry_arr, 0, alloc_size);

    /* Assign quality in the entry array */
    rv = _bcm_tr3_ecmp_dlb_quality_assign(unit, tx_load_percent, entry_arr);
    if (BCM_FAILURE(rv)) {
        sal_free(entry_arr);
        return rv;
    }

    mem = DLB_ECMP_QUALITY_MAPPINGm;
    soc_mem_lock(unit, mem);

    /* Add profile */
    entries = entry_arr;
    rv = soc_profile_mem_add(unit, profile, &entries, entries_per_profile,
            &base_index);
    sal_free(entry_arr);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Update member profile pointer */
    rv = READ_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY, member_id,
            &quality_control_entry);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }
    old_base_index = entries_per_profile * soc_mem_field32_get(unit,
            DLB_ECMP_QUALITY_CONTROLm, &quality_control_entry,
            PROFILE_PTRf);
    soc_DLB_ECMP_QUALITY_CONTROLm_field32_set(unit, &quality_control_entry, 
            PROFILE_PTRf, base_index / entries_per_profile);
    rv = WRITE_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ALL, member_id,
            &quality_control_entry);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Delete old profile */
    rv = soc_profile_mem_delete(unit, profile, old_base_index);
    soc_mem_unlock(unit, mem);

    ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[base_index/entries_per_profile] =
        tx_load_percent;

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_member_attr_set
 * Purpose:
 *      Set ECMP dynamic load balancing attributes for a member.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      member_id - (IN) Member ID.
 *      nh_index  - (IN) Next hop index.
 *      scaling_factor - (IN) Threshold scaling factor.
 *      load_weight    - (IN) Weighting of load in determining member quality.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_ecmp_dlb_member_attr_set(int unit, int member_id, int nh_index,
        int scaling_factor, int load_weight)
{
    dlb_ecmp_quality_control_entry_t dlb_ecmp_quality_control_entry;
    dlb_ecmp_member_attribute_entry_t dlb_ecmp_member_attr_entry;

    /* Set load weight */
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_member_quality_map_set(unit,
                    member_id, load_weight));

    /* Set scaling factor */
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                member_id, &dlb_ecmp_quality_control_entry));
    soc_DLB_ECMP_QUALITY_CONTROLm_field32_set(unit,
            &dlb_ecmp_quality_control_entry, LOADING_SCALING_FACTORf,
            scaling_factor);
    soc_DLB_ECMP_QUALITY_CONTROLm_field32_set(unit,
            &dlb_ecmp_quality_control_entry, QSIZE_SCALING_FACTORf,
            scaling_factor);
    SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ALL,
                member_id, &dlb_ecmp_quality_control_entry));

    /* Set next hop index */
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_MEMBER_ATTRIBUTEm(unit, MEM_BLOCK_ANY,
                member_id, &dlb_ecmp_member_attr_entry));
    soc_DLB_ECMP_MEMBER_ATTRIBUTEm_field32_set(unit,
            &dlb_ecmp_member_attr_entry, NEXT_HOP_INDEXf, nh_index);
    SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_MEMBER_ATTRIBUTEm(unit, MEM_BLOCK_ALL,
                member_id, &dlb_ecmp_member_attr_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_member_attr_get
 * Purpose:
 *      Get ECMP dynamic load balancing attributes for a member.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      member_id - (IN) Member ID.
 *      scaling_factor - (OUT) Threshold scaling factor.
 *      load_weight    - (OUT) Weighting of load in determining member quality.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_ecmp_dlb_member_attr_get(int unit, int member_id, int *scaling_factor,
        int *load_weight)
{
    dlb_ecmp_quality_control_entry_t dlb_ecmp_quality_control_entry;
    int profile_ptr;

    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                member_id, &dlb_ecmp_quality_control_entry));

    /* Get load weight */
    profile_ptr = soc_DLB_ECMP_QUALITY_CONTROLm_field32_get(unit,
            &dlb_ecmp_quality_control_entry, PROFILE_PTRf);
    *load_weight = ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[profile_ptr];

    /* Get scaling factor */
    *scaling_factor = soc_DLB_ECMP_QUALITY_CONTROLm_field32_get(unit,
            &dlb_ecmp_quality_control_entry, LOADING_SCALING_FACTORf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l3_egress_dlb_attr_set
 * Purpose:
 *      Set ECMP dynamic load balancing attributes for a next hop index.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      nh_index - (IN) Next hop index.
 *      egr      - (IN) L3 egress info.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l3_egress_dlb_attr_set(int unit, int nh_index, bcm_l3_egress_t *egr)
{
    int rv = BCM_E_NONE;
    int match, match_index;
    int free_index_found, free_index;
    int i;
    _tr3_ecmp_dlb_nh_membership_t *membership, *current_ptr, *next_ptr;
    int scaling_factor_invalid, load_weight_invalid;

    if (egr->dynamic_scaling_factor ==
             BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID) {
       scaling_factor_invalid = TRUE;
    } else {
       scaling_factor_invalid = FALSE;
    }
    if (egr->dynamic_load_weight ==
             BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID) {
        load_weight_invalid = TRUE;
    } else {
        load_weight_invalid = FALSE;
    }
    if (scaling_factor_invalid != load_weight_invalid) {
        /* The scaling factor and the load weight must be both invalid,
         * or both valid.
         */
        return BCM_E_PARAM;
    }

    /* Traverse ecmp_dlb_nh_info array to find the given nh_index */
    match = FALSE;
    match_index = 0;
    free_index_found = FALSE;
    free_index = 0;
    for (i = 0; i < ECMP_DLB_INFO(unit)->ecmp_dlb_nh_info_size; i++) {
        if (ECMP_DLB_NH_INFO(unit, i).valid) {
            if (nh_index == ECMP_DLB_NH_INFO(unit, i).nh_index) {
                match = TRUE;
                match_index = i;
            }
        } else {
            if (!free_index_found) {
                free_index_found = TRUE;
                free_index = i;
            }
        }
    }

    if (match) {

        if ((egr->dynamic_scaling_factor ==
                BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID) &&
            (egr->dynamic_load_weight ==
                BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID)) {
            /* Check that the given next hop index is currently not a member of
             * any ECMP DLB group.
             */
            current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
            while (NULL != current_ptr) {
                if (current_ptr->group != -1) {
                    return BCM_E_BUSY;
                }
                current_ptr = current_ptr->next;
            }

            /* Reset the DLB attributes, free member ID, free linked list */
            current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
            while (NULL != current_ptr) {
                BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_member_attr_set(unit,
                            current_ptr->member_id, 0, 0, 100));
                BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_member_id_free(unit,
                            current_ptr->member_id));
                next_ptr = current_ptr->next;
                sal_free(current_ptr);
                current_ptr = next_ptr;
            }
            ECMP_DLB_NH_INFO(unit, free_index).nh_membership_list = NULL;
            ECMP_DLB_NH_INFO(unit, free_index).nh_index = 0;
            ECMP_DLB_NH_INFO(unit, free_index).valid = 0;

        } else {
            /* Traverse next hop index's linked list of DLB member IDs and
             * update their DLB attributes.
             */
            current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
            while (NULL != current_ptr) {
                BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_member_attr_set(unit,
                            current_ptr->member_id, nh_index,
                            egr->dynamic_scaling_factor,
                            egr->dynamic_load_weight));
                current_ptr = current_ptr->next;
            }
        }

    } else { /* No match */

        if ((egr->dynamic_scaling_factor ==
                 BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID) &&
            (egr->dynamic_load_weight ==
                 BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID)) {
            /* Nothing to do */
            return BCM_E_NONE;
        }

        if (!free_index_found) {
            /* ecmp_dlb_nh_info array is full */
            return BCM_E_FULL;
        }

        /* Allocate a DLB member ID and set DLB attributes */
        membership = sal_alloc(sizeof(_tr3_ecmp_dlb_nh_membership_t),
                "ecmp dlb nh membership");
        if (NULL == membership) {
            return BCM_E_MEMORY;
        }
        rv = _bcm_tr3_ecmp_dlb_member_id_alloc(unit, &membership->member_id);
        if (BCM_FAILURE(rv)) {
            sal_free(membership);
            return rv;
        }
        rv = _bcm_tr3_ecmp_dlb_member_attr_set(unit, membership->member_id,
                nh_index, egr->dynamic_scaling_factor,
                egr->dynamic_load_weight);
        if (BCM_FAILURE(rv)) {
            sal_free(membership);
            return rv;
        }
        membership->group = -1;
        membership->next =
            ECMP_DLB_NH_INFO(unit, free_index).nh_membership_list; 
        ECMP_DLB_NH_INFO(unit, free_index).nh_membership_list = membership;
        ECMP_DLB_NH_INFO(unit, free_index).nh_index = nh_index;
        ECMP_DLB_NH_INFO(unit, free_index).valid = 1;
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l3_egress_dlb_attr_destroy
 * Purpose:
 *      Destroy ECMP dynamic load balancing attributes for a next hop index.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      nh_index - (IN) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l3_egress_dlb_attr_destroy(int unit, int nh_index)
{
    int rv = BCM_E_NONE;
    int match, match_index;
    int i;
    _tr3_ecmp_dlb_nh_membership_t *current_ptr, *next_ptr;

    /* Traverse ecmp_dlb_nh_info array to find the given nh_index */
    match = FALSE;
    match_index = 0;
    for (i = 0; i < ECMP_DLB_INFO(unit)->ecmp_dlb_nh_info_size; i++) {
        if (ECMP_DLB_NH_INFO(unit, i).valid) {
            if (nh_index == ECMP_DLB_NH_INFO(unit, i).nh_index) {
                match = TRUE;
                match_index = i;
                break;
            }
        }
    }

    if (match) {
        /* Check that the given next hop index is currently not a member of
         * any ECMP DLB group.
         */
        current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
        while (NULL != current_ptr) {
            if (current_ptr->group != -1) {
                return BCM_E_BUSY;
            }
            current_ptr = current_ptr->next;
        }

        /* Reset the DLB attributes, free member ID, free linked list */
        current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
        while (NULL != current_ptr) {
            BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_member_attr_set(unit,
                        current_ptr->member_id, 0, 0, 100));
            BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_member_id_free(unit,
                        current_ptr->member_id));
            next_ptr = current_ptr->next;
            sal_free(current_ptr);
            current_ptr = next_ptr;
        }
        ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list = NULL;
        ECMP_DLB_NH_INFO(unit, match_index).nh_index = 0;
        ECMP_DLB_NH_INFO(unit, match_index).valid = 0;
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l3_egress_dlb_attr_get
 * Purpose:
 *      Get ECMP dynamic load balancing attributes for a next hop index.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      nh_index - (IN) Next hop index.
 *      egr      - (OUT) L3 egress info.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_l3_egress_dlb_attr_get(int unit, int nh_index, bcm_l3_egress_t *egr)
{
    int rv = BCM_E_NONE;
    int match, match_index;
    int i;
    _tr3_ecmp_dlb_nh_membership_t *current_ptr;

    /* Traverse ecmp_dlb_nh_info array to find the given nh_index */
    match = FALSE;
    match_index = 0;
    for (i = 0; i < ECMP_DLB_INFO(unit)->ecmp_dlb_nh_info_size; i++) {
        if (ECMP_DLB_NH_INFO(unit, i).valid) {
            if (nh_index == ECMP_DLB_NH_INFO(unit, i).nh_index) {
                match = TRUE;
                match_index = i;
                break;
            }
        }
    }

    if (match) {
        /* Get the DLB attributes */
        current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
        BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_member_attr_get(unit,
                    current_ptr->member_id, &egr->dynamic_scaling_factor,
                    &egr->dynamic_load_weight));
    } else {
        egr->dynamic_scaling_factor = BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID;
        egr->dynamic_load_weight = BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_id_alloc
 * Purpose:
 *      Get a free ECMP DLB ID and mark it used.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      dlb_id - (OUT) Allocated DLB ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_id_alloc(int unit, int *dlb_id)
{
    int i;

    for (i = 0; i < soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm); i++) {
        if (!_BCM_ECMP_DLB_ID_USED_GET(unit, i)) {
            _BCM_ECMP_DLB_ID_USED_SET(unit, i);
            *dlb_id = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_id_free
 * Purpose:
 *      Free an ECMP DLB ID.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      dlb_id - (IN) DLB ID to be freed.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_id_free(int unit, int dlb_id)
{
    if (dlb_id < 0 ||
            dlb_id > soc_mem_index_max(unit, DLB_ECMP_GROUP_CONTROLm)) {
        return BCM_E_PARAM;
    }

    _BCM_ECMP_DLB_ID_USED_CLR(unit, dlb_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_dynamic_size_encode
 * Purpose:
 *      Encode ECMP dynamic load balancing flow set size.
 * Parameters:
 *      dynamic_size - (IN) Number of flow sets.
 *      encoded_value - (OUT) Encoded value.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_dynamic_size_encode(int dynamic_size,
        int *encoded_value)
{   
    switch (dynamic_size) {
        case 512:
            *encoded_value = 1;
            break;
        case 1024:
            *encoded_value = 2;
            break;
        case 2048:
            *encoded_value = 3;
            break;
        case 4096:
            *encoded_value = 4;
            break;
        case 8192:
            *encoded_value = 5;
            break;
        case 16384:
            *encoded_value = 6;
            break;
        case 32768:
            *encoded_value = 7;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_dynamic_size_decode
 * Purpose:
 *      Decode ECMP dynamic load balancing flow set size.
 * Parameters:
 *      encoded_value - (IN) Encoded value.
 *      dynamic_size - (OUT) Number of flow sets.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_dynamic_size_decode(int encoded_value,
        int *dynamic_size)
{   
    switch (encoded_value) {
        case 1:
            *dynamic_size = 512;
            break;
        case 2:
            *dynamic_size = 1024;
            break;
        case 3:
            *dynamic_size = 2048;
            break;
        case 4:
            *dynamic_size = 4096;
            break;
        case 5:
            *dynamic_size = 8192;
            break;
        case 6:
            *dynamic_size = 16384;
            break;
        case 7:
            *dynamic_size = 32768;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_nh_membership_free_resource
 * Purpose:
 *      Free resources in ECMP DLB next hop membership.
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      nh_index  - (IN) Next hop index. 
 *      member_id - (IN) ECMP DLB member ID. 
 *      dlb_id    - (IN) ECMP DLB group ID. 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_nh_membership_free_resource(int unit, int nh_index,
        int member_id, int dlb_id)
{
    int match, match_index;
    int i;
    int member_id_count;
    _tr3_ecmp_dlb_nh_membership_t *current_ptr, *prev_ptr;
    int member_match;

    /* Traverse ecmp_dlb_nh_info array to find the given nh_index */
    match = FALSE;
    match_index = 0;
    for (i = 0; i < ECMP_DLB_INFO(unit)->ecmp_dlb_nh_info_size; i++) {
        if (ECMP_DLB_NH_INFO(unit, i).valid) {
            if (nh_index == ECMP_DLB_NH_INFO(unit, i).nh_index) {
                match = TRUE;
                match_index = i;
                break;
            }
        }
    }
    if (!match) {
        return BCM_E_INTERNAL;
    }

    /* Count the number of member IDs assigned to this next hop */
    member_id_count = 0;
    current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
    while (NULL != current_ptr) {
        member_id_count++;
        current_ptr = current_ptr->next;
    }

    if (0 == member_id_count) {
        return BCM_E_INTERNAL;
    }

    /* If there is only one member ID assigned to the next hop index,
     * do not free the member ID, merely invalidate the group.
     */
    if (1 == member_id_count) {
        current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
        if ((current_ptr->member_id != member_id) ||
            (current_ptr->group != dlb_id)) {
           return BCM_E_INTERNAL;
        } 
        current_ptr->group = -1;
        return BCM_E_NONE;
    }

    /* If there are more than 1 member IDs assigned to the next hop index,
     * free the given member ID.
     */
    prev_ptr = NULL;
    current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
    member_match = FALSE;
    while (NULL != current_ptr) {
        if ((current_ptr->member_id == member_id) &&
                (current_ptr->group == dlb_id)) {
            if (NULL == prev_ptr) {
                ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list =
                    current_ptr->next;
            } else {
                prev_ptr->next = current_ptr->next;
            }
            sal_free(current_ptr);

            /* Reset member DLB attributes */
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_ecmp_dlb_member_attr_set(unit, member_id, 0, 0, 100));

            /* Free member ID */
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_ecmp_dlb_member_id_free(unit, member_id));

            member_match = TRUE;
            break;
        }
        prev_ptr = current_ptr;
        current_ptr = current_ptr->next;
    }
    if (!member_match) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_free_resource
 * Purpose:
 *      Free resources for an ECMP dynamic load balancing group.
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      ecmp_group_idx - (IN) ECMP group index.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_free_resource(int unit, int ecmp_group_idx)
{
    int rv = BCM_E_NONE;
    ecmp_count_entry_t ecmp_group_entry;
    int dlb_enable;
    int dlb_id;
    dlb_ecmp_group_control_entry_t dlb_ecmp_group_control_entry;
    int entry_base_ptr;
    int flow_set_size;
    int num_entries;
    int block_base_ptr;
    int num_blocks;
    int vla;
    dlb_ecmp_group_membership_entry_t dlb_ecmp_group_membership_entry;
    int member_bitmap_width;
    int alloc_size;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    int i;
    int reverse_map_index;
    dlb_ecmp_member_attribute_entry_t dlb_ecmp_member_attribute_entry;
    ing_l3_next_hop_entry_t ing_nh_entry;
    int nh_index;
    bcm_module_t mod;
    bcm_port_t port;
    int is_local;
    dlb_ecmp_member_sw_state_entry_t dlb_ecmp_member_sw_state_entry;

    SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                ecmp_group_idx, &ecmp_group_entry));
    dlb_enable = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_group_entry,
            GROUP_ENABLEf);
    dlb_id = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_group_entry,
            DLB_IDf);
    if (!dlb_enable) {
        return BCM_E_NONE;
    }

    /* Clear DLB fields in ECMP group table */
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_group_entry, GROUP_ENABLEf, 0);
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_group_entry, DLB_IDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ALL,
                ecmp_group_idx, &ecmp_group_entry));

    /* Clear VLA quality measure control */
    SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm(unit,
                MEM_BLOCK_ALL, dlb_id,
                soc_mem_entry_null(unit,
                    DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm)));

    /* Free flow set table resources */
    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY, dlb_id,
                                     &dlb_ecmp_group_control_entry));
    entry_base_ptr = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, FLOW_SET_BASEf);
    flow_set_size = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, FLOW_SET_SIZEf);
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_ecmp_dlb_dynamic_size_decode(flow_set_size, &num_entries));
    block_base_ptr = entry_base_ptr >> 9;
    num_blocks = num_entries >> 9; 
    _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_CLR_RANGE(unit, block_base_ptr, num_blocks);

    /* Clear ECMP DLB group control */
    SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_GROUP_CONTROLm(unit,
                MEM_BLOCK_ALL, dlb_id,
                soc_mem_entry_null(unit, DLB_ECMP_GROUP_CONTROLm)));

    /* Get member bitmap */
    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchEcmpDynamicAccountingSelect, &vla));
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ANY,
                dlb_id, &dlb_ecmp_group_membership_entry));
    member_bitmap_width = soc_mem_field_length(unit,
            DLB_ECMP_GROUP_MEMBERSHIPm, MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    member_bitmap = sal_alloc(alloc_size, "DLB ECMP member bitmap"); 
    if (NULL == member_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(member_bitmap, 0, alloc_size);
    soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_get(unit,
            &dlb_ecmp_group_membership_entry, MEMBER_BITMAPf, member_bitmap);

    /* Free Member IDs */
    for (i = 0; i < member_bitmap_width; i++) {
        if (SHR_BITGET(member_bitmap, i)) {

            /* Clear membership reverse map */
            rv = READ_DLB_ECMP_MEMBER_ATTRIBUTEm(unit, MEM_BLOCK_ANY, i,
                    &dlb_ecmp_member_attribute_entry); 
            if (BCM_FAILURE(rv)) {
                goto error;
            }
            nh_index = soc_DLB_ECMP_MEMBER_ATTRIBUTEm_field32_get(unit,
                    &dlb_ecmp_member_attribute_entry, NEXT_HOP_INDEXf);
            if (vla) {
                reverse_map_index = i;
            } else {
                rv = READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index,
                        &ing_nh_entry);
                if (BCM_FAILURE(rv)) {
                    goto error;
                }
                mod = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                        MODULE_IDf);
                port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                        PORT_NUMf);
                rv = _bcm_esw_modid_is_local(unit, mod, &is_local);
                if (BCM_FAILURE(rv)) {
                    goto error;
                }
                if (!is_local) {
                    rv = BCM_E_INTERNAL;
                    goto error;
                }
                reverse_map_index = port;
            }
            rv = WRITE_DLB_ECMP_MEMBERSHIP_REVERSE_MAPm(unit, MEM_BLOCK_ALL,
                    reverse_map_index, soc_mem_entry_null(unit,
                        DLB_ECMP_MEMBERSHIP_REVERSE_MAPm));
            if (BCM_FAILURE(rv)) {
                goto error;
            }

            /* Free next hop membership resource */
            rv = _bcm_tr3_ecmp_dlb_nh_membership_free_resource(unit, nh_index,
                    i, dlb_id);
            if (BCM_FAILURE(rv)) {
                goto error;
            }
        }
    }

    rv = READ_DLB_ECMP_MEMBER_SW_STATEm(unit, MEM_BLOCK_ANY, 0,
                &dlb_ecmp_member_sw_state_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Clear software status bitmap */
    status_bitmap = sal_alloc(alloc_size, "DLB ECMP member status bitmap"); 
    if (NULL == status_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_get(unit,
            &dlb_ecmp_member_sw_state_entry, MEMBER_BITMAPf, status_bitmap);
    SHR_BITREMOVE_RANGE(status_bitmap, member_bitmap, 0, member_bitmap_width,
            status_bitmap);
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_set(unit,
            &dlb_ecmp_member_sw_state_entry, MEMBER_BITMAPf, status_bitmap);

    /* Clear software override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB ECMP member override bitmap"); 
    if (NULL == override_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_get(unit,
            &dlb_ecmp_member_sw_state_entry, OVERRIDE_MEMBER_BITMAPf,
            override_bitmap);
    SHR_BITREMOVE_RANGE(override_bitmap, member_bitmap, 0, member_bitmap_width,
            override_bitmap);
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_set(unit,
            &dlb_ecmp_member_sw_state_entry, OVERRIDE_MEMBER_BITMAPf,
            override_bitmap);
    rv = WRITE_DLB_ECMP_MEMBER_SW_STATEm(unit, MEM_BLOCK_ALL, 0,
                &dlb_ecmp_member_sw_state_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    sal_free(member_bitmap);
    sal_free(status_bitmap);
    sal_free(override_bitmap);

    /* Clear group membership */
    SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_GROUP_MEMBERSHIPm(unit,
                MEM_BLOCK_ALL, dlb_id,
                soc_mem_entry_null(unit, DLB_ECMP_GROUP_MEMBERSHIPm)));

    /* Free DLB ID */
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_id_free(unit, dlb_id));

    return rv;

error:
    if (NULL != member_bitmap) {
        sal_free(member_bitmap);
    }
    if (NULL != status_bitmap) {
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) {
        sal_free(override_bitmap);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_member_id_array_get
 * Purpose:
 *      Get member IDs for each ECMP member.
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      dlb_id - (IN) DLB group ID.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      member_id_array - (OUT) Array of member IDs.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_member_id_array_get(int unit, int dlb_id, int intf_count,
        bcm_if_t *intf_array, int *member_id_array)
{
    int rv = BCM_E_NONE;
    int vla;
    int i, j;
    int nh_index;
    int match, match_index;
    _tr3_ecmp_dlb_nh_membership_t *current_ptr, *membership;
    int existing_member_id;
    dlb_ecmp_membership_reverse_map_entry_t member_reverse_map_entry;
    ing_l3_next_hop_entry_t ing_nh_entry;
    int mod, port;
    int is_local;
    port_tab_entry_t ptab;
    int scaling_factor, load_weight;

    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                        bcmSwitchEcmpDynamicAccountingSelect, &vla));

    for (i = 0; i < intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf_array[i])) {
            nh_index = intf_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf_array[i])) {
            nh_index = intf_array[i] - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            return BCM_E_PARAM;
        }

        /* Check if DLB attributes have been set for next hop index */
        match = FALSE;
        match_index = 0;
        for (j = 0; j < ECMP_DLB_INFO(unit)->ecmp_dlb_nh_info_size; j++) {
            if (ECMP_DLB_NH_INFO(unit, j).valid) {
                if (nh_index == ECMP_DLB_NH_INFO(unit, j).nh_index) {
                    match = TRUE;
                    match_index = j;
                }
            }
        }
        if (!match) {
            /* User did not configure DLB attributes for this next hop index */
            return BCM_E_CONFIG;
        }

        /* Look into next hop membership to see if a member ID has already
         * been assigned that does not yet belong to any group.
         */
        current_ptr = ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list;
        existing_member_id = -1;
        while (NULL != current_ptr) {
            if (-1 == current_ptr->group) {
                member_id_array[i] = current_ptr->member_id;
                current_ptr->group = dlb_id;
                break;
            } else {
                if (-1 == existing_member_id) {
                    existing_member_id = current_ptr->member_id;
                }
            }
            current_ptr = current_ptr->next;
        }

        /* If no available member ID was found, allocate a new DLB member ID,
         * set its DLB attributes, and insert it into next hop's linked list
         * of member IDs.
         */
        if (NULL == current_ptr) {
            membership = sal_alloc(sizeof(_tr3_ecmp_dlb_nh_membership_t),
                    "ecmp dlb nh membership");
            if (NULL == membership) {
                return BCM_E_MEMORY;
            }
            rv = _bcm_tr3_ecmp_dlb_member_id_alloc(unit, &membership->member_id);
            if (BCM_FAILURE(rv)) {
                sal_free(membership);
                return rv;
            }
            rv = _bcm_tr3_ecmp_dlb_member_attr_get(unit, existing_member_id,
                    &scaling_factor, &load_weight);
            if (BCM_FAILURE(rv)) {
                sal_free(membership);
                return rv;
            }
            rv = _bcm_tr3_ecmp_dlb_member_attr_set(unit, membership->member_id,
                    nh_index, scaling_factor, load_weight);
            if (BCM_FAILURE(rv)) {
                sal_free(membership);
                return rv;
            }
            membership->group = dlb_id;
            membership->next =
                ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list; 
            ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list = membership;
            ECMP_DLB_NH_INFO(unit, match_index).nh_index = nh_index;
            ECMP_DLB_NH_INFO(unit, match_index).valid = 1;

            member_id_array[i] = membership->member_id;
        }

        /* Set member reverse map */
        sal_memset(&member_reverse_map_entry, 0,
                sizeof(dlb_ecmp_membership_reverse_map_entry_t));
        if (vla) { /* Virtual link accounting */
            soc_DLB_ECMP_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry, VALIDf, 1);
            soc_DLB_ECMP_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry, GROUP_IDf, dlb_id);
            soc_DLB_ECMP_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry, MEMBER_COUNTf,
                    intf_count - 1);
            SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_MEMBERSHIP_REVERSE_MAPm(unit,
                    MEM_BLOCK_ALL, member_id_array[i],
                    &member_reverse_map_entry));
        } else { /* Physical link accounting */
            SOC_IF_ERROR_RETURN(READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                        nh_index, &ing_nh_entry));
            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, Tf)) {
                /* In physical link accounting mode, each next hop should
                 * correspond to a single physical port.
                 */
                return BCM_E_CONFIG;
            }
            mod = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                    MODULE_IDf);
            port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                    PORT_NUMf);
            BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, mod, &is_local));
            if (!is_local) {
                /* In physical link accounting mode, ECMP members should be
                 * local.
                 */
                return BCM_E_PARAM;
            }
            soc_DLB_ECMP_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry, VALIDf, 1);
            soc_DLB_ECMP_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                    &member_reverse_map_entry,
                    MEMBER_IDf, member_id_array[i]);
            SOC_IF_ERROR_RETURN
                (READ_PORT_TABm(unit, MEM_BLOCK_ANY, port, &ptab));
            if (soc_PORT_TABm_field32_get(unit, &ptab, PORT_TYPEf) == 3) {
                /* Port is an embedded Higig port */
                soc_DLB_ECMP_MEMBERSHIP_REVERSE_MAPm_field32_set(unit,
                        &member_reverse_map_entry, EMBEDDED_HGf, 1);
            }
            SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_MEMBERSHIP_REVERSE_MAPm(unit,
                        MEM_BLOCK_ALL, port, &member_reverse_map_entry));
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_set
 * Purpose:
 *      Configure an ECMP dynamic load balancing group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_set(int unit, bcm_l3_egress_ecmp_t *ecmp,
            int intf_count, bcm_if_t *intf_array)
{
    int rv = BCM_E_NONE;
    int dlb_id;
    int *member_id_array = NULL;
    soc_mem_t flowset_mem;
    int flowset_entry_size;
    dlb_ecmp_flowset_entry_t *flowset_entry;
    int num_blocks;
    int total_blocks;
    int max_block_base_ptr;
    int block_base_ptr;
    SHR_BITDCL occupied;
    int entry_base_ptr;
    int num_entries_per_block;
    int member_bitmap_width;
    int alloc_size;
    uint32 *block_ptr = NULL;
    int i, k;
    int index_min, index_max;
    dlb_ecmp_group_control_entry_t dlb_ecmp_group_control_entry;
    int flow_set_size;
    int dlb_mode;
    dlb_ecmp_group_membership_entry_t dlb_ecmp_group_membership_entry;
    SHR_BITDCL *member_bitmap = NULL;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    dlb_ecmp_member_sw_state_entry_t dlb_ecmp_member_sw_state_entry;
    int vla;
    dlb_ecmp_vla_quality_measure_control_entry_t vla_quality_measure_control_entry;
    int ecmp_group_idx;
    ecmp_count_entry_t ecmp_group_entry;

    /* Allocate a DLB ID */
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_id_alloc(unit, &dlb_id));

    /* Allocate member IDs */
    member_id_array = sal_alloc(sizeof(int) * intf_count,
            "ECMP DLB Member IDs");
    if (NULL == member_id_array) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    rv = _bcm_tr3_ecmp_dlb_member_id_array_get(unit, dlb_id, intf_count,
            intf_array, member_id_array);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Set group membership table */
    member_bitmap_width = soc_mem_field_length(unit,
            DLB_ECMP_GROUP_MEMBERSHIPm, MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    member_bitmap = sal_alloc(alloc_size, "DLB ECMP member bitmap"); 
    if (NULL == member_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(member_bitmap, 0, alloc_size);
    for (i = 0; i < intf_count; i++) {
        SHR_BITSET(member_bitmap, member_id_array[i]);
    }
    sal_memset(&dlb_ecmp_group_membership_entry, 0,
            sizeof(dlb_ecmp_group_membership_entry_t));
    soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_set(unit,
            &dlb_ecmp_group_membership_entry, MEMBER_BITMAPf, member_bitmap);
    rv = WRITE_DLB_ECMP_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ALL, dlb_id,
            &dlb_ecmp_group_membership_entry);
    if (SOC_FAILURE(rv)) {
        goto error;
    }

    /* Get software status bitmap */
    rv = READ_DLB_ECMP_MEMBER_SW_STATEm(unit, MEM_BLOCK_ANY, 0,
                &dlb_ecmp_member_sw_state_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }
    status_bitmap = sal_alloc(alloc_size, "DLB ECMP member status bitmap"); 
    if (NULL == status_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_get(unit,
            &dlb_ecmp_member_sw_state_entry, MEMBER_BITMAPf, status_bitmap);

    /* Set software status bitmap */
    rv = bcm_esw_switch_control_get(unit,
            bcmSwitchEcmpDynamicAccountingSelect, &vla);
    if (SOC_FAILURE(rv)) {
        goto error;
    }
    if (vla) {
        SHR_BITOR_RANGE(status_bitmap, member_bitmap, 0, member_bitmap_width,
                status_bitmap);
    } else {
        SHR_BITREMOVE_RANGE(status_bitmap, member_bitmap, 0, member_bitmap_width,
                status_bitmap);
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_set(unit,
            &dlb_ecmp_member_sw_state_entry, MEMBER_BITMAPf, status_bitmap);

    /* Set software override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB ECMP member override bitmap"); 
    if (NULL == override_bitmap) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_get(unit,
            &dlb_ecmp_member_sw_state_entry, OVERRIDE_MEMBER_BITMAPf,
            override_bitmap);
    if (vla) {
        SHR_BITOR_RANGE(override_bitmap, member_bitmap, 0, member_bitmap_width,
                override_bitmap);
    } else {
        SHR_BITREMOVE_RANGE(override_bitmap, member_bitmap, 0, member_bitmap_width,
                override_bitmap);
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_set(unit,
            &dlb_ecmp_member_sw_state_entry, OVERRIDE_MEMBER_BITMAPf,
            override_bitmap);
    rv = WRITE_DLB_ECMP_MEMBER_SW_STATEm(unit, MEM_BLOCK_ALL, 0,
                &dlb_ecmp_member_sw_state_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Find a contiguous region in flow set table */
    flowset_mem = DLB_ECMP_FLOWSETm;
    flowset_entry_size = sizeof(dlb_ecmp_flowset_entry_t);
    num_blocks = ecmp->dynamic_size >> 9;
    total_blocks = soc_mem_index_count(unit, flowset_mem) >> 9;
    max_block_base_ptr = total_blocks - num_blocks;
    for (block_base_ptr = 0;
            block_base_ptr <= max_block_base_ptr;
            block_base_ptr++) {
        /* Check if the contiguous region of flow set table from
         * block_base_ptr to (block_base_ptr + num_blocks - 1) is free. 
         */
        _BCM_ECMP_DLB_FLOWSET_BLOCK_TEST_RANGE(unit, block_base_ptr, num_blocks,
                occupied); 
        if (!occupied) {
            break;
        }
    }
    if (block_base_ptr > max_block_base_ptr) {
        /* A contiguous region of the desired size could not be found in
         * flow set table.
         */
        rv = BCM_E_RESOURCE;
        goto error;
    }

    /* Set DLB flow set table */
    entry_base_ptr = block_base_ptr << 9;
    num_entries_per_block = 512;
    alloc_size = num_entries_per_block * flowset_entry_size;
    block_ptr = soc_cm_salloc(unit, alloc_size,
            "Block of DLB_ECMP_FLOWSET entries");
    if (NULL == block_ptr) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(block_ptr, 0, alloc_size);
    for (i = 0; i < num_blocks; i++) {
        for (k = 0; k < num_entries_per_block; k++) {
            flowset_entry = soc_mem_table_idx_to_pointer(unit,
                    DLB_ECMP_FLOWSETm, dlb_ecmp_flowset_entry_t *,
                    block_ptr, k);
            if (intf_count > 0) {
                soc_DLB_ECMP_FLOWSETm_field32_set(unit, flowset_entry,
                        VALIDf, 1);
                soc_DLB_ECMP_FLOWSETm_field32_set(unit, flowset_entry,
                        MEMBER_IDf,
                        member_id_array[(i * num_entries_per_block + k) %
                        intf_count]);
            } else {
                soc_DLB_ECMP_FLOWSETm_field32_set(unit, flowset_entry,
                        VALIDf, 0);
            }
        }
        index_min = entry_base_ptr + i * num_entries_per_block;
        index_max = index_min + num_entries_per_block - 1;
        rv = soc_mem_write_range(unit, flowset_mem, MEM_BLOCK_ALL,
                index_min, index_max, block_ptr);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
    }
    _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr, num_blocks);

    /* Set ECMP DLB group control table */
    sal_memset(&dlb_ecmp_group_control_entry, 0,
            sizeof(dlb_ecmp_group_control_entry_t));
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            ENABLE_OPTIMAL_CANDIDATE_UPDATEf, 1);
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            FLOW_SET_BASEf, entry_base_ptr);
    rv = _bcm_tr3_ecmp_dlb_dynamic_size_encode(ecmp->dynamic_size,
            &flow_set_size);
    if (SOC_FAILURE(rv)) {
        goto error;
    }
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            FLOW_SET_SIZEf, flow_set_size);
    switch (ecmp->dynamic_mode) {
        case BCM_L3_ECMP_DYNAMIC_MODE_NORMAL:
            dlb_mode = 0;
            break;
        case BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED:
            dlb_mode = 1;
            break;
        case BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL:
            dlb_mode = 2;
            break;
        default:
            rv = BCM_E_PARAM;
            goto error;
    }
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            MEMBER_ASSIGNMENT_MODEf, dlb_mode);
    soc_DLB_ECMP_GROUP_CONTROLm_field32_set(unit, &dlb_ecmp_group_control_entry,
            INACTIVITY_DURATIONf, ecmp->dynamic_age);
    rv = WRITE_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ALL, dlb_id,
            &dlb_ecmp_group_control_entry);
    if (SOC_FAILURE(rv)) {
        goto error;
    }

    /* Set VLA quality measure control */
    if (vla) {
        sal_memset(&vla_quality_measure_control_entry, 0,
                sizeof(dlb_ecmp_vla_quality_measure_control_entry_t));
        soc_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                ENABLE_GROUP_AVG_CALCf, 1);
        soc_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                VLA_WEIGHT_LOADINGf, ecmp->dynamic_load_exponent);
        soc_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                VLA_WEIGHT_EXPECTED_LOADINGf,
                ecmp->dynamic_expected_load_exponent);
        soc_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                VLA_CAP_LOADING_AVGf, (ecmp->ecmp_group_flags &
                    BCM_L3_ECMP_DYNAMIC_LOAD_DECREASE_RESET) ? 1 : 0);
        soc_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm_field32_set(unit,
                &vla_quality_measure_control_entry,
                VLA_CAP_EXPECTED_LOADINGf, (ecmp->ecmp_group_flags &
                    BCM_L3_ECMP_DYNAMIC_EXPECTED_LOAD_DECREASE_RESET) ?
                1 : 0);
        rv = WRITE_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm(unit, MEM_BLOCK_ALL,
                dlb_id, &vla_quality_measure_control_entry);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
    }

    /* Update ECMP group table */
    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    rv = READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group_idx,
            &ecmp_group_entry);
    if (SOC_FAILURE(rv)) {
        goto error;
    }
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_group_entry, GROUP_ENABLEf, 1);
    soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_group_entry, DLB_IDf, dlb_id);
    rv = WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ALL, ecmp_group_idx,
            &ecmp_group_entry);
    if (SOC_FAILURE(rv)) {
        goto error;
    }

    sal_free(member_id_array);
    soc_cm_sfree(unit, block_ptr);
    sal_free(member_bitmap);
    sal_free(status_bitmap);
    sal_free(override_bitmap);

    return rv;

error:
    if (NULL != member_id_array) { 
        sal_free(member_id_array);
    }
    if (NULL != block_ptr) { 
        soc_cm_sfree(unit, block_ptr);
    }
    if (NULL != member_bitmap) { 
        sal_free(member_bitmap);
    }
    if (NULL != status_bitmap) { 
        sal_free(status_bitmap);
    }
    if (NULL != override_bitmap) { 
        sal_free(override_bitmap);
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l3_egress_ecmp_dlb_create 
 * Purpose:
 *      Create ECMP DLB group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr3_l3_egress_ecmp_dlb_create(int unit, bcm_l3_egress_ecmp_t *ecmp,
        int intf_count, bcm_if_t *intf_array)
{
    int dlb_enable;
    int dynamic_size_encode;
    int ecmp_group_idx;

    if ((ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_NORMAL) ||
           (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED) ||
           (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL)) {
        dlb_enable = 1;
    } else {
        dlb_enable = 0;
    }

    if (dlb_enable) {
        /* Verify dynamic_size */
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_ecmp_dlb_dynamic_size_encode(ecmp->dynamic_size,
                                                   &dynamic_size_encode));

        /* Verify dynamic_age */
        /* Error rate in calculating INACTIVITY_DURATION is higher
           if it is allowed to be configured too low.
           Minimum limit of 16 microseconds is required to reduce
           the error rate in time duration calculation.*/
        if ((ecmp->dynamic_age < 16) ||
                (ecmp->dynamic_age > 0x3ffff)) {
            return BCM_E_PARAM;
        }

        /* Verify dynamic exponents */
        if (ecmp->dynamic_load_exponent > 0xf) {
            return BCM_E_PARAM;
        }
        if (ecmp->dynamic_expected_load_exponent > 0xf) {
            return BCM_E_PARAM;
        }
    }

    /* Free resources associated with old DLB group, if exists */
    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_free_resource(unit, ecmp_group_idx));

    /* Set new DLB group */
    if (dlb_enable) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_ecmp_dlb_set(unit, ecmp, intf_count, intf_array));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l3_egress_ecmp_dlb_destroy
 * Purpose:
 *      Destroy an ECMP DLB group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      mpintf     - (IN) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr3_l3_egress_ecmp_dlb_destroy(int unit, bcm_if_t mpintf)
{
    int ecmp_group;

    /* Free resources associated with old DLB group, if exists */
    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, mpintf)) {
        ecmp_group = mpintf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_free_resource(unit, ecmp_group));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l3_egress_ecmp_dlb_get
 * Purpose:
 *      Get info about an ECMP DLB group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ecmp       - (INOUT) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr3_l3_egress_ecmp_dlb_get(int unit, bcm_l3_egress_ecmp_t *ecmp)
{
    int ecmp_group_idx;
    ecmp_count_entry_t ecmp_group_entry;
    int group_enable, dlb_id;
    dlb_ecmp_group_control_entry_t dlb_ecmp_group_control_entry;
    int dlb_mode;
    int flow_set_size;
    dlb_ecmp_vla_quality_measure_control_entry_t vla_quality_measure_control_entry;

    /* Get DLB ID */
    ecmp_group_idx = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                ecmp_group_idx, &ecmp_group_entry));
    group_enable = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_group_entry,
            GROUP_ENABLEf);
    if (!group_enable) {
        ecmp->dynamic_mode = 0;
        ecmp->dynamic_size = 0;
        ecmp->dynamic_age = 0;
        ecmp->dynamic_load_exponent = 0;
        ecmp->dynamic_expected_load_exponent = 0;
        return BCM_E_NONE;
    }
    dlb_id = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_group_entry, DLB_IDf);

    /* Get dynamic_mode */
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY,
                dlb_id, &dlb_ecmp_group_control_entry));
    dlb_mode = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, MEMBER_ASSIGNMENT_MODEf);
    switch (dlb_mode) {
        case 0:
            ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_NORMAL;
            break;
        case 1:
            ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED;
            break;
        case 2:
            ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    /* Get dynamic_size */
    flow_set_size = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, FLOW_SET_SIZEf);
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_ecmp_dlb_dynamic_size_decode(flow_set_size,
                                               (int *)&ecmp->dynamic_size));

    /* Get dynamic_age */
    ecmp->dynamic_age = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
            &dlb_ecmp_group_control_entry, INACTIVITY_DURATIONf);

    /* Get dynamic exponents and ecmp_group_flags */
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm(unit,
                MEM_BLOCK_ANY, dlb_id, &vla_quality_measure_control_entry));
    ecmp->dynamic_load_exponent =
        soc_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm_field32_get(unit,
                &vla_quality_measure_control_entry, VLA_WEIGHT_LOADINGf);
    ecmp->dynamic_expected_load_exponent =
        soc_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm_field32_get(unit,
                &vla_quality_measure_control_entry, VLA_WEIGHT_EXPECTED_LOADINGf);
    if (soc_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm_field32_get(unit,
                &vla_quality_measure_control_entry, VLA_CAP_LOADING_AVGf)) {
       ecmp->ecmp_group_flags |= BCM_L3_ECMP_DYNAMIC_LOAD_DECREASE_RESET;
    } 
    if (soc_DLB_ECMP_VLA_QUALITY_MEASURE_CONTROLm_field32_get(unit,
                &vla_quality_measure_control_entry,
                VLA_CAP_EXPECTED_LOADINGf)) {
       ecmp->ecmp_group_flags |=
           BCM_L3_ECMP_DYNAMIC_EXPECTED_LOAD_DECREASE_RESET;
    } 

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l3_egress_ecmp_member_status_set
 * Purpose:
 *      Set ECMP DLB member status.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      intf - (IN) L3 Interface ID pointing to an Egress forwarding object.
 *      status - (IN) ECMP member status.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_l3_egress_ecmp_member_status_set(int unit, bcm_if_t intf, int status)
{
    int nh_index;
    dlb_ecmp_member_sw_state_entry_t sw_state_entry;
    int member_bitmap_width, alloc_size;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    int dlb_group_valid;
    int i;
    _tr3_ecmp_dlb_nh_membership_t *current_ptr;

    /* Derive next hop index */
    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
        nh_index = intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
        nh_index = intf - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    /* Get status bitmap */
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_MEMBER_SW_STATEm(unit, MEM_BLOCK_ANY, 0,
                &sw_state_entry));
    member_bitmap_width = soc_mem_field_length(unit, DLB_ECMP_MEMBER_SW_STATEm,
            MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    status_bitmap = sal_alloc(alloc_size, "DLB ECMP member status bitmap"); 
    if (NULL == status_bitmap) {
        return BCM_E_MEMORY;
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_get(unit, &sw_state_entry,
            MEMBER_BITMAPf, status_bitmap);

    /* Get override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB ECMP member override bitmap"); 
    if (NULL == override_bitmap) {
        sal_free(status_bitmap);
        return BCM_E_MEMORY;
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_get(unit, &sw_state_entry,
            OVERRIDE_MEMBER_BITMAPf, override_bitmap);

    /* Update status and override bitmaps for any of the next hop's DLB
     * member IDs that belong to an ECMP DLB group.
     */
    dlb_group_valid = FALSE;
    for (i = 0; i < ECMP_DLB_INFO(unit)->ecmp_dlb_nh_info_size; i++) {
        if (ECMP_DLB_NH_INFO(unit, i).valid) {
            if (nh_index == ECMP_DLB_NH_INFO(unit, i).nh_index) {
                current_ptr = ECMP_DLB_NH_INFO(unit, i).nh_membership_list;
                while (NULL != current_ptr) {
                    if (current_ptr->group != -1) {
                        dlb_group_valid = TRUE;
                        switch (status) {
                            case BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_DOWN:
                                SHR_BITSET(override_bitmap,
                                        current_ptr->member_id);
                                SHR_BITCLR(status_bitmap,
                                        current_ptr->member_id);
                                break;
                            case BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_UP:
                                SHR_BITSET(override_bitmap,
                                        current_ptr->member_id);
                                SHR_BITSET(status_bitmap,
                                        current_ptr->member_id);
                                break;
                            case BCM_L3_ECMP_DYNAMIC_MEMBER_HW:
                                SHR_BITCLR(override_bitmap,
                                        current_ptr->member_id);
                                SHR_BITCLR(status_bitmap,
                                        current_ptr->member_id);
                                break;
                            default:
                                sal_free(status_bitmap);
                                sal_free(override_bitmap);
                                return BCM_E_PARAM;
                        }
                    }
                    current_ptr = current_ptr->next;
                }
                break;
            }
        }
    }
    if (!dlb_group_valid) {
        /* The given intf was not a member of any DLB group */
        sal_free(status_bitmap);
        sal_free(override_bitmap);
        return BCM_E_NOT_FOUND;
    }

    /* Write status and override bitmaps to hardware */
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_set(unit, &sw_state_entry,
            MEMBER_BITMAPf, status_bitmap);
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_set(unit, &sw_state_entry,
            OVERRIDE_MEMBER_BITMAPf, override_bitmap);
    sal_free(status_bitmap);
    sal_free(override_bitmap);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_MEMBER_SW_STATEm(unit, MEM_BLOCK_ALL, 0,
                                         &sw_state_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l3_egress_ecmp_member_status_get
 * Purpose:
 *      Get ECMP DLB member status.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      intf - (IN) L3 Interface ID pointing to an Egress forwarding object.
 *      status - (OUT) ECMP member status.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_tr3_l3_egress_ecmp_member_status_get(int unit, bcm_if_t intf, int *status)
{
    int rv = BCM_E_NONE;
    int nh_index;
    int member_id;
    int i;
    _tr3_ecmp_dlb_nh_membership_t *current_ptr;
    dlb_ecmp_member_sw_state_entry_t sw_state_entry;
    int member_bitmap_width, alloc_size;
    SHR_BITDCL *status_bitmap = NULL;
    SHR_BITDCL *override_bitmap = NULL;
    uint64 hw_state, hw_status_bitmap;
    uint32 bitmap32;

    if (NULL == status) {
        return BCM_E_PARAM;
    }
            
    /* Derive next hop index */
    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
        nh_index = intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
        nh_index = intf - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    /* Find next hop's DLB member ID */
    member_id = -1;
    for (i = 0; i < ECMP_DLB_INFO(unit)->ecmp_dlb_nh_info_size; i++) {
        if (ECMP_DLB_NH_INFO(unit, i).valid) {
            if (nh_index == ECMP_DLB_NH_INFO(unit, i).nh_index) {
                current_ptr = ECMP_DLB_NH_INFO(unit, i).nh_membership_list;
                while (NULL != current_ptr) {
                    if (current_ptr->group != -1) {
                        member_id = current_ptr->member_id;
                        break;
                    }
                    current_ptr = current_ptr->next;
                }
                break;
            }
        }
    }
    if (-1 == member_id) {
        /* The given intf was not a member of any DLB group */
        return BCM_E_NOT_FOUND;
    }

    /* Get status bitmap */
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_MEMBER_SW_STATEm(unit, MEM_BLOCK_ANY, 0,
                &sw_state_entry));
    member_bitmap_width = soc_mem_field_length(unit, DLB_ECMP_MEMBER_SW_STATEm,
            MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    status_bitmap = sal_alloc(alloc_size, "DLB ECMP member status bitmap"); 
    if (NULL == status_bitmap) {
        return BCM_E_MEMORY;
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_get(unit, &sw_state_entry,
            MEMBER_BITMAPf, status_bitmap);

    /* Get override bitmap */
    override_bitmap = sal_alloc(alloc_size, "DLB ECMP member override bitmap"); 
    if (NULL == override_bitmap) {
        sal_free(status_bitmap);
        return BCM_E_MEMORY;
    }
    soc_DLB_ECMP_MEMBER_SW_STATEm_field_get(unit, &sw_state_entry,
            OVERRIDE_MEMBER_BITMAPf, override_bitmap);

    /* Derive status from software override and status bitmaps or
     * hardware state bitmap.
     */
    if (SHR_BITGET(override_bitmap, member_id)) {
        if (SHR_BITGET(status_bitmap, member_id)) {
            *status = BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_UP;
        } else {
            *status = BCM_L3_ECMP_DYNAMIC_MEMBER_FORCE_DOWN;
        }
    } else {
        rv = READ_DLB_ECMP_MEMBER_HW_STATE_64r(unit, &hw_state); 
        if (SOC_FAILURE(rv)) {
            sal_free(status_bitmap);
            sal_free(override_bitmap);
            return rv;
        }
        hw_status_bitmap = soc_reg64_field_get(unit,
                DLB_ECMP_MEMBER_HW_STATE_64r, hw_state, BITMAPf);
        if (member_id < 32) {
            bitmap32 = COMPILER_64_LO(hw_status_bitmap);
        } else {
            bitmap32 = COMPILER_64_HI(hw_status_bitmap);
            member_id -= 32;
        }
        if ((1 << member_id) & bitmap32) {
            *status = BCM_L3_ECMP_DYNAMIC_MEMBER_HW_UP;
        } else {
            *status = BCM_L3_ECMP_DYNAMIC_MEMBER_HW_DOWN;
        }
    }

    /* Cleanup */
    if (status_bitmap) {
        sal_free(status_bitmap);
    }

    if (override_bitmap) {
        sal_free(override_bitmap);
    }

    return rv;
}
/*
 * Function:
 *      bcm_tr3_l3_egress_ecmp_dlb_ethertype_set
 * Purpose:
 *      Set the Ethertypes that are eligible or ineligible for
 *      ECMP dynamic load balancing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_count - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (IN) Array of Ethertypes.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr3_l3_egress_ecmp_dlb_ethertype_set(
    int unit, 
    uint32 flags, 
    int ethertype_count, 
    int *ethertype_array)
{
    uint32 measure_control_reg;
    int i, j;
    dlb_ecmp_ethertype_eligibility_map_entry_t ethertype_entry;

    /* Input check */
    if ((ethertype_count > 0) && (NULL == ethertype_array)) {
        return BCM_E_PARAM;
    }
    if (ethertype_count > soc_mem_index_count(unit,
                DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm)) {
        return BCM_E_RESOURCE;
    }

    /* Update quality measure control register */
    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, ETHERTYPE_ELIGIBILITY_CONFIGf,
            flags & BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE ? 1 : 0);
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, INNER_OUTER_ETHERTYPE_SELECTIONf,
            flags & BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER ? 1 : 0);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));

    /* Update Ethertype eligibility map table */
    for (i = 0; i < ethertype_count; i++) {
        sal_memset(&ethertype_entry, 0,
                sizeof(dlb_ecmp_ethertype_eligibility_map_entry_t));
        soc_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, VALIDf, 1);
        soc_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_set(unit,
                &ethertype_entry, ETHERTYPEf, ethertype_array[i]);
        SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, i, &ethertype_entry));
    }

    /* Zero out remaining entries of Ethertype eligibility map table */
    for (j = i; j < soc_mem_index_count(unit,
                DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm); j++) {
        SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ALL, j, soc_mem_entry_null(unit,
                        DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l3_egress_ecmp_dlb_ethertype_get
 * Purpose:
 *      Get the Ethertypes that are eligible or ineligible for
 *      ECMP dynamic load balancing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (INOUT) BCM_L3_ECMP_DYNAMIC_ETHERTYPE_xxx flags.
 *      ethertype_max - (IN) Number of elements in ethertype_array.
 *      ethertype_array - (OUT) Array of Ethertypes.
 *      ethertype_count - (OUT) Number of elements returned in ethertype_array.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr3_l3_egress_ecmp_dlb_ethertype_get(
    int unit, 
    uint32 *flags, 
    int ethertype_max, 
    int *ethertype_array, 
    int *ethertype_count)
{
    uint32 measure_control_reg;
    int i;
    int ethertype;
    dlb_ecmp_ethertype_eligibility_map_entry_t ethertype_entry;

    /* Input check */
    if (NULL == flags) {
        return BCM_E_PARAM;
    }
    if ((ethertype_max > 0) && (NULL == ethertype_array)) {
        return BCM_E_PARAM;
    }
    if (NULL == ethertype_count) {
        return BCM_E_PARAM;
    }

    *ethertype_count = 0;

    /* Get flags */
    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    if (soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                measure_control_reg, ETHERTYPE_ELIGIBILITY_CONFIGf)) {
        *flags |= BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE;
    }
    if (soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                measure_control_reg, INNER_OUTER_ETHERTYPE_SELECTIONf)) {
        *flags |= BCM_L3_ECMP_DYNAMIC_ETHERTYPE_INNER;
    }

    /* Get Ethertypes */
    for (i = 0; i < soc_mem_index_count(unit,
                DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm); i++) {
        SOC_IF_ERROR_RETURN(READ_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm(unit,
                    MEM_BLOCK_ANY, i, &ethertype_entry));
        if (soc_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
                    &ethertype_entry, VALIDf)) {
            ethertype = soc_DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm_field32_get(unit,
                    &ethertype_entry, ETHERTYPEf);
            if (NULL != ethertype_array) {
                ethertype_array[*ethertype_count] = ethertype;
            }
            (*ethertype_count)++;
            if ((ethertype_max > 0) && (*ethertype_count == ethertype_max)) {
                break;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_accounting_set
 * Purpose:
 *      Set ECMP DLB accounting mode.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      accounting_mode - (IN) Accounting mode.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_accounting_set(int unit, int accounting_mode)
{
    uint32 measure_control_reg;
    int old_accounting_mode;
    SHR_BITDCL active;

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    old_accounting_mode = soc_reg_field_get(unit,
            DLB_ECMP_QUALITY_MEASURE_CONTROLr, measure_control_reg,
            ACCOUNTING_SELf);
    if (old_accounting_mode == accounting_mode) {
        return BCM_E_NONE;
    }

    SHR_BITTEST_RANGE(ECMP_DLB_INFO(unit)->ecmp_dlb_id_used_bitmap,
            0, soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm), active);
    if (active) {
        /* Accounting mode cannot be changed while there are active
         * DLB groups.
         */
        return BCM_E_BUSY;
    }

    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, ACCOUNTING_SELf, accounting_mode);
    if (accounting_mode) { /* virtual link accounting */
        soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                &measure_control_reg, INST_METRIC_UPDATE_INTERVALf, 1);
    } else { /* physical link accounting */
        soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                &measure_control_reg, INST_METRIC_UPDATE_INTERVALf, 0);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_accounting_get
 * Purpose:
 *      Get ECMP DLB accounting mode.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      accounting_mode - (OUT) Accounting mode.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_accounting_get(int unit, int *accounting_mode)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *accounting_mode = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, ACCOUNTING_SELf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_sample_rate_pla_thresholds_set
 * Purpose:
 *      Set ECMP DLB sample period and physical link accounting thresholds.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 *      min_th      - (IN) Minimum port load threshold, in mbps.
 *      max_th      - (IN) Maximum port load threshold, in mbps.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_sample_rate_pla_thresholds_set(int unit, int sample_rate,
        int min_th, int max_th)
{
    int num_time_units;
    uint32 measure_control_reg;
    int max_th_bytes;
    int th_increment;
    dlb_ecmp_pla_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_bytes[7];

    if (sample_rate <= 0 || min_th < 0 || max_th < 0) {
        return BCM_E_PARAM;
    }

    if (min_th > max_th) {
        max_th = min_th;
    }

    /* Compute sampling period in units of 1us:
     *     number of 1us time units = 10^6 / sample_rate
     */ 
    num_time_units = 1000000 / sample_rate;
    if (num_time_units < 1 || num_time_units > 0x3fff) {
        /* Hardware limits the sampling period to 14 bits */ 
        return BCM_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, HISTORICAL_SAMPLING_PERIODf, num_time_units);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate = sample_rate;

    /* Compute threshold in bytes per sampling period:
     * bytes per 1us = (million bits per sec) * 10^6 / 8 * 10^(-6),
     * bytes per sampling period = (bytes per 1us) * (number of 1us 
     *                             time units in sampling period)
     *                           = mbps * num_time_units / 8
     */
    max_th_bytes = max_th * num_time_units / 8;

    if (max_th_bytes > 0x3fffffff) {
        /* Hardware limits port load threshold to 30 bits */
        return BCM_E_PARAM;
    }

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        SOC_IF_ERROR_RETURN
            (READ_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_bytes[i] = (min_th + i * th_increment) * num_time_units / 8;
        soc_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, THRESHOLD_HIST_LOADf,
             th_bytes[i]);
        SOC_IF_ERROR_RETURN
            (WRITE_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th = min_th;
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th = max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_sample_rate_vla_thresholds_set
 * Purpose:
 *      Set ECMP DLB sample period and virtual link accounting thresholds.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 *      exp_load_min_th  - (IN) Minimum expected load threshold, in mbps.
 *      exp_load_max_th  - (IN) Maximum expected load threshold, in mbps.
 *      imbalance_min_th - (IN) Minimum member imbalance threshold,
 *                              in percentage.
 *      imbalance_max_th - (IN) Maximum member imbalance threshold.
 *                              in percentage.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_sample_rate_vla_thresholds_set(int unit, int sample_rate,
        int exp_load_min_th, int exp_load_max_th,
        int imbalance_min_th, int imbalance_max_th)
{
    int num_time_units;
    uint32 measure_control_reg;
    int exp_load_max_th_kbytes;
    int num_exp_load_th;
    int exp_load_th_increment;
    dlb_ecmp_vla_expected_loading_threshold_entry_t exp_load_threshold_entry;
    int exp_load_th_kbytes;
    int num_imbalance_th;
    int imbalance_th_increment;
    int imbalance_max_th_kbytes;
    int imbalance_th_kbytes;
    dlb_ecmp_vla_member_imbalance_threshold_entry_t imbalance_threshold_entry;
    int i, j;
    int imbalance_th_fields[7] = {THRESHOLD_0f, THRESHOLD_1f, THRESHOLD_2f,
                                  THRESHOLD_3f, THRESHOLD_4f, THRESHOLD_5f,
                                  THRESHOLD_6f};

    if (sample_rate <= 0 || exp_load_min_th < 0 || exp_load_max_th < 0) {
        return BCM_E_PARAM;
    }

    if (exp_load_min_th > exp_load_max_th) {
        exp_load_max_th = exp_load_min_th;
    }

    if (imbalance_min_th > imbalance_max_th) {
        imbalance_max_th = imbalance_min_th;
    }

    /* Compute sampling period in units of 1us:
     *     number of 1us time units = 10^6 / sample_rate
     */ 
    num_time_units = 1000000 / sample_rate;
    if (num_time_units < 1 || num_time_units > 0x3fff) {
        /* Hardware limits the sampling period to 14 bits */ 
        return BCM_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            &measure_control_reg, HISTORICAL_SAMPLING_PERIODf, num_time_units);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate = sample_rate;

    /* Compute expected loading threshold in kbytes per sampling period:
     * bytes per 1us = (million bits per sec) * 10^6 / 8 * 10^(-6)
     *               = mbps / 8.
     * bytes per sampling period = (bytes per 1us) * (number of 1us 
     *                             time units in sampling period)
     *                           = mbps * num_time_units / 8.
     * kbytes per sampling period =  mbps * num_times_units / 8000.
     */
    exp_load_max_th_kbytes = exp_load_max_th * num_time_units / 8000;
    if (exp_load_max_th_kbytes > 0xfffff) {
        /* Hardware limits expected loading threshold in kbytes to 20 bits */
        return BCM_E_PARAM;
    }

    /* Use expected load min threshold as threshold 0, and max threshold as
     * threshold 14. Thresholds 1 to 13 will be evenly spread out between them.
     */
    num_exp_load_th = soc_mem_index_count(unit,
            DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm);
    exp_load_th_increment = (exp_load_max_th - exp_load_min_th) /
                            (num_exp_load_th - 1);
    for (i = 0; i < num_exp_load_th; i++) {
        SOC_IF_ERROR_RETURN(READ_DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                    MEM_BLOCK_ANY, i, &exp_load_threshold_entry));
        exp_load_th_kbytes = (exp_load_min_th + i * exp_load_th_increment) *
                             num_time_units / 8000;
        soc_DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm_field32_set
            (unit, &exp_load_threshold_entry, THRESHOLDf, exp_load_th_kbytes);
        SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                    MEM_BLOCK_ALL, i, &exp_load_threshold_entry));
    }

    ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th = exp_load_min_th;
    ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th = exp_load_max_th;

    /* Use member imbalance min threshold as threshold 0, and max threshold as
     * threshold 6. Thresholds 1 to 5 will be evenly spread out between them.
     */
    num_imbalance_th = 7;
    imbalance_th_increment = (imbalance_max_th - imbalance_min_th) /
        (num_imbalance_th - 1);
    for (i = 0;
         i < soc_mem_index_count(unit,
             DLB_ECMP_VLA_MEMBER_IMBALANCE_THRESHOLDm);
         i++) {
        /* There are 16 sets of member imbalance thresholds.
         * For sets 0 to 14, use expected loading thresholds 0 to 14 from
         * DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLD table as the
         * expected load. For set 15, extrapolate the expected load.
         */
        if (i < num_exp_load_th) {
            SOC_IF_ERROR_RETURN
                (READ_DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                        MEM_BLOCK_ANY, i, &exp_load_threshold_entry));
            exp_load_th_kbytes =
                soc_DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm_field32_get(unit,
                        &exp_load_threshold_entry, THRESHOLDf);
        } else {
            exp_load_th_kbytes =
                (exp_load_min_th + num_exp_load_th * exp_load_th_increment) *
                num_time_units / 8000;
        }

        /* Compute member imbalance thresholds */
        imbalance_max_th_kbytes = exp_load_th_kbytes * imbalance_max_th / 100;
        if (imbalance_max_th_kbytes > 0xfffff) {
            /* Hardware limits imbalance threshold in kbytes to 1 sign bit
             * plus 20 bits.
             */
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(READ_DLB_ECMP_VLA_MEMBER_IMBALANCE_THRESHOLDm(unit,
                    MEM_BLOCK_ANY, i, &imbalance_threshold_entry));
        for (j = 0; j < num_imbalance_th; j++) {
            imbalance_th_kbytes = exp_load_th_kbytes *
                (imbalance_min_th + j * imbalance_th_increment) / 100;
            if (imbalance_th_kbytes < 0) {
                /* Convert to two's complement representation */
                imbalance_th_kbytes = -1 * imbalance_th_kbytes;
                imbalance_th_kbytes = (~imbalance_th_kbytes) + 1;
            } 
            soc_DLB_ECMP_VLA_MEMBER_IMBALANCE_THRESHOLDm_field32_set(unit,
                    &imbalance_threshold_entry, imbalance_th_fields[j],
                    imbalance_th_kbytes & 0x1fffff);
        }
        SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_VLA_MEMBER_IMBALANCE_THRESHOLDm(unit,
                    MEM_BLOCK_ALL, i, &imbalance_threshold_entry));
    }

    ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th = imbalance_min_th;
    ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th = imbalance_max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_sample_rate_set
 * Purpose:
 *      Set ECMP dynamic load balancing sample rate.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      sample_rate - (IN) Number of samplings per second.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_sample_rate_set(int unit, int sample_rate)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_sample_rate_pla_thresholds_set(unit,
                sample_rate, ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th));

    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_sample_rate_vla_thresholds_set(unit,
                sample_rate, ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_tx_load_min_th_set
 * Purpose:
 *      Set ECMP DLB port load minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum port loading threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_tx_load_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_sample_rate_pla_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate, min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_ecmp_dlb_tx_load_max_th_set
 * Purpose:
 *      Set LAG DLB port load maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum port loading threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_tx_load_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_sample_rate_pla_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate,
                ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th,
                max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_exp_load_min_th_set
 * Purpose:
 *      Set ECMP DLB expected load minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum expected load threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_exp_load_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_sample_rate_vla_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate,
                min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_exp_load_max_th_set
 * Purpose:
 *      Set ECMP DLB expected load maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum expected load threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_exp_load_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_sample_rate_vla_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate,
                ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th,
                max_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_imbalance_min_th_set
 * Purpose:
 *      Set ECMP DLB member imbalance minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum member imbalance threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_imbalance_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_sample_rate_vla_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate,
                ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th,
                min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_imbalance_max_th_set
 * Purpose:
 *      Set ECMP DLB member imbalance maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum member imbalance threshold.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_imbalance_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_sample_rate_vla_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate,
                ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th,
                ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th,
                max_th));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_qsize_thresholds_set
 * Purpose:
 *      Set LAG DLB queue size thresholds.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum queue size threshold, in bytes.
 *      max_th - (IN) Maximum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_qsize_thresholds_set(int unit, int min_th, int max_th)
{
    int max_th_cells;
    int th_increment;
    dlb_ecmp_pla_quantize_threshold_entry_t quantize_threshold_entry;
    int i;
    int th_cells[7];

    if (min_th < 0 || max_th < 0) {
        return BCM_E_PARAM;
    }

    if (min_th > max_th) {
        max_th = min_th;
    }

    /* Convert threshold to number of cells */
    max_th_cells = max_th / 208;
    if (max_th_cells > 0xffff) {
        /* Hardware limits queue size threshold to 16 bits */
        return BCM_E_PARAM;
    }

    /* Use min threshold as threshold 0, and max threshold as threshold 6.
     * Thresholds 1 to 5 will be evenly spread out between min and max
     * thresholds.
     */
    th_increment = (max_th - min_th) / 6;
    for (i = 0; i < 7; i++) {
        SOC_IF_ERROR_RETURN
            (READ_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ANY, i,
                                              &quantize_threshold_entry));
        th_cells[i] = (min_th + i * th_increment) / 208;
        soc_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm_field32_set
            (unit, &quantize_threshold_entry, THRESHOLD_HIST_QSIZEf,
             th_cells[i]);
        SOC_IF_ERROR_RETURN
            (WRITE_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm(unit, MEM_BLOCK_ALL, i,
                                               &quantize_threshold_entry));
    }

    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th = min_th;
    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th = max_th;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_qsize_min_th_set
 * Purpose:
 *      Set LAG DLB queue size minimum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      min_th - (IN) Minimum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_qsize_min_th_set(int unit, int min_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_qsize_thresholds_set(unit,
                min_th, ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_qsize_max_th_set
 * Purpose:
 *      Set LAG DLB queue size maximum threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      max_th - (IN) Maximum queue size threshold, in bytes.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_qsize_max_th_set(int unit, int max_th)
{
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_qsize_thresholds_set(unit,
                ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th, max_th));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_tx_load_weight_set
 * Purpose:
 *      Set LAG DLB port load weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (IN) Port load weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_tx_load_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;

    if (weight < 0 || 
        weight > 0xf) {
        /* Hardware limits port load weight to 4 bits */
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PLA_WEIGHT_LOADINGf, weight);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_tx_load_weight_get
 * Purpose:
 *      Get LAG DLB port load weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (OUT) Port load weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_tx_load_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *weight = soc_reg_field_get(unit, DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PLA_WEIGHT_LOADINGf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_qsize_weight_set
 * Purpose:
 *      Set LAG DLB qsize weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (IN) Qsize weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_qsize_weight_set(int unit, int weight)
{
    uint32 measure_control_reg;

    if (weight < 0 || 
        weight > 0xf) {
        /* Hardware limits qsize weight to 4 bits */
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PLA_WEIGHT_QSIZEf, weight);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_qsize_weight_get
 * Purpose:
 *      Get LAG DLB qsize weight.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      weight - (OUT) Qsize weight.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_qsize_weight_get(int unit, int *weight)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *weight = soc_reg_field_get(unit, DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PLA_WEIGHT_QSIZEf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_tx_load_cap_set
 * Purpose:
 *      Set LAG DLB port load cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (IN) Port load cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_tx_load_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;

    if (cap < 0 || 
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PLA_CAP_LOADING_AVGf, cap);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_tx_load_cap_get
 * Purpose:
 *      Get LAG DLB port load cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (OUT) Cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_tx_load_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *cap= soc_reg_field_get(unit, DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PLA_CAP_LOADING_AVGf);

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_qsize_cap_set
 * Purpose:
 *      Set LAG DLB queue size cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (IN) Queue size cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_qsize_cap_set(int unit, int cap)
{
    uint32 measure_control_reg;

    if (cap < 0 || 
        cap > 1) {
        /* Hardware limits port load cap control to 1 bit */
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    soc_reg_field_set(unit, DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr,
        &measure_control_reg, PLA_CAP_QSIZE_AVGf, cap);
    SOC_IF_ERROR_RETURN
        (WRITE_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, measure_control_reg));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_qsize_cap_get
 * Purpose:
 *      Get LAG DLB queue size cap control.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      cap - (OUT) Cap control.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_tr3_ecmp_dlb_qsize_cap_get(int unit, int *cap)
{
    uint32 measure_control_reg;

    SOC_IF_ERROR_RETURN
        (READ_DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr(unit, &measure_control_reg));
    *cap= soc_reg_field_get(unit, DLB_ECMP_PLA_QUALITY_MEASURE_CONTROLr,
        measure_control_reg, PLA_CAP_QSIZE_AVGf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_config_set
 * Purpose:
 *      Set per-chip LAG dynamic load balancing configuration parameters.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (IN) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_tr3_ecmp_dlb_config_set(int unit, bcm_switch_control_t type, int arg)
{
    switch (type) {
        case bcmSwitchEcmpDynamicSampleRate:
            return _bcm_tr3_ecmp_dlb_sample_rate_set(unit, arg);

        case bcmSwitchEcmpDynamicAccountingSelect:
            return _bcm_tr3_ecmp_dlb_accounting_set(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesExponent:
            return _bcm_tr3_ecmp_dlb_tx_load_weight_set(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesExponent:
            return _bcm_tr3_ecmp_dlb_qsize_weight_set(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesDecreaseReset:
            return _bcm_tr3_ecmp_dlb_tx_load_cap_set(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesDecreaseReset:
            return _bcm_tr3_ecmp_dlb_qsize_cap_set(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesMinThreshold:
            return _bcm_tr3_ecmp_dlb_tx_load_min_th_set(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesMaxThreshold:
            return _bcm_tr3_ecmp_dlb_tx_load_max_th_set(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesMinThreshold:
            return _bcm_tr3_ecmp_dlb_qsize_min_th_set(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesMaxThreshold:
            return _bcm_tr3_ecmp_dlb_qsize_max_th_set(unit, arg);

        case bcmSwitchEcmpDynamicExpectedLoadMinThreshold:
            return _bcm_tr3_ecmp_dlb_exp_load_min_th_set(unit, arg);

        case bcmSwitchEcmpDynamicExpectedLoadMaxThreshold:
            return _bcm_tr3_ecmp_dlb_exp_load_max_th_set(unit, arg);

        case bcmSwitchEcmpDynamicImbalanceMinThreshold:
            return _bcm_tr3_ecmp_dlb_imbalance_min_th_set(unit, arg);

        case bcmSwitchEcmpDynamicImbalanceMaxThreshold:
            return _bcm_tr3_ecmp_dlb_imbalance_max_th_set(unit, arg);

        default:
            return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_config_get
 * Purpose:
 *      Get per-chip LAG dynamic load balancing configuration parameters.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (OUT) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_tr3_ecmp_dlb_config_get(int unit, bcm_switch_control_t type, int *arg)
{
    switch (type) {
        case bcmSwitchEcmpDynamicSampleRate:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate;
            break;

        case bcmSwitchEcmpDynamicAccountingSelect:
            return _bcm_tr3_ecmp_dlb_accounting_get(unit, arg);
            break;

        case bcmSwitchEcmpDynamicEgressBytesExponent:
            return _bcm_tr3_ecmp_dlb_tx_load_weight_get(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesExponent:
            return _bcm_tr3_ecmp_dlb_qsize_weight_get(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesDecreaseReset:
            return _bcm_tr3_ecmp_dlb_tx_load_cap_get(unit, arg);

        case bcmSwitchEcmpDynamicQueuedBytesDecreaseReset:
            return _bcm_tr3_ecmp_dlb_qsize_cap_get(unit, arg);

        case bcmSwitchEcmpDynamicEgressBytesMinThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th;
            break;

        case bcmSwitchEcmpDynamicEgressBytesMaxThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th;
            break;

        case bcmSwitchEcmpDynamicQueuedBytesMinThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th;
            break;

        case bcmSwitchEcmpDynamicQueuedBytesMaxThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th;
            break;

        case bcmSwitchEcmpDynamicExpectedLoadMinThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th;
            break;

        case bcmSwitchEcmpDynamicExpectedLoadMaxThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th;
            break;

        case bcmSwitchEcmpDynamicImbalanceMinThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th;
            break;

        case bcmSwitchEcmpDynamicImbalanceMaxThreshold:
            *arg = ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th;
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_quality_map_profile_init
 * Purpose:
 *      Initialize ECMP DLB quality mapping profile.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int 
_bcm_tr3_ecmp_dlb_quality_map_profile_init(int unit) 
{
    soc_profile_mem_t *profile;
    soc_mem_t mem;
    int entry_words;
    int entries_per_profile;
    int tx_load_percent;
    int alloc_size;
    uint32 *entry_arr;
    int rv = BCM_E_NONE;
    void *entries;
    uint32 base_index;
    int i;
    int member_id;
    dlb_ecmp_quality_control_entry_t quality_control_entry;

    if (NULL == _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile) {
        _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile =
            sal_alloc(sizeof(soc_profile_mem_t),
                      "ECMP DLB Quality Map Profile Mem");
        if (NULL == _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile) {
            return BCM_E_MEMORY;
        }
    } else {
        soc_profile_mem_destroy(unit,
                _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile);
    }
    profile = _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile;
    soc_profile_mem_t_init(profile);

    mem = DLB_ECMP_QUALITY_MAPPINGm;
    entry_words = sizeof(dlb_ecmp_quality_mapping_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, &entry_words, 1, profile));

    entries_per_profile = 64;
    if (!SOC_WARM_BOOT(unit)) {
        alloc_size = entries_per_profile * entry_words * sizeof(uint32);
        entry_arr = sal_alloc(alloc_size, "ECMP DLB Quality Map entries");
        if (entry_arr == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(entry_arr, 0, alloc_size);

        /* Assign quality in the entry array, with 100% weight for port load,
         * 0% for queue size.
         */
        tx_load_percent = 100;
        rv = _bcm_tr3_ecmp_dlb_quality_assign(unit, tx_load_percent,
                entry_arr);
        if (BCM_FAILURE(rv)) {
            sal_free(entry_arr);
            return rv;
        }

        entries = entry_arr;
        rv = soc_profile_mem_add(unit, profile, &entries, entries_per_profile,
                &base_index);
        sal_free(entry_arr);
        if (SOC_FAILURE(rv)) {
            return rv;
        }

        for (member_id = 0;
                member_id < soc_mem_index_count(unit,
                    DLB_ECMP_QUALITY_CONTROLm);
                member_id++) {
            BCM_IF_ERROR_RETURN
                (READ_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                                               member_id,
                                               &quality_control_entry));
            soc_DLB_ECMP_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, PROFILE_PTRf,
                    base_index / entries_per_profile);
            BCM_IF_ERROR_RETURN
                (WRITE_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ALL,
                                                member_id,
                                                &quality_control_entry));
        }  

        for (i = 0; i < entries_per_profile; i++) {
            SOC_PROFILE_MEM_REFERENCE(unit, profile, base_index + i,
                    member_id - 1);
        }

        ECMP_DLB_INFO(unit)->
            ecmp_dlb_load_weight[base_index/entries_per_profile] = 
                tx_load_percent;

    } else { /* Warm boot, recover reference counts and entries_per_set */

        for (member_id = 0;
                member_id < soc_mem_index_count(unit,
                    DLB_ECMP_QUALITY_CONTROLm);
                member_id++) {
            BCM_IF_ERROR_RETURN
                (READ_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                                               member_id,
                                               &quality_control_entry));
            base_index = soc_DLB_ECMP_QUALITY_CONTROLm_field32_get(unit,
                    &quality_control_entry, PROFILE_PTRf);
            base_index *= entries_per_profile;
            for (i = 0; i < entries_per_profile; i++) {
                SOC_PROFILE_MEM_REFERENCE(unit, profile, base_index + i, 1);
                SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, profile,
                        base_index + i, entries_per_profile);
            }
        }  
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_deinit
 * Purpose:
 *      Deallocate _tr3_ecmp_dlb_bk
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
void
_bcm_tr3_ecmp_dlb_deinit(int unit) 
{
    int rv;
    uint32 rsel2_hw_control_reg;

    if (_tr3_ecmp_dlb_bk[unit]) {
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_nh_info) {
            sal_free(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_nh_info);
            _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_nh_info = NULL;
        }
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap) {
            sal_free(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap);
            _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap = NULL;
        }
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap) {
            sal_free(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap);
            _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap = NULL;
        }
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_member_id_used_bitmap) {
            sal_free(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_member_id_used_bitmap);
            _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_member_id_used_bitmap = NULL;
        }
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight) {
            sal_free(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight);
            _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight = NULL;
        }
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile) {
            soc_profile_mem_destroy(unit,
                    _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile);
            sal_free(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile);
            _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_quality_map_profile = NULL;
        }

        sal_free(_tr3_ecmp_dlb_bk[unit]);
        _tr3_ecmp_dlb_bk[unit] = NULL;
    }

    rv = READ_RSEL2_HW_CONTROLr(unit, &rsel2_hw_control_reg);
    if (SOC_SUCCESS(rv)) {
        soc_reg_field_set(unit, RSEL2_HW_CONTROLr, &rsel2_hw_control_reg,
                ENABLE_ECMP_DLBf, 0);
        (void)WRITE_RSEL2_HW_CONTROLr(unit, rsel2_hw_control_reg);
    }
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_init
 * Purpose:
 *      Allocate and initialize _tr3_ecmp_dlb_bk
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_tr3_ecmp_dlb_init(int unit) 
{
    int rv = BCM_E_NONE;
    int num_ecmp_dlb_id;
    int flowset_tbl_size;
    int total_num_blocks;
    int num_ecmp_dlb_member_id;
    int num_quality_map_profiles;
    int member_id;
    dlb_ecmp_quality_control_entry_t quality_control_entry;
    uint32 rsel2_hw_control_reg;
    uint32 measure_control_reg;
    int max_ecmp_dlb_next_hops;
    int sample_rate;

    _bcm_tr3_ecmp_dlb_deinit(unit);

    if (_tr3_ecmp_dlb_bk[unit] == NULL) {
        _tr3_ecmp_dlb_bk[unit] = sal_alloc(
                sizeof(_tr3_ecmp_dlb_bookkeeping_t), "_tr3_ecmp_dlb_bk");
        if (_tr3_ecmp_dlb_bk[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_ecmp_dlb_bk[unit], 0, sizeof(_tr3_ecmp_dlb_bookkeeping_t));

    max_ecmp_dlb_next_hops = soc_mem_index_count(unit,
            DLB_ECMP_MEMBER_ATTRIBUTEm);
    if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_nh_info == NULL) {
        _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_nh_info = 
            sal_alloc(sizeof(_tr3_ecmp_dlb_nh_info_t) * max_ecmp_dlb_next_hops,
                    "ecmp_dlb_nh_info");
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_nh_info == NULL) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_nh_info, 0,
            sizeof(_tr3_ecmp_dlb_nh_info_t) * max_ecmp_dlb_next_hops);
    _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_nh_info_size = max_ecmp_dlb_next_hops;

    num_ecmp_dlb_id = soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm);
    if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap == NULL) {
        _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap = 
            sal_alloc(SHR_BITALLOCSIZE(num_ecmp_dlb_id),
                    "ecmp_dlb_id_used_bitmap");
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap == NULL) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_id_used_bitmap, 0,
            SHR_BITALLOCSIZE(num_ecmp_dlb_id));

    flowset_tbl_size = soc_mem_index_count(unit, DLB_ECMP_FLOWSETm);
    /* Each bit in ecmp_dlb_flowset_block_bitmap corresponds to a block of 512
     * flow set table entries.
     */
    total_num_blocks = flowset_tbl_size / 512;
    if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap == NULL) {
        _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap = 
            sal_alloc(SHR_BITALLOCSIZE(total_num_blocks),
                    "ecmp_dlb_flowset_block_bitmap");
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap == NULL) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_flowset_block_bitmap, 0,
            SHR_BITALLOCSIZE(total_num_blocks));

    num_ecmp_dlb_member_id = soc_mem_index_count(unit,
            DLB_ECMP_MEMBER_ATTRIBUTEm);
    if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_member_id_used_bitmap == NULL) {
        _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_member_id_used_bitmap = 
            sal_alloc(SHR_BITALLOCSIZE(num_ecmp_dlb_member_id),
                    "ecmp_dlb_member_id_used_bitmap");
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_member_id_used_bitmap == NULL) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_member_id_used_bitmap, 0,
                SHR_BITALLOCSIZE(num_ecmp_dlb_member_id));

    num_quality_map_profiles = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUALITY_CONTROLm, PROFILE_PTRf);
    if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight == NULL) {
        _tr3_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight = 
            sal_alloc(num_quality_map_profiles * sizeof(uint8),
                    "ecmp_dlb_load_weight");
        if (_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight == NULL) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr3_ecmp_dlb_bk[unit]->ecmp_dlb_load_weight, 0,
            num_quality_map_profiles * sizeof(uint8));

    /* Initialize port quality mapping profile */
    rv = _bcm_tr3_ecmp_dlb_quality_map_profile_init(unit);
    if (BCM_FAILURE(rv)) {
        _bcm_tr3_ecmp_dlb_deinit(unit);
        return rv;
    }

    if (!SOC_WARM_BOOT(unit)) {

        /* Configure accounting mode to physical link accounting */
        rv = _bcm_tr3_ecmp_dlb_config_set(unit,
                bcmSwitchEcmpDynamicAccountingSelect, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Set DLB sampling period and thresholds */
        sample_rate = 1000;
        rv = _bcm_tr3_ecmp_dlb_sample_rate_pla_thresholds_set(unit, sample_rate,
                125, 875);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }
        rv = _bcm_tr3_ecmp_dlb_sample_rate_vla_thresholds_set(unit, sample_rate,
                125, 875, -75, 75);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB load EWMA weight */
        rv = _bcm_tr3_ecmp_dlb_config_set(unit,
                bcmSwitchEcmpDynamicEgressBytesExponent, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB queue size EWMA weight */
        rv = _bcm_tr3_ecmp_dlb_config_set(unit,
                bcmSwitchEcmpDynamicQueuedBytesExponent, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB load cap control */
        rv = _bcm_tr3_ecmp_dlb_config_set(unit,
                bcmSwitchEcmpDynamicEgressBytesDecreaseReset, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB queue size cap control */
        rv = _bcm_tr3_ecmp_dlb_config_set(unit,
                bcmSwitchEcmpDynamicQueuedBytesDecreaseReset, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure LAG DLB queue size thresholds */
        rv = _bcm_tr3_ecmp_dlb_qsize_thresholds_set(unit, 0, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Disable link status override by software */
        rv = soc_mem_clear(unit, DLB_ECMP_MEMBER_SW_STATEm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Clear group membership */
        rv = soc_mem_clear(unit, DLB_ECMP_GROUP_MEMBERSHIPm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Clear port<->member mapping tables */
        rv = soc_mem_clear(unit, DLB_ECMP_MEMBERSHIP_REVERSE_MAPm,
                MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }
        rv = soc_mem_clear(unit, DLB_ECMP_MEMBER_ATTRIBUTEm, MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure per member threshold scaling factor and 
         * quality measure update control.
         */
        for (member_id = 0;
                member_id < soc_mem_index_count(unit,
                    DLB_ECMP_QUALITY_CONTROLm);
                member_id++) {
            rv = READ_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ANY,
                    member_id, &quality_control_entry);
            if (SOC_FAILURE(rv)) {
                _bcm_tr3_ecmp_dlb_deinit(unit);
                return rv;
            }
            soc_DLB_ECMP_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, ENABLE_AVG_CALCf, 1);
            soc_DLB_ECMP_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, ENABLE_QUALITY_UPDATEf, 1);
            soc_DLB_ECMP_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, ENABLE_CREDIT_COLLECTIONf, 1);
            soc_DLB_ECMP_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, LOADING_SCALING_FACTORf, 1);
            soc_DLB_ECMP_QUALITY_CONTROLm_field32_set(unit,
                    &quality_control_entry, QSIZE_SCALING_FACTORf, 1);
            rv = WRITE_DLB_ECMP_QUALITY_CONTROLm(unit, MEM_BLOCK_ALL,
                    member_id, &quality_control_entry);
            if (SOC_FAILURE(rv)) {
                _bcm_tr3_ecmp_dlb_deinit(unit);
                return rv;
            }
        }

        /* Enable DLB HGT refresh */
        rv = READ_RSEL2_HW_CONTROLr(unit, &rsel2_hw_control_reg);
        if (BCM_SUCCESS(rv)) {
            soc_reg_field_set(unit, RSEL2_HW_CONTROLr, &rsel2_hw_control_reg,
                    ENABLE_ECMP_DLBf, 1);
            rv = WRITE_RSEL2_HW_CONTROLr(unit, rsel2_hw_control_reg);
        }
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

        /* Configure Ethertype eligibility */
        rv = soc_mem_clear(unit, DLB_ECMP_ETHERTYPE_ELIGIBILITY_MAPm,
                MEM_BLOCK_ALL, 0);
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }
        rv = READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit,
                &measure_control_reg);
        if (SOC_SUCCESS(rv)) {
            soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                    &measure_control_reg,
                    ETHERTYPE_ELIGIBILITY_CONFIGf, 0);
            soc_reg_field_set(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
                    &measure_control_reg,
                    INNER_OUTER_ETHERTYPE_SELECTIONf, 0);
            rv = WRITE_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit,
                    measure_control_reg);
        }
        if (BCM_FAILURE(rv)) {
            _bcm_tr3_ecmp_dlb_deinit(unit);
            return rv;
        }

    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_host_stat_attach
 * Description:
 *      Attach counters entries to the given L3 host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) L3 host description  
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      On BCM56640, the L3 host tables (ISM tables L3_ENTRY_2 and L3_ENTRY_4, 
 *      ESM table EXT_L3_UCAST_DATA_WIDE) may contain a flex stat pointer when 
 *      entry has a dereference next hop (no NEXT_HOP pointer, but the NH info 
 *      instead).  This counter is analogous to what would be in the egress 
 *      object, but is implemented in the ingress and so requires an ingress 
 *      counter pool allocated.
 */
bcm_error_t
_bcm_tr3_l3_host_stat_attach(
            int unit,
            bcm_l3_host_t *info,
            uint32 stat_counter_id)
{
    soc_mem_t                 table[4]={0};
    uint32                    flex_table_index=0;
    bcm_stat_flex_direction_t direction=bcmStatFlexDirectionIngress;
    uint32                    pool_number=0;
    uint32                    base_index=0;
    bcm_stat_flex_mode_t      offset_mode=0;
    bcm_stat_object_t         object=bcmStatObjectIngPort;
    bcm_stat_group_mode_t     group_mode= bcmStatGroupModeSingle;
    uint32                    count=0;
    uint32                     actual_num_tables=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode,&object,&offset_mode,&pool_number,&base_index);

    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,object,&direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit,group_mode));

    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit,object,4,&actual_num_tables,&table[0],&direction));

    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_host_stat_get_table_info(
                   unit, info,&num_of_tables,&table_info[0]));
    for (count=0; count < num_of_tables ; count++) {
         for (flex_table_index=0; 
              flex_table_index < actual_num_tables ; 
              flex_table_index++) {
              if ((table_info[count].direction == direction) &&
                  (table_info[count].table == table[flex_table_index]) ) {
                  if (direction == bcmStatFlexDirectionIngress) {
                      return _bcm_esw_stat_flex_attach_ingress_table_counters(
                             unit,
                             table_info[count].table,
                             table_info[count].index,
                             offset_mode,
                             base_index,
                             pool_number);
                  } else {
                      return _bcm_esw_stat_flex_attach_egress_table_counters(
                             unit,
                             table_info[count].table,
                             table_info[count].index,
                             0,
                             offset_mode,
                             base_index,
                             pool_number);
                  }
              }
         }
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_tr3_l3_host_stat_detach
 * Description:
 *      Detach counters entries to the given L3 host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info		 - (IN) L3 host description
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t
_bcm_tr3_l3_host_stat_detach(
            int unit,
            bcm_l3_host_t *info)
{
    uint32                     count=0;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    bcm_error_t                rv = BCM_E_NONE;
    bcm_error_t                err_code[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION] = {BCM_E_NONE};

    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_host_stat_get_table_info(
                   unit, info,&num_of_tables,&table_info[0]));

    for (count=0; count < num_of_tables ; count++) {
      if (table_info[count].direction == bcmStatFlexDirectionIngress) {
           rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionIngress]) {
                err_code[bcmStatFlexDirectionIngress] = rv;
           }
      } else {
           rv =  _bcm_esw_stat_flex_detach_egress_table_counters(
                          unit,
                          0,
                          table_info[count].table,
                          table_info[count].index);
           if (BCM_E_NONE != rv &&
                BCM_E_NONE == err_code[bcmStatFlexDirectionEgress]) {
                err_code[bcmStatFlexDirectionEgress] = rv;
           }
      }
    }

    BCM_STAT_FLEX_DETACH_RETURN(err_code)
}

/*
 * Function:
 *      _bcm_tr3_l3_host_stat_counter_get
 * Description:
 *      Get counter statistic values for specific L3 host entry
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) hwcount is to be synced to sw count 
 *      info             - (IN) L3 host description
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t 
_bcm_tr3_l3_host_stat_counter_get(int unit,
                                  int sync_mode,
                                  bcm_l3_host_t *info,
                                  bcm_l3_stat_t stat,
                                  uint32 num_entries,
                                  uint32 *counter_indexes,
                                  bcm_stat_value_t *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmL3StatInPackets) ||
        (stat == bcmL3StatInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         /* direction = bcmStatFlexDirectionEgress; */
         return BCM_E_PARAM;
    }
    if (stat == bcmL3StatInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1; 
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_host_stat_get_table_info(
                   unit, info,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
               /*ctr_offset_info.offset_index = counter_indexes[index_count];*/
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                         unit, sync_mode,
                         table_info[table_count].index,
                         table_info[table_count].table,
                         0,
                         byte_flag,
                         counter_indexes[index_count],
                         &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_host_stat_counter_set
 * Description:
 *      Set counter statistic values for specific L3 host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) L3 host description
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t 
_bcm_tr3_l3_host_stat_counter_set(
            int unit,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
    uint32                          table_count=0;
    uint32                          index_count=0;
    uint32                          num_of_tables=0;
    bcm_stat_flex_direction_t       direction=bcmStatFlexDirectionIngress;
    uint32                          byte_flag=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    if ((stat == bcmL3StatInPackets) ||
        (stat == bcmL3StatInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         /* direction = bcmStatFlexDirectionEgress; */
         return BCM_E_PARAM;
    }
    if (stat == bcmL3StatInPackets) {
        byte_flag=0;
    } else {
        byte_flag=1; 
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_host_stat_get_table_info(
                   unit, info,&num_of_tables,&table_info[0]));

    for (table_count=0; table_count < num_of_tables ; table_count++) {
      if (table_info[table_count].direction == direction) {
          for (index_count=0; index_count < num_entries ; index_count++) {
            BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                unit,
                                table_info[table_count].index,
                                table_info[table_count].table,
                                0,
                                byte_flag,
                                counter_indexes[index_count],
                                &counter_values[index_count]));
          }
      }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_host_stat_id_get
 * Description:
 *      Get stat counter id associated with given L3 host entry
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) L3 host description
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t 
_bcm_tr3_l3_host_stat_id_get(
            int unit,
            bcm_l3_host_t *info,
            bcm_l3_stat_t stat,
            uint32 *stat_counter_id)
{
    bcm_stat_flex_direction_t  direction=bcmStatFlexDirectionIngress;
    uint32                     num_of_tables=0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index=0;
    uint32                     num_stat_counter_ids=0;

    if ((stat == bcmL3StatInPackets) ||
        (stat == bcmL3StatInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         /* direction = bcmStatFlexDirectionEgress; */
         return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l3_host_stat_get_table_info(
                        unit,info,&num_of_tables,&table_info[0]));
    for (index=0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids,stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_tr3_l3_multicast_nh_add
 * Purpose:
 *      Set L3 multicast next hop entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      nh_index - (IN) Next hop index. 
 *      l3_egress - (IN) L3 egress structure. 
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_tr3_l3_ipmc_nh_add(int unit, int nh_index,
        bcm_l3_egress_t *l3_egress) 
{
    int rv = BCM_E_NONE;
    int nh_index_out;
    ing_l3_next_hop_entry_t ing_nh;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
#ifdef BCM_TOMAHAWK3_SUPPORT
    egr_l3_next_hop_2_entry_t egr_nh_2;
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    int large_scale_nat_profile_mac = 0;
#endif
    int i;
    int flag_set;
    int macda_index = -1, use_configured_mac = 0;
#ifdef BCM_RIOT_SUPPORT
    uint32 nh_dest = 0;
    int vp;
#endif
    uint32 flag_array[] = {
        BCM_L3_MULTICAST_L2_DEST_PRESERVE,
        BCM_L3_MULTICAST_L2_SRC_PRESERVE,
        BCM_L3_MULTICAST_L2_VLAN_PRESERVE,
        BCM_L3_MULTICAST_TTL_PRESERVE,
        BCM_L3_MULTICAST_DEST_PRESERVE,
        BCM_L3_MULTICAST_SRC_PRESERVE,
        BCM_L3_MULTICAST_VLAN_PRESERVE,
        BCM_L3_MULTICAST_L3_DROP,
        BCM_L3_MULTICAST_L2_DROP
    };
    soc_field_t field_array[] = {
        L3MC__L2_MC_DA_DISABLEf,
        L3MC__L2_MC_SA_DISABLEf,
        L3MC__L2_MC_VLAN_DISABLEf,
        L3MC__L3_MC_TTL_DISABLEf,
        L3MC__L3_MC_DA_DISABLEf,
        L3MC__L3_MC_SA_DISABLEf,
        L3MC__L3_MC_VLAN_DISABLEf,
        L3MC__L3_DROPf,
        L3MC__L2_DROPf
    };
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
    soc_field_t field = INVALIDf, field2 = INVALIDf;
#endif
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_HURRICANE4_SUPPORT)
    bcm_xflow_macsec_port_info_t port_info;
    bcm_xflow_macsec_port_info_t_init(&port_info);

    if (SOC_IS_HURRICANE4(unit) &&
        soc_feature(unit, soc_feature_xflow_macsec)) {
        bcm_common_xflow_macsec_port_info_get(unit, l3_egress->port, &port_info);
    }
#endif
    if (l3_egress->encap_id == 0) {
        /* encap_id does not contain a VP or a L2 egress object ID. Hence,
         * a L3 egress object is being created on a physical port.
         */
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            rv = _bcm_th_ipmc_l3_intf_next_hop_get(unit, l3_egress->intf,
                                                   &nh_index_out);
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            rv = bcm_tr3_ipmc_l3_intf_next_hop_get(unit, l3_egress->intf,
                                                   &nh_index_out);
        }
        if (rv == BCM_E_NONE) {
            /* IPMC module had already silently allocated a next hop index for the
             * L3 interface, as a result of user adding the L3 interface to an IPMC
             * group directly. Creating a L3 egress object based on such an L3
             * interface is not allowed.
             */
            return BCM_E_CONFIG;
        } else if (rv == BCM_E_NOT_FOUND) {
            /* IPMC module has not silently allocated a next hop index for the L3
             * interface. Inform the IPMC module that a next hop index has been
             * allocated for the L3 interface by the bcm_l3_egress_create API.
             * This is to prevent IPMC module from silently allocating another
             * next hop index when user attempts to add the same L3 interface
             * directly to an IPMC group. Such an attempt will result in BCM_E_CONFIG.
             */
#if defined(BCM_TOMAHAWK_SUPPORT)
            if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th_ipmc_l3_intf_next_hop_l3_egress_set(unit,
                                                                 l3_egress->intf));
            } else 
#endif /* BCM_TOMAHAWK_SUPPORT */
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_ipmc_l3_intf_next_hop_l3_egress_set(unit, 
                                                                 l3_egress->intf));
            }
        } else {
            return rv;
        }
    }

    /* Configure the ingress next hop entry. Although ingress next hop entry
     * is not used by IPMC packet flow, storing port and vlan info in it
     * allows retrieval by bcm_l3_egress_get function.
     */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm,
                MEM_BLOCK_ANY, nh_index, &ing_nh));
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
            VLAN_IDf, l3_egress->vlan);

#ifdef BCM_RIOT_SUPPORT
    /*
    * TD2+ has a separate encoding values for different destinations.
    * If Riot in not enabled then we may need to change this to
    * modid + port combination.
    */
    if (soc_feature(unit, soc_feature_ing_l3_next_hop_encoded_dest)) {
        BCM_IF_ERROR_RETURN(
            bcmi_l3_nh_dest_set(unit, l3_egress, &nh_dest));
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DESTINATIONf,
                nh_dest);
    } else
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_generic_dest)) {
        /* This can handle non-RIOT cases as well */
        bcmi_td3_l3_nh_dest_set(unit, l3_egress, &nh_dest);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DESTINATIONf,
            nh_dest);
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* BCM_RIOT_SUPPORT */
    {
        /* Set module id. */
        if (l3_egress->flags & BCM_L3_TGID) {
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf, 1);
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, TGIDf,
                l3_egress->port);
        } else {
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf, 0);
            if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm, MODULE_IDf)) {
                soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, MODULE_IDf,
                    l3_egress->module);
            }
            soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, PORT_NUMf,
                l3_egress->port);
        }
    }

    /* Keep initial ingress next hop entry consistent with
     * ingress next hop entry */
    if (SOC_MEM_IS_VALID(unit, INITIAL_ING_L3_NEXT_HOPm)) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, INITIAL_ING_L3_NEXT_HOPm,
                MEM_BLOCK_ANY, nh_index, &initial_ing_nh));
    }

#ifdef BCM_RIOT_SUPPORT
   /*
    * TD2+ has a separate encoding values for different destinations.
    * If Riot in not enabled then we may need to change this to
    * modid + port combination.
    */
    if (soc_feature(unit, soc_feature_ing_l3_next_hop_encoded_dest)) {
        BCM_IF_ERROR_RETURN(
            bcmi_l3_nh_dest_set(unit, l3_egress, &nh_dest));
            soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm, &initial_ing_nh, DESTINATIONf,
                nh_dest);
    } else
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_generic_dest)) {
        bcmi_td3_l3_nh_dest_set(unit, l3_egress, &nh_dest);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm, &initial_ing_nh, DESTINATIONf,
            nh_dest);
    } else
#endif  /* BCM_TRIDENT3_SUPPORT */
#endif  /* BCM_RIOT_SUPPORT */
    if (SOC_MEM_IS_VALID(unit, INITIAL_ING_L3_NEXT_HOPm)) {
        if (l3_egress->flags & BCM_L3_TGID) {
            soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm, &initial_ing_nh,
                Tf, 1);
            soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm, &initial_ing_nh,
                TGIDf, l3_egress->port);
        } else {
            soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm, &initial_ing_nh,
                Tf, 0);
            soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm, &initial_ing_nh,
                MODULE_IDf, l3_egress->module);
            soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm, &initial_ing_nh,
                PORT_NUMf, l3_egress->port);
        }
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flow) &&
        (l3_egress->flow_handle != 0)) {

        SOC_IF_ERROR_RETURN(
             _bcm_td3_l3_flex_nh_add(unit, nh_index, l3_egress, (uint32 *)&egr_nh, 1));

        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ALL, nh_index, &ing_nh));

        if (SOC_MEM_IS_VALID(unit, INITIAL_ING_L3_NEXT_HOPm)) {
            SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                MEM_BLOCK_ALL, nh_index, &initial_ing_nh));
        }
        return BCM_E_NONE;
    }
#endif
    /* Configure the egress next hop entry */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                MEM_BLOCK_ANY, nh_index, &egr_nh));
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, DATA_TYPEf, 7);
    } else
#endif
    {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, ENTRY_TYPEf, 3);
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOP_2m,
                        MEM_BLOCK_ANY, nh_index, &egr_nh_2));
        } else
#endif
        {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, ENTRY_TYPEf, 7);
        }
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_MEM_IS_VALID(unit, EGR_L3_NEXT_HOP_2m)) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOP_2m, &egr_nh_2,
                INTF_NUMf, l3_egress->intf);
    } else
#endif
    {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                L3MC__INTF_NUMf, l3_egress->intf);
    }

    for (i = 0; i < COUNTOF(flag_array); i++) {
        if (l3_egress->multicast_flags & flag_array[i]) {
            flag_set = 1;
        } else {
            flag_set = 0;
        }
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, field_array[i])) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    field_array[i], flag_set);
        } else {
            if (flag_set) {
                return BCM_E_PARAM;
            }
        }
    }

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_virtual_port_routing)) {
        int vp;
        bcm_niv_port_t niv_port;
        int count;
        bcm_niv_egress_t niv_egress;
        int multicast_flag;
        int virtual_interface_id;
        bcm_extender_port_t eport;
        int l2_egr_nh_index;
        egr_l3_next_hop_entry_t l2_egr_nh;
        int etag_dot1p_mapping_ptr = 0;
        bcm_extender_egress_t extender_egress;
        uint16 extended_port_vid;
        uint8 pcp, de;
        uint32 pcp_de_select;
        int qos_map_id;

        if (l3_egress->encap_id > 0 &&
                l3_egress->encap_id < BCM_XGS3_EGRESS_IDX_MIN(unit)) {
            vp = l3_egress->encap_id;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__NEXT_PTR_TYPEf, 1);
            } else 
#endif
            {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__DVP_VALIDf, 1);
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    L3MC__DVPf, vp);
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                bcm_niv_port_t_init(&niv_port);
                BCM_GPORT_NIV_PORT_ID_SET(niv_port.niv_port_id, vp);
                BCM_IF_ERROR_RETURN
                    (bcm_esw_niv_port_get(unit, &niv_port));
                if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
                    BCM_IF_ERROR_RETURN(bcm_esw_niv_egress_get(unit,
                                niv_port.niv_port_id, 1, &niv_egress, &count));
                    if (count == 0) {
                        /* No NIV egress object has been added to VP yet. */
                        return BCM_E_CONFIG;
                    }
                    multicast_flag = (niv_egress.flags & BCM_NIV_EGRESS_MULTICAST) ? 1 : 0;
                    if (multicast_flag) {
                        /* Multiple multicast NIV egress object can be
                         * overloaded onto the same VP. In such cases,
                         * L3 egress object should be created on individual
                         * NIV egress object, not the overloaded NIV VP.
                         */
                        return BCM_E_PARAM;
                    }
                    virtual_interface_id = niv_egress.virtual_interface_id;
                } else {
                    multicast_flag = (niv_port.flags & BCM_NIV_PORT_MULTICAST) ? 1 : 0;
                    virtual_interface_id = niv_port.virtual_interface_id;
                }
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__VNTAG_ACTIONSf, 1); /* Add or replace VNTAG */
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__VNTAG_DST_VIFf, virtual_interface_id);
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__VNTAG_Pf, multicast_flag);
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__VNTAG_FORCE_Lf, 0);
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                bcm_extender_port_t_init(&eport);
                BCM_GPORT_EXTENDER_PORT_ID_SET(eport.extender_port_id, vp);
                BCM_IF_ERROR_RETURN
                    (bcm_esw_extender_port_get(unit, &eport));
                if (eport.flags & BCM_EXTENDER_PORT_MATCH_NONE) {
                    bcm_extender_egress_t_init(&extender_egress);
                    BCM_IF_ERROR_RETURN(bcm_esw_extender_egress_get_all(unit,
                                eport.extender_port_id, 1, &extender_egress, &count));
                    if (count == 0) {
                        /* No Extender egress object has been added to VP yet. */
                        return BCM_E_CONFIG;
                    }
                    if (extender_egress.flags & BCM_EXTENDER_EGRESS_MULTICAST) {
                        return BCM_E_PARAM;
                    }
                    qos_map_id        = extender_egress.qos_map_id;
                    extended_port_vid = extender_egress.extended_port_vid;
                    pcp_de_select     = extender_egress.pcp_de_select;
                    pcp               = extender_egress.pcp;
                    de                = extender_egress.de;
                } else {
                    qos_map_id        = eport.qos_map_id;
                    extended_port_vid = eport.extended_port_vid;
                    pcp_de_select     = eport.pcp_de_select;
                    pcp               = eport.pcp;
                    de                = eport.de;
                }

                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__VNTAG_ACTIONSf, 2); /* Add or replace ETAG */
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__VNTAG_DST_VIFf, extended_port_vid);
                if (pcp_de_select == BCM_EXTENDER_PCP_DE_SELECT_DEFAULT) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__ETAG_PCP_DE_SOURCEf, 2);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__ETAG_PCPf, pcp);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__ETAG_DEf, de);
                } else if (pcp_de_select == BCM_EXTENDER_PCP_DE_SELECT_PHB) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__ETAG_PCP_DE_SOURCEf, 3);
                    bcm_td2_qos_egr_etag_id2profile(unit, qos_map_id,
                                                    &etag_dot1p_mapping_ptr);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__ETAG_DOT1P_MAPPING_PTRf, etag_dot1p_mapping_ptr);
                }
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__VNTAG_ACTIONSf, 0); /* NOOP */
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
            } else {
                return BCM_E_PARAM;
            }
        } else if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, l3_egress->encap_id)) {
            uint32 entry_type;
            /* Creating a L3 egress objet on a L2 egress objet */
            l2_egr_nh_index = l3_egress->encap_id - BCM_XGS3_EGRESS_IDX_MIN(unit);
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                        MEM_BLOCK_ANY, l2_egr_nh_index, &l2_egr_nh));
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
                entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &l2_egr_nh,
                                                              DATA_TYPEf);
            } else
#endif
            {
                entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &l2_egr_nh,
                                                              ENTRY_TYPEf);
            }
            if (2 != entry_type) {
                return BCM_E_PARAM;
            }
            vp = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &l2_egr_nh,
                    SD_TAG__DVPf);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                return BCM_E_PARAM;
            }
            multicast_flag = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                    &l2_egr_nh, SD_TAG__VNTAG_Pf);
            if (!multicast_flag) {
                return BCM_E_PARAM;
            }
            virtual_interface_id = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                    &l2_egr_nh, SD_TAG__VNTAG_DST_VIFf);

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__NEXT_PTR_TYPEf, 1);
            } else 
#endif
            {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__DVP_VALIDf, 1);
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    L3MC__DVPf, vp);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    L3MC__VNTAG_ACTIONSf, 1); /* Add or replace VNTAG */
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    L3MC__VNTAG_DST_VIFf, virtual_interface_id);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    L3MC__VNTAG_Pf, multicast_flag);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    L3MC__VNTAG_FORCE_Lf, 0);
        } else {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__NEXT_PTR_TYPEf, 0);
            } else 
#endif
            {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__DVP_VALIDf, 0);
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                    L3MC__VNTAG_ACTIONSf, 0); /* NOOP */
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_RIOT_SUPPORT
    /*
     * TD2Plus and TD3 have multi layer routing.
     * The multicast packet can be routed in the overlay
     * layer and then underlay encapsulation is performed.
     * To achieve this, there will be overlay egr nexthop association
     * with undelray egress next hop.
     * the overlay egr nexthop will point to underlay next hop.
     */
    if (BCMI_RIOT_IS_ENABLED(unit) &&
        _bcm_vp_vfi_type_vp_get(unit, l3_egress->port, &vp)) {
        /* This is a overlay NH case */
        ing_dvp_table_entry_t dvp;
        int vp_nh_index;
        /* Get the vp */
        SOC_IF_ERROR_RETURN
                (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        /* read vp to get next hop */
        vp_nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                NEXT_HOP_INDEXf);

        /*
         * check if nh-nh association is needed.
         * else set up nh-dvp association
         * NH-NH association will be done for all the
         * nhs other than SDTAG which does not modify any packets field.
         * e.g. on access ports, it may be needed to change a vlan/tpid,
         * in that case, we may have to set nh-nh association else we may
         * setup vlan/tpid from DVP.
         */
         if (bcmi_l3_nh_assoc_ol_ul_link_is_required(unit, vp_nh_index)) {
             soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                 L3MC__NEXT_PTR_TYPEf, BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH);
             soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                 L3MC__NEXT_PTRf, vp_nh_index);
             /* Set up the association of NHs in sw */
             BCM_IF_ERROR_RETURN(
                 bcmi_l3_nh_assoc_ol_ul_link_sw_add(unit, nh_index,
                 vp_nh_index));
         } else {
             soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                 L3MC__NEXT_PTR_TYPEf, BCMI_L3_NH_EGR_NEXT_PTR_TYPE_DVP);
             soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                 L3MC__NEXT_PTRf, vp);
         }
    }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_large_scale_nat) &&
        l3_egress->replication_id != 0 &&
        soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__EH_QUEUE_TAGf) &&
        soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__MAC_DA_PROFILE_INDEXf) &&
        soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__USE_MAC_DA_PROFILEf)) {
        /* large scale MC nat uses EH_QUEUE_TAGf with MAC_DA_PROFILE_INDEXf */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            L3MC__EH_QUEUE_TAGf, l3_egress->replication_id);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            L3MC__EH_TAG_TYPEf, 0x1);
        large_scale_nat_profile_mac = 1;
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit) &&
        soc_feature(unit, soc_feature_xflow_macsec) &&
        ((l3_egress->flags2 & BCM_L3_FLAGS2_XFLOW_MACSEC_ENCRYPT) ||
        (l3_egress->flags2 & BCM_L3_FLAGS2_XFLOW_MACSEC_DECRYPT))) {
        uint32 entry_type;
        egr_mac_da_profile_entry_t macda;
        void *entries[1] = { NULL };

        if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, DATA_TYPEf)) {
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                          DATA_TYPEf);
        } else
        {
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                          ENTRY_TYPEf);
        }
        switch (entry_type) {
            case 7:
                field = L3MC__MACSEC_ENCRYPTf;
                field2 = L3MC__MACSEC_DECRYPTf;
                break;
            default:
                break;
        }
        if ((field != INVALIDf) && (field2 != INVALIDf)) {
            if (l3_egress->flags2 & BCM_L3_FLAGS2_XFLOW_MACSEC_ENCRYPT) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, field, 1);
            } else {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, field, 0);
            }

            if (l3_egress->flags2 & BCM_L3_FLAGS2_XFLOW_MACSEC_DECRYPT) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, field2, 1);
            } else {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, field2, 0);
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_MC_DST_MODIDf, l3_egress->module);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_MC_DST_PORT_NUMf, l3_egress->port);

            sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
            soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, 
                                 &macda, MAC_ADDRESSf, l3_egress->mac_addr);
            entries[0] = &macda;
            rv = _bcm_mac_da_profile_entry_add(unit, entries, 1,
                                               (uint32 *)&macda_index);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__MAC_DA_PROFILE_INDEXf, macda_index);

            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__USE_MAC_DA_PROFILEf, 1);
            use_configured_mac = 1;
        }
    } else
#endif
#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_HURRICANE4_SUPPORT)
    if (SOC_IS_HURRICANE4(unit) &&
        soc_feature(unit, soc_feature_xflow_macsec) &&
        port_info.enable) {
        egr_mac_da_profile_entry_t macda;
        void *entries[1] = { NULL };
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__HG_MC_DST_MODIDf, l3_egress->module);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__HG_MC_DST_PORT_NUMf, l3_egress->port);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__HG_CHANGE_DESTINATIONf, 1);

        sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
        soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm,
                             &macda, MAC_ADDRESSf, l3_egress->mac_addr);
        entries[0] = &macda;
        rv = _bcm_mac_da_profile_entry_add(unit, entries, 1,
                                           (uint32 *)&macda_index);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__MAC_DA_PROFILE_INDEXf, macda_index);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__USE_MAC_DA_PROFILEf, 1);
        use_configured_mac = 1;
    } else
#endif
    /*
     * TD2Plus has a overlaid  mac_address.
     * This mac_address field may be overlaid by some other fields
     * used for CoE/HG feature. Meanwhile a mac_da_profile_index
     * is present for the actual mac_address.
     * This index points to EGR_MAC_DA_PROFILE, which is managed in
     * Profile mechanism.
     */
#ifdef BCM_HGPROXY_COE_SUPPORT
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                            L3MC__MAC_DA_PROFILE_INDEXf) &&
        _bcm_xgs5_subport_coe_mod_port_local(unit, l3_egress->module,
                                             l3_egress->port)) {
        egr_mac_da_profile_entry_t macda;
        void *entries[1] = { NULL };

        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__USE_MAC_DA_PROFILEf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__USE_MAC_DA_PROFILEf, 1);
        }
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__HG_MC_DST_MODIDf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_MC_DST_MODIDf, l3_egress->module);
        }
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                L3MC__HG_MC_DST_PORT_NUMf)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_MC_DST_PORT_NUMf, l3_egress->port);
        }
        if (soc_feature(unit, soc_feature_channelized_switching)) {
            /* Set HG change destination fied to pick HG destination. */
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__HG_CHANGE_DESTINATIONf, 1);
        }


        sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
        soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, 
                             &macda, MAC_ADDRESSf, l3_egress->mac_addr);
        entries[0] = &macda;
        rv = _bcm_mac_da_profile_entry_add(unit, entries, 1,
                                           (uint32 *)&macda_index);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            L3MC__MAC_DA_PROFILE_INDEXf, macda_index);
        use_configured_mac = 1;
            
    } else
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (large_scale_nat_profile_mac == 1) {
        egr_mac_da_profile_entry_t macda;
        void *entries[1] = { NULL };

        sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
        soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm,
            &macda, MAC_ADDRESSf, l3_egress->mac_addr);
        entries[0] = &macda;
        rv = _bcm_mac_da_profile_entry_add(unit, entries, 1,
            (uint32 *)&macda_index);

        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            L3MC__MAC_DA_PROFILE_INDEXf, macda_index);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            L3MC__USE_MAC_DA_PROFILEf, 1);
        use_configured_mac = 1;
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__MAC_ADDRESSf)) {
        soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                L3MC__MAC_ADDRESSf, l3_egress->mac_addr);
        use_configured_mac = 1;
    } else {    
        if (!BCM_MAC_IS_ZERO(l3_egress->mac_addr)) {
            return BCM_E_PARAM;
        }
    }
    if (use_configured_mac) {
        if (BCM_MAC_IS_ZERO(l3_egress->mac_addr)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L3MC_USE_CONFIGURED_MACf, 0);
        } else {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                L3MC__L3MC_USE_CONFIGURED_MACf, 1);
        }
    }

    rv = soc_mem_write(unit, ING_L3_NEXT_HOPm,
        MEM_BLOCK_ALL, nh_index, &ing_nh);

    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    if (SOC_MEM_IS_VALID(unit, INITIAL_ING_L3_NEXT_HOPm)) {
        rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, nh_index, &initial_ing_nh);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, &egr_nh);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_MEM_IS_VALID(unit, EGR_L3_NEXT_HOP_2m)) {
        rv = soc_mem_write(unit, EGR_L3_NEXT_HOP_2m,
                           MEM_BLOCK_ALL, nh_index, &egr_nh_2);
    }
#endif
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    return BCM_E_NONE;

cleanup:
    if (macda_index != -1) {
        (void) _bcm_mac_da_profile_entry_delete(unit, macda_index);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l3_ipmc_nh_entry_parse
 * Purpose:
 *      Parse L3 multicast next hop entry.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      ing_nh    - (IN) Ingress next hop table entry. 
 *      egr_nh    - (IN) Egress next hop table entry. 
 *      l3_egress - (OUT) L3 egress structure. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_ipmc_nh_entry_parse(int unit, uint32 *ing_nh, 
        uint32 *egr_nh, uint32 *egr_nh_2, bcm_l3_egress_t *l3_egress)
{
    bcm_module_t module = 0, module_out = 0;
    bcm_port_t port = 0, port_out = 0;
    int i;
    uint32 flag_array[] = {
        BCM_L3_MULTICAST_L2_DEST_PRESERVE,
        BCM_L3_MULTICAST_L2_SRC_PRESERVE,
        BCM_L3_MULTICAST_L2_VLAN_PRESERVE,
        BCM_L3_MULTICAST_TTL_PRESERVE,
        BCM_L3_MULTICAST_DEST_PRESERVE,
        BCM_L3_MULTICAST_SRC_PRESERVE,
        BCM_L3_MULTICAST_VLAN_PRESERVE,
        BCM_L3_MULTICAST_L3_DROP,
        BCM_L3_MULTICAST_L2_DROP
    };
    soc_field_t field_array[] = {
        L3MC__L2_MC_DA_DISABLEf,
        L3MC__L2_MC_SA_DISABLEf,
        L3MC__L2_MC_VLAN_DISABLEf,
        L3MC__L3_MC_TTL_DISABLEf,
        L3MC__L3_MC_DA_DISABLEf,
        L3MC__L3_MC_SA_DISABLEf,
        L3MC__L3_MC_VLAN_DISABLEf,
        L3MC__L3_DROPf,
        L3MC__L2_DROPf
    };


#ifdef BCM_RIOT_SUPPORT
    if (soc_feature(unit, soc_feature_ing_l3_next_hop_encoded_dest)) {
        uint32 nh_dest = 0;
        nh_dest = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                      ing_nh, DESTINATIONf);
        bcmi_get_port_from_destination(unit, nh_dest, l3_egress);
        if (l3_egress->flags == BCM_L3_TGID) {
            l3_egress->trunk = l3_egress->port;
        }
    } else
#endif
    {
        /* Get trunk or module and port */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_generic_dest)) {
            uint32 dest_val = 0, dest_type = 0;
            int dvp_gport = 0;
            dest_val = soc_mem_field32_dest_get(unit, ING_L3_NEXT_HOPm, ing_nh,
                                                DESTINATIONf, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_LAG) {
                l3_egress->flags |= BCM_L3_TGID;
                l3_egress->trunk = dest_val & SOC_MEM_FIF_DGPP_TGID_MASK;
            } else if (dest_type == SOC_MEM_FIF_DEST_DGPP) {
                port = (dest_val & SOC_MEM_FIF_DGPP_PORT_MASK);
                module = (dest_val & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                            SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit,
                    BCM_STK_MODMAP_GET, module, port, &module_out, &port_out));
                l3_egress->module = module_out;
                l3_egress->port = port_out;
            } else if (dest_type == SOC_MEM_FIF_DEST_DVP) {
                _bcm_vp_encode_gport(unit, dest_val, &dvp_gport);
                l3_egress->port = dvp_gport;
            }
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            if (soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, ing_nh, Tf)) {
                l3_egress->flags |= BCM_L3_TGID;
                l3_egress->trunk = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                                       ing_nh, TGIDf);
            } else {
                port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, ing_nh,
                                           PORT_NUMf);
                if (!SOC_IS_TOMAHAWK3(unit)) {
                    module = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, ing_nh,
                                                 MODULE_IDf);

                    BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    module, port, &module_out, &port_out));
                } else {
                    module_out = 0;
                    port_out = port;
                }
                l3_egress->module = module_out;
                l3_egress->port = port_out;
            }
        }
    }

    /* Get VLAN */
    l3_egress->vlan = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, ing_nh,
            VLAN_IDf);

    /* Get L3 interface */
    if (SOC_IS_TOMAHAWK3(unit)) {
        l3_egress->intf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOP_2m, egr_nh_2,
                INTF_NUMf);
    } else {
        l3_egress->intf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                L3MC__INTF_NUMf);
    }

    /* Get L3 multicast flags */
    for (i = 0; i < COUNTOF(field_array); i++) {
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, field_array[i])) {
            if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                        field_array[i])) {
                l3_egress->multicast_flags |= flag_array[i];
            }
        }
    }

    /* Get MAC address */
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_large_scale_nat) &&
        soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__MAC_DA_PROFILE_INDEXf) &&
        soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__USE_MAC_DA_PROFILEf) &&
        soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh, L3MC__USE_MAC_DA_PROFILEf)) {
        egr_mac_da_profile_entry_t macda;
        int macda_index = -1;

        macda_index = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            L3MC__MAC_DA_PROFILE_INDEXf);

        /* Read entry from MAC-DA-PROFILEm */
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_MAC_DA_PROFILEm,
            MEM_BLOCK_ANY, macda_index, &macda));

        /* Get mac address from EGR_MACDA_PROFILE. */
        soc_mem_mac_addr_get(unit, EGR_MAC_DA_PROFILEm, &macda,
            MAC_ADDRESSf, l3_egress->mac_addr);
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__MAC_ADDRESSf)) {
        soc_mem_mac_addr_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                L3MC__MAC_ADDRESSf, l3_egress->mac_addr);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_large_scale_nat) &&
        soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__USE_MAC_DA_PROFILEf) &&
        soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, L3MC__EH_QUEUE_TAGf)) {
        if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
            L3MC__USE_MAC_DA_PROFILEf)) {
            l3_egress->replication_id = soc_mem_field32_get(unit,
                EGR_L3_NEXT_HOPm, egr_nh, L3MC__EH_QUEUE_TAGf);
        }
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_virtual_port_routing)) {
        int vp;
        int multicast_flag;
        int virtual_interface_id;
        bcm_niv_port_t niv_port;
        int count;
        bcm_niv_egress_t *niv_egress_array;
        int rv;
        int dvp_valid = 0;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
            dvp_valid = (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                    L3MC__NEXT_PTR_TYPEf) & 0x1);
        } else 
#endif
        {
            dvp_valid = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                    L3MC__DVP_VALIDf);
        }
        if (dvp_valid) {
            vp = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                    L3MC__DVPf);
            l3_egress->encap_id = vp;
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                /* If the VP is of NIV type, need to determine if a NIV
                 * egress object ID needs to be returned in encap_id.
                 */
                multicast_flag = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                        egr_nh, L3MC__VNTAG_Pf);
                virtual_interface_id = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                        egr_nh, L3MC__VNTAG_DST_VIFf);
                if (multicast_flag) {
                    bcm_niv_port_t_init(&niv_port);
                    BCM_GPORT_NIV_PORT_ID_SET(niv_port.niv_port_id, vp);
                    BCM_IF_ERROR_RETURN(bcm_esw_niv_port_get(unit, &niv_port));
                    if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
                        BCM_IF_ERROR_RETURN(bcm_esw_niv_egress_get(unit,
                                    niv_port.niv_port_id, 0, NULL, &count));
                        if (count > 0) {
                            niv_egress_array = sal_alloc
                                (count * sizeof(bcm_niv_egress_t),
                                 "niv_egress_array");
                            if (NULL == niv_egress_array) {
                                return BCM_E_MEMORY;
                            }
                            rv = bcm_esw_niv_egress_get(unit, niv_port.niv_port_id,
                                    count, niv_egress_array, &count);
                            if (BCM_FAILURE(rv)) {
                                sal_free(niv_egress_array);
                                return rv;
                            }
                            for (i = 0; i < count; i++) {
                                if (virtual_interface_id ==
                                    niv_egress_array[i].virtual_interface_id) {
                                    break;
                                }
                            }
                            if (i == count) {
                                sal_free(niv_egress_array);
                                return BCM_E_NOT_FOUND;
                            }
                            l3_egress->encap_id = niv_egress_array[i].egress_if;
                            sal_free(niv_egress_array);
                        } else {
                            return BCM_E_INTERNAL;
                        }
                    }
                }
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                        L3MC__L3_DROPf)) {
                    l3_egress->multicast_flags |= BCM_L3_MULTICAST_L3_DROP;
                }
                if (l3_egress->port == 0) {
                    l3_egress->flags2 |= BCM_L3_FLAGS2_UNDERLAY;
                }
            }
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    /* Set IPMC flag */
    l3_egress->flags |= BCM_L3_IPMC;

#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit) &&
        soc_feature(unit, soc_feature_xflow_macsec)) {
        if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, L3MC__MACSEC_ENCRYPTf) &&
            (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                L3MC__MACSEC_ENCRYPTf))) {
            l3_egress->flags2 |= BCM_L3_FLAGS2_XFLOW_MACSEC_ENCRYPT;
        }
        if (SOC_MEM_FIELD_VALID(unit, EGR_L3_NEXT_HOPm, L3MC__MACSEC_DECRYPTf) &&
            (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, egr_nh,
                L3MC__MACSEC_DECRYPTf))) {
            l3_egress->flags2 |= BCM_L3_FLAGS2_XFLOW_MACSEC_DECRYPT;
        }
    }
#endif
    return BCM_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      bcm_tr3_ecmp_dlb_wb_alloc_size_get
 * Purpose:
 *      Get level 2 warm boot scache size for ECMP DLB.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      size - (OUT) Allocation size.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ecmp_dlb_wb_alloc_size_get(int unit, int *size) 
{
    int alloc_size = 0;
    int num_elements;

    /* Allocate size for the following ECMP DLB parameters: 
     * int ecmp_dlb_sample_rate;
     * int ecmp_dlb_tx_load_min_th;
     * int ecmp_dlb_tx_load_max_th;
     * int ecmp_dlb_qsize_min_th;
     * int ecmp_dlb_qsize_max_th;
     * int ecmp_dlb_exp_load_min_th;
     * int ecmp_dlb_exp_load_max_th;
     * int ecmp_dlb_imbalance_min_th;
     * int ecmp_dlb_imbalance_max_th;
     */
    alloc_size += sizeof(int) * 9;

    /* Allocate size of ecmp_dlb_load_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUALITY_CONTROLm, PROFILE_PTRf);
    alloc_size += sizeof(uint8) * num_elements;

    *size = alloc_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ecmp_dlb_sync
 * Purpose:
 *      Store ECMP DLB parameters into scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ecmp_dlb_sync(int unit, uint8 **scache_ptr) 
{
    int value;
    int num_elements;
    int i;
    uint8 load_weight;

    /* Store the following ECMP DLB parameters: 
     * int ecmp_dlb_sample_rate;
     * int ecmp_dlb_tx_load_min_th;
     * int ecmp_dlb_tx_load_max_th;
     * int ecmp_dlb_qsize_min_th;
     * int ecmp_dlb_qsize_max_th;
     * int ecmp_dlb_exp_load_min_th;
     * int ecmp_dlb_exp_load_max_th;
     * int ecmp_dlb_imbalance_min_th;
     * int ecmp_dlb_imbalance_max_th;
     */
    value = ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    value = ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    (*scache_ptr) += sizeof(int);

    /* Store ecmp_dlb_load_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUALITY_CONTROLm, PROFILE_PTRf);
    for (i = 0; i < num_elements; i++) {
        load_weight = ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[i];
        sal_memcpy((*scache_ptr), &load_weight, sizeof(uint8));
        (*scache_ptr) += sizeof(uint8);
    }
        
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ecmp_dlb_scache_recover
 * Purpose:
 *      Recover ECMP DLB parameters from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ecmp_dlb_scache_recover(int unit, uint8 **scache_ptr) 
{
    int value;
    int num_elements;
    int i;
    uint8 load_weight;

    /* Recover the following ECMP DLB parameters: 
     * int ecmp_dlb_sample_rate;
     * int ecmp_dlb_tx_load_min_th;
     * int ecmp_dlb_tx_load_max_th;
     * int ecmp_dlb_qsize_min_th;
     * int ecmp_dlb_qsize_max_th;
     * int ecmp_dlb_exp_load_min_th;
     * int ecmp_dlb_exp_load_max_th;
     * int ecmp_dlb_imbalance_min_th;
     * int ecmp_dlb_imbalance_max_th;
     */
    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th = value;
    (*scache_ptr) += sizeof(int);

    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th = value;
    (*scache_ptr) += sizeof(int);

    /* Recover ecmp_dlb_load_weight array */
    num_elements = 1 << soc_mem_field_length(unit,
            DLB_ECMP_QUALITY_CONTROLm, PROFILE_PTRf);
    for (i = 0; i < num_elements; i++) {
        sal_memcpy(&load_weight, (*scache_ptr), sizeof(uint8));
        ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[i] = load_weight;
        (*scache_ptr) += sizeof(uint8);
    }
        
    ECMP_DLB_INFO(unit)->recovered_from_scache = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_member_recover
 * Purpose:
 *      Recover ECMP DLB member ID usage and the next hop indices associated
 *      with the member IDs.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_ecmp_dlb_member_recover(int unit) 
{
    int rv = BCM_E_NONE;
    int member_bitmap_width;
    int alloc_size;
    SHR_BITDCL *member_bitmap = NULL;
    int member_id;
    dlb_ecmp_member_attribute_entry_t member_attr_entry;
    int nh_index;
    int group;
    int i;
    dlb_ecmp_group_membership_entry_t group_membership_entry;
    int match, match_index, free_index_found, free_index;
    _tr3_ecmp_dlb_nh_membership_t *membership;

    member_bitmap_width = soc_mem_field_length(unit,
            DLB_ECMP_GROUP_MEMBERSHIPm, MEMBER_BITMAPf);
    alloc_size = SHR_BITALLOCSIZE(member_bitmap_width);
    member_bitmap = sal_alloc(alloc_size, "DLB ECMP member bitmap"); 
    if (NULL == member_bitmap) {
        return BCM_E_MEMORY;
    }

    for (member_id = 0; member_id < soc_mem_index_count(unit,
                DLB_ECMP_MEMBER_ATTRIBUTEm); member_id++) {
        rv = READ_DLB_ECMP_MEMBER_ATTRIBUTEm(unit, MEM_BLOCK_ANY,
                member_id, &member_attr_entry);
        if (BCM_FAILURE(rv)) {
            sal_free(member_bitmap);
            return rv;
        }
        nh_index = soc_DLB_ECMP_MEMBER_ATTRIBUTEm_field32_get(unit,
                &member_attr_entry, NEXT_HOP_INDEXf);
        if (0 == nh_index) {
            /* If next hop index is 0, this member ID is not used. */
            continue;
        }

        /* Member ID is used */
        _BCM_ECMP_DLB_MEMBER_ID_USED_SET(unit, member_id);

        /* Find the ECMP DLB group this member ID belongs to */
        group = -1;
        for (i = 0; i < soc_mem_index_count(unit,
                    DLB_ECMP_GROUP_MEMBERSHIPm); i++) {
            rv = READ_DLB_ECMP_GROUP_MEMBERSHIPm(unit,
                    MEM_BLOCK_ANY, i, &group_membership_entry);
            if (BCM_FAILURE(rv)) {
                sal_free(member_bitmap);
                return rv;
            }
            soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_get(unit,
                    &group_membership_entry, MEMBER_BITMAPf, member_bitmap);
            if (SHR_BITGET(member_bitmap, member_id)) {
                group = i;
                break;
            }
        }

        /* Traverse ecmp_dlb_nh_info array to find the nh_index */
        match = FALSE;
        match_index = 0;
        free_index_found = FALSE;
        free_index = 0;
        for (i = 0; i < ECMP_DLB_INFO(unit)->ecmp_dlb_nh_info_size; i++) {
            if (ECMP_DLB_NH_INFO(unit, i).valid) {
                if (nh_index == ECMP_DLB_NH_INFO(unit, i).nh_index) {
                    match = TRUE;
                    match_index = i;
                    break;
                }
            } else {
                if (!free_index_found) {
                    free_index_found = TRUE;
                    free_index = i;
                }
            }
        }

        /* Insert member ID and group to the next hop's linked list of member
         * IDs and groups.
         */
        membership = sal_alloc(sizeof(_tr3_ecmp_dlb_nh_membership_t),
                "ecmp dlb nh membership");
        if (NULL == membership) {
            sal_free(member_bitmap);
            return BCM_E_MEMORY;
        }
        membership->member_id = member_id;
        membership->group = group;
        if (match) {
            membership->next =
                ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list; 
            ECMP_DLB_NH_INFO(unit, match_index).nh_membership_list = membership;
        } else {
            if (!free_index_found) {
                return BCM_E_FULL;
            }
            membership->next =
                ECMP_DLB_NH_INFO(unit, free_index).nh_membership_list; 
            ECMP_DLB_NH_INFO(unit, free_index).nh_membership_list = membership;
            ECMP_DLB_NH_INFO(unit, free_index).nh_index = nh_index;
            ECMP_DLB_NH_INFO(unit, free_index).valid = 1;
        }
    }

    sal_free(member_bitmap);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_group_recover
 * Purpose:
 *      Recover ECMP DLB group usage and flowset table usage.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_ecmp_dlb_group_recover(int unit) 
{
    int i;
    ecmp_count_entry_t ecmp_group_entry;
    int group_enable, dlb_id;
    dlb_ecmp_group_control_entry_t group_control_entry;
    int entry_base_ptr, dynamic_size_encode, dynamic_size;
    int block_base_ptr, num_blocks;

    for (i = 0; i < soc_mem_index_count(unit, L3_ECMP_COUNTm); i++) {
        SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, i,
                    &ecmp_group_entry));
        group_enable = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_group_entry,
                GROUP_ENABLEf);
        if (0 == group_enable) {
            /* DLB not enabled */
            continue;
        }

        /* Get DLB group ID and marked it as used */
        dlb_id = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_group_entry,
                DLB_IDf);
        _BCM_ECMP_DLB_ID_USED_SET(unit, dlb_id);

        /* Get flow set table usage */
        SOC_IF_ERROR_RETURN(READ_DLB_ECMP_GROUP_CONTROLm(unit,
                    MEM_BLOCK_ANY, dlb_id, &group_control_entry));
        entry_base_ptr = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, FLOW_SET_BASEf);
        dynamic_size_encode = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, FLOW_SET_SIZEf);
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_ecmp_dlb_dynamic_size_decode(dynamic_size_encode,
                                                   &dynamic_size));
        block_base_ptr = entry_base_ptr >> 9;
        num_blocks = dynamic_size >> 9; 
        _BCM_ECMP_DLB_FLOWSET_BLOCK_USED_SET_RANGE(unit, block_base_ptr,
                num_blocks);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_ecmp_dlb_quality_parameters_recover
 * Purpose:
 *      Recover ECMP DLB parameters used to derive a member's quality.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_ecmp_dlb_quality_parameters_recover(int unit) 
{
    uint32 measure_control_reg;
    int num_time_units;
    dlb_ecmp_pla_quantize_threshold_entry_t quantize_threshold_entry;
    int min_th_bytes, min_th_cells;
    int max_th_bytes, max_th_cells;
    dlb_ecmp_vla_expected_loading_threshold_entry_t expected_loading_threshold_entry;
    int exp_load_min_th_kbytes, exp_load_max_th_kbytes;
    dlb_ecmp_vla_member_imbalance_threshold_entry_t imbalance_threshold_entry;
    int imbalance_min_th_kbytes, imbalance_max_th_kbytes;
    int entries_per_profile;
    int profile_ptr, base_index;
    dlb_ecmp_quality_mapping_entry_t quality_mapping_entry;
    int quality;

    if (ECMP_DLB_INFO(unit)->recovered_from_scache) {
        /* If already recovered from scache, do nothing. */ 
        return BCM_E_NONE;
    }

    /* Recover sampling rate */
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_QUALITY_MEASURE_CONTROLr(unit,
                &measure_control_reg));
    num_time_units = soc_reg_field_get(unit, DLB_ECMP_QUALITY_MEASURE_CONTROLr,
            measure_control_reg, HISTORICAL_SAMPLING_PERIODf);
    if (num_time_units > 0) {
        ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate = 1000000 / num_time_units;
    }

    /* Recover min and max load and queue size thresholds: 
     * Load threshold (in mbps)
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (sampling period in seconds) / 10^6
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (num_time_units * 1 us per time unit * 10^-6) / 10^6
     *     = (bytes per sampling period) * 8 / num_time_units 
     */ 
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 0, &quantize_threshold_entry));
    min_th_bytes = soc_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, THRESHOLD_HIST_LOADf); 
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th = (min_th_bytes << 3) /
        num_time_units;
    min_th_cells = soc_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, THRESHOLD_HIST_QSIZEf); 
    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th = min_th_cells * 208;

    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, soc_mem_index_max(unit,
                    DLB_ECMP_PLA_QUANTIZE_THRESHOLDm),
                &quantize_threshold_entry));
    max_th_bytes = soc_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, THRESHOLD_HIST_LOADf); 
    ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th = (max_th_bytes << 3) /
        num_time_units;
    max_th_cells = soc_DLB_ECMP_PLA_QUANTIZE_THRESHOLDm_field32_get
        (unit, &quantize_threshold_entry, THRESHOLD_HIST_QSIZEf); 
    ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th = max_th_cells * 208;

    /* Recover min and max expected load thresholds: 
     * Load threshold (in mbps)
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (sampling period in seconds) / 10^6
     *     = (bytes per sampling period) * 8 bits per byte /
     *       (num_time_units * 1 us per time unit * 10^-6) / 10^6
     *     = (bytes per sampling period) * 8 / num_time_units 
     *     = (kilobytes per sampling period) * 8000 / num_time_units 
     */ 
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 0, &expected_loading_threshold_entry));
    exp_load_min_th_kbytes =
        soc_DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm_field32_get
        (unit, &expected_loading_threshold_entry, THRESHOLDf); 
    ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th =
        (exp_load_min_th_kbytes << 3) * 1000 / num_time_units;

    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 14, &expected_loading_threshold_entry));
    exp_load_max_th_kbytes =
        soc_DLB_ECMP_VLA_EXPECTED_LOADING_THRESHOLDm_field32_get
        (unit, &expected_loading_threshold_entry, THRESHOLDf); 
    ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th =
        (exp_load_max_th_kbytes << 3) * 1000 / num_time_units;

    /* Recover min and max imbalance thresholds: 
     * Imbalance threshold (in percentage)
     *     = (imbalance threshold in kbytes) / 
     *       (expected load threshold in kbytes) * 100
     */
    SOC_IF_ERROR_RETURN(READ_DLB_ECMP_VLA_MEMBER_IMBALANCE_THRESHOLDm(unit,
                MEM_BLOCK_ANY, 14, &imbalance_threshold_entry));
    imbalance_min_th_kbytes =
        soc_DLB_ECMP_VLA_MEMBER_IMBALANCE_THRESHOLDm_field32_get
        (unit, &imbalance_threshold_entry, THRESHOLD_0f); 
    if ((imbalance_min_th_kbytes >> 20) & 0x1) {
        /* Imbalance threshold is negative. Take absolute value. */
        imbalance_min_th_kbytes = ((~imbalance_min_th_kbytes) + 1) & 0x1fffff; 
        ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th =
            imbalance_min_th_kbytes * 100 / (exp_load_max_th_kbytes) * -1;
    } else {
        ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th =
            imbalance_min_th_kbytes * 100 / (exp_load_max_th_kbytes);
    }

    imbalance_max_th_kbytes =
        soc_DLB_ECMP_VLA_MEMBER_IMBALANCE_THRESHOLDm_field32_get
        (unit, &imbalance_threshold_entry, THRESHOLD_6f); 
    if ((imbalance_max_th_kbytes >> 20) & 0x1) {
        /* Imbalance threshold is negative. Take absolute value. */
        imbalance_max_th_kbytes = ((~imbalance_max_th_kbytes) + 1) & 0x1fffff; 
        ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th =
            imbalance_max_th_kbytes * 100 / (exp_load_max_th_kbytes) * -1;
    } else {
        ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th =
            imbalance_max_th_kbytes * 100 / (exp_load_max_th_kbytes);
    }

    /* Recover load weights */
    entries_per_profile = 64;
    for (profile_ptr = 0; profile_ptr < (1 << soc_mem_field_length(unit,
                    DLB_ECMP_QUALITY_CONTROLm, PROFILE_PTRf)); profile_ptr++) {
        base_index = profile_ptr * entries_per_profile;
        /* quality = (quantized_tx_load * tx_load_percent +
         *            quantized_qsize * (100 - tx_load_percent)) /
         *           100;
         * quality = ((quantized_tx_load - quantized_qsize) *
         *            tx_load_percent + quantized_qsize * 100) /
         *           100;
         * quality = (quantized_tx_load - quantized_qsize) *
         *           tx_load_percent / 100 + quantized_qsize;
         * tx_load_percent = (quality - quantized_qsize) * 100 /
         *                   (quantized_tx_load - quantized_qsize); 
         *
         * The index to DLB_ECMP_QUALITY_MAPPING table consists of
         * base_index + quantized_tx_load * 8 + quantized_qsize,
         * where quantized_tx_load and quantized_qsize range from
         * 0 to 7.
         *
         * With quantized_tx_load = 7 and quantized_qsize = 0, the 
         * index to DLB_ECMP_QUALITY_MAPPING table is
         * base_index + 56, and the tx_load_percent expression
         * simplifies to quality * 100 / 7;
         */
        BCM_IF_ERROR_RETURN(READ_DLB_ECMP_QUALITY_MAPPINGm(unit,
                    MEM_BLOCK_ANY, base_index + 56,
                    &quality_mapping_entry));
        quality = soc_DLB_ECMP_QUALITY_MAPPINGm_field32_get(unit,
                &quality_mapping_entry, QUALITYf);
        ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[profile_ptr] =
            quality * 100 / 7;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_ecmp_dlb_hw_recover
 * Purpose:
 *      Recover ECMP DLB internal state from hardware.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr3_ecmp_dlb_hw_recover(int unit) 
{
    /* TD3TBD */
    if (SOC_IS_TRIDENT3X(unit)) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_member_recover(unit));
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_group_recover(unit));
    BCM_IF_ERROR_RETURN(_bcm_tr3_ecmp_dlb_quality_parameters_recover(unit));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l3_esm_host_state_recover
 * Purpose:
 *      Warmboot recovery for ESM host tbl state.
 *      This is a 'level one' recovery.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr3_l3_esm_host_state_recover(int unit) 
{
#if 0
    int tbl;
    soc_mem_t mem[_BCM_TR3_NUM_ESM_HOST_TABLES];

    mem[0] = BCM_XGS3_L3_MEM(unit, v4_esm);
    mem[1] = BCM_XGS3_L3_MEM(unit, v4_esm_wide);
    mem[2] = BCM_XGS3_L3_MEM(unit, v6_esm);
    mem[3] = BCM_XGS3_L3_MEM(unit, v6_esm_wide);

    for (tbl = 0; tbl < _BCM_TR3_NUM_ESM_HOST_TABLES; tbl++) {
    /* For each of the ESM tables, starting from zero,
     * check the index of the first EXT_TCAM_VBITm 
     * that is invalid. That is the first available 
     * free entry. Set _TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl) 
     * to this value */
    
    }
#endif
    
    return (BCM_E_NONE);
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE

/*
 * Function:
 *     _bcm_l3_sw_dump
 * Purpose:
 *     Displays L3 information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_tr3_ecmp_dlb_sw_dump(int unit)
{
    int i;
    _tr3_ecmp_dlb_nh_membership_t *current_ptr;
    int num_entries_per_profile;
    int num_profiles;
    int rv;
    int ref_count;

    LOG_CLI((BSL_META_U(unit,
                        "  ECMP DLB Info -\n")));

    /* Print ECMP DLB next hop info */
    LOG_CLI((BSL_META_U(unit,
                        "    ECMP DLB Next Hop Info -\n")));
    for (i = 0; i < ECMP_DLB_INFO(unit)->ecmp_dlb_nh_info_size; i++) {
        if (ECMP_DLB_NH_INFO(unit, i).valid) {
            LOG_CLI((BSL_META_U(unit,
                                "      Next hop index %d is "),
                     ECMP_DLB_NH_INFO(unit, i).nh_index));
            current_ptr = ECMP_DLB_NH_INFO(unit, i).nh_membership_list;
            while (NULL != current_ptr) {
                LOG_CLI((BSL_META_U(unit,
                                    "member %d in group %d"),
                         current_ptr->member_id, current_ptr->group));
                current_ptr = current_ptr->next;
                if (NULL == current_ptr) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        ", ")));
                }
            }
        }
    }

    /* Print ECMP DLB group usage */
    LOG_CLI((BSL_META_U(unit,
                        "    ECMP DLB Groups Used:")));
    for (i = 0; i < soc_mem_index_count(unit, DLB_ECMP_GROUP_CONTROLm); i++) {
        if (_BCM_ECMP_DLB_ID_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print ECMP DLB flowset table usage */
    LOG_CLI((BSL_META_U(unit,
                        "    ECMP DLB Flowset Table Blocks Used:")));
    for (i = 0; i < soc_mem_index_count(unit, DLB_ECMP_FLOWSETm) >> 9; i++) {
        if (_BCM_ECMP_DLB_FLOWSET_BLOCK_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print ECMP DLB member ID usage */
    LOG_CLI((BSL_META_U(unit,
                        "    ECMP DLB Member IDs Used:")));
    for (i = 0; i < soc_mem_index_count(unit, DLB_ECMP_MEMBER_ATTRIBUTEm); i++) {
        if (_BCM_ECMP_DLB_MEMBER_ID_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                " %d"), i));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Print sample rate and threshold parameters */
    LOG_CLI((BSL_META_U(unit,
                        "    Sample rate: %d per second\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_sample_rate));
    LOG_CLI((BSL_META_U(unit,
                        "    Tx load min threshold: %d mbps\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Tx load max threshold: %d mbps\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_tx_load_max_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Queue size min threshold: %d cells\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Queue size max threshold: %d cells\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_qsize_max_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Expected load min threshold: %d mbps\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Expected load max threshold: %d mbps\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_exp_load_max_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Imbalance min threshold: %d percent\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_min_th));
    LOG_CLI((BSL_META_U(unit,
                        "    Imbalance max threshold: %d percent\n"),
             ECMP_DLB_INFO(unit)->ecmp_dlb_imbalance_max_th));

    /* Print quality mapping profiles */
    LOG_CLI((BSL_META_U(unit,
                        "    Quality mapping profiles:\n")));
    num_entries_per_profile = 64;
    num_profiles = soc_mem_index_count(unit, DLB_ECMP_QUALITY_MAPPINGm) /
        num_entries_per_profile;
    for (i = 0; i < num_profiles; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "      Profile %d: load weight %d percent, "),
                 i, ECMP_DLB_INFO(unit)->ecmp_dlb_load_weight[i]));
        rv = soc_profile_mem_ref_count_get(unit, 
                ECMP_DLB_INFO(unit)->ecmp_dlb_quality_map_profile,
                i * num_entries_per_profile, &ref_count);
        if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "ref count %d\n"), ref_count));
    }

    return;
}

/*
 * Function:
 *     _bcm_tr3_esm_host_tbl_sw_dump
 * Purpose:
 *     Displays L3 information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void 
_bcm_tr3_esm_host_tbl_sw_dump(int unit) {

    int tbl;
    for (tbl = 0; tbl < _BCM_TR3_NUM_ESM_HOST_TABLES ; tbl++) {
        LOG_CLI((BSL_META_U(unit,
                            "ESM Host table %d, free entry: %d\n"), tbl, 
                 _TR3_ESM_HOST_TBL_FREE_ENTRY(unit, tbl)));    
    }
}

#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* INCLUDE_L3 && BCM_TRIUMPH3_SUPPORT  */
