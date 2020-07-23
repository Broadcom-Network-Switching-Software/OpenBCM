/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_instru.c
 *
 * Wrapper functions for utilex_multi_set.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm_int/dnx/algo/instru/algo_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_instru.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/instru_access.h>

/**
 * }
 */

#define DNX_ALGO_INSTRU_UDP_TUNNEL_PROFILE "sFlow UDP tunnel profile"

void
dnx_algo_instru_sflow_destination_profile_print_cb(
    int unit,
    const void *data)
{
    sflow_destination_key_t *destination_key = (sflow_destination_key_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "UDP tunnel", destination_key->udp_tunnel,
                                   NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Sub agent id", destination_key->sub_agent_id,
                                   NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "Eventor id", destination_key->eventor_id,
                                   NULL, "0x%08x");

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief -
 * Create Template instru UDP tunnel. Each profile only uses
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_instru_sflow_destination_profile_create(
    int unit)
{
    dnx_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set a template for vsi profiles
     */
    sal_memset(&data, 0, sizeof(data));

    data.data_size = sizeof(sflow_destination_key_t);
    data.first_profile = 0;
    data.flags = 0;
    /*
     * At most one profile per encap, at most all encaps on one profile.
     */
    data.max_references = dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit);
    data.nof_profiles = dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit);

    sal_strncpy(data.name, DNX_ALGO_INSTRU_UDP_TUNNEL_PROFILE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_destination_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_instru_encap_to_sflow_destination_profile_init(
    int unit)
{
    sw_state_htbl_init_info_t htb_init_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Map from encap to UDP tunnel profile.
     */
    sal_memset(&htb_init_info, 0, sizeof(sw_state_htbl_init_info_t));
    htb_init_info.max_nof_elements = dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit);
    htb_init_info.expected_nof_elements = dnx_data_instru.sflow.max_nof_sflow_encaps_get(unit);
    htb_init_info.hash_function = NULL;

    SHR_IF_ERR_EXIT(instru.sflow_info.sflow_encap_to_sflow_destination_profile.create(unit, &htb_init_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_instru_eventor_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_bitmap.alloc_bitmap(unit));
    SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_bitmap.alloc_bitmap(unit));
    SHR_IF_ERR_EXIT(instru.eventor_info.rx_sram_banks_bitmap.alloc_bitmap(unit));
    SHR_IF_ERR_EXIT(instru.eventor_info.tx_sram_banks_bitmap.alloc_bitmap(unit));
    SHR_IF_ERR_EXIT(instru.eventor_info.builder_configured_bitmap.alloc_bitmap(unit));
    SHR_IF_ERR_EXIT(instru.eventor_info.rx_contexts_info.alloc(unit));
    SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_info.alloc(unit));
    SHR_IF_ERR_EXIT(instru.eventor_info.tx_contexts_queue_type_bitmap.alloc_bitmap(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_instru_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init instru sw state
     */
    SHR_IF_ERR_EXIT(instru.init(unit));

    SHR_IF_ERR_EXIT(instru.sflow_info.raw_outlifs_by_profile.alloc(unit));

    /*
     * Initialize the UDP tunnel profile template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_instru_sflow_destination_profile_create(unit));

    /*
     * Initialize the encap to UDP tunnel profile hash.
     */
    SHR_IF_ERR_EXIT(dnx_algo_instru_encap_to_sflow_destination_profile_init(unit));

    /** Initialize eventor sw state */
    if (dnx_data_instru.eventor.feature_get(unit, dnx_data_instru_eventor_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_algo_instru_eventor_sw_state_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
