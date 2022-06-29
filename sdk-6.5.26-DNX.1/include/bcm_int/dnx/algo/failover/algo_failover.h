/** \file bcm_int/dnx/algo/failover/algo_failover.h
 *
 * Reserved.$
 */

#ifndef _DNX_ALGO_FAILOVER_H_INCLUDED_
/*
 * {
 */
#define _DNX_ALGO_FAILOVER_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_failover_access.h>

/*
 * MACROs
 * {
 */
#define DNX_ALGO_FAILOVER_RES_FEC_PATH_SELECT "DNX_ALGO_FAILOVER_RES_FEC_PATH_SELECT"
#define DNX_ALGO_FAILOVER_RES_ING_PATH_SELECT "DNX_ALGO_FAILOVER_RES_ING_PATH_SELECT"
#define DNX_ALGO_FAILOVER_RES_ING_MC_PATH_SELECT "DNX_ALGO_FAILOVER_RES_ING_MC_PATH_SELECT"
#define DNX_ALGO_FAILOVER_RES_EGR_PATH_SELECT "DNX_ALGO_FAILOVER_RES_EGR_PATH_SELECT"

/*
 * }
 * MACROs
 */
/**
 * \brief Wrapper function for allocation manager which creates tagged allocation bitmap for failover FEC path select.
 *           The tag will represent FEC hierarchy,
 *           each bank in FEC path select will be tagged upon allocating the first element in the bank.
 *           Refer to resource_tag_bitmap_create() for details about the input parameters.
 */
shr_error_e dnx_algo_failover_fec_path_select_bitmap_create(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags);

/**
 * \brief Wrapper function for allocation manager which allocating failover FEC path select.
 *           The tag represents FEC hierarchy,
 *           each bank in FEC path select will be tagged upon allocating the first element in the bank.
 *           Refer to resource_tag_bitmap_allocate() for details about the input parameters.
 *           extra_arguments - pointer to uint32 which represents the tag
 */
shr_error_e dnx_algo_failover_fec_path_select_bitmap_allocate(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
 * \brief Wrapper function for resource manager which frees failover FEC path select.
 *           The tag represents FEC hierarchy,
 *           each bank in FEC path select will be tagged upon allocating the first element in the bank.
 *           Refer to resource_tag_bitmap_free() for details about the input parameters.
 *           extra_arguments - pointer to uint32 which represents the tag
 */
shr_error_e dnx_algo_failover_fec_path_select_bitmap_free(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int element,
    void *extra_argument);

/**
 * \brief Wrapper function for allocation manager which allocating several failover FEC path select.
 *           The tag represents FEC hierarchy,
 *           each bank in FEC path select will be tagged upon allocating the first element in the bank.
 *           Refer to resource_tag_bitmap_allocate() for details about the input parameters.
 *           extra_arguments - pointer to uint32 which represents the tag
 */
shr_error_e dnx_algo_failover_fec_path_select_bitmap_several_allocate(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element);

/**
 * \brief
 *   Reserves a bank of memory from failover module at ecmp bank resolution for the use of the ecmp extended
 * \param [in] unit - The unit number.
 * \param [in] ecmp_bank_id - The bank ID that is going to be 
 *        reserved. Valid bank IDs are 0 to 3.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_failover_fec_bank_for_ecmp_extended_allocate(
    int unit,
    int ecmp_bank_id);

/**
 * \brief
 *   Frees a bank of memory from failover module at ecmp bank resolution which previously allocated by ecmp extended
 * \param [in] unit - The unit number.
 * \param [in] ecmp_bank_id - The bank ID that is going to be 
 *             freed. Valid bank IDs are 0 to 3.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_failover_fec_bank_for_ecmp_extended_free(
    int unit,
    int ecmp_bank_id);

/**
 * \brief Initialize dnx algo failover module.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_failover_init(
    int unit);

#endif /*_DNX_ALGO_FAILOVER_H_INCLUDED_*/
