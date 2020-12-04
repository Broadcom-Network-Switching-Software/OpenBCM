#include <shared/bsl.h>
#include <soc/mcm/memregs.h> 
#if defined(BCM_88675_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_METERING
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>





#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/JER/jer_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/JER/JER_PP/jer_pp_metering.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/drv.h>





typedef enum {
  JER_PP_MTR_PCD_COL_GREEN     = 0,
  JER_PP_MTR_PCD_COL_INVALID   = 1,
  JER_PP_MTR_PCD_COL_YELLOW    = 2,
  JER_PP_MTR_PCD_COL_RED       = 3,
  JER_PP_MTR_PCD_NOF_COLS      = 4
} jer_pp_mtr_pcd_col_t;

#define JER_PP_MTR_PCD_MCDA_COL_LSB                1
#define JER_PP_MTR_PCD_MCDB_COL_LSB                (SOC_IS_QUX(unit) ? 5 : 4)
#define JER_PP_MTR_PCD_COL_NOF_BITS                2

#define	JER_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS			1
#define	JER_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS	64
#define	JER_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR	125
#define	JER_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR	0

#define JER_PP_ETH_POLICER_PCD_OPT_NUM    (SOC_IS_QUX(unit) ? 256 : 64)


#define JER_PP_MTR_PACKET_SIZE_TO_2S_COMPLIMENT(__integer)  (((__integer) < 0) ? ((((-1 * (__integer)) & 0xFF) ^ 0xFF) + 1) : ((__integer) & 0xFF))

#define JER_PP_MTR_2S_COMPLIMENT_TO_PACKET_SIZE(__twos)     (((__twos) & 0x80) ? (-1 * (((__twos) ^ 0xFF) + 1)) : (__twos))

#define JER_PP_MTR_2S_COMPLIMENT_MAX_VALUE      128
#define JER_PP_MTR_2S_COMPLIMENT_MIN_VALUE      -127

#define JER_PP_MTR_IN_PP_PORT_MAP_NOF_PROFILES  8
#define JER_PP_MTR_IN_PP_PORT_NOF				255


#define JER_PP_MTR_IN_PP_PORT_AND_MTR_PRF_TO_TABLE_ROW(__pp_port_prf, __map_to_3_prf) 	(((__pp_port_prf & 0x07) << 1) + ((__map_to_3_prf & 0x04) >> 2))
#define JER_PP_MTR_MAP_TO_3_PRF_TO_TABLE_CELL(__map_to_3_prf) 	                        (__map_to_3_prf & 0x03)





STATIC uint32 jer_pp_metering_init_header_compensation(int unit);
STATIC uint32 jer_pp_metering_packet_size_profile_key_get(int unit,JER_PACKET_SIZE_PROFILES_TBL_DATA  profile_tbl_data, uint32 *packet_size_profile_multiset_key);
STATIC uint32 jer_pp_metering_in_pp_port_map_and_mtr_prf_map_get(int unit, int core_id, int meter_group, uint32 map_to_3_prf, uint32 pp_port_prf, int *packet_size);
STATIC uint32 jer_pp_metering_in_pp_port_map_and_mtr_prf_map_set(int unit, int core_id, int meter_group, uint32 map_to_3_prf, uint32 pp_port_prf, int packet_size);
STATIC uint32 jer_pp_metering_in_pp_port_map_get(int unit, int core_id, int meter_group, uint32 pp_port, uint32 *pp_port_prf);
STATIC uint32 jer_pp_metering_in_pp_port_map_set(int unit, int core_id, int meter_group, uint32 pp_port, uint32 pp_port_prf);
STATIC uint32 jer_pp_mtr_policer_hdr_compensation_verify(int unit, int compensation_size);





uint32
  jer_pp_metering_init_mrps_config(
		SOC_SAND_IN int unit
  )
{
uint32
	range_mode,
	reg_val,
	reg_val_a,
	reg_val_b;

uint32 
	glbl_cfg_reg,
	mcda_cfg_reg,
	mcdb_cfg_reg,
	mcda_refresh_reg,
	mcdb_refresh_reg,
	mcda_wrap_index_reg,
	mcdb_wrap_index_reg;

uint8
    sharing_mode;
int
	core_index;
 
	SOCDNX_INIT_FUNC_DEFS;

	range_mode = SOC_DPP_CONFIG(unit)->meter.meter_range_mode;
	sharing_mode = SOC_DPP_CONFIG(unit)->meter.sharing_mode;

	glbl_cfg_reg 		= JER_MRPS_REG_FORMAT_BY_CHIP(unit,GLBL_CFG);
	mcda_cfg_reg 		= JER_MRPS_REG_FORMAT_BY_CHIP(unit,MCDA_CFG);
	mcdb_cfg_reg 		= JER_MRPS_REG_FORMAT_BY_CHIP(unit,MCDB_CFG);
	mcda_refresh_reg 	= JER_MRPS_REG_FORMAT_BY_CHIP(unit,MCDA_REFRESH_CFG);
	mcdb_refresh_reg 	= JER_MRPS_REG_FORMAT_BY_CHIP(unit,MCDB_REFRESH_CFG);
	mcda_wrap_index_reg = JER_MRPS_REG_FORMAT_BY_CHIP(unit,MCDA_WRAP_INDEX);
	mcdb_wrap_index_reg = JER_MRPS_REG_FORMAT_BY_CHIP(unit,MCDB_WRAP_INDEX);

	
	SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, glbl_cfg_reg, 0, 0, &reg_val));
	
	
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCDA_INITf, 0);
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCDB_INITf, 0);

	
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, HIERARCHICAL_MODEf, 1);

	
    soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, PACKET_MODE_ENf, 1);

	
	if (SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit) && !ARAD_PP_MTR_IS_SINGLE_CORE(unit) ){
		
		if (64 == range_mode && SOC_PPC_MTR_SHARING_MODE_PARALLEL != sharing_mode) {
			SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Policer Sharing Mode have to be PARALLEL with Ingress Count 64.")));
		}
		if (128 == range_mode && SOC_PPC_MTR_SHARING_MODE_NONE != sharing_mode) {
			SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Policer Sharing Mode have to be NONE with Ingress Count 128.")));
		}
	}
	else { 
		if (32 == range_mode && SOC_PPC_MTR_SHARING_MODE_NONE == sharing_mode) {
			SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Policer Sharing Mode can't be NONE with Ingress Count 32.")));
		}
		if (64 == range_mode && SOC_PPC_MTR_SHARING_MODE_NONE != sharing_mode) {
			SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Policer Sharing Mode have to be NONE with Ingress Count 64.")));
		}
	}

	
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, PCD_MAP_ENf, (SOC_PPC_MTR_SHARING_MODE_NONE == sharing_mode) ? 0 : 1);

	
	if (SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit) && !ARAD_PP_MTR_IS_SINGLE_CORE(unit)) {
		
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCQ_SIZE_SELf, (128 == range_mode)? 1 : 0);
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MRPS_SECOND_PTR_ENf, (64 == range_mode)? 1 : 0);
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCD_SECOND_PTR_ENf, 1);
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCQ_MCDS_PARALLELf, 1);

	} else {
		
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCQ_SIZE_SELf, (64 == range_mode)? 1 : 0);
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MRPS_SECOND_PTR_ENf, 1);
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCD_SECOND_PTR_ENf, (32 == range_mode)? 1 : 0);
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCQ_MCDS_PARALLELf, 
						  (64 == range_mode || SOC_PPC_MTR_SHARING_MODE_PARALLEL == sharing_mode) ? 1 : 0);
	}

	
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MEF_10_DOT_3_ENf, 0);

	if (SOC_IS_QAX(unit)) {
		
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, DUAL_FAP_MODEf, 1);
	}

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, glbl_cfg_reg, core_index, 0, reg_val));
	}

	reg_val_a = 0;
	reg_val_b = 0;

	
	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_TIMER_ENf, 1);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_TIMER_ENf, 1);

	
	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_REFRESH_ENf, 1);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_REFRESH_ENf, 1);

	
	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_CBL_RND_MODE_ENf, 1);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_CBL_RND_MODE_ENf, 1);

	
	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_ERR_COMP_ENABLEf, 1);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_ERR_COMP_ENABLEf, 1);

	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_RND_RANGEf, 3);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_RND_RANGEf, 3);

	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_BUBBLE_RATEf, 0xff);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_BUBBLE_RATEf, 0xff);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcda_cfg_reg, core_index, 0, reg_val_a));
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcdb_cfg_reg, core_index, 0, reg_val_b));
	}

	reg_val_a = 0;
	reg_val_b = 0;

	
	soc_reg_field_set(unit, mcda_refresh_reg, &reg_val_a, MCDA_REFRESH_START_INDEXf, 0);
	soc_reg_field_set(unit, mcdb_refresh_reg, &reg_val_b, MCDB_REFRESH_START_INDEXf, 0);

	soc_reg_field_set(unit, mcda_refresh_reg, &reg_val_a, MCDA_REFRESH_END_INDEXf, 0x1fff);
	soc_reg_field_set(unit, mcdb_refresh_reg, &reg_val_b, MCDB_REFRESH_END_INDEXf, 0x1fff);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcda_refresh_reg, core_index, 0, reg_val_a));
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcdb_refresh_reg, core_index, 0, reg_val_b));
	}

	reg_val_a = 0;
	reg_val_b = 0;

	
	soc_reg_field_set(unit, mcda_wrap_index_reg, &reg_val_a, MCDA_WRAP_INT_ENf, 1);
	soc_reg_field_set(unit, mcdb_wrap_index_reg, &reg_val_b, MCDB_WRAP_INT_ENf, 1);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcda_wrap_index_reg, core_index, 0, reg_val_a));
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcdb_wrap_index_reg, core_index, 0, reg_val_b));
	}
	
	
	if (sharing_mode != SOC_PPC_MTR_SHARING_MODE_NONE) {
		SOCDNX_SAND_IF_ERR_EXIT(arad_pp_metering_pcd_init(unit, sharing_mode));
	}

    
    
    
	if(SOC_IS_QAX(unit))
    {
        reg_val_a = 0;
        SOCDNX_IF_ERR_EXIT(READ_IMP_INDIRECT_COMMANDr(unit, &reg_val_a));
        soc_reg_field_set(unit, IMP_INDIRECT_COMMANDr, &reg_val_a, INDIRECT_COMMAND_TIMEOUTf, 0x10);
        SOCDNX_IF_ERR_EXIT(WRITE_IMP_INDIRECT_COMMANDr(unit, reg_val_a));
        reg_val_a = 0;
        SOCDNX_IF_ERR_EXIT(READ_IMP_INDIRECT_FORCE_BUBBLEr(unit, &reg_val_a));        
        soc_reg_field_set(unit, IMP_INDIRECT_FORCE_BUBBLEr, &reg_val_a, FORCE_BUBBLE_ENf, 1);
        soc_reg_field_set(unit, IMP_INDIRECT_FORCE_BUBBLEr, &reg_val_a, FORCE_BUBBLE_PERIODf, 0xa);
        soc_reg_field_set(unit, IMP_INDIRECT_FORCE_BUBBLEr, &reg_val_a, FORCE_BUBBLE_PULSE_WIDTHf, 1);    
        SOCDNX_IF_ERR_EXIT(WRITE_IMP_INDIRECT_FORCE_BUBBLEr(unit, reg_val_a));                
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  jer_pp_metering_init_mrpsEm_config(
		SOC_SAND_IN int unit
  )
{
uint32
	reg_val,
	reg_val_a,
	reg_val_b;

uint32 
	glbl_cfg_reg,
	mcda_cfg_reg,
	mcdb_cfg_reg,
	mcda_refresh_reg,
	mcdb_refresh_reg,
	mcda_wrap_index_reg,
	mcdb_wrap_index_reg;

int
	core_index;
 
	SOCDNX_INIT_FUNC_DEFS;

	glbl_cfg_reg 		= JER_MRPS_EM_REG_FORMAT_BY_CHIP(unit,GLBL_CFG);
	mcda_cfg_reg 		= JER_MRPS_EM_REG_FORMAT_BY_CHIP(unit,MCDA_CFG);
	mcdb_cfg_reg 		= JER_MRPS_EM_REG_FORMAT_BY_CHIP(unit,MCDB_CFG);
	mcda_refresh_reg 	= JER_MRPS_EM_REG_FORMAT_BY_CHIP(unit,MCDA_REFRESH_CFG);
	mcdb_refresh_reg 	= JER_MRPS_EM_REG_FORMAT_BY_CHIP(unit,MCDB_REFRESH_CFG);
	mcda_wrap_index_reg = JER_MRPS_EM_REG_FORMAT_BY_CHIP(unit,MCDA_WRAP_INDEX);
	mcdb_wrap_index_reg = JER_MRPS_EM_REG_FORMAT_BY_CHIP(unit,MCDB_WRAP_INDEX);

	
	SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, glbl_cfg_reg, 0, 0, &reg_val));

	
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCDA_INITf, 0);
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCDB_INITf, 0);

	
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, HIERARCHICAL_MODEf, 1);

	
    soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, PACKET_MODE_ENf, 1);

	
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, PCD_MAP_ENf, 1);

	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCQ_SIZE_SELf, 0);
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MRPS_SECOND_PTR_ENf, 1);
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCD_SECOND_PTR_ENf, 1);
	soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, MCQ_MCDS_PARALLELf, 1);	

	if (SOC_IS_QAX(unit)) {
		
		soc_reg_field_set(unit, glbl_cfg_reg, &reg_val, DUAL_FAP_MODEf, 1);
	}

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, glbl_cfg_reg, core_index, 0, reg_val));
	}

	reg_val_a = 0;
	reg_val_b = 0;

	
	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_TIMER_ENf, 1);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_TIMER_ENf, 1);

	
	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_REFRESH_ENf, 1);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_REFRESH_ENf, 1);

	
	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_CBL_RND_MODE_ENf, 1);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_CBL_RND_MODE_ENf, 1);

	
	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_EBL_RND_MODE_ENf, 1);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_EBL_RND_MODE_ENf, 1);

	
	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_ERR_COMP_ENABLEf, 1);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_ERR_COMP_ENABLEf, 1);

	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_RND_RANGEf, 3);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_RND_RANGEf, 3);

	soc_reg_field_set(unit, mcda_cfg_reg, &reg_val_a, MCDA_BUBBLE_RATEf, 0xff);
	soc_reg_field_set(unit, mcdb_cfg_reg, &reg_val_b, MCDB_BUBBLE_RATEf, 0xff);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcda_cfg_reg, core_index, 0, reg_val_a));
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcdb_cfg_reg, core_index, 0, reg_val_b));
	}

	reg_val_a = 0;
	reg_val_b = 0;

	
	soc_reg_field_set(unit, mcda_refresh_reg, &reg_val_a, MCDA_REFRESH_START_INDEXf, 0);
	soc_reg_field_set(unit, mcdb_refresh_reg, &reg_val_b, MCDB_REFRESH_START_INDEXf, 0);

	soc_reg_field_set(unit, mcda_refresh_reg, &reg_val_a, MCDA_REFRESH_END_INDEXf, 0x13f);
	soc_reg_field_set(unit, mcdb_refresh_reg, &reg_val_b, MCDB_REFRESH_END_INDEXf, 0x1);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcda_refresh_reg, core_index, 0, reg_val_a));
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcdb_refresh_reg, core_index, 0, reg_val_b));
	}

	reg_val_a = 0;
	reg_val_b = 0;

	
	soc_reg_field_set(unit, mcda_wrap_index_reg, &reg_val_a, MCDA_WRAP_INT_ENf, 1);
	soc_reg_field_set(unit, mcdb_wrap_index_reg, &reg_val_b, MCDB_WRAP_INT_ENf, 1);

	ARAD_PP_MTR_CORES_ITER(core_index){
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcda_wrap_index_reg, core_index, 0, reg_val_a));
		SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcdb_wrap_index_reg, core_index, 0, reg_val_b));
	}

    
    
    
	if(SOC_IS_QAX(unit))
    {
        reg_val_a = 0;
        SOCDNX_IF_ERR_EXIT(READ_IEP_INDIRECT_COMMANDr(unit, &reg_val_a));
        soc_reg_field_set(unit, IEP_INDIRECT_COMMANDr, &reg_val_a, INDIRECT_COMMAND_TIMEOUTf, 0x10);
        SOCDNX_IF_ERR_EXIT(WRITE_IEP_INDIRECT_COMMANDr(unit, reg_val_a));
        reg_val_a = 0;
        SOCDNX_IF_ERR_EXIT(READ_IEP_INDIRECT_FORCE_BUBBLEr(unit, &reg_val_a));        
        soc_reg_field_set(unit, IEP_INDIRECT_FORCE_BUBBLEr, &reg_val_a, FORCE_BUBBLE_ENf, 1);
        soc_reg_field_set(unit, IEP_INDIRECT_FORCE_BUBBLEr, &reg_val_a, FORCE_BUBBLE_PERIODf, 0xa);
        soc_reg_field_set(unit, IEP_INDIRECT_FORCE_BUBBLEr, &reg_val_a, FORCE_BUBBLE_PULSE_WIDTHf, 1);    
        SOCDNX_IF_ERR_EXIT(WRITE_IEP_INDIRECT_FORCE_BUBBLEr(unit, reg_val_a));                
    }	
exit:
  SOCDNX_FUNC_RETURN;
}

uint32 
  jer_pp_eth_policer_pcd_init(
     SOC_SAND_IN int unit)
{
	uint32
		
		mcda_mtr_color,
		mcdb_mtr_color,	
		
		final_color, 
		
		mcda_update_color,
		mcdb_update_color,
		addr_idx,
		pcd_line;
	uint32
		res;
	soc_mem_t
		mem = JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,PCD_MAP);
	int
		core_index;

	SOCDNX_INIT_FUNC_DEFS;

	
	for (addr_idx = 0; addr_idx < JER_PP_ETH_POLICER_PCD_OPT_NUM; addr_idx++) {

		mcda_mtr_color = mcdb_mtr_color = 0;
		mcda_update_color = mcdb_update_color = final_color = 0;

		
        SHR_BITCOPY_RANGE(&(mcda_mtr_color), 0, &addr_idx, 
                          JER_PP_MTR_PCD_MCDA_COL_LSB, JER_PP_MTR_PCD_COL_NOF_BITS);
		SHR_BITCOPY_RANGE(&(mcdb_mtr_color), 0, &addr_idx, 
                          JER_PP_MTR_PCD_MCDB_COL_LSB, JER_PP_MTR_PCD_COL_NOF_BITS);

		

		if ((mcda_mtr_color == JER_PP_MTR_PCD_COL_INVALID) && (mcdb_mtr_color == JER_PP_MTR_PCD_COL_INVALID)) {
			final_color = mcda_update_color = mcdb_update_color = JER_PP_MTR_PCD_COL_RED;
		}
		else if (mcda_mtr_color == JER_PP_MTR_PCD_COL_INVALID) {
			if (mcdb_mtr_color == JER_PP_MTR_PCD_COL_GREEN) {
				final_color = mcdb_update_color = JER_PP_MTR_PCD_COL_GREEN;
			}else{
				final_color = mcdb_update_color = JER_PP_MTR_PCD_COL_RED;
			}
			mcda_update_color = JER_PP_MTR_PCD_COL_RED;
		}
		else if (mcdb_mtr_color == JER_PP_MTR_PCD_COL_INVALID) {
			if (mcda_mtr_color == JER_PP_MTR_PCD_COL_GREEN) {
				final_color = mcda_update_color = JER_PP_MTR_PCD_COL_GREEN;
			}else{
				final_color = mcda_update_color = JER_PP_MTR_PCD_COL_RED;
			}
			mcdb_update_color = JER_PP_MTR_PCD_COL_RED;
		}
		else if ((mcda_mtr_color == JER_PP_MTR_PCD_COL_GREEN) && (mcdb_mtr_color == JER_PP_MTR_PCD_COL_GREEN)) {
			final_color = mcda_update_color = mcdb_update_color = JER_PP_MTR_PCD_COL_GREEN;
		}
		else {
			final_color = mcda_update_color = mcdb_update_color = JER_PP_MTR_PCD_COL_RED;
		}

		
		pcd_line = 0;
		soc_mem_field32_set(unit, mem, &pcd_line, MCDA_UPDATE_COLORf, mcda_update_color);
		soc_mem_field32_set(unit, mem, &pcd_line, MCDB_UPDATE_COLORf, mcdb_update_color);
		soc_mem_field32_set(unit, mem, &pcd_line, OUT_COLORf, final_color);

		
		ARAD_PP_MTR_CORES_ITER(core_index){
			res = soc_mem_write(unit, mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_index), addr_idx, &pcd_line); 
			SOCDNX_IF_ERR_EXIT(res);
		}
	}

exit:
	SOCDNX_FUNC_RETURN;
}

STATIC uint32
  jer_pp_metering_init_eth_policer_default_config(
							SOC_SAND_IN int unit,
							SOC_SAND_IN int core_id
  )
{
	uint32
	  res;
	unsigned int
	  soc_sand_dev_id;

	soc_reg_above_64_val_t
	  eth_mtr_prfCfg_above_64_val;

	soc_mem_t
	  eth_mtr_prfCfg_fields_mem,
	  eth_mtr_prfCfg_0_mem,
	  eth_mtr_prfCfg_1_mem;

	  SOCDNX_INIT_FUNC_DEFS;
	  
	  soc_sand_dev_id = (unit);
	  SOC_REG_ABOVE_64_CLEAR(eth_mtr_prfCfg_above_64_val);

	  eth_mtr_prfCfg_fields_mem = JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_SHARING_DIS); 
	  eth_mtr_prfCfg_0_mem 		= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_0);
	  eth_mtr_prfCfg_1_mem 		= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_1);

	  soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, RESET_CIRf     , 0);
	  soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANTf      , 63);
	  soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANT_EXPf  , 7);
	  soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_MANT_64f   , 63);
	  soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_EXPONENTf  , 15);
	  soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, PACKET_MODEf	  , 0);
	  soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_REV_EXP_2f , 0);
	  soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, COLOR_AWAREf   , 0);
	  soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_4f , 9);

	  res = soc_mem_write(soc_sand_dev_id, eth_mtr_prfCfg_0_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), 0, (void*)eth_mtr_prfCfg_above_64_val); 
	  SOCDNX_IF_ERR_EXIT(res);
	  res = soc_mem_write(soc_sand_dev_id, eth_mtr_prfCfg_1_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), 0, (void*)eth_mtr_prfCfg_above_64_val); 
	  SOCDNX_IF_ERR_EXIT(res);

  exit:
	  SOCDNX_FUNC_RETURN;


}
uint32
  jer_pp_metering_init(
		SOC_SAND_IN int unit
  )
{
  uint32
	res,
	eth_mtr_profile_multiset_key = 0x0,
    eth_mtr_profile_multiset_ndx = 0x0;
  uint8
    eth_mtr_profile_multiset_first_appear = 0x0;
  int core_index;

  SOC_SAND_SUCCESS_FAILURE
    eth_mtr_profile_multiset_success = SOC_SAND_SUCCESS;
  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data = {0};

	SOCDNX_INIT_FUNC_DEFS;

	
	SOCDNX_IF_ERR_EXIT(jer_itm_setup_dp_map(unit));

	if (!SOC_IS_QAX(unit)) { 
		
		
		SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, IDR_BLOCK(unit), 0, FTMH_DP_OVERWRITEf, 1));
	}
	SOCDNX_IF_ERR_EXIT(jer_pp_metering_init_mrps_config(unit));

	
	SOCDNX_IF_ERR_EXIT(jer_pp_metering_init_mrpsEm_config(unit));
	SOCDNX_IF_ERR_EXIT(jer_pp_eth_policer_pcd_init(unit));

	
	arad_pp_mtr_eth_policer_profile_key_get(
		unit,
		profile_tbl_data,
		&eth_mtr_profile_multiset_key
		);

	ARAD_PP_MTR_CORES_ITER(core_index){
		res = arad_sw_db_multiset_add(
			unit,
			core_index,
			ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
			&eth_mtr_profile_multiset_key,
			&eth_mtr_profile_multiset_ndx,
			&eth_mtr_profile_multiset_first_appear,
			&eth_mtr_profile_multiset_success
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);

		SOCDNX_IF_ERR_EXIT(jer_pp_metering_init_eth_policer_default_config(unit,core_index));
	}

    SOCDNX_IF_ERR_EXIT(jer_pp_metering_init_header_compensation(unit)); 


exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_pp_mtr_policer_global_sharing_get(
    int                         unit,
	int                         core_id,
	int        					meter_id,
	int							meter_id_group,
	uint32* 					global_sharing_ptr
)
{
	unsigned int 			soc_sand_dev_id;
    uint32					soc_sand_rv;
	uint32 					mem_val[2];
	int mem,index;

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(global_sharing_ptr);
	soc_sand_dev_id = (unit);

	
	mem = (meter_id_group == 0) ? JER_PP_MTR_MEMORY(unit, MCDA_PRFSELm) : JER_PP_MTR_MEMORY(unit, MCDB_PRFSELm);
	index = meter_id / 4;

	
	soc_sand_rv = soc_mem_read(soc_sand_dev_id, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, mem_val);
	SOCDNX_IF_ERR_EXIT(soc_sand_rv);

	
	soc_mem_field_get(soc_sand_dev_id, mem, mem_val, GLOBAL_SHARINGf, global_sharing_ptr);
exit:
	SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_pp_mtr_policer_global_sharing_set(
    int                         unit,
	int                         core_id,
	int        					meter_id,
	int							meter_group,
	uint32* 					global_sharing_ptr
)
{
	unsigned int 			soc_sand_dev_id;
    uint32					soc_sand_rv;
	uint32 					mem_val[2];
	int mem,index;

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(global_sharing_ptr);
	soc_sand_dev_id = (unit);

	
	mem = (meter_group == 0) ? JER_PP_MTR_MEMORY(unit, MCDA_PRFSELm) : JER_PP_MTR_MEMORY(unit, MCDB_PRFSELm);
	
	index = meter_id / 4;

	
	soc_sand_rv = soc_mem_read(soc_sand_dev_id, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, mem_val); 
	SOCDNX_IF_ERR_EXIT(soc_sand_rv);

	
	soc_mem_field_set(soc_sand_dev_id, mem, mem_val, GLOBAL_SHARINGf, global_sharing_ptr);

	soc_sand_rv = soc_mem_write(soc_sand_dev_id, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, mem_val); 
	SOCDNX_IF_ERR_EXIT(soc_sand_rv);

exit:
	SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_pp_mtr_policer_global_mef_10_3_get(
    int                         unit,
	int*       					arg
)
{
	uint32 					reg_val;
    uint32                  enable;
    
	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(arg);

    
    if(SOC_IS_QAX(unit))
    {
        SOCDNX_IF_ERR_EXIT(READ_IMP_GLBL_CFGr(unit, &reg_val));
        enable = soc_reg_field_get(unit, IMP_GLBL_CFGr, reg_val, MEF_10_DOT_3_ENf);
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_MRPS_GLBL_CFGr(unit, &reg_val));
        enable = soc_reg_field_get(unit, MRPS_GLBL_CFGr, reg_val, MEF_10_DOT_3_ENf);
    } 

    *arg = (int)enable;
    
exit:
	SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_pp_mtr_policer_global_mef_10_3_set(
    int                         unit,
	int*       					arg
)
{
	uint32 					reg_val;
    uint32                  enable;
    int                     core_id;
	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(arg);

    enable = (uint32)*arg;

    if(enable != TRUE && enable != FALSE)
    {
        LOG_ERROR(BSL_LS_SOC_METERING, (BSL_META_U(unit, "unexpected value: enable=%d\n"), enable));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);        
    }
    if(SOC_IS_QAX(unit))
    {
        SOCDNX_IF_ERR_EXIT(READ_IMP_GLBL_CFGr(unit, &reg_val));
        soc_reg_field_set(unit, IMP_GLBL_CFGr, &reg_val, MEF_10_DOT_3_ENf, enable);       
        SOCDNX_IF_ERR_EXIT(WRITE_IMP_GLBL_CFGr(unit, reg_val));
    }
    else
    {
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id){			
            reg_val = 0;
            soc_reg32_get(unit, MRPS_GLBL_CFGr, core_id, 0, &reg_val);
            soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MEF_10_DOT_3_ENf, enable);
            soc_reg32_set(unit, MRPS_GLBL_CFGr, core_id, 0, reg_val);        
        }
    }

exit:
	SOCDNX_FUNC_RETURN;
}

uint32
  jer_pp_mtr_eth_policer_params_set(
    SOC_SAND_IN  int                         	unit,
	SOC_SAND_IN  int                         	core_id,
    SOC_SAND_IN  SOC_PPC_PORT                   port_ndx,
	SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE           eth_type_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO    *policer_info
  )
{
  unsigned int
	soc_sand_dev_id;

  uint32
	res,
	profile,
	write_access_enable,
	init_bucket_level = 1,
	config_tbl_entry,
	config_array_index,
    status_tbl_offset,
    prfSel_tbl_offset,
	prfSel_tbl_entry,
	bckLvl_tbl_offset,
	bckLvl_tbl_entry;
  int ret;

  soc_reg_above_64_val_t
	eth_mtr_bck_lvl_above_64_val,
    eth_mtr_prfCfg_above_64_val;
  uint32
	backet_init_val = 0,
	eth_mtr_prfSel_val = 0,
	eth_mtr_config_val = 0;

  soc_mem_t
	eth_mtr_prfSel_mem,
	eth_mtr_config_mem,
	eth_mtr_prfCfg_0_mem,
	eth_mtr_prfCfg_1_mem,
	eth_mtr_prfCfg_fields_mem,
	eth_mtr_bck_lvl_mem,
	eth_mtr_enable_dyn_access;

  uint32
	cir, cbs;
  uint8
	info_enable;

  uint32
	eth_mtr_profile_multiset_key = 0x0,
    eth_mtr_profile_multiset_ndx = 0x0;
  uint8
    eth_mtr_profile_multiset_first_appear = 0x0,
    eth_mtr_profile_multiset_last_appear = 0x0,
    sw_db_enable_bit;

  SOC_SAND_SUCCESS_FAILURE
    eth_mtr_profile_multiset_success = SOC_SAND_SUCCESS;
  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data;

  uint32
	profile_field[4] = {PROFILE_0f, PROFILE_1f, PROFILE_2f, PROFILE_3f};
  uint32
	cbl_bucket_fld[4]= {CBL_0f	  , CBL_1f    , CBL_2f    , CBL_3f};

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(policer_info);
	SOC_REG_ABOVE_64_CLEAR(eth_mtr_prfCfg_above_64_val);
	SOC_REG_ABOVE_64_CLEAR(eth_mtr_bck_lvl_above_64_val);
	info_enable = SOC_SAND_BOOL2NUM_INVERSE(policer_info->disable_cir);

	soc_sand_dev_id = (unit);
	eth_mtr_prfSel_mem 			= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFSEL);
	eth_mtr_prfCfg_fields_mem 	= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_SHARING_DIS); 
    eth_mtr_prfCfg_0_mem 		= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_0);
	eth_mtr_prfCfg_1_mem 		= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_1);
	eth_mtr_bck_lvl_mem			= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_DYNAMIC);
	eth_mtr_enable_dyn_access   = JER_MRPS_EM_REG_FORMAT_BY_CHIP(unit,ENABLE_DYNAMIC_MEMORY_ACCESS);

	if (SOC_IS_QAX(unit)) {
		eth_mtr_config_mem 		= CGM_ETH_MTR_PTR_MAPm;
	}else{
		eth_mtr_config_mem 		= IDR_ETHERNET_METER_CONFIGm;
	}

	config_array_index = core_id;
	status_tbl_offset = config_tbl_entry  = port_ndx * SOC_PPC_NOF_MTR_ETH_TYPES + eth_type_ndx;
	
	bckLvl_tbl_entry  = prfSel_tbl_entry  = config_tbl_entry >> 2;  
	bckLvl_tbl_offset = prfSel_tbl_offset = config_tbl_entry & 0x3; 

	  
	res = soc_mem_read(soc_sand_dev_id, eth_mtr_prfSel_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), prfSel_tbl_entry, (void*)&eth_mtr_prfSel_val);
	SOCDNX_IF_ERR_EXIT(res);

	if (SOC_IS_QAX(unit)) {
		res = soc_mem_read(soc_sand_dev_id, eth_mtr_config_mem, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(res);
	}else{
		res = soc_mem_array_read(soc_sand_dev_id, eth_mtr_config_mem, config_array_index, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(res);
	}

	profile = soc_mem_field32_get(unit, eth_mtr_prfSel_mem, (void*)&eth_mtr_prfSel_val, profile_field[prfSel_tbl_offset]);

	res = arad_sw_db_multiset_get_enable_bit(
			unit,
			core_id,
			status_tbl_offset,
			&sw_db_enable_bit
			);
	SOCDNX_SAND_IF_ERR_EXIT(res);

    if (sw_db_enable_bit == TRUE)
	{
		
		res = arad_sw_db_multiset_remove_by_index(
			unit,
			core_id,
			ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
			profile,
			&eth_mtr_profile_multiset_last_appear
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);
    }
 
	cir = policer_info->cir;
	cbs = policer_info->cbs;

	
	if (info_enable == TRUE)
	{
        if (cir != 0) {
            
            res = arad_pp_mtr_profile_rate_to_res_exp_mnt(
                unit,
                cir,
                &profile_tbl_data.meter_resolution,
                &profile_tbl_data.rate_mantissa,
                &profile_tbl_data.rate_exp
                );
            SOCDNX_SAND_IF_ERR_EXIT(res);
        } else {
            profile_tbl_data.meter_resolution = 0;
            profile_tbl_data.rate_mantissa = 0;
            profile_tbl_data.rate_exp = 0;
        }

		
		res = arad_pp_mtr_bs_val_to_exp_mnt(
			unit, 
			cbs,
			&profile_tbl_data.burst_exp,
			&profile_tbl_data.burst_mantissa
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);

		arad_pp_mtr_verify_valid_bucket_size(unit,
											 profile_tbl_data.rate_mantissa,profile_tbl_data.rate_exp,
											 0,0,
											 &profile_tbl_data.burst_mantissa,&profile_tbl_data.burst_exp);

		profile_tbl_data.packet_mode = SOC_SAND_BOOL2NUM(policer_info->is_packet_mode);
		profile_tbl_data.pkt_adj_header_truncate = SOC_SAND_BOOL2NUM(policer_info->is_pkt_truncate);
		profile_tbl_data.color_blind = policer_info->color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND ? 1 : 0;
		
		arad_pp_mtr_eth_policer_profile_key_get(
			unit,
			profile_tbl_data,
			&eth_mtr_profile_multiset_key
			);
    
		res = arad_sw_db_multiset_add(
			unit,
			core_id,
			ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
			&eth_mtr_profile_multiset_key,
			&eth_mtr_profile_multiset_ndx,
			&eth_mtr_profile_multiset_first_appear,
			&eth_mtr_profile_multiset_success
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);

		if (eth_mtr_profile_multiset_success != SOC_SAND_SUCCESS)
		{ 
			if (sw_db_enable_bit == TRUE) {
				
				res = soc_mem_read(soc_sand_dev_id, eth_mtr_prfCfg_0_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), profile, (void*)eth_mtr_prfCfg_above_64_val);
				SOCDNX_IF_ERR_EXIT(res);

				profile_tbl_data.rate_mantissa 	  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANTf);
			
				profile_tbl_data.rate_exp 		  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANT_EXPf);
				profile_tbl_data.burst_mantissa   = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_MANT_64f);
				profile_tbl_data.burst_exp 		  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_EXPONENTf);
				profile_tbl_data.packet_mode 	  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, PACKET_MODEf);
				profile_tbl_data.meter_resolution = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_REV_EXP_2f);
				profile_tbl_data.color_blind 	  = SOC_SAND_BOOL2NUM_INVERSE(soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, COLOR_AWAREf));
				profile_tbl_data.pkt_adj_header_truncate = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_4f);

				arad_pp_mtr_eth_policer_profile_key_get(
					unit,
					profile_tbl_data,
					&eth_mtr_profile_multiset_key
					);

				res = arad_sw_db_multiset_add(
					unit,
					core_id,
					ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
					&eth_mtr_profile_multiset_key,
					&eth_mtr_profile_multiset_ndx,
					&eth_mtr_profile_multiset_first_appear,
					&eth_mtr_profile_multiset_success
					);
				SOCDNX_SAND_IF_ERR_EXIT(res);

				
				init_bucket_level = 0;
			}
			else{
				SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Policer set - out of resources.\n")));
			}
		}
	}

	
	res = arad_pp_sw_db_eth_policer_config_status_bit_set(
		unit,
		core_id,
		status_tbl_offset,
		info_enable
		);

	SOCDNX_IF_ERR_EXIT(res);

	if (info_enable == TRUE) {
		
		if (
			(eth_mtr_profile_multiset_first_appear == 0x1) || 											
			((eth_mtr_profile_multiset_last_appear == 0x1) && (eth_mtr_profile_multiset_ndx != profile))
			)
		{
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, RESET_CIRf, policer_info->disable_cir);
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANTf, profile_tbl_data.rate_mantissa);
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANT_EXPf, profile_tbl_data.rate_exp);
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_MANT_64f, profile_tbl_data.burst_mantissa);
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_EXPONENTf, profile_tbl_data.burst_exp);
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, PACKET_MODEf, profile_tbl_data.packet_mode);
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_REV_EXP_2f, profile_tbl_data.meter_resolution);
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, COLOR_AWAREf, (uint32)SOC_SAND_BOOL2NUM_INVERSE(profile_tbl_data.color_blind));
			
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_3f, 0);
			
			if (policer_info->is_pkt_truncate) {
				soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void *)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_4f, 11);
			}else{
				soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void *)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_4f, 9);
			}

			res = soc_mem_write(soc_sand_dev_id, eth_mtr_prfCfg_0_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), eth_mtr_profile_multiset_ndx, (void*)eth_mtr_prfCfg_above_64_val); 
			SOCDNX_IF_ERR_EXIT(res);
			res = soc_mem_write(soc_sand_dev_id, eth_mtr_prfCfg_1_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), eth_mtr_profile_multiset_ndx, (void*)eth_mtr_prfCfg_above_64_val); 
			SOCDNX_IF_ERR_EXIT(res);
		}
	}

	
	soc_mem_field32_set(unit, eth_mtr_prfSel_mem, (void*)&eth_mtr_prfSel_val, profile_field[prfSel_tbl_offset],eth_mtr_profile_multiset_ndx);
	res = soc_mem_write(soc_sand_dev_id, eth_mtr_prfSel_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), prfSel_tbl_entry, (void*)&eth_mtr_prfSel_val);
	SOCDNX_IF_ERR_EXIT(res);

	if (init_bucket_level == 1) {
		
		
		soc_reg32_get(unit, eth_mtr_enable_dyn_access, core_id, 0, (void*)&write_access_enable);
		if (0 == write_access_enable) {
			res = soc_reg32_set(unit, eth_mtr_enable_dyn_access, core_id, 0, 1);
			SOCDNX_IF_ERR_EXIT(res);
		}

		
		res = soc_mem_read(soc_sand_dev_id, eth_mtr_bck_lvl_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), bckLvl_tbl_entry, (void*)eth_mtr_bck_lvl_above_64_val); 
		SOCDNX_IF_ERR_EXIT(res);

		ARAD_PP_MTR_DECIMAL_TO_BUKCET(cbs, backet_init_val);
		soc_mem_field32_set(unit, eth_mtr_bck_lvl_mem, (void*)eth_mtr_bck_lvl_above_64_val, cbl_bucket_fld[bckLvl_tbl_offset],backet_init_val);

		res = soc_mem_write(soc_sand_dev_id, eth_mtr_bck_lvl_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), bckLvl_tbl_entry, (void*)eth_mtr_bck_lvl_above_64_val);
		SOCDNX_IF_ERR_EXIT(res);

		
		if (0 == write_access_enable) {
			res = soc_reg32_set(unit, eth_mtr_enable_dyn_access, core_id, 0, 0);
			SOCDNX_IF_ERR_EXIT(res);
		}

	}

	
	if (SOC_IS_QAX(unit)) {
		soc_mem_field32_set(unit, eth_mtr_config_mem, &eth_mtr_config_val, ETH_MTR_PTR_VALIDf, info_enable);
		res = soc_mem_write(soc_sand_dev_id, eth_mtr_config_mem, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(res);
	}else{
		soc_mem_field32_set(unit, eth_mtr_config_mem, &eth_mtr_config_val, ETHERNET_METER_PROFILE_VALIDf, info_enable);
		ret = soc_mem_array_write(soc_sand_dev_id, eth_mtr_config_mem, config_array_index, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(ret);
	}

exit:
	SOCDNX_FUNC_RETURN;
}

uint32
  jer_pp_mtr_eth_policer_params_get(
    SOC_SAND_IN  int                        	unit,
	SOC_SAND_IN  int                         	core_id,
    SOC_SAND_IN  SOC_PPC_PORT                   port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE           eth_type_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO    *policer_info
  )
{
  unsigned int
	soc_sand_dev_id;

  uint32
	res,
	profile,
    status_tbl_offset,
    prfSel_tbl_offset,
	prfSel_tbl_entry;

  soc_reg_above_64_val_t
    eth_mtr_prfCfg_above_64_val;
  uint32
	eth_mtr_prfSel_val;

  int
	eth_mtr_prfSel_mem,
	eth_mtr_prfCfg_fields_mem,
    eth_mtr_prfCfg_mem;

  uint8
    sw_db_enable_bit;

  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data;

uint32
	profile_field[4] = {PROFILE_0f, PROFILE_1f, PROFILE_2f, PROFILE_3f};

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(policer_info);
	
	soc_sand_dev_id = (unit);
	eth_mtr_prfSel_mem 			= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFSEL);
	eth_mtr_prfCfg_fields_mem 	= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_SHARING_DIS); 
	eth_mtr_prfCfg_mem 			= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_0); 

	status_tbl_offset = port_ndx * SOC_PPC_NOF_MTR_ETH_TYPES + eth_type_ndx;
	
	prfSel_tbl_entry  = status_tbl_offset >> 2;  
	prfSel_tbl_offset = status_tbl_offset & 0x3; 

	
	res = soc_mem_read(soc_sand_dev_id, eth_mtr_prfSel_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), prfSel_tbl_entry, &eth_mtr_prfSel_val);
	SOCDNX_IF_ERR_EXIT(res);

	profile = soc_mem_field32_get(unit, eth_mtr_prfSel_mem, &eth_mtr_prfSel_val, profile_field[prfSel_tbl_offset]);

	res = arad_sw_db_multiset_get_enable_bit(
			unit,
			core_id,
			status_tbl_offset,
			&sw_db_enable_bit
			);
	SOCDNX_SAND_IF_ERR_EXIT(res);

	if (sw_db_enable_bit == TRUE) {
		
		res = soc_mem_read(soc_sand_dev_id, eth_mtr_prfCfg_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), profile, (void*)eth_mtr_prfCfg_above_64_val);
		SOCDNX_IF_ERR_EXIT(res);

		profile_tbl_data.rate_mantissa 	  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANTf);
		profile_tbl_data.rate_exp 	 	  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANT_EXPf);
		profile_tbl_data.burst_mantissa   = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_MANT_64f);
		profile_tbl_data.burst_exp 		  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_EXPONENTf);
		profile_tbl_data.packet_mode 	  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, PACKET_MODEf);
		profile_tbl_data.pkt_adj_header_truncate = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_4f);
		profile_tbl_data.meter_resolution = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_REV_EXP_2f);
		profile_tbl_data.color_blind 	  = SOC_SAND_BOOL2NUM_INVERSE(soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, COLOR_AWAREf));

		
		policer_info->is_packet_mode = SOC_SAND_NUM2BOOL(profile_tbl_data.packet_mode);
		policer_info->is_pkt_truncate= SOC_SAND_NUM2BOOL(profile_tbl_data.pkt_adj_header_truncate & 0x2);
		policer_info->color_mode 	 = profile_tbl_data.color_blind ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;
		policer_info->disable_cir 	 = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, RESET_CIRf);

		res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
			unit,
			profile_tbl_data.meter_resolution,
			profile_tbl_data.rate_exp,
			profile_tbl_data.rate_mantissa,
			&policer_info->cir
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);

		res = soc_sand_compute_complex_to_mnt_exp(
			profile_tbl_data.burst_mantissa,
			profile_tbl_data.burst_exp,
			JER_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
			JER_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
			&policer_info->cbs
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);
	}else{
		policer_info->cbs = 0;
		policer_info->cir = 0;
		policer_info->color_mode = 0;
		policer_info->is_packet_mode = 0;
		policer_info->is_pkt_truncate = 0;
		policer_info->color_mode = 0;
	}
exit:
	SOCDNX_FUNC_RETURN;
}

uint32
	jer_pp_mtr_eth_policer_glbl_profile_set(
	   SOC_SAND_IN int       						unit,
	   SOC_SAND_IN int								core_id,
	   SOC_SAND_IN uint32	                		policer_ndx,
	   SOC_SAND_IN SOC_PPC_MTR_BW_PROFILE_INFO    	*policer_info
	)
{
  unsigned int
	soc_sand_dev_id;

  uint32
	res;
  uint8
	glbl_info_enable,
	write_access_enable;
  uint32 cir,cbs;
  uint32
	prfCfg_tbl_entry,
	prfSel_tbl_offset,
	prfSel_tbl_entry,
	bckLvl_tbl_entry,
	bckLvl_tbl_offset;

  soc_reg_above_64_val_t
	eth_mtr_prfCfg_above_64_val,
	eth_mtr_bck_lvl_above_64_val;
  uint32
	eth_mtr_prfSel_val = 0,
	backet_init_val = 0;

  soc_mem_t
	eth_mtr_prfSel_mem,
	eth_mtr_prfCfg_fields_mem,
	eth_mtr_prfCfg_0_mem,
	eth_mtr_prfCfg_1_mem,
	eth_mtr_bck_lvl_mem,
	eth_mtr_enable_dyn_access;

	ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA
	profile_tbl_data;

  uint32
	profile_field[4] = {PROFILE_0f, PROFILE_1f, PROFILE_2f, PROFILE_3f};
  uint32
	cbl_bucket_fld[4]= {CBL_0f	  , CBL_1f    , CBL_2f    , CBL_3f};

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(policer_info);
	SOC_REG_ABOVE_64_CLEAR(eth_mtr_prfCfg_above_64_val);
	SOC_REG_ABOVE_64_CLEAR(eth_mtr_bck_lvl_above_64_val);

	glbl_info_enable = SOC_SAND_BOOL2NUM_INVERSE(policer_info->disable_cir);

	soc_sand_dev_id = (unit);
	eth_mtr_prfSel_mem 			= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDB_PRFSEL);
	eth_mtr_prfCfg_fields_mem 	= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_SHARING_DIS); 
    eth_mtr_prfCfg_0_mem 		= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDB_PRFCFG_0);
	eth_mtr_prfCfg_1_mem 		= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDB_PRFCFG_1);
	eth_mtr_bck_lvl_mem			= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDB_DYNAMIC);
	eth_mtr_enable_dyn_access   = JER_MRPS_EM_REG_FORMAT_BY_CHIP(unit,ENABLE_DYNAMIC_MEMORY_ACCESS);
	
	prfCfg_tbl_entry   = policer_ndx;
	
	bckLvl_tbl_entry = prfSel_tbl_entry  = prfCfg_tbl_entry >> 2;  
	bckLvl_tbl_offset= prfSel_tbl_offset = prfCfg_tbl_entry & 0x3; 

	  
	res = soc_mem_read(soc_sand_dev_id, eth_mtr_prfSel_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), prfSel_tbl_entry, (void*)&eth_mtr_prfSel_val);
	SOCDNX_IF_ERR_EXIT(res);

	cir = policer_info->cir;
	cbs = policer_info->cbs;

	if(glbl_info_enable == TRUE){
		
		res = arad_pp_mtr_profile_rate_to_res_exp_mnt(
		   unit,
		   cir,
		   &profile_tbl_data.meter_resolution,
		   &profile_tbl_data.rate_mantissa,
		   &profile_tbl_data.rate_exp
		   );
		SOCDNX_SAND_IF_ERR_EXIT(res);

		
		res = arad_pp_mtr_bs_val_to_exp_mnt(
			unit, 
			cbs,
			&profile_tbl_data.burst_exp,
			&profile_tbl_data.burst_mantissa
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);

		arad_pp_mtr_verify_valid_bucket_size(unit,
											 profile_tbl_data.rate_mantissa,profile_tbl_data.rate_exp,
											 0,0,
											 &profile_tbl_data.burst_mantissa,&profile_tbl_data.burst_exp);

		res = arad_pp_sw_db_eth_policer_config_status_bit_set(
			unit,
			core_id,
			ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + policer_ndx,
			glbl_info_enable
		);

		SOCDNX_SAND_IF_ERR_EXIT(res);
		profile_tbl_data.packet_mode = SOC_SAND_BOOL2NUM(policer_info->is_packet_mode);
		profile_tbl_data.color_blind = policer_info->color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND ? 1 : 0;
		
		if (soc_property_get(unit, spn_RATE_COLOR_BLIND, 0) || policer_info->color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND) {
		  profile_tbl_data.color_blind = 1;
		}
	
		
		soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, RESET_CIRf, policer_info->disable_cir);
		soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANTf, profile_tbl_data.rate_mantissa);
		soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANT_EXPf, profile_tbl_data.rate_exp);
		soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_MANT_64f, profile_tbl_data.burst_mantissa);
		soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_EXPONENTf, profile_tbl_data.burst_exp);
		soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, PACKET_MODEf, profile_tbl_data.packet_mode);
		soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_REV_EXP_2f, profile_tbl_data.meter_resolution);
		soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, COLOR_AWAREf, (uint32)SOC_SAND_BOOL2NUM_INVERSE(profile_tbl_data.color_blind));

		
		soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_3f, 0);
		
		if (policer_info->is_pkt_truncate) {
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void *)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_4f, 11);
		}else{
			soc_mem_field32_set(unit, eth_mtr_prfCfg_fields_mem, (void *)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_4f, 9);
		}

		res = soc_mem_write(soc_sand_dev_id, eth_mtr_prfCfg_0_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), prfCfg_tbl_entry, (void*)eth_mtr_prfCfg_above_64_val); 
		SOCDNX_IF_ERR_EXIT(res);
		res = soc_mem_write(soc_sand_dev_id, eth_mtr_prfCfg_1_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), prfCfg_tbl_entry, (void*)eth_mtr_prfCfg_above_64_val); 
		SOCDNX_IF_ERR_EXIT(res);
	
		
		soc_mem_field32_set(unit, eth_mtr_prfSel_mem, &eth_mtr_prfSel_val, profile_field[prfSel_tbl_offset],policer_ndx);
		res = soc_mem_write(soc_sand_dev_id, eth_mtr_prfSel_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), prfSel_tbl_entry, (void*)&eth_mtr_prfSel_val);
		SOCDNX_IF_ERR_EXIT(res);
	}else{
		

		
		soc_mem_field32_set(unit, eth_mtr_prfSel_mem, &eth_mtr_prfSel_val, profile_field[prfSel_tbl_offset],0);
		res = soc_mem_write(soc_sand_dev_id, eth_mtr_prfSel_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), prfSel_tbl_entry, (void*)&eth_mtr_prfSel_val);
		SOCDNX_IF_ERR_EXIT(res);

		
		
		soc_reg32_get(unit, eth_mtr_enable_dyn_access, core_id, 0, (void*)&write_access_enable);
		if (0 == write_access_enable) {
			res = soc_reg32_set(unit, eth_mtr_enable_dyn_access, core_id, 0, 1);
			SOCDNX_IF_ERR_EXIT(res);
		}

		
		res = soc_mem_read(soc_sand_dev_id, eth_mtr_bck_lvl_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), bckLvl_tbl_entry, (void*)eth_mtr_bck_lvl_above_64_val); 
		SOCDNX_IF_ERR_EXIT(res);

		backet_init_val = 0x7FDFFF;
		soc_mem_field32_set(unit, eth_mtr_bck_lvl_mem, (void*)eth_mtr_bck_lvl_above_64_val, cbl_bucket_fld[bckLvl_tbl_offset],backet_init_val);

		res = soc_mem_write(soc_sand_dev_id, eth_mtr_bck_lvl_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), bckLvl_tbl_entry, (void*)eth_mtr_bck_lvl_above_64_val);
		SOCDNX_IF_ERR_EXIT(res);

		
		if (0 == write_access_enable) {
			res = soc_reg32_set(unit, eth_mtr_enable_dyn_access, core_id, 0, 0);
			SOCDNX_IF_ERR_EXIT(res);
		}
	}
exit:
	SOCDNX_FUNC_RETURN;
}

uint32  
  jer_pp_mtr_eth_policer_glbl_profile_get(
	 SOC_SAND_IN  int                      		unit,
	 SOC_SAND_IN  int                      		core_id,
	 SOC_SAND_IN  uint32                      	glbl_profile_idx,
	 SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO 	*policer_info
  )
{
  unsigned int
	soc_sand_dev_id;

  uint32
	res,
	profile;
  uint8
	sw_db_enable_bit;

  uint32
	prfCfg_tbl_entry,
	prfSel_tbl_offset,
	prfSel_tbl_entry;

  soc_reg_above_64_val_t
	eth_mtr_prfCfg_above_64_val;
  uint32
	eth_mtr_prfSel_val = 0;

  soc_mem_t
	eth_mtr_prfSel_mem,
	eth_mtr_prfCfg_fields_mem,
	eth_mtr_prfCfg_mem;

	ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA
	profile_tbl_data;

  uint32
    profile_field[4] = {PROFILE_0f, PROFILE_1f, PROFILE_2f, PROFILE_3f};

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(policer_info);
	SOC_REG_ABOVE_64_CLEAR(eth_mtr_prfCfg_above_64_val);
	SOC_PPC_MTR_BW_PROFILE_INFO_clear(policer_info);

	soc_sand_dev_id = (unit);
	eth_mtr_prfSel_mem 			= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDB_PRFSEL);
	eth_mtr_prfCfg_fields_mem 	= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_PRFCFG_SHARING_DIS); 
	eth_mtr_prfCfg_mem 			= JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDB_PRFCFG_0); 			 

	prfCfg_tbl_entry = glbl_profile_idx;
	
	prfSel_tbl_entry  = prfCfg_tbl_entry >> 2;  
	prfSel_tbl_offset = prfCfg_tbl_entry & 0x3; 

	
	res = soc_mem_read(soc_sand_dev_id, eth_mtr_prfSel_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), prfSel_tbl_entry, &eth_mtr_prfSel_val);
	SOCDNX_IF_ERR_EXIT(res);

	profile = soc_mem_field32_get(unit, eth_mtr_prfSel_mem, &eth_mtr_prfSel_val, profile_field[prfSel_tbl_offset]);

	
	res = soc_mem_read(soc_sand_dev_id, eth_mtr_prfCfg_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), profile, (void*)eth_mtr_prfCfg_above_64_val);
	SOCDNX_IF_ERR_EXIT(res);

	res = arad_sw_db_multiset_get_enable_bit(
			unit,
			core_id,
			ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + prfCfg_tbl_entry,
			&sw_db_enable_bit
			);
	SOCDNX_SAND_IF_ERR_EXIT(res);

	if (sw_db_enable_bit == TRUE) {
		profile_tbl_data.rate_mantissa 	  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANTf);
		profile_tbl_data.rate_exp 	 	  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_MANT_EXPf);
		profile_tbl_data.burst_mantissa   = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_MANT_64f);
		profile_tbl_data.burst_exp 		  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CBS_EXPONENTf);
		profile_tbl_data.packet_mode 	  = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, PACKET_MODEf);
		profile_tbl_data.meter_resolution = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, CIR_REV_EXP_2f);
		profile_tbl_data.color_blind 	  = SOC_SAND_BOOL2NUM_INVERSE(soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, COLOR_AWAREf));
		profile_tbl_data.pkt_adj_header_truncate = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, MTR_PROFILE_MAP_TO_4f);

		
		policer_info->is_packet_mode = SOC_SAND_NUM2BOOL(profile_tbl_data.packet_mode);
		policer_info->color_mode 	 = profile_tbl_data.color_blind ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;
		policer_info->disable_cir 	 = soc_mem_field32_get(unit, eth_mtr_prfCfg_fields_mem, (void*)eth_mtr_prfCfg_above_64_val, RESET_CIRf);
		policer_info->is_pkt_truncate= SOC_SAND_NUM2BOOL(profile_tbl_data.pkt_adj_header_truncate & 0x2);

		res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
			unit,
			profile_tbl_data.meter_resolution,
			profile_tbl_data.rate_exp,
			profile_tbl_data.rate_mantissa,
			&policer_info->cir
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);

		res = soc_sand_compute_complex_to_mnt_exp(
			profile_tbl_data.burst_mantissa,
			profile_tbl_data.burst_exp,
			JER_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
			JER_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
			&policer_info->cbs
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);
	}else{
		policer_info->cbs = 0;
		policer_info->cir = 0;
		policer_info->color_mode = 0;
		policer_info->is_packet_mode = 0;
		policer_info->color_mode = 0;
	}

exit:
	 SOCDNX_FUNC_RETURN;
}

uint32  
  jer_pp_mtr_eth_policer_glbl_profile_map_set(
    SOC_SAND_IN  int                  	unit,
	SOC_SAND_IN  int                  	core_id,
    SOC_SAND_IN  SOC_PPC_PORT           port,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE   eth_type_ndx,
    SOC_SAND_IN  uint32                 glbl_profile_idx
  )
{
  unsigned int 
	soc_sand_dev_id;

  uint32
    config_tbl_entry,
    eth_mtr_config_val,
    glbl_mtr_ptr_val;
  int res;

  soc_mem_t
	eth_mtr_config_mem;
  uint32
	glbl_prf_ptr_field,
	glbl_prf_valid_field;

  int
	config_array_index;

	SOCDNX_INIT_FUNC_DEFS;

	soc_sand_dev_id = (unit);
	config_array_index = core_id;
	config_tbl_entry   = port * SOC_PPC_NOF_MTR_ETH_TYPES + eth_type_ndx;
	glbl_mtr_ptr_val = glbl_profile_idx;

	glbl_prf_ptr_field   = SOC_IS_QAX(unit) ? GLBL_ETH_MTR_PTRf : GLOBAL_METER_PROFILEf ;
	glbl_prf_valid_field = SOC_IS_QAX(unit) ? GLBL_ETH_MTR_PTR_VALIDf : GLOBAL_METER_PROFILE_VALIDf ;
	eth_mtr_config_mem	 = SOC_IS_QAX(unit) ? CGM_ETH_MTR_PTR_MAPm : IDR_ETHERNET_METER_CONFIGm;

	
	if (SOC_IS_QAX(unit)) {
		res = soc_mem_read(soc_sand_dev_id, eth_mtr_config_mem, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(res);
	}else{
		res = soc_mem_array_read(soc_sand_dev_id, eth_mtr_config_mem, config_array_index, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(res);
	}

	
	soc_mem_field32_set(unit, eth_mtr_config_mem, &eth_mtr_config_val, glbl_prf_ptr_field, glbl_mtr_ptr_val);
	if (glbl_profile_idx == 0) { 
		soc_mem_field32_set(unit, eth_mtr_config_mem, &eth_mtr_config_val, glbl_prf_valid_field, 0);
	}else{
		soc_mem_field32_set(unit, eth_mtr_config_mem, &eth_mtr_config_val, glbl_prf_valid_field, 1);
	}

	if (SOC_IS_QAX(unit)) {
		res = soc_mem_write(soc_sand_dev_id, eth_mtr_config_mem, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(res);
	}else{
		res = soc_mem_array_write(soc_sand_dev_id, eth_mtr_config_mem, config_array_index, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(res);
	}
  
exit:
	SOCDNX_FUNC_RETURN;
}

uint32  
  jer_pp_mtr_eth_policer_glbl_profile_map_get(
	 SOC_SAND_IN  int                  	unit,
	 SOC_SAND_IN  int                  	core_id,
	 SOC_SAND_IN  SOC_PPC_PORT          port,
	 SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE  eth_type_ndx,
	 SOC_SAND_OUT uint32                *glbl_profile_idx
   )
 {
  unsigned int 
	soc_sand_dev_id;

  uint32
	res,
	config_tbl_entry,
	eth_mtr_config_val,
	glbl_mtr_ptr_val;

  soc_mem_t
	eth_mtr_config_mem;
  uint32
	glbl_prf_ptr_field;
  int
	config_array_index;

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(glbl_profile_idx);

	soc_sand_dev_id = (unit);
	config_array_index = core_id;
	config_tbl_entry   = port * SOC_PPC_NOF_MTR_ETH_TYPES + eth_type_ndx;

	glbl_prf_ptr_field   = SOC_IS_QAX(unit) ? GLBL_ETH_MTR_PTRf : GLOBAL_METER_PROFILEf ;	
    eth_mtr_config_mem	 = SOC_IS_QAX(unit) ? CGM_ETH_MTR_PTR_MAPm : IDR_ETHERNET_METER_CONFIGm;

    
    if (SOC_IS_QAX(unit)) {
		res = soc_mem_read(soc_sand_dev_id, eth_mtr_config_mem, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(res);
	}else{
		res = soc_mem_array_read(soc_sand_dev_id, eth_mtr_config_mem, config_array_index, MEM_BLOCK_ANY, config_tbl_entry, (void*)&eth_mtr_config_val);
		SOCDNX_IF_ERR_EXIT(res);
	}

	soc_mem_field_get(unit, eth_mtr_config_mem, &eth_mtr_config_val, glbl_prf_ptr_field, &glbl_mtr_ptr_val);
    *glbl_profile_idx = glbl_mtr_ptr_val;

 exit:
	 SOCDNX_FUNC_RETURN;
}

uint32 
jer_pp_metering_init_header_compensation(
	int unit
	)
{
uint32
	res,
	packet_size_profile_multiset_key = 0x0,
    packet_size_profile_multiset_ndx = 0x0;
uint8
    packet_size_profile_multiset_first_appear = 0x0;
int
	pp_port, core_index;
SOC_SAND_SUCCESS_FAILURE
    packet_size_profile_multiset_success = SOC_SAND_SUCCESS;
JER_PACKET_SIZE_PROFILES_TBL_DATA
    profile_tbl_data = {0};

	SOCDNX_INIT_FUNC_DEFS;

	

	
	res = jer_pp_metering_packet_size_profile_key_get(
		unit,
		profile_tbl_data,
		&packet_size_profile_multiset_key
		);
	SOCDNX_IF_ERR_EXIT(res);

	
	ARAD_PP_MTR_CORES_ITER(core_index){
		
		res = arad_sw_db_multiset_add(
			unit,
			core_index,
			JER_PP_SW_DB_MULTI_SET_POLICER_SIZE_PROFILE,
			&packet_size_profile_multiset_key,
			&packet_size_profile_multiset_ndx,
			&packet_size_profile_multiset_first_appear,
			&packet_size_profile_multiset_success
			);
		SOCDNX_SAND_IF_ERR_EXIT(res);

		
		for (pp_port = 0; pp_port < JER_PP_MTR_IN_PP_PORT_NOF; pp_port++) {
			res = arad_sw_db_multiset_add_by_index(
				unit,
				core_index,
				JER_PP_SW_DB_MULTI_SET_POLICER_SIZE_PROFILE,
				&packet_size_profile_multiset_key,
				packet_size_profile_multiset_ndx,
				&packet_size_profile_multiset_first_appear,
				&packet_size_profile_multiset_success
				);
			SOCDNX_SAND_IF_ERR_EXIT(res);
		}
	}

exit:
  SOCDNX_FUNC_RETURN;
}

uint32 
  jer_pp_metering_in_pp_port_map_and_mtr_prf_map_set(
	  int                     unit, 
	  int                     core_id, 
	  int                     meter_group, 
	  uint32                  map_to_3_prf, 
	  uint32                  pp_port_prf, 
	  int                     packet_size
  )
{
uint32
	line_number,
	cell_number,
	table_line,
	fldval,
	rv;
soc_mem_t
	mem[2][2];

soc_field_t
	field[2][4] = {{MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_0f, 
		            MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_1f, 
 		            MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_2f,
		            MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_3f},
				   {MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_0f, 
		            MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_1f, 
 		            MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_2f,
		            MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_3f}};

	SOCDNX_INIT_FUNC_DEFS;

	mem[0][0] = JER_MRPS_MEM_FORMAT_BY_CHIP(unit,MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP);
	mem[0][1] = JER_MRPS_MEM_FORMAT_BY_CHIP(unit,MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP);
	mem[1][0] = JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP);
	mem[1][1] = JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP);

	line_number = JER_PP_MTR_IN_PP_PORT_AND_MTR_PRF_TO_TABLE_ROW(pp_port_prf, map_to_3_prf);
	cell_number = JER_PP_MTR_MAP_TO_3_PRF_TO_TABLE_CELL(map_to_3_prf);

	rv = soc_mem_read(unit, mem[0][meter_group], ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, &table_line);
	SOCDNX_IF_ERR_EXIT(rv);

	fldval = JER_PP_MTR_PACKET_SIZE_TO_2S_COMPLIMENT(packet_size);

	soc_mem_field_set(unit, mem[0][meter_group], &table_line, field[meter_group][cell_number], &fldval);

	
	rv = soc_mem_write(unit, mem[0][meter_group], ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, &table_line);
	SOCDNX_IF_ERR_EXIT(rv);

	rv = soc_mem_write(unit, mem[1][meter_group], ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), line_number, &table_line);
	SOCDNX_IF_ERR_EXIT(rv);

exit:
	SOCDNX_FUNC_RETURN;
}

uint32 
  jer_pp_metering_in_pp_port_map_and_mtr_prf_map_get(
	  int                     unit, 
	  int                     core_id, 
	  int                     meter_group, 
	  uint32                  map_to_3_prf, 
	  uint32                  pp_port_prf, 
	  int                     *packet_size
  )
{
uint32
    line_number,
	cell_number,
	table_line,
	fldval,
    rv;
soc_mem_t
	mem[2];

soc_field_t
	field[2][4] = {{MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_0f, 
		            MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_1f, 
 		            MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_2f,
		            MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_3f},
				   {MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_0f, 
		            MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_1f, 
 		            MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_2f,
		            MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP_3f}};

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(packet_size);

	mem[0] = JER_MRPS_MEM_FORMAT_BY_CHIP(unit,MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAP);
	mem[1] = JER_MRPS_MEM_FORMAT_BY_CHIP(unit,MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAP);

	*packet_size = 0;

	line_number = JER_PP_MTR_IN_PP_PORT_AND_MTR_PRF_TO_TABLE_ROW(pp_port_prf, map_to_3_prf);
	cell_number = JER_PP_MTR_MAP_TO_3_PRF_TO_TABLE_CELL(map_to_3_prf);

	rv = soc_mem_read(unit, mem[meter_group], ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, &table_line);
	SOCDNX_IF_ERR_EXIT(rv);

	soc_mem_field_get(unit, mem[meter_group], &table_line, field[meter_group][cell_number], &fldval);

	*packet_size = JER_PP_MTR_2S_COMPLIMENT_TO_PACKET_SIZE(fldval);

exit:
	SOCDNX_FUNC_RETURN;
}

uint32 
  jer_pp_metering_in_pp_port_map_get(
	  int           unit, 
	  int           core_id, 
	  int           meter_group, 
	  uint32        pp_port,
	  uint32        *pp_port_prf)
{
uint32
    line_number, 
	cell_number,
	table_line,
	rv;
soc_mem_t
	mem[2];

soc_field_t
	port_profile_field[2][4] = {{MCDA_IN_PP_PORT_MAP_0f, 
		                         MCDA_IN_PP_PORT_MAP_1f, 
								 MCDA_IN_PP_PORT_MAP_2f, 
		                         MCDA_IN_PP_PORT_MAP_3f},
                                {MCDB_IN_PP_PORT_MAP_0f, 
		                         MCDB_IN_PP_PORT_MAP_1f, 
								 MCDB_IN_PP_PORT_MAP_2f, 
		                         MCDB_IN_PP_PORT_MAP_3f}};

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(pp_port_prf);

	mem[0] = JER_MRPS_MEM_FORMAT_BY_CHIP(unit,MCDA_IN_PP_PORT_MAP);
	mem[1] = JER_MRPS_MEM_FORMAT_BY_CHIP(unit,MCDB_IN_PP_PORT_MAP);

	
	line_number = pp_port / 4;
	cell_number = pp_port % 4;
	
	rv = soc_mem_read(unit, mem[meter_group], ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, &table_line);
	SOCDNX_IF_ERR_EXIT(rv);

	*pp_port_prf = soc_mem_field32_get(unit, mem[meter_group], &table_line, port_profile_field[meter_group][cell_number]);

exit:
	SOCDNX_FUNC_RETURN;
}

uint32 
  jer_pp_metering_in_pp_port_map_set(
	  int           unit, 
	  int           core_id, 
	  int           meter_group, 
	  uint32        pp_port, 
	  uint32        pp_port_prf)
{
uint32
    line_number, 
	cell_number,
	table_line,
	fldval,
	rv;
soc_mem_t
	mem[2][2];

soc_field_t
	port_profile_field[2][4] = {{MCDA_IN_PP_PORT_MAP_0f, 
		                         MCDA_IN_PP_PORT_MAP_1f, 
								 MCDA_IN_PP_PORT_MAP_2f, 
		                         MCDA_IN_PP_PORT_MAP_3f},
                                {MCDB_IN_PP_PORT_MAP_0f, 
		                         MCDB_IN_PP_PORT_MAP_1f, 
								 MCDB_IN_PP_PORT_MAP_2f, 
		                         MCDB_IN_PP_PORT_MAP_3f}};

	SOCDNX_INIT_FUNC_DEFS;

	mem[0][0] = JER_MRPS_MEM_FORMAT_BY_CHIP(unit,MCDA_IN_PP_PORT_MAP);
	mem[0][1] = JER_MRPS_MEM_FORMAT_BY_CHIP(unit,MCDB_IN_PP_PORT_MAP);
	mem[1][0] = JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDA_IN_PP_PORT_MAP);
	mem[1][1] = JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit,MCDB_IN_PP_PORT_MAP);

	
	line_number = pp_port / 4;
	cell_number = pp_port % 4;
	
	rv = soc_mem_read(unit, mem[0][meter_group], ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, &table_line);
	SOCDNX_IF_ERR_EXIT(rv);

	fldval = pp_port_prf;
	soc_mem_field_set(unit, mem[0][meter_group], &table_line, port_profile_field[meter_group][cell_number], &fldval);

	
	rv = soc_mem_write(unit, mem[0][meter_group], ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, &table_line);
	SOCDNX_IF_ERR_EXIT(rv);

	rv = soc_mem_write(unit, mem[1][meter_group], ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), line_number, &table_line);
	SOCDNX_IF_ERR_EXIT(rv);

exit:
	SOCDNX_FUNC_RETURN;
}

uint32
  jer_pp_metering_packet_size_profile_key_get(
    int                                unit,
    JER_PACKET_SIZE_PROFILES_TBL_DATA  profile_tbl_data,
	uint32                             *packet_size_profile_multiset_key
  )
{
	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(packet_size_profile_multiset_key);

	*packet_size_profile_multiset_key = 0;
	*packet_size_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.in_pp_port_size_delta,7,0);

exit:
	SOCDNX_FUNC_RETURN;
}

uint32
jer_pp_mtr_policer_ipg_compensation_set(
    int                         unit,
	uint8						ipg_compensation_enabled
)
{
uint32
	rv,
	meter_group, 
	pp_port_prf, 
	core_id;
uint8
	is_enabled;
int
	packet_size;

	SOCDNX_INIT_FUNC_DEFS;

	
	rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_get(unit, 0, 0, 0, 0, &packet_size);
	SOCDNX_IF_ERR_EXIT(rv);

	is_enabled = ((packet_size == SOC_PPC_MTR_IPG_COMPENSATION_ENABLED_SIZE) ? TRUE : FALSE);

	
	if (is_enabled == ipg_compensation_enabled) {
		SOC_EXIT;
	}

	ARAD_PP_MTR_CORES_ITER(core_id){
		
		for (meter_group = 0; meter_group < 2; meter_group++) {

			
			rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_set(unit, core_id, meter_group, 0, 0, 
														ipg_compensation_enabled ? SOC_PPC_MTR_IPG_COMPENSATION_ENABLED_SIZE : SOC_PPC_MTR_IPG_COMPENSATION_DISABLED_SIZE);
			SOCDNX_IF_ERR_EXIT(rv);

			
			for (pp_port_prf = 1; pp_port_prf < JER_PP_MTR_IN_PP_PORT_MAP_NOF_PROFILES; pp_port_prf++) {

				rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_get(unit, core_id, meter_group, 0, pp_port_prf, &packet_size);
				SOCDNX_IF_ERR_EXIT(rv);
					
				
				if (ipg_compensation_enabled) {
					packet_size += SOC_PPC_MTR_IPG_COMPENSATION_ENABLED_SIZE;

					
					if (packet_size > JER_PP_MTR_2S_COMPLIMENT_MAX_VALUE) {
						packet_size = JER_PP_MTR_2S_COMPLIMENT_MAX_VALUE;
					}
				} else{
					packet_size -= SOC_PPC_MTR_IPG_COMPENSATION_ENABLED_SIZE;

					
					if (packet_size < JER_PP_MTR_2S_COMPLIMENT_MIN_VALUE) {
						packet_size = JER_PP_MTR_2S_COMPLIMENT_MIN_VALUE;
					}
				}

				rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_set(unit, core_id, meter_group, 0, pp_port_prf, packet_size);
				SOCDNX_IF_ERR_EXIT(rv);
			}
		}
	}

exit:
	SOCDNX_FUNC_RETURN;
}

uint32
jer_pp_mtr_policer_ipg_compensation_get(
    int                         unit,
	uint8						*ipg_compensation_enabled
)
{
uint32 
	rv;
int
	packet_size;

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(ipg_compensation_enabled);

	rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_get(unit, 0, 0, 0, 0, &packet_size);
	SOCDNX_IF_ERR_EXIT(rv);

	*ipg_compensation_enabled = ((packet_size == SOC_PPC_MTR_IPG_COMPENSATION_ENABLED_SIZE) ? TRUE : FALSE);

exit:
	SOCDNX_FUNC_RETURN;
}

uint32 
jer_pp_mtr_policer_hdr_compensation_verify(
	int             unit, 
	int             compensation_size
)
{
	SOCDNX_INIT_FUNC_DEFS;

	if ((compensation_size + SOC_PPC_MTR_IPG_COMPENSATION_ENABLED_SIZE > JER_PP_MTR_2S_COMPLIMENT_MAX_VALUE) ||
		(compensation_size < JER_PP_MTR_2S_COMPLIMENT_MIN_VALUE)){
		SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Requested compensation size is outside of allowed range [-108,+127].")));
	}

exit:
	SOCDNX_FUNC_RETURN;
}

uint32
jer_pp_mtr_policer_hdr_compensation_set(
    int                         unit,
	int                         core_id,
	uint32                      pp_port,
	int                         compensation_size
)
{
uint32
	rv,
	meter_group,
	previous_profile,
	packet_size_profile_multiset_key = 0,
	packet_size_profile_multiset_ndx = 0;
uint8
    packet_size_profile_multiset_last_appear = 0,
	packet_size_profile_multiset_first_appear = 0;
int
	previous_profile_compensation, 
	default_profile_compensation;
JER_PACKET_SIZE_PROFILES_TBL_DATA
    profile_tbl_data = {0};
SOC_SAND_SUCCESS_FAILURE
    packet_size_profile_multiset_success = SOC_SAND_SUCCESS;

	SOCDNX_INIT_FUNC_DEFS;

	
	compensation_size *= -1;

	rv = jer_pp_mtr_policer_hdr_compensation_verify(unit, compensation_size);
	SOCDNX_IF_ERR_EXIT(rv);

	
	rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_get(
		unit, 
		core_id, 
		0, 
		0 , 
		0 , 
		&default_profile_compensation
		);
	SOCDNX_IF_ERR_EXIT(rv);

	
	rv = jer_pp_metering_in_pp_port_map_get(
		unit, 
		core_id, 
		0, 
		pp_port, 
		&previous_profile);
	SOCDNX_IF_ERR_EXIT(rv);

	
	rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_get(
		unit, 
		core_id, 
		0, 
		0 , 
		previous_profile, 
		&previous_profile_compensation
		);
	SOCDNX_IF_ERR_EXIT(rv);

	
	if (compensation_size == previous_profile_compensation - default_profile_compensation) {
		SOC_EXIT;
	}

	
	rv = arad_sw_db_multiset_remove_by_index(
		unit,
		core_id,
        JER_PP_SW_DB_MULTI_SET_POLICER_SIZE_PROFILE,
        previous_profile,
        &packet_size_profile_multiset_last_appear
		);
	SOCDNX_SAND_IF_ERR_EXIT(rv);

	
	profile_tbl_data.in_pp_port_size_delta = compensation_size;
	rv  = jer_pp_metering_packet_size_profile_key_get(unit, profile_tbl_data, &packet_size_profile_multiset_key);
	SOCDNX_IF_ERR_EXIT(rv);

	rv = arad_sw_db_multiset_add(
		unit,
		core_id,
		JER_PP_SW_DB_MULTI_SET_POLICER_SIZE_PROFILE,
		&packet_size_profile_multiset_key,
        &packet_size_profile_multiset_ndx,
        &packet_size_profile_multiset_first_appear,
        &packet_size_profile_multiset_success
		);
	SOCDNX_SAND_IF_ERR_EXIT(rv);

	
	if (packet_size_profile_multiset_success != SOC_SAND_SUCCESS) {

		
		profile_tbl_data.in_pp_port_size_delta = previous_profile_compensation - default_profile_compensation;
		rv  = jer_pp_metering_packet_size_profile_key_get(unit, profile_tbl_data, &packet_size_profile_multiset_key);
		SOCDNX_IF_ERR_EXIT(rv);

		rv = arad_sw_db_multiset_add_by_index(
			unit, 
			core_id, 
			JER_PP_SW_DB_MULTI_SET_POLICER_SIZE_PROFILE, 
			&packet_size_profile_multiset_key,
			previous_profile,
			&packet_size_profile_multiset_first_appear,
			&packet_size_profile_multiset_success 
			);
		SOCDNX_SAND_IF_ERR_EXIT(rv);
		
		if (packet_size_profile_multiset_success == SOC_SAND_SUCCESS){
			SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Exceeded the maximum number of metering compensation profiles.\n"
																  "Reverted to previous profile.")));
		}
		else {
			SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Exceeded the maximum number of metering compensation profiles.\n"
																  "Failed to revert to previous profile.")));
		}
	}

	
	for (meter_group = 0; meter_group < 2; meter_group++) {

		
		if (packet_size_profile_multiset_first_appear) {
			rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_set(
				unit, 
				core_id, 
				meter_group, 
				0 , 
				packet_size_profile_multiset_ndx, 
				compensation_size + default_profile_compensation
				);
			SOCDNX_IF_ERR_EXIT(rv);
		}

		
		rv = jer_pp_metering_in_pp_port_map_set(
			unit, 
			core_id, 
			meter_group, 
			pp_port, 
			packet_size_profile_multiset_ndx
			);
		SOCDNX_IF_ERR_EXIT(rv);

		
		if (packet_size_profile_multiset_last_appear && (previous_profile != packet_size_profile_multiset_ndx)) {
			rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_set(
				unit, 
				core_id, 
				meter_group, 
				0 , 
				previous_profile, 
				default_profile_compensation
				);
			SOCDNX_IF_ERR_EXIT(rv);
		}
	}

exit:
	SOCDNX_FUNC_RETURN;
}

uint32
jer_pp_mtr_policer_hdr_compensation_get(
    int                         unit,
	int                         core_id,
	uint32                      pp_port,
	int                         *compensation_size
)
{
uint32
	rv, 
	pp_port_prf;
int
	port_compensation,
	default_compensation;

	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(compensation_size);

	
	rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_get(unit, 0, 0, 0, 0, &default_compensation);
	SOCDNX_IF_ERR_EXIT(rv);

	
	rv = jer_pp_metering_in_pp_port_map_get(unit, core_id, 0, pp_port, &pp_port_prf);
	SOCDNX_IF_ERR_EXIT(rv);

	
	rv = jer_pp_metering_in_pp_port_map_and_mtr_prf_map_get(unit, core_id, 0, 0, pp_port_prf, &port_compensation);
	SOCDNX_IF_ERR_EXIT(rv);

	
	*compensation_size = -1 * (port_compensation - default_compensation);

exit:
	SOCDNX_FUNC_RETURN;
}


#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
