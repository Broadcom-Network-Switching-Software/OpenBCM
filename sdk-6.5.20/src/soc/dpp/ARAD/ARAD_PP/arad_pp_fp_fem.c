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
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_fem.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>






#define ARAD_PP_FP_FEM_DB_ID_NDX_MAX                             (SOC_PPC_FP_NOF_DBS-1)
#define ARAD_PP_FP_FEM_ACTION_TYPE_MAX                           (SOC_PPC_NOF_FP_ACTION_TYPES_ARAD - 1)
#define ARAD_PP_FP_FEM_DB_STRENGTH_MAX                           (1023)
#define ARAD_PP_FP_FEM_DB_ID_MAX                                 (ARAD_PP_FP_FEM_DB_ID_NDX_MAX)
#define SOC_PPC_FP_FEM_ENTRY_STRENGTH_MAX                        (SOC_SAND_U16_MAX)

#define ARAD_PP_FP_FEM_SAND_U64_NOF_BITS                         (64)
#define ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS                       (4)

#define ARAD_PP_FP_BIT_LOC_LSB_CHANGE_KEY                        (5)
#define ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE                (2 * SOC_DPP_DEFS_GET(unit, tcam_action_width))
#define ARAD_PP_FP_KAPS_RESULT_ACTION_WIDTH                      (32)









typedef struct
{
  uint32 indx;
  uint32 size;
} action_inf;


typedef enum
{
  
  ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER = 0,
  
  ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION = 1,
  
  ARAD_PP_NOF_FP_ACTION_BUFFER_DIRECTIONS = 2
}ARAD_PP_FP_ACTION_BUFFER_DIRECTION;





CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_fp_fem[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_GET_ERRS_PTR),
  
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_TAG_ACTION_TYPE_CONVERT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_FEM_BLOCKING_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_DUPLICATE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_CONFIGURE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_CONFIGURATION_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_REORGANIZE),



  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_fp_fem[] =
{
  
  {
    ARAD_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pfg_ndx' is out of range. \n\r "
    "The range is: 0 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'db_id_ndx' is out of range. \n\r "
    "The range is: 0 - 127.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'db_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR",
    "The parameter 'db_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PPC_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'entry_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR",
    "The parameter 'entry_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  



  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static ARAD_PMF_FES_INPUT_INFO invalid_next_FES_info;







uint32
    arad_pp_fp_fem_pgm_per_pmf_pgm_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_id,
    SOC_SAND_OUT uint8                          *fem_pgm_id
    )
{

    uint32
      res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.fem_pgm_per_pmf_prog.get
                                                (unit,stage,pmf_pgm_id,fem_pgm_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    

    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_per_pmf_pgm_get()", 0, pmf_pgm_id);

}


uint32
    arad_pp_fp_fem_pgm_per_pmf_pgm_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_id,
    SOC_SAND_IN  uint8                          fem_pgm_id
    )
{

    uint32
      res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.fem_pgm_per_pmf_prog.set
                                                (unit,stage,pmf_pgm_id,fem_pgm_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    

    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_per_pmf_pgm_set()", 0, pmf_pgm_id);

}



uint32
    arad_pp_fp_fem_pgm_id_remove(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_id
    )
{
    uint32
      res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

    
    res = arad_pp_fp_fem_pgm_per_pmf_pgm_set(unit,stage,pmf_pgm_id,ARAD_PMF_FEM_PGM_INVALID);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_per_pmf_pgm_set()", 0, pmf_pgm_id);  
}


uint32
    arad_pp_fp_fem_pgm_id_alloc(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_OUT uint8                          *fem_pgm_id
    )
{
    uint32
      res = SOC_SAND_OK,
      pmf_pgm_id,
      num_of_pgm0 = 0,
      num_of_pgm1 = 0,
      num_of_pgm2 = 0;
    uint8
      fem_pgm_id_temp = ARAD_PMF_FEM_PGM_INVALID;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    for(pmf_pgm_id = 0; pmf_pgm_id < ARAD_PMF_NOF_PROGS; pmf_pgm_id++)
    {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.fem_pgm_per_pmf_prog.get
                                                (unit,stage,pmf_pgm_id,&fem_pgm_id_temp);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        switch(fem_pgm_id_temp)
        {
            case ARAD_PMF_FEM_PGM_FOR_ETH_0:
                num_of_pgm0++;
                break;
            case ARAD_PMF_FEM_PGM_FOR_ETH_1:
                num_of_pgm1++;
                break;
            case ARAD_PMF_FEM_PGM_FOR_ETH_2:
                num_of_pgm2++;
                break;
            default:
                
                break;
        }
    }

    
    if(num_of_pgm0 <= num_of_pgm1 && num_of_pgm0 <= num_of_pgm2)
    {
        *fem_pgm_id = ARAD_PMF_FEM_PGM_FOR_ETH_0;
    }
    else if(num_of_pgm1 <= num_of_pgm0 && num_of_pgm1 <= num_of_pgm2)
    {
        *fem_pgm_id = ARAD_PMF_FEM_PGM_FOR_ETH_1;
    }
    else
    {
        *fem_pgm_id = ARAD_PMF_FEM_PGM_FOR_ETH_2;
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_id_new_get()", 0, pmf_pgm_id);
}


uint32
  arad_pp_fp_fem_pgm_id_bmp_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_OUT uint8                     *fem_pgm_bmp
    )
{
    uint32
      res = SOC_SAND_OK,
      pmf_pgm_bmp_used = 0;
    uint8 
      is_for_tm,
      is_default_tm,
      fem_pgm_id_temp = ARAD_PMF_FEM_PGM_INVALID;
    SOC_PPC_FP_DATABASE_INFO
      fp_database_info;
    uint8 pmf_pgm_id =0;
    SOC_PPC_FP_DATABASE_STAGE  stage;
    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(fem_pgm_bmp);
    
    *fem_pgm_bmp  = 0; 
    
    res = arad_pp_fp_db_stage_get(
            unit,
            db_id,
            &stage
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_PPC_FP_DATABASE_INFO_clear(&fp_database_info);
    res = arad_pp_fp_database_get_unsafe(
          unit,
          db_id,
          &fp_database_info
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(unit, stage, db_id, 0, &pmf_pgm_bmp_used);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
    
    res = arad_pp_fp_database_is_tm_get(unit, &fp_database_info, &is_for_tm, &is_default_tm);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);


    if(is_for_tm)
    {
        
        SOC_SAND_SET_BIT(*fem_pgm_bmp,1,ARAD_PMF_FEM_PGM_FOR_TM);
        
        for(pmf_pgm_id = 0 ; pmf_pgm_id < ARAD_PMF_NOF_PROGS; pmf_pgm_id++)
        {
            if (SOC_SAND_GET_BIT(pmf_pgm_bmp_used, pmf_pgm_id) == TRUE)
            {
                res = arad_pp_fp_fem_pgm_per_pmf_pgm_set(unit,stage,pmf_pgm_id,ARAD_PMF_FEM_PGM_FOR_TM);
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            }
        }
    }
    else
    {
        
        for(pmf_pgm_id = 0 ; pmf_pgm_id < ARAD_PMF_NOF_PROGS; pmf_pgm_id++)
        {
            if (SOC_SAND_GET_BIT(pmf_pgm_bmp_used, pmf_pgm_id) == TRUE)
            {
                res = arad_pp_fp_fem_pgm_per_pmf_pgm_get(unit,stage,pmf_pgm_id,&fem_pgm_id_temp);
                SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
                if(fem_pgm_id_temp == ARAD_PMF_FEM_PGM_INVALID)
                {
                    res = arad_pp_fp_fem_pgm_id_alloc(unit,stage,&fem_pgm_id_temp);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

                    res = arad_pp_fp_fem_pgm_per_pmf_pgm_set(unit,stage,pmf_pgm_id,fem_pgm_id_temp);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
                    
                }
                if(fem_pgm_id_temp <= ARAD_PMF_FEM_PGM_FOR_ETH_2)
                {
                    SOC_SAND_SET_BIT(*fem_pgm_bmp,1,fem_pgm_id_temp);   
                }
                else
                {
                    
                    SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_ERR, 20, exit);
                }
            }
        }
         
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_id_set()", 0, db_id);
}


uint32
  arad_pp_fp_action_alloc_fes(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       entry_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_type, 
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  uint32                       my_priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_IN  uint32                       action_lsb,
    SOC_SAND_IN  uint32                       action_len,
    SOC_SAND_IN  int32                        required_nof_feses,
    SOC_SAND_INOUT ARAD_PMF_FES               fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint8                        *found
  )
{
    uint32
        fem_id_ndx,
        fes_tm_lsb,
        new_fes_id = 0;
    ARAD_PMF_FES_INPUT_INFO
        my_fes_info;
    ARAD_PMF_FEM_INPUT_INFO
        fes_input_info;
    ARAD_PMF_FES
        fes_sw_db_info;
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_FP_DATABASE_STAGE
        stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
    SOC_PPC_FP_FEM_ENTRY
        fem_entry2,
      fem_entry;
    SOC_PPC_FP_FEM_CYCLE
      fem_cycle;
    
    SOC_PPC_FP_DATABASE_INFO               
        fp_db_info;
    uint8
        is_fem_blocking,
        use_kaps,
      is_fes_free,
      fem_pgm_id;
    SOC_SAND_SUCCESS_FAILURE
        success;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   "
                          "Allocate FES in prog:%d,  db_id:%d, action_type:%s \n\r"), prog_id, db_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_type)));

    *found = 0;

    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            stage,
            db_id,
            &fp_db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit); 

    use_kaps = (fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS) ? TRUE : FALSE;

    res = arad_pp_fp_fem_pgm_per_pmf_pgm_get(unit,stage,prog_id,&fem_pgm_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit); 
    if(fem_pgm_id == ARAD_PMF_FEM_PGM_INVALID)
    {
        res = arad_pp_fp_fem_pgm_id_alloc(unit,stage,&fem_pgm_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
        res = arad_pp_fp_fem_pgm_per_pmf_pgm_set(unit,stage,prog_id,fem_pgm_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
    }
    
    SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
    SOC_PPC_FP_FEM_CYCLE_clear(&fem_cycle);
    fem_entry.is_for_entry = FALSE;
    fem_entry.db_id = db_id;
    fem_entry.db_strength = my_priority;
    fem_entry.entry_id = prog_id; 
    fem_entry.action_type[0] = action_type;

    
    is_fes_free = FALSE;
    for (fem_id_ndx = 0; (fem_id_ndx < ARAD_PMF_LOW_LEVEL_NOF_FESS) && (!is_fes_free); ++fem_id_ndx)
    {
        if (!fes_info[fem_id_ndx].is_used) {
            new_fes_id = fem_id_ndx;
            is_fes_free = TRUE;
        }
    }

    success = SOC_SAND_SUCCESS;
    if (is_fes_free == FALSE)
    {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   "
                              "FEM not found for db_id:%d\n\r"), db_id));
      success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      *found = 0;
      ARAD_DO_NOTHING_AND_EXIT;
    }

    
    if  ( (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) == 0 ) && (flags & ARAD_PP_FP_FEM_ALLOC_FES_TM) ) {
        success = SOC_SAND_SUCCESS;
        goto exit_tm;
    }
    
    {
        
        res = arad_pp_fp_fem_insert_unsafe(
                unit,
                &fem_entry,
                &fem_cycle,
                flags,
                NULL,
                fes_info,
                fem_pgm_id,
                0,
                &new_fes_id,
                &success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    

    
    if ((success != SOC_SAND_SUCCESS) &&
        (fem_pgm_id != ARAD_PMF_FEM_PGM_INVALID)) 
    {
                
        is_fem_blocking = FALSE;
        for (fem_id_ndx = 0; (fem_id_ndx < ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP) && (!is_fem_blocking); ++fem_id_ndx)
        {
              SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry2);
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                      unit,
                      stage,
                      fem_pgm_id,
                      fem_id_ndx,
                      &fem_entry2
                    );
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 33, exit);
              if ((fem_entry2.is_for_entry == TRUE) && (fem_entry2.action_type[0] == action_type)) {
                  is_fem_blocking = TRUE;
              }
        }

        
        if (!is_fem_blocking) {
            res = arad_pp_fp_fem_insert_unsafe(
                    unit,
                    &fem_entry,
                    &fem_cycle,
                    (flags | ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP),
                    NULL,
                    fes_info,
                    fem_pgm_id,
                    0,
                    &new_fes_id,
                    &success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }

exit_tm:
    *found = (success == SOC_SAND_SUCCESS);


    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   allocated FES-id:%d, \n\r"), new_fes_id));

    
    if(*found && (flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "FES action:%s, length: %d, lsb: %d\n\r"), SOC_PPC_FP_ACTION_TYPE_to_string(action_type), action_len, action_lsb));
    }
    
    else if(*found  && !(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
         

        ARAD_PMF_FES_INPUT_INFO_clear(&my_fes_info);
        ARAD_PMF_FEM_INPUT_INFO_clear(&fes_input_info);
        
        
        if ((flags & ARAD_PP_FP_FEM_ALLOC_FES_TM) && (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) ==0 ) ) {
            fes_input_info.src_arad.is_key_src = TRUE;
            fes_input_info.src_arad.key_tcam_id = 0 ;
            
            fes_tm_lsb = (ARAD_PP_FP_KEY_LENGTH_TM_IN_BITS - action_lsb  - 1);
            fes_input_info.src_arad.key_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(fes_tm_lsb); 
            fes_input_info.src_arad.lookup_cycle_id = 1; 
            fes_input_info.src_arad.use_kaps = use_kaps;

            my_fes_info.shift = fes_tm_lsb - fes_input_info.src_arad.key_lsb;
            my_fes_info.action_type = action_type;
            my_fes_info.is_action_always_valid = TRUE;
        }
        else if (flags & ARAD_PP_FP_FEM_ALLOC_FES_FROM_KEY) {
            fes_input_info.src_arad.is_key_src = TRUE;
            fes_input_info.src_arad.key_tcam_id = constraint->tcam_res_id[0];
            
            fes_input_info.src_arad.key_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(constraint->tcam_res_id[1]); 
            fes_input_info.src_arad.lookup_cycle_id = 1; 
            fes_input_info.src_arad.use_kaps = use_kaps;

            my_fes_info.shift = constraint->tcam_res_id[1] - fes_input_info.src_arad.key_lsb;
            my_fes_info.action_type = action_type;
            my_fes_info.is_action_always_valid = (flags & ARAD_PP_FP_FEM_ALLOC_FES_KEY_IS_CONDITIONAL_VALID)? FALSE: TRUE;
        }
        else {
            fes_input_info.src_arad.is_key_src = FALSE;
            
            fes_input_info.src_arad.key_tcam_id = constraint->tcam_res_id[ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID(action_lsb)];
            fes_input_info.src_arad.key_lsb = action_lsb; 
            fes_input_info.src_arad.lookup_cycle_id = constraint->cycle;
            fes_input_info.src_arad.use_kaps = use_kaps;

            my_fes_info.shift = (use_kaps) ? ARAD_PP_FP_FEM_ACTION_LSB_TO_SHIFT_USE_KAPS(action_lsb) : ARAD_PP_FP_FEM_ACTION_LSB_TO_SHIFT(action_lsb);
            my_fes_info.action_type = action_type;
            my_fes_info.is_action_always_valid = FALSE;
        }
        
        my_fes_info.is_action_always_valid = (flags & ARAD_PP_FP_FEM_ALLOC_FES_ALWAYS_VALID)? TRUE : my_fes_info.is_action_always_valid;
        if (SOC_IS_JERICHO_PLUS(unit) && action_len > 0) {
                my_fes_info.valid_bits = 32 - action_len;
        }
        
        my_fes_info.polarity = (flags & ARAD_PP_FP_FEM_ALLOC_FES_POLARITY)? 1 : 0;
        
        if (use_kaps && (fp_db_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE) && SOC_IS_JERICHO_PLUS(unit)) {
            if ((action_type == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) || (action_type == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2)) {
                my_fes_info.is_action_always_valid = TRUE;
            } else if (ARAD_PP_FP_FEM_IS_ALWAYS_VALID_LARGE_DIRECT(action_type)) {
                
                my_fes_info.is_action_always_valid = TRUE;
            }
        }

        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   commit changes to HW: "
                              "FES action :%s, length: %d, "
                              "next action-lsb:%d  \n\r"), SOC_PPC_FP_ACTION_TYPE_to_string(action_type), action_len, action_lsb));

        res = arad_pmf_db_fem_input_set_unsafe(
                unit,
                prog_id,
                TRUE, 
                new_fes_id,
                &fes_input_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        
        
        if (!(flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED)) {
            res = arad_pmf_db_fes_set_unsafe(
                    unit,
                    prog_id,
                    new_fes_id,
                    &my_fes_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        } else {
            if ((action_type == SOC_PPC_FP_ACTION_TYPE_COUNTER_A) || (action_type == SOC_PPC_FP_ACTION_TYPE_SNP)) {
                
                res = arad_pmf_db_fes_set_unsafe(
                        unit,
                        prog_id,
                        new_fes_id-1,
                        &invalid_next_FES_info
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                
                res = arad_pmf_db_fes_set_unsafe(
                        unit,
                        prog_id,
                        new_fes_id,
                        &my_fes_info
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
                sal_memset(&invalid_next_FES_info, 0, sizeof(ARAD_PMF_FES_INPUT_INFO));
            } else {
                                
                sal_memcpy(&invalid_next_FES_info, &my_fes_info, sizeof(ARAD_PMF_FES_INPUT_INFO));
                my_fes_info.action_type = SOC_PPC_FP_ACTION_TYPE_NOP;
                res = arad_pmf_db_fes_set_unsafe(
                        unit,
                        prog_id,
                        new_fes_id,
                        &my_fes_info
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

            }
        }

        sal_memset(&fes_sw_db_info, 0, sizeof(fes_sw_db_info));
        fes_sw_db_info.action_type = action_type;
        fes_sw_db_info.db_id = db_id;
        fes_sw_db_info.entry_id = entry_id;
        fes_sw_db_info.is_used = 1;
        fes_sw_db_info.is_combined = (flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED) ? 1 : 0;        

        

        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.set(
                    unit,
                    stage,
                    prog_id,
                    new_fes_id,
                    &fes_sw_db_info
                  );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc_fes()", 0, 0);
}


uint32
  arad_pp_fp_action_alloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  )
{
    uint32 entry_id = -1;

    return arad_pp_fp_action_alloc_in_prog_with_entry(
                unit,
                db_id,
                prog_id,
                entry_id,
                flags,
                action_types,
                priority,
                constraint,
                action_alloced);

}

uint32 arad_pp_fp_action_alloc_feses(
    SOC_SAND_IN    int                            unit,
    SOC_SAND_IN    uint32                         db_id,
    SOC_SAND_IN    uint32                         prog_id,
    SOC_SAND_IN    uint32                         entry_id,
    SOC_SAND_IN    uint32                         flags[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
    SOC_SAND_IN    SOC_PPC_FP_ACTION_TYPE         action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], 
    SOC_SAND_IN    uint32                         action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
    SOC_SAND_IN    uint32                         action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
    SOC_SAND_IN    uint32                         priority,
    SOC_SAND_IN    ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_INOUT ARAD_PMF_FES                   fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT   uint8                          *action_alloced
  )
{
  uint8
      fes_alloced = 0;
  uint32
      action_indx,
      res = SOC_SAND_OK;

  
  uint32
      required_nof_feses = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
      if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(action_types[action_indx])) {
          
          continue;
      }
      required_nof_feses++ ;
  }

  
  for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
      if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(action_types[action_indx])) {
          
          continue;
      }

      res = arad_pp_fp_action_alloc_fes(
                unit,
                db_id,
                prog_id,
                entry_id,
                action_types[action_indx],
                flags[action_indx],
                priority,
                constraint,
                action_lsbs[action_indx],
                action_lengths[action_indx], 
                required_nof_feses,
                fes_info,
                &fes_alloced
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20 + action_indx, exit);

      if(!fes_alloced) {
          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "    "
                                "FES: fail to allocate for DB %d, program %d, action %s, flags %d, priority %d, constraint action size %d \n\r"),
                     db_id, prog_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_types[action_indx]), flags[action_indx], priority, constraint->action_size));
          *action_alloced = 0; 
          goto exit;
      }
  }
  
  
  *action_alloced = 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc_feses()", db_id, prog_id);
}
uint32
  arad_pp_fp_action_alloc_in_prog_with_entry(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       entry_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  )
{
  uint32
      action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      exp_action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      exp_action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      exp_action_flags[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      exp_action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX + 1],
      action_size,
      nof_actions,
      action_indx,
      exp_action_indx,
      fes_idx;
  ARAD_PMF_FES
      fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS];
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_STAGE
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
  
  SOC_PPC_FP_DATABASE_INFO               
        fp_db_info;
  SOC_SAND_SUCCESS_FAILURE  
      success;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for(fes_idx = 0; fes_idx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_idx) {
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(
                unit,
                stage,
                prog_id,
                fes_idx,
                &fes_info[fes_idx]
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

  
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            stage,
            db_id,
            &fp_db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit); 

  sal_memcpy(fp_db_info.action_types, action_types, sizeof(SOC_PPC_FP_ACTION_TYPE) * SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX);

  
  res = arad_pp_fp_action_to_lsbs(
          unit,
          stage,
          &fp_db_info,
          action_lsbs,
          action_lengths,
          &action_size,
          &nof_actions,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

   
  
  if((action_size > constraint->action_size) || (success != SOC_SAND_SUCCESS)) {
    LOG_ERROR(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "Unit %d, Program Id %d, Failed to allocate action in program.\n\r"
                          "Action size exceed the maximal size in the constraint.\n\r"
                          "Action size %d, Maximal size in the constraint %d.\n\r"), unit, prog_id, action_size, constraint->action_size));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 80, exit); 
  }

  
  exp_action_indx = 0;
  for (action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
      
      if (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER) {
          

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT;
          exp_action_lengths[exp_action_indx] = 0;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx];
          exp_action_flags[exp_action_indx] = flags | ARAD_PP_FP_FEM_ALLOC_FES_POLARITY | ARAD_PP_FP_FEM_ALLOC_FES_COMBINED;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_COUNTER_A;
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx] - 2;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx] + 2;
          exp_action_flags[exp_action_indx] = flags | ARAD_PP_FP_FEM_ALLOC_FES_ALWAYS_VALID | ARAD_PP_FP_FEM_ALLOC_FES_COMBINED;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_METER_A;
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx] - 1;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx] + 1;
          exp_action_flags[exp_action_indx] = flags;
          exp_action_indx++;
      } else if (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP) {
          

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT;
          exp_action_lengths[exp_action_indx] = 0;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx];
          exp_action_flags[exp_action_indx] = flags | ARAD_PP_FP_FEM_ALLOC_FES_POLARITY | ARAD_PP_FP_FEM_ALLOC_FES_COMBINED;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_SNP;
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx] - 2;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx] + 2;
          exp_action_flags[exp_action_indx] = flags | ARAD_PP_FP_FEM_ALLOC_FES_ALWAYS_VALID | ARAD_PP_FP_FEM_ALLOC_FES_COMBINED;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED;
          
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx] - 2;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx] + 1;
          exp_action_flags[exp_action_indx] = flags;
          exp_action_indx++;
      } else {

          exp_action_types[exp_action_indx] = action_types[action_indx];
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx];
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx];
          exp_action_flags[exp_action_indx] = flags;
          exp_action_indx++;
      }
  }
  if (exp_action_indx > SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
      LOG_ERROR(BSL_LS_SOC_FP,
          (BSL_META_U(unit,
                  "Unit %d, Program Id %d, Failed to allocate action in program.\n\r"
                  "Number of actions %d exceeds number of allowed actions per program (%d).\n\r"),
                  unit, prog_id, exp_action_indx, SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ACTION_FOMAT_ID_OUT_OF_RANGE_ERR, 90, exit);
  }
  exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID;

  res = arad_pp_fp_action_alloc_feses(
        unit,
        db_id,
        prog_id,
        entry_id,
        exp_action_flags,
        exp_action_types,
        exp_action_lsbs,
        exp_action_lengths,
        priority,
        constraint,
        fes_info,
        action_alloced
      );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc_in_prog()", db_id, prog_id);
}


uint32
  arad_pp_fp_action_alloc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  uint32                       selected_cycle[ARAD_PMF_LOW_LEVEL_NOF_PROGS_ALL_STAGES],
    SOC_SAND_INOUT  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  )
{
  uint32   
      cur_prog;
  uint32
      exist_progs[1],
      prog_result;
  uint8
      action_alloced_in_prog = 0;
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_STAGE
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            stage,
            db_id,
            0,
            exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  
  

  
  cur_prog = 0;
  ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,ARAD_PMF_LOW_LEVEL_NOF_PROGS,FALSE,prog_result);
  while(prog_result != 0) {
      constraint->cycle = selected_cycle[cur_prog];
      res = arad_pp_fp_action_alloc_in_prog(
                unit,
                db_id,
                cur_prog,
                flags,
                action_types,
                priority,
                constraint,
                &action_alloced_in_prog
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if(!action_alloced_in_prog) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "    "
                              "FES: fail to allocate for DB %d, program %d, flags %d, priority %d, constraint action size %d \n\r"),
                   db_id, cur_prog, flags, priority, constraint->action_size));
          *action_alloced = 0;  
          goto exit; 
      }
      
      ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS-cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,TRUE,prog_result);
  }

  *action_alloced = 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc()", 0, 0);
}



uint32
  arad_pp_fp_action_dealloc_fes(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_type 
  )
{
    uint32
        new_fes_id = 0;
    int32
        indx = 0;
    uint8
        found;
    ARAD_PMF_FES_INPUT_INFO
        my_fes_info;
    ARAD_PMF_FEM_INPUT_INFO
        fes_input_info;
    ARAD_PMF_FES
        fes_sw_db_info;
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_FP_DATABASE_STAGE
        stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   "
                          "De-allocate FES in prog:%d,  db_id:%d, action_type:%s \n\r"), prog_id, db_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_type)));

    found = 0;
    
    for(indx = 0; indx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++indx) {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(
                    unit,
                    stage,
                    prog_id,
                    indx,
                    &fes_sw_db_info
                  );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);


        if(fes_sw_db_info.is_used
           && (fes_sw_db_info.db_id == db_id)
           && ((fes_sw_db_info.action_type == action_type)
           
           || ((fes_sw_db_info.action_type == SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED) && (action_type == SOC_PPC_FP_ACTION_TYPE_TRAP))
           || ((fes_sw_db_info.action_type == SOC_PPC_FP_ACTION_TYPE_TRAP) && (action_type == SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED)))) {
            new_fes_id = indx;
            found = 1;
            break;
        }
    }


    if(found == 0) {
      LOG_ERROR(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "   "
                            "FES deallocation failed:%d  - no FES with this action %s for DB %d and PMF-Program %d \n\r"), 
                 found, SOC_PPC_FP_ACTION_TYPE_to_string(action_type), db_id, prog_id));
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_ENTRY_ACTION_TYPE_NOT_IN_DB_ERR, 7, exit); 
    }

    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   "
                          "Commit changes to HW: "
                          "De-allocate FES %d action :%s, "), new_fes_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_type)));

    ARAD_PMF_FES_INPUT_INFO_clear(&my_fes_info);
    my_fes_info.action_type = SOC_PPC_FP_ACTION_TYPE_NOP;
    res = arad_pmf_db_fes_set_unsafe(
            unit,
            prog_id,
            new_fes_id,
            &my_fes_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ARAD_PMF_FEM_INPUT_INFO_clear(&fes_input_info);
    fes_input_info.src_arad.is_nop = 1;

    res = arad_pmf_db_fem_input_set_unsafe(
            unit,
            prog_id,
            TRUE, 
            new_fes_id,
            &fes_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    fes_sw_db_info.is_used = 0;
    fes_sw_db_info.db_id = 0;
    fes_sw_db_info.action_type = SOC_PPC_FP_ACTION_TYPE_INVALID;
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.set(
                unit,
                stage,
                prog_id,
                new_fes_id,
                &fes_sw_db_info
              );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);


    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_dealloc_fes()", 0, 0);
}




uint32
  arad_pp_fp_action_dealloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX] 
  )
{
  uint32
      action_indx;
  uint32
      exp_action_indx = 0;
  uint32
      exp_action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX + 1];
  uint32
    res = SOC_SAND_OK;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
      if (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER) {
          

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_COUNTER_A;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_METER_A;
          exp_action_indx++;
      } else if (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP) {
          

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_SNP;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED;
          exp_action_indx++;
      } else {

          exp_action_types[exp_action_indx] = action_types[action_indx];
          exp_action_indx++;
      }
  }

  if (exp_action_indx > SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
      LOG_ERROR(BSL_LS_SOC_FP,
          (BSL_META_U(unit,
                  "Unit %d, Program Id %d\n\r"
                  "Number of actions %d exceeds number of allowed actions per program (%d).\n\r"
                  "This is an errornous state, since allocation stage should make sure this never happens\n\r"),
                  unit, prog_id, exp_action_indx, SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_FEM_NDX_OUT_OF_RANGE_ERR, 101, exit);
  }
  exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID;

  
  for(exp_action_indx = 0; exp_action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && exp_action_types[exp_action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++exp_action_indx) {
      if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(exp_action_types[exp_action_indx])) {
          
          continue;
      }

      
      res = arad_pp_fp_action_dealloc_fes(
                unit,
                db_id,
                prog_id,
                exp_action_types[exp_action_indx]
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20 + exp_action_indx, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_dealloc_in_prog()", db_id, prog_id);
}


uint32
  arad_pp_fp_action_dealloc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX] 
  )
{
  uint32   
      cur_prog;
  uint32
      exist_progs;
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            db_id,
            0,
            &exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  
  

  
  for (cur_prog = 0; cur_prog < SOC_DPP_DEFS_GET(unit, nof_ingress_pmf_programs); cur_prog++) {
      if ((1 << cur_prog) & exist_progs) {
          res = arad_pp_fp_action_dealloc_in_prog(
                    unit,
                    db_id,
                    cur_prog,
                    action_types
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_dealloc()", 0, 0);
}





uint32
  arad_pp_fp_fem_db_actions_get(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_OUT  uint32                   action_lsbs[2]
  )
{
   return 0;
   
}


int action_sort_to_length(action_inf *action_array, uint32 n, uint32 sum, int *seq, int *idx, int *min_diff)
{
	if (n < 1) {
		
		return 0;
	}
	
    if ((action_array[n-1].size+1) == sum) { 
		
		seq[*idx] = n-1;
		*idx += 1;
		return 1;
	}
	else {
		if (n == 1) return 0; 
		
		if (action_sort_to_length(action_array, n-1, sum, seq, idx, min_diff)) {
			
			return 1;
		}
		else if (sum > (action_array[n-1].size+1)) {
		    if ((*min_diff == 0) || (*min_diff >  (sum-(action_array[n-1].size+1)))) {
				*min_diff = sum-(action_array[n-1].size+1);
			}

			if (action_sort_to_length(action_array, n-1, sum-(action_array[n-1].size+1), seq, idx, min_diff)) {
                
				seq[*idx] = n-1;
        		*idx += 1;
				return 1;
			}
		}
	}
	
	return 0;
}



uint32
  arad_pp_fp_action_to_lsbs(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE stage,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO  *info,
      SOC_SAND_OUT uint32                    action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      SOC_SAND_OUT uint32                    action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      SOC_SAND_OUT ARAD_TCAM_ACTION_SIZE     *action_size,
      SOC_SAND_OUT uint32                    *nof_actions,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  )
{
    int32
        action_indx = 0;
    uint32
        action_lsb_ingress_tcam = 0,
      action_lsb_egress = 0,
        max_zone_length = 0,
        nof_bits, 
        action_len_complete,
      action_len,
        zone_length[2] = {0, 0}, 
      res = SOC_SAND_OK;
    uint8 
      action_change_key_loop;
    int algorithm_indx = 0;
    int i = 0;
    int sequence[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX] = { 0 }; 
	int idx = 0;
	int min_diff = 0;
    action_inf act;    
    action_inf action_array[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    uint32 action_lsbs_tmp[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1];
    uint32 width_dft, width, move_index;
    uint32 loc_hw_id;
    uint8 resort=0;
    uint8 action_len_total;
    uint8 use_kaps = (info->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS) ? TRUE : FALSE;
    SOC_PPC_FP_DATABASE_TYPE db_type = info->db_type;
    uint32 *action_widths = (uint32 *)info->action_widths;
    SOC_PPC_FP_ACTION_TYPE *action_types = (SOC_PPC_FP_ACTION_TYPE *)info->action_types;
    SOC_PPC_FP_ACTION_TYPE* non_const_action_types = (SOC_PPC_FP_ACTION_TYPE*) action_types;
   

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    action_len_total = 0;
    for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID); ++action_indx) {
        if (db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE) {
            res = arad_pmf_db_fes_action_size_get_unsafe(
                    unit,
                    action_types[action_indx],
                    stage,
                    &action_len,
                    &action_lsb_egress
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
            action_len_total += action_len;
        }
    }

    
    
    for (algorithm_indx = 0; algorithm_indx < 2; algorithm_indx++) {

        
        if (algorithm_indx == 1) {
            sal_memset(action_lsbs, 0x0, sizeof(uint32) * (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1));
            sal_memset(action_lengths, 0x0, sizeof(uint32) * (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1));
            if (action_sort_to_length(action_array, *nof_actions, max_zone_length, sequence, &idx, &min_diff) == 0) {
                
				action_sort_to_length(action_array, *nof_actions, max_zone_length-min_diff, sequence, &idx, &min_diff);
            }

            
            for(i = 0; i < idx; i++) {
                act = action_array[i];
                action_array[i] = action_array[sequence[i]];
                action_array[sequence[i]] = act;
            }
                
            for (action_indx = 0;  action_indx < *nof_actions ; action_indx++)
                non_const_action_types[action_indx]= action_array[action_indx].indx;
            action_lsb_ingress_tcam = 0;
            zone_length[0] = 0;
            zone_length[1] = 0;

        }


        nof_bits = 0;
        *nof_actions = 0;
        *success = SOC_SAND_SUCCESS;

    
    action_lsbs[0] = 0;
    action_lengths[0] = 0;
    max_zone_length = (use_kaps) ? ARAD_PP_FP_KAPS_RESULT_ACTION_WIDTH : ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE;
    
    for (action_change_key_loop = 0; action_change_key_loop <= 1; action_change_key_loop++) {
        if ((stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) && (action_change_key_loop == 0)) {
            
            continue;
        }
        for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID); ++action_indx) {
            if ((action_change_key_loop == 0) && (action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY)) {
                continue;
            }
            else if ((action_change_key_loop == 1) && (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY)) {
                continue;
            }

                
                if ((SOC_IS_JERICHO_PLUS(unit) ||
                    action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1 ||
                    action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2) &&
                    action_widths != NULL && action_widths[action_indx] > 0) {

                    action_len = action_widths[action_indx];
                } else {
                    res = arad_pmf_db_fes_action_size_get_unsafe(
                        unit,
                        action_types[action_indx],
                        stage,
                        &action_len,
                        &action_lsb_egress
                        );
                }
                action_array[action_indx].size=action_len;
                action_array[action_indx].indx=action_types[action_indx];

                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
                if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(action_types[action_indx])) {
                    action_lsbs[action_indx] = 0;
                    action_lsb_ingress_tcam = 0;
                    action_len_complete = action_len; 
                } else if (stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
                    action_len_complete = action_len; 
                    action_lsbs[action_indx] = action_lsb_egress; 
                } else {
                    
                    if (((action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) || (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2)
                         || (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_LEARN_SA_16_TO_47))
                        && (action_len == 32)) {
                        
                        action_len = 31;
                    }
                    
                    if (ARAD_PP_FP_FEM_IS_ALWAYS_VALID_LARGE_DIRECT(action_types[action_indx]) && (action_len == 24)) {
                        action_len = 23;
                    }
                    
                    action_len_complete = action_len + 1;                     
                    action_lsb_ingress_tcam = ARAD_PP_FP_FEM_ACTION_LOCAL_LSB(use_kaps,zone_length[0], action_len_complete);
                    if ((zone_length[0] < max_zone_length )
                        && (action_lsb_ingress_tcam + action_len_complete <= max_zone_length)) {
                        action_lsbs[action_indx] = action_lsb_ingress_tcam; 
                    } else {
                        action_lsb_ingress_tcam = ARAD_PP_FP_FEM_ACTION_LOCAL_LSB(use_kaps,zone_length[1], action_len_complete);
                        if (zone_length[1] >= max_zone_length && action_len_complete > 0) {
                           LOG_ERROR(BSL_LS_SOC_FP,
                                      (BSL_META_U(unit,
                                                  "    "
                                                  "Error in action computation: "
                                                  "Not enough resources, try to use less actions\n\r")
                                       ));

                            *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
                            ARAD_DO_NOTHING_AND_EXIT;

                        }
                        action_lsbs[action_indx] = max_zone_length + action_lsb_ingress_tcam; 
                    }
                }
                
                if (action_lsbs[action_indx] < max_zone_length) {
                    zone_length[0] = action_len_complete + action_lsb_ingress_tcam;
                } else {
                    zone_length[1] = action_len_complete + action_lsb_ingress_tcam;
                }
                nof_bits = SOC_SAND_MAX(nof_bits, action_lsbs[action_indx] + action_len_complete);
                
                action_lengths[action_indx] = action_len_complete;
                *nof_actions += 1;
            }
        }

        
        action_lsbs[action_indx] = nof_bits;
        if (use_kaps) {
            
            if (nof_bits <= SOC_DPP_IMP_DEFS_GET(unit, kaps_dma_zone_width)) { 
                
                *action_size = BITS2BYTES(SOC_DPP_IMP_DEFS_GET(unit, kaps_dma_zone_width));
                break;
            } else { 
                
                *action_size = 2 * BITS2BYTES(SOC_DPP_IMP_DEFS_GET(unit, kaps_dma_zone_width));
                break;
            }
        }
        else if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) || (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS)) {
            if (nof_bits <= SOC_DPP_DEFS_GET(unit, tcam_action_width)) {
                *action_size = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
                break; 
            } else if (nof_bits <= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                *action_size = ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
                break; 
            } else if (nof_bits <= 3 * SOC_DPP_DEFS_GET(unit, tcam_action_width)) {
                *action_size = ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS;
                break;  
            } else if (nof_bits <= 4 * SOC_DPP_DEFS_GET(unit, tcam_action_width)) {
                *action_size = ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS;
                break;  
            } else if (algorithm_indx==1){
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "    "
                                      "Error in action computation: "
                                      "For stage %s, total action size %d bits \n\r"),
                           SOC_PPC_FP_DATABASE_STAGE_to_string(stage), nof_bits));
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "\n\r")));

                *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
                ARAD_DO_NOTHING_AND_EXIT;
            }
        } else {
            break;
        }

    }

    
    if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)
        && (db_type != SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION)
        && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "reduced_action_support", 0)) {        

        
        sal_memcpy(action_lsbs_tmp, action_lsbs, (sizeof(uint32)*(*nof_actions)));
        for (action_indx = 0; action_indx < *nof_actions; action_indx++) {
            res = arad_pmf_fem_action_width_default_get(unit, action_types[action_indx], &width_dft);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            res = arad_pmf_fem_action_width_get_unsafe(unit, action_types[action_indx],&width, &loc_hw_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            if ((width_dft > width) && (action_indx != (*nof_actions - 1))) {
                resort=1;
                move_index = action_indx;
                break;
            }
        }

        if (resort == 1) {
            
            if (action_lsbs_tmp[move_index] < ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                for (action_indx = (move_index+1); action_indx < *nof_actions; action_indx++) {
                    
                    if (action_lsbs_tmp[action_indx] < ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                        action_lsbs[action_indx] = action_lsbs_tmp[action_indx] - action_lengths[move_index];
                        action_lsbs[move_index] += action_lengths[action_indx];
                    }
                }
            } else {
                for (action_indx = (move_index+1); action_indx < *nof_actions; action_indx++) {
                    
                    if (action_lsbs_tmp[action_indx] >= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                        action_lsbs[action_indx] = action_lsbs_tmp[action_indx] - action_lengths[move_index];
                        action_lsbs[move_index] += action_lengths[action_indx];
                    }
                }
            }
        }        
    } 

    
    if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        *action_size = ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_to_lsbs()", action_indx, 0);
}




uint32
  arad_pp_fp_action_value_buffer_mapping(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_IN  ARAD_PP_FP_ACTION_BUFFER_DIRECTION direction,
      SOC_SAND_INOUT SOC_PPC_FP_ACTION_VAL   action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
      SOC_SAND_INOUT uint32                  buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE],
      SOC_SAND_OUT   uint32                  *buffer_size
  )
{
    uint32
        action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
        action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
        nof_actions,
        action_val_indx,
        action_val_indx2,
        action_indx,
        is_valid_bit,
        len,
        ext_count = 0,
        ext_index;
    uint8
        action_found;        
    uint32
      res = SOC_SAND_OK;
    uint32 action_val_16_msb_for_flp_0[3] = {0};

    SOC_PPC_FP_ACTION_VAL   
        action_vals_lcl[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    SOC_PPC_FP_DATABASE_STAGE
        stage; 
    ARAD_TCAM_ACTION_SIZE  
        action_size;
    
    SOC_PPC_FP_DATABASE_INFO               
        fp_db_info;
    SOC_SAND_SUCCESS_FAILURE  
        success;
    uint8 is_vt_match_ipv4 = FALSE;
    uint8 is_fp_db_extended_fwrd = FALSE;
    uint8 is_fp_flp_match = FALSE;
    uint8 is_kaps_direct_action_always_valid=FALSE;
    uint8 check_condition=FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pp_fp_db_stage_get(
              unit,
              db_id,
              &stage
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            stage,
            db_id,
            &fp_db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit); 

    if ((fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_DBAL)){        
        if (fp_db_info.action_types[0]== SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) {
            is_fp_flp_match = TRUE;
        } else {
			is_vt_match_ipv4 = TRUE;
		}
    }

#if defined(INCLUDE_KBP)
    if (fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_EXTENDED_DATABASES  && (fp_db_info.internal_table_id != ARAD_KBP_FRWRD_TBL_ID_IPV4_MC)){
         is_fp_db_extended_fwrd = TRUE;
    }
#endif 

    if ( is_vt_match_ipv4 ) 
    {
        action_lsbs[0] = 0;
        action_lengths[0] = 16 ; 
        action_size = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
        nof_actions = 1 ; 
        fp_db_info.action_types[0] = SOC_PPC_FP_ACTION_TYPE_IN_LIF;
    }else if ( is_fp_flp_match ) 
    {
        action_lsbs[0] = 0;
        action_lengths[0] = 19 ;         

        action_lsbs[1] = 32;
        action_lengths[1] = 32 ;
        fp_db_info.action_types[1] = SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2;

        nof_actions = 2 ; 

        if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
            buffer[0] = action_vals[0].val; 
            buffer[1] = action_vals[1].val;
            goto exit;
        }
    }
#if defined(INCLUDE_KBP)
    else if (is_fp_db_extended_fwrd) {
        action_lsbs[0] = 64;
        action_lengths[0] = 24 ;
        action_lsbs[1] = 32;
        action_lengths[1] = 32 ;
        action_lsbs[2] = 0;
        action_lengths[2] = 32 ;
        action_lsbs[3] = 88;
        action_lengths[3] = 32 ;
        nof_actions = 4;
    }
#endif 
    else
    {
      
        res = arad_pp_fp_action_to_lsbs(
                unit,
                stage,
                &fp_db_info,
                action_lsbs,
                action_lengths,
                &action_size,
                &nof_actions,
                &success
              );
        if (buffer_size != NULL) {
            *buffer_size = action_size;
        }
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        if (success != SOC_SAND_SUCCESS) {
          LOG_ERROR(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "Unit %d DB-Id %d, Invalid action composition.\n\r"),
                     unit, db_id));
            SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 101, exit);
        }
    }

    
    if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) && (!is_fp_db_extended_fwrd)) {
        sal_memset(action_lsbs, 0x0, sizeof(uint32) * (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1));
    }

    
    for(action_val_indx = 0; action_val_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_val_indx) {
        SOC_PPC_FP_ACTION_VAL_clear(&action_vals_lcl[action_val_indx]);
        if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
            action_vals_lcl[action_val_indx].type = action_vals[action_val_indx].type;
            action_vals_lcl[action_val_indx].val = action_vals[action_val_indx].val;
        }
        else if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
            
            check_condition = ARAD_PP_FP_FEM_IS_ALWAYS_VALID_LARGE_DIRECT(fp_db_info.action_types[action_val_indx]) ? TRUE : FALSE;
            is_kaps_direct_action_always_valid = ((fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS) 
                && SOC_IS_JERICHO_PLUS(unit)
                && (fp_db_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE)
                && ((fp_db_info.action_types[action_val_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) 
                    || (fp_db_info.action_types[action_val_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2)
                    || check_condition)) ? TRUE : FALSE;

            is_valid_bit = ((stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) 
                            || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) 
                            || (is_vt_match_ipv4 )
                            || (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(fp_db_info.action_types[action_val_indx])) 
                            || (fp_db_info.action_types[action_val_indx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER)
                            || (fp_db_info.action_types[action_val_indx] == SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP)
                            || is_kaps_direct_action_always_valid)? 0: 1;

            
            if ((action_val_indx < nof_actions) && (SHR_BITGET(buffer,action_lsbs[action_val_indx]) || (!is_valid_bit))) {
                action_vals_lcl[action_val_indx].type = fp_db_info.action_types[action_val_indx];
                len = action_lengths[action_val_indx];
                SHR_BITCOPY_RANGE(&(action_vals_lcl[action_val_indx].val), 0, buffer, action_lsbs[action_val_indx]+is_valid_bit, len-is_valid_bit); 
                if(SOC_IS_JERICHO(unit)){
                    if ((action_vals_lcl[action_val_indx].type >= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) &&
                        (action_vals_lcl[action_val_indx].type <= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7)) {
                        ext_count = SOC_PPC_BIT_TO_U32(len);

                        for (ext_index = 1; ext_index < ext_count; ext_index++) {
                            if (ext_index == (ext_count -1)) {
                                SHR_BITCOPY_RANGE(&(action_val_16_msb_for_flp_0[ext_index - 1]), (ext_index + 1) * 32 - len, buffer, action_lsbs[action_val_indx] + ext_index * 32, (len - ext_index * 32));
                            } else {
                                SHR_BITCOPY_RANGE(&(action_val_16_msb_for_flp_0[ext_index - 1]), 0, buffer, action_lsbs[action_val_indx] + ext_index * 32, 32);
                            }
                        }
                    }
                }else{
                    if (action_vals_lcl[action_val_indx].type == SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) {
                        
                        SHR_BITCOPY_RANGE(&(action_val_16_msb_for_flp_0[0]), 0, buffer, action_lsbs[action_val_indx]+32, 16); 
                    }
                }
            }
            else
            {
                action_vals_lcl[action_val_indx].type = SOC_PPC_FP_ACTION_TYPE_INVALID;
            }
        }
    }

    
    action_val_indx2 = 0; 
    for(action_val_indx = 0; action_val_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_val_indx) {
        if (action_vals_lcl[action_val_indx].type == SOC_PPC_FP_ACTION_TYPE_INVALID) {
            if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
                
                break;
            }
            else if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
                
                action_vals[action_val_indx2].type = SOC_PPC_FP_ACTION_TYPE_INVALID;
                action_vals[action_val_indx2].val = 0;
                continue;
            }
        }

        action_found = 0;
        for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && fp_db_info.action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
            
            if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
                action_indx = action_val_indx;
            }

            if(action_vals_lcl[action_val_indx].type == fp_db_info.action_types[action_indx]) { 
                if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
                    
                    check_condition = ARAD_PP_FP_FEM_IS_ALWAYS_VALID_LARGE_DIRECT(fp_db_info.action_types[action_indx]) ? TRUE : FALSE;

                    is_kaps_direct_action_always_valid = ((fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS)
                        && SOC_IS_JERICHO_PLUS(unit)
                        && (fp_db_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE)
                        && ((fp_db_info.action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) 
                            || (fp_db_info.action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2)
                            || check_condition)) ? TRUE : FALSE;

                    is_valid_bit = ((stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)
                                    || (is_vt_match_ipv4 )
                                    || (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(action_vals_lcl[action_val_indx].type)) 
                                    || (fp_db_info.action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER)
                                    || (fp_db_info.action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP)
                                    || is_kaps_direct_action_always_valid)? 0: 1;
                    len = action_lengths[action_indx];
                    if (is_valid_bit) {
                        SHR_BITSET(buffer, action_lsbs[action_indx]); 
                    }
                    
                    SHR_BITCOPY_RANGE(buffer, action_lsbs[action_indx]+is_valid_bit,&(action_vals_lcl[action_val_indx].val), 0, len-is_valid_bit); 
                    if (action_val_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX - 1) {
                        if (SOC_IS_JERICHO(unit)){
                            if ((action_vals_lcl[action_val_indx].type >= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) &&
                                (action_vals_lcl[action_val_indx].type <= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7)) {
                                ext_count = SOC_PPC_BIT_TO_U32(len);
                                for (ext_index = 1; ext_index < ext_count; ext_index++) {
                                    if (action_vals_lcl[action_val_indx + ext_index].type != action_vals_lcl[action_val_indx].type) {
                                        break;
                                    }

                                    if (ext_index == (ext_count -1)) {
                                        SHR_BITCOPY_RANGE(buffer, action_lsbs[action_indx] + ext_index * 32, &(action_vals_lcl[action_val_indx + ext_index].val), ((ext_index + 1) * 32 - len), (len - ext_index * 32));
                                        action_val_indx += ext_index;
                                    } else {
                                        SHR_BITCOPY_RANGE(buffer, action_lsbs[action_indx] + ext_index * 32, &(action_vals_lcl[action_val_indx + ext_index].val), 0, 32);
                                    }
                                }
                            }
                        }else{
                            if ((action_vals_lcl[action_val_indx].type == SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) && (action_vals_lcl[action_val_indx+1].type == SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0)) {
                                SHR_BITCOPY_RANGE(buffer, action_lsbs[action_indx]+32, &(action_vals_lcl[action_val_indx+1].val), 16, 16); 
                                action_val_indx++;
                            }
                        }
                    }
                }
                else if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
                    action_vals[action_val_indx2].type = action_vals_lcl[action_val_indx].type;
                    action_vals[action_val_indx2].val = action_vals_lcl[action_val_indx].val;
                    if(SOC_IS_JERICHO(unit)){
                        if ((action_vals[action_val_indx2].type >= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) &&
                            (action_vals[action_val_indx2].type <= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7)) {
                            for (ext_index = 1; ext_index < ext_count; ext_index++) {
                                if (action_val_16_msb_for_flp_0[ext_index-1] != 0) {
                                    action_val_indx2++;
                                    action_vals[action_val_indx2].val = action_val_16_msb_for_flp_0[ext_index - 1];
                                    action_vals[action_val_indx2].type = action_vals_lcl[action_val_indx].type;
                                }
                            }
                        }
                    }else{
                        if (action_vals[action_val_indx2].type == SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) {
                            action_vals[action_val_indx2+1].val = action_val_16_msb_for_flp_0[0];
                            action_vals[action_val_indx2+1].type = action_vals_lcl[action_val_indx].type;
                            action_val_indx2++; 
                        }
                    }
                }
                LOG_DEBUG(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "   "
                                      "Action Value to buffer: %d, "
                                      "Action type:%s, "
                                      "Action val:%d \n\r"), direction, 
                           SOC_PPC_FP_ACTION_TYPE_to_string(action_vals_lcl[action_val_indx].type), 
                           action_vals_lcl[action_val_indx].val));
                action_val_indx2 ++;
                action_found = 1;
                break;
            }
        }
        
        if(!action_found) {
          LOG_ERROR(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "Unit %d DB id %d Action Type %s : action Value type does not exist in DB action.\n\r"),
                     unit, db_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_vals_lcl[action_val_indx].type)));
            SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 101, exit);
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_value_buffer_mapping()", db_id, 0);
}




uint32
  arad_pp_fp_action_value_to_buffer(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_VAL     action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_OUT  uint32                   buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE],
      SOC_SAND_OUT  uint32                   *buffer_size
  )
{
    uint32
        fld_val,
        key_bitmap_constraint_cascaded,
        action_indx,
        action_size,
        action_lsb_egress,
      res = SOC_SAND_OK;
    SOC_PPC_FP_ACTION_VAL     
        action_vals_lcl[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    SOC_PPC_FP_DATABASE_STAGE
        stage; 
    SOC_PPC_FP_DATABASE_INFO
      fp_database_info; 
    uint8
        egress_action_ndx,
        key_ndx,
        is_key_fixed;
    SOC_PPC_FP_ACTION_TYPE
        action_egress_disabled[] = {SOC_PPC_FP_ACTION_TYPE_ACE_POINTER, SOC_PPC_FP_ACTION_TYPE_EGR_OFP};
    uint8 is_vt_match_ipv4 = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    sal_memset(buffer,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE);
    sal_memcpy(action_vals_lcl, action_vals, SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX * sizeof(SOC_PPC_FP_ACTION_VAL));

    
    res = arad_pp_fp_db_stage_get(
              unit,
              db_id,
              &stage
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    
    if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        for (egress_action_ndx = 0; egress_action_ndx < sizeof(action_egress_disabled) / sizeof(SOC_PPC_FP_ACTION_TYPE); egress_action_ndx++) {
            
            res = arad_pmf_db_fes_action_size_get_unsafe(
                    unit,
                    action_egress_disabled[egress_action_ndx],
                    SOC_PPC_FP_DATABASE_STAGE_EGRESS,
                    &action_size,
                    &action_lsb_egress
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            if (SOC_IS_JERICHO(unit) && (action_egress_disabled[egress_action_ndx] == SOC_PPC_FP_ACTION_TYPE_EGR_OFP)) {
                fld_val = (((1 << action_size) - 1) & ~1);
            }
            else {
                fld_val = (1 << action_size) - 1;  
            }
            SHR_BITCOPY_RANGE(buffer, action_lsb_egress, &fld_val, 0, action_size); 
        }
#if (0)

        
        if (SOC_IS_JERICHO(unit))
        {
            res = arad_pmf_db_fes_action_size_get_unsafe(
                    unit,
                    SOC_PPC_FP_ACTION_TYPE_ACE_TYPE,
                    SOC_PPC_FP_DATABASE_STAGE_EGRESS,
                    &action_size,
                    &action_lsb_egress
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            fld_val = 0x1; 
            SHR_BITCOPY_RANGE(buffer, action_lsb_egress, &fld_val, 0, action_size); 
        }

#endif
    }
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
                 unit,
                 stage,
                 db_id,
                 &fp_database_info
        );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        if (fp_database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_DBAL )
        {
             is_vt_match_ipv4 = TRUE;
        }
        if (!is_vt_match_ipv4) {
            for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (fp_database_info.action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID); ++action_indx) {
                if ( (action_vals_lcl[action_indx].type == SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY) &&
                     !((fp_database_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE && fp_database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS ) ) ) {
                     

                    res = arad_pp_fp_key_alloc_key_cascaded_key_get(
                            unit,
                            fp_database_info.cascaded_coupled_db_id,
                            &is_key_fixed,
                            &key_ndx,
                            &key_bitmap_constraint_cascaded
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
                    if (!is_key_fixed) {
                      LOG_ERROR(BSL_LS_SOC_FP,
                                (BSL_META_U(unit,
                                            "   Error in entry add: "
                                            "For database %d, stage %s, the database is indicated to be cascaded with DB-Id %d."
                                            "This latest Database is not set as cascaded. \n\r"),
                                 db_id, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), fp_database_info.cascaded_coupled_db_id));
                        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 50, exit); 
                    }

                    
                    
                    if (!SOC_IS_JERICHO_PLUS(unit)) {
                        action_vals_lcl[action_indx].val = action_vals_lcl[action_indx].val << 4;
                        action_vals_lcl[action_indx].val |= key_ndx << 2;
                        action_vals_lcl[action_indx].val |= 2; 
                    }
                }
            }
        }
    }

    res = arad_pp_fp_action_value_buffer_mapping(
            unit,
            db_id,
            ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER,
            action_vals_lcl,
            buffer,
            buffer_size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_value_to_buffer()", db_id, 0);
}


uint32
  arad_pp_fp_action_buffer_to_value(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_IN  uint32                   buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE],
      SOC_SAND_OUT  SOC_PPC_FP_ACTION_VAL     action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX]
  )
{
    uint32
        action_indx,
      res = SOC_SAND_OK;
    uint32                   
        buffer_lcl[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE];
    SOC_PPC_FP_DATABASE_STAGE
        stage; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pp_fp_db_stage_get(
              unit,
              db_id,
              &stage
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    
    for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_indx) {
        SOC_PPC_FP_ACTION_VAL_clear(&action_vals[action_indx]);
    }
    sal_memcpy(buffer_lcl, buffer, ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE * sizeof(uint32));

    res = arad_pp_fp_action_value_buffer_mapping(
            unit,
            db_id,
            ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION,
            action_vals,
            buffer_lcl,
            NULL
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
        for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (action_vals[action_indx].type != SOC_PPC_FP_ACTION_TYPE_INVALID); ++action_indx) {
            if (action_vals[action_indx].type == SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY) {
               
               action_vals[action_indx].val = action_vals[action_indx].val >> 4;
           }
       }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_buffer_to_value()", db_id, 0);
}




uint32
  arad_pp_fp_action_type_max_size_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE fp_action_type,
    SOC_SAND_OUT uint32            *action_size_in_bits,
    SOC_SAND_OUT uint32            *action_size_in_bits_in_fem
  )
{
  uint32
      action_lsb_egress,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_ACTION_TYPE_MAX_SIZE_GET);

  res = arad_pmf_db_fes_action_size_get_unsafe(
            unit,
            fp_action_type,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, 
            action_size_in_bits,
            &action_lsb_egress
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *action_size_in_bits_in_fem = *action_size_in_bits;
  
  if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(fp_action_type))
  {
    *action_size_in_bits_in_fem = *action_size_in_bits + ARAD_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
  }

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_type_max_size_get()", 0, 0);
}


uint32
  arad_pp_fp_qual_lsb_and_length_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   db_id_ndx,
    SOC_SAND_IN  uint8                    used_kaps,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE     qual_type,
    SOC_SAND_OUT uint32                   *qual_lsb,
    SOC_SAND_OUT uint32                   *qual_length_no_padding
  )
{
  uint32
    qual_lsb_lcl = 0,
      exist_progs[1],
      db_prog,
      prog_result,
      qual_type_ndx,
      ce_indx,
    res;
  uint32
    qual_size_in_bits_no_padding = 0;
  SOC_PPC_FP_DATABASE_STAGE
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
  ARAD_PP_FP_KEY_DP_PROG_INFO
      db_prog_info;
  ARAD_PMF_CE
    sw_db_ce;
  
  SOC_PPC_FP_DATABASE_INFO
    database_info;
  ARAD_PMF_CE_IRPP_QUALIFIER_INFO
    qual_info;
  uint8
      is_equal = FALSE,
      is_found,
      is_slb_hash_in_quals = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_QUAL_LSB_AND_LENGTH_GET);

  
  SOC_PPC_FP_DATABASE_INFO_clear(&database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  
  if (SOC_IS_ARADPLUS(unit)) {
      if ((database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_IS_EQUAL_MSB)
          && (qual_type == SOC_PPC_FP_QUAL_IS_EQUAL))
      {
          is_equal = TRUE;
      }
  }

  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            stage,
            db_id_ndx,
            0,
            exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  db_prog = 0;
  ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,db_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,ARAD_PMF_LOW_LEVEL_NOF_PROGS,FALSE,prog_result);
  if(prog_result == 0) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 20, exit); 
  }

  
  res = arad_pp_fp_db_prog_info_get(
          unit,
          stage,
          db_id_ndx,
          db_prog,
          &db_prog_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_fp_is_qual_identical(unit, qual_type, SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, &is_slb_hash_in_quals);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 151, exit);

  is_found = FALSE;
  *qual_lsb = 0;
  *qual_length_no_padding = 0;

  if (SOC_IS_ARADPLUS(unit)
      && is_equal) 
  {
      
      *qual_lsb = ARAD_PP_FP_TCAM_KEY_SIZE - 5;
      *qual_length_no_padding = 4;
  }
  else if (is_slb_hash_in_quals == TRUE) 
  {
      if (SOC_IS_JERICHO_PLUS(unit)) {
          
          uint32 slb_hash_qual_lsb = 0;
          uint32 slb_hash_qual_nof_bits = 0;
          uint32 cascaded_key_qual_nof_bits = 0;

          for (qual_type_ndx = 0; qual_type_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_type_ndx++) {
              if (database_info.qual_types[qual_type_ndx] == qual_type) { 
                  res = arad_pmf_ce_internal_field_info_find(unit, qual_type, stage, 1, &is_found, &qual_info); 
                  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
                  if (!is_found) {
                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 46, exit);
                  }
                  slb_hash_qual_lsb = (db_prog_info.alloc_place == ARAD_PP_FP_KEY_CE_LOW) ? 0 : ARAD_PP_FP_TCAM_HALF_KEY_SIZE;
                  slb_hash_qual_nof_bits = qual_info.info.qual_nof_bits;
                  break;
              }
          } 

          if (qual_type_ndx >= SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
              LOG_ERROR(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "Unit %d DB id %d Program %d Cycle %d Qual Type %s was not found.\n\r"),
                         unit, db_id_ndx, db_prog, db_prog_info.cycle, SOC_PPC_FP_QUAL_TYPE_to_string(qual_type)));
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 40, exit);
          }
          
          for (qual_type_ndx = 0; qual_type_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_type_ndx++) { 
              res = arad_pp_fp_is_qual_identical(unit, database_info.qual_types[qual_type_ndx], SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED, &is_found);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 47, exit); 
              if (is_found) { 
                  res = arad_pmf_ce_internal_field_info_find(unit, database_info.qual_types[qual_type_ndx], stage, 1, &is_found, &qual_info); 
                  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit); 
                  if (!is_found) { 
                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 49, exit);
                  }
                  cascaded_key_qual_nof_bits = qual_info.info.qual_nof_bits;
                  break;
              }
          }

          *qual_lsb = slb_hash_qual_lsb + cascaded_key_qual_nof_bits;
          *qual_length_no_padding = slb_hash_qual_nof_bits;          
      } else {
          for (qual_type_ndx = 0; qual_type_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_type_ndx++) {
              if (database_info.qual_types[qual_type_ndx] == qual_type) {
                  break;
              }
          }

          if (qual_type_ndx >= SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
              LOG_ERROR(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "Unit %d DB id %d Program %d Cycle %d Qual Type %s was not found.\n\r"),
                         unit, db_id_ndx, db_prog, db_prog_info.cycle, SOC_PPC_FP_QUAL_TYPE_to_string(qual_type)));
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 40, exit);
          }

          res = arad_pmf_ce_internal_field_info_find(unit, qual_type, stage, 1, &is_found, &qual_info);
          SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

          if (is_found) {
              *qual_lsb = (db_prog_info.alloc_place == ARAD_PP_FP_KEY_CE_HIGH)?0:ARAD_PP_FP_TCAM_HALF_KEY_SIZE;
              *qual_length_no_padding = qual_info.info.qual_nof_bits;
          }
      }

  }
  else 
  {
      
          
          for(ce_indx = 0; ce_indx < db_prog_info.nof_ces; ++ce_indx) 
          {
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(
                  unit,
                  stage,
                  db_prog,
                  db_prog_info.cycle,
                  db_prog_info.ces[db_prog_info.nof_ces - ce_indx - 1], 
                  &sw_db_ce
                );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

            if (sw_db_ce.qual_type == qual_type)
            {
                
                if (!is_found) {
                    qual_lsb_lcl += sw_db_ce.lsb;
                }
                is_found = TRUE;
                if ((used_kaps == TRUE) && (ARAD_PP_FP_DIRECT_TABLE_KAPS_KEY_LENGTH > (SOC_IS_QUX(unit) ? SOC_DPP_DEFS_GET(unit, field_large_direct_lu_key_min_length) : 16))) {
                    qual_size_in_bits_no_padding += (sw_db_ce.msb - sw_db_ce.lsb + 1);
                    continue;
                } else {
                    qual_size_in_bits_no_padding = (sw_db_ce.msb - sw_db_ce.lsb + 1); 
                    break;
                }
            } else if (is_found == TRUE) {
                continue;
            }

            
            qual_lsb_lcl += (sw_db_ce.msb + 1);
          }
          if (!SOC_IS_JERICHO_PLUS(unit))
          {
              if (!is_found) {
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit %d DB id %d Program %d Cycle %d Qual Type %s was not found.\n\r"),
                           unit, db_id_ndx, db_prog, db_prog_info.cycle, SOC_PPC_FP_QUAL_TYPE_to_string(qual_type)));
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 50, exit);
              }
          }
          *qual_lsb = qual_lsb_lcl;
          *qual_length_no_padding = qual_size_in_bits_no_padding;
      
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_lsb_and_length_get()", 0, 0);
}




    uint32
      arad_pp_fp_qual_type_and_local_lsb_get(
        SOC_SAND_IN  int                  unit,
        SOC_SAND_IN  uint32                   bit_ndx,
        SOC_SAND_IN  uint32                   db_id_ndx,
        SOC_SAND_OUT SOC_PPC_FP_QUAL_TYPE         *qual_type,
        SOC_SAND_OUT uint32                   *qual_lsb
      )
{
  uint32
      qual_indx,
      qual_length_no_padding,
      key_lsb,
    res;
  
  SOC_PPC_FP_DATABASE_INFO
    database_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_QUAL_TYPE_AND_LOCAL_LSB_GET);

  *qual_type = BCM_FIELD_ENTRY_INVALID;
  *qual_lsb = 0;

  
  SOC_PPC_FP_DATABASE_INFO_clear(&database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for(qual_indx = 0;  (qual_indx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) 
       && (database_info.qual_types[qual_indx]!= BCM_FIELD_ENTRY_INVALID); ++qual_indx) {
        res = arad_pp_fp_qual_lsb_and_length_get(
                unit,
                db_id_ndx,
                FALSE, 
                database_info.qual_types[qual_indx],
                &key_lsb, 
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        if ((bit_ndx >= key_lsb) && (bit_ndx < key_lsb + qual_length_no_padding)) {
            *qual_type = database_info.qual_types[qual_indx];
            *qual_lsb = bit_ndx - key_lsb; 
            break;
        }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_type_and_local_lsb_get()", 0, 0);
}

uint32
  arad_pp_fp_action_lsb_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_type,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO     *fp_database_info,
    SOC_SAND_OUT uint32                   *action_lsb
  )
{
  uint32
    action_size_in_bits,
    action_size_in_bits_in_fem,
    action_lsb_lcl = 0,
    res;
  uint32
    action_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_ACTION_LSB_GET);

  for (action_type_ndx = 0; action_type_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    if (fp_database_info->action_types[action_type_ndx] != action_type)
    {
      
      res = arad_pp_fp_action_type_max_size_get(
              unit,
              fp_database_info->action_types[action_type_ndx],
              &action_size_in_bits,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      action_lsb_lcl += action_size_in_bits;
    }
    else
    {
      break;
    }
  }

  *action_lsb = action_lsb_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_lsb_get()", 0, 0);
}


uint32
  arad_pp_fp_fem_duplicate(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                     is_for_tm,
    SOC_SAND_IN  uint32                    fem_id_orig,
    SOC_SAND_IN  uint32                    fem_id_dest,
    SOC_SAND_IN  uint8                     fem_pgm_id
  )
{
  uint32
      pmf_pgm_ndx,
      pmf_pgm_ndx_min,
      pmf_pgm_ndx_max,
    res = SOC_SAND_OK;
  ARAD_PMF_FEM_NDX
    fem_ndx_orig,
    fem_ndx_dest;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    action_format_info;
  uint32
    selected_bits_ndx;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    action_format_map_info;
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    selected_bits_info;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  SOC_PPC_FP_FEM_ENTRY
    fem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_DUPLICATE);
  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "Duplicate FEM %d to FEM %d \n\r"), 
             fem_id_orig, fem_id_dest));


  
  ARAD_PMF_FEM_NDX_clear(&fem_ndx_orig);
  fem_ndx_orig.id = fem_id_orig;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&action_format_info);
  res = arad_pmf_fem_action_format_get_unsafe(
          unit,
          &fem_ndx_orig,
          fem_pgm_id, 
          &action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_PMF_FEM_NDX_clear(&fem_ndx_dest);
  fem_ndx_dest.id = fem_id_dest;
  res = arad_pmf_fem_action_format_set_unsafe(
          unit,
          &fem_ndx_dest,
          fem_pgm_id, 
          &action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
  {
    ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&action_format_map_info);
    res = arad_pmf_fem_action_format_map_get_unsafe(
            unit,
            &fem_ndx_orig,
            fem_pgm_id,
            selected_bits_ndx,
            &action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_pmf_fem_action_format_map_set_unsafe(
            unit,
            &fem_ndx_dest,
            fem_pgm_id,
            selected_bits_ndx,
            &action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&selected_bits_info);
  res = arad_pmf_fem_select_bits_get_unsafe(
          unit,
          &fem_ndx_orig,
          fem_pgm_id,
          &selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = arad_pmf_fem_select_bits_set_unsafe(
          unit,
          &fem_ndx_dest,
          fem_pgm_id,
          &selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = arad_pmf_prog_select_pmf_pgm_borders_get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            is_for_tm, 
            &pmf_pgm_ndx_min,
            &pmf_pgm_ndx_max
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  for (pmf_pgm_ndx = pmf_pgm_ndx_min; pmf_pgm_ndx < pmf_pgm_ndx_max; ++pmf_pgm_ndx)
  {
    ARAD_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
    res = arad_pmf_db_fem_input_get_unsafe(
            unit,
            pmf_pgm_ndx,
            FALSE, 
            fem_id_orig,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = arad_pmf_db_fem_input_set_unsafe(
            unit,
            pmf_pgm_ndx,
            FALSE, 
            fem_id_dest,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

  
  SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_orig,
          &fem_entry
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 90, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_dest,
          &fem_entry
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);

  SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_orig,
          &fem_entry
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_duplicate()", 0, 0);
}




STATIC
  uint32
    arad_pp_fp_fem_is_fem_blocking_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY     *entry_ndx_to_insert, 
      SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY     *entry_ndx_to_compare,
      SOC_SAND_OUT uint8                    *is_fem_blocking
    )
{
  uint32
    db_strength_strong,
    db_strength_weak,
    entry_strength_strong,
    entry_strength_weak,
    res = SOC_SAND_OK;
  uint8
    forbidden_fem_cycle2_found = FALSE;
  uint8 is_qax_drop_dest = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_FEM_BLOCKING_GET);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx_to_insert);
  SOC_SAND_CHECK_NULL_INPUT(entry_ndx_to_compare);

    if (SOC_IS_JERICHO_PLUS(unit)) {
      if ((entry_ndx_to_insert->action_type[0] == SOC_PPC_FP_ACTION_TYPE_DEST_DROP)
          && (entry_ndx_to_compare->action_type[0] == SOC_PPC_FP_ACTION_TYPE_DEST)) {
          is_qax_drop_dest = TRUE;
      }

      if ((entry_ndx_to_insert->action_type[0] == SOC_PPC_FP_ACTION_TYPE_DEST)
          && (entry_ndx_to_compare->action_type[0] == SOC_PPC_FP_ACTION_TYPE_DEST_DROP)) {
          is_qax_drop_dest = TRUE;
      }  
    }

  
  if ((entry_ndx_to_insert->action_type[0] == entry_ndx_to_compare->action_type[0]) || is_qax_drop_dest)
  {
    
    db_strength_strong = entry_ndx_to_insert->db_strength;
    db_strength_weak = entry_ndx_to_compare->db_strength;

    
    if (db_strength_weak > db_strength_strong)
    {
      forbidden_fem_cycle2_found = TRUE;
    }
    else if ((db_strength_strong == db_strength_weak)
              && (entry_ndx_to_insert->db_id == entry_ndx_to_compare->db_id)
              && (entry_ndx_to_insert->is_for_entry == TRUE))
    {
      
      entry_strength_strong = entry_ndx_to_insert->entry_strength;
      entry_strength_weak = entry_ndx_to_compare->entry_strength;

      
      if (entry_strength_weak > entry_strength_strong)
      {
        forbidden_fem_cycle2_found = TRUE;
      }
    }
  }

  *is_fem_blocking = forbidden_fem_cycle2_found;

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_fem_blocking_get()", 0, 0);
}


uint32
  arad_pp_fp_action_type_to_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE    fp_action_type,
    SOC_SAND_OUT uint32  *pmf_fem_action_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_ACTION_TYPE_TO_PMF_CONVERT);

  
    *pmf_fem_action_type = fp_action_type;

    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_type_to_pmf_convert()", 0, 0);
}

uint32
  arad_pp_fp_action_type_from_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               pmf_fem_action_type,
    SOC_SAND_OUT SOC_PPC_FP_ACTION_TYPE    *fp_action_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_ACTION_TYPE_FROM_PMF_CONVERT);

  
  *fp_action_type = pmf_fem_action_type;

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_type_from_pmf_convert()", 0, 0);
}





STATIC
  uint32
    arad_pp_fp_fem_set(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  ARAD_PMF_FEM_NDX             *fem,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE        action_type,
      SOC_SAND_IN  uint32                    extraction_lsb,
      SOC_SAND_IN  uint32                    nof_bits,
      SOC_SAND_IN  uint32                    base_value,
      SOC_SAND_IN  uint8                  fem_pgm_id
    )
{
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  uint32
    res,
    bit_loc_lsb,
    bit_ndx,
    bit_loc_ndx,
    selected_bits_ndx;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_SET);

  
  
  ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
  fem_selected_bits_info.sel_bit_msb = 3;
  res = arad_pmf_fem_select_bits_set_unsafe(
          unit,
          fem,
          fem_pgm_id,
          &fem_selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
  fem_action_format_map_info.map_data = 0;
  fem_action_format_map_info.action_fomat_id = fem_pgm_id;
  fem_action_format_map_info.is_action_valid = TRUE;
  for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
  {
    res = arad_pmf_fem_action_format_map_set_unsafe(
            unit,
            fem,
            fem_pgm_id,
            selected_bits_ndx,
            &fem_action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

  
  ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
  res = arad_pp_fp_action_type_to_pmf_convert(
          unit,
          action_type,
          &(fem_action_format_info.type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  fem_action_format_info.size = nof_bits;
  fem_action_format_info.base_value = base_value;

  
  bit_loc_lsb = 0;


  bit_ndx = bit_loc_lsb;
  for (bit_loc_ndx = 0; bit_loc_ndx < nof_bits - bit_loc_lsb; ++bit_loc_ndx)
  {
    fem_action_format_info.bit_loc[bit_ndx].type = ARAD_PMF_FEM_BIT_LOC_TYPE_KEY;
    fem_action_format_info.bit_loc[bit_ndx].val = extraction_lsb + bit_loc_ndx;

    bit_ndx ++;
  }

  res = arad_pmf_fem_action_format_set_unsafe(
          unit,
          fem,
          fem_pgm_id,
          &fem_action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_set()", 0, 0);
}


uint32
  arad_pp_fp_fem_configure(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL            *qual_info,
    SOC_SAND_IN  uint8                           fem_pgm_id,
    SOC_SAND_IN uint32                          fem_bypass_pgm_bmp
  )
{
  uint32
    selected_bits_ndx_msb,
    selected_bits_ndx_lsb,
    select_msb,
    select_lsb,
    expected_select,
    selected_value = 0,
    selected_mask = 0,
    get_select,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    qual_length_no_padding,
    key_select,
    action_lsb,
    qual_lsb,
    extraction_lsb = ARAD_PP_FP_TCAM_KEY_SIZE,
    extraction_msb = ARAD_PP_FP_TCAM_KEY_SIZE,
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_INFO
    *database_info = NULL;
  ARAD_PMF_FEM_NDX
    fem_ndx;
  uint32
    bit_loc_ndx,
    bit_ndx,
    fld_ndx,
    pmf_pgm_ndx,
    exist_progs,
    selected_bits_ndx;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  int32
    u64_ndx;
  uint8
    fem_pgm_id_temp,
    is_qual_bit_valid,
    is_action_applied,
    use_kaps = 0,
    use_tcam = 0,
    use_key = 0;
  ARAD_PP_FP_KEY_DP_PROG_INFO
      *db_prog_info = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_CONFIGURE);

  
  ARAD_ALLOC(database_info, SOC_PPC_FP_DATABASE_INFO, 1, "arad_pp_fp_fem_configure.database_info");
  SOC_PPC_FP_DATABASE_INFO_clear(database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  
  
  res = arad_pp_fp_action_lsb_get(
          unit,
          entry_ndx->action_type[0],
          database_info,
          &action_lsb
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_pp_fp_action_type_max_size_get(
          unit,
          entry_ndx->action_type[0],
          &action_size_in_bits_max,
          &action_size_in_bits_in_fem
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_PMF_FEM_NDX_clear(&fem_ndx);
  fem_ndx.id = fem_id_ndx;
  if (
      ((database_info->db_type == SOC_PPC_FP_DB_TYPE_TCAM)
        ||(database_info->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE))
      && (entry_ndx->is_for_entry == FALSE)
      )
  {
    res = arad_pp_fp_fem_set(
            unit,
            &fem_ndx,
            entry_ndx->action_type[0],
            action_lsb,
            action_size_in_bits_in_fem,
            0 ,
            fem_pgm_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else if (entry_ndx->is_for_entry == TRUE)
  {
    
    ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
    res = arad_pp_fp_action_type_to_pmf_convert(
            unit,
            entry_ndx->action_type[0],
            &(fem_action_format_info.type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    fem_action_format_info.size = action_size_in_bits_in_fem;
    fem_action_format_info.base_value = fem_info->base_val;

    
    bit_ndx = 0;

    
    for (fld_ndx = 0; fld_ndx < fem_info->nof_fields; ++fld_ndx)
    {
        qual_lsb = 0;
        
        if (fem_info->fld_ext[fld_ndx].type == SOC_PPC_FP_QUAL_IRPP_KAPS_PASS1_PAYLOAD) {
            use_kaps = 1;
            if (extraction_lsb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
                key_select = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID_USE_KAPS(fem_info->fld_ext[fld_ndx].fld_lsb);
                extraction_lsb = key_select * ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS_USE_KAPS;
            }
            else if (fem_info->fld_ext[fld_ndx].fld_lsb < extraction_lsb) {
                key_select = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID_USE_KAPS(fem_info->fld_ext[fld_ndx].fld_lsb);
                extraction_lsb = key_select * ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS_USE_KAPS;
            }

            if ((fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits) > extraction_msb) {
                extraction_msb = fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits;
            }
        }
        else if((fem_info->fld_ext[fld_ndx].type >= SOC_PPC_FP_QUAL_IRPP_TCAM_0_RESULT)
            && (fem_info->fld_ext[fld_ndx].type <= SOC_PPC_FP_QUAL_IRPP_TCAM_3_RESULT)) {
            use_tcam = 1;
            if (extraction_lsb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
                key_select = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(fem_info->fld_ext[fld_ndx].fld_lsb);
                extraction_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_FES_LSB(key_select);
            }
            else if (fem_info->fld_ext[fld_ndx].fld_lsb < extraction_lsb) {
                key_select = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(fem_info->fld_ext[fld_ndx].fld_lsb);
                extraction_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_FES_LSB(key_select);
            }

            if ((fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits) > extraction_msb) {
                extraction_msb = fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits;
            }
        }
        else if (fem_info->fld_ext[fld_ndx].type != BCM_FIELD_ENTRY_INVALID) {
            use_key = 1;
            res = arad_pp_fp_qual_lsb_and_length_get(
                    unit,
                    entry_ndx->db_id,
                    FALSE,
                    fem_info->fld_ext[fld_ndx].type,
                    &qual_lsb, 
                    &qual_length_no_padding
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  
            if (fem_info->fld_ext[fld_ndx].nof_bits + fem_info->fld_ext[fld_ndx].fld_lsb > qual_length_no_padding) {
              LOG_ERROR(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "Unit %d Qualifier %s : not enough qualifier bits in the Key\n\r"
                                    "Field number of bits %d, Field lsb %d, Field length without padding %d.\n\r"),
                         unit, SOC_PPC_FP_QUAL_TYPE_to_string(fem_info->fld_ext[fld_ndx].type), fem_info->fld_ext[fld_ndx].nof_bits,
                         fem_info->fld_ext[fld_ndx].fld_lsb, qual_length_no_padding));
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 43, exit);
            }

            if (extraction_lsb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
                extraction_lsb = ((qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb)/ARAD_PMF_FEM_KEY_LSB_MULTIPLE)*ARAD_PMF_FEM_KEY_LSB_MULTIPLE;
                extraction_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(extraction_lsb);
                extraction_msb = qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits;
            }
            else if ((qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb) < extraction_lsb) {
                extraction_lsb = ((qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb)/ARAD_PMF_FEM_KEY_LSB_MULTIPLE)*ARAD_PMF_FEM_KEY_LSB_MULTIPLE;
            }

            if ((qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits) > extraction_msb) {
                extraction_msb = qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits;
            }
        }
    }

    if ((use_kaps + use_tcam + use_key) > 1) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "FEM: trying to extract from different sources\n\r")));
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 44, exit);
    }

    
    
    
    for (u64_ndx = ARAD_PP_FP_FEM_SAND_U64_NOF_BITS - 1; u64_ndx >= 0; u64_ndx--)
    {
      if (u64_ndx >= 32)
      {
        is_qual_bit_valid = SOC_SAND_GET_BIT(qual_info->is_valid.arr[1], (u64_ndx - 32));
      }
      else
      {
        is_qual_bit_valid = SOC_SAND_GET_BIT(qual_info->is_valid.arr[0], u64_ndx);
      }

      if (is_qual_bit_valid > 0)
      {
        break;
      }
    }

    ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
    fem_selected_bits_info.sel_bit_msb = 3;
    qual_lsb = 0;
    qual_length_no_padding = 0;
    if (is_qual_bit_valid) { 
        res = arad_pp_fp_qual_lsb_and_length_get(
                unit,
                entry_ndx->db_id,
                FALSE,
                qual_info->type,
                &qual_lsb, 
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
        if (u64_ndx > qual_length_no_padding) {
            LOG_ERROR(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "Unit %d Qualifier %s : not enough qualifier bits in the Key\n\r"
                                  "Field number of bits %d, Field length without padding %d.\n\r"),
                       unit, SOC_PPC_FP_QUAL_TYPE_to_string(fem_info->fld_ext[fld_ndx].type), u64_ndx,
                       qual_length_no_padding));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 46, exit);
        }

        
        if (((use_kaps + use_tcam + use_key) == 0)
            || ((extraction_lsb > 0)
                && (((extraction_lsb > qual_lsb) && (u64_ndx<3))
                    || ((u64_ndx>=3) && (extraction_lsb > (qual_lsb+u64_ndx-3)))))){

            
            extraction_lsb = (qual_lsb/ARAD_PMF_FEM_KEY_LSB_MULTIPLE)*ARAD_PMF_FEM_KEY_LSB_MULTIPLE;
            extraction_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(extraction_lsb);
        }
        
        if ((qual_lsb + u64_ndx) >= (extraction_lsb + 32)) {
            LOG_ERROR(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "FEM : qualifier bits not in the selected key:\n\r"
                                  "    qual_lsb %d, u64_ndx %d, extraction_lsb %d\n\r"),
                                  qual_lsb, u64_ndx, extraction_lsb));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 47, exit);
        }
        
        fem_selected_bits_info.sel_bit_msb = SOC_SAND_MAX(qual_lsb + u64_ndx - extraction_lsb, 3);
    }

    if (extraction_lsb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
        extraction_lsb = 0;
    }

    if (extraction_msb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
        extraction_msb = 0;
    }

    if (((use_kaps + use_tcam + use_key) != 0) && (extraction_msb > (extraction_lsb + 32))) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "FEM extracted bits could not be put in one key:\n\r"
                              "    extraction_msb %d, extraction_lsb %d\n\r"),
                   extraction_msb, extraction_lsb));
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 48, exit);
    }

    for (fld_ndx = 0; fld_ndx < fem_info->nof_fields; ++fld_ndx)
    {
        qual_lsb = 0;

        if (!((fem_info->fld_ext[fld_ndx].type >= SOC_PPC_FP_QUAL_IRPP_TCAM_0_RESULT) &&
            (fem_info->fld_ext[fld_ndx].type >= SOC_PPC_FP_QUAL_IRPP_KAPS_PASS1_PAYLOAD)) &&
            (fem_info->fld_ext[fld_ndx].type != BCM_FIELD_ENTRY_INVALID)) {
            res = arad_pp_fp_qual_lsb_and_length_get(
                    unit,
                    entry_ndx->db_id,
                    FALSE,
                    fem_info->fld_ext[fld_ndx].type,
                    &qual_lsb, 
                    &qual_length_no_padding
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);
        }

        for (bit_loc_ndx = 0; bit_loc_ndx < fem_info->fld_ext[fld_ndx].nof_bits; ++bit_loc_ndx)
        {
            
            if (fem_info->fld_ext[fld_ndx].type == BCM_FIELD_ENTRY_INVALID)
            {
              fem_action_format_info.bit_loc[bit_ndx].type = ARAD_PMF_FEM_BIT_LOC_TYPE_CST;
              fem_action_format_info.bit_loc[bit_ndx].val = SOC_SAND_GET_BIT(fem_info->fld_ext[fld_ndx].cst_val, bit_loc_ndx);
            }
            else {
              fem_action_format_info.bit_loc[bit_ndx].type = ARAD_PMF_FEM_BIT_LOC_TYPE_KEY;
              fem_action_format_info.bit_loc[bit_ndx].val = qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + bit_loc_ndx;
              fem_action_format_info.bit_loc[bit_ndx].val -= extraction_lsb;
            }        
        
          bit_ndx ++;
        }
    }

    res = arad_pmf_fem_action_format_set_unsafe(
            unit,
            &fem_ndx,
            fem_pgm_id,
            &fem_action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


    res = arad_pmf_fem_select_bits_set_unsafe(
            unit,
            &fem_ndx,
            fem_pgm_id,
            &fem_selected_bits_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
    fem_action_format_map_info.map_data = 0;
    fem_action_format_map_info.action_fomat_id = fem_pgm_id;
    if (is_qual_bit_valid) {
        res = arad_pp_fp_qual_lsb_and_length_get(
                unit,
                entry_ndx->db_id,
                FALSE,
                qual_info->type,
                &qual_lsb, 
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

        
        select_msb = is_qual_bit_valid? u64_ndx : 0;
        select_lsb = (is_qual_bit_valid && (u64_ndx > 3))?(u64_ndx - 3) : 0;
        
        if ((qual_lsb + u64_ndx - extraction_lsb) >= 3) {
            selected_bits_ndx_lsb = (u64_ndx >= 3)?0:(3-u64_ndx);
            selected_bits_ndx_msb = 3;
        }
        else {
            selected_bits_ndx_msb = qual_lsb + u64_ndx - extraction_lsb;
            selected_bits_ndx_lsb = qual_lsb - extraction_lsb;
        }

        SHR_BITCOPY_RANGE(&selected_value, 0, &qual_info->val.arr[0], select_lsb, select_msb-select_lsb+1);
        SHR_BITCOPY_RANGE(&selected_mask, 0, &qual_info->is_valid.arr[0], select_lsb, select_msb-select_lsb+1);
        expected_select = selected_value & selected_mask;
    }
    else {
        select_msb = 0;
        select_lsb = 0;
        selected_bits_ndx_msb = 0;
        selected_bits_ndx_lsb = 0;
        expected_select = 0;
    }
    for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
    {
      SHR_BITCOPY_RANGE(&selected_value, 0, &selected_bits_ndx, selected_bits_ndx_lsb, selected_bits_ndx_msb-selected_bits_ndx_lsb+1);
      get_select = selected_value & selected_mask;
      if ((expected_select == get_select) || (!is_qual_bit_valid))
      {
        is_action_applied = TRUE;
      }
      else
      {
        is_action_applied = FALSE;
      }

      fem_action_format_map_info.is_action_valid = is_action_applied;
      res = arad_pmf_fem_action_format_map_set_unsafe(
              unit,
              &fem_ndx,
              fem_pgm_id,
              selected_bits_ndx,
              &fem_action_format_map_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }

  }
  else if (
            (database_info->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION)
           && (entry_ndx->is_for_entry == FALSE)
          )
  {
    
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR, 100, exit);
  }

  
    
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            entry_ndx->db_id,
            0,
            &exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);

  ARAD_ALLOC(db_prog_info, ARAD_PP_FP_KEY_DP_PROG_INFO, 1, "arad_pp_fp_fem_configure.db_prog_info");

  for (pmf_pgm_ndx = 0; pmf_pgm_ndx < SOC_DPP_DEFS_GET(unit, nof_ingress_pmf_programs); ++pmf_pgm_ndx)
  {
    if ((exist_progs & (1 << pmf_pgm_ndx)) == 0)
    {
      continue;
    }

    res = arad_pp_fp_fem_pgm_per_pmf_pgm_get(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,pmf_pgm_ndx,&fem_pgm_id_temp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

    if (fem_pgm_id_temp != fem_pgm_id) {
        continue;
    }

    ARAD_PMF_FEM_INPUT_INFO_clear(&fem_input_info);

    fem_input_info.db_id = SOC_PPC_FP_NOF_DBS;
    res = arad_pp_fp_fem_pgm_per_pmf_pgm_get(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                                         pmf_pgm_ndx,&fem_input_info.pgm_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 113, exit);

    sal_memset(db_prog_info, 0, sizeof(*db_prog_info));
    
    res = arad_pp_fp_db_prog_info_get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            entry_ndx->db_id,
            pmf_pgm_ndx,
            db_prog_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

    fem_input_info.db_id = entry_ndx->db_id; 
    if (SOC_SAND_GET_BIT(fem_bypass_pgm_bmp, pmf_pgm_ndx) != 0){
        
        fem_input_info.src_arad.is_nop = TRUE; 
    }                
    else if ((database_info->db_type == SOC_PPC_FP_DB_TYPE_TCAM)
        || (database_info->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE))
    {
        fem_input_info.src_arad.is_key_src = use_key; 
        fem_input_info.src_arad.use_kaps = use_kaps;
        fem_input_info.src_arad.key_lsb = extraction_lsb;
        if (use_tcam) {
            if (extraction_lsb >= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                fem_input_info.src_arad.key_lsb -= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE;
                fem_input_info.src_arad.key_tcam_id = db_prog_info->key_id[1];
            }
            else {
                fem_input_info.src_arad.key_tcam_id = db_prog_info->key_id[0];
            }
        }
        fem_input_info.src_arad.lookup_cycle_id = db_prog_info->cycle;
    }
    else if (database_info->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION)
    {
      fem_input_info.src_arad.is_key_src = TRUE; 
      fem_input_info.src_arad.key_tcam_id = db_prog_info->key_id[0];
      fem_input_info.src_arad.lookup_cycle_id = db_prog_info->cycle;

      if (database_info->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_IS_EQUAL_MSB) {
          
          fem_input_info.src_arad.key_lsb = 128; 
          if (SOC_IS_JERICHO_PLUS(unit)) {
              
              fem_input_info.src_arad.is_key_src = FALSE; 
              fem_input_info.src_arad.is_compare = TRUE; 
          }
      }
      else{
          fem_input_info.src_arad.key_lsb = (db_prog_info->alloc_place == ARAD_PP_FP_KEY_CE_HIGH)? ARAD_PMF_FEM_KEY_LSB_FIRST_IN_MSB : 0; 
          fem_input_info.src_arad.key_lsb += extraction_lsb; 
      }
    }
    else
    {
        fem_input_info.src_arad.is_nop = TRUE; 
    }
    res = arad_pmf_db_fem_input_set_unsafe(
            unit,
            pmf_pgm_ndx,
            FALSE, 
            fem_id_ndx,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);
  }

  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_ndx,
          entry_ndx
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);


exit:
  ARAD_FREE(database_info);
  ARAD_FREE(db_prog_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_configure()", 0, 0);
}


uint32
  arad_pp_fp_fem_configuration_de_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  uint32                    cycle_ndx,
    SOC_SAND_IN  uint8                     fem_pgm_id,
    SOC_SAND_OUT SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL            *qual_info
  )
{
  uint32
    bit_ndx_lsb,
    local_lsb,
    key_bit_last,
    key_select_lsb = 0,
    exist_progs,
    fem_id_absolute,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_INFO
    database_info;
  ARAD_PMF_FEM_NDX
    fem_ndx;
  uint32
    bit_ndx,
    pmf_pgm_ndx,
    pmf_pgm_ndx_min,
    pmf_pgm_ndx_max,
    selected_bits_ndx_mask_last,
    selected_bits_ndx_all,
    selected_bits_ndx;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  ARAD_PP_FP_KEY_DP_PROG_INFO
    *db_prog_info = NULL;
  int32
    fld_ndx,
    u64_ndx,
    u64_ndx2;
  uint8
    is_for_tm,
    is_default_tm,
    has_a_valid_entry[2],
    is_one,
    mask[ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS],
    val_mask[ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS];
  ARAD_PMF_FEM_BIT_LOC_TYPE
    type_previous;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_CONFIGURATION_GET);

  fem_id_absolute = fem_id_ndx + (cycle_ndx * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP);
  
  SOC_PPC_FP_DATABASE_INFO_clear(&database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_fp_database_is_tm_get(unit, &database_info, &is_for_tm, &is_default_tm);
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, entry_ndx->db_id, 0, &exist_progs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);  

  
  res = arad_pmf_prog_select_pmf_pgm_borders_get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            is_for_tm , 
            &pmf_pgm_ndx_min,
            &pmf_pgm_ndx_max
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  for (pmf_pgm_ndx = pmf_pgm_ndx_min; pmf_pgm_ndx < pmf_pgm_ndx_max; ++pmf_pgm_ndx)
  {
      if (SOC_SAND_GET_BIT(exist_progs, pmf_pgm_ndx) != 0x1){
          
          continue;
      }

      res = arad_pmf_db_fem_input_get_unsafe(
              unit,
              pmf_pgm_ndx,
              FALSE, 
              fem_id_ndx,
              &fem_input_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);      

      if (!fem_input_info.src_arad.is_nop) {
          if (fem_input_info.src_arad.is_key_src) {
              key_select_lsb = fem_input_info.src_arad.key_lsb;

              ARAD_ALLOC(db_prog_info, ARAD_PP_FP_KEY_DP_PROG_INFO, 1, "arad_pp_fp_fem_configuration_de_get.db_prog_info");
              res = arad_pp_fp_db_prog_info_get(
                      unit,
                      SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                      entry_ndx->db_id,
                      pmf_pgm_ndx,
                      db_prog_info
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

              if ((db_prog_info->alloc_place == ARAD_PP_FP_KEY_CE_HIGH)
                && (key_select_lsb >= ARAD_PMF_FEM_KEY_LSB_FIRST_IN_MSB)){
                  key_select_lsb -= ARAD_PMF_FEM_KEY_LSB_FIRST_IN_MSB;
              }
          }
          break;
      }
  }  
 
  
  ARAD_PMF_FEM_NDX_clear(&fem_ndx);
  fem_ndx.id = fem_id_absolute;
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_absolute,
          entry_ndx
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if (
       (database_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION)
       && (entry_ndx->is_for_entry == TRUE)
      )
  {
      res = arad_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[0],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);


    
    ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
    res = arad_pmf_fem_action_format_get_unsafe(
            unit,
            &fem_ndx,
            fem_pgm_id,
            &fem_action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_pp_fp_action_type_from_pmf_convert(
            unit,
            fem_action_format_info.type,
            &(fem_info->type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    fem_info->base_val = fem_action_format_info.base_value;

    
    type_previous = ARAD_NOF_PMF_FEM_BIT_LOC_TYPES;
    key_bit_last = 0;
    fld_ndx = -1; 

    
    bit_ndx_lsb = 0;
    if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(fem_info->type))
    {
      bit_ndx_lsb = ARAD_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
    }

    for (bit_ndx = bit_ndx_lsb; bit_ndx < action_size_in_bits_in_fem; ++bit_ndx)
    {
      if (fem_action_format_info.bit_loc[bit_ndx].type == ARAD_PMF_FEM_BIT_LOC_TYPE_KEY)
      {
        if (
            (type_previous == fem_action_format_info.bit_loc[bit_ndx].type)
            && (key_bit_last + 1 == fem_action_format_info.bit_loc[bit_ndx].val)
            && (fld_ndx >= 0)
           )
        {
          fem_info->fld_ext[fld_ndx].nof_bits ++;
          key_bit_last = fem_action_format_info.bit_loc[bit_ndx].val;
          continue;
        }
        else
        {
          
          fld_ndx ++;
          type_previous = fem_action_format_info.bit_loc[bit_ndx].type;
          key_bit_last = fem_action_format_info.bit_loc[bit_ndx].val;
          res = arad_pp_fp_qual_type_and_local_lsb_get(
                  unit,
                  key_bit_last + key_select_lsb,
                  entry_ndx->db_id,
                  &(fem_info->fld_ext[fld_ndx].type),
                  &(fem_info->fld_ext[fld_ndx].fld_lsb)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          fem_info->fld_ext[fld_ndx].nof_bits = 1;
        }
      }
      else if (!ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(fem_info->type)) 
      {
        if (type_previous == fem_action_format_info.bit_loc[bit_ndx].type && fld_ndx >= 0)
        {
          SOC_SAND_SET_BIT(fem_info->fld_ext[fld_ndx].cst_val, fem_action_format_info.bit_loc[bit_ndx].val, fem_info->fld_ext[fld_ndx].nof_bits);
          fem_info->fld_ext[fld_ndx].nof_bits ++;
          continue;
        }
        else
        {
          
          fld_ndx ++;
          SOC_SAND_SET_BIT(fem_info->fld_ext[fld_ndx].cst_val, fem_action_format_info.bit_loc[bit_ndx].val, 0);
          fem_info->fld_ext[fld_ndx].nof_bits = 1;
          type_previous = fem_action_format_info.bit_loc[bit_ndx].type;
        }
      }
    }
    fem_info->nof_fields = fld_ndx + 1;


    
    ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
    res = arad_pmf_fem_select_bits_get_unsafe(
            unit,
            &fem_ndx,
            fem_pgm_id,
            &fem_selected_bits_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    selected_bits_ndx_mask_last = 0;
    for (selected_bits_ndx = 0; selected_bits_ndx < ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS; ++selected_bits_ndx)
    {
        for (is_one = FALSE; is_one <= TRUE; ++is_one)
        {
            has_a_valid_entry[is_one] = FALSE;
            for (selected_bits_ndx_all = 0; (selected_bits_ndx_all <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX) && (!has_a_valid_entry[is_one]); ++selected_bits_ndx_all)
            {
                if (SOC_SAND_GET_BIT(selected_bits_ndx_all, selected_bits_ndx) != is_one) {
                    
                    continue;
                }

              ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
              res = arad_pmf_fem_action_format_map_get_unsafe(
                      unit,
                      &fem_ndx,
                      fem_pgm_id,
                      selected_bits_ndx_all,
                      &fem_action_format_map_info
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

              has_a_valid_entry[is_one] |= fem_action_format_map_info.is_action_valid;
            }
        }

      
      if (has_a_valid_entry[0] && has_a_valid_entry[1])
      {
        mask[selected_bits_ndx] = 0;
        val_mask[selected_bits_ndx] = 0;
      }
      else 
      {
        selected_bits_ndx_mask_last = selected_bits_ndx;
        mask[selected_bits_ndx] = 0x1;
        val_mask[selected_bits_ndx] = (has_a_valid_entry[1])? 1 : 0;
      }
    }

    res = arad_pp_fp_qual_type_and_local_lsb_get(
            unit,
            fem_selected_bits_info.sel_bit_msb - 3 + selected_bits_ndx_mask_last + key_select_lsb,
            entry_ndx->db_id,
            &(qual_info->type),
            &(local_lsb)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    
    for (bit_ndx = ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS; bit_ndx > 0; bit_ndx--)
    {
      u64_ndx = local_lsb - (ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS - bit_ndx);
      u64_ndx2 = selected_bits_ndx_mask_last - (ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS - bit_ndx);
      SOC_SAND_SET_BIT(qual_info->is_valid.arr[0], mask[u64_ndx2], u64_ndx);
      SOC_SAND_SET_BIT(qual_info->val.arr[0], val_mask[u64_ndx2], u64_ndx);
      if (u64_ndx == 0)
      {
        break;
      }
    }
  }


exit:
  ARAD_FREE(db_prog_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_configuration_de_get()", 0, 0);
}



uint32 arad_pp_fp_fes_sole_invalid_next_idx_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8                   pgm_id,
    SOC_SAND_IN uint32                  cycle_to_use,
    SOC_SAND_IN uint32                  nof_fes_group,
    SOC_SAND_IN ARAD_PMF_FES            *fes_info,
    SOC_SAND_OUT int                    *idx
)
{
    uint32 fes_idx;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_COMBINE_INVALID_GET);

    SOC_SAND_CHECK_NULL_INPUT(idx);
    SOC_SAND_CHECK_NULL_INPUT(fes_info);

    *idx = -1;
    for (fes_idx = (cycle_to_use*nof_fes_group); fes_idx < ARAD_PMF_LOW_LEVEL_NOF_FESS; fes_idx++) {
        if ((fes_info[fes_idx].action_type == SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT)
            && (fes_info[fes_idx].is_combined)
            && ((fes_info[fes_idx+1].action_type != SOC_PPC_FP_ACTION_TYPE_COUNTER_A) 
                && (fes_info[fes_idx+1].action_type != SOC_PPC_FP_ACTION_TYPE_SNP))) {
            *idx = fes_idx - cycle_to_use*nof_fes_group;
            break;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fes_sole_invalid_next_idx_get()", 0, 0);

}


uint32 arad_pp_fp_fes_combined_action_pre(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                  pmf_pgm_ndx,
    SOC_SAND_IN uint32                  flags,
    SOC_SAND_IN uint8                   idx_free,
    SOC_SAND_IN uint8                   idx_invalid_next,
    SOC_SAND_IN uint32                  cycle_to_use,
    SOC_SAND_IN uint32                  nof_fes_per_group,
    SOC_SAND_INOUT uint8                *is_fem_higher,
    SOC_SAND_INOUT uint8                *is_fem_lower,
    SOC_SAND_INOUT ARAD_PMF_FES         *fes_info
)
{
    uint32 res = SOC_SAND_OK;
    uint32 idx_move_start, idx_move_end;
    uint32 idx_move_from, idx_move_to;
    uint32 group_base;
    uint8 idx, value_high, value_low;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_COMBINE_PRE_PROCESS);

    SOC_SAND_CHECK_NULL_INPUT(is_fem_higher);
    SOC_SAND_CHECK_NULL_INPUT(is_fem_higher);
    SOC_SAND_CHECK_NULL_INPUT(fes_info);

    if (idx_invalid_next == (idx_free-1)) {
        ARAD_PP_DO_NOTHING_AND_EXIT;
    }

    if (idx_invalid_next >= idx_free) {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR, 110, exit);
    }

    group_base = cycle_to_use * nof_fes_per_group;

    
     
    
    idx_move_from = idx_invalid_next + group_base;
    idx_move_to = idx_free + group_base;
    ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

    idx_move_start = idx_free + group_base;
    idx_move_end = idx_invalid_next + 1 + group_base;
    res = arad_pmf_db_fes_chunk_move_unsafe(
            unit,
            pmf_pgm_ndx,
            flags,
            idx_move_start,
            idx_move_end,
            -1,
            fes_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

    
    value_high = is_fem_higher[idx_invalid_next];
    value_low = is_fem_lower[idx_invalid_next];
    for (idx = idx_invalid_next; idx < idx_free-1; idx++) {
        is_fem_higher[idx] = is_fem_higher[idx+1];
        is_fem_lower[idx] = is_fem_lower[idx+1];
    }
    is_fem_higher[idx_free-1] = value_high;
    is_fem_lower[idx_free-1] = value_low;
    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fes_combined_action_inpre()", 0, 0);
}


uint32 arad_pp_fp_fes_combined_action_process(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                  pmf_pgm_ndx,
    SOC_SAND_IN uint8                   type,
    SOC_SAND_IN uint32                  flags,
    SOC_SAND_IN uint8                   is_insert_combine,
    SOC_SAND_IN uint8                   is_compare_combine,
    SOC_SAND_IN uint32                  cycle_to_use,
    SOC_SAND_IN uint32                  nof_fes_per_group,
    SOC_SAND_IN uint8                   idx_compare,
    SOC_SAND_INOUT uint8                *idx_free,
    SOC_SAND_INOUT ARAD_PMF_FES         *fes_info
)
{
    uint32 res = SOC_SAND_OK;
    uint32 idx_move_start, idx_move_end;
    uint32 idx_move_from, idx_move_to;
    uint32 group_base;
    
    int move_step = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_COMBINE_PROCESS);

    SOC_SAND_CHECK_NULL_INPUT(idx_free);
    SOC_SAND_CHECK_NULL_INPUT(fes_info);

    group_base = cycle_to_use * nof_fes_per_group;
    
    if (is_insert_combine && !is_compare_combine) {
        
        
        
        idx_move_from = idx_compare + group_base;
        idx_move_to = *idx_free + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);
        
        idx_move_from = *idx_free - 1 + group_base;
        idx_move_to = idx_compare + group_base;            
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

        
        if (type == ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H) {            
            idx_move_start = (*idx_free < idx_compare) ? (*idx_free+group_base+1) : (idx_compare+group_base+1);
            idx_move_end = (*idx_free > idx_compare) ? (*idx_free+group_base-2) : (idx_compare+group_base-2);
            move_step = (idx_move_start == idx_move_end) ? 0 : 1;
        } else {            
            idx_move_start = (*idx_free < idx_compare) ? (*idx_free+group_base) : (idx_compare+group_base);
            idx_move_end = (*idx_free > idx_compare) ? (*idx_free+group_base) : (idx_compare+group_base);
            move_step = (idx_move_start == idx_move_end) ? 0 : -1;
        }
        res = arad_pmf_db_fes_chunk_move_unsafe(
                unit,
                pmf_pgm_ndx,
                flags,
                idx_move_start,
                idx_move_end,
                move_step,
                fes_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
        if (type == ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H) {
            *idx_free = idx_compare + 1;
        } else {
            *idx_free = idx_compare;
        }
    } else if (!is_insert_combine && is_compare_combine) {
        

        
        if (type == ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H) {            
            idx_move_start = (*idx_free < idx_compare) ? (*idx_free+group_base-1) : (idx_compare+group_base-1);
            idx_move_end = (*idx_free > idx_compare) ? (*idx_free+group_base-1) : (idx_compare+group_base-1);
            move_step = (idx_move_start == idx_move_end) ? 0 : 1;
        } else {            
            idx_move_start = (*idx_free < idx_compare) ? (*idx_free+group_base+1) : (idx_compare+group_base+1);
            idx_move_end = (*idx_free > idx_compare) ? (*idx_free+group_base) : (idx_compare+group_base);
            move_step = (idx_move_start == idx_move_end) ? 0 : -1;
        }
        res = arad_pmf_db_fes_chunk_move_unsafe(
                unit,
                pmf_pgm_ndx,
                flags,
                idx_move_start,
                idx_move_end,
                move_step,
                fes_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
        
        if (type == ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H) {
            *idx_free = idx_compare - 1;
        } else {
            *idx_free = idx_compare;
        }
    } else if (is_insert_combine && is_compare_combine) {
        

        
        idx_move_from = idx_compare + group_base;
        idx_move_to = *idx_free + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

        
        idx_move_from = *idx_free - 1 + group_base;
        idx_move_to = idx_compare + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

        idx_move_from = idx_compare - 1 + group_base;
        idx_move_to = *idx_free -1 + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

        idx_move_from = idx_compare + group_base;
        idx_move_to = idx_compare - 1 + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);
        
        *idx_free = idx_compare;
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fes_combined_action_process()", 0, 0);

}



uint32 arad_pp_fp_fes_combined_action_post(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                  pmf_pgm_ndx,
    SOC_SAND_IN uint32                  flags,
    SOC_SAND_IN uint8                   idx_invalid_next,
    SOC_SAND_INOUT uint8                *idx_free,
    SOC_SAND_INOUT ARAD_PMF_FES         *fes_info
)
{
    uint32 res = SOC_SAND_OK;
    int fes_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(idx_free);
    SOC_SAND_CHECK_NULL_INPUT(fes_info);

    if (*idx_free > idx_invalid_next) {
        for (fes_id=idx_invalid_next-1; fes_id>=0; --fes_id) {
            if (!fes_info[fes_id].is_used) {
                
                res = arad_pmf_db_fes_chunk_move_unsafe(
                        unit,
                        pmf_pgm_ndx,
                        flags,
                        fes_id + 1,
                        idx_invalid_next + 1,
                        -1,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
                break;
            }
            else if (!fes_info[fes_id].is_combined && (fes_info[fes_id].action_type != SOC_PPC_FP_ACTION_TYPE_SNP)
                && (fes_info[fes_id].action_type != SOC_PPC_FP_ACTION_TYPE_COUNTER_A)) {
                
                ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, fes_id, *idx_free);
    
                
                res = arad_pmf_db_fes_chunk_move_unsafe(
                        unit,
                        pmf_pgm_ndx,
                        flags,
                        fes_id + 1,
                        idx_invalid_next + 1,
                        -1,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

                
                ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, *idx_free, idx_invalid_next+1);

                
                if (*idx_free == (idx_invalid_next+1)) {
                    *idx_free -= 1;
                }

                break;
            }
        }
    }
    else {
        for (fes_id=idx_invalid_next+2; fes_id<ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_id) {
            if (!fes_info[fes_id].is_used) {
                
                res = arad_pmf_db_fes_chunk_move_unsafe(
                        unit,
                        pmf_pgm_ndx,
                        flags,
                        idx_invalid_next,
                        fes_id - 1,
                        1,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
                break;
            }
            else if (!fes_info[fes_id].is_combined && (fes_info[fes_id].action_type != SOC_PPC_FP_ACTION_TYPE_SNP)
                && (fes_info[fes_id].action_type != SOC_PPC_FP_ACTION_TYPE_COUNTER_A)) {
                
                ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, fes_id, *idx_free);
    
                
                res = arad_pmf_db_fes_chunk_move_unsafe(
                        unit,
                        pmf_pgm_ndx,
                        flags,
                        idx_invalid_next,
                        fes_id - 1,
                        1,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

                
                ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, *idx_free, idx_invalid_next);
                break;
            }
        }
    }

    if ((fes_id < 0) || (fes_id>ARAD_PMF_LOW_LEVEL_NOF_FESS)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_FAILURE_OUT_OF_RESOURCES, SOC_SAND_GEN_ERR, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fes_combined_action_post()", 0, 0);
}



uint32
  arad_pp_fp_fem_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      pmf_pgm_ndx,
      pmf_pgm_ndx_min,
      pmf_pgm_ndx_max,
      pmf_pgm_bmp_used,
    cycle_ndx,
      fem_id_absolute,
    fem_id_ndx;
  SOC_PPC_FP_FEM_ENTRY
    fem_entry;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  uint32
    selected_bits_ndx;
  ARAD_PMF_FEM_NDX
    fem_ndx;
  uint8
      fem_pgm_id;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_REMOVE);

  
  
  
  for (cycle_ndx = 0; cycle_ndx < ARAD_PMF_NOF_CYCLES; ++cycle_ndx)
  {
    for (fem_id_ndx = 0; fem_id_ndx < ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP; ++fem_id_ndx)
    {
        for(fem_pgm_id = 0 ; (fem_pgm_id < ARAD_PMF_NOF_FEM_PGMS); ++fem_pgm_id)
        {
            fem_id_absolute = fem_id_ndx + (cycle_ndx * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP);
          SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                  unit,
                  SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                  fem_pgm_id,
                  fem_id_absolute,
                  &fem_entry
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
          if (entry_ndx->db_id == fem_entry.db_id)
          {
            if (
                (entry_ndx->is_for_entry == FALSE)
                || (entry_ndx->entry_id == fem_entry.entry_id)
               )
            {
              
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, fem_entry.db_id, 0, &pmf_pgm_bmp_used);

              SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
                      unit,
                      SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                      fem_pgm_id,
                      fem_id_absolute,
                      &fem_entry
                    );
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

              res = arad_pmf_prog_select_pmf_pgm_borders_get(
                        unit,
                        SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                        fem_pgm_id, 
                        &pmf_pgm_ndx_min,
                        &pmf_pgm_ndx_max
                      );
              SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

              ARAD_PMF_FEM_INPUT_INFO_clear(&fem_input_info);

              
              for (pmf_pgm_ndx = pmf_pgm_ndx_min; pmf_pgm_ndx < pmf_pgm_ndx_max; ++pmf_pgm_ndx)
              {
                    if (SOC_SAND_GET_BIT(pmf_pgm_bmp_used, pmf_pgm_ndx) == FALSE){
                      
                      continue;
                    }
                    fem_input_info.db_id = 0; 
                    fem_input_info.pgm_id = fem_pgm_id;
                    fem_input_info.src_arad.is_nop = TRUE;
                    res = arad_pmf_db_fem_input_set_unsafe(
                            unit,
                            pmf_pgm_ndx,
                            FALSE, 
                            fem_id_absolute,
                            &fem_input_info
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
              }

              
              ARAD_PMF_FEM_NDX_clear(&fem_ndx);
              fem_ndx.id = fem_id_absolute;

              ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
              fem_action_format_map_info.map_data = 0;
              fem_action_format_map_info.action_fomat_id = ARAD_PMF_FEM_ACTION_DEFAULT_NOP_3; 
              fem_action_format_map_info.is_action_valid = FALSE;
              for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
              {
                res = arad_pmf_fem_action_format_map_set_unsafe(
                        unit,
                        &fem_ndx,
                        fem_pgm_id,
                        selected_bits_ndx,
                        &fem_action_format_map_info
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
              }
            }
          }
        }
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_remove()", 0, 0);
}



uint32
  arad_pp_fp_fem_insert_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  uint32                         flags,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *fem_info,
    SOC_SAND_INOUT ARAD_PMF_FES                 fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_IN uint8                           fem_pgm_id,
    SOC_SAND_IN uint32                          fem_bypass_pgm_bmp,
    SOC_SAND_OUT uint32                         *fes_fem_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  )
{
  uint32
    output_size_in_bits,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
      nof_fem_fes_per_group,
    res = SOC_SAND_OK;
  uint8
      is_for_fem,
      is_for_tm,
      
      is_fem_higher[ARAD_PMF_LOW_LEVEL_NOF_FESS] = {FALSE},
    is_fem_lower[ARAD_PMF_LOW_LEVEL_NOF_FESS] = {FALSE},
    place_found;
  uint32
    action_ndx,
    fes_prio,
    fem_free_min_ndx,
    fem_free_min,
    fem_free_first=0,
    fem_free_last,
    cycle_ndx,
    nof_free_fems[ARAD_PMF_NOF_CYCLES],
    cycle_to_use,
    fes_id_indx,
    fes_id_indx_2,
    fem_id_ndx;
  SOC_PPC_FP_FEM_ENTRY
    entry_replicated,
    fem_entry;
  SOC_PPC_FP_FEM_CYCLE
    fem_cycle_lcl;
  ARAD_PMF_FEM_NDX
    fem;
  SOC_PPC_FP_DATABASE_STAGE
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;
  char
      reason_for_fail[640] ;
  uint8 is_insert_combined = FALSE, is_exist_combined = FALSE;
  uint8 idx_free, idx_compare;
  int idx_invalid_next;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_INSERT_UNSAFE);

  

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(success);

  
  res = arad_pp_fp_fem_insert_verify(
          unit,
          entry_ndx,
          fem_cycle,
          fem_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  is_for_fem = entry_ndx->is_for_entry;
  nof_fem_fes_per_group = (is_for_fem)? ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP:
                             ((flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP)? 
                                    ARAD_PMF_LOW_LEVEL_NOF_FESS:ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP);
  is_for_tm = ( flags & ARAD_PP_FP_FEM_ALLOC_FES_TM ) ? TRUE : FALSE ;                        
  

  
  if (!is_for_fem) {
      SOC_SAND_CHECK_NULL_INPUT(fes_info);
  }
  else {
      SOC_SAND_CHECK_NULL_INPUT(fem_info);
  }

      

      reason_for_fail[0] = 0 ;
      place_found = TRUE ;
      if (!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP)) {
          
          res = arad_pp_fp_fem_is_place_get_unsafe(
              unit,
              entry_ndx,
              fem_cycle,
              is_for_tm,
              fem_pgm_id,
              fes_info,
              &place_found,
              reason_for_fail,
              sizeof(reason_for_fail)
            ) ;
         SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
      if (place_found == TRUE)
      {
        *success = SOC_SAND_SUCCESS;
      }
      else if(!is_for_fem && !(flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP)) {
          
          *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES ;
          ARAD_PP_DO_NOTHING_AND_EXIT ;
      }
      else
      {
          LOG_ERROR(BSL_LS_SOC_FP,
              (BSL_META_U(
                  unit,
                  "\r\n  ==> FEM not found for db_id: %d, entry:%d \n\r"
                  "%s%s\n\r"
                  ), 
                  entry_ndx->db_id, entry_ndx->entry_id,
                  (sal_strlen(reason_for_fail) ? "  ==> Reason: " : ""),
                  reason_for_fail
              )
          ) ;
          *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES ;
          ARAD_PP_DO_NOTHING_AND_EXIT ;
      }
  

  
  if (flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP) {
      cycle_to_use = 0;
  }
  else if (fem_cycle->is_cycle_fixed)
  {
    cycle_to_use = fem_cycle->cycle_id;
  }
  else
  {
    
    
    for (cycle_ndx = 0; cycle_ndx < ARAD_PMF_NOF_CYCLES; ++cycle_ndx)
    {
      char
        reason_for_fail[1] ;
      reason_for_fail[0] = 0 ;
      SOC_PPC_FP_FEM_CYCLE_clear(&fem_cycle_lcl);
      fem_cycle_lcl.is_cycle_fixed = TRUE;
      fem_cycle_lcl.cycle_id = SOC_SAND_NUM2BOOL(cycle_ndx);
      res = arad_pp_fp_fem_is_place_get_unsafe(
              unit,
              entry_ndx,
              &fem_cycle_lcl,
              is_for_tm,
              fem_pgm_id,
              fes_info,
              &place_found,
              reason_for_fail,
              sizeof(reason_for_fail)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      nof_free_fems[cycle_ndx] = 0;

      if (place_found == FALSE)
      {
        continue;
      }

      
      for (fem_id_ndx = 0; fem_id_ndx < nof_fem_fes_per_group; ++fem_id_ndx)
      {
        fes_id_indx = fem_id_ndx + (fem_cycle_lcl.cycle_id * nof_fem_fes_per_group);
        SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
        if (is_for_fem) {
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                    unit,
                    stage,
                    fem_pgm_id,
                    fes_id_indx,
                    &fem_entry
                  );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
        }
        else {
            fem_entry.action_type[0] = fes_info[fes_id_indx].is_used? 
                fes_info[fes_id_indx].action_type:SOC_PPC_FP_ACTION_TYPE_INVALID;
        }

        if (fem_entry.action_type[0] == SOC_PPC_FP_ACTION_TYPE_INVALID)
        {
          nof_free_fems[cycle_ndx] ++;
        }
      }
    }

    
    cycle_ndx = (is_for_fem)? 0:1;
    if (nof_free_fems[cycle_ndx])
    {
      cycle_to_use = cycle_ndx;
    }
    else
    {
      cycle_to_use = 1 - cycle_ndx;
    }
  }

  
  for (action_ndx = 0; action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
      
      if ((!is_for_fem) && action_ndx) {
          break;
      }

    if (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    
    fem_free_min = 0; 
    for (fem_id_ndx = 0; (fem_id_ndx < nof_fem_fes_per_group) && is_for_fem; ++fem_id_ndx)
    {
      ARAD_PMF_FEM_NDX_clear(&fem);
      fem.cycle_ndx = cycle_to_use;
      fem.id = fem_id_ndx;
      res = arad_pmf_fem_output_size_get(
              unit,
              &fem,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = arad_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[action_ndx],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      if (action_size_in_bits_in_fem <= output_size_in_bits)
      {
        
        if ((output_size_in_bits != 4) || (!(entry_ndx->is_base_positive[action_ndx])))
        {
          fem_free_min = fem_id_ndx;
          break;
        }
      }
    }

    
    for (fem_id_ndx = 0; fem_id_ndx < nof_fem_fes_per_group; ++fem_id_ndx)
    {
      is_fem_higher[fem_id_ndx] = 0;
    }

    
    fem_free_min_ndx = nof_fem_fes_per_group;
    for (fem_free_first = fem_free_min; fem_free_first < nof_fem_fes_per_group; ++fem_free_first)
    {
        if (is_for_fem) {
          SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                  unit,
                  stage,
                  fem_pgm_id,
                  fem_free_first + (cycle_to_use * nof_fem_fes_per_group),
                  &fem_entry
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

          SOC_PPC_FP_FEM_ENTRY_clear(&entry_replicated);
          ARAD_COPY(&entry_replicated, entry_ndx, SOC_PPC_FP_FEM_ENTRY, 1);
          entry_replicated.action_type[0] = entry_ndx->action_type[action_ndx];
          res = arad_pp_fp_fem_is_fem_blocking_get(
                  unit,
                  &entry_replicated,
                  &fem_entry,
                  &(is_fem_higher[fem_free_first])
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

          res = arad_pp_fp_fem_is_fem_blocking_get(
                  unit,
                  &fem_entry,
                  &entry_replicated,
                  &(is_fem_lower[fem_free_first])
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

          if ((fem_entry.action_type[0] == SOC_PPC_FP_ACTION_TYPE_INVALID) 
              && (fem_free_min_ndx == nof_fem_fes_per_group))
          {
            fem_free_min_ndx = fem_free_first;
          }
        }
        else  {
            fes_id_indx = fem_free_first + (cycle_to_use * nof_fem_fes_per_group);
            if(fes_info[fes_id_indx].is_used 
                && ((fes_info[fes_id_indx].action_type == entry_ndx->action_type[action_ndx])
                    || ((fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_DEST_DROP) && (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_DEST))
                    || ((fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_DEST) && (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_DEST_DROP)))) {
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.prio.get(unit, stage, fes_info[fes_id_indx].db_id, &fes_prio);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
                
                if (!((flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED)
                        && (fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT))) {
                    
                    if (fes_prio > entry_ndx->db_strength) {
                        is_fem_higher[fem_free_first] = TRUE;
                    }
                    else if (fes_prio < entry_ndx->db_strength) {
                        is_fem_lower[fem_free_first] = TRUE;
                    }
                }
                
            }
            
            if ((!fes_info[fes_id_indx].is_used) 
                && (fem_free_min_ndx == nof_fem_fes_per_group)) {
                fem_free_min_ndx = fem_free_first;
            }
        }
    }

    if (flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED) {
        is_insert_combined = ((entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_A)
                            || (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_SNP)) ? TRUE : FALSE;
    }

    

      fem_free_first = fem_free_min_ndx;
      fem_free_last = fem_free_first;
      idx_free = fem_free_min_ndx;  

        
        if (!is_for_fem && is_insert_combined) {
            res = arad_pp_fp_fes_sole_invalid_next_idx_get(
                        unit,
                        entry_ndx->entry_id,
                        cycle_to_use,
                        nof_fem_fes_per_group,
                        fes_info,
                        &idx_invalid_next);
            SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
            if (idx_invalid_next != -1) {
                res = arad_pp_fp_fes_combined_action_pre(
                        unit,
                        entry_ndx->entry_id,
                        flags,
                        idx_free,
                        idx_invalid_next,
                        cycle_to_use,
                        nof_fem_fes_per_group,
                        is_fem_higher,
                        is_fem_lower,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
            }
        }
      
      
      for (fem_id_ndx = fem_free_first + 1; fem_id_ndx < nof_fem_fes_per_group; fem_id_ndx++)
      {
        if (is_fem_lower[fem_id_ndx] == TRUE)
        {
          fem_free_last = fem_id_ndx;

          if (is_for_fem) {
              res = arad_pp_fp_fem_duplicate(
                      unit,
                      is_for_tm,
                      fem_free_last + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_free_first + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_pgm_id
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
          } else {              
              
              fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group);
              if (fes_info[fes_id_indx].is_combined) {
                  if ((fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_SNP)
                        || (fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_COUNTER_A)) {
                      is_exist_combined = TRUE;
                  }
              }
              
              if (is_insert_combined || is_exist_combined) {
                  idx_compare = fem_free_last;                  
                  res = arad_pp_fp_fes_combined_action_process(
                            unit,
                            entry_ndx->entry_id,
                            ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_L,
                            flags,
                            is_insert_combined,
                            is_exist_combined,
                            cycle_to_use,
                            nof_fem_fes_per_group,
                            idx_compare,
                            &idx_free,
                            fes_info);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
              } else {
                  
                  fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group); 
                  fes_id_indx_2 = fem_free_first + (cycle_to_use * nof_fem_fes_per_group); 

                  sal_memcpy(&fes_info[fes_id_indx_2],&fes_info[fes_id_indx],sizeof(ARAD_PMF_FES));
                  fes_info[fes_id_indx].is_used = 0;
                  
                  if(!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
                      res = arad_pmf_db_fes_move_unsafe(
                            unit,
                            entry_ndx->entry_id, 
                            fes_id_indx,
                            fes_id_indx_2,
                            &fes_info[fes_id_indx_2]
                          );
                      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
                  }
              }
          }
          if (is_insert_combined || is_exist_combined) {
            fem_free_first = idx_free;
          } else {
            fem_free_first = fem_id_ndx;
          }
      
        }
      }

      
      fem_free_last = fem_free_first;
      idx_free = fem_free_first; 
      for (fem_id_ndx = fem_free_first; fem_id_ndx > 0; fem_id_ndx--)
      {
        if (is_fem_higher[fem_id_ndx - 1] == TRUE)
        {
          fem_free_last = fem_id_ndx - 1;

          if (is_for_fem) {
              res = arad_pp_fp_fem_duplicate(
                      unit,
                      is_for_tm,
                      fem_free_last + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_free_first + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_pgm_id
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
          } else {
              fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group);
              if (fes_info[fes_id_indx].is_combined) {
                  if ((fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_SNP)
                        || (fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_COUNTER_A)) {
                      is_exist_combined = TRUE;
                  }
              }

              if (is_insert_combined || is_exist_combined) {
                  idx_compare = fem_free_last;                  
                  res = arad_pp_fp_fes_combined_action_process(
                            unit,
                            entry_ndx->entry_id,
                            ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H,
                            flags,
                            is_insert_combined,
                            is_exist_combined,
                            cycle_to_use,
                            nof_fem_fes_per_group,
                            idx_compare,
                            &idx_free,
                            fes_info);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
              } else {
                  
                  fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group); 
                  fes_id_indx_2 = fem_free_first + (cycle_to_use * nof_fem_fes_per_group); 
                  
                  sal_memcpy(&fes_info[fes_id_indx_2],&fes_info[fes_id_indx],sizeof(ARAD_PMF_FES));
                  fes_info[fes_id_indx].is_used = 0;
                  
                  if(!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
                      res = arad_pmf_db_fes_move_unsafe(
                            unit,
                            entry_ndx->entry_id, 
                            fes_id_indx,
                            fes_id_indx_2,
                            &fes_info[fes_id_indx_2]
                          );
                      SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
                  }
              }
          }
          
          fem_free_first = fem_id_ndx - 1;
        }
      }

        if (is_insert_combined || is_exist_combined) {
            fem_free_last = idx_free;
        }
        *fes_fem_id = fem_free_last + (cycle_to_use * nof_fem_fes_per_group);

        
        if (is_for_fem) {
            res = arad_pp_fp_fem_configure(
                    unit,
                    fem_free_last + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                    &entry_replicated,
                    &(fem_info->actions[action_ndx]),
                    &(fem_info->qual_vals[0]),
                    fem_pgm_id,
                    fem_bypass_pgm_bmp
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
        }
        else {
            fes_id_indx = ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP - 1;
            if (fes_info[fes_id_indx].is_used && fes_info[fes_id_indx].is_combined
                && (fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT)) {
                res = arad_pp_fp_fes_combined_action_post(
                          unit,
                          entry_ndx->entry_id,
                          flags,
                          fes_id_indx,
                          &idx_free,
                          fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
            }

            fes_info[*fes_fem_id].is_used = 1;
            fes_info[*fes_fem_id].db_id = entry_ndx->db_id;
            fes_info[*fes_fem_id].action_type = entry_ndx->action_type[0];
            fes_info[*fes_fem_id].is_combined = (flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED) ? 1 : 0;
        }
  }
        


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_insert_unsafe()", 0, 0);
}

uint32
  arad_pp_fp_fem_insert_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *fem_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_INSERT_VERIFY);

  res = SOC_PPC_FP_FEM_ENTRY_verify(unit, entry_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FP_FEM_CYCLE, fem_cycle, 15, exit);
  if (entry_ndx->is_for_entry) {
    
      res = SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_verify(unit, fem_info, SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, FALSE );
      SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_insert_verify()", 0, 0);
}

STATIC
  uint32
    arad_pp_fp_fem_is_place_get_for_cycle(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY   *entry_ndx,
      SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE   *fem_info,
      SOC_SAND_IN  uint8                  fem_pgm_id,
      SOC_SAND_INOUT ARAD_PMF_FES         fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
      SOC_SAND_OUT uint8                  *place_found,
      SOC_SAND_OUT char                   *reason_for_fail,
      SOC_SAND_IN int32                   reason_for_fail_len
    )
{
  uint32
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    output_size_in_bits,
    res = SOC_SAND_OK;
  uint32
    fem_fes_group_ndx,
    nof_fem_fes_per_group,
    nof_fem_fes_groups,
    nof_fem_fes_per_group_other_type,
    action_ndx,
    fem_id_ndx;
  uint8
    is_higher_prio, 
    is_other_type_fes_fem, 
    is_for_fem,
    fes_is_found = FALSE,
    fem_is_found = FALSE;
  SOC_PPC_FP_FEM_ENTRY
    fem_entry_reference,
    fem_entry;
  ARAD_PMF_FEM_NDX
    fem;
  SOC_PPC_FP_ACTION_TYPE
    fem_action_type_bmp[ARAD_PMF_LOW_LEVEL_FEM_ID_MAX + 1];
  SOC_PPC_FP_FEM_ENTRY
    fem_entry_high,
    fem_entry_low;
  SOC_PPC_FP_DATABASE_INFO
    fp_database_info;
  uint8
    is_for_tm,
    is_default_tm,
    forbidden_fem_cycle2_found = FALSE;
  uint32
    fes_id_indx,
    cycle_curr, 
    other_cycle;
  SOC_PPC_FP_DATABASE_STAGE
    stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
  char
    *loc_reason_for_fail ;
  int32
    loc_reason_for_fail_len ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE);

  loc_reason_for_fail = reason_for_fail ;
  loc_reason_for_fail_len = reason_for_fail_len ;
  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fem_info);
  SOC_SAND_CHECK_NULL_INPUT(place_found);
  SOC_SAND_CHECK_NULL_INPUT(reason_for_fail);

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "Try to find FEM/FES for db_id:%d, entry:%d, with Info: cycle %d, cycle-fixed: %d  \n\r"), 
             entry_ndx->db_id, entry_ndx->entry_id, fem_info->cycle_id, fem_info->is_cycle_fixed));

  
  is_for_fem = entry_ndx->is_for_entry;
  nof_fem_fes_per_group = (is_for_fem)? ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP:ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP;
  nof_fem_fes_per_group_other_type = (!is_for_fem)? ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP:ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP;
  nof_fem_fes_groups = 2; 

  if (!is_for_fem) {
      SOC_SAND_CHECK_NULL_INPUT(fes_info);
  }

  SOC_PPC_FP_DATABASE_INFO_clear(&fp_database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_fp_database_is_tm_get(unit, &fp_database_info, &is_for_tm, &is_default_tm);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);


  
  other_cycle = ARAD_PMF_NOF_CYCLES - fem_info->cycle_id - 1;
  for (action_ndx = 0; action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
      
      if ((!is_for_fem) && action_ndx) {
          break;
      }

    if (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry_reference);
    ARAD_COPY(&fem_entry_reference, entry_ndx, SOC_PPC_FP_FEM_ENTRY, 1);
    fem_entry_reference.action_type[0] = entry_ndx->action_type[action_ndx];
    
    for (is_other_type_fes_fem = FALSE; (is_other_type_fes_fem <= TRUE)
                                 && (forbidden_fem_cycle2_found == FALSE); is_other_type_fes_fem++) {
        for (fem_fes_group_ndx = 0; fem_fes_group_ndx < (is_other_type_fes_fem?nof_fem_fes_groups:1 )
                                 && (forbidden_fem_cycle2_found == FALSE); fem_fes_group_ndx++) {
            for (fem_id_ndx = 0; (fem_id_ndx < (is_other_type_fes_fem? nof_fem_fes_per_group_other_type:nof_fem_fes_per_group)) 
                                 && (forbidden_fem_cycle2_found == FALSE); ++fem_id_ndx)
            {
                
                if (!is_other_type_fes_fem) {
                    
                    is_higher_prio = (other_cycle == 1);
                }
                else if (is_for_fem && (other_cycle == 0)) {
                    
                    is_higher_prio = FALSE;
                }
                else if (is_for_fem && (other_cycle == 1)) {
                    
                    is_higher_prio = fem_fes_group_ndx;
                }
                else if ((!is_for_fem) && (other_cycle == 1)) {
                    
                    is_higher_prio = TRUE;
                }
                else  {
                    
                    is_higher_prio = fem_fes_group_ndx;
                }
                
                cycle_curr = is_other_type_fes_fem? fem_fes_group_ndx:other_cycle;

                
                SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry_high);
                SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry_low);
                SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
                
                fes_id_indx = fem_id_ndx + (cycle_curr * (is_other_type_fes_fem? nof_fem_fes_per_group_other_type: nof_fem_fes_per_group));
                if (is_for_fem? (!is_other_type_fes_fem): is_other_type_fes_fem) {
                    
                  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                          unit,
                          stage,
                          fem_pgm_id,
                          fes_id_indx,
                          &fem_entry
                        );
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
                }
                else  {
                    if(fes_info[fes_id_indx].is_used) {
                        fem_entry.is_for_entry = FALSE;
                        fem_entry.db_id = fes_info[fes_id_indx].db_id;
                        fem_entry.action_type[0] = fes_info[fes_id_indx].action_type;

                        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.prio.get(unit, stage, fes_info[fes_id_indx].db_id, &fem_entry.db_strength);
                        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);
                    }
                }
                
                if (is_higher_prio)
                {
                  ARAD_COPY(&fem_entry_high, &fem_entry, SOC_PPC_FP_FEM_ENTRY, 1);
                  ARAD_COPY(&fem_entry_low, &fem_entry_reference, SOC_PPC_FP_FEM_ENTRY, 1);
                }
                else
                {
                  ARAD_COPY(&fem_entry_high, &fem_entry_reference, SOC_PPC_FP_FEM_ENTRY, 1);
                  ARAD_COPY(&fem_entry_low, &fem_entry, SOC_PPC_FP_FEM_ENTRY, 1);
                }

                res = arad_pp_fp_fem_is_fem_blocking_get(
                      unit,
                      &fem_entry_high,
                      &fem_entry_low,
                      &forbidden_fem_cycle2_found
                    ) ;
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            }
        }
    }
  }
  if (forbidden_fem_cycle2_found == TRUE)
  {
    *place_found = FALSE;
    if (entry_ndx) {
      if (reason_for_fail) {
        if (loc_reason_for_fail_len > 1) {
          uint32
            strlen_reason_for_fail ;
          sal_snprintf(
            loc_reason_for_fail,
            loc_reason_for_fail_len,
            "%s(): BLOCKING found for %s for action %d ('%s') on db %d\r\n"
            "  ==> fem_entry_high: db %d action %d db_strength %d\r\n"
            "  ==> fem_entry_low: db %d action %d db_strength %d\r\n",
            __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
            entry_ndx->action_type[0],
            SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[0]),entry_ndx->db_id,
            fem_entry_high.db_id,fem_entry_high.action_type[0],fem_entry_high.db_strength,
            fem_entry_low.db_id,fem_entry_low.action_type[0],fem_entry_low.db_strength
          ) ;
          loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
          
          strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
          loc_reason_for_fail_len -= strlen_reason_for_fail ;
          loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
        }
      }
    }
    ARAD_PP_DO_NOTHING_AND_EXIT;
  } else {
    
    reason_for_fail[0] = 0 ;
    loc_reason_for_fail = reason_for_fail ;
    loc_reason_for_fail_len = reason_for_fail_len ;
  }
  
  for (fem_id_ndx = 0; (fem_id_ndx < nof_fem_fes_per_group) && (fes_is_found == FALSE); ++fem_id_ndx)
  {
      if (is_for_fem) {
        
        SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                unit,
                stage,
                fem_pgm_id,
                fem_id_ndx + (fem_info->cycle_id * nof_fem_fes_per_group),
                &fem_entry
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        fem_action_type_bmp[fem_id_ndx] = fem_entry.action_type[0];
      }
      else  {
          fes_id_indx = fem_id_ndx + (fem_info->cycle_id * nof_fem_fes_per_group);
          if (!fes_info[fes_id_indx].is_used) {
              fes_is_found = TRUE;
              LOG_DEBUG(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "   "
                                    "FES found for db_id:%d, entry:%d, action: %s , FES-ID: %d\n\r"), 
                         entry_ndx->db_id, entry_ndx->entry_id, 
                         SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), fes_id_indx));
          }
      }
  }

  for (action_ndx = 0; (action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX); action_ndx ++)
  {
      
      if ((!is_for_fem) && action_ndx) {
          break;
      }

    if (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    fem_is_found = FALSE;
    for (fem_id_ndx = 0; (fem_id_ndx < nof_fem_fes_per_group) && (fem_is_found == FALSE) && is_for_fem; ++fem_id_ndx)
    {
      
      ARAD_PMF_FEM_NDX_clear(&fem);
      fem.cycle_ndx = fem_info->cycle_id;
      fem.id = fem_id_ndx;
      res = arad_pmf_fem_output_size_get(
              unit,
              &fem,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

      res = arad_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[action_ndx],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      
      if (action_size_in_bits_in_fem > output_size_in_bits)
      {
          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "   "
                                "FEM skipped for action: %s (size: %d). FEM-ID: %d, max-size %d  is too small \n\r"), 
                     SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), action_size_in_bits_in_fem,
                     fem_id_ndx, output_size_in_bits));
        continue;
      }

      if ((output_size_in_bits == 4) && (entry_ndx->is_base_positive[action_ndx] == TRUE))
      {
          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "   "
                                "FEM skipped for action: %s (size: %d). FEM-ID: %d, max-size %d  is 4b and has a positive base (0/1) %d \n\r"), 
                     SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), action_size_in_bits_in_fem,
                     fem_id_ndx, output_size_in_bits,
                     entry_ndx->is_base_positive[action_ndx]));
        continue;
      }

      if (fem_action_type_bmp[fem_id_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
      {
        fem_is_found = TRUE;
        fem_action_type_bmp[fem_id_ndx] = entry_ndx->action_type[action_ndx];
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   "
                              "FEM found for db_id:%d, entry:%d, action: %s (size: %d). FEM-ID: %d, max-size %d  \n\r"), 
                   entry_ndx->db_id, entry_ndx->entry_id, 
                   SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), action_size_in_bits_in_fem,
                   fem_id_ndx, output_size_in_bits));
      }
    }
    if ( (is_for_fem && (fem_is_found == FALSE)) || ((!is_for_fem) && (!fes_is_found))) {
      *place_found = FALSE;
      if (entry_ndx) {
        if (reason_for_fail) {
          if (loc_reason_for_fail_len > 1) {
            uint32
              strlen_reason_for_fail ;
            sal_snprintf(
              loc_reason_for_fail,
              loc_reason_for_fail_len,
              "%s(): NO SPACE found for %s for action %d ('%s') on db %d\r\n",
              __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
              entry_ndx->action_type[0],
              SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[0]),entry_ndx->db_id
            ) ;
            loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
            
            strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
            loc_reason_for_fail_len -= strlen_reason_for_fail ;
            loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
          }
        }
      }
      ARAD_PP_DO_NOTHING_AND_EXIT;
    } else {
      
      reason_for_fail[0] = 0 ;
      loc_reason_for_fail = reason_for_fail ;
      loc_reason_for_fail_len = reason_for_fail_len ;
    }
  }

  *place_found = TRUE;

exit:
  
  if (*place_found == 0) {
    if (entry_ndx) {
      if (reason_for_fail) {
        if (loc_reason_for_fail_len > 1) {
          uint32
            strlen_reason_for_fail ;
          sal_snprintf(
            loc_reason_for_fail,
            loc_reason_for_fail_len,
            "%s(): No %s found for action '%s'\r\n",
            __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
            SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[0])
          ) ;
          loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
          
          strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
          loc_reason_for_fail_len -= strlen_reason_for_fail ;
          loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
        }
      }
    }
  } else {
    
    if (reason_for_fail) {
      loc_reason_for_fail_len = reason_for_fail_len ;
      loc_reason_for_fail = reason_for_fail ;
      reason_for_fail[0] = 0 ;
    }
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_place_get_for_cycle()", 0, 0);
}

uint32
  arad_pp_fp_fem_is_place_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY   *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE   *fem_info,
    SOC_SAND_IN  uint8                  is_for_tm,
    SOC_SAND_IN  uint8                  fem_pgm_id,
    SOC_SAND_INOUT ARAD_PMF_FES         fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint8                  *place_found,
    SOC_SAND_OUT char                   *reason_for_fail,
    SOC_SAND_IN int32                   reason_for_fail_len
  )
{
  uint32
    program_rsrc = 0,
    res = SOC_SAND_OK;
  uint32
    pmf_pgm_ndx_min,
    pmf_pgm_ndx_max,
    pmf_pgm_ndx,
    cycle_ndx,
    fes_idx;
  uint8
    is_for_fem,
    fem_is_found_lcl,
    fem_is_found = FALSE;
  SOC_PPC_FP_FEM_CYCLE
    fem_info_cycle;
  ARAD_PMF_FES         
    fes_info_lcl[ARAD_PMF_LOW_LEVEL_NOF_FESS]; 
  SOC_PPC_FP_DATABASE_STAGE
    stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
  char
    *loc_reason_for_fail ;
  int32
    loc_reason_for_fail_len ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_PLACE_GET_UNSAFE);

  loc_reason_for_fail = reason_for_fail ;
  loc_reason_for_fail_len = reason_for_fail_len ;

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fem_info);
  SOC_SAND_CHECK_NULL_INPUT(place_found);
  SOC_SAND_CHECK_NULL_INPUT(reason_for_fail) ;

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "Find FEM for db_id:%d, entry:%d, with Info: cycle %d, cycle-fixed: %d  \n\r"), 
             entry_ndx->db_id, entry_ndx->entry_id, fem_info->cycle_id, fem_info->is_cycle_fixed));

  
  res = arad_pp_fp_fem_is_place_get_verify(
          unit,
          entry_ndx,
          fem_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  is_for_fem = entry_ndx->is_for_entry;
  if (!is_for_fem) {
      sal_memcpy(fes_info_lcl, fes_info, ARAD_PMF_LOW_LEVEL_NOF_FESS * sizeof(ARAD_PMF_FES));
  } else {
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.get(unit, stage, 0, &program_rsrc);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  }

  res = arad_pmf_prog_select_pmf_pgm_borders_get(
            unit,
            stage,
            is_for_tm , 
            &pmf_pgm_ndx_min,
            &pmf_pgm_ndx_max
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  
  
  
  for (cycle_ndx = 0; (cycle_ndx < ARAD_PMF_NOF_CYCLES) && (fem_is_found == FALSE); ++cycle_ndx)
  {
    if ((fem_info->is_cycle_fixed == TRUE) && (cycle_ndx != fem_info->cycle_id)) {
      continue;
    }
    SOC_PPC_FP_FEM_CYCLE_clear(&fem_info_cycle);
    fem_info_cycle.is_cycle_fixed = TRUE; 
    fem_info_cycle.cycle_id = SOC_SAND_NUM2BOOL(cycle_ndx);

    
    fem_is_found_lcl = TRUE;
    for (pmf_pgm_ndx = pmf_pgm_ndx_min; (pmf_pgm_ndx < pmf_pgm_ndx_max) && (fem_is_found_lcl == TRUE); ++pmf_pgm_ndx) {
        
        if ((!is_for_fem) && (pmf_pgm_ndx != pmf_pgm_ndx_min)) {
            break;
        }

        
        if (is_for_fem) {
            if (!SHR_BITGET(&program_rsrc, pmf_pgm_ndx)) {
                
                continue;
            }

            for(fes_idx = 0; fes_idx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_idx) {
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(
                          unit,
                          stage,
                          pmf_pgm_ndx,
                          fes_idx,
                          &fes_info_lcl[fes_idx]
                      );
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
            }
        }

        res = arad_pp_fp_fem_is_place_get_for_cycle(
                unit,
                entry_ndx,
                &fem_info_cycle,
                fem_pgm_id,
                fes_info_lcl,               
                &fem_is_found_lcl,
                loc_reason_for_fail,
                loc_reason_for_fail_len
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        
        if (loc_reason_for_fail) {
            if (loc_reason_for_fail_len > 1) {
                if (!fem_is_found_lcl) {
                    uint32
                      strlen_reason_for_fail ;
                    strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
                    loc_reason_for_fail_len -= strlen_reason_for_fail ;
                    loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
		    if (loc_reason_for_fail_len > 1) {
                        if (entry_ndx) {
                            sal_snprintf(
                              loc_reason_for_fail,
                              loc_reason_for_fail_len,
                              "%s(): No %s found on cycle %d program %d\r\n",
                              __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
                              cycle_ndx,pmf_pgm_ndx
                            ) ;
                            loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
                            
                            strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
                            loc_reason_for_fail_len -= strlen_reason_for_fail ;
                            loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
                        }
                    }
                } else {
                    
                    loc_reason_for_fail_len = reason_for_fail_len ;
                    loc_reason_for_fail = reason_for_fail ;
                    reason_for_fail[0] = 0 ;
                }
            }
        }
    }

    
    fem_is_found = fem_is_found_lcl;
  }

  *place_found = fem_is_found;

exit:
  
  if (*place_found == 0) {
    if (entry_ndx) {
      if (reason_for_fail) {
        if (loc_reason_for_fail_len > 1) {
          uint32
            strlen_reason_for_fail ;
          sal_snprintf(
            loc_reason_for_fail,
            loc_reason_for_fail_len,
            "%s(): No %s found for action '%s'\r\n",
            __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
            SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[0])
          ) ;
          loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
          
          strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
          loc_reason_for_fail_len -= strlen_reason_for_fail ;
          loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
        }
      }
    }
  } else {
    
    if (reason_for_fail) {
      loc_reason_for_fail_len = reason_for_fail_len ;
      loc_reason_for_fail = reason_for_fail ;
      reason_for_fail[0] = 0 ;
    }
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_place_get_unsafe()", 0, 0);
}

uint32
  arad_pp_fp_fem_is_place_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE            *fem_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_PLACE_GET_VERIFY);

  res = SOC_PPC_FP_FEM_ENTRY_verify(unit, entry_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FP_FEM_CYCLE, fem_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_place_get_verify()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_fp_fem_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_fp_fem;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_fp_fem_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_fp_fem;
}
uint32
  SOC_PPC_FP_FEM_ENTRY_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY *info
  )
{
  uint32
    action_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_strength, ARAD_PP_FP_FEM_DB_STRENGTH_MAX, ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_id, ARAD_PP_FP_FEM_DB_ID_MAX, ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry_strength, SOC_PPC_FP_FEM_ENTRY_STRENGTH_MAX, SOC_PPC_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->entry_id, SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit), SOC_PPC_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR, 14, exit);
  for (action_ndx = 0; action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
      if (action_ndx && (!info->is_for_entry)) {
          
          break;
      }

    if (info->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(info->action_type[action_ndx], ARAD_PP_FP_FEM_ACTION_TYPE_MAX, SOC_PPC_FP_ACTION_TYPES_OUT_OF_RANGE_ERR, 20 + action_ndx, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FP_FEM_ENTRY_verify()",0,0);
}

uint32
  SOC_PPC_FP_FEM_CYCLE_verify(
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FP_FEM_CYCLE_verify()",0,0);
}


void
  ARAD_PP_FEM_ACTIONS_CONSTRAINT_clear(
    SOC_SAND_OUT ARAD_PP_FEM_ACTIONS_CONSTRAINT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_PP_FEM_ACTIONS_CONSTRAINT));
  info->cycle = ARAD_PP_FP_KEY_CYCLE_ANY;
  info->action_size = ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

#endif 

