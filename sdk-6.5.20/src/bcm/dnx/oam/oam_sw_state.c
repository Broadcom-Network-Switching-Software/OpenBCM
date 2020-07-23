/** \file oam_sw_state.c
 * 
 *
 * OAM sw_state access functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

/*
 *  * Include files. * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <bcm/oam.h>
#include <soc/dnx/mdb.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_max_oam.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <include/soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
/*
 *  }
 */

/*
 * See prototype in oam_internal.h for details
 */
shr_error_e
dnx_oam_lif_info_t_init(
    int unit,
    dnx_oam_lif_info_t * lif_info)
{
    uint8 level;

    SHR_FUNC_INIT_VARS(unit);

    lif_info->mp_profile = BCM_OAM_PROFILE_INVALID;

    for (level = 0; level <= DNX_OAM_MAX_MDL; level++)
    {
        lif_info->endpoint_id[level] = BCM_OAM_ENDPOINT_INVALID;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function translated accelerated endpoint_id to 
 * index to rmep linked list's head. 
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - accelerated oam mep's ID
 * \param [out] ll_index - pointer to linked list index
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_sw_db_get_rmep_ll_index(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    int *ll_index)
{

    int mep_db_treshold;
    SHR_FUNC_INIT_VARS(unit);

    mep_db_treshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);

    /*
     * Each mep below threshold holds 4 ids 
     */
    if (endpoint_id >= mep_db_treshold)
    {
        *ll_index = mep_db_treshold + endpoint_id / 4;
    }
    else
    {
        *ll_index = endpoint_id;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - This function looks up an oam-lif from the OAM SW 
 *        state LIF table, and reads the data to the struct
 *        referenced by the user provided pointer.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_lif - oam_lif ID of the lif to retrieve
 *                       global-lif for ingress,
 *                       local-lif for egress.
 * \param [in] is_ingress - side of entry to get.
 * \param [out] lif_info - pointer to struct to return the 
 *             lif information
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
dnx_oam_sw_state_oam_lif_info_get(
    int unit,
    uint32 oam_lif,
    uint8 is_ingress,
    dnx_oam_lif_info_t * lif_info)
{
    int index;
    uint32 entry_handle_id;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_lif_info_t_init(unit, lif_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_LIF_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF, oam_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS, is_ingress);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, INST_SINGLE, &lif_info->mp_profile);
    /** Value fields */
    for (index = 0; index <= DNX_OAM_MAX_MDL; index++)
    {
        /** Loop over all possible levels defined in standard: 0-7 */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_ID, index,
                                   &lif_info->endpoint_id[index]);
    }

    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    SHR_SET_CURRENT_ERR(rv);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See oam_internal.h
 */
shr_error_e
dnx_oam_sw_state_oam_lif_info_set(
    int unit,
    dbal_entry_action_flags_e dbal_oper_flag,
    uint32 oam_lif,
    uint8 is_ingress,
    dnx_oam_lif_info_t * lif_info)
{
    int index;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_LIF_INFO_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF, oam_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS, is_ingress);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, INST_SINGLE, lif_info->mp_profile);

    for (index = 0; index <= DNX_OAM_MAX_MDL; index++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_ID, index,
                                     lif_info->endpoint_id[index]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_oper_flag));

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function looks up an Remote endpoint-info from the OAM
 *        SW state REMOTE_ENDPOINT_INFO table, and reads the data to
 *        the struct referenced by the user provided pointer.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - Remote MEP HW idx.
 * \param [out] sw_endpoint_info - Pointer to temporary
 *        structure to which data will be written for subsequent
 *        use.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
dnx_oam_sw_state_remote_endpoint_info_get(
    int unit,
    uint32 rmep_idx,
    dnx_oam_remote_endpoint_info_t * sw_endpoint_info)
{
    uint32 entry_handle_id, rmep_id, oam_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_REMOTE_ENDPOINT_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RMEP_ID, INST_SINGLE, &rmep_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, &oam_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    sw_endpoint_info->rmep_id = rmep_id;
    sw_endpoint_info->oam_id = oam_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function clear an Remote endpoint-info from the OAM
 *        SW state REMOTE_ENDPOINT_INFO table
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - Remote MEP HW idx.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
dnx_oam_sw_state_remote_endpoint_info_delete(
    int unit,
    uint32 rmep_idx)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_REMOTE_ENDPOINT_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - This function sets an Remote endpoint-info into the OAM
 *        SW state REMOTE_ENDPOINT_INFO table, based on the data in 
 *        the struct referenced by the user provided pointer.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - Remote MEP HW idx.
 * \param [in] sw_endpoint_info - Pointer to temporary
 *        structure from which data will be written for subsequent
 *        use.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
dnx_oam_sw_state_remote_endpoint_info_set(
    int unit,
    uint32 rmep_idx,
    const dnx_oam_remote_endpoint_info_t * sw_endpoint_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_REMOTE_ENDPOINT_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_ID, INST_SINGLE, sw_endpoint_info->rmep_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, sw_endpoint_info->oam_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See oam_internal.h
 */
shr_error_e
dnx_oam_sw_state_endpoint_info_get(
    int unit,
    uint32 endpoint_id,
    dnx_oam_endpoint_info_t * sw_endpoint_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ENDPOINT_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_ID, endpoint_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_SW_STATE_FLAGS, INST_SINGLE,
                               &sw_endpoint_info->sw_state_flags);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_SW_STATE_TYPE, INST_SINGLE,
                               &sw_endpoint_info->type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, INST_SINGLE, &sw_endpoint_info->group);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE, &sw_endpoint_info->level);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &sw_endpoint_info->gport);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LM_COUNTER_BASE_IDX, INST_SINGLE,
                               &sw_endpoint_info->lm_counter_idx);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LM_COUNTER_IF, INST_SINGLE,
                               &sw_endpoint_info->lm_counter_if);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MPLS_OUT_GPORT, INST_SINGLE,
                               &sw_endpoint_info->mpls_out_gport);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SERVER_TRAP, INST_SINGLE,
                               &sw_endpoint_info->server_trap);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TCAM_INDEX, INST_SINGLE, &sw_endpoint_info->tcam_index);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_INDEX, INST_SINGLE,
                               &sw_endpoint_info->extra_data_index);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_LENGTH, INST_SINGLE,
                              &sw_endpoint_info->extra_data_length);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_RX_SIGNAL, INST_SINGLE, &sw_endpoint_info->rx_signal);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TX_SIGNAL, INST_SINGLE, &sw_endpoint_info->tx_signal);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See oam_internal.h
 */
shr_error_e
dnx_oam_sw_state_endpoint_info_set(
    int unit,
    uint32 endpoint_id,
    dnx_oam_endpoint_info_t * sw_endpoint_info,
    uint8 is_update)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ENDPOINT_INFO_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_ID, endpoint_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_SW_STATE_FLAGS, INST_SINGLE,
                                 sw_endpoint_info->sw_state_flags);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_SW_STATE_TYPE, INST_SINGLE,
                                sw_endpoint_info->type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, INST_SINGLE, sw_endpoint_info->group);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDL, INST_SINGLE, sw_endpoint_info->level);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, sw_endpoint_info->gport);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_COUNTER_BASE_IDX, INST_SINGLE,
                                 sw_endpoint_info->lm_counter_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_COUNTER_IF, INST_SINGLE,
                                 sw_endpoint_info->lm_counter_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_OUT_GPORT, INST_SINGLE,
                                 sw_endpoint_info->mpls_out_gport);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVER_TRAP, INST_SINGLE,
                                 sw_endpoint_info->server_trap);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCAM_INDEX, INST_SINGLE,
                                 sw_endpoint_info->tcam_index);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_INDEX, INST_SINGLE,
                                 sw_endpoint_info->extra_data_index);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_LENGTH, INST_SINGLE,
                                sw_endpoint_info->extra_data_length);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RX_SIGNAL, INST_SINGLE, sw_endpoint_info->rx_signal);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_SIGNAL, INST_SINGLE, sw_endpoint_info->tx_signal);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, is_update ? DBAL_COMMIT_UPDATE : DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function deletes an endpoint-info from the OAM 
 *        SW state LIF table,
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - id of the endpoint to retrieve
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
dnx_oam_endpoint_info_remove_from_sw_state(
    int unit,
    uint32 endpoint_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ENDPOINT_INFO_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_ID, endpoint_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  The function initializes the sw db structure for oam group
 *  , which has an array of linked lists. The size of the 
 *  array is also allocated to be the size of number of OAM
 *  groups in device. The index of the array is the group id and
 *  the linked list is the eps related to the group
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e
dnx_oam_sw_db_ma_to_mep_create(
    int unit)
{
    int capacity;
    int nof_entries;
    int nof_endpoints;
    int nof_groups;
    sw_state_ll_init_info_t init_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_IOEM_1, &capacity));

    /*
     * Get Number of groups ==================== Number of entries in lif db = IOEM_capacity / 2 ( size of elemnt is
     * 30 bits, each entry has <60) Number of endpoints = 8 * Number of entries in lif db Number of groups = Number
     * of endpoints 
     */
    nof_entries = capacity / 2;
    nof_endpoints = 8 * nof_entries;
    nof_groups = nof_endpoints;

    init_info.max_nof_elements = nof_endpoints;
    init_info.expected_nof_elements = nof_endpoints;
    init_info.nof_heads = nof_groups;

    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.oam_group_array_of_linked_lists.create_empty(unit, &init_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  The function destroyes the sw db structure and frees the
 *  array
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e
dnx_oam_sw_db_ma_to_mep_destroy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   The function adds the OAM EP to relavent OAM group link
 *   list
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group_id- 
 *  ID of the OAM group linked list
 * \param [in] ep_id - 
 *  Id of EP to add to linked list
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e
dnx_oam_sw_db_ma_to_mep_insert_mep(
    int unit,
    bcm_oam_group_t group_id,
    bcm_oam_endpoint_t ep_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.
                    oam_group_array_of_linked_lists.add_last(unit, group_id, &ep_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   The function returns the node of EP, if it is found in the
 *   group sw link list. function sets found param to TRUE if EP
 *   is found in group
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group_id- 
 *  ID of the OAM group  linked list
 * \param [in] ep_id- 
 *  Id of EP to add to linked list [in] ep_id- id of EP to add
 * to linked list 
 * \param [out] node - 
 *   if value of EP is found in list than node gets the node
 *   with ep_id. if not, it gets the value of last node
 *  \param [out] found -
 *    if value of EP is found in list the found param
 *    is set to TRUE. if not, found is set to false.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
static shr_error_e
dnx_oam_sw_db_ma_to_mep_find_mep(
    int unit,
    bcm_oam_group_t group_id,
    bcm_oam_endpoint_t ep_id,
    sw_state_ll_node_t * node,
    uint8 *found)
{
    bcm_oam_group_t val = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.
                    oam_group_array_of_linked_lists.get_first(unit, group_id, node));

    *found = FALSE;
    /*
     * Iterate over the linked list to check if EP exsists in group link listed 
     */
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.
                        oam_group_array_of_linked_lists.node_value(unit, *node, &val));
        /** EP found in group linked list */

        if (val == ep_id)
        {
            *found = TRUE;
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.
                        oam_group_array_of_linked_lists.next_node(unit, group_id, *node, node));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See oam_internal.h
*/
shr_error_e
dnx_oam_sw_db_ma_to_mep_delete_mep(
    int unit,
    bcm_oam_group_t group_id,
    bcm_oam_endpoint_t ep_id)
{
    sw_state_ll_node_t node_to_remove;
    uint8 found = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_ma_to_mep_find_mep(unit, group_id, ep_id, &node_to_remove, &found));
    /** EP found in group linked list */
    if (found == TRUE)
    {
        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.
                        oam_group_array_of_linked_lists.remove_node(unit, group_id, node_to_remove));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MEP not found in group linked list ");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  The function initializes sw db structures for oam
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e
dnx_oam_sw_db_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(oam_sw_db_info.init(unit));

    /** init the SW DB of OAM EPs linked to each OAM group */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_ma_to_mep_create(unit));

    /** init the SW DB of OAM RMEPs linked to each OAM endpoint */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_create(unit));

        /** Initialize profile_is_slm table */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lif_mp_profile_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  The function de-initializes sw db structures for oam
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e
dnx_oam_sw_db_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** deinit the SW DB of OAM EPs linked to each OAM group */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_ma_to_mep_destroy(unit));

    /** deinit the SW DB of OAM RMEPs linked to each OAM endpoint */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_destroy(unit));

    /*
     * sw state module deinit is done automatically at device deinit 
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * See oam_internal.h
*/
shr_error_e
dnx_oam_sw_db_lmep_to_rmep_create(
    int unit)
{
    sw_state_ll_init_info_t init_info;
    uint32 nof_scan_rmep_entries = 0;
    uint32 nof_mep_db_endpoint_entries = 0;
    uint32 rmep_db_treshold = 0;
    uint32 mep_db_treshold;

    SHR_FUNC_INIT_VARS(unit);

    nof_scan_rmep_entries = dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) + 1;
    rmep_db_treshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));

    if (rmep_db_treshold < nof_scan_rmep_entries)
    {
        init_info.max_nof_elements = rmep_db_treshold + (nof_scan_rmep_entries - rmep_db_treshold) / 2;
        init_info.expected_nof_elements = rmep_db_treshold + (nof_scan_rmep_entries - rmep_db_treshold) / 2;
    }
    else
    {
        init_info.max_nof_elements = nof_scan_rmep_entries;
        init_info.expected_nof_elements = nof_scan_rmep_entries;
    }

    mep_db_treshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_mep_db_endpoint_entries = dnx_data_oam.oamp.nof_mep_db_endpoint_entries_get(unit);
    init_info.nof_heads = mep_db_treshold + (nof_mep_db_endpoint_entries - (mep_db_treshold / 4));

    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_endpoint_sw_db_info.
                    oam_endpoint_array_of_rmep_linked_lists.create_empty(unit, &init_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See oam.internal.h
*/
shr_error_e
dnx_oam_sw_db_lmep_to_rmep_destroy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * See oam_internal.h
*/
shr_error_e
dnx_oam_sw_db_lmep_to_rmep_insert_rmep(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    bcm_oam_endpoint_t rmep_id)
{
    int ll_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_get_rmep_ll_index(unit, endpoint_id, &ll_index));

    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_endpoint_sw_db_info.
                    oam_endpoint_array_of_rmep_linked_lists.add_last(unit, ll_index, &rmep_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   The function traverses the RMEP linked list and calls the callback function
 *   for each RMEP in the linked list.
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] endpoint_id- 
 *  ID of the OAM endpoint
 * \param [in] cb - 
 *  Callback function called for each RMEP in the list.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e
dnx_oam_sw_db_lmep_to_rmep_traverse_list(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    dnx_oam_sw_db_lmep_to_rmep_traverse_cb_f cb)
{
    bcm_oam_endpoint_t val = 0;
    shr_error_e rv = _SHR_E_NONE;
    sw_state_ll_node_t node;
    int ll_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_get_rmep_ll_index(unit, endpoint_id, &ll_index));

    rv = oam_sw_db_info.oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists.get_first(unit, ll_index,
                                                                                                  &node);
    if (rv != _SHR_E_NOT_FOUND)
    {
        if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(rv, "Getting first node failed\r\n");
        }
    }
    else
    {
        SHR_EXIT();
    }

    /*
     * Iterate over the linked list to call for each RMEP exists in endpoint link list 
     */
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_endpoint_sw_db_info.
                        oam_endpoint_array_of_rmep_linked_lists.node_value(unit, node, &val));

        /*
         * Call the callback 
         */
        SHR_IF_ERR_EXIT(cb(unit, endpoint_id, val));
        oam_sw_db_info.oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists.next_node(unit, ll_index,
                                                                                                 node, &node);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   The function returns the node of RMEP, if it is found in the
 *   endpoint sw link list. function sets found param to TRUE if RMEP
 *   is found in endpoint linked list
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] endpoint_id- 
 *  ID of the OAM endpoint
 * \param [in] rmep_id- 
 *  Id of RMEP to add to linked list 
 * \param [out] node - 
 *   if value of RMEP is found in list than node gets the node
 *   with rmep_id. if not, it gets the value of last node
 *  \param [out] found -
 *    if value of RMEP is found in list the found param
 *    is set to TRUE. if not, found is set to false.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
static shr_error_e
dnx_oam_sw_db_lmep_to_rmep_find_rmep(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    bcm_oam_endpoint_t rmep_id,
    sw_state_ll_node_t * node,
    uint8 *found)
{
    int ll_index;
    bcm_oam_endpoint_t val = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_get_rmep_ll_index(unit, endpoint_id, &ll_index));

    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_endpoint_sw_db_info.
                    oam_endpoint_array_of_rmep_linked_lists.get_first(unit, ll_index, node));

    /*
     * Iterate over the linked list to check if RMEP exsists in endpoint link listed 
     */
    while (DNX_SW_STATE_LL_IS_NODE_VALID(*node))
    {
        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_endpoint_sw_db_info.
                        oam_endpoint_array_of_rmep_linked_lists.node_value(unit, *node, &val));

        /** RMEP found in endpoint linked list */
        if (val == rmep_id)
        {
            *found = TRUE;
            SHR_EXIT();
        }
        oam_sw_db_info.oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists.next_node(unit, ll_index,
                                                                                                 *node, node);
    }
    *found = FALSE;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See oam_internal.h
*/
shr_error_e
dnx_oam_sw_db_lmep_to_rmep_delete_rmep(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    bcm_oam_endpoint_t rmep_id)
{
    int ll_index;
    sw_state_ll_node_t node_to_remove;
    uint8 found = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_find_rmep(unit, endpoint_id, rmep_id, &node_to_remove, &found));
    /** EP found in endpoint linked list */
    if (found == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_get_rmep_ll_index(unit, endpoint_id, &ll_index));

        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_endpoint_sw_db_info.
                        oam_endpoint_array_of_rmep_linked_lists.remove_node(unit, ll_index, node_to_remove));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MEP not found in endpoint linked list ");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   The function empties the OAM RMEP LIST of given LMEP
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] endpoint_id- 
 *  ID of the OAM endpoint
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e
dnx_oam_sw_db_lmep_to_rmep_empty_rmep_list(
    int unit,
    bcm_oam_endpoint_t endpoint_id)
{
    int ll_index;
    bcm_oam_endpoint_t val = 0;
    sw_state_ll_node_t node;
    bcm_oam_endpoint_t *rmep_id_list = NULL;
    uint32 num_rmeps_in_list = 0;
    uint32 count = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_get_rmep_ll_index(unit, endpoint_id, &ll_index));

    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_endpoint_sw_db_info.
                    oam_endpoint_array_of_rmep_linked_lists.nof_elements(unit, ll_index, &num_rmeps_in_list));

    /*
     * No elements in list. Nothing to empty 
     */
    if (num_rmeps_in_list == 0)
    {
        SHR_EXIT();
    }
    rmep_id_list = sal_alloc(num_rmeps_in_list * sizeof(bcm_oam_endpoint_t), "oam event fifo");

    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_endpoint_sw_db_info.
                    oam_endpoint_array_of_rmep_linked_lists.get_first(unit, ll_index, &node));

    /*
     * Iterate over the linked list get each RMEP exists in endpoint link list 
     */
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_endpoint_sw_db_info.
                        oam_endpoint_array_of_rmep_linked_lists.node_value(unit, node, &val));
        /*
         * Store the RMEP ID in the temporary list.
         */
        rmep_id_list[count] = val;
        count++;

        oam_sw_db_info.oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists.next_node(unit, ll_index,
                                                                                                 node, &node);
    }

    if (count != num_rmeps_in_list)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "count of nodes not equal to no of rmeps in list\n");
    }

    count = 0;

    /*
     * Iterate over the linked list delete each RMEP exists in endpoint link list 
     */
    while (count != num_rmeps_in_list)
    {
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_delete_rmep(unit, endpoint_id, rmep_id_list[count]));
        count++;
    }

exit:
    if (rmep_id_list != NULL)
    {
        sal_free(rmep_id_list);
    }
    SHR_FUNC_EXIT;
}

/** See details in oam_internal.h */
shr_error_e
dnx_oam_sw_db_lm_mode_get(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 *profile_is_slm,
    uint8 *profile_is_section)
{
    uint32 entry_handle_id;
    int dbal_table = is_acc ? DBAL_TABLE_OAM_LIF_ACC_MP_PROFILE_INFO_SW : DBAL_TABLE_OAM_LIF_NON_ACC_MP_PROFILE_INFO_SW;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS, is_ingress);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LM_MODE, INST_SINGLE, profile_is_slm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_OAM_SECTION, INST_SINGLE, profile_is_section);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See details in oam_internal.h */
shr_error_e
dnx_oam_sw_db_lm_mode_set(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 profile_is_slm,
    uint8 profile_is_section)
{
    uint32 entry_handle_id;
    int dbal_table = is_acc ? DBAL_TABLE_OAM_LIF_ACC_MP_PROFILE_INFO_SW : DBAL_TABLE_OAM_LIF_NON_ACC_MP_PROFILE_INFO_SW;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS, is_ingress);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_MODE, INST_SINGLE, profile_is_slm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OAM_SECTION, INST_SINGLE, profile_is_section);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See details in oam_internal.h */
shr_error_e
dnx_oam_sw_db_dm_mode_get(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 *profile_is_ntp,
    uint8 *profile_is_section)
{
    uint32 entry_handle_id;
    int dbal_table = is_acc ? DBAL_TABLE_OAM_LIF_ACC_MP_PROFILE_INFO_SW : DBAL_TABLE_OAM_LIF_NON_ACC_MP_PROFILE_INFO_SW;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS, is_ingress);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DM_MODE, INST_SINGLE, profile_is_ntp);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_OAM_SECTION, INST_SINGLE, profile_is_section);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See details in oam_internal.h */
shr_error_e
dnx_oam_sw_db_dm_mode_set(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 profile_is_ntp,
    uint8 profile_is_section)
{
    uint32 entry_handle_id;
    int dbal_table = is_acc ? DBAL_TABLE_OAM_LIF_ACC_MP_PROFILE_INFO_SW : DBAL_TABLE_OAM_LIF_NON_ACC_MP_PROFILE_INFO_SW;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS, is_ingress);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DM_MODE, INST_SINGLE, profile_is_ntp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OAM_SECTION, INST_SINGLE, profile_is_section);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See details in oam_internal.h */
shr_error_e
dnx_oam_sw_db_hlm_mode_set(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 hlm_dis)
{
    uint32 entry_handle_id;
    int dbal_table = is_acc ? DBAL_TABLE_OAM_LIF_ACC_MP_PROFILE_INFO_SW : DBAL_TABLE_OAM_LIF_NON_ACC_MP_PROFILE_INFO_SW;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS, is_ingress);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HLM_DIS, INST_SINGLE, hlm_dis);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See details in oam_internal.h */
shr_error_e
dnx_oam_sw_db_hlm_mode_get(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 *hlm_dis)
{
    uint32 entry_handle_id;
    int dbal_table = is_acc ? DBAL_TABLE_OAM_LIF_ACC_MP_PROFILE_INFO_SW : DBAL_TABLE_OAM_LIF_NON_ACC_MP_PROFILE_INFO_SW;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS, is_ingress);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_HLM_DIS, INST_SINGLE, hlm_dis);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See details in oam_internal.h */
shr_error_e
dnx_oam_sw_db_lif_mp_profile_init(
    int unit)
{
    uint32 entry_handle_id;
    int table_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Init both accelerated and non-accelerated profile tables */
    for (table_index = 0; table_index < 2; table_index++)
    {
        if (table_index == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_LIF_ACC_MP_PROFILE_INFO_SW, &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_LIF_NON_ACC_MP_PROFILE_INFO_SW, &entry_handle_id));
        }

        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IS_INGRESS, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LM_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_LM_MODE_LM_MODE_INVALID);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DM_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_DM_MODE_DM_MODE_INVALID);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See details in oam_internal.h */
shr_error_e
dnx_oam_sw_db_lm_session_oam_id_set(
    int unit,
    uint16 d_oam_id,
    uint16 o_oam_id,
    int priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_OAM_ID_SW, &entry_handle_id));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, d_oam_id);

    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, o_oam_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /** See details in oam_internal.h */
shr_error_e
dnx_oam_sw_db_lm_session_oam_id_get(
    int unit,
    uint16 d_oam_id,
    uint16 *o_oam_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_OAM_ID_SW, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, d_oam_id);

    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, o_oam_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
