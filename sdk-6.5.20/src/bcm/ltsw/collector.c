/*
 *BCM level APIs for Collector.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/collector_int.h>
#include <bcm/types.h>
#include <bcm/collector.h>

#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <shr/shr_bitop.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcm_int/ltsw/util.h>

#define BSL_LOG_MODULE BSL_LS_BCM_COLLECTOR

/* Collector control db. */
typedef struct ltsw_collector_info_s {
    /* Collector module initialized flag. */
    int initialized;

    /* Collector module lock. */
    sal_mutex_t lock;
} ltsw_collector_info_t;

static ltsw_collector_info_t ltsw_collector_info[BCM_MAX_NUM_UNITS];

#define COLLECTOR_INFO(unit) (&ltsw_collector_info[unit])

#define COLLECTOR_INIT_CHECK(unit) \
    do { \
        if (COLLECTOR_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define COLLECTOR_LOCK(unit) \
    do { \
        if (COLLECTOR_INFO(unit)->lock) { \
            sal_mutex_take(COLLECTOR_INFO(unit)->lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define COLLECTOR_UNLOCK(unit) \
    do { \
        if (COLLECTOR_INFO(unit)->lock) { \
            sal_mutex_give(COLLECTOR_INFO(unit)->lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

static bcmint_collector_bookkeeping_info_t collector_bk[BCM_MAX_NUM_UNITS];

#define COL_BK(unit) collector_bk[unit].collector_info

#define COL_IPV4_ID_BMP(unit)               \
    (COL_BK(unit).ipv4_id_bitmap)
#define COL_IPV4_ID_BMP_SET(unit, id)       \
    SHR_BITSET(COL_IPV4_ID_BMP(unit),id)
#define COL_IPV4_ID_BMP_GET(unit, id)       \
    SHR_BITGET(COL_IPV4_ID_BMP(unit),id)
#define COL_IPV4_ID_BMP_CLEAR(unit, id)     \
    SHR_BITCLR(COL_IPV4_ID_BMP(unit),id)


#define COL_IPV6_ID_BMP(unit)               \
    (COL_BK(unit).ipv6_id_bitmap)
#define COL_IPV6_ID_BMP_SET(unit, id)       \
    SHR_BITSET(COL_IPV6_ID_BMP(unit),id)
#define COL_IPV6_ID_BMP_GET(unit, id)       \
    SHR_BITGET(COL_IPV6_ID_BMP(unit),id)
#define COL_IPV6_ID_BMP_CLEAR(unit, id)     \
    SHR_BITCLR(COL_IPV6_ID_BMP(unit),id)


#define COL_EXT_ID_BMP(unit)               \
    (COL_BK(unit).externel_id_bitmap)
#define COL_EXT_ID_BMP_SET(unit, id)       \
    SHR_BITSET(COL_EXT_ID_BMP(unit),id)
#define COL_EXT_ID_BMP_GET(unit, id)       \
    SHR_BITGET(COL_EXT_ID_BMP(unit),id)
#define COL_EXT_ID_BMP_CLEAR(unit, id)     \
    SHR_BITCLR(COL_EXT_ID_BMP(unit),id)
#define COL_EXT_ID_ITER(unit, id)    \
    SHR_BIT_ITER(COL_EXT_ID_BMP(unit), _COLLECTOR_ID_MAX, id)



#define COL_REF_COUNT(unit, id)             \
    (*((COL_BK(unit).ref_count) + id))

#define COL_USER_UPDATE(unit,id)            \
    (*((COL_BK(unit).user) + id))

/*!
 * Mapping of collector ID to transport type and LT IDs.
 * Managed as two dimensional array [ID][TYPE]
 */
#define COL_ID_TO_TYPE_MAP(unit)            \
    (COL_BK(unit).id_to_type_map)

/* Mapping of collector ID to LT IDs. */
#define COL_INT_ID_MAP(unit, id)            \
    (*(COL_ID_TO_TYPE_MAP(unit) + (id * 2)))

/* Mapping of collector ID to transport type. */
#define COL_TYPE_MAP(unit, id)              \
    (*(COL_ID_TO_TYPE_MAP(unit) + (id * 2)+ 1))

#define EXPORT_BK(unit) collector_bk[unit].export_profile_info

#define EXPORT_ID_BMP(unit)             \
    EXPORT_BK(unit).export_profile_id_bmp

#define EXPORT_ID_BMP_SET(unit,id)      \
    SHR_BITSET(EXPORT_ID_BMP(unit),id)


#define EXPORT_ID_BMP_GET(unit,id)      \
    SHR_BITGET(EXPORT_ID_BMP(unit),id)


#define EXPORT_ID_BMP_CLEAR(unit,id)    \
    SHR_BITCLR(EXPORT_ID_BMP(unit),id)

#define EXPORT_REF_COUNT(unit,id)       \
    (*((EXPORT_BK(unit).ref_count) + id))



/******************************************************************************
 * Private functions
 */

/*!
 * \brief De-initialize the Collector module.
 *
 * Free Collector module resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_collector_detach(int unit)
{
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    if (COLLECTOR_INFO(unit)->initialized == 0) {
        SHR_EXIT();
    }

    COLLECTOR_LOCK(unit);
    lock = 1;

    COLLECTOR_INFO(unit)->initialized = 0;

exit:
    if (lock == 1) {
        COLLECTOR_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();


}

/*!
 * \brief Initialize the Collector module.
 *
 * Initialize Collector database and Collector related logical tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */

int
bcm_ltsw_collector_init(int unit)
{
    uint32_t ha_req_size, ha_alloc_size;
    uint32_t export_profile_id_bmp_size;
    uint32_t export_ref_count_size;
    uint32_t collector_id_bitmap_size;
    uint32_t externel_id_bitmap_size;
    uint32_t id_to_type_map_size;
    uint32_t collector_ref_count_size;
    uint32_t user_size;

    void *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (COLLECTOR_INFO(unit)->initialized) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_collector_detach(unit));
    }

    if (COLLECTOR_INFO(unit)->lock == NULL) {
        COLLECTOR_INFO(unit)->lock = sal_mutex_create("bcmLtswCollectorMutex");
        SHR_NULL_CHECK(COLLECTOR_INFO(unit)->lock, SHR_E_MEMORY);
    }
    export_profile_id_bmp_size = SHR_BITALLOCSIZE(_EXPORT_PROFILE_ID_MAX);
    export_ref_count_size = sizeof(int) * _EXPORT_PROFILE_ID_MAX;

    collector_id_bitmap_size = SHR_BITALLOCSIZE(_COLLECTOR_ID_MAX_PER_TYPE);
    externel_id_bitmap_size = SHR_BITALLOCSIZE(_COLLECTOR_ID_MAX);
    id_to_type_map_size = sizeof(int) * _COLLECTOR_ID_MAX * 2;
    collector_ref_count_size = _COLLECTOR_ID_MAX * sizeof(int);
    user_size = _COLLECTOR_ID_MAX * sizeof(int);

    ha_req_size = export_profile_id_bmp_size + export_ref_count_size + (collector_id_bitmap_size * 2) +
                    externel_id_bitmap_size + id_to_type_map_size + collector_ref_count_size + user_size;
    ha_alloc_size = ha_req_size;

    ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_COLLECTOR, 0, "bcmCollector", &ha_alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    if (!bcmi_warmboot_get(unit)) {
        sal_memset(ptr, 0, ha_req_size);
    }

    collector_bk[unit].export_profile_info.export_profile_id_bmp = ptr;
    collector_bk[unit].export_profile_info.ref_count = ptr + export_profile_id_bmp_size;
    ptr = collector_bk[unit].export_profile_info.ref_count;
    collector_bk[unit].collector_info.ipv4_id_bitmap = ptr + export_ref_count_size;
    ptr = collector_bk[unit].collector_info.ipv4_id_bitmap;
    collector_bk[unit].collector_info.ipv6_id_bitmap = ptr + collector_id_bitmap_size;
    ptr = collector_bk[unit].collector_info.ipv6_id_bitmap;
    collector_bk[unit].collector_info.externel_id_bitmap = ptr + collector_id_bitmap_size;
    ptr = collector_bk[unit].collector_info.externel_id_bitmap;
    collector_bk[unit].collector_info.id_to_type_map = ptr + externel_id_bitmap_size;
    ptr = collector_bk[unit].collector_info.id_to_type_map;
    collector_bk[unit].collector_info.ref_count = ptr + id_to_type_map_size;
    ptr = collector_bk[unit].collector_info.ref_count;
    collector_bk[unit].collector_info.user =  ptr + collector_ref_count_size;

    COLLECTOR_INFO(unit)->initialized = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (COLLECTOR_INFO(unit)->lock != NULL) {
            sal_mutex_destroy(COLLECTOR_INFO(unit)->lock);
            COLLECTOR_INFO(unit)->lock = NULL;
        }
    }

    SHR_FUNC_EXIT();
}


/*!
 * Fucntion : bcm_ltsw_export_profile_create
 *
 * Purpose:
 *      Creates a collector export profile with given export profile info.
 *
 *Parameters:
 *      unit                 - (IN) BCM device number
 *      options              - (IN)  Collector export profile create options
 *      id                   - (INOUT)  Collector Id
 *      export_profile_info  - (IN)  Collector export profile info
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */

int
bcm_ltsw_collector_export_profile_create(
    int unit,
    uint32_t options,
    int *id,
    bcm_collector_export_profile_t *export_profile_info) {
	int index = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bool id_found = FALSE;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);

    if ((options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE) &&
       !(options & BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID)) {
        return BCM_E_PARAM;
    }

    /* WITH_ID */
    if (!(options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE)) {
        if (options & BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID) {
            if (EXPORT_ID_BMP_GET(unit, *id)) {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                    (BSL_META_U(unit,
                        "Collector(unit %d) Error: Export Profile ID is already in use\n"), unit));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            id_found = TRUE;
        } else {
            for (index = 0; index < _EXPORT_PROFILE_ID_MAX; index++) {
                if (!EXPORT_ID_BMP_GET(unit, index)) {
                    id_found = TRUE;
                    *id = index;
                    break;
                }
            }
        }
        if (id_found == FALSE) {
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                    "Collector(unit %d) Error: All Export Profile IDs are in use\n"), unit));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if ((options & BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID)  &&
        (options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE)) {
        if (!EXPORT_ID_BMP_GET(unit, *id)) {
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                    (BSL_META_U(unit,
                        "Collector(unit %d) Error: Export Profile ID is already in use\n"), unit));
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_EXPORT_PROFILEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_EXPORT_PROFILE_IDs, *id));


    if (export_profile_info->wire_format == bcmCollectorWireFormatIpfix) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, WIRE_FORMATs, "IPFIX"));
    } else if (export_profile_info->wire_format == bcmCollectorWireFormatProtoBuf) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, WIRE_FORMATs, "PROTOBUF"));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, INTERVALs, export_profile_info->export_interval));

    if (export_profile_info->flags & BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, PACKET_LEN_INDICATORs,
                                          "NUM_RECORDS"));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, NUM_RECORDSs,
                                   export_profile_info->num_records));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, PACKET_LEN_INDICATORs,
                                          "MAX_PKT_LENGTH"));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MAX_PKT_LENGTHs,
                                   export_profile_info->max_packet_length));
    }
    if (id_found == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));
        EXPORT_ID_BMP_SET(unit, *id);
    }

    if ((options & BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID)  &&
        (options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL));
    }

    COLLECTOR_UNLOCK(unit);

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_collector_export_profile_get
 * Purpose:
 *      Gets a collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      id              - (IN)  Collector Id
 *      export_profile_info  - (OUT) Collector export profile info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_ltsw_collector_export_profile_get(int unit,
    int id,
    bcm_collector_export_profile_t *export_profile_info) {

    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    const char* symbol = NULL;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);

    if (!EXPORT_ID_BMP_GET(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
        (BSL_META_U(unit,
                    "Collector(unit %d) Error: Export Profile ID does not exist\n"), unit));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_EXPORT_PROFILEs, &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, MON_EXPORT_PROFILE_IDs, id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry, WIRE_FORMATs, &symbol));

    if (!sal_strcmp(symbol, "IPFIX")) {
        export_profile_info->wire_format = bcmCollectorWireFormatIpfix;
    } else if (!sal_strcmp(symbol, "PROTOBUF")) {
        export_profile_info->wire_format = bcmCollectorWireFormatProtoBuf;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry, INTERVALs, (uint64_t *)&export_profile_info->export_interval));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry, PACKET_LEN_INDICATORs, &symbol));

    if (!sal_strcmp(symbol, "MAX_PKT_LENGTH")) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry, MAX_PKT_LENGTHs, (uint64_t *)&export_profile_info->max_packet_length));
    } else if (!sal_strcmp(symbol, "NUM_RECORDS")) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry, NUM_RECORDSs, (uint64_t *)&export_profile_info->num_records));
    }
    COLLECTOR_UNLOCK(unit);

exit:
    if (BCMLT_INVALID_HDL != entry) {
        SHR_IF_ERR_EXIT(bcmlt_entry_free(entry));
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_collector_export_profile_ids_get_all
 * Purpose:
 *      Get the ID list of all collectors configured.
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      max_size             - (IN)  Size of the collector list array
 *      collector_list       - (OUT) Collector id list
 *      list_szie            - (OUT) NUmber of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_collector_export_profile_ids_get_all(
                           int unit,
                           int max_size,
                           int *collector_list,
                           int *list_size)
{
	int i;
    int num_export_profiles = 0;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);

    if (max_size != 0 && collector_list == NULL) {
        return SHR_E_PARAM;
    }

    for (i = 0; i < _EXPORT_PROFILE_ID_MAX; i++) {
        if (EXPORT_ID_BMP_GET(unit, i)) {
            num_export_profiles++;
            if (*list_size < max_size) {
                if(max_size != 0) {
                    collector_list[*list_size] = i;
                }
                (*list_size)++;
            }
        }
    }

    /* If the max_size is 0 then return the number of collector export profiles
     * configured in list_size
     */
    if (max_size == 0) {
        *list_size = num_export_profiles;
    }

    COLLECTOR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *      bcm_ltsw_collector_export_profile_destroy
 * Purpose:
 *      Destroys a collector
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      id             - (IN) Collector expor profile Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_collector_export_profile_destroy(int unit,
                                 int id)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);

    if (EXPORT_REF_COUNT(unit,id) != 0) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
        (BSL_META_U(unit,
                    "Collector(unit %d) Error: Export Profile is in use\n"), unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_EXPORT_PROFILEs, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry, MON_EXPORT_PROFILE_IDs, id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_DELETE,BCMLT_PRIORITY_NORMAL));

    EXPORT_ID_BMP_CLEAR(unit, id);

    COLLECTOR_UNLOCK(unit);

exit:
    if (BCMLT_INVALID_HDL != entry) {
        SHR_IF_ERR_EXIT(bcmlt_entry_free(entry));
    }
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *      bcm_ltsw_collector_info_t_init
 * Purpose:
 *      Initialize a collector information structure.
 * Parameters:
 *      collector_info - (IN)  collector info structure
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void bcm_ltsw_collector_info_t_init(bcm_collector_info_t *collector_info)
{
    sal_memset(collector_info, 0, sizeof(bcm_collector_info_t));
}

/*
 * Function:
 *      bcm_ltsw_collector_export_profile_t_init
 * Purpose:
 *      Initialize a export profile structure.
 * Parameters:
 *      export_profile_info - (IN)  export profile info structure
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void bcm_ltsw_collector_export_profile_t_init(bcm_collector_export_profile_t *export_profile_info)
{
    sal_memset(export_profile_info, 0, sizeof(bcm_collector_export_profile_t));
	export_profile_info->export_interval = BCM_COLLECTOR_EXPORT_INTERVAL_INVALID;

}

/*
 * Function:
 *      bcm_ltsw_collector_callback_options_t_init
 * Purpose:
 *      Initialize a collector callback options structure.
 * Parameters:
 *      callback_options - (IN)  callback options
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void bcm_ltsw_collector_callback_options_t_init(bcm_collector_callback_options_t *callback_options)
{
	sal_memset(callback_options, 0, sizeof(bcm_collector_callback_options_t));
}

/*
 * Function:
 *      bcm_ltsw_collector_export_record_t_init
 * Purpose:
 *      Initialize a export record structure.
 * Parameters:
 *      record - (IN) export record structure
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
void bcm_ltsw_collector_export_record_t_init(bcm_collector_export_record_t *record)
{
	sal_memset(record, 0, sizeof(bcm_collector_export_record_t));
}

/*
 * Function:
 *      bcm_ltsw_collector_create
 * Purpose:
 *      Createa a collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector create options
 *      id              - (INOUT)  Collector Id
 *      collector_info  - (IN)  Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_collector_create(int unit,
                                uint32 options,
                                bcm_collector_t *id,
                                bcm_collector_info_t *collector_info)
{
    int external_id = 0, internal_id = 0, i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bool id_found = FALSE;
    uint64_t mac = 0, tmp_64_val;
    uint64_t src_ipv6_lower = 0, src_ipv6_upper = 0, dst_ipv6_lower = 0, dst_ipv6_upper = 0;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);

    if ((options & BCM_COLLECTOR_EXPORT_PROFILE_REPLACE) &&
       !(options & BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID)) {
        return BCM_E_PARAM;
    }

    /* Validate ETH header */
    switch (collector_info->eth.vlan_tag_structure) {
        case BCM_COLLECTOR_ETH_HDR_UNTAGGED:
            /* Nothing to validate */
            break;

        case BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED:
            if (collector_info->eth.outer_vlan_tag > BCM_VLAN_MAX) {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                          (BSL_META_U(unit,
                                      "COLLECTOR(unit %d) Error: Invalid outer_vlan_tag=%d\n"),
                           unit, collector_info->eth.outer_vlan_tag));
                return BCM_E_PARAM;
            }
            break;

        case BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED:
            if ((collector_info->eth.outer_vlan_tag > BCM_VLAN_MAX) ||
                (collector_info->eth.inner_vlan_tag > BCM_VLAN_MAX)) {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                          (BSL_META_U(unit,
                                      "COLLECTOR(unit %d) Error: Invalid outer_vlan_tag(%d)"
                                      " or inner_vlan_tag(%d) \n"),
                           unit, collector_info->eth.outer_vlan_tag,
                           collector_info->eth.inner_vlan_tag));
                return BCM_E_PARAM;
            }
            break;

        default:
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                      (BSL_META_U(unit,
                                  "COLLECTOR(unit %d) Error: Invalid vlan_tag_structure=%d\n"),
                       unit, collector_info->eth.vlan_tag_structure));
            return BCM_E_PARAM;
    }

    /* Check collector transport type is valid or not */
    if ((collector_info->transport_type < bcmCollectorTransportTypeIpv4Udp) ||
        (collector_info->transport_type >= bcmCollectorTransportTypeCount)) {
        return BCM_E_PARAM;
    }

    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        if((collector_info->ipv4.src_ip == 0) ||
           (collector_info->ipv4.dst_ip == 0))
        {
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                  (BSL_META_U(unit,
                  "COLLECTOR(unit %d) Error: Invalid IP configuration"), unit));
            return BCM_E_PARAM;
        }
        if((collector_info->udp.src_port == 0) ||
           (collector_info->udp.dst_port == 0))
        {
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                 (BSL_META_U(unit,
                 "COLLECTOR(unit %d) Error: Invalid UDP configuration"), unit));
            return BCM_E_PARAM;
        }
    }

    /* WITH_ID */
    if (!(options & BCM_COLLECTOR_REPLACE)) {
        if (options & BCM_COLLECTOR_WITH_ID) {
            /* Check the global array if the id is available .*/
            if (COL_EXT_ID_BMP_GET(unit, *id)) {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                    (BSL_META_U(unit,
                        "Collector(unit %d) Error: Collector ID is already in use\n"), unit));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            external_id = *id;
        } else {
            for (external_id = 0; external_id < _COLLECTOR_ID_MAX; external_id++) {
                if (!COL_EXT_ID_BMP_GET(unit, external_id)) {
                    id_found = TRUE;
                    *id = external_id;
                    break;
                }
            }

            if (id_found == FALSE) {
                LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                    (BSL_META_U(unit,
                    "Collector(unit %d) Error: All Collector IDs are in use\n"), unit));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        /* Reserve the index in respective transport type LT. */
        if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
            for (internal_id = 0; internal_id < _COLLECTOR_ID_MAX_PER_TYPE; internal_id++) {
                if (!COL_IPV4_ID_BMP_GET(unit, internal_id)) {
                    COL_TYPE_MAP(unit, external_id) = LTSW_COLLECTOR_TYPE_IPV4_UDP;
                    COL_INT_ID_MAP(unit, external_id) = internal_id;
                    COL_IPV4_ID_BMP_SET(unit, internal_id);
                    id_found = TRUE;
                    break;
                }
            }
        }
        if (collector_info->transport_type == bcmCollectorTransportTypeIpv6Udp) {
            for (internal_id = 0; internal_id < _COLLECTOR_ID_MAX_PER_TYPE; internal_id++) {
                if (!COL_IPV6_ID_BMP_GET(unit, internal_id)) {
                    COL_TYPE_MAP(unit, external_id) = LTSW_COLLECTOR_TYPE_IPV6_UDP;
                    COL_INT_ID_MAP(unit, external_id) = internal_id;
                    COL_IPV6_ID_BMP_SET(unit, internal_id);
                    id_found = TRUE;
                    break;
                }
            }
        }
        COL_EXT_ID_BMP_SET(unit, external_id);
    }

    /* Collector update */
    if ((options & BCM_COLLECTOR_WITH_ID) && (options & BCM_COLLECTOR_REPLACE)) {
        if (!COL_EXT_ID_BMP_GET(unit, *id)) {
            LOG_ERROR(BSL_LS_BCM_COLLECTOR,
                (BSL_META_U(unit,
                "Collector(unit %d) Error: Collector ID does not exist\n"), unit));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(unit, MON_COLLECTOR_IPV4s, &entry_hdl));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV4_IDs, internal_id));
    } else if (collector_info->transport_type == bcmCollectorTransportTypeIpv6Udp){
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, MON_COLLECTOR_IPV6s, &entry_hdl));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV6_IDs, internal_id));

    }

    /* WITH_ID and replace, lookup the entry */
    if ((options & BCM_COLLECTOR_WITH_ID)  && (options & BCM_COLLECTOR_REPLACE)) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    }
    if (collector_info->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_UNTAGGED) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, TAG_STRUCTUREs, "UNTAGGED"));
    } else if (collector_info->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, TAG_STRUCTUREs, "SINGLE_TAGGED"));
    } else if (collector_info->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, TAG_STRUCTUREs, "DOUBLE_TAGGED"));
    }
    bcmi_ltsw_util_mac_to_uint64(&mac, collector_info->eth.dst_mac);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_MACs, mac));

    bcmi_ltsw_util_mac_to_uint64(&mac, collector_info->eth.src_mac);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_MACs, mac));

     if (collector_info->eth.vlan_tag_structure != BCM_COLLECTOR_ETH_HDR_UNTAGGED) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, collector_info->eth.outer_vlan_tag));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, TPIDs, collector_info->eth.outer_tpid));
    }

    if (collector_info->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_TPIDs, collector_info->eth.inner_tpid));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_VLAN_IDs, collector_info->eth.inner_vlan_tag));
    }

    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, TOSs, collector_info->ipv4.dscp));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, TTLs, collector_info->ipv4.ttl));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, SRC_IPV4s, collector_info->ipv4.src_ip));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_IPV4s, collector_info->ipv4.dst_ip));

    } else if(collector_info->transport_type == bcmCollectorTransportTypeIpv6Udp) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, TOSs, collector_info->ipv6.traffic_class));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, TTLs, collector_info->ipv6.hop_limit));

        sal_memcpy(&src_ipv6_lower, collector_info->ipv6.src_ip, 8);
        sal_memcpy(&src_ipv6_upper, &(collector_info->ipv6.src_ip[8]), 8);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, SRC_IPV6_LOWERs, src_ipv6_lower));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, SRC_IPV6_UPPERs, src_ipv6_upper));

        sal_memcpy(&dst_ipv6_lower, collector_info->ipv6.dst_ip, 8);
        sal_memcpy(&dst_ipv6_upper, &(collector_info->ipv6.dst_ip[8]), 8);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_IPV6_LOWERs, dst_ipv6_lower));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_IPV6_UPPERs, dst_ipv6_upper));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_L4_UDP_PORTs, collector_info->udp.dst_port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_L4_UDP_PORTs, collector_info->udp.src_port));

    if (collector_info->udp.flags & BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, UDP_CHKSUMs, 1));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SYSTEM_ID_LENs, collector_info->protobuf.system_id_length));

    for (i = 0; i < collector_info->protobuf.system_id_length; i++) {
        tmp_64_val = collector_info->protobuf.system_id[i];
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl, SYSTEM_IDs, i, &tmp_64_val, 1));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, COMPONENT_IDs, collector_info->protobuf.component_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, OBSERVATION_DOMAINs,
                               collector_info->ipfix.observation_domain_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, IPFIX_ENTERPRISE_NUMBERs,
                               collector_info->ipfix.enterprise_number));

    if (id_found == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    if ((options & BCM_COLLECTOR_WITH_ID)  &&
        (options & BCM_COLLECTOR_REPLACE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

    COLLECTOR_UNLOCK(unit);
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    }
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *      bcm_ltsw_collector_get
 * Purpose:
 *      Gets a collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      id              - (IN)  Collector Id
 *      collector_info  - (OUT) Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_collector_get(int unit,
                             bcm_collector_t id,
                             bcm_collector_info_t *collector_info)
{
    int internal_id = 0, transport_type = 0,i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    const char* symbol;
    uint32_t r_cnt;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);
    if (!COL_EXT_ID_BMP_GET(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
            (BSL_META_U(unit,
            "Collector(unit %d) Error: Collector ID does not exist\n"), unit));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    internal_id = COL_INT_ID_MAP(unit,id);
    transport_type = COL_TYPE_MAP(unit, id);
    if (transport_type == LTSW_COLLECTOR_TYPE_IPV4_UDP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(unit, MON_COLLECTOR_IPV4s, &entry_hdl));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV4_IDs, internal_id));
        collector_info->transport_type = bcmCollectorTransportTypeIpv4Udp;
    }

    if (transport_type == LTSW_COLLECTOR_TYPE_IPV6_UDP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, MON_COLLECTOR_IPV6s, &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV6_IDs, internal_id));
        collector_info->transport_type = bcmCollectorTransportTypeIpv6Udp;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, TAG_STRUCTUREs, &symbol));

    if (!sal_strcmp(symbol, "UNTAGGED")) {
        collector_info->eth.vlan_tag_structure = BCM_COLLECTOR_ETH_HDR_UNTAGGED;
    } else if (!sal_strcmp(symbol, "SINGLE_TAGGED")) {
        collector_info->eth.vlan_tag_structure = BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
    } else if (!sal_strcmp(symbol, "DOUBLE_TAGGED")) {
        collector_info->eth.vlan_tag_structure = BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_MACs, &value));
    bcmi_ltsw_util_uint64_to_mac(collector_info->eth.dst_mac, &value);


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_MACs, &value));
    bcmi_ltsw_util_uint64_to_mac(collector_info->eth.src_mac, &value);

    if (collector_info->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_VLAN_IDs, &value));
        collector_info->eth.inner_vlan_tag = value;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_TPIDs, &value));
        collector_info->eth.inner_tpid = value;
    }

    if (collector_info->eth.vlan_tag_structure != BCM_COLLECTOR_ETH_HDR_UNTAGGED) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, VLAN_IDs, &value));
        collector_info->eth.outer_vlan_tag = value;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TPIDs, &value));
        collector_info->eth.outer_tpid = value;
    }



    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, TOSs, &value));
        collector_info->ipv4.dscp = value;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, TTLs, &value));
        collector_info->ipv4.ttl = value;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, SRC_IPV4s, &value));
        collector_info->ipv4.src_ip = value;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, DST_IPV4s, &value));
        collector_info->ipv4.dst_ip = value;
    } else if(collector_info->transport_type == bcmCollectorTransportTypeIpv6Udp) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, TOSs, &value));
        collector_info->ipv6.traffic_class = value;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, TTLs, &value));
        collector_info->ipv6.hop_limit = value;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, SRC_IPV6_LOWERs, &value));
        sal_memcpy(&collector_info->ipv6.src_ip, &value, 8);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, SRC_IPV6_UPPERs, &value));
        sal_memcpy(&collector_info->ipv6.src_ip[8], &value, 8);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, DST_IPV6_LOWERs, &value));
        sal_memcpy(&collector_info->ipv6.dst_ip, &value, 8);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, DST_IPV6_UPPERs, &value));
        sal_memcpy(&collector_info->ipv6.dst_ip[8], &value, 8);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_L4_UDP_PORTs, &value));
    collector_info->udp.dst_port = value;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_L4_UDP_PORTs, &value));
    collector_info->udp.src_port= value;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, UDP_CHKSUMs, &value));
    if (value != 0) {
        collector_info->udp.flags |= BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE;
    }


    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, SYSTEM_ID_LENs, &value));
    collector_info->protobuf.system_id_length = value;

    for (i = 0; i < collector_info->protobuf.system_id_length; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_get(entry_hdl, SYSTEM_IDs, i, &value, 1, &r_cnt));
        collector_info->protobuf.system_id[i] = value;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, COMPONENT_IDs, &value));
    collector_info->protobuf.component_id = value;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, OBSERVATION_DOMAINs, &value));
    collector_info->ipfix.observation_domain_id = value;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, IPFIX_ENTERPRISE_NUMBERs, &value));
    collector_info->ipfix.enterprise_number = value;


    COLLECTOR_UNLOCK(unit);

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_collector_get_all
 * Purpose:
 *      Get the ID list of all collectors configured.
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      max_size             - (IN)  Size of the collector list array
 *      collector_list       - (OUT) Collector id list
 *      list_szie            - (OUT) NUmber of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_collector_get_all(
                           int unit,
                           int max_size,
                           bcm_collector_t *collector_list,
                           int *list_size)
{   int i;
    int num_collectors;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    if (max_size != 0 && collector_list == NULL) {
        return SHR_E_PARAM;
    }
    COLLECTOR_LOCK(unit);

    num_collectors = 0;
    *list_size = 0;

    for (i = 0;  i < _COLLECTOR_ID_MAX; i++) {
        if (COL_EXT_ID_BMP_GET(unit, i)) {
            num_collectors++;
            if (*list_size < max_size) {
                if(max_size != 0) {
                    collector_list[*list_size] = i;
                }
                (*list_size)++;
            }
        }
    }

    if (max_size == 0) {
         *list_size = num_collectors;
    }

    COLLECTOR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_collector_destroy
 * Purpose:
 *      Destroys a collector
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      id             - (IN) Collector Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_collector_destroy(int unit,
                               bcm_collector_t id)
{
    int internal_id , transport_type;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);
    if (!COL_EXT_ID_BMP_GET(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
            (BSL_META_U(unit,
                "Collector(unit %d) Error: Collector ID is not in use\n"), unit));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (COL_REF_COUNT(unit, id) != 0) {
        LOG_ERROR(BSL_LS_BCM_COLLECTOR,
            (BSL_META_U(unit,
                "Collector(unit %d) Error: Collector in use\n"), unit));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    internal_id = COL_INT_ID_MAP(unit, id);
    transport_type = COL_TYPE_MAP(unit, id);

    if (transport_type == LTSW_COLLECTOR_TYPE_IPV4_UDP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, MON_COLLECTOR_IPV4s, &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV4_IDs, internal_id));

    } else if (transport_type == LTSW_COLLECTOR_TYPE_IPV6_UDP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, MON_COLLECTOR_IPV6s, &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IPV6_IDs, internal_id));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    if (transport_type == LTSW_COLLECTOR_TYPE_IPV4_UDP) {
        COL_IPV4_ID_BMP_CLEAR(unit, internal_id);
    } else if (transport_type == LTSW_COLLECTOR_TYPE_IPV6_UDP) {
        COL_IPV6_ID_BMP_CLEAR(unit, internal_id);
    }
    COL_EXT_ID_BMP_CLEAR(unit, id);

    COLLECTOR_UNLOCK(unit);

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmi_ltsw_collector_ref_count_update
 * Purpose:
 *     Increment/Decrement reference count for a collector
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     update        - (IN) Reference count update (+/-)
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ltsw_collector_ref_count_update(int unit, bcm_collector_t id, int update)
{
    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    if (id > _COLLECTOR_ID_MAX) {
        return BCM_E_PARAM;
    }

    if (COL_REF_COUNT(unit, id) + update < 0) {
        return BCM_E_PARAM;
    }
    COL_REF_COUNT(unit, id) += update;
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcmi_ltsw_collector_export_profile_ref_count_update
 * Purpose:
 *     Increment/Decrement reference count for an export profile
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Export profile Id
 *     update        - (IN) Reference count update (+/-)
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ltsw_collector_export_profile_ref_count_update(int unit, int id, int update)
{
    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    if (id >= _EXPORT_PROFILE_ID_MAX) {
        return SHR_E_PARAM;
    }
    COLLECTOR_LOCK(unit);

    if (EXPORT_REF_COUNT(unit, id) < 0) {
        return SHR_E_PARAM;
    }
    EXPORT_REF_COUNT(unit,id) += update;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmi_ltsw_collector_user_update
 * Purpose:
 *     Add user for a collector.
 *     Currently one user is expected per collector.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     user          - (IN) The user type of calling app
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ltsw_collector_user_update(int unit, bcm_collector_t id, bcm_ltsw_collector_export_app_t user)
{
    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    if (id > _COLLECTOR_ID_MAX) {
        return SHR_E_PARAM;
    }
    COLLECTOR_LOCK(unit);

    COL_USER_UPDATE(unit,id) = user;

    COLLECTOR_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmi_ltsw_collector_check_user_is_other
 * Purpose:
 *     Check if collector ID is used by other user.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     my_user       - (IN) The user type of calling app
 * Returns:
 *     BCM_E_XXX
 */
uint8
bcmi_ltsw_collector_check_user_is_other(int unit, bcm_collector_t id, bcm_ltsw_collector_export_app_t my_user)
{
    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    if (id > _COLLECTOR_ID_MAX) {
        return SHR_E_PARAM;
    }
    COLLECTOR_LOCK(unit);

    if ((COL_USER_UPDATE(unit,id) != LTSW_COLLECTOR_EXPORT_APP_NONE) &&
        (COL_USER_UPDATE(unit,id) != my_user)) {
        return 1;
    }

    COLLECTOR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmi_ltsw_collector_internal_id_get
 * Purpose:
 *     Get the internal ID (the ID that is programmed in LT) and the type of a
 *     collector.
 * Parameters:
 *     unit          - (IN)  BCM device number
 *     id            - (IN)  Collector Id
 *     internal_id   - (OUT) Internal ID of the collector.
 *     type          - (OUT) Type of the collector.
 * Returns:
 *     SHR_E_XXX
 */
int
bcmi_ltsw_collector_internal_id_get(int unit, bcm_collector_t id,
                                    uint32_t *internal_id,
                                    bcm_ltsw_collector_transport_type_t *type)
{
    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);

    if (!COL_EXT_ID_BMP_GET(unit, id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *internal_id = COL_INT_ID_MAP(unit, id);
    *type = COL_TYPE_MAP(unit, id);


    COLLECTOR_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmi_ltsw_collector_external_id_get
 * Purpose:
 *     Get the external ID of a collector.
 * Parameters:
 *     unit          - (IN)  BCM device number
 *     internal_id   - (IN)  Collector internal ID.
 *     type          - (IN)  Collector type.
 *     id            - (OUT) External ID of the collector.
 * Returns:
 *     SHR_E_XXX
 */
int
bcmi_ltsw_collector_external_id_get(int unit, uint32_t internal_id,
                                    bcm_ltsw_collector_transport_type_t type,
                                    bcm_collector_t *id)
{
    int rv;

    SHR_FUNC_ENTER(unit);
    COLLECTOR_INIT_CHECK(unit);

    COLLECTOR_LOCK(unit);

    switch (type) {
        case LTSW_COLLECTOR_TYPE_IPV4_UDP:
            if (COL_IPV4_ID_BMP_GET(unit, internal_id) == 0) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;

        case LTSW_COLLECTOR_TYPE_IPV6_UDP:
            if (COL_IPV6_ID_BMP_GET(unit, internal_id) == 0) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = SHR_E_NOT_FOUND;
    *id = 0;
    COL_EXT_ID_ITER(unit, *id) {
        if ((COL_INT_ID_MAP(unit, *id) == internal_id) &&
            (COL_TYPE_MAP(unit, *id) == type)) {
            rv = SHR_E_NONE;
            break;
        }
    }

    COLLECTOR_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}
