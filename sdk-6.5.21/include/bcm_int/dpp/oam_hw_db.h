/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * OAM HW DB (MEP-DB, RMEP-DB, ...)
 */

#ifndef _BCM_INT_DPP_OAM_HW_DB_H_
#define _BCM_INT_DPP_OAM_HW_DB_H_

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

/* Used to encode a portion of a packet into crc. */
#define _BCM_OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION  (16)
#define _BCM_OAM_OAMP_CRC_BUFFER_SIZE                   (128)

typedef uint8 _BCM_OAM_OAMP_CRC_BUFFER[_BCM_OAM_OAMP_CRC_BUFFER_SIZE];

int _bcm_oam_stat_lem_entry_add(uint32 unit, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry, bcm_oam_endpoint_info_t *endpoint_info);


int _bcm_dpp_oam_bfd_rmep_db_entry_dealloc(int unit, uint32 rmep_index_internal);
int _bcm_oam_rmep_db_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY * rmep_db_entry);
int _bcm_oam_rmep_db_entry_struct_get(int unit, SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY * rmep_db_entry, uint16 rmep_id, uint32 local_id, bcm_oam_endpoint_info_t *endpoint_info);
int _bcm_oam_mep_db_entry_dealloc(int unit, uint32 mep_index, SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info);
int _bcm_oam_mep_db_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry, int icc_ndx, _bcm_oam_ma_name_t *group_name_struct, int do_not_set_mep_db_ccm_interval);
int _bcm_oam_mep_db_entry_struct_get(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry, int original_tx_gport);
int _bcm_oam_classifier_default_mep_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry);
int _bcm_oam_classifier_mep_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry);
int _bcm_oam_classifier_mep_entry_struct_get(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry);

int _bcm_oam_classifier_rfc6374_mep_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry);

void 
_bcm_dpp_oam_bfd_flexible_verification_expected_crc16_get(_BCM_OAM_OAMP_CRC_BUFFER buffer, SOC_PPC_OAM_OAMP_CRC_MASK *mask, uint16 *crc_val);

#define _BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(mep_db_mep_type) \
            ((mep_db_mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) || (mep_db_mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) \
             || (mep_db_mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION))

#endif  /* _BCM_INT_DPP_OAM_HW_DB_H_ */
