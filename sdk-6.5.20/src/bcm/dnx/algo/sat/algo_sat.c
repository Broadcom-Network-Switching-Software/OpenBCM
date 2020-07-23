/** \file algo_sat.c
 *
 *   Procedures for initializing sat resources.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <soc/dnx/dnx_data/auto_generated/dnx_data_sat.h>

#include <bcm/types.h>
#include <bcm_int/dnx/algo/sat/algo_sat.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_sat_access.h>

/**
 * }
 */

/**
 * \brief
 *    Initialize gtf id resources.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *     \retval Zero if no error was detected
 *     \retval Negative if error was detected. See \ref shr_error_e

 * \remark
 *    The resource of SAT GTF id is initialized
 *
 * \see
 *    dnx_algo_res_create
 */
shr_error_e
dnx_algo_gtf_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * GTF_ID resource management - Allocate GTF ID
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.flags = 0;
    /** The available sat IDs defined by BCM APIs are 0 ~ nof_gtf_ids.*/
    data.first_element = 0;
    data.nof_elements = dnx_data_sat.generator.nof_gtf_ids_get(unit);
    sal_strncpy(data.name, DNX_ALGO_RES_TYPE_SAT_GTF_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Initialize ctf id resources.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *     \retval Zero if no error was detected
 *     \retval Negative if error was detected. See \ref shr_error_e

 * \remark
 *    The resource of ctf ID is initialized
 *
 * \see
 *    dnx_algo_res_create
 */
shr_error_e
dnx_algo_ctf_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * CTF_ID resource management - Allocate CTF ID
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.flags = 0;
    /** The available ctf IDs defined by BCM APIs are 0 ~ nof_ctf_ids.*/
    data.first_element = 0;
    data.nof_elements = dnx_data_sat.collector.nof_ctf_ids_get(unit);
    sal_strncpy(data.name, DNX_ALGO_RES_TYPE_SAT_CTF_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Initialize ctf trap id resources.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *     \retval Zero if no error was detected
 *     \retval Negative if error was detected. See \ref shr_error_e

 * \remark
 *    The resource of ctf trap id  is initialized
 *
 * \see
 *    dnx_algo_res_create
 */

shr_error_e
dnx_algo_ctf_trap_id_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * TRAP_ID resource management - Allocate TRAP ID entry
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.flags = 0;
    /** The available ctf IDs defined by BCM APIs are 0 ~ nof_ctf_ids.*/
    data.first_element = 0;
    data.nof_elements = dnx_data_sat.collector.trap_id_entries_get(unit);
    sal_strncpy(data.name, DNX_ALGO_RES_TYPE_SAT_TRAP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_trap_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Initialize ctf trap data entry resources.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *     \retval Zero if no error was detected
 *     \retval Negative if error was detected. See \ref shr_error_e

 * \remark
 *    The resource of  ctf trap data entry  is initialized
 *
 * \see
 *    dnx_algo_res_create
 */
shr_error_e
dnx_algo_ctf_trap_data_entries_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * TRAP_ID resource management - Allocate TRAP DATA entry
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.flags = 0;
    /** The available ctf IDs defined by BCM APIs are 0 ~ nof_ctf_ids.*/
    data.first_element = 0;
    data.nof_elements = dnx_data_sat.collector.trap_data_entries_get(unit);
    sal_strncpy(data.name, DNX_ALGO_RES_TYPE_SAT_TRAP_DATA, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_trap_data.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Initialize SAT resources.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e

 * \remark
 *  The resource of SAT GTF/CTF IDs is initialized
 *
 * \see
 *  dnx_algo_res_create
 */
shr_error_e
dnx_algo_sat_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init sw state algo_sat_db.
     */
    SHR_IF_ERR_EXIT(algo_sat_db.init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_gtf_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_ctf_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_ctf_trap_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_ctf_trap_data_entries_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  De-Initialize SAT resources.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e

 * \remark
 *   De-initialize  SAT GTF/CTF  ID resource that is created by dnx_algo_sat_init
 *
 * \see
 *  dnx_algo_sat_init
 *  dnx_algo_res_destroy
 */
shr_error_e
dnx_algo_sat_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sw state resources will be deinit together.
     */
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
