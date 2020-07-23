
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. $
 */
int
tmp_adapter_gen_workaround_func(
    void)
{
    return 0;
}
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX

#ifdef ADAPTER_SERVER_MODE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <shared/bsl.h>
#include <errno.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnx/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnx/swstate/auto_generated/types/adapter_types.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dcmn/error.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/adapter/adapter_gen.h>


shr_error_e
adapter_egq_interface_port_map_set(
    int unit,
    bcm_port_t port,
    int egr_if)
{
    uint32 entry_handle_id;
    int first_phy;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ADAPTER_EGQ_IF_PORT_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egr_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_PHY, INST_SINGLE, first_phy);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#endif
