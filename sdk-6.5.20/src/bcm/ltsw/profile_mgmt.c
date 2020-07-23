/*! \file profile_mgmt.c
 *
 * Profile management interface.
 * This file contains the management for profile table index.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcm/types.h>

#include <bcm_int/ltsw/profile_mgmt.h>

#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_PROFILE

/*!
 * \brief Profile entry information.
 *
 * This data structure is used to identify the profile entry information.
 */
typedef struct ltsw_profile_entry_s {
    /*! Reference counter. */
    uint32_t   ref_count;
    /*! Hash(signature) of profile set. */
    uint32_t   hash;
    /*! Number of entries in the profile set for master. */
    int        entries_per_set;
} ltsw_profile_entry_t;

/*!
 * \brief Profile table information.
 *
 * This data structure is used to identify the profile table information.
 */
typedef struct ltsw_profile_table_s {
    /*! Master profile handle. */
    bcmi_ltsw_profile_hdl_t    master_hdl;
    /*! Slave profile handle array. */
    bcmi_ltsw_profile_hdl_t    *slave_hdls;
    /*! Number of slave profile handles. */
    int                        slave_count;
    /*! Callback function to calculate the hash(signature) of profile set. */
    bcmi_ltsw_profile_hash_cb  hash_func;
    /*! Callback function to compare profile set. */
    bcmi_ltsw_profile_cmp_cb   cmp_func;
    /*! Profile entry array. */
    ltsw_profile_entry_t       *entries;
    /*! Minimum profile entry index. */
    int                        entry_idx_min;
    /*! Maximum profile entry index. */
    int                        entry_idx_max;
} ltsw_profile_table_t;

/*!
 * \brief Bookkeeping information for profile management.
 */
typedef struct ltsw_profile_bookkeeping_s {
    /*! Pointer to the profile tables. */
    ltsw_profile_table_t  *profile_table;
    /*! Initialized flag. */
    int                   initialized;
    /*! Protection mutex. */
    sal_mutex_t           mutex;
} ltsw_profile_bookkeeping_t;

/* Profile name string for debug usage */
const char *ltsw_profile_str[BCMI_LTSW_PROFILE_COUNT] = {
    "BCMI_LTSW_PROFILE_INVALID",
    "BCMI_LTSW_PROFILE_VLAN",
    "BCMI_LTSW_PROFILE_VLAN_ASSIGN_PROT",
    "BCMI_LTSW_PROFILE_PORT_PKT_CTRL"
};

static
ltsw_profile_bookkeeping_t ltsw_profile_bk_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* Profile information */
#define PROFILE_INFO(u)             (&ltsw_profile_bk_info[u])

/* Profile table information */
#define PROFILE_TABLE_HANDLE(u)     (PROFILE_INFO(u)->profile_table)
#define PROFILE_TABLE(u, id)        (PROFILE_TABLE_HANDLE(u)[id])

/* Profile entry information */
#define PROFILE_ENTRY_HANDLE(u, id) (PROFILE_TABLE((u), (id)).entries)
#define PROFILE_ENTRY(u, id, idx)   (PROFILE_ENTRY_HANDLE((u), (id))[idx])
#define PROFILE_ENTRY_COUNT(u, id)  (PROFILE_TABLE((u), (id)).entry_idx_max - \
                                     PROFILE_TABLE((u), (id)).entry_idx_min + 1)
/* Slave profile information */
#define PROFILE_SLAVE_HANDLE(u, id) (PROFILE_TABLE((u), (id)).slave_hdls)
#define PROFILE_SLAVE(u, id, idx)   (PROFILE_SLAVE_HANDLE((u), (id))[idx])

/* Calculate profile index */
#define PROFILE_INDEX(u, id, num, set) \
    (PROFILE_TABLE((u), (id)).entry_idx_min + (num) * (set))
#define PROFILE_BASE_INDEX(u, id, idx) \
    ((idx) - PROFILE_TABLE((u), (id)).entry_idx_min)

/* Profile Lock */
#define PROFILE_LOCK(u) \
    sal_mutex_take(PROFILE_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define PROFILE_UNLOCK(u) \
    sal_mutex_give(PROFILE_INFO(u)->mutex);

/* Profile entry update flag */
#define PROFILE_ENTRY_F_HASH_UPDATE     (1 << 0) /* Update Hash */
#define PROFILE_ENTRY_F_COUNT_UPDATE    (1 << 1) /* Update ref count */
#define PROFILE_ENTRY_F_SET_INFO_UPDATE (1 << 2) /* Update set info */

/******************************************************************************
* Private functions
 */

/*!
 * \brief Check the validity of profile register.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl_array    Pointer to profile handle array.
 * \param [in]  entry_idx_min_array  Pointer to minimum entry index array.
 * \param [in]  entry_idx_max_array  Pointer to maximum entry index array.
 * \param [in]  array_count          Number of entries in array.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
ltsw_profile_register_valid_check(int unit,
                                  bcmi_ltsw_profile_hdl_t *profile_hdl_array,
                                  int *entry_idx_min_array,
                                  int *entry_idx_max_array, int array_count)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_hdl_array, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_idx_min_array, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_idx_max_array, SHR_E_PARAM);

    for (i = 0; i < array_count; i++) {
        if (PROFILE_ENTRY_HANDLE(unit, profile_hdl_array[i])) {
            /* Has been registered */
            SHR_IF_ERR_EXIT(SHR_E_EXISTS);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the validity of profile set.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 * \param [in]  entries_per_set    Number of entries in the set of master
 * profile table.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
ltsw_profile_set_valid_check(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                             int entries_per_set)
{
    bcmi_ltsw_profile_hdl_t slave_profile_hdl = BCMI_LTSW_PROFILE_INVALID;
    int num_entries = 0;
    int num_sets    = 0;
    int i           = 0;

    SHR_FUNC_ENTER(unit);

    if (PROFILE_TABLE(unit, profile_hdl).master_hdl != profile_hdl) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    num_entries = PROFILE_ENTRY_COUNT(unit, profile_hdl);

    if ((entries_per_set == 0) || (num_entries % entries_per_set)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    num_sets = num_entries / entries_per_set;

    for (i = 0; i < PROFILE_TABLE(unit, profile_hdl).slave_count; i++) {
        slave_profile_hdl = PROFILE_SLAVE(unit, profile_hdl, i);

        num_entries = PROFILE_ENTRY_COUNT(unit, slave_profile_hdl);

        if ((num_sets == 0) || (num_entries % num_sets)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if the profile solt in master is free.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 * \param [in]  set_idx            Index of profile set.
 * \param [in]  entries_per_set    Number of entries in the set of master
 * profile table.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
ltsw_profile_set_free_master_check(int unit,
                                   bcmi_ltsw_profile_hdl_t profile_hdl,
                                   int set_idx, int entries_per_set)
{
    int base = 0;
    int i    = 0;

    SHR_FUNC_ENTER(unit);

    base = entries_per_set * set_idx;

    /* Check if there are free contiguous space for master profile */
    for (i = 0; i < entries_per_set; i++) {
        if (PROFILE_ENTRY(unit, profile_hdl, base + i).ref_count) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if the profile solt in slave is free.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 * \param [in]  set_idx            Index of profile set.
 * \param [in]  entries_per_set    Number of entries in the set of master
 * profile table.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
ltsw_profile_set_free_slave_check(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                                  int set_idx, int entries_per_set)
{
    bcmi_ltsw_profile_hdl_t slave_profile_hdl = BCMI_LTSW_PROFILE_INVALID;
    int base = 0;
    int slave_entries_per_set = 0;
    int i = 0, j = 0;
    int num_sets = 0;

    SHR_FUNC_ENTER(unit);

    num_sets = PROFILE_ENTRY_COUNT(unit, profile_hdl) / entries_per_set;

    /* Check if there are free contiguous space for slave profiles */
    for (i = 0; i < PROFILE_TABLE(unit, profile_hdl).slave_count; i++) {
        slave_profile_hdl = PROFILE_SLAVE(unit, profile_hdl, i);

        slave_entries_per_set =
            PROFILE_ENTRY_COUNT(unit, slave_profile_hdl) / num_sets;
        base = slave_entries_per_set * set_idx;

        for (j = 0; j < slave_entries_per_set; j++) {
            if (PROFILE_ENTRY(unit, slave_profile_hdl, base + j).ref_count) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if the profile solt is free.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 * \param [in]  set_idx            Index of profile set.
 * \param [in]  entries_per_set    Number of entries in the set of master
 * profile table.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
ltsw_profile_set_free_check(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                            int set_idx, int entries_per_set)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_profile_set_free_master_check(unit, profile_hdl,
                                            set_idx, entries_per_set));

    if (PROFILE_TABLE(unit, profile_hdl).slave_count > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_profile_set_free_slave_check(unit, profile_hdl,
                                               set_idx, entries_per_set));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update profile entry information for master.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 * \param [in]  num_sets           Number of profile set.
 * \param [in]  set_idx            Index of profile set.
 * \param [in]  entries_per_set    Number of entries in the set of master
 * profile table.
 * \param [in]  incr_depth         Increased depth.
 * \param [in]  hash               Hash signature of profile set.
 * \param [in]  flags              Update flags(PROFILE_ENTRY_F_XXX).
 *
 * \retval None.
 */
static void
ltsw_profile_entry_master_update(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                                 int set_idx, int entries_per_set,
                                 int incr_depth, uint32_t hash, int flags)
{
    int base = 0;
    int i    = 0;

    base = entries_per_set * set_idx;

    for (i = 0; i < entries_per_set; i++) {
        if (flags & PROFILE_ENTRY_F_HASH_UPDATE) {
            PROFILE_ENTRY(unit, profile_hdl, base + i).hash = hash;
        }
        if (flags & PROFILE_ENTRY_F_SET_INFO_UPDATE) {
            PROFILE_ENTRY(unit, profile_hdl,
                          base + i).entries_per_set = entries_per_set;
        }
        if (flags & PROFILE_ENTRY_F_COUNT_UPDATE) {
            PROFILE_ENTRY(unit, profile_hdl, base + i).ref_count += incr_depth;
            if (PROFILE_ENTRY(unit, profile_hdl, base + i).ref_count == 0) {
                sal_memset(&PROFILE_ENTRY(unit, profile_hdl, base + i),
                           0, sizeof(ltsw_profile_entry_t));
            }
        }
    }

    return;
}

/*!
 * \brief Update profile entry information for slaves.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 * \param [in]  num_sets           Number of profile set.
 * \param [in]  set_idx            Index of profile set.
 * \param [in]  entries_per_set    Number of entries in the set of master
 * profile table.
 * \param [in]  incr_depth         Increased depth.
 * \param [in]  hash               Hash signature of profile set.
 * \param [in]  flags              Update flags(PROFILE_ENTRY_F_XXX).
 *
 * \retval None.
 */
static void
ltsw_profile_entry_slave_update(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                                int set_idx, int entries_per_set,
                                int incr_depth, uint32_t hash, int flags)
{
    bcmi_ltsw_profile_hdl_t slave_profile_hdl = BCMI_LTSW_PROFILE_INVALID;
    int base = 0;
    int slave_entries_per_set = 0;
    int num_sets = 0;
    int i = 0, j = 0;

    num_sets = PROFILE_ENTRY_COUNT(unit, profile_hdl) / entries_per_set;

    for (i = 0; i < PROFILE_TABLE(unit, profile_hdl).slave_count; i++) {
        slave_profile_hdl = PROFILE_SLAVE(unit, profile_hdl, i);

        slave_entries_per_set =
            PROFILE_ENTRY_COUNT(unit, slave_profile_hdl) / num_sets;

        base = slave_entries_per_set * set_idx;

        for (j = 0; j < slave_entries_per_set; j++) {

            if (flags & PROFILE_ENTRY_F_HASH_UPDATE) {
                PROFILE_ENTRY(unit, slave_profile_hdl, base + j).hash = hash;
            }
            if (flags & PROFILE_ENTRY_F_SET_INFO_UPDATE) {
                PROFILE_ENTRY(unit, slave_profile_hdl,
                              base + j).entries_per_set = slave_entries_per_set;
            }
            if (flags & PROFILE_ENTRY_F_COUNT_UPDATE) {
                PROFILE_ENTRY(unit, slave_profile_hdl,
                              base + j).ref_count += incr_depth;
                if (PROFILE_ENTRY(unit, slave_profile_hdl,
                                  base + j).ref_count == 0) {
                    sal_memset(&PROFILE_ENTRY(unit, slave_profile_hdl,
                                              base + j),
                               0, sizeof(ltsw_profile_entry_t));
                }
            }
        }
    }

    return;
}

/*!
 * \brief Update profile entry information.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 * \param [in]  num_sets           Number of profile set.
 * \param [in]  set_idx            Index of profile set.
 * \param [in]  entries_per_set    Number of entries in the set of master
 * profile table.
 * \param [in]  incr_depth         Increased depth.
 * \param [in]  hash               Hash signature of profile set.
 * \param [in]  flags              Update flags(PROFILE_ENTRY_F_XXX).
 *
 * \retval None.
 */
static void
ltsw_profile_entry_update(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                          int set_idx, int entries_per_set,
                          int incr_depth, uint32_t hash, int flags)
{
    ltsw_profile_entry_master_update(unit, profile_hdl, set_idx,
                                     entries_per_set, incr_depth,
                                     hash, flags);

    if (PROFILE_TABLE(unit, profile_hdl).slave_count > 0) {
        ltsw_profile_entry_slave_update(unit, profile_hdl, set_idx,
                                        entries_per_set, incr_depth,
                                        hash, flags);
    }

    return;
}

/*!
 * \brief Check the validity of profile index.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 * \param [in]  entries_per_set    Number of entries in the set of master
 * profile table.
 * \param [in]  index              Base index to the entry in master profile
 * table.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
ltsw_profile_index_valid_check(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                               int entries_per_set, int index)
{
    int base = 0;

    SHR_FUNC_ENTER(unit);

    if (PROFILE_TABLE(unit, profile_hdl).master_hdl != profile_hdl) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (index < PROFILE_TABLE(unit, profile_hdl).entry_idx_min ||
        index > PROFILE_TABLE(unit, profile_hdl).entry_idx_max) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    base = PROFILE_BASE_INDEX(unit, profile_hdl, index);

    if (entries_per_set == -1) {
        entries_per_set =
            PROFILE_ENTRY(unit, profile_hdl, base).entries_per_set;

        if (entries_per_set == 0) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    } else {
        if (entries_per_set == 0) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (base % entries_per_set) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the reference count of profile set.
 *
 * This function is used to update the reference count of profile set at the
 * specified base profile index.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 * \param [in]  entries_per_set    Number of entries in the set of master
 * profile table.
 * \param [in]  index              Base index to the entry in master profile
 * table.
 * \param [in]  depth              Increased depth
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
ltsw_profile_ref_count_update(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                              int entries_per_set, int index, int depth)
{
    int base = 0;
    int set = 0;
    int update_flag = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ltsw_profile_index_valid_check(unit, profile_hdl,
                                        entries_per_set, index));

    base = PROFILE_BASE_INDEX(unit, profile_hdl, index);

    if ((depth < 0) &&
        ((0 - depth) > PROFILE_ENTRY(unit, profile_hdl, base).ref_count)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    set = base / entries_per_set;

    /* Update reference count and entires hash(signature) */
    update_flag = PROFILE_ENTRY_F_COUNT_UPDATE;

    if (PROFILE_ENTRY(unit, profile_hdl, base).entries_per_set == 0) {
        /* Warmboot case */
        update_flag |= PROFILE_ENTRY_F_SET_INFO_UPDATE;
    } else {
       if (entries_per_set !=
           PROFILE_ENTRY(unit, profile_hdl, base).entries_per_set) {
           SHR_IF_ERR_EXIT(SHR_E_PARAM);
       }
    }

    ltsw_profile_entry_update(unit, profile_hdl, set, entries_per_set,
                              depth, 0, update_flag);
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* Public functions
 */

/*!
 * \brief Clear the resource of profile index management.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval None.
 */
void
bcmi_ltsw_profile_cleanup(int unit)
{
    int i = 0;

    /* Destory mutex */
    if (PROFILE_INFO(unit)->mutex) {
        sal_mutex_destroy(PROFILE_INFO(unit)->mutex);
        PROFILE_INFO(unit)->mutex = NULL;
    }

    /* Cleanup the resource of profile table */
    if (PROFILE_TABLE_HANDLE(unit)) {
        for(i = 0; i < BCMI_LTSW_PROFILE_COUNT; i++) {
            if (PROFILE_SLAVE_HANDLE(unit, i)) {
                sal_free(PROFILE_SLAVE_HANDLE(unit, i));
                PROFILE_SLAVE_HANDLE(unit, i) = NULL;
            }

            if (PROFILE_ENTRY_HANDLE(unit, i)) {
                sal_free(PROFILE_ENTRY_HANDLE(unit, i));
                PROFILE_ENTRY_HANDLE(unit, i) = NULL;
            }
        }
    }

    PROFILE_INFO(unit)->profile_table = NULL;
    PROFILE_INFO(unit)->initialized   = FALSE;
}

/*!
 * \brief Initialize the resource of profile index management.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_profile_init(int unit)
{
    int alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    /* Cleanup previous resource */
    if (PROFILE_INFO(unit)->initialized) {
        bcmi_ltsw_profile_cleanup(unit);
    }

    /* Create mutex */
    if (NULL == PROFILE_INFO(unit)->mutex) {
        PROFILE_INFO(unit)->mutex = sal_mutex_create("Profile mutex");
        SHR_NULL_CHECK(PROFILE_INFO(unit)->mutex, SHR_E_MEMORY);
    }

    /* Allocate resource for Profile Tables */
    alloc_size = sizeof(ltsw_profile_table_t) * BCMI_LTSW_PROFILE_COUNT;

    PROFILE_TABLE_HANDLE(unit) = sal_alloc(alloc_size, "Profile Tables");

    SHR_NULL_CHECK(PROFILE_TABLE_HANDLE(unit), SHR_E_MEMORY);

    sal_memset(PROFILE_TABLE_HANDLE(unit), 0, alloc_size);

    PROFILE_INFO(unit)->initialized = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        bcmi_ltsw_profile_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Unregister profile configuration information.
 *
 * This function is used to unregister the configuration information of profile
 * table based on the profile handle.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcmi_ltsw_profile_unregister(int unit, bcmi_ltsw_profile_hdl_t profile_hdl)
{
    bcmi_ltsw_profile_hdl_t slave_profile_hdl = BCMI_LTSW_PROFILE_INVALID;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    if (!PROFILE_INFO(unit)->initialized) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (PROFILE_TABLE(unit, profile_hdl).master_hdl ==
        BCMI_LTSW_PROFILE_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (PROFILE_TABLE(unit, profile_hdl).master_hdl != profile_hdl) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Free the resource of slave profiles */
    for (i = 0; i < PROFILE_TABLE(unit, profile_hdl).slave_count; i++) {
        slave_profile_hdl = PROFILE_SLAVE(unit, profile_hdl, i);

        if (PROFILE_ENTRY_HANDLE(unit, slave_profile_hdl)) {
            sal_free(PROFILE_ENTRY_HANDLE(unit, slave_profile_hdl));
            PROFILE_ENTRY_HANDLE(unit, slave_profile_hdl) = NULL;
        }
        sal_memset(&PROFILE_TABLE(unit, slave_profile_hdl),
                   0, sizeof(ltsw_profile_table_t));
    }

    /* Free the resource of master profile */
    if (PROFILE_SLAVE_HANDLE(unit, profile_hdl)) {
        sal_free(PROFILE_SLAVE_HANDLE(unit, profile_hdl));
        PROFILE_SLAVE_HANDLE(unit, profile_hdl) = NULL;
    }

    if (PROFILE_ENTRY_HANDLE(unit, profile_hdl)) {
        sal_free(PROFILE_ENTRY_HANDLE(unit, profile_hdl));
        PROFILE_ENTRY_HANDLE(unit, profile_hdl) = NULL;
    }

    sal_memset(&PROFILE_TABLE(unit, profile_hdl),
               0, sizeof(ltsw_profile_table_t));
exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if the master profile is registered.
 *
 * This function is used to check if the master profile is registered.
 *
 * \param [in]  unit               Unit Number.
 * \param [in]  profile_hdl        Profile handle for master.
 *
 * \retval Zero                    Profile has not been registered.
 * \retval None-zero               Profile has been registered.
 */
int
bcmi_ltsw_profile_register_check(int unit, bcmi_ltsw_profile_hdl_t profile_hdl)
{
    return (PROFILE_INFO(unit)->initialized &&
           (PROFILE_TABLE(unit, profile_hdl).master_hdl == profile_hdl));
}

/*!
 * \brief Register profile configuration information.
 *
 * This function is used to register the configuration information of profile
 * table based on the profile handle. A profile structure can be formed by
 * single profile table, or a combination of multiple profile tables. The first
 * profile in the combination is called master profile, others can be treated as
 * slave profiles if any.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl_array    Pointer to profile handle array.
 * \param [in]  entry_idx_min_array  Pointer to minimum entry index array.
 * \param [in]  entry_idx_max_array  Pointer to maximum entry index array.
 * \param [in]  array_count          Number of entries in array.
 * \param [in]  hash_func            Callback function to calculate profile
 * set hash(signature).
 * \param [in]  cmp_func             Callback function to compare profile set.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_profile_register(int unit, bcmi_ltsw_profile_hdl_t *profile_hdl_array,
                           int *entry_idx_min_array, int *entry_idx_max_array,
                           int array_count, bcmi_ltsw_profile_hash_cb hash_func,
                           bcmi_ltsw_profile_cmp_cb cmp_func)
{
    int profile_hdl         = 0;
    int entry_idx_min       = 0;
    int entry_idx_max       = 0;
    int alloc_size          = 0;
    int i                   = 0;
    int master_profile_hdl  = 0;

    SHR_FUNC_ENTER(unit);

    if (!PROFILE_INFO(unit)->initialized) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (ltsw_profile_register_valid_check(unit, profile_hdl_array,
                                           entry_idx_min_array,
                                           entry_idx_max_array, array_count));
    for (i = 0; i < array_count; i++) {
        profile_hdl   = profile_hdl_array[i];
        entry_idx_min = entry_idx_min_array[i];
        entry_idx_max = entry_idx_max_array[i];

        if (0 == i) {
            /* Master profile */
            PROFILE_TABLE(unit, profile_hdl).master_hdl = profile_hdl;
            PROFILE_TABLE(unit, profile_hdl).hash_func  = hash_func;
            PROFILE_TABLE(unit, profile_hdl).cmp_func   = cmp_func;

            master_profile_hdl = profile_hdl;

            if (array_count > 1) {
                alloc_size =
                    sizeof(bcmi_ltsw_profile_hdl_t) * (array_count - 1);

                PROFILE_SLAVE_HANDLE(unit, master_profile_hdl) =
                    sal_alloc(alloc_size, "Profile slave ids");

                SHR_NULL_CHECK
                    (PROFILE_SLAVE_HANDLE(unit, master_profile_hdl),
                     SHR_E_MEMORY);

                sal_memset(PROFILE_SLAVE_HANDLE(unit, master_profile_hdl),
                           0, alloc_size);
            }
        } else {
            /* Slave profile */
            PROFILE_TABLE(unit, profile_hdl).master_hdl = master_profile_hdl;
            PROFILE_SLAVE(unit, master_profile_hdl, i - 1) = profile_hdl;
            PROFILE_TABLE(unit, master_profile_hdl).slave_count++;
        }

        alloc_size =
            sizeof(ltsw_profile_entry_t) * (entry_idx_max - entry_idx_min + 1);
        PROFILE_ENTRY_HANDLE(unit, profile_hdl) =
            sal_alloc(alloc_size, "Profile entries");

        SHR_NULL_CHECK(PROFILE_ENTRY_HANDLE(unit, profile_hdl), SHR_E_MEMORY);

        sal_memset(PROFILE_ENTRY_HANDLE(unit, profile_hdl), 0, alloc_size);

        PROFILE_TABLE(unit, profile_hdl).entry_idx_max = entry_idx_max;
        PROFILE_TABLE(unit, profile_hdl).entry_idx_min = entry_idx_min;
    }

exit:

    if (SHR_FUNC_VAL_IS(SHR_E_MEMORY)) {
        /* Rollback processing  */
        (void) bcmi_ltsw_profile_unregister(unit, master_profile_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate profile entry index.
 *
 * This function is used to allocate base index to the entries in profile table
 * based on a profile set(one or more entries). This routine searches for a
 * matching set in the profile table. If a matching set is found, the ref
 * count for the related entries will be incremented and the base entry index
 * of master profile is returned. If a matching set is not found and a free set
 * is found, hash(signature) of the set will be updated, the reference count
 * will be incremented, and the base entry index of the profile set for master
 * will be returned. If no free set is found, an error should be returned.
 * Slave profile index can be derived from master information by profile user
 * itself or call \ref ltsw_profile_slave_index_get optionally.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  profile_entries      Pointer to the profile set memory which is
 * defined by user and can be used for hash calculation and profile set
 * comparison.
 * \param [in]  flags                Operation flags(LTSW_PROFILE_F_XXX). For
 * example, user can disable profile match, and allocate profile index directly.
 * \param [in]  entries_per_set      Number of entries in the set of master
 * profile table.
 * \param [out]  index               Base index to the entries in master profile
 * table.
 *
 * \retval SHR_E_EXISTS              Return an existing profile index.
 * \retval SHR_E_NONE                Return a free profile index.
 * \retval !(SHR_E_NONE|SHR_E_EXISTS)Failure.
 */
int
bcmi_ltsw_profile_index_allocate(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                                 void *profile_entries, int flags,
                                 int entries_per_set, int *index)
{
    int      num_sets      = 0;
    int      free_set      = -1;
    int      set           = 0;
    int      base          = 0;
    int      cmp_result    = -1;
    int      profile_index = 0;
    uint32_t profile_hash  = 0;
    int      update_flag   = 0;
    int      existing_set  = FALSE;
    int      rv            = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    PROFILE_LOCK(unit);

    SHR_IF_ERR_EXIT
        (ltsw_profile_set_valid_check(unit, profile_hdl, entries_per_set));

    /* Calculate entry data hash. */
    if (PROFILE_TABLE(unit, profile_hdl).hash_func) {
        SHR_IF_ERR_EXIT
            (PROFILE_TABLE(unit, profile_hdl).hash_func(
                unit, profile_entries, entries_per_set, &profile_hash));
    }

    num_sets = PROFILE_ENTRY_COUNT(unit, profile_hdl) / entries_per_set;

    /* Traverse all profile sets to find an available set */
    for (set = 0; set < num_sets; set++) {
        base = set * entries_per_set;

        rv = ltsw_profile_set_free_check(unit, profile_hdl,
                                         set, entries_per_set);
        if (rv != SHR_E_EXISTS) {
            /* Preserve location of free slot. */
            if (free_set == -1) {
                free_set = set;
            }

            if ((flags & BCMI_LTSW_PROFILE_F_MATACH_DISABLE) &&
                free_set >= 0) {
                /* Use the first free set directly */
                break;
            }
        } else {
            /* If match disabled, skip match operations */
            if (flags & BCMI_LTSW_PROFILE_F_MATACH_DISABLE) {
                continue;
            }

            /* Skip set of different size */
            if (PROFILE_ENTRY(unit, profile_hdl, base).entries_per_set !=
                entries_per_set) {
                continue;
            }

            /* Compare valid entry hash(signature). */
            if (PROFILE_ENTRY(unit, profile_hdl, base).hash !=
                profile_hash) {
                continue;
            }

            /* If hash matches, compare the entries themselves. */
            if (PROFILE_TABLE(unit, profile_hdl).cmp_func) {
                profile_index = PROFILE_INDEX(unit, profile_hdl,
                                              entries_per_set, set);
                SHR_IF_ERR_EXIT
                    (PROFILE_TABLE(unit, profile_hdl).cmp_func(
                        unit, profile_entries, entries_per_set,
                        profile_index, &cmp_result));

                /* Check comparison result. */
                if (cmp_result == 0) {
                    /* Use existing set if any */
                    free_set     = set;
                    existing_set = TRUE;
                    break;
                }
            }
        }
    }

    if (free_set == -1) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }

    /* Update entry information - reference count and entires hash(signature) */
    update_flag = PROFILE_ENTRY_F_COUNT_UPDATE;
    if (!existing_set) {
        update_flag |= (PROFILE_ENTRY_F_HASH_UPDATE |
                        PROFILE_ENTRY_F_SET_INFO_UPDATE);
    }

    ltsw_profile_entry_update(unit, profile_hdl, free_set, entries_per_set,
                              1, profile_hash, update_flag);

    /* Set base profile index for master */
    *index = PROFILE_INDEX(unit, profile_hdl, entries_per_set, free_set);

    if (existing_set) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

exit:
    PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free profile entry index.
 *
 * This function is used to decrease the reference count of profile set at the
 * specified base profile entry index and try to free the profile index. If the
 * reference count is decreased to ZERO, the index will be freed. If the ref
 * count is greater than ZERO, which means the profile set is still used by
 * other users, the profile index won't be freed and SHR_E_BUSY will be
 * returned accordingly.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 *
 * \retval SHR_E_BUSY                Profile index is still used by others.
 * \retval SHR_E_NONE                Free profile index successfully.
 * \retval !(SHR_E_NONE|SHR_E_BUSY)  Failure.
 */
int
bcmi_ltsw_profile_index_free(int unit,
                             bcmi_ltsw_profile_hdl_t profile_hdl, int index)
{
    int base            = 0;
    int entries_per_set = 0;
    int set             = 0;

    SHR_FUNC_ENTER(unit);
    PROFILE_LOCK(unit);

    SHR_IF_ERR_EXIT
        (ltsw_profile_index_valid_check(unit, profile_hdl, -1, index));

    base = PROFILE_BASE_INDEX(unit, profile_hdl, index);

    if (PROFILE_ENTRY(unit, profile_hdl, base).ref_count == 0) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    entries_per_set = PROFILE_ENTRY(unit, profile_hdl, base).entries_per_set;

    set = base / entries_per_set;

    /* Update reference count */
    ltsw_profile_entry_update(unit, profile_hdl, set, entries_per_set,
                              -1, 0, PROFILE_ENTRY_F_COUNT_UPDATE);

    if (PROFILE_ENTRY(unit, profile_hdl, base).ref_count) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

exit:
    PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get slave profile index based on master profile information.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  master_profile_hdl   Profile handle for master.
 * \param [in]  slave_profile_hdl    Profile handle for slave.
 * \param [in]  master_index         Base index to the entries in master profile
 * table.
 * \param [out] slave_index          Base index to the entries in slave profile
 * table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_profile_slave_index_get(int unit,
                                  bcmi_ltsw_profile_hdl_t master_profile_hdl,
                                  bcmi_ltsw_profile_hdl_t slave_profile_hdl,
                                  int master_index, int *slave_index)
{
    int base = 0;
    int entries_per_set = 0;
    int set = 0;
    int num_sets = 0;
    int slave_entries_per_set = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ltsw_profile_index_valid_check(unit, master_profile_hdl,
                                        -1, master_index));

    base = PROFILE_BASE_INDEX(unit, master_profile_hdl, master_index);

    entries_per_set =
        PROFILE_ENTRY(unit, master_profile_hdl, base).entries_per_set;

    set = base / entries_per_set;

    num_sets = PROFILE_ENTRY_COUNT(unit, master_profile_hdl) / entries_per_set;

    slave_entries_per_set =
        PROFILE_ENTRY_COUNT(unit, slave_profile_hdl) / num_sets;

    *slave_index = PROFILE_INDEX(unit, slave_profile_hdl,
                                 slave_entries_per_set, set);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Increase the reference count of profile set.
 *
 * This function is used to increase the reference count of profile set at the
 * specified base profile entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  entries_per_set      Number of entries in the set of master
 * profile table.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 * \param [in]  depth                Increased depth.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_profile_ref_count_increase(int unit,
                                     bcmi_ltsw_profile_hdl_t profile_hdl,
                                     int entries_per_set,
                                     int index, uint32_t depth)
{
    SHR_FUNC_ENTER(unit);
    PROFILE_LOCK(unit);

    SHR_IF_ERR_EXIT
        (ltsw_profile_ref_count_update(unit, profile_hdl, entries_per_set,
                                       index, depth));
exit:
    PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Decrease the reference count of profile set.
 *
 * This function is used to decrease the reference count of profile set at the
 * specified base profile entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  entries_per_set      Number of entries in the set of master
 * profile table.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 * \param [in]  depth                Increased depth.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_profile_ref_count_decrease(int unit,
                                     bcmi_ltsw_profile_hdl_t profile_hdl,
                                     int entries_per_set,
                                     int index, uint32_t depth)

{
    SHR_FUNC_ENTER(unit);
    PROFILE_LOCK(unit);

    SHR_IF_ERR_EXIT
        (ltsw_profile_ref_count_update(unit, profile_hdl, entries_per_set,
                                       index, 0 - depth));
exit:
    PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the reference count of profile set.
 *
 * This function is used to get the reference count of profile set at the
 * specified base profile entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 * \param [OUT] ref_count            Reference count of profile set.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_profile_ref_count_get(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                                int index, uint32_t *ref_count)
{
    int base = 0;
    int entries_per_set = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ref_count, SHR_E_PARAM);

    if (PROFILE_TABLE(unit, profile_hdl).master_hdl != profile_hdl) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (index < PROFILE_TABLE(unit, profile_hdl).entry_idx_min ||
        index > PROFILE_TABLE(unit, profile_hdl).entry_idx_max) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    base = PROFILE_BASE_INDEX(unit, profile_hdl, index);

    entries_per_set =
        PROFILE_ENTRY(unit, profile_hdl, base).entries_per_set;

    if (entries_per_set == 0) {
        *ref_count = 0;
    } else {
        if (base % entries_per_set) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        *ref_count = PROFILE_ENTRY(unit, profile_hdl, base).ref_count;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update hash(signature) of profile set.
 *
 * This function is used to update hash (signature) of profile set at the
 * specified base profile entry index.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_hdl          Profile handle for master.
 * \param [in]  profile_entries      Pointer to the profile set memory which is
 * defined by user and can be used for hash calculation and profile set
 * comparison.
 * \param [in]  entries_per_set      Number of entries in the set of master
 * profile table.
 * \param [in]  index                Base index to the entries in master profile
 * table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
bcmi_ltsw_profile_hash_update(int unit, bcmi_ltsw_profile_hdl_t profile_hdl,
                              void *profile_entries, int entries_per_set,
                              int index)
{
    int base = 0;
    int set = 0;
    uint32_t profile_hash = 0;
    int update_flag = 0;

    SHR_FUNC_ENTER(unit);
    PROFILE_LOCK(unit);

    SHR_IF_ERR_EXIT
        (ltsw_profile_index_valid_check(unit, profile_hdl,
                                        entries_per_set, index));

    /* Calculate entry data hash. */
    if (PROFILE_TABLE(unit, profile_hdl).hash_func == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (PROFILE_TABLE(unit, profile_hdl).hash_func(unit, profile_entries,
                                                    entries_per_set,
                                                    &profile_hash));

    base = PROFILE_BASE_INDEX(unit, profile_hdl, index);

    set = base / entries_per_set;

    /* Update hash(signature) of profile set */
    update_flag = PROFILE_ENTRY_F_HASH_UPDATE;

    if (PROFILE_ENTRY(unit, profile_hdl, base).entries_per_set == 0) {
        /* Warmboot case */
        update_flag |= PROFILE_ENTRY_F_SET_INFO_UPDATE;
    } else {
       if (entries_per_set !=
           PROFILE_ENTRY(unit, profile_hdl, base).entries_per_set) {
           SHR_IF_ERR_EXIT(SHR_E_PARAM);
       }
    }

    ltsw_profile_entry_update(unit, profile_hdl, set, entries_per_set,
                              0, profile_hash, update_flag);
exit:
    PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump profile information for debug usage.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval None.
 */
void
bcmi_ltsw_profile_sw_dump(int unit)
{
    int i = 0, j = 0;
    int m = 0, n = 0;
    int set      = 0;
    int num_sets = 0;
    int base     = 0;
    int entries_per_set = 0;

    if (!PROFILE_INFO(unit)->initialized || !PROFILE_TABLE_HANDLE(unit)) {
        return;
    }

    /* Dump profile information */
    LOG_CLI((BSL_META_U(unit, "PROFILE INFOMATION DUMP.\n")));

    for (i = 1; i < BCMI_LTSW_PROFILE_COUNT; i++) {

        if (i != PROFILE_TABLE(unit, i).master_hdl) {
            continue;
        }

        /* Master profile */
        LOG_CLI(
            (BSL_META_U(unit, "%4d - Master %s: "), i, ltsw_profile_str[i]));

        LOG_CLI((BSL_META_U(unit, "min_idx = %4d, max_idx = %4d\n"),
                 PROFILE_TABLE(unit, i).entry_idx_min,
                 PROFILE_TABLE(unit, i).entry_idx_max));

        for (j = 0; j < PROFILE_ENTRY_COUNT(unit, i); j++) {
            if (PROFILE_ENTRY(unit, i, j).ref_count == 0) {
                continue;
            }
            LOG_CLI((BSL_META_U(unit, "       entry %4d - ref_count = %4d"),
                     j + PROFILE_TABLE(unit, i).entry_idx_min,
                     PROFILE_ENTRY(unit, i, j).ref_count));
            LOG_CLI((BSL_META_U(unit, "  hash = %12d"),
                     PROFILE_ENTRY(unit, i, j).hash));
            LOG_CLI((BSL_META_U(unit, "  entries_per_set = %2d\n"),
                     PROFILE_ENTRY(unit, i, j).entries_per_set));
        }

        /* Slave profile */
        for (m = 0; m < PROFILE_TABLE(unit, i).slave_count; m++) {
            n = PROFILE_SLAVE(unit, i, m);
            LOG_CLI((BSL_META_U(unit, "%4d - Slave  %s: "),
                     n, ltsw_profile_str[n]));
            LOG_CLI((BSL_META_U(unit, "min_idx = %4d, max_idx = %4d\n"),
                     PROFILE_TABLE(unit, n).entry_idx_min,
                     PROFILE_TABLE(unit, n).entry_idx_max));

            for (j = 0; j < PROFILE_ENTRY_COUNT(unit, n); j++) {
                if (PROFILE_ENTRY(unit, n, j).ref_count == 0) {
                    continue;
                }
                LOG_CLI((BSL_META_U(unit, "       entry %4d - ref_count = %4d"),
                         j + PROFILE_TABLE(unit, n).entry_idx_min,
                         PROFILE_ENTRY(unit, n, j).ref_count));
                LOG_CLI((BSL_META_U(unit, "  hash = %12d"),
                         PROFILE_ENTRY(unit, n, j).hash));
                LOG_CLI((BSL_META_U(unit, "  entries_per_set = %2d\n"),
                         PROFILE_ENTRY(unit, n, j).entries_per_set));

                /* Profile integrity check */
                set = j / PROFILE_ENTRY(unit, n, j).entries_per_set;
                num_sets = PROFILE_ENTRY_COUNT(unit, n) /
                           PROFILE_ENTRY(unit, n, j).entries_per_set;
                entries_per_set = PROFILE_ENTRY_COUNT(unit, i) / num_sets;
                base = set * entries_per_set;

                if (PROFILE_ENTRY(unit, n, j).ref_count !=
                    PROFILE_ENTRY(unit, i, base).ref_count) {
                    LOG_CLI((BSL_META_U(unit,
                             "  mismatched ref_count = %4d, idx = %d\n"),
                             PROFILE_ENTRY(unit, i, base).ref_count, base));
                }

                if (PROFILE_ENTRY(unit, n, j).hash !=
                    PROFILE_ENTRY(unit, i, base).hash) {
                    LOG_CLI((BSL_META_U(unit,
                             "  mismatched hash = %12d, idx = %d\n"),
                             PROFILE_ENTRY(unit, i, base).hash, base));
                }

                if (entries_per_set !=
                    PROFILE_ENTRY(unit, i, base).entries_per_set) {
                    LOG_CLI((BSL_META_U(unit,
                             "  mismatched entries_per_set = %2d, idx = %d\n"),
                             PROFILE_ENTRY(unit, i, base).entries_per_set,
                             base));
                }
            }
        }
    }

    return;
}
