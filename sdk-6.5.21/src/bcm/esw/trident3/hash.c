/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hash.c
 * Purpose:     Implement Trident3 specific hash flex bin selection APIs.
 *   Trident3 enhanced the RTAG7 hash module with the flexible bin selection
 * logic. It can overwrite the existing fixed bin selection logic. The flexible
 * logic uses a 128 entry deep TCAM. Each entry consists of the key and
 * policy. The key uses number of flex objects defined in the dynamically loaded
 * Cancun CEH. User will need to use the bcm_hash_flex_field_t parameter to
 * pass into the API.
 */

#include <soc/drv.h>
#include <soc/scache.h>

#include <shared/bsl.h>
#include <assert.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <soc/esw/cancun.h>
#include <bcm/hash.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/hash.h>
#include <bcm_int/esw_dispatch.h>

#if defined(BCM_TRIDENT3_SUPPORT)

/* fixed packet type's corresponding fields and value */
typedef struct {
    int         pkt_type;
    soc_format_t fmt;
    soc_field_t fld1;
    int         val1;
    soc_field_t fld2;
    int         val2;
} _bcm_hash_pfld_info_t;

/* fixed packet status corresponding fields and value */
typedef struct {
    int         pkt_status;
    soc_field_t fld_mask;
    soc_field_t fld;
    int         bit_pos;
    int         fld_size;
} _bcm_hash_sfld_info_t;

/* TD3 flex objects */
typedef struct {
    uint64 match_id;
    uint16 flow_id0;
    uint16 flow_id1;
    uint16 flow_id2;
    uint8  hve_ctrl1;
    uint8  hve_ctrl2;
} _bcm_hash_td3_flex_obj_t;

/* Flex field ID: object and field name id */
#define FIELD_OBJ_ID(_id) ((_id) >> 24)
#define FIELD_NAME_ID(_id) ((_id) & 0xFFFFFF)
#define FIELD_OBJ_ID_SET(_id, _obj) ((_id) |  ((_obj) << 24))

/* TD3 profile mem and field */
#define _BCM_HASH_TD3_NUM_BINS_PER_SET   13
#define _BCM_HASH_TD3_NUM_PROF_MEM       4

#define PROF_BASE(_u, _num) \
	(soc_mem_index_count((_u), FLEX_BIN_HASH_PROFILE_0m) * \
	_BCM_HASH_TD3_NUM_BINS_PER_SET * (_num))
#define BIN_INX(_u, _mn, _ent, _bn) (PROF_BASE((_u),(_mn)) + \
	(_ent) * _BCM_HASH_TD3_NUM_BINS_PER_SET + \
	(_bn) % _BCM_HASH_TD3_NUM_BINS_PER_SET)

#define BIN_ID_SIZE8_VALID(_id)  ((((_id) >= BCM_HASH_BIN_A_0) && \
	 ((_id) <= BCM_HASH_BIN_A_MAX)) || \
	(((_id) >= BCM_HASH_BIN_A_0_UPPER_HALF) && \
	 ((_id) <= BCM_HASH_BIN_A_UPPER_HALF_MAX)) || \
	(((_id) >= BCM_HASH_BIN_B_0) && \
	 ((_id) <= BCM_HASH_BIN_B_MAX)) || \
	(((_id) >= BCM_HASH_BIN_B_0_UPPER_HALF) && \
	 ((_id) <= BCM_HASH_BIN_B_UPPER_HALF_MAX)))

#define BIN_ID_SIZE16_VALID(_id)  ((((_id) >= BCM_HASH_BIN_A_0) && \
	 ((_id) <= BCM_HASH_BIN_A_MAX)) || \
	(((_id) >= BCM_HASH_BIN_B_0) && \
	 ((_id) <= BCM_HASH_BIN_B_MAX)))

#define BIN_ID_VALID(_cfg, _id)  ( \
	((_cfg)->option & BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT) ? \
	BIN_ID_SIZE8_VALID((_id)) : \
	BIN_ID_SIZE16_VALID((_id)))

typedef struct {
    soc_mem_t   mem[_BCM_HASH_TD3_NUM_PROF_MEM];
    soc_field_t fld[_BCM_HASH_TD3_NUM_BINS_PER_SET];
} _bcm_hash_prof_info_t;


STATIC _bcm_hash_prof_info_t _bcm_hash_td3_prof_info = {
    {FLEX_BIN_HASH_PROFILE_0m, FLEX_BIN_HASH_PROFILE_1m,
     FLEX_BIN_HASH_PROFILE_2m, FLEX_BIN_HASH_PROFILE_3m},
    {HASH_BIN_0f, HASH_BIN_1f, HASH_BIN_2f, HASH_BIN_3f,
     HASH_BIN_4f, HASH_BIN_5f, HASH_BIN_6f, HASH_BIN_7f,
     HASH_BIN_8f, HASH_BIN_9f, HASH_BIN_10f, HASH_BIN_11f,
     HASH_BIN_12f}
};

STATIC _bcm_hash_sfld_info_t _bcm_hash_td3_sfld_info[] = {
    {BCM_HASH_PKT_STATUS_GTP_ENDPOINT_ID, 
     GTP_ENDPOINT_ID_VALID_MASKf, GTP_ENDPOINT_ID_VALIDf, 0, 1},

    {BCM_HASH_PKT_STATUS_L3_FORWARDED, 
     L3_FORWARDED_MASKf, L3_FORWARDEDf, 0, 1},

    {BCM_HASH_PKT_STATUS_TUNNEL_TERM, 
     TUNNEL_TERM_STATUS_MASKf, TUNNEL_TERM_STATUSf, 0, 1},

    /* end marker */
    {BCM_HASH_PKT_STATUS_MAX, INVALIDm, INVALIDm, 0, 0},
};

STATIC _bcm_hash_pfld_info_t _bcm_hash_td3_pfld_info[] = {
    {BCM_HASH_PKT_FLOW_TYPE_FRONT_PANEL, PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 1, MICRO_FLOW_TYPE_FIXED_FRONT_PANELf, 0},

    {BCM_HASH_PKT_FLOW_TYPE_FRONT_PANEL_SYSTEM_HDR_PROXY, 
     PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 1, MICRO_FLOW_TYPE_FIXED_FRONT_PANELf, 1},
   
    {BCM_HASH_PKT_FLOW_TYPE_FRONT_PANEL_LOOPBACK_GENERIC, 
     PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 1, MICRO_FLOW_TYPE_FIXED_FRONT_PANELf, 6},
   
    {BCM_HASH_PKT_FLOW_TYPE_FRONT_PANEL_LOOPBACK_TUNNEL_TERM, 
     PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 1, MICRO_FLOW_TYPE_FIXED_FRONT_PANELf, 7},
   
    {BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR, 
     PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 2, MICRO_FLOW_TYPE_FIXED_SYSTEM_HDRf, 0},
   
    {BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_CONTROL, 
     PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 2, MICRO_FLOW_TYPE_FIXED_SYSTEM_HDRf, 0},
   
    {BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_UNICAST, 
     PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 2, MICRO_FLOW_TYPE_FIXED_SYSTEM_HDRf, 1},
   
    {BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_FLOOD, 
     PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 2, MICRO_FLOW_TYPE_FIXED_SYSTEM_HDRf, 2},
   
    {BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_L2_MULTICAST, 
     PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 2, MICRO_FLOW_TYPE_FIXED_SYSTEM_HDRf, 3},
   
    {BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_L3_MULTICAST, 
     PKT_FLOW_ID_0_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 2, MICRO_FLOW_TYPE_FIXED_SYSTEM_HDRf, 4},
   
    {BCM_HASH_PKT_FLOW_TYPE_TUNNEL_VXLAN, 
     PKT_FLOW_ID_1_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 4, INVALIDf, 0},
   
    {BCM_HASH_PKT_FLOW_TYPE_TUNNEL_NVGRE, 
     PKT_FLOW_ID_1_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 3, INVALIDf, 0},
   
    {BCM_HASH_PKT_FLOW_TYPE_TUNNEL_MIM, 
     PKT_FLOW_ID_1_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 2, INVALIDf, 0},
   
    {BCM_HASH_PKT_FLOW_TYPE_TUNNEL_IPINIP, 
     PKT_FLOW_ID_1_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 5, INVALIDf, 0},
   
    {BCM_HASH_PKT_FLOW_TYPE_TUNNEL_MPLS, 
     PKT_FLOW_ID_1_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 6, INVALIDf, 0},
   
    {BCM_HASH_PKT_FLOW_TYPE_LAYER_L2, 
     PKT_FLOW_ID_2_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 2, INVALIDf, 0},
   
    {BCM_HASH_PKT_FLOW_TYPE_LAYER_L3, 
     PKT_FLOW_ID_2_FORMATfmt,
     MACRO_FLOW_ID_FIXEDf, 3, INVALIDf, 0},

    /* end marker */
    {BCM_HASH_PKT_TYPE_MAX, INVALIDfmt, INVALIDf, 0, INVALIDf, 0},
};

/* TD3 flex object names */
STATIC char *_bcm_hash_td3_obj_names[] = {
    "Z0_MATCH_ID",
    "Z1_MATCH_ID",
    "Z2_MATCH_ID",
    "Z3_MATCH_ID",
    "Z4_MATCH_ID",
    "PKT_FLOW_ID_0",
    "PKT_FLOW_ID_1",
    "PKT_FLOW_ID_2",
    "HVE_CONTROL_1",
    "HVE_CONTROL_2"
};
    
STATIC uint8 _bcm_hash_td3_cmd_offset[BCM_HASH_OBJ_OFFSET_MAX];
STATIC uint8 _bcm_hash_td3_cmd_obj[BCM_HASH_OBJ_ID_MAX];

STATIC int _bcm_hash_td3_flex_field_get(int unit, int num_fields,
    bcm_hash_flex_field_t *flex_field, _bcm_hash_td3_flex_obj_t *flex_obj,
    _bcm_hash_td3_flex_obj_t *flex_obj_mask);
STATIC int _bcm_hash_td3_flex_field_add(int unit, int num_fields,
    bcm_hash_flex_field_t *flex_field, _bcm_hash_td3_flex_obj_t *flex_obj,
    _bcm_hash_td3_flex_obj_t *flex_obj_mask);
STATIC int _bcm_td3_hash_flex_bin_delete_all(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      _bcm_hash_td3_fbin_wb_alloc
 * Purpose:
 *      Alloc persisitent memory for Level 2 Warm Boot scache.
 * Parameters:
 *      unit - (IN) Device Unit Number.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_hash_td3_fbin_wb_alloc (int unit)
{
    int alloc_sz = 0, rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8 *hash_scache_ptr = NULL;
    int stable_size;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    alloc_sz = soc_mem_index_count(unit, FLEX_BIN_HASH_PROFILE_0m);
    alloc_sz *= _BCM_HASH_TD3_NUM_PROF_MEM;
    alloc_sz *= _BCM_HASH_TD3_NUM_BINS_PER_SET;
    alloc_sz *= sizeof(uint16);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_HASH, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 alloc_sz, (uint8**)&hash_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_hash_td3_fbin_wb_recover
 * Purpose:
 *      Recover hash flex bin module info for Level 2 Warm Boot from
 *      persisitent memory.
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_hash_td3_fbin_wb_recover (int unit)
{
    int sz = 0, rv = BCM_E_NONE;
    int stable_size;
    uint16 recovered_ver = 0;
    uint8 *hash_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    bcmi_hash_flex_bin_info_t *info;

    info = &bcmi_hash_flex_bin_info[unit];

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_HASH, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &hash_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } else if (rv == BCM_E_NOT_FOUND) {
        return _bcm_hash_td3_fbin_wb_alloc(unit);
    }

    if (hash_scache_ptr != NULL) {
        sz = soc_mem_index_count(unit, FLEX_BIN_HASH_PROFILE_0m);
        sz *= _BCM_HASH_TD3_NUM_PROF_MEM;
        sz *= _BCM_HASH_TD3_NUM_BINS_PER_SET;
        sz *= sizeof(uint16);

        /* recover bin object id array */
        sal_memcpy(info->bin_obj_arr, hash_scache_ptr, sz);
        hash_scache_ptr += sz;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_hash_td3_fbin_reinit
 * Purpose:
 *      Warm boot recovery for the hash flex bin module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_hash_td3_fbin_reinit (int unit)
{
    int rv;
    int i;
    int size;
    bcmi_hash_flex_bin_info_t *info;
    flex_rtag7_hash_tcam_entry_t rule_entry;
    soc_mem_t mem;

    info = &bcmi_hash_flex_bin_info[unit];
    mem  = FLEX_RTAG7_HASH_TCAMm;

    /* recover entry usage bitmap from hardware */
    size = soc_mem_index_count(unit, mem);
    for (i = 0; i < size; i++) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, i,
                 &rule_entry);
        SOC_IF_ERROR_RETURN(rv);

        if (soc_mem_field32_get(unit, mem, &rule_entry, VALIDf)) {
            SHR_BITSET(info->entry_bmp, i);
        }
    }

    /* Recover L2 scache */
    rv = _bcm_hash_td3_fbin_wb_recover(unit);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_hash_flex_bin_sync
 * Purpose:
 *      Record hash flex bin module persistent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_td3_hash_flex_bin_sync(int unit)
{
    int sz = 0, rv = BCM_E_NONE;
    int stable_size;
    uint8 *hash_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    bcmi_hash_flex_bin_info_t *info;

    if (!soc_feature(unit, soc_feature_hash_flex_bin)) {
        return BCM_E_UNAVAIL;
    }

    info = &bcmi_hash_flex_bin_info[unit];

    if (info->initialized == FALSE) {
         return BCM_E_INIT;
    }

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_HASH, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &hash_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INTERNAL)) {
        return rv;
    }

    if (hash_scache_ptr != NULL) {
        sz = soc_mem_index_count(unit, FLEX_BIN_HASH_PROFILE_0m);
        sz *= _BCM_HASH_TD3_NUM_PROF_MEM;
        sz *= _BCM_HASH_TD3_NUM_BINS_PER_SET;
        sz *= sizeof(uint16);

        /* recover bin object id array */
        sal_memcpy(hash_scache_ptr, info->bin_obj_arr, sz);
        hash_scache_ptr += sz;
    }
    return BCM_E_NONE;
}

#endif /*  BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *   bcmi_td3_hash_flex_bin_init
 * Purpose:
 *   Initialize the TD3 hash flex bin module. Should only be called during
 * initialization stage.
 *
 * Parameters:
 *   unit    - (IN)    Unit ID.
 */

int bcmi_td3_hash_flex_bin_init (int unit)
{
    bcmi_hash_flex_bin_info_t *info;
    uint8 *cmd_offset;
    uint8 *cmd_obj;
    int offset;
    int size;
    int rv = BCM_E_NONE;

    info = &bcmi_hash_flex_bin_info[unit];

    if (info->entry_bmp != NULL) {
        sal_free(info->entry_bmp);
         info->entry_bmp = NULL;
    }

    /* allocate the TCAM entry usage bitmap */
    size = soc_mem_index_count(unit, FLEX_RTAG7_HASH_TCAMm);
    size = SHR_BITALLOCSIZE(size);
    info->entry_bmp = sal_alloc(size, "Hash flex bin TCAM usage bitmap");
    if (info->entry_bmp == NULL) {
        rv = BCM_E_MEMORY;
        goto exit;
    }
    sal_memset(info->entry_bmp, 0, size);

    /* allocate buffer to record the bin command object id 
     * TD3 objects have overlays. Multiple objects can map to the same hardware
     * value. So the correct object Id cannot be retrieved from hardware.
     */
    if (info->bin_obj_arr != NULL) {
        sal_free(info->bin_obj_arr);
         info->bin_obj_arr = NULL;
    }
    size = soc_mem_index_count(unit, FLEX_BIN_HASH_PROFILE_0m);
    size *= _BCM_HASH_TD3_NUM_PROF_MEM;
    size *= _BCM_HASH_TD3_NUM_BINS_PER_SET;
    size *= sizeof(uint16);
    info->bin_obj_arr = sal_alloc(size, "Hash flex bin object id map");
    if (info->bin_obj_arr == NULL) {
        rv = BCM_E_MEMORY;
        goto exit;
    }
    sal_memset(info->bin_obj_arr, 0, size);

    /* This is TD3 specific command offset mapping */
    cmd_obj     = _bcm_hash_td3_cmd_obj;
    cmd_offset  = _bcm_hash_td3_cmd_offset; 

    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT0] = 0;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT4] = 1;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT8] = 2;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT12] = 3;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT16] = 4;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT20] = 5;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT24] = 6;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT28] = 7;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT32] = 8;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT36] = 9;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT40] = 10;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT44] = 11;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT48] = 12;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT52] = 13;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT56] = 14;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT60] = 15;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT64] = 16;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT68] = 17;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT72] = 18;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT76] = 19;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT80] = 20;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT84] = 21;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT88] = 22;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT92] = 23;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT96] = 24;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT100] = 25;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT104] = 26;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT108] = 27;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT112] = 28;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT116] = 29;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT120] = 30;
    cmd_offset[BCM_HASH_OBJ_OFFSET_BIT124] = 31;

    /* hash object bus container */
    cmd_obj[BCM_HASH_OBJ_SGPP]        = 96;
    cmd_obj[BCM_HASH_OBJ_SVP]         = 97;
    cmd_obj[BCM_HASH_OBJ_OUTER_PROTO] = 98;
    cmd_obj[BCM_HASH_OBJ_HG_LBID]     = 99;
    cmd_obj[BCM_HASH_OBJ_OUTER_SIP]   = 100;
    cmd_obj[BCM_HASH_OBJ_OUTER_DIP]   = 101;
    cmd_obj[BCM_HASH_OBJ_OUTER_SIP_NORMALIZED] = 102;
    cmd_obj[BCM_HASH_OBJ_OUTER_DIP_NORMALIZED] = 103;
    cmd_obj[BCM_HASH_OBJ_INNER_SIP]   = 104;
    cmd_obj[BCM_HASH_OBJ_INNER_DIP]   = 105;
    cmd_obj[BCM_HASH_OBJ_INNER_SIP_NORMALIZED] = 106;
    cmd_obj[BCM_HASH_OBJ_INNER_DIP_NORMALIZED] = 107;
    cmd_obj[BCM_HASH_OBJ_OUTER_L4_SRC_PORT]    = 108;
    cmd_obj[BCM_HASH_OBJ_OUTER_L4_DEST_PORT]   = 109;
    cmd_obj[BCM_HASH_OBJ_INNER_L4_SRC_PORT]    = 110;
    cmd_obj[BCM_HASH_OBJ_INNER_L4_DEST_PORT]   = 111;
    cmd_obj[BCM_HASH_OBJ_OUTER_L4_SRC_PORT_NORMALIZED]  = 112;
    cmd_obj[BCM_HASH_OBJ_OUTER_L4_DEST_PORT_NORMALIZED] = 113;
    cmd_obj[BCM_HASH_OBJ_INNER_L4_SRC_PORT_NORMALIZED]  = 114;
    cmd_obj[BCM_HASH_OBJ_INNER_L4_DEST_PORT_NORMALIZED] = 115;
    cmd_obj[BCM_HASH_OBJ_INNER_PROTO]          = 116; 
    cmd_obj[BCM_HASH_OBJ_MPLS_LABEL_1]         = 117; 
    cmd_obj[BCM_HASH_OBJ_MPLS_LABEL_2]         = 118;
    cmd_obj[BCM_HASH_OBJ_MPLS_LABEL_3]         = 119;
    cmd_obj[BCM_HASH_OBJ_MPLS_LABEL_4]         = 120;
    cmd_obj[BCM_HASH_OBJ_MPLS_LABEL_5]         = 121;

    /* field bus 1 */
    cmd_obj[BCM_HASH_OBJ_OUTER_MAC_SA] = 0;
    cmd_obj[BCM_HASH_OBJ_OUTER_MAC_DA] = 0;
    cmd_obj[BCM_HASH_OBJ_OUTER_VNTAG_ETAG]  = 1;
    cmd_obj[BCM_HASH_OBJ_OUTER_OTAG]   = 2;
    cmd_obj[BCM_HASH_OBJ_OUTER_ITAG]   = 3;
    cmd_obj[BCM_HASH_OBJ_OUTER_CNTAG]  = 4;
    cmd_obj[BCM_HASH_OBJ_OUTER_ETHTYPE] = 5;
    cmd_obj[BCM_HASH_OBJ_OUTER_IPV6_SIP]    = 6;
    cmd_obj[BCM_HASH_OBJ_OUTER_IPV4_HEADER] = 6;
    cmd_obj[BCM_HASH_OBJ_OUTER_IPV4_SIP]    = 6;
    cmd_obj[BCM_HASH_OBJ_OUTER_IPV6_HEADER1]    = 7;
    cmd_obj[BCM_HASH_OBJ_OUTER_IPV4_DIP]        = 7;
    cmd_obj[BCM_HASH_OBJ_OUTER_IPV6_DIP]        = 8;
    cmd_obj[BCM_HASH_OBJ_OUTER_IPV6_HEADER2]    = 9;
    cmd_obj[BCM_HASH_OBJ_OUTER_GRE_HEADER] = 10;
    cmd_obj[BCM_HASH_OBJ_OUTER_L4]     = 10;
    cmd_obj[BCM_HASH_OBJ_OUTER_L5]     = 12;
    cmd_obj[BCM_HASH_OBJ_OUTER_VXLAN]  = 12;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF0_0] = 13;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF0_1] = 14;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF0_2] = 15;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF0_3] = 16;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF0_4] = 17;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF0_5] = 18;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF0_6] = 19;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF0_7] = 20;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF1_0] = 21;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF1_1] = 22;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF1_2] = 23;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF1_3] = 24;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF1_4] = 25;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF1_5] = 26;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF1_6] = 27;
    cmd_obj[BCM_HASH_OBJ_OUTER_UDF1_7] = 28;
    cmd_obj[BCM_HASH_OBJ_OUTER_TOS] = 29;
    cmd_obj[BCM_HASH_OBJ_OUTER_TTL] = 30;
    cmd_obj[BCM_HASH_OBJ_SUBPORT_TAG] = 31;
    
    /* field bus 2  */
    offset = 48;
    cmd_obj[BCM_HASH_OBJ_INNER_MAC_SA] = (offset + 0);
    cmd_obj[BCM_HASH_OBJ_INNER_MAC_DA] = (offset + 0);
    cmd_obj[BCM_HASH_OBJ_INNER_VNTAG_ETAG]  = (offset + 1);
    cmd_obj[BCM_HASH_OBJ_INNER_OTAG]   = (offset + 2);
    cmd_obj[BCM_HASH_OBJ_INNER_ITAG]   = (offset + 3);
    cmd_obj[BCM_HASH_OBJ_INNER_CNTAG]  = (offset + 4);
    cmd_obj[BCM_HASH_OBJ_INNER_ETHTYPE] = (offset + 5);
    cmd_obj[BCM_HASH_OBJ_INNER_IPV6_SIP]    = (offset + 6);
    cmd_obj[BCM_HASH_OBJ_INNER_IPV4_HEADER] = (offset + 6);
    cmd_obj[BCM_HASH_OBJ_INNER_IPV4_SIP]    = (offset + 6);
    cmd_obj[BCM_HASH_OBJ_INNER_IPV6_HEADER1]    = (offset + 7);
    cmd_obj[BCM_HASH_OBJ_INNER_IPV4_DIP]        = (offset + 7);
    cmd_obj[BCM_HASH_OBJ_INNER_IPV6_DIP]        = (offset + 8);
    cmd_obj[BCM_HASH_OBJ_INNER_IPV6_HEADER2]    = (offset + 9);
    cmd_obj[BCM_HASH_OBJ_INNER_GRE_HEADER] = (offset + 10);
    cmd_obj[BCM_HASH_OBJ_INNER_L4]         = (offset + 10);  
    cmd_obj[BCM_HASH_OBJ_INNER_VXLAN]   = (offset + 12);
    cmd_obj[BCM_HASH_OBJ_INNER_L5]      = (offset + 12);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF0_0]  = (offset + 13);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF0_1]  = (offset + 14);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF0_2] = (offset + 15);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF0_3] = (offset + 16);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF0_4] = (offset + 17);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF0_5] = (offset + 18);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF0_6] = (offset + 19);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF0_7] = (offset + 20);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF1_0] = (offset + 21);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF1_1] = (offset + 22);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF1_2] = (offset + 23);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF1_3] = (offset + 24);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF1_4] = (offset + 25);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF1_5] = (offset + 26);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF1_6] = (offset + 27);
    cmd_obj[BCM_HASH_OBJ_INNER_UDF1_7] = (offset + 28);
    cmd_obj[BCM_HASH_OBJ_INNER_TOS]    = (offset + 29);
    cmd_obj[BCM_HASH_OBJ_INNER_TTL]    = (offset + 30);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_hash_td3_fbin_reinit(unit);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
    } else {
        rv = _bcm_td3_hash_flex_bin_delete_all(unit);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
        rv = _bcm_hash_td3_fbin_wb_alloc(unit);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
    }
#else
    rv = _bcm_td3_hash_flex_bin_delete_all(unit);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    info->initialized = TRUE;
    return BCM_E_NONE;

exit:
    if (info->entry_bmp) {
        sal_free (info->entry_bmp);
        info->entry_bmp = NULL;
    }
    if (info->bin_obj_arr) {
        sal_free (info->bin_obj_arr);
        info->bin_obj_arr = NULL;
    }
    return rv;
}

/*
 * Function:
 *     bcmi_td3_hash_flex_field_id_get
 * Description:
 *     Get the field_id from the flex object's field name. In Trident3,
 * the flex objects are defined in the ceh.yml file. Each object defines a
 * list of flex fields. For example:
 *   Flex object:  Z1_MATCH_ID
 *   Field:        ETAG
 * The ceh.yml file is packaged and loaded in the runtime.
 *
 * Parameters:
 *    Unit           (IN) Unit number
 *    object         (IN) Flex object id
 *    field_name     (IN) Field name
 *    field_id       (OUT) field ID
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *    This API is used for bcm_hash_flex_bin_add API's flex field.
 */

int
bcmi_td3_hash_flex_field_id_get (
    int unit,
    bcm_hash_flex_object_t object,
    const char *field_name,
    uint32 *field_id)
{
    int rv;
    
    *field_id = 0;
    rv = soc_cancun_ceh_obj_field_id_get(unit, (char *)field_name, field_id);
    SOC_IF_ERROR_RETURN(rv);

    *field_id |= object << 24;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_td3_hash_flex_field_name_get
 * Description:
 *     Get the flex object ID and field name from the field_id. In Trident3,
 * the flex objects are defined in the ceh.yml file. Each object defines a
 * list of flex fields. For example:
 *   Flex object:  Z1_MATCH_ID
 *   Field:        ETAG
 * The ceh.yml file is packaged and loaded in the runtime.
 *
 * Parameters:
 *    unit           (IN) Unit number
 *    field_id       (IN) flex field ID
 *    object         (OUT) Flex object id
 *    size           (IN) size of the field name buffer
 *    field_name     (OUT) Field name buffer
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *    This API is used for bcm_hash_flex_bin_get API's flex field.
 */

int
bcmi_td3_hash_flex_field_name_get (
    int unit,
    uint32 field_id,
    bcm_hash_flex_object_t *object,
    int *size,
    char *field_name)
{
    int rv;
   
    *object = FIELD_OBJ_ID(field_id);
    rv = soc_cancun_ceh_obj_field_name_get(unit, FIELD_NAME_ID(field_id), 
             size, field_name);
    SOC_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_td3_hash_flex_bin_add
 * Purpose:
 *   Specify the flex bin selection rule and write to the TCAM entry. Higher
 * entry_id has high priority during TCAM lookup in case of multiple rules
 * are matched. If entry_id is not specified, i.e. option
 * BCM_HASH_FLEX_BIN_OPTION_WITH_ID is not given, then a lowest available
 * entry_id is automatically assigned.
 *
 * Parameters:
 *   unit     - (IN)    Unit ID.
 *   cfg      - (INOUT) Key part of selection rule and other parameters
 *   num_bins - (IN)   Number of bin cmds.
 *   bin_cmd  - (IN) Array of bin commands. Policy data part of rule.
 *   num_fields - (IN) Number of flex fields.
 *   flex_field - (IN) Array of flex fields.
 */

int
bcmi_td3_hash_flex_bin_add (int unit,
    bcm_hash_flex_bin_config_t *cfg,
    int num_bins,
    bcm_hash_flex_bin_cmd_t *bin_cmd,
    int num_fields,
    bcm_hash_flex_field_t *flex_field)
{
    soc_mem_t mem;
    int ix,iy;
    int tcam_size;
    flex_rtag7_hash_tcam_entry_t rule_entry;
    flex_bin_hash_profile_0_entry_t prof_entry;
    flex_bin_hash_profile_0_entry_t prof_entry1;
    flex_bin_hash_profile_0_entry_t prof_entry2;
    flex_bin_hash_profile_0_entry_t prof_entry3;
    flex_bin_hash_profile_0_entry_t *prof[_BCM_HASH_TD3_NUM_PROF_MEM];
    int rv = BCM_E_UNAVAIL;
    uint32 data32;
    uint32 data1_32;
    uint32 flow_id0;
    uint32 flow_id1;
    uint32 flow_id2;
    uint32 flow_id0_mask;
    uint32 flow_id1_mask;
    uint32 flow_id2_mask;
    uint32 *data32_p;
    uint32 *data32_mask_p;
    _bcm_hash_td3_flex_obj_t flex_obj;
    _bcm_hash_td3_flex_obj_t flex_obj_mask;
    int fld_size;
    _bcm_hash_sfld_info_t *sfld_info;
    _bcm_hash_pfld_info_t *pfld_info;
    uint8 *cmd_offset;
    uint8 *cmd_obj;
    uint8 offset_val;
    uint8 obj_val;
    _bcm_hash_prof_info_t *prof_info;
    bcmi_hash_flex_bin_info_t *info;
    int tcam_inx;
    uint32 tmp_buf[2];
    int bin_num;
    int mem_cnt;
    int bin_inx;

    info = &bcmi_hash_flex_bin_info[unit];
    if (info->initialized == FALSE) {
         return BCM_E_INIT;
    }
    pfld_info = _bcm_hash_td3_pfld_info;
    sfld_info = _bcm_hash_td3_sfld_info;
    cmd_offset = _bcm_hash_td3_cmd_offset;
    cmd_obj    = _bcm_hash_td3_cmd_obj;
    prof_info  = &_bcm_hash_td3_prof_info;
    prof[0] = &prof_entry;
    prof[1] = &prof_entry1;
    prof[2] = &prof_entry2;
    prof[3] = &prof_entry3;

    mem = FLEX_RTAG7_HASH_TCAMm;
    tcam_size = soc_mem_index_count(unit, mem);
    sal_memset(&flex_obj, 0, sizeof(flex_obj));
    sal_memset(&flex_obj_mask, 0, sizeof(flex_obj_mask));
    sal_memset(&rule_entry, 0, sizeof(rule_entry));
    flow_id0 = 0;
    flow_id1 = 0;
    flow_id2 = 0;
    flow_id0_mask = 0;
    flow_id1_mask = 0;
    flow_id2_mask = 0;

    /* error check */
    if (cfg->option & BCM_HASH_FLEX_BIN_OPTION_REPLACE) {
        if (!(cfg->option & BCM_HASH_FLEX_BIN_OPTION_WITH_ID)) {
            return BCM_E_PARAM;
        }
    }

    /* validate bin_id */
    for (ix = 0; ix < num_bins; ix++) {
        if (!BIN_ID_VALID(cfg, bin_cmd[ix].bin_id)) {
            return BCM_E_PARAM;
        }
    }

    /* find an empty entry */
    if (!(cfg->option & BCM_HASH_FLEX_BIN_OPTION_WITH_ID)) {
        for (tcam_inx = tcam_size - 1; tcam_inx >= 0; tcam_inx--) {
            if (!SHR_BITGET(info->entry_bmp, tcam_inx)) {
                break;
            }
        }
        if (tcam_inx < 0) {   /* TCAM Full */
            return BCM_E_FULL;
        }
        cfg->entry_id = tcam_size - 1 - tcam_inx;
    } else {
        tcam_inx = tcam_size - 1 - cfg->entry_id;
        if (tcam_inx < 0) {
            LOG_ERROR(BSL_LS_BCM_HASH, (BSL_META_U(unit,
                "Entry ID %d must be less than the hardware table size: %d\n"),
                        cfg->entry_id, tcam_size));
            return BCM_E_PARAM;
        }
        if (cfg->option & BCM_HASH_FLEX_BIN_OPTION_REPLACE) {
            if (!SHR_BITGET(info->entry_bmp, tcam_inx)) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            if (SHR_BITGET(info->entry_bmp, tcam_inx)) {
                return BCM_E_EXISTS;
            }
        }
    }

    /* handle the pkt_type */
    iy = 0;
    while (pfld_info[iy].pkt_type != BCM_HASH_PKT_TYPE_MAX) {
        if (BCM_HASH_PKT_TYPE_SET_TEST(cfg->tset, pfld_info[iy].pkt_type)) {
            if (pfld_info[iy].fmt == PKT_FLOW_ID_2_FORMATfmt) {
                data32_p = &flow_id2;
                data32_mask_p = &flow_id2_mask;
            } else if (pfld_info[iy].fmt == PKT_FLOW_ID_1_FORMATfmt) {
                data32_p = &flow_id1;
                data32_mask_p = &flow_id1_mask;
            } else if (pfld_info[iy].fmt == PKT_FLOW_ID_0_FORMATfmt) {
                data32_p = &flow_id0;
                data32_mask_p = &flow_id0_mask;
            } else {
                return BCM_E_INTERNAL;
            } 
            fld_size = soc_format_field_length(unit,
                            pfld_info[iy].fmt, pfld_info[iy].fld1);
            data32 = 0;
            soc_format_field32_set(unit, pfld_info[iy].fmt,
                &data32, pfld_info[iy].fld1, (1 << fld_size) - 1);
           
            /* indicate error if field is overlapped */
            if ((*data32_mask_p) & data32) { 
                LOG_ERROR(BSL_LS_BCM_HASH, (BSL_META_U(unit,
                    "Packet flow type conflict: %d\n"),
                    pfld_info[iy].pkt_type));
                return BCM_E_PARAM;
            }
            soc_format_field32_set(unit, pfld_info[iy].fmt,
                data32_mask_p, pfld_info[iy].fld1, (1 << fld_size) - 1);

            soc_format_field32_set(unit, pfld_info[iy].fmt,
                data32_p, pfld_info[iy].fld1, pfld_info[iy].val1);

            if (pfld_info[iy].fld2 != INVALIDf) {
                soc_format_field32_set(unit, pfld_info[iy].fmt,
                     data32_p, pfld_info[iy].fld2, pfld_info[iy].val2);
                fld_size = soc_format_field_length(unit,
                            pfld_info[iy].fmt, pfld_info[iy].fld2);
                soc_format_field32_set(unit, pfld_info[iy].fmt,
                             data32_mask_p, pfld_info[iy].fld2, 
                             (1 << fld_size) - 1);
            }
        }
        iy++;
    }

    /* handle the pkt status */   
    iy = 0;
    while (sfld_info[iy].pkt_status != BCM_HASH_PKT_STATUS_MAX) {
        ix = sfld_info[iy].pkt_status;
        if (BCM_HASH_PKT_STATUS_SET_TEST(cfg->sset_mask, ix)) {
            if (sfld_info[iy].fld_size == 1) {
                soc_mem_field32_set(unit, mem, &rule_entry, 
                    sfld_info[iy].fld_mask, 1);
                soc_mem_field32_set(unit, mem, &rule_entry, 
                    sfld_info[iy].fld, 
                    BCM_HASH_PKT_STATUS_SET_TEST(cfg->sset, ix)? 1 :0);
            } else {
                return BCM_E_INTERNAL;
            }
        }
        iy++;
    }

    /* handle flex objects */
    rv = _bcm_hash_td3_flex_field_add(unit, num_fields, flex_field, &flex_obj, 
        &flex_obj_mask);
    BCM_IF_ERROR_RETURN(rv);

    /* check if flex object conflicts with the fixed assignment */
    if (flex_obj_mask.flow_id0 & flow_id0_mask) {
        LOG_ERROR(BSL_LS_BCM_HASH, (BSL_META_U(unit,
            "Flex object flow_id_0 conflicts with packet flow type\n")));
        return BCM_E_PARAM;
    } else {
        flow_id0_mask |= flex_obj_mask.flow_id0;
        flow_id0      |= flex_obj.flow_id0;
    }

    if (flex_obj_mask.flow_id1 & flow_id1_mask) {
        LOG_ERROR(BSL_LS_BCM_HASH, (BSL_META_U(unit,
            "Flex object flow_id_1 conflicts with packet flow type\n")));
        return BCM_E_PARAM;
    } else {
        flow_id1_mask |= flex_obj_mask.flow_id1;
        flow_id1      |= flex_obj.flow_id1;
    }

    if (flex_obj_mask.flow_id2 & flow_id2_mask) {
        LOG_ERROR(BSL_LS_BCM_HASH, (BSL_META_U(unit,
            "Flex object flow_id_2 conflicts with packet flow type\n")));
        return BCM_E_PARAM;
    } else {
        flow_id2_mask |= flex_obj_mask.flow_id2;
        flow_id2      |= flex_obj.flow_id2;
    }

    /* set flow ids */
    if (flow_id2_mask) {
        soc_mem_field32_set(unit, mem, &rule_entry, PKT_FLOW_ID_2_MASKf, 
                        flow_id2_mask);
        soc_mem_field32_set(unit, mem, &rule_entry, PKT_FLOW_ID_2f, flow_id2);
    }
    if (flow_id1_mask) {
        soc_mem_field32_set(unit, mem, &rule_entry, PKT_FLOW_ID_1_MASKf, 
                        flow_id1_mask);
        soc_mem_field32_set(unit, mem, &rule_entry, PKT_FLOW_ID_1f, flow_id1);
    }
    if (flow_id0_mask) {
        soc_mem_field32_set(unit, mem, &rule_entry, PKT_FLOW_ID_0_MASKf, 
                        flow_id0_mask);
        soc_mem_field32_set(unit, mem, &rule_entry, PKT_FLOW_ID_0f, flow_id0);
    }

    /* match_id */
    if (!COMPILER_64_IS_ZERO(flex_obj_mask.match_id)) {
        tmp_buf[1] = COMPILER_64_HI(flex_obj_mask.match_id);
        tmp_buf[0] = COMPILER_64_LO(flex_obj_mask.match_id);
        soc_mem_field_set(unit, mem, (uint32 *)&rule_entry, MATCH_ID_MASKf, 
                      tmp_buf);
        tmp_buf[1] = COMPILER_64_HI(flex_obj.match_id);
        tmp_buf[0] = COMPILER_64_LO(flex_obj.match_id);
        soc_mem_field_set(unit, mem, (uint32 *)&rule_entry, MATCH_IDf,
                      tmp_buf);
    }

    /* hve control */
    if (flex_obj_mask.hve_ctrl2) {
        soc_mem_field32_set(unit, mem, &rule_entry, HVE_CONTROL_2_MASKf, 
                        flex_obj_mask.hve_ctrl2);
        soc_mem_field32_set(unit, mem, &rule_entry, HVE_CONTROL_2f, 
                        flex_obj.hve_ctrl2);
    }
    if (flex_obj_mask.hve_ctrl1) {
        soc_mem_field32_set(unit, mem, &rule_entry, HVE_CONTROL_1_MASKf, 
                        flex_obj_mask.hve_ctrl1);
        soc_mem_field32_set(unit, mem, &rule_entry, HVE_CONTROL_1f, 
                        flex_obj.hve_ctrl1);
    }

    /* handle the control fields */
    soc_mem_field32_set(unit, mem, &rule_entry, VALIDf, 3);

    soc_mem_field32_set(unit, mem, &rule_entry, VFI_HASH_CTRL_ID_MASKf, 
                        cfg->vfi_ctrl_id_mask);
    soc_mem_field32_set(unit, mem, &rule_entry, VFI_HASH_CTRL_IDf, 
                        cfg->vfi_ctrl_id);
    
    soc_mem_field32_set(unit, mem, &rule_entry, VLAN_HASH_CTRL_ID_MASKf, 
                            cfg->vlan_ctrl_id_mask);
    soc_mem_field32_set(unit, mem, &rule_entry, VLAN_HASH_CTRL_IDf, 
                            cfg->vlan_ctrl_id);
    
    soc_mem_field32_set(unit, mem, &rule_entry, L3_IIF_HASH_CTRL_ID_MASKf, 
                            cfg->l3iif_ctrl_id_mask);
    soc_mem_field32_set(unit, mem, &rule_entry, L3_IIF_HASH_CTRL_IDf, 
                            cfg->l3iif_ctrl_id);
    
    soc_mem_field32_set(unit, mem, &rule_entry, SVP_HASH_CTRL_ID_MASKf, 
                            cfg->svp_ctrl_id_mask);
    soc_mem_field32_set(unit, mem, &rule_entry, SVP_HASH_CTRL_IDf, 
                            cfg->svp_ctrl_id);
    
    soc_mem_field32_set(unit, mem, &rule_entry, SGPP_HASH_CTRL_ID_MASKf, 
                            cfg->sgpp_ctrl_id_mask);
    soc_mem_field32_set(unit, mem, &rule_entry, SGPP_HASH_CTRL_IDf, 
                            cfg->sgpp_ctrl_id);
    
    soc_mem_field32_set(unit, mem, &rule_entry, 
                        BIN_CONTROL_PROFILEf, tcam_inx);

    /* handle policy data command */
    /* In TD3, the bin_cmd profile is 128 deep same as the TCAM. Use 
     * one-to-one mapping. No need to employ profile mechanism
     */

    /* TD3
     * FLEX_BIN_HASH_PROFILE_0: LSB 8-bit, A-bin 0-12
     * FLEX_BIN_HASH_PROFILE_1: MSB 8-bit, A-bin 0-12
     * FLEX_BIN_HASH_PROFILE_2: LSB 8-bit, B-bin 0-12
     * FLEX_BIN_HASH_PROFILE_3: MSB 8-bit, B-bin 0-12
     */
    for (ix = 0; ix < _BCM_HASH_TD3_NUM_PROF_MEM; ix++) {
        sal_memset(prof[ix], 0, sizeof(prof_entry));
    }
    for (ix = 0; ix < num_bins; ix++) {
        if (cfg->option & BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT) {
            if (bin_cmd[ix].bin_id <= BCM_HASH_BIN_A_MAX) {
                bin_num = bin_cmd[ix].bin_id - BCM_HASH_BIN_A_0;
            } else if (bin_cmd[ix].bin_id <= BCM_HASH_BIN_A_UPPER_HALF_MAX) {
                bin_num = bin_cmd[ix].bin_id - BCM_HASH_BIN_A_0_UPPER_HALF;
                bin_num += _BCM_HASH_TD3_NUM_BINS_PER_SET;
            } else if (bin_cmd[ix].bin_id <= BCM_HASH_BIN_B_MAX) {
                bin_num = bin_cmd[ix].bin_id - BCM_HASH_BIN_B_0;
                bin_num += 2 * _BCM_HASH_TD3_NUM_BINS_PER_SET;
            } else { /* bin_cmd[ix].bin_id <= BCM_HASH_BIN_B_UPPER_HALF_MAX */
                bin_num = bin_cmd[ix].bin_id - BCM_HASH_BIN_B_0_UPPER_HALF;
                bin_num += 3 * _BCM_HASH_TD3_NUM_BINS_PER_SET;
            }
        } else {
            if (bin_cmd[ix].bin_id <= BCM_HASH_BIN_A_MAX) {
                bin_num = bin_cmd[ix].bin_id - BCM_HASH_BIN_A_0;
            } else {
                bin_num = (bin_cmd[ix].bin_id - BCM_HASH_BIN_B_0);
                bin_num += _BCM_HASH_TD3_NUM_BINS_PER_SET;
            }
        }              
       
        /* save the object_id for the given bin */
        if (cfg->option & BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT) {
            mem_cnt = bin_num / _BCM_HASH_TD3_NUM_BINS_PER_SET;
            bin_inx = BIN_INX(unit, mem_cnt, tcam_inx, bin_num);
            info->bin_obj_arr[bin_inx] = bin_cmd[ix].obj_id;
        } else {
            mem_cnt = 2 * (bin_num / _BCM_HASH_TD3_NUM_BINS_PER_SET);
            bin_inx = BIN_INX(unit, mem_cnt, tcam_inx, bin_num);
            info->bin_obj_arr[bin_inx] = bin_cmd[ix].obj_id;
            bin_inx = BIN_INX(unit, mem_cnt + 1, tcam_inx, bin_num);
            info->bin_obj_arr[bin_inx] = bin_cmd[ix].obj_id;
        }
  
        /* convert to TD3 specific hardware offset and object value */
        offset_val = cmd_offset[bin_cmd[ix].obj_offset];
        obj_val    = cmd_obj[bin_cmd[ix].obj_id];
  
        switch (bin_cmd[ix].obj_id) {
            case BCM_HASH_OBJ_OUTER_MAC_DA:
            case BCM_HASH_OBJ_INNER_MAC_DA:
                offset_val += cmd_offset[BCM_HASH_OBJ_OFFSET_BIT48];
                break;
            case BCM_HASH_OBJ_OUTER_IPV4_SIP:
            case BCM_HASH_OBJ_INNER_IPV4_SIP:
                offset_val += cmd_offset[BCM_HASH_OBJ_OFFSET_BIT96];
                break;
            default:
                break;
        }
 
        /* LSB 8-bit */ 
        data32 = 0;
        if (bin_cmd[ix].obj_id == BCM_HASH_OBJ_ID_INVALID) {
            soc_format_field32_set(unit, FLEX_BIN_HASH_PROFILE_FORMATfmt,
                    &data32, CMD_VALIDf, 0);
        } else {
            soc_format_field32_set(unit, FLEX_BIN_HASH_PROFILE_FORMATfmt,
                    &data32, CMD_VALIDf, 1);
            soc_format_field32_set(unit, FLEX_BIN_HASH_PROFILE_FORMATfmt,
                    &data32, CMD_SRC_CONTAINER_IDf, obj_val);
            soc_format_field32_set(unit, FLEX_BIN_HASH_PROFILE_FORMATfmt,
                    &data32, CMD_SRC_CONTAINER_OFFSETf, offset_val);
            soc_format_field32_set(unit, FLEX_BIN_HASH_PROFILE_FORMATfmt,
                    &data32, CMD_MASKf, (uint8)bin_cmd[ix].bin_mask);
        }

        if (!(cfg->option & BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT)) {
            /* MSB 8-bit */
            data1_32 = data32;
            soc_format_field32_set(unit, FLEX_BIN_HASH_PROFILE_FORMATfmt,
                    &data1_32, CMD_SRC_CONTAINER_OFFSETf, 
                    offset_val + 2); /* TD3 4-bit gradularity */
            soc_format_field32_set(unit, FLEX_BIN_HASH_PROFILE_FORMATfmt,
                    &data1_32, CMD_MASKf, (uint8)(bin_cmd[ix].bin_mask >> 8));

            soc_mem_field32_set(unit, 
                prof_info->mem[2*(bin_num/_BCM_HASH_TD3_NUM_BINS_PER_SET)], 
                prof[2*(bin_num/_BCM_HASH_TD3_NUM_BINS_PER_SET)], 
                prof_info->fld[bin_num%_BCM_HASH_TD3_NUM_BINS_PER_SET], 
                data32);

            soc_mem_field32_set(unit, 
                prof_info->mem[2*(bin_num/_BCM_HASH_TD3_NUM_BINS_PER_SET) + 1], 
                prof[2*(bin_num/_BCM_HASH_TD3_NUM_BINS_PER_SET) + 1], 
                prof_info->fld[bin_num%_BCM_HASH_TD3_NUM_BINS_PER_SET], 
                data1_32);
        } else {
            soc_mem_field32_set(unit, 
                 prof_info->mem[(bin_num/_BCM_HASH_TD3_NUM_BINS_PER_SET)], 
                 prof[(bin_num/_BCM_HASH_TD3_NUM_BINS_PER_SET)], 
                 prof_info->fld[bin_num%_BCM_HASH_TD3_NUM_BINS_PER_SET], 
                 data32);
        }
    }
    for (ix = 0; ix < _BCM_HASH_TD3_NUM_PROF_MEM; ix++) {
        rv = soc_mem_write(unit, prof_info->mem[ix], MEM_BLOCK_ALL, 
                           tcam_inx, prof[ix]);
        SOC_IF_ERROR_RETURN(rv);
    }

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, tcam_inx, &rule_entry);
    SHR_BITSET(info->entry_bmp, tcam_inx);
    return rv;
}

/*
 * Function:
 *   bcmi_td3_hash_flex_bin_delete
 * Purpose:
 *   Delete the given entry
 * Parameters:
 *   unit        - (IN)  Unit ID.
 *   entry_id    - (IN)  The entry ID
 */

int bcmi_td3_hash_flex_bin_delete( int unit, int entry_id)
{
    soc_mem_t mem;
    flex_rtag7_hash_tcam_entry_t rule_entry;
    flex_bin_hash_profile_0_entry_t prof_entry;
    int rv;
    int ix,iy;
    _bcm_hash_prof_info_t *prof_info;
    int tcam_size;
    int tcam_inx;
    bcmi_hash_flex_bin_info_t *info;
    int bin_inx;

    info = &bcmi_hash_flex_bin_info[unit];
    if (info->initialized == FALSE) {
         return BCM_E_INIT;
    }
    mem = FLEX_RTAG7_HASH_TCAMm;
    tcam_size = soc_mem_index_count(unit, mem);
    tcam_inx  = tcam_size - 1 - entry_id;

    if (tcam_inx < 0) {
        LOG_ERROR(BSL_LS_BCM_HASH, (BSL_META_U(unit,
            "Entry ID %d must be less than the hardware table size: %d\n"),
                    entry_id, tcam_size));
        return BCM_E_PARAM;
    }
    SHR_BITCLR(info->entry_bmp, tcam_inx);
    
    prof_info = &_bcm_hash_td3_prof_info;
    sal_memset(&rule_entry, 0, sizeof(rule_entry));
    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, tcam_inx, &rule_entry);
    SOC_IF_ERROR_RETURN(rv);

    for (ix = 0; ix < _BCM_HASH_TD3_NUM_PROF_MEM; ix++) {
        rv = soc_mem_write(unit, prof_info->mem[ix], MEM_BLOCK_ALL,
                           tcam_inx, &prof_entry);
        SOC_IF_ERROR_RETURN(rv);
    }

    for (iy = 0; iy < _BCM_HASH_TD3_NUM_PROF_MEM; iy++) {
        for (ix = 0; ix < _BCM_HASH_TD3_NUM_BINS_PER_SET; ix++) {
            bin_inx = BIN_INX(unit, iy, tcam_inx, ix);
            info->bin_obj_arr[bin_inx] = 0;
        }
    }

    return rv;
}

/*
 * Function:
 *   bcmi_td3_hash_flex_bin_delete_all
 * Purpose:
 *   Delete all entries
 * Parameters:
 *   unit - (IN)  Unit ID.
 */

STATIC int _bcm_td3_hash_flex_bin_delete_all(int unit)
{
    soc_mem_t mem;
    flex_rtag7_hash_tcam_entry_t rule_entry;
    flex_bin_hash_profile_0_entry_t prof_entry;
    int rv;
    int size;
    int ix;
    int iy;
    _bcm_hash_prof_info_t *prof_info;
    bcmi_hash_flex_bin_info_t *info;

    info = &bcmi_hash_flex_bin_info[unit];
    mem = FLEX_RTAG7_HASH_TCAMm;
    size = soc_mem_index_count(unit, mem);
    prof_info = &_bcm_hash_td3_prof_info;
    sal_memset(&rule_entry, 0, sizeof(rule_entry));
    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    for (ix = size - 1; ix >= 0; ix--) {
        SHR_BITCLR(info->entry_bmp, ix);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, ix, &rule_entry);
        SOC_IF_ERROR_RETURN(rv);
        for (iy = 0; iy < _BCM_HASH_TD3_NUM_PROF_MEM; iy++) {
            rv = soc_mem_write(unit, prof_info->mem[iy], MEM_BLOCK_ALL,
                           ix, &prof_entry);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    size = soc_mem_index_count(unit, FLEX_BIN_HASH_PROFILE_0m);
    size *= _BCM_HASH_TD3_NUM_PROF_MEM;
    size *= _BCM_HASH_TD3_NUM_BINS_PER_SET;
    size *= sizeof(uint16);
    sal_memset(info->bin_obj_arr, 0, size);

    return BCM_E_NONE;
}

int bcmi_td3_hash_flex_bin_delete_all(int unit)
{
    bcmi_hash_flex_bin_info_t *info;

    info = &bcmi_hash_flex_bin_info[unit];
    if (info->initialized == FALSE) {
         return BCM_E_INIT;
    }
    return _bcm_td3_hash_flex_bin_delete_all(unit);
}

/*
 * Function:
 *   bcmi_td3_hash_flex_bin_get
 * Purpose:
 *   Get the entry configuration parameters for the given entry_id.
 * Parameters:
 *   unit     - (IN)    Unit ID.
 *   cfg      - (INOUT) Key part of selection rule and other parameters
 *   num_bins - (IN)   Number of bin cmds.
 *   bin_cmd  - (IN) Array of bin commands. Policy part of selection rule.
 *   num_fields - (IN) Number of flex fields.
 *   flex_field - (IN) Array of flex fields.
 */

int bcmi_td3_hash_flex_bin_get (int unit,
              bcm_hash_flex_bin_config_t *cfg,
              int num_bins,
              bcm_hash_flex_bin_cmd_t *bin_cmd,
              int num_fields,
              bcm_hash_flex_field_t *flex_field)
{
    soc_mem_t mem;
    int ix,iy,inx;
    flex_rtag7_hash_tcam_entry_t rule_entry;
    flex_bin_hash_profile_0_entry_t prof_entry;
    flex_bin_hash_profile_0_entry_t prof_entry1;
    flex_bin_hash_profile_0_entry_t prof_entry2;
    flex_bin_hash_profile_0_entry_t prof_entry3;
    flex_bin_hash_profile_0_entry_t *prof[_BCM_HASH_TD3_NUM_PROF_MEM];
    int rv = BCM_E_UNAVAIL;
    uint32 data32;
    uint32 *data32_p;
    uint32 flow_id0;
    uint32 flow_id1;
    uint32 flow_id2;
    _bcm_hash_td3_flex_obj_t flex_obj;
    _bcm_hash_td3_flex_obj_t flex_obj_mask;
    _bcm_hash_sfld_info_t *sfld_info;
    _bcm_hash_pfld_info_t *pfld_info;
    uint8 *cmd_offset;
    uint8 offset_val;
    _bcm_hash_prof_info_t *prof_info;
    int val1;
    int val2;
    int val1_mask;
    uint32 tmp_buf[2];
    int tcam_inx;
    int tcam_size;
    bcmi_hash_flex_bin_info_t *info;
    int num_bin_max;
    int bin_inx;
    int cmd_inx;

    pfld_info = _bcm_hash_td3_pfld_info;
    sfld_info = _bcm_hash_td3_sfld_info;
    cmd_offset = _bcm_hash_td3_cmd_offset;
    prof_info  = &_bcm_hash_td3_prof_info;
    prof[0] = &prof_entry;
    prof[1] = &prof_entry1;
    prof[2] = &prof_entry2;
    prof[3] = &prof_entry3;
    info = &bcmi_hash_flex_bin_info[unit];

    if (info->initialized == FALSE) {
         return BCM_E_INIT;
    }

    mem = FLEX_RTAG7_HASH_TCAMm;
    tcam_size = soc_mem_index_count(unit, mem);
    sal_memset(&flex_obj, 0, sizeof(flex_obj));
    sal_memset(&flex_obj_mask, 0, sizeof(flex_obj_mask));
    sal_memset(&rule_entry, 0, sizeof(rule_entry));
    flow_id0 = 0;
    flow_id1 = 0;
    flow_id2 = 0;

    if (cfg->entry_id >= tcam_size) {
        return BCM_E_PARAM;
    }
    tcam_inx = tcam_size - cfg->entry_id - 1;
    if (tcam_inx < 0) {
        LOG_ERROR(BSL_LS_BCM_HASH, (BSL_META_U(unit,
            "Entry ID %d must be less than the hardware table size: %d\n"),
                    cfg->entry_id, tcam_size));
        return BCM_E_PARAM;
    }
    if (!SHR_BITGET(info->entry_bmp, tcam_inx)) {
        return BCM_E_NOT_FOUND;
    }

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, tcam_inx, &rule_entry);
    SOC_IF_ERROR_RETURN(rv);

    if (!soc_mem_field32_get(unit, mem, &rule_entry, VALIDf)) {
        return SOC_E_NOT_FOUND;
    }

    /* get flow ids */
    flow_id2 = soc_mem_field32_get(unit, mem, &rule_entry, PKT_FLOW_ID_2f);
    flow_id1 = soc_mem_field32_get(unit, mem, &rule_entry, PKT_FLOW_ID_1f);
    flow_id0 = soc_mem_field32_get(unit, mem, &rule_entry, PKT_FLOW_ID_0f);

    /* handle the pkt_type */
    for (ix = 0; ix < BCM_HASH_PKT_TYPE_MAX; ix++) {
        if (pfld_info[ix].fmt == PKT_FLOW_ID_2_FORMATfmt) {
            data32_p = &flow_id2;
        } else if (pfld_info[ix].fmt == PKT_FLOW_ID_1_FORMATfmt) {
            data32_p = &flow_id1;
        } else if (pfld_info[ix].fmt == PKT_FLOW_ID_0_FORMATfmt) {
            data32_p = &flow_id0;
        } else {
            return BCM_E_INTERNAL;
        }
        val2 = 0; 
        val1 = soc_format_field32_get(unit, pfld_info[ix].fmt,
                        data32_p, pfld_info[ix].fld1);
        if (pfld_info[ix].fld2 != INVALIDf) {
            val2 = soc_format_field32_get(unit, pfld_info[ix].fmt,
                 data32_p, pfld_info[ix].fld2);
        }
        if ((val1 == pfld_info[ix].val1) && (val2 == pfld_info[ix].val2)) {
            BCM_HASH_PKT_TYPE_SET_ADD(cfg->tset, pfld_info[ix].pkt_type); 
        }
    }

    /* handle the pkt status */   
    for (iy = 0; iy < BCM_HASH_PKT_STATUS_MAX; iy++) {
        if (sfld_info[iy].fld_size == 1) {
            val1_mask = soc_mem_field32_get(unit, mem, &rule_entry, 
                sfld_info[iy].fld_mask);
            if (val1_mask) {
                BCM_HASH_PKT_STATUS_SET_ADD(cfg->sset_mask, 
                       sfld_info[iy].pkt_status);
            }
            val1 = soc_mem_field32_get(unit, mem, &rule_entry, 
                sfld_info[iy].fld); 
            if (val1) {
                BCM_HASH_PKT_STATUS_SET_ADD(cfg->sset,sfld_info[iy].pkt_status);
            }
        } else {
            return BCM_E_INTERNAL;
        }
    }

    soc_mem_field_get(unit, mem, (uint32 *)&rule_entry, MATCH_IDf, tmp_buf);
    sal_memcpy(&flex_obj.match_id, tmp_buf, 8);

    soc_mem_field_get(unit, mem, (uint32 *)&rule_entry, MATCH_ID_MASKf,
                      tmp_buf);
    sal_memcpy(&flex_obj_mask.match_id, tmp_buf, 8);

    flex_obj_mask.flow_id0 = (uint16)soc_mem_field32_get(unit, mem,
                             &rule_entry, PKT_FLOW_ID_0_MASKf);
    flex_obj.flow_id0 = (uint16)soc_mem_field32_get(unit, mem,
                             &rule_entry, PKT_FLOW_ID_0f);
    flex_obj_mask.flow_id0 = (uint16)soc_mem_field32_get(unit, mem,
                             &rule_entry, PKT_FLOW_ID_0_MASKf);
    flex_obj.flow_id1 = (uint16)soc_mem_field32_get(unit, mem,
                             &rule_entry, PKT_FLOW_ID_1f);
    flex_obj_mask.flow_id1 = (uint16)soc_mem_field32_get(unit, mem,
                             &rule_entry, PKT_FLOW_ID_1_MASKf);
    flex_obj.flow_id2 = (uint16)soc_mem_field32_get(unit, mem,
                             &rule_entry, PKT_FLOW_ID_2f);
    flex_obj_mask.flow_id2 = (uint16)soc_mem_field32_get(unit, mem,
                             &rule_entry, PKT_FLOW_ID_2_MASKf);
    flex_obj.hve_ctrl1 = (uint8)soc_mem_field32_get(unit, mem,
                             &rule_entry, HVE_CONTROL_1f);
    flex_obj_mask.hve_ctrl1 = (uint8)soc_mem_field32_get(unit, mem,
                             &rule_entry, HVE_CONTROL_1_MASKf);
    flex_obj.hve_ctrl2 = (uint8)soc_mem_field32_get(unit, mem,
                             &rule_entry, HVE_CONTROL_2f);
    flex_obj_mask.hve_ctrl2 = (uint8)soc_mem_field32_get(unit, mem,
                             &rule_entry, HVE_CONTROL_2_MASKf);

    rv = _bcm_hash_td3_flex_field_get(unit, num_fields, flex_field,
             &flex_obj, &flex_obj_mask);
    BCM_IF_ERROR_RETURN(rv);
    
    cfg->vfi_ctrl_id_mask = soc_mem_field32_get(unit, mem, &rule_entry,
                                   VFI_HASH_CTRL_ID_MASKf);
    cfg->vfi_ctrl_id = soc_mem_field32_get(unit, mem, &rule_entry,
                                   VFI_HASH_CTRL_IDf);
    cfg->vlan_ctrl_id_mask = soc_mem_field32_get(unit, mem, &rule_entry, 
                                   VLAN_HASH_CTRL_ID_MASKf);
    cfg->vlan_ctrl_id = soc_mem_field32_get(unit, mem, &rule_entry, 
                                   VLAN_HASH_CTRL_IDf);
    cfg->l3iif_ctrl_id_mask = soc_mem_field32_get(unit, mem, &rule_entry, 
                                   L3_IIF_HASH_CTRL_ID_MASKf);
    cfg->l3iif_ctrl_id = soc_mem_field32_get(unit, mem, &rule_entry, 
                                   L3_IIF_HASH_CTRL_IDf);
    cfg->svp_ctrl_id_mask = soc_mem_field32_get(unit, mem, &rule_entry, 
                                   SVP_HASH_CTRL_ID_MASKf);
    cfg->svp_ctrl_id = soc_mem_field32_get(unit, mem, &rule_entry, 
                                   SVP_HASH_CTRL_IDf);
    cfg->sgpp_ctrl_id_mask = soc_mem_field32_get(unit, mem, &rule_entry, 
                                   SGPP_HASH_CTRL_ID_MASKf);
    cfg->sgpp_ctrl_id = soc_mem_field32_get(unit, mem, &rule_entry, 
                                   SGPP_HASH_CTRL_IDf);
 
    /* handle policy data command */
    /* In TD3, the bin_cmd profile is 128 deep same as the TCAM. Use 
     * one-to-one mapping. No need to employ profile mechanism
     */
    /* TD3
     * FLEX_BIN_HASH_PROFILE_0: LSB 8-bit, A-bin 0-12
     * FLEX_BIN_HASH_PROFILE_1: MSB 8-bit, A-bin 0-12
     * FLEX_BIN_HASH_PROFILE_2: LSB 8-bit, B-bin 0-12
     * FLEX_BIN_HASH_PROFILE_3: MSB 8-bit, B-bin 0-12
     */
    for (ix = 0; ix < _BCM_HASH_TD3_NUM_PROF_MEM; ix++) {
        rv = soc_mem_read(unit, prof_info->mem[ix], MEM_BLOCK_ALL, 
                           tcam_inx, prof[ix]);
        SOC_IF_ERROR_RETURN(rv);
    }
    num_bin_max = cfg->option & BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT ?
                  4 * _BCM_HASH_TD3_NUM_BINS_PER_SET:
                  2 * _BCM_HASH_TD3_NUM_BINS_PER_SET;

    for (cmd_inx = 0, ix = 0; ix < num_bin_max; ix++) {

        if (cfg->option & BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT) {
            inx = ix/_BCM_HASH_TD3_NUM_BINS_PER_SET;
        } else {
            inx = 2 * (ix/_BCM_HASH_TD3_NUM_BINS_PER_SET);
        }
        data32 = soc_mem_field32_get(unit, 
             prof_info->mem[inx], prof[inx], 
             prof_info->fld[ix%_BCM_HASH_TD3_NUM_BINS_PER_SET]);

        if (!soc_format_field32_get(unit, FLEX_BIN_HASH_PROFILE_FORMATfmt,
                    &data32, CMD_VALIDf)) {
            continue;
        }
        bin_inx = BIN_INX(unit, inx, tcam_inx, ix);
        bin_cmd[cmd_inx].obj_id = info->bin_obj_arr[bin_inx];
        if (cfg->option & BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT) {
            if (ix < _BCM_HASH_TD3_NUM_BINS_PER_SET) {
                bin_cmd[cmd_inx].bin_id = BCM_HASH_BIN_A_0 + ix;
            } else if (ix < 2*_BCM_HASH_TD3_NUM_BINS_PER_SET) {
                bin_cmd[cmd_inx].bin_id = BCM_HASH_BIN_A_0_UPPER_HALF +
                    ix % _BCM_HASH_TD3_NUM_BINS_PER_SET;
            } else if (ix < 3*_BCM_HASH_TD3_NUM_BINS_PER_SET) {
                bin_cmd[cmd_inx].bin_id = BCM_HASH_BIN_B_0 +
                    ix % _BCM_HASH_TD3_NUM_BINS_PER_SET;
            } else {
                bin_cmd[cmd_inx].bin_id = BCM_HASH_BIN_B_0_UPPER_HALF +
                    ix % _BCM_HASH_TD3_NUM_BINS_PER_SET;
            }
        } else {
            if (ix < _BCM_HASH_TD3_NUM_BINS_PER_SET) {
                bin_cmd[cmd_inx].bin_id = BCM_HASH_BIN_A_0 + ix;
            } else {
                bin_cmd[cmd_inx].bin_id = BCM_HASH_BIN_B_0 + 
                    ix % _BCM_HASH_TD3_NUM_BINS_PER_SET;
            }
        }
        offset_val = soc_format_field32_get(unit, 
                    FLEX_BIN_HASH_PROFILE_FORMATfmt,
                    &data32, CMD_SRC_CONTAINER_OFFSETf);
        switch (bin_cmd[cmd_inx].obj_id) {
            case BCM_HASH_OBJ_OUTER_MAC_DA:
            case BCM_HASH_OBJ_INNER_MAC_DA:
                offset_val -= cmd_offset[BCM_HASH_OBJ_OFFSET_BIT48];
                break;
            case BCM_HASH_OBJ_OUTER_IPV4_SIP:
            case BCM_HASH_OBJ_INNER_IPV4_SIP:
                offset_val -= cmd_offset[BCM_HASH_OBJ_OFFSET_BIT96];
                break;
            default:
                break;
        }
        for (iy = 0; iy < BCM_HASH_OBJ_OFFSET_MAX; iy++) {
            if (cmd_offset[iy] == offset_val) {
                bin_cmd[cmd_inx].obj_offset = iy;
            }
        }

        bin_cmd[cmd_inx].bin_mask = soc_format_field32_get(unit, 
                                    FLEX_BIN_HASH_PROFILE_FORMATfmt,
                                    &data32, CMD_MASKf);
        if (!(cfg->option & BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT)) {
            uint16 upper_mask;

            data32 = soc_mem_field32_get(unit, 
                 prof_info->mem[inx+1], prof[inx+1], 
                 prof_info->fld[ix%_BCM_HASH_TD3_NUM_BINS_PER_SET]);
            upper_mask = soc_format_field32_get(unit, 
                                    FLEX_BIN_HASH_PROFILE_FORMATfmt,
                                    &data32, CMD_MASKf);
            bin_cmd[cmd_inx].bin_mask |= (upper_mask << 8);
        }
        cmd_inx++;
    }
    if (cmd_inx < num_bins) {
        /* mark the end if not all entries are filled */
        bin_cmd[cmd_inx].obj_id = BCM_HASH_OBJ_ID_INVALID;
    } 
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_hash_flex_bin_traverse
 * Purpose:
 *      Traverse all added entries in the TCAM. 
 *   Parameters:
 *      unit      - (IN)  Unit ID.
 *      option    - (IN)  bin_cmd for 8bit data or 16bit. Default 16bit.
 *                        BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT: 8bit
 *      cb        - (IN)  callback function
 *      user_data - (IN)  User context data
 */

int bcmi_td3_hash_flex_bin_traverse(
    int unit,
    uint32 option,
    bcm_hash_flex_bin_traverse_cb cb,
    void *user_data)
{
    int ix,iy;
    int size;
    int rv = BCM_E_NONE;
    bcm_hash_flex_bin_config_t cfg;
    bcm_hash_flex_bin_cmd_t  bin_cmd[4 * _BCM_HASH_TD3_NUM_BINS_PER_SET];
    int numBins;
    bcm_hash_flex_field_t flex_field[40];
    int num_fields;
    int inx;

    num_fields = 40;
    size = soc_mem_index_count(unit, FLEX_RTAG7_HASH_TCAMm);
    if (option & BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT) {
        numBins = 4 * _BCM_HASH_TD3_NUM_BINS_PER_SET;
    } else {
        numBins = 2 * _BCM_HASH_TD3_NUM_BINS_PER_SET;
    }

    for (ix = size - 1; ix >= 0; ix--) {
        bcm_hash_flex_bin_config_t_init(&cfg);
        cfg.option = option;
        cfg.entry_id = ix;
        for (iy = 0; iy < numBins; iy++) {
            bin_cmd[iy].obj_id = BCM_HASH_OBJ_ID_INVALID;
        }
        rv = bcmi_td3_hash_flex_bin_get(unit, &cfg, numBins, bin_cmd,
                 num_fields, flex_field);

        if (rv == BCM_E_NOT_FOUND) { /* for entries not exposed to user */
            rv = BCM_E_NONE;
            continue;
        } else if (BCM_FAILURE(rv)) {
            break;
        }
        /* find the actual number of flex field retrieved */
        for (iy = 0; iy < num_fields; iy++) {
            if (!flex_field[iy].field_id) {
                break;
            }
        }
        /* find the actual number of bin_cmd entries retrieved */
        for (inx = 0; inx < numBins; inx++) {
            if (bin_cmd[inx].obj_id == BCM_HASH_OBJ_ID_INVALID) {
                break;
            }
        }
        rv = cb(unit, &cfg, inx, bin_cmd, iy,
                 iy? flex_field: NULL,
                 user_data);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_hash_td3_flex_field_add
 * Purpose:
 *      Process user inputed flex fields into TD3's Cancun CEH defined flex 
 * objects such as match_id, flow_id and hve_control. Those objects are part
 * of the TCAM's key.
 */

STATIC int _bcm_hash_td3_flex_field_add(
    int unit,
    int num_fields,  /* number of flex fields */
    bcm_hash_flex_field_t *flex_field, /* user provided flex fields */
    _bcm_hash_td3_flex_obj_t *flex_obj, /* Cancun CEH defined flex object */
    _bcm_hash_td3_flex_obj_t *flex_obj_mask) /* flex object's mask */
{
    soc_cancun_ceh_field_info_t fld_info;
    soc_cancun_ceh_field_info_t m_zone;
    char *flex_obj_name;
    uint16 *flow_obj = NULL;
    uint16 *flow_obj_mask = NULL;
    uint8 *hve_obj = NULL;
    uint8 *hve_obj_mask = NULL;
    int match_obj = FALSE;
    int rv;
    uint32 fld_mask;
    uint32 obj;
    int ix;
    int size;
    char *field_name = NULL;
    int op_flag; 
    uint64 tmp64;

    for (ix = 0; ix < num_fields; ix++) {
        match_obj      = FALSE;
        flow_obj       = NULL;
        flow_obj_mask  = NULL;
        hve_obj        = NULL;
        hve_obj_mask   = NULL; 

        obj = FIELD_OBJ_ID(flex_field[ix].field_id);
        flex_obj_name = _bcm_hash_td3_obj_names[obj];
        
        if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE0) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE1) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE2) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE3) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE4) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_FLOW_ID_0) {
            flow_obj = &flex_obj->flow_id0;
            flow_obj_mask = &flex_obj_mask->flow_id0;
        } else if (obj == BCM_HASH_FLEX_OBJ_FLOW_ID_1) {
            flow_obj = &flex_obj->flow_id1;
            flow_obj_mask = &flex_obj_mask->flow_id1;
        } else if (obj == BCM_HASH_FLEX_OBJ_FLOW_ID_2) {
            flow_obj = &flex_obj->flow_id2;
            flow_obj_mask = &flex_obj_mask->flow_id2;
        } else if (obj == BCM_HASH_FLEX_OBJ_HVE_CONTROL_1) {
            hve_obj = &flex_obj->hve_ctrl1;
            hve_obj_mask = &flex_obj_mask->hve_ctrl1;
        } else if (obj == BCM_HASH_FLEX_OBJ_HVE_CONTROL_2) {
            hve_obj = &flex_obj->hve_ctrl2;
            hve_obj_mask = &flex_obj_mask->hve_ctrl2;
        } else {
            rv = BCM_E_UNAVAIL;
            goto exit;
        }

        /* get the field_name size first */
        rv = soc_cancun_ceh_obj_field_name_get (unit,
                 FIELD_NAME_ID(flex_field[ix].field_id),
                 &size, NULL);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }

        field_name = sal_alloc(size, "td3_hash_flex_temp_buffer");
        if (field_name == NULL) {
            rv = BCM_E_MEMORY;
            goto exit;
        }

        /* get the field_name */
        rv = soc_cancun_ceh_obj_field_name_get (unit,
                 FIELD_NAME_ID(flex_field[ix].field_id),
                 &size, field_name);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }

        rv = soc_cancun_ceh_obj_field_get(unit,flex_obj_name, 
                 field_name, &fld_info);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }

        /* The Cancun defined Match_id overall width 50 bit. divided
         * into 5 zone.
         */

        if (match_obj) {
            uint64 m_id;
            uint64 m_id_mask;

            COMPILER_64_ZERO(m_id);
            COMPILER_64_ZERO(m_id_mask);
            /* Retrieve the match zone value and mask */
            rv = soc_cancun_ceh_obj_field_get(unit,"MATCH_ID",flex_obj_name, 
                     &m_zone);
            if (BCM_FAILURE(rv)) {
                goto exit;
            }
            fld_mask = (1 << fld_info.width) - 1;
            fld_mask = fld_mask << fld_info.offset;
            COMPILER_64_SET(m_id_mask, 0, fld_mask);
            COMPILER_64_SHL(m_id_mask, m_zone.offset);
            COMPILER_64_SET(m_id, 0, fld_info.value);
            COMPILER_64_SHL(m_id, fld_info.offset);
            COMPILER_64_SHL(m_id, m_zone.offset);

            if (flex_field[ix].mask == 0 || flex_field[ix].value == 0) {
                op_flag = 0;
            } else {
                op_flag = 1;
            }

            /* add field, check if field collision: mask overlap */
            if (op_flag) {
                COMPILER_64_COPY(tmp64, m_id_mask);
                COMPILER_64_AND(tmp64, flex_obj_mask->match_id);
                if (!COMPILER_64_IS_ZERO(tmp64)) {
                    LOG_ERROR(BSL_LS_BCM_HASH, (BSL_META_U(unit,
                        "Flex field conflict: %s\n"), field_name));
                    rv = BCM_E_PARAM;
                    goto exit;
                }
                COMPILER_64_OR(flex_obj_mask->match_id, m_id_mask);
                COMPILER_64_OR(flex_obj->match_id, m_id);
            } else {
                /* clear the field */
                COMPILER_64_NOT(m_id_mask);
                COMPILER_64_AND(flex_obj_mask->match_id, m_id_mask);
                COMPILER_64_AND(flex_obj->match_id, m_id_mask);
            }
        } else if (flow_obj != NULL) {

            if (flex_field[ix].mask == 0 || flex_field[ix].value == 0) {
                op_flag = 0;
            } else {
                op_flag = 1;
            }

            fld_mask = (1 << fld_info.width) - 1;
            fld_mask = fld_mask << fld_info.offset;
            if (op_flag) {
                if (fld_mask & *flow_obj_mask) {
                    LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                        "Flex field conflict: %s\n"), field_name));
                    rv = BCM_E_PARAM;
                    goto exit;
                }
                *flow_obj &= (uint16)(~fld_mask); 
                *flow_obj |= (uint16)((fld_info.value << fld_info.offset) & 
                          fld_mask);
                *flow_obj_mask |= fld_mask; 
            } else {
                *flow_obj      &= (uint16)(~fld_mask); 
                *flow_obj_mask &= (uint16)(~fld_mask); 
            }
        } else if (hve_obj != NULL) {
            fld_mask = (1 << fld_info.width) - 1;
            fld_mask = fld_mask << fld_info.offset;
            if (flex_field[ix].mask == 0) {
                *hve_obj_mask &= (uint8)(~fld_mask);
                *hve_obj      &= (uint8)(~fld_mask);
            } else {
                *hve_obj_mask |= (uint8)(fld_mask);
                *hve_obj |= (uint8)((flex_field[ix].value << fld_info.offset) &
                               fld_mask);
            }
        } else {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        sal_free(field_name);
        field_name = NULL;
    }
    return BCM_E_NONE;

exit:
    if (field_name != NULL) {
        sal_free(field_name);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_hash_td3_flex_field_get
 * Purpose:
 *      Retrieve data from TD3's Cancun CEH defined flex objects such as
 * match_id, flow_id and hve_control. And fill the flex_field buffer array
 * provided by user.
 */

STATIC int _bcm_hash_td3_flex_field_get(
    int unit,
    int num_fields,
    bcm_hash_flex_field_t *flex_field,
    _bcm_hash_td3_flex_obj_t *flex_obj,
    _bcm_hash_td3_flex_obj_t *flex_obj_mask)
{
    soc_cancun_ceh_field_info_t fld_info;
    soc_cancun_ceh_field_info_t m_zone;
    char *flex_obj_name;
    uint16 *flow_obj = NULL;
    uint16 *flow_obj_mask = NULL;
    uint8 *hve_obj = NULL;
    uint8 *hve_obj_mask = NULL;
    int match_obj = FALSE;
    int rv;
    uint32 fld_mask;
    bcm_hash_flex_object_t obj;
    int ix;
    int size;
    char *field_name = NULL;
    uint32 *id_arr = NULL;
    int inx;
    int fld_size;
    uint32 mz_mask;
    uint32 fld_value;
    int buf_limit = FALSE;
    uint64 m_id;
    uint64 m_id_mask;
    uint64 tmp64;
    uint32 tmp32;

    COMPILER_64_ZERO(m_id);
    COMPILER_64_ZERO(m_id_mask);
    inx = 0;
    for (obj = BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE0; obj < BCM_HASH_FLEX_OBJ_MAX;
         obj++) {

        match_obj      = FALSE;
        flow_obj       = NULL;
        flow_obj_mask  = NULL;
        hve_obj        = NULL;
        hve_obj_mask   = NULL; 
        if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE0) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE1) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE2) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE3) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE4) {
            match_obj = TRUE;
        } else if (obj == BCM_HASH_FLEX_OBJ_FLOW_ID_0) {
            flow_obj = &flex_obj->flow_id0;
            flow_obj_mask = &flex_obj_mask->flow_id0;
        } else if (obj == BCM_HASH_FLEX_OBJ_FLOW_ID_1) {
            flow_obj = &flex_obj->flow_id1;
            flow_obj_mask = &flex_obj_mask->flow_id1;
        } else if (obj == BCM_HASH_FLEX_OBJ_FLOW_ID_2) {
            flow_obj = &flex_obj->flow_id2;
            flow_obj_mask = &flex_obj_mask->flow_id2;
        } else if (obj == BCM_HASH_FLEX_OBJ_HVE_CONTROL_1) {
            hve_obj = &flex_obj->hve_ctrl1;
            hve_obj_mask = &flex_obj_mask->hve_ctrl1;
        } else { /* (obj == BCM_HASH_FLEX_OBJ_HVE_CONTROL_2) */
            hve_obj = &flex_obj->hve_ctrl2;
            hve_obj_mask = &flex_obj_mask->hve_ctrl2;
        }

        flex_obj_name = _bcm_hash_td3_obj_names[obj];
        rv = soc_cancun_ceh_obj_field_id_list_get(unit, flex_obj_name, &size,
                 NULL);
        if (BCM_FAILURE(rv)) {
            if (rv == SOC_E_NOT_FOUND) {
                continue;
            } else {
                goto exit;
            }
        }
   
        id_arr = sal_alloc(size * sizeof(uint32), "td3_hash_flex_temp_buffer");
        if (id_arr == NULL) {
            rv = BCM_E_MEMORY;
            goto exit;
        }

        rv = soc_cancun_ceh_obj_field_id_list_get(unit, flex_obj_name, &size,
                 id_arr);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }

        if (match_obj) {
            COMPILER_64_ZERO(m_id);
            COMPILER_64_ZERO(m_id_mask);

            /* Retrieve the match zone value and mask */
            rv = soc_cancun_ceh_obj_field_get(unit,"MATCH_ID",
                     flex_obj_name, &m_zone);
            if (BCM_FAILURE(rv)) {
                goto exit;
            }
            COMPILER_64_COPY(m_id_mask, flex_obj_mask->match_id);
            COMPILER_64_SHR(m_id_mask, m_zone.offset);
            COMPILER_64_COPY(m_id, flex_obj->match_id);
            COMPILER_64_SHR(m_id, m_zone.offset);
            mz_mask = (1 << m_zone.width) - 1;
            COMPILER_64_SET(tmp64, 0, mz_mask);
            COMPILER_64_AND(m_id_mask, tmp64);
            COMPILER_64_AND(m_id, tmp64);
        }

        for (ix = 0; ix < size; ix++) {
            /* get the field_name size first */
            rv = soc_cancun_ceh_obj_field_name_get (unit,
                     id_arr[ix], &fld_size, NULL);
            if (BCM_FAILURE(rv)) {
                goto exit;
            }

            field_name = sal_alloc(fld_size, "td3_hash_flex_temp_buffer");
            if (field_name == NULL) {
                rv = BCM_E_MEMORY;
                goto exit;
            }

            /* get the field_name */
            rv = soc_cancun_ceh_obj_field_name_get (unit,
                     id_arr[ix], &fld_size, field_name);
            if (BCM_FAILURE(rv)) {
                goto exit;
            }

            rv = soc_cancun_ceh_obj_field_get(unit,flex_obj_name, 
                     field_name, &fld_info);
            if (BCM_FAILURE(rv)) {
                goto exit;
            }

            /* The Cancun defined Match_id overall width 50 bit. divided
             * into 5 zone.
             */

            if (match_obj) {
                fld_mask = (1 << fld_info.width) - 1;
                fld_mask = fld_mask << fld_info.offset;
                COMPILER_64_TO_32_LO(tmp32, m_id_mask);
                if ((fld_mask & tmp32) != fld_mask) {
                    sal_free(field_name);
                    field_name = NULL;
                    continue;
                }
                fld_value = fld_info.value << fld_info.offset;
                COMPILER_64_TO_32_LO(tmp32, m_id);
                if (fld_value != (tmp32 & fld_mask)) {
                    sal_free(field_name);
                    field_name = NULL;
                    continue;
                }
                /* found one. remove the mask */
                COMPILER_64_SET(tmp64, 0, ~fld_mask);
                COMPILER_64_AND(m_id_mask, tmp64);

                if (num_fields <= inx) {
                    buf_limit = TRUE;
                    break;
                }
                flex_field[inx].field_id = FIELD_OBJ_ID_SET(id_arr[ix], obj);
                flex_field[inx].mask     = 1;
                flex_field[inx].value    = 1;
                inx++;
            } else if (flow_obj_mask != NULL) {

                fld_mask = (1 << fld_info.width) - 1;
                fld_mask = fld_mask << fld_info.offset;
                if ((fld_mask & (*flow_obj_mask)) != fld_mask) {
                    sal_free(field_name);
                    field_name = NULL;
                    continue;
                }
                fld_value = fld_info.value << fld_info.offset;
                if (fld_value != ((*flow_obj) & fld_mask)) {
                    sal_free(field_name);
                    field_name = NULL;
                    continue;
                }
                /* found one. remove the mask */
                *flow_obj_mask = (~fld_mask) & (*flow_obj_mask);

                if (num_fields <= inx) {
                    buf_limit = TRUE;
                    break;
                }
                flex_field[inx].field_id = FIELD_OBJ_ID_SET(id_arr[ix], obj);
                flex_field[inx].mask     = 1;
                flex_field[inx].value    = 1;
                inx++;
            } else if (hve_obj_mask != NULL) {
                fld_mask = (1 << fld_info.width) - 1;
                fld_mask = fld_mask << fld_info.offset;
                if ((fld_mask & (*hve_obj_mask)) != fld_mask) {
                    sal_free(field_name);
                    field_name = NULL;
                    continue;
                }
                /* found one. remove the mask */
                *hve_obj_mask = (~fld_mask) & (*hve_obj_mask);

                if (num_fields <= inx) {
                    buf_limit = TRUE;
                    break;
                }
                flex_field[inx].field_id = FIELD_OBJ_ID_SET(id_arr[ix], obj);
                flex_field[inx].mask     = 1;
                if ((*hve_obj) & fld_mask) {
                    flex_field[inx].value    = 1;
                } else {
                    flex_field[inx].value    = 0;
                }
                inx++;
            } else {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
            sal_free(field_name);
            field_name = NULL;
        }
        if (field_name != NULL) {
            sal_free(field_name);
            field_name = NULL;
        }
        sal_free(id_arr);
        id_arr = NULL;
        if (buf_limit == TRUE) {
            break;
        }
    }
    /* clear the remaining buffer */
    for (ix = inx; ix < num_fields; ix++) {
        flex_field[inx].field_id = 0;
        flex_field[inx].mask     = 0;
        flex_field[inx].value    = 0;
    }
    return BCM_E_NONE;

exit:
    if (id_arr != NULL) {
        sal_free(id_arr);
    }
    if (field_name != NULL) {
        sal_free(field_name);
    }
    return rv;
}

#endif  /* #if defined(BCM_TRIDENT3_SUPPORT) */
