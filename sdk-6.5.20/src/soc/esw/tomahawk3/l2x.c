/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/ptable.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/iproc.h>
#include <soc/mcm/intr_iproc.h>
#include <soc/tomahawk3.h>
#include <soc/scache.h>

#define BSL_LSS_SCACHE (BSL_LS_SHARED_SCACHE | BSL_VERBOSE)
#define SOC_TH3_L2_MODULE_NAME "l2"

#if defined(BCM_TOMAHAWK3_SUPPORT)
#ifdef BCM_XGS_SWITCH_SUPPORT

/* Chunk size of L2 table to read in aging thread */
/* The chunk size should be small, ideally, less than or equal to 64, since L2
 * lock is acquired for modifying/deleting entries. If chunk size is large,
 * other threads (example, learning, application threads and so on) will have
 * to wait longer to access L2 table. The trade-off is the number of soc library
 * read calls for readng L2 table - if chunk size is small more number of read
 * calls are issued
 */
#define _SOC_TH3_AGE_L2_TABLE_DMA_CHUNK_SIZE 8

/* Size of AVL table used for learning entries */
#define _SOC_TH3_L2_LRN_TBL_SIZE 8192

#define SOC_MEM_COMPARE_RETURN(a, b) {		\
        if ((a) < (b)) { return -1; }		\
        if ((a) > (b)) { return  1; }		\
}

typedef struct soc_l2_lrn_avl_info_s {
    vlan_id_t       vlan;
    soc_module_t    mod;
    int             dest_type; /* 0=dest. is port, 1=dest. is trunk */
    int             port_tgid; /* Holds port num if dest_type is 0.
                                  Holds TGID if dest_type is 1 */
    sal_mac_addr_t  mac;
    int             in_hw; /* Entry  programmed in h/w. Used to avoid hits due
                              to duplicate pkts */
} soc_l2_lrn_avl_info_t, *soc_l2_lrn_avl_info_p;

typedef struct soc_l2x_callback_info_s {
    sal_mac_addr_t  mac;  /* MAC-VLAN pair defines key for AVL tree lookup */
    vlan_id_t       vlan;
    int             callback_disable; /* If set, callback to application will
                                         not be made. Modified in l2 add/delete
                                         APIs */
} soc_l2x_callback_info_t, *soc_l2x_callback_info_p;

/* Callback info for each entry added through the application */
static shr_avl_t *soc_th3_l2x_callback_info;

/* Mutex to restrict access to 'callback_info' during modification/lookup */
static sal_mutex_t soc_th3_l2x_callback_info_mutex;

/* Control to enable/disable application trigerred L2 address adds/deletes */
static int soc_th3_l2x_callback_info_enable = FALSE;

/* THe following two bitmaps are used to sync and store in scache the
 * information about API-added L2 entries
 */
static SHR_BITDCL *soc_th3_l2x_callback_entry_map;
static SHR_BITDCL *soc_th3_l2x_callback_state_map;

static int _soc_th3_l2_bulk_age_iter[SOC_MAX_NUM_DEVICES] = {0};

static uint8 rev_id = 0;
static uint16 dev_id = 0;

/*
 * Function:
 *	soc_th3_l2x_appl_callback_enable
 * Purpose:
 *	This function sets up internal database used to maintain L2 entries added
 *  and removed by the application, through bcm_l2_addr_add and
 *  bc_l2_addr_delete APIs. A mutex is created to serialize access to the
 *  database. The allocated resources will be freed during soc_detach,
 *  soc_shutdown, because the application can potentially enable/disable this
 *  feature multiple times during the time the device is initialized/active - we
 *  re-use the resources instead of destroying and re-allocating them.
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	None
 */
int
soc_th3_l2x_appl_callback_enable(int unit)
{
    int num_entries = 0;
    int alloc_status = -1;

    /* Create resources here. The application can enable/disable the feature
     * multiple times during the time the device is active, hence the resources
     * will be re-used. Therefore we will be free the resources in soc_detach,
     * soc_shutdown
     */

    /* Create mutex if this is the first call */
    if (soc_th3_l2x_callback_info_mutex == NULL) {
        if ((soc_th3_l2x_callback_info_mutex =
                sal_mutex_create("L2CbInfoMutex")) == NULL) {

            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "%d: Error calling sal_mutex_create for"
                " L2 Callback Info Mutex\n"), unit));

            return SOC_E_RESOURCE;
        }
    }

    /* Create callback db only if the feature is enabled first time */
    if (soc_th3_l2x_callback_info == NULL) {
        if (shr_avl_create(&soc_th3_l2x_callback_info, INT_TO_PTR(unit),
                           sizeof(soc_l2x_callback_info_t),
                           soc_mem_index_count(unit, L2Xm)) < 0) {

                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "%d: Error calling shr_avl_create\n"), unit));

                return SOC_E_RESOURCE;
        }
    }

    num_entries = soc_mem_index_count(unit, L2Xm);

    if (soc_th3_l2x_callback_entry_map == NULL) {
        soc_th3_l2x_callback_entry_map = sal_alloc(
                                             SHR_BITALLOCSIZE(num_entries),
                                             "l2x_appl_callback_map");

        if (soc_th3_l2x_callback_entry_map == NULL) {
            alloc_status = FALSE;
        } else {
            alloc_status = TRUE;
        }
    }

    if (soc_th3_l2x_callback_state_map == NULL) {
        soc_th3_l2x_callback_state_map = sal_alloc(
                                             SHR_BITALLOCSIZE(num_entries),
                                             "l2x_appl_callback_state_map");

        if (soc_th3_l2x_callback_state_map == NULL) {
            alloc_status = FALSE;
        } else {
            alloc_status = TRUE;
        }
    }

    /* Check if memory allocation failed */
    if (alloc_status == FALSE) {

        /* Free up previously allocated resources */
        if (soc_th3_l2x_callback_info != NULL) {
            (void)shr_avl_destroy(soc_th3_l2x_callback_info);
            soc_th3_l2x_callback_info = NULL;
        }

        if (soc_th3_l2x_callback_info_mutex != NULL) {
            sal_mutex_destroy(soc_th3_l2x_callback_info_mutex);
            soc_th3_l2x_callback_info_mutex = NULL;
        }

        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "%d: Memory allocation failed\n"), unit));

        return SOC_E_MEMORY;
    }

    if (alloc_status == TRUE) {
         int sz = 0;
        /* Clear all bits */
        sz = SHR_BITALLOCSIZE(num_entries);
        /* Convert to number of bits */
        sz <<= 3;
        SHR_BITCLR_RANGE(soc_th3_l2x_callback_entry_map, 0, sz);
        SHR_BITCLR_RANGE(soc_th3_l2x_callback_state_map, 0, sz);
    }

    soc_th3_l2x_callback_info_enable = TRUE;

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_disable
 * Purpose:
 *	This function clears internal database which maintains L2 entries added and
 *  deleted by the application. All entries are removed.
 *	feature
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	None
 */
int
soc_th3_l2x_appl_callback_disable(int unit)
{
    /* Clear all entries recorded so far. When the feature is enabled again,
     * the buffer will be re-used
     */
	if ((soc_th3_l2x_callback_info != NULL) &&
        (soc_th3_l2x_callback_info_mutex != NULL)) {
	    sal_mutex_take(soc_th3_l2x_callback_info_mutex, sal_mutex_FOREVER);
	    (void)shr_avl_delete_all(soc_th3_l2x_callback_info);
	    sal_mutex_give(soc_th3_l2x_callback_info_mutex);
	}

    soc_th3_l2x_callback_info_enable = FALSE;

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_config_state_get
 * Purpose:
 *	This function returns current configuration state of application callback
 *	feature
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	TRUE  : If feature is enabled by application
 *	FALSE : If feature is disabled by application
 * Notes:
 *	None
 */
int
soc_th3_l2x_appl_callback_config_state_get(int unit)
{
    return soc_th3_l2x_callback_info_enable;
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_ready
 * Purpose:
 *	This function returns initialization state of application callback
 *	feature to the caller
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	TRUE  : If feature is initialized
 *	FALSE : If feature is not initialized
 * Notes:
 *	None
 */
int
soc_th3_l2x_appl_callback_ready(int unit)
{
    if (soc_th3_l2x_callback_info_enable &&
        (soc_th3_l2x_callback_info_mutex != NULL) &&
        (soc_th3_l2x_callback_info != NULL) &&
        (soc_th3_l2x_callback_entry_map != NULL) &&
        (soc_th3_l2x_callback_state_map != NULL)) {
        return TRUE;
    }

    return FALSE;
}

/*
 * Function:
 * 	_soc_th3_appl_callback_entry_compare_key
 * Purpose:
 *  Comparison function for AVL tree used for application callback feature's
 *  operations
 * Parameters:
 *	user_data - User supplied data reuired by AVL library
 *	datum1    - First data item to compare
 *	datum2    - Second data item to compare
 * Returns:
 *	0 : If comparison items are equal
 *	1 : If comparison items are different
 * Notes:
 *	None
 */
STATIC int
_soc_th3_appl_callback_entry_compare_key(void *user_data,
                                         shr_avl_datum_t *datum1,
                                         shr_avl_datum_t *datum2)
{
     soc_l2x_callback_info_p k1, k2;

    /* COMPILER_REFERENCE(user_data);*/
    k1 = (soc_l2x_callback_info_p)datum1;
    k2 = (soc_l2x_callback_info_p)datum2;

    SOC_MEM_COMPARE_RETURN(k1->vlan, k2->vlan);

    return ENET_CMP_MACADDR(k1->mac, k2->mac);
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_entry_add
 * Purpose:
 *	This function adds an entry to internal application callback database. It is
 *	called as part of bcm_l2_addr_add API
 * Parameters:
 *	unit - StrataSwitch unit number
 *	mac  - Mac address. MAC-VLAN pair is used as key for locating corresponding
 *	       entry
 *	vlan - Vlan id
 *	state - Setting of callback suppress (TRUE/FALSE)
 * Returns:
 *  SOC_E_xxx
 * Notes:
 *	None
 */
int
soc_th3_l2x_appl_callback_entry_add(int unit, sal_mac_addr_t mac,
                                    vlan_id_t vlan, int state)
{
    soc_l2x_callback_info_t entry;

    /* If feature is not initialized, do nothing */
    if (!soc_th3_l2x_appl_callback_ready(unit)) {
        return SOC_E_NONE;
    }

    if (shr_avl_count(soc_th3_l2x_callback_info) <
                      soc_mem_index_count(unit, L2Xm)) {
        sal_memset(&entry, 0x0, sizeof(entry));
        sal_memcpy(entry.mac, mac, sizeof(entry.mac));
        entry.vlan = vlan;
        entry.callback_disable = state;

        sal_mutex_take(soc_th3_l2x_callback_info_mutex, sal_mutex_FOREVER);
        (void)shr_avl_insert(soc_th3_l2x_callback_info,
                             _soc_th3_appl_callback_entry_compare_key,
                             (shr_avl_datum_t *)(&entry));
        sal_mutex_give(soc_th3_l2x_callback_info_mutex);
    } else {
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                 "shr_avl_insert - tree full\n")));
        return SOC_E_FULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_entry_delete
 * Purpose:
 *	This function removes an entry from internal application callback database.
 *  It is called as part of bcm_l2_addr_delete API. It is called from aging
 *  thread to delete the application added entry that aged out
 * Parameters:
 *	unit - StrataSwitch unit number
 *	mac  - Mac address. MAC-VLAN pair is used as key for locating corresponding
 *	       entry
 *	vlan - Vlan id
 * Returns:
 *  SOC_E_xxx
 * Notes:
 *	None
 */
int
soc_th3_l2x_appl_callback_entry_delete(int unit, sal_mac_addr_t mac,
                                       vlan_id_t vlan)
{
    soc_l2x_callback_info_t entry;

    /* If feature is not initialized, do nothing */
    if (!soc_th3_l2x_appl_callback_ready(unit)) {
        return SOC_E_NONE;
    }

    if (shr_avl_count(soc_th3_l2x_callback_info) > 0) {
        int rv;

        sal_memset(&entry, 0x0, sizeof(entry));
        sal_memcpy(entry.mac, mac, sizeof(entry.mac));
        entry.vlan = vlan;
        entry.callback_disable = 0;

        sal_mutex_take(soc_th3_l2x_callback_info_mutex, sal_mutex_FOREVER);
        rv = shr_avl_delete(soc_th3_l2x_callback_info,
                             _soc_th3_appl_callback_entry_compare_key,
                             (shr_avl_datum_t *)(&entry));
        sal_mutex_give(soc_th3_l2x_callback_info_mutex);

        if (rv == 0) {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "Info: soc_th3_l2x_appl_callback_entry_delete:shr_avl_delete:"
                " Did not find datum\n")));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_entry_lookup
 * Purpose:
 *	This function searches for an entry in internal application callback
 *	database.
 * Parameters:
 *	unit(IN)    - StrataSwitch unit number
 *	mac(IN)     - Mac address. MAC-VLAN pair is used as key for locating
 *	              corresponding entry
 *	vlan(IN)    - Vlan id
 *	found(OUT)  - Returns result of search; see 'Returns' section below
 *	enable(OUT) - Returns state of callback flag for the entry
 * Returns:
 *  SOC_E_XXX and
 *
 *  *found -      TRUE if entry was found,
 *                FALSE otherwise
 *  *enable -     Value of callback flag from database, if entry was found,
 *                -1, otherwise
 * Notes:
 *	None
 */
int
soc_th3_l2x_appl_callback_entry_lookup(int unit, sal_mac_addr_t mac,
                                       vlan_id_t vlan, int *found, int *enable)
{
    soc_l2x_callback_info_t entry;
    int result;

    /* If feature is not initialized, do nothing */
    if (!soc_th3_l2x_appl_callback_ready(unit)) {
        *found = FALSE;
        *enable = -1;
        return SOC_E_NONE;
    }

    sal_memset(&entry, 0x0, sizeof(entry));
    sal_memcpy(entry.mac, mac, sizeof(entry.mac));
    entry.vlan = vlan;

	sal_mutex_take(soc_th3_l2x_callback_info_mutex, sal_mutex_FOREVER);
    result = shr_avl_lookup(soc_th3_l2x_callback_info,
                            _soc_th3_appl_callback_entry_compare_key,
                            (shr_avl_datum_t *)(&entry));
    sal_mutex_give(soc_th3_l2x_callback_info_mutex);

    /* Entry found */
    *found = (result == 1) ? TRUE : FALSE;

    /* If entry is found, get the callback setting (lookup writes back the
     * datum completely)
     */
    *enable = (*found == TRUE) ? entry.callback_disable : -1;

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_entry_delete_all
 * Purpose:
 *	This function removes all entries from internal application callback
 *	database.
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *	None
 */
int
soc_th3_l2x_appl_callback_entry_delete_all(int unit)
{
    if ((soc_th3_l2x_callback_info_mutex != NULL) &&
        (soc_th3_l2x_callback_info != NULL)) {
	    sal_mutex_take(soc_th3_l2x_callback_info_mutex, sal_mutex_FOREVER);
        (void)shr_avl_delete_all(soc_th3_l2x_callback_info);
        sal_mutex_give(soc_th3_l2x_callback_info_mutex);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_resources_destroy
 * Purpose:
 *	This function frees up mutex and memory allocated for internal application
 *	callback database.
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *	None
 */
int
soc_th3_l2x_appl_callback_resources_destroy(int unit)
{
    if ((soc_th3_l2x_callback_info_mutex != NULL) &&
        (soc_th3_l2x_callback_info != NULL)) {

        shr_avl_destroy(soc_th3_l2x_callback_info);
        soc_th3_l2x_callback_info = NULL;

        sal_mutex_destroy(soc_th3_l2x_callback_info_mutex);
        soc_th3_l2x_callback_info_mutex = NULL;
    }

    if (soc_th3_l2x_callback_entry_map != NULL) {
        sal_free(soc_th3_l2x_callback_entry_map);
        soc_th3_l2x_callback_entry_map = NULL;
    }

    if (soc_th3_l2x_callback_state_map != NULL) {
        sal_free(soc_th3_l2x_callback_state_map);
        soc_th3_l2x_callback_state_map = NULL;
    }

    soc_th3_l2x_callback_info_enable = FALSE;

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
soc_th3_l2x_appl_callback_entry_index_set(void *user_data,
                                          shr_avl_datum_t *datum,
                                          void *extra_data)
{
    int unit = PTR_TO_INT(user_data);
    soc_l2x_callback_info_p entry = (soc_l2x_callback_info_p)datum;
    l2x_entry_t l2x_entry, l2x_entry_out;
    int index = 0;
    int rv;

    COMPILER_REFERENCE(extra_data);

    /* Check if database is empty. If so, do nothing */
    if (entry == NULL) {
        return SOC_E_NONE;
    }

    sal_memset(&l2x_entry, 0, sizeof (l2x_entry));

    soc_L2Xm_mac_addr_set(unit, &l2x_entry, MAC_ADDRf, entry->mac);
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf, entry->vlan);
    soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                         TH3_L2_HASH_KEY_TYPE_BRIDGE);

    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &index,
                        (void *)&l2x_entry, (void *)&l2x_entry_out, 0);

    /* Program bit corresponding to the index of the entry from h/w table */
    if (rv == SOC_E_NONE) {
        SHR_BITSET(soc_th3_l2x_callback_entry_map, index);

        SHR_BITWRITE(soc_th3_l2x_callback_state_map, index,
                     !!entry->callback_disable);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_index_bitmap_create
 * Purpose:
 *	This function creates reads mac-vlan pair for each entry from internal
 *	application. It converts finds the corresponding index of the entry from
 *	h/w L2X table, and then it sets the corresponding bit in the callback
 *	bitmap. The callback state is also stored. This information is then saved
 *	in to the scache during warmboot sync stage
 * Parameters:
 *	unit - StrataSwitch unit number
 *	count - Pointer to store the number of entries in the database (at the time
 *	        of recording the indices)
 * Returns:
 *  Not applicable
 * Notes:
 *	Locks for L2x and the appl database have been obtained, so the info is not
 *	modified when we are populating scache
 */
STATIC void
soc_th3_l2x_appl_callback_index_bitmap_create(int unit, int *count)
{
    /* Create bitmaps only if there is atleast one entry in the database */
    *count = shr_avl_count(soc_th3_l2x_callback_info);
    if (*count > 0) {
        shr_avl_traverse(soc_th3_l2x_callback_info,
                         soc_th3_l2x_appl_callback_entry_index_set, NULL);
    }
}

/*
 * Function:
 *      soc_th3_l2_scache_appl_callback_info_dump
 * Purpose:
 *      Dump contents of scache - callback feature enable state, count of
 *      application added entries, entry bitmap, entry state bitmap
 * Parameters:
 *      unit            - StrataSwitch unit number
 *      scache_bmap_ptr - Start of scache locations where entry index and
 *                        corresponding entrys' state bitmaps are located;
 *                        they are always the same size
 *      bmap_size       - Size of bitmaps in bytes
 * Returns:
 *      void
 */
void soc_th3_l2_scache_appl_callback_info_dump(int unit, uint8 *scache_bmap_ptr,
                                               int bmap_size)
{

    SOC_SCACHE_DUMP_DECL;

    /* Dump entry bitmap */
    SOC_SCACHE_DUMP_START(unit, SOC_TH3_L2_MODULE_NAME, scache_bmap_ptr);

    SOC_SCACHE_DUMP_DATA_BEGIN("offset:entry_bmap_word0_word255");

    SOC_SCACHE_DUMP_META_L(scache_bmap_ptr, bmap_size);

    SOC_SCACHE_DUMP_DATA_END();

    SOC_SCACHE_DUMP_DATA_BEGIN("offset:entry_state_bmap_word0_word255");
    SOC_SCACHE_DUMP_META_L(scache_bmap_ptr + bmap_size, bmap_size);
    SOC_SCACHE_DUMP_DATA_END();

    SOC_SCACHE_DUMP_STOP();
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_info_sync
 * Purpose:
 *	This function creates reads mac-vlan pair for each entry from internal
 *	application. It converts finds the corresponding index of the entry from
 *	h/w L2X table, and then it sets the corresponding bit in the callback
 *	bitmap. The callback state is also stored. This information is then saved
 *	in to the scache during warmboot sync stage. Should be called for wb
 *	versions >= BCM_WB_VERSION_1_2
 * Parameters:
 *	unit - StrataSwitch unit number
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *	We obtain locks for L2x and the appl database. The locks may be held
 *	for long time if application added a large number of entries. Since this
 *	happens during warmboot, when h/w modifications are not allowed, we can
 *	afford to wait for the bitmaps to be created
 */
int
soc_th3_l2x_appl_callback_info_sync(int unit, uint8 **scache_ptr)
{
    int sz;
    int num_entries;
    int enable;
    int count;
    uint8 *scache_bmap_ptr = NULL;
    SOC_SCACHE_DUMP_DECL;

    num_entries = soc_mem_index_count(unit, L2Xm);

    SOC_SCACHE_DUMP_START(unit, SOC_TH3_L2_MODULE_NAME, *scache_ptr);
    if (soc_th3_l2x_appl_callback_ready(unit)) {
        /* Get lock, mutex since we check and modify database items */
        soc_mem_lock(unit, L2Xm);
        sal_mutex_take(soc_th3_l2x_callback_info_mutex, sal_mutex_FOREVER);

        enable = soc_th3_l2x_appl_callback_config_state_get(unit);
        sz = sizeof(int);
        SOC_SCACHE_DUMP_DATA_BEGIN("cb_supress_feature_enable");
        sal_memcpy(*scache_ptr, &enable, sz);
        SOC_SCACHE_DUMP_DATA_INT32d_V(*scache_ptr);
        *scache_ptr += sz;
        SOC_SCACHE_DUMP_DATA_END();

        soc_th3_l2x_appl_callback_index_bitmap_create(unit, &count);

        SOC_SCACHE_DUMP_DATA_BEGIN("entry_count");
        /* Save count of entries */
        sal_memcpy(*scache_ptr, &count, sz);
        SOC_SCACHE_DUMP_DATA_INT32d_V(*scache_ptr);
        *scache_ptr += sz;
        SOC_SCACHE_DUMP_DATA_END();

        scache_bmap_ptr = *scache_ptr;

        sz = SHR_BITALLOCSIZE(num_entries);

        /* Copy information if there is atleast one entry in the database */
        if (count > 0) {
            sal_memcpy(*scache_ptr, soc_th3_l2x_callback_entry_map, sz);
            *scache_ptr += sz;
            sal_memcpy(*scache_ptr, soc_th3_l2x_callback_state_map, sz);
            *scache_ptr += sz;
        } else {
            /* Database is empty, store bitmaps as 0's */
            sal_memset(*scache_ptr, 0x0, sz);
            *scache_ptr += sz;
            sal_memset(*scache_ptr, 0x0, sz);
            *scache_ptr += sz;
        }

        sal_mutex_give(soc_th3_l2x_callback_info_mutex);
        soc_mem_unlock(unit, L2Xm);
    } else {
        sz = sizeof(int);
        enable = 0;
        SOC_SCACHE_DUMP_DATA_BEGIN("cb_supress_feature_enable");
        /* Feature disabled */
        sal_memcpy(*scache_ptr, &enable, sz);
        SOC_SCACHE_DUMP_DATA_INT32d_V(*scache_ptr);
        *scache_ptr += sz;
        SOC_SCACHE_DUMP_DATA_END();

        count = 0;
        SOC_SCACHE_DUMP_DATA_BEGIN("entry_count");
        /* Feature disabled, save count as 0 */
        sal_memcpy(*scache_ptr, &count, sz);
        SOC_SCACHE_DUMP_DATA_INT32d_V(*scache_ptr);
        *scache_ptr += sz;
        SOC_SCACHE_DUMP_DATA_END();

        scache_bmap_ptr = *scache_ptr;

        /* Initialize locations with zeroes */
        sz = SHR_BITALLOCSIZE(num_entries);

        sal_memset(*scache_ptr, 0x0, sz);
        *scache_ptr += sz;
        sal_memset(*scache_ptr, 0x0, sz);
        *scache_ptr += sz;
    }

    SOC_SCACHE_DUMP_STOP();

    if (enable) {
        soc_th3_l2_scache_appl_callback_info_dump(unit, scache_bmap_ptr, sz);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_l2x_appl_callback_info_restore
 * Purpose:
 *	This function re-creates application callback database. It gets the bitmap
 *	information stored in the scache. For each bit set in the bitmap, it reads
 *	mac-vlan information from the h/w L2 table, and issues insert command in to
 *	the application callback database. This function is called during reload
 *	phase of warmboot. Should be called for wb versions >= BCM_WB_VERSION_1_2
 * Parameters:
 *	unit       - StrataSwitch unit number
 *	scache_ptr - scache pointer
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *	Lock for L2x may be held for a longer time depending on the number of
 *	entries processed. Since during this time the h/w L2 table is not modified,
 *	we can afford to wait for a longer time
 */
int
soc_th3_l2x_appl_callback_info_restore(int unit, uint8 **scache_ptr)
{
    int index = 0;
    l2x_entry_t l2x_entry;
    int num_entries = 0;
    int sz;
    int rv;
    int cb_enable = 0;
    int count;
    uint8 *scache_bmap_ptr = *scache_ptr;
    SOC_SCACHE_DUMP_DECL;

    SOC_SCACHE_DUMP_START(unit, SOC_TH3_L2_MODULE_NAME, *scache_ptr);

    SOC_SCACHE_DUMP_DATA_BEGIN("cb_supress_feature_enable");
    /* Read setting for application callback suppress feature */
    sal_memcpy(&cb_enable, scache_bmap_ptr, sizeof(int));
    SOC_SCACHE_DUMP_DATA_INT32d_V(scache_bmap_ptr);
    scache_bmap_ptr += sizeof(int);
    SOC_SCACHE_DUMP_DATA_END();

    SOC_SCACHE_DUMP_DATA_BEGIN("entry_count");
    /* Read the count of the number of entries */
    sal_memcpy(&count, scache_bmap_ptr, sizeof(int));
    SOC_SCACHE_DUMP_DATA_INT32d_V(scache_bmap_ptr);
    scache_bmap_ptr += sizeof(int);
    SOC_SCACHE_DUMP_DATA_END();
    SOC_SCACHE_DUMP_STOP();

    if (cb_enable) {
        /* Allocate resources to be used by the feature. Bitmaps are cleared
         * in enable function
         */
        SOC_IF_ERROR_RETURN(soc_th3_l2x_appl_callback_enable(unit));
    }

    /* If cb_enable is not set, we do not allocate resources now; it will be
     * done later when application sets up the feature
     */

    /* Get entry count */
    num_entries = soc_mem_index_count(unit, L2Xm);
    sz = SHR_BITALLOCSIZE(num_entries);

    if (cb_enable) {
        soc_th3_l2_scache_appl_callback_info_dump(unit, scache_bmap_ptr, sz);
    }

    /* Database was empty before warmboot, do nothing */
    if (count <= 0) {
        return SOC_E_NONE;
    }

    /* Copy bitmaps */
    sal_memcpy(soc_th3_l2x_callback_entry_map, scache_bmap_ptr, sz);
    scache_bmap_ptr += sz;

    sal_memcpy(soc_th3_l2x_callback_state_map, scache_bmap_ptr, sz);
    scache_bmap_ptr += sz;

    *scache_ptr = scache_bmap_ptr;

    soc_mem_lock(unit, L2Xm);

    SHR_BIT_ITER(soc_th3_l2x_callback_entry_map, num_entries, index) {
        sal_mac_addr_t mac;
        vlan_id_t vlan;

        /* Get mac-vlan information from h/w entry pointed by 'index' */
        rv = soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, index, (void *)&l2x_entry);

        if (SOC_SUCCESS(rv)) {
            int state = FALSE;

            /* Process valid entry only */
            if (soc_L2Xm_field32_get(unit, &l2x_entry, BASE_VALIDf)) {
                soc_L2Xm_mac_addr_get(unit, &l2x_entry, MAC_ADDRf, mac);
                vlan = soc_L2Xm_field32_get(unit, &l2x_entry, VLAN_IDf);

                state = SHR_IS_BITSET(soc_th3_l2x_callback_state_map, index);
                (void)soc_th3_l2x_appl_callback_entry_add(unit, mac, vlan,
                                                          state);
            }
        }
    }

    soc_mem_unlock(unit, L2Xm);

    return SOC_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */
/*
 * Function:
 *	soc_th3_l2x_shadow_callback
 * Purpose:
 *	Internal callback routine for updating an AVL tree shadow table
 * Parameters:
 *	unit - StrataSwitch unit number.
 *	entry_del - Entry to be deleted or updated, NULL if none.
 *	entry_add - Entry to be inserted or updated, NULL if none.
 *	fn_data - unused.
 * Notes:
 *	Used only if L2X shadow table is enabled.
 */

STATIC void
soc_th3_l2x_shadow_callback(int unit,
                        int flags,
                        l2x_entry_t *entry_del,
                        l2x_entry_t *entry_add,
                        void *fn_data)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    if (flags & (SOC_L2X_ENTRY_DUMMY | SOC_L2X_ENTRY_OVERFLOW)) {
        return;
    }

    /* If application callback suppress feature is enabled, we use
     * 'no action' flag to stop issuing callback
     */
    if ((flags & SOC_L2X_ENTRY_NO_ACTION) &&
        (!soc_th3_l2x_appl_callback_ready(unit))) {
        return;
    }

    /* Since sync thread (bcmL2X) updates both its own database and learn
     * shadow database, we make sure both threads are running, and are synced
     * together
     */  
    if ((soc->l2x_pid != SAL_THREAD_ERROR) &&
        (soc->arlShadowMutex != NULL) &&
        (soc->arlShadow != NULL) &&
        (soc->l2x_learn_pid != SAL_THREAD_ERROR) &&
        (soc->l2x_lrn_shadow_mutex != NULL)) {
        int rv;

        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);

        if (entry_del != NULL) {
            rv = shr_avl_delete(soc->arlShadow, soc_l2x_entry_compare_key,
                           (shr_avl_datum_t *)entry_del);
            if (rv == 0) {
                sal_mac_addr_t mac;
                vlan_id_t vlan;
                int dest;

                soc_mem_mac_addr_get(unit, L2Xm, entry_del, MAC_ADDRf, mac);
                vlan = soc_mem_field32_get(unit, L2Xm, entry_del, VLAN_IDf);
                dest = soc_mem_field32_get(unit, L2Xm, entry_del, DESTINATIONf);

                LOG_INFO(BSL_LS_SOC_L2,
                         (BSL_META_U(unit,
                          "AVL delete: datum not found:\n dest %d, vlan %d,"
                          " mac(hex) %02X:%02X:%02X:%02X:%02X:%02X\n"), dest, vlan,
                          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));
            }
        }

        if (entry_add != NULL) {
            shr_avl_insert(soc->arlShadow, soc_l2x_entry_compare_key,
                           (shr_avl_datum_t *)entry_add);
        }

        sal_mutex_give(soc->arlShadowMutex);

        /* Update shadow learn table after successful h/w update(s) */
        /* Note the order of delete and insert below is important, since for
         * station move condition in _soc_th3_learn_cache_entry_process, we
         * delete an entry first and then insert it with the new port info
         */
        sal_mutex_take(SOC_CONTROL(unit)->l2x_lrn_shadow_mutex,
                                   sal_mutex_FOREVER);
        if (entry_del != NULL) {
            soc_th3_lrn_shadow_delete(unit, entry_del);
        }

        if (entry_add != NULL) {
            soc_th3_lrn_shadow_insert(unit, entry_add);
        }
        sal_mutex_give(SOC_CONTROL(unit)->l2x_lrn_shadow_mutex);
    }
}

/*
 * Function:
 *	soc_th3_l2x_detach
 * Purpose:
 *	Deallocate L2X subsystem resources
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 * SOC_E_XXX
 * Notes:
 *  Learn cache interrupt is disabled. Learn cache is cleared, learn cache
 *  status bits are cleared	
 */
int
soc_th3_l2x_detach(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    

    /* Free reources allocated for shadow table */

    soc_l2x_unregister(unit, soc_th3_l2x_shadow_callback, NULL);

    /* Free cml_freeze structure  */
    _soc_l2x_cml_struct_free(unit);

    if (soc->arlShadow != NULL) {
        shr_avl_destroy(soc->arlShadow);
        soc->arlShadow = NULL;
    }

    if (soc->arlShadowMutex != NULL) {
        sal_mutex_destroy(soc->arlShadowMutex);
        soc->arlShadowMutex = NULL;
    }

    

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_l2x_attach
 * Purpose:
 *	Allocate L2X subsystem resources
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	The L2X tree shadow table is always allocated, since it will be used in
 *  learning, aging and table management. Value of spn_L2XMSG_AVL will be
 *  ignored
 */

int
soc_th3_l2x_attach(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    int datum_bytes, datum_max;

    (void)soc_th3_l2x_detach(unit);

    datum_bytes = sizeof (l2x_entry_t);
    datum_max = soc_mem_index_count(unit, L2Xm);

    if (shr_avl_create(&soc->arlShadow,
                       INT_TO_PTR(unit),
                       datum_bytes,
                       datum_max) < 0) {
        return SOC_E_MEMORY;
    }

    if ((soc->arlShadowMutex = sal_mutex_create("asMutex")) == NULL) {
        (void)soc_l2x_detach(unit);
        return SOC_E_MEMORY;
    }

    soc_l2x_register(unit, soc_th3_l2x_shadow_callback, NULL);

    /* Reset l2 freeze structure */
    soc_th3_l2x_reset_freeze_state(unit);

    /* Allocate cml freeze structure */
    SOC_IF_ERROR_RETURN(_soc_l2x_cml_struct_alloc(unit));

    return SOC_E_NONE;
}

/*
 * Function:
 *  _soc_th3_l2_age_entries_process
 * Purpose:
 *  This function is invoked as part of aging mechanism to check for hit bits
 *  and take appropriate action (either clear the hit bits, or delete the entry)
 *  Since there is no h/w aging support, nor do we have bulk operations block in
 *  hardware's L2 implementation, this function reads L2X entries, and takes
 *  decision one entry at a time
 * Parameters:
 *	unit - unit number
 * Returns:
 *	SOC_E_NONE on success, or other SOC_E_* error code on failure
 * Notes:
 *  This function will execute much slower than other devices that have
 *  hardware support for aging. Entries are processed one at a time
 */
STATIC int
_soc_th3_l2_age_entries_process(int unit, l2x_entry_t *l2x_entries,
                                int entry_buf_count)
{
    uint32 index_min, index_max;
    int chnk_idx, chnk_idx_max, idx;
    int chunk_size = entry_buf_count;
    int rv;

    index_min = soc_mem_index_min(unit, L2Xm);
    index_max = soc_mem_index_max(unit, L2Xm);


    for (chnk_idx = index_min; chnk_idx <= index_max; chnk_idx += chunk_size) {
        /* Clear buffer */
        sal_memset((void *)l2x_entries, 0, sizeof(l2x_entry_t) * chunk_size);

        chnk_idx_max = ((chnk_idx + chunk_size) <= index_max) ?
                        (chnk_idx + chunk_size - 1) : index_max;

        soc_mem_lock(unit, L2Xm);

         /* Read portion of L2 Table */
        rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY, chnk_idx,
                                chnk_idx_max, l2x_entries);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);

            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                       "%s:DMA read failed: %s\n"), FUNCTION_NAME(), soc_errmsg(rv)));
            /* We do not return error, otherwise thread will be killed. If
             * thread is alive it can be debugged
             */
            return SOC_E_NONE;
        }

        /* Process each entry in the chunk for aging */
        for (idx = 0; idx <= (chnk_idx_max - chnk_idx); idx++) {
            l2x_entry_t *l2x_entry;
            uint32 hit_da, hit_sa, local_sa;

            l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm, l2x_entry_t*,
                                                     l2x_entries, idx);

            /* Skip invalid entry */
            if (!soc_L2Xm_field32_get(unit, l2x_entry, BASE_VALIDf)) {
                continue;
            }

            /* Skip static entry */
            if (soc_L2Xm_field32_get(unit, l2x_entry, STATIC_BITf)) {
                continue;
            }

            hit_da = soc_L2Xm_field32_get(unit, l2x_entry, HITDAf);
            hit_sa = soc_L2Xm_field32_get(unit, l2x_entry, HITSAf);
            local_sa = soc_L2Xm_field32_get(unit, l2x_entry, LOCAL_SAf);

            /* If no hot bits are set, delete the entry */
            if (!(hit_da || hit_sa || local_sa)) {
                /* Delete entry. Table is already locked */
                rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL,
                                    (void *)l2x_entry);
                if (SOC_FAILURE(rv)) {
                    /* If entry is not found, it has been deleted by other
                     * source, e.g. address delete from application. So we
                     * ignore not found condition here
                     */
                    if (rv != SOC_E_NOT_FOUND) {
                        soc_mem_unlock(unit, L2Xm);

                        LOG_WARN(BSL_LS_SOC_L2,
                                 (BSL_META_U(unit,
                                 "%s:soc mem delete failed: %s\n"), FUNCTION_NAME(),
                                 soc_errmsg(rv)));

                        return SOC_E_NONE;
                    }
                }

                if (soc_th3_l2x_appl_callback_ready(unit)) {
                    sal_mac_addr_t mac;
                    bcm_vlan_t vlan;

                    /* If an entry is recorded in application callback db,
                     * remove it
                     */
                    soc_L2Xm_mac_addr_get(unit, l2x_entry, MAC_ADDRf, mac);
                    vlan = soc_L2Xm_field32_get(unit, l2x_entry, VLAN_IDf);

                    (void)soc_th3_l2x_appl_callback_entry_delete(unit, mac,
                                                                 vlan);
                }
            } else {
                /* Clear hit bits */
                soc_L2Xm_field32_set(unit, l2x_entry, HITDAf, 0);
                soc_L2Xm_field32_set(unit, l2x_entry, HITSAf, 0);
                soc_L2Xm_field32_set(unit, l2x_entry, LOCAL_SAf, 0);

                /* Update hit bits. Table is already locked */
                rv = soc_mem_write(unit, L2Xm, MEM_BLOCK_ALL, chnk_idx + idx,
                                   (void *)l2x_entry);
                if (SOC_FAILURE(rv)) {
                    soc_mem_unlock(unit, L2Xm);

                    /* We do not return error, otherwise thread will be
                     * killed. If thread is alive it can be debugged
                     */
                    LOG_WARN(BSL_LS_SOC_L2,
                             (BSL_META_U(unit,
                             "%s:soc mem write failed: %s\n"), FUNCTION_NAME(),
                             soc_errmsg(rv)));

                     return SOC_E_NONE;
                }
            }

            /* When an entry is deleted, soc_th3_l2x_shadow_callback will
             * call learn shadow upadate function. So we don't call learn
             * shadow update function here
             */
        } /* End processing entries within current chunk */

        soc_mem_unlock(unit, L2Xm);
    } /* End processing current chunk */


    return SOC_E_NONE;
}

/*
 * Function:
 * 	_soc_th3_l2_age
 * Purpose:
 *   	Handler function for L2 entry aging thread
 * Parameters:
 *	unit - unit number
 * Returns:
 *	none
 */
STATIC void
_soc_th3_l2_age(void *unit_ptr)
{
    int unit = PTR_TO_INT(unit_ptr);
    int c, m, r, rv, iter = 0;
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t interval;
    sal_usecs_t stime, etime;
    l2x_entry_t *buffer;
    int num_entries = _SOC_TH3_AGE_L2_TABLE_DMA_CHUNK_SIZE;

    /* Allocate memory to accomodate L2X table chunk */
    buffer = soc_cm_salloc(unit,
                           sizeof(l2x_entry_t) * num_entries, "L2Xm_age");

    if (buffer == NULL) {
        LOG_ERROR(BSL_LS_SOC_L2, (BSL_META_U(unit, "_soc_th3_l2_age: "
                                             "Memory alloc failed, size %d\n"),
                                             (int)(sizeof(l2x_entry_t) *
                                             num_entries)));

        goto cleanup_exit;
    }

    while((interval = soc->l2x_age_interval) != 0) {
        if (!iter) {
            goto age_delay;
        }

        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "l2_age_thread: "
                                "Process iters(total:%d, this run:%d\n"),
                     ++_soc_th3_l2_bulk_age_iter[unit], iter));

        stime = sal_time_usecs();

        if (!soc->l2x_age_enable) {
            goto age_delay;
        }

        if (soc_mem_index_count(unit, L2Xm) == 0) {
            goto cleanup_exit;
        }

        rv = _soc_th3_l2_age_entries_process(unit, buffer, num_entries);

        if (!SOC_SUCCESS(rv)) {
            goto cleanup_exit;
        }

        etime = sal_time_usecs();
        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "l2_bulk_age_thread: unit=%d: done in %d usec\n"),
                     unit, SAL_USECS_SUB(etime, stime)));
age_delay:
        rv = -1; /* timeout */
        if (interval > 2147) {
            m = (interval / 2147) * 1000;
            r = (interval % 2147) * 1000000;
            for (c = 0; c < m; c++) {
                rv = sal_sem_take(soc->l2x_age_notify, 2147000);
                /* age interval is changed */
                if (rv == 0 || interval != soc->l2x_age_interval) {
                    break;
                }
            }
            /* age interval is changed */
            if (soc->l2x_age_interval &&
                (rv == 0 || interval != soc->l2x_age_interval)) {
                interval = soc->l2x_age_interval;
                goto age_delay;
            } else if (r) {
                 /* age interval is not changed */
                (void)sal_sem_take(soc->l2x_age_notify, r);
            }
        } else {
            rv = sal_sem_take(soc->l2x_age_notify, interval * 1000000);
            /* age interval is changed */
            if (soc->l2x_age_interval &&
                (rv == 0 || interval != soc->l2x_age_interval)) {
                interval = soc->l2x_age_interval;

                goto age_delay;
            }
        }
        iter++;
    }

cleanup_exit:
    if (buffer != NULL) {
        soc_cm_sfree(unit, buffer);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "l2_age_thread: exiting\n")));
    soc->l2x_age_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);

 /*   return; */
}

/*
 * Function:
 * 	soc_th3_l2_age_start
 * Purpose:
 *   	Start L2 aging thread
 * Parameters:
 *	unit - unit number
 * Returns:
 *	SOC_E_XXX
 */
int
soc_th3_l2_age_start(int unit, int interval)
{
    int cfg_interval;
    soc_control_t *soc = SOC_CONTROL(unit);

    cfg_interval = soc_property_get(unit, spn_L2_SW_AGING_INTERVAL, 
                                    SAL_BOOT_QUICKTURN ? 30 : 10);
    SOC_CONTROL_LOCK(unit);

    soc->l2x_age_interval = interval ? interval : cfg_interval;
    sal_snprintf(soc->l2x_age_name, sizeof (soc->l2x_age_name),
                 "bcmL2age.%d", unit);

    soc->l2x_age_pid = sal_thread_create(soc->l2x_age_name, SAL_THREAD_STKSZ,
                                         soc_property_get(unit,
                                             spn_L2AGE_THREAD_PRI, 50),
                                         _soc_th3_l2_age, INT_TO_PTR(unit));

    if (soc->l2x_age_pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "soc_th3_l2_age_start: Could not start"
                              " L2 aging thread\n")));

        SOC_CONTROL_UNLOCK(unit);

        return SOC_E_MEMORY;
    }

    SOC_CONTROL_UNLOCK(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_th3_l2_age_stop
 * Purpose:
 *   	Stop l2 aging thread
 * Parameters:
 *	unit - unit number
 * Returns:
 *	SOC_E_XXX
 */
int
soc_th3_l2_age_stop(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int           rv = SOC_E_NONE;
    soc_timeout_t to;

    SOC_CONTROL_LOCK(unit);
    soc->l2x_age_interval = 0;  /* Request exit */
    SOC_CONTROL_UNLOCK(unit);

    if (soc->l2x_age_pid && (soc->l2x_age_pid != SAL_THREAD_ERROR)) {
        /* Wake up thread so it will check the exit flag */
        sal_sem_give(soc->l2x_age_notify);

        /* Give thread a few seconds to wake up and exit */
        if (SAL_BOOT_SIMULATION) {
            soc_timeout_init(&to, 300 * 1000000, 0);
        } else {
            soc_timeout_init(&to, 60 * 1000000, 0);
        }

        while (soc->l2x_age_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "thread will not exit\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 * 	soc_th3_l2_lrn_cache_entry_invalidate
 * Purpose:
 *  This function clears a specific L2 learn cache entry from a given pipe.
 *  It will be called during learning process to clear entries after they are 
 *  learned
 * Parameters:
 *	unit - unit number
 *  pipe - pipe number (0 based)
 *  entry - entry index with learn cache (0 based)
 * Returns:
 *	SOC_E_XXX
 */
STATIC
int soc_th3_l2_lrn_cache_entry_invalidate(int unit, int pipe, int entry)
{
    soc_mem_t mem;
 
    if ((pipe < 0) || (pipe > (NUM_PIPE(unit) - 1))) {
        return SOC_E_PARAM;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, L2_LEARN_CACHEm)[pipe];

    if ((entry < soc_mem_index_min(unit, mem)) ||
        (entry > soc_mem_index_max(unit, mem))) {
        return SOC_E_PARAM;
    }

    soc_mem_lock(unit, mem);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, entry,
                                      soc_mem_entry_zeroes(unit, mem)));
    soc_mem_unlock(unit, mem);

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_th3_l2_learn_cache_clear
 * Purpose:
 *  This function clears L2 learn cache. All copies of learn cache (in all
 *  pipes) are cleared  
 * Parameters:
 *	unit - unit number
 * Returns:
 *	SOC_E_XXX
 */
STATIC
int soc_th3_l2_learn_cache_clear(int unit)
{
    int  pipe;
    soc_info_t *si;
    soc_mem_t mem;

    si = &SOC_INFO(unit);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {

        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            continue;
        }

        mem = SOC_MEM_UNIQUE_ACC(unit, L2_LEARN_CACHEm)[pipe];

        soc_mem_lock(unit, mem);

        SOC_IF_ERROR_RETURN(soc_mem_clear(unit, mem, MEM_BLOCK_ALL, TRUE));

        soc_mem_unlock(unit, mem);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_th3_l2_learn_cache_status_clear
 * Purpose:
 *  This function clears L2 learn cache status registers. All copies of learn
 *  cache (in all pipes) are cleared. These are sticky bits and need to be
 *  explicitly cleared by software during init or shutdown of L2 module
 * Parameters:
 *	unit - unit number
 * Returns:
 *	SOC_E_XXX
 */
STATIC
int soc_th3_l2_learn_cache_status_clear(int unit)
{
    int  pipe;
    soc_reg_t reg = INVALIDr;
    uint32 rval = 0;

    reg = SOC_REG_UNIQUE_ACC(unit, L2_LEARN_CACHE_STATUSr)[0];
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, L2_LEARN_CACHE_FULLf, 0x0);
    soc_reg_field_set(unit, reg, &rval, L2_LEARN_CACHE_THRESHOLD_EXCEEDEDf,
                      0x0);
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        reg = SOC_REG_UNIQUE_ACC(unit, L2_LEARN_CACHE_STATUSr)[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	_soc_th3_l2_learn_cache_status_check_clear
 * Purpose:
 *  This function checks status of one or more status bits in a pipe's
 *  L2 learn cache status register for the specified pipe. If any bit is set,
 *  it clears the bit(s). These are sticky bits and need to be explicitly
 *  cleared by software
 * Parameters:
 *	unit     - Unit number
 *	pipe     - Pipe whose status bit needs to be cleared
 *	fld_ptr  - Pointer to an array of one or more fields which are to be cleared
 *	num_flds - Size of fld_ptr array
 * Returns:
 *	SOC_E_XXX
 *	Notes:
 *	Caller must provide correct pipe number and correct field value(s)
 */
STATIC
int _soc_th3_l2_learn_cache_status_check_clear(int unit, int pipe,
                                               soc_field_t *fld_ptr,
                                               int num_flds)
{
    soc_reg_t reg = INVALIDr;
    uint32 rval = 0;
    uint32 bit_val;
    int i;
    int clear;

    reg = SOC_REG_UNIQUE_ACC(unit, L2_LEARN_CACHE_STATUSr)[pipe];

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    clear = FALSE;

    for (i = 0; i < num_flds; i++) {

        /* Check if a status bit is set. If so clear it, else check next bit */
        bit_val = soc_reg_field_get(unit, reg, rval, fld_ptr[i]);

        if (bit_val) {
            soc_reg_field_set(unit, reg, &rval, fld_ptr[i], 0x0);
            clear = TRUE;
        }
    }

    /* Program register only if atleast one bit was modified */
    if (clear == TRUE) {
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_th3_l2_learn_cache_read
 * Purpose:
 *  This function reads all L2 learn cache entries for a given pipe
 * Parameters:
 *	unit   - unit number
 *	pipe   - pipe to read from (range: 0-7)
 *	buffer - Buffer filled by memory read operation
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	Caller must do range check and provide correct pipe number
 */
STATIC
int soc_th3_l2_learn_cache_read(int unit, int pipe, uint32 *buffer)
{
    soc_mem_t mem;
    uint32 index_min, index_max;
    int rv;

    rv = SOC_E_NONE;

    mem = SOC_MEM_UNIQUE_ACC(unit, L2_LEARN_CACHEm)[pipe];

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    /* Read learn cache entries from the specified pipe */
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            index_min, index_max, buffer);

    /* Explicitly clear learn cache for rev A0 */
    if (SOC_CONTROL(unit)->lrn_cache_clr_on_rd &&
        (rev_id == BCM56980_A0_REV_ID)) {
        if (rv == SOC_E_NONE) {
            rv = soc_mem_clear(unit, mem, MEM_BLOCK_ALL, FALSE);
        }
    }

    soc_mem_unlock(unit, mem);

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                   "%s:DMA read failed: %s\n"), FUNCTION_NAME(), soc_errmsg(rv)));
    }

    return rv;
}

/*
 * Function:
 * 	_soc_th3_learn_avl_compare_key
 * Purpose:
 *  Comparison function for AVL shadow table operations
 * Parameters:
 *	user_data - User supplied data reuired by AVL library
 *	datum1    - First data item to compare
 *	datum2    - Second data item to compare
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	None
 */
STATIC int
_soc_th3_learn_avl_compare_key(void *user_data,
                               shr_avl_datum_t *datum1,
                               shr_avl_datum_t *datum2)
{
     soc_l2_lrn_avl_info_p k1, k2;

    /* COMPILER_REFERENCE(user_data);*/
    k1 = (soc_l2_lrn_avl_info_p)datum1;
    k2 = (soc_l2_lrn_avl_info_p)datum2;

    SOC_MEM_COMPARE_RETURN(k1->vlan, k2->vlan);

    return ENET_CMP_MACADDR(k1->mac, k2->mac);
}

/*
 * Function:
 * 	soc_th3_lrn_shadow_insert
 * Purpose:
 *  This function is used to insert an entry in to learn shadow table,
 *  corresponding to the hardware L2 entry added before calling this function.
 *  Since there is no relevance of L2 multicast, static entries and
 *  vlan cross connect entries for learning process we ignore these entry types.
 *  See Notes
 * Parameters:
 *	unit - Switch unit #
 *	l2x_entry_t - Entry to be inserted in to AVL tree
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	Caller _must_ lock mutex l2x_lrn_shadow_mutex before calling this function
 */
int
soc_th3_lrn_shadow_insert(int unit, l2x_entry_t *entry)
{
    soc_l2_lrn_avl_info_t k;
    int rv;

    /* If shadow memory is freed, or not set up, do nothing */
    if (SOC_CONTROL(unit)->l2x_lrn_shadow == NULL) {
        return SOC_E_NONE;
    }

    /* If entry is not valid, do not insert in to shadow table */
    if (!soc_mem_field32_get(unit, L2Xm, entry, BASE_VALIDf)) {
        return SOC_E_NONE;
    }

    /* Ignore static entry */
    if (soc_L2Xm_field32_get(unit, entry, STATIC_BITf)) {
        return SOC_E_NONE;
    }

    /* Do not add single cross connect entries, since they are niether learned,
     * nor aged
     */
    if (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf) !=
                            TH3_L2_HASH_KEY_TYPE_BRIDGE) {
        return SOC_E_NONE;
    }

    sal_memset(&k, 0x0, sizeof(k));
    soc_mem_mac_addr_get(unit, L2Xm, entry, MAC_ADDRf, k.mac);

    /* Do not add multicast entries */
    if (SOC_TH3_MAC_IS_MCAST(k.mac)) {
        return SOC_E_NONE;
    }

    k.vlan = soc_mem_field32_get(unit, L2Xm, entry, VLAN_IDf);
    k.dest_type = soc_mem_field32_get(unit, L2Xm, entry, Tf);
    k.port_tgid = soc_mem_field32_get(unit, L2Xm, entry, DESTINATIONf);
    /* Entry has already been added to h/w, so we set in_hw to 'true' */
    k.in_hw = TRUE;

    rv = shr_avl_insert(SOC_CONTROL(unit)->l2x_lrn_shadow,
                        _soc_th3_learn_avl_compare_key,
                        (shr_avl_datum_t *)&k);

    /* We do not return error since normally there will always be space for a
     * new entry to add in the tree. If this were not the case, mem insert/write
     * called before invoking this function will fail. Also, the full condition
     * may be cleared by software replace mechanism, or aging, or application
     * deleting L2 entries. Also hardware h/w has already been updated at this
     * point
     */
    if (rv == -1) {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                     "shr_avl_insert - tree full\n")));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_th3_lrn_shadow_delete
 * Purpose:
 *  This function deletes an entry from learn shadow table,
 *  after the hardware L2 entry is deleted. Since there is no relevance of L2
 *  multicast, static entries and vlan cross connect entries for learning
 *  process we ignore these entry types. See Notes
 * Parameters:
 *	unit - Switch unit #
 *	l2x_entry_t - Entry to be inserted in to AVL tree
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	Caller _must_ lock mutex l2x_lrn_shadow_mutex before calling this function
 */
int
soc_th3_lrn_shadow_delete(int unit, l2x_entry_t *entry)
{
    soc_l2_lrn_avl_info_t k;
    int rv;

    /* If shadow memory is freed, or not set up, don't do anything */
    if (SOC_CONTROL(unit)->l2x_lrn_shadow == NULL) {
        return SOC_E_NONE;
    }

#if 0
    /* If entry is not valid, do not insert in to shadow table */
    if (!soc_mem_field32_get(unit, L2Xm, entry, BASE_VALIDf)) {
        return SOC_E_NONE;
    }
#endif

    /* Ignore static entry */
    if (soc_L2Xm_field32_get(unit, entry, STATIC_BITf)) {
        return SOC_E_NONE;
    }

    /* Ignore single cross connect entries, since they are niether learned,
     * nor aged
     */
    if (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf) !=
                            TH3_L2_HASH_KEY_TYPE_BRIDGE) {
        return SOC_E_NONE;
    }

    sal_memset(&k, 0x0, sizeof(k));
    soc_mem_mac_addr_get(unit, L2Xm, entry, MAC_ADDRf, k.mac);

    /* Ignore multicast entries */
    if (SOC_TH3_MAC_IS_MCAST(k.mac)) {
        return SOC_E_NONE;
    }

    k.vlan = soc_mem_field32_get(unit, L2Xm, entry, VLAN_IDf);
    k.dest_type = soc_mem_field32_get(unit, L2Xm, entry, Tf);
    k.port_tgid = soc_mem_field32_get(unit, L2Xm, entry, DESTINATIONf);
    /* Entry has already been deleted from h/w, so we set in_hw to 'false' */
    k.in_hw = FALSE;

    rv = shr_avl_delete(SOC_CONTROL(unit)->l2x_lrn_shadow,
                            _soc_th3_learn_avl_compare_key,
                            (shr_avl_datum_t *)&k);
    if (rv == 0) {
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
            "shr_avl_delete: Did not find datum\n")));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_th3_lrn_shadow_show
 * Purpose:
 *	Debug display function for AVL learn shadow table
 * Parameters:
 *	user_data - Used to pass StrataSwitch unit #
 *	datum - AVL node to display
 *	extra_data - Unused
 * Returns:
 *	SOC_E_XXX
 */

int
soc_th3_lrn_shadow_show(void *user_data, shr_avl_datum_t *datum, void *extra_data)
{
    int		unit = PTR_TO_INT(user_data);
    soc_l2_lrn_avl_info_p k = (soc_l2_lrn_avl_info_p)datum;

    COMPILER_REFERENCE(extra_data);

            BSL_LOG(BSL_LSS_CLI, (BSL_META_U(unit, "dest_type: %d, port_tgid: %d, mod: %d, in_hw: %d \n"),k->dest_type, k->port_tgid, k->mod, k->in_hw));
            BSL_LOG(BSL_LSS_CLI, (BSL_META_U(unit, "mac(in hex) %02X:%02X:%02X:%02X:%02X:%02X, vlan: %d\n"),k->mac[0], k->mac[1], k->mac[2], k->mac[3], k->mac[4], k->mac[5], k->vlan));
    LOG_CLI((BSL_META_U(unit,
                        "----------------------------------------\n")));

    return SOC_E_NONE;
}

/*
 * Function:
 * 	_soc_th3_learn_do_lookup
 * Purpose:
 *  This function searches shadow table for matching key, and sets passed
 *  arguments accordingly
 * Parameters:
 *	unit(IN)      - Device unit number
 *	k(IN/OUT)     - key items to search for. The AVL library updates other
 *                  fields of this structure, if key is found
 *	found(OUT)    - Set to true if item is found, else set to false
 *	stn_move(OUT) - Set to true if station move condition is detected,
 *                  else set to false
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	None
 */
STATIC int
_soc_th3_learn_do_lookup(int unit,
                         soc_l2_lrn_avl_info_p k,
                         int *found,
                         int *stn_move)
{

    int result;
    int port_tgid;
    int dest_type;

    /* Save port number obtained from hardware
     * Note AVL lookup overwrites 'k' completely, if the entry was found
     */
    port_tgid = k->port_tgid;
    dest_type = k->dest_type;

    sal_mutex_take(SOC_CONTROL(unit)->l2x_lrn_shadow_mutex, sal_mutex_FOREVER);

    result = shr_avl_lookup(SOC_CONTROL(unit)->l2x_lrn_shadow,
                 _soc_th3_learn_avl_compare_key, (shr_avl_datum_t *)k);

    sal_mutex_give(SOC_CONTROL(unit)->l2x_lrn_shadow_mutex);

    /* Check if a matching node was found in AVL tree. If so, check if the
     * entry has been added to L2 table in h/w. If so, then do nothing.
     * If not, the entry needs to be flagged for programming in h/w
     */
    *found = FALSE;
    *stn_move = FALSE;
    if (result) {

        /* Entry found */
        *found = TRUE;

        /* If entry is found, check for station move */
        *stn_move = ((k->dest_type == dest_type) && (k->port_tgid == port_tgid)) ? FALSE : TRUE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * 	_soc_th3_learn_cache_entry_process
 * Purpose:
 *  This function processes each entry from the learn cache. It check if the
 *  if the entry is present in learn shadow table. If entry is not found, it is
 *  a new L2 flow, so its added programmed in to main L2 table. Learn cache
 *  is updated after addition.
 * Parameters:
 *	unit  - Unit number of device
 *	pipe  - Pipe in which the entry was detected (range: 0-7)
 *	entry - Learn cache entry from the pipe
 *  index - Location of entry within the learn cache (range 0-15)
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	None
 */
STATIC int
_soc_th3_learn_cache_entry_process(int unit,
                                   int pipe,
                                   l2_learn_cache_entry_t *entry,
                                   int entry_idx)
{
    int rv;
    int found;
    int stn_move;
    soc_mem_t mem;
    soc_l2_lrn_avl_info_t k;
    int invalidated = FALSE;
    int curr_l2_table_entries;
    int max_l2_table_entries;
    int l2copyno;

    max_l2_table_entries = soc_mem_index_count(unit, L2Xm);
    l2copyno = SOC_MEM_BLOCK_ANY(unit, L2Xm);

    mem = SOC_MEM_UNIQUE_ACC(unit, L2_LEARN_CACHEm)[pipe];

    sal_memset(&k, 0x0, sizeof(k));

    soc_mem_mac_addr_get(unit, mem, entry, MAC_ADDRf, k.mac);
    k.vlan = soc_mem_field32_get(unit, mem, entry, VLAN_IDf);
    k.dest_type = soc_mem_field32_get(unit, mem, entry, DEST_TYPEf);
    k.port_tgid = soc_mem_field32_get(unit, mem, entry, DESTINATIONf);
    k.in_hw = FALSE;

    rv = _soc_th3_learn_do_lookup(unit, &k, &found, &stn_move);

    /* Check if learn interrupt needs to be disabled during processing below */
    if (rv == SOC_E_NONE) {
        l2x_entry_t l2x_entry;
        soc_port_t port_tgid = 0;
        soc_field_t field = INVALIDf;

        if (SOC_CONTROL(unit)->lrn_cache_clr_on_rd) {
            /* Check if entry was added to h/w by previous learn cache entry */
            /* Duplicate entries can result only in clear on read mode */
            if (found == TRUE) {
                if (k.in_hw == TRUE) {
                    /* In case of station move, L2X entry is already present in
                     * h/w (in_hw is true); we should not invalidate the first
                     * learn cache entry here (for station move), without
                     * checking station move condition (station move is handled
                     * later in this function)
                     */
                    if (stn_move == FALSE) {
                        /* Entry cleared by h/w in clr-on-rd mode, so we do not
                         * explicitly invalidate the entry here
                         */
                        /* SOC_IF_ERROR_RETURN(
                            soc_th3_l2_lrn_cache_entry_invalidate(unit, pipe,
                                                                  entry_idx)); */
                        LOG_INFO(BSL_LS_SOC_L2,
                            (BSL_META_U(unit, "Duplicate lrn cache entry:"
                            " pipe %d, index %d\n"), pipe, entry_idx));

                        return rv;
                    }
                } else {
                    /* If k.in_hw is FALSE, it means h/w was
                     * updated, but software table entry was not, which should
                     * never happen. It may point to software table corruption,
                     * or a problem arising out of table write sequence.
                     * Tables should always be in sync
                     */
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "%s: S/w"
                                  " entry %d, in pipe %d out of sync with"
                                  " h/w\n"), FUNCTION_NAME(), entry_idx, pipe));
                    return SOC_E_INTERNAL;
                }
            }
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, BASE_VALIDf, 0x1);
        soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf, k.vlan);
        soc_L2Xm_mac_addr_set(unit, &l2x_entry, MAC_ADDRf, k.mac);
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                                 TH3_L2_HASH_KEY_TYPE_BRIDGE);
        if (k.dest_type) {
            soc_L2Xm_field32_set(unit, &l2x_entry, Tf, 0x1);
        }

        if (found == FALSE) {
            /* Replace original port with new port */
            field = k.dest_type ? TGIDf : PORT_NUMf;
            soc_L2Xm_field32_set(unit, &l2x_entry, field, k.port_tgid);
            soc_L2Xm_field32_set(unit, &l2x_entry, HITSAf, 1);

            /* Insert L2 entry in h/w */
            soc_mem_lock(unit, L2Xm);
            curr_l2_table_entries = SOP_MEM_STATE(unit, L2Xm).count[l2copyno];

            /* If there is no space in the L2 table, do not issue insert. Note
             * that current L2 table size is dynamically changing; entries can
             * be added/deleted though other sources like application thread
             * (using L2 APIs), cmd shell, other internal SDK modules and so on.
             * So the current table enttries is only a tentative (but closer to
             * accurate) value
             */
            rv = SOC_E_NONE;
            if ((curr_l2_table_entries >= 0) &&
                (curr_l2_table_entries < max_l2_table_entries)) {
                rv = soc_mem_insert(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
            }
            soc_mem_unlock(unit, L2Xm);

            /* AVL tree will be updated through soc_th3_l2x_shadow_callback */

            if ((rv == SOC_E_FULL) || (rv == SOC_E_EXISTS) ||
                (rv == SOC_E_NOT_FOUND)) {
                /* If full, exist or not found conditions are encountered, we
                 * simply log an error. If we return error, learn thread will
                 * exit. We don't want the thread to exit based on certain
                 * 'conditions', or search result, since it will stop learning
                 * altogether. Full condition can get cleared later on by aging,
                 * deletions by application(s), or by s/w replace operation.
                 */
                LOG_INFO(BSL_LS_SOC_L2,
                            (BSL_META_U(unit, "%s: soc_mem_insert retval %d\n"),
                            FUNCTION_NAME(), rv));
                rv = SOC_E_NONE;
            } else {
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
            }

            /* Clear entry in learn cache only if clr on rd is not enabled */
            if (!(SOC_CONTROL(unit)->lrn_cache_clr_on_rd)) {
                SOC_IF_ERROR_RETURN(soc_th3_l2_lrn_cache_entry_invalidate(unit,
                                        pipe, entry_idx));
            }

            invalidated = TRUE;
        }

        /* Handle station move */
        if ((stn_move == TRUE) && (found == TRUE)) {
            soc_mem_lock(unit, L2Xm);
            rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
            if (SOC_FAILURE(rv)) {
                soc_mem_unlock(unit, L2Xm);
                if (rv == SOC_E_NOT_FOUND) {
                    /* If entry is not found, log an error. Do not return this
                     * error code since it is not a critical/fatal error. If
                     * same error code is returned, the learn thread will exit
                     * and learning will stop
                     */
                    LOG_INFO(BSL_LS_SOC_L2,
                                (BSL_META_U(unit, "%s: soc_mem_delete"
                                 " retval %d\n"), FUNCTION_NAME(), rv));
                    rv = SOC_E_NONE;
                }

                return rv;
            }

            /* Replace original port with new port */
            port_tgid = soc_mem_field32_get(unit, mem, entry, DESTINATIONf);
            /* k.dest_type, k.port_tgid are overwritten by
             * _soc_th3_learn_do_lookup, so we get k.dest_type from learn cache
             * again (k.port_tgid is not used here though)
             */
            k.dest_type = soc_mem_field32_get(unit, mem, entry, DEST_TYPEf);
            field = k.dest_type ? TGIDf : PORT_NUMf;
            if (k.dest_type) {
                soc_L2Xm_field32_set(unit, &l2x_entry, Tf, 0x1);
            } else {
                soc_L2Xm_field32_set(unit, &l2x_entry, Tf, 0x0);
            }
            soc_L2Xm_field32_set(unit, &l2x_entry, field, port_tgid);
            soc_L2Xm_field32_set(unit, &l2x_entry, HITSAf, 1);

            rv = soc_mem_insert(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
            soc_mem_unlock(unit, L2Xm);

            if ((rv == SOC_E_FULL) || (rv == SOC_E_EXISTS) ||
                (rv == SOC_E_NOT_FOUND)) {
                /* If full, exist or not found conditions are encountered, we
                 * simply log an error. If we return error, learn thread will
                 * exit. We don't want the thread to exit based on certain
                 * 'conditions', or search result, since it will stop learning
                 * altogether. Full condition can get cleared later on by aging,
                 * deletions by application(s), or by s/w replace operation.
                 */
                LOG_INFO(BSL_LS_SOC_L2,
                            (BSL_META_U(unit, "%s: soc_mem_insert retval %d\n"),
                            FUNCTION_NAME(), rv));
                rv = SOC_E_NONE;
            } else {
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
            }


            /* AVL tree will be updated through soc_th3_l2x_shadow_callback */

            /* Clear entry in learn cache only if clr on rd is not enabled */
            if (!(SOC_CONTROL(unit)->lrn_cache_clr_on_rd)) {
                SOC_IF_ERROR_RETURN(soc_th3_l2_lrn_cache_entry_invalidate(unit,
                                        pipe, entry_idx));
            }
            invalidated = TRUE;
        }
    }

    /* This case should not happen, added here as a precaution to avoid
     * full condition for learn cache due to unprocessed entries (if
     * this condition is reached, it means the entry was not processed
     * earlier)
     */
    if (invalidated == FALSE) {
        LOG_INFO(BSL_LS_SOC_L2,
                    (BSL_META_U(unit, "%s: Entry %d in pipe %d not processed,"
                     " removing it from lrn cache\n"), FUNCTION_NAME(),
                     entry_idx, pipe));

        SOC_IF_ERROR_RETURN(soc_th3_l2_lrn_cache_entry_invalidate(unit,
                                pipe, entry_idx));
    }

    return rv;
}

/*
 * Function:
 *      _soc_th3_lrn_cache_intr_configure
 * Purpose:
 *      This function is used to enable or disable L2 learn cache interrupt
 *      generation
 * Parameters:
 *      unit   - SOC unit #
 *      bit    - Bit number corresponding to a pipe. Bit 8 is for pipe 0 and
 *               bit 15 is for pipe 7. See Notes.
 *      enable - To enable interrupt, use 1 or a non-zero value.
 *               To disable interrupt, use 0.
 * Returns:
 *     SOC_E_XXX
 * Notes:
 *     This function uses hard-coded values for interrupt numbers/bit positions.
 *     Also it assumes fixed number of bits (1 per pipe), contiguous numbering,
 *     and fixed bit positions for each pipe; so it is not portable
 */
STATIC int
_soc_th3_lrn_cache_intr_configure(int unit, int bit, int enable)
{
    int rv = SOC_E_NONE;
    uint32 regval = 0;
    soc_reg_t reg = ICFG_CHIP_LP_INTR_ENABLE_REG1r;

    /* Accept only bits 8-15 (both numbers included) */
    if ((bit < 8) || (bit > 15)) {
        return SOC_E_INTERNAL;
    }

    rv = soc_iproc_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                          &regval);
    if (rv == SOC_E_NONE) {
        if (enable) {
            regval |= 1 << bit;
        } else {
            regval &= ~(1 << bit);
        }

        rv = soc_iproc_setreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                              regval);
    }

    return rv;
}

/*
 * Function:
 *      soc_th3_lrn_cache_intr_handler
 * Purpose:
 *      Interrupt handler for (per-pipe) learn cache (fifo) interrupt
 * Parameters:
 *      unit  - SOC unit #
 *      data - Data used by the isr, initialized during interrupt registration
 * Returns:
 *     Nothing
 */
void
soc_th3_lrn_cache_intr_handler(int unit, void *data)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int i;

    if (soc->l2x_lrn_mode == L2_LRN_MODE_INTR) {
        /* Disable learn cache interrupts from all pipes */
        for (i = 8; i <= 15; i++) {
            (void)_soc_th3_lrn_cache_intr_configure(unit, i, 0);
        }

        /* Signal lrn thread of learn event(s) */
        sal_sem_give(soc->arl_notify);
    }

    /* If we see interrupt in polled mode, then there is some misconfiguration.
     * In this case, check L2_LEARN_COPY_CACHE_CTRL's interrupt control bit
     */

    return;
}

/*
 * Function:
 *     soc_th3_l2_learn_alloc_resources
 * Purpose:
 *     This function is used to create learn shadow table memory and mutex for
 *     safe access to the shadow table. It is called during L2 initialization
 * Parameters:
 *     u - Pointer to unit number
 * Returns:
 *     SOC_E_NONE on success
 *     Other SOC_E_* codes on error
 */
int
soc_th3_l2_learn_alloc_resources(int unit)
{
    /* Create AVL table and semaphore used for L2 learning */
    if (SOC_CONTROL(unit)->l2x_lrn_shadow != NULL) {

        if (shr_avl_destroy(SOC_CONTROL(unit)->l2x_lrn_shadow) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "%d: Error calling shr_avl_destroy\n"), unit));

            return SOC_E_INTERNAL;
        }

        SOC_CONTROL(unit)->l2x_lrn_shadow = NULL;
    }

    if (shr_avl_create(&SOC_CONTROL(unit)->l2x_lrn_shadow, INT_TO_PTR(unit),
           sizeof(soc_l2_lrn_avl_info_t), _SOC_TH3_L2_LRN_TBL_SIZE) < 0) {

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "%d: Error calling shr_avl_create\n"), unit));

            return SOC_E_MEMORY;
    }

    if ((SOC_CONTROL(unit)->l2x_lrn_shadow_mutex =
            sal_mutex_create("L2AvlMutex")) == NULL) {

        if (SOC_CONTROL(unit)->l2x_lrn_shadow != NULL) {
            shr_avl_destroy(SOC_CONTROL(unit)->l2x_lrn_shadow);
            SOC_CONTROL(unit)->l2x_lrn_shadow = NULL;
        }

        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit, "%d: Error calling sal_mutex_create for"
            " L2 AVL Mutex\n"), unit));

        return SOC_E_MEMORY;
    }

    LOG_INFO(BSL_LS_SOC_L2,
                (BSL_META_U(unit, "%d: %s: Created"
                     " shadow table and mutex\n"), unit, FUNCTION_NAME()));

    return SOC_E_NONE;
}

/*
 * Function:
 *     _soc_th3_l2_learn_process
 * Purpose:
 *     This function is the main handler for learn thread. It will be used by
 *     learn thread during learning. It will read learn cache entries, perform
 *     look-ups in shadow (AVL) table during station move, and write to it
 *     after an L2 entry is learned
 * Parameters:
 *     u - Pointer to unit number
 * Returns:
 *     Nothing
 */
STATIC void
_soc_th3_l2_learn_process(void *u)
{
    int unit = PTR_TO_INT(u);
    soc_control_t *soc = SOC_CONTROL(unit);
    int interval;
    void *buffer;
    uint32 index_min;
    int count;
    int num_bytes;
    int rv;
    int pipe;
    soc_mem_t mem;
    int i;
    int valid;

    LOG_INFO(BSL_LS_SOC_L2,
                (BSL_META_U(unit, "%d: In _soc_th3_l2_learn_process\n"), unit));

    mem = SOC_MEM_UNIQUE_ACC(unit, L2_LEARN_CACHEm)[0];
    num_bytes = soc_mem_entry_bytes(unit, mem);
    count = soc_mem_index_count(unit, mem);
    index_min = soc_mem_index_min(unit, mem);

    buffer = soc_cm_salloc(unit, num_bytes * count, "L2_LEARN_CACHEm");

    if (buffer == NULL) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_L2X_LEARN, __LINE__,
                           SOC_E_MEMORY);
        goto cleanup_exit;
    }

    if (soc->l2x_lrn_mode == L2_LRN_MODE_INTR) {
        /* Enable learn cache interrupts for all pipes */
        for (i = 8; i <= 15; i++) {
            (void)_soc_th3_lrn_cache_intr_configure(unit, i, 1);
        }
    }

    while((interval = soc->l2x_learn_interval)) {

        uint32 sts_reg = 0;
        uint32 en_reg = 0;
        uint32 mask = 0;
        uint32 poll_all_pipes = 1;

        if (soc->l2x_lrn_mode == L2_LRN_MODE_INTR) {
            soc_reg_t reg = INVALIDr;
            uint32 shift = 0;

            sal_sem_take(soc->arl_notify, interval);

            /*
             * We read the interrupt status here, and process the pipes whose
             * learn cache has reached or exceeded threshold. While processing,
             * if other pipes raise interrupts, we will process them in the
             * next cycle
             */
            reg = ICFG_CHIP_LP_INTR_RAW_STATUS_REG1r;

            rv = soc_iproc_getreg(unit,
                                  soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                                  &sts_reg);

            if (SOC_SUCCESS(rv)) {
                reg = ICFG_CHIP_LP_INTR_ENABLE_REG1r;

                rv = soc_iproc_getreg(unit,
                                      soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                                      &en_reg);
            }

            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "Failed to read register"
                    " %s, rv = %d\n"), SOC_REG_NAME(unit, reg), rv));

                goto cleanup_exit;
            }

            shift = 8; /* L2 learn interrupt bits start at bit 8 */

            /* Create mask to pick the set of learning bits */
            mask = (1U << NUM_PIPE(unit)) - 1;
            mask <<= shift;

            /* Clear out intr bits other than those that are for learning */
            en_reg &= mask;
            sts_reg &= mask;

            /* Select intr bits which are currently disabled by the isr
             * (soc_th3_lrn_cache_intr_handler), since those are the ones that
             *  need to be serviced. (Note currently we reset all bits in the
             * isr to simplify interrupt processing)
             */
            sts_reg &= ~en_reg;
            sts_reg >>= shift;

            /* Process all pipes if there was no interrupt during learn
             * interval. This way we handle pipes which have entries, but the
             * threshold has not reached, so the interrupt is not generated
             * (for those pipes)
             */
            poll_all_pipes = !sts_reg ? 1 : 0;
        }

        /* The system is in warmboot phase. We do not do any learn processing
         * until we are out of warmboot
         */
        if (SOC_WARM_BOOT(unit)) {
            goto skip_processing;
        }

        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            int full_cleared;
            int thr_cleared;
            /* Number of valid learn cache entries processed successfully */
            int processed_cnt;
            soc_info_t *si = &SOC_INFO(unit);

            /* For half-pipe configuration, this check has been added */
            if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
                continue;
            }

            /* The system is in warmboot phase. We do not do any learn
             * processing until we are out of warmboot
             */
            if (SOC_WARM_BOOT(unit)) {
                goto skip_processing;
            }

            full_cleared = FALSE;
            thr_cleared = FALSE;

            /* Count the number of valid entries processed, and use it to
             * compare to threshold value
             */
            processed_cnt = 0;

            if (soc->l2x_lrn_mode == L2_LRN_MODE_INTR) {
                /* sts_reg has bits set for pipes whose caches need
                 * to be processed
                 * If we are not polling all pipes, then we should check
                 * individual bits in the sts_reg bitmap (corresponding to each
                 * pipe), and process entries in that pipe (interrupt was
                 * asserted for pipes in sts_reg bitmap)
                 */
                /* If we shouldn't process all pipes, then specific pipes in the
                 * bitmap are the ones which need to be serviced
                 */
                if ((!poll_all_pipes) && (!(sts_reg & (1U << pipe)))) {
                    continue;
                }
            }

            /* Read cache entries for the specified pipe */
            rv = soc_th3_l2_learn_cache_read(unit, pipe, buffer);

            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                                   SOC_SWITCH_EVENT_THREAD_L2X_LEARN,
                                   __LINE__, rv);
                goto cleanup_exit;
            }

            /* Process each learn cache entry */
            for (i = index_min; i < (index_min + count); i++) {
                l2_learn_cache_entry_t *entry;

                if (!soc->l2x_learn_interval) {
                    goto cleanup_exit;
                }

                /* The system is in warmboot phase. We do not do any learn
                 * processing until we are out of warmboot
                 */
                if (SOC_WARM_BOOT(unit)) {
                    goto skip_processing;
                }

                /* Point to the next entry in the buffer */
                entry = (l2_learn_cache_entry_t *)((uint8 *)buffer +
                                                   i * num_bytes);

                mem = SOC_MEM_UNIQUE_ACC(unit, L2_LEARN_CACHEm)[pipe];

                valid = soc_mem_field32_get(unit, mem, entry, VALIDf);

                /* Process valid entries only */
                if (valid) {
                    LOG_DEBUG(BSL_LS_SOC_L2,
                              (BSL_META_U(unit, "%s: Valid entry in pipe %d, index %d\n"),
                               FUNCTION_NAME(), pipe, i));

                    rv = _soc_th3_learn_cache_entry_process(unit, pipe, entry, i);
                    if (SOC_FAILURE(rv)) {
                        /* In case of failure, we do not exit the thread, since
                         * learning will stop altogether. Assumption is that
                         * the error may be transitory in nature
                         */
                        LOG_INFO(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Failed to add entry"
                            " in pipe %d, index %d, rv = %d\n"),
                            pipe, i, rv));
                        continue;
                    }

                    if (soc->l2x_lrn_mode == L2_LRN_MODE_INTR) {
                        processed_cnt++;

                        /* Check if cache full condition exists. If so, clear
                         * it only once in this processing cycle, for each pipe,
                         * after 1st entry is processed.  (If cache fills again,
                         * we will handle it in the next interrupt handling
                         * cycle)
                         */
                        if (full_cleared == FALSE) {
                            soc_field_t fld = L2_LEARN_CACHE_FULLf;

                            rv = _soc_th3_l2_learn_cache_status_check_clear(
                                     unit, pipe, &fld, 1);

                            if (SOC_FAILURE(rv)) {
                                LOG_ERROR(BSL_LS_SOC_COMMON,
                                    (BSL_META_U(unit, "Cache full bit"
                                    " could not be cleared in pipe %d,"
                                    " index %d, rv = %d\n"),
                                    pipe, i, rv));
                                goto cleanup_exit;
                            }

                            full_cleared = TRUE;
                        }

                        if (thr_cleared == FALSE) {
                            /* Clear thresold exceeded bit if number of valid
                             * entries processed crossed the programmed
                             * threshold value. If the threshold is set to 1,
                             * we have already processed the entry, so we
                             * immediately clear threshold bit
                             */
                            if ((processed_cnt > soc->lrn_cache_threshold) ||
                                (soc->lrn_cache_threshold == 1)) {

                                soc_field_t fld =
                                            L2_LEARN_CACHE_THRESHOLD_EXCEEDEDf;

                                rv = _soc_th3_l2_learn_cache_status_check_clear(
                                         unit, pipe, &fld, 1);

                                if (SOC_FAILURE(rv)) {
                                    LOG_ERROR(BSL_LS_SOC_COMMON,
                                        (BSL_META_U(unit, "Threshold exc. bit"
                                        " could not be cleared in pipe %d,"
                                        " index %d, rv = %d\n"),
                                        pipe, i, rv));
                                    goto cleanup_exit;
                                }

                                thr_cleared = TRUE;
                            }
                        }
                    }
                }
            }
        }

skip_processing:
        if (soc->l2x_lrn_mode == L2_LRN_MODE_INTR) {
            /* Enable learn cache interrupts for all pipes */
            for (i = 8; i <= 15; i++) {
                (void)_soc_th3_lrn_cache_intr_configure(unit, i, 1);
            }
        } else {
            sal_usleep(interval);
        }
    }

cleanup_exit:
/*
    Check if this is required
    if (SOC_CONTROL(unit)->l2x_lrn_shadow != NULL) {
        shr_avl_destroy(SOC_CONTROL(unit)->l2x_lrn_shadow);
        SOC_CONTROL(unit)->l2x_lrn_shadow = NULL;
    }

    if (SOC_CONTROL(unit)->l2x_lrn_shadow_mutex != NULL) {
        sal_mutex_destroy(SOC_CONTROL(unit)->l2x_lrn_shadow_mutex);
        SOC_CONTROL(unit)->l2x_lrn_shadow_mutex = NULL;
    }
*/

    if (buffer != NULL) {
        soc_cm_sfree(unit, buffer);
    }
    soc->l2x_learn_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*
 * Function:
 * 	soc_th3_l2_learn_start
 * Purpose:
 *   	Start l2 learn thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 * soc_th3_l2_learn_alloc_resources must be called before calling this function
 * for the _first_ time
 */
int
soc_th3_l2_learn_thread_start(int unit, int interval)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 reg_val = 0;
    int pri = SOC_TH3_LRN_THREAD_PRI_DEFAULT;

    if (soc->l2x_learn_interval != 0) {
        SOC_IF_ERROR_RETURN(soc_th3_l2_learn_thread_stop(unit));
    }

    SOC_CONTROL_LOCK(unit);
    sal_snprintf(soc->l2x_learn_name, sizeof (soc->l2x_age_name), "L2Lrn.%d",
                 unit);

    if (soc->l2x_learn_pid == SAL_THREAD_ERROR) {
        soc_th3_l2x_lrn_mode_t mode;

        soc_cm_get_id(unit, &dev_id, &rev_id);

        if (soc_property_get(unit, spn_L2XLRN_INTR_EN,
                             SOC_TH3_LRN_CACHE_INTR_CTL_DEFAULT)) {
            mode = L2_LRN_MODE_INTR;
        } else {
            mode = L2_LRN_MODE_POLL;
        }

        /* Always polled mode for simulation */
        if (SAL_BOOT_BCMSIM) {
            mode = L2_LRN_MODE_POLL;
        }

        soc->l2x_lrn_mode = mode;

        /* Do not use clear-on-read by default, unless user wants it */
        soc->lrn_cache_clr_on_rd = soc_property_get(unit,
                                       spn_L2XLRN_CLEAR_ON_READ,
                                       SOC_TH3_LRN_CACHE_CLR_ON_RD_DEFAULT);

        soc->l2x_learn_interval = interval;

        if (interval == 0) {
            SOC_CONTROL_UNLOCK(unit);
            return SOC_E_NONE;
        }

        /* Set initial values for learn cache operation */
        SOC_IF_ERROR_RETURN(READ_L2_LEARN_COPY_CACHE_CTRLr(unit, &reg_val));

        soc_reg_field_set(unit, L2_LEARN_COPY_CACHE_CTRLr, &reg_val,
                          L2_LEARN_CACHE_ENf, 0x1);
        soc_reg_field_set(unit, L2_LEARN_COPY_CACHE_CTRLr, &reg_val,
                          CLEAR_ON_READ_ENf,
                          (uint32)(soc->lrn_cache_clr_on_rd));

        if (soc->l2x_lrn_mode == L2_LRN_MODE_INTR) {
            /*
             * 1. Set default interrupt threshold (from soc property, or soc)
             * 2. Mark all cache entries as 'invalid' - done below this 'if'
             * 3. Enable l2 learn cache interrupt.
             */
            soc->lrn_cache_threshold = soc_property_get(unit,
                                           spn_L2XLRN_INTR_THRESHOLD,
                                           SOC_TH3_LRN_CACHE_THRESHOLD_DEFAULT);

            /* A value of 0 or less is illegal. Also a value above 16 is
             * illegal, flag error
             */
            if ((soc->lrn_cache_threshold <= 0) ||
                (soc->lrn_cache_threshold > 16)) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                          "soc_th3_l2_learn_start: Illegal value of intr"
                          " threshold: %d\n"), soc->lrn_cache_threshold));
                return SOC_E_CONFIG;
            }

            /* By default, generate interrupt only when # cache entries equals
             * the programmed threshold value. For generating interrupt on each
             * learn event, user may set spn_L2XLRN_INTR_THRESHOLD to 1 
             */
            soc->lrn_cache_intr_ctl = 0x0;

            soc_reg_field_set(unit, L2_LEARN_COPY_CACHE_CTRLr, &reg_val,
                              CACHE_INTERRUPT_CTRLf,
                              (uint32)(soc->lrn_cache_intr_ctl));

            soc_reg_field_set(unit, L2_LEARN_COPY_CACHE_CTRLr, &reg_val,
                              CACHE_INTERRUPT_THRESHOLDf,
                              (uint32)(soc->lrn_cache_threshold));

        } else {
           soc->lrn_cache_intr_ctl = 0x0;

           /* In polled mode, set these fields to reset values */
           soc_reg_field_set(unit, L2_LEARN_COPY_CACHE_CTRLr, &reg_val,
                             CACHE_INTERRUPT_CTRLf,
                             (uint32)(soc->lrn_cache_intr_ctl));

           soc_reg_field_set(unit, L2_LEARN_COPY_CACHE_CTRLr, &reg_val,
                             CACHE_INTERRUPT_THRESHOLDf, 0x8);

        }

        SOC_IF_ERROR_RETURN(WRITE_L2_LEARN_COPY_CACHE_CTRLr(unit, reg_val));

        /* Clear all entries of L2 learn cache */
        SOC_IF_ERROR_RETURN(soc_th3_l2_learn_cache_clear(unit));

        /* Reset cache status bits */
        SOC_IF_ERROR_RETURN(soc_th3_l2_learn_cache_status_clear(unit));

        pri = soc_property_get(unit, spn_L2XLRN_THREAD_PRI,
                               SOC_TH3_LRN_THREAD_PRI_DEFAULT);

        /* Make sure that learn cache interrupt is enabled in CMICx,
         * later in the main initialization sequence
         */
        soc->l2x_learn_pid = sal_thread_create(soc->l2x_learn_name,
                                               SAL_THREAD_STKSZ, pri,
                                               _soc_th3_l2_learn_process,
                                               INT_TO_PTR(unit));

        if (soc->l2x_learn_pid == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                           "soc_th3_l2_learn_start: Could not start L2 learn"
                           " thread\n")));
            SOC_CONTROL_UNLOCK(unit);
            return SOC_E_MEMORY;
        }
    }

    SOC_CONTROL_UNLOCK(unit);

    /* More programming might be reqd depending on learn cache en/dis setting */

    return SOC_E_NONE;
}


int
soc_th3_l2_learn_thread_stop(int unit)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    soc_timeout_t   to;
    sal_usecs_t interval;

    LOG_INFO(BSL_LS_SOC_ARL, (BSL_META_U(unit, "Stopping learn"
                                         " thread: unit=%d\n"), unit));

   /* Save interval to wait for thread to wake up again */
   if (SAL_BOOT_SIMULATION) {
        /* Allow more time on simulation, similar to other devices */
        interval = 30 * 1000000;
    } else {
        interval = 10 * 1000000;
    }

    SOC_CONTROL_LOCK(unit);
    soc->l2x_learn_interval = 0;  /* Request exit */
    SOC_CONTROL_UNLOCK(unit);

    if (soc->l2x_learn_pid != SAL_THREAD_ERROR) {

        if (soc->l2x_lrn_mode == L2_LRN_MODE_INTR) {
            int i;

            /* Disable learn cache interrupts from all pipes */
            for (i = 8; i <= 15; i++) {
                (void)_soc_th3_lrn_cache_intr_configure(unit, i, 0);
            }
        }

        /* Wake up thread so it will check the exit flag */
        /*sal_sem_give(soc->arl_notify); Check if notification to learn thread
          is required */

        /* Give thread a few seconds to wake up and exit */
        soc_timeout_init(&to, interval, 0);

        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit, "Learn thread stop: Wait may be longer if"
                 " cfg polling interval is high, cfg interval = %u\n"),
                 interval));

        while (soc->l2x_learn_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit, "Learn thread did not stop\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    return (rv);
}

int
soc_th3_l2_learn_thread_running(int unit, sal_usecs_t* interval)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    if (soc->l2x_learn_pid != SAL_THREAD_ERROR) {
        if (interval != NULL) {
            *interval = soc->l2x_learn_interval;
        }
    }

    return(soc->l2x_learn_pid != SAL_THREAD_ERROR);
}
#endif /* BCM_XGS_SWITCH_SUPPORT */
#endif /* BCM_TOMAHAWK3_SUPPORT */
