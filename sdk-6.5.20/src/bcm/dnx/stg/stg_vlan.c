/*! \file stg_vlan.c
 *
 * Spanning Tree procedures per vlan for DNX.
 * Allows to associate a STG-ID with a VLAN.
 *
 * Note: VID means Virtual Switch Instance ID here.
 *       It represents vlan-id if VID < 4096, else it represents a VSI-ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STG

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>

#include <bcm/types.h>
#include <bcm_int/dnx/stg/stg.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/stg/algo_stg.h>

#include <src/bcm/dnx/stg/stg_stp.h>
#include <src/bcm/dnx/stg/stg_vlan.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/**
 * \brief
 *   Verify if the given VSI is in the range (and if it is allocated).
 *
 * \param [in] unit - Relevant unit.
 * \param [in] vsi - The Given VSI.
 * \param [in] verify_alloc - If verify the allocation of the VSI.
 *                            0: don't care its allocation.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   None.
 *
 * \see
 *   None.
 */
static shr_error_e
dnx_vsi_id_verify(
    int unit,
    bcm_vlan_t vsi,
    int verify_alloc)
{
    uint8 is_allocated = 0;
    int nof_vsis;
    SHR_FUNC_INIT_VARS(unit);

    /** VSI should be at the range of [1, nof_vsis].*/
    BCM_DNX_VLAN_CHK_ID_NONE(unit, vsi, _SHR_E_PARAM);
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    if (vsi >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid VSI (%d), legal range is 0 - %d\n", vsi, nof_vsis - 1);
    }

    /** To check if the vsi is allocated already.*/
    if (verify_alloc != 0)
    {
        SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));
        if (is_allocated == 0)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "VSI (%d) is not allocated!\n", vsi);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*!
 * \brief
 * Verify STG-ID and VSI for BCM-API: bcm_dnx_stg_vlan_add(). \n
 */
static shr_error_e
dnx_stg_vlan_add_verify(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vsi)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * STG Validations, need to check that the STG is already created
     */
    SHR_IF_ERR_EXIT(dnx_stg_id_verify(unit, stg, TRUE));

    /*
     * VSI Validation, VSI should be at the range of [1, nof_vsis] and created.
     */
    SHR_IF_ERR_EXIT(dnx_vsi_id_verify(unit, vsi, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/*!
 * \brief
 * Verify STG-ID and VSI for BCM-API: bcm_dnx_stg_vlan_remove(). \n
 */
static shr_error_e
dnx_stg_vlan_remove_verify(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vsi)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the legality of the inputs
     */

    /** VSI should be at the range of [1, nof_vsis].*/
    SHR_IF_ERR_EXIT(dnx_vsi_id_verify(unit, vsi, FALSE));

    /** STG Validation*/
    SHR_IF_ERR_EXIT(dnx_stg_id_verify(unit, stg, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/*!
 * \brief
 * Verify STG-ID and VSI for BCM-API: bcm_dnx_stg_vlan_remove_all(). \n
 */
static shr_error_e
dnx_stg_vlan_remove_all_verify(
    int unit,
    bcm_stg_t stg)
{
    bcm_stg_t stg_defl;

    SHR_FUNC_INIT_VARS(unit);

    /** STG Validation*/
    SHR_IF_ERR_EXIT(dnx_stg_id_verify(unit, stg, TRUE));

    /** Verify if the given stg is default stg */
    SHR_IF_ERR_EXIT(dnx_stg_general_info_get(unit, NULL, NULL, &stg_defl, NULL));
    if (stg_defl == stg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Removing vsi from default stg(%d) is not allowed!\n", stg);
    }

exit:
    SHR_FUNC_EXIT;
}

/*!
 * \brief
 * Verify Pointer and VSI for BCM-API: bcm_dnx_vlan_stg_set(). \n
 */
shr_error_e
dnx_vlan_stg_set_verify(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vsi)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_stg_vlan_add_verify(unit, stg, vsi));

exit:
    SHR_FUNC_EXIT;
}

/*!
 * \brief
 * Verify Pointer and VSI for BCM-API: bcm_dnx_vlan_stg_get(). \n
 */
shr_error_e
dnx_vlan_stg_get_verify(
    int unit,
    bcm_vlan_t vsi,
    bcm_stg_t * stg_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    /** VSI should be at the range of [1, nof_vsis] and allocated.*/
    SHR_IF_ERR_EXIT(dnx_vsi_id_verify(unit, vsi, TRUE));

    /** stg_ptr should not be NULL.*/
    SHR_NULL_CHECK(stg_ptr, _SHR_E_PARAM, "stg_ptr");

exit:
    SHR_FUNC_EXIT;

}

/*!
 * \brief
 * Verify the parameters for BCM-API: bcm_dnx_vlan_stp_set(). \n
 */
static bcm_error_t
dnx_vlan_stp_set_verify(
    int unit,
    bcm_vlan_t vsi,
    bcm_port_t port,
    int stp_state)
{
    SHR_FUNC_INIT_VARS(unit);

    /** VSI should be at the range of [1, nof_vsis] and allocated.*/
    SHR_IF_ERR_EXIT(dnx_vsi_id_verify(unit, vsi, TRUE));

    /** The port should be ethernet port.*/
    SHR_IF_ERR_EXIT(dnx_stg_port_verify(unit, port));

    /** The stp_state should be valid state.*/
    if ((stp_state < BCM_STG_STP_DISABLE) || (stp_state > BCM_STG_STP_FORWARD))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid STP state (%d)\n", stp_state);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the VSI to the given stg.
 *
 * \param [in] unit - The unit ID
 * \param [in] vsi  - The given vsi
 * \param [in] stg  - The given stg_id
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   STP_TOPOLOGY_ID field in ING_VSI_INFO/EGR_VSI_INFO was updated.
 *
 * \see
 *   None.
 */
static shr_error_e
dnx_vlan_stg_set(
    int unit,
    bcm_vlan_t vsi,
    bcm_stg_t stg)
{
    uint32 entry_handle_id;
    uint32 get_entry_handle_id;
    uint8 stp_topology_id;
    uint32 result_type = DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_BASIC;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (stg == BCM_STG_INVALID)
    {
        stp_topology_id = 0;
    }
    else
    {
        stp_topology_id = BCM_DNX_STG_TO_TOPOLOGY_ID_GET(stg);
    }

    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));

    if (vsi < nof_vsis)
    {
        /** get result type of entry in ingress VSI table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &get_entry_handle_id));
        dbal_entry_key_field32_set(unit, get_entry_handle_id, DBAL_FIELD_VSI, vsi);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            get_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                            &result_type));
         /** update Ingress VSI table*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOPOLOGY_ID, INST_SINGLE, stp_topology_id);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

         /** Egress VSI table*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGR_VSI_INFO_DB, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOPOLOGY_ID, INST_SINGLE, stp_topology_id);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** VSI SW table - for traversing convenience.*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_VSI_STG_INFO_SW, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STP_TOPOLOGY_ID, INST_SINGLE, stp_topology_id);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid VSI (%d)!\n", vsi);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get the VSI to the given stg.
 *
 * \param [in] unit - The unit ID
 * \param [in] vsi  - The given vsi
 * \param [out] stg  - The retrieved stg_id
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   None.
 *
 * \see
 *   None.
 */
static shr_error_e
dnx_vlan_stg_get(
    int unit,
    bcm_vlan_t vsi,
    bcm_stg_t * stg)
{
    uint32 entry_handle_id;
    uint32 stg_topology_id;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stg, _SHR_E_INTERNAL, "A valid pointer for stg id is required!");

    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));

    if (vsi < nof_vsis)
    {
        /** get the stg the VSI belongs to. */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_STG_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STP_TOPOLOGY_ID, INST_SINGLE, &stg_topology_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /** Encoding the topology-id to BCM STG-ID*/
        *stg = BCM_DNX_TOPOLOGY_ID_TO_STG_GET(stg_topology_id);
    }
    else
    {
        *stg = BCM_STG_INVALID;
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid VSI (%d)!\n", vsi);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Removes a VSI from a spanning tree group.
 *   See stg.h for more information.
 */
shr_error_e
dnx_stg_vlan_remove(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vsi,
    int add_defl)
{
    bcm_stg_t stg_defl, stg_cur;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Delete the vsi from current STG
     */

    /** Get the STG the VSI is currently associated to.*/
    stg_cur = BCM_STG_INVALID;
    SHR_IF_ERR_EXIT(dnx_vlan_stg_get(unit, vsi, &stg_cur));
    if (stg_cur != stg)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "vsi %d is currently in STG ID %d, can't delete it from STG ID %d\n", vsi, stg_cur, stg);
    }

    /*
     * Add the vsi to default STG if it is not destroyed
     */

    /** Check whether the vsi was destroyed already.*/
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));
    if ((is_allocated == FALSE) || (add_defl == FALSE))
    {
        /** The VSI was destroyed already, clear the stg info in the VSI*/
        SHR_IF_ERR_EXIT(dnx_vlan_stg_set(unit, vsi, BCM_STG_INVALID));
    }
    else
    {
        /*
         * Add the VSI to default STG if required.
         * add_defl should be FALSE if the vsi is to be destroyed.
         */
        SHR_IF_ERR_EXIT(dnx_stg_general_info_get(unit, NULL, NULL, &stg_defl, NULL));
        SHR_IF_ERR_EXIT(dnx_vlan_stg_set(unit, vsi, stg_defl));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Compare routine for sorting on VSI.
 */
static int
dnx_stg_vsi_compare(
    void *a,
    void *b)
{
    uint16 a16, b16;

    a16 = *(uint16 *) a;
    b16 = *(uint16 *) b;

    return (a16 - b16);
}

/**
 * \brief
 *   Associate a VSI with a Spanning Tree Group.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - The specified Spanning Tree Group.
 * \param [in] vid - The incoming VSI
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t

 * \remark
 *   VSI - Virtual Switching Instance is a generalization of the VLAN concept used primarily in advanced bridging
 *   application. A VSI interconnects Logical Interfaces (LIFs).
 *   VSI is a logical partition of the MAC table and a flooding domain (comprising its member interfaces).
 *   For more information about VSI , see the Programmer's
 *
 * \see
 *   None.
 */
bcm_error_t
bcm_dnx_stg_vlan_add(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vid)
{
    bcm_stg_t stg_cur;
    shr_error_e ret = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stg_vlan_add_verify(unit, stg, vid));

    /*
     * Delete the vsi from current STG
     */

    /** Get the STG the VSI is currently associated to.*/
    stg_cur = BCM_STG_INVALID;
    ret = dnx_vlan_stg_get(unit, vid, &stg_cur);
    if ((ret != _SHR_E_NONE) && (ret != _SHR_E_NOT_FOUND))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error in getting the stg by the VSI!\n");
    }

    /*
     * Add the VSI the new STG
     */
    if (stg_cur == stg)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Vid %d is in the stg %d already%s%s !\r\n", vid, stg_cur, EMPTY, EMPTY);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_stg_set(unit, vid, stg));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Removes a VSI from a spanning tree group.
 *   The VSI is placed back in the default spanning tree group.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - The specified Spanning Tree Group.
 * \param [in] vid - The incoming VSI
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   None.
 */
bcm_error_t
bcm_dnx_stg_vlan_remove(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t vid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stg_vlan_remove_verify(unit, stg, vid));

    SHR_IF_ERR_EXIT(dnx_stg_vlan_remove(unit, stg, vid, TRUE));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Removes all VSIs from a spanning tree group.
 *   The VSIs are placed back in the default spanning tree group.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - The specified Spanning Tree Group.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   bcm_dnx_stg_vlan_remove.
 */
bcm_error_t
bcm_dnx_stg_vlan_remove_all(
    int unit,
    bcm_stg_t stg)
{
    bcm_vlan_t vsi_id = BCM_VLAN_NONE;
    int nof_vsis;
    bcm_stg_t cur_stg;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stg_vlan_remove_all_verify(unit, stg));

    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));

    for (vsi_id = BCM_VLAN_DEFAULT; vsi_id < nof_vsis; vsi_id++)
    {
        /** Check whether the vsi was allocated already.*/
        SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi_id, &is_allocated));
        if (is_allocated == FALSE)
        {
            continue;
        }

        /** Get the stg the VSI belongs t0. */
        SHR_IF_ERR_EXIT(dnx_vlan_stg_get(unit, vsi_id, &cur_stg));
        if (cur_stg != stg)
        {
            continue;
        }

        /** Remove the vsi from the stg, reset it to default stg. */
        SHR_IF_ERR_EXIT(bcm_dnx_stg_vlan_remove(unit, stg, vsi_id));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieve the VSIs belonging to the specified Spanning Tree Group.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - The given Spanning Tree Group ID.
 * \param [out] list - The poniter used to receive the VSIs.
 * \param [out] count - The number of VSIs in the list.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   Generally, bcm_dnx_stg_vlan_list_destroy should be called after this API.
 *
 * \see
 *   bcm_dnx_stg_vlan_list_destroy
 */
bcm_error_t
bcm_dnx_stg_vlan_list(
    int unit,
    bcm_stg_t stg,
    bcm_vlan_t ** list,
    int *count)
{
    bcm_vlan_t vsi_id = BCM_VLAN_NONE;
    int nof_vsis;
    bcm_stg_t cur_stg;
    uint8 is_allocated;
    bcm_vlan_t *vsi_list = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(count, BCM_E_PARAM, "count");

    /** Verify the stg-id */
    SHR_INVOKE_VERIFY_DNX(dnx_stg_id_verify(unit, stg, TRUE));

    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));

    /** Alloc mem for storing the vsi list.*/
    SHR_ALLOC(vsi_list, nof_vsis * sizeof(bcm_vlan_t), "bcm_dnx_stg_vlan_list", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    if (vsi_list == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Alloc mem failed.\n");
    }

    /** Iterate all vaild VSIs and check if they blong to the stg*/
    *count = 0;
    for (vsi_id = BCM_VLAN_DEFAULT; vsi_id < nof_vsis; vsi_id++)
    {
        /** Check whether the vsi was allocted already.*/
        SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi_id, &is_allocated));
        if (is_allocated == FALSE)
        {
            continue;
        }

        /** Get the stg the VSI belongs to. */
        SHR_IF_ERR_EXIT(dnx_vlan_stg_get(unit, vsi_id, &cur_stg));
        if (cur_stg != stg)
        {
            continue;
        }

        /** Store the vsi in vsi_list. */
        vsi_list[*count] = vsi_id;
        (*count)++;
    }

    if (*count != 0)
    {
        /** Found VSIs in the stg, sorting them even they may be in sequence.*/
        _shr_sort(vsi_list, *count, sizeof(bcm_vlan_t), dnx_stg_vsi_compare);

        if (*list != NULL)
        {
            /** User should ensure the space in the list should be enough.*/
            LOG_WARN(BSL_LS_BCM_STG,
                     (BSL_META_U(unit, "%s: unit(%d), User should ensure enough space in the list\n"),
                      FUNCTION_NAME(), unit));
        }
        else
        {
            SHR_ALLOC(*list, (*count) * sizeof(bcm_vlan_t), "bcm_dnx_stg_vlan_list", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            if (*list == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "Alloc mem failed.\n");
            }
        }

        /** Copy the list to output*/
        sal_memcpy(*list, vsi_list, (*count) * sizeof(bcm_vlan_t));
    }

exit:
    if (vsi_list != NULL)
    {
        SHR_FREE(vsi_list);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Free the memory used to recieve the VSIs from bcm_stg_vlan_list.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] list - The poniter to receive the VSIs.
 * \param [in] count - The number of VSIs in the list.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   Generally, this API should be called after bcm_stg_vlan_list.
 *
 * \see
 *   bcm_dnx_stg_vlan_list
 */
bcm_error_t
bcm_dnx_stg_vlan_list_destroy(
    int unit,
    bcm_vlan_t * list,
    int count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (NULL != list)
    {
        SHR_FREE(list);
    }

    SHR_IF_ERR_EXIT(BCM_E_NONE);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Puts the specified VSI into the specified spanning tree group.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] vid - The specified VSI.
 * \param [in] stg - The STG the VSI will be added to.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   bcm_dnx_vlan_stg_vlan_get
 */
bcm_error_t
bcm_dnx_vlan_stg_set(
    int unit,
    bcm_vlan_t vid,
    bcm_stg_t stg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_stg_set_verify(unit, stg, vid));

    /*
     * If not BVID, add the vsi to the stg
     */
    SHR_IF_ERR_EXIT(bcm_dnx_stg_vlan_add(unit, stg, vid));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieves the spanning tree group that contains the specified VSI.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] vid - The specified ID for VSI.
 * \param [in] stg_ptr - Pointer to returned STG ID.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   bcm_dnx_vlan_stg_vlan_set
 */
bcm_error_t
bcm_dnx_vlan_stg_get(
    int unit,
    bcm_vlan_t vid,
    bcm_stg_t * stg_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_stg_get_verify(unit, vid, stg_ptr));

    /** Get the stg from VSI */
    SHR_IF_ERR_EXIT(dnx_vlan_stg_get(unit, vid, stg_ptr));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the spanning tree state for a port in the whole spanning tree group
 *   that contains the specified VSI.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] vid - The specified VSI.
 * \param [in] port - Device or logical port number or virtual port number.
 * \param [in] stp_state - Spanning Tree Protocol state.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   bcm_dnx_vlan_stp_get
 */
bcm_error_t
bcm_dnx_vlan_stp_set(
    int unit,
    bcm_vlan_t vid,
    bcm_port_t port,
    int stp_state)
{
    bcm_stg_t stg = BCM_STG_INVALID;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Defined APIs were called, so no needes to verify the inputs specially.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_stp_set_verify(unit, vid, port, stp_state));

    /*
     * Get the stg the vsi was associated to
     */
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_stg_get(unit, vid, &stg));

    /*
     * Set the STP state for the port in the stg
     */
    SHR_IF_ERR_EXIT(bcm_dnx_stg_stp_set(unit, stg, port, stp_state));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the spanning tree state for a port in the whole spanning tree group
 *   that contains the specified VSI.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] vid - The specified VSI.
 * \param [in] port - Device or logical port number or virtual port number.
 * \param [out] stp_state - Spanning Tree Protocol state.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   bcm_dnx_vlan_stp_set
 */
bcm_error_t
bcm_dnx_vlan_stp_get(
    int unit,
    bcm_vlan_t vid,
    bcm_port_t port,
    int *stp_state)
{
    bcm_stg_t stg = BCM_STG_INVALID;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Defined APIs were called, so no needes to verify the inputs specially.
     */
    /** SHR_INVOKE_VERIFY_DNX(dnx_vlan_stp_get_verify(unit, vsi, port, stp_state));*/

    /*
     * Get the stg the vsi was associated to
     */
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_stg_get(unit, vid, &stg));

    /*
     * Set the STP state for the port in the stg
     */
    SHR_IF_ERR_EXIT(bcm_dnx_stg_stp_get(unit, stg, port, stp_state));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sets the spanning tree state for a port for all the VLANs in the VLAN Vector.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] vlan_vector - VLAN Vector.
 * \param [in] port - Device or logical port number or virtual port number.
 * \param [in] stp_state - Spanning Tree Protocol state.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   bcm_dnx_vlan_stg_vlan_set
 */
bcm_error_t
bcm_dnx_vlan_vector_stp_set(
    int unit,
    bcm_vlan_vector_t vlan_vector,
    bcm_port_t port,
    int stp_state)
{
    bcm_vlan_t vlan_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Defined APIs were called, so no needes to verify the inputs specially.
     */

    /*
     * Iterate all the VLANs in the vector to set the stp state
     */
    /** Start the iterator from vlan 1, since vlan 0 is priority tag*/
    vlan_id = BCM_VLAN_MIN + 1;
    for (; vlan_id <= BCM_VLAN_MAX; vlan_id++)
    {
        if (BCM_VLAN_VEC_GET(vlan_vector, vlan_id))
        {
            SHR_IF_ERR_EXIT(bcm_dnx_vlan_stp_set(unit, vlan_id, port, stp_state));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
