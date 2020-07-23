/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_METERING_INCLUDED__

#define __ARAD_PP_METERING_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_metering.h>





#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_PROFILE_METER_PROFILE_NOF_MEMBER 32
#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE_NOF_MEMBER 8

#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT 1300
#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BYTE 41

#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN 1308
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_REV_EXP 15
#define ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR 250

#define ARAD_BUCKET_EXP_MAX_VALUE (15)
#define ARAD_BUCKET_MANTISSA_MAX_VALUE (63)




#define ARAD_PP_MTR_MEMORY(unit, memory)				SOC_IS_ARADPLUS_AND_BELOW(unit)? IDR_##memory : (SOC_IS_QAX(unit)? IMP_##memory: MRPS_##memory)
#define ARAD_PP_ETH_MTR_MEMORY(unit, memory)			SOC_IS_ARADPLUS_AND_BELOW(unit)? IDR_##memory : (SOC_IS_QAX(unit)? IEP_##memory: MTRPS_EM_##memory)
#define ARAD_PP_MTR_REG_PORT(unit, core_id)             SOC_IS_ARADPLUS_AND_BELOW(unit)? REG_PORT_ANY : (SOC_IS_QAX(unit)? REG_PORT_ANY: core_id)
#define ARAD_PP_MTR_MEM_BLOCK(unit, core_id)       		SOC_IS_ARADPLUS_AND_BELOW(unit)? MEM_BLOCK_ANY :(SOC_IS_QAX(unit)? MEM_BLOCK_ANY: MRPS_BLOCK(unit, core_id))
#define ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id)       	SOC_IS_ARADPLUS_AND_BELOW(unit)? MEM_BLOCK_ANY :(SOC_IS_QAX(unit)? MEM_BLOCK_ANY: MTRPS_EM_BLOCK(unit, core_id))

#define JER_PP_MTR_MEMORY(unit, memory)					SOC_IS_QAX(unit)? IMP_##memory : MRPS_##memory


#define ARAD_PP_MTR_BUCKET_TO_DECIMAL(__twos, __decimal) do {  \
				__decimal =  ((1 << 22) & (__twos)) ? (-1 * (((__twos) ^ 0x7FFFFF) + 1)) : (__twos); \
			} while (0)

#define ARAD_PP_MTR_DECIMAL_TO_BUKCET(__decimal, __twos) do {  \
                __twos =  ((__decimal) & 0x3FFFFF); \
			} while (0) ; \
            if(__twos <= 64) { \
                __twos = 0x7FE000; \
            }


typedef struct
{
  uint32 color_aware;
  uint32 coupling_flag;
  uint32 sharing_flag;
  uint32 cir_mantissa;
  uint32 cir_mantissa_exponent;
  uint32 reset_cir;
  uint32 cbs_mantissa_64;
  uint32 cbs_exponent;
  uint32 eir_mantissa;
  uint32 eir_mantissa_exponent;
  uint32 max_eir_mantissa;
  uint32 max_eir_mantissa_exponent;
  uint32 reset_eir;
  uint32 ebs_mantissa_64;
  uint32 ebs_exponent;

#ifdef BCM_88660_A0
  uint32 packet_mode;
#endif
} __ATTRIBUTE_PACKED__ ARAD_IDR_PRFCFG_TBL_DATA;

typedef struct
{
  uint32 rate_mantissa;
  uint32 rate_exp;
  uint32 burst_mantissa;
  uint32 burst_exp;
  uint32 packet_mode;
  uint32 meter_resolution;
  uint32 color_blind;
  uint32 pkt_adj_header_truncate;
} __ATTRIBUTE_PACKED__ ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA;

typedef struct
{
  uint32 profile;
  uint32 global_meter_ptr;
  uint32 enable;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA;

typedef struct
{
  uint32 rate_mantissa;
  uint32 rate_exp;
  uint32 burst_mantissa;
  uint32 burst_exp;
  uint32 packet_mode;
  uint32 meter_resolution;
  uint32 enable;
  uint32 color_blind;
} __ATTRIBUTE_PACKED__ ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA;




#define ARAD_PP_MTR_CORES_ITER(index) \
    for(index = 0; index < SOC_DPP_CONFIG(unit)->meter.nof_meter_cores; index++)
#define ARAD_PP_MTR_IS_SINGLE_CORE(unit)              (SOC_DPP_CONFIG(unit)->meter.nof_meter_cores == 1)





typedef enum
{
  
  ARAD_PP_MTR_METERS_GROUP_INFO_SET = ARAD_PP_PROC_DESC_BASE_METERING_FIRST,
  ARAD_PP_MTR_METERS_GROUP_INFO_SET_PRINT,
  ARAD_PP_MTR_METERS_GROUP_INFO_SET_UNSAFE,
  ARAD_PP_MTR_METERS_GROUP_INFO_SET_VERIFY,
  ARAD_PP_MTR_METERS_GROUP_INFO_GET,
  ARAD_PP_MTR_METERS_GROUP_INFO_GET_PRINT,
  ARAD_PP_MTR_METERS_GROUP_INFO_GET_VERIFY,
  ARAD_PP_MTR_METERS_GROUP_INFO_GET_UNSAFE,
  ARAD_PP_MTR_BW_PROFILE_ADD,
  ARAD_PP_MTR_BW_PROFILE_ADD_PRINT,
  ARAD_PP_MTR_BW_PROFILE_ADD_UNSAFE,
  ARAD_PP_MTR_BW_PROFILE_ADD_VERIFY,
  ARAD_PP_MTR_BW_PROFILE_GET,
  ARAD_PP_MTR_BW_PROFILE_GET_PRINT,
  ARAD_PP_MTR_BW_PROFILE_GET_UNSAFE,
  ARAD_PP_MTR_BW_PROFILE_GET_VERIFY,
  ARAD_PP_MTR_BW_PROFILE_REMOVE,
  ARAD_PP_MTR_BW_PROFILE_REMOVE_PRINT,
  ARAD_PP_MTR_BW_PROFILE_REMOVE_UNSAFE,
  ARAD_PP_MTR_BW_PROFILE_REMOVE_VERIFY,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_PRINT,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_UNSAFE,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_VERIFY,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_PRINT,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_VERIFY,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_SET,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_GET,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_GET,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_UNSAFE,
  ARAD_PP_METERING_INIT_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_UNSAFE,
  ARAD_PP_METERING_GET_PROCS_PTR,
  ARAD_PP_METERING_GET_ERRS_PTR,
  
  ARAD_PP_MTR_IR_VAL_FROM_REVERSE_EXP_MNT,
  ARAD_PP_MTR_BS_VAL_TO_EXP_MNT,
  ARAD_PP_MTR_CHK_PROFILE_RATE,
  ARAD_PP_MTR_IR_VAL_TO_MAX_REV_EXP,
  ARAD_PP_MTR_PROFILE_RATE_TO_RES_EXP_MNT,
  ARAD_PP_METERING_PCD_INIT,

  
  ARAD_PP_METERING_PROCEDURE_DESC_LAST
} ARAD_PP_METERING_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_METERING_FIRST,
  ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_RESULT_USE_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_METERED_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_FIRST_METERED_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_NOF_METER_INS_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_GROUP_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_IS_HR_ENABLED_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_TYPE_OUT_OF_RANGE_ERR,
  
  
  ARAD_PP_METERING_CHK_PROFILE_RATE_ERR,
  ARAD_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR,
  ARAD_PP_METERING_UPDATE_ALL_COPIES_MAX_RANGE_ERR,
  ARAD_PP_METERING_BW_PROFILE_HR_RATE_ERR,
  ARAD_PP_METERING_BW_PROFILE_LR_RATE_ERR,
  ARAD_PP_MTR_ETH_POLICER_ADD_FAIL_ERR,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_POLICER_INGRESS_COUNT_INVALID_ERR,
  ARAD_PP_METERING_POLICER_INGRESS_SHARING_MODE_INVALID_ERR,
  ARAD_PP_METERING_MAX_IR_INVALID_ERR,
  ARAD_PP_METERING_POLICER_RESULT_PARALLEL_COLOR_MAPPING_INVALID_ERR,
  ARAD_PP_METERING_POLICER_RESULT_PARALLEL_BUCKET_UPDATE_INVALID_ERR,
  
  
  ARAD_PP_METERING_ERR_LAST
} ARAD_PP_METERING_ERR;









int
  arad_pp_mtr_translate_kbits_to_packets(
    SOC_SAND_IN int                       unit,
	SOC_SAND_IN int                       kbits
	);

int
  arad_pp_mtr_translate_packets_to_kbits(
    SOC_SAND_IN int                       unit,
	SOC_SAND_IN int                       packets
	);

uint32
  arad_pp_mtr_profile_rate_to_res_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      *rev_exp,
    SOC_SAND_OUT uint32                      *mnt,
    SOC_SAND_OUT uint32                      *exp
  );

uint32
  arad_pp_mtr_profile_burst_to_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      resolution, 
    SOC_SAND_OUT uint32                      *mnt,
    SOC_SAND_OUT uint32                      *exp
  );

uint32
  arad_pp_mtr_ir_val_from_reverse_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       rev_exp,
    SOC_SAND_IN uint32                       mant_exp,
    SOC_SAND_IN uint32                       mnt,
    SOC_SAND_OUT uint32                      *val
  );

uint32
  arad_pp_mtr_bs_val_to_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      *exp,
    SOC_SAND_OUT uint32                      *mnt
  );

void
  arad_pp_mtr_verify_valid_bucket_size(
    SOC_SAND_IN 	int			unit,
    SOC_SAND_IN 	uint32      rate1_man,
    SOC_SAND_IN 	uint32      rate1_exp,
    SOC_SAND_IN 	uint32      rate2_man,
    SOC_SAND_IN 	uint32      rate2_exp,
    SOC_SAND_INOUT	uint32      *bucket_man,
    SOC_SAND_INOUT 	uint32      *bucket_exp
  );

void
  arad_pp_mtr_eth_policer_profile_key_get(
    SOC_SAND_IN  int                                               unit,
    SOC_SAND_IN  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA         profile_tbl_data,
    SOC_SAND_OUT uint32                                            *eth_mtr_profile_multiset_key
  );

uint32
  arad_pp_metering_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32 arad_pp_metering_pcd_entry_set(
	SOC_SAND_IN int                                  unit, 
	SOC_SAND_IN SOC_PPC_MTR_COLOR_DECISION_INFO      *pcd_entry
  );

uint32 arad_pp_metering_pcd_entry_get(
	SOC_SAND_IN int                                  unit, 
	SOC_SAND_OUT SOC_PPC_MTR_COLOR_DECISION_INFO      *pcd_entry
  );


uint32 arad_pp_metering_dp_map_entry_set(
	SOC_SAND_IN int                                  unit, 
	SOC_SAND_IN SOC_PPC_MTR_COLOR_RESOLUTION_INFO     *dp_map_entry
  );

uint32 arad_pp_metering_dp_map_entry_get(
	SOC_SAND_IN int                                  unit, 
	SOC_SAND_OUT SOC_PPC_MTR_COLOR_RESOLUTION_INFO     *dp_map_entry
  );


uint32 arad_pp_metering_pcd_init(
	SOC_SAND_IN int                                  unit, 
	SOC_SAND_IN uint8                                sharing_mode
  );



uint32
  arad_pp_mtr_meters_group_info_set_unsafe(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO                  *mtr_group_info
  );

uint32
  arad_pp_mtr_meters_group_info_set_verify(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO                  *mtr_group_info
  );

uint32
  arad_pp_mtr_meters_group_info_get_verify(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx
  );


uint32
  arad_pp_mtr_meters_group_info_get_unsafe(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_GROUP_INFO                  *mtr_group_info
  );


uint32
  arad_pp_mtr_bw_profile_add_unsafe(
    SOC_SAND_IN  int                                   unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO             *bw_profile_info,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO             *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );

uint32
  arad_pp_mtr_bw_profile_add_verify(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO             *bw_profile_info
  );


uint32
  arad_pp_mtr_bw_profile_get_unsafe(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO             *bw_profile_info
  );

uint32
  arad_pp_mtr_bw_profile_get_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  );


uint32
  arad_pp_mtr_bw_profile_remove_unsafe(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  );

uint32
  arad_pp_mtr_bw_profile_remove_verify(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  );


uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  );

uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_set_verify(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  );

uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get_verify(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                    core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                   *meter_ins_ndx
  );


uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_OUT uint32                                  *bw_profile_id
  );


soc_error_t
  arad_pp_mtr_meter_ins_bucket_get(
    int                                  unit,
	int                                  core_id,
    SOC_PPC_MTR_METER_ID                 *meter_ins_ndx,
    int                                  *cbl,
	int                                  *ebl
  );


uint32
  arad_pp_mtr_eth_policer_enable_set_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  arad_pp_mtr_eth_policer_enable_set_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  arad_pp_mtr_eth_policer_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_mtr_eth_policer_enable_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT uint8                                 *enable
  );


uint32
  arad_pp_mtr_eth_policer_params_set_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  );

uint32
  arad_pp_mtr_eth_policer_params_set_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  );

uint32
  arad_pp_mtr_eth_policer_params_get_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx
  );


uint32
  arad_pp_mtr_eth_policer_params_get_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  );


uint32  
  arad_pp_mtr_eth_policer_glbl_profile_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  );

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       glbl_profile_idx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  );

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  );


uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  );


uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                      glbl_profile_idx
  );

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                      glbl_profile_idx
  );

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE        eth_type_ndx
  );


uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_OUT uint32                      *glbl_profile_idx
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_metering_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_metering_get_errs_ptr(void);

uint32
  SOC_PPC_MTR_METER_ID_verify(
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID *info
  );
uint32
  SOC_PPC_MTR_GROUP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO *info
  );

uint32
  SOC_PPC_MTR_BW_PROFILE_INFO_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *info
  );


soc_error_t
  arad_pp_mtr_policer_global_sharing_get(
    int                         unit,
	int                         core_id,
	int        					meter_id,
	int							meter_group,
	uint32* 					global_sharing_ptr
);


soc_error_t
  arad_pp_mtr_policer_global_sharing_set(
    int                         unit,
	int                         core_id,
	int        					meter_id,
	int							meter_group,
	uint32* 					global_sharing_ptr
);


uint32
arad_pp_mtr_policer_ipg_compensation_set(
    int                         unit,
	uint8						ipg_compensation_enabled
);

uint32
  arad_pp_mtr_policer_ipg_compensation_get(
    int                         unit,
	uint8						*ipg_compensation_enabled
);


uint32
  arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                       *rates,
    SOC_SAND_IN uint32                       nof_rates,
    SOC_SAND_OUT uint32                      *rev_exp
  );



uint32
  arad_pp_meter_tc_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                   tc,
    SOC_SAND_IN  uint32                   meter_tc
  );
uint32
  arad_pp_meter_tc_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                               tc,
    SOC_SAND_OUT uint32                              *meter_tc
  );

soc_error_t
arad_pp_mtr_none_ethernet_packet_ptr_set(
    int                         unit,
	int*       					arg);

soc_error_t
arad_pp_mtr_none_ethernet_packet_ptr_get(
    int                         unit,
	int*       					arg);




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

