/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Tunnel terminator TCAM table insert/delete/lookup routines
 * soc_tunnel_term_init    - Called from bcm_l3_init
 * soc_tunnel_term_insert  - Insert/Update an IPv4/IPV6 tunnel 
 *                           termination entry into L3_TUNNEL table
 * soc_tunnel_term_delete  - Delete an IPv4/IPV6 tunnel termination 
 *                           entry from L3_TUNNEL table
 * soc_tunnel_term_match  -  Exact match for the key. 
 *                           Will match  tunnel type (GRE/IP/MCAST) &&
 *                                                   (DIP & DIP_MASK)&&
 *                                                   (SIP & SIP_MASK)
 */

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>
#include <shared/util.h>
#include <shared/bsl.h>
#include <shared/avl.h>
#include <shared/l3.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/esw/flow_db.h>
#endif /*BCM_TRIDENT3_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
#if defined(INCLUDE_L3)

/* Source IP prefix lengths.   */
#define SOC_TNL_TERM_TYPE_SZ(_type_) ((_type_) * (_SHR_L3_IP6_MAX_NETLEN + 1))

#if defined(BCM_TRIDENT3_SUPPORT)
#define SOC_TNL_TERM_KEY_TYPE_FIXED_MAX               0x03
/*        TUNNEL KEY TYPE                             KEY TYPE PRIORITY */
#define SOC_TNL_TERM_TYPE_FLEX                        SOC_TNL_TERM_TYPE_SZ(0x20)
#endif /*BCM_TRIDENT3_SUPPORT */
/*        TUNNEL KEY TYPE                            KEY TYPE PRIORITY */
#define SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4DST_L4SRC  SOC_TNL_TERM_TYPE_SZ(0x1f)
#define SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT_L4DST_L4SRC  SOC_TNL_TERM_TYPE_SZ(0x1e)
#define SOC_TNL_TERM_TYPE_DIP4_PROT_L4DST_L4SRC       SOC_TNL_TERM_TYPE_SZ(0x1d)
#define SOC_TNL_TERM_TYPE_DIP6_PROT_L4DST_L4SRC       SOC_TNL_TERM_TYPE_SZ(0x1c)
#define SOC_TNL_TERM_TYPE_DIP4_SIP4_L4DST_L4SRC       SOC_TNL_TERM_TYPE_SZ(0x1b)
#define SOC_TNL_TERM_TYPE_DIP6_SIP6_L4DST_L4SRC       SOC_TNL_TERM_TYPE_SZ(0x1a)
#define SOC_TNL_TERM_TYPE_DIP4_L4DST_L4SRC            SOC_TNL_TERM_TYPE_SZ(0x19)
#define SOC_TNL_TERM_TYPE_DIP6_L4DST_L4SRC            SOC_TNL_TERM_TYPE_SZ(0x18)

#define SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4DST        SOC_TNL_TERM_TYPE_SZ(0x17)
#define SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT_L4DST        SOC_TNL_TERM_TYPE_SZ(0x16)
#define SOC_TNL_TERM_TYPE_DIP4_PROT_L4DST             SOC_TNL_TERM_TYPE_SZ(0x15)
#define SOC_TNL_TERM_TYPE_DIP6_PROT_L4DST             SOC_TNL_TERM_TYPE_SZ(0x14)
#define SOC_TNL_TERM_TYPE_DIP4_SIP4_L4DST             SOC_TNL_TERM_TYPE_SZ(0x13)
#define SOC_TNL_TERM_TYPE_DIP6_SIP6_L4DST             SOC_TNL_TERM_TYPE_SZ(0x12)
#define SOC_TNL_TERM_TYPE_DIP4_L4DST                  SOC_TNL_TERM_TYPE_SZ(0x11)
#define SOC_TNL_TERM_TYPE_DIP6_L4DST                  SOC_TNL_TERM_TYPE_SZ(0x10)

#define SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4SRC        SOC_TNL_TERM_TYPE_SZ(0xf)
#define SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT_L4SRC        SOC_TNL_TERM_TYPE_SZ(0xe)
#define SOC_TNL_TERM_TYPE_DIP4_PROT_L4SRC             SOC_TNL_TERM_TYPE_SZ(0xd)
#define SOC_TNL_TERM_TYPE_DIP6_PROT_L4SRC             SOC_TNL_TERM_TYPE_SZ(0xc)
#define SOC_TNL_TERM_TYPE_DIP4_SIP4_L4SRC             SOC_TNL_TERM_TYPE_SZ(0xb)
#define SOC_TNL_TERM_TYPE_DIP6_SIP6_L4SRC             SOC_TNL_TERM_TYPE_SZ(0xa)
#define SOC_TNL_TERM_TYPE_DIP4_L4SRC                  SOC_TNL_TERM_TYPE_SZ(9)
#define SOC_TNL_TERM_TYPE_DIP6_L4SRC                  SOC_TNL_TERM_TYPE_SZ(8)

#define SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT              SOC_TNL_TERM_TYPE_SZ(7)
#define SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT              SOC_TNL_TERM_TYPE_SZ(6)
#define SOC_TNL_TERM_TYPE_DIP4_PROT                   SOC_TNL_TERM_TYPE_SZ(5)
#define SOC_TNL_TERM_TYPE_DIP6_PROT                   SOC_TNL_TERM_TYPE_SZ(4)

#define SOC_TNL_TERM_TYPE_DIP4_SIP4                   SOC_TNL_TERM_TYPE_SZ(3)
#define SOC_TNL_TERM_TYPE_DIP6_SIP6                   SOC_TNL_TERM_TYPE_SZ(2)
#define SOC_TNL_TERM_TYPE_DIP4                        SOC_TNL_TERM_TYPE_SZ(1)
#define SOC_TNL_TERM_TYPE_DIP6                        SOC_TNL_TERM_TYPE_SZ(0)



/*
 * TCAM based tunnel termination implementation. 
 * Each table entry holds 1 IPV4 entry
 * 4 entries are required for IPv6 tunnel termination. 
 * IPV6 entry. 
 * Assumption Src + Dst + tunnel type is a unique
 * combination.  
 *
 *              MAX_TNL_TERM_TYPE
 * tunnel_term_index[0x20].begin ---> ===============================
 *                                 ==                              ==
 *                                 ==   0                          ==
 * tunnel_term_index[0x20].end   ---> ===============================
 *
 * tunnel_term_index[0x19].begin ---> ===============================
 *                                 == DIP, SIP, PROT, L4SRC, L4DST ==
 *                                 ==   IPV4  tunnels              ==
 * tunnel_term_index[0x19].end   ---> ===============================
 *
 * tunnel_term_index[0x18].begin ---> ===============================
 *                                 == DIP6, SIP6, PROT,L4SRC,L4DST ==
 *                                 ==   IPV6  tunnels              ==
 * tunnel_term_index[0x18].end   ---> ===============================
 *
 *
  ................... ETC ...................................... 
 *
 *
 * tunnel_term_index[4].begin ---> ===============================
 *                                 ==  DIP + SIP + DON'T CARE   ==
 *                                 ==   IPV4  tunnels           ==
 * tunnel_term_index[4].end   ---> ===============================
 *
 *
 *
 * tunnel_term_index[3].begin ---> ===============================
 *                                 ==   DIP6 + SIP6 + DON'T CARE==
 *                                 ==   IPV6  tunnels           ==
 * tunnel_term_index[3].end   ---> ===============================
 *
 *
 *
 * tunnel_term_index[2].begin ---> ===============================
 *                                 ==   DIP + DON'T CARE        ==
 *                                 ==   IPV4  tunnels           ==
 * tunnel_term_index[2].end   ---> ===============================
 *
 *
 *
 * tunnel_term_index[1].begin ---> ===============================
 *                                 ==   DIP6 + DON'T CARE       ==
 *                                 ==   IPV6  tunnels           ==
 * tunnel_term_index[1].end   ---> ===============================
 */


/* Can move to SOC Control structures */
STATIC soc_tnl_term_state_p soc_tnl_term_state[SOC_MAX_NUM_DEVICES];
static int soc_tnl_term_entry_count;
#if defined(BCM_TRIDENT3_SUPPORT)
#define MAX_TNL_TERM_TYPES               SOC_TNL_TERM_TYPE_SZ(0x22)
#else
#define MAX_TNL_TERM_TYPES               SOC_TNL_TERM_TYPE_SZ(0x21)
#endif

#define MAX_TNL_TERM_TYPE                (MAX_TNL_TERM_TYPES - 1)
#define SOC_TNL_TERM_INIT_CHECK(_u_)       (soc_tnl_term_state[(_u_)] != NULL)
#define SOC_TNL_TERM_STATE(_u_)            (soc_tnl_term_state[(_u_)])
#define SOC_TNL_TERM_STATE_START(_u_, _type_) \
    (soc_tnl_term_state[(_u_)][(_type_)].start)

#define SOC_TNL_TERM_STATE_END(_u_, _type_) \
    (soc_tnl_term_state[(_u_)][(_type_)].end)
#define SOC_TNL_TERM_STATE_PREV(_u_, _type_) \
    (soc_tnl_term_state[(_u_)][(_type_)].prev)
#define SOC_TNL_TERM_STATE_NEXT(_u_, _type_) \
    (soc_tnl_term_state[(_u_)][(_type_)].next)
#define SOC_TNL_TERM_STATE_VENT(_u_, _type_) \
    (soc_tnl_term_state[(_u_)][(_type_)].vent)
#define SOC_TNL_TERM_STATE_FENT(_u_, _type_) \
    (soc_tnl_term_state[(_u_)][(_type_)].fent)

/* LOCKS */
#define SOC_TNL_TERM_LOCK(_u_)       soc_mem_lock(_u_, ((SOC_MEM_IS_VALID(_u_, L3_TUNNEL_DOUBLEm))?L3_TUNNEL_DOUBLEm:L3_TUNNELm))
#define SOC_TNL_TERM_UNLOCK(_u_)     soc_mem_unlock(_u_, ((SOC_MEM_IS_VALID(_u_, L3_TUNNEL_DOUBLEm))?L3_TUNNEL_DOUBLEm:L3_TUNNELm))


#define SOC_TNL_TERM_KEY_WIDTH_MAX     (4) /* 4 words for IPv6  address. */

typedef int (*_soc_tnl_term_hash_compare_fn)(soc_tnl_term_hash_key_t *key1,
                                             soc_tnl_term_hash_key_t *key2);

STATIC soc_tnl_term_hash_t *_tnl_term_hash_tab[SOC_MAX_NUM_DEVICES];
int SOC_TNL_TERM_BLOCK_SIZE;
STATIC int
_soc_tunnel_term_type_get(int unit, soc_tunnel_term_t *entry,
                          int *type, int *entry_type);


#define SOC_TNL_TERM_STATE_HASH(_u_)        (_tnl_term_hash_tab[(_u_)])

#define TNL_TERM_TYPE_NULL        (-1)
#define TNL_TERM_HASH_INDEX_NULL  (0xFFFF)

/*
 * Purpose:
 *	 Set SOC_TNL_TERM_BLOCK_SIZE
 */ 

int soc_tunnel_term_block_size_set (int unit, int size)
{
    SOC_TNL_TERM_BLOCK_SIZE = size;
    return SOC_E_NONE;
}

/*
 * Purpose:
 *	 Get SOC_TNL_TERM_BLOCK_SIZE
 */ 

int soc_tunnel_term_block_size_get (int unit, int *size)
{
    *size = SOC_TNL_TERM_BLOCK_SIZE;
    return SOC_E_NONE;
}

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _soc_tunnel_term_flex_type_width_get
 * Purpose:
 *      Get the width of flex tunnel terminator entry.
 * Parameters:
 *      unit          - (IN)  Bcm device number.
 *      type          - (IN)  Tunnel terminator type.
 *      entry         - (IN)  Tunnel termination entry.
 *      entry_width   - (IN)  Entry width.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_tunnel_term_flex_type_width_get(int unit, int type, soc_tunnel_term_t *entry,
                                     int *entry_width) {

    uint32 mode = 0;
    if (entry_width == NULL) {
        return SOC_E_INTERNAL;
    }
    if (type != SOC_TNL_TERM_TYPE_FLEX) {
         return SOC_E_INTERNAL;
    }
    mode = soc_mem_field32_get(unit, L3_TUNNELm, (uint32 *)&entry->entry_arr[0], MODEf);
    if (mode == SOC_TNL_TERM_ENTRY_MODE_QUAD) {
       *entry_width = SOC_TNL_TERM_ENTRY_WIDTH_QUAD;
    } else if (mode == SOC_TNL_TERM_ENTRY_MODE_DOUBLE) {
       *entry_width = SOC_TNL_TERM_ENTRY_WIDTH_DOUBLE;
    }  else {
       *entry_width = SOC_TNL_TERM_ENTRY_WIDTH_SINGLE;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_tunnel_term_flex_hash_key_set
 * Purpose:
 *     Form the hash key for flex tunnel terminator entry.
 * Parameters:
 *      unit             - (IN)      Bcm device number.
 *      entry            - (IN)      Tunnel termination entry.
 *      flex_key_entry   - (IN/OUT)  Hash key.
 * Returns:
 *      SOC_E_XXX
 */
STATIC void
_soc_tunnel_term_flex_hash_key_set(int unit, soc_tunnel_term_t *entry,
                                uint32  *flex_key_entry) {
    soc_mem_t mem;
    soc_mem_t mem_view_id;
    uint32 key_type = SOC_FLOW_DB_KEY_TYPE_INVALID;
    uint32 data_type = SOC_FLOW_DB_KEY_TYPE_INVALID;
    uint32 key_typef[] = {KEY_TYPEf,
                         KEY_TYPE0f,
                         KEY_TYPE1f};
    uint32 data_typef[] = {DATA_TYPEf,
                          DATA_TYPE0f,
                          DATA_TYPE1f};
    uint32 key_list[SOC_FLOW_DB_MAX_VIEW_FIELDS];
    uint32 key_count = 0;
    uint32 key_value = 0;
    uint32 trav_entry[SOC_MAX_MEM_WORDS*4];
    uint32 entry_width = sizeof(tunnel_entry_t);
    uint32 i = 0;
    ip6_addr_t ip6;
    int rv = -1;
    mem = L3_TUNNELm;
    sal_memset(trav_entry, 0, sizeof(trav_entry));

    /* Get the view for the memory */
    for (i = 0; i < 4; i++) {
        sal_memcpy(((uint8 *)trav_entry + (entry_width * i)),
                   &entry->entry_arr[i], entry_width);
    }
    for (i = 0; i < SOC_FLOW_DB_MAX_KEY_TYPE; i++) {
        if (SOC_MEM_FIELD_VALID(unit,mem,key_typef[i])) {
             key_type = soc_mem_field32_get(unit, mem, trav_entry, key_typef[i]);
             break;
        }
    }

    for (i = 0; i < SOC_FLOW_DB_MAX_DATA_TYPE; i++) {
        if (SOC_MEM_FIELD_VALID(unit,mem,data_typef[i])) {
            data_type = soc_mem_field32_get(unit, mem, trav_entry, data_typef[i]);
        }
    }

    /* find the memory view based on the memory and key type */
    rv = soc_flow_db_mem_to_view_id_get (unit, mem, key_type, data_type,
                    0, NULL, (uint32 *) &mem_view_id);
    if (SOC_FAILURE(rv)) {
        return;
    }

    rv = soc_flow_db_mem_view_field_list_get(unit,
                mem_view_id,
                SOC_FLOW_DB_FIELD_TYPE_KEY,
                SOC_FLOW_DB_MAX_VIEW_FIELDS,
                key_list,
                &key_count);
    if (SOC_FAILURE(rv)) {
        return;
    }

    for(i = 0; i < key_count; i++) {
       if ((key_list[i] == IPV6__SIPf) ||
           (key_list[i] == IPV6__DIPf) ||
           (key_list[i] == IPV6__SIP_MASKf) ||
           (key_list[i] == IPV6__DIP_MASKf)) {
           sal_memset(&ip6, 0, sizeof(ip6_addr_t));
           soc_mem_ip6_addr_get(unit, mem_view_id,
                            trav_entry, key_list[i], ip6, 0);
           soc_mem_ip6_addr_set(unit, mem_view_id,
                            flex_key_entry, key_list[i], ip6, 0);
       } else if ((key_list[i] == KEY_0f) || 
                  (key_list[i] == KEY_1f) ) {
           /* ignore */
           continue;
       }
       key_value = soc_mem_field32_get(unit,
                                       mem_view_id,
                                       trav_entry,
                                       key_list[i]);
       soc_mem_field32_set(unit, mem_view_id,
                    flex_key_entry, key_list[i], key_value);
    }

    rv = soc_flow_db_mem_view_field_list_get(unit,
                mem_view_id,
                SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                SOC_FLOW_DB_MAX_VIEW_FIELDS,
                key_list,
                &key_count);
    if (SOC_FAILURE(rv)) {
        return;
    }

    for(i = 0; i < key_count; i++) {
        key_value = soc_mem_field32_get(unit,
                                       mem_view_id,
                                       trav_entry,
                                       key_list[i]);
        soc_mem_field32_set(unit, mem_view_id,
                flex_key_entry, key_list[i], key_value);
    }
    return;
}

/*
 * Function:
 *      _soc_tunnel_term_flex_entry_clear
 * Purpose:
 *      Clear flex tunnel termination entry to the hardware.
 * Parameters:
 *      unit          - (IN)  Bcm device number.
 *      index         - (IN)  Table index.
 *      entry         - (IN)  Tunnel termination entry.
 *      ipv6          - (IN)  Type.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_tunnel_term_flex_entry_clear(int unit, int index,
                             soc_tunnel_term_t *entry, int entry_type)
{
    int idx;       /* Entry width iteration index. */
    int idx_max;   /* Entry width.                 */
    int type;
    int key_type;
    soc_tunnel_term_t null_entry;
    soc_tunnel_term_t *entry_ptr = NULL;

    /* Input parameters check. */
    if (NULL == entry) {
        return (SOC_E_PARAM);
    }

    SOC_IF_ERROR_RETURN
        (_soc_tunnel_term_type_get(unit, entry, &type, &key_type));
    SOC_IF_ERROR_RETURN (
        _soc_tunnel_term_flex_type_width_get(unit, type, entry, &idx_max));
    /* null entry */
    sal_memset(&null_entry, 0, sizeof(soc_tunnel_term_t));
    entry_ptr = &null_entry;

    for (idx = 0; idx < idx_max; idx++) {
        SOC_IF_ERROR_RETURN (WRITE_L3_TUNNELm(unit, MEM_BLOCK_ALL,
                                              (idx + index),
                                              (uint32 *)&entry_ptr->entry_arr[idx]));
    }
    return (SOC_E_NONE);
}

#endif /*BCM_TRIDENT3_SUPPORT*/

/*
 * Function:
 *   _soc_tunnel_term_hash_entry_get
 * Purpose:
 *	 Extract key data from an entry at the given index.
 * Parameters:
 *      unit          - (IN) BCM device number. 
 *      entry_arr     - (IN) Array of entries forming single 
 *                           tunnel termination entry.  
 *      ipv6          - (IN) IPv4/IPv6 entry flag.  
 *      entry_key     - (OUT) Tunnel termination key. 
 * Returns: 
 *      none
 */ 
STATIC void
_soc_tunnel_term_hash_entry_get(int unit, soc_tunnel_term_t *entry, int entry_type,
                                soc_tnl_term_hash_key_t *entry_key)
{
    uint32 *entry_ptr;                       /* HW entry pointer.        */
    uint8 sip_mask[_SHR_L3_IP6_ADDRLEN];     /* Source ip subent mask.   */
    soc_mem_t mem = L3_TUNNELm;
    soc_field_t ipv4f = IP_ADDRf;
    soc_field_t ipv4_maskf = IP_ADDR_MASKf;
#if defined(BCM_TRIUMPH2_SUPPORT)
    soc_field_t l4_src_port_f = L4_SRC_PORTf; 
    soc_field_t l4_dest_port_f = L4_DEST_PORTf;
    soc_field_t protocol_f = PROTOCOLf;
#endif
    int ipv6_key_type = 0x1;
    int ipv4_key_type = 0x0;
    uint8 *ip6 = NULL;
    uint32              ip6_field[4];

    /* Input parameters check */
    if (NULL == entry_key) {
        return;
    }

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        mem = L3_TUNNEL_DOUBLEm;
        ipv6_key_type = 0x2;
        ipv4_key_type = 0x1;
#if defined(BCM_TRIUMPH2_SUPPORT)
        l4_src_port_f = IPV4__L4_SRC_PORTf;
        l4_dest_port_f = IPV4__L4_DEST_PORTf;
        protocol_f = IPV4__PROTOCOLf;
#endif
        if (entry_type == ipv6_key_type) {
            mem = L3_TUNNEL_QUADm;
#if defined(BCM_TRIUMPH2_SUPPORT)
            l4_src_port_f = IPV6__L4_SRC_PORTf;
            l4_dest_port_f = IPV6__L4_DEST_PORTf;
            protocol_f = IPV6__PROTOCOLf;
#endif
        }
    }

    /* Initialization part. */
    sal_memset(entry_key, 0, sizeof(soc_tnl_term_hash_key_t)); 
    entry_ptr = (uint32 *)&entry->entry_arr[0];

#if defined(BCM_TRIUMPH3_SUPPORT) || defined (BCM_KATANA2_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT)
    /* For Triumph3 get MIM and MPLS L3 tunnel entries */
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit) ||
        soc_feature(unit, soc_feature_l3_tunnel_mpls_frr)) {
        if (entry_type == 0x3) { /* MIM Type */
            /* Get MIM::SGLP */
            entry_key->mim_hash_key.sglp = 
                   soc_mem_field32_get(unit, L3_TUNNELm, entry_ptr, MIM__SGLPf);
            /* Get MIM::BVID */
            entry_key->mim_hash_key.bvid = 
                   soc_mem_field32_get(unit, L3_TUNNELm, entry_ptr, MIM__BVIDf);
            /* Get MIM::BMACSA */
            soc_mem_mac_addr_get(unit, L3_TUNNELm, entry_ptr, MIM__SGLPf, 
                          entry_key->mim_hash_key.bmacsa);
            return;

        } 
    } 
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_KATANA2_SUPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_KATANA2_SUPORT) || defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit) ||
            soc_feature(unit, soc_feature_l3_tunnel_mpls_frr)) {
        if (entry_type == 0x2) { /* MPLS Type */
            /* Get MPLS::MPLS_LABEL */
            entry_key->mpls_hash_key.mpls_label =
                    soc_mem_field32_get(unit, mem, entry_ptr,
                                     MPLS__MPLS_LABELf);
            /* Get MPLS::MODULE_ID */
            entry_key->mpls_hash_key.module_id =
                    soc_mem_field32_get(unit, mem, entry_ptr, MPLS__MODULE_IDf);
            /* Get MPLS::PORT_NUM */
            entry_key->mpls_hash_key.port =
                    soc_mem_field32_get(unit, mem, entry_ptr, MPLS__PORT_NUMf);
            /* Get MPLS::TGID */
            entry_key->mpls_hash_key.trunk_id =
                    soc_mem_field32_get(unit, mem, entry_ptr, MPLS__TGIDf);
            return;
        }
    }
#endif /* BCM_TOMAHAWK2_SUPPORT || BCM_TRIUMPH3_SUPPORT || BCM_KATANA2_SUPORT */

    if (entry_type == ipv6_key_type) { /* IPV6 Type */
        if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
            ip6 = entry_key->ip_hash_key.sip;
            /* SIP [0-89] */
            soc_mem_field_get(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__SIP_LWR_90f, ip6_field);

            ip6[4] = (ip6_field[2] & 0xff);
            ip6[5] = (ip6_field[2]  >> 8) & 0xff;
            ip6[6] = (ip6_field[2]  >> 16) & 0xff;
            ip6[7] = (ip6_field[2]  >> 24) & 0x3;

            ip6[8] = (ip6_field[1] & 0xff);
            ip6[9] = (ip6_field[1]  >> 8) & 0xff;
            ip6[10] = (ip6_field[1]  >> 16) & 0xff;
            ip6[11] = (ip6_field[1]  >> 24) & 0xff;

            ip6[12] = (ip6_field[0] & 0xff);
            ip6[13] = (ip6_field[0]  >> 8) & 0xff;
            ip6[14] = (ip6_field[0]  >> 16) & 0xff;
            ip6[15] = (ip6_field[0]  >> 24) & 0xff;

            /* SIP [90-127] */
            soc_mem_field_get(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__SIP_UPR_38f, ip6_field);
            ip6[0] = (ip6_field[0] & 0xff);
            ip6[1] = (ip6_field[1]  >> 8) & 0xff;
            ip6[2] = (ip6_field[2]  >> 16) & 0xff;
            ip6[3] = (ip6_field[3]  >> 24) & 0xff;

            ip6 = entry_key->ip_hash_key.dip;
            /* DIP [0-89] */
            soc_mem_field_get(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__DIP_LWR_90f, ip6_field);
            ip6[4] = (ip6_field[2] & 0xff);
            ip6[5] = (ip6_field[2]  >> 8) & 0xff;
            ip6[6] = (ip6_field[2]  >> 16) & 0xff;
            ip6[7] = (ip6_field[2]  >> 24) & 0x3;

            ip6[8] = (ip6_field[1] & 0xff);
            ip6[9] = (ip6_field[1]  >> 8) & 0xff;
            ip6[10] = (ip6_field[1]  >> 16) & 0xff;
            ip6[11] = (ip6_field[1]  >> 24) & 0xff;

            ip6[12] = (ip6_field[0] & 0xff);
            ip6[13] = (ip6_field[0]  >> 8) & 0xff;
            ip6[14] = (ip6_field[0]  >> 16) & 0xff;
            ip6[15] = (ip6_field[0]  >> 24) & 0xff;

            /* DIP [90-127] */
            soc_mem_field_get(unit, mem, (uint32 *)&entry->entry_arr[0], IPV6__DIP_UPR_38f, ip6_field);
            ip6[0] = (ip6_field[0] & 0xff);
            ip6[1] = (ip6_field[1]  >> 8) & 0xff;
            ip6[2] = (ip6_field[2]  >> 16) & 0xff;
            ip6[3] = (ip6_field[3]  >> 24) & 0xff;
            entry_key->ip_hash_key.sip_plen = 128;
        } else {
            /* SIP [0-63] */
            soc_mem_ip6_addr_get(unit, mem,
                                 (uint32 *)&entry->entry_arr[0], ipv4f,
                                 entry_key->ip_hash_key.sip, SOC_MEM_IP6_LOWER_ONLY);
            /* SIP [64-127] */
            soc_mem_ip6_addr_get(unit, mem,
                                 (uint32 *)&entry->entry_arr[1], ipv4f,
                                 entry_key->ip_hash_key.sip, SOC_MEM_IP6_UPPER_ONLY);
            /* DIP [0-63] */
            soc_mem_ip6_addr_get(unit, mem,
                                 (uint32 *)&entry->entry_arr[2], ipv4f,
                                 entry_key->ip_hash_key.dip, SOC_MEM_IP6_LOWER_ONLY);
            /* DIP [64-127] */
            soc_mem_ip6_addr_get(unit, mem,
                                 (uint32 *)&entry->entry_arr[3], ipv4f,
                                 entry_key->ip_hash_key.dip, SOC_MEM_IP6_UPPER_ONLY);

            /* SIP MASK [0-63] */
            soc_mem_ip6_addr_get(unit, mem,
                                 (uint32 *)&entry->entry_arr[0], ipv4_maskf,
                                 sip_mask, SOC_MEM_IP6_LOWER_ONLY);
            /* SIP MASK [64-127] */
            soc_mem_ip6_addr_get(unit, mem,
                                 (uint32 *)&entry->entry_arr[1], ipv4_maskf,
                                 sip_mask, SOC_MEM_IP6_UPPER_ONLY);
            entry_key->ip_hash_key.sip_plen =  _shr_ip6_mask_length(sip_mask);
        }
    } else if (entry_type == ipv4_key_type) { /* IPV4 Type */
        soc_field_t dipf = DIPf, sipf = SIPf;
        /* Get destination ip. */
        if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
            sipf = IPV4__SIPf;
            dipf = IPV4__DIPf;
        } 
        soc_mem_field_get(unit, mem, entry_ptr, dipf,
                          (uint32 *)entry_key->ip_hash_key.dip);

        /* Get source ip. */
        soc_mem_field_get(unit, mem, entry_ptr, sipf,
                          (uint32 *)entry_key->ip_hash_key.sip);

        /* Get source ip. */
        if (SOC_MEM_FIELD_VALID(unit, mem, SIP_MASKf)) {
            soc_mem_field_get(unit, mem, entry_ptr, SIP_MASKf,
                              (uint32 *)sip_mask);
            entry_key->ip_hash_key.sip_plen =  
                            _shr_ip_mask_length((*(uint32 *)sip_mask));
        } else {
            entry_key->ip_hash_key.sip_plen = 
                            (*(uint32 *)entry_key->ip_hash_key.sip) ? 32 : 0;
        }
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    else if (soc_feature(unit,soc_feature_flex_flow)
             && (entry_type > SOC_TNL_TERM_KEY_TYPE_FIXED_MAX)){
        /* Flex views */
        _soc_tunnel_term_flex_hash_key_set(unit, entry, entry_key->flex_hash_key.entry);
        return;
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (!soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
         l4_src_port_f = L4_SRC_PORTf;
         l4_dest_port_f = L4_DEST_PORTf;
         protocol_f = PROTOCOLf;
    }
    if (SOC_MEM_FIELD_VALID(unit, mem, l4_src_port_f)) {
        entry_key->ip_hash_key.l4_src_port =
            soc_mem_field32_get(unit, mem, entry_ptr, l4_src_port_f);
    }
    if (SOC_MEM_FIELD_VALID(unit, mem, l4_dest_port_f)) {
        entry_key->ip_hash_key.l4_dst_port =
            soc_mem_field32_get(unit, mem, entry_ptr, l4_dest_port_f);
    }
    if (SOC_MEM_FIELD_VALID(unit, mem, protocol_f)) {
        entry_key->ip_hash_key.ip_protocol =
            soc_mem_field32_get(unit, mem, entry_ptr, protocol_f);
    }
    
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_TRX_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, GRE_TUNNELf)) {
        entry_key->ip_hash_key.ip_protocol =
            soc_mem_field32_get(unit, mem, entry_ptr, GRE_TUNNELf);
    }
#endif /* BCM_TRX_SUPPORT*/
    return;
}

#define SOC_TNL_TERM_HASH_ENTRY_GET _soc_tunnel_term_hash_entry_get
/*
 * Function:
 *   _soc_tunnel_term_type_get
 * Purpose:
 *	 Extract tunnel terminator entry type 
 *   (DIP6/SIP6)/(DIP4/SIP4)/(DIP 6/(*))/(DIP4/(*)) 
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      entry     - (IN)  Tunnel terminator entry.
 *      type      - (OUT) Tunnel terminator search result.
 *      ipv6      - (OUT) IPv6 Tunnel termination flag. 
 * Returns: 
 *      SOC_E_XXX
 */ 
STATIC int
_soc_tunnel_term_type_get(int unit, soc_tunnel_term_t *entry, 
                          int *type, int *entry_type)
{
    int key;                              /* IPv6 tunnel terminator flag.    */
    soc_tnl_term_hash_key_t key_hash; /* Tunnel termination hashing key. */
#if defined(BCM_TRX_SUPPORT)
    soc_mem_t  mem = L3_TUNNELm;
#endif
    int ipv6_key_type = 0x1;

    /* Input parameters check. */
    if (NULL == entry) {
        return (SOC_E_PARAM);
    }

#if defined(BCM_TRX_SUPPORT)
    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        mem = L3_TUNNEL_DOUBLEm;
        ipv6_key_type = 0x2;
    }

    if (soc_feature(unit,soc_feature_flex_flow) &&
        ((soc_mem_field32_get(unit, mem,
               (uint32 *)&entry->entry_arr[0], KEY_TYPEf)) > 3)) {
         /* flex view */
         key = soc_mem_field32_get(unit, mem,
                             (uint32 *)&entry->entry_arr[0], KEY_TYPEf);
    /* Get tunnel termination type IPv4/IPv6 */     
    } else if (SOC_MEM_FIELD_VALID(unit, mem, MODEf)) {
        key = soc_mem_field32_get(unit, mem,
                                 (uint32 *)&entry->entry_arr[0], MODEf);
    } else if (SOC_MEM_FIELD_VALID(unit, mem, KEY_TYPEf)) {
        key = soc_mem_field32_get(unit, mem,
                                 (uint32 *)&entry->entry_arr[0], KEY_TYPEf);
    } else
#endif /* BCM_TRX_SUPPORT */ 
    {
        key = 0; /* IPV4 default type */
    }

    /* Assign ipv6 flag if caller provided ipv6 pointer. */
    if (NULL != entry_type) {
        *entry_type = key;
    }

    /* If caller doesn't want tunnel type skip this section. */
    if (NULL == type) {
        return (SOC_E_NONE);
    }

    /* Extract source ip portion as part of hash. */
    SOC_TNL_TERM_HASH_ENTRY_GET(unit, entry, key, &key_hash);

    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_HELIX4(unit) ||
        SOC_IS_TRIDENT3X(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
        if (key == ipv6_key_type) { /* IPV6 */

            if (key_hash.ip_hash_key.l4_src_port 
                        && key_hash.ip_hash_key.l4_dst_port) {
                if (key_hash.ip_hash_key.ip_protocol) {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP6_PROT_L4DST_L4SRC; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT_L4DST_L4SRC;
                    }
                } else {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP6_L4DST_L4SRC; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP6_SIP6_L4DST_L4SRC;
                    }
                }
            } else if (key_hash.ip_hash_key.l4_src_port) {
                if (key_hash.ip_hash_key.ip_protocol) {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP6_PROT_L4SRC; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT_L4SRC;
                    }
                } else {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP6_L4SRC; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP6_SIP6_L4SRC;
                    }
                }
            } else if (key_hash.ip_hash_key.l4_dst_port) {
                if (key_hash.ip_hash_key.ip_protocol) {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP6_PROT_L4DST; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT_L4DST;
                    }
                } else {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP6_L4DST; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP6_SIP6_L4DST;
                    }
                }
            } else if (key_hash.ip_hash_key.ip_protocol) {
                if (0 == key_hash.ip_hash_key.sip_plen) {
                    *type = SOC_TNL_TERM_TYPE_DIP6_PROT;
                } else {
                    *type = SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT;
                }
            } else {
                if (0 == key_hash.ip_hash_key.sip_plen) {
                    *type = SOC_TNL_TERM_TYPE_DIP6;
                } else {
                    *type = SOC_TNL_TERM_TYPE_DIP6_SIP6;
                }
            }
        } else  {
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit,soc_feature_flex_flow) &&
           (key > SOC_TNL_TERM_KEY_TYPE_FIXED_MAX)) {
            *type = SOC_TNL_TERM_TYPE_FLEX;
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            if (key_hash.ip_hash_key.l4_src_port 
                        && key_hash.ip_hash_key.l4_dst_port) {
                if (key_hash.ip_hash_key.ip_protocol) {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP4_PROT_L4DST_L4SRC; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4DST_L4SRC;
                    }
                } else {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP4_L4DST_L4SRC; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP4_SIP4_L4DST_L4SRC;
                    }
                }
            } else if (key_hash.ip_hash_key.l4_src_port) {
                if (key_hash.ip_hash_key.ip_protocol) {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP4_PROT_L4SRC; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4SRC;
                    }
                } else {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP4_L4SRC; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP4_SIP4_L4SRC;
                    }
                }
            } else if (key_hash.ip_hash_key.l4_dst_port) {
                if (key_hash.ip_hash_key.ip_protocol) {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP4_PROT_L4DST; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4DST;
                    }
                } else {
                    if (0 == key_hash.ip_hash_key.sip_plen) {
                        *type = SOC_TNL_TERM_TYPE_DIP4_L4DST; 
                    } else {
                        *type = SOC_TNL_TERM_TYPE_DIP4_SIP4_L4DST;
                    }
                }
            } else if (key_hash.ip_hash_key.ip_protocol) {
                if (0 == key_hash.ip_hash_key.sip_plen) {
                    *type = SOC_TNL_TERM_TYPE_DIP4_PROT;
                } else {
                    *type = SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT;
                }
            } else {
                if (0 == key_hash.ip_hash_key.sip_plen) {
                    *type = SOC_TNL_TERM_TYPE_DIP4;
                } else {
                    *type = SOC_TNL_TERM_TYPE_DIP4_SIP4;
                    }
                }
            }
        }
    } else {
        if (key == ipv6_key_type) { /* IPV6 Type */
            if (0 == key_hash.ip_hash_key.sip_plen) { 
                *type = SOC_TNL_TERM_TYPE_DIP6;
            } else {
                *type = SOC_TNL_TERM_TYPE_DIP6_SIP6;
            }
        } else
#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit,soc_feature_flex_flow) &&
           (key > SOC_TNL_TERM_KEY_TYPE_FIXED_MAX)) {
            *type = SOC_TNL_TERM_TYPE_FLEX;
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            if (0 == key_hash.ip_hash_key.sip_plen) {
                *type =SOC_TNL_TERM_TYPE_DIP4;
            } else {
                *type = SOC_TNL_TERM_TYPE_DIP4_SIP4;
            }
        }
    }

    if (!(soc_feature(unit,soc_feature_flex_flow) &&
           (key > 0x03))) {
        /* Add sip prefix length on top of resovled type. */
        *type += key_hash.ip_hash_key.sip_plen;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *   _soc_tunnel_term_hash_compare_key
 * Purpose:
 *   Tunnel termination entry comparison routine 
 *   for sw table management operations.
 * Parameters:
 *      key1  - (IN) First compared tunnel termination key. 
 *      key2  - (IN) Second compared tunnel termination key. 
 * Returns: 
 *      Comparison result(LESS/GREATER/EQUALS) key1 <=> key2.  
 */ 
static INLINE int
_soc_tunnel_term_hash_compare_key(soc_tnl_term_hash_key_t *key1,
                                  soc_tnl_term_hash_key_t *key2)
{
    return sal_memcmp(key1, key2, sizeof(soc_tnl_term_hash_key_t));
}

/* 
 * Function:
 *      _soc_tunnel_term_hash_compute
 * Purpose:
 *      Compute CRC hash for key data.
 * Parameters:
 *      data       - (IN) Key data
 *      data_nbits - (IN) Number of data bits
 *      hash_val   - (OUT) Hash value 
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_tunnel_term_hash_compute(uint8 *data, int data_nbits, uint16 *hash_val)
{
    /* Input parameters check. */
    if ((NULL == data) || (NULL == hash_val)) {
        return (SOC_E_PARAM);
    }
    *hash_val = _shr_crc16b(0, data, data_nbits);

    return (SOC_E_NONE);
}

/* 
 * Function:
 *      _soc_tunnel_term_hash_create
 * Purpose:
 *      Create an empty hash table
 * Parameters:
 *      unit        - (IN)BCM device number.  
 *      entry_count - (IN) Limit for number of entries in table
 *      index_count - (IN) Hash index max + 1. (index_count <= count)
 *      tnl_term_hash_ptr - (OUT) Return pointer (handle) to new Hash Table
 * Returns:
 *      SOC_E_NONE       Success
 *      SOC_E_MEMORY     Out of memory (system allocator)
 */

STATIC int 
_soc_tunnel_term_hash_create(int unit, 
                             int entry_count,
                             int index_count,
                             soc_tnl_term_hash_t **tnl_term_hash_ptr)
{
    soc_tnl_term_hash_t   *hash;        /* Tunnel termination hash. */
    int                   index;        /* Hash iteration index.    */
    int                   alloc_size;   /* Allocation size.         */

    /* Input parameters check. */
    if ((NULL == tnl_term_hash_ptr) || (index_count > entry_count))  {
        return (SOC_E_PARAM);
    }

    alloc_size = sizeof (soc_tnl_term_hash_t);
    hash = sal_alloc(alloc_size, "tnl_term_hash");
    if (hash == NULL) {
        return SOC_E_MEMORY;
    }
    /* Initialize hash structure. */
    sal_memset(hash, 0, alloc_size);
    hash->unit = unit;
    hash->entry_count = entry_count;
    hash->index_count = index_count;

    /*
     * Pre-allocate the hash table storage.
     */
    alloc_size = hash->index_count * sizeof(uint16);
    hash->table = sal_alloc(alloc_size, "tnl_term_hash_table");
    if (hash->table == NULL) {
        sal_free(hash);
        return (SOC_E_MEMORY);
    }

    /*
     * In case where all the entries should hash into the same bucket
     * this will prevent the hash table overflow
     */
    alloc_size = hash->entry_count * sizeof(uint16);
    hash->link_table = sal_alloc(alloc_size, "tnl_term_link_table");
    if (hash->link_table == NULL) {
        sal_free(hash->table);
        sal_free(hash);
        return (SOC_E_MEMORY);
    }

    /*
     * Set the entries in the hash table to TNL_TERM_HASH_INDEX_NULL
     * Link the entries beyond hash->index_max for handling collisions
     */
    for(index = 0; index < hash->index_count; index++) {
        hash->table[index] = TNL_TERM_HASH_INDEX_NULL;
    }
    for(index = 0; index < hash->entry_count; index++) {
        hash->link_table[index] = TNL_TERM_HASH_INDEX_NULL;
    }
    *tnl_term_hash_ptr = hash;
    return (SOC_E_NONE);
}

/* 
 * Function:
 *      _soc_tunnel_term_hash_destroy
 * Purpose:
 *      Destroy the hash table.
 * Parameters:
 *      tnl_term_hash - (IN) Hash Table handler address.
 * Returns:
 *      SOC_E_NONE       Success
 */
STATIC int 
_soc_tunnel_term_hash_destroy(soc_tnl_term_hash_t **tnl_term_hash)
{
    soc_tnl_term_hash_t *ptr;   /* Hash pointer. */

    if (NULL == tnl_term_hash) {
        return (SOC_E_PARAM);
    }

    ptr = *tnl_term_hash;
    if (NULL == ptr) {
        return (SOC_E_NONE);
    }

    /* Free hash entries table. */ 
    if (NULL != ptr->table){   
        sal_free(ptr->table);
        ptr->table = NULL;
    }

    /* Free collisions table. */ 
    if (NULL != ptr->link_table) {  
        sal_free(ptr->link_table);
        ptr->link_table = NULL;
    }

    /* Free hash table itself. */
    sal_free(ptr);
    *tnl_term_hash = NULL;

    return (SOC_E_NONE);
}

/* 
 * Function:
 *      _soc_tunnel_term_entry_read
 * Purpose:
 *      Read tunnel termination entry from hardware. 
 * Parameters:
 *      unit    - (IN)  Bcm device number. 
 *      index   - (IN)  Table index. 
 *      entry   - (OUT) Tunnel termination entry.   
 *      ipv6    - (OUT) Entry is ipv6 tunnel terminator.  
 * Returns:
 *      SOC_E_XXX
 */
STATIC int 
_soc_tunnel_term_entry_read(int unit, int index, 
                            soc_tunnel_term_t *entry, int *entry_type)
{
    int idx;     /* Entry width iteration index. */
    int type;
    soc_mem_t mem = L3_TUNNELm;
    soc_field_t validf = VALIDf;
#ifdef BCM_TRIDENT3_SUPPORT
    int idx_max;
#endif /* BCM_TRIDENT3_SUPPORT */
    int ipv6type = 0x1;

    /* Input parameters check. */
    if ((NULL == entry) || (NULL == entry_type)) {
        return (SOC_E_PARAM);
    }

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        int ipv6 = 0;
        uint32 *entry_ptr;

        mem = L3_TUNNEL_DOUBLEm;
        entry_ptr = (uint32 *)&entry->entry_arr[0];
        ipv6 =
            soc_mem_field32_get(unit, mem, entry_ptr, KEY_TYPEf) - 1;

        if (ipv6) {
            mem = L3_TUNNEL_QUADm;
        }
        validf = BASE_VALID_0f;
        ipv6type = 0x2;
    }

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                         (uint32 *)&entry->entry_arr[0]));

    if (!soc_mem_field32_get(unit, mem,
                             (uint32 *)&entry->entry_arr[0], validf)) {
        sal_memset (entry, 0, sizeof(soc_tunnel_term_t));
        *entry_type = FALSE;
        return (SOC_E_NOT_FOUND);
    }
    /* Check if entry is ipv6 */
    SOC_IF_ERROR_RETURN(_soc_tunnel_term_type_get(unit, entry, &type, entry_type));

#ifdef BCM_TRIDENT3_SUPPORT
     if (soc_feature(unit,soc_feature_flex_flow) &&
           (*entry_type > SOC_TNL_TERM_KEY_TYPE_FIXED_MAX)) {
                SOC_IF_ERROR_RETURN (
        _soc_tunnel_term_flex_type_width_get(unit, type, entry, &idx_max));
        for (idx = 1; idx < idx_max; idx++) {
            SOC_IF_ERROR_RETURN
                   (soc_mem_read(unit, mem, MEM_BLOCK_ANY, (index + idx) ,
                           (uint32 *)&entry->entry_arr[idx]));
        }
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        int size = SOC_TNL_TERM_IPV6_ENTRY_WIDTH;
        if (*entry_type == ipv6type) { /* IPV6 */
            if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
                mem = L3_TUNNEL_QUADm;
                size = 1;
            }
            for (idx = 1; idx < size; idx++) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ANY, (idx + index),
                                 (uint32 *)&entry->entry_arr[idx]));
            }
        }
    }
    return (SOC_E_NONE);
}


/* 
 * Function:
 *      _soc_tunnel_term_entry_write
 * Purpose:
 *      Write tunnel termination entry to the hardware. 
 * Parameters:
 *      unit    - (IN)  Bcm device number. 
 *      index   - (IN)  Table index. 
 *      entry   - (IN)  Tunnel termination entry.   
 *      ipv6    - (IN) Entry is ipv6 tunnel terminator.  
 * Returns:
 *      SOC_E_XXX
 */
STATIC int 
_soc_tunnel_term_entry_write(int unit, int index, 
                             soc_tunnel_term_t *entry, int entry_type)
{
    int idx;       /* Entry width iteration index. */
    int idx_max;   /* Entry width.                 */
#ifdef BCM_TRIDENT3_SUPPORT
    int type;
    int key_type;
#endif /* BCM_TRIDENT3_SUPPORT */
    int ipv6_key_type = 0x1;

    /* Input parameters check. */
    if (NULL == entry) {
        return (SOC_E_PARAM);
    }

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        ipv6_key_type = 0x2;
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit,soc_feature_flex_flow) &&
           (entry_type > SOC_TNL_TERM_KEY_TYPE_FIXED_MAX)) {
        SOC_IF_ERROR_RETURN
            (_soc_tunnel_term_type_get(unit, entry, &type, &key_type));
        SOC_IF_ERROR_RETURN (
             _soc_tunnel_term_flex_type_width_get(unit, type, entry, &idx_max));
    } else
 #endif /* BCM_TRIDENT3_SUPPORT */
    {
        idx_max = (entry_type == ipv6_key_type) ? SOC_TNL_TERM_IPV6_ENTRY_WIDTH : \
              SOC_TNL_TERM_IPV4_ENTRY_WIDTH;
    }

    
    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        soc_mem_t mem;
        mem = (entry_type == ipv6_key_type)? L3_TUNNEL_QUADm:L3_TUNNEL_DOUBLEm; 
        SOC_IF_ERROR_RETURN(soc_mem_insert(unit, mem, MEM_BLOCK_ANY, entry->entry_arr));
    } else {
        for (idx = 0; idx < idx_max; idx++) {
            SOC_IF_ERROR_RETURN (WRITE_L3_TUNNELm(unit, MEM_BLOCK_ALL, 
                                                  (idx + index), 
                                                  (uint32 *)&entry->entry_arr[idx]));
        }
    }
    return (SOC_E_NONE);
}


/*
 * Function:
 *      _soc_tunnel_term_hash_lookup
 * Purpose:
 *      Look up a key in the hash table
 * Parameters:
 *      hash       - (IN)  Pointer (handle) to Hash Table
 *      key_cmp_fn - (IN)  Compare function which should compare key
 *      entry      - (IN)  The key to lookup
 *      key_index  - (OUT) Index where the key was found.
 * Returns:
 *      SOC_E_NONE      Key found
 *      SOC_E_NOT_FOUND Key not found
 */
STATIC int 
_soc_tunnel_term_hash_lookup(soc_tnl_term_hash_t *hash,
                             _soc_tnl_term_hash_compare_fn key_cmp_fn,
                             soc_tnl_term_hash_key_t *entry,
                             uint16 *key_index)
{

    soc_tunnel_term_t hw_entry;          /* Tunnel terminator entry from HW.*/
    soc_tnl_term_hash_key_t  r_entry;   /* Entry key portion.              */
    uint16  hash_val;                    /* Computed hash value.            */
    uint16  index;                       /* Entry index in L3_TUNNEL table. */
    int     type;                        /* Entry type flag.                */
    int     rv;                          /* Operation return status.        */
    int     unit;                        /* BCM device number.              */
    uint16  mask;                        /* Index mask based on hash size   */

    /* Input parameters check. */
    if ((NULL == entry) || (NULL == key_index) || (NULL == hash)) {
        return (SOC_E_PARAM);
    }

    /* Extract bcm device number. */
    unit = hash->unit;

    /* Calculate entry hash. */
    rv = _soc_tunnel_term_hash_compute((uint8 *)entry,
                                       SOC_TNL_TERM_HASH_BIT_WIDTH, &hash_val);
    if (SOC_FAILURE(rv)){
        return (rv);
    }

    mask = soc_mem_index_count(unit, L3_TUNNELm) - 1;
    hash_val %= hash->index_count;

    /* Iterate over indexes with identical hash value. */
    index = hash->table[hash_val];
    while(index != TNL_TERM_HASH_INDEX_NULL) {
        index &= mask;
        /* Read entry from HW.  */
        rv = _soc_tunnel_term_entry_read(unit, index, &hw_entry, &type);
        if (SOC_FAILURE(rv)){
            return (SOC_E_INTERNAL);
        }
        SOC_TNL_TERM_HASH_ENTRY_GET(unit, &hw_entry, type, &r_entry);
        if ((*key_cmp_fn)(entry, &r_entry) == 0) {
            *key_index = index;
            return(SOC_E_NONE);
        }
        index = hash->link_table[index];
    }
    return(SOC_E_NOT_FOUND);
}

#define INDEX_ADD(hash, hash_idx, new_idx)                      \
    hash->link_table[new_idx] = hash->table[hash_idx];          \
    hash->table[hash_idx] = new_idx

#define INDEX_UPDATE(hash, hash_idx, old_idx, new_idx)          \
    hash->table[hash_idx] = new_idx;                            \
    hash->link_table[new_idx] = hash->link_table[old_idx];      \
    hash->link_table[old_idx] = TNL_TERM_HASH_INDEX_NULL

#define INDEX_UPDATE_LINK(hash, prev_idx, old_idx, new_idx)     \
    hash->link_table[prev_idx] = new_idx;                       \
    hash->link_table[new_idx] = hash->link_table[old_idx];      \
    hash->link_table[old_idx] = TNL_TERM_HASH_INDEX_NULL

/*
 * Function:
 *      _soc_tunnel_term_hash_insert
 * Purpose:
 *      Insert/Update a key index in the hash table
 * Parameters:
 *      hash       - (IN) Pointer (handle) to Hash Table
 *      key_cmp_fn - (IN) Compare function which should compare key
 *      entry      - (IN) The key to lookup
 *      old_index  - (IN) Index where the key was moved from.
 *                        TNL_TERM_HASH_INDEX_NULL if new entry.
 *      new_index  - (IN) Index where the key was moved to.
 * Returns:
 *      SOC_E_XXX
 * NOTE: 
 *      Should be caled before updating the L3_TUNNEL table so that the
 *      data in the hash table is consistent with the hardware table
 */
STATIC int 
_soc_tunnel_term_hash_insert(soc_tnl_term_hash_t *hash,
                             _soc_tnl_term_hash_compare_fn key_cmp_fn,
                             soc_tnl_term_hash_key_t *entry,
                             uint16 old_index,
                             uint16 new_index)
{
    soc_tunnel_term_t hw_entry;         /* Tunnel terminator entry from HW.  */
    soc_tnl_term_hash_key_t  r_entry;  /* Entry key portion.                */
    uint16  hash_val;                   /* Computed hash value.              */
    uint16  prev_index;                 /* Previous entry with identical hash*/
    uint16  index;                      /* Entry index in L3_TUNNEL table.   */
    int     type;                       /* Entry type  flag.                 */
    int     rv;                         /* Operation return status.          */
    int     unit;                       /* BCM device number.                */
    uint16  mask;                       /* Index mask based on hash size     */

    /* Input parameters check. */
    if ((NULL == entry) || (NULL == hash)) {
        return (SOC_E_PARAM);
    }

    rv = _soc_tunnel_term_hash_compute((uint8 *)entry, 
                                       SOC_TNL_TERM_HASH_BIT_WIDTH, &hash_val);
    if (SOC_FAILURE(rv)) {
        return (rv);
    }
    hash_val %= hash->index_count;

    index = hash->table[hash_val];
    unit = hash->unit;
    mask = soc_mem_index_count(unit, L3_TUNNELm) - 1;

    prev_index = TNL_TERM_HASH_INDEX_NULL;
    if (old_index != TNL_TERM_HASH_INDEX_NULL) {
        while(index != TNL_TERM_HASH_INDEX_NULL) {
            index &= mask;

            /*
             * Check prefix length and skip index if not valid for given length
             */

            rv = _soc_tunnel_term_entry_read(unit, index, &hw_entry, &type);
            if (SOC_FAILURE(rv)) {
                return (SOC_E_INTERNAL);
            }
            SOC_TNL_TERM_HASH_ENTRY_GET(unit, &hw_entry, type, &r_entry);
            if ((*key_cmp_fn)(entry, &r_entry) == 0) {
                if (new_index != index) {
                    if (prev_index == TNL_TERM_HASH_INDEX_NULL) {
                        INDEX_UPDATE(hash, hash_val, index & mask,
                                     new_index & mask);
                    } else {
                        INDEX_UPDATE_LINK(hash, prev_index & mask, 
                                          index & mask, new_index & mask);
                    }
                }
                return(SOC_E_NONE);
            }
            prev_index = index;
            index = hash->link_table[index];
        }
    }
    INDEX_ADD(hash, hash_val, new_index & mask);  /* new entry */
    return(SOC_E_NONE);
}
#undef INDEX_ADD
#undef INDEX_UPDATE
#undef INDEX_UPDATE_LINK

#define INDEX_DELETE(hash, hash_idx, del_idx)                 \
     hash->table[hash_idx] = hash->link_table[del_idx];       \
     hash->link_table[del_idx] = TNL_TERM_HASH_INDEX_NULL     \

#define INDEX_DELETE_LINK(hash, prev_idx, del_idx)            \
     hash->link_table[prev_idx] = hash->link_table[del_idx];  \
     hash->link_table[del_idx] = TNL_TERM_HASH_INDEX_NULL      
/*
 * Function:
 *      _soc_tunnel_term_hash_delete
 * Purpose:
 *      Delete a key index in the hash table
 * Parameters:
 *      hash         - (IN) Pointer (handle) to Hash Table
 *      key_cmp_fn   - (IN) Compare function which should compare key
 *      entry        - (IN) The key to lookup
 *      delete_index - (IN) Index to delete.
 * Returns:
 *      SOC_E_NONE      Success
 *      SOC_E_NOT_FOUND Key index not found.
 */
STATIC int 
_soc_tunnel_term_hash_delete(soc_tnl_term_hash_t *hash,
                             _soc_tnl_term_hash_compare_fn key_cmp_fn,
                             soc_tnl_term_hash_key_t *entry,
                             uint16 delete_index)
{
    uint16  hash_val;                    /* Computed hash value.              */
    uint16  prev_index;                  /* Previous entry with identical hash*/
    uint16  index;                       /* Entry index in L3_TUNNEL table.   */
    int     rv;                          /* Internal operatin return status.  */
    int     unit;                        /* BCM device number.                */
    uint16  mask;                        /* Index mask based on hash size     */

    /* Input parameters check. */
    if ((NULL == entry) || (NULL == hash)) {
        return (SOC_E_PARAM);
    }

    rv = _soc_tunnel_term_hash_compute((uint8 *)entry, 
                                       SOC_TNL_TERM_HASH_BIT_WIDTH, &hash_val);
    if (SOC_FAILURE(rv)) {
        return (rv);
    }

    unit = hash->unit;
    mask = soc_mem_index_count(unit, L3_TUNNELm) - 1;
    hash_val %= hash->index_count;

    index = hash->table[hash_val];
    prev_index = TNL_TERM_HASH_INDEX_NULL;
    while(index != TNL_TERM_HASH_INDEX_NULL) {
        if (delete_index == index) {
            if (prev_index == TNL_TERM_HASH_INDEX_NULL) {
                INDEX_DELETE(hash, hash_val, delete_index & mask);
            } else {
                INDEX_DELETE_LINK(hash, prev_index & mask, 
                        delete_index & mask);
            }
            return(SOC_E_NONE);
        }
        prev_index = index;
        index = hash->link_table[index & mask];
    }
    return(SOC_E_NOT_FOUND);
}
#undef INDEX_DELETE
#undef INDEX_DELETE_LINK
#define TNL_TERM_HASH_INSERT(_u_, _entry_data_, _tab_index_)       \
    soc_tunnel_term_hash_insert((_u_), (_entry_data_), (_tab_index_), 0)

#define TNL_TERM_HASH_DELETE(_u_, _key_data_, _tab_index_)         \
    soc_tunnel_term_hash_delete((_u_), (_key_data_), (_tab_index_))

#define TNL_TERM_HASH_LOOKUP(_u_, _key_data_, _tab_index_)         \
    soc_tunnel_term_hash_lookup((_u_), (_key_data_), (_tab_index_))
/*
 * Function:
 *   soc_tunnel_term_hash_insert
 * Purpose:
 *   Tunnel termination entry hash insertion routine.
 * Parameters:
 *      unit          - (IN) BCM device number. 
 *      entry         - (IN) Inserted entry. 
 *      tab_index     - (IN) Insertion index. 
 *      old_index     - (IN) Original entry index. 
 * Returns: 
 *      SOC_E_XXX
 */ 

STATIC int
soc_tunnel_term_hash_insert(int unit, soc_tunnel_term_t *entry, 
                            uint32 tab_index, uint32 old_index)
{
    soc_tnl_term_hash_key_t    key_hash;      /* Entry tcam lookup key.   */
    int type;                                  /* Tunnel Entry Type        */
    int rv;                                    /* Operation return status. */

    /* Input parameters check. */
    if (NULL == entry) {
        return (SOC_E_PARAM);
    }

    if (!soc_mem_field32_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[0], VALIDf)) {
        return (SOC_E_EMPTY);
    }

    SOC_IF_ERROR_RETURN(_soc_tunnel_term_type_get(unit, entry, NULL, &type));
    SOC_TNL_TERM_HASH_ENTRY_GET(unit, entry, type, &key_hash);

    rv = _soc_tunnel_term_hash_insert (SOC_TNL_TERM_STATE_HASH(unit), 
                                       _soc_tunnel_term_hash_compare_key,
                                       &key_hash, old_index, 
                                       ((uint16)tab_index));
    return (rv);
}

/*
 * Function:
 *   soc_tunnel_term_hash_delete
 * Purpose:
 *   Tunnel termination entry hash removal routine.
 * Parameters:
 *      unit          - (IN) BCM device number.
 *      key_data      - (IN) Deleted entry.
 *      tab_index     - (IN) Removed index.
 * Returns: 
 *      SOC_E_XXX
 */
STATIC int
soc_tunnel_term_hash_delete(int unit, soc_tunnel_term_t *key_data, 
                         uint32 tab_index)
{
    soc_tnl_term_hash_key_t  key_hash;    /* Entry lookup key.        */
    int                       type;        /* Tunnel term type         */

    /* Input parameters check. */
    if (NULL == key_data) { 
        return (SOC_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(_soc_tunnel_term_type_get(unit, key_data, NULL, &type));
    SOC_TNL_TERM_HASH_ENTRY_GET(unit, key_data, type, &key_hash);

    return  _soc_tunnel_term_hash_delete(SOC_TNL_TERM_STATE_HASH(unit),
                                         _soc_tunnel_term_hash_compare_key,
                                         &key_hash, tab_index);
}

/*
 * Function:
 *   soc_tunnel_term_hash_lookup
 * Purpose:
 *   Tunnel termination entry hash lookup routine.
 * Parameters:
 *      unit          - (IN)  BCM device number. 
 *      key_data      - (IN)  Entry lookup key.
 *      key_index     - (OUT) Search result index if any. 
 * Returns: 
 *      SOC_E_XXX
 */

STATIC int 
soc_tunnel_term_hash_lookup(int unit, soc_tunnel_term_t *key_data, 
                         int *key_index)
{
    soc_tnl_term_hash_key_t    key_hash;  /* Entry lookup key.        */
    uint16                      index;     /* Entry index.             */
    int                         type;      /* Tunnel Term type         */
    int                         rv;        /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == key_data) || (NULL == key_index))  {
        return (SOC_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(_soc_tunnel_term_type_get(unit, key_data, NULL, &type));
    SOC_TNL_TERM_HASH_ENTRY_GET(unit, key_data, type, &key_hash);

    rv = _soc_tunnel_term_hash_lookup(SOC_TNL_TERM_STATE_HASH(unit),
                                      _soc_tunnel_term_hash_compare_key,
                                      &key_hash, &index);
    if (SOC_FAILURE(rv)) {
        return (rv);
    }
    *key_index = index;
    return (SOC_E_NONE);
}


/*
 * Function:
 *      soc_tunnel_term_sw_dump
 * Purpose:
 *      Dump tunnel termination hash status for warm boot debug. 
 * Parameters:
 *      unit - (IN) BCM device number.
 * Returns:
 *      none. 
 */
void
soc_tunnel_term_sw_dump(int unit)
{
    int i;
    int max_type;
    max_type = MAX_TNL_TERM_TYPE;

    LOG_CLI((BSL_META_U(unit,
                        "\n    Tunnel Terminator State -\n")));

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        return;
    }
    if (!SOC_TNL_TERM_INIT_CHECK(unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "\nTunnel Terminator in Uninitialized state -\n")));
        return;
    }

    for(i = max_type; i >= 0 ; i--) {
        if ((i != MAX_TNL_TERM_TYPE) && \
            (SOC_TNL_TERM_STATE_START(unit, i) == TNL_TERM_TYPE_NULL)) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),
                 i,
                 SOC_TNL_TERM_STATE_PREV(unit, i),
                 SOC_TNL_TERM_STATE_NEXT(unit, i),
                 SOC_TNL_TERM_STATE_START(unit, i),
                 SOC_TNL_TERM_STATE_END(unit, i),
                 SOC_TNL_TERM_STATE_VENT(unit, i),
                 SOC_TNL_TERM_STATE_FENT(unit, i)));
    }
    return;
}

/*
 * Function:
 *   _soc_tunnel_term_type_width_get
 * Purpose:
 *   Given tunnel terminator type get number of hw slots 
 *   it takes to write it to hardware.  
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      type      - (IN)  Tunnel terminator type.
 *      width     - (OUT) Number of hw slot entry requires. 
 * Returns: 
 *      SOC_E_XXX
 */ 
STATIC int
_soc_tunnel_term_type_width_get(int unit, int type, int *width)
{
    /* Input parameters check. */
    if (NULL == width) {
        return (SOC_E_PARAM);
    }

    switch (type - (type % (_SHR_L3_IP6_MAX_NETLEN + 1))) {
      case  SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4DST_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP4_PROT_L4DST_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP4_SIP4_L4DST_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP4_L4DST_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4DST:
      case  SOC_TNL_TERM_TYPE_DIP4_PROT_L4DST:
      case  SOC_TNL_TERM_TYPE_DIP4_SIP4_L4DST:
      case  SOC_TNL_TERM_TYPE_DIP4_L4DST:
      case  SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP4_PROT_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP4_SIP4_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP4_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT:
      case  SOC_TNL_TERM_TYPE_DIP4_PROT:
      case  SOC_TNL_TERM_TYPE_DIP4_SIP4:
      case  SOC_TNL_TERM_TYPE_DIP4:
          *width = SOC_TNL_TERM_IPV4_ENTRY_WIDTH;
          break;
      case  SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT_L4DST_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP6_PROT_L4DST_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP6_SIP6_L4DST_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT_L4DST:
      case  SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP6_L4DST_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP6_SIP6_L4DST:
      case  SOC_TNL_TERM_TYPE_DIP6_PROT_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP6_SIP6_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP6_PROT_L4DST:
      case  SOC_TNL_TERM_TYPE_DIP6_SIP6_PROT:
      case  SOC_TNL_TERM_TYPE_DIP6_L4SRC:
      case  SOC_TNL_TERM_TYPE_DIP6_SIP6:
      case  SOC_TNL_TERM_TYPE_DIP6_PROT:
      case  SOC_TNL_TERM_TYPE_DIP6_L4DST:
      case  SOC_TNL_TERM_TYPE_DIP6:
          *width = SOC_TNL_TERM_IPV6_ENTRY_WIDTH;
          break;
      default:
          return (SOC_E_INTERNAL);
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      _soc_tunnel_term_entry_shift
 * Purpose:
 *      Move entry from one index to another tcam. 
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      from_ent   - (IN) Entry source. 
 *      to_ent     - (IN) Entry destination. 
 * Returns:
 *      SOC_E_XXX.
 */
STATIC int 
_soc_tunnel_term_entry_shift(int unit, int from_ent, int to_ent)
{
    soc_tunnel_term_t hw_entry;       /* Tunnel terminator entry from hw. */
    int entry_type;                   /* Entry type indicator.            */
    int rv;                           /* Operation return status.         */

    if (from_ent == to_ent) {
        return (SOC_E_NONE);
    }

    /* Read source entry. */
    rv = _soc_tunnel_term_entry_read(unit, from_ent, &hw_entry, &entry_type);
    if (SOC_FAILURE(rv)) {
        return ((SOC_E_NOT_FOUND == rv) ? (SOC_E_NONE) : (rv));
    }
    TNL_TERM_HASH_INSERT(unit, &hw_entry, to_ent);

    /* Write destination entry. */
    SOC_IF_ERROR_RETURN
        (_soc_tunnel_term_entry_write(unit, to_ent, &hw_entry, entry_type));

    /* Reset source entry. */ 
    sal_memset(&hw_entry, 0, sizeof(soc_tunnel_term_t));
    SOC_IF_ERROR_RETURN
        (_soc_tunnel_term_entry_write(unit, from_ent, &hw_entry, entry_type));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      _soc_tunnel_term_entry_block_shift
 * Purpose:
 *      Create a slot for the new entry rippling the entries if required.
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      from_ent   - (IN) Block source start index.
 *      to_ent     - (IN) Block destination start index. 
 *      type       - (IN) Block entries type. 
 * Returns:
 *      SOC_E_XXX.
 */
STATIC int 
_soc_tunnel_term_entry_block_shift(int unit, int from_ent, int to_ent,int type)
{
    int width;                             /* Single entry width in block. */
    int idx;                               /* Block indexes iterator.      */
#ifdef BCM_TRIDENT3_SUPPORT
    soc_tunnel_term_t hw_entry;            /* Tunnel terminator entry from hw. */
    int entry_type;                        /* Entry type indicator.            */
#endif /* BCM_TRIDENT3_SUPPORT */

    if (from_ent == to_ent) {
        return (SOC_E_NONE);
    }
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit,soc_feature_flex_flow) &&
       (type == SOC_TNL_TERM_TYPE_FLEX)) {

       /* Read source entry. */
       SOC_IF_ERROR_RETURN (
        _soc_tunnel_term_entry_read(unit, from_ent, &hw_entry, &entry_type));
       SOC_IF_ERROR_RETURN (
        _soc_tunnel_term_flex_type_width_get(unit, type, &hw_entry, &width));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        /* Get entry width for tunnel termination type. */
        SOC_IF_ERROR_RETURN(_soc_tunnel_term_type_width_get(unit, type, &width));
    }

    for (idx = 0; idx < SOC_TNL_TERM_BLOCK_SIZE; idx += width) {
        SOC_IF_ERROR_RETURN
            (_soc_tunnel_term_entry_shift(unit, from_ent + idx, to_ent + idx));
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      _soc_tunnel_term_entry_shift_up
 * Purpose:
 *      Shift entries 4 entries UP, while preserving  
 *      last 4 ipv4 entries last.
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      type       - (IN) Free entry type. 
 * Returns:
 *      SOC_E_XXX.
 */
STATIC
int _soc_tunnel_term_shift_type_up(int unit, int type)
{
    int from_ent;      /* Moved entry block index.                  */
    int to_ent;        /* Destination entry block index.            */
    int entry_width;   /* Number of hw slots single entry occupies. */
    int from_block;    /* "From" block start index.                 */
    int to_block;      /* "To" block start index.                   */

#ifdef BCM_TRIDENT3_SUPPORT
    soc_tunnel_term_t hw_entry;            /* Tunnel terminator entry from hw. */
    int entry_type;                        /* Entry type indicator.            */
#endif /* BCM_TRIDENT3_SUPPORT */

    to_ent = SOC_TNL_TERM_STATE_END(unit, type) + SOC_TNL_TERM_BLOCK_SIZE;
    SOC_TNL_TERM_IDX_TO_BLOCK_START(to_ent, to_block,SOC_TNL_TERM_BLOCK_SIZE);

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit,soc_feature_flex_flow) &&
       (type == SOC_TNL_TERM_TYPE_FLEX)) {
        from_ent = SOC_TNL_TERM_STATE_END(unit, type);
        SOC_TNL_TERM_IDX_TO_BLOCK_START(from_ent,
                         from_block,SOC_TNL_TERM_BLOCK_SIZE);
        /* Read source entry. */
        SOC_IF_ERROR_RETURN (
         _soc_tunnel_term_entry_read(unit, from_block, &hw_entry, &entry_type));
        SOC_IF_ERROR_RETURN (
        _soc_tunnel_term_flex_type_width_get(unit, type,
                             &hw_entry, &entry_width));
        if (SOC_TNL_TERM_BLOCK_SIZE != entry_width) {

            if (from_ent != (from_block + SOC_TNL_TERM_BLOCK_SIZE - 1)) {
                /* Last block is not full -> keep it last. */
                SOC_IF_ERROR_RETURN
                    (_soc_tunnel_term_entry_block_shift(unit, from_block,
                                                    to_block, type));

                to_block -= SOC_TNL_TERM_BLOCK_SIZE;
            }

        }

        from_ent = SOC_TNL_TERM_STATE_START(unit, type);
        SOC_TNL_TERM_IDX_TO_BLOCK_START(from_ent,
                   from_block,SOC_TNL_TERM_BLOCK_SIZE);

        if(from_block != to_block) {
            SOC_IF_ERROR_RETURN(
                    _soc_tunnel_term_entry_block_shift(unit, from_block,
                                                       to_block, type));
        }

    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        SOC_IF_ERROR_RETURN
            (_soc_tunnel_term_type_width_get(unit, type, &entry_width));

        if (SOC_TNL_TERM_BLOCK_SIZE != entry_width) {
            from_ent = SOC_TNL_TERM_STATE_END(unit, type);
            SOC_TNL_TERM_IDX_TO_BLOCK_START(from_ent,
                       from_block,SOC_TNL_TERM_BLOCK_SIZE);

            if (from_ent != (from_block + SOC_TNL_TERM_BLOCK_SIZE - 1)) {
                /* Last block is not full -> keep it last. */
                SOC_IF_ERROR_RETURN
                    (_soc_tunnel_term_entry_block_shift(unit, from_block,
                                                        to_block, type));

                to_block -= SOC_TNL_TERM_BLOCK_SIZE;
            }
        }

        from_ent = SOC_TNL_TERM_STATE_START(unit, type);
        SOC_TNL_TERM_IDX_TO_BLOCK_START(from_ent,
                             from_block,SOC_TNL_TERM_BLOCK_SIZE);

        if(from_block != to_block) {
            SOC_IF_ERROR_RETURN(
                  _soc_tunnel_term_entry_block_shift(unit, from_block,
                                                     to_block, type));
        }
    }

    SOC_TNL_TERM_STATE_START(unit, type) += SOC_TNL_TERM_BLOCK_SIZE;
    SOC_TNL_TERM_STATE_END(unit, type) += SOC_TNL_TERM_BLOCK_SIZE;
    return (SOC_E_NONE);
}

/*
 * Function:
 *      _soc_tunnel_term_entry_shift_down
 * Purpose:
 *      Shift entries 4 entries DOWN, while preserving  
 *      last 4 ipv4 entries last.
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      type       - (IN) Free entry type. 
 * Returns:
 *      SOC_E_XXX.
 */
STATIC int 
_soc_tunnel_term_shift_type_down(int unit, int type)
{
    int from_ent;      /* Moved entry block index.                  */
    int to_ent;        /* Destination entry block index.            */
    int entry_width;   /* Number of hw slots single entry occupies. */
    int from_block;    /* "From" block start index.                 */
    int to_block;      /* "To" block start index.                   */
#ifdef BCM_TRIDENT3_SUPPORT
    soc_tunnel_term_t hw_entry;            /* Tunnel terminator entry from hw. */
    int entry_type;                        /* Entry type indicator.            */
#endif /* BCM_TRIDENT3_SUPPORT */

    to_ent = SOC_TNL_TERM_STATE_START(unit, type) - SOC_TNL_TERM_BLOCK_SIZE;
    to_block = to_ent;

    /* Don't move empty types. */ 
    if (SOC_TNL_TERM_STATE_VENT(unit, type) == 0) {
        SOC_TNL_TERM_STATE_START(unit, type) = to_ent;
        SOC_TNL_TERM_STATE_END(unit, type) = to_ent - 1;
        return (SOC_E_NONE);
    }

    from_ent = SOC_TNL_TERM_STATE_END(unit, type);
    SOC_TNL_TERM_IDX_TO_BLOCK_START(from_ent, from_block,SOC_TNL_TERM_BLOCK_SIZE);

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit,soc_feature_flex_flow) &&
       (type == SOC_TNL_TERM_TYPE_FLEX)) {
        SOC_IF_ERROR_RETURN (
         _soc_tunnel_term_entry_read(unit, from_block, &hw_entry, &entry_type));
        SOC_IF_ERROR_RETURN (
        _soc_tunnel_term_flex_type_width_get(unit, type, &hw_entry, &entry_width));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        SOC_IF_ERROR_RETURN
            (_soc_tunnel_term_type_width_get(unit, type, &entry_width));
    }

    if ((SOC_TNL_TERM_BLOCK_SIZE != entry_width) && 
        (SOC_TNL_TERM_STATE_VENT(unit, type) > SOC_TNL_TERM_BLOCK_SIZE))  {  

        if (from_ent != (from_block + SOC_TNL_TERM_BLOCK_SIZE - 1)) {
            /* Last block is not full -> keep it last. */
            /* Shift entry before last to start - 1 position. */
            from_block -= SOC_TNL_TERM_BLOCK_SIZE;
            SOC_IF_ERROR_RETURN
                (_soc_tunnel_term_entry_block_shift(unit, from_block,
                                                    to_block, type));
            to_block = from_block; 
            from_block += SOC_TNL_TERM_BLOCK_SIZE;
        }
    }
    SOC_IF_ERROR_RETURN (_soc_tunnel_term_entry_block_shift(unit, from_block,
                                                            to_block, type));

    SOC_TNL_TERM_STATE_START(unit, type) -= SOC_TNL_TERM_BLOCK_SIZE;
    SOC_TNL_TERM_STATE_END(unit, type) -= SOC_TNL_TERM_BLOCK_SIZE;
    return (SOC_E_NONE);
}
/*
 * Function:
 *   _soc_tunnel_term_type_entries_ripple
 * Purpose:
 *	    Ceate a slot for the new entry rippling the entries if required. 
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      type     - (IN)  Inserted Tunnel terminator entry type.
 * Returns: 
 *      SOC_E_XXX
 */ 
STATIC int 
_soc_tunnel_term_type_entries_ripple (int unit, int type) 
{
    int         prev_type;
    int         next_type;
    int         free_type;

    free_type = type;
    while (SOC_TNL_TERM_STATE_FENT(unit, free_type) <
           SOC_TNL_TERM_BLOCK_SIZE) {
        free_type = SOC_TNL_TERM_STATE_NEXT(unit, free_type);
        if (free_type == TNL_TERM_TYPE_NULL) {
            /* No free entries on this side try the other side */
            free_type = type;
            break;
        }
    }
		
    while (SOC_TNL_TERM_STATE_FENT(unit, free_type) < SOC_TNL_TERM_BLOCK_SIZE) {
        free_type = SOC_TNL_TERM_STATE_PREV(unit, free_type);
        if (free_type == TNL_TERM_TYPE_NULL) {
            return (SOC_E_FULL);
        }
    }

    /*
     * Ripple entries to create free space
     */
    while (free_type > type) {
        next_type = SOC_TNL_TERM_STATE_NEXT(unit, free_type);
        SOC_IF_ERROR_RETURN(_soc_tunnel_term_shift_type_down(unit, next_type));
        SOC_TNL_TERM_STATE_FENT(unit, free_type) -= SOC_TNL_TERM_BLOCK_SIZE;
        SOC_TNL_TERM_STATE_FENT(unit, next_type) += SOC_TNL_TERM_BLOCK_SIZE;
        free_type = next_type;
    }

    while (free_type < type) {
        SOC_IF_ERROR_RETURN(_soc_tunnel_term_shift_type_up(unit, free_type));
        SOC_TNL_TERM_STATE_FENT(unit, free_type) -= SOC_TNL_TERM_BLOCK_SIZE;
        prev_type = SOC_TNL_TERM_STATE_PREV(unit, free_type); 
        SOC_TNL_TERM_STATE_FENT(unit, prev_type) += SOC_TNL_TERM_BLOCK_SIZE;
        free_type = prev_type;
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *   _soc_tunnel_term_slot_allocate
 * Purpose:
 *	    Create a slot for the new entry rippling the entries if required. 
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      entry     - (IN)  Inserted Tunnel terminator entry.
 *      index     - (OUT) Allocated index. 
 * Returns: 
 *      SOC_E_XXX
 */ 
STATIC
int _soc_tunnel_term_slot_allocate(int unit, soc_tunnel_term_t *entry, int *index)
{
    int         type;          /* Tunnel entry type.                      */
    int         entry_type;    /* entry type                              */
    int         entry_size;    /* Requested entry size.                   */ 
    int         curr_type;     /* Iteration index to find insertion index.*/
    int         prev_type;     /* Previous entry type in tcam.            */
    int         next_type;     /* Next entry type in tcam.                */
    int         rv;            /* Operation return status.                */

    /* Get tunnel termination type. */
    SOC_IF_ERROR_RETURN (_soc_tunnel_term_type_get(unit, entry, &type, 
                        &entry_type));
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit,soc_feature_flex_flow) &&
           (entry_type > SOC_TNL_TERM_KEY_TYPE_FIXED_MAX)) {
        SOC_IF_ERROR_RETURN (
        _soc_tunnel_term_flex_type_width_get(unit, type, entry, &entry_size));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */

    {
        entry_size = (entry_type == 0x1) ? SOC_TNL_TERM_IPV6_ENTRY_WIDTH :
           SOC_TNL_TERM_IPV4_ENTRY_WIDTH;
    }
    if (SOC_TNL_TERM_STATE_VENT(unit, type) == 0) {
        /*
         * Find the type position. Only type with valid
         * entries are in the list.
         * next -> high to low type priority. low to high index
         * prev -> low to high type priority. high to low index
         * Unused prefix length MAX_TNL_TERM_TYPE is the head of the
         * list and is node corresponding to this is always
         * present.
         */
        curr_type = MAX_TNL_TERM_TYPE;
        while ((SOC_TNL_TERM_STATE_NEXT(unit, curr_type) !=
                TNL_TERM_TYPE_NULL) &&  
               (SOC_TNL_TERM_STATE_NEXT(unit, curr_type) > type)) {
            curr_type = SOC_TNL_TERM_STATE_NEXT(unit, curr_type);
        }

        /* Insert the new prefix */
        next_type = SOC_TNL_TERM_STATE_NEXT(unit, curr_type);
        if (next_type != TNL_TERM_TYPE_NULL) {
            SOC_TNL_TERM_STATE_PREV(unit, next_type) = type;
        }
        SOC_TNL_TERM_STATE_NEXT(unit, type) = 
            SOC_TNL_TERM_STATE_NEXT(unit, curr_type);
        SOC_TNL_TERM_STATE_PREV(unit, type) = curr_type;
        SOC_TNL_TERM_STATE_NEXT(unit, curr_type) = type;

        SOC_TNL_TERM_STATE_VENT(unit, type) = 0;
        SOC_TNL_TERM_STATE_FENT(unit, type) = 
            (SOC_TNL_TERM_STATE_FENT(unit, curr_type) + 1) / 2;

        /* Adjust number of free entries to 
         * multiples of SOC_TNL_TERM_BLOCK_SIZE  boundary.
         */  
        SOC_TNL_TERM_STATE_FENT(unit, type) /= SOC_TNL_TERM_BLOCK_SIZE;
        SOC_TNL_TERM_STATE_FENT(unit, type) *= SOC_TNL_TERM_BLOCK_SIZE;
        SOC_TNL_TERM_STATE_FENT(unit, curr_type) -= SOC_TNL_TERM_STATE_FENT(unit, type);

        SOC_TNL_TERM_STATE_START(unit, type) =  
            SOC_TNL_TERM_STATE_END(unit, curr_type) +
            SOC_TNL_TERM_STATE_FENT(unit, curr_type) + 1;
        SOC_TNL_TERM_STATE_END(unit, type) = SOC_TNL_TERM_STATE_START(unit, type) - 1;

    } 

    /* Check if there is sufficient room  to accomodate the entry.*/ 
    if (SOC_TNL_TERM_STATE_FENT(unit, type) < entry_size) {
        rv = _soc_tunnel_term_type_entries_ripple (unit, type);
        if (SOC_FAILURE(rv)) {
            if (!SOC_TNL_TERM_STATE_VENT(unit, type)) {
                /* We failed to allocate entries for a newly allocated type.*/
                prev_type = SOC_TNL_TERM_STATE_PREV(unit, type);
                next_type = SOC_TNL_TERM_STATE_NEXT(unit, type);
                if (TNL_TERM_TYPE_NULL != prev_type) {
                    SOC_TNL_TERM_STATE_NEXT(unit, prev_type) = next_type;
                }
                if (TNL_TERM_TYPE_NULL != next_type) {
                    SOC_TNL_TERM_STATE_PREV(unit, next_type) = prev_type;
                }
            }
            return (rv);
        }
    }

    /* Insert the entry. */
    SOC_TNL_TERM_STATE_VENT(unit, type) += entry_size;
    SOC_TNL_TERM_STATE_FENT(unit, type) -= entry_size;
    *index = SOC_TNL_TERM_STATE_END(unit, type) + 1;
    SOC_TNL_TERM_STATE_END(unit, type) += entry_size;

    return(SOC_E_NONE);
}

/*
 * Function:
 *   _soc_tunnel_term_slot_free
 * Purpose:
 *      Delete a slot and adjust entry pointers if required.
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      entry     - (IN)  Inserted Tunnel terminator entry.
 *      index     - (OUT) Allocated index. 
 * Returns: 
 *      SOC_E_XXX
 */ 
STATIC int 
_soc_tunnel_term_slot_free (int unit, soc_tunnel_term_t *entry, int index)
{
    int         type;          /* Tunnel entry type.                      */
    int         entry_type;    /* entry type                              */
    int         from_ent;      /* Moved entry block index.                */
    int         to_ent;        /* Destination entry block index.          */
    int         entry_size;    /* Requested entry size.                   */ 
    int         prev_type;     /* Previous entry type in tcam.            */
    int         next_type;     /* Next entry type in tcam.                */
    soc_tunnel_term_t null_entry;   /* NULL entry buffer.                 */

    /* Input parameters check. */
    if (NULL == entry) {
        return (SOC_E_PARAM);
    }

    /* Get tunnel termination type. */
    SOC_IF_ERROR_RETURN (_soc_tunnel_term_type_get(unit, entry, &type,
                        &entry_type));
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit,soc_feature_flex_flow) &&
           (entry_type > SOC_TNL_TERM_KEY_TYPE_FIXED_MAX)) {
        SOC_IF_ERROR_RETURN (
        _soc_tunnel_term_flex_type_width_get(unit, type,
                                  entry, &entry_size));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        SOC_IF_ERROR_RETURN (
        _soc_tunnel_term_type_width_get(unit, type, &entry_size));
    }

    SOC_TNL_TERM_STATE_VENT(unit, type) -= entry_size;
    SOC_TNL_TERM_STATE_FENT(unit, type) += entry_size;
    
    if (SOC_TNL_TERM_STATE_VENT(unit, type) != 0) {
        from_ent = SOC_TNL_TERM_STATE_END(unit, type) + 1  - entry_size;
        to_ent = index;
        /* Move last entry to deleted entry position. */
        SOC_IF_ERROR_RETURN(
          _soc_tunnel_term_entry_shift(unit, from_ent, to_ent));
    } else {
        from_ent = index;
    }

    /* Update number of free & valid entries for this type. */
    SOC_TNL_TERM_STATE_END(unit, type) -= entry_size;

    /* Reset deleted entry. */
    sal_memset(&null_entry, 0, sizeof(soc_tunnel_term_t));
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit,soc_feature_flex_flow) &&
           (entry_type > SOC_TNL_TERM_KEY_TYPE_FIXED_MAX)) {
        SOC_IF_ERROR_RETURN
            (_soc_tunnel_term_flex_entry_clear(unit,
                         from_ent, entry, entry_type));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        SOC_IF_ERROR_RETURN
            (_soc_tunnel_term_entry_write(unit,
                     from_ent, &null_entry, entry_type));
    }
    /* 
     * If last entry for the type was deleted -> remove the type 
     * from hash tracking table. 
     */
    if (SOC_TNL_TERM_STATE_VENT(unit, type) == 0) {
        /* remove from the list */
        prev_type = SOC_TNL_TERM_STATE_PREV(unit, type); /* Always present */
        next_type = SOC_TNL_TERM_STATE_NEXT(unit, type);
        SOC_TNL_TERM_STATE_NEXT(unit, prev_type) = next_type;
        SOC_TNL_TERM_STATE_FENT(unit, prev_type) += SOC_TNL_TERM_STATE_FENT(unit, type);
        SOC_TNL_TERM_STATE_FENT(unit, type) = 0;
        if (next_type != TNL_TERM_TYPE_NULL) {
            SOC_TNL_TERM_STATE_PREV(unit, next_type) = prev_type;
        }
        SOC_TNL_TERM_STATE_NEXT(unit, type) = TNL_TERM_TYPE_NULL;
        SOC_TNL_TERM_STATE_PREV(unit, type) = TNL_TERM_TYPE_NULL;
        SOC_TNL_TERM_STATE_START(unit, type) = TNL_TERM_TYPE_NULL;
        SOC_TNL_TERM_STATE_END(unit, type) = TNL_TERM_TYPE_NULL;
    }

    return(SOC_E_NONE);
}

/*
 * Function:
 *   _soc_tunnel_term_match
 * Purpose:
 *	 Find tunnel terminator entry.  
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      key       - (IN)  Tunnel terminator lookup key.
 *      result    - (OUT) Tunnel terminator search result.
 *      index     - (OUT) Tunnel termination entry index. 
 * Returns: 
 *      SOC_E_XXX
 */ 
STATIC int
_soc_tunnel_term_match(int unit, soc_tunnel_term_t *key,
                       soc_tunnel_term_t *result, int *index)
{
    int         key_index;  /* Entry key lookup result. */
    int         entry_type; /* Entry type               */
    int         rv;         /* Operation return status. */

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        /* Perform lookup hw. */
        int ipv6 = 0;
        uint32 *entry_ptr;
        soc_mem_t mem = L3_TUNNEL_DOUBLEm;

        entry_ptr = (uint32 *)&key->entry_arr[0];
        ipv6 =
            soc_mem_field32_get(unit, mem, entry_ptr, KEY_TYPEf) - 1;

        if (ipv6) {
            mem = L3_TUNNEL_QUADm;
        }
        rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY, -1,
                                    ((void *)key->entry_arr[0].entry_data), result, index);
        if (SOC_SUCCESS(rv)) {
            rv = _soc_tunnel_term_entry_read(unit, *index, result, &entry_type);
        }

        return(rv);
    }

    rv = TNL_TERM_HASH_LOOKUP(unit, key, &key_index);
    if (SOC_SUCCESS(rv)) {
        *index = key_index;
        rv = _soc_tunnel_term_entry_read(unit, key_index, result, &entry_type);
    }
    return (rv);
}


/*
 * Function:
 *   soc_tunnel_term_init
 * Purpose:
 *	 Initialize tunnel terminator table tracking start/end   
 *   valid/free indexes for each tunnel type. 
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 * Returns: 
 *      SOC_E_XXX
 */ 
int
soc_tunnel_term_init(int unit)
{
    int max_type;
    int tnl_term_table_size;
    int tnl_term_state_size;
    int idx;
    int rv = SOC_E_NONE;

    SOC_TNL_TERM_BLOCK_SIZE  = SOC_TNL_TERM_IPV6_ENTRY_WIDTH;

    max_type = MAX_TNL_TERM_TYPES;
    tnl_term_state_size = sizeof(soc_tnl_term_state_t) * max_type;

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        soc_tnl_term_entry_count = 0;
        return(rv);
    }

    /* Allocated tunnel types tracking table. */
    if (!SOC_TNL_TERM_INIT_CHECK(unit)) {
        SOC_TNL_TERM_STATE(unit) = sal_alloc(tnl_term_state_size, "tnl_term_tcam");
        if (NULL == SOC_TNL_TERM_STATE(unit)) {
            return(SOC_E_MEMORY);
        }
    }

    SOC_TNL_TERM_LOCK(unit);
    sal_memset(SOC_TNL_TERM_STATE(unit), 0, tnl_term_state_size);

    /* Initialize all tunnel types ranges to empty ones. */
    for(idx = 0; idx < max_type; idx++) {
        SOC_TNL_TERM_STATE_START(unit, idx) = TNL_TERM_TYPE_NULL;
        SOC_TNL_TERM_STATE_END(unit, idx)   = TNL_TERM_TYPE_NULL;
        SOC_TNL_TERM_STATE_PREV(unit, idx)  = TNL_TERM_TYPE_NULL;
        SOC_TNL_TERM_STATE_NEXT(unit, idx)  = TNL_TERM_TYPE_NULL;
        SOC_TNL_TERM_STATE_VENT(unit, idx)  = 0;
        SOC_TNL_TERM_STATE_FENT(unit, idx)  = 0;
    }

    tnl_term_table_size = soc_mem_index_count(unit, L3_TUNNELm);
    SOC_TNL_TERM_STATE_FENT(unit, (MAX_TNL_TERM_TYPE)) = tnl_term_table_size;

    if (SOC_TNL_TERM_STATE_HASH(unit) != NULL) {
        rv =_soc_tunnel_term_hash_destroy(&SOC_TNL_TERM_STATE_HASH(unit));
        if (SOC_FAILURE(rv)) {
            SOC_TNL_TERM_UNLOCK(unit);
            return SOC_E_INTERNAL;
        }
        SOC_TNL_TERM_STATE_HASH(unit) = NULL;
    }

    rv = _soc_tunnel_term_hash_create(unit, tnl_term_table_size, 
                                      tnl_term_table_size,
                                      &SOC_TNL_TERM_STATE_HASH(unit));

    SOC_TNL_TERM_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *   soc_tunnel_term_deinit
 * Purpose:
 *	 Deinitialize tunnel terminator table. 
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 * Returns: 
 *      SOC_E_XXX
 */ 
int
soc_tunnel_term_deinit(int unit)
{

    SOC_TNL_TERM_LOCK(unit);

    if (SOC_TNL_TERM_STATE_HASH(unit) != NULL) {
        _soc_tunnel_term_hash_destroy(&SOC_TNL_TERM_STATE_HASH(unit));
        SOC_TNL_TERM_STATE_HASH(unit) = NULL;
    }

    if (SOC_TNL_TERM_INIT_CHECK(unit)) {
        sal_free(SOC_TNL_TERM_STATE(unit));
        SOC_TNL_TERM_STATE(unit) = NULL;
    }

    SOC_TNL_TERM_UNLOCK(unit);

    return(SOC_E_NONE);
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *   _soc_tunnel_term_reinit_done
 * Purpose:
 *	 Adjust tunnel types start/end index after warm reboot 
 *   reinitialization is complete.
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 * Returns: 
 *      SOC_E_XXX
 */ 
STATIC int
_soc_tunnel_term_reinit_done(int unit)
{
    int idx, first = 0, first_valid_entry = 0;
    int prev_idx = MAX_TNL_TERM_TYPE;
    int tnl_term_table_size = soc_mem_index_count(unit, L3_TUNNELm);

    SOC_TNL_TERM_STATE_PREV(unit, MAX_TNL_TERM_TYPE) = TNL_TERM_TYPE_NULL;

    for (idx = MAX_TNL_TERM_TYPE; idx >= 0 ; idx--) {
        if (TNL_TERM_TYPE_NULL == SOC_TNL_TERM_STATE_START(unit, idx)) {
            continue;
        }

        SOC_TNL_TERM_STATE_PREV(unit, idx) = prev_idx;
        SOC_TNL_TERM_STATE_NEXT(unit, prev_idx) = idx;

        if ((SOC_TNL_TERM_STATE_START(unit, prev_idx) == TNL_TERM_TYPE_NULL) &&
            (SOC_TNL_TERM_STATE_END(unit, prev_idx) == TNL_TERM_TYPE_NULL)) {
            first = prev_idx;
            first_valid_entry = SOC_TNL_TERM_STATE_START(unit, idx);
        } else {
            SOC_TNL_TERM_STATE_FENT(unit, prev_idx) = 
            SOC_TNL_TERM_STATE_START(unit, idx) - \
                    SOC_TNL_TERM_STATE_END(unit, prev_idx) - 1;       
        } 
        if (SOC_TNL_TERM_STATE_NEXT(unit, idx) == TNL_TERM_TYPE_NULL) {
            SOC_TNL_TERM_STATE_FENT(unit, idx) =
              tnl_term_table_size - SOC_TNL_TERM_STATE_END(unit, idx) - 1;
        }

        prev_idx = idx;
    }
    SOC_TNL_TERM_STATE_FENT(unit, first) = first_valid_entry;
    SOC_TNL_TERM_STATE_NEXT(unit, prev_idx) = TNL_TERM_TYPE_NULL;

    return (SOC_E_NONE);
}


/*
 * Function:
 *   soc_tunnel_term_hash_reinit
 * Purpose:
 *	 reinit tunnel terminator entries.
 * Parameters:
 *      unit      - (IN)  BCM device number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_tunnel_term_hash_reinit(int unit, soc_mem_t mem)
{
    soc_tunnel_term_t entry;/* Tunnel terminator entry. */
    int entry_type;         /* Tunnel entry type.       */
    int idx;                /* Table iteration index.   */
    int idx_max;            /* Maximum Table index.     */
    int exp_key_type = 0x1; /* expected key type */
    uint32 *entry_ptr;

    if (mem == L3_TUNNEL_DOUBLEm) {
        exp_key_type = 0x1;
    } else if (mem == L3_TUNNEL_QUADm) {
        exp_key_type = 0x2;
    }
    idx = soc_mem_index_min(unit, mem);
    idx_max = soc_mem_index_max(unit, mem);

    while (idx <= idx_max) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                             (uint32 *)&entry.entry_arr[0]));

        entry_ptr = (uint32 *)&entry.entry_arr[0];
        entry_type =
            soc_mem_field32_get(unit, mem, entry_ptr, KEY_TYPEf);

        /* Skip invalid entries. */
        if (!soc_mem_field32_get(unit, mem,
                             (uint32 *)&entry.entry_arr[0], BASE_VALID_0f)) {
            idx++;
            continue;
        }

        if (entry_type == exp_key_type) {
            soc_tnl_term_entry_count++;
        }
        idx += 1;
    }
    return SOC_E_NONE;
}
/*
 * Function:
 *   soc_tunnel_term_reinit
 * Purpose:
 *	 Insert tunnel terminator entry.
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 * Returns: 
 *      SOC_E_XXX
 */ 
int
soc_tunnel_term_reinit(int unit)
{
    soc_tunnel_term_t entry;/* Tunnel terminator entry. */
    int entry_size;         /* Entry size.              */ 
    int entry_type;         /* Tunnel entry type.       */
    int type;               /* soc Tunnel  type.       */
    int idx;                /* Table iteration index.   */
    int idx_max;            /* Maximum Table index.     */
    int rv;                 /* Operation return status. */

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        rv = soc_tunnel_term_hash_reinit(unit, L3_TUNNEL_DOUBLEm);
        if (SOC_E_NONE != rv) {
            return rv;
        }
        rv = soc_tunnel_term_hash_reinit(unit, L3_TUNNEL_QUADm);
        return rv;
    }
    if (!SOC_MEM_IS_VALID(unit,L3_TUNNELm)) {
        /* if (SOC_IS_HURRICANE(unit)) */
        return SOC_E_NONE;
    }

    idx = soc_mem_index_min(unit, L3_TUNNELm);
    idx_max = soc_mem_index_max(unit, L3_TUNNELm);

    while (idx <= idx_max) {
        rv = _soc_tunnel_term_entry_read(unit, idx, &entry, &entry_type);
        /* Skip invalid entries. */
        if (SOC_E_NOT_FOUND == rv) {
            idx++;
            continue;
        }

        /* If read failed return. */
        if (SOC_FAILURE(rv)){
            return (rv);
        }

        /* Get entry type.(SIP/DIP SIP6/DIP6 DIP/(*) DIP6/(*) */
        SOC_IF_ERROR_RETURN
            (_soc_tunnel_term_type_get(unit, &entry, &type, &entry_type));

#ifdef BCM_TRIDENT3_SUPPORT
        if (soc_feature(unit,soc_feature_flex_flow) &&
               (entry_type > SOC_TNL_TERM_KEY_TYPE_FIXED_MAX)) {
            SOC_IF_ERROR_RETURN (
            _soc_tunnel_term_flex_type_width_get(unit, type,
                       &entry, &entry_size));
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            entry_size = (entry_type == 0x1) ? SOC_TNL_TERM_IPV6_ENTRY_WIDTH :
               SOC_TNL_TERM_IPV4_ENTRY_WIDTH;
        }

        /* Insert type into software range tracking structure. */ 
        if (SOC_TNL_TERM_STATE_VENT(unit, type) == 0) {
            SOC_TNL_TERM_STATE_START(unit, type) = idx;
        }

        SOC_TNL_TERM_STATE_END(unit, type) = idx + entry_size - 1;
        SOC_TNL_TERM_STATE_VENT(unit, type)+= entry_size;
        TNL_TERM_HASH_INSERT(unit, &entry, idx);
        idx += entry_size;
    }

    rv = _soc_tunnel_term_reinit_done(unit);
    return (rv);
}
#endif /* BCM_WARM_BOOT_SUPPORT */




/*
 * Function:
 *   soc_tunnel_term_insert
 * Purpose:
 *	 Insert tunnel terminator entry.  
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      entry     - (IN)  Inserted Tunnel terminator entry.
 *      index     - (OUT) Index used for inserted entry.
 * Returns: 
 *      SOC_E_XXX
 */ 
int
soc_tunnel_term_insert(int unit, soc_tunnel_term_t *entry, uint32 *index)
{
    soc_tunnel_term_t lkup_result;  /* Tunnel terminator identical */
    /* entry lookup result.        */
    int   temp_index;               /* Entry write index.          */
    int   entry_type;               /* entry type                  */
    int   rv = SOC_E_NONE;          /* Operation return status.    */


    /* Input parameters check. */
    if (NULL == entry) {
        return (SOC_E_PARAM);
    }


    SOC_TNL_TERM_LOCK(unit);

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        int ipv6 = 0;
        uint32 *entry_ptr;
        soc_mem_t mem = L3_TUNNEL_DOUBLEm;

        entry_ptr = (uint32 *)&entry->entry_arr[0];
        ipv6 =
            soc_mem_field32_get(unit, mem, entry_ptr, KEY_TYPEf) - 1;

        if (ipv6) {
            mem = L3_TUNNEL_QUADm;
        }

        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, (void *)entry);

        if (SOC_E_EXISTS == rv) {
            rv = SOC_E_NONE;
        }
        if (SOC_SUCCESS(rv)) {
            soc_tnl_term_entry_count++;
            if (ipv6) {
                /* IPv6 entries occupy 2 entries */
                soc_tnl_term_entry_count++;
            }
        }
        SOC_TNL_TERM_UNLOCK(unit);
        return(rv);
    }

    /* Check if identical entry already exists. */
    rv = _soc_tunnel_term_match(unit, entry, &lkup_result, &temp_index);

    if (rv == SOC_E_NOT_FOUND) {
        /* Allocate a new slot if we are writing a new entry. */
        rv = _soc_tunnel_term_slot_allocate(unit, entry, &temp_index);
    }

    if (SOC_SUCCESS(rv)) {
  
        rv = _soc_tunnel_term_type_get(unit, entry, NULL, &entry_type);
        if (SOC_FAILURE(rv)) {
            SOC_TNL_TERM_UNLOCK(unit);
            return(rv);
        }
        
        /* Entry already present. Update the entry */
        TNL_TERM_HASH_INSERT(unit, entry, temp_index);
        rv = _soc_tunnel_term_entry_write(unit, temp_index, entry, entry_type);
        *index = temp_index;
    }
    SOC_TNL_TERM_UNLOCK(unit);
    return(rv);
}

/*
 * Function:
 *   soc_tunnel_term_delete
 * Purpose:
 *	 Delete tunnel terminator entry.  
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      key       - (IN)  Deleted Tunnel terminator entry.
 * Returns: 
 *      SOC_E_XXX
 */ 
int
soc_tunnel_term_delete(int unit, soc_tunnel_term_t *key)
{
    soc_tunnel_term_t hw_entry; /* Entry lookup result.     */
    int index;                        /* Lookup result index.     */
    int rv;                           /* Operation return status. */

    /* Input parameters check. */
    if (NULL == key) {
        return (SOC_E_PARAM);
    }

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        int ipv6 = 0;
        uint32 *entry_ptr;
        soc_mem_t mem = L3_TUNNEL_DOUBLEm;

        entry_ptr = (uint32 *)&key->entry_arr[0];
        ipv6 =
            soc_mem_field32_get(unit, mem, entry_ptr, KEY_TYPEf) - 1;

        if (ipv6) {
            mem = L3_TUNNEL_QUADm;
        }

        rv = soc_mem_generic_delete(unit, mem, MEM_BLOCK_ANY,
                                    -1, ((void *)key->entry_arr[0].entry_data),
                                    NULL, 0);
        if (SOC_SUCCESS(rv)) {
            soc_tnl_term_entry_count--;
            if (ipv6) {
                /* IPv6 entries occupy 2 entries */
                soc_tnl_term_entry_count--;
            }
        }
        return(rv);
    }

    SOC_TNL_TERM_LOCK(unit);
    /* Find entry matching deletion key. */
    rv = _soc_tunnel_term_match(unit, key, &hw_entry, &index);
    if (SOC_SUCCESS(rv)) {
        TNL_TERM_HASH_DELETE(unit, key, index);
        rv = _soc_tunnel_term_slot_free(unit, &hw_entry, index);
    }
    SOC_TNL_TERM_UNLOCK(unit);
    return(rv);
}
#undef TNL_TERM_HASH_INSERT
#undef TNL_TERM_HASH_DELETE
#undef TNL_TERM_HASH_LOOKUP

/*
 * Function:
 *      soc_tunnel_term_match
 * Purpose:
 *	    Flush all tunnel terminator entries. 
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 * Returns: 
 *      SOC_E_XXX
 */ 
int
soc_tunnel_term_delete_all(int unit)
{
    return soc_tunnel_term_init(unit);
}

/*
 * Function:
 *   soc_tunnel_term_match
 * Purpose:
 *	 Find tunnel terminator entry.  
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      key       - (IN)  Tunnel terminator lookup key.
 *      result    - (OUT) Tunnel terminator search result.
 *      index     - (OUT) Tunnel termination entry index. 
 * Returns: 
 *      SOC_E_XXX
 */ 
int
soc_tunnel_term_match(int unit, soc_tunnel_term_t *key,
                      soc_tunnel_term_t *result, int *entry_index)
{
    int       rv = SOC_E_NONE;  /* Operation return status. */
    int       index;    /* Lookup result index.     */

    /* Input parameters check. */
    if ((NULL == result) || (NULL == key)) {
        return (SOC_E_PARAM);
    }

    SOC_TNL_TERM_LOCK(unit);

    rv = _soc_tunnel_term_match(unit, key, result, &index);
    *entry_index = index;
    SOC_TNL_TERM_UNLOCK(unit);
    return(rv);
}

/*
 * Function:
 *   soc_tunnel_term_used_get
 * Purpose:
 *	 Obtain Used Tunnel terminator entry size
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 * Returns: 
 *      used_count
 */ 
int 
soc_tunnel_term_used_get(int unit)
{
   int used_count=0;

   if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
       return soc_tnl_term_entry_count;
   }
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_SIP4);    
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_PROT);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_SIP4_L4DST);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_L4DST);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4DST);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_PROT_L4DST);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_SIP4_L4SRC);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_L4SRC);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_PROT_L4SRC);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4SRC);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_PROT_L4SRC);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_SIP4_L4DST_L4SRC);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_L4DST_L4SRC);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_SIP4_PROT_L4DST_L4SRC);
    used_count += SOC_TNL_TERM_STATE_VENT(unit, SOC_TNL_TERM_TYPE_DIP4_PROT_L4DST_L4SRC);

    return used_count;
}
#endif /* BCM_FIREBOLT_SUPPORT */
#endif /* INCLUDE_L3 */

