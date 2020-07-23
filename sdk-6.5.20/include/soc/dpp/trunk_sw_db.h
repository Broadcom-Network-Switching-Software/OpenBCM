/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



#ifndef _BCM_INT_DPP_TRUNK_SW_DB_H_
#define _BCM_INT_DPP_TRUNK_SW_DB_H_

#include <soc/dpp/PPD/ppd_api_lag.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

#define TRUNK_ACCESS         sw_state_access[unit].dpp.bcm.trunk

int soc_dpp_trunk_sw_db_get_nof_replications(int unit, int tid, const SOC_PPC_LAG_MEMBER *lag_member, int* nof_replications, int* last_replicated_member_index);
int soc_dpp_trunk_sw_db_get_nof_enabled_members(int unit, int tid, int* nof_enabled_members);
int soc_dpp_trunk_sw_db_get_first_replication_index(int unit, int tid, const SOC_PPC_LAG_MEMBER *lag_member, int* first_replication_index);
int soc_dpp_trunk_sw_db_add(int unit, int tid, const SOC_PPC_LAG_MEMBER *lag_member);
int soc_dpp_trunk_sw_db_remove_all(int unit, int tid);
int soc_dpp_trunk_sw_db_set(int unit, int tid, const SOC_PPC_LAG_INFO *lag_info);
int soc_dpp_trunk_sw_db_remove(int unit, int tid, SOC_PPC_LAG_MEMBER *lag_member);
int soc_dpp_trunk_sw_db_set_trunk_attributes(int unit, int tid, int psc, int is_stateful);
int soc_dpp_trunk_sw_db_get(int unit, int tid, SOC_PPC_LAG_INFO *lag_info);
int soc_dpp_trunk_sw_db_max_nof_trunks_get(int unit, int* max_nof_trunks);

#endif 
