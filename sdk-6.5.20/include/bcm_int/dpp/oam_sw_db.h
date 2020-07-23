/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * OAM SW DB
 */

#ifndef _BCM_INT_DPP_OAM_SW_DB_H_
#define _BCM_INT_DPP_OAM_SW_DB_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/oam.h>
#include <bcm/bfd.h>
#include <shared/hash_tbl.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>


#define BCMDNX_INIT_FUNC_DEFS_RMEP_INFO_DB_WITH_LOCK \
    BCMDNX_INIT_FUNC_DEFS;\
    if (sal_mutex_take(_bcm_dpp_oam_bfd_rmep_info_db_lock[unit].lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d."), FUNCTION_NAME(), unit)); \
    }

#define BCMDNX_FUNC_RETURN_RMEP_INFO_DB_WITH_UNLOCK   \
do {\
    if (sal_mutex_give(_bcm_dpp_oam_bfd_rmep_info_db_lock[unit].lock)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d."), FUNCTION_NAME(), unit)); \
    } \
    BCMDNX_FUNC_RETURN;\
}while (0) ;

#define BCMDNX_INIT_FUNC_DEFS_WITH_MEP_INFO_LOCK \
    BCMDNX_INIT_FUNC_DEFS;\
    if (sal_mutex_take(_bcm_dpp_oam_bfd_mep_info_db_lock[unit].lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d."), FUNCTION_NAME(), unit)); \
    }


#define BCMDNX_FUNC_RETURN_WITH_MEP_INFO_UNLOCK \
    if (sal_mutex_give(_bcm_dpp_oam_bfd_mep_info_db_lock[unit].lock)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d."), FUNCTION_NAME(), unit)); \
    } \
    BCMDNX_FUNC_RETURN ;

void _bcm_dpp_oam_htb_cast_key_f(sw_state_htb_key_t key, uint8 **key_bytes, uint32 *key_size);
int _bcm_dpp_sw_db_hash_oam_ma_name_create(int unit);
int _bcm_dpp_sw_db_hash_oam_ma_name_destroy(int unit);
int _bcm_dpp_sw_db_hash_oam_ma_name_iterate(int unit, sw_state_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_oam_ma_name_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove);
int _bcm_dpp_sw_db_hash_oam_ma_name_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_info_create(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_info_iterate(int unit, sw_state_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_info_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_info_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);
int _bcm_dpp_sw_db_hash_oam_ma_to_mep_create(int unit);
int _bcm_dpp_sw_db_hash_oam_ma_to_mep_destroy(int unit);
int _bcm_dpp_sw_db_hash_oam_ma_to_mep_iterate(int unit, sw_state_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_oam_ma_to_mep_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);
int _bcm_dpp_sw_db_hash_oam_ma_to_mep_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_create(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_destroy(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_iterate(int unit, sw_state_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove);
int _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);
int _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_create(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy(int unit);
int _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_iterate(int unit, sw_state_htb_cb_t restore_cb);
int _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove);
int _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_create(int unit);
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_insert(int unit, uint32 endpoint_id, int ais_id);
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(int unit, uint32 endpoint_id, int * ais_id, uint8 * num_found);
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_delete_by_id(int unit, uint32 endpoint_id );
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_destroy(int unit);
int _bcm_dpp_oam_dbs_init(int unit, uint8 is_oam) ;
int _bcm_dpp_oam_dbs_destroy(int unit, uint8 is_oam) ;


int _bcm_dpp_oam_ma_db_delete_ma_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);
int _bcm_dpp_oam_ma_db_insert(int unit, uint32 ma_index, _bcm_oam_ma_name_t * ma_name);
int _bcm_dpp_oam_ma_db_get(int unit, uint32 ma_index, _bcm_oam_ma_name_t * ma_name, uint8 *found);
int _bcm_dpp_oam_ma_db_delete(int unit, uint32 ma_index);
int _bcm_dpp_oam_ma_to_mep_db_insert(int unit, uint32 ma_index, uint32 mep_index);
int _bcm_dpp_oam_ma_to_mep_db_get(int unit, uint32 ma_index, ENDPOINT_LIST_PTR *mep_list, uint8* found);
int _bcm_dpp_oam_ma_to_mep_db_mep_delete(int unit, uint32 ma_index, uint32 mep_index);

int _bcm_dpp_oam_bfd_mep_info_db_insert_no_lock(int unit, uint32 mep_index, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info);
int _bcm_dpp_oam_bfd_mep_info_db_insert(int unit, uint32 mep_index, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info);
int _bcm_dpp_oam_bfd_mep_info_db_get(int unit, uint32 mep_index, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info, uint8 *found);
int _bcm_dpp_oam_bfd_mep_info_db_delete_no_lock(int unit, uint32 mep_index);
int _bcm_dpp_oam_bfd_mep_info_db_delete(int unit, uint32 mep_index);
int _bcm_dpp_oam_bfd_mep_info_db_update(int unit, uint32 mep_index, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info);

int _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(int unit, uint32 mep_index, uint32 rmep_index);
int _bcm_dpp_oam_bfd_mep_to_rmep_db_get(int unit, uint32 mep_index, ENDPOINT_LIST_PTR *rmep_list, uint8* found);
int _bcm_dpp_oam_bfd_mep_to_rmep_db_mep_delete(int unit, uint32 mep_index);
int _bcm_dpp_oam_bfd_mep_to_rmep_db_rmep_delete(int unit, uint32 mep_index, uint32 rmep_index);

int _bcm_dpp_oam_sw_db_locks_init(int unit);
int _bcm_dpp_oam_bfd_rmep_info_db_insert_no_lock(int unit, uint32 rmep_index, SOC_PPC_OAM_RMEP_INFO_DATA * rmep_info);
int _bcm_dpp_oam_bfd_rmep_info_db_insert(int unit, uint32 rmep_index, SOC_PPC_OAM_RMEP_INFO_DATA * rmep_info);
int _bcm_dpp_oam_bfd_rmep_info_db_get(int unit, uint32 rmep_index,  SOC_PPC_OAM_RMEP_INFO_DATA * rmep_info, uint8 *found);
int _bcm_dpp_oam_bfd_rmep_info_db_delete_no_lock(int unit, uint32 rmep_index);
int _bcm_dpp_oam_bfd_rmep_info_db_delete(int unit, uint32 rmep_index);
int _bcm_dpp_oam_bfd_rmep_to_mep_db_update(int unit, uint32 rmep_index, SOC_PPC_OAM_RMEP_INFO_DATA * rmep_info );

int _bcm_dpp_oam_sat_gtf_info_db_insert(int unit, uint32 gtf_id, SOC_PPC_OAM_SAT_GTF_ENTRY * gtf_info); 
int _bcm_dpp_oam_sat_gtf_info_db_get(int unit, uint32 gtf_id, SOC_PPC_OAM_SAT_GTF_ENTRY * gtf_info, uint8 *found);
int _bcm_dpp_oam_sat_gtf_info_db_delete(int unit, uint32 gtf_id); 
int _bcm_dpp_oam_sat_gtf_info_db_update(int unit, uint32 gtf_id, SOC_PPC_OAM_SAT_GTF_ENTRY * gtf_info); 
int _bcm_dpp_oam_sat_ctf_info_db_insert(int unit, uint32 ctf_id, SOC_PPC_OAM_SAT_CTF_ENTRY * ctf_info);
int _bcm_dpp_oam_sat_ctf_info_db_get(int unit, uint32 ctf_id, SOC_PPC_OAM_SAT_CTF_ENTRY * ctf_info, uint8 *found);
int _bcm_dpp_oam_sat_ctf_info_db_delete(int unit, uint32 ctf_id); 
int _bcm_dpp_oam_sat_ctf_info_db_update(int unit, uint32 ctf_id, SOC_PPC_OAM_SAT_CTF_ENTRY * ctf_info);


int _bcm_dpp_oam_endpoint_list_init(int unit, ENDPOINT_LIST_PTR *endpoint_list_ptr);
int _bcm_dpp_oam_endpoint_list_set_member_index(int unit, ENDPOINT_LIST_MEMBER_PTR endpoint_list_member, uint32 index) ;
int _bcm_dpp_oam_endpoint_list_get_member_index(int unit, ENDPOINT_LIST_MEMBER_PTR endpoint_list_member, uint32 *index_ptr) ;
int _bcm_dpp_oam_endpoint_list_set_member_next(int unit, ENDPOINT_LIST_MEMBER_PTR endpoint_list_member, ENDPOINT_LIST_MEMBER_PTR next) ;
int _bcm_dpp_oam_endpoint_list_get_member_next(int unit, ENDPOINT_LIST_MEMBER_PTR endpoint_list_member, ENDPOINT_LIST_MEMBER_PTR *index_ptr) ;
int _bcm_dpp_oam_endpoint_list_set_first_member(int unit, ENDPOINT_LIST_PTR endpoint_list, ENDPOINT_LIST_MEMBER_PTR first_member) ;
int _bcm_dpp_oam_endpoint_list_get_first_member(int unit, ENDPOINT_LIST_PTR endpoint_list, ENDPOINT_LIST_MEMBER_PTR *first_member_ptr) ;
int _bcm_dpp_oam_endpoint_list_member_add(int unit, ENDPOINT_LIST_PTR endpoint_list, uint32 endpoint_index);
int _bcm_dpp_oam_endpoint_list_member_add_after(
        int unit, ENDPOINT_LIST_PTR endpoint_list, uint32 endpoint_index,
        ENDPOINT_LIST_MEMBER_PTR specified_member, ENDPOINT_LIST_MEMBER_PTR *new_member_ptr) ;
int _bcm_dpp_oam_endpoint_list_empty(int unit, ENDPOINT_LIST_PTR endpoint_list, int *is_empty);
int _bcm_dpp_oam_endpoint_list_member_find(int unit, ENDPOINT_LIST_PTR endpoint_list, uint32 index, uint8 delete_endpoint, uint8 *found);
int _bcm_dpp_oam_endpoint_list_destroy(int unit, ENDPOINT_LIST_PTR endpoint_list);
int _bcm_dpp_oam_endpoint_list_deinit(int unit, ENDPOINT_LIST_PTR endpoint_list) ;

int _bcm_dpp_am_template_oam_local_port_2_sys_port_entry_reserve(int unit, int local_port);
int _bcm_dpp_am_template_oam_local_port_2_sys_port_entry_unreserve(int unit, int local_port);

#endif /* _BCM_INT_DPP_OAM_SW_DB_H_ */

