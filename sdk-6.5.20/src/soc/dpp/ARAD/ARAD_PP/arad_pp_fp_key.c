#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_FP

#include <shared/bsl.h>



#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>


#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_fem.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_tcam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>

#if defined(INCLUDE_KBP)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif









#define ARAD_PP_FP_KEY_SWAP_PRIORITY_FIRST                  (100)

#define ARAD_PP_FP_KEY_SWAP_PRIORITY_BTW_FIRST_SECOND       (95)
#define ARAD_PP_FP_KEY_SWAP_PRIORITY_SECOND                 (90)
#define ARAD_PP_FP_KEY_SWAP_PRIORITY_THIRD                  (80)
#define ARAD_PP_FP_KEY_SWAP_PRIORITY_FOURTH                 (70)
#define ARAD_PP_FP_KEY_SWAP_PRIORITY_FIFTH                  (60)


#define ARAD_PP_FP_KEY_NOF_CATEGORIES (8)
#define ARAD_PP_FP_KEY_CATEGORY_GET(is_32b, is_msb, is_high_group) (((is_32b) << 2) + ((is_msb) << 1) + (is_high_group))



#define ARAD_PP_FP_KEY_FROM_CASCADED_KEY_GET(cascaded_key) (cascaded_key - 1)
#define ARAD_PP_FP_KEY_TO_CASCADED_KEY_GET(real_key) ((uint8) (real_key + 1))


#define ARAD_PP_FP_KEY_NUMBER_80_PARTS_IN_320B (4)











typedef enum 
{
    ARAD_PP_FP_QUAL_CE_16_BITS = 0,
    ARAD_PP_FP_QUAL_CE_32_BITS = 1,
    ARAD_PP_FP_QUAL_NOF_CE_SIZES = 2
} ARAD_PP_FP_QUAL_CE_SIZES;

typedef enum 
{
    ARAD_PP_FP_KEY_BUFF_LSB = 0,
    ARAD_PP_FP_KEY_BUFF_MSB = 1,
    ARAD_PP_FP_KEY_NOF_BUFFERS = 2
} ARAD_PP_FP_KEY_BUFF_TYPES;

typedef struct
{
    uint8   is_header;
    uint8   is_msb;
    uint8   is_lsb;
    uint8   nof_ce;
    uint8   place_cons[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    uint8   group_cons[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    uint8   size_cons[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    uint32  ce_zone_id[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    uint32  ce_nof_bits[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    uint32  ce_qual_lsb[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    uint32  ce_lost_bits[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    uint8   qual_size; 
    uint8   lost_bits; 
    uint8   lost_bits_worst; 

} ARAD_PP_FP_QUAL_CE_INFO;


typedef enum 
{
    ARAD_PP_FP_VT_MATCH_MODE_IPV4 = 0,
    ARAD_PP_FP_VT_MATCH_MODE_EFP = 1,
    ARAD_PP_FP_QUAL_NOF_MATCH_MODES = 2
} ARAD_PP_FP_VT_MATCH_MODES;


typedef struct
{
    uint8 balance_enabled;
    uint8 balance_lsb_msb;

} ARAD_PP_FP_ALLOC_ALG_ARGS;





CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_fp_key[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_LENGTH_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_LENGTH_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_LENGTH_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_LENGTH_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_SPACE_ALLOC),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_SPACE_ALLOC_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_SPACE_ALLOC_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_SPACE_ALLOC_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_SPACE_FREE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_SPACE_FREE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_SPACE_FREE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_SPACE_FREE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_DESC_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_DESC_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_DESC_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_DESC_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_GET_ERRS_PTR),
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_HEADER_NDX_FIND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_NOF_VLAN_TAGS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_TOTAL_SIZE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_CE_INSTR_BOUNDS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_CE_INSTR_INSTALL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_KEY_NEW_DB_POSSIBLE_CONFS_GET_UNSAFE),



  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_fp_key[] =
{
  
  {
    ARAD_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pfg' is out of range. \n\r "
    "The range is: No min - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_KEY_DB_ID_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_KEY_DB_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'db_id_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_KEY_CYCLE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_KEY_CYCLE_OUT_OF_RANGE_ERR",
    "The parameter 'cycle' is out of range. \n\r "
    "The range is: No min - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_KEY_LSB_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_KEY_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'lsb' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_KEY_LENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_KEY_LENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'length' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_KEY_KEY_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_KEY_KEY_OUT_OF_RANGE_ERR",
    "The parameter 'key' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_FP_PMF_KEYS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR,
    "ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR",
    "The requested qualifier is not supported.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_KEY_BAD_PADDING_ERR,
    "ARAD_PP_FP_KEY_BAD_PADDING_ERR",
    "The hardware cannot add the requested amount of padding bits.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_KEY_COPY_OVERFLOW_ERR,
    "ARAD_PP_FP_KEY_COPY_OVERFLOW_ERR",
    "Attempted read beyond the end of the requested qualifier.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_KEY_TOO_MANY_BITS_ERR,
    "ARAD_PP_FP_KEY_TOO_MANY_BITS_ERR",
    "The hardware cannot copy the requested amount of bits.\n\r"
    "The maximum number of bits per copy instruction is 32.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  



  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};


static 
    uint32
        Arad_pp_fp_place_const_convert[ARAD_PP_FP_NOF_KEY_CE_PLACES][6] = {
            
            {ARAD_PP_FP_KEY_CE_LOW                    , 1, ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS,   1, 1,               1},
            {ARAD_PP_FP_KEY_CE_HIGH                   , 1, ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS,  1, 0,               1},
            {ARAD_PP_FP_KEY_CE_PLACE_ANY_NOT_DOUBLE   , 1, ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS,  2, 1,               0},
            {ARAD_PP_FP_KEY_CE_PLACE_ANY              , 2, ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS,  4, 1,               0}
        };





STATIC
uint32 arad_pp_fp_kbp_match_key_ces_get(      
          SOC_SAND_IN  int                    unit,
          SOC_SAND_IN  uint32                 table_id,
          SOC_SAND_OUT ARAD_PMF_CE      *ce_array,
          SOC_SAND_OUT uint8                  *nof_ces
       )
{
#if defined(INCLUDE_KBP)
    const ARAD_PMF_CE ce_array_kbp_ipv6_match[] = {
                {1,   0, 0, 15, 0, 0, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF},
                {1,   0, 0, 31, 0, 0, 0, 32, SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH},
                {1,   0, 0, 31, 0, 0, 0, 0, SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH},
                {1,   0, 0, 31, 0, 0, 0, 32, SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW},
                {1,   0, 0, 31, 0, 0, 0, 0, SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW}
               };

    const ARAD_PMF_CE ce_array_kbp_match[] = {
                {1,   0, 0, 2,  0, 0, 0, 0, SOC_PPC_FP_QUAL_HDR_MPLS_EXP_FWD},
                {1,   0, 0, 19, 0, 0, 0, 0, SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD},
                {1,   0, 0, 0,  0, 0, 0, 0, SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD},
                {1,   0, 0, 15, 0, 0, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF},
                {1,   0, 0, 31, 0, 0, 0, 0, SOC_PPC_FP_QUAL_HDR_IPV4_DIP}
               };

    const ARAD_PMF_CE ce_array_kbp_p2p_match[] = {
                {1,   0, 0, 15, 0, 0, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_LIF}
               };

    const ARAD_PMF_CE ce_array_kbp_rif_match[] = {
                {1,   0, 0, 15, 0, 0, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF}
               };
	
	const ARAD_PMF_CE ce_array_kbp_ipv4_mc_match[] = {
		{1,   0, 0, 15, 0, 0, 0, 0, SOC_PPC_FP_QUAL_IRPP_VRF},
		{1,   0, 0, 15, 0, 0, 0, 0, SOC_PPC_FP_QUAL_IRPP_IN_RIF},
		{1,   0, 0, 31, 0, 0, 0, 0, SOC_PPC_FP_QUAL_HDR_IPV4_DIP},
		{1,   0, 0, 31, 0, 0, 0, 0, SOC_PPC_FP_QUAL_HDR_IPV4_SIP}
	};
	    

    switch (table_id) {
        case ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED:
            (*nof_ces) = sizeof(ce_array_kbp_match)/sizeof(ARAD_PMF_CE);
            sal_memcpy(ce_array,&ce_array_kbp_match,sizeof(ARAD_PMF_CE)* (*nof_ces) );
            break;

        case ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6:
            (*nof_ces) = sizeof(ce_array_kbp_ipv6_match)/sizeof(ARAD_PMF_CE);
            sal_memcpy(ce_array,&ce_array_kbp_ipv6_match,sizeof(ARAD_PMF_CE)* (*nof_ces) );        
            break;

        case ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P:
            (*nof_ces) = sizeof(ce_array_kbp_p2p_match)/sizeof(ARAD_PMF_CE);
            sal_memcpy(ce_array,&ce_array_kbp_p2p_match,sizeof(ARAD_PMF_CE)* (*nof_ces) );
            break;

        case ARAD_KBP_FRWRD_TBL_ID_INRIF_MAPPING:
            (*nof_ces) = sizeof(ce_array_kbp_rif_match)/sizeof(ARAD_PMF_CE);
            sal_memcpy(ce_array,&ce_array_kbp_rif_match,sizeof(ARAD_PMF_CE)* (*nof_ces) );
            break;

        case ARAD_KBP_FRWRD_TBL_ID_IPV4_MC:
            (*nof_ces) = sizeof(ce_array_kbp_ipv4_mc_match)/sizeof(ARAD_PMF_CE);
            sal_memcpy(ce_array,&ce_array_kbp_ipv4_mc_match,sizeof(ARAD_PMF_CE)* (*nof_ces) );
            break;
    default:
        return -1;
    }



    return SOC_SAND_OK;
#else
    return -1;
#endif
}

STATIC
    uint32
        arad_pp_fp_key_access_profile_hw_set(
            SOC_SAND_IN  int                       unit,
            SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
            SOC_SAND_IN  uint32                       prog_ndx,
            SOC_SAND_IN  uint32                       access_profile_array_id,
            SOC_SAND_IN  uint32                       access_profile_id,
            SOC_SAND_IN  ARAD_PP_FP_KEY_ALLOC_INFO   *alloc_info
        )
{
    uint32
      tbl_data[2];
    soc_mem_t
        ce_table;
    uint8
        cycle;
    soc_field_t
        fields[] = {TCAM_DB_PROFILE_KEY_Af,TCAM_DB_PROFILE_KEY_Bf,TCAM_DB_PROFILE_KEY_Cf,TCAM_DB_PROFILE_KEY_Df};
    soc_field_t
        qax_fields[] = {TCAM_DB_PROFILE_KEY_0f,TCAM_DB_PROFILE_KEY_1f,TCAM_DB_PROFILE_KEY_2f,TCAM_DB_PROFILE_KEY_3f};
    uint32
        access_profile_id_lcl,
      res = SOC_SAND_OK;
    soc_reg_t
        egq_key_data_base_profile[2] = {EGQ_KEYA_DATA_BASE_PROFILEr, EGQ_KEYB_DATA_BASE_PROFILEr};
    soc_reg_above_64_val_t
            reg_above_64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);

    
    if (stage ==  SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        access_profile_id_lcl = access_profile_id;

#ifdef ARAD_TCAM_EGQ_DUMMY_ACCESS_PROFILE_WA_ENABLE
        
        if(!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_pmf_lookups_always_valid_disable", 0))
        {
            if (access_profile_id_lcl == ARAD_TCAM_NOF_ACCESS_PROFILE_IDS) {
                access_profile_id_lcl = ARAD_TCAM_EGQ_DUMMY_ACCESS_PROFILE_NO_LOOKUP;
            }
        }
#endif 

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 122, exit, soc_reg_above_64_get(unit, egq_key_data_base_profile[alloc_info->key_id[access_profile_array_id]], REG_PORT_ANY, 0, reg_above_64));
        SHR_BITCOPY_RANGE(reg_above_64, (ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS * prog_ndx), &access_profile_id_lcl, 0, ARAD_PMF_KEY_TCAM_DB_PROFILE_NOF_BITS);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 124, exit, soc_reg_above_64_set(unit, egq_key_data_base_profile[alloc_info->key_id[access_profile_array_id]], REG_PORT_ANY, 0, reg_above_64));
    }
    else if (stage ==  SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
        cycle = (alloc_info->cycle == ARAD_PP_FP_KEY_CYCLE_EVEN)?0:1;
        ce_table = (cycle == 0)? IHB_PMF_PASS_1_LOOKUPm : IHB_PMF_PASS_2_LOOKUPm;
        res = soc_mem_read(
                unit,
                ce_table,
                MEM_BLOCK_ANY,
                prog_ndx, 
                tbl_data
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);
        if (alloc_info->use_kaps)
        {
            soc_mem_field32_set(unit, ce_table, tbl_data, KAPS_LOOKUP_SELECTf, alloc_info->key_id[access_profile_array_id]);
        } 
        else {
            if (SOC_IS_JERICHO_PLUS(unit)) {
                soc_mem_field32_set(unit, ce_table, tbl_data, qax_fields[alloc_info->key_id[access_profile_array_id]], access_profile_id); 
            } else {
                soc_mem_field32_set(unit, ce_table, tbl_data, fields[alloc_info->key_id[access_profile_array_id]], access_profile_id); 
            }
        }

        res = soc_mem_write(
                unit,
                ce_table,
                MEM_BLOCK_ANY,
                prog_ndx, 
                tbl_data
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);
    }
    else {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 150, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_access_profile_hw_set()", 0, 0);
}



uint32
  arad_pp_fp_db_prog_info_get(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                           db_ndx,
      SOC_SAND_IN  uint32                           prog_ndx,
      SOC_SAND_OUT  ARAD_PP_FP_KEY_DP_PROG_INFO     *db_prog_info
  )
{
    ARAD_PMF_DB_INFO
      db_info;
    uint32
        ce_indx;
    ARAD_PMF_CE
      sw_db_ce;
    uint32
      prog_used_cycle_bmp_lcl[1];
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    db_prog_info->nof_ces = 0;

    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(
            unit,
            stage,
            db_ndx,
            &db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    
    if(!SHR_BITGET(db_info.progs,prog_ndx)){
        db_prog_info->used = 0;
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Unit %d, Stage %s, Data Base Id %d, Program %d - Failed to get the information; The Data Base doen\'t include The program.\n\r"),
                   unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), db_ndx, prog_ndx));
        goto exit;
    }

    prog_used_cycle_bmp_lcl[0] = db_info.prog_used_cycle_bmp;
    db_prog_info->cycle = SHR_BITGET(prog_used_cycle_bmp_lcl, prog_ndx) ? 1 : 0;
    db_prog_info->is_320b = db_info.is_320b;
    db_prog_info->alloc_place = db_info.alloc_place;
    db_prog_info->key_id[0] = db_info.used_key[prog_ndx][0];
    db_prog_info->key_id[1] = db_info.used_key[prog_ndx][1];

    
    for(ce_indx = 0; ce_indx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG; ++ce_indx) {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(
                unit,
                stage,
                prog_ndx,
                db_prog_info->cycle,
                ce_indx,
                &sw_db_ce
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
        if (sw_db_ce.is_used && sw_db_ce.is_ce_shared)
        {
            int ii;
            for (ii = 0; ii < 8; ii++)
            {
                if (sw_db_ce.ce_sharing_info[ii].db_id == db_ndx)
                {
                    db_prog_info->ces[db_prog_info->nof_ces] = ce_indx;
                    db_prog_info->nof_ces++;
                    LOG_DEBUG(BSL_LS_SOC_FP,
                              (BSL_META_U(unit,
                                          "Unit %d, Stage %s, Data Base Id %d, Program %d ce %d nof_ces %d.\n\r"),
                               unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), db_ndx, prog_ndx, ce_indx, db_prog_info->nof_ces));
                }
            }
        }
        else
        {
            if(sw_db_ce.is_used && sw_db_ce.db_id == db_ndx) {
                db_prog_info->ces[db_prog_info->nof_ces] = ce_indx;
                db_prog_info->nof_ces++;
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_db_prog_info_get()", 0, 0);
}

soc_error_t
  arad_pp_fp_is_qual_in_qual_arr(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE  *qual_types,
      SOC_SAND_IN  uint32                nof_qual_types,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE  search_qual,
      SOC_SAND_OUT uint8                 *found
  )
{
    int qual_idx;
    SOCDNX_INIT_FUNC_DEFS;

    *found = FALSE;

    for (qual_idx = 0; qual_idx < nof_qual_types; qual_idx++, qual_types++) {
        if (*qual_types == search_qual) {
            *found = TRUE;
            SOC_EXIT;
        }
        
        
        if (SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(*qual_types)) {
            SOC_PPC_FP_CONTROL_INDEX index;
            SOC_PPC_FP_CONTROL_INFO info;
            uint32 sand_res;

            SOC_PPC_FP_CONTROL_INDEX_clear(&index);
            SOC_PPC_FP_CONTROL_INFO_clear(&info);

            index.type = SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF;
            index.val_ndx = *qual_types - SOC_PPC_FP_QUAL_HDR_USER_DEF_0;
            sand_res = soc_ppd_fp_control_get(unit, SOC_CORE_INVALID, &index, &info);
            SOCDNX_SAND_IF_ERR_EXIT(sand_res);

            
            if ((info.val[0] == SOC_TMC_NOF_PMF_CE_SUB_HEADERS) && (info.val[3 ] == search_qual)) {
                *found = TRUE;
                SOC_EXIT;
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_pp_fp_is_qual_identical(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE  qual_type,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE  search_qual,
      SOC_SAND_OUT uint8                 *found
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    *found = FALSE;

    if (qual_type == search_qual) {
        *found = TRUE;
        SOC_EXIT;
    }

    
    if (SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(qual_type)) {
        SOC_PPC_FP_CONTROL_INDEX index;
        SOC_PPC_FP_CONTROL_INFO info;
        uint32 sand_res;

        SOC_PPC_FP_CONTROL_INDEX_clear(&index);
        SOC_PPC_FP_CONTROL_INFO_clear(&info);

        index.type = SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF;
        index.val_ndx = qual_type - SOC_PPC_FP_QUAL_HDR_USER_DEF_0;
        sand_res = soc_ppd_fp_control_get(unit, SOC_CORE_INVALID, &index, &info);
        SOCDNX_SAND_IF_ERR_EXIT(sand_res);

        
        if ((info.val[0] == SOC_TMC_NOF_PMF_CE_SUB_HEADERS) && (info.val[3 ] == search_qual)) {
            *found = TRUE;
            SOC_EXIT;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

void
    arad_pp_fp_key_qual_to_ce_const(
        SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
        SOC_SAND_IN ARAD_PP_FP_QUAL_CE_INFO   *ce_info,
        SOC_SAND_IN  uint32                    ce_indx,
        SOC_SAND_OUT ARAD_PP_FP_CE_CONSTRAINT *ce_const
    )
{
    ARAD_PP_FP_CE_CONSTRAINT_clear(ce_const);
    ce_const->place_cons = ce_info->place_cons[ce_indx];
    ce_const->cycle_cons = ARAD_PP_FP_KEY_CYCLE_ANY;
    ce_const->group_cons = ce_info->group_cons[ce_indx];
    ce_const->qual_lsb = ce_info->ce_qual_lsb[ce_indx];
    ce_const->lost_bits = ce_info->ce_lost_bits[ce_indx];
    ce_const->nof_bits = ce_info->ce_nof_bits[ce_indx];
    ce_const->size_cons = ce_info->size_cons[ce_indx];
}


void
    arad_pp_fp_key_ce_to_ce_const(
       SOC_SAND_IN  int                            unit,
        SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
        SOC_SAND_IN ARAD_PP_KEY_CE_ID         ce_id,
        SOC_SAND_OUT ARAD_PP_FP_CE_CONSTRAINT *ce_const
    )
{
    ARAD_PP_FP_CE_CONSTRAINT_clear(ce_const);
    ce_const->place_cons = (ce_id >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB)?ARAD_PP_FP_KEY_CE_HIGH:ARAD_PP_FP_KEY_CE_LOW;
    ce_const->size_cons =  arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_id)?ARAD_PP_FP_KEY_CE_SIZE_32:ARAD_PP_FP_KEY_CE_SIZE_16;
    ce_const->cycle_cons = ARAD_PP_FP_KEY_CYCLE_ANY;
    ce_const->group_cons = (ce_id % ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP)?ARAD_PP_FP_KEY_CE_HIGH:ARAD_PP_FP_KEY_CE_LOW;
}


 
STATIC
uint32
  arad_pp_fp_qual_ce_min_max_lost_bits_get(
      SOC_SAND_IN uint32 nof_buffs,
      SOC_SAND_IN uint8 is_msb, 
      SOC_SAND_IN uint8 want_max,
      SOC_SAND_INOUT uint32 ce_lost_bits_options[ARAD_PP_FP_KEY_NOF_BUFFERS][ARAD_PP_FP_QUAL_NOF_CE_SIZES]
  )
{
    uint32 
        min_max_lost_bits,
        buff_idx, 
        ce_size_idx;

    buff_idx = ((nof_buffs < 2) && is_msb) ? 1 : 0;
    min_max_lost_bits = ce_lost_bits_options[buff_idx][0];

    for(; buff_idx < 2; buff_idx++)
    {
        for(ce_size_idx = 0; ce_size_idx < ARAD_PP_FP_QUAL_NOF_CE_SIZES; ce_size_idx++)
        {
            min_max_lost_bits = want_max ? SOC_SAND_MAX(min_max_lost_bits, ce_lost_bits_options[buff_idx][ce_size_idx]) :
                SOC_SAND_MIN(min_max_lost_bits, ce_lost_bits_options[buff_idx][ce_size_idx]);
        }
    }
    return min_max_lost_bits;
}

STATIC
  uint32
  arad_pp_fp_key_next_zone_get(
    SOC_SAND_IN uint8  zone_overloaded[ARAD_PP_FP_KEY_NOF_ZONES],
    SOC_SAND_IN uint8  ce_is_msb
    )
{
    uint32 zone_id;

    
    if(!ce_is_msb)
    {
        
        zone_id = !zone_overloaded[ARAD_PP_FP_KEY_ZONE_LSB_0] ? ARAD_PP_FP_KEY_ZONE_LSB_0 : ARAD_PP_FP_KEY_ZONE_LSB_1;
    }
    else
    {
        
        zone_id = !zone_overloaded[ARAD_PP_FP_KEY_ZONE_MSB_0] ? ARAD_PP_FP_KEY_ZONE_MSB_0 : ARAD_PP_FP_KEY_ZONE_MSB_1;
    }

    return zone_id;
}

STATIC
    uint8
    arad_pp_fp_replace_options_get(
        SOC_SAND_IN     int     unit,
        SOC_SAND_IN     uint8   balance_enabled,
        SOC_SAND_IN     uint8   balance_msb,
        SOC_SAND_IN     uint32  ce_idx,
        SOC_SAND_IN     uint8   ce_is_32[ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS],
        SOC_SAND_IN     uint32  qual_nof_bits[ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS],
        SOC_SAND_IN     uint32  remaining_bits_in_zone[ARAD_PP_FP_KEY_NOF_ZONES],
        SOC_SAND_INOUT  uint32  qual_idx_sorted[ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS]
    )
{
    uint8 replace_val = FALSE;
    uint32 
        zone_idx,
        zone_id,
        next_available_zone[2] = {ARAD_PP_FP_KEY_NOF_ZONES,ARAD_PP_FP_KEY_NOF_ZONES},
        idx = qual_idx_sorted[ce_idx], 
        idx_1 = qual_idx_sorted[ce_idx + 1];

    
    for(zone_idx = 0; zone_idx < ARAD_PP_FP_KEY_NOF_ZONES; zone_idx++)
    {
        if (SOC_IS_JERICHO(unit) && 
            balance_enabled && 
            soc_property_get(unit, spn_FIELD_KEY_ALLOCATION_MSB_BALANCE_ENABLE, FALSE)) 
        {
            
            zone_id = (zone_idx % 2) ? (zone_idx - balance_msb) : (zone_idx + balance_msb);
        }
        else
        {
            zone_id = zone_idx;
        }

        if(next_available_zone[0] == ARAD_PP_FP_KEY_NOF_ZONES && 
           remaining_bits_in_zone[zone_id] >= qual_nof_bits[idx])
        {
            next_available_zone[0] = zone_id;
        }
        if(next_available_zone[1] == ARAD_PP_FP_KEY_NOF_ZONES && 
           remaining_bits_in_zone[zone_id] >= qual_nof_bits[idx_1])
        {
            next_available_zone[1] = zone_id;
        }
    }
    
    
    if(next_available_zone[1] == next_available_zone[0] &&
       (idx_1 / 2) == (next_available_zone[1] % 2) &&
       (idx / 2) != (next_available_zone[0] % 2))
    {
        replace_val = TRUE;
    }
    
    else if(!(next_available_zone[1] == next_available_zone[0] &&
            (idx / 2) == (next_available_zone[0] % 2) &&
            (idx_1 / 2) != (next_available_zone[1] % 2)))
    {
        
        if(qual_nof_bits[idx] > qual_nof_bits[idx_1] && 
           !(ce_is_32[idx] && ce_is_32[idx_1] && 
           (idx % 2) && !(idx_1 % 2)))
        {
            replace_val = TRUE;
        }
        
        else if(qual_nof_bits[idx] == qual_nof_bits[idx_1] && 
                !ce_is_32[idx] && !ce_is_32[idx_1] && 
                (idx % 2) && !(idx_1 % 2))
        {
            replace_val = TRUE;
        }
        
        else if(qual_nof_bits[idx] == qual_nof_bits[idx_1] && 
                ce_is_32[idx] && ce_is_32[idx_1] && 
                !(idx % 2) && (idx_1 % 2))
        {
            replace_val = TRUE;
        }
    }

    return replace_val;
}

STATIC
    void 
    arad_pp_fp_qual_size_sort_idx(
       SOC_SAND_IN  int                            unit,
        SOC_SAND_IN     SOC_PPC_FP_DATABASE_STAGE  stage,
        SOC_SAND_IN     uint32                  nof_options,
        SOC_SAND_IN     uint8                   use_kaps_split,
        SOC_SAND_IN     uint8                   ce_is_msb,
        SOC_SAND_IN     ARAD_PP_FP_ALLOC_ALG_ARGS  algorithm_args,
        SOC_SAND_IN     uint32                  qual_nof_bits[ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS],
        SOC_SAND_IN     uint32                  total_bits_in_zone[ARAD_PP_FP_KEY_NOF_ZONES],
        SOC_SAND_IN     uint32                  nof_free_ces_in_zone[ARAD_PP_FP_KEY_NOF_CATEGORIES],
        SOC_SAND_OUT    uint32                  qual_idx_sorted[ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS]
    )
{
    uint8 
        swaped, 
        ce_is_32[ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS];
    uint32 
        ce_idx, 
        ce_idx_tmp,
        zone_idx,
        remaining_bits_in_zone[ARAD_PP_FP_KEY_NOF_ZONES],
        nof_free_ces_bits; 

    sal_memset(ce_is_32, 0x0, sizeof(uint8) * ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS);

    
    for(zone_idx = 0; zone_idx < ARAD_PP_FP_KEY_NOF_ZONES; zone_idx++)
    {
        remaining_bits_in_zone[zone_idx] = ARAD_PMF_LOW_LEVEL_ZONE_SIZE_PER_STAGE - total_bits_in_zone[zone_idx];

        
        nof_free_ces_bits = ((nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(0, (zone_idx % 2), 0)] + 
                              nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(0, (zone_idx % 2), 1)]) * 16) 
                            + ((nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(1, (zone_idx % 2), 0)] + 
                               nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(1, (zone_idx % 2), 1)]) * 32)
                            - ((zone_idx >= 2)? remaining_bits_in_zone[zone_idx - 2] : 0);
        remaining_bits_in_zone[zone_idx] = SOC_SAND_MIN(nof_free_ces_bits, remaining_bits_in_zone[zone_idx]);  
    }

    
    for(ce_idx = 0; ce_idx < nof_options; ce_idx++)
    {
        
        ce_idx_tmp = (ce_is_msb && nof_options <=2) ? ce_idx + 2 : ce_idx;
        qual_idx_sorted[ce_idx] = ce_idx_tmp;

        
        if (use_kaps_split == TRUE) {
            ce_is_32[ce_idx_tmp] = FALSE;
        } else if(qual_nof_bits[ce_idx_tmp] > 16) {
            ce_is_32[ce_idx_tmp] = TRUE;
        }
    }

    do {
        swaped = FALSE;
        for(ce_idx = 0; ce_idx < (nof_options - 1); ce_idx++)
        {
            
            if(arad_pp_fp_replace_options_get(
                unit,
                algorithm_args.balance_enabled, 
                algorithm_args.balance_lsb_msb, 
                ce_idx,
                ce_is_32,
                qual_nof_bits,
                remaining_bits_in_zone,
                qual_idx_sorted))
            {
                ce_idx_tmp = qual_idx_sorted[ce_idx];
                qual_idx_sorted[ce_idx] = qual_idx_sorted[ce_idx+1];
                qual_idx_sorted[ce_idx+1] = ce_idx_tmp;
                swaped = TRUE;
            }
        }
    } while(swaped); 
}

STATIC
    uint8 
    arad_pp_fp_free_ces_in_zone_get(
        SOC_SAND_IN     uint32  flags,
        SOC_SAND_IN     uint8   modify,
        SOC_SAND_IN     uint8   use_kaps_split,
        SOC_SAND_IN     uint8   ce_is_msb,
        SOC_SAND_INOUT  uint8   *ce_is_32,
        SOC_SAND_INOUT  uint8   *ce_is_high_group,
        SOC_SAND_INOUT  uint32  nof_free_ces_in_zone[ARAD_PP_FP_KEY_NOF_CATEGORIES]
    )
{
    uint8 success = TRUE;

    if (*ce_is_32) {
        
        if (nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(1, ce_is_msb, 0)] + nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(1, ce_is_msb, 1)]) { 
            *ce_is_32 = 1;
            
            if (flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER) {
                
                *ce_is_high_group = (nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, 1)])? 1: 0;
            }
            else if (flags & ARAD_PP_FP_KEY_ALLOC_CE_FLP_NO_SPLIT) {
                
                *ce_is_high_group = (nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, 0)])? 0: 1;
            }
            else {
                *ce_is_high_group = (nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, 0)] < nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, 1)])? 1: 0;
            }
            if(modify){
                nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, *ce_is_high_group)] --;
            }
        }
        
        else 
        {
          
          success = FALSE;
        }
    }
    else {
        
        if (nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(0, ce_is_msb, 0)] + nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(0, ce_is_msb, 1)]) { 
            *ce_is_32 = 0;
        }
        
        else if (nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(1, ce_is_msb, 0)] + nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(1, ce_is_msb, 1)]) { 
            *ce_is_32 = 1;
        }
        else 
        {
          
          success = FALSE;
        }
        
        if ((flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER) || (use_kaps_split == TRUE)) {
            
            *ce_is_high_group = (nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, 1)])? 1: 0;
        }
        else if (flags & ARAD_PP_FP_KEY_ALLOC_CE_FLP_NO_SPLIT) {
            
            *ce_is_high_group = (nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, 0)])? 0: 1;
        }
        else {
            *ce_is_high_group = (nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, 0)] < nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, 1)])? 1: 0;
        }
        if(modify){
            nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(*ce_is_32, ce_is_msb, *ce_is_high_group)] --;
        }
    }
    return success;
}

STATIC
  uint8
  arad_pp_fp_key_not_enough_free_instr_get(
      SOC_SAND_IN  uint32   nof_free_ces_in_zone[ARAD_PP_FP_KEY_NOF_CATEGORIES],
      SOC_SAND_IN  uint32   qual_nof_bits, 
      SOC_SAND_IN  uint8    is_msb 
    )
{
  uint32 
      tmp_size,
      nof_ce_32b_free[2] = {0, 0}, 
      nof_ce_32b_needed[2] = {0, 0}; 
  uint8
      is_overloaded;
  int8
      is_high_group,
      is_32b;

  
  tmp_size = qual_nof_bits;
  while(tmp_size > 0) 
  {
      if (tmp_size > 32) {
          nof_ce_32b_needed[1] ++;
          tmp_size -= 32;
      }
      else {
          nof_ce_32b_needed[tmp_size > 16] ++;
          break;
      }
  }

  
  for(is_32b = 0; is_32b <= 1; ++is_32b) {
      for(is_high_group = 0; is_high_group <= 1; ++is_high_group) {
          nof_ce_32b_free[is_32b] += nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(is_32b, is_msb, is_high_group)]; 
      }
  }

     
  is_overloaded = FALSE;                                                                                                  
  for(is_32b = 1; (is_32b >= 0) && (!is_overloaded); is_32b--) {
      if (is_32b) {
          if (nof_ce_32b_free[is_32b] >= nof_ce_32b_needed[is_32b]) {
              nof_ce_32b_free[is_32b] -= nof_ce_32b_needed[is_32b];
          }
          else {
              nof_ce_32b_needed[is_32b] -= nof_ce_32b_free[is_32b];
              
              if (nof_ce_32b_needed[is_32b] <= (nof_ce_32b_free[0] / 2)) {
                  nof_ce_32b_free[0] -= (nof_ce_32b_needed[is_32b] * 2);
              }
              else {
                  is_overloaded = TRUE;
              }
          }
      }
      else {
          
          if (nof_ce_32b_needed[is_32b] > (nof_ce_32b_free[0] + nof_ce_32b_free[1])) {
              is_overloaded = TRUE;
          }
      }
  }

  return is_overloaded;
}


STATIC
  uint32
  arad_pp_fp_key_next_zone_probe(
     SOC_SAND_IN  int                            unit,
    SOC_SAND_IN     SOC_PPC_FP_DATABASE_STAGE  stage,
    SOC_SAND_IN     uint32  flags,
    SOC_SAND_IN     ARAD_PP_FP_ALLOC_ALG_ARGS  algorithm_args,
    SOC_SAND_IN     uint32  total_bits_in_zone[ARAD_PP_FP_KEY_NOF_ZONES],
    SOC_SAND_IN     uint8   use_kaps_split,
    SOC_SAND_IN     uint8   is_lsb,
    SOC_SAND_IN     uint8   is_msb,
    SOC_SAND_IN     uint32  nof_options,
    SOC_SAND_IN     uint32  total_qual_size,
    SOC_SAND_IN     uint32  qual_nof_bits[ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS],
    SOC_SAND_INOUT  uint32  nof_free_ces_in_zone[ARAD_PP_FP_KEY_NOF_CATEGORIES],
    SOC_SAND_OUT    uint8   *ce_is_msb,
    SOC_SAND_OUT    uint8   *ce_is_32,
    SOC_SAND_OUT    uint8   *ce_is_high_group,
    SOC_SAND_OUT    uint32  *curr_size
    )
{
    uint32 
        zone_id = 0,
        zone_idx,
        max_remaining_size,
        qual_id,
        qual_idx,
        qual_idx_sorted[ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS];
    uint8 
        success = FALSE,
        zone_overloaded[ARAD_PP_FP_KEY_NOF_ZONES];

    
    arad_pp_fp_qual_size_sort_idx(
        unit,
        stage,
        nof_options, 
        use_kaps_split,
        (!is_lsb && is_msb), 
        algorithm_args,
        qual_nof_bits, 
        total_bits_in_zone,
        nof_free_ces_in_zone,
        qual_idx_sorted);

    max_remaining_size = ARAD_PMF_LOW_LEVEL_ZONE_SIZE_PER_STAGE - total_bits_in_zone[0];

    for(qual_idx = 0; qual_idx < nof_options; qual_idx++)
    {
        qual_id = qual_idx_sorted[qual_idx];
        
        
        *ce_is_msb = (qual_id < 2) ? FALSE : TRUE;
        *ce_is_32 = (qual_id % 2) ? TRUE : FALSE;
        
        for(zone_idx = 0; zone_idx < ARAD_PP_FP_KEY_NOF_ZONES; zone_idx++)
        {
            if((ARAD_PMF_LOW_LEVEL_ZONE_SIZE_PER_STAGE - total_bits_in_zone[zone_idx]) > max_remaining_size) {
                max_remaining_size = ARAD_PMF_LOW_LEVEL_ZONE_SIZE_PER_STAGE - total_bits_in_zone[zone_idx];
            }

            
            zone_overloaded[zone_idx] = 
                ((total_bits_in_zone[zone_idx] + qual_nof_bits[qual_id]) > ARAD_PMF_LOW_LEVEL_ZONE_SIZE_PER_STAGE)? 
                TRUE: FALSE;

            
            zone_overloaded[zone_idx] |= 
                arad_pp_fp_key_not_enough_free_instr_get(
                    nof_free_ces_in_zone,
                    qual_nof_bits[qual_id], 
                    *ce_is_msb
                );

            
            if(!zone_overloaded[zone_idx] && (*ce_is_msb == (zone_idx % 2))){
                success = TRUE;
            }
        }

        
        if(!success) {
            continue;
        }

        
        success = arad_pp_fp_free_ces_in_zone_get(
                    flags,
                    FALSE, 
                    use_kaps_split,
                    *ce_is_msb,
                    ce_is_32, 
                    ce_is_high_group,
                    nof_free_ces_in_zone);
        
        if(success){

            
            if(((qual_id % 2) != *ce_is_32) ||
               ((!use_kaps_split && (qual_nof_bits[qual_id] > 16 && !(*ce_is_32))) ||
               (use_kaps_split && (qual_nof_bits[qual_id] > 16 && (*ce_is_32)))))
            {
                success = FALSE;
                continue;
            }

            
            zone_id = arad_pp_fp_key_next_zone_get(
                        zone_overloaded, 
                        *ce_is_msb
                      );

            
            success = arad_pp_fp_free_ces_in_zone_get(
                        flags,
                        TRUE, 
                        use_kaps_split,
                        *ce_is_msb,
                        ce_is_32, 
                        ce_is_high_group,
                        nof_free_ces_in_zone);

            
            if (SOC_IS_QUX(unit) &&
                (use_kaps_split == TRUE) &&
                (total_qual_size < SOC_DPP_DEFS_GET(unit, field_large_direct_lu_key_min_length)) &&
                ((qual_nof_bits[qual_id] + total_qual_size) > SOC_DPP_DEFS_GET(unit, field_large_direct_lu_key_min_length))) {
                *curr_size = SOC_DPP_DEFS_GET(unit, field_large_direct_lu_key_min_length) - total_qual_size;
            } else if (use_kaps_split && (qual_nof_bits[qual_id] > 16)) {
                *curr_size = 16;
            } else {
                *curr_size = qual_nof_bits[qual_id];
            }

            break;
        }
    }

    
    if(!success) {
        zone_id = ARAD_PP_FP_KEY_NOF_ZONES;
        *curr_size = max_remaining_size;
    }
    return zone_id;
}



STATIC uint32
arad_pp_fp_qual_ce_info_uneven_specific_ce_list_get(
      SOC_SAND_IN       int                         unit,
      SOC_SAND_IN       SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN       uint32                      flags,
      SOC_SAND_IN       SOC_PPC_FP_QUAL_TYPE        qual_type,
      SOC_SAND_IN       int                         ce_id[BCM_FIELD_NOF_CE_ID_PER_QUAL],
      SOC_SAND_OUT      ARAD_PP_FP_QUAL_CE_INFO     *qual_ce_info
      )
{
    uint8
        found,
        found_internal,
        is_header,
        ce_is_32 = 0,
        ce_max_size,
        is_msb;
    uint32
        res = SOC_SAND_OK,
        bits_for_ce,
        hw_buffer_jericho,
        qual_lsb,
        ce_offset,
        tmp_size = 0,
        ce_cur_size = 32,
        qual_cur_size = 0,
        ce_lost_bits;
    ARAD_PMF_CE_IRPP_QUALIFIER_INFO
        int_qual_info;
    ARAD_PMF_CE_HEADER_QUAL_INFO
        hdr_qual_info;
    ARAD_PMF_CE_PACKET_HEADER_INFO
        hdr_offset;
    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(qual_ce_info);

    qual_ce_info->is_header = 0;
    qual_ce_info->place_cons[0] = 0;
    qual_ce_info->nof_ce = 0;
    qual_ce_info->qual_size = 0;
    qual_ce_info->lost_bits = 0;
    qual_ce_info->lost_bits_worst = 0;
    is_msb = 1;

    if (SOC_IS_ARADPLUS(unit))
    {
        
        if(qual_type == SOC_PPC_FP_QUAL_IS_EQUAL)
        {
            goto exit;
        }
    }

    
    res = arad_pmf_ce_header_info_find(
             unit,
             qual_type,
             stage,
             &is_header,
             &hdr_qual_info
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    qual_ce_info->is_lsb = 1;
    qual_ce_info->is_msb = 1;
    qual_ce_info->place_cons[0] = ARAD_PP_FP_KEY_CE_PLACE_ANY;

    if(is_header)
    {
        qual_ce_info->is_header = 1;

        qual_ce_info->qual_size = hdr_qual_info.lsb - hdr_qual_info.msb + 1;
        hdr_offset.nof_bits = qual_ce_info->qual_size;
        hdr_offset.offset = hdr_qual_info.msb;
    }
    else
    {
        
        res = arad_pmf_ce_internal_field_info_find(
                unit,
                qual_type,
                stage,
                is_msb,
                &found_internal,
                &int_qual_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        qual_ce_info->qual_size = int_qual_info.info.qual_nof_bits;

    }
    tmp_size = qual_ce_info->qual_size;

    qual_lsb = 0;

    

    while(tmp_size > 0)
    {
        if ((flags & ARAD_PP_FP_KEY_ALLOC_CE_SINGLE_MAPPING) && (qual_ce_info->nof_ce != 0)) {
            
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  SOC_DPP_MSG("    "
                                  "Key: fail to split qualifier because the key zones are too small \n\r"))));
            goto exit; 
        }

        ce_is_32 = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_id[qual_ce_info->nof_ce]);
        ce_max_size = ce_is_32 ? 32 : 16;

        
        if(is_header)
        {
            
                
            res = arad_pmf_ce_nof_real_bits_compute_header(
                    unit,
                    &hdr_offset,
                    0,     
                    ce_is_32,    
                    &bits_for_ce
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

            
            ce_lost_bits = bits_for_ce - hdr_offset.nof_bits;
        }
        else
        {
            
            res = arad_pmf_ce_internal_field_offset_compute(
                    unit,
                    qual_type,
                    stage,
                    is_msb,                                   
                    qual_ce_info->qual_size - qual_lsb,       
                    qual_lsb,
                    0,              
                    ce_is_32,       
                    &found,
                    &ce_offset,
                    &bits_for_ce,
                    &hw_buffer_jericho
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

            if ((flags & ARAD_PP_FP_KEY_ALLOC_CE_FLP_NO_SPLIT) && (qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES)) {
                ce_lost_bits = 0; 
            }
            else {
                ce_lost_bits = bits_for_ce - (qual_ce_info->qual_size - qual_lsb);
            }
        }

        
        qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce] =  ce_lost_bits;
        qual_ce_info->lost_bits_worst = ce_lost_bits;

        qual_ce_info->place_cons[qual_ce_info->nof_ce] = qual_ce_info->place_cons[0];
        qual_ce_info->group_cons[qual_ce_info->nof_ce] = ARAD_PP_FP_KEY_CE_PLACE_ANY;
        qual_ce_info->size_cons[qual_ce_info->nof_ce] = ARAD_PP_FP_KEY_CE_SIZE_ANY;
        ce_cur_size = ((tmp_size + qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce]) < ce_max_size) ?
            (tmp_size + qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce]) : ce_max_size;


        qual_ce_info->ce_nof_bits[qual_ce_info->nof_ce] = ce_cur_size;
        qual_ce_info->lost_bits += qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce];

        
        qual_cur_size = ce_cur_size - qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce];
        qual_ce_info->ce_qual_lsb[qual_ce_info->nof_ce] = qual_lsb;
        qual_lsb += qual_cur_size;

        tmp_size -= qual_cur_size;
        if(is_header)
        {
            hdr_offset.nof_bits -= qual_cur_size;
        }
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "end of handling qualifier %s  ce_id %d ce_lost_bits %d qual_lsb %d. tmp_size %d\n\r"),
                          SOC_PPC_FP_QUAL_TYPE_to_string(qual_type), ce_id[qual_ce_info->nof_ce], ce_lost_bits, qual_lsb, tmp_size));

        qual_ce_info->nof_ce++;
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_ce_info_uneven_get()", qual_type, 0);
}

uint32
arad_pp_fp_key_alloc_specific_ce_constrain_calc(
        SOC_SAND_IN       int                         unit,
        SOC_SAND_IN       SOC_PPC_FP_DATABASE_STAGE   stage,
        SOC_SAND_IN       uint32                      flags,
        SOC_SAND_IN       SOC_PPC_FP_QUAL_TYPE        qual_types[ARAD_PP_QUAL_CHAIN_MAX], 
        SOC_SAND_IN       uint32                      max_nof_ppc_qual,
        SOC_SAND_IN       int                         ce_id[BCM_FIELD_NOF_CE_ID_PER_QUAL],
        SOC_SAND_OUT      ARAD_PP_FP_CE_CONSTRAINT    ce_const[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS], 
        SOC_SAND_OUT      uint32                      *nof_consts
        )
{
    uint32  ce_indx,
            total_ce_indx,
            qual_indx;
    uint32
        res = SOC_SAND_OK;
    ARAD_PP_FP_QUAL_CE_INFO             qual_ce_info;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    qual_ce_info.nof_ce = 0;

    for(qual_indx = 0, total_ce_indx = 0; (qual_indx < max_nof_ppc_qual ) &&
                                          (qual_types[qual_indx] != SOC_PPC_NOF_FP_QUAL_TYPES) &&
                                          (qual_types[qual_indx] != BCM_FIELD_ENTRY_INVALID)
                                          ; qual_indx++)
    {
        res = arad_pp_fp_qual_ce_info_uneven_specific_ce_list_get(unit, stage, flags, qual_types[qual_indx], &ce_id[qual_ce_info.nof_ce], &qual_ce_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "qualifiers %s nof ce %d.\n\r"),
                          SOC_PPC_FP_QUAL_TYPE_to_string(qual_types[qual_indx]), qual_ce_info.nof_ce));
        
        for(ce_indx = 0; (ce_indx < qual_ce_info.nof_ce) && (ce_indx < BCM_FIELD_NOF_CE_ID_PER_QUAL); ce_indx++)
        {
            if(total_ce_indx >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG)
            {
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit: %d too much qualifiers\n\r"
                                      "Total qualifiers %d, Maximum qualifiers allowed %d.\n\r"),
                           unit, total_ce_indx, ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG));
                 SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 60, exit);
            }
            arad_pp_fp_key_qual_to_ce_const(stage, &qual_ce_info, ce_indx, &(ce_const[total_ce_indx]));
            ce_const[total_ce_indx].qual_type = qual_types[qual_indx];
            ce_const[total_ce_indx].place_cons = ARAD_PP_FP_KEY_CE_PLACE_ANY;

            total_ce_indx++;
        }
    }
    *nof_consts = total_ce_indx;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_alloc_specific_ce_constrain_calc()", qual_types[qual_indx], 0);
}

STATIC
uint32
arad_pp_fp_qual_ce_info_uneven_get(
      SOC_SAND_IN       int                         unit,
      SOC_SAND_IN       SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN       uint8                       break_uneven,
      SOC_SAND_IN       uint8                       use_kaps,
      SOC_SAND_IN       uint32                      flags,
      SOC_SAND_IN       SOC_PPC_FP_QUAL_TYPE        qual_type,
      SOC_SAND_INOUT    uint32                      *total_qual_size,
      SOC_SAND_INOUT    ARAD_PP_FP_ALLOC_ALG_ARGS   *algorithm_args,
      SOC_SAND_INOUT    uint32                      total_bits_in_zone[ARAD_PP_FP_KEY_NOF_ZONES],
      SOC_SAND_INOUT    uint32                      nof_free_ces_in_zone[ARAD_PP_FP_KEY_NOF_CATEGORIES],
      SOC_SAND_OUT      uint8                       zone_used[ARAD_PP_FP_KEY_NOF_ZONES],
      SOC_SAND_OUT      ARAD_PP_FP_QUAL_CE_INFO     *qual_ce_info, 
      SOC_SAND_OUT      uint8                       *success 
      )
{
    uint8
        found = 0xff, 
        found_internal[2],
        is_header,
        ce_is_high_group = 0,
        ce_is_msb = 0,
        ce_is_32 = 0,
        use_kaps_split = FALSE,
        is_msb;
    uint32
        res = SOC_SAND_OK,
        bits_for_ce,
        hw_buffer_jericho,
        qual_lsb,
        ce_offset,
        tmp_size = 0,
        break_idx,
        max_break_idx,
        found_indx,
        ce_size_idx,
        nof_found = 0,
        ce_cur_size = 32,
        alloc_size = 0,
        qual_cur_size = 0,
        zone_id = 0,
        lost_bits,
        qual_nof_bits[ARAD_PP_FP_KEY_NOF_BUFFERS * ARAD_PP_FP_QUAL_NOF_CE_SIZES],
        ce_lost_bits_options[ARAD_PP_FP_KEY_NOF_BUFFERS][ARAD_PP_FP_QUAL_NOF_CE_SIZES],
        found_place[2] = {0,0};
    ARAD_PMF_CE_IRPP_QUALIFIER_INFO
        int_qual_info[2];
    ARAD_PMF_CE_HEADER_QUAL_INFO
        hdr_qual_info;
    ARAD_PMF_CE_PACKET_HEADER_INFO
        hdr_offset;

    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(qual_ce_info);
    SOC_SAND_CHECK_NULL_INPUT(success);

    
    if(break_uneven)
    {
        SOC_SAND_CHECK_NULL_INPUT(total_bits_in_zone);
        SOC_SAND_CHECK_NULL_INPUT(nof_free_ces_in_zone);
        SOC_SAND_CHECK_NULL_INPUT(zone_used);
    }
    
    qual_ce_info->is_header = 0;
    qual_ce_info->place_cons[0] = 0;
    qual_ce_info->nof_ce = 0;
    qual_ce_info->qual_size = 0;
    qual_ce_info->lost_bits = 0;
    qual_ce_info->lost_bits_worst = 0;
    
    *success = TRUE;

    if (SOC_IS_ARADPLUS(unit))
    {  
        
        if(qual_type == SOC_PPC_FP_QUAL_IS_EQUAL)
        {
            if (break_uneven) {
                zone_used[ARAD_PP_FP_KEY_ZONE_MSB_0] = TRUE;
            }
            goto exit;
        }
    }

    
    res = arad_pmf_ce_header_info_find(
             unit,
             qual_type,
             stage,
             &is_header,
             &hdr_qual_info
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(is_header) 
    {
        qual_ce_info->is_header = 1;

        
        qual_ce_info->is_lsb = 1;
        qual_ce_info->is_msb = 1;
        qual_ce_info->place_cons[0] = ARAD_PP_FP_KEY_CE_PLACE_ANY;

        qual_ce_info->qual_size = hdr_qual_info.lsb - hdr_qual_info.msb + 1;
        hdr_offset.nof_bits = qual_ce_info->qual_size;
        hdr_offset.offset = hdr_qual_info.msb;

        
        tmp_size = qual_ce_info->qual_size;
    }
    else
    {
        
        for(is_msb = 0; is_msb <=1; is_msb++)
        {
            
            res = arad_pmf_ce_internal_field_info_find(
                    unit,
                    qual_type,
                    stage,
                    is_msb,
                    &(found_internal[is_msb]),
                    &(int_qual_info[is_msb])
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            if(is_msb)
                qual_ce_info->is_msb = found_internal[is_msb];
            else
                qual_ce_info->is_lsb = found_internal[is_msb];

            
            if(found_internal[is_msb]) {
                found_place[nof_found++] = is_msb;
                qual_ce_info->place_cons[0] |= is_msb ? ARAD_PP_FP_KEY_CE_HIGH : ARAD_PP_FP_KEY_CE_LOW;
            }
        }

        
        if(!qual_ce_info->is_msb && !qual_ce_info->is_lsb) {
           LOG_ERROR(BSL_LS_SOC_FP,
                     (BSL_META_U(unit,
                                 "Unit %d Stage %s Qualifier %s : The qualifier does not exist in no buffer.\n\r"),
                      unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), SOC_PPC_FP_QUAL_TYPE_to_string(qual_type)));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 50, exit);
        }
        if((nof_found == 2) && (int_qual_info[0].info.qual_nof_bits != int_qual_info[1].info.qual_nof_bits)) {
           LOG_ERROR(BSL_LS_SOC_FP,
                     (BSL_META_U(unit,
                                 "Unit %d Stage %s Qualifier %s : Number of bits differs between lsb to msb.\n\r"
                                 "Number of bits in lsb %d, Munber of bits in msb %d.\n\r"),
                      unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), SOC_PPC_FP_QUAL_TYPE_to_string(qual_type), int_qual_info[0].info.qual_nof_bits, int_qual_info[1].info.qual_nof_bits));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 51, exit); 
        }

        
        if (nof_found > 0) {
            qual_ce_info->qual_size = (found_internal[1])? int_qual_info[1].info.qual_nof_bits : int_qual_info[0].info.qual_nof_bits;
            tmp_size = qual_ce_info->qual_size;
        }
    }

    qual_lsb = 0;

    
    while(tmp_size > 0) 
    {
        if ((flags & ARAD_PP_FP_KEY_ALLOC_CE_SINGLE_MAPPING) && (qual_ce_info->nof_ce != 0)) {
                
                *success = FALSE;
                LOG_DEBUG(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      SOC_DPP_MSG("    "
                                      "Key: fail to split qualifier because the key zones are too small \n\r"))));
                goto exit; 
        }
        sal_memset(ce_lost_bits_options, 0x0, sizeof(uint32) * ARAD_PP_FP_QUAL_NOF_CE_SIZES * ARAD_PP_FP_KEY_NOF_BUFFERS);

        
        if(is_header) 
        {
            
            nof_found = ARAD_PP_FP_KEY_NOF_BUFFERS;
            found_place[0] = 0;
            found_place[1] = 1;

            for(ce_size_idx = 0; ce_size_idx < ARAD_PP_FP_QUAL_NOF_CE_SIZES; ce_size_idx++)
            {
                
                res = arad_pmf_ce_nof_real_bits_compute_header(
                        unit,
                        &hdr_offset,
                        0,     
                        ce_size_idx,    
                        &bits_for_ce 
                        );
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

                
                ce_lost_bits_options[ARAD_PP_FP_KEY_BUFF_LSB][ce_size_idx] = bits_for_ce - hdr_offset.nof_bits;
                ce_lost_bits_options[ARAD_PP_FP_KEY_BUFF_MSB][ce_size_idx] = bits_for_ce - hdr_offset.nof_bits;
            }
        }
        else
        {
             
            for(found_indx = 0; found_indx < nof_found; ++found_indx) 
            {
                for(ce_size_idx = 0; ce_size_idx < ARAD_PP_FP_QUAL_NOF_CE_SIZES; ce_size_idx++)
                {
                    res = arad_pmf_ce_internal_field_offset_compute(
                            unit,
                            qual_type,
                            stage,
                            found_place[found_indx],    
                            qual_ce_info->qual_size - qual_lsb,       
                            qual_lsb,
                            0,              
                            ce_size_idx,    
                            &found,
                            &ce_offset,
                            &bits_for_ce,
                            &hw_buffer_jericho
                            );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

                    ce_lost_bits_options[found_place[found_indx]][ce_size_idx] = bits_for_ce - (qual_ce_info->qual_size - qual_lsb);
                }
            }
        }
      
        
        if(break_uneven) 
        {
            
            max_break_idx =  (((flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER) ||
                               (flags & ARAD_PP_FP_KEY_ALLOC_CE_FLP_NO_SPLIT)) ? 
                              1: 2);

            for(break_idx = 0; break_idx < max_break_idx; break_idx++)
            {
                
                for(found_indx = 0; found_indx < nof_found; ++found_indx) 
                {
                    for(ce_size_idx = 0; ce_size_idx < ARAD_PP_FP_QUAL_NOF_CE_SIZES; ce_size_idx++)
                    {
                        
                        lost_bits = ce_lost_bits_options[found_place[found_indx]][ce_size_idx];

                        
                        if(break_idx > 0 && alloc_size > lost_bits && (tmp_size + lost_bits) >= alloc_size) {
                            ce_cur_size = (alloc_size < 32) ? alloc_size : 32;
                        }
                        else {
                            ce_cur_size = (tmp_size + lost_bits) < 32 ? (tmp_size + lost_bits) : 32;
                        }
                        qual_nof_bits[found_place[found_indx] * 2 + ce_size_idx] = ce_cur_size;
                    }
                }

                if ((use_kaps == TRUE) &&
                    (((ARAD_PP_FP_DIRECT_TABLE_KAPS_KEY_LENGTH > 16) && (ce_lost_bits_options[0][0] == 0) && (ce_lost_bits_options[0][1] == 0x4)) ||
                     (SOC_IS_QUX(unit) && (ARAD_PP_FP_DIRECT_TABLE_KAPS_KEY_LENGTH > SOC_DPP_DEFS_GET(unit, field_large_direct_lu_key_min_length)))))
                {
                    use_kaps_split = TRUE;
                }

                zone_id =  arad_pp_fp_key_next_zone_probe(
                                unit,
                                stage,
                                flags,
                                *algorithm_args,
                                total_bits_in_zone,
                                use_kaps_split,
                                qual_ce_info->is_lsb,
                                qual_ce_info->is_msb,
                                nof_found * 2, 
                                ((total_qual_size != NULL) ? *total_qual_size : 0),
                                qual_nof_bits,
                                nof_free_ces_in_zone,
                                &ce_is_msb,
                                &ce_is_32,
                                &ce_is_high_group,
                                &alloc_size
                                );
                                
                
                if (algorithm_args->balance_enabled) {
                    algorithm_args->balance_lsb_msb ^= 1;
                }

                
                if(zone_id < ARAD_PP_FP_KEY_NOF_ZONES || alloc_size <= 0)
                    break;
            }

            
            if(zone_id >= ARAD_PP_FP_KEY_NOF_ZONES)
            {
                
                *success = FALSE;
                LOG_DEBUG(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "    "
                                      "Key: fail to split qualifier because the key zones are too small \n\r")));
                goto exit; 
            }

            if (total_qual_size != NULL) {
                *total_qual_size += alloc_size;
            }

            zone_used[zone_id] = TRUE;

            
            ce_cur_size = alloc_size;
            qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce] = ce_lost_bits_options[ce_is_msb][ce_is_32];
            qual_ce_info->place_cons[qual_ce_info->nof_ce] = ce_is_msb ? ARAD_PP_FP_KEY_CE_HIGH : ARAD_PP_FP_KEY_CE_LOW;
            qual_ce_info->group_cons[qual_ce_info->nof_ce] = ce_is_high_group? ARAD_PP_FP_KEY_CE_HIGH : ARAD_PP_FP_KEY_CE_LOW;
            qual_ce_info->size_cons[qual_ce_info->nof_ce] = ce_is_32 ? ARAD_PP_FP_KEY_CE_SIZE_32 : ARAD_PP_FP_KEY_CE_SIZE_16;

             
            total_bits_in_zone[zone_id] += ce_cur_size;
        }
        else 
        {
            
            qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce] = arad_pp_fp_qual_ce_min_max_lost_bits_get(nof_found, found_internal[1], FALSE, ce_lost_bits_options);
            qual_ce_info->lost_bits_worst = arad_pp_fp_qual_ce_min_max_lost_bits_get(nof_found, found_internal[1], TRUE, ce_lost_bits_options);

            qual_ce_info->place_cons[qual_ce_info->nof_ce] = qual_ce_info->place_cons[0];
            qual_ce_info->group_cons[qual_ce_info->nof_ce] = ARAD_PP_FP_KEY_CE_PLACE_ANY;
            qual_ce_info->size_cons[qual_ce_info->nof_ce] = ARAD_PP_FP_KEY_CE_SIZE_ANY;
            ce_cur_size = ((tmp_size + qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce]) < 32) ? 
                (tmp_size + qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce]) : 32;

        }

        qual_ce_info->ce_nof_bits[qual_ce_info->nof_ce] = ce_cur_size;
        qual_ce_info->lost_bits += qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce];

        
        qual_cur_size = ce_cur_size - qual_ce_info->ce_lost_bits[qual_ce_info->nof_ce];
        qual_ce_info->ce_qual_lsb[qual_ce_info->nof_ce] = qual_lsb;
        qual_lsb += qual_cur_size;

        tmp_size -= qual_cur_size;
        hdr_offset.nof_bits -= qual_cur_size;

        if(break_uneven)
        {
            qual_ce_info->ce_zone_id[qual_ce_info->nof_ce] = zone_id;
            zone_used[zone_id] = TRUE;
        }

        qual_ce_info->nof_ce++;
    }

    
    if (SOC_IS_QUX(unit) && use_kaps_split && (qual_ce_info->nof_ce > 1)) {
        if (qual_ce_info->ce_lost_bits[1] > QUX_KAPS_DIRECT_LOOKUP_NOF_UNUSED_BITS) {
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  SOC_DPP_MSG("    "
                                  "Key: CE 1 has more than %d lost bits, not supported \n\r")), QUX_KAPS_DIRECT_LOOKUP_NOF_UNUSED_BITS));
            goto exit;
        }

        qual_ce_info->ce_nof_bits[0] += QUX_KAPS_DIRECT_LOOKUP_NOF_UNUSED_BITS;
        qual_ce_info->ce_nof_bits[0] -= qual_ce_info->ce_lost_bits[1];
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_ce_info_uneven_get()", qual_type, 0);
}

uint32
  arad_pp_fp_qual_ce_info_get(
	  SOC_SAND_IN  int                              unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
	  SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE                qual_type,
      SOC_SAND_OUT  ARAD_PP_FP_QUAL_CE_INFO          *qual_ce_info 
  )
{
    uint8 
        success,
        dummy_u8[ARAD_PP_FP_KEY_NOF_ZONES];
    uint32 
        res, 
        dummy_u32[ARAD_PP_FP_KEY_NOF_ZONES];
    ARAD_PP_FP_ALLOC_ALG_ARGS
        algorithm_args;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  algorithm_args.balance_enabled = 0;
  algorithm_args.balance_lsb_msb = 0;

  res = arad_pp_fp_qual_ce_info_uneven_get(
          unit,
          stage,
          FALSE,
          FALSE,
          0,
          qual_type,
          NULL,
          &algorithm_args,
          dummy_u32, 
          dummy_u32, 
          dummy_u8,   
          qual_ce_info,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_ce_info_get()", qual_type, 0);
}


uint32
  arad_pp_fp_key_get_available_ce_under_constraint(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 place_cons,
      SOC_SAND_IN  uint32                 size_cons,  
      SOC_SAND_IN  uint32                 group_cons  
  )
{
  uint32
    avail_bmp_ce = 0,
    avail_bmp_place = 0,
    avail_bmp_group = 0,
    avail_bmp_size = 0;

  if(place_cons == ARAD_PP_FP_KEY_CE_PLACE_ANY) {
      avail_bmp_place = 0xffffffff;
  }
  else if(place_cons == 0) {
      return 0;
  }
  
  else
  {
      if (SOC_IS_JERICHO_PLUS(unit) 
        && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)
        && (place_cons == ARAD_PP_FP_KEY_CE_PLACE_HASH_KEY)) {
          
          SHR_BITSET_RANGE(&avail_bmp_place, 12, 4);
      } else {
          SHR_BITSET_RANGE(&avail_bmp_place,0,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB);
          
          if(place_cons == ARAD_PP_FP_KEY_CE_HIGH) 
          {
              avail_bmp_place = ~avail_bmp_place;
          }
      }
  }

  if(size_cons == ARAD_PP_FP_KEY_CE_SIZE_ANY) {
      avail_bmp_size = 0xffffffff;
  }
  else if(size_cons == 0) {
      return 0;
  }
  else{
       
      if (ARAD_PMF_LOW_LEVEL_PROG_GROUP_IS_ALL_32) {
          avail_bmp_size = 0;
      }
      else {
          
          
          SHR_BITSET_RANGE(&avail_bmp_size,0,ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP);

          
          SHR_BITSET_RANGE(&avail_bmp_size,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP,ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP);
          
          
          SHR_BITSET_RANGE(&avail_bmp_size,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB,ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP);
          
          
          SHR_BITSET_RANGE(&avail_bmp_size,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB+ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP,ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP);
      }
      
      
      if((size_cons == ARAD_PP_FP_KEY_CE_SIZE_32)){
          avail_bmp_size = ~avail_bmp_size;
      }
  }


  if(group_cons == ARAD_PP_FP_KEY_CE_PLACE_ANY) {
      avail_bmp_group = 0xffffffff;
  }
  else if(group_cons == 0) {
      return 0;
  }
  else{
      
      
    SHR_BITSET_RANGE(&avail_bmp_group,0,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP);
      
      
	SHR_BITSET_RANGE(&avail_bmp_group,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP);
      
      
      if((group_cons == ARAD_PP_FP_KEY_CE_HIGH)){
          avail_bmp_group = ~avail_bmp_group;
      }
  }

  
  avail_bmp_ce = 0xffffffff;
  if (ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG != 32) {
      
      
      avail_bmp_ce = (1 << ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG) - 1;
  }
  

  
  return avail_bmp_place & avail_bmp_group & avail_bmp_size & avail_bmp_ce;
}




uint32
  arad_pp_fp_key_ce_is_in_use(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 prog_ndx,
      SOC_SAND_IN  uint32                 cycle_ndx, 
      SOC_SAND_IN  ARAD_PP_KEY_CE_ID      ce_ndx,
      SOC_SAND_OUT  uint8                 *in_use

  )
{
    ARAD_PMF_CE
      sw_db_ce;
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(
            unit,
            stage,
            prog_ndx,
            cycle_ndx,
            ce_ndx,
            &sw_db_ce
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    *in_use = (sw_db_ce.is_used == TRUE);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_ce_is_in_use()", 0, 0);
}




uint32
  arad_pp_fp_key_ce_set_use(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 prog_ndx,
      SOC_SAND_IN  uint32                 cycle_ndx, 
      SOC_SAND_IN  ARAD_PP_KEY_CE_ID      ce_ndx,
      SOC_SAND_IN  uint8                  in_use

  )
{
    ARAD_PMF_CE
      sw_db_ce;
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(
            unit,
            stage,
            prog_ndx,
            cycle_ndx,
            ce_ndx,
            &sw_db_ce
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    sw_db_ce.is_used = in_use;

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.set(
            unit,
            stage,
            prog_ndx,
            cycle_ndx,
            ce_ndx,
            &sw_db_ce
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_ce_set_use()", 0, 0);
}


uint32
    arad_pp_fp_key_alloc_key_cascaded_key_get(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  db_id_ndx,
      SOC_SAND_OUT uint8  *is_key_fixed,
      SOC_SAND_OUT uint8  *key_id, 
      SOC_SAND_OUT uint32  *key_bitmap_constraint 
    )
{
  uint32
      pmf_pgm_ndx_min,
      pmf_pgm_ndx_max,
      pmf_pgm_ndx,
      keys_bmp,
    res = SOC_SAND_OK;
  uint8
      is_cascaded,
      lookup_id;
  ARAD_PMF_DB_INFO
      db_info;
  SOC_PPC_FP_DATABASE_STAGE
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *is_key_fixed = FALSE;
  *key_id = 0;
  *key_bitmap_constraint = 0;

  
  
  res = arad_pp_fp_db_stage_get(
            unit,
            db_id_ndx,
            &stage
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  
  res = arad_pp_fp_db_cascaded_cycle_get(
          unit,
          db_id_ndx,
          &is_cascaded,
          &lookup_id 
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
  if (is_cascaded && (lookup_id == 1)) {
      
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(
              unit,
              stage,
              db_id_ndx,
              &db_info
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

       
      if (db_info.cascaded_key) {
          *is_key_fixed = TRUE;
          *key_id = ARAD_PP_FP_KEY_FROM_CASCADED_KEY_GET(db_info.cascaded_key);
      }
      else {
          *is_key_fixed = FALSE;
          
          res = arad_pmf_prog_select_pmf_pgm_borders_get(
                    unit,
                    stage,
                    0 , 
                    &pmf_pgm_ndx_min,
                    &pmf_pgm_ndx_max
                  );
          SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

          for (pmf_pgm_ndx = pmf_pgm_ndx_min; pmf_pgm_ndx < pmf_pgm_ndx_max; ++pmf_pgm_ndx)
          {
              if (SHR_BITGET(db_info.progs, pmf_pgm_ndx)) {
                  
                  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.get(unit, stage, pmf_pgm_ndx, lookup_id , &keys_bmp);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
                  *key_bitmap_constraint |= keys_bmp;
              }
          }
      }
  }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_alloc_key_cascaded_key_get()", 0, 0);
}


uint32
    arad_pp_fp_key_alloc_key_is_equal_get(
      SOC_SAND_IN int    unit,
      SOC_SAND_IN uint32    db_id_ndx,
      SOC_SAND_OUT uint8    *is_key_fixed,
      SOC_SAND_OUT uint8    *key_id,
      SOC_SAND_OUT uint32   *key_place,
      SOC_SAND_OUT uint32   *key_bitmap_constraint 
    )
{
    uint32
        pmf_pgm_ndx_min,
        pmf_pgm_ndx_max,
        pmf_pgm_ndx,
        keys_bmp,
        place_cons,
        res = SOC_SAND_OK;
    uint8
        is_equal;
    ARAD_PMF_DB_INFO
        db_info;
    SOC_PPC_FP_DATABASE_STAGE
        stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *is_key_fixed = FALSE;
    *key_id = 0;
    *key_place = 0;
    *key_bitmap_constraint = 0;

    
    
    res = arad_pp_fp_db_stage_get(
            unit,
            db_id_ndx,
            &stage
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(
            unit,
            stage,
            db_id_ndx,
            &db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

    
    res = arad_pp_fp_db_is_equal_place_get(
            unit,
            db_id_ndx,
            &is_equal,
            &place_cons 
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);

     
    if (is_equal) 
    {
      *is_key_fixed = TRUE;
      *key_id = 3;
      *key_place = place_cons;

      
      res = arad_pmf_prog_select_pmf_pgm_borders_get(
                unit,
                stage,
                0 , 
                &pmf_pgm_ndx_min,
                &pmf_pgm_ndx_max
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

      for (pmf_pgm_ndx = pmf_pgm_ndx_min; pmf_pgm_ndx < pmf_pgm_ndx_max; ++pmf_pgm_ndx)
      {
          if (SHR_BITGET(db_info.progs, pmf_pgm_ndx)) 
          {
              
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.get(
                        unit, 
                        stage, 
                        pmf_pgm_ndx, 
                        1 , 
                        &keys_bmp
                    );
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 49, exit);

              *key_bitmap_constraint |= keys_bmp;
          }
      }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_alloc_key_is_equal_get()", 0, 0);
}






uint32
  arad_pp_fp_key_alloc_key_bitmap_configure(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 db_id, 
      SOC_SAND_IN  uint32                 alloc_place, 
      SOC_SAND_IN  uint8                  is_to_set, 
      SOC_SAND_INOUT  uint32                 *keys_bmp, 
      SOC_SAND_INOUT  uint32                *key_id, 
      SOC_SAND_OUT  uint8                 *found
  )
{
    uint32
        nof_bits_option, 
        nof_bits_to_set = 0, 
        bit_id = 0, 
        bit_ndx, 
        place_ndx,
        key_bitmap_constraint_cascaded = 0,
        key_bitmap_constraint_compare = 0,
        nof_80s = 0,
        key_place_compare,
        keys_bmp_tmp,
        key_bmp_ndx = 0,
        res,
      index; 
    uint8
        key_ndx_cascaded,
        key_ndx_compare,
        is_key_fixed_cascaded,
        is_key_fixed_compare,
        key_bitmap_constraint_dir_ext = FALSE,
        key_ndx_dir_ext,
        is_for_upper_80,
        set_config;
    SOC_PPC_FP_DATABASE_INFO
    fp_database_info; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

      
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
              unit,
              SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, 
              db_id,
              &fp_database_info
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    for(place_ndx = 0; place_ndx < ARAD_PP_FP_NOF_KEY_CE_PLACES_STAGE; place_ndx++) {
      if (Arad_pp_fp_place_const_convert[place_ndx][0] == alloc_place) {
          
          nof_80s = Arad_pp_fp_place_const_convert[place_ndx][3];
      }
    }

    nof_bits_option = (nof_80s == ARAD_PP_FP_KEY_NUMBER_80_PARTS_IN_320B)? 
        (ARAD_PMF_LOW_LEVEL_NOF_KEYS / 2) : ARAD_PMF_LOW_LEVEL_NOF_KEYS ;
    is_for_upper_80 = (alloc_place == ARAD_PP_FP_KEY_CE_HIGH);
    for(index = 0; index < nof_bits_option; ++index)
    {
      
        if (nof_80s == 1) {
            bit_id =  (2 * index) + is_for_upper_80; 
            nof_bits_to_set = 1;
            if (is_to_set) {
              *key_id = index;
            }
            else if (*key_id != index) {
                
                continue;
            }
        }
        else if (nof_80s == 2) {
            bit_id =  2 * index;  
            nof_bits_to_set = 2;
            if (is_to_set) {
                *key_id = index;
            }
            else if (*key_id != index) {
                continue;
            }
        }
        
        else if (nof_80s == 4) {
          bit_id = index * 4; 
          nof_bits_to_set = 4;
          if (is_to_set) {
              *key_id = index * 2;
          }
          else if (*key_id != (index * 2)) {
              continue;
          }
        }

        
        set_config = TRUE;
        key_bitmap_constraint_dir_ext = FALSE;

        
        if (fp_database_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION  && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "optimized_de_allocation", 0)) {
            keys_bmp_tmp = *keys_bmp;
            for (key_bmp_ndx = 0 ; key_bmp_ndx < ARAD_PMF_LOW_LEVEL_NOF_KEYS ; key_bmp_ndx++)
            {
                if ( (is_for_upper_80 && (keys_bmp_tmp == 0x1) ) || (!is_for_upper_80 && (keys_bmp_tmp == 0x2)) )
                {
                    key_ndx_dir_ext = key_bmp_ndx;
                    key_bitmap_constraint_dir_ext = TRUE;
                    break;
                }
                keys_bmp_tmp = keys_bmp_tmp >> 2;

            }
            
            if (key_bitmap_constraint_dir_ext && key_ndx_dir_ext != *key_id) 
            {
                set_config = FALSE;
                continue;
            }
        }

        
        res = arad_pp_fp_key_alloc_key_cascaded_key_get(
                unit,
                db_id,
                &is_key_fixed_cascaded,
                &key_ndx_cascaded,
                &key_bitmap_constraint_cascaded 
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (SOC_IS_ARADPLUS(unit))
        {
            
            res = arad_pp_fp_key_alloc_key_is_equal_get(
                    unit,
                    db_id,
                    &is_key_fixed_compare,
                    &key_ndx_compare,
                    &key_place_compare,
                    &key_bitmap_constraint_compare 
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        
            
            if (is_key_fixed_compare) 
            {
                if (*key_id != key_ndx_compare)
                {
                    set_config = FALSE;
                    continue;
                }
            }
        }
        
        if (is_key_fixed_cascaded) 
        {
            if (*key_id != key_ndx_cascaded)
            {
                set_config = FALSE;
                continue;
            }

            if (SOC_IS_ARADPLUS(unit))
            {
                if (is_key_fixed_compare 
                    && (key_ndx_cascaded != key_ndx_compare)) 
                {
                    set_config = FALSE;
                    break;
                }
            }
        }
        else {
            *keys_bmp |= key_bitmap_constraint_cascaded | key_bitmap_constraint_compare;
            for(bit_ndx = 0; bit_ndx < nof_bits_to_set; ++bit_ndx)
            {
                if(SHR_BITGET(keys_bmp, bit_id + bit_ndx)){
                    set_config = FALSE;
                    break;
                }
            }
        }

        if (set_config || (!is_to_set)) {
            *found = 1;
            for(bit_ndx = 0; bit_ndx < nof_bits_to_set; ++bit_ndx)
            {
                if (is_to_set) {
                    SHR_BITSET(keys_bmp, bit_id + bit_ndx);
                }
                else {
                    SHR_BITCLR(keys_bmp, bit_id + bit_ndx);
                }
            }
            break;
        }

    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_alloc_key_bitmap_configure()", 0, 0);
}




uint32
  arad_pp_fp_key_alloc_key_id(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 prog_ndx,
      SOC_SAND_IN  uint32                 db_id,
      SOC_SAND_IN  uint32                 cycle_ndx, 
      SOC_SAND_IN  uint32                 alloc_place, 
      SOC_SAND_IN  uint32                 flags,
      SOC_SAND_OUT  uint32                *key_id, 
      SOC_SAND_OUT  uint8                 *found
  )
{
    uint32
      key_id_lcl = 0,
      keys_bmp = 0,
      tmp_key_a_bmp = 0,
      res = SOC_SAND_OK;
    ARAD_PMF_DB_INFO
       db_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = 0;

    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.get(
            unit,
            stage,
            prog_ndx,
            cycle_ndx,
            &keys_bmp  
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) && (cycle_ndx == 0) && !(flags & ARAD_PP_FP_KEY_ALLOC_USE_KEY_A))  {
            tmp_key_a_bmp = ( keys_bmp & 0x3 ); 
            keys_bmp |= 0x3; 
        }
    }

    res = arad_pp_fp_key_alloc_key_bitmap_configure(
            unit,
            stage,
            db_id,
            alloc_place,
            TRUE, 
            &keys_bmp, 
            &key_id_lcl, 
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
    *key_id = key_id_lcl;

    
    if(*found) {
        if (!(flags & ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY)) {
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) && (cycle_ndx == 0) && !(flags & ARAD_PP_FP_KEY_ALLOC_USE_KEY_A))  
                {
                   keys_bmp = (keys_bmp & ~(0x3) ) | tmp_key_a_bmp ;  
                }
            }
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.set(
                    unit,
                    stage,
                    prog_ndx,
                    cycle_ndx,
                    keys_bmp  
                  );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

            
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(unit, stage, db_id, &db_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
            db_info.cascaded_key = ARAD_PP_FP_KEY_TO_CASCADED_KEY_GET((*key_id));
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.set(unit, stage, db_id, &db_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

        }
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_alloc_key_id()", 0, 0);
}


uint32
  arad_pp_fp_key_alloc_ce(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                 prog_ndx,
      SOC_SAND_IN  uint32                 cycle_ndx, 
      SOC_SAND_IN  uint32                 place_cons,
      SOC_SAND_IN  uint32                 size_cons, 
      SOC_SAND_IN  uint32                 group_cons, 
      SOC_SAND_IN  uint32                 flags, 
      SOC_SAND_INOUT  uint32              *ce_rsrc_bmp, 
      SOC_SAND_INOUT  ARAD_PP_KEY_CE_ID   *cur_ce,
      SOC_SAND_OUT  uint8                 *found
  )
{
    uint32
      fit_ce[1] = {0},
      ce_rsrc_lcl[1],
      ce_ndx,
      index = 0;
    uint8
      in_use;
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = FALSE;
    *fit_ce = arad_pp_fp_key_get_available_ce_under_constraint(unit, stage, place_cons,size_cons,group_cons);
    if(flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_RSRC_BMP) {
        *fit_ce = *fit_ce & (~(*ce_rsrc_bmp)); 
    }
    
    if ((flags & ARAD_PP_FP_KEY_ALLOC_ELK_RANGE) && (place_cons == ARAD_PP_FP_KEY_CE_LOW)) {
        if (size_cons != ARAD_PP_FP_KEY_CE_SIZE_16) {
            *fit_ce = *fit_ce & 0x00f0; 
        } else {
            *fit_ce = *fit_ce & 0x0f00; 
        }
    }

    
    if(flags & ARAD_PP_FP_KEY_ALLOC_CE_WITH_ID) {
        index = *cur_ce;

        
        if(flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_RSRC_BMP) {
            in_use = FALSE;
        }
        else{
            
            res = arad_pp_fp_key_ce_is_in_use(
                    unit,
                    stage,
                    prog_ndx,
                    cycle_ndx,
                    index,
                    &in_use
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
        

        *found = TRUE;
    }
    else{ 
        for(ce_ndx = 0; ce_ndx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG; ++ce_ndx)
        {
            if (flags & ARAD_PP_FP_KEY_ALLOC_ELK_RANGE) {
                index = ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG - ce_ndx - 1;
            } else if (flags & ARAD_PP_FP_KEY_ALLOC_CE_FLP_NO_SPLIT) {
                
                index = ce_ndx;
            }
            else {
                
                index = ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG - ce_ndx - 1;
            }

          
          
          
          if(!SHR_BITGET(fit_ce,index)){
              continue;
          }

          if((flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_RSRC_BMP) || (flags & ARAD_PP_FP_KEY_ALLOC_CE_WITH_ID)) {
              in_use = FALSE;
          }
          else{
              res = arad_pp_fp_key_ce_is_in_use(
                      unit,
                      stage,
                      prog_ndx,
                      cycle_ndx,
                      index,
                      &in_use
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
          }

          
          if(!in_use) {
              *cur_ce = index;
              *found = TRUE;
              break;
          }
        }
    }

    
    if(*found && (flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_RSRC_BMP)) {
        SHR_BITSET(ce_rsrc_bmp,*cur_ce);
    }
    
    if(*found == TRUE && !(flags & ARAD_PP_FP_KEY_ALLOC_CE_CHECK_ONLY)) {
        res = arad_pp_fp_key_ce_set_use(
                unit,
                stage,
                prog_ndx,
                cycle_ndx,
                *cur_ce,
                in_use
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(
                  unit,
                  stage,
                  prog_ndx,
                  cycle_ndx,
                  ce_rsrc_lcl
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
        
        
        SHR_BITSET(ce_rsrc_lcl,*cur_ce);
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(
                  unit,
                  stage,
                  prog_ndx,
                  cycle_ndx,
                  *ce_rsrc_lcl
              );
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   "
                              "CE ALLOC: %d, cycle: %d, "
                              "*cur_ce:%d index %d"
                              "\n\r"), prog_ndx, cycle_ndx, *cur_ce, index));

        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
    }
     

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_alloc_ce()", 0, 0);
}






STATIC
 uint32
  arad_pp_fp_key_alloc_in_prog_commit(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
    SOC_SAND_IN  uint32                       prog_ndx,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], 
    SOC_SAND_INOUT  ARAD_PP_FP_KEY_ALLOC_INFO   *alloc_info
  )
{
    uint32
      res = SOC_SAND_OK,
      qual_index,
      ce_indx=0,
      cascaded_key_bitmap_constraint, 
      access_profile_array_id,
      nof_access_profiles,
      offset_start=0,
      access_profile_id,
      place_cons,
      nof_bits_zone[ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX][2],
      prog_used_cycle_bmp_lcl[1];
    uint8
        is_cascaded,
        lookup_id,
        cascaded_is_key_fixed,
        cascaded_key_id, 
        is_equal = 0,
        found = 0xff, 
        is_header = FALSE,
        ce_msb,
        is_320b=0,
        is_slb_hash = FALSE,
        is_slb_hash_found = FALSE,
        is_slb_hash_in_quals = FALSE;
    ARAD_PMF_CE_HEADER_QUAL_INFO
        hdr_qual_info;
    ARAD_PMF_CE_PACKET_HEADER_INFO
        cur_hdr_offset;
    ARAD_PMF_CE_IRPP_QUALIFIER_INFO
        inter_qual_info,
        cur_inter_qual_info;
    SOC_PPC_FP_QUAL_TYPE  
        cascaded_qual_type,              
        qual_type;
    ARAD_PMF_CE
        sw_db_ce;
    ARAD_PMF_DB_INFO 
        pmf_db_info; 
    ARAD_TCAM_BANK_ENTRY_SIZE
        entry_size;
    ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_DATA
        pass_2_key_update_tbl_data;
    SOC_PPC_FP_DATABASE_INFO
        db_info;
    soc_error_t soc_res;
    uint8 ii;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_memset(nof_bits_zone, 0x0, ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX * 2 * sizeof(uint32));
    soc_sand_os_memset(&sw_db_ce, 0, sizeof(sw_db_ce));
    ARAD_PMF_CE_PACKET_HEADER_INFO_clear(&cur_hdr_offset);



    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            stage, 
            db_id,
            &db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    for(ce_indx = 0; ce_indx < alloc_info->nof_ce; ++ce_indx) {

        if (!(flags&ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY))
        {
            if (flags & ARAD_PP_FP_KEY_ALLOC_CE_WITH_ID)
            {
                
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(
                        unit,
                        stage,
                        prog_ndx,
                        alloc_info->cycle, 
                        alloc_info->ce[ce_indx], 
                        &sw_db_ce
                      );
                
                if (!sw_db_ce.is_ce_shared)
                {
                    for (ii = 0; ii < 8; ii++)
                    {
                        sw_db_ce.ce_sharing_info[ii].db_id = (uint8)BCM_FIELD_ENTRY_INVALID;
                    }

                }
            }
        }

        qual_type = alloc_info->qual_type[ce_indx];
        ce_msb = alloc_info->ce[ce_indx] >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB;

        
        
        res = arad_pmf_ce_header_info_find(
                 unit,
                 qual_type,
                 stage,
                 &found,
                 &hdr_qual_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        
        if(found) {
            is_header = TRUE;
            
            
             cur_hdr_offset.sub_header = ((flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_HEADER_SELECTION) && 
                                          ((hdr_qual_info.qual_type == SOC_PPC_FP_QUAL_HDR_INNER_SA) || (hdr_qual_info.qual_type == SOC_PPC_FP_QUAL_HDR_INNER_DA) ) ) ? 
                                          hdr_qual_info.header_ndx_1 : hdr_qual_info.header_ndx_0;
        }
        
        else{
            is_header = FALSE;
            res = arad_pmf_ce_internal_field_info_find(
                     unit,
                     qual_type,
                     stage,
                     ce_msb,
                     &found,
                     &inter_qual_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            if(found == 0) {
               LOG_ERROR(BSL_LS_SOC_FP,
                         (BSL_META_U(unit,
                                     "   "
                                     "CE HW commit: "
                                     "qual_type %s unsupported \n\r"), SOC_PPC_FP_QUAL_TYPE_to_string(qual_type)));
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 60, exit); 
            }

            sal_memcpy(&cur_inter_qual_info,&inter_qual_info,sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_INFO));
        }

        
        if(is_header) {

          
            cur_hdr_offset.nof_bits = alloc_info->act_ce_const[ce_indx].nof_bits;
            cur_hdr_offset.nof_bits += alloc_info->act_ce_const[ce_indx].msb_padding_bits;
            
            cur_hdr_offset.offset = hdr_qual_info.lsb - alloc_info->act_ce_const[ce_indx].qual_lsb - 
                (alloc_info->act_ce_const[ce_indx].nof_bits - alloc_info->act_ce_const[ce_indx].lost_bits - 1);
            cur_hdr_offset.offset -= alloc_info->act_ce_const[ce_indx].msb_padding_bits;
            offset_start = cur_hdr_offset.offset;

            sw_db_ce.lsb = alloc_info->act_ce_const[ce_indx].lost_bits;
            sw_db_ce.qual_lsb = alloc_info->act_ce_const[ce_indx].qual_lsb;

            if (!(flags&ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY)) {
                
                res = arad_pmf_ce_packet_header_entry_set_unsafe(
                        unit,
                        stage,
                        prog_ndx,
                        alloc_info->key_id[alloc_info->ce_key[ce_indx]],
                        alloc_info->ce[ce_indx]%ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB, 
                        ce_msb, 
                        (alloc_info->cycle == ARAD_PP_FP_KEY_CYCLE_EVEN)?0:1, 
                        &cur_hdr_offset
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
            }

            
             
            sw_db_ce.db_id = db_id;
            sw_db_ce.is_used = 1;
            sw_db_ce.msb = cur_hdr_offset.nof_bits-1; 
            sw_db_ce.msb_padding = alloc_info->act_ce_const[ce_indx].msb_padding_bits;
            sw_db_ce.msb -= sw_db_ce.msb_padding; 
            sw_db_ce.qual_type = qual_type;
            sw_db_ce.is_msb = ce_msb;
            sw_db_ce.is_second_key = alloc_info->ce_key[ce_indx];
            nof_bits_zone[sw_db_ce.is_second_key][ce_msb] += cur_hdr_offset.nof_bits;
            
            if (sw_db_ce.is_second_key) {
                is_320b = 1;
            }
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "   "
                                  "CE HW commit: "
                                  "qual-lsb %d, "
                                  "qual-size %d, ce-size:%d, already-coppied:%d \n\r"),
                       sw_db_ce.qual_lsb, cur_hdr_offset.nof_bits,cur_hdr_offset.nof_bits,(cur_hdr_offset.offset - offset_start)));

        }
        else{ 

            
            cur_inter_qual_info.info.qual_nof_bits = alloc_info->act_ce_const[ce_indx].nof_bits;
            cur_inter_qual_info.info.qual_nof_bits += alloc_info->act_ce_const[ce_indx].msb_padding_bits;

            
            cur_inter_qual_info.info.buffer_lsb = alloc_info->act_ce_const[ce_indx].qual_lsb; 

            
            sw_db_ce.lsb = alloc_info->act_ce_const[ce_indx].lost_bits; 

            if (SOC_IS_JERICHO_PLUS(unit)) {
                is_slb_hash = FALSE;
                soc_res = arad_pp_fp_is_qual_identical(
                    unit,
                    qual_type,
                    SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, 
                    &is_slb_hash);
                SOC_SAND_CHECK_FUNC_RESULT(soc_res, 10, exit);

                
                if (is_slb_hash) {
                    is_slb_hash = (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) ? TRUE : FALSE;
                }
            }
            
            if (!(flags&ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY)) {
                if (!(SOC_IS_JERICHO_PLUS(unit) 
                    && ((qual_type == SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED) || is_slb_hash))) {
                    

                    
                    res = arad_pmf_ce_internal_info_entry_set_unsafe(
                            unit,
                            stage,
                            prog_ndx,
                            alloc_info->key_id[alloc_info->ce_key[ce_indx]],
                            alloc_info->ce[ce_indx]%ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB, 
                            ce_msb, 
                            (alloc_info->cycle == ARAD_PP_FP_KEY_CYCLE_EVEN)?0:1, 
                            FALSE, 
                            cur_inter_qual_info.info.buffer_lsb,
                            alloc_info->act_ce_const[ce_indx].lost_bits,
                            cur_inter_qual_info.info.qual_nof_bits,
                            qual_type
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
                }
            }

            
            sw_db_ce.db_id = db_id;
            sw_db_ce.is_used = 1;
            sw_db_ce.qual_lsb = cur_inter_qual_info.info.buffer_lsb;
            sw_db_ce.msb = cur_inter_qual_info.info.qual_nof_bits-1; 
            sw_db_ce.msb_padding = alloc_info->act_ce_const[ce_indx].msb_padding_bits;
            sw_db_ce.msb -= sw_db_ce.msb_padding; 
            sw_db_ce.qual_type = qual_type;
            sw_db_ce.is_msb = ce_msb;
            sw_db_ce.is_second_key = alloc_info->ce_key[ce_indx];
            nof_bits_zone[sw_db_ce.is_second_key][ce_msb] += cur_inter_qual_info.info.qual_nof_bits;

            
            if (sw_db_ce.is_second_key) {
                is_320b = 1;
            }


        }

        if (!(flags&ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY)) {

            if (flags & ARAD_PP_FP_KEY_ALLOC_CE_WITH_ID)
            {
                int ii = 0;
                sw_db_ce.is_ce_shared = TRUE;
                
                for (ii = 0; ii < 8; ii++)
                {
                    if (sw_db_ce.ce_sharing_info[ii].db_id == db_id)
                        break;
                    if (sw_db_ce.ce_sharing_info[ii].db_id == (uint8)BCM_FIELD_ENTRY_INVALID)
                        break;
                }
                if (ii < 8)
                {
                    sw_db_ce.ce_sharing_info[ii].db_id = db_id;
                    sw_db_ce.ce_sharing_info[ii].is_second_key = sw_db_ce.is_second_key;
                }
                else
                {
                    res = SOC_SAND_ERR;
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);
                }

            }
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.set(
                    unit,
                    stage,
                    prog_ndx,
                    (alloc_info->cycle == ARAD_PP_FP_KEY_CYCLE_EVEN)?0:1, 
                    alloc_info->ce[ce_indx], 
                    &sw_db_ce
                  );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);
       }
    }

    if (!(flags&ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY)) {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit,ARAD_PP_FP_DB_ID_TO_TCAM_DB(db_id), &entry_size);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 115, exit);
        nof_access_profiles = 0;
        if ((db_info.db_type != SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION) 
            && (db_info.db_type != SOC_PPC_FP_DB_TYPE_FLP)
            && (db_info.db_type != SOC_PPC_FP_DB_TYPE_SLB)) {
            nof_access_profiles = (is_320b || (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS))? 2 : 1; 
        }
        alloc_info->use_kaps = ( db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS ) ? TRUE : FALSE ;
        for (access_profile_array_id = 0; access_profile_array_id < nof_access_profiles; access_profile_array_id++) {
            
            res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(
                    unit,
                    ARAD_PP_FP_DB_ID_TO_TCAM_DB(db_id),
                    access_profile_array_id,
                    &access_profile_id
                  ); 
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);

            res = arad_pp_fp_key_access_profile_hw_set(
                    unit,
                    stage,
                    prog_ndx,
                    access_profile_array_id,
                    access_profile_id,
                    alloc_info
                  );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 145, exit);
        }


        
        
        res = arad_pp_fp_db_cascaded_cycle_get(
                unit,
                db_id,
                &is_cascaded,
                &lookup_id 
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

        if (SOC_IS_ARADPLUS(unit))
        {
            
            res = arad_pp_fp_db_is_equal_place_get(
                    unit,
                    db_id,
                    &is_equal,  
                    &place_cons 
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

            
            soc_res = arad_pp_fp_is_qual_in_qual_arr(unit, 
                                                     qual_types, 
                                                     ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS, 
                                                     SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, 
                                                     &is_slb_hash_in_quals);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(soc_res, 151, exit);
        }

        if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) &&
                ((is_cascaded && (((lookup_id == 0) && !SOC_IS_JERICHO_PLUS(unit)) || SOC_IS_JERICHO_PLUS(unit))) 
                || is_equal || is_slb_hash_in_quals))
        {
            

            if (!SOC_IS_JERICHO_PLUS(unit))
            {
                res = arad_pp_ihb_pmf_pass_2_key_update_tbl_get_unsafe(
                        unit,
                        prog_ndx,
                        &pass_2_key_update_tbl_data
                  );
                SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

            }


            if (is_cascaded) 
            {
                
                if (SOC_IS_JERICHO_PLUS(unit)) 
                {
                    res = arad_pp_fp_key_alloc_key_cascaded_key_get(
                            unit,
                            db_info.cascaded_coupled_db_id,
                            &cascaded_is_key_fixed,
                            &cascaded_key_id,
                            &cascaded_key_bitmap_constraint
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

                    
                    if (!cascaded_is_key_fixed && (lookup_id == 0)) {
                      LOG_ERROR(BSL_LS_SOC_FP,
                                (BSL_META_U(unit,
                                            "   Error in cascaded configuration: "
                                            "For database %d, stage %s, the database is indicated to be cascaded with DB-Id %d."
                                            "This latest Database is not set as cascaded. \n\r"), 
                                 db_id, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), db_info.cascaded_coupled_db_id));
                        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 50, exit); 
                    }

                    if(db_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE && 
                       (db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS)) 
                    {
                        cascaded_qual_type = SOC_PPC_FP_QUAL_IRPP_KAPS_PASS1_PAYLOAD;
                    }
                    else
                    {
                        
                        if (lookup_id == 0) {                            
                            cascaded_qual_type = SOC_PPC_FP_QUAL_IRPP_TCAM_0_RESULT + alloc_info->key_id[0];
                        } else {
                            cascaded_qual_type = SOC_PPC_FP_QUAL_IRPP_TCAM_0_RESULT;
                        }
                    }

                    
                    if (lookup_id != 0) {
                        cascaded_key_id = alloc_info->key_id[0];
                    }

                    
                    res = arad_pmf_ce_internal_info_entry_set_unsafe(
                            unit,
                            stage,
                            prog_ndx,
                            cascaded_key_id,
                            7, 
                            FALSE, 
                            1, 
                            TRUE, 
                            0,
                            0,
                            0,
                            cascaded_qual_type
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

                } else {
                    pass_2_key_update_tbl_data.enable_update_key = 0x1;
                    pass_2_key_update_tbl_data.action_select = alloc_info->key_id[0]; 
                }

                if(SOC_IS_JERICHO(unit) && ( db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS ) )
                {
                    pass_2_key_update_tbl_data.action_select = ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_ACTION_SELECT_KAPS;
                }
            }
            if (SOC_IS_ARADPLUS(unit))
            {
                if (is_equal) 
                {
                    soc_reg_above_64_val_t reg_above_64, field_above_64;
                    uint32 xor_mask, xor_input_selection;

                    if (SOC_IS_JERICHO_PLUS(unit))
                    {
                        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
                        SOC_REG_ABOVE_64_CLEAR(field_above_64);
                        SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, READ_IHB_XOR_MASKSr(unit, reg_above_64));
                        SHR_BITSET_RANGE(field_above_64, 0, 80);
                        soc_reg_above_64_field_set(unit, IHB_XOR_MASKSr, reg_above_64, XOR_MASK_3f, field_above_64);
                        SOC_SAND_SOC_IF_ERROR_RETURN(res, 166, exit, WRITE_IHB_XOR_MASKSr(unit, reg_above_64));

                        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
                        SOC_REG_ABOVE_64_CLEAR(field_above_64);
                        SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, READ_IHB_FEM_FES_COMPARE_SELECTIONr(unit, reg_above_64));
                        soc_reg_above_64_field_get(unit, IHB_FEM_FES_COMPARE_SELECTIONr, reg_above_64, XOR_MASK_SELECTf, field_above_64);
                        xor_mask = 0x3;
                        SHR_BITCOPY_RANGE(field_above_64,(prog_ndx*2),&xor_mask,0,2);
                        soc_reg_above_64_field_set(unit, IHB_FEM_FES_COMPARE_SELECTIONr, reg_above_64, XOR_MASK_SELECTf, field_above_64);
                        SOC_SAND_SOC_IF_ERROR_RETURN(res, 166, exit, WRITE_IHB_FEM_FES_COMPARE_SELECTIONr(unit, reg_above_64));

                        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
                        SOC_REG_ABOVE_64_CLEAR(field_above_64);
                        SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, READ_IHB_FEM_FES_COMPARE_SELECTIONr(unit, reg_above_64));
                        soc_reg_above_64_field_get(unit, IHB_FEM_FES_COMPARE_SELECTIONr, reg_above_64, XOR_INPUT_SELECTf, field_above_64);
                        xor_input_selection = 0x2;
                        SHR_BITCOPY_RANGE(field_above_64,(prog_ndx*2),&xor_input_selection,0,2);
                        soc_reg_above_64_field_set(unit, IHB_FEM_FES_COMPARE_SELECTIONr, reg_above_64, XOR_INPUT_SELECTf, field_above_64);
                        SOC_SAND_SOC_IF_ERROR_RETURN(res, 166, exit, WRITE_IHB_FEM_FES_COMPARE_SELECTIONr(unit, reg_above_64));  
                    }
                    else
                    {
                        int core;

                        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                            SOC_REG_ABOVE_64_CLEAR(reg_above_64);
                            SOC_REG_ABOVE_64_CLEAR(field_above_64);
                            SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, READ_IHB_KEY_D_XOR_MASKSr(unit, core, reg_above_64));
                            SHR_BITSET_RANGE(field_above_64, 0, 80);
                            soc_reg_above_64_field_set(unit, IHB_KEY_D_XOR_MASKSr, reg_above_64, KEY_D_XOR_MASK_0f, field_above_64);
                            SOC_SAND_SOC_IF_ERROR_RETURN(res, 166, exit, WRITE_IHB_KEY_D_XOR_MASKSr(unit, core, reg_above_64));
                        }

                        pass_2_key_update_tbl_data.key_d_use_compare_result = 0x1;
                        pass_2_key_update_tbl_data.key_d_mask_select = 0x0;
                        pass_2_key_update_tbl_data.key_d_xor_enable = 0x1;
                    }
                }

                if (is_slb_hash_in_quals) {
                    if (SOC_IS_JERICHO_PLUS(unit)) {
                        soc_mem_t mem;
                        for (qual_index=0; qual_index<SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++qual_index) {
                            uint32 reg;
                            
                            uint32 data[5];
                            int key_idx;
                            int core;

                            mem = (alloc_info->alloc_place == ARAD_PP_FP_KEY_CE_HIGH) ? IHB_PMF_UPDATE_KEY_GEN_MSBm : IHB_PMF_UPDATE_KEY_GEN_LSBm;
                            if (qual_types[qual_index] == SOC_PPC_FP_QUAL_KEY_AFTER_HASHING) {
                                
                                is_slb_hash_found = TRUE;
                                res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, prog_ndx, &data);
                                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 167, exit);

                                
                                key_idx = alloc_info->key_id[0];
                                data[2] = 0xfe087dc8;
                                data[3] = 0xfe48;
                                data[4] = 0x70 << (key_idx * 8);
                            }
                            else {
                                
                                if (SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(qual_types[qual_index])) {
                                    
                                    res = arad_pmf_ce_internal_field_info_find(unit,qual_types[qual_index],stage,alloc_info->alloc_place,&found,&inter_qual_info);
                                    SOC_SAND_CHECK_FUNC_RESULT(res, 89, exit);

                                    if (found && (inter_qual_info.info.irpp_field == SOC_PPC_FP_QUAL_KEY_AFTER_HASHING)) {
                                        
                                        is_slb_hash_found = TRUE;

                                        res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, prog_ndx, &data);
                                        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 168, exit);

                                        key_idx = alloc_info->key_id[0];
                                        
                                        data[3] |= 8;
                                        data[3] |= (inter_qual_info.info.qual_nof_bits-1) << 11;
                                        data[3] |= (0x64 - inter_qual_info.info.buffer_lsb) << 4;
                                        data[4] |= 0x40 << (key_idx * 8);                                  
                                    }
                                }
                            }

                            if (is_slb_hash_found) {
                                
                                SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, READ_IHB_PMF_GENERALr(unit, core, &reg));
                                    soc_reg_field_set(unit, IHB_PMF_GENERALr, &reg, DISABLE_2ND_PASS_KEY_UPDATEf, 0);
                                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, WRITE_IHB_PMF_GENERALr(unit, core, reg));
                                }
                                res = soc_mem_write(unit, mem, MEM_BLOCK_ANY, prog_ndx, data);

                                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 171, exit);

                                
                                break;
                            }
                        }
                    } else {
                        
                        
                        
                        uint32 *key_fields[4];
                        int i = 0;
                        int key_idx = alloc_info->key_id[0];

                        key_fields[i++] = &pass_2_key_update_tbl_data.key_a_lem_operation_select;
                        key_fields[i++] = &pass_2_key_update_tbl_data.key_b_lem_operation_select;
                        key_fields[i++] = &pass_2_key_update_tbl_data.key_c_lem_operation_select;
                        key_fields[i++] = &pass_2_key_update_tbl_data.key_d_lem_operation_select;

                        
                        *(key_fields[key_idx]) = 1;
                    }
                 }
            }

            if (!SOC_IS_JERICHO_PLUS(unit))
            {
                res = arad_pp_ihb_pmf_pass_2_key_update_tbl_set_unsafe(
                        unit,
                        prog_ndx,
                        &pass_2_key_update_tbl_data
                  );
                SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
            }
        }
    }

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(
            unit,
            stage,
            db_id,
            &pmf_db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 180, exit);

    *prog_used_cycle_bmp_lcl = pmf_db_info.prog_used_cycle_bmp;
    if(alloc_info->cycle) {
        SHR_BITSET(prog_used_cycle_bmp_lcl, prog_ndx);
    }
    else{
        SHR_BITCLR(prog_used_cycle_bmp_lcl, prog_ndx);
    }
    pmf_db_info.prog_used_cycle_bmp = *prog_used_cycle_bmp_lcl;
    pmf_db_info.used_key[prog_ndx][0] = alloc_info->key_id[0];
    pmf_db_info.used_key[prog_ndx][1] = alloc_info->key_id[1];
    pmf_db_info.is_320b = is_320b;
    pmf_db_info.alloc_place = alloc_info->alloc_place;
    pmf_db_info.nof_bits_zone[0][0] = nof_bits_zone[0][0];
    pmf_db_info.nof_bits_zone[0][1] = nof_bits_zone[0][1];
    pmf_db_info.nof_bits_zone[1][0] = nof_bits_zone[1][0];
    pmf_db_info.nof_bits_zone[1][1] = nof_bits_zone[1][1];
    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.set(
            unit,
            stage,
            db_id,
            &pmf_db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 190, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_alloc_in_prog_commit()", prog_ndx, ce_indx);
}

STATIC
 uint32
  arad_pp_fp_key_ce_rsrc_normalize(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                       flags,
      SOC_SAND_IN  uint32                       cur_rsrc
    )
{
    uint32
        tmp_rsrc[1],
        new_val,
        indx,
        new_rsrc_bmp=0;
    int
        set_count;

    *tmp_rsrc = cur_rsrc;
    
    
    for(indx = 0; indx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG; indx+=ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP) {
        
        shr_bitop_range_count(tmp_rsrc, indx, ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP, &set_count);

        
        if ((stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)
            && (flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER) 
            && arad_pmf_low_level_ce_is_32b_ce(unit, stage, indx)) {
            set_count = ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP;
        }

        
        new_val = (1 << set_count) - 1;
        SHR_BITCOPY_RANGE(&new_rsrc_bmp, indx, &new_val, 0, ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP);  
    }
    return new_rsrc_bmp;
}


uint32
  arad_pp_fp_key_alloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
    SOC_SAND_IN  uint32                       prog_ndx,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       cycle_cons, 
    SOC_SAND_IN  uint32                       place_cons,
    SOC_SAND_IN  uint32                       start_place,
    SOC_SAND_IN ARAD_PP_FP_CE_CONSTRAINT      ce_cons[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS], 
    SOC_SAND_IN  uint32                       nof_ce_const,
   
    SOC_SAND_OUT  ARAD_PP_FP_KEY_ALLOC_INFO   *alloc_info,
    SOC_SAND_OUT  uint8                       *key_alloced
  )
{
  int32
    alloc_exact = 1;
  uint32
      size_cons,
      group_cons,
      place_ndx,
      start_place_ndx = 0,
      end_place_ndx = 0,
      alloc_place_ndx,
      alloc_place = 0,
      alloc_place_tmp = 0,
      cycle_first,
      cycle_last,
      cycle=0,
      ce_rsrc,
      key_id[2],
      nof_keys,
      ce_index;
  uint8
      success = FALSE,
      is_slb_hash_in_quals = FALSE;
  ARAD_PP_KEY_CE_ID
      ce_id;
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_INFO
    fp_database_info; 
  soc_error_t 
    soc_res;
     
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *key_alloced = 0;
  sal_memset(key_id, 0, sizeof(key_id));


  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, 
          db_id,
          &fp_database_info
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  
  cycle_first = (cycle_cons != ARAD_PP_FP_KEY_CYCLE_ANY)? cycle_cons : ARAD_PP_FP_KEY_CYCLE_EVEN;
  cycle_last = (cycle_cons != ARAD_PP_FP_KEY_CYCLE_ANY)? cycle_cons : ARAD_PP_FP_KEY_CYCLE_ODD;

  
  for(place_ndx = 0; place_ndx < ARAD_PP_FP_NOF_KEY_CE_PLACES_STAGE; place_ndx++) {
      if (Arad_pp_fp_place_const_convert[place_ndx][0] == start_place) {
          
          start_place_ndx = place_ndx;
      }
      if (Arad_pp_fp_place_const_convert[place_ndx][0] == place_cons) {
          
          end_place_ndx = place_ndx;
      }
  }

  
  for(alloc_place_ndx = start_place_ndx; alloc_place_ndx <= end_place_ndx; alloc_place_ndx++) {
      alloc_place = Arad_pp_fp_place_const_convert[alloc_place_ndx][0];
      nof_keys = Arad_pp_fp_place_const_convert[alloc_place_ndx][1];

      
      if (fp_database_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION) {
          
          if (alloc_place_ndx == end_place_ndx) {
              alloc_place = ARAD_PP_FP_KEY_CE_LOW;
          }
      }

      if (SOC_IS_ARADPLUS(unit))
      {
          
          if (fp_database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_IS_EQUAL_LSB) {
              alloc_place = ARAD_PP_FP_KEY_CE_LOW;
          }
          else if (fp_database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_IS_EQUAL_MSB) {
              alloc_place = ARAD_PP_FP_KEY_CE_HIGH;
          }


      }

      
      for(alloc_exact = 1; alloc_exact >= 0; --alloc_exact) {
          for (cycle = cycle_first; cycle <= cycle_last; ++cycle) {

              LOG_DEBUG(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "   "
                                    "allocating key + CE for db: %d program: %d, "
                                    "alloc_place(high|low|high|low): %d alloc_exact (no, yes): %d cycle(0,1,..):%d \n\r"),
                         db_id, prog_ndx, alloc_place, alloc_exact, cycle));

              
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(
                        unit,
                        stage,
                        prog_ndx,
                        cycle,
                        &ce_rsrc
                    );
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
              
              ce_rsrc |= arad_pp_fp_key_ce_rsrc_normalize(unit, stage, flags, 0 );

              
              if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB) {
                  ce_rsrc = 0;
              }

              
              soc_res = arad_pp_fp_is_qual_in_qual_arr(unit, 
                                                       qual_types, 
                                                       SOC_PPC_FP_NOF_QUALS_PER_DB_MAX, 
                                                       SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, 
                                                       &is_slb_hash_in_quals);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(soc_res, 151, exit);

              if (is_slb_hash_in_quals && !SOC_IS_JERICHO_PLUS(unit)) {
                  alloc_place = ARAD_PP_FP_KEY_CE_HIGH;
              }

			  
              res = arad_pp_fp_key_alloc_key_id(
                       unit,
                       stage,
                       prog_ndx,
                       db_id,
                       cycle,
                       alloc_place, 
                       flags|ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY, 
                       &key_id[0],
                       &success
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
              if(!success) { 
                  continue;
              }
              else if (nof_keys > 1) {
                  key_id[1] = key_id[0] + 1;  
              }
              if (nof_keys == 2) {
                  LOG_DEBUG(BSL_LS_SOC_FP,
                            (BSL_META_U(unit,
                                        "   "
                                        "PMF-Program: %d, cycle: %d, "
                                        "keys:%d and %d "
                                        "allocated for db:%d \n\r"), prog_ndx, cycle, key_id[0], key_id[1], db_id));
              }
              else { 
                  LOG_DEBUG(BSL_LS_SOC_FP,
                            (BSL_META_U(unit,
                                        "   "
                                        "PMF-Program: %d, cycle: %d, "
                                        "key:%d "
                                        "allocated for db:%d \n\r"), prog_ndx, cycle, key_id[0], db_id));
              }
              alloc_info->nof_ce = 0;
              
              for(ce_index = 0; ce_index  < nof_ce_const; ++ce_index) {

                  
                  size_cons = ((ce_cons[ce_index].qual_type == SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED) 
                                && (!(flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER)))?
                      ARAD_PP_FP_KEY_CE_SIZE_32: ce_cons[ce_index].size_cons;

                  LOG_DEBUG(BSL_LS_SOC_FP,
                            (BSL_META_U(unit,
                                        "   "
                                        "PMF-Program: %d, cycle: %d, "
                                        "key:%d "
                                        "try to allocate qual %15s for db:%d size_cons %d"
                                        " nof_ce_const %d ce_index %d ce_id %d\n\r"), prog_ndx, cycle, key_id[0], SOC_PPC_FP_QUAL_TYPE_to_string(ce_cons[ce_index].qual_type),db_id, size_cons,
                                    nof_ce_const, ce_index, alloc_info->ce[ce_index]));
                  group_cons = ((ce_cons[ce_index].qual_type == SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED) 
                                && (!(flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER)))?
                      ARAD_PP_FP_KEY_CE_HIGH: ce_cons[ce_index].group_cons;
                  if(flags & ARAD_PP_FP_KEY_ALLOC_CE_WITH_ID)
                  {
                      ce_id = alloc_info->ce[ce_index];
                      size_cons = ce_cons[ce_index].size_cons;
                  }

                  
                  alloc_place_tmp = ce_cons[ce_index].place_cons & alloc_place;

                  soc_res = arad_pp_fp_is_qual_identical(
                      unit,
                      ce_cons[ce_index].qual_type,
                      SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, 
                      &is_slb_hash_in_quals);
                  SOC_SAND_CHECK_FUNC_RESULT(soc_res, 10, exit);

                  if (SOC_IS_JERICHO_PLUS(unit) 
                    && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)
                    && is_slb_hash_in_quals) {
                      if (ce_cons[ce_index].qual_type == SOC_PPC_FP_QUAL_KEY_AFTER_HASHING) {
                          alloc_place_tmp = ARAD_PP_FP_KEY_CE_PLACE_HASH_KEY;
                      } else {
                          if (SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(ce_cons[ce_index].qual_type)) {
                              SOC_PPC_FP_CONTROL_INDEX index;
                              SOC_PPC_FP_CONTROL_INFO info;
  
                              SOC_PPC_FP_CONTROL_INDEX_clear(&index);
                              SOC_PPC_FP_CONTROL_INFO_clear(&info);
  
                              index.type = SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF;
                              index.val_ndx = ce_cons[ce_index].qual_type - SOC_PPC_FP_QUAL_HDR_USER_DEF_0;
                              res = soc_ppd_fp_control_get(unit, SOC_CORE_INVALID, &index, &info);
                              SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  
                              
                              if ((info.val[0] == SOC_TMC_NOF_PMF_CE_SUB_HEADERS) 
                                  && (info.val[3] == SOC_PPC_FP_QUAL_KEY_AFTER_HASHING)) {
                                  alloc_place_tmp = ARAD_PP_FP_KEY_CE_PLACE_HASH_KEY;
                              }
                          }
                      }                      
                  }

                  res = arad_pp_fp_key_alloc_ce(
                           unit,
                           stage,
                           prog_ndx,
                           cycle,
                           alloc_place_tmp,
                           size_cons, 
                           group_cons, 
                           flags|ARAD_PP_FP_KEY_ALLOC_CE_USE_RSRC_BMP,
                           &ce_rsrc,
                           &ce_id,
                           &success
                        );
                  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

				  if(success) {
                      arad_pp_fp_key_ce_to_ce_const(unit, stage, ce_id, &alloc_info->act_ce_const[alloc_info->nof_ce]);
                      alloc_info->act_ce_const[alloc_info->nof_ce].lost_bits = ce_cons[alloc_info->nof_ce].lost_bits;
                      alloc_info->act_ce_const[alloc_info->nof_ce].nof_bits = ce_cons[alloc_info->nof_ce].nof_bits;
                      alloc_info->act_ce_const[alloc_info->nof_ce].qual_lsb = ce_cons[alloc_info->nof_ce].qual_lsb;
                      alloc_info->ce[alloc_info->nof_ce] = ce_id;
                      alloc_info->ce_key[alloc_info->nof_ce] = ce_cons[ce_index].is_second_key;
                      alloc_info->qual_type[alloc_info->nof_ce] = ce_cons[ce_index].qual_type;
                      ++alloc_info->nof_ce;
                      LOG_DEBUG(BSL_LS_SOC_FP,
                                (BSL_META_U(unit,
                                            "   "
                                            "program: %d cycle: %d CE-ID:%d allocated for qualifier:%s \n\r"), prog_ndx, cycle, ce_id, SOC_PPC_FP_QUAL_TYPE_to_string(ce_cons[ce_index].qual_type)));
                  }

                  if(!success) {
                      LOG_DEBUG(BSL_LS_SOC_FP,
                                (BSL_META_U(unit,
                                            "   "
                                            "program: %d cycle: %d CE-ID:%d allocation not succeeded for qualifier:%s \n\r"), prog_ndx, cycle, ce_id, SOC_PPC_FP_QUAL_TYPE_to_string(ce_cons[ce_index].qual_type)));
                      break; 
                  }
              }

              if(success && ce_index == nof_ce_const) {
                  *key_alloced = 1;  
                  alloc_info->key_id[0] = key_id[0];
                  alloc_info->key_id[1] = key_id[1];
                  alloc_info->cycle = cycle;
                  alloc_info->alloc_place = alloc_place;
                  
                  alloc_info->key_size = Arad_pp_fp_place_const_convert[alloc_place_ndx][2];

                  
                  res = arad_pp_fp_key_alloc_key_id(
                           unit,
                           stage,
                           prog_ndx,
                           db_id,
                           cycle,
                           alloc_place, 
                           flags, 
                           &key_id[0],
                           &success
                        );
                  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

                  goto exit;
              }
              else{
                  LOG_DEBUG(BSL_LS_SOC_FP,
                            (BSL_META_U(unit,
                                        "   "
                                        "program: %d cycle: %d allocation not succeeded %d, nof_ce_const %d \n\r"), prog_ndx, cycle, success, nof_ce_const));
              }
          } 
          
          if(flags & ARAD_PP_FP_KEY_ALLOC_USE_CE_CONS) {
             LOG_ERROR(BSL_LS_SOC_FP,
                       (BSL_META_U(unit,
                                   "    "
                                   "Key: fail to allocate in all cycle with given sizes/places  \n\r")));
              goto exit;
          }

      } 
  } 

  
exit:
  
  if(success) {

      LOG_DEBUG(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "   "
                            "commit allocation to hw, nof CE used:%d , prog_ndx %d, flags %d\n\r"), alloc_info->nof_ce, prog_ndx, flags));

      res = arad_pp_fp_key_alloc_in_prog_commit(
                unit,
                stage,
                prog_ndx,
                db_id,
                flags,
                qual_types,
                alloc_info
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit2);
  }

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "allocation succeeded:%d  \n\r"), *key_alloced));

exit2:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_alloc_in_prog()", alloc_place, cycle);
}

#if defined(INCLUDE_KBP)
uint32
  arad_pp_fp_elk_key_alloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
    SOC_SAND_IN  uint32                       prog_ndx,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], 
    SOC_SAND_IN ARAD_PP_FP_CE_CONSTRAINT      ce_cons[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS], 
    SOC_SAND_IN  uint32                       nof_ce_const,
   
    SOC_SAND_OUT  ARAD_PP_FP_KEY_ALLOC_INFO   *alloc_info,
    SOC_SAND_OUT  uint8                       *key_alloced
  )
{
  uint32
      cycle,
      ce_rsrc,
      ce_index,
      size_cons,
      alloc_ndx,
      res = SOC_SAND_OK;
  uint8
      success = FALSE;
  ARAD_PP_KEY_CE_ID
      ce_id_highest = 0,
      ce_id;

  SOC_PPC_FP_DATABASE_INFO
    fp_database_info; 
  uint32 group_cons_tmp;   
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  cycle = 0,

  *key_alloced = 0;
  ARAD_PP_FP_KEY_ALLOC_INFO_clear(alloc_info);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, 
          db_id,
          &fp_database_info
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(
            unit,
            stage,
            prog_ndx,
            cycle,
            &ce_rsrc
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  for(ce_index = 0; ce_index  < nof_ce_const; ++ce_index) 
  {
      
      for (alloc_ndx = 0; alloc_ndx < 2; alloc_ndx++) 
      {
          size_cons = (alloc_ndx == 0) ? ce_cons[ce_index].size_cons : ARAD_PP_FP_KEY_CE_SIZE_32;
            if ((flags & ARAD_PP_FP_KEY_ALLOC_ELK_RANGE) && (ce_cons[ce_index].place_cons == ARAD_PP_FP_KEY_CE_LOW)) {
                group_cons_tmp = ARAD_PP_FP_KEY_CE_PLACE_ANY;
            } else {
                group_cons_tmp = ce_cons[ce_index].group_cons;
            }
          
          res = arad_pp_fp_key_alloc_ce(
                   unit,
                   stage,
                   prog_ndx,
                   cycle,
                   ce_cons[ce_index].place_cons,
                   size_cons, 
                   group_cons_tmp, 
                   flags|ARAD_PP_FP_KEY_ALLOC_CE_USE_RSRC_BMP,
                   &ce_rsrc,
                   &ce_id,
                   &success
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          
          if(success) {
              break;
          }
      }

      if(success) {
          arad_pp_fp_key_ce_to_ce_const(unit, stage, ce_id, &alloc_info->act_ce_const[alloc_info->nof_ce]);
          alloc_info->act_ce_const[alloc_info->nof_ce].lost_bits = ce_cons[alloc_info->nof_ce].lost_bits;
          alloc_info->act_ce_const[alloc_info->nof_ce].nof_bits = ce_cons[alloc_info->nof_ce].nof_bits;
          alloc_info->act_ce_const[alloc_info->nof_ce].qual_lsb = ce_cons[alloc_info->nof_ce].qual_lsb;
          alloc_info->ce[alloc_info->nof_ce] = ce_id;
          alloc_info->ce_key[alloc_info->nof_ce] = ce_cons[ce_index].is_second_key;
          alloc_info->qual_type[alloc_info->nof_ce] = ce_cons[ce_index].qual_type;
          ++alloc_info->nof_ce;
          ce_id_highest = SOC_SAND_MAX(ce_id_highest, ce_id);

          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "   "
                                "Stage: FLP, program: %d cycle: %d CE-ID:%d alloced for qualifier:%s \n\r"), 
                     prog_ndx, cycle, ce_id, SOC_PPC_FP_QUAL_TYPE_to_string(ce_cons[ce_index].qual_type)));
      }
  }

  
  if(success && ce_index == nof_ce_const) {
      *key_alloced = 1;  
      alloc_info->key_id[0] = 2; 
      alloc_info->cycle = cycle;
      alloc_info->alloc_place = ARAD_PP_FP_KEY_CE_LOW;
      alloc_info->key_size = ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS;
  }
  else{
      LOG_DEBUG(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "   "
                            "program: %d cycle: %d allocation not succeeded %d, nof_ce_const %d \n\r"), prog_ndx, cycle, success, nof_ce_const));
  }
  
  
  if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) && (!(flags & ARAD_PP_FP_KEY_ALLOC_CE_CHECK_ONLY))) {
      uint32 total_lsb_bits = 0;
      uint32 total_msb_bits = 0;
      int lsb_padding_bits = 0;
      int msb_padding_bits = 0;
      uint32 ce_copy_bits = 0;

      SHR_BITSET_RANGE(&ce_rsrc, 0, ce_id_highest); 

      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(unit, stage, prog_ndx, cycle, ce_rsrc);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);

      
      for(ce_index = 0; ce_index < alloc_info->nof_ce; ++ce_index) {
        if (alloc_info->ce[ce_index] < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB) {
            total_lsb_bits += alloc_info->act_ce_const[ce_index].nof_bits;
        }
        else {
            total_msb_bits += alloc_info->act_ce_const[ce_index].nof_bits;
        }
      }

      lsb_padding_bits = SOC_SAND_DIV_ROUND_UP(total_lsb_bits, 8)*8 - total_lsb_bits;
      if (lsb_padding_bits > 0) {
          for(ce_index = 0; (ce_index < alloc_info->nof_ce) && (lsb_padding_bits > 0); ++ce_index) {
              if (alloc_info->ce[ce_index] < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB) {
                  ce_copy_bits = alloc_info->act_ce_const[ce_index].nof_bits;

                  if (arad_pmf_low_level_ce_is_32b_ce(unit, stage, alloc_info->ce[ce_index]) && (ce_copy_bits<32)) {
                      alloc_info->act_ce_const[ce_index].msb_padding_bits
                          = SOC_SAND_MIN(ce_copy_bits+lsb_padding_bits, 32) - ce_copy_bits;
                      lsb_padding_bits -= alloc_info->act_ce_const[ce_index].msb_padding_bits;
                  }
                  else if (ce_copy_bits < 16) {
                      alloc_info->act_ce_const[ce_index].msb_padding_bits
                          = SOC_SAND_MIN(ce_copy_bits+lsb_padding_bits, 16) - ce_copy_bits;
                      lsb_padding_bits -= alloc_info->act_ce_const[ce_index].msb_padding_bits;
                  }
              }
          }
      }

      msb_padding_bits = SOC_SAND_DIV_ROUND_UP(total_msb_bits, 8)*8 - total_msb_bits;
      if (msb_padding_bits > 0) {
          for(ce_index = 0; (ce_index < alloc_info->nof_ce) && (msb_padding_bits > 0); ++ce_index) {
              if (alloc_info->ce[ce_index] >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB) {
                  ce_copy_bits = alloc_info->act_ce_const[ce_index].nof_bits;

                  if (arad_pmf_low_level_ce_is_32b_ce(unit, stage, alloc_info->ce[ce_index]) && (ce_copy_bits<32)) {
                      alloc_info->act_ce_const[ce_index].msb_padding_bits
                          = SOC_SAND_MIN(ce_copy_bits+msb_padding_bits, 32) - ce_copy_bits;
                      msb_padding_bits -= alloc_info->act_ce_const[ce_index].msb_padding_bits;
                  }
                  else if (ce_copy_bits < 16) {
                      alloc_info->act_ce_const[ce_index].msb_padding_bits
                          = SOC_SAND_MIN(ce_copy_bits+msb_padding_bits, 16) - ce_copy_bits;
                      msb_padding_bits -= alloc_info->act_ce_const[ce_index].msb_padding_bits;
                  }
              }
          }
      }
  }


exit:
  
  if(success) {

      LOG_DEBUG(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "   "
                            "commit allocation to hw, nof CE used:%d \n\r"), alloc_info->nof_ce));

      res = arad_pp_fp_key_alloc_in_prog_commit(
                unit,
                stage,
                prog_ndx,
                db_id,
                flags,
                qual_types,
                alloc_info
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit2);
  }

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "allocation succeeded:%d  \n\r"), *key_alloced));

exit2:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_elk_key_alloc_in_prog()", 0, cycle);
}
#endif




STATIC
 uint32
  arad_pp_fp_key_ce_rsrc_select(
     SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                       rsrc1,
      SOC_SAND_IN  uint32                       rsrc2
    )
{
    uint32
        new_rsrc_bmp[1];
    int
        nof_set_bits1,
        nof_set_bits2;

    *new_rsrc_bmp = rsrc1;
    shr_bitop_range_count(new_rsrc_bmp,0,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG,&nof_set_bits1);


    *new_rsrc_bmp = rsrc2;
    shr_bitop_range_count(new_rsrc_bmp,0,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG,&nof_set_bits2);

    if(nof_set_bits1 <= nof_set_bits2) {
        return rsrc1;
    }

    return rsrc2;
}


uint32
  arad_pp_fp_key_cycle_no_320b_key_get(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE    stage,
      SOC_SAND_IN  uint32                    cur_prog,
      SOC_SAND_OUT uint8                    *no_320b_cycle_found, 
      SOC_SAND_OUT uint32                   *selected_cycle
  )
{
    uint32 
        key_ndx,
        keys_bmp, 
        cycle_ndx,
        fld_val,
        res;
    uint8
        is_key_found_for_320b[SOC_PPC_FP_NOF_CYCLES];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for(cycle_ndx = 0; cycle_ndx < SOC_PPC_FP_NOF_CYCLES; cycle_ndx++) {
      
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.get(
                unit,
                stage,
                cur_prog,
                cycle_ndx,
                &keys_bmp
              );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      
      is_key_found_for_320b[cycle_ndx] = FALSE;
      for (key_ndx = 0; (key_ndx < (ARAD_PMF_LOW_LEVEL_NOF_KEYS / 2 )) && (!is_key_found_for_320b[cycle_ndx]); key_ndx++) {
          fld_val = 0;
          
          SHR_BITCOPY_RANGE(&fld_val, 0, &keys_bmp, (ARAD_PP_FP_KEY_NUMBER_80_PARTS_IN_320B * key_ndx), ARAD_PP_FP_KEY_NUMBER_80_PARTS_IN_320B);
          if(fld_val == 0 ) {
              is_key_found_for_320b[cycle_ndx] = TRUE;
          }
      }
  }

  *no_320b_cycle_found = FALSE; 
  *selected_cycle = 0;
  
  if(is_key_found_for_320b[0] && (!is_key_found_for_320b[1])) {
      *no_320b_cycle_found = TRUE; 
      *selected_cycle = 1;
  }
  else if(is_key_found_for_320b[1] && (!is_key_found_for_320b[0])) {
      *no_320b_cycle_found = TRUE; 
      *selected_cycle = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_cycle_no_320b_key_get()", cur_prog, 0);
}


STATIC
 uint32
  arad_pp_fp_key_progs_rsrc_stat(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  uint32                       db_id, 
    SOC_SAND_IN  uint32                       progs_bmp,
    SOC_SAND_IN  uint32                       cycles,
    SOC_SAND_IN  uint8                        is_for_direct_extraction,
    SOC_SAND_OUT  uint8                       cycle_needed[SOC_PPC_FP_NOF_CYCLES],
    SOC_SAND_OUT  uint32                      ce_rsrc[SOC_PPC_FP_NOF_CYCLES],
    SOC_SAND_OUT  uint32                      selected_cycle[ARAD_PMF_LOW_LEVEL_NOF_PROGS_ALL_STAGES],
    SOC_SAND_OUT  uint8                       key_zone_enough[ARAD_PP_FP_NOF_KEY_CE_PLACES][SOC_PPC_FP_NOF_CYCLES],
    SOC_SAND_OUT  uint32                      *flp_max_key_size_in_bits,
    SOC_SAND_OUT  uint32                      *dir_ext_start_place
  )
{
  uint32   
      key_id[2],
      place_ndx,
      cur_prog,
      prog_result,
      cur_rsrc1,
      cur_rsrc2,
      cur_rsrc1_normalized,
      cur_rsrc2_normalized=0,
      max_rsrc1_used_ces=0,
      max_rsrc2_used_ces=0,
      sel_rsrc,
      cycle_ndx,
      ce_indx,
      bmp_index,
      nof_cycles=0,
      cyc_1 = 0,
      selected_cycle_320b,
      progs_bmp_lcl[1],
      keys_bmp,
      res = SOC_SAND_OK,
      key_masks[2] = {0x55,0xaa};
  uint8
      is_high_and_low_diff_fgs_allowed,
      bit_to_set = FALSE,
      no_320b_cycle_found,
      key_index = 0,
      success = FALSE;
  int
      cur_rsrc1_count,
      cur_rsrc2_count;

      
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  if (((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)&&(SOC_IS_JERICHO(unit)))) {
      flp_max_key_size_in_bits[0] = ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS; 
      flp_max_key_size_in_bits[1] = ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS; 
  }else{
      *flp_max_key_size_in_bits = ARAD_PP_FLP_KEY_C_ZONE_SIZE_ARAD_PLUS_BITS; 
  }
  
  sal_memset(key_zone_enough, 0x0, sizeof(uint8) * ARAD_PP_FP_NOF_KEY_CE_PLACES * SOC_PPC_FP_NOF_CYCLES);
  
  for(cycle_ndx = 0; cycle_ndx < SOC_PPC_FP_NOF_CYCLES; cycle_ndx++) {
      for(place_ndx = 0; place_ndx < ARAD_PP_FP_NOF_KEY_CE_PLACES_STAGE; place_ndx++) {
          
          key_zone_enough[place_ndx][cycle_ndx] = TRUE;
      }
  }

  ce_rsrc[0] = 0;
  ce_rsrc[1] = 0;
  cycle_needed[0] = FALSE;
  cycle_needed[1] = FALSE;

  if(cycles == ARAD_PP_FP_KEY_CYCLE_ANY) {
      nof_cycles = 2;
      cyc_1 = 0;
  }
  else if(cycles == ARAD_PP_FP_KEY_CYCLE_EVEN) {
      nof_cycles = 1;
      cyc_1 = 0;
  }
  else if(cycles == ARAD_PP_FP_KEY_CYCLE_ODD) {
      nof_cycles = 1;
      cyc_1 = 1;
  }

  cur_prog = 0;
  *progs_bmp_lcl = progs_bmp;
  ARAD_PP_FP_KEY_FIRST_SET_BIT(progs_bmp_lcl,cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS-cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,FALSE,prog_result);
  if(prog_result == 0) {
     LOG_ERROR(BSL_LS_SOC_FP,
               (BSL_META_U(unit,
                           "Unit %d Stage %s No programss available.\n\r"),
                unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage)));
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 5, exit); 
  }
  while(prog_result != 0) {
      selected_cycle[cur_prog] = (cycles == ARAD_PP_FP_KEY_CYCLE_ODD) ? 1 : 0;

      
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(
              unit,
              stage,
              cur_prog,
              cyc_1,
              &cur_rsrc1
          );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    cur_rsrc1_normalized = arad_pp_fp_key_ce_rsrc_normalize(unit, stage, flags, cur_rsrc1); 
    sel_rsrc = cur_rsrc1_normalized;

    
    if(nof_cycles == 2) {
        
        res = arad_pp_fp_key_cycle_no_320b_key_get(
                unit,
                stage,
                cur_prog,
                &no_320b_cycle_found, 
                &selected_cycle_320b
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(
                unit,
                stage,
                cur_prog,
                1 - cyc_1,
                &cur_rsrc2
            );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        cur_rsrc2_normalized = arad_pp_fp_key_ce_rsrc_normalize(unit, stage, flags, cur_rsrc2); 

        
        shr_bitop_range_count(&cur_rsrc1_normalized,0,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG,&cur_rsrc1_count);
        shr_bitop_range_count(&cur_rsrc2_normalized,0,ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG,&cur_rsrc2_count);
        if (max_rsrc1_used_ces < cur_rsrc1_count) {
            max_rsrc1_used_ces = cur_rsrc1_count;
        }
        if (max_rsrc2_used_ces < cur_rsrc2_count) {
            max_rsrc2_used_ces = cur_rsrc2_count;
        }

       
        if((!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pmf_320b_key_opt_disable", 0))
            && (no_320b_cycle_found)) {
            selected_cycle[cur_prog] = selected_cycle_320b;
            sel_rsrc = (selected_cycle[cur_prog] == 0)? cur_rsrc1_normalized: cur_rsrc2_normalized;
        }
        else {
            sel_rsrc = arad_pp_fp_key_ce_rsrc_select(unit, stage, cur_rsrc1_normalized, cur_rsrc2_normalized);
            selected_cycle[cur_prog] = (sel_rsrc == cur_rsrc1_normalized)?0:1;

            
            if (SOC_IS_ARADPLUS_AND_BELOW(unit))
            {
                if (flags & ARAD_PP_FP_KEY_ALLOC_USE_KEY_A)
                {
                    selected_cycle[cur_prog] = 0;       
                    sel_rsrc = cur_rsrc1_normalized;
                }
            }

        }
    }

    cycle_needed[selected_cycle[cur_prog]] = TRUE;
    ce_rsrc[selected_cycle[cur_prog]] |= sel_rsrc;
    if(nof_cycles == 2) {
        ce_rsrc[1 - selected_cycle[cur_prog]] |= (sel_rsrc == cur_rsrc1_normalized)? cur_rsrc2_normalized: cur_rsrc1_normalized;
        cycle_needed[selected_cycle[cur_prog]] = TRUE;
        cycle_needed[1 - selected_cycle[cur_prog]] = FALSE;
    }
    
    
    is_high_and_low_diff_fgs_allowed = is_for_direct_extraction || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB);
    for(cycle_ndx = 0; cycle_ndx < SOC_PPC_FP_NOF_CYCLES; cycle_ndx++) {
        for(place_ndx = 0; place_ndx < ARAD_PP_FP_NOF_KEY_CE_PLACES_STAGE; place_ndx++) {
            
            if (!key_zone_enough[place_ndx][cycle_ndx])
            {
                continue;
            }
            
            key_zone_enough[place_ndx][cycle_ndx] = FALSE;

            
            if ((((!is_high_and_low_diff_fgs_allowed) && Arad_pp_fp_place_const_convert[place_ndx][4]) 
                 || (is_high_and_low_diff_fgs_allowed && Arad_pp_fp_place_const_convert[place_ndx][5])))
            {
                if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)
                {
#if defined(INCLUDE_KBP)
                    uint32
                        max_key_size_in_bits;

                    
					if (SOC_IS_JERICHO(unit)) {
                        uint32 i;
						for (i=0;i<ARAD_PP_FLP_KEY_C_NUM_OF_ZONES_JERICHO;i++) {
                            res = arad_pp_flp_elk_prog_config_max_key_size_get(unit, cur_prog, i, &max_key_size_in_bits);
                            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 192, exit);
                            flp_max_key_size_in_bits[i] = SOC_SAND_MIN(flp_max_key_size_in_bits[i], max_key_size_in_bits);
						}
					}else{
                        res = arad_pp_flp_elk_prog_config_max_key_size_get(unit, cur_prog, 0, &max_key_size_in_bits);
                        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 192, exit);
                        *flp_max_key_size_in_bits = SOC_SAND_MIN(*flp_max_key_size_in_bits, max_key_size_in_bits);
                    }
                    success = TRUE;
#else
                    success = FALSE;
#endif
                }
                else
                {
                    
                    
                    res = arad_pp_fp_key_alloc_key_id(
                             unit,
                             stage,
                             cur_prog,
                             db_id,
                             cycle_ndx,
                             Arad_pp_fp_place_const_convert[place_ndx][0], 
                             ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY | (flags & ARAD_PP_FP_KEY_ALLOC_USE_KEY_A ), 
                             &key_id[0],
                             &success
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
                }

                if (success) {
                    key_zone_enough[place_ndx][cycle_ndx] = TRUE;
                    continue; 
                }
            }
        }

        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.get(
                unit,
                stage,
                cur_prog,
                cycle_ndx,
                &keys_bmp 
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        
        if (is_for_direct_extraction && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "optimized_de_allocation", 0)) 
        {
            for (key_index = 0  ; key_index < ARAD_PMF_LOW_LEVEL_NOF_KEYS ; key_index ++) 
            {
                if ( ((keys_bmp >> (key_index * 2)) == 0x1) )  
                {
                    *dir_ext_start_place = ARAD_PP_FP_KEY_CE_HIGH;
                    break;
                }
                else if ( ((keys_bmp >> (key_index * 2)) == 0x2) ) 
                {
                    *dir_ext_start_place = ARAD_PP_FP_KEY_CE_LOW;
                    break;
                }
            }
        }

        for (bmp_index = 0 ; bmp_index < 2 ; bmp_index ++) { 

            
            if ( (keys_bmp & key_masks[bmp_index]) == key_masks[bmp_index] ) 
            {
                  for(ce_indx = 0; ce_indx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG; ++ce_indx) {
                     bit_to_set = (bmp_index == 0 ) ? (ce_indx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB) : ( ce_indx >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB );
                     if (bit_to_set)
                     {
                         SHR_BITSET(&ce_rsrc[cycle_ndx], ce_indx);
                     }
                  }
             }
        }
    }

    ARAD_PP_FP_KEY_FIRST_SET_BIT(progs_bmp_lcl,cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS-cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,TRUE,prog_result);
  }
  if (nof_cycles == 2) {
      
      if (max_rsrc1_used_ces <= max_rsrc2_used_ces) {
          cycle_needed[0] = TRUE;
          cycle_needed[1] = FALSE;
      } else {
          cycle_needed[0] = FALSE;
          cycle_needed[1] = TRUE;
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_progs_rsrc_stat()", 0, 0);
}

STATIC
uint32
  arad_pp_fp_qual_ce_mltpl_info_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
	  SOC_SAND_IN  uint32                       flags,
      SOC_SAND_INOUT SOC_PPC_FP_QUAL_TYPE       qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
      SOC_SAND_OUT uint32                       *nof_quals,
      SOC_SAND_OUT ARAD_PP_FP_QUAL_CE_INFO      qual_ce_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]
  )
{
  uint8 
      success;
  uint32 
      res, 
      qual_indx;

  ARAD_PP_FP_ALLOC_ALG_ARGS
      algorithm_args;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *nof_quals = 0;
  algorithm_args.balance_enabled = 0;
  algorithm_args.balance_lsb_msb = 0;

  for(qual_indx = 0; (qual_indx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) && (qual_types[qual_indx]!= SOC_PPC_NOF_FP_QUAL_TYPES) && (qual_types[qual_indx]!= BCM_FIELD_ENTRY_INVALID); ++qual_indx)
  {
    
    res = arad_pp_fp_qual_ce_info_uneven_get(
            unit,
            stage,
            FALSE, 
            FALSE,
            0,
            qual_types[qual_indx],
            NULL,
            &algorithm_args,
            NULL, 
            NULL, 
            NULL, 
            &(qual_ce_info[(*nof_quals)]),
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10 + qual_indx, exit);

    (*nof_quals)++;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_ce_mltpl_info_get()", 0, 0);
}



STATIC
uint32 
    arad_pp_fp_qual_ce_info_sort(
    SOC_SAND_IN  int                       unit_num,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE  stage,
    SOC_SAND_IN    uint32                   flags,
    SOC_SAND_IN    uint32                   nof_quals,
    SOC_SAND_INOUT SOC_PPC_FP_QUAL_TYPE     qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
    SOC_SAND_INOUT ARAD_PP_FP_QUAL_CE_INFO  qual_ce_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]
    )
{
  uint32 
      qual_idx,
      swap_idx,
      next_qual,
      qual_nof_bits,
      qual_nof_bits_next,
      swap_priority,
      no_swap_priority;
  soc_error_t soc_res;
  uint8 
    same_category,
    swap_needed = FALSE,
    swaped = FALSE,
    udf_is_base = FALSE;

  ARAD_PP_FP_QUAL_CE_INFO ce_info_tmp;
  SOC_PPC_FP_QUAL_TYPE qual_tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  
  if (nof_quals < 2) {
      SOC_SAND_EXIT_NO_ERROR
  }

  for(swap_idx = 0; swap_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; swap_idx++) {
    swaped = FALSE;  
    for (qual_idx = 0; qual_idx < nof_quals-1; qual_idx++) {
        swap_priority = 0;
        no_swap_priority = 0;
        if ((!qual_ce_info[qual_idx].is_lsb) && (!qual_ce_info[qual_idx].is_msb)) {
           LOG_ERROR(BSL_LS_SOC_FP,
                     (BSL_META_U(unit,
                                 "Qualifier %s : Qualifier is not allowed in neither lsb nor msb.\n\r"),
                      SOC_PPC_FP_QUAL_TYPE_to_string(qual_types[qual_idx])));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 10, exit);
        }

        next_qual = qual_idx + 1;

        qual_nof_bits = qual_ce_info[qual_idx].lost_bits + qual_ce_info[qual_idx].qual_size;
        qual_nof_bits_next = qual_ce_info[next_qual].lost_bits + qual_ce_info[next_qual].qual_size;
        same_category = (qual_ce_info[qual_idx].is_lsb == qual_ce_info[next_qual].is_lsb) && 
            (qual_ce_info[qual_idx].is_msb == qual_ce_info[next_qual].is_msb) ? TRUE : FALSE;
        swap_needed = FALSE;

        if (SOC_IS_JERICHO_PLUS(unit_num)) {
            soc_res = arad_pp_fp_is_qual_identical(unit_num, qual_types[next_qual], SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, &udf_is_base);
            SOC_SAND_CHECK_FUNC_RESULT(soc_res, 10, exit);
        }

        
        if (SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED == qual_types[next_qual]) {
            swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_FIRST;
        } else if (SOC_PPC_FP_QUAL_KEY_AFTER_HASHING == qual_types[next_qual]) {
            swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_BTW_FIRST_SECOND;
        } else if (udf_is_base) {
            swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_BTW_FIRST_SECOND;
        }
        
        else if ((flags & ARAD_PP_FP_KEY_ALLOC_CE_FLP_NO_SPLIT) 
                 && SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(qual_types[next_qual])
                 && (!SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(qual_types[qual_idx]))) {
            swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_SECOND;
        }
        else if (!qual_ce_info[qual_idx].is_lsb && !qual_ce_info[next_qual].is_msb) {
            swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_THIRD;
        }
        else if (qual_ce_info[qual_idx].is_lsb && qual_ce_info[qual_idx].is_msb && (!qual_ce_info[next_qual].is_lsb || !qual_ce_info[next_qual].is_msb)) {
            swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_FOURTH;
        }
        else if ((qual_nof_bits < qual_nof_bits_next) && same_category) {
            swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_FIFTH;
        }

        udf_is_base = FALSE;
        
        if (SOC_IS_JERICHO_PLUS(unit_num)) {
            soc_res = arad_pp_fp_is_qual_identical(unit_num, qual_types[qual_idx], SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, &udf_is_base);
            SOC_SAND_CHECK_FUNC_RESULT(soc_res, 20, exit);
        }

        
        if (SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED == qual_types[qual_idx]) {
            no_swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_FIRST;
        } 
        
        else if (SOC_PPC_FP_QUAL_KEY_AFTER_HASHING == qual_types[next_qual]) {
            no_swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_BTW_FIRST_SECOND;
        }
        else if (udf_is_base) {
            no_swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_BTW_FIRST_SECOND;
        }
        else if ((flags & ARAD_PP_FP_KEY_ALLOC_CE_FLP_NO_SPLIT) 
                 && SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(qual_types[qual_idx])) {
            no_swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_SECOND;
        }
        else if (!qual_ce_info[qual_idx].is_msb) {
            no_swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_THIRD;
        }      
        else if ((!qual_ce_info[qual_idx].is_lsb) || (!qual_ce_info[qual_idx].is_msb)) {
            no_swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_FOURTH;
        }
        else if ((qual_nof_bits >= qual_nof_bits_next) && same_category) {
            no_swap_priority = ARAD_PP_FP_KEY_SWAP_PRIORITY_FIFTH;
        }

        
        if (swap_priority > no_swap_priority) {
            swap_needed = TRUE;
        }

        if (swap_needed) {
            
            sal_memcpy(&ce_info_tmp, &(qual_ce_info[qual_idx]), sizeof(ARAD_PP_FP_QUAL_CE_INFO));
            qual_tmp = qual_types[qual_idx];

            sal_memcpy(&(qual_ce_info[qual_idx]), &(qual_ce_info[next_qual]), sizeof(ARAD_PP_FP_QUAL_CE_INFO));
            qual_types[qual_idx] = qual_types[next_qual];

            sal_memcpy(&(qual_ce_info[next_qual]), &ce_info_tmp, sizeof(ARAD_PP_FP_QUAL_CE_INFO));
            qual_types[next_qual] = qual_tmp;

             
            swaped = TRUE;
        }
      } 
      if (swaped == FALSE) {
          break;
      }
    } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_ce_info_sort()", 0, 0);
}

STATIC
uint32 
    arad_pp_fp_qual_sum(
    SOC_SAND_IN    ARAD_PP_FP_QUAL_CE_INFO  qual_ce_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
    SOC_SAND_IN    uint32                   nof_quals
    )
{
    uint32 qual_idx, qual_sum = 0;
  
    for(qual_idx = 0; qual_idx < nof_quals; qual_idx++) 
    {
        qual_sum += qual_ce_info[qual_idx].lost_bits + qual_ce_info[qual_idx].qual_size;
    }

    return qual_sum;
}



uint32
  arad_pp_fp_key_nof_free_instr_get(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
      SOC_SAND_IN  uint32                       ce_rsrc_bmp_glbl,
      SOC_SAND_IN  uint8                        is_32b,
      SOC_SAND_IN  uint8                        is_msb,
      SOC_SAND_IN  uint8                        is_high_group
    )
{
  uint32 
      nof_free_instr,
      ce_indx,
      ce_rsrc_bmp_glbl_lcl[1];
  uint8
      high_group,
      ce_32b,
      ce_msb;

  *ce_rsrc_bmp_glbl_lcl = ce_rsrc_bmp_glbl;
  nof_free_instr = 0;
  for(ce_indx = 0; ce_indx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG; ++ce_indx) {
      ce_32b = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_indx);
      ce_msb = (ce_indx >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB);
      high_group = arad_pmf_low_level_ce_is_second_group(unit, stage, (ce_indx % ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB));


      
      if ((is_32b == ce_32b) && (is_msb == ce_msb) && (high_group == is_high_group)) {
          
          if (!(SHR_BITGET(ce_rsrc_bmp_glbl_lcl, ce_indx))) {
              nof_free_instr ++;
          }
      }
  }

  return nof_free_instr;
}


uint32
  arad_pp_fp_key_alloc_constrain_calc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       cycle_cons, 
    SOC_SAND_IN  uint32                       place_cons,
    SOC_SAND_IN  uint8                        use_kaps,
    SOC_SAND_IN  uint8                        is_for_direct_extraction,
    SOC_SAND_OUT ARAD_PP_FP_CE_CONSTRAINT     ce_const[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS],
    SOC_SAND_OUT  uint32                      *nof_consts,
    SOC_SAND_INOUT  uint32                      *place_start,
    SOC_SAND_OUT  uint32                      selected_cycle[ARAD_PMF_LOW_LEVEL_NOF_PROGS_ALL_STAGES],
    SOC_SAND_OUT  uint32                      total_bits_in_zone[ARAD_PP_FP_KEY_NOF_ZONES],
    SOC_SAND_OUT  uint8                       *found
  )
{
  uint32   
    res = SOC_SAND_OK,
    place_ndx,
    zone_idx,
    zone_id = 0,
      zone_id_first =0 ,
      zone_id_second = 0,
    direct_ext_loop_indx, 
    qual_indx,
    nof_quals,
    qual_sum,
    exist_progs,
    exist_progs_array[1],
    nof_cycles,
    cycle_idx,
    cycle_id,
    cycle_base,
    new_cycle_cons,
    ce_indx,
    total_ce_indx = 0,
    prog_ndx,
      flp_max_key_size_in_bits[ARAD_PP_FP_KEY_NOF_ZONES]={0},
    place_start_lcl, tmp_start_place,
    dir_ext_start_place = 0,
    total_qual_size = 0,
    ce_rsrc_bmp_glbl_lcl[SOC_PPC_FP_NOF_CYCLES], 
    nof_free_ces_in_zone[ARAD_PP_FP_KEY_NOF_CATEGORIES]; 
  uint8
      success,
      is_32b,
      is_msb,
      is_high_group,
      is_key_not_found,
      is_slb_hash_in_quals,
      key_zone_enough[ARAD_PP_FP_NOF_KEY_CE_PLACES][SOC_PPC_FP_NOF_CYCLES],
      direct_ext_2_needed, 
      cycle_needed[SOC_PPC_FP_NOF_CYCLES], 
      lookup_id,
      is_cascaded,
      zone_used[ARAD_PP_FP_KEY_NOF_ZONES];
  SOC_PPC_FP_QUAL_TYPE          
      new_qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
  ARAD_PP_FP_QUAL_CE_INFO       
      *ce_info = NULL;
  soc_error_t 
    soc_res;
  ARAD_PP_FP_ALLOC_ALG_ARGS
      algorithm_args;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  sal_memcpy(new_qual_types, qual_types, sizeof(SOC_PPC_FP_QUAL_TYPE) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
  ARAD_ALLOC(ce_info, ARAD_PP_FP_QUAL_CE_INFO, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX, "arad_pp_fp_key_alloc_constrain_calc.ce_info");
  sal_memset(ce_info, 0x0, sizeof(ARAD_PP_FP_QUAL_CE_INFO) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
          unit,
          stage,
          db_id,
          0,
          &exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  if (  (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)) {
      uint32 dip_sip_sharing_enabled = 0, prog_result = 0;
	  int i,j;
  	  int actual_nof_quals = 0;
  	  int nof_shared_quals = 0;
  	  int is_exists = 0;
      int cluster_id = -1;

      arad_pp_fp_dip_sip_sharing_is_sharing_enabled_for_db(unit, stage, db_id, &dip_sip_sharing_enabled, &cluster_id);
      
	  exist_progs_array[0] = exist_progs;      
      ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs_array, prog_result, ARAD_PMF_LOW_LEVEL_NOF_PROGS, ARAD_PMF_LOW_LEVEL_NOF_PROGS, FALSE, res);

      if (dip_sip_sharing_enabled) {
          for (i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
              if((qual_types[i] == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_types[i] == BCM_FIELD_ENTRY_INVALID)){
                  break;
              }
              actual_nof_quals++;

              dip_sip_sharing_qual_info_get(unit, prog_result, new_qual_types[i], &is_exists, NULL);
              if (is_exists) {              
                  nof_shared_quals++;
                  for (j = i; j < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX - nof_shared_quals; j++) {
                      if (j < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                          new_qual_types[j] = qual_types[j+nof_shared_quals];
                      }else{
                          assert(0);
                      }
                  }
                  i--;
              }
          }
      }
  }

  
  *found = TRUE;
  nof_cycles = (cycle_cons == ARAD_PP_FP_KEY_CYCLE_ANY)? 2 :1;
  cycle_base = (cycle_cons == ARAD_PP_FP_KEY_CYCLE_ODD)? 1 :0;
  new_cycle_cons = cycle_cons;
  tmp_start_place = *place_start;
  dir_ext_start_place = *place_start;

  res = arad_pp_fp_key_progs_rsrc_stat(
          unit, 
          stage, 
          flags,
          db_id, 
          exist_progs, 
          new_cycle_cons, 
          is_for_direct_extraction, 
          cycle_needed,
          ce_rsrc_bmp_glbl_lcl, 
          selected_cycle, 
          key_zone_enough,
          &flp_max_key_size_in_bits[0],
          &dir_ext_start_place
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_fp_db_cascaded_cycle_get(
      unit,
     db_id,
      &is_cascaded,
      &lookup_id 
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  for (cycle_idx = cycle_base; cycle_idx < (cycle_base + nof_cycles); cycle_idx++) {

      
      tmp_start_place = (is_for_direct_extraction && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "optimized_de_allocation", 0)) ? dir_ext_start_place  : *place_start;

      cycle_id = cycle_idx;
      if (nof_cycles == 2)
      {
          if (cycle_idx == 0)
          {
              cycle_id = cycle_needed[0]? 0: 1;
          }
          else {
              cycle_id = cycle_needed[0]? 1: 0;
          }
      }

   
        

         
        res = arad_pp_fp_qual_ce_mltpl_info_get(
                    unit,
                    stage,
                    flags,
                    new_qual_types,
                    &nof_quals,
                    ce_info
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        
        if (!(flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER)) {
            
            res = arad_pp_fp_qual_ce_info_sort(
                    unit,
                    stage,
                    flags,
                    nof_quals, 
                    new_qual_types, 
                    ce_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        }
        
         

        
        algorithm_args.balance_enabled = FALSE;
        algorithm_args.balance_lsb_msb = 0;

        if (SOC_IS_JERICHO(unit) &&
            soc_property_get(unit, spn_FIELD_KEY_ALLOCATION_MSB_BALANCE_ENABLE, FALSE) &&
            ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) ||
            (SOC_DPP_IS_ELK_ENABLE(unit) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)) || 
            (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS)))
        {
            
            qual_sum = arad_pp_fp_qual_sum(ce_info, nof_quals);
            
            if (qual_sum > ARAD_PMF_LOW_LEVEL_ZONE_SIZE_PER_STAGE) 
            {
                
                algorithm_args.balance_enabled = TRUE;

                 
                algorithm_args.balance_lsb_msb = 0;
            }
        }
        
        direct_ext_2_needed = TRUE; 
        total_ce_indx = 0;
        for (direct_ext_loop_indx = 0; (direct_ext_loop_indx < (1 + is_for_direct_extraction)) && direct_ext_2_needed; direct_ext_loop_indx++) {
            *found = TRUE;
            direct_ext_2_needed = FALSE; 

            soc_res = arad_pp_fp_is_qual_in_qual_arr(unit, 
                                                     new_qual_types, 
                                                     nof_quals, 
                                                     SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, 
                                                     &is_slb_hash_in_quals);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(soc_res, 151, exit);

            
            for(zone_idx = 0; zone_idx < ARAD_PP_FP_KEY_NOF_ZONES; zone_idx++)
            {
              total_bits_in_zone[zone_idx] = 0;
              
              if ( is_for_direct_extraction ) 
              {
                  
                  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "optimized_de_allocation", 0)
                      || is_cascaded || is_slb_hash_in_quals)
                  {
                      zone_id_first  = ( tmp_start_place == ARAD_PP_FP_KEY_CE_LOW ) ?  ARAD_PP_FP_KEY_ZONE_LSB_0 : ARAD_PP_FP_KEY_ZONE_MSB_0;
                      zone_id_second = ( tmp_start_place == ARAD_PP_FP_KEY_CE_LOW ) ?  ARAD_PP_FP_KEY_ZONE_MSB_0 : ARAD_PP_FP_KEY_ZONE_LSB_0;
                       if (((direct_ext_loop_indx == 0) && (zone_idx != zone_id_first))
                          || ((direct_ext_loop_indx == 1) && (zone_idx != zone_id_second))) {
                          total_bits_in_zone[zone_idx] = ARAD_PMF_LOW_LEVEL_ZONE_SIZE_PER_STAGE;
                      }

                  }
                  else 
                  {
                      if (((direct_ext_loop_indx == 0) && (zone_idx != ARAD_PP_FP_KEY_ZONE_MSB_0))
                          || ((direct_ext_loop_indx == 1) && (zone_idx != ARAD_PP_FP_KEY_ZONE_LSB_0))) {
                          total_bits_in_zone[zone_idx] = ARAD_PMF_LOW_LEVEL_ZONE_SIZE_PER_STAGE;
                      }
                  }
              }
              if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB) {
                  if (((place_cons == ARAD_PP_FP_KEY_CE_HIGH) && (zone_idx != ARAD_PP_FP_KEY_ZONE_MSB_0))
                      || ((place_cons == ARAD_PP_FP_KEY_CE_LOW) && (zone_idx != ARAD_PP_FP_KEY_ZONE_LSB_0))) {
                      total_bits_in_zone[zone_idx] = ARAD_PMF_LOW_LEVEL_ZONE_SIZE_PER_STAGE;
                  }
              }
            }
            
            for(is_32b = FALSE; is_32b <= TRUE; is_32b++)
            {
                for(is_msb = FALSE; is_msb <= TRUE; is_msb++)
                {
                    for(is_high_group = FALSE; is_high_group <= TRUE; is_high_group++)
                    {
                      nof_free_ces_in_zone[ARAD_PP_FP_KEY_CATEGORY_GET(is_32b, is_msb, is_high_group)] = 
                          arad_pp_fp_key_nof_free_instr_get(
                              unit,
                              stage, 
                              ce_rsrc_bmp_glbl_lcl[cycle_id],
                              is_32b,
                              is_msb,
                              is_high_group);
                    }
                }
            }

            if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)&&SOC_IS_JERICHO(unit)) {
                total_bits_in_zone[0] = ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS - flp_max_key_size_in_bits[0];
                total_bits_in_zone[1] = ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS - flp_max_key_size_in_bits[1];
            }

            for(qual_indx = 0;  (qual_indx < nof_quals) && (*found); ++qual_indx) 
            {
              
              sal_memset(&(ce_info[qual_indx]), 0x0, sizeof(ARAD_PP_FP_QUAL_CE_INFO));
              sal_memset(zone_used, 0x0, sizeof(uint8) * ARAD_PP_FP_KEY_NOF_ZONES);

              soc_res = arad_pp_fp_is_qual_identical(unit,
                                                       new_qual_types[qual_indx],
                                                       SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, 
                                                       &is_slb_hash_in_quals);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(soc_res, 151, exit);

              
               if (is_slb_hash_in_quals && stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
                  if (!SOC_IS_JERICHO_PLUS(unit)) {
                      total_bits_in_zone[ARAD_PP_FP_KEY_ZONE_MSB_0] = 0;
                      continue;
                  } else {
                      if (flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER) {
                          continue;
                      }
                  }
               }

              res = arad_pp_fp_qual_ce_info_uneven_get(
                      unit,
                      stage,
                      TRUE, 
                      use_kaps,
                      flags,
                      new_qual_types[qual_indx],
                      &total_qual_size,
                      &algorithm_args, 
                      total_bits_in_zone,
                      nof_free_ces_in_zone,
                      zone_used,
                      &(ce_info[qual_indx]),
                      &success
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);


              
              if(!success)
              {
                LOG_DEBUG(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "    "
                                      "Key: fail to allocate Database %d because not enough space to split qualifiers\n\r"), db_id));

                if ((direct_ext_loop_indx == 1) && (flags & ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER)) {
                    LOG_ERROR(BSL_LS_SOC_FP,
                              (BSL_META_U(unit,
                                          "Key: not enough space to split qualifiers or failed to allocate CE16 for large DE groups\n\r")));
                }

                *found = FALSE;
                total_ce_indx = 0;
                direct_ext_2_needed = TRUE;
                continue; 
              }

              
              for(zone_idx = 0; zone_idx < ARAD_PP_FP_KEY_NOF_ZONES; zone_idx++)
              {
                  int max_bits_for_zone;
                  
                  if(!zone_used[zone_idx])
                      continue;

                  zone_id = zone_idx;
                  max_bits_for_zone = ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)&&(SOC_IS_JERICHO(unit)))? ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS : flp_max_key_size_in_bits[0];
                
                if ((place_cons == ARAD_PP_FP_KEY_CE_LOW) && 
                    (zone_id > ARAD_PP_FP_KEY_ZONE_MSB_0 || 
                     zone_id == ARAD_PP_FP_KEY_ZONE_MSB_0 || 
                     zone_id == ARAD_PP_FP_KEY_ZONE_MSB_1)) 
                {
                        LOG_DEBUG(BSL_LS_SOC_FP,
                                  (BSL_META_U(unit,
                                              "    "
                                              "Key: fail to allocate Database %d because zone tested %d is not acceptable with place_cons %d \n\r"), 
                                   db_id, zone_id, place_cons));
                        *found = FALSE;
                        total_ce_indx = 0;
                        direct_ext_2_needed = TRUE;
                        continue; 
                }
                
                else if ((place_cons == ARAD_PP_FP_KEY_CE_PLACE_ANY_NOT_DOUBLE) && (zone_id > ARAD_PP_FP_KEY_ZONE_MSB_0)) 
                {
                        LOG_DEBUG(BSL_LS_SOC_FP,
                                  (BSL_META_U(unit,
                                              "    "
                                              "Key: fail to allocate Database %d because zone tested %d is not acceptable with place_cons %d \n\r"), 
                                   db_id, zone_id, place_cons));
                        *found = FALSE;
                        total_ce_indx = 0;
                        direct_ext_2_needed = TRUE;
                        continue; 
                }
                else if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) && ( max_bits_for_zone < total_bits_in_zone[zone_id])) {
                    
                    LOG_DEBUG(BSL_LS_SOC_FP,
                              (BSL_META_U(unit,
                                          "    "
                                          "Key: fail to allocate Database %d because FLP Key-C size remaining is %d bits less than ACL current key size %d \n\r"), 
                               db_id, flp_max_key_size_in_bits[zone_id], total_bits_in_zone[zone_id]));
                    *found = FALSE;
                    total_ce_indx = 0;
                    direct_ext_2_needed = FALSE;
                    ARAD_DO_NOTHING_AND_EXIT; 
                }
              }
              
              
              for(ce_indx = 0; ce_indx < ce_info[qual_indx].nof_ce; ++ce_indx) 
              {
                if(total_ce_indx >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG) 
                {
                 LOG_ERROR(BSL_LS_SOC_FP,
                           (BSL_META_U(unit,
                                       "Unit: %d too much qualifiers\n\r"
                                       "Total qualifiers %d, Maximum qualifiers allowed %d.\n\r"),
                            unit, total_ce_indx, ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG));
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 60, exit);
                }
                arad_pp_fp_key_qual_to_ce_const(stage, &(ce_info[qual_indx]),ce_indx,&(ce_const[total_ce_indx]));
                ce_const[total_ce_indx].qual_type = new_qual_types[qual_indx];

                zone_id = ce_info[qual_indx].ce_zone_id[ce_indx];
                
                
                if((zone_id == ARAD_PP_FP_KEY_ZONE_MSB_0) || (zone_id == ARAD_PP_FP_KEY_ZONE_MSB_1)) {
                  ce_const[total_ce_indx].place_cons = ARAD_PP_FP_KEY_CE_HIGH;
                }
                else {
                  ce_const[total_ce_indx].place_cons = ARAD_PP_FP_KEY_CE_LOW;
                }

                if (zone_id > ARAD_PP_FP_KEY_ZONE_MSB_0) {
                  ce_const[total_ce_indx].is_second_key = TRUE;
                }

                
                if (SOC_IS_JERICHO_PLUS(unit) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)) {                    
                    soc_res = arad_pp_fp_is_qual_identical(
                        unit,
                        new_qual_types[qual_indx],
                        SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, 
                        &is_slb_hash_in_quals);
                    SOC_SAND_CHECK_FUNC_RESULT(soc_res, 10, exit);
                    
                    if (is_slb_hash_in_quals) {
                        ce_const[total_ce_indx].place_cons = ARAD_PP_FP_KEY_CE_LOW;
                        ce_const[total_ce_indx].group_cons = ARAD_PP_FP_KEY_CE_HIGH;
                        ce_const[total_ce_indx].size_cons = ARAD_PP_FP_KEY_CE_SIZE_32;
                    }
                }

                total_ce_indx++;
              }
            } 
        } 

        if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)&&(SOC_IS_JERICHO(unit))) {
            total_bits_in_zone[0] -= (ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS - flp_max_key_size_in_bits[0]);
            total_bits_in_zone[1] -= (ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS - flp_max_key_size_in_bits[1]);
        }

        if(is_for_direct_extraction || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)) {
            place_start_lcl = (zone_id ==0) ? ARAD_PP_FP_KEY_CE_LOW : ARAD_PP_FP_KEY_CE_HIGH;
        }
        else {
            
            if (total_bits_in_zone[ARAD_PP_FP_KEY_ZONE_MSB_1] > 0 || total_bits_in_zone[ARAD_PP_FP_KEY_ZONE_LSB_1] > 0) 
            {
              
              place_start_lcl = ARAD_PP_FP_KEY_CE_PLACE_ANY; 
            }
            
            else if(total_bits_in_zone[ARAD_PP_FP_KEY_ZONE_MSB_0] > 0) 
            {
              
              place_start_lcl = ARAD_PP_FP_KEY_CE_PLACE_ANY_NOT_DOUBLE;
            }
            else 
            {
              place_start_lcl = ARAD_PP_FP_KEY_CE_LOW;
            }
        }

        
        is_key_not_found = FALSE;

        

        tmp_start_place = is_for_direct_extraction 
            ? place_start_lcl : SOC_SAND_MAX(place_start_lcl, tmp_start_place);
        
        for(place_ndx = 0; place_ndx < ARAD_PP_FP_NOF_KEY_CE_PLACES_STAGE; place_ndx++) {
            
            if (Arad_pp_fp_place_const_convert[place_ndx][0] == tmp_start_place) {
                if (!key_zone_enough[place_ndx][cycle_id]) {
                    
                    LOG_DEBUG(BSL_LS_SOC_FP,
                              (BSL_META_U(unit,
                                          "    "
                                          "Key: fail to allocate Database %d because the no key available at this size %d \n\r"), 
                               db_id, tmp_start_place));
                    *found = FALSE;
                    is_key_not_found = TRUE;
                    break; 
                }
            }
        } 
        if(is_key_not_found || (*found == FALSE)) {
            
            continue;
        }

      *nof_consts = total_ce_indx;
      

      
      
      for(prog_ndx = 0; prog_ndx < ARAD_PMF_LOW_LEVEL_NOF_PROGS_ALL_STAGES; prog_ndx++) {
          selected_cycle[prog_ndx] = cycle_id;
      }
      
      break;
  } 

  *place_start = tmp_start_place;

exit:
  ARAD_FREE(ce_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_alloc_constrain_calc()", 0, 0);
}


STATIC
 uint32
  arad_pp_fp_key_dealloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
    SOC_SAND_IN  uint32                       prog_ndx,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] 
  )
{
    uint32
        key_id,
        ce_indx=0,
        ce_rsrc_bmp[1],
        keys_bmp[1],
        access_profile_array_id,
        nof_access_profiles;
    uint8
        is_cascaded,
        lookup_id,
        found,
        ce_msb;
    ARAD_PMF_CE
        sw_db_ce;
    uint32
      res = SOC_SAND_OK;
    ARAD_PP_FP_KEY_DP_PROG_INFO
        db_prog_info;
    ARAD_PP_FP_KEY_ALLOC_INFO   
        alloc_info;
    ARAD_TCAM_BANK_ENTRY_SIZE
        entry_size;
    ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_DATA
        pass_2_key_update_tbl_data;
    SOC_PPC_FP_DATABASE_INFO
        db_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_FP_KEY_ALLOC_INFO_clear(&alloc_info);

    
    res = arad_pp_fp_db_prog_info_get(
            unit,
            stage,
            db_id,
            prog_ndx,
            &db_prog_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            stage,
            db_id,
            &db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 17, exit);

    
    for(ce_indx = 0; ce_indx < db_prog_info.nof_ces; ++ce_indx) {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(
                unit,
                stage,
                prog_ndx,
                db_prog_info.cycle,
                db_prog_info.ces[ce_indx],
                &sw_db_ce
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        ce_msb = db_prog_info.ces[ce_indx] >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB;
        res = arad_pmf_ce_nop_entry_set_unsafe(
                unit,
                stage,
                prog_ndx,
                db_prog_info.key_id[sw_db_ce.is_second_key],
                db_prog_info.ces[ce_indx]%ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB, 
                ce_msb,
                db_prog_info.cycle,
                TRUE 
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10+ce_indx, exit);

        
        ARAD_CLEAR(&sw_db_ce, ARAD_PMF_CE, 1);
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.set(
                unit,
                stage,
                prog_ndx,
                db_prog_info.cycle,
                db_prog_info.ces[ce_indx],
                &sw_db_ce
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(
                  unit,
                  stage,
                  prog_ndx,
                  db_prog_info.cycle,
                  ce_rsrc_bmp
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
        SHR_BITCLR(ce_rsrc_bmp, db_prog_info.ces[ce_indx]);
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(
                  unit,
                  stage,
                  prog_ndx,
                  db_prog_info.cycle,
                  *ce_rsrc_bmp
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 33, exit);
    }

    
    alloc_info.key_id[0] = db_prog_info.key_id[0];
    alloc_info.key_id[1] = db_prog_info.key_id[1];
    alloc_info.cycle = db_prog_info.cycle;

    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, ARAD_PP_FP_DB_ID_TO_TCAM_DB(db_id), &entry_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);

    
    nof_access_profiles = 0;
    if ((db_info.db_type != SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION) && (db_info.db_type != SOC_PPC_FP_DB_TYPE_SLB)) {
        nof_access_profiles = (db_prog_info.is_320b || (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS))? 2 : 1; 
    }
    for (access_profile_array_id = 0; access_profile_array_id < nof_access_profiles; access_profile_array_id++) {
        
        
        res = arad_pp_fp_key_access_profile_hw_set(
                unit,
                stage,
                prog_ndx,
                access_profile_array_id,
                ARAD_TCAM_NOF_ACCESS_PROFILE_IDS,  
                &alloc_info
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    }

    
    
    res = arad_pp_fp_db_cascaded_cycle_get(
            unit,
            db_id,
            &is_cascaded,
            &lookup_id 
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    if (is_cascaded && (lookup_id == 0)) {
        if (!SOC_IS_JERICHO_PLUS(unit)) {
            res = arad_pp_ihb_pmf_pass_2_key_update_tbl_get_unsafe(
                    unit,
                    prog_ndx,
                    &pass_2_key_update_tbl_data
              );
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

            pass_2_key_update_tbl_data.enable_update_key = 0;
            pass_2_key_update_tbl_data.action_select = 0;

            if (SOC_IS_ARADPLUS(unit))
            {
                uint8 is_equal;
                uint32 place_unused;
                uint32 *key_bits[4];
                int i = 0;
                key_bits[i++] = &pass_2_key_update_tbl_data.key_a_lem_operation_select;
                key_bits[i++] = &pass_2_key_update_tbl_data.key_b_lem_operation_select;
                key_bits[i++] = &pass_2_key_update_tbl_data.key_c_lem_operation_select;
                key_bits[i++] = &pass_2_key_update_tbl_data.key_d_lem_operation_select;
             
                res = arad_pp_fp_db_is_equal_place_get(
                    unit,
                    db_id,
                    &is_equal,
                    &place_unused
                );
                SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);

                if (is_equal) {
                    pass_2_key_update_tbl_data.key_d_use_compare_result = 0; 
                    pass_2_key_update_tbl_data.key_d_mask_select = 0; 
                    pass_2_key_update_tbl_data.key_d_xor_enable = 0;
                }

                
                *(key_bits[db_prog_info.key_id[0]]) = 0;
            }
            
            res = arad_pp_ihb_pmf_pass_2_key_update_tbl_set_unsafe(
                    unit,
                    prog_ndx,
                    &pass_2_key_update_tbl_data
              );
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
        }
    }

    
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.get(
                unit,
                stage,
                prog_ndx,
                db_prog_info.cycle,
                keys_bmp  
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);

    
    key_id = db_prog_info.key_id[0];
    res = arad_pp_fp_key_alloc_key_bitmap_configure(
            unit,
            stage,
            db_id,
            db_prog_info.alloc_place,
            FALSE, 
            keys_bmp, 
            &key_id, 
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.key.set(
            unit,
            stage,
            prog_ndx,
            db_prog_info.cycle,
            *keys_bmp  
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_dealloc_in_prog()", 0, 0);
}



uint32
  arad_pp_fp_key_dealloc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] 
  )
{
  uint32   
      cur_prog = 0;
  uint32
      exist_progs;
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            stage,
            db_id,
            0,
            &exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  for (cur_prog = 0; cur_prog < ARAD_PMF_LOW_LEVEL_NOF_PROGS; cur_prog++) {
      if (exist_progs & (1 << cur_prog)) {
          res = arad_pp_fp_key_dealloc_in_prog(
                    unit,
                    stage,
                    cur_prog,
                    db_id,
                    qual_types
                 );
          SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      }
  }
          
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_dealloc()", 0, 0);
}


uint32 arad_pp_fp_ce_info_get(      
          SOC_SAND_IN  int                    unit,
          SOC_SAND_IN  uint32                 db_id,
          SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE stage,
          SOC_SAND_OUT ARAD_PMF_CE      *ce_array,
          SOC_SAND_OUT uint8                  *nof_ces,
          SOC_SAND_OUT uint8                  *is_key_320b,
          SOC_SAND_OUT uint8                  *ces_ids
       )
{

    ARAD_PP_FP_KEY_DP_PROG_INFO db_prog_info;
    SOC_PPC_FP_DATABASE_INFO *fp_database_info = NULL;
    uint32 res = SOC_SAND_OK, db_prog, prog_result, exist_progs[1];
    int i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_ALLOC(fp_database_info, SOC_PPC_FP_DATABASE_INFO, 1, "arad_pp_fp_ce_info_get.fp_database_info");
    
     SOC_PPC_FP_DATABASE_INFO_clear(fp_database_info);

     res = arad_pp_fp_database_get_unsafe(unit, db_id, fp_database_info);
     SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

     if (fp_database_info->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_DBAL) {

         res = arad_pp_dbal_ce_info_get(unit, db_id, stage, ce_array, nof_ces, is_key_320b, ces_ids);              
         SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
     }else if (fp_database_info->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_EXTENDED_DATABASES) {
        res = arad_pp_fp_kbp_match_key_ces_get(unit, fp_database_info->internal_table_id, ce_array, nof_ces);
        SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
        (*is_key_320b) = 0;
        for(i = 0; i < (*nof_ces); i++){
            ces_ids[i] = i;
        }
     } else {        
        
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(unit, stage, db_id, 0, exist_progs);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        db_prog = 0;
        ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,db_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,ARAD_PMF_LOW_LEVEL_NOF_PROGS,FALSE,prog_result);
        if(prog_result == 0) {
            LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META_U(unit,"Unit %d Stage %s No programss available.\n\r"),unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage)));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 20, exit);
        }

        
        res = arad_pp_fp_db_prog_info_get( unit, stage, db_id, db_prog, &db_prog_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

        (*nof_ces) = db_prog_info.nof_ces;
        (*is_key_320b) = db_prog_info.is_320b;

        for(i = 0; i < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS; ++i) {
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(unit, stage, db_prog, db_prog_info.cycle, i, &ce_array[i]);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);
        }
        sal_memcpy(ces_ids, &(db_prog_info.ces[0]), ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS);
    }

exit:
    ARAD_FREE(fp_database_info);
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_ce_info_get()", db_id, 0);
}

extern ARAD_PP_FP_DIP_SIP_CLUSTER_INFO dip_sip_sharing_cluster[ARAD_PP_FP_NOF_DIP_SIP_CLUSTERS];

uint32
  arad_pp_fp_key_value_buffer_mapping(
      SOC_SAND_IN  int                              unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE           stage,
      SOC_SAND_IN  uint32                           db_id,
      SOC_SAND_IN  ARAD_PP_FP_KEY_BUFFER_DIRECTION  direction, 
      SOC_SAND_IN  uint8                            is_for_kbp,
      SOC_SAND_IN  uint8                            is_from_dbal,
      SOC_SAND_IN  uint8                            nof_valid_bytes,
      SOC_SAND_INOUT  SOC_PPC_FP_QUAL_VAL      qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
      SOC_SAND_INOUT  uint32                   value_out[ARAD_PP_FP_TCAM_ENTRY_SIZE], 
      SOC_SAND_INOUT  uint32                   mask_out[ARAD_PP_FP_TCAM_ENTRY_SIZE] 
  )
{    
    int32
        key_ndx,   
        ce_indx=0;
    uint32 
        qual_index,  
        curr_msb,
        buffer_to_key_nof_lsb[2] = {0, 0},
        buffer_to_key_nof_msb[2] = {0, 0},
        qual_type = -1,
        nof_lsb[2]={0, 0}, 
        nof_msb[2]={0, 0},
        moved_to_msb=0, 
        nof_copy_bits,
        skip_lsb = 0,
        first_key_size,
        qual_lsb[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX+1], 
        buf_msb=ARAD_PP_FP_TCAM_ENTRY_SIZE*32,
        flp_lsb = 0,
        res = SOC_SAND_OK;

    ARAD_PMF_CE sw_db_ce;
    uint32                   
        value[ARAD_PP_FP_TCAM_ENTRY_SIZE],
        mask[ARAD_PP_FP_TCAM_ENTRY_SIZE];

    SOC_PPC_FP_QUAL_TYPE qual_type_lcl = SOC_PPC_FP_QUAL_IRPP_INVALID;
    
    uint8 qual_found;
    uint8 ce_ids[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS] = {0};
    uint8 is_key_320b = 0;
    uint8 nof_ces_to_handle;
    ARAD_PMF_CE ce_array[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS] = {{0}};

    uint32 dip_sip_sharing_enabled;
    int cluster_id;
    uint32 nof_share_qual = 0, nof_of_bits, nof_shared_bits=0;
    SOC_PPC_FP_DATABASE_INFO *fp_database_info = NULL;
#if defined(INCLUDE_KBP)
    SOC_PPC_FP_DATABASE_INFO *fp_database_info_l = NULL;
#endif
    ARAD_PP_FP_SHARED_QUAL_INFO shard_qualifiers_info[MAX_NOF_SHARED_QUALIFIERS_PER_PROGRAM];
    int i;
#if defined(INCLUDE_KBP)
    uint32 frwrd_table_id=0, table_size_in_bytes=0, table_payload_in_bytes=0;
#endif 
    uint32 table_size_in_bits=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    fp_database_info = sal_alloc(sizeof(SOC_PPC_FP_DATABASE_INFO), "SOC_PPC_FP_DATABASE_INFO");
    if (fp_database_info == NULL) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Fail to allocate SOC_PPC_FP_DATABASE_INFO in arad_pp_fp_key_value_buffer_mapping \n\r")));
        goto exit;
    }
#if defined(INCLUDE_KBP)
    fp_database_info_l = sal_alloc(sizeof(SOC_PPC_FP_DATABASE_INFO), "SOC_PPC_FP_DATABASE_INFO");
    if (fp_database_info_l == NULL) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Fail to allocate SOC_PPC_FP_DATABASE_INFO in arad_pp_fp_key_value_buffer_mapping \n\r")));
        goto exit;
    }

    if (is_for_kbp) {		 
        if (is_from_dbal) {
            arad_pp_dbal_db_predfix_get(unit, db_id, &frwrd_table_id);
        } else {
            res = arad_pp_fp_database_get_unsafe(unit, db_id, fp_database_info_l);
            SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

            if (fp_database_info_l->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_EXTENDED_DATABASES) {
                frwrd_table_id = fp_database_info_l->internal_table_id; 
            } else {
                frwrd_table_id = (ARAD_KBP_ACL_TABLE_ID_OFFSET + db_id);
            }
        }

        
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)) 
        {
            ARAD_KBP_DB_HANDLES db_handles_info;
            res = KBP_INFO.db_info.get(unit, frwrd_table_id,&db_handles_info);
            if (db_handles_info.is_valid) 
            {
                res = arad_kbp_table_size_get(unit, frwrd_table_id, &table_size_in_bytes, &table_payload_in_bytes); 
                SOC_SAND_CHECK_FUNC_RESULT(res,  71, exit);
                table_size_in_bits = table_size_in_bytes * 8;
            }
        }
        else
        {
            res = arad_kbp_table_size_get(unit, frwrd_table_id, &table_size_in_bytes, &table_payload_in_bytes); 	 
            SOC_SAND_CHECK_FUNC_RESULT(res,  71, exit);
            table_size_in_bits = table_size_in_bytes * 8;
        }

	}
#endif

    
    if (is_for_kbp && (direction == ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY)) {
        arad_pp_fp_dip_sip_sharing_is_sharing_enabled_for_db(unit, stage, db_id, &dip_sip_sharing_enabled, &cluster_id);
        if (dip_sip_sharing_enabled) {

            
            res = arad_pp_fp_database_get_unsafe(unit, db_id, fp_database_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);
            
            
            
            for (i = 0; i < dip_sip_sharing_cluster[cluster_id].nof_qualifiers; i++) {
                for (qual_index = 0; qual_index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_index++) {
                    if ((fp_database_info->qual_types[qual_index] == SOC_PPC_NOF_FP_QUAL_TYPES) 
                        || (fp_database_info->qual_types[qual_index] == BCM_FIELD_ENTRY_INVALID)) {
                        break;
                    }
                    if (dip_sip_sharing_cluster[cluster_id].shared_quals[i].qual_type == fp_database_info->qual_types[qual_index]) {
                        sal_memcpy(shard_qualifiers_info[nof_share_qual].name, dip_sip_sharing_cluster[cluster_id].shared_quals[i].name, 20);
                        shard_qualifiers_info[nof_share_qual].qual_type = dip_sip_sharing_cluster[cluster_id].shared_quals[i].qual_type;
                        shard_qualifiers_info[nof_share_qual].size = dip_sip_sharing_cluster[cluster_id].shared_quals[i].size;       
                        nof_share_qual++;
                        break;
                    }
                }              
            }
            
            nof_of_bits = table_size_in_bits;
            for (qual_index = 0; qual_index < nof_share_qual; qual_index++) {
                nof_of_bits -= shard_qualifiers_info[qual_index].size<<3;
                SHR_BITCOPY_RANGE(qual_vals[qual_index].val.arr, 0, value_out, nof_of_bits, (shard_qualifiers_info[qual_index].size<<3));
                SHR_BITCOPY_RANGE(qual_vals[qual_index].is_valid.arr, 0, mask_out, nof_of_bits, (shard_qualifiers_info[qual_index].size<<3));
                qual_vals[qual_index].type = shard_qualifiers_info[qual_index].qual_type;
            }
            nof_shared_bits = table_size_in_bits - nof_of_bits;
        }
    }

    

    
    sal_memset(value,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);
    sal_memset(mask,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);
    sal_memset(qual_lsb,0x0, sizeof(uint32) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX+1);

    if(is_from_dbal)
    {
        res = arad_pp_dbal_ce_info_get(unit, db_id, stage, &ce_array[0], &nof_ces_to_handle, &is_key_320b, &ce_ids[0]);              
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    }else{
        res = arad_pp_fp_ce_info_get(unit, db_id, stage, &ce_array[0], &nof_ces_to_handle, &is_key_320b, &ce_ids[0]);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }    

#ifdef ARAD_PP_FP_DEBUG_PRINTS
    {
        int i;

        LOG_CLI((BSL_META("   ""\nTable qualifiers:\n")));        
        for (i = 0; i < nof_ces_to_handle; i++) {
            LOG_CLI((BSL_META("   ""\tqual_type = %s, qual_lsb=%d\n"), SOC_PPC_FP_QUAL_TYPE_to_string(ce_array[i].qual_type), ce_array[i].qual_lsb));        
        }

        LOG_CLI((BSL_META("   ""\nInput qualifiers:\n")));
        for (i = 0; i < nof_ces_to_handle+5; i++) {
            LOG_CLI((BSL_META("   ""\tqual_type = %s (%d), val = %d\n"), SOC_PPC_FP_QUAL_TYPE_to_string(qual_vals[i].type), qual_vals[i].type, qual_vals[i].val.arr[0]));                
        }
    }
#endif
    
    
    if (direction == ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY) {
        for(ce_indx = 0; ce_indx < nof_ces_to_handle; ++ce_indx) {

            if(ce_ids[ce_indx] >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB) {
                buffer_to_key_nof_msb[ce_array[ce_ids[ce_indx]].is_second_key] += ce_array[ce_ids[ce_indx]].msb_padding;
                buffer_to_key_nof_msb[ce_array[ce_ids[ce_indx]].is_second_key] += ce_array[ce_ids[ce_indx]].msb + 1;
            }else {
                buffer_to_key_nof_lsb[ce_array[ce_ids[ce_indx]].is_second_key] += ce_array[ce_ids[ce_indx]].msb_padding;
                buffer_to_key_nof_lsb[ce_array[ce_ids[ce_indx]].is_second_key] += ce_array[ce_ids[ce_indx]].msb + 1;
            }
        }
        
        
        curr_msb = ARAD_PP_FP_TCAM_ENTRY_SIZE*32;
        for (key_ndx = 0; key_ndx <= is_key_320b; key_ndx++) {
            
            curr_msb -= buffer_to_key_nof_lsb[key_ndx];
            nof_copy_bits = buffer_to_key_nof_lsb[key_ndx];
            
            if (is_for_kbp) {
                skip_lsb = (((nof_copy_bits + 7)/8)*8); 
                skip_lsb = (table_size_in_bits * (key_ndx+1)) - skip_lsb - nof_shared_bits;
            } else {
                skip_lsb = 160 * key_ndx;
            }
            SHR_BITCOPY_RANGE(mask, curr_msb, mask_out, skip_lsb, nof_copy_bits);
            SHR_BITCOPY_RANGE(value, curr_msb, value_out, skip_lsb, nof_copy_bits);
            LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META_U(unit,"LSB  Buffer-->Key:  ce-id:%d, type:%s, curr_msb %d, nof_copy_bits %d val %X %X %X mask %X\n\r"),
                       ce_ids[ce_indx], SOC_PPC_FP_QUAL_TYPE_to_string(ce_array[ce_ids[ce_indx]].qual_type), curr_msb, nof_copy_bits, value_out[160 * key_ndx - 1], value_out[160 * key_ndx], value_out[160 * key_ndx + 1], mask_out[160 * key_ndx]));

            
            curr_msb -= buffer_to_key_nof_msb[key_ndx];
            nof_copy_bits = buffer_to_key_nof_msb[key_ndx];
            
            if (is_for_kbp) {
                skip_lsb = table_size_in_bits - SOC_SAND_ALIGN_UP(buffer_to_key_nof_lsb[key_ndx], 8) - SOC_SAND_ALIGN_UP(nof_copy_bits, 8);
            } else {
                skip_lsb = (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB) ? 0 : 80;                
            }
            SHR_BITCOPY_RANGE(mask, curr_msb, mask_out, skip_lsb + (160 * key_ndx), nof_copy_bits);
            SHR_BITCOPY_RANGE(value, curr_msb, value_out, skip_lsb + (160 * key_ndx), nof_copy_bits);
            LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META_U(unit,"MSB  Buffer-->Key:  ce-id:%d, type:%s, curr_msb %d, nof_copy_bits %d skip_lsb %d val %X mask %X\n\r"),
                       ce_ids[ce_indx], SOC_PPC_FP_QUAL_TYPE_to_string(ce_array[ce_ids[ce_indx]].qual_type), curr_msb, nof_copy_bits, skip_lsb, value_out[(skip_lsb+160 * key_ndx)/32], mask_out[(skip_lsb+ 160 * key_ndx)/32]));
        }
    }

     
    
    for (key_ndx = 0; key_ndx <= is_key_320b; key_ndx++) {
        first_key_size = nof_lsb[0] + nof_msb[0]; 
        moved_to_msb = 0;
        
        for(ce_indx = 0; ce_indx < nof_ces_to_handle; ++ce_indx) {

            sw_db_ce = ce_array[ce_ids[ce_indx]];
            
            if (sw_db_ce.is_second_key != key_ndx) {
                
                continue;
            }
            LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META_U(unit,"   Key value-buffer: direction:%d, ce-id:%d, type:%s \n\r"), direction, 
                       ce_ids[ce_indx], SOC_PPC_FP_QUAL_TYPE_to_string(sw_db_ce.qual_type)));

            if(ce_ids[ce_indx] >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB && moved_to_msb == 0) {
                nof_lsb[key_ndx]= (ARAD_PP_FP_TCAM_ENTRY_SIZE*32 ) - buf_msb - first_key_size;
                moved_to_msb = 1;
            }
            
            
            qual_found = 0;
            for(qual_index = nof_share_qual; qual_index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX ; ++qual_index) {
                
                qual_type_lcl = qual_vals[qual_index].type;
                if (direction == ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER) {
                    
                    if(qual_type_lcl == BCM_FIELD_ENTRY_INVALID) {
                        continue;
                    }
                }
                else if (direction == ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY) {
                    
                    if(qual_type_lcl == BCM_FIELD_ENTRY_INVALID) {
                        
                        qual_found = 1;
                        qual_type = qual_type_lcl;
                        break;
                    }
                }

                
                if(sw_db_ce.qual_type == qual_type_lcl){
                    qual_found = 1;
                    qual_type = qual_type_lcl;
                    break;
                }
            }
            if(!qual_found) {
                
                buf_msb -= (sw_db_ce.msb_padding + sw_db_ce.msb + 1); 
                if (moved_to_msb == 0) {
                    nof_lsb[key_ndx] = (ARAD_PP_FP_TCAM_ENTRY_SIZE*32) - buf_msb - first_key_size;
                }
                LOG_DEBUG(BSL_LS_SOC_FP,
                          (BSL_META_U(unit," db_id %d  qual:%s  qual is masked. skip to %d (%d bits)\n\r"), db_id, SOC_PPC_FP_QUAL_TYPE_to_string(qual_type_lcl), buf_msb, sw_db_ce.msb + 1));
                continue;
            }


            
            nof_copy_bits = sw_db_ce.msb - sw_db_ce.lsb + 1;
            LOG_DEBUG(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "   db_id %d, "
                                  "skip lsb:%d, "
                                  "buf_msb:%d, "
                                  "qual-lsb bits:%d, "
                                  "qual-lsb bits sw db:%d, "
                                  "# copied bits:%d,"
                                  " nof_lsb[key_ndx %d] %d\n\r"), db_id, sw_db_ce.lsb, buf_msb, qual_lsb[qual_index], sw_db_ce.qual_lsb, nof_copy_bits, key_ndx, nof_lsb[key_ndx]));

            {
                uint32
                    copied_val=0,
                    copied_maks=0;

                if (direction == ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER) {
                    SHR_BITCOPY_RANGE(&copied_val, 0, qual_vals[qual_index].val.arr, sw_db_ce.qual_lsb, nof_copy_bits);
                    SHR_BITCOPY_RANGE(&copied_maks, 0, qual_vals[qual_index].is_valid.arr, sw_db_ce.qual_lsb, nof_copy_bits);
                    LOG_DEBUG(BSL_LS_SOC_FP,
                              (BSL_META_U(unit,
                                          "   db_id %d, "
                                          "qual:%s, "
                                          "copied val:%08x, "
                                          "copied mask:%08x \n\r"
                                          "qual_vals[qual_index].val.arr:%08x %08x "
                                          "qual_vals[qual_index].is_valid.arr:%08x %08x \n\r"), db_id, SOC_PPC_FP_QUAL_TYPE_to_string(qual_type), copied_val, copied_maks,
                                      qual_vals[qual_index].val.arr[1],qual_vals[qual_index].val.arr[0],
                                      qual_vals[qual_index].is_valid.arr[1], qual_vals[qual_index].is_valid.arr[0]));
                }
                else if (direction == ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY) {
                    SHR_BITCOPY_RANGE(&copied_val, 0, value, buf_msb-nof_copy_bits, nof_copy_bits);
                    SHR_BITCOPY_RANGE(&copied_maks, 0, mask, buf_msb-nof_copy_bits, nof_copy_bits);
                    LOG_DEBUG(BSL_LS_SOC_FP,
                              (BSL_META_U(unit,
                                          "   db_id %d, "
                                          "qual:%s, "
                                          "copied val:%08x, "
                                          "copied mask:%08x \n\r"
                                          "value[buf_msb - nof_copy_bits %d]:%08x"
                                          "mask[buf_msb - nof_copy_bits %d]:%08x \n\r"), db_id, SOC_PPC_FP_QUAL_TYPE_to_string(sw_db_ce.qual_type), copied_val, copied_maks,
                                      buf_msb-nof_copy_bits, value[nof_copy_bits],
                                      buf_msb-nof_copy_bits, mask[nof_copy_bits]));
                }

            }
            
            if (direction == ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER) {
                SHR_BITCOPY_RANGE(value, buf_msb-nof_copy_bits-sw_db_ce.msb_padding, qual_vals[qual_index].val.arr, sw_db_ce.qual_lsb, nof_copy_bits);
                SHR_BITCOPY_RANGE(mask, buf_msb-nof_copy_bits-sw_db_ce.msb_padding, qual_vals[qual_index].is_valid.arr, sw_db_ce.qual_lsb, nof_copy_bits);
            }
            else if (direction == ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY) {
                
                SHR_BITCOPY_RANGE(qual_vals[qual_index].val.arr, sw_db_ce.qual_lsb, value, buf_msb-nof_copy_bits-sw_db_ce.msb_padding, nof_copy_bits);
                SHR_BITCOPY_RANGE(qual_vals[qual_index].is_valid.arr, sw_db_ce.qual_lsb, mask, buf_msb-nof_copy_bits-sw_db_ce.msb_padding, nof_copy_bits);
                qual_vals[qual_index].type = sw_db_ce.qual_type;
            }
            buf_msb -= sw_db_ce.msb_padding;
            buf_msb -= nof_copy_bits;
            
            buf_msb -= sw_db_ce.lsb;
            
            qual_lsb[qual_index] += nof_copy_bits;

            
            if (moved_to_msb == 0) {
                nof_lsb[key_ndx] = (ARAD_PP_FP_TCAM_ENTRY_SIZE*32) - buf_msb - first_key_size;
            }
        } 

         
        
        if (direction == ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER) {
            
            if(buf_msb != ARAD_PP_FP_TCAM_ENTRY_SIZE*32) {
                 
                
                if(moved_to_msb) {
                    nof_msb[key_ndx] = (ARAD_PP_FP_TCAM_ENTRY_SIZE*32-buf_msb) - nof_lsb[key_ndx] - first_key_size;
                }
                else{
                    nof_msb[key_ndx] = 0;
                }

                
#if defined(INCLUDE_KBP)
                if (is_for_kbp) {
                    flp_lsb = table_size_in_bits - SOC_SAND_ALIGN_UP(nof_lsb[key_ndx], 8);

                }
#endif

                if(nof_lsb[key_ndx] > 0) 
                {
#if defined(INCLUDE_KBP)
                    if (!is_for_kbp) {
                        flp_lsb = 0;
                    }
#else
                    flp_lsb = 0;
#endif
                    LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META("key_ndx=%d, flp_lsb=%d, buf_msb=%d, nof_msb[key_ndx]=%d, nof_lsb[key_ndx]=%d\n"),
                             key_ndx, flp_lsb, buf_msb, nof_msb[key_ndx], nof_lsb[key_ndx]));

                    SHR_BITCOPY_RANGE(mask_out,(160 * key_ndx) + flp_lsb, mask, buf_msb+nof_msb[key_ndx], nof_lsb[key_ndx]);
                    SHR_BITCOPY_RANGE(value_out,(160 * key_ndx) + flp_lsb, value, buf_msb+nof_msb[key_ndx], nof_lsb[key_ndx]);
                    LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META_U(unit,"LSB  Key-->Buffer: (160 * key_ndx) + flp_lsb %d  buf_msb+nof_msb[key_ndx]:%d, key_ndx %d, nof_lsb[key_ndx] %d val %X mask %X\n\r"),
                            (160 * key_ndx) + flp_lsb, buf_msb+nof_msb[key_ndx], key_ndx, nof_lsb[key_ndx], value_out[(160 * key_ndx) + flp_lsb], mask_out[(160 * key_ndx) + flp_lsb]));
                }

                
                
                if((is_for_kbp && SOC_IS_JERICHO(unit))){
                    skip_lsb = flp_lsb - nof_msb[key_ndx];
				}else{
                	skip_lsb = 80;
				}

                LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META("moved_to_msb=%d, key_ndx=%d, skip_lsb=%d, buf_msb=%d, nof_msb[key_ndx]=%d\n"),
                         moved_to_msb, key_ndx, skip_lsb, buf_msb, nof_msb[key_ndx]));

                if(moved_to_msb) {
                    SHR_BITCOPY_RANGE(mask_out, skip_lsb + (160 * key_ndx), mask, buf_msb, nof_msb[key_ndx]);
                    SHR_BITCOPY_RANGE(value_out, skip_lsb + (160 * key_ndx), value, buf_msb, nof_msb[key_ndx]);
                }
            }

#ifdef ARAD_PP_FP_DEBUG_PRINTS
            {
                uint32 print_idx;                
                char string_to_print[20*ARAD_PP_FP_TCAM_ENTRY_SIZE + 10] = "";

                LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META_U(unit,"   ""Buffer after shifting:\n\r")));

                for(print_idx = 0; print_idx < ARAD_PP_FP_TCAM_ENTRY_SIZE; ++print_idx) {
                    sal_sprintf(string_to_print + sal_strlen(string_to_print), "%08x:", value_out[ARAD_PP_FP_TCAM_ENTRY_SIZE-print_idx-1]);
                }

                LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META_U(unit,"    ""%s\n\r"), string_to_print));

                sal_sprintf(string_to_print, "    ");

                for(print_idx = 0; print_idx < ARAD_PP_FP_TCAM_ENTRY_SIZE; ++print_idx) {
                    sal_sprintf(string_to_print + sal_strlen(string_to_print), "%08x:", mask_out[ARAD_PP_FP_TCAM_ENTRY_SIZE-print_idx-1]);
                }
                LOG_DEBUG(BSL_LS_SOC_FP,(BSL_META_U(unit,"%s\n\r"), string_to_print));
            }
#endif
        } 
    } 

    
    
exit:
    if (fp_database_info != NULL)
        sal_free(fp_database_info);
#if defined(INCLUDE_KBP)
    if (fp_database_info_l != NULL)
        sal_free(fp_database_info_l);
#endif
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_value_buffer_mapping()", db_id, 0);
}



uint32
  arad_pp_fp_key_buffer_to_value(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                   db_id,
      SOC_SAND_IN  uint8                    nof_valid_bytes,
      SOC_SAND_IN  uint32                   value_in[ARAD_PP_FP_TCAM_ENTRY_SIZE],
      SOC_SAND_IN  uint32                   mask_in[ARAD_PP_FP_TCAM_ENTRY_SIZE],
      SOC_SAND_OUT  SOC_PPC_FP_QUAL_VAL     qual_vals_out[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]
  )
{
    uint32
      res = SOC_SAND_OK;
    uint32        
        qual_val_ndx,           
        value_in_lcl[ARAD_PP_FP_TCAM_ENTRY_SIZE],
        mask_in_lcl[ARAD_PP_FP_TCAM_ENTRY_SIZE];
    uint32 is_for_kbp = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for(qual_val_ndx = 0; qual_val_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++qual_val_ndx) {
        SOC_PPC_FP_QUAL_VAL_clear(&qual_vals_out[qual_val_ndx]);
    }


    sal_memcpy(value_in_lcl, value_in, ARAD_PP_FP_TCAM_ENTRY_SIZE * sizeof(uint32));
    sal_memcpy(mask_in_lcl, mask_in, ARAD_PP_FP_TCAM_ENTRY_SIZE * sizeof(uint32));

    
    if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
        is_for_kbp = TRUE;

    }

    res = arad_pp_fp_key_value_buffer_mapping(
            unit,
            stage,
            db_id,
            ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY,
            is_for_kbp,
            0, 
            nof_valid_bytes,
            qual_vals_out,
            value_in_lcl, 
            mask_in_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_buffer_to_value()", db_id, 0);
}




uint32
  arad_pp_fp_key_value_to_buffer(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE stage,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL    qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
      SOC_SAND_IN  uint32                 db_id,
      SOC_SAND_OUT  uint32                value_out[ARAD_PP_FP_TCAM_ENTRY_SIZE],
      SOC_SAND_OUT  uint32                mask_out[ARAD_PP_FP_TCAM_ENTRY_SIZE]
  )
{
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_FP_QUAL_VAL      
        qual_vals_lcl[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint8 is_for_kbp = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    sal_memset(value_out,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);
    sal_memset(mask_out,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);
    sal_memcpy(qual_vals_lcl, qual_vals, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX * sizeof(SOC_PPC_FP_QUAL_VAL));


    
    if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
        is_for_kbp = TRUE;

    }

    res = arad_pp_fp_key_value_buffer_mapping(
            unit,
            stage,
            db_id,
            ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER,
            is_for_kbp,
            0,
            0,
            qual_vals_lcl,
            value_out, 
            mask_out
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_key_value_to_buffer()", db_id, 0);
}



uint32
  arad_pp_fp_qual_length_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE       qual_type,
    SOC_SAND_OUT uint32                     *found,
    SOC_SAND_OUT uint32                     *length_padded_best_case,
    SOC_SAND_OUT uint32                     *length_padded_worst_case,
    SOC_SAND_OUT uint32                     *length_logical
  )
{
  uint32
    res;
  ARAD_PP_FP_QUAL_CE_INFO            
      ce_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(length_padded_best_case);
  SOC_SAND_CHECK_NULL_INPUT(length_padded_worst_case);
  SOC_SAND_CHECK_NULL_INPUT(length_logical);
  SOC_SAND_CHECK_NULL_INPUT(found);

  if (SOC_IS_ARADPLUS(unit))
  {  
      
      if(qual_type == SOC_PPC_FP_QUAL_IS_EQUAL)
      {
          *found = TRUE;
          *length_logical = 4;
          *length_padded_best_case = 4;
          *length_padded_worst_case = 4;
          ARAD_DO_NOTHING_AND_EXIT;
      }
  }
  if((qual_type == SOC_PPC_FP_QUAL_HDR_IPV6_L4OPS) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP))
  {
      *found = TRUE;
      *length_logical = 32;
      *length_padded_best_case = 16;
      *length_padded_worst_case = 16;
      ARAD_DO_NOTHING_AND_EXIT;
  }
  
  
  res = arad_pp_fp_qual_ce_info_get(
            unit,
            stage,
            qual_type,
            &ce_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if (ce_info.is_lsb || ce_info.is_msb)
  {
      *found = TRUE;
      *length_logical = ce_info.qual_size;
      *length_padded_best_case = ce_info.lost_bits + ce_info.qual_size;
      *length_padded_worst_case = ce_info.lost_bits_worst + ce_info.qual_size;
      ARAD_DO_NOTHING_AND_EXIT;
 }

    *found = FALSE;
    *length_logical = 0;
    *length_padded_best_case = 0;
    *length_padded_worst_case = 0;
    
    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_length_get()", 0, 0);
}




CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_fp_key_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_fp_key;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_fp_key_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_fp_key;
}

void
  ARAD_PP_FP_CE_CONSTRAINT_clear(
    SOC_SAND_OUT ARAD_PP_FP_CE_CONSTRAINT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_FP_CE_CONSTRAINT));
  info->cycle_cons = ARAD_PP_FP_KEY_CYCLE_ANY;
  info->place_cons = ARAD_PP_FP_KEY_CE_PLACE_ANY;
  info->size_cons = ARAD_PP_FP_KEY_CE_SIZE_ANY;
  info->group_cons = ARAD_PP_FP_KEY_CE_PLACE_ANY;
  info->is_second_key = FALSE;
  info->lost_bits = 0;
  info->qual_lsb = 0;
  info->qual_type = BCM_FIELD_ENTRY_INVALID;
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  ARAD_PP_FP_KEY_ALLOC_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_KEY_ALLOC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_FP_KEY_ALLOC_INFO));
  info->key_size =  ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  ARAD_PP_FP_KEY_DESC_clear(
    SOC_SAND_OUT ARAD_PP_FP_KEY_DESC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_FP_KEY_DESC));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}






#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

#endif 

