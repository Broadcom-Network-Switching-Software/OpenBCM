/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file multi_set.h
 *
 * Definitions and prototypes for all common utilities related to multi set.
 *
 * A multi-set is essentially a hash table with control over
 * the number of duplications (reference count) both per member and
 * over the total number of duplications.
 */
#ifndef MULTI_SET_H_INCLUDED
/** { */
#define MULTI_SET_H_INCLUDED

/*************
* INCLUDES  *
 */
/*
 * {
 */

/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
/*
 * }
 */

/*************
 * DEFINES   *
 */

/**
 * Add a element with a given ID.
 */
#define MULTI_SET_ADD_WITH_ID   DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID

/**********************************************/
/************ MULTI SET MACROS ****************/
/**********************************************/
/**
 * {
 */

/**
 * }
 */
/*************
 * MACROS    *
 */
/** { */
/**
 * Reset the multiset iterator to point to the beginning of the multiset
 */
#define MULTI_SET_ITER_BEGIN(iter) (iter = 0)
/**
 * Check whether the multiset iterator has reached the end of the multiset
 */
#define MULTI_SET_ITER_END(iter)   (iter == UTILEX_U32_MAX)
/**
 * Verify specific multi set is active. If not, software goes to
 * exit with error code.
 */
#define MULTI_SET_VERIFY_MULTISET_IS_ACTIVE(_multi_set) \
  { \
    if (_multi_set == NULL) { \
      SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL) ; \
    } \
  }
/**
 * Verify specified unit has a legal value. If not, software goes to
 * exit with error code.
 *
 * Notes:
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define MULTI_SET_VERIFY_UNIT_IS_LEGAL(_unit) \
  if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    SHR_IF_ERR_EXIT(_SHR_E_UNIT) ; \
  }

/** } */

/*************
 * TYPE DEFS *
 */
/** { */

/**
 * Type of the multiset key. Key is, essentially, an array of MULTI_SET_KEY.
 */
typedef uint8 MULTI_SET_KEY;
/**
 * Type of the multiset data
 */
typedef uint8 *MULTI_SET_DATA;
/**
 * Type of iterator over the multiset. Use this type to traverse the multiset.
 */
typedef uint32 MULTI_SET_ITER;
/**
 * Note: MULTI_SET_PTR is just a handle to the 'multi set'
 * structure (actually, index into 'multis_array' {of pointers})
 *
 * Note that the name is kept as is to minimize changes in current code relative
 * to the original which has been ported.
 */
typedef uint32 MULTI_SET_PTR;

/**
 * Includes the information user needs to supply for multiset creation
 * \see multi_set_create()
 */
typedef struct
{
    /**
     * Number of different members can be added to the set.
     * A member can be any values in the range
     * 0,1,...,nof_members - 1
     */
    uint32 nof_members;
    /**
     * Size of a member (in bytes). For this implementation, this is the size of the key
     * on the corresponding hash table.
     */
    uint32 member_size;
    /**
     * The maximum duplications/occurrences of a member in the multi_set.
     * A member is not removed till
     * all the 'add' operation reverted with 'remove' operation
     * i.e. # removes = # add.
     */
    uint32 max_duplications;
    /**
     * Procedures to move entries to/from software DB. Not used
     * on this implementation.
     */
    char *print_cb_name;
    /**
     * First profile id of the template.
     */
    int first_profile;
    /**
     * Size of the template's data.
     */
    int data_size;
} multi_set_info_t;

/**
 * Includes the information software updates for a LIVE multiset while various
 * operations (such as 'add' or 'remove') are applied.
 * \see sw_state_multimet_info_t
 */
typedef struct
{
    /**
     * Array to include reference counting of the used members.
     */
    DNX_SW_STATE_BUFF *ref_counter;
    /**
     * The size of the reference counter in bytes. This is
     * the number of bytes used for storage of ref_counter. It
     * is calculated using 'max_duplications'. \see multi_set_create()
     */
    uint32 counter_size;
    /**
     * Size of the key (in bytes)
     */
    uint32 key_size;
    /**
     * Size of the data (in bytes)
     */
    uint32 data_size;
    /**
     * Hash table structure.
     */
    sw_state_htbl_t hash_table;
    /**
     * Includes the information user needs to supply for multiset creation
     */
    multi_set_info_t init_info;
    /**
     * For use by smart templates, this resource_tag_bitmap will manage the entry allocation for the template.
     */
    resource_tag_bitmap_t allocation_bitmap;
}  *multi_set_t;

/** } */

/*************
* GLOBALS   *
 */
/** { */

/** } */

/*************
* FUNCTIONS *
 */
/** { */

/**
 * \brief - Creates a new Multi set instance.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - Multi set to be created.
 * \param [in] init_info - Pointed memory contains setup parameters required for for the creation of the hash table.
 * \param [in] extra_arguments - Pointed memory to hold extra arguments required for creating this multiset.
 *  Not used and is only there to be generic with template manager advanced algorithm prototype callback.
 * \param [in] alloc_flags - SW state allocation flags.
 *        This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e multi_set_create(
    int unit,
    uint32 module_id,
    multi_set_t * multi_set,
    dnx_algo_template_create_data_t * init_info,
    void *extra_arguments,
    uint32 alloc_flags);

/**
 * \brief - Free the multiset instance
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - Pointer to multi set to be destroyed.
 * \param [in] extra_arguments - Pointed memory to hold extra arguments required for destroying this multiset.
 *   Not used and is only there to be generic with template manager advanced algorithm prototype callback.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e multi_set_destroy(
    int unit,
    uint32 module_id,
    multi_set_t * multi_set,
    void *extra_arguments);

/**
 * \brief - Add nof members to the mutli-set to either a specific index or a new index.
 *  If member already exists then update the reference counter of this member.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] flags - Relevant allocation flags - see MULTI_SET_ADD_WITH_ID.
 * \param [in] key - The member to add.
 * \param [in,out] data_indx - *data_indx is loaded by the place of the added member.
 *       If *data_indx is set to UTILEX_U32_MAX then a new location is automatically assigned for the newly added member (and it is loaded into *data_indx as output)
 *       Otherwise, this is the location identifying an existing member which should have the same key.
 * \param [in] nof_additions - Declare nof_additions to add the given key. If given UTILEX_U32_MAX, the maximum number of entries will be added.
 * \param [in] first_appear - This procedure loads pointed memory by a zero value if a member corresponding to the specified 'key' already exists.
 * \param [in] extra_arguments - Pointed memory to hold extra arguemnts required for creating this multiset.
 * \return
 *   shr_error_e
 *     This may be Fail,
 *       a. If there was no more space available in the multi-set
 *       b. If an attempt was make to load more instances (ref_count), on a member, than allowed.
 *       c. If 'nof_additions' is zero
 *       d. If number of instances per member (ref_count) is '1' and caller has pointed
 *          a specific index (via *data_indx) and the same key already exists for another index.
 *       e. Trying to set a ref_count which is too high (larger than number of bits assigned,
 *          larger than 'max_duplications' (maximal ref_count per member).
 *     Note that if caller is trying to set ref_count larger than 'global_max' (maximal total
 *     ref_counter), then an error is returned (_SHR_E_FULL).
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e multi_set_member_add(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    uint32 flags,
    int nof_additions,
    void *key,
    void *extra_arguments,
    int *data_indx,
    uint8 *first_appear);

/**
 * \brief - Like remove single member by index but done multiple times (without using a loop).
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] data_indx - This is the identifying index of the new member to add.
 * \param [in] remove_amount - How many members should be removed.
 * \param [out] last_appear - This procedure loads pointed memory by a zero value if a
 *       member corresponding to the specified 'key' already exists.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e multi_set_member_remove_by_index_multiple(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    int data_indx,
    int remove_amount,
    uint8 *last_appear);

/**
 * \brief - Like remove single member by index but done multiple times (without using a loop).
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] key - The key to lookup.
 * \param [out] data_indx - index identifying this member place.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e multi_set_member_lookup(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    const void *key,
    int *data_indx);

/**
 * \brief - get the next valid entry (key and data) in the multiset.
 *  start traversing from the place pointed by the given iterator.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] iter - iterator points to the entry to start traverse from.
 * \param [in] key - The key to lookup.
 * \param [out] data_indx - index identifying this member place.
 * \param [out] ref_count - The occurrences/duplications of this member in the multi-set.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e multi_set_get_next(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    MULTI_SET_ITER * iter,
    MULTI_SET_KEY * key,
    uint32 *data_indx,
    uint32 *ref_count);

/**
 * \brief - Get the number of occurences of an index (zero if does not exist).
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] data_indx - Data index.
 * \param [out] ref_count - The occurrences/duplications of this member in the multi-set.
 * \param [out] key - the multiset key returned.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e multi_set_get_by_index(
    int unit,
    uint32 module_id,
    multi_set_t multi_set,
    int data_indx,
    int *ref_count,
    void *key);

/**
 * \brief - Clear the Multiset without freeing the memory.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] multi_set - The mutli-set to clear.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e multi_set_clear(
    int unit,
    uint32 module_id,
    multi_set_t multi_set);

/** } */

#endif /* MULTI_SET_H_INCLUDED */
