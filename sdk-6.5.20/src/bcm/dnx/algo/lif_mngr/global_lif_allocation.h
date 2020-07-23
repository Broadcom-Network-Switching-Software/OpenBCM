/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file global_lif_allocation.h
 *  
 * 
 * This file contains the APIs required to allocate and free global lifs.
 */

#ifndef  INCLUDE_GLOBAL_LIF_ALLOCATION_H
#define  INCLUDE_GLOBAL_LIF_ALLOCATION_H

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

/** 
 * \brief 
 * Used by the function dnx_algo_global_lif_allocation_verify to indicate 
 *    that the verify operation is performed for deallocation rather than
 *    allocation.
 */
#define DNX_ALGO_GLOBAL_LIF_ALLOCATION_DEALLOCATE_VERIFY  (SAL_BIT(25))

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
 * Given ingress indications, and egress allocation verifier, returns the appropriate global lif allocation flags.
 */
#define DNX_ALGO_GLOBAL_LIF_ALLOCATION_SET_DIRECTION_FLAGS(_is_ingress, _allocate_egress_global_lif) \
    (_is_ingress ? DNX_ALGO_LIF_INGRESS : 0) | ((_allocate_egress_global_lif) ? DNX_ALGO_LIF_EGRESS : 0)

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
 *   Init global lif allocation module.
 *  
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par INDIRECT INPUT
 *   Device data: Inlif and GLEM table sizes, used to initialize the global lif resources.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Initialize global lif resources in the resource manager, and global lif allocation
 *      submodule in the sw state.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_global_lif_allocation_init(
    int unit);

/**
 * \brief
 *   Deinit global lif allocation module.
 *  
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par INDIRECT INPUT
 *   Global lif resources in the resource manager, and global lif allocation
 *      submodule in the sw state.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Denitialize global lif resources in the resource manager, and global lif allocation
 *      submodule in the sw state.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_global_lif_allocation_deinit(
    int unit);

/**
 * \brief 
 *     Verify global lif before allocation / dealloction:
 *     1. If allocating - make sure there are enough free IDs.
 *     2. If deallocating / with id - make sure the id is legal.
 *     3. If deallocating - make sure symmetric lifs are deallocated on both sides, and non symmetric on one side.
 *  
 *     We assume that this function is ALWAYS called before \ref dnx_algo_global_lif_allocation_allocate
 *     and \ref dnx_algo_global_lif_allocation_free.
 *  
 *     We also assume that the local lif allocation was successful before calling this function.
 *  
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] allocation_flags -
 *     Flags for the resource manager, or deallocation indication flag.
 *     \ref DNX_ALGO_RES_ALLOCATE_WITH_ID and/or \ref DNX_ALGO_GLOBAL_LIF_ALLOCATION_DEALLOCATE_VERIFY
 *   \param [in] direction_flags -
 *     Ingress or egress indication: 
 *     \ref DNX_ALGO_LIF_INGRESS and/or \ref DNX_ALGO_LIF_EGRESS
 *   \param [in] global_lif -
 *       If either \ref DNX_ALGO_GLOBAL_LIF_ALLOCATION_DEALLOCATE_VERIFY or \ref DNX_ALGO_RES_ALLOCATE_WITH_ID
 *       flags are set, this holds the id to be allocated / deallocated.
 * \par INDIRECT INPUT 
 *   Global lif resource manager and sw state.
 *   Device data used for input validation.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   None.
 * \remark
 *   None.
 *  
 * \see 
 *   dnx_algo_global_lif_allocation_allocate
 *   dnx_algo_global_lif_allocation_free
 */
shr_error_e dnx_algo_global_lif_allocation_verify(
    int unit,
    uint32 allocation_flags,
    uint32 direction_flags,
    int global_lif);

/*
 * } 
 */

#endif /* INCLUDE_GLOBAL_LIF_ALLOCATION_H */
