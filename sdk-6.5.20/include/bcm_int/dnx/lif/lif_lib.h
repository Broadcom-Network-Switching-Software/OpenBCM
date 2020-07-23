/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file lif_lib.h
 *  
 * 
 * This file contains the APIs required to perform general lif HW writes, as well as lif allocation.
 */

#ifndef  INCLUDE_LIF_LIB_H
#define  INCLUDE_LIF_LIB_H

/*************
 * INCLUDES  *
 */
/*
 * { 
 */

#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>

/*
 * } 
 */
/*************
 * DEFINES   *
 */
/*
 * { 
 */
/**
 * \brief
 *   Do not replace the value of current filed in GLEM (can be used for either outlif or lif profile field
 * \see
 *   dnx_lif_lib_replace_glem
 */
#define LIF_LIB_GLEM_KEEP_OLD_VALUE 0xFFFFFFFF
/*
 * } 
 */
/*************
 * MACROS    *
 */
/*
 * { 
 */
/**
 * \brief
 * If LIF_LIB_GLEM_ACCESS_IS_OPTIONAL is non-zero then the code accepts failure to
 * read an entry from GLEM provided it is 'not found'.
 * This is mainly for 'remove', in which case, this passes silently.
 * For 'read', it is up to the caller to decide.
 * Most probably this flag and option will be removed on the final version.
 */
#define LIF_LIB_GLEM_ACCESS_IS_OPTIONAL              1
/*
 * } 
 */
/*************
 * TYPE DEFS *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * GLOBALS   *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * FUNCTIONS *
 */
/*
 * { 
 */

/**
 * \brief
 *   Allocate local inlif and save its dbal table and result in sw state.
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in,out] inlif_info -
 *   Pointer to memory for local inlif allocation input/output. \n
 *   \b As \b input - \n
 *     All elements in inlif info are required as input, except for local_inlif which will be ignored.
 *     In the case of duplicated per core lif table, the field 'core_id' will specify which core is used
 *        for allocation, or it can be set to _SHR_CORE_ALL to allocate one lif per core.
 *     See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 *   \b As \b output - \n
 *     This procedure loads the field inlif_info->local_inlifs with the local in lifs allocated, in the manner
 *     detailed in the struct's description.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_inlif_allocate(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info);

/**
 * \brief
 *   Deallocate the given local inlif.
 *
 * \param [in] unit -
 *     Identifier of the device to access.
 * \param [in] inlif_info -
 *     Pointer to memory for local inlif deallocation. \n
 *     \b As \b input - \n
 *       The local inlif[s] in the local_inlifs array will be freed. \n
 *       All elements in inlif info are required as input, except for dbal_result_type, which should be 0.
 *       See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_inlif_free(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info);

/**
 * \brief
 *   Allocate local outlif, save its dbal table and result in sw state and configure HW.
 *
 *   This API performs several functions:
 *   1. Allocate local outlif and save its dbal table+result.
 *   2. Sometimes, configured the HW to map this lif to its logical phase and match it with Linked list entry.
 *   3. Link list on entry can be forced (with or without) using DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST,
 *      DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST (see documentation)
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in,out] outlif_info -
 *   Pointer to memory for local outlif allocation input/output. \n
 *   \b As \b input - \n
 *     All elements in outlif info are inputs, except for local_outlif.
 *   \b As \b output - \n
 *     This procedure loads the field outlif_info->local_outlif with the local out lif allocated.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_outlif_allocate(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info);

/**
 * \brief
 *   Deallocate the given local outlif.
 *
 *   All the steps described in lif_lib_allocate_outlif will be reverted.
 *
 * \param [in] unit -
 *     Identifier of the device to access.
 * \param [in] local_outlif -
 *     Local outlif to be freed.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_outlif_free(
    int unit,
    int local_outlif);

/*
 * }
 */

/*************
 * FUNCTIONS *
 */
/*
 * {
 */

/**
 * \brief
 *   Allocate global lif and local lif, create mapping between them, and configure HW.
 *
 *   This API performs several functions:
 *   1. Allocate local and global lifs (ingress and egress, as required).
 *   2. Keep mapping of global to local lifs and vice versa in the sw state.
 *   3. Keep LIF TABLE DBAL ID and type in SW state.
 *   4. If outlif was allocated, then the HW will be configured to map this lif
 *       to its logical phase and match it with Linked list entry.
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in] flags -
 *   Currently supported flag is \ref LIF_MNGR_GLOBAL_LIF_WITH_ID \n
 *      to allocate the global lif with the ID given in global lif argument.
 * \param [in,out] global_lif -
 *   Int pointer to memory to write output into. \n
 *   For \ref LIF_MNGR_GLOBAL_LIF_WITH_ID, this may also be indirect input.
 *   \b As \b output - \n
 *     This procedure loads pointed memory with the global lif allocated. \n
 *     Not used as output when \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is used.
 *   \b As \b input - \n
 *     If flag LIF_MNGR_GLOBAL_LIF_WITH_ID is set, this holds
 *       the id to be allocated.
 * \param [in,out] inlif_info -
 *   Pointer to memory for local inlif allocation input/output. \n
 *   If NULL, then this will be ignored, local inlifs will not be allocated,
 *      and outlif_info must not be NULL.
 *   \b As \b input - \n
 *     All elements in inlif info are required as input, except for local_inlifs which will be ignored.
 *     In the case of duplicated per core lif table, the field 'core_id' will specify which core is used
 *        for allocation, or it can be set to _SHR_CORE_ALL to allocate one lif per core.
 *     See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 *   \b As \b output - \n
 *     This procedure loads the field inlif_info->local_inlifs with the local in lifs allocated, in the manner
 *     detailed in the struct's description.
 * \param [in,out] outlif_info -
 *   Pointer to memory for local outlif allocation input/output. \n
 *   If NULL, then this will be ignored, local outlif will not be allocated,
 *      and inlif_info must not be NULL.
 *   \b As \b input - \n
 *     All elements in outlif info are inputs, except for local_outlif.
 *   \b As \b output - \n
 *     This procedure loads the field outlif_info->local_outlif with the local out lif allocated.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_allocate(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info);

/**
 * \brief
 *   Deallocate the given global lif and its local in/outlif.
 *
 *   All the steps described in lif_lib_allocate will be reverted.
 *
 * \param [in] unit -
 *     Identifier of the device to access.
 * \param [in] global_lif -
 *    Global lif to be deallocated.
 * \param [in] inlif_info -
 *     Pointer to memory for local inlif deallocation. \n
 *     If NULL, then this will be ignored, local inlif will not be freed,
 *        and local_outlif must not be LIF_MNGR_INVALID.
 *     \b As \b input - \n
 *       The local inlif[s] in the local_inlifs array will be freed. \n
 *       All elements in inlif info are required as input, except for dbal_result_type, which should be 0.
 *       See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 * \param [in] local_outlif -
 *     Local outlif to be deallocated, or LIF_MNGR_INVALID to deallocate ingress only.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_free(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    int local_outlif);

/**
 * \brief 
 *  Given core and egress global and local lifs, adds mapping from the global lif to the local lif in the GLEM table.
 *  
 *  This function should be called after calling dnx_lif_mngr_lif_allocate, and use the allocated global and local lifs
 *     for the mapping.
 *  
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] core_id -
 *     Relevant core. Can be either a single core, or _BCM_CORE_ALL if both cores are used.
 *   \param [in] global_lif -
 *     Global lif to be written as key.
 *   \param [in] local_lif -
 *     Local lif to be written as result.
 * \par INDIRECT INPUT
 *   None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval _SHR_E_PARAM if one of the arguments is illegal, \ref dbal_entry_commit errors if dbal failed.
 *      See \ref shr_error_e
 * \remark
 *   None.
 * \see 
 *   dnx_lif_mngr_lif_allocate 
 */
shr_error_e dnx_lif_lib_add_to_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif);

/**
 * \brief 
 *  Same as dnx_lif_lib_add_to_glem but for existing glem entries
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] core_id -
 *     Relevant core. Can be either a single core, or _BCM_CORE_ALL if both cores are used.
 *   \param [in] global_lif -
 *     Global lif to be written as key.
 *   \param [in] local_lif -
 *     Local lif to be written as result, LIF_LIB_GLEM_KEEP_OLD_VALUE if to be kept.
 *   \param [in] outlif_profile -
 *     Outlif profile lif to be written as result, LIF_LIB_GLEM_KEEP_OLD_VALUE if to be kept.
 * \par INDIRECT INPUT
 *   None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval _SHR_E_PARAM if one of the arguments is illegal, \ref dbal_entry_commit errors if dbal failed.
 *      See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   dnx_lif_lib_add_to_glem
 */
shr_error_e dnx_lif_lib_replace_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif,
    uint32 outlif_profile);

/**
 * \brief
 *  Given egress global lif and core, returns the local lif mapped to this global lif and the lif profile.
 *  
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] core_id -
 *     Relevant core. Must be explicit core, because CORE_ALL would be meaningless.
 *   \param [in] global_lif -
 *     Global lif key.
 *   \param [out] local_lif_p -
 *     Pointer to uint32.
 *     This procedure loads the pointer with the local lif mapped from this core_id X global lif combination,
 *     in case global lif is not a rif.
 *   \param [out] outlif_profile_p -
 *     Pointer to uint32.
 *     This procedure loads the pointer with the ERPP outlif profile mapped from this core_id X global lif combination.
 * \par INDIRECT INPUT
 *   None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval _SHR_E_PARAM if one of the arguments is illegal, \ref dbal_entry_get errors if dbal failed.
 *      See \ref shr_error_e
 * \remark
 *   None.
 */
shr_error_e dnx_lif_lib_read_from_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 *local_lif_p,
    uint32 *outlif_profile_p);

/**
 * \brief 
 *  Given egress global lif and core, removes the mapping for this global lif from the GLEM table.
 *  
 *  This function should be called after calling dnx_lif_mngr_lif_free, and use the allocated global and local lifs
 *     for the mapping.
 *  
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] core_id -
 *     Relevant core. Can be either a single core, or _BCM_CORE_ALL if both cores are used.
 *   \param [in] global_lif -
 *     Global lif key to remove.
 * \par INDIRECT INPUT
 *   None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval _SHR_E_PARAM if one of the arguments is illegal, \ref dbal_entry_clear errors if dbal failed.
 *      See \ref shr_error_e
 * \remark
 *   None.
 * \see 
 *   dnx_lif_mngr_lif_free
 */
shr_error_e dnx_lif_lib_remove_from_glem(
    int unit,
    int core_id,
    uint32 global_lif);

/**
 * \brief
 *      Initialize lif lib module.
 *      Submodules initialized in this function:
 *      - Outlif logical to physical mapping.
 *      - Outrif mapping in EEDB.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_lif_lib_init(
    int unit);

/**
 * \brief
 *      close lif logger verbosity
 *
 * \param [in] unit - Unit-ID
 * \param [out] orig_severity - original severity
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_lif_lib_logger_close(
    int unit,
    bsl_severity_t * orig_severity);

/**
 * \brief
 *      restore lif logger verbosity
 *
 * \param [in] unit - Unit-ID
 * \param [out] orig_severity - original severity - to be re-set
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_lif_lib_logger_restore(
    int unit,
    bsl_severity_t orig_severity);
/*
 * } 
 */

#endif /* INCLUDE_LIF_LIB_H */
