/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Triumph LPM TCAM table insert/delete/lookup routines
 * _bcm_tr_ext_lpm_init - Called from bcm_l3_init
 * _bcm_tr_ext_lpm_insert - Insert/Update an IPv4/IPV6 route entry into LPM table
 * _bcm_tr_ext_lpm_delete - Delete an IPv4/IPV6 route entry from LPM table
 * _bcm_tr_ext_lpm_match  - (Exact match for the key. Will match both IP address
 *                      and mask)
 *
 * Hit bit preservation - May loose HIT bit state when entries are moved
 * around due to race condition. This happens if the HIT bit gets set in
 * hardware after reading the entry to be moved and before the move is
 * completed. If the HIT bit for an entry is already set when the move is
 * initiated then it is preserved.
 */

#include <soc/defs.h>

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH_SUPPORT)  && defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/l3x.h>
#include <soc/er_tcam.h>

#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>

/*
 * TCAM based LPM implementation. Each table entry can hold two IPV4 entries or
 * one IPV6 entry. VRF independent routes placed at the beginning or 
 * at the end of table based on application provided entry vrf id 
 * (BCM_L3_VRF_OVERRIDE/BCM_L3_VRF_GLOBAL).   
 *
 *              _TR_LPM_PREFIX_MAX
 *
 * lpm_prefix_index[96].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 32  ==
 *                                 ==    VRF OVERRIDE ROUTES    ==
 * lpm_prefix_index[95].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[64].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 0   ==
 *                                 ==    VRF OVERRIDE ROUTES    ==
 * lpm_prefix_index[64].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[63].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 32  ==
 *                                 ==    VRF SPECIFIC ROUTES    ==
 * lpm_prefix_index[63].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[32].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 0   ==
 *                                 ==    VRF SPECIFIC ROUTES    ==
 * lpm_prefix_index[32].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[31].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 32  ==
 *                                 ==    VRF GLOBAL ROUTES      ==
 * lpm_prefix_index[31].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[0].begin --->  ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 0   ==
 *                                 ==    VRF GLOBAL ROUTES      ==
 * lpm_prefix_index[0].end   --->  ===============================
 */

typedef struct _tr_ext_lpm_state_s {
    int start;  /* start index for this prefix length */
    int end;    /* End index for this prefix length */
    int prev;   /* Previous (Lo to Hi) prefix length with non zero entry count*/
    int next;   /* Next (Hi to Lo) prefix length with non zero entry count */
    int vent;   /* valid entries */
    int fent;   /* free entries */
} _tr_ext_lpm_state_t, *_tr_ext_lpm_state_p;

#define SOC_MEM_COMPARE_RETURN(a, b) {          \
        if ((a) < (b)) { return -1; }           \
        if ((a) > (b)) { return  1; }           \
}

#define _TR_LPM_IPV4  (0)
#define _TR_LPM_IPV6  (1)
#define _TR_LPM_IP_VERSIONS (2)

#define _TR_LPM_BLOCK_SZ (0x200)

#define _TR_LPM_ROUTE_IS_V6(_entry_)        \
       (((_entry_)->defip_flags & BCM_L3_IP6) ?  _TR_LPM_IPV6 : _TR_LPM_IPV4)


static _tr_ext_lpm_state_p _tr_lpm_state[_TR_LPM_IP_VERSIONS][SOC_MAX_NUM_DEVICES];

#define _TR_LPM_INIT_CHECK(_u_, _v_) \
                    (_tr_lpm_state[(_v_)][(_u_)] != NULL)
#define _TR_LPM_STATE(_u_, _v_)  \
                    (_tr_lpm_state[(_v_)][(_u_)]) 
#define _TR_LPM_STATE_START(_u_, _v_, _pfx_) \
                    (_tr_lpm_state[(_v_)][(_u_)][(_pfx_)]).start 
#define _TR_LPM_STATE_END(_u_, _v_, _pfx_) \
                    (_tr_lpm_state[(_v_)][(_u_)][(_pfx_)]).end
#define _TR_LPM_STATE_PREV(_u_, _v_, _pfx_)  \
                    (_tr_lpm_state[(_v_)][(_u_)][(_pfx_)]).prev
#define _TR_LPM_STATE_NEXT(_u_, _v_, _pfx_) \
                    (_tr_lpm_state[(_v_)][(_u_)][(_pfx_)]).next
#define _TR_LPM_STATE_VENT(_u_, _v_, _pfx_) \
                    (_tr_lpm_state[(_v_)][(_u_)][(_pfx_)]).vent
#define _TR_LPM_STATE_FENT(_u_, _v_, _pfx_) \
                    (_tr_lpm_state[(_v_)][(_u_)][(_pfx_)]).fent
#define _TR_LPM_PREFIX_COUNT(_mem_) \
            (((_mem_) == EXT_IPV4_DEFIPm) ?  33 :  \
              ((_mem_) == EXT_IPV6_64_DEFIPm) ?  65 : 129)
/* 1 set for VRF_OVERRIDE, 1 set for VRF DEPENDENT, 1 set for VRF_GLOBAL */
#define _TR_LPM_PREFIX_MAX(_mem_) (3 * _TR_LPM_PREFIX_COUNT(_mem_))
                 
#define _TR_LPM_PREFIX_MAX_INDEX(_mem_) (_TR_LPM_PREFIX_MAX(_mem_) - 1)

/* IPv4 tcam software entry. */
typedef struct _tr_lpm_v4_key_s {
    uint32      addr;           /* IPv4 address. */
    uint32      vrf;            /* Vrf id.                             */ 
    uint32      next_entry:21;  /* Next entry producing identical hash.*/
} _tr_lpm_v4_key_t;

/* IPv6 tcam software entry. */
typedef struct _tr_lpm_v6_key_s {
    uint8       addr[16];        /* IPv6 address.                        */
    uint32      vrf;             /* VRF id.                              */
    uint32      next_entry:21;   /* Next entry producing identical hash. */
} _tr_lpm_v6_key_t;

/* IPv6 tcam software image. */
typedef struct _tr_lpm_sw_image_s {
    int         unit;
    int         hash_size;      /* Number entries in hash table       */
    int         *hash_table;    /* Hash table with 16 bit index.      */
    soc_mem_t   mem;            /* Combined route table view memory.  */
    soc_mem_t   data_mem;       /* Memory route data resides in.      */
    soc_mem_t   hit_bit_mem;    /* Memory route hit_bit resides in.   */
    _tr_lpm_v4_key_t *fib4;     /* Sw image of ipv4 route table.   */
    _tr_lpm_v6_key_t *fib6;     /* Sw image of ipv6 route table.   */
} _tr_lpm_sw_image_t, *_tr_lpm_sw_image_p;


static _tr_lpm_sw_image_p _lpm_sw_image[_TR_LPM_IP_VERSIONS][SOC_MAX_NUM_DEVICES];

#define _TR_LPM_SW_IMAGE(_u_, _v_) (_lpm_sw_image[(_v_)][(_u_)])
#define _TR_LPM_MEM(_u_, _v_) (_TR_LPM_SW_IMAGE((_u_), (_v_))->mem)
#define _TR_LPM_DATA_MEM(_u_, _v_) (_TR_LPM_SW_IMAGE((_u_), (_v_))->data_mem)
#define _TR_LPM_HIT_BIT_MEM(_u_, _v_) \
                   (_TR_LPM_SW_IMAGE((_u_), (_v_))->hit_bit_mem)
#define _TR_LPM_FIB4(_u_) (_TR_LPM_SW_IMAGE((_u_), (_TR_LPM_IPV4))->fib4)
#define _TR_LPM_FIB6(_u_) (_TR_LPM_SW_IMAGE((_u_), (_TR_LPM_IPV6))->fib6)


#define _TR_LPM_HASH_INDEX_NULL  (0x1FFFFF)
#define _TR_LPM_HASH_SZ   (5)
typedef uint32 _tr_ext_lpm_hash_entry_t[_TR_LPM_HASH_SZ];
#define _TR_LPM_HASH_ENTRY_IPV6_GET(_entry_, _odata_)                    \
    sal_memcpy((_odata_), (_entry_)->defip_ip6_addr, sizeof(bcm_ip6_t)); \
    _odata_[4] = (_entry_)->defip_vrf

#define _TR_LPM_HASH_ENTRY_IPV4_GET(_entry_, _odata_)                   \
    sal_memset((_odata_), 0, _TR_LPM_HASH_SZ * sizeof(uint32));         \
    sal_memcpy((_odata_), &(_entry_)->defip_ip_addr, sizeof(bcm_ip_t)); \
    _odata_[4] = (_entry_)->defip_vrf


/* 
 * Function:
 *      _tr_lpm_sw_image_create
 * Purpose:
 *      Create an sw image of external lpm table.
 * Parameters:
 *      unit   - (IN) Device unit
 *      v6     - (IN) IPv6 sw image indicator. 
 *      mem    - (IN) Memory route table resides in.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int 
_tr_lpm_sw_image_create(int unit, int v6, soc_mem_t mem) 
{
    _tr_lpm_sw_image_p sw_image;     /* Allocated sw image.       */
    int     index;                   /* Sw image iteration index. */  
    int     mem_size;                /* Allocate memory size.     */

    /* 
     * Allocate software image. 
     */
    sw_image = sal_alloc(sizeof(_tr_lpm_sw_image_t), "External lpm sw image");
    if (sw_image == NULL) {
        return (BCM_E_MEMORY);
    }

    sal_memset(sw_image, 0, sizeof(_tr_lpm_sw_image_t));

    /*
     *  Initialize sw image properties (unit, hash size, memory size).
     */
    sw_image->unit = unit;
    sw_image->hash_size = (1 << BYTES2BITS(sizeof(uint16)));
    switch(mem) {
      case EXT_IPV4_DEFIPm:
          sw_image->mem = EXT_IPV4_DEFIPm;
          sw_image->data_mem = EXT_DEFIP_DATA_IPV4m;
          sw_image->hit_bit_mem = EXT_DST_HIT_BITS_IPV4m;
          break;
      case EXT_IPV6_64_DEFIPm:
          sw_image->mem = EXT_IPV6_64_DEFIPm;
          sw_image->data_mem = EXT_DEFIP_DATA_IPV6_64m;
          sw_image->hit_bit_mem = EXT_DST_HIT_BITS_IPV6_64m;
          break;
      case EXT_IPV6_128_DEFIPm:
          sw_image->mem = EXT_IPV6_128_DEFIPm;
          sw_image->data_mem = EXT_DEFIP_DATA_IPV6_128m;
          sw_image->hit_bit_mem = EXT_DST_HIT_BITS_IPV6_128m;
          break;
      default:
          sal_free(sw_image);
          return (BCM_E_PARAM);
    }

    /*
     * Pre-allocate the hash table storage.
     */
    mem_size = sw_image->hash_size * sizeof(int);
    sw_image->hash_table = sal_alloc(mem_size, "External lpm hash table");
    if (NULL == sw_image->hash_table) {
        sal_free(sw_image);
        return (BCM_E_MEMORY);
    }

    /*
     * Pre-allocate a complete sw view of installed entries.
     */
    if (v6) {
        mem_size = soc_mem_index_count(unit, sw_image->mem) *  \
                   sizeof(_tr_lpm_v6_key_t); 
        sw_image->fib6 = sal_alloc(mem_size, "External lpm fib");
        if (NULL == sw_image->fib6) {
            sal_free(sw_image->hash_table);
            sal_free(sw_image);
            return (BCM_E_MEMORY);
        }
        sal_memset(sw_image->fib6, 0, mem_size);
    } else {
        mem_size = soc_mem_index_count(unit, sw_image->mem) *  \
                   sizeof(_tr_lpm_v4_key_t); 
        sw_image->fib4 = sal_alloc(mem_size, "External lpm fib");
        if (NULL == sw_image->fib4) {
            sal_free(sw_image->hash_table);
            sal_free(sw_image);
            return (BCM_E_MEMORY);
        }
        sal_memset(sw_image->fib4, 0, mem_size);
    }


    /*
     * Set the entries in the hash table to _TR_LPM_HASH_INDEX_NULL
     * Link the entries beyond hash->index_max for handling collisions
     */
    for(index = 0; index < sw_image->hash_size; index++) {
        sw_image->hash_table[index] = _TR_LPM_HASH_INDEX_NULL;
    }

    for(index = 0; index < soc_mem_index_count(unit,sw_image->mem); index++) {
        if (v6) { 
            sw_image->fib6[index].next_entry = _TR_LPM_HASH_INDEX_NULL;
        } else {
            sw_image->fib4[index].next_entry = _TR_LPM_HASH_INDEX_NULL;
        }
    }

    _TR_LPM_SW_IMAGE(unit, v6) = sw_image;

    return (BCM_E_NONE);
}

/* 
 * Function:
 *      _tr_lpm_sw_image_destroy
 * Purpose:
 *      Destroy the sw image table
 * Parameters:
 *      unit - (IN) BCM device nu
 *      v6   - (IN) IP version. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_tr_lpm_sw_image_destroy(int unit, int v6)
{
    if (NULL != _TR_LPM_SW_IMAGE(unit, v6)) {
        if (NULL != _TR_LPM_SW_IMAGE(unit, v6)->hash_table)  {
            sal_free(_TR_LPM_SW_IMAGE(unit, v6)->hash_table);
        }

        if ((v6) && (NULL != _TR_LPM_FIB6(unit))) {
            sal_free(_TR_LPM_FIB6(unit));
        } 
        
        if ((!v6) &&  (NULL != _TR_LPM_FIB4(unit))) {
            sal_free(_TR_LPM_FIB4(unit));
        }
        sal_free(_TR_LPM_SW_IMAGE(unit, v6));
    }

    _TR_LPM_SW_IMAGE(unit, v6) = NULL;
    return (BCM_E_NONE);
}


/* 
 * Function:
 *      _tr_ext_lpm_hash_compute
 * Purpose:
 *      Compute CRC hash for key data.
 * Parameters:
 *      data - (IN) Key data
 *      hash - (OUT)Computed 16 bit hash
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_tr_ext_lpm_hash_compute(_bcm_defip_cfg_t *data, uint16 *hash)
{
    bcm_ip6_t v6_mask;                 /* IPv6 subnet mask. */
    uint32  v4_mask;                   /* IPv4 subnet mask. */
    _tr_ext_lpm_hash_entry_t buf;      /* Scratch buffer.   */

    if ((NULL == data) || (NULL == hash)) {
        return (BCM_E_PARAM);
    }

    if (_TR_LPM_ROUTE_IS_V6(data)) {
        /* Create mask from prefix length. */
        bcm_ip6_mask_create(v6_mask, data->defip_sub_len);
        /* Apply prefix mask. */
        bcm_xgs3_l3_mask6_apply(v6_mask, data->defip_ip6_addr);
        /* Extract buffer for hash */
        _TR_LPM_HASH_ENTRY_IPV6_GET(data, buf); 

    } else {
        /* Create mask from prefix length. */
        v4_mask = BCM_IP4_MASKLEN_TO_ADDR(data->defip_sub_len);
        /* Apply prefix mask. */
        data->defip_ip_addr &= v4_mask;
        /* Extract buffer for hash */
        _TR_LPM_HASH_ENTRY_IPV4_GET(data, buf); 
    }

    /* Calculate hash value. */
    *hash = _shr_crc16b(0, (void *)buf, BYTES2BITS(_TR_LPM_HASH_SZ * sizeof(uint32)));

    return (BCM_E_NONE);
}

/* 
 * Function:
 *      _tr_ext_lpm_sw_entry_reset
 * Purpose:
 *      Reset software  entry.
 * Parameters:
 *      unit       - (IN) BCM device number. 
 *      v6         - (IN) IPv6 entry flag.
 *      index      - (IN) SW entry index. 
 * Returns:
 *      BCM_E_XXX
 */
static INLINE int
_tr_ext_lpm_sw_entry_reset(int unit, int v6, int index)
{
    if (v6) {
        sal_memset(_TR_LPM_FIB6(unit) + index, 0, sizeof (_tr_lpm_v6_key_t));
        _TR_LPM_FIB6(unit)[index].next_entry = _TR_LPM_HASH_INDEX_NULL;
    } else {
        sal_memset(_TR_LPM_FIB4(unit) + index, 0, sizeof (_tr_lpm_v4_key_t));
        _TR_LPM_FIB4(unit)[index].next_entry = _TR_LPM_HASH_INDEX_NULL;
    }
    return (BCM_E_NONE);
}

/* 
 * Function:
 *      _tr_ext_lpm_key_compare
 * Purpose:
 *      Compare API provided route entry with entry in sw image.
 * Parameters:
 *      unit        - (IN) Bcm device number.
 *      index       - (IN) SW image entry index.
 *      data        - (IN) Route entry. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_tr_ext_lpm_key_compare(int unit, int index, _bcm_defip_cfg_t *data)
{
    bcm_ip6_t v6_mask;                 /* IPv6 subnet mask.    */
    int        result;                 /* Comparison result.   */
    int            v6;                 /* IPv6 route indicator.*/         

    if (NULL == data)  {
        return (BCM_E_PARAM);
    }

    v6 = _TR_LPM_ROUTE_IS_V6(data);

    /* Perform index range sanity check. */
    if ((index < 0) || 
        (index >= soc_mem_index_count(unit, _TR_LPM_MEM(unit, v6)))) {
        return (BCM_E_PARAM);
    }


    /* Prefix comparison. */
    if (v6) {
        /* Vrf comparison. */
        if (data->defip_vrf != _TR_LPM_FIB6(unit)[index].vrf) {
            return (-1);
        } 
        /* IPv6 Create mask from prefix length. */
        bcm_ip6_mask_create(v6_mask, data->defip_sub_len);
        /* Apply prefix mask. */
        bcm_xgs3_l3_mask6_apply(v6_mask, data->defip_ip6_addr);
        /* Compare prefixes */
        result = sal_memcmp(data->defip_ip6_addr, 
                            _TR_LPM_FIB6(unit)[index].addr, 
                            sizeof(bcm_ip6_t));
    }  else {
        /* Vrf comparison. */
        if (data->defip_vrf != _TR_LPM_FIB4(unit)[index].vrf) {
            return (-1);
        } 
        /* Apply prefix mask. */
        data->defip_ip_addr &= BCM_IP4_MASKLEN_TO_ADDR(data->defip_sub_len);
        /* Compare prefixes */
        result = sal_memcmp(&data->defip_ip_addr, 
                            &_TR_LPM_FIB4(unit)[index].addr, sizeof(bcm_ip_t));
    }

    return (result);
}
/*
 * Function:
 *      _tr_ext_lpm_prefix_length_get
 * Purpose:
 *      Extract vrf weighted prefix length from the route entry.
 * Parameters:
 *      unit    - (IN)BCM device number.
 *      entry   - (IN)Lookup key. 
 *      pfx_len     - (OUT)Prefix length.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_tr_ext_lpm_prefix_length_get(int unit, _bcm_defip_cfg_t *entry, int *pfx_len)
{
    soc_mem_t   mem;      /* Route table memory.   */
    int          v6;      /* IPv6 entry indicator. */ 

    /* Input parameters check. */
    if ((NULL == entry) || (NULL == pfx_len)) {
        return (BCM_E_PARAM);
    }

    v6  = _TR_LPM_ROUTE_IS_V6(entry);
    mem = _TR_LPM_MEM(unit, v6);

    switch (entry->defip_vrf) { 
      case BCM_L3_VRF_GLOBAL:
          *pfx_len = entry->defip_sub_len;
          break;
      case BCM_L3_VRF_OVERRIDE:
          *pfx_len = entry->defip_sub_len + 2 * _TR_LPM_PREFIX_COUNT(mem);
          break;
      default:   
          *pfx_len = entry->defip_sub_len + _TR_LPM_PREFIX_COUNT(mem);
          break;
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _tr_ext_lpm_reset
 * Purpose:
 *      Flush  an entry from external route table.
 * Parameters:
 *      unit       - (IN) BCM unit number.
 *      v6         - (IN) IPv6 entry indicator. 
 *      index      - (IN) Entry index.  
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_tr_ext_lpm_reset(int unit, int v6, int index)
{
    soc_mem_t       mem;                       /* Route table memory.    */ 

    /* Get memory name. */
    mem = _TR_LPM_MEM(unit, v6);

    /* Check index range. */
    if ((index > soc_mem_index_max(unit, mem)) || 
        (index < soc_mem_index_min(unit, mem))) {
        return (BCM_E_PARAM); 
    }

    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, 
                         &soc_mem_entry_null(unit, mem));
}


/*
 * Function:
 *      _tr_ext_lpm_write
 * Purpose:
 *      Get an entry from external route table.
 * Parameters:
 *      unit           - (IN) BCM unit number.
 *      data           - (IN) Entry data. 
 *      nh_ecmp_idx    - (IN) Next hop ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_tr_ext_lpm_write(int unit, _bcm_defip_cfg_t *data, int nh_ecmp_idx)
               
{
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS];       /* Buffer for HW entry.   */
    soc_mem_t       mem;                       /* Route table memory.    */ 
    bcm_ip6_t   v6_mask;                       /* IPv6 route mask.       */
    int          vrf_id;                       /* VRF id.                */
    int        vrf_mask;                       /* VRF mask.              */
    int       field_len;                       /* Vrf field length.      */
    uint32   field_mask;                       /* Vrf field mask.        */
    int              v6;                       /* IPv6 route indicator.  */

    /* Input parameters check. */
    if (NULL == data) {
        return (BCM_E_PARAM);
    }

    v6  = _TR_LPM_ROUTE_IS_V6(data);
    mem = _TR_LPM_MEM(unit, v6);

    /* Entry index sanity check. */
    if ((data->defip_index > soc_mem_index_max(unit, mem)) ||
        (data->defip_index < soc_mem_index_min(unit, mem))) {
        return (BCM_E_PARAM);
    }

    /* Extract entry vrf id & vrf mask. */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_internal_lpm_vrf_calc(unit, data, &vrf_id, &vrf_mask));

    /* Reset hw buffer first. */
    sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Set valid bit. */
    soc_mem_field32_set(unit, mem, buf, VALIDf, 0x1);

    /* Check if entry points to ecmp group. */
    if (data->defip_flags & BCM_L3_MULTIPATH) {  
        /* Mark entry as ECMP & set ecmp group id. */
        soc_mem_field32_set(unit, mem, buf, ECMPf, 0x1);
        soc_mem_field32_set(unit, mem, buf, ECMP_PTRf, nh_ecmp_idx);
    } else {
        soc_mem_field32_set(unit, mem, buf, NEXT_HOP_INDEXf, nh_ecmp_idx);
    }

    /* Set priority override bit & entry priority. */
    if (data->defip_flags & BCM_L3_RPE) {
        soc_mem_field32_set(unit, mem, buf, RPEf, 0x1);
        /* Set entry priority. */
        soc_mem_field32_set(unit, mem, buf, PRIf, data->defip_prio);
    }

    /* Set destination discard flag. */
    if (data->defip_flags & BCM_L3_DST_DISCARD) {
        soc_mem_field32_set(unit, mem, buf, DST_DISCARDf, 0x1);
    }

    /* Set classification group id. */
    soc_mem_field32_set(unit, mem, buf, CLASS_IDf, 
                        data->defip_lookup_class);

    /* Set hit bit . */
    if (data->defip_flags & BCM_L3_HIT) {
        soc_mem_field32_set(unit, mem, buf, DST_HITf, 0x1);
    }

    /* Set default route indication. */
    if (0 == data->defip_sub_len) {
        soc_mem_field32_set(unit, mem, buf, DEFAULTROUTEf, 0x1);
    }

    /* Set Global route flag. */
    if (BCM_L3_VRF_GLOBAL == data->defip_vrf) { 
        soc_mem_field32_set(unit, mem, buf, GLOBAL_ROUTEf, 0x1);
    }

    /* Set vrf id. */
    field_len = soc_mem_field_length(unit, mem, VRF_LOf);
    field_mask = (1 << field_len) - 1;

    soc_mem_field32_set(unit, mem, buf, VRF_LOf, vrf_id & field_mask);
    soc_mem_mask_field32_set(unit, mem, buf, MASK_VRF_LOf, 
                             vrf_mask & field_mask);

    if (SOC_MEM_FIELD_VALID(unit, mem, VRF_HIf)) {
        soc_mem_field32_set(unit, mem, buf, VRF_HIf, vrf_id >> field_len);
        soc_mem_mask_field32_set(unit, mem, buf, MASK_VRF_HIf, 
                                 vrf_mask >> field_len);
    }

    /* Set ip address. */
    if (v6) {
        bcm_ip6_mask_create(v6_mask, data->defip_sub_len);
        bcm_xgs3_l3_mask6_apply(v6_mask, data->defip_ip6_addr);

        if (SOC_MEM_FIELD_VALID(unit, mem, IP_ADDRf)) {
            /* Set address upper part (0-63). */
            soc_mem_ip6_addr_set(unit, mem, buf, IP_ADDRf, 
                                 data->defip_ip6_addr, SOC_MEM_IP6_UPPER_ONLY);
            /* Set address mask upper part (0-63). */
            soc_mem_ip6_addr_mask_set(unit, mem, buf, MASK_IP_ADDRf, 
                                      v6_mask, SOC_MEM_IP6_UPPER_ONLY);
        } else {
            /* Set address upper part (0-63). */
            soc_mem_ip6_addr_set(unit, mem, buf, IP_ADDR_HIf, 
                                 data->defip_ip6_addr, SOC_MEM_IP6_UPPER_ONLY);
            /* Set address mask upper part (0-63). */
            soc_mem_ip6_addr_mask_set(unit, mem, buf, MASK_IP_ADDR_HIf, 
                                      v6_mask, SOC_MEM_IP6_UPPER_ONLY);

            /* Set address lower part (64-127). */
            soc_mem_ip6_addr_set(unit, mem, buf, IP_ADDR_LOf, 
                                 data->defip_ip6_addr, SOC_MEM_IP6_LOWER_ONLY);
            /* Set address mask upper part (64-127). */
            soc_mem_ip6_addr_mask_set(unit, mem, buf, MASK_IP_ADDR_LOf, 
                                      v6_mask, SOC_MEM_IP6_LOWER_ONLY);
        }
    } else {
        /* Set ip  address. */
        soc_mem_field32_set(unit, mem, buf, IP_ADDRf, data->defip_ip_addr);
        /* Set ip  address mask. */
        soc_mem_mask_field32_set(unit, mem, buf, MASK_IP_ADDRf,
                                 BCM_IP4_MASKLEN_TO_ADDR(data->defip_sub_len));
    }

    /* Mask reserved bits as "don't compare" */
    if (SOC_MEM_FIELD_VALID(unit, mem, MASK_RESERVEDf)) {
        soc_mem_mask_field32_set(unit, mem, buf, MASK_RESERVEDf, 0);
    }

    /* Write entry to the hardware. */
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                       data->defip_index, buf));

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _tr_ext_lpm_parse_route_data
 * Purpose:
 *      Parse  an entry from external route table.
 * Parameters:
 *      unit           - (IN)  SOC unit number.
 *      v6             - (IN)  IPv6 route indicator.
 *      index          - (IN)  Entry index to read.
 *      sub_net_length - (IN)  Subnet prefix length.
 *      data_buf       - (IN)  filled EXT_DATA_MEMORY buffer.
 *      data           - (OUT) Entry data. 
 *      nh_ecmp_idx    - (OUT) Next hop ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_tr_ext_lpm_parse_route_data(int unit, int v6, int index,
                             int sub_net_length, uint32 *buf, 
                             uint32 *usage_buf, _bcm_defip_cfg_t *data, 
                             int *nh_ecmp_idx)
{
    soc_mem_t data_mem;                        /* Route data memory.         */
    soc_mem_t hit_bit_mem;                     /* Route data memory.         */
    uint32 hit_bit_word;                       /* Hit bit for 32 entries.    */
    int clear_hit;                             /* Clear hit bit.             */

    /* Input parameters check. */
    if ((NULL == data) || (NULL == buf) || (NULL == usage_buf)) {
        return (BCM_E_PARAM);
    }

    /* Get data/hit_bit memory . */
    data_mem = _TR_LPM_DATA_MEM(unit, v6);
    hit_bit_mem = _TR_LPM_HIT_BIT_MEM(unit, v6);
    clear_hit = data->defip_flags & BCM_L3_HIT_CLEAR;

    /* Reset destination buffer. */
    sal_memset(data, 0, sizeof(_bcm_defip_cfg_t));

    /* Set route index in the tcam. */
    data->defip_index = index;

    /* Parse buffer fields. */
    data->defip_flags = (v6) ? BCM_L3_IP6 : 0;

    /* Check if entry points to ecmp group. */
    if (soc_mem_field32_get(unit, data_mem, buf, ECMPf)) {
        /* Mark entry as ecmp */
        data->defip_ecmp = 1;
        data->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (nh_ecmp_idx) {
            *nh_ecmp_idx =
                soc_mem_field32_get(unit, data_mem, buf, ECMP_PTRf); 
        }
    } else {
        /* Mark entry as non-ecmp. */
        data->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        data->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (nh_ecmp_idx) {
            *nh_ecmp_idx = soc_mem_field32_get(unit, data_mem, buf,
                                                NEXT_HOP_INDEXf); 
        }
    }
    /* Get entry priority. */
    data->defip_prio = soc_mem_field32_get(unit, data_mem, buf, PRIf);

    /* Get priority override bit. */
    if (soc_mem_field32_get(unit, data_mem, buf, RPEf)) {
        data->defip_flags |= BCM_L3_RPE;
    }

    /* Get destination discard flag. */
    if(soc_mem_field32_get(unit, data_mem, buf, DST_DISCARDf)) {
        data->defip_flags |= BCM_L3_DST_DISCARD;
    }

    /* Set classification group id. */
    data->defip_lookup_class = 
        soc_mem_field32_get(unit, data_mem, buf, CLASS_IDf);

    /* Vrf id. */

    /* Subnet address */
    if (v6) {
        data->defip_vrf = _TR_LPM_FIB6(unit)[index].vrf;
        sal_memcpy(data->defip_ip6_addr, 
                _TR_LPM_FIB6(unit)[index].addr, sizeof(bcm_ip6_t));
    } else {
        data->defip_vrf = _TR_LPM_FIB4(unit)[index].vrf;
        data->defip_ip_addr = _TR_LPM_FIB4(unit)[index].addr;
    }

    /* Sub net prefix length. */
    data->defip_sub_len = sub_net_length;

    /* Get hit bit. */    
    hit_bit_word = soc_mem_field32_get(unit, hit_bit_mem, 
                                       usage_buf, DST_HITf);
    if ((hit_bit_word >> (index % 32)) & 0x1) {
        data->defip_flags |= BCM_L3_HIT;
    }
    /* Clear hit bit if required. */
    if (clear_hit) {
        hit_bit_word &= ~(0x1 << (index % 32));
        soc_mem_field32_set(unit, hit_bit_mem, usage_buf, DST_HITf, hit_bit_word);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, hit_bit_mem, MEM_BLOCK_ALL, 
                           (index >> 5), usage_buf));
    }
    return (BCM_E_NONE);
} 

/*
 * Function:
 *      _tr_ext_lpm_read_route_data
 * Purpose:
 *      Get an entry from external route table.
 * Parameters:
 *      unit           - (IN)  SOC unit number.
 *      v6             - (IN)  IPv6 route indicator.
 *      index          - (IN)  Entry index to read.
 *      sub_net_length - (IN)  Subnet prefix length.
 *      data           - (OUT) Entry data. 
 *      nh_ecmp_idx    - (OUT) Next hop ecmp group index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_tr_ext_lpm_read_route_data(int unit, int v6, int index, int sub_net_length, 
                 _bcm_defip_cfg_t *data, int *nh_ecmp_idx)
{
    soc_mem_t data_mem;                        /* Route data memory.         */
    soc_mem_t hit_bit_mem;                     /* Route data memory.         */
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS];       /* Buffer for HW entry.       */
    uint32 usage_buf[2];                       /* Buffer for hit bit.        */

    /* Input parameters check. */
    if (NULL == data) {
        return (BCM_E_PARAM);
    }

    /* Get data/hit_bit memory . */
    data_mem = _TR_LPM_DATA_MEM(unit, v6);
    hit_bit_mem = _TR_LPM_HIT_BIT_MEM(unit, v6);

    /* Read buffer from HW. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, data_mem, MEM_BLOCK_ANY, index, buf));

    /* Read hit bit buffer from HW. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, hit_bit_mem, MEM_BLOCK_ANY, (index >> 5),
                      usage_buf));

    return  _tr_ext_lpm_parse_route_data(unit, v6, index, sub_net_length, 
                                         buf, usage_buf, data, nh_ecmp_idx);

}

/*
 * Function:
 *      _tr_ext_lpm_match
 * Purpose:
 *      Get an entry from external route table.
 * Parameters:
 *      unit        - (IN)  SOC unit number.
 *      key         - (IN)  Lookup key. 
 *      pfx         - (IN)  VRF weighted prefix length.
 *      result      - (OUT) Matching entry index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_tr_ext_lpm_match(int unit, _bcm_defip_cfg_t *key, int pfx, int *result)
               
{
    uint16     hash_val;   /* Entry lookup hash.                     */
    int              v6;   /* Entry is IPv6 flag.                    */
    int           index;   /* Same hash linked list iteration index. */


    /* Input parameters check. */
    if ((NULL == key) || (NULL == result)) {
        return (BCM_E_PARAM);
    }

    v6 = _TR_LPM_ROUTE_IS_V6(key);

    /* Check if any entry with this prefix length present in sw table. */
    if (0 == _TR_LPM_STATE_VENT(unit, v6, pfx)) {
        return (BCM_E_NOT_FOUND);
    }

    /* Compute route entry hash value. */
    BCM_IF_ERROR_RETURN(_tr_ext_lpm_hash_compute(key, &hash_val));
    hash_val %= _TR_LPM_SW_IMAGE(unit, v6)->hash_size;

    /* Get first route table entry matching the hash. */
    index = _TR_LPM_SW_IMAGE(unit, v6)->hash_table[hash_val];

    while(index != _TR_LPM_HASH_INDEX_NULL) {

        /* Skip indexes with not matching prefix length. */
        if ((index < _TR_LPM_STATE_START(unit, v6, pfx)) ||  
            (index > _TR_LPM_STATE_END(unit, v6, pfx))) {
            if (v6) {
                index = _TR_LPM_FIB6(unit)[index].next_entry;
            } else {
                index = _TR_LPM_FIB4(unit)[index].next_entry;
            }
            continue;
        }

        /* Compare entry itself. */
        if (!_tr_ext_lpm_key_compare(unit, index,  key)) {
            break;
        }

        /* Continue to the next entry if no match found. */
        if (v6) {
            index = _TR_LPM_FIB6(unit)[index].next_entry;
        } else {
            index = _TR_LPM_FIB4(unit)[index].next_entry;
        }
    }

    /* Check lookup result. */
    if (_TR_LPM_HASH_INDEX_NULL == index) {
        return (BCM_E_NOT_FOUND);
    }

    *result = index;
    return (BCM_E_NONE);
}


/* 
 * Function:
 *      _tr_ext_lpm_sw_entry_delete
 * Purpose:
 *      Remove a route entry from sw image. 
 * Parameters:
 *      unit  - (IN) BCM device number. 
 *      key   - (IN) Route entry.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_tr_ext_lpm_sw_entry_delete(int unit, _bcm_defip_cfg_t *key)
{
    int              v6;   /* Entry is IPv6 flag.                    */
    int           index;   /* Same hash linked list iteration index. */
    uint16     hash_val;   /* Entry lookup hash.                     */
    int      prev_index;   /* Same hash linked list iteration index. */
    

    /* Input parameters check */
    if (NULL == key) {
        return (BCM_E_PARAM);
    }

    v6 = _TR_LPM_ROUTE_IS_V6(key);

    /* Compute route entry hash value. */
    BCM_IF_ERROR_RETURN(_tr_ext_lpm_hash_compute(key, &hash_val));
    hash_val %= _TR_LPM_SW_IMAGE(unit, v6)->hash_size;

    /* Linked list deletion procedure. */
    index =  _TR_LPM_SW_IMAGE(unit, v6)->hash_table[hash_val];
    prev_index = _TR_LPM_HASH_INDEX_NULL;

    while(index != _TR_LPM_HASH_INDEX_NULL) {
        if (key->defip_index == index) {
            if (prev_index == _TR_LPM_HASH_INDEX_NULL) {
                /* Delete from head of the list. */
                if (v6) {
                    _TR_LPM_SW_IMAGE(unit, v6)->hash_table[hash_val] = \
                        _TR_LPM_FIB6(unit)[index].next_entry;
                } else {
                    _TR_LPM_SW_IMAGE(unit, v6)->hash_table[hash_val] = \
                        _TR_LPM_FIB4(unit)[index].next_entry;
                }
            } else {
                /* Delete from the middle/end of the list. */
                if (v6) {
                    _TR_LPM_FIB6(unit)[prev_index].next_entry =  \
                        _TR_LPM_FIB6(unit)[index].next_entry;
                } else {
                    _TR_LPM_FIB4(unit)[prev_index].next_entry =  \
                        _TR_LPM_FIB4(unit)[index].next_entry;
                }
            }
            break;
        }
        /* Check next entry in the linked list. */
        prev_index = index;
        if (v6) {
            index = _TR_LPM_FIB6(unit)[index].next_entry;
        } else {
            index = _TR_LPM_FIB4(unit)[index].next_entry;
        }
    }

    /* Check if index was found. */
    if (_TR_LPM_HASH_INDEX_NULL == index) {
        return(BCM_E_NOT_FOUND);
    }

    /* Reset original sw entry.  */
    BCM_IF_ERROR_RETURN (_tr_ext_lpm_sw_entry_reset(unit, v6, index)); 
    return (BCM_E_NONE);
}

/* 
 * Function:
 *      _tr_ext_lpm_sw_entry_insert
 * Purpose:
 *      Insert a route entry to sw image. 
 * Parameters:
 *      unit  - (IN) BCM device number. 
 *      key   - (IN) Route entry.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_tr_ext_lpm_sw_entry_insert(int unit, _bcm_defip_cfg_t *key)
{
    int              v6;   /* Entry is IPv6 flag.                    */
    int           index;   /* Same hash linked list iteration index. */
    uint16     hash_val;   /* Entry lookup hash.                     */
    

    /* Input parameters check */
    if (NULL == key) {
        return (BCM_E_PARAM);
    }

    v6 = _TR_LPM_ROUTE_IS_V6(key);
    index = key->defip_index;

    /* Compute route entry hash value. */
    BCM_IF_ERROR_RETURN(_tr_ext_lpm_hash_compute(key, &hash_val));
    hash_val %= _TR_LPM_SW_IMAGE(unit, v6)->hash_size;

    if (v6) {
        /* Point entry next to the head of linked list. */
        _TR_LPM_FIB6(unit)[index].next_entry = \
            _TR_LPM_SW_IMAGE(unit, v6)->hash_table[hash_val];
        /* Set prefix subnet ip. */
        sal_memcpy(_TR_LPM_FIB6(unit)[index].addr, 
                   key->defip_ip6_addr, sizeof(bcm_ip6_t));
        /* Set prefix vrf. */
        _TR_LPM_FIB6(unit)[index].vrf = key->defip_vrf;
    } else {
        /* Point entry next to the head of linked list. */
        _TR_LPM_FIB4(unit)[index].next_entry = \
            _TR_LPM_SW_IMAGE(unit, v6)->hash_table[hash_val];
        /* Set prefix subnet ip. */
        _TR_LPM_FIB4(unit)[index].addr = key->defip_ip_addr;
        /* Set prefix vrf. */
        _TR_LPM_FIB4(unit)[index].vrf = key->defip_vrf;
    }

    /* Point hash linked list head to the entry. */
    _TR_LPM_SW_IMAGE(unit, v6)->hash_table[hash_val] = index;

    return (BCM_E_NONE);
}


/* 
 * Function:
 *      _tr_lpm_ext_entry_shift
 * Purpose:
 *      Move an entry in external route table  
 * Parameters:
 *      unit      - (IN) BCM device number. 
 *      v6        - (IN) IPv6 indicator.  
 *      from_ent  - (IN) Source entry index. 
 *      to_ent    - (IN) Destination entry index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_tr_lpm_ext_entry_shift(int unit, int v6, int pfx, int from_ent, int to_ent)
{
    int    nh_ecmp_idx;               /* Next hop/ecmp group index. */
    _bcm_defip_cfg_t data;            /* Route data.                */

    sal_memset(&data, 0, sizeof(_bcm_defip_cfg_t));

    pfx %= _TR_LPM_PREFIX_COUNT(_TR_LPM_MEM(unit, v6));

    if (to_ent != from_ent) {
        BCM_IF_ERROR_RETURN 
            (_tr_ext_lpm_read_route_data(unit, v6, from_ent, pfx,
                                         &data, &nh_ecmp_idx));

        BCM_IF_ERROR_RETURN(_tr_ext_lpm_sw_entry_delete(unit, &data)); 
        data.defip_index = to_ent;

        BCM_IF_ERROR_RETURN(_tr_ext_lpm_write(unit, &data, nh_ecmp_idx));
        BCM_IF_ERROR_RETURN(_tr_ext_lpm_sw_entry_insert(unit, &data)); 
    }

    return (BCM_E_NONE);
}



/* 
 * Function:
 *      _tr_lpm_ext_shift_pfx_up
 * Purpose:
 *      Ripple prefixes 1 entry  up. 
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      v6        - (IN)  IPv6 indicator.  
 *      pfx       - (IN)  Prefix subnet length. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_tr_lpm_ext_shift_pfx_up(int unit, int v6, int pfx)
{
    int         from_ent;
    int         to_ent;

    to_ent = _TR_LPM_STATE_END(unit, v6, pfx) + 1;

    from_ent = _TR_LPM_STATE_START(unit, v6, pfx);
    if(from_ent != to_ent) {
        BCM_IF_ERROR_RETURN
            (_tr_lpm_ext_entry_shift(unit, v6, pfx, from_ent, to_ent));
    }
    _TR_LPM_STATE_START(unit, v6, pfx) += 1;
    _TR_LPM_STATE_END(unit, v6, pfx) += 1;
    return (BCM_E_NONE);
}


/* 
 * Function:
 *      _tr_lpm_ext_shift_pfx_down
 * Purpose:
 *      Ripple prefixes 1 entry  down. 
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      v6        - (IN)  IPv6 indicator.  
 *      pfx       - (IN)  Prefix subnet length. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_tr_lpm_ext_shift_pfx_down(int unit, int v6, int pfx)
{
    int         from_ent;
    int         to_ent;

    to_ent = _TR_LPM_STATE_START(unit, v6, pfx) - 1;

    /* Don't move empty prefix . */
    if (_TR_LPM_STATE_VENT(unit, v6, pfx) == 0) {
        _TR_LPM_STATE_START(unit, v6, pfx) = to_ent;
        _TR_LPM_STATE_END(unit, v6, pfx) = to_ent - 1;
        return (BCM_E_NONE);
    }

    to_ent   = _TR_LPM_STATE_START(unit, v6, pfx) - 1;
    from_ent = _TR_LPM_STATE_END(unit, v6, pfx);
    BCM_IF_ERROR_RETURN
        (_tr_lpm_ext_entry_shift(unit, v6, pfx, from_ent, to_ent));

    _TR_LPM_STATE_START(unit, v6, pfx) -= 1;
    _TR_LPM_STATE_END(unit, v6, pfx) -= 1;

    return (BCM_E_NONE);
}

/* 
 * Function:
 *      _tr_ext_lpm_free_slot_create
 * Purpose:
 *      Create a slot for the new entry rippling the entries if required.
 * Parameters:
 *      unit      - (IN)  BCM device number. 
 *      v6        - (IN)  IPv6 indicator.  
 *      pfx       - (IN)  Prefix subnet length. 
 *      free_slot - (OUT) Free slot index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_tr_ext_lpm_free_slot_create(int unit, int v6, int pfx, int *free_slot)
{
    int         prev_pfx;         /* Prefixes iteration index.     */
    int         next_pfx;         /* Prefixes iteration index.     */
    int         free_pfx;         /* Prefix that has free entries. */        
    int         curr_pfx;         /* Prefixes iteration index.     */
    soc_mem_t   mem;              /* Route table entry.            */

    mem = _TR_LPM_MEM(unit, v6);

    if (_TR_LPM_STATE_VENT(unit, v6, pfx) == 0) {
        /*
         * Find the  prefix position. Only prefix with valid
         * entries are in the list.
         * next -> high to low prefix. low to high index
         * prev -> low to high prefix. high to low index
         * Unused prefix length MAX_PFX_INDEX is the head of the
         * list and is node corresponding to this is always
         * present.
         */
        curr_pfx = _TR_LPM_PREFIX_MAX_INDEX(mem);
        while (_TR_LPM_STATE_NEXT(unit, v6, curr_pfx) > pfx) {
            curr_pfx = _TR_LPM_STATE_NEXT(unit, v6, curr_pfx);
        }
        /* Insert the new prefix */
        next_pfx = _TR_LPM_STATE_NEXT(unit, v6, curr_pfx);
        if (next_pfx != -1) {
            _TR_LPM_STATE_PREV(unit, v6, next_pfx) = pfx;
        }
        _TR_LPM_STATE_NEXT(unit, v6, pfx) = _TR_LPM_STATE_NEXT(unit, v6, curr_pfx);
        _TR_LPM_STATE_PREV(unit, v6, pfx) = curr_pfx;
        _TR_LPM_STATE_NEXT(unit, v6, curr_pfx) = pfx;

        _TR_LPM_STATE_FENT(unit, v6, pfx) =  (_TR_LPM_STATE_FENT(unit, v6, curr_pfx) + 1) / 2;
        _TR_LPM_STATE_FENT(unit, v6, curr_pfx) -= _TR_LPM_STATE_FENT(unit, v6, pfx);
        _TR_LPM_STATE_START(unit, v6, pfx) =  _TR_LPM_STATE_END(unit, v6, curr_pfx) +
                                       _TR_LPM_STATE_FENT(unit, v6, curr_pfx) + 1;
        _TR_LPM_STATE_END(unit, v6, pfx) = _TR_LPM_STATE_START(unit, v6, pfx) - 1;
        _TR_LPM_STATE_VENT(unit, v6, pfx) = 0;
    } 

    free_pfx = pfx;
    while(_TR_LPM_STATE_FENT(unit, v6, free_pfx) == 0) {
        free_pfx = _TR_LPM_STATE_NEXT(unit, v6, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on this side try the other side */
            free_pfx = pfx;
            break;
        }
    }

    while(_TR_LPM_STATE_FENT(unit, v6, free_pfx) == 0) {
        free_pfx = _TR_LPM_STATE_PREV(unit, v6, free_pfx);
        if (free_pfx == -1) {
            if (_TR_LPM_STATE_VENT(unit, v6, pfx) == 0) {
                /* We failed to allocate entries for a newly allocated prefix.*/
                prev_pfx = _TR_LPM_STATE_PREV(unit, v6, pfx);
                next_pfx = _TR_LPM_STATE_NEXT(unit, v6, pfx);
                if (-1 != prev_pfx) {
                    _TR_LPM_STATE_NEXT(unit, v6, prev_pfx) = next_pfx;
                }
                if (-1 != next_pfx) {
                    _TR_LPM_STATE_PREV(unit, v6, next_pfx) = prev_pfx;
                }
            }
            return(BCM_E_FULL);
        }
    }

    /*
     * Ripple entries to create free space
     */
    while (free_pfx > pfx) {
        next_pfx = _TR_LPM_STATE_NEXT(unit, v6, free_pfx); 
        BCM_IF_ERROR_RETURN(_tr_lpm_ext_shift_pfx_down(unit, v6, next_pfx));
        _TR_LPM_STATE_FENT(unit, v6, free_pfx) -= 1;
        _TR_LPM_STATE_FENT(unit, v6, next_pfx) += 1;
        free_pfx = next_pfx;
    }

    while (free_pfx < pfx) {
        BCM_IF_ERROR_RETURN(_tr_lpm_ext_shift_pfx_up(unit, v6, free_pfx));
        _TR_LPM_STATE_FENT(unit, v6, free_pfx) -= 1;
        prev_pfx = _TR_LPM_STATE_PREV(unit, v6, free_pfx); 
        _TR_LPM_STATE_FENT(unit, v6, prev_pfx) += 1;
        free_pfx = prev_pfx;
    }

    _TR_LPM_STATE_VENT(unit, v6, pfx) += 1;
    _TR_LPM_STATE_FENT(unit, v6, pfx) -= 1;
    _TR_LPM_STATE_END(unit, v6, pfx) += 1;

    *free_slot = _TR_LPM_STATE_END(unit, v6, pfx);
    return(BCM_E_NONE);
}

/* 
 * Function:
 *      _tr_ext_lpm_free_slot_delete
 * Purpose:
 *      Delete entry matching prefix, vrf in external route table.
 * Parameters:
 *      unit  -  (IN) BCM device number. 
 *      v6    -  (IN) IPv6 indicator.  
 *      pfx   -  (IN) Prefix subnet length. 
 *      slot  -  (IN) Deleted entry index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_tr_ext_lpm_free_slot_delete (int unit, int v6, int pfx, int slot)
{
    int              prev_pfx;       /* Prefixes iteration index.  */
    int              next_pfx;       /* Prefixes iteration index.  */ 
    int              from_ent;       /* Entry source index.        */
    int              to_ent;         /* Entry destination index.   */

    from_ent = _TR_LPM_STATE_END(unit, v6, pfx);
    to_ent = slot;

    _TR_LPM_STATE_VENT(unit, v6, pfx) -= 1;
    _TR_LPM_STATE_FENT(unit, v6, pfx) += 1;
    _TR_LPM_STATE_END(unit,  v6, pfx) -= 1;

    if (to_ent != from_ent) {
        BCM_IF_ERROR_RETURN 
            (_tr_lpm_ext_entry_shift(unit, v6, pfx, from_ent, to_ent));
    }

    BCM_IF_ERROR_RETURN(_tr_ext_lpm_reset(unit, v6, from_ent));

    if (_TR_LPM_STATE_VENT(unit, v6, pfx) == 0) {
        /* remove from the list */
        prev_pfx = _TR_LPM_STATE_PREV(unit, v6, pfx); /* Always present */
        next_pfx = _TR_LPM_STATE_NEXT(unit, v6, pfx);
        _TR_LPM_STATE_NEXT(unit, v6, prev_pfx) = next_pfx;
        _TR_LPM_STATE_FENT(unit, v6, prev_pfx) += _TR_LPM_STATE_FENT(unit, v6, pfx);
        _TR_LPM_STATE_FENT(unit, v6, pfx) = 0;
        if (next_pfx != -1) {
            _TR_LPM_STATE_PREV(unit, v6, next_pfx) = prev_pfx;
        }
        _TR_LPM_STATE_NEXT(unit, v6, pfx) = -1;
        _TR_LPM_STATE_PREV(unit, v6, pfx) = -1;
        _TR_LPM_STATE_START(unit, v6, pfx) = -1;
        _TR_LPM_STATE_END(unit, v6, pfx) = -1;
    }

    return(BCM_E_NONE);
}



/*
 * Function:
 *      _bcm_tr_ext_lpm_init
 * Purpose:
 *      Initialize external route table sw image. 
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      mem         - (IN)External route table memory name. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_ext_lpm_init(int unit, soc_mem_t mem)
{
    int num_prefixes;     /* Number of different prefixes.     */
    int pfx_state_size;   /* Prefix state tracking array size. */
    int defip_table_size; /* Route table size.                 */
    int index;            /* Iteration index.                  */
    int v6;               /* IP version.                       */ 
    
     /* Calculate sw image type & number of prefixes based on memory name. */
     v6 = (EXT_IPV4_DEFIPm == mem) ? _TR_LPM_IPV4 : _TR_LPM_IPV6; 
     num_prefixes =  _TR_LPM_PREFIX_MAX(mem);

     /* Allocate prefix tracking table. */
     pfx_state_size = sizeof(_tr_ext_lpm_state_t) * (num_prefixes);
     if (!_TR_LPM_INIT_CHECK(unit, v6)) {
         _TR_LPM_STATE(unit, v6) = (_tr_ext_lpm_state_t *)sal_alloc(pfx_state_size, "LPM prefix info");
         if (NULL == _TR_LPM_STATE(unit, v6)) {
             return (BCM_E_MEMORY);
         }
     }

    /* Initialize prefix tracking table. */
    sal_memset(_TR_LPM_STATE(unit, v6), 0, pfx_state_size);

    for(index = 0; index < num_prefixes; index++) {
        _TR_LPM_STATE_START(unit, v6, index) = -1;
        _TR_LPM_STATE_END(unit,   v6, index) = -1;
        _TR_LPM_STATE_PREV(unit,  v6, index) = -1;
        _TR_LPM_STATE_NEXT(unit,  v6, index) = -1;
        _TR_LPM_STATE_VENT(unit,  v6, index) = 0;
        _TR_LPM_STATE_FENT(unit,  v6, index) = 0;
    }

    /* Allocate sw image based on number of entries in the memory. */
    defip_table_size = soc_mem_index_count(unit, mem);
    _TR_LPM_STATE_FENT(unit, v6, num_prefixes - 1) = defip_table_size;

    /* If image was previously allocated free it. */
    if (_TR_LPM_SW_IMAGE(unit, v6) != NULL) {
        if (BCM_FAILURE(_tr_lpm_sw_image_destroy(unit, v6))) {
            return (BCM_E_INTERNAL);
        }
    }

    /* Allocate  sw image. */
    if (_tr_lpm_sw_image_create(unit, v6, mem)) { 
        return (BCM_E_MEMORY);
    }

    return(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr_ext_lpm_deinit
 * Purpose:
 *      De-initialize external route table sw image. 
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      mem         - (IN)External route table memory name. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_ext_lpm_deinit(int unit, soc_mem_t mem)
{
    int rv;       /* Operation return status. */
    int v6;       /* IP version.              */ 
    
    v6 = (EXT_IPV4_DEFIPm == mem) ? _TR_LPM_IPV4 : _TR_LPM_IPV6;

    rv = _tr_lpm_sw_image_destroy(unit, v6);

    if (_TR_LPM_STATE(unit, v6) != NULL) {
        sal_free(_TR_LPM_STATE(unit, v6));
        _TR_LPM_STATE(unit, v6) = NULL;
    }
    return(rv);
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *      _bcm_tr_ext_lpm_reinit_done
 * Purpose:
 *      Re-initialize external route table sw image 
 *      prefix ranges completion call. 
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      mem         - (IN)External route table memory name. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_ext_lpm_reinit_done(int unit, soc_mem_t mem)
{
    int idx;              /* Prefix length iteration index.  */
    int num_prefixes;     /* Number of different prefixes.   */
    int prev_idx;         /* Prefix length iteration index.  */
    int defip_table_size; /* Entry count in route table.     */
    int v6;               /* IPv6 route indicator.           */

    num_prefixes = _TR_LPM_PREFIX_MAX_INDEX(mem);
    prev_idx = num_prefixes;
    defip_table_size = soc_mem_index_count(unit, mem);
    v6 = (EXT_IPV4_DEFIPm != mem);

    _TR_LPM_STATE_PREV(unit, v6, num_prefixes) = -1;

    for (idx = num_prefixes; idx > 0 ; idx--) {
        if (-1 == _TR_LPM_STATE_START(unit, v6, idx)) {
            continue;
        }

        _TR_LPM_STATE_PREV(unit, v6, idx) = prev_idx;
        _TR_LPM_STATE_NEXT(unit, v6, prev_idx) = idx;

        _TR_LPM_STATE_FENT(unit, v6, prev_idx) =                    \
                          _TR_LPM_STATE_START(unit, v6, idx) -      \
                          _TR_LPM_STATE_END(unit, v6, prev_idx) - 1;
        prev_idx = idx;
        
    }

    _TR_LPM_STATE_NEXT(unit, v6, prev_idx) = -1;
    _TR_LPM_STATE_FENT(unit, v6, prev_idx) =                   \
                          defip_table_size -                   \
                          _TR_LPM_STATE_END(unit, v6, prev_idx) - 1;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr_ext_lpm_reinit
 * Purpose:
 *      Re-initialize external route table sw image. 
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      mem         - (IN)External route table memory name. 
 *      idx         - (IN)Entry index.
 *      lpm_cfg     - (IN)Entry index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_ext_lpm_reinit(int unit, soc_mem_t mem, 
                       int idx, _bcm_defip_cfg_t *lpm_cfg)
{
    int pfx_len;
    int v6;

    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    v6 = (lpm_cfg->defip_flags & BCM_L3_IP6) ? 1 : 0;

    BCM_IF_ERROR_RETURN
        (_tr_ext_lpm_prefix_length_get(unit, lpm_cfg, &pfx_len));

    if (_TR_LPM_STATE_VENT(unit, v6, pfx_len) == 0) {
        _TR_LPM_STATE_START(unit, v6, pfx_len) = idx;
        _TR_LPM_STATE_END(unit, v6, pfx_len) = idx;
    } else {
        _TR_LPM_STATE_END(unit, v6, pfx_len) = idx;
    }

    _TR_LPM_STATE_VENT(unit, v6, pfx_len)++;

    return (BCM_E_NONE);
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* 
 * Function:
 *      _bcm_tr_ext_lpm_add
 * Purpose:
 *      Insert prefix, vrf in external route table.
 * Parameters:
 *      unit         - (IN) BCM device number. 
 *      data         - (IN) Inserted entry.   
 *      nh_ecmp_idx  - (IN) Next hop ecmp index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_ext_lpm_add(int unit, _bcm_defip_cfg_t *data, int nh_ecmp_idx)
{
    int        rv;       /* Operation return status. */
    int        v6;       /* IP protocol version.       */
    int        pfx;      /* VRF weighted prefix length.*/

    /* Input parameters sanitcy check */
    if (NULL == data) {
        return (BCM_E_PARAM);
    }

    v6 = _TR_LPM_ROUTE_IS_V6(data);

    /* Calculate vrf weighted prefix lengh. */
    _tr_ext_lpm_prefix_length_get(unit, data, &pfx);

    rv = _tr_ext_lpm_match(unit, data, pfx, &data->defip_index);
    if (BCM_SUCCESS(rv)) {
        /* Entry already present. Update the entry */
        rv = _tr_ext_lpm_write(unit, data, nh_ecmp_idx);
    } else if (rv == BCM_E_NOT_FOUND) {


        /* Allocate free slot for entry.  */
        rv = _tr_ext_lpm_free_slot_create(unit, v6, pfx,
                                          &data->defip_index);
        if (BCM_SUCCESS(rv)) {
            rv = _tr_ext_lpm_write(unit, data, nh_ecmp_idx);
            if (BCM_SUCCESS(rv)) {
                rv = _tr_ext_lpm_sw_entry_insert(unit, data); 
            }
            if (BCM_SUCCESS(rv)) {
                BCM_XGS3_L3_DEFIP_CNT_INC(unit, v6);
            }
        }
    }
    return(rv);
}

/* 
 * Function:
 *      _bcm_tr_ext_lpm_delete
 * Purpose:
 *      Delete entry matching prefix, vrf in external route table.
 * Parameters:
 *      unit  -     (IN) BCM device number. 
 *      key   -     (IN) Lookup key.   
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_ext_lpm_delete(int unit, _bcm_defip_cfg_t *key)
{
    int        rv;       /* Operation return status. */
    int        v6;       /* IP protocol version.     */
    int       pfx;       /* VRF weighted prefix length.*/

    /* Input parameters sanitcy check */
    if (NULL == key) {
        return (BCM_E_PARAM);
    }

    v6 = _TR_LPM_ROUTE_IS_V6(key);
    /* Calculate vrf weighted prefix lengh. */
    _tr_ext_lpm_prefix_length_get(unit, key, &pfx);

    rv = _tr_ext_lpm_match(unit, key, pfx, &key->defip_index);

    if (BCM_SUCCESS(rv)) {
        _tr_ext_lpm_sw_entry_delete(unit, key);

        rv = _tr_ext_lpm_free_slot_delete(unit, v6, pfx, key->defip_index);

        if (BCM_SUCCESS(rv)) {
            BCM_XGS3_L3_DEFIP_CNT_DEC(unit, v6);
        }
    }
    return(rv);
}

/* 
 * Function:
 *      _bcm_tr_ext_lpm_match
 * Purpose:
 *      Find entry matching prefix, vrf in external route table.
 * Parameters:
 *      unit  -     (IN) BCM device number. 
 *      key   -     (IN/OUT) (IN)Lookup key (OUT) Entry data if found.   
 *      index -     (OUT) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_ext_lpm_match(int unit, _bcm_defip_cfg_t *key, int *next_hop_index) 
{
    int        rv;       /* Operation return status.   */
    int        v6;       /* IP protocol version.       */
    int     index;       /* Entry index if found.      */
    int       pfx;       /* VRF weighted prefix length.*/


    /* Input parameters sanitcy check */
    if (NULL == key) {
        return (BCM_E_PARAM);
    }

    v6 = _TR_LPM_ROUTE_IS_V6(key);
    /* Calculate vrf weighted prefix lengh. */
    _tr_ext_lpm_prefix_length_get(unit, key, &pfx);

    rv = _tr_ext_lpm_match(unit, key, pfx, &index);
    if (BCM_SUCCESS(rv)) {
        rv = _tr_ext_lpm_read_route_data(unit, v6, index, key->defip_sub_len,
                                         key, next_hop_index);
    }
    return(rv);
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* 
 * Function:
 *      _tr_ext_lpm_ip6_mask_len
 * Purpose:
 *      Return ipv6 mask length given ipv6 mask.
 * Parameters:
 *      bcm_ip6_t: ip6mask
 * Returns:
 *      int: ipv6 mask length
 */
STATIC int
_tr_ext_lpm_ip6_mask_len(bcm_ip6_t ip6mask)
{
    int idx, tempcount, masklen = 0;
    uint32 maskbits;
    
    for (idx = 0; idx < 16; idx++) {
        maskbits = ip6mask[idx];
        for( tempcount = 0; maskbits; tempcount++) {
            maskbits &= (maskbits - 1);     
        }
        masklen += tempcount;
    }
    return masklen;
}

/* 
 * Function:
 *      _bcm_tr_ext_lpm_state_recover
 * Purpose:
 *      Reconstruct LPM s/w state from ESM. 
 *      Reconstruct s/w FIB4 and FIB6 state from ESM. 
 * Parameters:
 *      unit  - (IN) BCM device number. 
 *      v6    - Flag to indicate entry is ipv4/ipv6.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_tr_ext_lpm_state_recover(int unit, int v6)
{
    int       rv, idx;
    int       nh_ecmp_idx, vrf_fld_len;  
    uint32    usage_buf[2];    
    uint32    v4_mask, vrf_fld_mask,  prfx = 0;
    soc_mem_t mem, data_mem, hit_bit_mem;
    uint32    buf[SOC_MAX_MEM_FIELD_WORDS];   
    uint32    data_buf[SOC_MAX_MEM_FIELD_WORDS];   
    bcm_ip6_t v6_64_mask, v6_128_mask;
    ip6_addr_t v6_addr;
    bcm_vrf_t vrf, vrf_hi, vrf_mask, vrf_mask_hi;
    _bcm_defip_cfg_t *lpm_cfg;

    lpm_cfg = sal_alloc(sizeof(_bcm_defip_cfg_t), "TR route table");
    if (NULL == lpm_cfg) {
        return (BCM_E_MEMORY);
    }
    sal_memset(lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

    /* Get table memory. */
    mem = _TR_LPM_MEM(unit, v6);
    data_mem = _TR_LPM_DATA_MEM(unit, v6);
    hit_bit_mem = _TR_LPM_HIT_BIT_MEM(unit, v6);

    for (idx = 0; idx < soc_mem_index_count(unit, mem); idx++) {

        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, buf);
        if (!BCM_SUCCESS(rv)) {
            sal_free(lpm_cfg);
            return rv;
        }
        
        if (soc_mem_field32_get(unit, mem, buf, VALIDf) == 0) {
            continue;
        }
        
        if (v6) { /*Ipv6 entries */

            if (SOC_MEM_FIELD_VALID(unit, mem, IP_ADDRf)) {
                /* EXT_IPV6_64_DEFIPm */
                soc_mem_ip6_addr_get(unit, mem, buf, IP_ADDRf, v6_addr,
                                      SOC_MEM_IP6_UPPER_ONLY); 
                sal_memcpy(_TR_LPM_FIB6(unit)[idx].addr, v6_addr,
                                         sizeof(ip6_addr_t));
                bcm_ip6_mask_create(v6_64_mask, 0);
                soc_mem_ip6_addr_mask_get(unit, mem, buf, MASK_IP_ADDRf,
                                          v6_64_mask, SOC_MEM_IP6_UPPER_ONLY);
                prfx = _tr_ext_lpm_ip6_mask_len(v6_64_mask);
            } else { /* EXT_IPV6_128_DEFIPm */
                soc_mem_ip6_addr_get(unit, mem, buf, IP_ADDR_HIf, v6_addr,
                                      SOC_MEM_IP6_UPPER_ONLY); 
                soc_mem_ip6_addr_get(unit, mem, buf, IP_ADDR_LOf, v6_addr,
                                      SOC_MEM_IP6_LOWER_ONLY); 
                sal_memcpy(_TR_LPM_FIB6(unit)[idx].addr, v6_addr,
                                         sizeof(ip6_addr_t));
    
                bcm_ip6_mask_create(v6_128_mask, 0);
                soc_mem_ip6_addr_mask_get(unit, mem, buf, MASK_IP_ADDR_HIf,
                                          v6_128_mask, SOC_MEM_IP6_UPPER_ONLY);
                soc_mem_ip6_addr_mask_get(unit, mem, buf, MASK_IP_ADDR_LOf,
                                          v6_128_mask, SOC_MEM_IP6_LOWER_ONLY);
                prfx = _tr_ext_lpm_ip6_mask_len(v6_128_mask);
            }
        } else { /* Ipv4 entries - EXT_IPV4_DEFIPm */
            
            _TR_LPM_FIB4(unit)[idx].addr = 
                soc_mem_field32_get(unit, mem, buf, IP_ADDRf);
            v4_mask = soc_mem_field32_get(unit, mem, buf, MASK_IP_ADDRf);
            for( prfx = 0; v4_mask; prfx++) {
                v4_mask &= (v4_mask - 1);     
            }
            prfx = _TR_LPM_PREFIX_COUNT(mem) - prfx - 1;
        }

        /* Common to IPv4 and IPv6 */
        vrf_fld_len = soc_mem_field_length(unit, mem, VRF_LOf);
        vrf_fld_mask = (1 << vrf_fld_len) - 1;        
        vrf = vrf_fld_mask & soc_mem_field32_get(unit, mem, buf, VRF_LOf);
        if (SOC_MEM_FIELD_VALID(unit, mem, VRF_HIf)) {
            vrf_hi = soc_mem_field32_get(unit, mem, buf, VRF_HIf);
            vrf |= vrf_hi << vrf_fld_len;
        }

        vrf_mask = vrf_fld_mask & soc_mem_mask_field32_get(unit, mem, buf,
                MASK_VRF_LOf);
        if (SOC_MEM_FIELD_VALID(unit, mem, VRF_HIf)) {
            vrf_mask_hi = soc_mem_field32_get(unit, mem, buf, 
                    MASK_VRF_HIf);
            vrf_mask |= vrf_mask_hi << vrf_fld_len;
        }


        if (v6) { 
            _TR_LPM_FIB6(unit)[idx].vrf = bcm_ext_lpm_vrf_get( unit, vrf,
                    vrf_mask, soc_mem_field32_get(unit, mem, buf, 
                        GLOBAL_ROUTEf));
        } else {
            _TR_LPM_FIB4(unit)[idx].vrf = bcm_ext_lpm_vrf_get( unit, vrf,
                    vrf_mask, soc_mem_field32_get(unit, mem, buf, 
                        GLOBAL_ROUTEf));
        }

        rv = soc_mem_read(unit, data_mem, MEM_BLOCK_ANY, idx, data_buf);
        if (!BCM_SUCCESS(rv)) {
            sal_free(lpm_cfg);
            return rv;
        }
        
        rv = soc_mem_read(unit, hit_bit_mem, MEM_BLOCK_ANY,
                                         (idx >> 5), usage_buf);
        if (!BCM_SUCCESS(rv)) {
            sal_free(lpm_cfg);
            return rv;
        }

        _tr_ext_lpm_parse_route_data(unit, v6, idx, prfx, data_buf,
                                       usage_buf, lpm_cfg, &nh_ecmp_idx);
        _tr_ext_lpm_sw_entry_insert(unit, lpm_cfg);
        _bcm_tr_ext_lpm_reinit(unit, mem, idx, lpm_cfg);
    }

    _bcm_tr_ext_lpm_reinit_done(unit, mem);

    sal_free(lpm_cfg);
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_tr_ext_defip_traverse
 * Purpose:
 *      Traverse all LPM entries, call back the test
 *      function and save entries which pass.  After a configuration number
 *      of entries are saved, call back the operation function on each.
 *      Repeat from beginning of table after operations are complete.
 *      Continue until the no entries test TRUE.
 * Parameters:
 *      unit      - BCM device number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.
 * Notes:
 *      The test function must not perform any changes to the
 *      table.  Also, it must know how to exclude entries which have
 *      already been processed by the operation function.  Otherwise, the
 *      loop may never terminate.
 *      The operation function may update the table.
 *      We require to pass table index so caller can acccess entry hit 
 *      information for traverse & aging functionalities.
 *      Table index is not available during op callback and should be ignored.
 */
int
_bcm_tr_defip_traverse(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    _bcm_defip_cfg_t *search_data_array;
    int              *nh_array;
    uint32           *lpm_data;
    uint32           *usage_data;
    int              dma_start;
    int              alloc_sz;
    char             *dma_ptr;
    char             *usage_dma_ptr;
    int              dma_end;
    int              idx;
    soc_mem_t        mem;
    int              curr_pfx;
    int              v6;
    int              array_index;
    int              rv=SOC_E_NONE;
    int              cmp_result = BCM_L3_CMP_NOT_EQUAL; 
    int              nxt_pfx = -1;

    v6 = (trv_data->flags & BCM_L3_IP6) ? 1 : 0 ;

    alloc_sz = _TR_LPM_BLOCK_SZ * sizeof(_bcm_defip_cfg_t);
    search_data_array = sal_alloc(alloc_sz, "TR route table");
    if (NULL == search_data_array) {
        return (BCM_E_MEMORY);
    }
    sal_memset(search_data_array, 0, alloc_sz);

    alloc_sz = _TR_LPM_BLOCK_SZ * sizeof(int);
    nh_array = sal_alloc(alloc_sz, "TR route table");
    if (NULL == nh_array) {
        sal_free(search_data_array);
        return (BCM_E_MEMORY);
    }
    sal_memset(nh_array, 0, alloc_sz);

    alloc_sz = _TR_LPM_BLOCK_SZ * sizeof(ext_defip_data_entry_t);
    dma_ptr = soc_cm_salloc(unit, alloc_sz, "TR route table dma");
    if (NULL == dma_ptr) {
        sal_free(nh_array);
        sal_free(search_data_array);
        return (BCM_E_MEMORY);
    }

    alloc_sz = _TR_LPM_BLOCK_SZ * sizeof(ext_dst_hit_bits_entry_t);
    usage_dma_ptr = soc_cm_salloc(unit, alloc_sz, "TR route table hit bits");
    if (NULL == usage_dma_ptr) {
        sal_free(nh_array);
        sal_free(search_data_array);
        soc_cm_sfree(unit, dma_ptr);
        return (BCM_E_MEMORY);
    }

    /* Get table memory. */
    mem = _TR_LPM_MEM(unit, v6);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Reconstruct EXT LPM s/w state */
    if (SOC_WARM_BOOT(unit)) {
        _bcm_tr_ext_lpm_state_recover(unit, v6);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    while (1) {
        array_index = 0;
        curr_pfx = _TR_LPM_PREFIX_MAX_INDEX(mem);

        /* Iterate over prefixes to fill search data. */
        while (curr_pfx != -1) {

            /* Skip empty prefixes. */
            if (0 == _TR_LPM_STATE_VENT(unit, v6, curr_pfx)) {
                curr_pfx = _TR_LPM_STATE_NEXT(unit, v6, curr_pfx);
                continue;
            }

            nxt_pfx = -1;
            /* Read entries for current prefix. */
            for(dma_start = _TR_LPM_STATE_START(unit, v6, curr_pfx);
                dma_start <= _TR_LPM_STATE_END(unit, v6, curr_pfx);
                dma_start += _TR_LPM_BLOCK_SZ) {

                dma_end = dma_start + _TR_LPM_BLOCK_SZ - 1;
                if (dma_end > _TR_LPM_STATE_END(unit, v6, curr_pfx)) {
                    dma_end = _TR_LPM_STATE_END(unit, v6, curr_pfx);
                }
                rv = soc_mem_read_range(unit, _TR_LPM_DATA_MEM(unit, v6),
                                        MEM_BLOCK_ANY, dma_start, dma_end,
                                        dma_ptr);
                if (rv < 0) {  
                    soc_cm_sfree(unit, dma_ptr);
                    soc_cm_sfree(unit, usage_dma_ptr);
                    sal_free(search_data_array);
                    sal_free(nh_array);
                    return (rv);
                }

                rv = soc_mem_read_range(unit, _TR_LPM_HIT_BIT_MEM(unit, v6),
                                        MEM_BLOCK_ANY, (dma_start >> 5),
                                        (dma_end >> 5), usage_dma_ptr);
                if (BCM_FAILURE(rv)) {  
                    soc_cm_sfree(unit, dma_ptr);
                    soc_cm_sfree(unit, usage_dma_ptr);
                    sal_free(search_data_array);
                    sal_free(nh_array);
                    return (rv);
                }

                for(idx = (dma_end - dma_start); idx >= 0; idx--) {
                    /* Calculate entry offset. */
                    lpm_data = soc_mem_table_idx_to_pointer
                        (unit, _TR_LPM_DATA_MEM(unit, v6),
                         uint32 *, dma_ptr, idx);

                    /* Calculate hit bit entry offset. */
                    usage_data = soc_mem_table_idx_to_pointer
                        (unit, _TR_LPM_HIT_BIT_MEM(unit, v6),
                         uint32 *, usage_dma_ptr, (idx >> 5));

                    /* Parse route entry. */
                    _tr_ext_lpm_parse_route_data(unit, v6, 
                                                 dma_start +  idx,
                                                 (curr_pfx % _TR_LPM_PREFIX_COUNT(mem)),
                                                 lpm_data, usage_data,
                                                 (search_data_array + array_index), 
                                                 (nh_array + array_index));

                    /* Execute test routine if any. */
                    if (NULL != trv_data->cmp_cb) {
                        if (nxt_pfx == -1) {
                            nxt_pfx = _TR_LPM_STATE_NEXT(unit, v6, curr_pfx);
                        }
                        rv = (*trv_data->cmp_cb) (unit, (void *)trv_data,
                                                  (void *)(search_data_array + array_index),
                                                  (void *)(nh_array + array_index),
                                                  &cmp_result);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, dma_ptr);
                            soc_cm_sfree(unit, usage_dma_ptr);
                            sal_free(nh_array);
                            sal_free(search_data_array);
                            return rv;
                        }
                    } 

                    if ((BCM_L3_CMP_EQUAL == cmp_result) ||
                        (NULL == trv_data->cmp_cb)) {
                        array_index++;
                        if (array_index >= _TR_LPM_BLOCK_SZ) {
                            break;
                        }
                    }
                }
                if (array_index >= _TR_LPM_BLOCK_SZ) {
                    break;
                }
            }
            if (array_index >= _TR_LPM_BLOCK_SZ) {
                break;
            }
            if (nxt_pfx != -1) {
                curr_pfx = nxt_pfx;
            } else {
                curr_pfx = _TR_LPM_STATE_NEXT(unit, v6, curr_pfx);
            }
        }

        for (idx = 0; idx < array_index; idx++) {
            /* Execute operation routine if any. */
            if (NULL != trv_data->op_cb) {
                rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                         (void *)(search_data_array + idx),
                                         (void *)(nh_array + idx), &cmp_result);
                if (rv < 0) {
                    soc_cm_sfree(unit, dma_ptr);
                    soc_cm_sfree(unit, usage_dma_ptr);
                    sal_free(nh_array);
                    sal_free(search_data_array);
                    return (rv);
                }
            }
        }

        if (0 == array_index) {
            break;
        }
    }

    soc_cm_sfree(unit, dma_ptr);
    soc_cm_sfree(unit, usage_dma_ptr);
    sal_free(search_data_array);
    sal_free(nh_array);
    return (rv);
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     tr_ext_lpm_sw_dump
 * Purpose:
 *     Displays ESM LPM information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_tr_ext_lpm_sw_dump(int unit)
{
    soc_mem_t           mem;
    int                 i;
    _tr_lpm_sw_image_p v4, v6;
  
    v4 = _TR_LPM_SW_IMAGE(unit, _TR_LPM_IPV4);
    v6 = _TR_LPM_SW_IMAGE(unit, _TR_LPM_IPV6);

    LOG_CLI((BSL_META_U(unit,
                        "\n TRIUMPH External LPM State -\n")));
    if ((NULL == v4) && (NULL == v6)) {
        LOG_CLI((BSL_META_U(unit,
                            " ESM not present!\n")));
        return;
    }

    if (NULL != v4) {
        LOG_CLI((BSL_META_U(unit,
                            "  IPv4 Prefix entries:\n")));
        mem = _TR_LPM_MEM(unit, _TR_LPM_IPV4);

        for (i = 0; i <  _TR_LPM_PREFIX_MAX(mem); i++) {
            if (_TR_LPM_STATE_VENT(unit, _TR_LPM_IPV4, i) != 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "   Prefix length (bits): %d\n"),
                         (i%_TR_LPM_PREFIX_COUNT(mem))));
                LOG_CLI((BSL_META_U(unit,
                                    "    Start : %d\n"),  
                         _TR_LPM_STATE_START(unit, _TR_LPM_IPV4, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    End   : %d\n"),  
                         _TR_LPM_STATE_END(unit, _TR_LPM_IPV4, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    Prev  : %d\n"), 
                         _TR_LPM_STATE_PREV(unit, _TR_LPM_IPV4, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    Next  : %d\n"), 
                         _TR_LPM_STATE_NEXT(unit, _TR_LPM_IPV4, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    Valid Entries : %d\n"), 
                         _TR_LPM_STATE_VENT(unit, _TR_LPM_IPV4, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    Free  Entries : %d\n"), 
                         _TR_LPM_STATE_FENT(unit, _TR_LPM_IPV4, i)));
            }
        }
    }

    if (NULL != v6) {
        LOG_CLI((BSL_META_U(unit,
                            "\n  IPv6 Prefix entries: \n" )));
        mem = _TR_LPM_MEM(unit, _TR_LPM_IPV6);
               
        for (i = 0; i <  _TR_LPM_PREFIX_MAX(mem); i++) {
            if (_TR_LPM_STATE_VENT(unit, _TR_LPM_IPV6, i) != 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "   Prefix length (bits): %d\n"),
                         (i%_TR_LPM_PREFIX_COUNT(mem))));
                LOG_CLI((BSL_META_U(unit,
                                    "    Start : %d\n"),  
                         _TR_LPM_STATE_START(unit, _TR_LPM_IPV6, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    End   : %d\n"),  
                         _TR_LPM_STATE_END(unit, _TR_LPM_IPV6, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    Prev  : %d\n"), 
                         _TR_LPM_STATE_PREV(unit, _TR_LPM_IPV6, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    Next  : %d\n"), 
                         _TR_LPM_STATE_NEXT(unit, _TR_LPM_IPV6, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    Valid Entries : %d\n"), 
                         _TR_LPM_STATE_VENT(unit, _TR_LPM_IPV6, i)));
                LOG_CLI((BSL_META_U(unit,
                                    "    Free  Entries : %d\n"), 
                         _TR_LPM_STATE_FENT(unit, _TR_LPM_IPV6, i)));
            }
        }
    }
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */



#else /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
typedef int bcm_esw_triumph_external_lpm_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */

