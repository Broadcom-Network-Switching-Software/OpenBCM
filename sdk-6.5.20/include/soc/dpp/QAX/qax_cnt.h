/* $Id: qax_cnt.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __QAX_CNT_H__
#define __QAX_CNT_H__


#include <soc/dpp/ARAD/arad_api_cnt.h>
#include <soc/dpp/TMC/tmc_api_cnt.h>
#include <soc/dpp/ARAD/arad_cnt.h>





#define QAX_CNT_MODE_EG_TYPE_TM_FLD_VAL                           (0x2)
#define QAX_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL                       (0x8)
#define QAX_CNT_MODE_EG_TYPE_VSI_FLD_VAL                          (0x4)
#define QAX_CNT_MODE_EG_TYPE_LATENCY_FLD_VAL                      (0x1)
#define QAX_VSI_OFFSET_RESOLUTION                                 (0x800)



#define _BMAP_FIELDS_BLOCK_CMD_ID(BLOCK, CMD_ID)                              \
        BLOCK ## CMD_ID ## _GREEN_ADMITf,                   \
        BLOCK ## CMD_ID ## _GREEN_DISCARDf,                 \
        BLOCK ## CMD_ID ## _YELLOW_1_ADMITf,                \
        BLOCK ## CMD_ID ## _YELLOW_1_DISCARDf,              \
        BLOCK ## CMD_ID ## _YELLOW_2_ADMITf,                \
        BLOCK ## CMD_ID ## _YELLOW_2_DISCARDf,              \
        BLOCK ## CMD_ID ## _RED_ADMITf,                     \
        BLOCK ## CMD_ID ## _RED_DISCARDf                    \















int qax_cnt_get_field_name_per_src_type(int unit, SOC_TMC_CNT_SRC_TYPE src_type, int command_id, soc_field_t * src_mask_field_name, soc_field_t * src_cfg_field_name, soc_field_t * group_size_field_name);
int qax_cnt_ingress_compensation_profile_delta_set(SOC_SAND_IN int unit, SOC_SAND_IN int core, SOC_SAND_IN int src_type, SOC_SAND_IN int profileIndex, SOC_SAND_IN int delta);
int qax_cnt_ingress_compensation_port_profile_set(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int port, SOC_SAND_IN  int profileIndex);
int qax_cnt_ingress_compensation_outLif_delta_set(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int lifIndex, SOC_SAND_IN  int delta);
int qax_cnt_ingress_compensation_outLif_delta_get(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int lifIndex, SOC_SAND_OUT  int * delta);
int qax_cnt_ingress_compensation_port_delta_and_profile_get(SOC_SAND_IN  int unit, SOC_SAND_IN  int core, SOC_SAND_IN  int commandId, SOC_SAND_IN  int src_type, SOC_SAND_IN  int port, SOC_SAND_OUT  int * profileIndex,  SOC_SAND_OUT  int * delta);
int qax_cnt_filter_config_ingress_set_get(int unit, int relevant_bit, int command_id, int get, int* value);
uint32
  _qax_cnt_counter_bmap_mem_by_src_type_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                proc_id,
    SOC_SAND_IN  int                   src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
    SOC_SAND_IN  int                   command_id,
    SOC_SAND_OUT soc_reg_t             *reg, 
    SOC_SAND_OUT soc_field_t           *fields);
int qax_cnt_epni_counter_base_set(int unit, int src_core, int base_offset_field_val, int command_id, SOC_TMC_CNT_MODE_EG_TYPE type, SOC_TMC_CNT_SRC_TYPE source);
uint32 qax_cnt_crps_cgm_cmd_get(int unit, int proc_id, int command_id, SOC_SAND_OUT ARAD_CNT_CRPS_IQM_CMD *crps_iqm_cmd);
uint32 qax_cnt_do_not_count_field_by_src_type_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 proc_id,
    SOC_SAND_IN  int                    src_core,
    SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
    SOC_SAND_IN  int                    command_id,
    SOC_SAND_OUT soc_field_t            *field
    );
int qax_filter_config_egress_receive_set_get(int unit, int relevant_bit, int command_id, int get, int* value);
int qax_stat_counter_filter_group_get(int unit, int command_id, int filter_group, int *is_active);
int qax_stat_counter_filter_group_set(int unit, int command_id, int filter_group, int is_active);
int qax_stat_group_drop_reasons_get(int unit, int filter_group, uint32 *drop_reasons_mask);
int qax_stat_group_drop_reasons_set(int unit, int filter_group, uint32 *drop_reasons_mask);

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
