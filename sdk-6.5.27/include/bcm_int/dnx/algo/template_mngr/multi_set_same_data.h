/**
 * \file multi_set_same_data.h
 */
#ifndef MULTI_SET_SAME_DATA_H_INCLUDED
#define MULTI_SET_SAME_DATA_H_INCLUDED

#include <bcm_int/dnx/algo/template_mngr/multi_set.h>

/**
 * \brief Multi set with same data profiles.
 *
 * This template manager advanced algorithm is used to allocate template manager profiles
 * in an advanced manner by using a built in resource manager and a multi head linked list. The idea of the algorithm
 * is to allow multiple profiles with same data, when the profiles are in different ranges.
 *
 */

/**
 * \brief
 * Pass this struct when allocating entries to provide specific parameters for the allocation.
 */
typedef struct
{
    uint32 range_start;
    uint32 range_end;
} multi_set_same_data_alloc_info_t;

/**
 * \brief
 *   Create a new template manager.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] node_id - Node Id.
 *  \param [in] multi_set - Mutli set template.
 *  \param [in] create_data -
 *      Pointed memory contains setup parameters required for the
 *      creation of the template.
 *  \param [in] extra_arguments - Pointed memory to hold extra arguments required for creating this multiset.
 *  \param [in] alloc_flags - SW state allocation flags.
 *       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e multi_set_same_data_create(
    int unit,
    uint32 node_id,
    multi_set_t * multi_set,
    dnx_algo_template_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags);

/**
 * \brief
 *   Callback to allocate a free profile of an advanced algorithm.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] node_id - Node Id.
 *  \param [in] multi_set - Mutli set template.
 *  \param [in] flags - SW_STATEALGO_TEMPLATE_ALLOCATE_* flags
 *  \param [in] nof_references - Number of references to be allocated.
 *  \param [in] data - Pointer to memory holding template data to be saved to a profile.
 *  \param [in] extra_arguments -
 *      A pointer to multi_set_same_data_alloc_info_t. See \ref resource_tag_bitmap_create for
 *      more info on the inner struct.
 *  \param [in,out] profile -
 *      Pointer to place the allocated profile.
 *      \b As \b output - \n
 *        Holds the allocated profile.
 *      \b As \b input - \n
 *        If flag \ref DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
 *  \param [in,out] first_appear -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of the profile's prior existence. \n
 *        If TRUE, this is the first reference to the profile, and the data needs to be
 *          written to the relevant table.
 *
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e multi_set_same_data_member_alloc(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    uint32 flags,
    int nof_references,
    void *data,
    void *extra_arguments,
    int *profile,
    uint8 *first_appear);

/**
 * \brief
 *   Callback to free an allocated profile in an advanced algorithm.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] node_id - Node Id.
 *  \param [in] multi_set - Mutli set template.
 *  \param [in] profile - Profile to be freed.
 *  \param [in] nof_references - Number of references to be freed.
 *  \param [in,out] last_appear -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of whether it's the last reference to this profile.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e multi_set_same_data_member_free(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    int profile,
    int nof_references,
    uint8 *last_appear);

/**
 * \brief
 *   Callback to exchange profile/s for the same_data advanced algorithm.
 *   The exchange is performed by freeing the profile/s from old_profile and allocating them in new_profile.
 *  \param [in] unit - Relevant unit.
 *  \param [in] node_id - Node Id.
 *  \param [in] multi_set - Mutli set template.
 *  \param [in] flags - Exchange and allocation flags.
 *  \param [in] nof_references - Number of references to be exchanged.
 *  \param [in] profile_data - Pointer to memory holding template data to be saved to a profile. Not in use if IGNORE_DATA flag is set.
 *      \b As \b input - \n
*         Fill with the data to be saved.
 *  \param [in] old_profile - Profile to exchange the existing reference/s from.
 *  \param [in] extra_arguments -
 *      A pointer to multi_set_same_data_alloc_info_t. See \ref resource_tag_bitmap_create for
 *      more info on the inner struct.
 *  \param [in,out] new_profile - Pointer to place the allocated profile.
 *      \b As \b output - \n
 *        Holds the allocated profile.
 *      \b As \b input - \n
 *        If flag \ref DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
 *  \param [out] first_appear -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of the profile's prior existence. \n
 *        If TRUE, this is the first reference to the profile, and the data needs to be
 *          written to the relevant table.
 *  \param [out] last_appear -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of whether it's the last reference to this profile.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e multi_set_same_data_member_exchange(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    uint32 flags,
    int nof_references,
    const void *profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_appear,
    uint8 *last_appear);

/**
 * \brief - Since in the algorithm we can have multiple profiles with the same data.
 * So this function is not usable(it will return NOT_FOUND error even if the data exists)
 *
 * \param [in] unit - Relevant unit.
 * \param [in] node_id - Node ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] key - The key to lookup.
 * \param [out] profile - index identifying this member place.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e multi_set_same_data_member_lookup(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    const void *key,
    int *profile);

/**
 * \brief
 *   Get the number of occurrences of an index (zero if does not exist).
 *
 * \param [in] unit - Relevant unit.
 * \param [in] node_id - Node ID.
 * \param [in] multi_set - The mutli-set instance.
 * \param [in] profile - Profile index.
 * \param [out] ref_count - The occurrences/duplications of this member in the multi-set.
 * \param [out] data - the profile data returned.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e multi_set_same_data_get_by_index(
    int unit,
    uint32 node_id,
    multi_set_t multi_set,
    int profile,
    int *ref_count,
    void *data);

/**
 * \brief
 *   Clear all template profiles.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] node_id - Node ID.
 * \param [in] multi_set - The mutli-set instance.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e multi_set_same_data_clear(
    int unit,
    uint32 node_id,
    multi_set_t multi_set);

#endif /* MULTI_SET_SAME_DATA_H_INCLUDED */
