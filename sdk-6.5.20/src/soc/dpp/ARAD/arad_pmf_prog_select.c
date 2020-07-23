/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FP



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/dcmn/error.h>
#include <soc/mem.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <soc/dpp/ARAD/arad_pmf_prog_select.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_pgm.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/PPC/ppc_api_general.h>








typedef enum
{
    
    ARAD_PMF_SEL_LINE_REMOVE = 0x1,
    
    ARAD_PMF_SEL_LINE_NEW = 0x2,
    ARAD_PMF_SEL_LINE_VALID = 0x4,
    ARAD_PMF_SEL_LINE_WITH_ID = 0x8
} ARAD_PMF_SEL_LINE_ACTION;

#ifdef ARAD_PMF_OAM_MIRROR_WA_ENABLED
    #define ARAD_PMF_OAM_MIRROR_WA_NOF_STATIC_PROG_SEL_LINES(unit) \
        (SOC_DPP_CONFIG(unit)->pp.oam_enable ? \
        (SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM + (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0) > 0)) : 0)
#else
    #define ARAD_PMF_OAM_MIRROR_WA_NOF_STATIC_PROG_SEL_LINES(unit) (0)
#endif 


#define ARAD_PMF_PROG_SELECT_FIRST_LINE(unit, stage, is_tm, init_info) \
    (is_tm ? (ARAD_PMF_NOF_LINES - init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM]): \
    ((stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) ? 0 : ARAD_PMF_OAM_MIRROR_WA_NOF_STATIC_PROG_SEL_LINES(unit))) 













typedef enum
{
  ARAD_PMF_SEL_GROUP_INTERSECT,
  ARAD_PMF_SEL_GROUP_CONTAIN,
  ARAD_PMF_SEL_GROUP_CONTAINED,
  ARAD_PMF_SEL_GROUP_EQUAL,
  ARAD_PMF_SEL_GROUP_DISJOINT
} ARAD_PMF_SEL_GROUP_RELATION;


typedef struct
{
  
  uint32 start_index;
  
  uint8 low_to_high;

} ARAD_PMF_HW_LOAD_INFO;










#if ARAD_DEBUG_IS_LVL1
void ARAD_PMF_SEL_GROUP_print(
    SOC_SAND_IN ARAD_PMF_SEL_GROUP *group
  );

#endif 
uint32
  arad_pmf_psl_slb_field_update(
      SOC_SAND_IN    int                    unit,
      SOC_SAND_INOUT SOC_PPC_FP_QUAL_TYPE   *type,
      SOC_SAND_IN    ARAD_PMF_PSL           *psl,
      SOC_SAND_IN    SOC_PPC_FP_QUAL_VAL    *qual_val
  );

uint32
  ARAD_PMF_SEL_GROUP_clear(
    SOC_SAND_OUT ARAD_PMF_SEL_GROUP *in_group
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(in_group);
    sal_memset(in_group,0x0, sizeof(ARAD_PMF_SEL_GROUP));
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_clone()",0,0);
}

uint32
  ARAD_PMF_LINE_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_PSL_LINE_INFO *line
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(line);
    sal_memset(line,0x0, sizeof(ARAD_PMF_PSL_LINE_INFO));

    ARAD_PMF_SEL_GROUP_clear(&line->group);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_clone()",0,0);
}


 
uint32 
  ARAD_PMF_SEL_GROUP_to_line(
      SOC_SAND_IN  int                         unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE         stage,
      SOC_SAND_IN  uint32                         pfg_id,
      SOC_SAND_IN  uint32                         flags,
      SOC_SAND_IN  ARAD_PMF_SEL_GROUP             *group,
      SOC_SAND_OUT  ARAD_PMF_PSL_LINE_INFO  *line
  )
{
    ARAD_PMF_SEL_INIT_INFO  
        init_info;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0;
    soc_error_t
        rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memcpy(&line->group,group, sizeof(ARAD_PMF_SEL_GROUP));
    line->flags = 0;
    line->line_id = pfg_id;
    sal_memset(line->groups, 0x0, sizeof(uint32) * ARAD_PMF_GROUP_LEN);
    soc_sand_bitstream_set_bit(line->groups, pfg_id);

    
    rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.init_info.get(unit, stage, &init_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    line->prog_id = init_info.pmf_pgm_default[is_tm];

    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_clone()",0,0);
}



uint32 
  arad_pmf_sel_prog_alloc(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint8                    is_for_tm, 
      SOC_SAND_OUT   uint32              *alloced_prog_id,
      SOC_SAND_OUT   uint8               *found
  )
{
    uint32
        pmf_pgm_ndx_min,
        pmf_pgm_ndx_max,
      program_rsrc = 0,
      prog_id=0;
    uint32
      res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = 0;

    res = arad_pmf_prog_select_pmf_pgm_borders_get(
              unit,
              stage,
              is_for_tm, 
              &pmf_pgm_ndx_min,
              &pmf_pgm_ndx_max
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.get(unit, stage, 0, &program_rsrc);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    for(prog_id = pmf_pgm_ndx_min; prog_id < pmf_pgm_ndx_max; ++prog_id) {
        if(!SHR_BITGET(&program_rsrc,prog_id)){
            *alloced_prog_id = prog_id;
            *found = 1;
            break;
        }
    }

    if(*found) {
        SHR_BITSET(&program_rsrc,prog_id);

        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.set(unit, stage, 0, program_rsrc);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_prog_alloc()", prog_id, stage);
}


uint32 
  arad_pmf_sel_prog_free(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                       is_for_tm,
      SOC_SAND_IN   uint32              prog_id,
      SOC_SAND_IN  uint32                   pmf_pgm_to
  )
{
    uint32
        is_tm = is_for_tm,
      program_rsrc = 0,
        pmf_pgm_clean,
      res;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.get(unit, stage, 0, &program_rsrc);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    SHR_BITCLR(&program_rsrc,prog_id);
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.set(unit, stage, 0, program_rsrc);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_db_pmb.bit_range_clear(unit, stage, prog_id, 0, ARAD_PMF_NOF_DBS);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.init_info.get(unit, stage, &init_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    if (init_info.pmf_pgm_default[is_tm] == prog_id) {
        init_info.pmf_pgm_default[is_tm] = pmf_pgm_to;
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.init_info.set(unit, stage, &init_info);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
    }

    
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
        pmf_pgm_clean = ARAD_PMF_PROGRAM_STATIC_INDEX_GET(init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM], 
                                              ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN, 
                                              ARAD_PMF_PGM_TYPE_RAW);
    }
    else {
        pmf_pgm_clean = ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX;
    }
    res = arad_pmf_psl_pmf_pgm_duplicate(
            unit, 
            stage,               
            pmf_pgm_clean, 
            prog_id, 
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

    
    res = arad_pp_fp_fem_pgm_id_remove(unit,stage,prog_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 96, exit);
    


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_prog_free()", prog_id, 0);
}

STATIC
soc_mem_t 
  arad_pmf_sel_line_hw_table_get(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage
  )
{
    soc_mem_t
        pmf_sel_line_hw_table;

    switch (stage) {
    case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
        pmf_sel_line_hw_table = EGQ_PMF_PROGRAM_SELECTION_CAMm;
        break;
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
        if(SOC_IS_JERICHO(unit)) {
            pmf_sel_line_hw_table = IHP_FLP_PROGRAM_SELECTION_CAMm;
        } else {
            pmf_sel_line_hw_table = IHB_FLP_PROGRAM_SELECTION_CAMm;
        }
        break;
#ifdef BCM_88660_A0
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
        pmf_sel_line_hw_table = IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm;
        break;
#endif 
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
        pmf_sel_line_hw_table = IHP_VTT_1ST_KEY_PROG_SEL_TCAMm;
        break;
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
        pmf_sel_line_hw_table = IHP_VTT_2ND_KEY_PROG_SEL_TCAMm;
        break;
    default:
    case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
        pmf_sel_line_hw_table = IHB_PMF_PROGRAM_SELECTION_CAMm;
        break;
    }
    return pmf_sel_line_hw_table;
}


uint32 
  arad_pmf_sel_line_hw_write(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 entry_offset,
      SOC_SAND_IN   ARAD_PMF_PSL_LINE_INFO      *line,
      SOC_SAND_IN   uint32                prog,
      SOC_SAND_IN   uint8               valid
  )
{
    uint32
      data[SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)];
    uint32
      res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_CLEAR(data, uint32, SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS));

    

    
    SHR_BITCOPY_RANGE(data,0,line->group.value,0,ARAD_PMF_SEL_HW_DATA_NOF_BITS);
    
    SHR_BITCOPY_RANGE(data,ARAD_PMF_SEL_HW_DATA_NOF_BITS,line->group.mask,0,ARAD_PMF_SEL_HW_DATA_NOF_BITS);

    
    if((SOC_IS_JERICHO_PLUS(unit)) &&
           ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) ||
            (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT)  ||
            (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)  ||
            (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) ))
    {
        soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, PROGRAM_DATAf, prog);
    } else {
        soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, PROGRAMf, prog);
    }
    soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, VALIDf, valid);


    
    res = soc_mem_write(
            unit,
            arad_pmf_sel_line_hw_table_get(unit, stage),
            MEM_BLOCK_ANY,
            entry_offset,
            data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    {
        char
            string_to_print[ARAD_PMF_SEL_LINE_NOF_BITS + 100];
        uint32
            bit_indx,
            mask_val,
            val;
        sal_sprintf(string_to_print, "     ");
        sal_sprintf(string_to_print + sal_strlen(string_to_print), "Program selection: write to stage %s, line %d, ", SOC_PPC_FP_DATABASE_STAGE_to_string(stage), entry_offset);
    if(valid) 
    {
            sal_sprintf(string_to_print + sal_strlen(string_to_print), "  program: %d, ", prog);
    }
    else
    {
            sal_sprintf(string_to_print + sal_strlen(string_to_print), "  program: invalid, ");
    }
    

        for(bit_indx = 0; bit_indx < ARAD_PMF_SEL_LINE_NOF_BITS; ++bit_indx) {
            mask_val = soc_sand_bitstream_test_bit(line->group.mask, bit_indx);
            if(mask_val == 1) 
            {
                sal_sprintf(string_to_print + sal_strlen(string_to_print), "*");
            }
            else
            {
                val = soc_sand_bitstream_test_bit(line->group.value, bit_indx);
                sal_sprintf(string_to_print + sal_strlen(string_to_print), "%d", val);
            }
            if((bit_indx+1)%32 == 0) 
            {
                sal_sprintf(string_to_print + sal_strlen(string_to_print), " ");
            }
        }
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "%s\n\r"), string_to_print));
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_line_hw_write()", entry_offset, stage);
}



uint32 
  arad_pmf_sel_line_valid_set(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 entry_offset,
      SOC_SAND_IN   uint8               is_valid,
      SOC_SAND_IN   uint32               valid_or_program
  )
{
    uint32
      data[SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)];
    uint32
      res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_CLEAR(data, uint32, SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS));

    res = soc_mem_read(
            unit,
            arad_pmf_sel_line_hw_table_get(unit, stage),
            MEM_BLOCK_ANY,
            entry_offset,
            data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (is_valid) {
        soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, VALIDf, valid_or_program);
    }
    else {
        if((SOC_IS_JERICHO_PLUS(unit)) &&
                ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) ||
                 (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT)  ||
                 (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)  ||
                 (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) ))
        {
            soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, PROGRAM_DATAf, valid_or_program);
        } else {
            soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), data, PROGRAMf, valid_or_program);
        }
    }

    
    res = soc_mem_write(
            unit,
            arad_pmf_sel_line_hw_table_get(unit, stage),
            MEM_BLOCK_ANY,
            entry_offset,
            data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_line_valid_set()", entry_offset, stage);
}


uint32
  arad_pmf_psl_to_group_map(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  ARAD_PMF_PSL                    *tbl_data,
    SOC_SAND_OUT  ARAD_PMF_SEL_GROUP                 *group
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  ARAD_PMF_SEL_GROUP_clear(group);

  ARAD_CLEAR(data, uint32, SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS));

  switch (stage) {
  case SOC_PPC_FP_DATABASE_STAGE_EGRESS:
      
      res = arad_pp_egq_pmf_program_selection_cam_tbl_data_set_unsafe(
                unit,
                &(tbl_data->egress_pmf),
                data
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      break;
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
      
      if (!SOC_IS_ARDON(unit)) {
          res = arad_pp_ihb_pmf_program_selection_cam_tbl_data_set_unsafe(
                    unit,
                    &(tbl_data->ingress_pmf),
                    data
                  );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
      break;
#ifdef BCM_88660_A0
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
      if (SOC_IS_ARADPLUS(unit)) {
          
          if (!SOC_IS_ARDON(unit)) {
              res = arad_pp_ihb_slb_program_selection_cam_tbl_data_set_unsafe(
                        unit,
                        &(tbl_data->ingress_slb),
                        data
                      );
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          }
          break;
      }
#endif 
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
      
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 30, exit);
  }


  

  
  SHR_BITCOPY_RANGE(group->value,0,data,0,ARAD_PMF_SEL_HW_DATA_NOF_BITS);
  
  SHR_BITCOPY_RANGE(group->mask,0,data,ARAD_PMF_SEL_HW_DATA_NOF_BITS, ARAD_PMF_SEL_HW_DATA_NOF_BITS);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_to_group_map()", stage, 0);
}



uint32 
  arad_pmf_sel_calc_program(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN   ARAD_PMF_PSL_LINE_INFO      *line,
      SOC_SAND_OUT   uint32               *prog
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *prog = line->prog_id;

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_calc_program()", 0, 0);
}



STATIC uint8
  arad_pmf_bitstream_equal(
    SOC_SAND_IN    uint32       *bit_stream1,
    SOC_SAND_IN       uint32    *bit_stream2,
    SOC_SAND_IN       uint32    size 
  )
{
  uint32 i;
  
  for(i=0; i<size; ++i)
  {
    if(bit_stream1[i] != bit_stream2[i])
    {
        return FALSE;
    }
  }
  return TRUE;
}


uint32
  ARAD_PMF_SEL_GROUP_clone(
    SOC_SAND_IN ARAD_PMF_SEL_GROUP *in_group,
    SOC_SAND_OUT ARAD_PMF_SEL_GROUP *out_group
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(in_group);
    sal_memcpy(out_group,in_group, sizeof(ARAD_PMF_SEL_GROUP));
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_clone()",0,0);

}



uint8
  arad_pmf_sel_group_is_disjoint(
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group1,
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group2
  )
{
    ARAD_PMF_SEL_GROUP
        value_diff;

    
    ARAD_PMF_SEL_GROUP_clone(group1, &value_diff);
    
    soc_sand_bitstream_or(value_diff.mask, group2->mask, ARAD_PMF_SEL_LINE_LEN);
    
    soc_sand_bitstream_not(value_diff.mask, ARAD_PMF_SEL_LINE_LEN);
    
    soc_sand_bitstream_xor(value_diff.value, group2->value, ARAD_PMF_SEL_LINE_LEN);
    
    soc_sand_bitstream_and(value_diff.value, value_diff.mask, ARAD_PMF_SEL_LINE_LEN);
    
    if(soc_sand_bitstream_have_one(value_diff.value,ARAD_PMF_SEL_LINE_LEN))
    {
        return TRUE;
    }
    return FALSE;
}




ARAD_PMF_SEL_GROUP_RELATION
  arad_pmf_sel_group_rel_check(
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group1,
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group2
  )
{
    ARAD_PMF_SEL_GROUP
        or_group;
    uint8
        gr1_contain = FALSE,
        gr2_contain = FALSE;
    
    
    if(arad_pmf_sel_group_is_disjoint(group1, group2))
    {
        return ARAD_PMF_SEL_GROUP_DISJOINT;
    }

    
    ARAD_PMF_SEL_GROUP_clone(group1, &or_group);

    
    soc_sand_bitstream_or(or_group.mask, group2->mask, ARAD_PMF_SEL_LINE_LEN);

    
    if(arad_pmf_bitstream_equal(or_group.mask, group1->mask, ARAD_PMF_SEL_LINE_LEN))
    {
        gr1_contain = TRUE;
    }

    if(arad_pmf_bitstream_equal(or_group.mask, group2->mask, ARAD_PMF_SEL_LINE_LEN))
    {
        gr2_contain = TRUE;
    }

    if(gr1_contain && gr2_contain) 
    {
        return ARAD_PMF_SEL_GROUP_EQUAL;
    }
    if(gr1_contain) 
    {
        return ARAD_PMF_SEL_GROUP_CONTAIN;
    }
    if(gr2_contain) 
    {
        return ARAD_PMF_SEL_GROUP_CONTAINED;
    }

    return ARAD_PMF_SEL_GROUP_INTERSECT;
}

uint32
  ARAD_PMF_SEL_GROUP_build(
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group1,
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group2,
    SOC_SAND_OUT  ARAD_PMF_SEL_GROUP *res_group
  )
{
    
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(group1);
    SOC_SAND_CHECK_NULL_INPUT(group2);
    SOC_SAND_CHECK_NULL_INPUT(res_group);


    ARAD_PMF_SEL_GROUP_clone(group1, res_group);
    
    soc_sand_bitstream_and(res_group->mask, group2->mask, ARAD_PMF_SEL_LINE_LEN);

    
    soc_sand_bitstream_or(res_group->value, group2->value, ARAD_PMF_SEL_LINE_LEN);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_build()",0,0);
}


uint32
  ARAD_PMF_SEL_GROUP_intersection(
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group1,
    SOC_SAND_IN  ARAD_PMF_SEL_GROUP *group2,
    SOC_SAND_OUT  ARAD_PMF_SEL_GROUP *res_group,
    SOC_SAND_OUT  ARAD_PMF_SEL_GROUP_RELATION    *rel
  )
{
    uint32
        res;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(group1);
    SOC_SAND_CHECK_NULL_INPUT(group2);
    SOC_SAND_CHECK_NULL_INPUT(res_group);


    *rel = arad_pmf_sel_group_rel_check(group1,group2);
    
    if(*rel == ARAD_PMF_SEL_GROUP_DISJOINT)
    {
        goto exit;
    }

    res = ARAD_PMF_SEL_GROUP_build(group1, group2, res_group);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_intersection()",0,0);
}

 
uint32
  arad_pmf_line_intersection(
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE        stage,
    SOC_SAND_IN  ARAD_PMF_PSL_LINE_INFO *line1,
    SOC_SAND_IN  ARAD_PMF_PSL_LINE_INFO *line2,
    SOC_SAND_OUT  ARAD_PMF_PSL_LINE_INFO *res_line,
    SOC_SAND_OUT  uint8          *empty
  )
{
    ARAD_PMF_SEL_GROUP_RELATION
        rel;
    uint32
        res = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(line1);
    SOC_SAND_CHECK_NULL_INPUT(line2);
    SOC_SAND_CHECK_NULL_INPUT(res_line);

    ARAD_PMF_LINE_INFO_clear(res_line);

    
    res = ARAD_PMF_SEL_GROUP_intersection(&(line1->group), &(line2->group), &(res_line->group), &rel);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    *empty = (rel == ARAD_PMF_SEL_GROUP_DISJOINT);
    
    if(!*empty) 
    {
        
        sal_memcpy(res_line->groups, line1->groups,sizeof(line1->groups));
        soc_sand_bitstream_or(res_line->groups, line2->groups, ARAD_PMF_GROUP_LEN);
    }

    
    res_line->prog_id = line2->prog_id;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_line_intersection()",0,0);
}




uint32 
  arad_pmf_is_line_covered_at_level(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_PMF_PSL_LINE_INFO       *line,
      SOC_SAND_IN  ARAD_PMF_PSL_LEVEL_INFO            *level_info,
      SOC_SAND_OUT uint8                *covered
  )
{
    uint32
      indx;
    ARAD_PMF_SEL_GROUP_RELATION
        rel;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for(indx = 0; indx < level_info->nof_lines; ++indx) 
    {
        rel = arad_pmf_sel_group_rel_check(&(line->group),&(level_info->lines[indx].group));
        if(rel == ARAD_PMF_SEL_GROUP_EQUAL || rel == ARAD_PMF_SEL_GROUP_CONTAINED)
        {

            *covered = TRUE;
            goto exit;
        }
    }

    *covered = FALSE;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_is_line_covered_at_level()",0,0);
}


uint32 
  arad_pmf_check_group_on_upper_levels(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN     ARAD_PMF_PSL_LINE_INFO    *group_line,  
      SOC_SAND_IN     uint32              start_level,
      SOC_SAND_IN  uint32                       flags,
      SOC_SAND_OUT    uint8             *covered
      
  )
{
    uint32
      level_indx;
    ARAD_PMF_PSL_LEVEL_INFO
        cur_level;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for(level_indx = start_level; level_indx < ARAD_PMF_NOF_LEVELS; ++level_indx) 
    {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, level_indx, &cur_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        res = arad_pmf_is_line_covered_at_level(unit,group_line,&cur_level,covered);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        if(*covered) 
        {
            goto exit;
        }
    }

    *covered = FALSE;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_check_group_on_upper_levels()", stage, 0);
}



uint32 
  arad_pmf_add_line_at_level(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                            flags,
      SOC_SAND_IN  ARAD_PMF_PSL_LINE_INFO      *line,
      SOC_SAND_INOUT  ARAD_PMF_PSL_LEVEL_INFO        *level_info
  )
{
    uint32
      indx;
    ARAD_PMF_SEL_GROUP_RELATION
        rel;
    uint8
        covered;
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if ( flags & ARAD_PMF_SEL_ALLOC_ADVANCED_MODE ) {
        sal_memcpy(&level_info->lines[line->line_id], line, sizeof(ARAD_PMF_PSL_LINE_INFO));
        level_info->lines[line->line_id].flags |= ARAD_PMF_SEL_LINE_VALID;
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    if (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)) {
        res = arad_pmf_check_group_on_upper_levels(unit, stage, line,level_info->level_index+1, flags, &covered);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        
        if(covered)
        {
            goto exit;
        }
    }


    for(indx = 0; indx < level_info->nof_lines; ++indx) 
    {
        
        if (flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE) {
            break;
        }

        rel = arad_pmf_sel_group_rel_check(&(line->group),&(level_info->lines[indx].group));
        if(rel == ARAD_PMF_SEL_GROUP_CONTAINED)
        {
            
            goto exit;
        }
        if(rel == ARAD_PMF_SEL_GROUP_EQUAL)
        {
            level_info->lines[indx].flags |= ARAD_PMF_SEL_LINE_REMOVE;
            level_info->nof_removed_lines++;
            
            goto exit;
        }
        if((rel == ARAD_PMF_SEL_GROUP_DISJOINT) || (rel == ARAD_PMF_SEL_GROUP_INTERSECT))
        {
            continue;
        }

        if(rel == ARAD_PMF_SEL_GROUP_CONTAIN)
        {
            level_info->lines[indx].flags |= ARAD_PMF_SEL_LINE_REMOVE;
            level_info->nof_removed_lines++;
        }
    }

    if(!(flags & ARAD_PMF_SEL_ALLOC_DO_NOT_ADD)) {
        
        sal_memcpy(&level_info->lines[level_info->nof_lines + level_info->nof_new_lines], line, sizeof(ARAD_PMF_PSL_LINE_INFO));
        level_info->lines[level_info->nof_lines + level_info->nof_new_lines].flags |= ARAD_PMF_SEL_LINE_VALID;
        level_info->nof_new_lines++;
    }

    
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_add_line_at_level()", stage, 0);
}


uint32 
  arad_pmf_level_transfer_lines(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_PMF_PSL_LEVEL_INFO      *from_level,
      SOC_SAND_INOUT  ARAD_PMF_PSL_LEVEL_INFO   *to_level,  
      SOC_SAND_OUT    uint8             *empty
      
  )
{
    uint32
      indx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *empty = TRUE;

    for(indx = 0; indx < from_level->nof_lines; ++indx) 
    {
        if( from_level->lines[indx].flags & ARAD_PMF_SEL_LINE_REMOVE)
        {
            *empty = FALSE;
            sal_memcpy(&to_level->lines[to_level->nof_lines + to_level->nof_new_lines],
                       &from_level->lines[indx],
                       sizeof(ARAD_PMF_PSL_LINE_INFO));
            to_level->nof_new_lines++;
        }
    }

    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_level_transfer_lines()",0,0);
}


uint32 
  arad_pmf_add_group_at_level(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                            pfg_id,
      SOC_SAND_IN  uint32                            flags,
      SOC_SAND_IN     ARAD_PMF_PSL_LINE_INFO    *group_line,  
      SOC_SAND_INOUT  ARAD_PMF_PSL_LEVEL_INFO         *prev_level,
      SOC_SAND_INOUT  ARAD_PMF_PSL_LEVEL_INFO         *next_level,  
      SOC_SAND_OUT    uint8                           *intersected
  )
{
    uint32
      local_flags,
      curr_level_line_indx,
      prev_level_line_indx;
    ARAD_PMF_PSL_LINE_INFO       
        cur_line;
    uint8
        is_first_pfg,
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        duplicate_found,
        is_empty;
    uint32
        pfg_idx,
        groups_curr[ARAD_PMF_GROUP_LEN],
        res;
    SOC_PPC_PMF_PFG_INFO      
        pfg_info;
    ARAD_PMF_PSL         
        psl;
    ARAD_PMF_SEL_GROUP
        res_group,
        pmf_group;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *intersected = FALSE;

    if (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)) {
        
        for(prev_level_line_indx = 0; prev_level_line_indx < prev_level->nof_lines; ++prev_level_line_indx) 
        {
            res = arad_pmf_line_intersection(stage, group_line, &prev_level->lines[prev_level_line_indx], &cur_line, &is_empty);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            if(is_empty) 
            {
                continue;
            }
            *intersected = TRUE;
            res = arad_pmf_add_line_at_level(unit, stage, flags, &cur_line, next_level);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
        
        if ((!prev_level->nof_lines) && next_level->nof_lines) {
            local_flags = flags | ARAD_PMF_SEL_ALLOC_DO_NOT_ADD;
            res = arad_pmf_add_line_at_level(unit, stage, local_flags, group_line, next_level);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
    }
    else {
        
        for(curr_level_line_indx = 0; curr_level_line_indx < next_level->nof_lines; ++curr_level_line_indx) 
        {
            sal_memcpy(groups_curr, next_level->lines[curr_level_line_indx].groups, sizeof(groups_curr));

            if(!soc_sand_bitstream_test_bit(groups_curr, pfg_id)) 
            {
                continue;
            }
            
            soc_sand_bitstream_test_and_reset_bit(groups_curr, pfg_id);

            
            duplicate_found = FALSE;
            for(prev_level_line_indx = 0; prev_level_line_indx < prev_level->nof_lines; ++prev_level_line_indx) 
            {
                if(sal_memcmp(groups_curr, prev_level->lines[prev_level_line_indx].groups, sizeof(groups_curr)) == 0) 
                {
                    
                    duplicate_found = TRUE;
                    break;
                }
            }

            
            if (!duplicate_found) {
                
                sal_memcpy(&cur_line, &(next_level->lines[curr_level_line_indx]), sizeof(cur_line));
                soc_sand_bitstream_test_and_reset_bit(cur_line.groups, pfg_id);

                
                cur_line.flags = ARAD_PMF_SEL_LINE_NEW;
                ARAD_PMF_SEL_GROUP_clear(&(cur_line.group));
                is_first_pfg = TRUE;
                for (pfg_idx = 0; pfg_idx < SOC_PPC_FP_NOF_PFGS_ARAD; pfg_idx++) {
                    if(!soc_sand_bitstream_test_bit(groups_curr, pfg_idx)) 
                    {
                        continue;
                    }

                    
                    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.pfg_info.get(unit, stage, pfg_idx, &pfg_info);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);
                    if (!pfg_info.is_array_qualifier) {
                        LOG_ERROR(BSL_LS_SOC_FP,
                                  (BSL_META_U(unit,
                                              "Unit %d Stage %s Pfg index %d - pre-selector not found.\n\r"),
                                   unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), pfg_id));
                        
                        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 40, exit);
                    }
                    
                    ARAD_PMF_PSL_clear(unit, stage, &psl, 1+is_tm, TRUE);
                    res = arad_pmf_psl_map(unit, &pfg_info, is_tm, &psl);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                    
                    res = arad_pmf_psl_to_group_map(unit, stage, &psl, &pmf_group);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

                    
                    if(is_first_pfg)
                    {
                        sal_memcpy(&(cur_line.group), &pmf_group, sizeof(ARAD_PMF_SEL_GROUP));
                        is_first_pfg = FALSE;
                    }
                    else {
                        
                        res = ARAD_PMF_SEL_GROUP_build(&(cur_line.group), &pmf_group, &res_group);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
                        
                        sal_memcpy(&(cur_line.group), &res_group, sizeof(ARAD_PMF_SEL_GROUP));
                    }
                }

                
                res = arad_pmf_add_line_at_level(unit, stage, flags, &cur_line, prev_level);
                SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
            }

            
            next_level->lines[curr_level_line_indx].flags |= ARAD_PMF_SEL_LINE_REMOVE;
            next_level->nof_removed_lines++;
        }

    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_add_group_at_level()",0,0);
}





uint32 
  arad_pmf_table_sort(
      SOC_SAND_IN  int                    unit,  
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                       flags
  )
{
    int32
        level_indx,
        line_indx;
    ARAD_PMF_PSL_LEVEL_INFO
        *curr_level = NULL;
    uint32
        entry_indx;
    uint8
        line_remove_to_copy,
        line_remove_to_copy_sorted,
        first_in_level = TRUE;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        res;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.init_info.get(
            unit,
             stage, 
            &init_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    entry_indx = ARAD_PMF_PROG_SELECT_FIRST_LINE(unit, stage, is_tm, init_info);

    ARAD_ALLOC(curr_level, ARAD_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_table_sort.curr_level");

    
    for(level_indx = ARAD_PMF_NOF_LEVELS - 1; level_indx >= 0; --level_indx) 
    {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, level_indx, curr_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        
        first_in_level = TRUE;
        
        
        
        for(line_remove_to_copy = 0 ; line_remove_to_copy <= 1 ; line_remove_to_copy++) {
            
            line_remove_to_copy_sorted = (flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)? line_remove_to_copy: (!line_remove_to_copy);
            for(line_indx = curr_level->nof_lines-1 ; line_indx >= 0 ; --line_indx) 
            {
                
                if (((line_remove_to_copy_sorted == 0) && (!(curr_level->lines[line_indx].flags & ARAD_PMF_SEL_LINE_REMOVE))) 
                    || ((line_remove_to_copy_sorted == 1) && (curr_level->lines[line_indx].flags & ARAD_PMF_SEL_LINE_REMOVE))) 
                {
                    continue;
                }

                if(first_in_level) {
                    first_in_level = FALSE;
                    curr_level->last_index =  entry_indx;
                }

                
                res = arad_pmf_sel_line_hw_write(
                        unit,
                        stage,
                        entry_indx++,
                        &curr_level->lines[line_indx],
                        curr_level->lines[line_indx].prog_id,
                        TRUE
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            }
        }

        
        if(!first_in_level) 
        {
            curr_level->first_index = entry_indx - 1;
            
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.set(unit, stage, is_tm, level_indx, curr_level);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);
        }
    }

exit:
    ARAD_FREE(curr_level);
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_table_sort()", stage,0);
}


uint32
  arad_pmf_prog_select_verify_space(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE    stage,
      SOC_SAND_IN  uint32                   flags,
      SOC_SAND_IN  int32                     total_in_use,
      SOC_SAND_OUT  uint8                    *success
  )
{
    ARAD_PMF_SEL_INIT_INFO  
        init_info;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        available_entries,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.init_info.get(
            unit,
             stage, 
            &init_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if (is_tm) {    
        available_entries = init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM];
    }
    else {
        available_entries = ARAD_PMF_NOF_LINES - init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_ETH] - 1;
        
        if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
            available_entries -= ARAD_PMF_OAM_MIRROR_WA_NOF_STATIC_PROG_SEL_LINES(unit);
        }
    }

    *success = total_in_use <= available_entries;
    if ((*success) == 0) {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "    ")));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "No place in program selection at stage %s,: \n\r"), SOC_PPC_FP_DATABASE_STAGE_to_string(stage)));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "    ")));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Available entries: %d, due to the %d reserved lines is less than "), available_entries, init_info.nof_reserved_lines[0]));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "the needed entries: %d"), total_in_use));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "\n\r")));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_verify_space()", 0, 0);
}


uint32 
  arad_pmf_psl_hw_commit(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                       flags
  )
{
    int32
        first_line,
        level_indx,
        entry_indx,
        line_indx;
    ARAD_PMF_PSL_LEVEL_INFO
        *curr_level = NULL,
        *updated_level_info = NULL;
    uint32
        new_prog,
        update_level_entry_index=0;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;
    uint8
        first_in_level = TRUE,
        had_entries = FALSE,
        update_level_info = FALSE;
    uint32
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.init_info.get(
            unit,
             stage, 
            &init_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = arad_pmf_table_sort(unit, stage, flags);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    entry_indx = ARAD_PMF_NOF_LINES  - 1
        - (is_tm? ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN: init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_ETH]);

    ARAD_ALLOC(curr_level, ARAD_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_psl_hw_commit.curr_level");
    ARAD_ALLOC(updated_level_info, ARAD_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_psl_hw_commit.updated_level_info");
    
    for(level_indx = 0; level_indx < ARAD_PMF_NOF_LEVELS; ++level_indx) 
    {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, level_indx, curr_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        updated_level_info->level_index = curr_level->level_index;

        
        update_level_entry_index = 0;
        first_in_level = TRUE;
        had_entries = FALSE;
        update_level_info = FALSE;
        
        
        
        for(line_indx = 0 ; line_indx < curr_level->nof_lines; ++line_indx) 
        {
            had_entries = TRUE;
            
            if (curr_level->lines[line_indx].flags & ARAD_PMF_SEL_LINE_REMOVE) 
            {
                continue;
            }

            sal_memcpy(&updated_level_info->lines[update_level_entry_index],&curr_level->lines[line_indx], sizeof(ARAD_PMF_PSL_LINE_INFO));

            if(first_in_level) {
                first_in_level = FALSE;
                updated_level_info->first_index =  entry_indx;
            }

            
            res = arad_pmf_sel_line_hw_write(
                    unit,
                    stage,
                    entry_indx--,
                    &curr_level->lines[line_indx],
                    curr_level->lines[line_indx].prog_id,
                    TRUE
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            ++update_level_entry_index;
        }
        
        
        for(line_indx = curr_level->nof_lines ; line_indx < curr_level->nof_lines + curr_level->nof_new_lines; ++line_indx) 
        {
            
            if (curr_level->lines[line_indx].flags & ARAD_PMF_SEL_LINE_REMOVE) 
            {
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit %d Level index %d Line index %d : Line is for remove.\n\r"),
                           unit, level_indx, line_indx));
                SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 40, exit);
            }

           res = arad_pmf_sel_calc_program(unit,&curr_level->lines[line_indx],&new_prog);
           SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

           sal_memcpy(&updated_level_info->lines[update_level_entry_index],&curr_level->lines[line_indx], sizeof(ARAD_PMF_PSL_LINE_INFO));

           updated_level_info->lines[update_level_entry_index].prog_id = new_prog;

           if(first_in_level) {
               first_in_level = FALSE;
               updated_level_info->first_index =  entry_indx;
           }

            
            res = arad_pmf_sel_line_hw_write(
                    unit,
                    stage,
                    entry_indx--,
                    &updated_level_info->lines[update_level_entry_index],
                    updated_level_info->lines[update_level_entry_index].prog_id,
                    TRUE
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

            ++update_level_entry_index;
        }

        
        updated_level_info->nof_new_lines = 0;
        updated_level_info->nof_removed_lines = 0;

        
        if(had_entries && first_in_level) 
        {
            updated_level_info->nof_lines = 0;
            updated_level_info->last_index = 0;
            updated_level_info->first_index =  0;
            update_level_info = TRUE;
        }
        else if (!first_in_level) 
        {
            updated_level_info->nof_lines = update_level_entry_index;
            updated_level_info->last_index = entry_indx + 1;
            update_level_info = TRUE;
        }

        if(update_level_info == TRUE) {
            
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.set(unit, stage, is_tm, level_indx, updated_level_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);
        }
    }

    
    first_line = ARAD_PMF_PROG_SELECT_FIRST_LINE(unit, stage, is_tm, init_info);
    for( ; entry_indx >= first_line; entry_indx--)
    {
        res = arad_pmf_sel_line_valid_set(unit, stage, entry_indx, TRUE , FALSE );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }

exit:
    ARAD_FREE(curr_level);
    ARAD_FREE(updated_level_info);
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_hw_commit()",stage,0);
}


uint32 
  ARAD_PMF_SEL_GROUP_add(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                   pfg_id,
      SOC_SAND_IN  uint32                   flags,
      SOC_SAND_IN  ARAD_PMF_SEL_GROUP       *group,
      SOC_SAND_IN  uint32                   prog_id,
      SOC_SAND_OUT  uint8                   *success
  )
{
    uint32
      indx,
        is_tm = (flags & ARAD_PMF_SEL_ALLOC_TM)? 1: 0,
        res;
    ARAD_PMF_PSL_LEVEL_INFO
        *prev_level = NULL,
        *curr_level = NULL;
    ARAD_PMF_PSL_LINE_INFO
        *group_line_ptr,
        group_line;
    uint8 
        intersected = FALSE,
        lines_alloc_succes;
        
    int32
        total_in_use=0,
        program_id=0,
        is_with_prog_id=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)) {
        res = ARAD_PMF_SEL_GROUP_to_line(
                unit,
                stage,
                pfg_id,
                flags,
                group,
                &group_line
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    group_line_ptr = (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE))? &group_line : NULL;

    ARAD_ALLOC(prev_level, ARAD_PMF_PSL_LEVEL_INFO, 1, "ARAD_PMF_SEL_GROUP_add.prev_level");
    ARAD_ALLOC(curr_level, ARAD_PMF_PSL_LEVEL_INFO, 1, "ARAD_PMF_SEL_GROUP_add.curr_level");

    if ( flags & ARAD_PMF_SEL_ALLOC_ADVANCED_MODE ) {

       
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, 1, curr_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
        if (!(flags & ARAD_PMF_SEL_ALLOC_CHECK_ONLY)) {
            
            
            if (curr_level->lines[group_line.line_id].flags & ARAD_PMF_SEL_LINE_WITH_ID) {
                program_id = curr_level->lines[group_line.line_id].prog_id;
                is_with_prog_id = 1;
            }

            
            res = arad_pmf_add_line_at_level(unit, stage, flags, &group_line, curr_level);
            SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

            if (is_with_prog_id) {
                uint32 program_rsrc;
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.get(unit, stage, 0, &program_rsrc);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

                curr_level->lines[group_line.line_id].prog_id = program_id;

                
                SHR_BITSET(&program_rsrc, program_id);
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.set(unit, stage, 0, program_rsrc);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
            }
            
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.set(unit, stage, is_tm, 1, curr_level);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
        }
        *success = 1;
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, 0, prev_level);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

    
    for(indx = 1; indx < ARAD_PMF_NOF_LEVELS; ++indx) 
    {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, indx, curr_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        

        

        
        
        res = arad_pmf_add_group_at_level(unit, stage, pfg_id, flags, group_line_ptr, prev_level, curr_level, &intersected);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        
        
        

        
        
        if (!(flags & ARAD_PMF_SEL_ALLOC_CHECK_ONLY)) {
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.set(unit, stage, is_tm, indx, curr_level);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

            
            if (flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE) {
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.set(unit, stage, is_tm, indx-1, prev_level);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 52, exit);
            }
        }

        
        sal_memcpy(prev_level, curr_level,sizeof(ARAD_PMF_PSL_LEVEL_INFO));

        
        total_in_use += curr_level->nof_lines;
        total_in_use += curr_level->nof_new_lines;
        total_in_use -= curr_level->nof_removed_lines;
        if (total_in_use < 0) {
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "    ")));
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "The number of needed entries at stage %s, level %d is %u: \n\r"), 
                       SOC_PPC_FP_DATABASE_STAGE_to_string(stage), indx, total_in_use));
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "    ")));
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "For this level, we use %d lines, we add %d lines, and we remove %d lines"), 
                       curr_level->nof_lines, curr_level->nof_new_lines, curr_level->nof_removed_lines));
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "\n\r")));
        }
    }

   
    res = arad_pmf_prog_select_verify_space(unit, stage, flags, total_in_use, &lines_alloc_succes);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    if(!lines_alloc_succes) {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "    ")));
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Program selection: no space to alloc the new line for PFG %d \n\r"), pfg_id));
        *success = 0;
        goto exit;
    }

    *success = 1;


exit:
    ARAD_FREE(prev_level);
    ARAD_FREE(curr_level);
    SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_SEL_GROUP_add()",0,0);
}


uint32 
  arad_pmf_psl_add(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32               pfg_id,
      SOC_SAND_IN  uint32                       flags,
      SOC_SAND_IN  ARAD_PMF_PSL       *psl,
      SOC_SAND_OUT  uint8             *success
  )
{
    ARAD_PMF_SEL_GROUP
        *pmf_group_ptr,
        pmf_group;
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (flags & ARAD_PMF_SEL_ALLOC_SECOND_PASS) {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.second_pass_psl_info.set(unit,stage, pfg_id, psl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
        *success = 1;
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    if (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE)) {
        res = arad_pmf_psl_to_group_map(
                unit,
                stage,
                psl,
                &pmf_group
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    pmf_group_ptr = (!(flags & ARAD_PMF_SEL_ALLOC_FOR_DELETE))? &pmf_group: NULL;

    

    
    
    if (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
    res = ARAD_PMF_SEL_GROUP_add(
        unit,
        stage, 
        pfg_id,
        flags,
        pmf_group_ptr,
        psl->ingress_pmf.program,
        success
    );
    }
    else {
    res = ARAD_PMF_SEL_GROUP_add(
            unit,
            stage, 
            pfg_id,
            flags,
            pmf_group_ptr,
            0,
            success
        );
    }

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);



exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_add()",0,0);
}

void
ARAD_PMF_PSL_clear(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_OUT ARAD_PMF_PSL *select_data,
    uint8 ethernet_or_tm_only, 
    uint8 mask_all
)
{
    sal_memset(select_data, 0x0, sizeof(ARAD_PMF_PSL));
    if(mask_all)
    {
        if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
            
            select_data->egress_pmf.egress_pmf_profile_mask         = 7;
            select_data->egress_pmf.format_code_mask                = 7;
            select_data->egress_pmf.value_1_mask                    = 0xff;
            select_data->egress_pmf.value_2_mask                    = 0xff;
            select_data->egress_pmf.header_code_mask                = 0xf;
            select_data->egress_pmf.qualifier_mask                  = 0xf;
            if (SOC_IS_JERICHO_PLUS(unit)) {
                select_data->egress_pmf.ethernet_tag_format_mask        = 0x7f;
                if (!SOC_IS_QAX_A0(unit)) {
                    select_data->egress_pmf.fall_to_bridge_mask         = 0x1;
                }
            } else {
                select_data->egress_pmf.ethernet_tag_format_mask        = 0x1f;
            }
        }
#ifdef BCM_88660_A0
        else if (SOC_IS_ARADPLUS(unit) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)) {
              select_data->ingress_slb.parser_leaf_context_mask     = ARAD_PARSER_PLC_MATCH_ANY;
              select_data->ingress_slb.vsi_profile_mask             = 0xF;
              select_data->ingress_slb.in_lif_profile_mask          = 0xF;
              select_data->ingress_slb.flp_program_mask             = 0x1F;
              select_data->ingress_slb.packet_format_code_mask      = ARAD_PARSER_PFC_MATCH_ANY;
              select_data->ingress_slb.pfq_next_protocol_fwd_mask   = 0xF;
              select_data->ingress_slb.pfq_next_protocol_fwd_next_mask = 0xF;
              select_data->ingress_slb.forwarding_code_mask         = 0xF;
              select_data->ingress_slb.forwarding_offset_index_mask = 0x7;
              select_data->ingress_slb.destination_msbs_mask        = 0x3F;
              select_data->ingress_slb.unknown_addr_mask            = 0x1;
          }
#endif 

        else {
            
            select_data->ingress_pmf.mask_parser_leaf_context         = ARAD_PARSER_PLC_MATCH_ANY;
            select_data->ingress_pmf.mask_in_port_profile             = 0x7;
            select_data->ingress_pmf.mask_ptc_profile                 = 0x7; 
            select_data->ingress_pmf.mask_packet_format_code          = ARAD_PARSER_PFC_MATCH_ANY;
            select_data->ingress_pmf.mask_packet_format_qualifier_0   = 0x7;
            select_data->ingress_pmf.mask_packet_format_qualifier_fwd = 0x7ff;
            select_data->ingress_pmf.mask_forwarding_code             = 0xf;
            select_data->ingress_pmf.mask_forwarding_offset_index     = 0x3;
            select_data->ingress_pmf.mask_forwarding_offset_index_ext = 0x3;
            select_data->ingress_pmf.mask_cpu_trap_code               = 0xff;
            select_data->ingress_pmf.mask_in_lif_profile              = 0xf;
            select_data->ingress_pmf.mask_out_lif_range               = 0x3;
            select_data->ingress_pmf.mask_llvp_incoming_tag_structure = 0xf;
            select_data->ingress_pmf.mask_vt_processing_profile       = 0x7;
            select_data->ingress_pmf.mask_vt_lookup_0_found           = 0x1;
            select_data->ingress_pmf.mask_vt_lookup_1_found           = 0x1;
            select_data->ingress_pmf.mask_tt_processing_profile       = 0x7;
            select_data->ingress_pmf.mask_tt_lookup_0_found           = 0x1;
            select_data->ingress_pmf.mask_tt_lookup_1_found           = 0x1;
            select_data->ingress_pmf.mask_fwd_prcessing_profile       = 0x7;
            select_data->ingress_pmf.mask_lem_1st_lkp_found            = 0x1;
            select_data->ingress_pmf.mask_lem_2nd_lkp_found            = 0x1;
            select_data->ingress_pmf.mask_lpm_1st_lkp_found            = 0x1;
            select_data->ingress_pmf.mask_lpm_2nd_lkp_found            = 0x1;
            select_data->ingress_pmf.mask_tcam_found                  = 0x1;
            select_data->ingress_pmf.mask_tcam_traps_found            = 0x1;
            select_data->ingress_pmf.mask_elk_error                   = 0x1;
            select_data->ingress_pmf.mask_elk_found_result            = 0xff;
#ifdef BCM_88660_A0
            
            if (SOC_IS_ARADPLUS(unit) && (!(SOC_IS_ARDON(unit)))) {
              select_data->ingress_pmf.mask_forwarding_offset_index  = 0x7;
              select_data->ingress_pmf.mask_stamp_native_vsi      = 0x1;
            }
#endif 
            if (SOC_IS_JERICHO(unit)) {
                select_data->ingress_pmf.mask_in_rif_profile          = 0x3F;
                select_data->ingress_pmf.mask_tcam_found_1            = 0x1;
                select_data->ingress_pmf.mask_tcam_traps_found_1      = 0x1;
                select_data->ingress_pmf.mask_rpf_stamp_native_vsi    = 0x1;
            }
     }
    }

    if (ethernet_or_tm_only == 1) {
        
        if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
            
            
            dpp_parser_pfc_get_pmf_by_sw(unit, DPP_PFC_ANY_ETH,
                    &select_data->ingress_pmf.packet_format_code,
                    &select_data->ingress_pmf.mask_packet_format_code);

            
            if (SOC_IS_JERICHO_PLUS(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "raw_port_filter", 0)) {
                select_data->ingress_pmf.mask_packet_format_code = ARAD_PARSER_PFC_MATCH_ANY;
            }
        }
    }
    if (ethernet_or_tm_only == 2) {
        if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
            
            
            dpp_parser_pfc_get_pmf_by_sw(unit, DPP_PFC_ANY_TM,
                    &select_data->ingress_pmf.packet_format_code,
                    &select_data->ingress_pmf.mask_packet_format_code);
        }
    }

    if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        select_data->egress_pmf.valid = 0;
        select_data->egress_pmf.program = 0;
    }
#ifdef BCM_88660_A0
    else if (SOC_IS_ARADPLUS(unit) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)) {
        select_data->ingress_slb.valid     = 0;
        select_data->ingress_slb.program   = 0;
    }
#endif 
    else {
        select_data->ingress_pmf.valid = 0;
        select_data->ingress_pmf.program = 0;
    }
}

void
ARAD_PMF_SEL_INIT_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_SEL_INIT_INFO *info)
{
    sal_memset(info, 0x0, sizeof(ARAD_PMF_SEL_INIT_INFO));
}


void
ARAD_PMF_PSL_reset(
    SOC_SAND_OUT ARAD_PMF_PSL *select_data
)
{
    sal_memset(select_data, 0x0, sizeof(ARAD_PMF_PSL));
}


uint32
  arad_pmf_prog_select_init(
    SOC_SAND_IN  int                 unit
  )
{
    uint32
        pgm_sel_tbl_data[SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)],
        line_ndx,
        res;
    SOC_PPC_FP_DATABASE_STAGE
        stage;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for(stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; stage++) {
        uint32 pmf_nof_lines = ARAD_PMF_NOF_LINES; 
        if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)
            || (( SOC_IS_ARAD_B1_AND_BELOW(unit) || SOC_IS_ARDON(unit) ) && stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)) {
            
            continue;
        }
        sal_memset(pgm_sel_tbl_data, 0x0, SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS) * sizeof(uint32));
        soc_mem_field32_set(unit, arad_pmf_sel_line_hw_table_get(unit, stage), pgm_sel_tbl_data, VALIDf, 0x0);
        for (line_ndx = 0; line_ndx < pmf_nof_lines; line_ndx++) {
            res = soc_mem_write(
                    unit,
                    arad_pmf_sel_line_hw_table_get(unit, stage),
                    MEM_BLOCK_ANY,
                    line_ndx,
                    pgm_sel_tbl_data
                  );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
        }
    }

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_init()", 0, 0);
}

uint32
arad_pmf_counter_set(
   SOC_SAND_IN  int                      unit,
   SOC_SAND_IN  int                      command_id,
   SOC_SAND_IN  SOC_TMC_CNT_MODE_EG_TYPE eg_type, 
   SOC_SAND_IN  int                      counter_base)
{
    uint32      
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (command_id < 0 || command_id >= 2) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_ERR, 20, exit);
    }
    if (counter_base < 0) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_ERR, 30, exit);
    }
    if (eg_type == SOC_TMC_CNT_MODE_EG_TYPE_TM  || 
        eg_type == SOC_TMC_CNT_MODE_EG_TYPE_VSI || 
        eg_type == SOC_TMC_CNT_MODE_EG_TYPE_PMF ||
        eg_type == SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF) {
        res = arad_pmf_low_level_pgm_counter_set_unsafe(unit, command_id, eg_type, counter_base); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    } else {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_ERR, 50, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_counter_set()", 0, 0);
}

uint32
  arad_pmf_prog_select_pmf_pgm_borders_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint8                    is_for_tm, 
    SOC_SAND_OUT  uint32  *pmf_pgm_ndx_min,
    SOC_SAND_OUT  uint32  *pmf_pgm_ndx_max
  )
{
    uint32
        res;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)) {
         *pmf_pgm_ndx_min = 0;
         *pmf_pgm_ndx_max = ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX;
         ARAD_DO_NOTHING_AND_EXIT;
    }

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.init_info.get(unit, stage, &init_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    *pmf_pgm_ndx_min = is_for_tm? ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN: init_info.nof_reserved_lines[is_for_tm];
    *pmf_pgm_ndx_max = is_for_tm? (init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM] + ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN): ARAD_PMF_LOW_LEVEL_NOF_PROGS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_pmf_pgm_borders_get()", 0, 0);
}

uint32
  arad_pmf_prog_select_line_borders_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint8                    is_for_tm, 
    SOC_SAND_OUT  uint32  *line_ndx_min,
    SOC_SAND_OUT  uint32  *line_ndx_max
  )
{
    soc_error_t
        rv;
    ARAD_PMF_SEL_INIT_INFO  
        init_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)||(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT)||(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.init_info.get(unit, stage, &init_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

    *line_ndx_min = ARAD_PMF_PROG_SELECT_FIRST_LINE(unit, stage, is_for_tm, init_info);
    
    
    *line_ndx_max = is_for_tm? ARAD_PMF_NOF_LINES : ARAD_PMF_NOF_LINES - init_info.nof_reserved_lines[ARAD_PMF_PSL_TYPE_ETH] - 2;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_pmf_pgm_borders_get()", 0, 0);
}

uint32
  arad_pmf_prog_select_eth_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint8                      is_tm_pmf_per_port_mode,
    SOC_SAND_INOUT  ARAD_PMF_SEL_INIT_INFO  *init_info
  )
{
    ARAD_PMF_PSL
        cam_data;
    ARAD_PMF_PSL_LEVEL_INFO
        *dflt_level_info = NULL,
        *level_1_info = NULL;
    uint8
        is_tm,
        found;
    uint32
        res, nof_eth_presel;
#ifdef ARAD_PMF_OAM_MIRROR_WA_ENABLED
  ARAD_PMF_PSL         
      psl;
  ARAD_PMF_PSL_LINE_INFO       
      cur_line;
  uint32
      pmf_pgm_ndx;
#endif 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.init_info.set(unit, stage, init_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    ARAD_ALLOC(dflt_level_info, ARAD_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_prog_select_eth_init.dflt_level_info");
    ARAD_ALLOC(level_1_info, ARAD_PMF_PSL_LEVEL_INFO, 1, "arad_pmf_prog_select_eth_init.level_1_info");

    for (is_tm = FALSE; is_tm <= (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF); is_tm++) {
        ARAD_PMF_PSL_clear(unit, stage, &cam_data, 1+is_tm, TRUE);

        sal_memset(dflt_level_info, 0x0, sizeof(ARAD_PMF_PSL_LEVEL_INFO)); 
        res = arad_pmf_psl_to_group_map(unit, stage, &cam_data, &dflt_level_info->lines[0].group);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        res = arad_pmf_sel_prog_alloc(unit, stage, is_tm, &dflt_level_info->lines[0].prog_id, &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        if(!found) {
            SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 55, exit);
        }

        sal_memset(dflt_level_info->lines[0].groups, 0x0, sizeof(uint32) * ARAD_PMF_GROUP_LEN); 
        dflt_level_info->nof_lines = 1;
        dflt_level_info->nof_new_lines = 0;
        dflt_level_info->nof_removed_lines = 0;
        dflt_level_info->level_index = 0;
        dflt_level_info->first_index = ARAD_PMF_NOF_LINES - dflt_level_info->lines[0].prog_id - 1;
        dflt_level_info->last_index = dflt_level_info->first_index;
        dflt_level_info->lines[0].flags = ARAD_PMF_SEL_LINE_VALID;
        
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.set(unit, stage, is_tm, 0, dflt_level_info);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

        
        res = arad_pmf_sel_line_hw_write(
                unit,
                stage,
                dflt_level_info->first_index,
                &dflt_level_info->lines[0],
                dflt_level_info->lines[0].prog_id,
                TRUE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

        
        if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
            res = arad_pmf_low_level_pgm_set(
                     unit,
                     dflt_level_info->lines[0].prog_id,
                     (is_tm? ARAD_PMF_PGM_TYPE_TM: ARAD_PMF_PGM_TYPE_ETH)
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        }
        if (!is_tm) {
            if (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
                SOC_SAND_CHECK_FUNC_RESULT(arad_pmf_prog_select_eth_section_nof_lines_get(unit,stage, &nof_eth_presel), 25, exit);
                level_1_info->nof_lines = nof_eth_presel;
                level_1_info->nof_new_lines = 0;
                level_1_info->nof_removed_lines = 0;
                level_1_info->level_index = 1;
                level_1_info->first_index = dflt_level_info->last_index - 1;
                level_1_info->last_index = level_1_info->first_index - nof_eth_presel +1; 
                
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.set(unit, stage, is_tm, 1, level_1_info);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
            }
        }
        
    }
    

#ifdef ARAD_PMF_OAM_MIRROR_WA_ENABLED
  
  if (SOC_DPP_CONFIG(unit)->pp.oam_enable && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)) {
      uint32
          prog_selection_line_indx;
      SOC_PPC_TRAP_CODE_INTERNAL
        trap_code_internal;
      ARAD_SOC_REG_FIELD
        strength_fld_fwd,
        strength_fld_snp;
      uint8
          prog_id = ARAD_PMF_PGM_TYPE_STACK,
          is_oam_wa_snoop_enabled = 0;
      SOC_PPC_TRAP_CODE                           
          trap_code_ndx;

#ifdef ARAD_PMF_OAM_MIRROR_WA_SNOOP_OUT_LIF_IN_DSP_EXT_ENABLED
      
      if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
          is_oam_wa_snoop_enabled = 1;
      }
#endif 

      if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_statistics_with_stacking", 0) ) {
          prog_id = ARAD_PMF_PGM_TYPE_XGS;
      }

      for (prog_selection_line_indx = 0; prog_selection_line_indx < (SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM + is_oam_wa_snoop_enabled); prog_selection_line_indx++) {
          
          ARAD_PMF_PSL_clear(unit, stage, &psl, FALSE, TRUE);
          
          ARAD_PMF_LINE_INFO_clear(&cur_line);
          
          trap_code_ndx = (prog_selection_line_indx == SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM)? 
              SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_MIP_EGRESS_SNOOP : (SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST + prog_selection_line_indx);
          res = arad_pp_trap_mgmt_trap_code_to_internal(
                  unit,
                  trap_code_ndx, 
                  &trap_code_internal,
                  &strength_fld_fwd,
                  &strength_fld_snp
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          psl.ingress_pmf.cpu_trap_code = trap_code_internal; 
          psl.ingress_pmf.mask_cpu_trap_code = 0; 
          res = arad_pmf_psl_to_group_map(unit, stage, &psl, &(cur_line.group));
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

          pmf_pgm_ndx = ARAD_PMF_PROGRAM_STATIC_INDEX_GET(init_info->nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM], 
                                                ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN, prog_id);
#ifdef ARAD_PMF_OAM_MIRROR_WA_SNOOP_OUT_LIF_IN_DSP_EXT_ENABLED
          if (prog_selection_line_indx == SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM) {
              
              pmf_pgm_ndx = ARAD_PMF_PROGRAM_STATIC_INDEX_GET(init_info->nof_reserved_lines[ARAD_PMF_PSL_TYPE_TM], 
                                                    ARAD_PMF_PROG_SELECT_TM_PMF_PGM_MIN, ARAD_PMF_PGM_TYPE_TM);
          }
#endif 
          res = arad_pmf_sel_line_hw_write(
                  unit,
                  stage,
                  prog_selection_line_indx,  
                  &cur_line,
                  pmf_pgm_ndx,
                  TRUE
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
  }
#endif 

    
exit:
  ARAD_FREE(dflt_level_info);
  ARAD_FREE(level_1_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_eth_init()", 0, 0);
}


uint32 
  arad_pmf_psl_map(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  SOC_PPC_PMF_PFG_INFO *pfg_info,
      SOC_SAND_IN  uint8                    is_for_tm, 
      SOC_SAND_OUT ARAD_PMF_PSL         *psl
  )
{
    uint32
        fld_val,
        lif_profile_offset,
        qual_ndx,
        res;
    uint8
        is_tm = is_for_tm;
    SOC_PPC_FP_QUAL_TYPE
        type;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PMF_PSL_clear(unit, pfg_info->stage, psl, 1 + is_tm, TRUE );

    for (qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; qual_ndx++) {
        if ((pfg_info->qual_vals[qual_ndx].type == SOC_PPC_FP_QUAL_IRPP_INVALID)
            || (pfg_info->qual_vals[qual_ndx].type == SOC_PPC_NOF_FP_QUAL_TYPES)
            || (pfg_info->qual_vals[qual_ndx].type == BCM_FIELD_ENTRY_INVALID)) {
            continue;
        }

        if (pfg_info->stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
            
            if (pfg_info->is_staggered) 
            {
                switch (pfg_info->qual_vals[qual_ndx].type) {
                case SOC_PPC_FP_QUAL_IRPP_PRESEL_ID :
                    psl->second_pass_ingress_pmf.program_selection_profile = pfg_info->qual_vals[qual_ndx].val.arr[0]; 
                    psl->second_pass_ingress_pmf.mask_program_selection_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1F;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE0 :
                    psl->second_pass_ingress_pmf.tcam_action_0_lsb = pfg_info->qual_vals[qual_ndx].val.arr[0]; 
                    psl->second_pass_ingress_pmf.mask_tcam_action_0_lsb = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE1 :
                    psl->second_pass_ingress_pmf.tcam_action_1_lsb = pfg_info->qual_vals[qual_ndx].val.arr[0]; 
                    psl->second_pass_ingress_pmf.mask_tcam_action_1_lsb = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE2 :
                    psl->second_pass_ingress_pmf.tcam_action_2_lsb = pfg_info->qual_vals[qual_ndx].val.arr[0]; 
                    psl->second_pass_ingress_pmf.mask_tcam_action_2_lsb = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE3 :
                    psl->second_pass_ingress_pmf.tcam_action_3_lsb = pfg_info->qual_vals[qual_ndx].val.arr[0]; 
                    psl->second_pass_ingress_pmf.mask_tcam_action_3_lsb = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE_KAPS :
                    psl->second_pass_ingress_pmf.kaps_payload_lsb = pfg_info->qual_vals[qual_ndx].val.arr[0]; 
                    psl->second_pass_ingress_pmf.mask_kaps_payload_lsb = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                    break;
                 default:
                    LOG_ERROR(BSL_LS_SOC_FP,
                              (BSL_META_U(unit,
                                          "Unit %d Stage Ingress Qualifier %d - Invalid qualifier.\n\r"),
                               unit, pfg_info->qual_vals[qual_ndx].type));
                    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 13, exit);
                }
            } else {

                switch (pfg_info->qual_vals[qual_ndx].type) {
                case SOC_PPC_FP_QUAL_OUT_LIF_RANGE:
                    psl->ingress_pmf.out_lif_range = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_out_lif_range = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT:
                    psl->ingress_pmf.parser_leaf_context = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_parser_leaf_context = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS:
                    psl->ingress_pmf.in_port_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_in_port_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR_PS:  
                    psl->ingress_pmf.ptc_profile = pfg_info->qual_vals[qual_ndx].val.arr[0]; 
                    psl->ingress_pmf.mask_ptc_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7; 
                    break; 
                case SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE:
                    psl->ingress_pmf.packet_format_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_packet_format_code = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3F;
                    break;
                case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0:
                    psl->ingress_pmf.packet_format_qualifier_0 = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_packet_format_qualifier_0 = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                    break;
                
                case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION:
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 5, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 2);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 5, &fld_val, 0, 2);
                    break;
                case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL:
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 7, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 4);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 7, &fld_val, 0, 4);
                    break;
                case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL:
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 7, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 4);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 7, &fld_val, 0, 4);
                    break;
                case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER_NEXT_PROTOCOL_PLUS_ONE:
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 7, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 4);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 7, &fld_val, 0, 4);
                    break;
                case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED:
                case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED:
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 6, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 6, &fld_val, 0, 1);
                    break;
                case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS:
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 4, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 4, &fld_val, 0, 1);
                    break;
                case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS:
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.packet_format_qualifier_fwd), 0, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_packet_format_qualifier_fwd), 0, &fld_val, 0, 1);
                    break;

                case SOC_PPC_FP_QUAL_IRPP_FWD_TYPE:
                    psl->ingress_pmf.forwarding_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_forwarding_code = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX:
                    
                    fld_val = 0x3;
    #ifdef BCM_88660_A0
                      if (SOC_IS_ARADPLUS(unit)) {
                          
                          fld_val = 0x7;
                      }
    #endif 
                    psl->ingress_pmf.forwarding_offset_index = pfg_info->qual_vals[qual_ndx].val.arr[0] & fld_val;
                    psl->ingress_pmf.mask_forwarding_offset_index = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & fld_val;
                    break;
                case SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION:
                    psl->ingress_pmf.forwarding_offset_index_ext = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_forwarding_offset_index_ext = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                    break;
                case SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE:
                    psl->ingress_pmf.cpu_trap_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_cpu_trap_code = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xFF;
                    break;
                case SOC_PPC_FP_QUAL_IN_LIF_PROFILE:
                    lif_profile_offset = 0;

                    
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.in_lif_profile), lif_profile_offset, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, (4-lif_profile_offset));
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & ((1 << (4-lif_profile_offset)) - 1);
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_in_lif_profile), lif_profile_offset, &fld_val, 0, (4-lif_profile_offset));
                    break;
                case SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF:
                    
                    if (soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0)) { 
                        
                        SHR_BITCOPY_RANGE(&(psl->ingress_pmf.in_lif_profile), 0, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                        fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                        SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_in_lif_profile), 0, &fld_val, 0, 1);
                    }
                    break;
                case SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT:
                    psl->ingress_pmf.llvp_incoming_tag_structure = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_llvp_incoming_tag_structure = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                    break;
                case SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE:
                    psl->ingress_pmf.vt_processing_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_vt_processing_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                    break;
                case SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND:
                    psl->ingress_pmf.vt_lookup_0_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_vt_lookup_0_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND:
                    psl->ingress_pmf.vt_lookup_1_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_vt_lookup_1_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE:
                    psl->ingress_pmf.tt_processing_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_tt_processing_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                    break;
                case SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND:
                    psl->ingress_pmf.tt_lookup_0_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_tt_lookup_0_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND:
                    psl->ingress_pmf.tt_lookup_1_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_tt_lookup_1_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE:
                    psl->ingress_pmf.fwd_prcessing_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_fwd_prcessing_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3;
                    break;
                case SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND:
                    psl->ingress_pmf.lem_1st_lkp_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_lem_1st_lkp_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND:
                    psl->ingress_pmf.lem_2nd_lkp_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_lem_2nd_lkp_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND:
                    psl->ingress_pmf.lpm_1st_lkp_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_lpm_1st_lkp_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND:
                    psl->ingress_pmf.lpm_2nd_lkp_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_lpm_2nd_lkp_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_TCAM_MATCH:
                    psl->ingress_pmf.tcam_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_tcam_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH:
                    psl->ingress_pmf.tcam_traps_found = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_tcam_traps_found = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_ELK_ERROR:
                    psl->ingress_pmf.elk_error = pfg_info->qual_vals[qual_ndx].val.arr[0];
                    psl->ingress_pmf.mask_elk_error = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    break;
                case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0:
                case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1:
                case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2:
                case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3:
                case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_4:
                case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_5:
                case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_6:
                case SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_7:
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.elk_found_result), 7 - (pfg_info->qual_vals[qual_ndx].type - SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0), 
                                        &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    SHR_BITCOPY_RANGE(&(psl->ingress_pmf.mask_elk_found_result), 7 - (pfg_info->qual_vals[qual_ndx].type - SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0), 
                                        &fld_val, 0, 1);
                    break;
                case SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5:
                    
                    break;
                case SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE:
                    if (SOC_IS_JERICHO(unit)) {
                        psl->ingress_pmf.in_rif_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                        psl->ingress_pmf.mask_in_rif_profile = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3f;
                    }
                    break;

                
                case SOC_PPC_FP_QUAL_STAMP_NATIVE_VSI:
    #ifdef BCM_88660_A0
                      if (SOC_IS_ARADPLUS(unit)) {
                          
                          psl->ingress_pmf.stamp_native_vsi = pfg_info->qual_vals[qual_ndx].val.arr[0];
                          psl->ingress_pmf.mask_stamp_native_vsi = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                          break;
                      }
    #endif 
                      
                default:
                    LOG_ERROR(BSL_LS_SOC_FP,
                              (BSL_META_U(unit,
                                          "Unit %d Stage Ingress Qualifier %d - Invalid qualifier.\n\r"),
                               unit, pfg_info->qual_vals[qual_ndx].type));
                    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 13, exit);
                }
            }
        }
#ifdef BCM_88660_A0
        else if (SOC_IS_ARADPLUS(unit) && (pfg_info->stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)) {
            

            res = arad_pmf_psl_slb_field_update(unit, &type, psl, &pfg_info->qual_vals[qual_ndx]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

            switch (type) {
            case SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT:
                psl->ingress_slb.parser_leaf_context = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.parser_leaf_context_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE:
                psl->ingress_slb.packet_format_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.packet_format_code_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x3F;
                break;
            case SOC_PPC_FP_QUAL_IRPP_FWD_TYPE:
                psl->ingress_slb.forwarding_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.forwarding_code_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX:
                fld_val = 0x7;
                psl->ingress_slb.forwarding_offset_index = pfg_info->qual_vals[qual_ndx].val.arr[0] & fld_val;
                psl->ingress_slb.forwarding_offset_index_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & fld_val;
                break;
            case SOC_PPC_FP_QUAL_IN_LIF_PROFILE:
                lif_profile_offset = 0;
                SHR_BITCOPY_RANGE(&(psl->ingress_slb.in_lif_profile), lif_profile_offset, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, (4-lif_profile_offset));
                fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & ((1 << (4-lif_profile_offset)) - 1);
                SHR_BITCOPY_RANGE(&(psl->ingress_slb.in_lif_profile_mask), lif_profile_offset, &fld_val, 0, (4-lif_profile_offset));
                break;
            case SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF:
                
                if (soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0)) { 
                    
                    SHR_BITCOPY_RANGE(&(psl->ingress_slb.in_lif_profile), 0, &(pfg_info->qual_vals[qual_ndx].val.arr[0]), 0, 1);
                    fld_val = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                    SHR_BITCOPY_RANGE(&(psl->ingress_slb.in_lif_profile_mask), 0, &fld_val, 0, 1);
                }
                break;
            case SOC_PPC_FP_QUAL_VSI_PROFILE:
                psl->ingress_slb.vsi_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.vsi_profile_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL:
                psl->ingress_slb.pfq_next_protocol_fwd = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.pfq_next_protocol_fwd_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL:
                psl->ingress_slb.pfq_next_protocol_fwd_next = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.pfq_next_protocol_fwd_next_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER_NEXT_PROTOCOL_PLUS_ONE:
                psl->ingress_slb.pfq_next_protocol_fwd_next = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.pfq_next_protocol_fwd_next_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_UNKNOWN_ADDR:
                psl->ingress_slb.unknown_addr = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->ingress_slb.unknown_addr_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1;
                break;
            case SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5:
                
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit %d Stage Ingress Qualifier %d - Invalid qualifier.\n\r"),
                           unit, pfg_info->qual_vals[qual_ndx].type));
                SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 15, exit);
            }
        }
#endif 
        else if (pfg_info->stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
            
            switch (pfg_info->qual_vals[qual_ndx].type) {
            case SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS:
                psl->egress_pmf.egress_pmf_profile = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.egress_pmf_profile_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                break;
            case SOC_PPC_FP_QUAL_ERPP_FORMAT_CODE:
            case SOC_PPC_FP_QUAL_ERPP_LEARN_EXT:
                psl->egress_pmf.format_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.format_code_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7;
                break;
            case SOC_PPC_FP_QUAL_ERPP_SYS_VALUE1:
                psl->egress_pmf.value_1 = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.value_1_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xFF;
                break;
            case SOC_PPC_FP_QUAL_ERPP_SYS_VALUE2:
                psl->egress_pmf.value_2 = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.value_2_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xFF;
                break;
            case SOC_PPC_FP_QUAL_ERPP_HEADER_CODE: 
            case SOC_PPC_FP_QUAL_ERPP_FWD_CODE: 
                psl->egress_pmf.header_code = pfg_info->qual_vals[qual_ndx].val.arr[0];
                psl->egress_pmf.header_code_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0xF;
                break;
            case SOC_PPC_FP_QUAL_ERPP_ETH_TAG_FORMAT:
                psl->egress_pmf.ethernet_tag_format = pfg_info->qual_vals[qual_ndx].val.arr[0];
                if (SOC_IS_JERICHO_PLUS(unit)) {
                    psl->egress_pmf.ethernet_tag_format_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x7F;
                } else {
                    psl->egress_pmf.ethernet_tag_format_mask = (~pfg_info->qual_vals[qual_ndx].is_valid.arr[0]) & 0x1F;
                }
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit %d Stage Egress Qualifier %d - Invalid qualifier.\n\r"),
                           unit, pfg_info->qual_vals[qual_ndx].type));
                SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 17, exit);
            }
        }
        else {
            
            SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 20, exit);
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_map()", 0, 0);
}

uint32
  arad_pmf_psl_pmf_pgms_get(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_INOUT  SOC_PPC_FP_DATABASE_INFO *database_info,
      SOC_SAND_IN  uint8                    is_default_db, 
      SOC_SAND_IN  uint8                    is_for_tm, 
      SOC_SAND_OUT uint32                  *pgm_bmp_used,
      SOC_SAND_OUT uint32                  *pgm_bmp_remain, 
      SOC_SAND_OUT uint32                   presel_bmp_update[ARAD_PMF_NOF_LINES_MAX_ALL_STAGES_LEN], 
      SOC_SAND_OUT uint8                   *success
  )
{
    uint32
        pfg_ndx,
        qual_ndx,
        min_indx,
        max_indx,
        line_id,
        action_id,
        db_id,
        valid,
        pmf_pgm_ndx,
        pgm_bmp_not_duplicate[1],
        nof_pmf_pgms_free,
        nof_pmf_pgms_needed,
        program_rsrc = 0,
        groups_lcl[ARAD_PMF_GROUP_LEN],
        is_tm = is_for_tm,
        nof_progs = ARAD_PMF_LOW_LEVEL_NOF_PROGS,
        nof_levels = ARAD_PMF_NOF_LEVELS, 
        res;
    uint32 db_pmb_used[SOC_PPC_FP_NOF_DBS_IN_LONGS];
    int32
        level_indx,
        staggered_idx,
        line_indx;
    ARAD_PMF_PSL_LEVEL_INFO
        curr_level;
    ARAD_PMF_PSL                  
        psl_tbl_data;
    SOC_PPC_PMF_PFG_INFO
        pfg_info_current;
    SOC_PPC_FP_CONTROL_INDEX       
        control_ndx;
    SOC_PPC_FP_CONTROL_INFO        
        control_info;
    SOC_PPC_FP_DATABASE_INFO
        fp_database_info;
    ARAD_PMF_DB_INFO        
        pmf_db_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)) {
        if (is_for_tm) {
            
            SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 3, exit);
        }
        else if (is_default_db) {
            *pgm_bmp_used = (1 << SOC_DPP_DEFS_GET(unit, nof_flp_programs)) - 1;
            *pgm_bmp_remain = 0;
            *success = TRUE;
            ARAD_DO_NOTHING_AND_EXIT;
        }
        else {
            *pgm_bmp_used = 0;
            *pgm_bmp_remain = 0;
            *success = TRUE;
            sal_memcpy(groups_lcl, database_info->supported_pfgs_arad, sizeof(groups_lcl));
            for (pfg_ndx = 0; pfg_ndx < SOC_PPC_FP_NOF_PFGS_ARAD; pfg_ndx++) {
                if (SHR_BITGET(groups_lcl, pfg_ndx)) {
                     
                    pfg_info_current.stage = stage;
                    res = arad_pp_fp_packet_format_group_get_unsafe(
                              unit,
                              pfg_ndx,
                              &pfg_info_current
                            );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

                    
                    for (qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; qual_ndx++) {
                        if (pfg_info_current.qual_vals[qual_ndx].type == SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE) {
                            
                            SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
                            control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_FLP_PGM_PROFILE;
                            control_ndx.val_ndx = pfg_info_current.qual_vals[qual_ndx].val.arr[0];
                            res = soc_ppd_fp_control_get(unit, SOC_CORE_INVALID, &control_ndx, &control_info);
                            SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
                            *pgm_bmp_used |= control_info.val[0];
                        }
                    }
                }
            }
        }

        ARAD_DO_NOTHING_AND_EXIT;
    }

    sal_memset(presel_bmp_update, 0x0, ARAD_PMF_NOF_LINES_MAX_ALL_STAGES_LEN * sizeof(uint32));

    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.get(unit, stage, 0, &program_rsrc);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    nof_pmf_pgms_free = 0;
    for (pmf_pgm_ndx = 0; pmf_pgm_ndx < nof_progs; pmf_pgm_ndx++) {
        if (!SHR_BITGET(&program_rsrc, pmf_pgm_ndx)) {
            nof_pmf_pgms_free++;
        }
    }

    *pgm_bmp_used = 0;
    *pgm_bmp_not_duplicate = 0;
    
    for(level_indx = 0; level_indx < nof_levels; ++level_indx) 
    {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, level_indx, &curr_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        
        for(line_indx = 0 ; line_indx < curr_level.nof_lines; ++line_indx) 
        {
            if ( (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) && (!(curr_level.lines[line_indx].flags & ARAD_PMF_SEL_LINE_VALID )) ) {
                continue;
            }
           
            sal_memcpy(groups_lcl,database_info->supported_pfgs_arad, sizeof(groups_lcl));
            soc_sand_bitstream_and(groups_lcl, curr_level.lines[line_indx].groups, ARAD_PMF_GROUP_LEN);
            
            if (groups_lcl[0] || groups_lcl[1] || is_default_db) 
            {
                
                *pgm_bmp_used |= (1 << curr_level.lines[line_indx].prog_id);
                
                SHR_BITSET(presel_bmp_update, (curr_level.first_index - line_indx));
            }
            else {
                
                
                *pgm_bmp_not_duplicate |= (1 << curr_level.lines[line_indx].prog_id);
            }

        }
    }

  
  if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) && (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) && (!is_for_tm) ) 
  {
      for (staggered_idx = 0; staggered_idx < SOC_DPP_NOF_INGRESS_PMF_PROGRAM_SELECTION_LINES_ARAD ; staggered_idx++ ) 
      {
          if (database_info->db_staggered_info[staggered_idx].valid) 
          {
              
              SOC_SAND_CHECK_FUNC_RESULT(arad_pmf_prog_select_line_borders_get(unit,stage, ARAD_PMF_PSL_TYPE_ETH, &min_indx,&max_indx), 25, exit);

              
              line_id = max_indx - database_info->db_staggered_info[staggered_idx].first_pass_presel_id;

              
              res = arad_pmf_sel_table_get(
                      unit, 
                      line_id, 
                      stage, 
                      &valid, 
                      &pmf_pgm_ndx, 
                      &psl_tbl_data
                    );
              
              
              
              SHR_BITSET(pgm_bmp_used, pmf_pgm_ndx);

               
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_db_pmb.bit_range_read(unit, stage, pmf_pgm_ndx, 0, 0, ARAD_PMF_NOF_DBS, db_pmb_used);

              for (db_id = 0 ; db_id < ARAD_PMF_NOF_DBS ; db_id++) 
              {
                  if (SHR_BITGET(db_pmb_used, db_id) != 0x1) {
                      
                      continue;
                  }
                    
                  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(unit,
                                                                                      stage,
                                                                                      db_id,
                                                                                      &fp_database_info);
                   res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(unit, 
                                                                               stage, 
                                                                               db_id, 
                                                                               &pmf_db_info);

                  
                  for (action_id = 0 ; action_id < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX ; action_id ++) 
                  {
                      if ( (fp_database_info.action_types[action_id] == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_0) ||
                           (fp_database_info.action_types[action_id] == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_1) ||
                           (fp_database_info.action_types[action_id] == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_2) ||
                           (fp_database_info.action_types[action_id] == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_3) ||
                           (fp_database_info.action_types[action_id] == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_KAPS) ) 
                      {
                          switch (fp_database_info.action_types[action_id]) 
                          {
                          case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_0:
                              database_info->db_staggered_info[staggered_idx].presel_res_0_key = pmf_db_info.used_key[pmf_pgm_ndx][0];
                              break;
                          case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_1:
                              database_info->db_staggered_info[staggered_idx].presel_res_1_key = pmf_db_info.used_key[pmf_pgm_ndx][0];
                              break;
                          case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_2:
                              database_info->db_staggered_info[staggered_idx].presel_res_2_key = pmf_db_info.used_key[pmf_pgm_ndx][0];
                              break;
                          case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_3:
                              database_info->db_staggered_info[staggered_idx].presel_res_3_key = pmf_db_info.used_key[pmf_pgm_ndx][0];
                              break;
                          case SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_KAPS:
                              database_info->db_staggered_info[staggered_idx].presel_kaps_key = pmf_db_info.used_key[pmf_pgm_ndx][0];
                              break;
                          default:
                              break;
                          }
                      }
                  }
              }
          
          }
      }
          
   }

    
    *pgm_bmp_remain = *pgm_bmp_not_duplicate;
    *pgm_bmp_not_duplicate &= (*pgm_bmp_used);
    nof_pmf_pgms_needed = 1; 
    for (pmf_pgm_ndx = 0; pmf_pgm_ndx < nof_progs; pmf_pgm_ndx++) {
        if (SHR_BITGET(pgm_bmp_not_duplicate, pmf_pgm_ndx)) {
            nof_pmf_pgms_needed++;
        }
    }
    *success = (nof_pmf_pgms_needed <= nof_pmf_pgms_free)? TRUE: FALSE;

    
    
    if ((((*pgm_bmp_used) & program_rsrc) != (*pgm_bmp_used)) && !(database_info->pgm_bmp)){
        
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Unit %d Inconsistency in program resources: Program resource 0x%x "
                              "Used program bitmap 0x%x.\n\r"), unit, program_rsrc, *pgm_bmp_used));
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 30, exit);
    }

   


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_pmf_pgms_get()", 0, 0);
}


uint32
  arad_pmf_psl_pmf_pgms_get_by_qual_match(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                     supported_pfgs[SOC_PPC_FP_NOF_PFGS_IN_LONGS_ARAD],
      SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL      *qual,
      SOC_SAND_OUT uint32                  *matched_pgm_bmp
  )
{
    uint32
        qual_type_ndx,
        pfg_ndx,
        match_pfg_found = 0,
        match_pfgs[SOC_PPC_FP_NOF_PFGS_IN_LONGS_ARAD],
        pmf_pgm_match_bmp,
        groups_lcl[SOC_PPC_FP_NOF_PFGS_IN_LONGS_ARAD],
        res;
    int level_indx,
        line_indx;
    SOC_SAND_U64
        qual_val1, qual_val2, is_valid;
    SOC_PPC_PMF_PFG_INFO
        pfg_info_current;
    ARAD_PMF_PSL_LEVEL_INFO
        curr_level;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(qual);
    SOC_SAND_CHECK_NULL_INPUT(matched_pgm_bmp);

    if (!soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        *matched_pgm_bmp = 0;
        ARAD_DO_NOTHING_AND_EXIT;
    }

    sal_memset(match_pfgs, 0x0, sizeof(match_pfgs));
    
    
    pfg_info_current.stage = stage;
    for (pfg_ndx = 0; pfg_ndx < SOC_PPC_FP_NOF_PFGS_ARAD; pfg_ndx++) {
        if (SHR_BITGET(supported_pfgs, pfg_ndx)) {                    
            
            res = arad_pp_fp_packet_format_group_get_unsafe(
                      unit,
                      pfg_ndx,
                      &pfg_info_current
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
            if (pfg_info_current.is_array_qualifier) {
                for (qual_type_ndx=0; qual_type_ndx<SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx) {
                    if (pfg_info_current.qual_vals[qual_type_ndx].type == qual->type) {
                        
                        is_valid = pfg_info_current.qual_vals[qual_type_ndx].is_valid;
                        soc_sand_bitstream_and(is_valid.arr, qual->is_valid.arr, sizeof(is_valid)/sizeof(uint32));
                        if (soc_sand_u64_are_equal(&is_valid, &qual->is_valid)) {
                            qual_val1 = pfg_info_current.qual_vals[qual_type_ndx].val;
                            qual_val2 = qual->val;
                            soc_sand_bitstream_and(qual_val1.arr, is_valid.arr, sizeof(is_valid)/sizeof(uint32));
                            soc_sand_bitstream_and(qual_val2.arr, is_valid.arr, sizeof(is_valid)/sizeof(uint32));
                            if (soc_sand_u64_are_equal(&qual_val1, &qual_val2)) {
                                SHR_BITSET(match_pfgs, pfg_ndx);
                                match_pfg_found = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    if (match_pfg_found == 0) {
        *matched_pgm_bmp = 0;
        ARAD_DO_NOTHING_AND_EXIT;
    }
    
    
    pmf_pgm_match_bmp = 0;
    
    for(level_indx = 0; level_indx < ARAD_PMF_NOF_LEVELS; ++level_indx) 
    {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, FALSE, level_indx, &curr_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    
        
        for(line_indx = 0 ; line_indx < curr_level.nof_lines; ++line_indx) 
        {
            if ( !(curr_level.lines[line_indx].flags & ARAD_PMF_SEL_LINE_VALID ))  {
                continue;
            }
           
            sal_memcpy(groups_lcl, match_pfgs, sizeof(groups_lcl));
            soc_sand_bitstream_and(groups_lcl, curr_level.lines[line_indx].groups, SOC_PPC_FP_NOF_PFGS_IN_LONGS_ARAD);
            
            if (groups_lcl[0] || groups_lcl[1]) 
            {
                
                pmf_pgm_match_bmp |= (1 << curr_level.lines[line_indx].prog_id);
            }
        }
    }    

    *matched_pgm_bmp = pmf_pgm_match_bmp;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_pmf_pgms_get_by_qual()", 0, 0);
}



uint32
  arad_pmf_psl_swicth_key_egq_get(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                   pmf_pgm_from,
      SOC_SAND_IN  uint32                   db_id_ndx,
      SOC_SAND_IN  uint32                   db_strength,
      SOC_SAND_OUT uint8                    *switch_key_egq
  )
{
    uint32
        db_ndx,
        res;
    ARAD_PMF_DB_INFO
        db_info;
    uint8 
        key_ndx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *switch_key_egq = FALSE;
    if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        
        for (db_ndx = 0; db_ndx < SOC_PPC_FP_NOF_DBS; ++db_ndx)
        {
            
            if (db_ndx == db_id_ndx) {
                continue;
            }
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(unit, stage, db_ndx, &db_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

            if (db_info.valid && SHR_BITGET(db_info.progs, pmf_pgm_from)) {
                
                key_ndx = db_info.used_key[pmf_pgm_from][0]; 
                if (((db_strength < db_info.prio) && (key_ndx == 0))
                    || ((db_strength > db_info.prio) && (key_ndx == 1))) {
                    *switch_key_egq = TRUE;
                }
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_swicth_key_egq_get()", pmf_pgm_from, db_id_ndx);
}



uint32
  arad_pmf_psl_pmf_pgm_duplicate(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                   pmf_pgm_from,
      SOC_SAND_IN  uint32                   pmf_pgm_to,
      SOC_SAND_IN  uint8                    switch_key_egq
  )
{
    uint32
        ce_indx,
        fes_ndx,
        ce_rsrc,
        db_id_ndx,
        keys_bmp,
        line_ndx,
        cycle_ndx,
        fld_val[SOC_DPP_DEFS_MAX(NOF_EGRESS_PMF_KEYS)] = {0},
        ce_group_ndx,
        key_ndx,
        key_ndx_resolved,
        table_entry[20] = {0},
        prog_used_cycle_bmp_lcl[1],
        res,
        nof_ce_in_prog = ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG,
        pgm_db_pmf_from[ARAD_BIT_TO_U32(ARAD_PMF_NOF_DBS)];
    ARAD_PMF_DB_INFO
        db_info;
    ARAD_PMF_CE
      sw_db_ce;
    ARAD_PMF_FES
        fes_sw_db_info;
    uint8
        is_msb;
    soc_mem_t
        ce_table;
    soc_reg_above_64_val_t
            reg_above_64[SOC_DPP_DEFS_MAX(NOF_EGRESS_PMF_KEYS)];
    soc_reg_t
        egq_key_data_base_profile[2] = {EGQ_KEYA_DATA_BASE_PROFILEr, EGQ_KEYB_DATA_BASE_PROFILEr};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(table_entry, 0x0, sizeof(table_entry));

    
    if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        for (is_msb = FALSE; is_msb <= TRUE; is_msb++) {
            for (ce_group_ndx = 0; ce_group_ndx < ARAD_PMF_CE_NOF_GROUP_PER_LSB; ce_group_ndx++) {
                ce_table = arad_pmf_ce_table_get(unit, stage, is_msb, 0, 0 , 0); 

                line_ndx = pmf_pgm_from + (ce_group_ndx * ARAD_PMF_LOW_LEVEL_NOF_PROGS); 
                
                res = soc_mem_read(unit, ce_table,  MEM_BLOCK_ANY, line_ndx, table_entry);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

                if (switch_key_egq) {
                    
                    for (key_ndx = 0; key_ndx < SOC_DPP_DEFS_GET(unit, nof_egress_pmf_keys); key_ndx++) {
                    
                        fld_val[key_ndx] = soc_mem_field32_get(unit, ce_table, table_entry, 
                                               arad_pmf_ce_valid_fld_group_get(unit,stage, ce_group_ndx, key_ndx));
                    }

                    
                    for (key_ndx = 0; key_ndx < SOC_DPP_DEFS_GET(unit, nof_egress_pmf_keys); key_ndx++) {
                    
                        soc_mem_field32_set(unit, ce_table, table_entry, 
                                            arad_pmf_ce_valid_fld_group_get(unit, stage, ce_group_ndx, key_ndx),
                                            fld_val[(SOC_DPP_DEFS_GET(unit, nof_egress_pmf_keys) - 1) - key_ndx]);
                    }
                }

                line_ndx = pmf_pgm_to + (ce_group_ndx * ARAD_PMF_LOW_LEVEL_NOF_PROGS); 
                res = soc_mem_write(unit, ce_table, MEM_BLOCK_ANY, line_ndx, table_entry);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
            }
        }

        
        
        for (key_ndx = 0; key_ndx < (SOC_DPP_DEFS_GET(unit, nof_egress_pmf_keys)); key_ndx++) {
            SOC_REG_ABOVE_64_CLEAR(reg_above_64[key_ndx]);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 18, exit, soc_reg_above_64_get(unit, egq_key_data_base_profile[key_ndx], REG_PORT_ANY, 0, reg_above_64[key_ndx]));
            key_ndx_resolved = (switch_key_egq)? ((SOC_DPP_DEFS_GET(unit, nof_egress_pmf_keys) -1) - key_ndx): key_ndx;
            SHR_BITCOPY_RANGE(&fld_val[key_ndx_resolved], 0, reg_above_64[key_ndx], (ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS * pmf_pgm_from), ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS);
        }

        for (key_ndx = 0; key_ndx < (SOC_DPP_DEFS_GET(unit, nof_egress_pmf_keys)); key_ndx++) {
            SHR_BITCOPY_RANGE(reg_above_64[key_ndx], (ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS * pmf_pgm_to), &fld_val[key_ndx], 0, ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 19, exit, soc_reg_above_64_set(unit, egq_key_data_base_profile[key_ndx], REG_PORT_ANY, 0, reg_above_64[key_ndx]));
        }
    }
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) { 
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHB_PROGRAM_KEY_GEN_VARm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 22, exit, WRITE_IHB_PROGRAM_KEY_GEN_VARm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_PMF_PASS_1_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, WRITE_IHB_PMF_PASS_1_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 34, exit, READ_IHB_PMF_PASS_1_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 36, exit, WRITE_IHB_PMF_PASS_1_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHB_PMF_PASS_2_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 42, exit, WRITE_IHB_PMF_PASS_2_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 44, exit, READ_IHB_PMF_PASS_2_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 46, exit, WRITE_IHB_PMF_PASS_2_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, READ_IHB_PMF_PASS_1_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 232, exit, WRITE_IHB_PMF_PASS_1_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 234, exit, READ_IHB_PMF_PASS_1_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 236, exit, WRITE_IHB_PMF_PASS_1_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 240, exit, READ_IHB_PMF_PASS_2_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 242, exit, WRITE_IHB_PMF_PASS_2_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 244, exit, READ_IHB_PMF_PASS_2_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 246, exit, WRITE_IHB_PMF_PASS_2_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));

        if (SOC_IS_JERICHO_PLUS(unit)) {
            soc_reg_above_64_val_t field_above_64;
            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_PMF_UPDATE_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, WRITE_IHB_PMF_UPDATE_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 34, exit, READ_IHB_PMF_UPDATE_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 36, exit, WRITE_IHB_PMF_UPDATE_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, READ_IHB_PMF_UPDATE_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 232, exit, WRITE_IHB_PMF_UPDATE_KEY_GEN_LSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 234, exit, READ_IHB_PMF_UPDATE_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 236, exit, WRITE_IHB_PMF_UPDATE_KEY_GEN_MSBm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
            SOC_REG_ABOVE_64_CLEAR(reg_above_64[0]);
            SOC_REG_ABOVE_64_CLEAR(field_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, READ_IHB_XOR_MASKSr(unit, reg_above_64[0]));
            SHR_BITSET_RANGE(field_above_64, 0, 80);
            soc_reg_above_64_field_set(unit, IHB_XOR_MASKSr, reg_above_64[0], XOR_MASK_3f, field_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 166, exit, WRITE_IHB_XOR_MASKSr(unit, reg_above_64[0]));

            SOC_REG_ABOVE_64_CLEAR(reg_above_64[0]);
            SOC_REG_ABOVE_64_CLEAR(field_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, READ_IHB_FEM_FES_COMPARE_SELECTIONr(unit, reg_above_64[0]));

            soc_reg_above_64_field_get(unit, IHB_FEM_FES_COMPARE_SELECTIONr, reg_above_64[0], XOR_MASK_SELECTf, field_above_64);
            SHR_BITCOPY_RANGE(field_above_64,(pmf_pgm_to*2),field_above_64,(pmf_pgm_from*2),2);
            soc_reg_above_64_field_set(unit, IHB_FEM_FES_COMPARE_SELECTIONr, reg_above_64[0], XOR_MASK_SELECTf, field_above_64);

            soc_reg_above_64_field_get(unit, IHB_FEM_FES_COMPARE_SELECTIONr, reg_above_64[0], XOR_INPUT_SELECTf, field_above_64);
            SHR_BITCOPY_RANGE(field_above_64,(pmf_pgm_to*2),field_above_64,(pmf_pgm_from*2),2);
            soc_reg_above_64_field_set(unit, IHB_FEM_FES_COMPARE_SELECTIONr, reg_above_64[0], XOR_INPUT_SELECTf, field_above_64);

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 166, exit, WRITE_IHB_FEM_FES_COMPARE_SELECTIONr(unit, reg_above_64[0]));
        }

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 250, exit, READ_IHB_PMF_PASS_1_LOOKUPm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 252, exit, WRITE_IHB_PMF_PASS_1_LOOKUPm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 260, exit, READ_IHB_PMF_PASS_2_LOOKUPm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 262, exit, WRITE_IHB_PMF_PASS_2_LOOKUPm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 270, exit, READ_IHB_PMF_INITIAL_KEY_2ND_PASSm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 272, exit, WRITE_IHB_PMF_INITIAL_KEY_2ND_PASSm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

 
        if(SOC_IS_JERICHO_AND_BELOW(unit)){ 
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 280, exit, READ_IHB_PMF_PASS_2_KEY_UPDATEm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 282, exit, WRITE_IHB_PMF_PASS_2_KEY_UPDATEm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        }

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 290, exit, READ_IHB_PMF_PROGRAM_COUNTERSm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 292, exit, WRITE_IHB_PMF_PROGRAM_COUNTERSm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 300, exit, READ_IHB_PMF_PROGRAM_GENERALm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 302, exit, WRITE_IHB_PMF_PROGRAM_GENERALm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));

        
        arad_pmf_fem_pgm_duplicate(unit,stage,pmf_pgm_from,pmf_pgm_to,0);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 320, exit, READ_IHB_PMF_FES_PROGRAMm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 322, exit, WRITE_IHB_PMF_FES_PROGRAMm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        
        arad_pmf_fem_pgm_duplicate(unit,stage,pmf_pgm_from,pmf_pgm_to,ARAD_PMF_LOW_LEVEL_NOF_PROGS);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 340, exit, READ_IHB_PMF_FES_PROGRAMm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 342, exit, WRITE_IHB_PMF_FES_PROGRAMm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));
    }
#ifdef BCM_88660_A0
  else if (SOC_IS_ARADPLUS(unit) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)) {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 400, exit, READ_IHP_FLP_CONSISTENT_HASHING_KEY_GENm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 402, exit, WRITE_IHP_FLP_CONSISTENT_HASHING_KEY_GENm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 404, exit, READ_IHP_FLP_CONSISTENT_HASHING_KEY_GENm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 406, exit, WRITE_IHP_FLP_CONSISTENT_HASHING_KEY_GENm(unit, MEM_BLOCK_ANY, ARAD_PMF_LOW_LEVEL_NOF_PROGS+pmf_pgm_to, table_entry));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 410, exit, READ_IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm(unit, MEM_BLOCK_ANY, pmf_pgm_from, table_entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 412, exit, WRITE_IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm(unit, MEM_BLOCK_ANY, pmf_pgm_to, table_entry));
    }
#endif 
    else {
        
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 350, exit);
    }


    
    
    
    for (cycle_ndx = 0; cycle_ndx < ARAD_PMF_NOF_CYCLES; ++cycle_ndx) {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(unit, stage, pmf_pgm_from, cycle_ndx, &ce_rsrc);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 402, exit);
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(unit, stage, pmf_pgm_to, cycle_ndx, ce_rsrc);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 404, exit);

        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.get(unit, stage, pmf_pgm_from, cycle_ndx, &keys_bmp);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 410, exit);
        if (switch_key_egq) {
            
            keys_bmp = (~keys_bmp) & 0xF;
        }
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.set(unit, stage, pmf_pgm_to, cycle_ndx, keys_bmp);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 412, exit);

        for(ce_indx = 0; ce_indx < nof_ce_in_prog; ++ce_indx) {
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(unit, stage, pmf_pgm_from, cycle_ndx, ce_indx, &sw_db_ce);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 416, exit);
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.set(unit, stage, pmf_pgm_to, cycle_ndx, ce_indx, &sw_db_ce);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 418, exit);
        }

        for(fes_ndx = 0; fes_ndx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_ndx) {
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(unit, stage, pmf_pgm_from, fes_ndx, &fes_sw_db_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 417, exit);
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.set(unit, stage, pmf_pgm_to, fes_ndx, &fes_sw_db_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 419, exit);
        }
    }

    for (db_id_ndx = 0; db_id_ndx < SOC_PPC_FP_NOF_DBS; ++db_id_ndx)
    {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(unit, stage, db_id_ndx, &db_info);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 420, exit);

        
        if (SHR_BITGET(db_info.progs, pmf_pgm_to)) {
            SHR_BITCLR(db_info.progs, pmf_pgm_to);
        }

        
        if (SHR_BITGET(db_info.progs, pmf_pgm_from)) {
            
            SHR_BITSET(db_info.progs, pmf_pgm_to);

            
            *prog_used_cycle_bmp_lcl = db_info.prog_used_cycle_bmp;
            if(SHR_BITGET(prog_used_cycle_bmp_lcl, pmf_pgm_from)) {
                SHR_BITSET(prog_used_cycle_bmp_lcl, pmf_pgm_to);
            }
            else{
                SHR_BITCLR(prog_used_cycle_bmp_lcl, pmf_pgm_to);
            }
            db_info.prog_used_cycle_bmp = *prog_used_cycle_bmp_lcl;

            
            db_info.used_key[pmf_pgm_to][0] = db_info.used_key[pmf_pgm_from][0];
            db_info.used_key[pmf_pgm_to][1] = db_info.used_key[pmf_pgm_from][1];

            if (switch_key_egq) {
                
                db_info.used_key[pmf_pgm_to][0] = (SOC_DPP_DEFS_GET(unit, nof_egress_pmf_keys) - 1) - db_info.used_key[pmf_pgm_to][0];
                db_info.used_key[pmf_pgm_to][1] = (SOC_DPP_DEFS_GET(unit, nof_egress_pmf_keys) - 1) - db_info.used_key[pmf_pgm_to][1];
            }
        }

        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.set(unit, stage, db_id_ndx, &db_info);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 422, exit);
    }

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_db_pmb.bit_range_read(unit, stage, pmf_pgm_from, 0, 0, ARAD_PMF_NOF_DBS, pgm_db_pmf_from);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 430, exit);
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_db_pmb.bit_range_write(unit, stage, pmf_pgm_to, 0, 0, ARAD_PMF_NOF_DBS, pgm_db_pmf_from);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 440, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_pmf_pgm_duplicate()", pmf_pgm_from, pmf_pgm_to);
}


uint32
  arad_pmf_psl_pmf_pgm_update(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                       is_for_tm,
      SOC_SAND_IN  uint32                   presel_bmp_update[ARAD_PMF_NOF_LINES_MAX_ALL_STAGES_LEN], 
      SOC_SAND_IN  uint8                    is_for_all_lines, 
      SOC_SAND_IN  uint32                   pmf_pgm_from,
      SOC_SAND_IN  uint32                   pmf_pgm_to
  )
{
    uint32
        offset_line_ndx,
        is_tm = is_for_tm, 
        res;
    int32
        level_indx,
        line_indx;
    ARAD_PMF_PSL_LEVEL_INFO
        curr_level;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    
    for(level_indx = 0; level_indx < ARAD_PMF_NOF_LEVELS; ++level_indx) 
    {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, level_indx, &curr_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        
        for(line_indx = 0 ; line_indx < curr_level.nof_lines; ++line_indx) 
        {
            if ( (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) && (!(curr_level.lines[line_indx].flags & ARAD_PMF_SEL_LINE_VALID )) ) {
                continue;
            } 
            offset_line_ndx = curr_level.first_index - line_indx;
        
            if (is_for_all_lines || (SHR_BITGET(presel_bmp_update, offset_line_ndx))) 
            {
                
                if (curr_level.lines[line_indx].prog_id == pmf_pgm_from) {

                    
                    curr_level.lines[line_indx].prog_id = pmf_pgm_to;

                    
                    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.set(unit, stage, is_tm, level_indx, &curr_level);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

                    
                    res = arad_pmf_sel_line_hw_write(
                            unit,
                            stage,
                            offset_line_ndx, 
                            &curr_level.lines[line_indx],
                            curr_level.lines[line_indx].prog_id,
                            TRUE
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

                }
            }
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_pmf_pgm_update()", pmf_pgm_from, pmf_pgm_to);
}

STATIC
uint32 arad_pmf_psl_second_pass_res_key_map(SOC_PPC_FP_DATABASE_STAGGERED_INFO *staggred_info, ARAD_PP_IHB_PMF_2ND_PASS_PROGRAM_SELECTION_CAM_TBL_DATA *second_pass_tbl_data ,uint32 src_key_id, uint32 *res_key_id, uint32 *data_ptr, uint32 *mask_ptr)
{

    switch (src_key_id)
    {
    case 0: 
        *res_key_id = staggred_info->presel_res_0_key; 
        *data_ptr = second_pass_tbl_data->tcam_action_0_lsb;
        *mask_ptr = second_pass_tbl_data->mask_tcam_action_0_lsb;
        break;
    case 1: 
        *res_key_id = staggred_info->presel_res_0_key; 
        *data_ptr = second_pass_tbl_data->tcam_action_0_lsb;
        *mask_ptr = second_pass_tbl_data->mask_tcam_action_0_lsb;
        break;
    case 2: 
        *res_key_id = staggred_info->presel_res_0_key; 
        *data_ptr = second_pass_tbl_data->tcam_action_0_lsb;
        *mask_ptr = second_pass_tbl_data->mask_tcam_action_0_lsb;
        break;
    case 3: 
        *res_key_id = staggred_info->presel_res_0_key; 
        *data_ptr = second_pass_tbl_data->tcam_action_0_lsb;
        *mask_ptr = second_pass_tbl_data->mask_tcam_action_0_lsb;
        break;
    case 4: 
        *res_key_id = staggred_info->presel_kaps_key; 
        *data_ptr = second_pass_tbl_data->kaps_payload_lsb;
        *mask_ptr = second_pass_tbl_data->mask_kaps_payload_lsb;
        break;
    }
    return 0;
}
STATIC
uint32 arad_pmf_psl_second_pass_entry_update(ARAD_PMF_PSL *psl, uint32 res_key_id, uint32 *data_ptr , uint32 *mask_ptr)
{

   switch (res_key_id) 
   {
   case 0:
       *data_ptr = psl->second_pass_ingress_pmf.tcam_action_0_lsb;
       *mask_ptr = psl->second_pass_ingress_pmf.mask_tcam_action_0_lsb;
       break;
   case 1:
       *data_ptr = psl->second_pass_ingress_pmf.tcam_action_1_lsb;
       *mask_ptr = psl->second_pass_ingress_pmf.mask_tcam_action_1_lsb;
       break;
   case 2:
       *data_ptr = psl->second_pass_ingress_pmf.tcam_action_2_lsb;
       *mask_ptr = psl->second_pass_ingress_pmf.mask_tcam_action_2_lsb;
       break;
   case 3:
       *data_ptr = psl->second_pass_ingress_pmf.tcam_action_3_lsb;
       *mask_ptr = psl->second_pass_ingress_pmf.mask_tcam_action_3_lsb;
       break;
    default: 
       break;
   }
   return 0;
}



uint32
arad_pmf_psl_second_pass_update(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_INOUT  SOC_PPC_FP_DATABASE_STAGGERED_INFO    *db_staggered_info,
      SOC_SAND_IN  uint32                   pmf_pgm_from,
      SOC_SAND_IN  uint32                   pmf_pgm_to
      )
    {
        uint32
            staggered_idx,
            key_id,
            res_key = 0,
            action_lsb_data,
            action_lsb_mask,
            res;
        ARAD_PMF_PSL
            psl;
        ARAD_PP_IHB_PMF_2ND_PASS_PROGRAM_SELECTION_CAM_TBL_DATA second_pass_tbl_data;

        SOC_SAND_INIT_ERROR_DEFINITIONS(0);

        ARAD_CLEAR(&second_pass_tbl_data, ARAD_PP_IHB_PMF_2ND_PASS_PROGRAM_SELECTION_CAM_TBL_DATA, 1);
        
         if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) && (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE))  ) 
         {
            for (staggered_idx = 0; staggered_idx < SOC_DPP_NOF_INGRESS_PMF_PROGRAM_SELECTION_LINES_ARAD ; staggered_idx++ ) 
            {
               if (db_staggered_info[staggered_idx].valid) 
               {
                   res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.second_pass_psl_info.get(unit,stage,db_staggered_info[staggered_idx].second_pass_presel_id, &psl);
                   SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
                   psl.second_pass_ingress_pmf.program_selection_profile = pmf_pgm_from;
                   for (key_id = 0 ; key_id < 5; key_id ++) 
                   {
                       res = arad_pmf_psl_second_pass_res_key_map(&db_staggered_info[staggered_idx],&second_pass_tbl_data, key_id, &res_key, &action_lsb_data, &action_lsb_mask);
                       SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

                       res = arad_pmf_psl_second_pass_entry_update(&psl,res_key,&action_lsb_data, &action_lsb_mask);
                       SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

                   }     
                       
                   res = arad_pp_ihb_pmf_2nd_pass_program_selection_cam_tbl_set_unsafe(unit,db_staggered_info[staggered_idx].second_pass_presel_id,&second_pass_tbl_data);
                   SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
               }
            }
         }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_pmf_pgm_update()", pmf_pgm_from, pmf_pgm_to);
}


uint32
  arad_pmf_psl_expected_match_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  ARAD_PMF_PSL              *psl,
      SOC_SAND_OUT uint32                  groups[ARAD_PMF_GROUP_LEN],
      SOC_SAND_OUT uint32                  *prog_id
  )
{
    ARAD_PMF_SEL_GROUP
        pmf_group;
    ARAD_PMF_PSL_LEVEL_INFO
        curr_level;
    uint32
        is_tm = FALSE,  
        res;
    int32
        line_indx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pmf_psl_to_group_map(
            unit,
            stage,
            psl,
            &pmf_group
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "check:\n\r")));
    ARAD_PMF_SEL_GROUP_print(&pmf_group);
    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "\n\r")));

    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, 1, &curr_level);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    sal_memset(groups, 0x0, sizeof(uint32) * ARAD_PMF_GROUP_LEN);
    *prog_id = 0;

    for(line_indx = curr_level.nof_lines-1 ; line_indx >= 0 ; --line_indx) 
    {
        if (!arad_pmf_sel_group_is_disjoint(&curr_level.lines[line_indx].group, &pmf_group)) 
        {
            soc_sand_bitstream_or(groups, curr_level.lines[line_indx].groups, ARAD_PMF_GROUP_LEN);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_expected_match_get()", 0, 0);
}


uint32 
  arad_pmf_sel_get_line_at_index(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN     uint32              index,
      SOC_SAND_OUT    ARAD_PMF_PSL_LINE_INFO   *line  
      
  )
{
    int32
      level_indx,
        is_tm = FALSE;  
    ARAD_PMF_PSL_LEVEL_INFO
        cur_level;
    soc_error_t
        rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for(level_indx = ARAD_PMF_NOF_LEVELS-1; level_indx >= 0; --level_indx) 
    {
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, level_indx, &cur_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);


        if(index >= cur_level.first_index  && index <=  cur_level.last_index ) 
        {
            sal_memcpy(line,&cur_level.lines[cur_level.first_index - index],sizeof(ARAD_PMF_PSL_LINE_INFO));
            goto exit;
        }

    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_sel_get_line_at_index()",0,0);
}
uint32
  arad_pmf_sel_table_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 line_indx,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE stage,
    SOC_SAND_OUT uint32 *valid,
    SOC_SAND_OUT uint32 *program,
    SOC_SAND_OUT ARAD_PMF_PSL *psl_tbl_data
  )
{
  uint32
	res = SOC_SAND_OK;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) 
  {
    res = arad_pp_egq_pmf_program_selection_cam_tbl_get_unsafe(
            unit,
            line_indx,
            &(psl_tbl_data->egress_pmf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    *valid = psl_tbl_data->egress_pmf.valid;
    *program = psl_tbl_data->egress_pmf.program;
  }
#ifdef BCM_88660_A0
  else if (SOC_IS_ARADPLUS(unit) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB))
  {
    res = arad_pp_ihb_slb_program_selection_cam_tbl_get_unsafe(
            unit,
            line_indx,
            &(psl_tbl_data->ingress_slb)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    *valid = psl_tbl_data->ingress_slb.valid;
    *program = psl_tbl_data->ingress_slb.program;
  }
#endif 
  else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)
  {
    res = arad_pp_ihb_pmf_program_selection_cam_tbl_get_unsafe(
            unit,
            line_indx,
            &(psl_tbl_data->ingress_pmf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    *valid = psl_tbl_data->ingress_pmf.valid;
    *program = psl_tbl_data->ingress_pmf.program;
  }
  else {
      
      SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_psl_table_get()", 0, 0);
}

uint32
  arad_pmf_psl_match_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  ARAD_PMF_PSL              *psl,
      SOC_SAND_OUT uint32                  groups[ARAD_PMF_GROUP_LEN],
      SOC_SAND_OUT uint32                  *prog_id
  )
{
    ARAD_PMF_SEL_GROUP
        pmf_group,
        line_group;
    uint32
        valid,
        program,
        res;
    ARAD_PMF_PSL
        tbl_data;
    int32
        line_indx;
    ARAD_PMF_PSL_LINE_INFO
        match_line;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pmf_psl_to_group_map(
            unit,
            stage,
            psl,
            &pmf_group
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    sal_memset(groups, 0x0, sizeof(uint32) * ARAD_PMF_GROUP_LEN);
    *prog_id = 0;

    for(line_indx = 0 ; line_indx < ARAD_PMF_NOF_LINES ; ++line_indx) 
    {
        
        res = arad_pmf_sel_table_get(
            unit, 
            line_indx, 
            stage, 
            &valid, 
            &program, 
            &tbl_data
        ); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

        
        if(!valid) 
        {
            continue;
        }

        res = arad_pmf_psl_to_group_map(
                unit,
                stage,
                &tbl_data,
                &line_group
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        if (!arad_pmf_sel_group_is_disjoint(&line_group, &pmf_group)) 
        {
            *prog_id = program;
            res = arad_pmf_sel_get_line_at_index(unit, stage, line_indx,&match_line);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            soc_sand_bitstream_or(groups, match_line.groups, ARAD_PMF_GROUP_LEN);
            
            break;
        }

    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_match_get()", 0, 0);
}

uint32
  arad_pmf_prog_select_eth_section_nof_lines_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
    SOC_SAND_OUT uint32                   *nof_eth_lines
  )
{
    uint32 min_indx, max_indx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_FUNC_RESULT(arad_pmf_prog_select_line_borders_get(unit,stage, ARAD_PMF_PSL_TYPE_ETH, &min_indx,&max_indx), 25, exit);

    *nof_eth_lines = max_indx-min_indx + 1;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_prog_select_eth_section_nof_lines_get()",0,0);

}

uint32
  arad_pp_fp_get_sw_presel_id(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         line_indx,
    SOC_SAND_IN  uint32                         prog_id,
    SOC_SAND_OUT int                            *sw_presel_id
  )
{
    uint32
      res,
      level_indx;
    bcm_field_presel_t
      presel_id;
    ARAD_PMF_PSL_LEVEL_INFO
      curr_level;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    *sw_presel_id = BCM_FIELD_ENTRY_INVALID;

    for(level_indx = 0; level_indx < ARAD_PMF_NOF_LEVELS; ++level_indx) {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, FALSE, level_indx, &curr_level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        presel_id = curr_level.first_index - line_indx;
        if ((soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) && (!(curr_level.lines[presel_id].flags & ARAD_PMF_SEL_LINE_VALID))) {
            continue;
        }

        if (curr_level.lines[presel_id].prog_id == prog_id) {
            *sw_presel_id = presel_id;
            LOG_DEBUG(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "MATCH line_indx %d, program %d, sw_presel_id 0x%X\n\r"),
                        line_indx, prog_id, *sw_presel_id));

            break;
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_get_sw_presel_id()", 0, 0);
}


uint32
  arad_pp_fp_map_program_to_sw_presel_id(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         prog_id,
    SOC_SAND_IN  uint32                         db_id,
    SOC_SAND_OUT int                            *sw_presel_id
  )
{
    int line_idx;
    uint32 res = SOC_E_NONE;
    bcm_field_group_presel_info_t presel_info;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for(line_idx = 0 ; line_idx < ARAD_PMF_NOF_LINES; line_idx++)
    {
        res = arad_pp_fp_get_sw_presel_id(unit, stage, line_idx, prog_id, sw_presel_id);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 340, exit);

        if (*sw_presel_id >= 0 && *sw_presel_id < BCM_FIELD_NOF_PRESEL_PER_FG)
        {
            SOC_SAND_CHECK_FUNC_RESULT(FIELD_ACCESS.groupHwInfo.presel_info.get(unit, db_id, *sw_presel_id, &presel_info), 25, exit);
            if (presel_info.presel_id == *sw_presel_id)
            {
                LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "MATCH line_idx %d, program %d, sw_presel_id 0x%X\n\r"),
                            line_idx, prog_id, *sw_presel_id));

                return res;
            }
        }
    }
    res = SOC_E_LIMIT;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 340, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_map_program_to_sw_presel_id()", 0, 0);
}


#if ARAD_DEBUG_IS_LVL1

void ARAD_PMF_SEL_GROUPS_print(
    SOC_SAND_IN uint32 groups[ARAD_PMF_GROUP_LEN]
  )
{
    uint32
        indx;
   
    for(indx  = 0; indx < ARAD_BIT_TO_U32(ARAD_PMF_GROUP_LEN); ++indx) 
    {
      LOG_CLI((BSL_META("%08x "),groups[indx]));
    }
    
    return;
}

void ARAD_PMF_SEL_GROUP_print(
    SOC_SAND_IN ARAD_PMF_SEL_GROUP *group
  )
{
    uint32
        bit_indx,
        mask_val,
        val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(group);

    for(bit_indx = 0; bit_indx < ARAD_PMF_SEL_LINE_NOF_BITS; ++bit_indx) {
        mask_val = soc_sand_bitstream_test_bit(group->mask, bit_indx);
        if(mask_val == 1) 
        {
            LOG_CLI((BSL_META_U(unit,
                                "*")));
        }
        else
        {
            val = soc_sand_bitstream_test_bit(group->value, bit_indx);
            LOG_CLI((BSL_META_U(unit,
                                "%u"), val));
        }
        if((bit_indx+1)%32 == 0) 
        {
            LOG_CLI((BSL_META_U(unit,
                                " ")));
        }
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void ARAD_PMF_LINE_INFO_print(
    SOC_SAND_IN ARAD_PMF_PSL_LINE_INFO *line
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(line);
    LOG_CLI((BSL_META_U(unit,
                        " index %u:"), line->line_id));
    ARAD_PMF_SEL_GROUP_print(&(line->group));
    LOG_CLI((BSL_META_U(unit,
                        "  ")));
    LOG_CLI((BSL_META_U(unit,
                        "groups:")));
    ARAD_PMF_SEL_GROUPS_print(line->groups);
    LOG_CLI((BSL_META_U(unit,
                        " prog: %u"),line->prog_id));

    if(line->flags & ARAD_PMF_SEL_LINE_REMOVE) 
    {
        LOG_CLI((BSL_META_U(unit,
                            "  X")));
    }
    if(line->flags & ARAD_PMF_SEL_LINE_VALID) 
    {
        LOG_CLI((BSL_META_U(unit,
                            "  V")));
    }
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void ARAD_PMF_LEVEL_INFO_print(
    SOC_SAND_IN ARAD_PMF_PSL_LEVEL_INFO *level,
    SOC_SAND_IN  uint32           flavor
  )
{
    int32
        line_indx;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(level);

    if(level->nof_lines == 0 && level->nof_new_lines == 0) {
        if(flavor == 0) {
            LOG_CLI((BSL_META_U(unit,
                                "level %u: "),level->level_index));
            LOG_CLI((BSL_META_U(unit,
                                "EMPTY \n\r")));
        }
        goto exit;
    }
    if(flavor <= 1) {
        LOG_CLI((BSL_META_U(unit,
                            "level %u: "),level->level_index));
        
        LOG_CLI((BSL_META_U(unit,
                            "[%u,%u] \n\r"),level->first_index, level->last_index));
    }
    
    
    for(line_indx = level->nof_lines-1 ; line_indx >= 0 ; --line_indx) 
    {
        ARAD_PMF_LINE_INFO_print(&level->lines[line_indx]);
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }
    if(level->nof_new_lines > 0) 
    {
        LOG_CLI((BSL_META_U(unit,
                            "new lines: \n\r")));
    }
    
    
    for(line_indx = level->nof_lines + level->nof_new_lines - 1 ; line_indx >= (int32)level->nof_lines ; --line_indx)
    {
        ARAD_PMF_LINE_INFO_print(&level->lines[line_indx]);
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  arad_pmf_prog_select_print_all(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                  flavor
  )
{
    ARAD_PMF_PSL_LEVEL_INFO
        level;
    int32
        level_indx;
    uint32
        is_tm = FALSE; 
    soc_error_t 
        rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
     for(level_indx = ARAD_PMF_NOF_LEVELS - 1; level_indx >= 0; --level_indx) 
    {
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.pmf.psl_info.levels_info.get(unit, stage, is_tm, level_indx, &level);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

        ARAD_PMF_LEVEL_INFO_print(&level,flavor);
     }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_pmf_prog_select_test1(
    SOC_SAND_IN  int                 unit
  )
{
    ARAD_PMF_PSL
        psl1;
    uint8
        success;
    uint32
        res;
    SOC_PPC_FP_DATABASE_STAGE
        stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    ARAD_PMF_PSL_clear(unit, stage, &psl1, TRUE,TRUE);

    

    
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x00f;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x550;
   
    res = arad_pmf_psl_add(unit, stage, 5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    

    
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x00f;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x650;

    res = arad_pmf_psl_add(unit, stage,  5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    arad_pmf_prog_select_print_all(unit, stage, 2);

    
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x0ff;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x600;

    res = arad_pmf_psl_add(unit, stage,  5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    arad_pmf_prog_select_print_all(unit, stage, 2);

    
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x007;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x658;

    res = arad_pmf_psl_add(unit, stage,  5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    arad_pmf_prog_select_print_all(unit, stage, 2);

    
    psl1.ingress_pmf.mask_packet_format_qualifier_fwd = 0x00c;
    psl1.ingress_pmf.packet_format_qualifier_fwd = 0x653;

    res = arad_pmf_psl_add(unit, stage,  5, 0, &psl1, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    arad_pmf_prog_select_print_all(unit, stage, 2);
   
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void arad_pmf_prog_test_rand_val(
        SOC_SAND_IN uint32 total_bits,
        SOC_SAND_IN uint32 unmasked_bits,
        SOC_SAND_OUT uint32 *val,
        SOC_SAND_OUT uint32 *mask
     )
{
    uint32
        tmp;
    *mask = soc_sand_os_rand() & ((1 << (unmasked_bits))-1);
    tmp = (((1 << (total_bits-unmasked_bits))-1) << unmasked_bits);
    *mask |= tmp;
    *val = soc_sand_os_rand() & ((1 << (unmasked_bits))-1);
    *val = *val & (~*mask);
}



uint32
  arad_pmf_psl_check(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  ARAD_PMF_PSL              *psl,
      SOC_SAND_OUT uint8                  *success
  )
{
    uint32                  
        expect_groups[ARAD_PMF_GROUP_LEN],
        act_groups[ARAD_PMF_GROUP_LEN];
    uint32                  
        expect_prog_id,
        act_prog_id;
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pmf_psl_expected_match_get(
            unit,
            stage,
            psl,
            expect_groups,
            &expect_prog_id
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "expected group: ")));
    ARAD_PMF_SEL_GROUPS_print(expect_groups);
    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "\n\r")));


    res = arad_pmf_psl_match_get(
            unit,
            stage,
            psl,
            act_groups,
            &act_prog_id
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "act group: ")));
    ARAD_PMF_SEL_GROUPS_print(act_groups);
    LOG_INFO(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "\n\r")));


    if(sal_memcmp(expect_groups,act_groups,sizeof(uint32)*ARAD_PMF_GROUP_LEN))
    {
        LOG_INFO(BSL_LS_SOC_FP,
                 (BSL_META_U(unit,
                             "****FAIL****\n\r")));
        *success = FALSE;
    }

    *success = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_check()",0,0);
}


void
  arad_pmf_prog_select_test2(
    SOC_SAND_IN  int                 unit
  )
{
    ARAD_PMF_SEL_GROUP
        pmf_group;
    ARAD_PMF_PSL
        add_psl,
        find_psl;
    uint32
        total_bits = 11,
        nof_bits = 5,
        nof_psls = 10,
        nof_lookups = 2,
        psl_index,
        lkup_indx,
        tmp_mask;
    uint8
        match;
    uint8
        success;
    uint32
        res;
    SOC_PPC_FP_DATABASE_STAGE
        stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_srand(1);

    for(psl_index = 0; psl_index < nof_psls; ++psl_index) 
    {
        
        ARAD_PMF_PSL_clear(unit, stage, &add_psl, TRUE,TRUE);

        
        ARAD_PMF_PSL_clear(unit, stage, &find_psl, TRUE,FALSE);

        
        arad_pmf_prog_test_rand_val(total_bits, nof_bits,&add_psl.ingress_pmf.packet_format_qualifier_fwd, &add_psl.ingress_pmf.mask_packet_format_qualifier_fwd);


       
        res = arad_pmf_psl_to_group_map(
                unit,
                stage,
                &add_psl,
                &pmf_group
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        LOG_INFO(BSL_LS_SOC_FP,
                 (BSL_META_U(unit,
                             "adding (%u): \n\r"), psl_index));
        ARAD_PMF_SEL_GROUP_print(&pmf_group);
        LOG_INFO(BSL_LS_SOC_FP,
                 (BSL_META_U(unit,
                             "\n\r")));

        res = arad_pmf_psl_add(unit, stage,  psl_index, 0, &add_psl, &success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        arad_pmf_prog_select_print_all(unit, stage, 1);

        for(lkup_indx = 0; lkup_indx < nof_lookups; ++lkup_indx) 
        {
            arad_pmf_prog_test_rand_val(total_bits, nof_bits,&find_psl.ingress_pmf.packet_format_qualifier_fwd, &tmp_mask);
            res = arad_pmf_psl_check(unit, stage, &find_psl, &match);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            if(!match) 
            {
                LOG_INFO(BSL_LS_SOC_FP,
                         (BSL_META_U(unit,
                                     "%u\n\r"), lkup_indx));
                goto exit;
            }
        }

    }


exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32
  arad_pmf_psl_slb_field_update(
      SOC_SAND_IN    int                    unit,
      SOC_SAND_INOUT SOC_PPC_FP_QUAL_TYPE   *type,
      SOC_SAND_IN    ARAD_PMF_PSL           *psl,
      SOC_SAND_IN    SOC_PPC_FP_QUAL_VAL    *qual_val
  )
{
    uint32 frwd_code;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *type = qual_val->type;
    frwd_code = psl->ingress_slb.forwarding_code;

    if (psl->ingress_slb.forwarding_code_mask != 0 &&
        (*type == SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL || *type == SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL)) {
        LOG_ERROR(BSL_LS_SOC_FP,(BSL_META_U(unit,"Forward header must be configured before next protocol configuration.\n\r")));
                        SOC_SAND_SET_ERROR_CODE(ARAD_INCONSISTENCY_ERR, 1, exit);
    } else if (*type == SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL  &&
               ( (frwd_code == SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC) || (frwd_code == SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC) ||
                 (frwd_code == SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC) || (frwd_code == SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC) ) ) {
               
               *type = SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_psl_slb_field_update",0,0);
}

#endif  



#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

#endif 

