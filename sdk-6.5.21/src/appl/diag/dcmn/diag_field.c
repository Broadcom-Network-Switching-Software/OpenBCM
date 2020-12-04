/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag.c
 * Purpose:     Device diagnostics commands.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_APPL_SHELL
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <shared/bsl.h>

#include <appl/diag/diag.h>
#include <appl/diag/diag_field.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/utils.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>

/* { */
/******************************************************************** 
 *  Function handler: packet_diag_get (section fp)
 */
cmd_result_t 
  cmd_ppd_api_fp_packet_diag_get(int unit, args_t* a)  
{   
  uint32 
    ret;   
  SOC_PPC_FP_PACKET_DIAG_INFO   
    *prm_info;
  uint32
    soc_sand_dev_id;
  int
    core, first_core, last_core, prm_core = -1;
  parse_table_t
    pt;

   parse_table_init(unit, &pt);
   parse_table_add(&pt, "core", PQ_DFL | PQ_INT, 0, &prm_core, NULL);
   if (0 > parse_arg_eq(a, &pt)) {
      return CMD_USAGE;
   }

   if ((prm_core < -1 ) || (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
        cli_out("error ilegal core ID for device\n");
        return CMD_FAIL;
   }

   if (prm_core == -1) {
       first_core = 0;
       last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
   } else {
       first_core = prm_core;
       last_core = prm_core + 1;
   }

  prm_info = sal_alloc(sizeof(SOC_PPC_FP_PACKET_DIAG_INFO), "cmd_ppd_api_fp_packet_diag_get.prm_info");
  if(!prm_info) {
      cli_out("Memory allocation failure\n");
      return CMD_FAIL;
  }
  soc_sand_dev_id = (unit);

  for (core = first_core; core < last_core; core++) {

      SOC_PPC_FP_PACKET_DIAG_INFO_clear(prm_info);
      cli_out("======================================== core %d ========================================\n", core);
      /* Get parameters */

      /* Call function */
      ret = soc_ppd_fp_packet_diag_get(
              soc_sand_dev_id,
              core,
              prm_info
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
          sal_free(prm_info);
          return CMD_FAIL;
      }

      SOC_PPC_FP_PACKET_DIAG_INFO_print(unit, prm_info);
  }
  sal_free(prm_info);
  return CMD_OK; 
} 
/* { */
static
  cmd_result_t 
    diag_soc_sand_print_indices(int unit, args_t *a)  
{   
  cmd_result_t 
    ret ;   
  char
    *arg_mode ;
  SOC_SAND_SORTED_LIST_PTR
    sorted_list ;
  int
    val ;

  ret = CMD_OK ;
  /* Get parameters */ 
  arg_mode = ARG_GET(a) ;
  val = sal_ctoi(arg_mode,0) ;
  sorted_list = (SOC_SAND_SORTED_LIST_PTR)val ;
  soc_sand_print_indices(unit,sorted_list) ;
  return (ret) ; 
}
static
  cmd_result_t 
    diag_soc_sand_print_list(int unit, args_t *a)  
{   
  cmd_result_t 
    ret ;   
  char
    *arg_mode ;
  SOC_SAND_SORTED_LIST_PTR
    sorted_list ;
  int
    val ;

  ret = CMD_OK ;
  /* Get parameters */ 
  arg_mode = ARG_GET(a) ;
  val = sal_ctoi(arg_mode,0) ;
  sorted_list = (SOC_SAND_SORTED_LIST_PTR)val ;
  soc_sand_print_list(unit,sorted_list) ;
  return (ret) ; 
}
/* } */


/******************************************************************** 
 *  Function handler: dbs_action_info_show (section fp)
 */

cmd_result_t 
  cmd_ppd_api_fp_dbs_action_info_show(int unit, args_t* a)  
{   
  uint32 
    ret;   

  /* Call function */
  ret = soc_ppd_fp_dbs_action_info_show(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  {
      return CMD_FAIL; 
  } 
  
  return CMD_OK; 
}



/******************************************************************** 
 *  Function handler: action_info_show (section fp)
 */

cmd_result_t 
  cmd_ppd_api_fp_action_info_show(int unit, args_t* a)  
{   
  uint32 
    ret;   

  /* Call function */
  ret = soc_ppd_fp_action_info_show(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  {
      return CMD_FAIL; 
  } 
  
  return CMD_OK; 
}

/******************************************************************** 
 *  Function handler: action_show (section fp)
 */
/**
 * Function:
 *      cmd_ppd_api_fp_action_show_action_size_get
 * Purpose:
 *      This function returns size of a given PPC action type.
 *
 * Parameters:
 *      unit - (IN) Device ID.
 *      stage - (IN) Action PPC stage.
 *      ppc_action_type - (IN) PPC action type.
 *      action_size_p - (OUT) In case of found it will store the size of the
 *                            given PPC action.
 *      found_p - (OUT) In case of TRUE the action was found for current unit,
 *                      otherwise FALSE.
 * Returns:
 *      \retval Error code (as per 'shr_error_e').
 * Notes:
 *      shr_error_e
 */
static shr_error_e
cmd_ppd_api_fp_action_show_action_size_get(
      int unit,
      SOC_PPC_FP_DATABASE_STAGE stage,
      SOC_PPC_FP_ACTION_TYPE ppc_action_type,
      uint32 *action_size_p,
      int *found_p)
{
    uint32 table_line;
    uint32 action_lsb_egress;

    SHR_FUNC_INIT_VARS(unit);

    *found_p = FALSE;

    SHR_IF_ERR_EXIT(arad_pmf_db_fes_action_table_line_get_unsafe(unit, ppc_action_type, stage, &table_line, found_p));
    if (*found_p == TRUE)
    {
        SHR_IF_ERR_EXIT(arad_pmf_db_fes_action_size_get_unsafe(unit, ppc_action_type, stage, action_size_p, &action_lsb_egress));
    }

exit:
    return SHR_ERR_TO_SHELL;
}

cmd_result_t
  cmd_ppd_api_fp_action_show_predefined_bcm(
      int unit,
      SOC_PPC_FP_DATABASE_STAGE stage)
{
    int internal_action;
    uint32 bcm_action_size;
    bcm_field_internal_to_bcm_map_t bcm_action_stage[40];
    uint32 bcm_action_size_actual;
    int bcm_action_index;
    uint32 action_size;
    SOC_PPC_FP_DATABASE_STAGE ppc_fp_stage;
    bcm_dpp_field_info_OLD_t *unitData = NULL;
    int _rv = BCM_E_NONE;
    int found;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    PRT_TITLE_SET("Action Map");
    PRT_COLUMN_ADD("bcmAction");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("SOC Action");
    PRT_COLUMN_ADD("Stage");

    bcm_action_size = sizeof(bcm_action_stage) / sizeof(bcm_action_stage[0]);
    for (internal_action = 0 ; internal_action < SOC_PPC_NOF_FP_ACTION_TYPES_ARAD ; internal_action++)
    {
        SHR_IF_ERR_EXIT(
          bcm_field_internal_to_bcm_action_map(
            unit, 0, internal_action, bcm_action_size, bcm_action_stage, &bcm_action_size_actual));

        if (bcm_action_size_actual)
        {
            for (bcm_action_index = 0 ; bcm_action_index < bcm_action_size_actual ; bcm_action_index++)
            {
                SHR_IF_ERR_EXIT(_bcm_dpp_ppd_stage_from_bcm_stage(unitData, bcm_action_stage[bcm_action_index].bcm_stage, &ppc_fp_stage));
                if (ppc_fp_stage != stage && stage != SOC_PPC_NOF_FP_DATABASE_STAGES)
                {
                    continue;
                }

                SHR_IF_ERR_EXIT(cmd_ppd_api_fp_action_show_action_size_get(unit, ppc_fp_stage, internal_action, &action_size, &found));
                if (found == FALSE)
                {
                    continue;
                }

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", (char *)_bcm_dpp_field_action_name[bcm_action_stage[bcm_action_index].bcm_action]);
                PRT_CELL_SET("%d", action_size);
                PRT_CELL_SET("%s", SOC_PPC_FP_ACTION_TYPE_to_string(internal_action));
                PRT_CELL_SET("%s", (char *)_bcm_dpp_field_stage_name[bcm_action_stage[bcm_action_index].bcm_stage]);
            }
        }
    }

    /* Coverity: We are not going to change the macros to avoid such defects. */
    /* coverity[dead_error_line]*/
    PRT_COMMIT;
    SHR_SET_CURRENT_ERR(_rv);
exit:
    PRT_FREE;
    return SHR_ERR_TO_SHELL;
}

cmd_result_t
  cmd_ppd_api_fp_action_show_predefined_soc(
      int unit,
      SOC_PPC_FP_DATABASE_STAGE stage)
{
    int internal_action;
    uint32 bcm_action_size;
    bcm_field_internal_to_bcm_map_t bcm_action_stage[40];
    uint32 bcm_action_size_actual;
    int bcm_action_index;
    SOC_PPC_FP_DATABASE_STAGE ppc_fp_stage;
    bcm_dpp_field_info_OLD_t *unitData = NULL;
    int _rv = BCM_E_NONE;
    _bcm_dpp_field_stage_idx_t dpp_stage_index;
    SOC_PPC_FP_DATABASE_STAGE bcm_to_ppc_fp_stage;
    uint32 action_size;
    bcm_field_stage_t bcm_stage;
    int found;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    PRT_TITLE_SET("Action Map");
    PRT_COLUMN_ADD("SOC Action");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("bcmAction");
    PRT_COLUMN_ADD("Stage");

    bcm_action_size = sizeof(bcm_action_stage) / sizeof(bcm_action_stage[0]);

    for (dpp_stage_index = 0; dpp_stage_index < 4; dpp_stage_index++)
    {
        ppc_fp_stage = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, dpp_stage_index).hwStageId;

        if (ppc_fp_stage != stage && stage != SOC_PPC_NOF_FP_DATABASE_STAGES)
        {
            continue;
        }

        for (internal_action = 0 ; internal_action < SOC_PPC_NOF_FP_ACTION_TYPES_ARAD ; internal_action++)
        {
            SHR_IF_ERR_EXIT(cmd_ppd_api_fp_action_show_action_size_get(unit, ppc_fp_stage, internal_action, &action_size, &found));
            if (found == FALSE)
            {
                continue;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", SOC_PPC_FP_ACTION_TYPE_to_string(internal_action));
            PRT_CELL_SET("%d", action_size);

            SHR_IF_ERR_EXIT(
              bcm_field_internal_to_bcm_action_map(
                unit, 0, internal_action, bcm_action_size, bcm_action_stage, &bcm_action_size_actual));

            if (bcm_action_size_actual)
            {
                uint8 skip_columns = FALSE;
                for (bcm_action_index = 0 ; bcm_action_index < bcm_action_size_actual ; bcm_action_index++)
                {

                    SHR_IF_ERR_EXIT(_bcm_dpp_ppd_stage_from_bcm_stage(unitData, bcm_action_stage[bcm_action_index].bcm_stage, &bcm_to_ppc_fp_stage));
                    if (bcm_to_ppc_fp_stage != ppc_fp_stage)
                    {
                        continue;
                    }

                    if (skip_columns == TRUE)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(2);
                    }

                    PRT_CELL_SET("%s", (char *)_bcm_dpp_field_action_name[bcm_action_stage[bcm_action_index].bcm_action]);
                    if (skip_columns == TRUE)
                    {
                        PRT_CELL_SKIP(1);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", (char *)_bcm_dpp_field_stage_name[bcm_action_stage[bcm_action_index].bcm_stage]);
                        skip_columns = TRUE;
                    }
                }
            }
            else
            {
                PRT_CELL_SET("%s", "N/A");
                SHR_IF_ERR_EXIT(_bcm_dpp_field_internal_stage_to_bcm_stage(unit, ppc_fp_stage, &bcm_stage));
                PRT_CELL_SET("%s", (char *)_bcm_dpp_field_stage_name[bcm_stage]);
            }
        }
    }

    /* Coverity: We are not going to change the macros to avoid such defects. */
    /* coverity[dead_error_line]*/
    PRT_COMMIT;
    SHR_SET_CURRENT_ERR(_rv);
exit:
    PRT_FREE;
    return SHR_ERR_TO_SHELL;
}

cmd_result_t
  cmd_ppd_api_fp_action_show(int unit, args_t* a)
{
    uint32 ret;
    parse_table_t pt;
    int predefined = 0;
    SOC_PPC_FP_DATABASE_STAGE stage = SOC_PPC_NOF_FP_DATABASE_STAGES;
    uint32 bcm = 0;
    uint32 soc = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "predefined", PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &predefined, NULL);
    parse_table_add(&pt, "bcm", PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &bcm, NULL);
    parse_table_add(&pt, "soc", PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &soc, NULL);
    parse_table_add(&pt, "stage", PQ_DFL|PQ_NO_EQ_OPT|PQ_INT, &stage, &stage, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if (predefined)
    {
        if (bcm)
        {
            ret = cmd_ppd_api_fp_action_show_predefined_bcm(unit, stage);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
                return CMD_FAIL;
            }
        }
        else if (soc)
        {
            ret = cmd_ppd_api_fp_action_show_predefined_soc(unit, stage);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
                return CMD_FAIL;
            }
        }
        else
        {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }

    parse_arg_eq_done(&pt);
    return CMD_OK;
}

/********************************************************************
 *  Function handler: qualifier_show (section fp)
 */
/**
 * Function:
 *      cmd_ppd_api_fp_qualifier_show_qset_group_type_text
 * Purpose:
 *      This function returns name of the given qset_group_type.
 *
 * Parameters:
 *      qset_group_type - (IN) Qset group type, which name have to be returned.
 *                             Look at 'Notes' section for supported types.
 * Returns:
 *      \retval Name of the qset grp type       - On success
 *      \retval 'Invalid type'                  - On Failure
 * Notes:
 *      Supported types:
 *          _BCM_DPP_FIELD_GROUP_L2
 *          _BCM_DPP_FIELD_GROUP_IPV4
 *          _BCM_DPP_FIELD_GROUP_IPV6
 *          _BCM_DPP_FIELD_GROUP_MPLS
 */
static char*
  cmd_ppd_api_fp_qualifier_show_qset_group_type_text(
      uint32 qset_group_type
  )
{
    char *qset_group_type_name;


    switch (qset_group_type)
    {
        case 1:
        {
            qset_group_type_name = "L2";
            break;
        }
        case 2:
        {
            qset_group_type_name = "IPV4";
            break;
        }
        case 4:
        {
            qset_group_type_name = "IPV6";
            break;
        }
        case 8:
        {
            qset_group_type_name = "MPLS";
            break;
        }
        default:
        {
            qset_group_type_name = "Invalid Type";
            break;
        }
    }

    return qset_group_type_name;
}

/**
 * Function:
 *      cmd_ppd_api_fp_qualifier_show_bcm_to_ppd_qual
 * Purpose:
 *      This function returns PPD qualifier mapped to the
 *      given BCM qualifier.
 *
 * Parameters:
 *      unitData - (IN) Device ID.
 *      dpp_stage - (IN) Qualifier DPP stage.
 *      qset_group_type - (IN) QSET group type.
 *      bcm_qual - (IN) BCM qualifier type.
 *      ppd_qual_p - (OUT) In case of found it will store the relevant PPC qualifier
 *                         mapped to the given bcm_qual.
 *      found_p - (OUT) In case of TRUE the qualifier was found for current unit,
 *                      otherwise FALSE.
 * Returns:
 *      \retval Error code (as per 'shr_error_e').
 * Notes:
 *      shr_error_e
 */
static shr_error_e
cmd_ppd_api_fp_qualifier_show_bcm_to_ppd_qual(
      bcm_dpp_field_info_OLD_t *unitData,
      _bcm_dpp_field_stage_idx_t dpp_stage,
      uint32 qset_group_type,
      bcm_field_qualify_t bcm_qual,
      SOC_PPC_FP_QUAL_TYPE **ppd_qual_p,
      int *found_p)
{
    _bcm_dpp_field_type_idx_t type;
    _bcm_dpp_field_type_idx_t ltype;
    _bcm_dpp_field_type_idx_t limit;
    bcm_field_qset_t qsetType;
    int unit;

    SHR_FUNC_INIT_VARS(unitData->unit);

    *found_p = FALSE;
    unit = unitData->unit;

    if (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, dpp_stage).stageFlags & _BCM_DPP_FIELD_DEV_STAGE_USES_PRESELECTORS)
    {
        /** Stage uses preselectors, consider them. */
        limit = _BCM_DPP_FIELD_MAX_GROUP_TYPES + 1;
    }
    else
    {
        /** Stage does not use preselectors, do not consider them. */
        limit = unitData->devInfo->types;
    }

    type = limit;
    ltype = limit;
    while (type > 0)
    {
        type--;
        FIELD_ACCESS.stageD.qsetType.get(unit, dpp_stage, type, &qsetType);
        if ((qset_group_type & (1 << type)) && BCM_FIELD_QSET_TEST(qsetType, bcm_qual))
        {
            ltype = type;
            break;
        }
    }

    if (ltype >= limit)
    {
        *found_p = FALSE;
    }
    else
    {
        SHR_IF_ERR_EXIT(_bcm_dpp_ppd_qual_from_bcm_qual(unitData, dpp_stage, qset_group_type, bcm_qual, ppd_qual_p));
        *found_p = TRUE;
    }

exit:
    return SHR_ERR_TO_SHELL;
}

cmd_result_t
  cmd_ppd_api_fp_qualifier_show_predefined_bcm(
      int unit,
      SOC_PPC_FP_DATABASE_STAGE stage,
      uint32 qset_type)
{
    bcm_field_qualify_t bcm_qual;
    unsigned int qualifier_size;
    SOC_PPC_FP_DATABASE_STAGE ppc_fp_stage;
    bcm_dpp_field_info_OLD_t *unitData = NULL;
    int _rv = BCM_E_NONE;
    int32 qualMaps_handle;
    SOC_PPC_FP_QUAL_TYPE *ppdQual;
    uint32 ppd_qual_index;
    _bcm_dpp_field_stage_idx_t dpp_stage_index;
    bcm_field_stage_t bcm_stage;
    uint32 qset_group_types[_BCM_DPP_FIELD_MAX_GROUP_TYPES] = {1, 2, 4, 8};
    uint32 qset_group_type_index;
    uint8 new_bcm_qual_add;
    uint8 new_qset_grp_type_add;
    int found;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    new_bcm_qual_add = FALSE;
    new_qset_grp_type_add = FALSE;

    PRT_TITLE_SET("Qualifier Map");
    PRT_COLUMN_ADD("bcmQualify");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("SOC Qualifier");
    PRT_COLUMN_ADD("Qset Types");
    PRT_COLUMN_ADD("Stage");

    for (dpp_stage_index = 0; dpp_stage_index < 4; dpp_stage_index++)
    {
        ppc_fp_stage = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, dpp_stage_index).hwStageId;

        if (ppc_fp_stage != stage && stage != SOC_PPC_NOF_FP_DATABASE_STAGES)
        {
            continue;
        }

        for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
        {
            SHR_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, bcm_qual, &qualMaps_handle));
            if (!qualMaps_handle)
            {
                continue;
            }

            new_bcm_qual_add = TRUE;

            for (qset_group_type_index = 0; qset_group_type_index < _BCM_DPP_FIELD_MAX_GROUP_TYPES; qset_group_type_index++)
            {
                if (qset_group_types[qset_group_type_index] != qset_type && qset_type != 0)
                {
                    continue;
                }

                new_qset_grp_type_add = TRUE;
                SHR_IF_ERR_EXIT(cmd_ppd_api_fp_qualifier_show_bcm_to_ppd_qual(unitData, dpp_stage_index, qset_group_types[qset_group_type_index], bcm_qual, &ppdQual, &found));
                if (found == TRUE)
                {
                    for (ppd_qual_index = 0; ppd_qual_index < unitData->devInfo->qualChain; ppd_qual_index++ )
                    {
                        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[ppd_qual_index]))
                        {
                            SHR_IF_ERR_EXIT(_bcm_dpp_ppd_qual_bits(unit, dpp_stage_index, ppdQual[ppd_qual_index], &qualifier_size, NULL, NULL));
                            SHR_IF_ERR_EXIT(_bcm_dpp_bcm_stage_from_ppd_stage(unitData, ppc_fp_stage, &bcm_stage));

                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

                            if (new_bcm_qual_add)
                            {
                                PRT_CELL_SET("%s", _bcm_dpp_field_qual_name[bcm_qual]);
                                PRT_CELL_SET("%d", qualifier_size);
                            }
                            else
                            {
                                PRT_CELL_SKIP(2);
                            }

                            PRT_CELL_SET("%s", SOC_PPC_FP_QUAL_TYPE_to_string(ppdQual[ppd_qual_index]));

                            if (new_qset_grp_type_add)
                            {
                                PRT_CELL_SET("%s", cmd_ppd_api_fp_qualifier_show_qset_group_type_text(qset_group_types[qset_group_type_index]));
                                new_qset_grp_type_add = FALSE;
                            }
                            else
                            {
                                PRT_CELL_SKIP(1);
                            }

                            if (new_bcm_qual_add)
                            {
                                PRT_CELL_SET("%s", (char *)_bcm_dpp_field_stage_name[bcm_stage]);
                                new_bcm_qual_add = FALSE;
                            }
                            else
                            {
                                PRT_CELL_SKIP(1);
                            }
                        }
                    }
                }
            }
        }
    }

    /* Coverity: We are not going to change the macros to avoid such defects. */
    /* coverity[dead_error_line]*/
    PRT_COMMIT;
    SHR_SET_CURRENT_ERR(_rv);
exit:
    PRT_FREE;
    return SHR_ERR_TO_SHELL;
}

cmd_result_t
  cmd_ppd_api_fp_qualifier_show_predefined_soc(
      int unit,
      SOC_PPC_FP_DATABASE_STAGE stage,
      uint32 qset_type)
{
    bcm_field_qualify_t bcm_qual;
    unsigned int qualifier_size;
    SOC_PPC_FP_DATABASE_STAGE ppc_fp_stage;
    bcm_dpp_field_info_OLD_t *unitData = NULL;
    int _rv = BCM_E_NONE;
    int32 qualMaps_handle;
    SOC_PPC_FP_QUAL_TYPE *ppdQual;
    SOC_PPC_FP_QUAL_TYPE ppd_qual_index;
    uint32 chain_ppd_qual_index;
    _bcm_dpp_field_stage_idx_t dpp_stage_index;
    bcm_field_stage_t bcm_stage;
    uint32 qset_group_types[_BCM_DPP_FIELD_MAX_GROUP_TYPES] = {1, 2, 4, 8};
    uint32 qset_group_type_index;
    uint8 skip_columns;
    int found;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    PRT_TITLE_SET("Qualifier Map");
    PRT_COLUMN_ADD("SOC Qualifier");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("bcmQualify");
    PRT_COLUMN_ADD("Qset Types");
    PRT_COLUMN_ADD("Stage");

    for (dpp_stage_index = 0; dpp_stage_index < 4; dpp_stage_index++)
    {
        ppc_fp_stage = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, dpp_stage_index).hwStageId;

        if (ppc_fp_stage != stage && stage != SOC_PPC_NOF_FP_DATABASE_STAGES)
        {
            continue;
        }

        for (ppd_qual_index = 0; ppd_qual_index < SOC_PPC_NOF_FP_QUAL_TYPES; ppd_qual_index++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", SOC_PPC_FP_QUAL_TYPE_to_string(ppd_qual_index));

            skip_columns = FALSE;

            for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
            {
                SHR_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, bcm_qual, &qualMaps_handle));
                if (!qualMaps_handle)
                {
                    continue;
                }

                for (qset_group_type_index = 0; qset_group_type_index < _BCM_DPP_FIELD_MAX_GROUP_TYPES; qset_group_type_index++)
                {
                    if (qset_group_types[qset_group_type_index] != qset_type && qset_type != 0)
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(cmd_ppd_api_fp_qualifier_show_bcm_to_ppd_qual(unitData, dpp_stage_index, qset_group_types[qset_group_type_index], bcm_qual, &ppdQual, &found));
                    if (found == TRUE)
                    {
                        for (chain_ppd_qual_index = 0; chain_ppd_qual_index < unitData->devInfo->qualChain; chain_ppd_qual_index++ )
                        {
                            if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[chain_ppd_qual_index]))
                            {
                                if (ppdQual[chain_ppd_qual_index] != ppd_qual_index)
                                {
                                    continue;
                                }

                                if (skip_columns == TRUE)
                                {
                                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                    PRT_CELL_SKIP(3);
                                }
                                else
                                {
                                    SHR_IF_ERR_EXIT(_bcm_dpp_ppd_qual_bits(unit, dpp_stage_index, ppd_qual_index, &qualifier_size, NULL, NULL));
                                    PRT_CELL_SET("%d", qualifier_size);
                                    PRT_CELL_SET("%s", _bcm_dpp_field_qual_name[bcm_qual]);
                                }

                                PRT_CELL_SET("%s", cmd_ppd_api_fp_qualifier_show_qset_group_type_text(qset_group_types[qset_group_type_index]));

                                if (skip_columns == TRUE)
                                {
                                    PRT_CELL_SKIP(1);
                                }
                                else
                                {
                                    SHR_IF_ERR_EXIT(_bcm_dpp_bcm_stage_from_ppd_stage(unitData, ppc_fp_stage, &bcm_stage));
                                    PRT_CELL_SET("%s", (char *)_bcm_dpp_field_stage_name[bcm_stage]);
                                    skip_columns = TRUE;
                                }
                            }
                        }
                    }
                }
            }

            if (skip_columns == FALSE)
            {
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                SHR_IF_ERR_EXIT(_bcm_dpp_bcm_stage_from_ppd_stage(unitData, ppc_fp_stage, &bcm_stage));
                PRT_CELL_SET("%s", (char *)_bcm_dpp_field_stage_name[bcm_stage]);
            }
        }
    }

    /* Coverity: We are not going to change the macros to avoid such defects. */
    /* coverity[dead_error_line]*/
    PRT_COMMIT;
    SHR_SET_CURRENT_ERR(_rv);
exit:
    PRT_FREE;
    return SHR_ERR_TO_SHELL;
}

cmd_result_t
  cmd_ppd_api_fp_qualifier_show_preselectors(
      int unit,
      SOC_PPC_FP_DATABASE_STAGE stage)
{
    bcm_field_qualify_t bcm_qual;
    unsigned int qualifier_size;
    SOC_PPC_FP_DATABASE_STAGE ppc_fp_stage;
    bcm_dpp_field_info_OLD_t *unitData = NULL;
    int _rv = BCM_E_NONE;
    int32 qualMaps_handle;
    SOC_PPC_FP_QUAL_TYPE *ppdQual;
    uint32 ppd_qual_index;
    _bcm_dpp_field_stage_idx_t dpp_stage_index;
    bcm_field_stage_t bcm_stage;
    uint8 new_bcm_qual_add;
    int found;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    new_bcm_qual_add = FALSE;

    PRT_TITLE_SET("Qualifier Map");
    PRT_COLUMN_ADD("bcmQualify");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("SOC Qualifier");
    PRT_COLUMN_ADD("Stage");

    for (dpp_stage_index = 0; dpp_stage_index < 4; dpp_stage_index++)
    {
        ppc_fp_stage = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, dpp_stage_index).hwStageId;

        if (ppc_fp_stage != stage && stage != SOC_PPC_NOF_FP_DATABASE_STAGES)
        {
            continue;
        }

        for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
        {
            SHR_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, bcm_qual, &qualMaps_handle));
            if (!qualMaps_handle)
            {
                continue;
            }

            new_bcm_qual_add = TRUE;
            SHR_IF_ERR_EXIT(cmd_ppd_api_fp_qualifier_show_bcm_to_ppd_qual(unitData, dpp_stage_index, 16, bcm_qual, &ppdQual, &found));
            if (found == TRUE)
            {
                for (ppd_qual_index = 0; ppd_qual_index < unitData->devInfo->qualChain; ppd_qual_index++ )
                {
                    if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[ppd_qual_index]))
                    {
                        SHR_IF_ERR_EXIT(_bcm_dpp_ppd_qual_bits(unit, dpp_stage_index, ppdQual[ppd_qual_index], &qualifier_size, NULL, NULL));
                        SHR_IF_ERR_EXIT(_bcm_dpp_bcm_stage_from_ppd_stage(unitData, ppc_fp_stage, &bcm_stage));

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

                        if (new_bcm_qual_add)
                        {
                            PRT_CELL_SET("%s", _bcm_dpp_field_qual_name[bcm_qual]);
                            PRT_CELL_SET("%d", qualifier_size);
                        }
                        else
                        {
                            PRT_CELL_SKIP(2);
                        }

                        PRT_CELL_SET("%s", SOC_PPC_FP_QUAL_TYPE_to_string(ppdQual[ppd_qual_index]));

                        if (new_bcm_qual_add)
                        {
                            PRT_CELL_SET("%s", (char *)_bcm_dpp_field_stage_name[bcm_stage]);
                            new_bcm_qual_add = FALSE;
                        }
                        else
                        {
                            PRT_CELL_SKIP(1);
                        }
                    }
                }
            }
        }
    }

    /* Coverity: We are not going to change the macros to avoid such defects. */
    /* coverity[dead_error_line]*/
    PRT_COMMIT;
    SHR_SET_CURRENT_ERR(_rv);
exit:
    PRT_FREE;
    return SHR_ERR_TO_SHELL;
}

cmd_result_t
  cmd_ppd_api_fp_qualifier_show(int unit, args_t* a)
{
    uint32 ret;
    parse_table_t pt;
    int predefined = 0;
    SOC_PPC_FP_DATABASE_STAGE stage = SOC_PPC_NOF_FP_DATABASE_STAGES;
    uint32 qset_type = 0;
    uint32 bcm = 0;
    uint32 soc = 0;
    uint32 preselectors = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "predefined", PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &predefined, NULL);
    parse_table_add(&pt, "bcm", PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &bcm, NULL);
    parse_table_add(&pt, "soc", PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &soc, NULL);
    parse_table_add(&pt, "preselectors", PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &preselectors, NULL);
    parse_table_add(&pt, "stage", PQ_DFL|PQ_NO_EQ_OPT|PQ_INT, &stage, &stage, NULL);
    parse_table_add(&pt, "qset_type", PQ_DFL|PQ_NO_EQ_OPT|PQ_INT, &qset_type, &qset_type, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if (predefined)
    {
        if (bcm)
        {
            ret = cmd_ppd_api_fp_qualifier_show_predefined_bcm(unit, stage, qset_type);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
                return CMD_FAIL;
            }
        }
        else if (soc)
        {
            ret = cmd_ppd_api_fp_qualifier_show_predefined_soc(unit, stage, qset_type);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
                return CMD_FAIL;
            }
        }
        else
        {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }
    else if (preselectors)
    {
        ret = cmd_ppd_api_fp_qualifier_show_preselectors(unit, stage);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
            return CMD_FAIL;
        }
    }
    else
    {
        cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);
    return CMD_OK;
}

/********************************************************************
 *  Function handler: print_all_fems_for_stage (section fp)
 *
 * Note:
 *   We use, below, _shr_ctoi() instead of the standard atoi() (sal_atoi())
 *   because, otherwise, compilation fails for '--kernel'
 */

cmd_result_t 
  cmd_ppd_api_fp_print_all_fems_for_stage(int unit, args_t* a)  
{   
  uint32
    val,
    ret,
    stage,
    is_for_tm ;   
  char
    *arg_mode ;

  ret = CMD_USAGE ;
  arg_mode = ARG_GET(a) ;
  /*
   * First parameter is 'stage'. See SOC_PPC_FP_DATABASE_STAGE.
   */
  if (arg_mode) {
    stage = (uint32)_shr_ctoi(arg_mode) ;
  } else {
    cli_out("\r\n") ;
    cli_out("%s: Missing first parameter. Quit.\r\n",__func__) ;
    cli_out("\r\n") ;
    goto exit ;
  }
  arg_mode = ARG_GET(a) ;
  /*
   * Second parameter 'is_for_tm':
   *   Set 1 for TM processing (packets with ITMH header)
   *   Set 0 for stacking or TDM processing (packets with FTMH header)
   */
  if (arg_mode) {
    is_for_tm = (uint32)_shr_ctoi(arg_mode) ;
  } else {
    cli_out("\r\n") ;
    cli_out("%s: Missing second parameter. Quit.\r\n",__func__) ;
    cli_out("\r\n") ;
    goto exit ;
  }
  ret = CMD_FAIL ; 
  /* Call function */
  val = soc_ppd_fp_print_all_fems_for_stage(unit,stage,is_for_tm) ;
  if (soc_sand_get_error_code_from_error_word(val) != SOC_SAND_OK) 
  {
    goto exit ;
  } 
  ret = CMD_OK ; 
exit:
  return (ret) ;
}

/******************************************************************** 
 *  Function handler: print_fes_info_for_stage (section fp)
 *
 * Note:
 *   We use, below, _shr_ctoi() instead of the standard atoi() (sal_atoi())
 *   because, otherwise, compilation fails for '--kernel'
 */

cmd_result_t 
  cmd_ppd_api_fp_print_fes_info_for_stage(int unit, args_t* a)  
{   
  uint32
    val,
    ret,
    stage,
    pmf_pgm_ndx ;   
  char
    *arg_mode ;

  ret = CMD_USAGE ;
  arg_mode = ARG_GET(a) ;
  /*
   * First parameter is 'stage'. See SOC_PPC_FP_DATABASE_STAGE.
   */
  if (arg_mode) {
    stage = (uint32)_shr_ctoi(arg_mode) ;
  } else {
    cli_out("\r\n") ;
    cli_out("%s: Missing first parameter. Quit.\r\n",__func__) ;
    cli_out("\r\n") ;
    goto exit ;
  }
  /*
   * Second parameter is 'PMF program'. This is a programmable parameter.
   * See SOC_DPP_DEFS_MAX(NOF_INGRESS_PMF_PROGRAMS).
   */
  arg_mode = ARG_GET(a) ;
  if (arg_mode) {
    pmf_pgm_ndx = (uint32)_shr_ctoi(arg_mode) ;
  } else {
    cli_out("\r\n") ;
    cli_out("%s: Missing second parameter. Quit.\r\n",__func__) ;
    cli_out("\r\n") ;
    goto exit ;
  }
  ret = CMD_FAIL ; 
  /* Call function */
  val = soc_ppd_fp_print_fes_info_for_stage(unit,stage,pmf_pgm_ndx) ;
  if (soc_sand_get_error_code_from_error_word(val) != SOC_SAND_OK) 
  {
    goto exit ;
  } 
  ret = CMD_OK ; 
exit:
  return (ret) ;
}

/******************************************************************** 
 *  Function handler: resource_diag_get (section fp)
 */

cmd_result_t 
  cmd_ppd_api_fp_resource_diag_get(int unit, args_t* a)  
{   
  uint32 
    soc_sand_dev_id,
    mode,
    ret;   
  SOC_PPC_FP_RESOURCE_DIAG_INFO   
    *prm_info;
  char *arg_mode;

  prm_info = sal_alloc(sizeof(SOC_PPC_FP_RESOURCE_DIAG_INFO), "cmd_ppd_api_fp_resource_diag_get.prm_info");
  if(prm_info == NULL) {
    cli_out("Memory allocation failure\n");
    return CMD_FAIL;
  }
  
  SOC_PPC_FP_RESOURCE_DIAG_INFO_clear(prm_info);
  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  arg_mode = ARG_GET(a);
  if (! arg_mode ) {
      mode = 0;
  /*
   * Make sure at least one of the strings is NULL terminated.
   */
  } else if (! sal_strcasecmp(arg_mode, "1")) {
      mode = 1;
  } else if (! sal_strcasecmp(arg_mode, "2")) {
      mode = 2;
  } else if (! sal_strcasecmp(arg_mode, "3")) {
      mode = 3;
  } else {
      mode = 0;
  } 

  /* Call function */
  ret = soc_ppd_fp_resource_diag_get(
          soc_sand_dev_id,
          mode,
          prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  {
      sal_free(prm_info); 
      return CMD_FAIL; 
  } 

  SOC_PPC_FP_RESOURCE_DIAG_INFO_print(unit, prm_info);
  sal_free(prm_info);
  
  return CMD_OK; 
}


extern cmd_result_t _bcm_petra_field_test_qualify_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_action_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
extern cmd_result_t _bcm_petra_field_test_field_group_2(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
extern cmd_result_t _bcm_petra_field_test_field_group_destroy(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_destroy_with_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_entry_traffic_perf(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_entry_priority(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
extern cmd_result_t _bcm_petra_field_test_entry_priority_2(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
extern cmd_result_t _bcm_petra_field_test_shared_bank(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_priority(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_destroy_with_traffic_and_de_fg(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_cascaded(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_presel(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_presel_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_presel(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_presel_1(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifier_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_direct_extraction(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_de_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_de_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_de_entry_traffic_large(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_direct_table(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_direct_table_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_direct_table_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_full_tcam(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_full_tcam_perf(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_full_tcam_diff_prio(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_compress(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_field_group(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_field_group_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_parsing_test(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_parsing_test_pb(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_resend_last_packet(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_user_header(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
STATIC cmd_result_t
cmd_diag_test(int unit, args_t* a)
{
    cmd_result_t res;
    cmd_result_t(*all_tests_func_ingress[])(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot) = {
        _bcm_petra_field_test_qualify_set,
        _bcm_petra_field_test_action_set,
        _bcm_petra_field_test_field_group,
        _bcm_petra_field_test_field_group_destroy,
        _bcm_petra_field_test_field_group_destroy_with_traffic,
        _bcm_petra_field_test_entry,
        _bcm_petra_field_test_entry_traffic,
        _bcm_petra_field_test_entry_traffic_perf,
        _bcm_petra_field_test_entry_priority,
        _bcm_petra_field_test_shared_bank,
        _bcm_petra_field_test_field_group_priority,
        _bcm_petra_field_test_cascaded,
        _bcm_petra_field_test_presel,
        _bcm_petra_field_test_presel_set,
        _bcm_petra_field_test_field_group_presel,
        _bcm_petra_field_test_data_qualifiers,
        _bcm_petra_field_test_data_qualifier_set,
        _bcm_petra_field_test_data_qualifiers_entry,
        _bcm_petra_field_test_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_field_group_direct_extraction,
        _bcm_petra_field_test_de_entry,
        _bcm_petra_field_test_de_entry_traffic,
        _bcm_petra_field_test_de_entry_traffic_large,
        _bcm_petra_field_test_field_group_direct_table,
        _bcm_petra_field_test_full_tcam,
        _bcm_petra_field_test_full_tcam_perf,
        _bcm_petra_field_test_full_tcam_diff_prio,
        _bcm_petra_field_test_compress,
        _bcm_petra_field_test_itmh_field_group_traffic,
        _bcm_petra_field_test_itmh_parsing_test,
        _bcm_petra_field_test_itmh_parsing_test_pb,
        _bcm_petra_field_test_user_header
    };

    cmd_result_t(*all_tests_func_egress[])(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot) = {
        _bcm_petra_field_test_qualify_set,
        _bcm_petra_field_test_action_set,
        _bcm_petra_field_test_field_group,
        _bcm_petra_field_test_field_group_destroy,
        _bcm_petra_field_test_entry,
        _bcm_petra_field_test_entry_traffic,
        _bcm_petra_field_test_entry_traffic_perf,
        _bcm_petra_field_test_presel,
        _bcm_petra_field_test_presel_set,
        _bcm_petra_field_test_data_qualifiers,
        _bcm_petra_field_test_data_qualifier_set,
        _bcm_petra_field_test_data_qualifiers_entry,
        _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic
    };
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    cmd_result_t(*all_tests_func_external[])(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot) = {
        _bcm_petra_field_test_qualify_set,
        _bcm_petra_field_test_action_set,
        _bcm_petra_field_test_field_group,
        _bcm_petra_field_test_field_group_2,
        _bcm_petra_field_test_entry,
        _bcm_petra_field_test_entry_traffic,
        _bcm_petra_field_test_entry_traffic_perf,
        _bcm_petra_field_test_entry_priority,
        _bcm_petra_field_test_entry_priority_2,
        _bcm_petra_field_test_data_qualifiers,
        _bcm_petra_field_test_data_qualifier_set,
        _bcm_petra_field_test_data_qualifiers_entry,
        _bcm_petra_field_test_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_full_tcam,
        _bcm_petra_field_test_full_tcam_perf,
        _bcm_petra_field_test_full_tcam_diff_prio
    };
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

    uint32 i;
    uint32 x, i_mode;
    uint8 i_stage;
    char *mode;
    char *stage;
    parse_table_t pt;
    char *option;
    uint32 do_warmboot = 0;
    int core=0;

    uint32 is_deterministic;
    uint32 srand_value;

    option = ARG_GET(a);
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "x", PQ_INT, (void *) (0),
            &x, NULL);
    parse_table_add(&pt, "Mode", PQ_STRING, (void *) "FAST",
          &mode, NULL);
    parse_table_add(&pt, "Stage", PQ_STRING, (void *) "INGRESS",
          &stage, NULL);
    parse_table_add(&pt, "core", PQ_INT, 0,
          &core, NULL);
    parse_table_add(&pt, "is_deterministic", PQ_INT, (void *) (0),
          &is_deterministic, NULL);
    parse_table_add(&pt, "Srand", PQ_INT, (void *) (5),
          &srand_value, NULL);

#ifdef BCM_WARM_BOOT_SUPPORT
    parse_table_add(&pt, "Warmboot", PQ_INT, (void *) (0),
          &do_warmboot, NULL);
#endif
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if (ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    
    if(is_deterministic) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "The test is deterministic\n")));
        sal_srand(srand_value);
    } else {
        srand_value = sal_time();
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "The test is not deterministic. srand_value = %d.\n"), srand_value));
        sal_srand(srand_value);

    }
    
#ifndef NO_SAL_APPL
/* { */
    {
        int strnlen_option;
        int strnlen_mode;
        int strnlen_stage;
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_mode = sal_strnlen(mode, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if(! sal_strncasecmp(mode, "SUPER_FAST", strnlen_mode) ) {
            i_mode = 0;
        } else if(! sal_strncasecmp(mode, "FAST", strnlen_mode) ) {
            i_mode = 1;
        } else if(! sal_strncasecmp(mode, "MEDIUM", strnlen_mode) ) {
            i_mode = 2;
        } else if(! sal_strncasecmp(mode, "SLOW", strnlen_mode) ) {
            i_mode = 3;
        } else if(! sal_strncasecmp(mode, "SCAN", strnlen_mode) ) {
            i_mode = 4;
        } else {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_stage = sal_strnlen(stage, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if(! sal_strncasecmp(stage, "INGRESS", strnlen_stage) ) {
            i_stage = 0;
        } else if(! sal_strncasecmp(stage, "EGRESS", strnlen_stage) ) {
            i_stage = 1;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        } else if(! sal_strncasecmp(stage, "EXTERNAL", strnlen_stage) ) {
            i_stage = 2;
#endif /*defined(INCLUDE_KBP) && !defined(BCM_88030) */
        } else {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if(!option) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if(! sal_strncasecmp(option, "all", strnlen_option) ){
            if(i_stage == 0) {
                /* ingress */
                for(i=0; i < sizeof(all_tests_func_ingress)/sizeof(all_tests_func_ingress[0]); ++i) {
                    res = all_tests_func_ingress[i](unit, i_stage, x, i_mode, do_warmboot);
                    if(res != CMD_OK) {
                        parse_arg_eq_done(&pt);
                        return res;
                    }
                }
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
            } else if(i_stage == 2) {
                /* external */
                for(i=0; i < sizeof(all_tests_func_external)/sizeof(all_tests_func_external[0]); ++i) {
                    res = all_tests_func_ingress[i](unit, i_stage, x, i_mode, do_warmboot);
                    if(res != CMD_OK) {
                        parse_arg_eq_done(&pt);
                        return res;
                    }
                }
#endif /*defined(INCLUDE_KBP) && !defined(BCM_88030) */
            } else {
                /* egress */
                for(i=0; i < sizeof(all_tests_func_egress)/sizeof(all_tests_func_egress[0]); ++i) {
                    res = all_tests_func_egress[i](unit, i_stage, x, i_mode, do_warmboot);
                    if(res != CMD_OK) {
                        parse_arg_eq_done(&pt);
                        return res;
                    }
                }
            }            
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if(! sal_strncasecmp(option, "qset", strnlen_option) ){
            return _bcm_petra_field_test_qualify_set(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "aset", strnlen_option) ){
            return _bcm_petra_field_test_action_set(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "field_group", strnlen_option) ){
            return _bcm_petra_field_test_field_group(unit, i_stage, x, i_mode, do_warmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        } else if(! sal_strncasecmp(option, "field_group_2", strnlen_option) ){
            return _bcm_petra_field_test_field_group_2(unit, i_stage, x, i_mode, do_warmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
        } else if(! sal_strncasecmp(option, "field_group_destroy", strnlen_option) ){
            return _bcm_petra_field_test_field_group_destroy(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "field_group_destroy_traffic", strnlen_option) ){
            return _bcm_petra_field_test_field_group_destroy_with_traffic(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "field_group_destroy_traffic_de", strnlen_option) ){
            return _bcm_petra_field_test_field_group_destroy_with_traffic_and_de_fg(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "entry", strnlen_option) ){
            return _bcm_petra_field_test_entry(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "entry_traffic", strnlen_option) ){
            return _bcm_petra_field_test_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "entry_traffic_perf", strnlen_option) ){
            return _bcm_petra_field_test_entry_traffic_perf(unit, i_stage, 0x01, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "entry_priority", strnlen_option) ){
            return _bcm_petra_field_test_entry_priority(unit, i_stage, x, i_mode, do_warmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        } else if(! sal_strncasecmp(option, "entry_priority_2", strnlen_option) ){
            return _bcm_petra_field_test_entry_priority_2(unit, i_stage, x, i_mode, do_warmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
        } else if(! sal_strncasecmp(option, "shared_bank", strnlen_option) ){
            return _bcm_petra_field_test_shared_bank(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "field_group_priority", strnlen_option) ){
            return _bcm_petra_field_test_field_group_priority(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "cascaded", strnlen_option) ){
            return _bcm_petra_field_test_cascaded(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "presel", strnlen_option) ){
            return _bcm_petra_field_test_presel(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "presel_set", strnlen_option) ){
            return _bcm_petra_field_test_presel_set(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "field_group_presel", strnlen_option) ){
            return _bcm_petra_field_test_field_group_presel(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "field_group_presel_1", strnlen_option) ){
            return _bcm_petra_field_test_field_group_presel_1(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "data_qualifiers", strnlen_option) ){
            return _bcm_petra_field_test_data_qualifiers(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "data_qualifier_set", strnlen_option) ){
            return _bcm_petra_field_test_data_qualifier_set(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "data_qualifiers_entry", strnlen_option) ){
            return _bcm_petra_field_test_data_qualifiers_entry(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "data_qualifiers_entry_traffic", strnlen_option) ){
            return _bcm_petra_field_test_data_qualifiers_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "predefined_data_qualifiers_entry_traffic", strnlen_option) ){
            return _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "direct_extraction_field_group", strnlen_option) ){
            return _bcm_petra_field_test_field_group_direct_extraction(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "direct_extraction_entry", strnlen_option) ){
            return _bcm_petra_field_test_de_entry(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "direct_extraction_entry_traffic", strnlen_option) ){
            return _bcm_petra_field_test_de_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "direct_extraction_entry_traffic_large", strnlen_option) ){
            return _bcm_petra_field_test_de_entry_traffic_large(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "direct_table_field_group", strnlen_option) ){
            return _bcm_petra_field_test_field_group_direct_table(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "direct_table_entry", strnlen_option) ){
            return _bcm_petra_field_test_direct_table_entry(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "direct_table_entry_traffic", strnlen_option) ){
            return _bcm_petra_field_test_direct_table_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "full_tcam", strnlen_option) ){
            return _bcm_petra_field_test_full_tcam(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "full_tcam_perf", strnlen_option) ){
            return _bcm_petra_field_test_full_tcam_perf(unit, i_stage, 0x01, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "full_tcam_diff_prio", strnlen_option) ){
            return _bcm_petra_field_test_full_tcam_diff_prio(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "compress", strnlen_option) ){
            return _bcm_petra_field_test_compress(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "itmh_field_group", strnlen_option) ){
            return _bcm_petra_field_test_itmh_field_group(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "itmh_field_group_traffic", strnlen_option) ){
            return _bcm_petra_field_test_itmh_field_group_traffic(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "itmh_parsing_test", strnlen_option) ){
            return _bcm_petra_field_test_itmh_parsing_test(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "itmh_parsing_test_pb", strnlen_option) ){
            return _bcm_petra_field_test_itmh_parsing_test_pb(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "user_header", strnlen_option) ){
            return _bcm_petra_field_test_user_header(unit, i_stage, x, i_mode, do_warmboot);
        } else if(! sal_strncasecmp(option, "resend_last_packet", strnlen_option) ){
            return _bcm_petra_field_test_resend_last_packet(unit, i_stage, x, i_mode, do_warmboot);
        } else {
        parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }
#else
     cli_out("Option parsing is not supported when NO_SAL_APPL is defined\n");
     return CMD_USAGE;
#endif
}

static void
cmd_dpp_field_get_qual_32bit(char *str_data, char *str_mask, int data_size, uint32 data, uint32 mask)
{
    uint32 number = 0;
    int i;

    for(i = 0; i < data_size; i++)
        number |= 1 << i;

    mask &= number;
    data &= number;

    if(mask == 0) {
        /* Qualifier fully masked */
        SET_EMPTY(str_data);
        SET_EMPTY(str_mask);
    } else if(mask == number) {
        /* Qualifier fully unmasked */
        sprintf(str_data, "%X", data);
        SET_EMPTY(str_mask);
    } else {
        /* Partial mask */
        sprintf(str_data, "%X", data);
        sprintf(str_mask, "%X", mask);
    }

    return;
}

static void
_dpp_field_get_qual_str(SOC_PPC_FP_QUAL_VAL *qual_val, char *str, int data_size)
{
    char str_data[36], str_mask[36];

    if(data_size <= 32) {
        cmd_dpp_field_get_qual_32bit(str_data, str_mask,
                                     data_size, qual_val->val.arr[0], qual_val->is_valid.arr[0]);
    } else if(data_size <= 64) {
        cmd_dpp_field_get_qual_32bit(str_data, str_mask,
                                     data_size - 32, qual_val->val.arr[1], qual_val->is_valid.arr[1]);
        cmd_dpp_field_get_qual_32bit(str_data + strlen(str_data), str_mask + strlen(str_mask),
                                     32, qual_val->val.arr[0], qual_val->is_valid.arr[0]);
    } else {
        cli_out("Qualifier size:%d\n", data_size);
    }

    if(ISEMPTY(str_mask))
        sprintf(str, "%s", str_data);
    else
        sprintf(str, "%s/%s", str_data, str_mask);

    return;
}

static void
_dpp_field_get_qual_list(SOC_PPC_FP_DATABASE_INFO *dbInfo_p, char **qual_str, int *qual_num_p)
{
    int i;
    int qual_num = 0;
    sal_memset(qual_str, 0, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX * sizeof(char *));
    for (i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(dbInfo_p->qual_types[i])) {
            qual_str[qual_num] = (char *)SOC_PPC_FP_QUAL_TYPE_to_string(dbInfo_p->qual_types[i]);
            qual_num++;
        }
    }

    *qual_num_p = qual_num;
    return;
}

static void
_dpp_field_get_action_list(SOC_PPC_FP_DATABASE_INFO *dbInfo_p, char **action_str, int *action_num_p)
{
    int i;
    int action_num = 0;
    sal_memset(action_str, 0, SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX * sizeof(char *));
    for (i = 0; i < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; i++) {
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(dbInfo_p->action_types[i])) {
            action_str[action_num] = (char *)SOC_PPC_FP_ACTION_TYPE_to_string(dbInfo_p->action_types[i]);
            action_num++;
        }
    }

    *action_num_p = action_num;
    return;
}

static void _dpp_get_range_flags_str(int flags, char *flags_str)
{
    SET_EMPTY(flags_str);

    if(flags & BCM_FIELD_RANGE_TCP) {
        strcat(flags_str, "TCP");
    }
    if(flags & BCM_FIELD_RANGE_UDP) {
        strcat(flags_str, " UDP");
    }
    if(flags & BCM_FIELD_RANGE_SRCPORT) {
        strcat(flags_str, " SrcPort");
    }
    if(flags & BCM_FIELD_RANGE_DSTPORT) {
        strcat(flags_str, " DstPort");
    }
    if(flags & BCM_FIELD_RANGE_PACKET_LENGTH) {
        strcat(flags_str, " PacketLength");
    }
}

static cmd_result_t
cmd_dpp_field_ranges_dump(int unit)
{
    bcm_field_range_t range;
    int index, actual, offset;
    int result;
    uint32 flags[_BCM_DPP_FIELD_RANGE_CHAIN_MAX];
    uint32 rangeMin[_BCM_DPP_FIELD_RANGE_CHAIN_MAX];
    uint32 rangeMax[_BCM_DPP_FIELD_RANGE_CHAIN_MAX];
    char flags_str[40];
    bcm_dpp_field_info_OLD_t *unitData;
    const _bcm_dpp_field_device_range_info_t *ranges;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    unitData = _bcm_dpp_field_unit_info[unit];
    ranges = unitData->devInfo->ranges;

    PRT_TITLE_SET("Range Dump");

    PRT_COLUMN_ADD("Qualifier");
    PRT_COLUMN_ADD("Range");
    PRT_COLUMN_ADD("Flags");
    PRT_COLUMN_ADD("Min");
    PRT_COLUMN_ADD("Max");

    for (index = 0; ranges[index].qualifier < bcmFieldQualifyCount; index++) {
        for (range = ranges[index].rangeBase; range < ranges[index].rangeBase + ranges[index].count; range++) {
            result = _bcm_dpp_field_range_get(unitData, range, _BCM_DPP_FIELD_RANGE_CHAIN_MAX,
                                              &(flags[0]), &(rangeMin[0]), &(rangeMax[0]), &actual);
            if (BCM_E_NONE == result) {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s(%d)", _bcm_dpp_field_qual_name[ranges[index].qualifier], ranges[index].qualifier);
                PRT_CELL_SET("%d", range);
                for (offset = 0; offset < actual; offset++) {
                    if(offset) {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(2);
                    }

                    _dpp_get_range_flags_str(flags[offset], flags_str);
                    PRT_CELL_SET("%s", flags_str);
                    PRT_CELL_SET("%d", rangeMin[offset]);
                    PRT_CELL_SET("%d", rangeMax[offset]);
                }
            }
        } /* for (all ranges of this type) */
    } /* for (all types of ranges) */

    /* coverity[stack_use_overflow : FALSE] */
    PRT_COMMIT;
exit:
    PRT_FREE;
    return SHR_ERR_TO_SHELL;
}

static cmd_result_t
cmd_dpp_field_table_list(int unit)
{
    uint32 soc_sandResult;
    int result;
    uint32 priority, key_size = 0, entry_size;
    SOC_PPC_FP_DATABASE_INFO    *dbInfo = NULL;
    SOC_PPC_FP_DATABASE_STAGE   stage;
    ARAD_FP_ENTRY               fp_entry;

    int i;
    char *qual_str[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int qual_num;
    char *action_str[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    int action_num;
    int line_num;
    int db_id, tcam_db_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Table List");

    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Stage");
    PRT_COLUMN_ADD("Priority");
    PRT_COLUMN_ADD("Key Size");
    PRT_COLUMN_ADD("NOF");
    PRT_COLUMN_ADD("Qualifiers");
    PRT_COLUMN_ADD("Actions");

    if((dbInfo = sal_alloc(sizeof(SOC_PPC_FP_DATABASE_INFO), "DB info")) == NULL) {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate SOC_PPC_FP_DATABASE_INFO\n");
    }

    for (db_id = 0; db_id < SOC_PPC_FP_NOF_DBS; db_id++) {
        sal_memset(dbInfo, 0, sizeof(SOC_PPC_FP_DATABASE_INFO));
        soc_sandResult = soc_ppd_fp_database_get(unit, db_id, dbInfo);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "DB %3d: unable to read %d (%s)\n"),
                      db_id, result, _SHR_ERRMSG(result)));
            continue;
        }
        if (SOC_PPC_NOF_FP_DATABASE_TYPES == dbInfo->db_type) {
            continue;
        }
        else if (SOC_PPC_NOF_FP_DATABASE_TYPES < dbInfo->db_type) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit,"DB %3d: unexpected type %d\n"),
                      db_id, dbInfo->db_type));
            continue;
        }

        soc_sandResult = arad_pp_fp_db_stage_get(unit, db_id, &stage);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "DB %3d: unable to get stage %d (%s)\n"),
                      db_id, result, _SHR_ERRMSG(result)));
            continue;
        }

        sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_entries.get(unit, stage, db_id, &fp_entry);
        sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.prio.get(unit, stage, db_id, &priority);

        /* Key size varies according to DB type */
        if(dbInfo->db_type == SOC_PPC_FP_DB_TYPE_TCAM || dbInfo->db_type == SOC_PPC_FP_DB_TYPE_EGRESS) {
            tcam_db_id = ARAD_PP_FP_DB_ID_TO_TCAM_DB(db_id);
            sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit,tcam_db_id, &entry_size);
            key_size = ARAD_SW_DB_ENTRY_SIZE_ID_TO_BITS(entry_size);
        } else if(dbInfo->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE) {
            key_size = (dbInfo->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS) ? ARAD_PP_FP_DIRECT_TABLE_KAPS_KEY_LENGTH : SOC_DPP_DEFS_GET(unit, tcam_big_bank_key_nof_bits);
        } else if(dbInfo->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION) {
            key_size = 32;
        }

        _dpp_field_get_qual_list(dbInfo, qual_str, &qual_num);
        _dpp_field_get_action_list(dbInfo, action_str, &action_num);
        line_num = action_num > qual_num ? action_num : qual_num;
        for(i = 0; i < line_num; i++) {
            if(i == line_num - 1) {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            }
            else {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }
            if(i == 0) {
                PRT_CELL_SET("%d", db_id);
                PRT_CELL_SET("%s", SOC_PPC_FP_DATABASE_TYPE_to_string(dbInfo->db_type));
                PRT_CELL_SET("%s", SOC_PPC_FP_DATABASE_STAGE_to_string(stage));
                PRT_CELL_SET("%d", priority);
                PRT_CELL_SET("%d", key_size);
                PRT_CELL_SET("%d", fp_entry.nof_db_entries);
            }
            else {
                PRT_CELL_SKIP(6);
            }

            if((qual_str[i] != NULL) && (i < qual_num))
            {
                PRT_CELL_SET("%s", qual_str[i]);
            }
            else
            {
                PRT_CELL_SKIP(1);
            }
            if((action_str[i] != NULL) && (i < action_num))
            {
                PRT_CELL_SET("%s", action_str[i]);
            }
            else
            {
                PRT_CELL_SKIP(1);
            }
        }
    }

    /* Coverity: We are not going to change the macros to aviod such defects */
    /* coverity[dead_error_line]*/
    PRT_COMMIT;
exit:
    if(dbInfo != NULL)
        sal_free(dbInfo);
    PRT_FREE;
    return SHR_ERR_TO_SHELL;
}

static cmd_result_t
cmd_dpp_field_table_dump(int unit, int table_id, int start_entry_id, int output_num)
{
    uint32 soc_sandResult;
    uint32 soc_sandOffset;
    uint8 okay;
    SOC_PPC_FP_DATABASE_INFO    *dbInfo = NULL;
    SOC_PPC_FP_DATABASE_STAGE   stage;
    int action_iter, db_iter, qual_iter;
    int count;
    int result;
    uint32 qual_size[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    char *qual_str[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int qual_num;
    char *action_str[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    SOC_PPC_FP_ACTION_VAL actions_value[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    int action_num;
    char qual_val_str[36];
	char qual_vals_list[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX][36];
    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *entInfoDe = NULL;
    SOC_PPC_FP_ENTRY_INFO *entInfoTc = NULL;

    _bcm_dpp_field_ent_idx_t entryLimit;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    soc_sandResult = arad_pp_fp_db_stage_get(unit, table_id, &stage);
    result = handle_sand_result(soc_sandResult);
    if (BCM_E_NONE != result) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "DB %3d: unable to get stage %d (%s)\n"),
                  table_id, result, _SHR_ERRMSG(result)));
        return CMD_FAIL;
    }

    if((dbInfo = sal_alloc(sizeof(SOC_PPC_FP_DATABASE_INFO), "DB info")) == NULL) {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate SOC_PPC_FP_DATABASE_INFO\n");
    }

    sal_memset(dbInfo, 0, sizeof(SOC_PPC_FP_DATABASE_INFO));
    soc_sandResult = soc_ppd_fp_database_get(unit, table_id, dbInfo);
    result = handle_sand_result(soc_sandResult);
    if (BCM_E_NONE != result) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "DB %3d: unable to read %d (%s)\n"),
                  table_id, result, _SHR_ERRMSG(result)));
        return CMD_FAIL;
    }

    if (SOC_PPC_NOF_FP_DATABASE_TYPES == dbInfo->db_type) {
        return CMD_FAIL;
    }
    else if (SOC_PPC_NOF_FP_DATABASE_TYPES < dbInfo->db_type) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit,"DB %3d: unexpected type %d\n"),
                  table_id, dbInfo->db_type));
        return CMD_FAIL;
    }

    _dpp_field_get_qual_list(dbInfo, qual_str, &qual_num);
    _dpp_field_get_action_list(dbInfo, action_str, &action_num);

    PRT_TITLE_SET("Show Table:%d", table_id);

    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Qualifiers");
    for(qual_iter = 0; qual_iter < qual_num - 1; qual_iter++)
        PRT_COLUMN_ADD("");
    PRT_COLUMN_ADD("");
    PRT_COLUMN_ADD("Pr");
    PRT_COLUMN_ADD("");
    PRT_COLUMN_ADD("Actions");
    for(action_iter = 0; action_iter < action_num - 1; action_iter++)
        PRT_COLUMN_ADD("");

    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL);

    /*
     * First line of data will be actual header, but we have already assigned the right number of columns above
     */
    PRT_CELL_SET("%d", table_id);
    for (qual_iter = 0; qual_iter < qual_num; qual_iter++) {
        PRT_CELL_SET("%s", qual_str[qual_iter]);
    }

    PRT_CELL_SKIP(1);
    PRT_CELL_SKIP(1); /* for priority */
    PRT_CELL_SKIP(1);
    for (action_iter = 0; action_iter < action_num; action_iter++) {
        PRT_CELL_SET("%s", action_str[action_iter]);
    }

    for (qual_iter = 0; qual_iter < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_iter++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(dbInfo->qual_types[qual_iter])) {
            arad_pp_fp_key_length_get_unsafe(unit, stage, dbInfo->qual_types[qual_iter], 0 , &qual_size[qual_iter]);
        }
    }

    if ((SOC_PPC_FP_DB_TYPE_TCAM == dbInfo->db_type) ||
        (SOC_PPC_FP_DB_TYPE_DIRECT_TABLE == dbInfo->db_type) ||
        (SOC_PPC_FP_DB_TYPE_FLP == dbInfo->db_type) ||
        (SOC_PPC_FP_DB_TYPE_EGRESS == dbInfo->db_type)) {
        if (SOC_PPC_FP_DB_TYPE_FLP == dbInfo->db_type) {
            FIELD_ACCESS.entryExtTcLimit.get(unit, &entryLimit);
        }
        else {
            FIELD_ACCESS.entryTcLimit.get(unit, &entryLimit);
        }

        if(start_entry_id >= entryLimit) {
            PRT_FREE;
            SHR_ERR_EXIT(_SHR_E_INTERNAL,"Entry ID exceeded limit:%d\n", entryLimit);
        }

        if(output_num <= 0)
            count = entryLimit;
        else
            count = output_num;

        if((entInfoTc = sal_alloc(sizeof(SOC_PPC_FP_ENTRY_INFO), "TCAM info")) == NULL) {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate SOC_PPC_FP_ENTRY_INFO\n");
        }

        for (soc_sandOffset = start_entry_id; soc_sandOffset < entryLimit; soc_sandOffset++) {
            /* For internal/external TCAM, check whether the entry can be found first */
            if ((SOC_PPC_FP_DB_TYPE_TCAM == dbInfo->db_type) || (SOC_PPC_FP_DB_TYPE_EGRESS == dbInfo->db_type)) {
                ARAD_TCAM_LOCATION location;
                uint8 found = 0;
                arad_tcam_db_entry_id_to_location_entry_get(
                        unit,
                        ARAD_PP_FP_DB_ID_TO_TCAM_DB(table_id),
                        soc_sandOffset,
                        TRUE,
                        &location,
                        &found
                      );

                if (!found) {
                    continue;
                }
            }
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
            else if (SOC_PPC_FP_DB_TYPE_FLP == dbInfo->db_type) {
                ARAD_SW_KBP_HANDLE location;

                sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.location_tbl.get(unit, soc_sandOffset, &location);                
                if (location.db_entry == NULL) {
                    continue;
                }
            }
#endif
            sal_memset(entInfoTc, 0, sizeof(SOC_PPC_FP_ENTRY_INFO));
            soc_sandResult = soc_ppd_fp_entry_get(unit,
                                                  table_id,
                                                  soc_sandOffset,
                                                  &okay,
                                                  entInfoTc);
            result = handle_sand_result(soc_sandResult);
            if ((BCM_E_NONE == result) && okay) {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%d", soc_sandOffset);
                for (db_iter = 0; db_iter < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; db_iter++) {
                    if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entInfoTc->qual_vals[db_iter].type)) {
                        for(qual_iter = 0; qual_iter < qual_num ; qual_iter++ ) {
                            if(entInfoTc->qual_vals[db_iter].type == dbInfo->qual_types[qual_iter]) {
                            _dpp_field_get_qual_str(&entInfoTc->qual_vals[db_iter], qual_val_str, qual_size[qual_iter]);
                            sal_strcpy(qual_vals_list[qual_iter] ,qual_val_str);
                            break;
                            }
                        }

                    } else if (_BCM_DPP_FIELD_PPD_QUAL_VALID(dbInfo->qual_types[db_iter])) {
                        PRT_CELL_SKIP(1);
                    }
                }
                for(qual_iter = 0; qual_iter < qual_num ; qual_iter++ ) {
                    PRT_CELL_SET("%s", qual_vals_list[qual_iter]);
                }
                PRT_CELL_SKIP(1);
                PRT_CELL_SET("%d", entInfoTc->priority);
                PRT_CELL_SKIP(1);

                for (action_iter = 0; action_iter < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_iter++) {                    
                    SOC_PPC_FP_ACTION_VAL_clear(&actions_value[action_iter]);
                }

                for (db_iter = 0; db_iter < action_num; db_iter++){
                    if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entInfoTc->actions[db_iter].type)) {
                        for (action_iter = 0; action_iter < action_num; action_iter++) {
                            if (entInfoTc->actions[db_iter].type == dbInfo->action_types[action_iter]) {
                                actions_value[action_iter].val = entInfoTc->actions[db_iter].val;
                                actions_value[action_iter].type = entInfoTc->actions[db_iter].type;
                                break;
                            }
                        }
                    }
                }
                for (action_iter = 0; action_iter < action_num; action_iter++) {
                    if (_BCM_DPP_FIELD_PPD_ACTION_VALID(actions_value[action_iter].type)) {
                        PRT_CELL_SET("%X", actions_value[action_iter].val);
                    } else {
                        PRT_CELL_SKIP(1);
                    }
                }

                if(!(--count))
                    break;
            } /* if ((BCM_E_NONE == result) && okay) */

            if ((soc_sandOffset>start_entry_id) && ((soc_sandOffset-start_entry_id) % 0x1000) == 0) {
                sal_thread_yield();
            }
        } /* for (soc_sandOffset = 0; soc_sandOffset < 4K; soc_sandOffset++) */
    } else if (SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION == dbInfo->db_type) {
        FIELD_ACCESS.entryDeLimit.get(unit, &entryLimit);
        if(start_entry_id >= entryLimit) {
            PRT_FREE;
            SHR_ERR_EXIT(_SHR_E_INTERNAL,"Entry ID exceeded limit:%d\n", entryLimit);
        }

        if(output_num <= 0)
            count = entryLimit;
        else
            count = output_num;

        if((entInfoDe = sal_alloc(sizeof(SOC_PPC_FP_DIR_EXTR_ENTRY_INFO), "DE info")) == NULL) {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate SOC_PPC_FP_DIR_EXTR_ENTRY_INFO\n");
        }

        for (soc_sandOffset = start_entry_id; soc_sandOffset < entryLimit; soc_sandOffset++) {
            sal_memset(entInfoDe, 0, sizeof(SOC_PPC_FP_DIR_EXTR_ENTRY_INFO));
            soc_sandResult = soc_ppd_fp_direct_extraction_entry_get(unit,
                                                                    table_id,
                                                                    soc_sandOffset,
                                                                    &okay,
                                                                    entInfoDe);
            result = handle_sand_result(soc_sandResult);
            if ((BCM_E_NONE == result) && okay) {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%d", soc_sandOffset);
                for (qual_iter = 0; qual_iter < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_iter++) {
                    if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entInfoDe->qual_vals[qual_iter].type)) {
                        _dpp_field_get_qual_str(&entInfoDe->qual_vals[qual_iter], qual_val_str, qual_size[qual_iter]);
                        PRT_CELL_SET("%s", qual_val_str);
                    } else if (_BCM_DPP_FIELD_PPD_QUAL_VALID(dbInfo->qual_types[qual_iter])) {
                        PRT_CELL_SKIP(1);
                    }
                }
                PRT_CELL_SKIP(1);
                PRT_CELL_SET("%d", entInfoDe->priority);
                PRT_CELL_SKIP(1);
                for (db_iter = 0; db_iter < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; db_iter++) {
                    if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entInfoDe->actions[db_iter].type)) {
                        PRT_CELL_SET("base  %08X", entInfoDe->actions[db_iter].base_val);
                        for (action_iter = 0; action_iter < entInfoDe->actions[db_iter].nof_fields; action_iter++) {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SKIP(qual_num + 4);
                            if ((entInfoDe->actions[db_iter].fld_ext[action_iter].cst_val) ||
                                (BCM_FIELD_ENTRY_INVALID == entInfoDe->actions[db_iter].fld_ext[action_iter].type)) {
                                PRT_CELL_SET("const %08X [%d-0]",
                                         entInfoDe->actions[db_iter].fld_ext[action_iter].cst_val,
                                         entInfoDe->actions[db_iter].fld_ext[action_iter].nof_bits - 1);
                            } else {
                                PRT_CELL_SET("qual  %s [%d-%d]",
                                         SOC_PPC_FP_QUAL_TYPE_to_string(entInfoDe->actions[db_iter].fld_ext[action_iter].type),
                                         entInfoDe->actions[db_iter].fld_ext[action_iter].nof_bits + entInfoDe->actions[db_iter].fld_ext[action_iter].fld_lsb - 1,
                                         entInfoDe->actions[db_iter].fld_ext[action_iter].fld_lsb);
                            }
                        } /* for (all extractions this action) */
                    } /* if (this action is valid) */
                } /* for (all actions this entry) */
                if(!(--count))
                    break;
            } /* if ((BCM_E_NONE == result) && okay) */
        } /* for (soc_sandOffset = 0; soc_sandOffset < 4K; soc_sandOffset++) */
    }

    /* Coverity: We are not going to change the macros to aviod such defects */
    /* coverity[dead_error_line]*/
    PRT_COMMIT;
exit:
    if(dbInfo != NULL)
        sal_free(dbInfo);
    if(entInfoDe != NULL)
        sal_free(entInfoDe);
    if(entInfoTc != NULL)
        sal_free(entInfoTc);
    PRT_FREE;
    return SHR_ERR_TO_SHELL;
}

static int
cmd_dpp_field_tables_dump(int unit, args_t* a)
{
    cmd_result_t res = CMD_OK;
    parse_table_t pt;
    int db_id = -1;
    int start_entry_id = 0, entry_num = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "db", PQ_DFL|PQ_INT, &db_id,  &db_id, NULL);
    parse_table_add(&pt, "start", PQ_DFL|PQ_INT, 0,  &start_entry_id, NULL);
    parse_table_add(&pt, "num", PQ_DFL|PQ_INT, 0,  &entry_num, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if (ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if(db_id == -1) {
        res = cmd_dpp_field_table_list(unit);
    } else if((db_id  >= 0) && (db_id < SOC_PPC_FP_NOF_DBS)) {
        res = cmd_dpp_field_table_dump(unit, db_id, start_entry_id, entry_num);
    } else {
        cli_out("Illegal db id:%d\n", db_id);
    }

    parse_arg_eq_done(&pt);
    return res;
}

cmd_result_t
cmd_dpp_diag_field(int unit, args_t* a)
{
    char      *function;
    int strnlen_function;

    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    }
    strnlen_function = sal_strnlen(function, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (DIAG_FUNC_STR_MATCH(function, "LAST_packet_get", "LAST")) {
        return cmd_ppd_api_fp_packet_diag_get(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "RESource_get", "RES")) {
        return cmd_ppd_api_fp_resource_diag_get(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "ACtion_info", "AC")) {
        return cmd_ppd_api_fp_action_info_show(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "DB_action_info", "DB")) {
        return cmd_ppd_api_fp_dbs_action_info_show(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "Tables", "T")) {
        return cmd_dpp_field_tables_dump(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "Ranges", "R")) {
        return cmd_dpp_field_ranges_dump(unit);
    } else if (DIAG_FUNC_STR_MATCH(function, "FEMS_for_stage", "FEMS")) {
        return cmd_ppd_api_fp_print_all_fems_for_stage(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "FESS_for_stage", "FESS")) {
        return cmd_ppd_api_fp_print_fes_info_for_stage(unit, a);
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (! sal_strncasecmp(function, "test", strnlen_function)){
        return cmd_diag_test(unit, a);
    } else if (! sal_strncasecmp(function, "pr_indices", strnlen_function)){
        return diag_soc_sand_print_indices(unit, a) ;
    } else if (! sal_strncasecmp(function, "pr_lists", strnlen_function)){
        return diag_soc_sand_print_list(unit, a) ;
    } else if (! sal_strncasecmp(function, "ACTION", strnlen_function)){
        return cmd_ppd_api_fp_action_show(unit, a) ;
    } else if (! sal_strncasecmp(function, "QUALIFIER", strnlen_function)){
        return cmd_ppd_api_fp_qualifier_show(unit, a) ;
    }
    else {
        return CMD_USAGE;
    } 
}

void
print_field_usage(int unit)
{
    char cmd_dpp_diag_field_usage[] =
    "Usage (DIAG field):"
    "\n\tDIAGnotsics field commands\n\t"
    "Usages:\n\t"
    "DIAG field [OPTION] <parameters> ..."

    "OPTION can be:"
    "\nLAST_packet_get [core=<Core ID>] - Field ACL results (Key built, TCAM hit and actions done) for the last packet"
    "\n"
    "\nRESource_get <0/1/2> - Diagnostics to reflect which HW resources are used" 
    "\n                       0 - resource usage"
    "\n                       1 - include consistency between SW and HW"
    "\n                       2 - include validation of bank entries"
    "\n"
    "\nACtion_info -          Diagnostics to display internal actions and corresponding BCM identifiers" 
    "\n" 
    "\nDB_action_info -       Diagnostics to display internal actions and corresponding data basess" 
    "\n" 
    "\nTables [db=<table id>] [start=<entry id>] [num=<number>] - Table show facility"
    "\n                       Using without parameters will present all tables with qualifiers and actions"
    "\n                         use db in order to present specific table"
    "\n                         use start and num parameters if the table has a lot of entries"
    "\n"
    "\nRanges          -      Show all ranges used by qualifiers"
    "\n"
    "\nFEMS_for_stage <stage> <is_for_tm> -"
    "\n                       Diagnostics to display all FEMs assigned to specified stage" 
    "\n                       First parameter (stage) may be:" 
    "\n                         0 - SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF" 
    "\n                         1 - SOC_PPC_FP_DATABASE_STAGE_EGRESS" 
    "\n                         etc." 
    "\n                       Second parameter (is_for_tm):" 
    "\n                         Set 1 for TM processing (packets with ITMH header)" 
    "\n                         Set 0 for stacking or TDM processing (packets with FTMH header)" 
    "\nFESS_for_stage <stage> <program> -"
    "\n                       Diagnostics to display all FESs assigned to specified stage and program" 
    "\n                       First parameter (stage) may be:" 
    "\n                         0 - SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF" 
    "\n                         1 - SOC_PPC_FP_DATABASE_STAGE_EGRESS" 
    "\n                         etc." 
    "\n                       Second parameter (program):" 
    "\n                         Currently 0 - 31" 
    "\n" 
    "\ntest <test name> (or \"all\") "
    "\n\t\tx - key number."
    "\n\t\tmode - <Fast/Medium/Slow/Scan>. \n"
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    "\n\t\tstage - <Ingress/Egress/External>. \n"
#else
    "\n\t\tstage - <Ingress/Egress>. \n"
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
#ifdef BCM_WARM_BOOT_SUPPORT
    "\n\t\twarmboot <0/1>. Do warmboot after field group and entries install. The default is FALSE.\n"
#endif
    "\npr_indices <sorted list id> - Print contents of direct indices created for specified sorted list"
    "\n"
    "\npr_lists <sorted list id>   - Print contents of specified sorted list"
        ;
  
    char cmd_dpp_diag_field_usage2[] =
    "\n"
    "\nACTION - The diag shows list of predefined/user actions supported by the SDK."
    "\n               Possible options:"
    "\n                   [predefined]"
    "\n                         Shows predefined actions."
    "\n                   [bcm]"
    "\n                         Shows BCM predefined actions."
    "\n                   [soc]"
    "\n                         Shows SOC predefined actions."
    "\n                   [stage] may be:"
    "\n                         0 - SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF"
    "\n                         1 - SOC_PPC_FP_DATABASE_STAGE_EGRESS"
    "\n                         etc. see SOC_PPC_FP_DATABASE_STAGE"
    "\n               Possible combinations:"
    "\n                   [predefined][bcm] - Shows list of predefined BCM actions for all stages."
    "\n                   [predefined][soc] - Shows list of predefined SOC actions for all stages."
    "\n                   [predefined][bcm/soc][stage=<stage_id>] - Shows list of predefined BCM/SOC actions for specific stage."
    "\n"
    "\nQUALIFIER - The diag shows list of predefined/user qualifiers supported by the SDK."
    "\n               Possible options:"
    "\n                   [predefined]"
    "\n                         Shows predefined qualifiers."
    "\n                   [bcm]"
    "\n                         Shows BCM predefined qualifiers."
    "\n                   [soc]"
    "\n                         Shows SOC predefined qualifiers."
    "\n                   [preselectors]"
    "\n                         Shows preselector qualifiers."
    "\n                   [stage] may be:"
    "\n                         0 - SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF"
    "\n                         1 - SOC_PPC_FP_DATABASE_STAGE_EGRESS"
    "\n                         etc. see SOC_PPC_FP_DATABASE_STAGE"
    "\n                   [qset_type] may be:"
    "\n                         0 - All types"
    "\n                         1 - L2"
    "\n                         2 - IPV4"
    "\n                         4 - IPV6"
    "\n                         8 - MPLS"
    "\n               Possible combinations:"
    "\n                   [predefined][bcm] - Shows list of predefined BCM qualifiers for all stages and all qset types."
    "\n                   [predefined][soc] - Shows list of predefined SOC qualifiers for all stages and all qset types."
    "\n                   [predefined][bcm/soc][stage=<stage_id>] - Shows list of predefined BCM/SOC qualifiers for specific stage and all qset types."
    "\n                   [predefined][bcm/soc][stage=<stage_id>][qset_type=<type>] - Shows list of predefined BCM/SOC qualifiers for specific stage and qset type."
    "\n                   [preselectors] - Shows list of qualifiers used for preselection, for all stages and all qset types."
    "\n                   [preselectors][stage=<stage_id>] - Shows list of qualifiers used for preselection, for specific stage."
    "\n"
      ;

    cli_out(cmd_dpp_diag_field_usage);
    cli_out(cmd_dpp_diag_field_usage2);
}







cmd_result_t
cmd_dpp_diag_dbal(int unit, args_t* a)
{
    char    *function, *function1, *function2;
    int     val = -1, val1, val2;

    function = ARG_GET(a);

    if (! function ) {
        return CMD_USAGE;
    } else if (DIAG_FUNC_STR_MATCH(function, "Tables_Info", "ti")) {
        if (arad_pp_dbal_tables_dump(unit, 0)) {
            return CMD_FAIL;
        }
        return CMD_OK;

    } else if (DIAG_FUNC_STR_MATCH(function, "Prefix_Info", "pi")) {
        if (arad_pp_dbal_lem_prefix_table_dump(unit)) {
            return CMD_FAIL;
        }
        if (arad_pp_dbal_isem_prefix_table_dump(unit)) {
            return CMD_FAIL;
        }
        if (arad_pp_dbal_tcam_prefix_table_dump(unit)) {
            return CMD_FAIL;
        }
        return CMD_OK;

    } else if (DIAG_FUNC_STR_MATCH(function, "Table", "t")) {
        function = ARG_GET(a);
        if (function) {
            val = sal_ctoi(function,0);
            if (arad_pp_dbal_table_info_dump(unit, val)) {
                return CMD_FAIL;
            }
        } else {
            return CMD_USAGE;
        }
    }else if (DIAG_FUNC_STR_MATCH(function, "DB_Dump", "dbd")) {
        function = ARG_GET(a);
        if (function) {
            val = sal_ctoi(function,0);
            if (arad_pp_dbal_phisycal_db_dump(unit, val)) {
                return CMD_FAIL;
            }
        } else {
            return CMD_USAGE;
        }
    } else if (DIAG_FUNC_STR_MATCH(function, "Last_Packet", "lp")) {
        int core_id;
        for (core_id = 0; core_id < SOC_DPP_DEFS_GET(unit, nof_cores); core_id++) {
            if (arad_pp_dbal_last_packet_dump(unit, core_id)) {
                return CMD_FAIL;
            }
        }
    } else if (DIAG_FUNC_STR_MATCH(function, "INstraction", "in")) {
        function = ARG_GET(a);
        function1 = ARG_GET(a);
        function2 = ARG_GET(a);
        if (function && function1 && function2) {
            val = sal_ctoi(function,0);
            val1 = sal_ctoi(function1,0);
            val2 = sal_ctoi(function2,0);
            if (arad_pp_dbal_qualifier_to_instruction_dump(unit, val, val1, 0, val2)) {
                return CMD_FAIL;
            }
        } else {
            return CMD_USAGE;
        }
    } else if (DIAG_FUNC_STR_MATCH(function, "Table_Entries", "te")) {
        function = ARG_GET(a);
        if (function) {
            val = sal_ctoi(function,0);
            if (arad_pp_dbal_table_print(unit, val)) {
                return CMD_FAIL;
            }
        } else {
            return CMD_USAGE;
        }
    } else if (DIAG_FUNC_STR_MATCH(function, "Tables_Info_Full", "tif")) {
        if (arad_pp_dbal_tables_dump(unit, 1)) {
            return CMD_FAIL;
        }
    } else if (DIAG_FUNC_STR_MATCH(function, "PRogram_Info", "pri")) {
        function = ARG_GET(a);
        function1 = ARG_GET(a);
        if (function && function1) {
            val = sal_ctoi(function,0);
            val1 = sal_ctoi(function1,0);
            if (arad_pp_dbal_ce_per_program_dump(unit, val, val1)) {
                return CMD_FAIL;
            }
        } else {
            return CMD_USAGE;
        }
    } else if (DIAG_FUNC_STR_MATCH(function, "entry_ADD", "add")) {
        function = ARG_GET(a);
        if (!function) {
            return CMD_USAGE;
        }
        val = sal_ctoi(function, 0);
        if (arad_pp_dbal_diag_entry_manage(unit, SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_ADD, val, a)) {
            return CMD_FAIL;
        }
    } else if (DIAG_FUNC_STR_MATCH(function, "entry_DELete", "del")) {
        function = ARG_GET(a);
        if (!function) {
            return CMD_USAGE;
        }
        val = sal_ctoi(function, 0);
        if (arad_pp_dbal_diag_entry_manage(unit, SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_DELETE, val, a)) {
            return CMD_FAIL;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

void
print_dbal_usage(int unit)
{
    char cmd_dpp_diag_dbal_usage[] =
    "Usage (DIAG dbal):"
    "\n\tDIAGnotsics dbal commands\n\t"
    "Usages:\n\t"
    "DIAG dbal [OPTION] <parameters> ..."

    "OPTION can be:"
    "\nTables_Info                           - Returns brief information about all existing tables."
    "\nTable <ID>                            - Returns full information for a specific table ID." 
    "\nTables_Info_Full                      - Returns full information for all existing tables."
    "\nTable_Entries <ID>                    - Returns all entries in a table."
    "\nDB_Dump <DB>                          - Returns all entries in physical DB, LEM=0, TCAM=1, KBP=2, SEM A=3, SEM B=4, "
    "                                          6=ESEM, OAM1=7, OAM2=8, RMEP=9, GLEM=10."
	/*"\nentry_ADD <ID> <q1>..<qn> <payload>   - Adds an entry with <q1>..<qn> => <payload> to table <ID>."
    "\nentry_DELete <ID> <q1>..<qn>          - Deletes the entry identified with <q1>..<qn> from table <ID>.\n"
	"                                          Leave <q1>..<qn> and <payload> empty to list required qualifiers.\n"
	"                                          Use ':' to seperate between words of a multi-world value [ms]:..:[ls]"*/
    "\nLast_Packet <core>                    - Returns full information for the last packet lookups from the VT, TT and FLP. if core not add core=0"
    "\nPrefix_Info                           - Returns prefix allocation mapping for exact match databases."
    "\nPRogram_Info <programID> <stage>      - Returns program information for a specific program and stage.\n"
    "                                          Stage selected according to the following: PMF = 0, FLP = 2, SLB = 3, VT = 4, TT = 5\n";

    cli_out(cmd_dpp_diag_dbal_usage);
}
/* } */
