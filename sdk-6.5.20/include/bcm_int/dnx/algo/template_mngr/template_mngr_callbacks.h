/** \file template_mngr_callbacks.h
 * 
 * Internal DNX template manager APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_TEMPLATE_MNGR_CALLBACKS_INCLUDED
/*
 * { 
 */
#define ALGO_TEMPLATE_MNGR_CALLBACKS_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/multi_set.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>

/*
 * {
 */

/**
 * Callbacks for dnx_algo_template_create.
 *
 * Note that these callbacks don't take core_id as an argument.
 *   If the advanced algorithm requires core as an argument, it must be provided in the extra arguments struct.
 *
 * Upon creation, the algorithm must create a unique identifier (algo_instance_id) for each instance. This identifier will be used
 *   to manage this template when the other APIs are called.
 *   If the algorithm is unique (has only one instance), the identifier can be garbage.
 *
 *
 * \see
 * dnx_algo_template_create
 * dnx_algo_template_allocate
 * dnx_algo_template_exchange
 */

/**
* \brief
*   Callback to create a new instance of the advanced algorithm.
*
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] create_data -
*      Pointed memory contains setup parameters required for the
*      creation of the resource. See \ref dnx_algo_template_create_data_t
*    \param [in] extra_arguments
*      Pointed memory to hold extra arguments required for creating this template. \n
*      \b As \b input - \n
*       The content of this pointer depends on the algorithm. It's the algorithm's \n
*       responsibility to verify it.
*       If the algorithm doesn't require extra arguments, this can be set to NULL.
*    \param [in] algo_instance_id
*      Pointer to place the Algorithm instance id that was created.
*      \b As \b output - \n
*        Holds the created instance id.
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_template_create
*    shr_error_e
*/
typedef shr_error_e(
    *dnx_algo_template_create_cb) (
    int unit,
    uint32 module_id,
    multi_set_t * multi_set_template,
    dnx_algo_template_create_data_t * create_data,
    void *extra_arguemnts,
    uint32 alloc_flags);

/**
* \brief
*   Callback to destroy an instance of the advanced algorithm.
*
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id
*      Internal id for the algorithm.
*    \param [in] extra_arguments
*      Pointed memory to hold extra arguments required for destroying this resource. \n
*      \b As \b input - \n
*       The content of this pointer depends on the algorithm. It's the algorithm's \n
*       responsibility to verify it.
*       If the algorithm doesn't require extra arguments, this can be set to NULL.
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \par INDIRECT INPUT:
*    Depends on the advanced algorithm, the specified advanced algorithm's sw state
*       will be destroyed.
*    \b *extra_arguments -\n
*       See above.
*  \par INDIRECT OUTPUT:
*    Depends on the advanced algorithm, the specified advanced algorithm's sw state
*       will be destroyed.
*  \remark
*    None
*  \see
*    dnx_algo_template_destroy
*    shr_error_e
*/
typedef shr_error_e(
    *dnx_algo_template_destroy_cb) (
    int unit,
    uint32 module_id,
    multi_set_t * multi_set_template,
    void *extra_arguemnts);

/**
* \brief
*   Callback to allocate a free profile of an advanced algorithm.
*
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*    \param [in] flags -
*      DNX_ALGO_TEMPLATE_ALLOCATE_* flags
*    \param [in] nof_references -
*      Number of references to be allocated.
*    \param [in] profile_data -
*      Pointer to memory holding template data to be saved to a profile.
*    \param [in] extra_arguments -
*      Pointer to memory holding extra arguments to be passed to the allocation function of an advanced algorithm.
*      If no advanced algorithm is used, use NULL.
*    \param [in] profile -
*      Pointer to place the allocated profile.
*      \b As \b output - \n
*        Holds the allocated profile.
*      \b As \b input - \n
*        If flag \ref DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
*    \param [in] first_reference
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of the profile's prior existence. \n
*        If TRUE, this is the first reference to the profile, and the data needs to be
*          written to the relevant table.
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_template_allocate
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_template_allocate_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference);

/**
* \brief
*   Free one reference from old_profile, add one reference to data.
*
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*    \param [in] flags -
*      DNX_ALGO_TEMPLATE_ALLOCATE_* or DNX_ALGO_TEMPLATE_EXCHANGE_* flags.
*    \param [in] nof_references -
*      Number of references to be exchanged.
*    \param [in] profile_data -
*      Pointer to memory holding template data to be saved to a profile.
*    \param [in] old_profile -
*      One reference will be freed from this profile.
*    \param [in] extra_arguments -
*      Pointer to memory holding extra arguments to be passed to the allocation function of an advanced algorithm.
*      If no advanced algorithm is used, use NULL.
*    \param [in] new_profile -
*      Pointer to place the allocated profile.
*      \b As \b output - \n
*        Holds the allocated profile.
*      \b As \b input - \n
*        If flag \ref DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
*    \param [in] first_reference
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of the new profile's prior existence. \n
*    \param [in] last_reference -
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of whether it's the last reference to old_profile.
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*       See the documentation for dnx_algo_template_exchange.
*  \see
*    dnx_algo_template_allocate
*    dnx_algo_template_free
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_template_exchange_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    const void *profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference);

/**
* \brief
*   Replace a data pointer with new data.
*
*   \param [in] unit -
*       Relevant unit.
*    \param [in] algo_instance_id -
*       Internal id for the algorithm.
*    \param [in] profile -
*       The data for this profile will be replaced with the provided new data.
*   \param [in] new_profile_data -
*       New data to replace the existing data in this profile.
*   \return
*       \retval Zero if no error was detected
*       \retval Negative if error was detected. See \ref shr_error_e
*   \remark
*       None
*   \see
*    dnx_algo_template_allocate
*    dnx_algo_template_free
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_template_replace_data_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int profile,
    const void *new_profile_data);

/**
* \brief
*   Get the data pointed by a template's profile.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*    \param [in] profile -
*      Profile to get the data from.
*    \param [in] ref_count -
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with number of refernces to this profile.
*    \param [in] profile_data -
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with data stored in this profile.
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_template_create_data_t
*    dnx_algo_template_advanced_alogrithm_cb_t
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_template_profile_data_get_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int profile,
    int *ref_count,
    void *profile_data);

/**
* \brief
*   Get a template's profile by given data.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*    \param [in] profile_data -
*      Pointer to memory for data to lookup. \n
*      \b As \b input - \n
*        Fill this with the data to find.
*    \param [in] profile -
*      Pointer to place the allocated profile.
*      \b As \b output - \n
*        Holds the profile that holds given data.
*
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_template_create_data_t
*    dnx_algo_template_advanced_alogrithm_cb_t
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_template_profile_get_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    const void *profile_data,
    int *profile);

/**
* \brief
*   Callback to free an allocated profile in an advanced algorithm.
*
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*    \param [in] profile -
*      Profile to be freed.
*    \param [in] nof_references -
*      Number of references to be freed.
*    \param [in] last_reference -
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of whether it's the last reference to this profile.
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_template_free
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_template_free_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference);

/**
* \brief
*   Free all reference to all profiles of this template.
*
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_template_create_data_t
*    dnx_algo_template_advanced_alogrithm_cb_t
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_template_clear_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template);

/**
* \brief
*   Free all references to a profile.
*
*
*   \param [in] unit -
*       Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
*   \param [in] profile -
*       All references, and the data, for this profile will be freed.
*   \return
*       \retval Zero if no error was detected
*       \retval Negative if error was detected. See \ref shr_error_e
*   \remark
*       None
*   \see
*    dnx_algo_template_free
 */
typedef shr_error_e(
    *dnx_algo_template_free_all_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int profile);

/**
 * \brief
 *      Given a template name, core and profile, returns the next allocated profile
 *      for this template on this core.
 *      The returned profile will be equal to or greater than the given profile. It is the user's
 *      responsibility to update it between iterations.
 *      When there are no more allocated profiles, DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE will be returned.
 *
 *  \param [in] unit -
 *      Relevant unit.
*    \param [in] algo_instance_id -
*      Internal id for the algorithm.
 *  \param [in,out] current_profile -
 *       \b As \b input - \n
 *       The profile to start searching for the next allocated profile from.
 *       \b As \b output - \n
 *       he next allocated profile. It will be >= from the profile given as input.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *      It is the user's responsibility to update the current_profile between iterations. This usually means
 *       giving it a ++.
 * \see
 *      None
 */
typedef shr_error_e(
    *dnx_algo_template_get_next_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int *current_profile);

/**
* \brief
*   Set advanced algorithm info.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template.
*    \param [in] algorithm_info - algorithm info to be set
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_template_advanced_algorithm_info_set_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    resource_tag_bitmap_tag_info_t * algorithm_info);

/**
* \brief
*   Get advanced algorithm info.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template.
*    \param [out] algorithm_info - Pointer where the algorithm will be save.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
 */
typedef shr_error_e(
    *dnx_algo_template_advanced_algorithm_info_get_cb) (
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    resource_tag_bitmap_tag_info_t * algorithm_info);

/**
 * \brief Callback struct for dnx_algo_template_create.
 *
 * This structure contains the callbacks that will be called when an advanced template
 *  is accessed.
 *
 *  The callbacks for create, allocate, is_allocated and free are mandatory, and the rest are optional.
 *
 *  The callback for destroy is currently not mandatory, but might be soon,
 *  so it's best to implement it.
 *
 *  \see
 * dnx_algo_template_create
 */
typedef struct
{
    dnx_algo_template_create_cb create_cb;      /* Mandatory */
    dnx_algo_template_allocate_cb allocate_cb;  /* Mandatory */
    dnx_algo_template_exchange_cb exchange_cb;
    dnx_algo_template_profile_data_get_cb profile_data_get_cb;  /* Mandatory */
    dnx_algo_template_profile_get_cb profile_get_cb;    /* Mandatory */
    dnx_algo_template_free_cb free_cb;  /* Mandatory */
    dnx_algo_template_destroy_cb destroy_cb;    /* Currently not mandatory, but better be implemented. */
    dnx_algo_template_replace_data_cb replace_data_cb;
    dnx_algo_template_clear_cb clear_cb;
    dnx_algo_template_free_all_cb free_all_cb;
    dnx_algo_template_get_next_cb get_next_cb;
    dnx_algo_template_advanced_algorithm_info_set_cb algorithm_info_set;
    dnx_algo_template_advanced_algorithm_info_get_cb algorithm_info_get;

} dnx_algo_template_advanced_alogrithm_cb_t;

/*
 * }
 */

/*
 * } 
 */
#endif/*_ALGO_TEMPLATE_MNGR_CALLBACKS_INCLUDED__*/
