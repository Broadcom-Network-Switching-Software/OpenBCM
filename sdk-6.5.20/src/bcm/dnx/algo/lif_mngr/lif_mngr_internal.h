/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file lif_mngr_internal.h
 * 
 *
 * This file contains the internal APIs required for lif algorithms.
 */

#ifndef  INCLUDE_LIF_MNGR_INTERNAL_H_INCLUDED
#define  INCLUDE_LIF_MNGR_INTERNAL_H_INCLUDED

/*************
 * INCLUDES  *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */

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
 * INTERNAL LIF MAPPING FUNCTIONS - START
 * The submodule's init and deinit functions should be here.
 * Also, the create and destroy mapping functions are here because only the lif manager
 * should be able to access them.
 * {
 */

/**
 * \brief
 *   Init lif mapping module.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par INDIRECT INPUT
 *   Lif submodule of DNX data, used to determine map sizes according to HW variables.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule are initialized by this function.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mapping_init(
    int unit);

/**
 * \brief
 *   Deinit lif mapping module.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par INDIRECT INPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule are deinitialized by this function.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mapping_deinit(
    int unit);

/**
 * }
 * INTERNAL LIF MAPPING FUNCTIONS - END
 */

/**
 * INTERNAL LOCAL INLIF FUNCTIONS - START
 * {
 */

/**
 * \brief - Init local inlif allocation module.
 *
 * \param [in] unit - Identifier of the device to access.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_local_inlif_allocation_init(
    int unit);

/**
 * \brief
 *   Allocate a local inlif.
 *
 *  Note - we don't verify input in this function because we assume that the only location calling this function
 *  is dnx_lif_mngr_lif_allocate and that input validation is done there.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in,out] inlif_info -
 *     Pointer to memory for local inlif allocation input/output. \n
 *     \b As \b input - \n
 *       All elements in inlif info are required as input, except for local_inlifs which will be ignored.
 *       In the case of duplicated per core lif table, the field 'core_id' will specify which core is used
 *          for allocation, or it can be set to _SHR_CORE_ALL to allocate one lif per core.
 *       See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 *     \b As \b output - \n
 *       This procedure loads the field inlif_info->local_inlifs with the local in lifs allocated, in the manner
 *       detailed in the struct's description.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 *
 * \see
 *   dnx_lif_mngr_lif_allocate
 */
shr_error_e dnx_algo_local_inlif_allocate(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info);

/**
 * \brief
 *   Free the allocated local inlif.
 *
 *  Note - we don't verify input in this function because we assume that the only location calling this
 *  function is dnx_lif_mngr_lif_free and that input validation is done there.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] inlif_info -
 *     Pointer to memory for local inlif deallocation. \n
 *     \b As \b input - \n
 *       The local inlif[s] in the local_inlifs array will be freed. \n
 *       All elements in inlif info are required as input, except for dbal_result_type, which should be 0.
 *       See \ref lif_mngr_local_inlif_info_t for detailed description of each field.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None.
 *
 * \see
 *   dnx_lif_mngr_lif_free
 */
shr_error_e dnx_algo_local_inlif_free(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info);

/**
 * }
 * INTERNAL LOCAL INLIF FUNCTIONS - END
 */

/**
 * INTERNAL LOCAL OUTLIF FUNCTIONS - START
 * {
 */

/**
 * \brief - Init local outlif allocation module.
 *
 * \param [in] unit - Identifier of the device to access.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_local_outlif_allocation_init(
    int unit);

/**
 * \brief - Deinit local outlif allocation module.
 *
 * \param [in] unit - Identifier of the device to access.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_local_outlif_allocation_deinit(
    int unit);

/**
 * \brief - Allocate a local outlif.
 *
 *  Note - we don't verify input in this function because we assume that the only location calling this function
 *  is dnx_lif_mngr_lif_allocate and that input validation is done there.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in,out] outlif_info -
 *     Pointer to memory for local outlif allocation input/output. \n
 *     \b As \b input - \n
 *       All elements in outlif info are required as input, except for local_outlif which will be ignored.
 *       See \ref lif_mngr_local_outlif_info_t for detailed description of each field.
 *     \b As \b output - \n
 *       This procedure loads the field inlif_info->local_outlif with the local out lif allocated, according to
 *       the given app type.
 *   \param [out] outlif_hw_info - Pointer to memory where the info to configure outlif HW will be filled.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_lif_mngr_lif_allocate
 */
shr_error_e dnx_algo_local_outlif_allocate(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info);

/**
 * \brief -  Free the allocated local outlif.
 *
 *  Note - we don't verify input in this function because we assume that the only location calling this
 *  function is dnx_lif_mngr_lif_free and that input validation is done there.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in,out] outlif_info - Info for deallocating the outlif.
 *     \b As \b input - \n
 *       The fields local_outlif, dbal_table and dbal_result will be used for deallocation.
 *     \b As \b output - \n
 *       The field outlif_flags will be filled with indication of whether updating HW is necessary.
 *   \param [out] outlif_hw_info - Pointer to memory where the info to configure outlif HW will be filled.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_lif_mngr_lif_free
 */
shr_error_e dnx_algo_local_outlif_free(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info);

/**
 * \brief - Given a unit, returns the number of glem lines in the device.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [out] nof_glem_lines - how many glem lines are in the device.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_lif_mngr_nof_glem_lines_get(
    int unit,
    int *nof_glem_lines);

/**
 * \brief - Given a unit, returns the maximum number of local
 *        Out-LIFs  for the device.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [out] nof_local_outlifs - Returned maximum number of
 *        local Out-LIFs.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_lif_mngr_nof_local_outlifs_get(
    int unit,
    int *nof_local_outlifs);

/**
 * \brief
 * Given an outlif info, with the old result type in the alterntaive_result_type and
 * the new result type in dbal_result_type, returns indication whether this combination is legal
 * in terms of linked list.
 *
 * If the old result type required LL, then it can be changed to entry without linked list without a
 * problem.
 * If new type has linked list - fine.
 * It it doesn't - it's possible not to have linked list in a linked list bank.
 *
 * If the new type doesn't require linked list, then we don't care.
 *
 * Only possible illegal case is new type requires linked list and old type didn't.
 * Can't put entry without linked list in a bank with linked list.
 *
 *
 *  \param [in] unit - Unit-ID
 *  \param [in] outlif_info - see above.
 *  \param [out] ll_match - see above.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_algo_local_outlif_ll_match_check(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info,
    int *ll_match);

/**
 * }
 * INTERNAL LOCAL OUTLIF FUNCTIONS - END
 */

/*
 * }
 */

#endif /* INCLUDE_LIF_MNGR_INTERNAL_H_INCLUDED */
