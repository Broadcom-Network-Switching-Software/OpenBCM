/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 

#include <sal/types.h>
#include <shared/bsl.h>
#include <appl/diag/shell.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <bcm/error.h>
#include <soc/dpp/cosq.h>

#ifdef BCM_PETRA_SUPPORT

#define FC_NIF_GEN_STATUS_USAGE \
    "Fc Nif Gen Status Show port=<> src=<global/vsq/NIF> vsq_type=<A/B/C/D/E/F> vsq_id=<> fc_type=<LLFC/PFC/SAFC>\n\t"

#define FC_NIF_REC_STATUS_USAGE \
    "Fc Nif Rec Status Show port=<> fc_type=<LLFC/PFC/SAFC>\n\t"

#define FC_ILKN_GEN_STATUS_USAGE \
    "Fc Ilkn Gen Status Show port=<> calendar_id=<>\n\t"

#define FC_ILKN_REC_STATUS_USAGE \
    "Fc Ilkn Rec Status Show port=<> calendar_id=<> fc_type=<LLFC/CHFC>\n\t"

#define FC_MUB_GEN_STATUS_USAGE \
    "Fc Mub Gen Status Show port=<> calendar_id=<>\n\t"

#define FC_MUB_REC_STATUS_USAGE \
    "Fc Mub Rec Status Show port=<> calendar_id=<> fc_type=<LLFC/CHFC>\n\t"

#define FC_OOB_GEN_STATUS_USAGE \
    "Fc Oob Gen Status Show port=<> calendar_id=<> oob_type=<ILKN/SPI/HCFC>\n\t"

#define FC_OOB_REC_STATUS_USAGE \
    "Fc Oob Rec Status Show port=<> calendar_id=<> oob_type=<ILKN/SPI/HCFC>\n\t"

#ifndef COMPILER_STRING_CONST_LIMIT
#define FC_NIF_GEN_USAGE \
    FC_NIF_GEN_STATUS_USAGE "\n\t"

#define FC_NIF_REC_USAGE \
    FC_NIF_REC_STATUS_USAGE "\n\t"

#define FC_ILKN_GEN_USAGE \
    FC_ILKN_GEN_STATUS_USAGE "\n\t"

#define FC_ILKN_REC_USAGE \
    FC_ILKN_REC_STATUS_USAGE "\n\t"

#define FC_MUB_GEN_USAGE \
    FC_MUB_GEN_STATUS_USAGE "\n\t"

#define FC_MUB_REC_USAGE \
    FC_MUB_REC_STATUS_USAGE "\n\t"

#define FC_OOB_GEN_USAGE \
    FC_OOB_GEN_STATUS_USAGE "\n\t"

#define FC_OOB_REC_USAGE \
    FC_OOB_REC_STATUS_USAGE "\n\t"

#else
#define FC_NIF_GEN_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_NIF_REC_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_ILKN_GEN_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_ILKN_REC_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_MUB_GEN_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_MUB_REC_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_OOB_GEN_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_OOB_REC_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"
#endif

#ifndef COMPILER_STRING_CONST_LIMIT
#define FC_NIF_USAGE \
    FC_NIF_GEN_USAGE "\n\t"  \
    FC_NIF_REC_USAGE "\n\t"  \
    "       - Set/display FC NIF configuration"

#define FC_ILKN_USAGE \
    FC_ILKN_GEN_USAGE "\n\t"  \
    FC_ILKN_REC_USAGE "\n\t"  \
    "       - Set/display FC ILKN configuration"

#define FC_MUB_USAGE \
    FC_MUB_GEN_USAGE "\n\t"  \
    FC_MUB_REC_USAGE "\n\t"  \
    "       - Set/display FC MUB configuration"

#define FC_OOB_USAGE \
    FC_OOB_GEN_USAGE "\n\t"  \
    FC_OOB_REC_USAGE "\n\t"  \
    "       - Set/display FC OOB configuration"
#else
#define FC_NIF_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_ILKN_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_MUB_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define FC_OOB_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"
#endif


char cmd_dpp_fc_usage[] =
    "Usages:\n\t"
    FC_NIF_USAGE "\n\n\t"
    FC_ILKN_USAGE "\n\n\t"
    FC_MUB_USAGE "\n\n\t"
    FC_OOB_USAGE "\n"
    ;

#define PARSE_FC_SRC           0
#define PARSE_FC_VSQ_TYPE      1
#define PARSE_NIF_FC_TYPE      2
#define PARSE_CAL_FC_TYPE      3
#define PARSE_OOB_TYPE         4

#define FC_SRC_STRs      {"global", "vsq", "nif"}
#define FC_VSQ_TYPE_STRs {"a", "b", "c", "d", "e", "f"}
#define NIF_FC_TYPE_STRs {"llfc", "pfc", "safc"}
#define CAL_FC_TYPE_STRs {"llfc", "chfc"}
#define OOB_TYPE_STRs    {"ilkn", "spi", "hcfc"}

STATIC cmd_result_t
_fc_parse_str(int type, char *str_in, int *value)
{
    char **_strs = NULL;
    char *src_strs[] = FC_SRC_STRs;
    char *vsq_type_strs[] = FC_VSQ_TYPE_STRs;
    char *nif_fc_type_strs[] = NIF_FC_TYPE_STRs;
    char *cal_fc_type_strs[] = CAL_FC_TYPE_STRs;
    char *oob_type_strs[] = OOB_TYPE_STRs;
    int i = 0, len = 0, found = 0;
    
    if ((NULL == str_in) || (NULL == value)) {
        return CMD_FAIL;
    }

    switch (type) {
    case PARSE_FC_SRC:
        _strs = src_strs;
        len = sizeof(src_strs)/sizeof(char *);
        break;
    case PARSE_FC_VSQ_TYPE:
        _strs = vsq_type_strs;
        len = sizeof(vsq_type_strs)/sizeof(char *);
        break;
    case PARSE_NIF_FC_TYPE:
        _strs = nif_fc_type_strs;
        len = sizeof(nif_fc_type_strs)/sizeof(char *);
        break;
    case PARSE_CAL_FC_TYPE:
        _strs = cal_fc_type_strs;
        len = sizeof(cal_fc_type_strs)/sizeof(char *);
        break;
    case PARSE_OOB_TYPE:
        _strs = oob_type_strs;
        len = sizeof(oob_type_strs)/sizeof(char *);
        break;
    default:
        break;
    }

    for (i = 0; i < len; i++) {
         if (!sal_strncasecmp(str_in, _strs[i], strlen(str_in))) {
             (*value) = i;
             found = 1;
             break;
         }
    }

    if (found != 1) {
        return CMD_FAIL;
    }

    return CMD_OK;
}

#define FC_BLOCK_IQM   0
#define FC_BLOCK_CFC   1
#define FC_BLOCK_CFC_2 2
#define FC_BLOCK_NBI   3
#define FC_BLOCK_NBI_2 4
#define FC_BLOCK_MIB   5
#define FC_BLOCK_EGQ   6
#define FC_BLOCK_SCH   7
#define FC_BLOCK_END   8
#define FC_BLOCK_END_2 9

STATIC void 
_dpp_fc_status_block_print(int unit, int block)
{
    switch (block) {
    case FC_BLOCK_IQM: /* IQM */
        if (SOC_IS_JERICHO(unit)) {
            cli_out("|---------------------------------------------------------------------------------------------------------------------------|\n");
            cli_out("|                                                            IQM                                                            |\n");
            cli_out("|                             Core0                           |                             Core1                           |\n");
        }
        else {
            cli_out("|-------------------------------------------------------------|\n");
            cli_out("|                             IQM                             |\n");
        }
        break;
    case FC_BLOCK_CFC: /* CFC */
            cli_out("|-------------------------------------------------------------|\n");
            cli_out("|                             CFC                             |\n");
        break;
    case FC_BLOCK_CFC_2: /* CFC */
        if (SOC_IS_JERICHO(unit)) {
            cli_out("|---------------------------------------------------------------------------------------------------------------------------|\n");
            cli_out("|                                                            CFC                                                            |\n");
            cli_out("|                            Core0                            |                            Core1                            |\n");
        }
        else {
            cli_out("|-------------------------------------------------------------|\n");
            cli_out("|                             CFC                             |\n");
        }
        break;
    case FC_BLOCK_NBI: /* NBI */
            cli_out("|-------------------------------------------------------------|\n");
            cli_out("|                             NBI                             |\n"); 
        break;
    case FC_BLOCK_NBI_2: /* NBI */
        if (SOC_IS_JERICHO(unit)) {
            cli_out("|---------------------------------------------------------------------------------------------------------------------------|\n");
            cli_out("|                             NBI                             |\n");
        }
        else {
            cli_out("|-------------------------------------------------------------|\n");
            cli_out("|                             NBI                             |\n");
        }
        break;
    case FC_BLOCK_MIB: /* MIB */
            cli_out("|-------------------------------------------------------------|\n");
            cli_out("|                             MIB                             |\n"); 
        break;
    case FC_BLOCK_EGQ: /* EGQ */
            cli_out("|-------------------------------------------------------------|\n");
            cli_out("|                             EGQ                             |\n");
        break;
    case FC_BLOCK_SCH: /* SCH */
            cli_out("|-------------------------------------------------------------|\n");
            cli_out("|                             SCH                             |\n");
        break;
    case FC_BLOCK_END: /* END */
            cli_out("|-------------------------------------------------------------|\n");
        break;
    case FC_BLOCK_END_2: /* END */
        if (SOC_IS_JERICHO(unit)) {
            cli_out("|-------------------------------------------------------------|-------------------------------------------------------------|\n");
        }
        else {
            cli_out("|-------------------------------------------------------------|\n");
        }
        break;

    default:
        break;
    }
}

STATIC void 
_dpp_fc_64_egq_print(soc_reg_above_64_val_t reg_abv_64_val, char* pStr0, char* pStr1, char* pStr2, char* pStr3)
{
    char *str;

    sal_sprintf(pStr0, "egq0   ");
    sal_sprintf(pStr1, "egq64  ");
    sal_sprintf(pStr2, "egq128 ");
    sal_sprintf(pStr3, "egq192 ");

    str = pStr3+7;
    sal_sprintf(str, "%08x", reg_abv_64_val[7]);
    str += 8;
    sal_sprintf(str, "%08x", reg_abv_64_val[6]);
    str = pStr2+7;
    sal_sprintf(str, "%08x", reg_abv_64_val[5]);
    str += 8;
    sal_sprintf(str, "%08x", reg_abv_64_val[4]);
    str = pStr1+7;
    sal_sprintf(str, "%08x", reg_abv_64_val[3]);
    str += 8;
    sal_sprintf(str, "%08x", reg_abv_64_val[2]);
    str = pStr0+7;
    sal_sprintf(str, "%08x", reg_abv_64_val[1]);
    str += 8;
    sal_sprintf(str, "%08x", reg_abv_64_val[0]);
}

#if 0
STATIC void 
_dpp_fc_256_egq_2_print(int unit, char* indication, soc_reg_above_64_val_t abv64_val_0, soc_reg_above_64_val_t abv64_val_1)
{
    char *blank_str = " ";
    char indication_str_0_0[64], indication_str_0_1[64], indication_str_0_2[64], indication_str_0_3[64], 
        indication_str_1_0[64], indication_str_1_1[64], indication_str_1_2[64], indication_str_1_3[64];

    if (SOC_IS_JERICHO(unit)) { 
        _dpp_fc_64_egq_print(abv64_val_0, 
            &(indication_str_0_0[0]), 
            &(indication_str_0_1[0]), 
            &(indication_str_0_2[0]), 
            &(indication_str_0_3[0]));
        _dpp_fc_64_egq_print(abv64_val_1, 
            &(indication_str_1_0[0]), 
            &(indication_str_1_1[0]), 
            &(indication_str_1_2[0]), 
            &(indication_str_1_3[0]));
        cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", indication, indication_str_0_0, indication, indication_str_1_0);
        cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", blank_str, indication_str_0_1, blank_str, indication_str_1_1);
        cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", blank_str, indication_str_0_2, blank_str, indication_str_1_2);
        cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", blank_str, indication_str_0_3, blank_str, indication_str_1_3);
    }
    else {
        _dpp_fc_64_egq_print(abv64_val_0, 
            &(indication_str_0_0[0]), 
            &(indication_str_0_1[0]), 
            &(indication_str_0_2[0]), 
            &(indication_str_0_3[0]));
        cli_out("|   %-30s = %-24s |\n", indication, indication_str_0_0);
        cli_out("|   %-30s = %-24s |\n", blank_str, indication_str_0_1);
        cli_out("|   %-30s = %-24s |\n", blank_str, indication_str_0_2);
        cli_out("|   %-30s = %-24s |\n", blank_str, indication_str_0_3);                
    }
}
#endif

STATIC void 
_dpp_fc_256_egq_print(int unit, char* indication, soc_reg_above_64_val_t abv64_val_0)
{
    char *blank_str = " ";
    char indication_str_0_0[64], indication_str_0_1[64], indication_str_0_2[64], indication_str_0_3[64];

    _dpp_fc_64_egq_print(abv64_val_0, 
            &(indication_str_0_0[0]), 
            &(indication_str_0_1[0]), 
            &(indication_str_0_2[0]), 
            &(indication_str_0_3[0]));
    cli_out("|   %-30s = %-24s |\n", indication, indication_str_0_0);
    cli_out("|   %-30s = %-24s |\n", blank_str, indication_str_0_1);
    cli_out("|   %-30s = %-24s |\n", blank_str, indication_str_0_2);
    cli_out("|   %-30s = %-24s |\n", blank_str, indication_str_0_3);
}


STATIC void
_dpp_fc_trigger_iqm_status_print(int unit, 
    uint32                 src_type,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    /* 1 bit status */
    if (src_type == SOC_TMC_FC_SRC_TYPE_GLB) {
        if (SOC_IS_JERICHO(unit)) {
            if(!SOC_IS_QAX(unit)) {
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_mnmc_db_hi", fc_status_info->iqm_glb_mnmc_db_hi_fc_state[0], "glb_mnmc_db_hi", fc_status_info->iqm_glb_mnmc_db_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_mnmc_db_lo", fc_status_info->iqm_glb_mnmc_db_lo_fc_state[0], "glb_mnmc_db_lo", fc_status_info->iqm_glb_mnmc_db_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_flmc_db_hi", fc_status_info->iqm_glb_flmc_db_hi_fc_state[0], "glb_flmc_db_hi", fc_status_info->iqm_glb_flmc_db_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_flmc_db_lo", fc_status_info->iqm_glb_flmc_db_lo_fc_state[0], "glb_flmc_db_lo", fc_status_info->iqm_glb_flmc_db_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_bdb_hi", fc_status_info->iqm_glb_bdb_hi_fc_state[0], "glb_bdb_hi", fc_status_info->iqm_glb_bdb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_bdb_lo", fc_status_info->iqm_glb_bdb_lo_fc_state[0], "glb_bdb_lo", fc_status_info->iqm_glb_bdb_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_ocb_hi", fc_status_info->iqm_glb_ocb_hi_fc_state[0], "glb_ocb_hi", fc_status_info->iqm_glb_ocb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_ocb_lo", fc_status_info->iqm_glb_ocb_lo_fc_state[0], "glb_ocb_lo", fc_status_info->iqm_glb_ocb_lo_fc_state[1]);
            } else {
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_headroom_pd_hi", fc_status_info->iqm_glb_headroom_pd_hi_fc_state[0], "glb_headroom_pd_hi", fc_status_info->iqm_glb_headroom_pd_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_headroom_pd_lo", fc_status_info->iqm_glb_headroom_pd_lo_fc_state[0], "glb_headroom_pd_lo", fc_status_info->iqm_glb_headroom_pd_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_headroom_bd_hi", fc_status_info->iqm_glb_headroom_bd_hi_fc_state[0], "glb_headroom_bd_hi", fc_status_info->iqm_glb_headroom_bd_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_headroom_bd_lo", fc_status_info->iqm_glb_headroom_bd_lo_fc_state[0], "glb_headroom_bd_lo", fc_status_info->iqm_glb_headroom_bd_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_pdb_hi", fc_status_info->iqm_glb_pdb_hi_fc_state[0], "glb_pdb_hi", fc_status_info->iqm_glb_pdb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_pdb_lo", fc_status_info->iqm_glb_pdb_lo_fc_state[0], "glb_pdb_lo", fc_status_info->iqm_glb_pdb_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_bdb_hi", fc_status_info->iqm_glb_bdb_hi_fc_state[0], "glb_bdb_hi", fc_status_info->iqm_glb_bdb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_bdb_lo", fc_status_info->iqm_glb_bdb_lo_fc_state[0], "glb_bdb_lo", fc_status_info->iqm_glb_bdb_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_ocb_hi", fc_status_info->iqm_glb_ocb_hi_fc_state[0], "glb_ocb_hi", fc_status_info->iqm_glb_ocb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_ocb_lo", fc_status_info->iqm_glb_ocb_lo_fc_state[0], "glb_ocb_lo", fc_status_info->iqm_glb_ocb_lo_fc_state[1]);
            }
        }
        else {
            cli_out("|   %-30s = %-24d |\n", "glb_mnmc_db_hi", fc_status_info->iqm_glb_mnmc_db_hi_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_mnmc_db_lo", fc_status_info->iqm_glb_mnmc_db_lo_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_flmc_db_hi", fc_status_info->iqm_glb_flmc_db_hi_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_flmc_db_lo", fc_status_info->iqm_glb_flmc_db_lo_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_bdb_hi", fc_status_info->iqm_glb_bdb_hi_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_bdb_lo", fc_status_info->iqm_glb_bdb_lo_fc_state[0]);
        }
    }
    else {
        if (SOC_IS_JERICHO(unit)) {
            cli_out("|                                                                                                                           |\n");
        }
        else {
            cli_out("|                                                             |\n");
        }
    }
}

STATIC void
_dpp_fc_trigger_cfc_status_print(int unit, 
    uint32                 src_type,
    uint32                 vsq_type,
    uint32                 vsq_id,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    char   indication_str[30], indication_str_2[30];
    char* vsq_strs[] = FC_VSQ_TYPE_STRs;

    if (src_type == SOC_TMC_FC_SRC_TYPE_GLB) {
        /* 1 bit status */
        if (SOC_IS_JERICHO(unit)) {
            if(!SOC_IS_QAX(unit)) {
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_mnmc_db_hi", fc_status_info->cfc_iqm_glb_mnmc_db_hi_fc_state[0], "glb_mnmc_db_hi", fc_status_info->cfc_iqm_glb_mnmc_db_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_mnmc_db_lo", fc_status_info->cfc_iqm_glb_mnmc_db_lo_fc_state[0], "glb_mnmc_db_lo", fc_status_info->cfc_iqm_glb_mnmc_db_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_flmc_db_hi", fc_status_info->cfc_iqm_glb_flmc_db_hi_fc_state[0], "glb_flmc_db_hi", fc_status_info->cfc_iqm_glb_flmc_db_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_flmc_db_lo", fc_status_info->cfc_iqm_glb_flmc_db_lo_fc_state[0], "glb_flmc_db_lo", fc_status_info->cfc_iqm_glb_flmc_db_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_bdb_hi", fc_status_info->cfc_iqm_glb_bdb_hi_fc_state[0], "glb_bdb_hi", fc_status_info->cfc_iqm_glb_bdb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_bdb_lo", fc_status_info->cfc_iqm_glb_bdb_lo_fc_state[0], "glb_bdb_lo", fc_status_info->cfc_iqm_glb_bdb_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_ocb_hi", fc_status_info->cfc_iqm_glb_ocb_hi_fc_state[0], "glb_ocb_hi", fc_status_info->cfc_iqm_glb_ocb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_ocb_lo", fc_status_info->cfc_iqm_glb_ocb_lo_fc_state[0], "glb_ocb_lo", fc_status_info->cfc_iqm_glb_ocb_lo_fc_state[1]);
            } else {
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_pdb_hi", fc_status_info->cfc_iqm_glb_pdb_hi_fc_state[0], "glb_pdb_hi", fc_status_info->cfc_iqm_glb_pdb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_pdb_lo", fc_status_info->cfc_iqm_glb_pdb_lo_fc_state[0], "glb_pdb_lo", fc_status_info->cfc_iqm_glb_pdb_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "pool_0_hi", fc_status_info->cfc_cgm_pool_0_hi_fc_state[0], "pool_0_hi", fc_status_info->cfc_cgm_pool_0_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "pool_0_lo", fc_status_info->cfc_cgm_pool_0_lo_fc_state[0], "pool_0_lo", fc_status_info->cfc_cgm_pool_0_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "pool_1_hi", fc_status_info->cfc_cgm_pool_1_hi_fc_state[0], "pool_1_hi", fc_status_info->cfc_cgm_pool_1_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "pool_1_lo", fc_status_info->cfc_cgm_pool_1_lo_fc_state[0], "pool_1_lo", fc_status_info->cfc_cgm_pool_1_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_bdb_hi", fc_status_info->cfc_iqm_glb_bdb_hi_fc_state[0], "glb_bdb_hi", fc_status_info->cfc_iqm_glb_bdb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_bdb_lo", fc_status_info->cfc_iqm_glb_bdb_lo_fc_state[0], "glb_bdb_lo", fc_status_info->cfc_iqm_glb_bdb_lo_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_ocb_hi", fc_status_info->cfc_iqm_glb_ocb_hi_fc_state[0], "glb_ocb_hi", fc_status_info->cfc_iqm_glb_ocb_hi_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", "glb_ocb_lo", fc_status_info->cfc_iqm_glb_ocb_lo_fc_state[0], "glb_ocb_lo", fc_status_info->cfc_iqm_glb_ocb_lo_fc_state[1]);
            }
        }
        else {
            cli_out("|   %-30s = %-24d |\n", "glb_mnmc_db_hi", fc_status_info->cfc_iqm_glb_mnmc_db_hi_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_mnmc_db_lo", fc_status_info->cfc_iqm_glb_mnmc_db_lo_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_flmc_db_hi", fc_status_info->cfc_iqm_glb_flmc_db_hi_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_flmc_db_lo", fc_status_info->cfc_iqm_glb_flmc_db_lo_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_bdb_hi", fc_status_info->cfc_iqm_glb_bdb_hi_fc_state[0]);
            cli_out("|   %-30s = %-24d |\n", "glb_bdb_lo", fc_status_info->cfc_iqm_glb_bdb_lo_fc_state[0]);
        }
    }
    else if (src_type == SOC_TMC_FC_SRC_TYPE_VSQ) {
        /* 1 bit status */
        sal_snprintf(indication_str, sizeof(indication_str), "vsq_%s_fc(Id:%d)", vsq_strs[vsq_type], vsq_id);
        if (SOC_IS_JERICHO(unit)) {
            if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
                sal_snprintf(indication_str_2, sizeof(indication_str_2), "vsq_%s_fc(Id:%d)", vsq_strs[vsq_type], vsq_id+148);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", indication_str, fc_status_info->cfc_iqm_vsq_fc_state[0], indication_str, fc_status_info->cfc_iqm_vsq_fc_state[1]);
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", indication_str_2, fc_status_info->cfc_iqm_vsq_fc_state_2[0], indication_str_2, fc_status_info->cfc_iqm_vsq_fc_state_2[1]);
            }
            else {
                cli_out("|   %-30s = %-24d |   %-30s = %-24d |\n", indication_str, fc_status_info->cfc_iqm_vsq_fc_state[0], indication_str, fc_status_info->cfc_iqm_vsq_fc_state[1]);
            }
        }
        else {
            cli_out("|   %-30s = %-24d |\n", indication_str, fc_status_info->cfc_iqm_vsq_fc_state[0]);
        }
    }
    else if (src_type == SOC_TMC_FC_SRC_TYPE_NIF) {
        /* 1 bit status */
        cli_out("|   %-30s = %-24d |\n", "nif_af_fc", fc_status_info->cfc_nif_af_fc_status);
    }
}

STATIC void
_dpp_fc_trigger_nif_status_print(int unit, 
    uint32                 src_type,
    uint32                 nif_fc_type,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    if (src_type == SOC_TMC_FC_SRC_TYPE_NIF) {
        /* 1 bit status */
        if (nif_fc_type == SOC_TMC_FC_NIF_TYPE_LLFC) {
            cli_out("|   %-30s = %-24d |\n", "llfc_status_from_mlf", fc_status_info->nbi_llfc_status_from_mlf);
        }
        else if ((nif_fc_type == SOC_TMC_FC_NIF_TYPE_PFC) || (nif_fc_type == SOC_TMC_FC_NIF_TYPE_SAFC)) {
            cli_out("|   %-30s = %-24d |\n", "pfc_status_from_mlf", fc_status_info->nbi_pfc_status_from_mlf);
        }
    }
}

STATIC cmd_result_t
_dpp_fc_nif_gen_print(int unit, 
    SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    char   indication_str[30];
    char   buf_val[32];
    int    i = 0, vsq_id = 0;

    /*** 1. print fc status in IQM ***/
    if (fc_status_key->src_type == SOC_TMC_FC_SRC_TYPE_GLB) {
        _dpp_fc_status_block_print(unit, FC_BLOCK_IQM);
        _dpp_fc_trigger_iqm_status_print(unit, fc_status_key->src_type, fc_status_info);
    }

    /*** 2. print fc status in CFC ***/
    if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_NIF) {
        _dpp_fc_status_block_print(unit, FC_BLOCK_CFC_2);
    }
    else {
        _dpp_fc_status_block_print(unit, FC_BLOCK_CFC);
    }
    if (fc_status_key->vsq_type == SOC_TMC_ITM_VSQ_GROUP_LLFC || fc_status_key->vsq_type == SOC_TMC_ITM_VSQ_GROUP_PFC) {
        vsq_id = fc_status_info->nif_tx_src_id;
    }
    else {
        vsq_id = fc_status_key->vsq_id;
    }
    _dpp_fc_trigger_cfc_status_print(unit, fc_status_key->src_type, fc_status_key->vsq_type, vsq_id, fc_status_info);

    /*** 3. print fc status in NBI ***/
    if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_NIF) {
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI_2);
    }
    else {
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI);
    }
    _dpp_fc_trigger_nif_status_print(unit, fc_status_key->src_type, fc_status_key->nif_fc_type, fc_status_info);

    if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_LLFC) { /* LLFC */
        /* 1 bit status */
        if (fc_status_key->src_type != SOC_TMC_FC_SRC_TYPE_NIF) {
            cli_out("|   %-30s = %-24d |\n", "tx_llfc_from_cfc", fc_status_info->nbi_tx_llfc_from_cfc);
        }
        cli_out("|   %-30s = %-24d |\n", "tx_llfc_to_mac", fc_status_info->nbi_tx_llfc_to_mac);

        /* 32 bits counter*/
        if (SOC_IS_JERICHO(unit)) {
            cli_out("|   %-30s = %-24d |\n", "eth_tx_llfc_cnt", fc_status_info->nbi_eth_tx_fc_cnt);        
        }
    }
    else {                              /* PFC/SAFC */
        /* 1 bit status */
        if (fc_status_key->src_type != SOC_TMC_FC_SRC_TYPE_NIF) {
            cli_out("|   %-30s = 0x%-22x |\n", "tx_pfc_from_cfc", fc_status_info->nbi_tx_pfc_from_cfc);
        }
        cli_out("|   %-30s = 0x%-22x |\n", "tx_pfc_to_mac", fc_status_info->nbi_tx_pfc_to_mac);        
    }

    /*** 4. print fc status in mib ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_MIB); 

    if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_LLFC) { /* LLFC */
        format_uint64_decimal(buf_val, fc_status_info->mib_tx_pause_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "tx_pause_cnt", buf_val);
    }
    else if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_PFC) { /* PFC */
        format_uint64_decimal(buf_val, fc_status_info->mib_tx_pfc_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "tx_pfc_cnt", buf_val);

        for (i = 0; i < 8; i++) {
            if (COMPILER_64_IS_ZERO(fc_status_info->mib_tx_pfc_x_cnt[i])) {
                continue;
            }

            sal_snprintf(indication_str, sizeof(indication_str), "tx_pfc_%d_cnt", i);
            format_uint64_decimal(buf_val, fc_status_info->mib_tx_pfc_x_cnt[i], ',');
            cli_out("|   %-30s = %-24s |\n", indication_str, buf_val);
        }        
    }
    else if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_SAFC) { /* SAFC */
        format_uint64_decimal(buf_val, fc_status_info->mib_tx_safc_log_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "tx_safc_log_cnt", buf_val);
    }

    /*** 5. print end ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_END);

    return CMD_OK;
}

STATIC cmd_result_t
_dpp_fc_nif_rec_print(int unit, 
    SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    char   indication_str[30];
    char   buf_val[32];
    int    i = 0;
    int    is_generic_map_used = 0;
    char *egq_pfc_str = "egq_pfc_status";
    char *cfc_pfc_str = "cfc_pfc_status";

    /*** 0. print NIF PFC mapping info ***/
    if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_PFC ||
        fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_SAFC) { /* PFC/SAFC */
        for (i = 0; i < 8; i++) {
            if (fc_status_info->nif_rx_dst_type[i] == SOC_TMC_FC_PFC_MAP_EGQ) {
                cli_out("map nif pfc %d to egq %d\n", i, fc_status_info->nif_rx_dst_id[i]);
            }
            else if (fc_status_info->nif_rx_dst_type[i] == SOC_TMC_FC_PFC_MAP_GENERIC_BITMAP) {
                is_generic_map_used = 1;
                cli_out("map nif pfc %d to generic bmp %d\n", i, fc_status_info->nif_rx_dst_id[i]);
            }
            else {
                cli_out("map nif pfc %d to invalid\n", i);
            }
        }
    }

    /*** 1. print fc status in MIB ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_MIB); 

    if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_LLFC) { /* LLFC */
        format_uint64_decimal(buf_val, fc_status_info->mib_rx_pause_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "rx_pause_cnt", buf_val);
    }
    else if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_PFC) { /* PFC */
        format_uint64_decimal(buf_val, fc_status_info->mib_rx_pfc_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "rx_pfc_cnt", buf_val);

        for (i = 0; i < 8; i++) {
            if (COMPILER_64_IS_ZERO(fc_status_info->mib_rx_pfc_x_cnt[i])) {
                continue;
            }

            sal_snprintf(indication_str, sizeof(indication_str), "rx_pfc_%d_cnt", i);
            format_uint64_decimal(buf_val, fc_status_info->mib_rx_pfc_x_cnt[i], ',');
            cli_out("|   %-30s = %-24s |\n", indication_str, buf_val);
        }        
    }
    else if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_SAFC) { /* SAFC */
        format_uint64_decimal(buf_val, fc_status_info->mib_rx_safc_log_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "tx_safc_log_cnt", buf_val);

        format_uint64_decimal(buf_val, fc_status_info->mib_rx_safc_phy_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "tx_safc_phy_cnt", buf_val);

        format_uint64_decimal(buf_val, fc_status_info->mib_rx_safc_crc_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "tx_safc_crc_cnt", buf_val);
    }

    if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_PFC ||
        fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_SAFC) { /* PFC/SAFC */

        /*** 2. print fc status in NBI ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI);

        /* 8 bit status */
        cli_out("|   %-30s = 0x%-22x |\n", "rx_pfc_from_mac", fc_status_info->nbi_rx_pfc_from_mac);
        cli_out("|   %-30s = 0x%-22x |\n", "rx_pfc_to_cfc", fc_status_info->nbi_rx_pfc_to_cfc);

        /*** 3. print fc status in CFC ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_CFC);

        /* 8 bit status */
        cli_out("|   %-30s = 0x%-22x |\n", "nif_pfc_status", fc_status_info->cfc_nif_pfc_status);

        if (is_generic_map_used) {
            _dpp_fc_256_egq_print(unit, egq_pfc_str, fc_status_info->cfc_egq_pfc_status_full[0]);
        }
        else {
            cli_out("|   %-30s = 0x%-22x |\n", "egq_pfc_status", fc_status_info->cfc_egq_pfc_status[0]);
            cli_out("|   %-30s = 0x%-22x |\n", "egq_inf_fc", fc_status_info->cfc_egq_inf_fc);
            cli_out("|   %-30s = 0x%-22x |\n", "egq_dev_fc", fc_status_info->cfc_egq_dev_fc);
        }
    }

    /*** 4. print fc status in EGQ ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_EGQ);

    if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_LLFC) { /* LLFC */
        cli_out("|   %-30s = %-24d |\n", "nif_fc_status", fc_status_info->egq_nif_fc_status[0]);
    }
    else if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_PFC ||
        fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_SAFC) { /* PFC/SAFC */
        /* 8 bit status */
        if (is_generic_map_used) {
            _dpp_fc_256_egq_print(unit, cfc_pfc_str, fc_status_info->egq_cfc_fc_status_full[0]);
        }
        else {
            cli_out("|   %-30s = 0x%-22x |\n", "cfc_pfc_status", fc_status_info->egq_cfc_fc_status[0]);
        }
    }

    /*** 5. print fc status in SCH ***/
    if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_PFC ||
        fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_SAFC) { /* PFC/SAFC */

        if (is_generic_map_used == 0) {
            _dpp_fc_status_block_print(unit, FC_BLOCK_SCH);

            for (i = 0; i < 8; i++) {
                if (fc_status_info->sch_fc_port_cnt[i] != 0) {
                    sal_snprintf(indication_str, sizeof(indication_str), "fc_port_%d_cnt", i);
                    cli_out("|   %-30s = %-24d |\n", indication_str, fc_status_info->sch_fc_port_cnt[i]);
                }
            }
            cli_out("|   %-30s = 0x%-22x |\n", "fc_inf_cnt", fc_status_info->sch_fc_inf_cnt);
            cli_out("|   %-30s = 0x%-22x |\n", "fc_dev_cnt", fc_status_info->sch_fc_dev_cnt);
        }
    }

    /*** 6. print end ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_END);

    return CMD_OK;
}

#define CAL_RX_DST_TYPE_STRs {"pfc", "nif", "2p port", "8p port", "generic bmp"}
STATIC cmd_result_t
_dpp_fc_ilkn_gen_print(int unit, 
    SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    COMPILER_REFERENCE(fc_status_key);

    /*** 0. only print calendar mapping info ***/
    if (fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_HCFC) {
        cli_out("map hcfc %d to calendar entry %d\n", fc_status_info->cal_tx_src_id, fc_status_key->calendar_id);
    }

    /*** 1. print fc status in IQM ***/
    if (fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_GLB) {
        _dpp_fc_status_block_print(unit, FC_BLOCK_IQM);
        _dpp_fc_trigger_iqm_status_print(unit, fc_status_info->cal_tx_src_type, fc_status_info);
    }

    /*** 2. print fc status in CFC ***/
    if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_HCFC) {
        if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_NIF) {
            _dpp_fc_status_block_print(unit, FC_BLOCK_CFC_2);
        }
        else {
            _dpp_fc_status_block_print(unit, FC_BLOCK_CFC);
        }
        _dpp_fc_trigger_cfc_status_print(unit, 
            fc_status_info->cal_tx_src_type, 
            fc_status_info->cal_tx_src_vsq_type, 
            fc_status_info->cal_tx_src_id, 
            fc_status_info);
    }

    /*** 3. print fc status in NBI ***/
    if (fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_GLB ||
        fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_VSQ) {
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI_2);
    }
    else {
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI);
    }
    _dpp_fc_trigger_nif_status_print(unit, fc_status_info->cal_tx_src_type, SOC_TMC_FC_NIF_TYPE_LLFC, fc_status_info);

    /* 1 bit status */
    cli_out("|   %-30s = %-24d |\n", "rx_chfc_from_cfc_raw", fc_status_info->nbi_ilkn_rx_chfc_from_cfc_raw);
    cli_out("|   %-30s = %-24d |\n", "tx_chfc_roc", fc_status_info->nbi_ilkn_tx_chfc_roc);

    /*** 4. print end ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_END);

    return CMD_OK;
}

STATIC cmd_result_t
_dpp_fc_ilkn_rec_print(int unit, 
    SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    char   indication_str[30];
    int    i = 0;
    char *egq_pfc_str = "egq_pfc_status";
    char *cal_rx_dst_strs[] = CAL_RX_DST_TYPE_STRs;
    char   buf_val[32];

    COMPILER_REFERENCE(fc_status_key);

    if (fc_status_key->calendar_fc_type == SOC_TMC_FC_CALENDAR_TYPE_LLFC) {
        /*** 1. print fc status in NBI ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI);

        format_uint64_decimal(buf_val, fc_status_info->nbi_ilkn_rx_llfc_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "rx_llfc_cnt", buf_val);

        format_uint64_decimal(buf_val, fc_status_info->nbi_ilkn_llfc_stop_tx_cnt, ',');
        cli_out("|   %-30s = %-24s |\n", "llfc_stop_tx_cnt", buf_val);
    }
    else {

        /*** 0. print calendar mapping info ***/
        cli_out("map calendar entry %d to %s dst_id %d\n", fc_status_key->calendar_id, 
            cal_rx_dst_strs[fc_status_info->cal_rx_dst_type - SOC_TMC_FC_REC_CAL_DEST_PFC], 
            fc_status_info->cal_rx_dst_id);

        /*** 1. print fc status in NBI ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI);

        /* 8 bit status */
        cli_out("|   %-30s = %-24d |\n", "rx_chfc_from_port_raw", fc_status_info->nbi_ilkn_rx_chfc_from_port_raw);
        cli_out("|   %-30s = %-24d |\n", "rx_chfc_from_port_roc", fc_status_info->nbi_ilkn_rx_chfc_from_port_roc);
        if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_NIF_LL) {
            cli_out("|   %-30s = %-24d |\n", "llfc_stop_tx_from_cfc", fc_status_info->nbi_llfc_stop_tx_from_cfc);
            cli_out("|   %-30s = %-24d |\n", "llfc_stop_tx_to_mac", fc_status_info->nbi_llfc_stop_tx_to_mac);
        }

        /*** 2. print fc status in CFC ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_CFC);

        cli_out("|   %-30s = %-24d |\n", "ilkn_fc_status", fc_status_info->cfc_ilkn_fc_status);
        if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PFC ||
            fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY ||
            fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY
        ) {
            /* 8 bit status */
            cli_out("|   %-30s = 0x%-22x |\n", "egq_pfc_status", fc_status_info->cfc_egq_pfc_status[0]);
            cli_out("|   %-30s = 0x%-22x |\n", "egq_inf_fc", fc_status_info->cfc_egq_inf_fc);
            cli_out("|   %-30s = 0x%-22x |\n", "egq_dev_fc", fc_status_info->cfc_egq_dev_fc);
        }
        else if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC) 
        {
            _dpp_fc_256_egq_print(unit, egq_pfc_str, fc_status_info->cfc_egq_pfc_status_full[0]);
        }

        /*** 3. print fc status in EGQ ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_EGQ);

        /* LLFC */
        if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_NIF_LL) {
            cli_out("|   %-30s = %-24d |\n", "nif_fc_status", fc_status_info->egq_nif_fc_status[0]);
        }
        else if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PFC ||
            fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY ||
            fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY
        ) {    
            /* 8 bit status */
            cli_out("|   %-30s = 0x%-22x |\n", "cfc_pfc_status", fc_status_info->egq_cfc_fc_status[0]);
        }
        else if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC) 
        {
            _dpp_fc_256_egq_print(unit, egq_pfc_str, fc_status_info->egq_cfc_fc_status_full[0]);
        }

        /*** 4. print fc status in SCH ***/
        if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PFC ||
            fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY ||
            fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY
        ) { 
            _dpp_fc_status_block_print(unit, FC_BLOCK_SCH);

            for (i = 0; i < 8; i++) {
                if (fc_status_info->sch_fc_port_cnt[i] != 0) {
                    sal_snprintf(indication_str, sizeof(indication_str), "fc_port_%d_cnt", i);
                    cli_out("|   %-30s = %-24d |\n", indication_str, fc_status_info->sch_fc_port_cnt[i]);
                }
            }
            cli_out("|   %-30s = 0x%-22x |\n", "fc_inf_cnt", fc_status_info->sch_fc_inf_cnt);
            cli_out("|   %-30s = 0x%-22x |\n", "fc_dev_cnt", fc_status_info->sch_fc_dev_cnt);
        }
    }

    /*** 5. print end ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_END);

    return CMD_OK;
}

STATIC cmd_result_t
_dpp_fc_mub_gen_print(int unit, 
    SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    COMPILER_REFERENCE(fc_status_key);

    /*** 0. print calendar mapping info ***/
    if (fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_HCFC) {
        cli_out("map hcfc %d to calendar entry %d\n", fc_status_info->cal_tx_src_id, fc_status_key->calendar_id);
    }

    /*** 1. print fc status in IQM ***/
    if (fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_GLB) {
        _dpp_fc_status_block_print(unit, FC_BLOCK_IQM);
        _dpp_fc_trigger_iqm_status_print(unit, fc_status_info->cal_tx_src_type, fc_status_info);
    }

    /*** 2. print fc status in CFC ***/
    if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_HCFC) {
        if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_NIF) {
            _dpp_fc_status_block_print(unit, FC_BLOCK_CFC_2);
        }
        else {
            _dpp_fc_status_block_print(unit, FC_BLOCK_CFC);
        }
        _dpp_fc_trigger_cfc_status_print(unit, 
            fc_status_info->cal_tx_src_type, 
            fc_status_info->cal_tx_src_vsq_type, 
            fc_status_info->cal_tx_src_id, 
            fc_status_info);
    }

    /*** 3. print fc status in NBI ***/
    if (fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_GLB ||
        fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_VSQ) {
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI_2);
    }
    else {
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI);
    }
    _dpp_fc_trigger_nif_status_print(unit, fc_status_info->cal_tx_src_type, SOC_TMC_FC_NIF_TYPE_LLFC, fc_status_info);

    /* 1 bit status */
    if (SOC_IS_JERICHO(unit)) {
        cli_out("|   %-30s = %-24d |\n", "mub_tx_from_cfc", fc_status_info->nbi_mub_tx_from_cfc);
    }
    else {
        cli_out("|   %-30s = %-24d |\n", "mub_tx_from_cfc", fc_status_info->nbi_mub_tx_from_cfc);
        cli_out("|   %-30s = %-24d |\n", "mub_tx_value", fc_status_info->nib_mub_tx_value);
    }

    /*** 4. print end ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_END);

    return CMD_OK;
}

STATIC cmd_result_t
_dpp_fc_mub_rec_print(int unit, 
    SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    char *egq_pfc_str = "egq_pfc_status";

    COMPILER_REFERENCE(fc_status_key);

    if (fc_status_key->calendar_fc_type == SOC_TMC_FC_CALENDAR_TYPE_LLFC) {
        /*** 1. print fc status in NBI ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI);

        if (SOC_IS_JERICHO(unit)) {
            cli_out("|   %-30s = %-24d |\n", "llfc_stop_tx_from_mub", fc_status_info->nbi_mub_llfc_stop_tx_from_mub);
        }
        else {
            cli_out("|   %-30s = %-24d |\n", "mub_rx_value", fc_status_info->nbi_mub_rx_value);
            cli_out("|   %-30s = %-24d |\n", "llfc_stop_tx_from_mub", fc_status_info->nbi_mub_llfc_stop_tx_from_mub);
        }
    }
    else {        
        /*** 1. print fc status in NBI ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI);

        /* 8 bit status */
        if (SOC_IS_JERICHO(unit)) {
            cli_out("|   %-30s = %-24d |\n", "mub_rx_to_cfc", fc_status_info->nbi_mub_rx_to_cfc);
        }
        else {
            cli_out("|   %-30s = %-24d |\n", "mub_rx_value", fc_status_info->nbi_mub_rx_value);
            cli_out("|   %-30s = %-24d |\n", "mub_rx_to_cfc", fc_status_info->nbi_mub_rx_to_cfc);
        }

        /*** 2. print fc status in CFC ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_CFC);

        _dpp_fc_256_egq_print(unit, egq_pfc_str, fc_status_info->cfc_egq_pfc_status_full[0]);

        /*** 3. print fc status in EGQ ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_EGQ);

        _dpp_fc_256_egq_print(unit, egq_pfc_str, fc_status_info->egq_cfc_fc_status_full[0]);
    }

    /*** 4. print end ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_END);

    return CMD_OK;
}

STATIC cmd_result_t
_dpp_fc_oob_gen_print(int unit, 
    SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    COMPILER_REFERENCE(fc_status_key);

    /*** 0. print calendar mapping info ***/
    if (fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_HCFC) {
        cli_out("map hcfc %d to calendar entry %d\n", fc_status_info->cal_tx_src_id, fc_status_key->calendar_id);
        return CMD_OK;
    }

    /*** 1. print fc status in IQM ***/
    if (fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_GLB) {
        _dpp_fc_status_block_print(unit, FC_BLOCK_IQM);
        _dpp_fc_trigger_iqm_status_print(unit, fc_status_info->cal_tx_src_type, fc_status_info);
    }

    /*** 2. print fc status in CFC ***/
    if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_HCFC) {
        if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_NIF) {
            _dpp_fc_status_block_print(unit, FC_BLOCK_CFC_2);
        }
        else {
            _dpp_fc_status_block_print(unit, FC_BLOCK_CFC);
        }
        _dpp_fc_trigger_cfc_status_print(unit, 
            fc_status_info->cal_tx_src_type, 
            fc_status_info->cal_tx_src_vsq_type, 
            fc_status_info->cal_tx_src_id, 
            fc_status_info);
    }

    /*** 3. print end ***/
    if (fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_GLB ||
        fc_status_info->cal_tx_src_type == SOC_TMC_FC_SRC_TYPE_VSQ) {
        _dpp_fc_status_block_print(unit, FC_BLOCK_END_2);
    }
    else {
        _dpp_fc_status_block_print(unit, FC_BLOCK_END);
    }

    return CMD_OK;
}

STATIC cmd_result_t
_dpp_fc_oob_rec_print(int unit, 
    SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_TMC_FC_STATUS_INFO *fc_status_info)
{
    char   indication_str[30];
    int    i = 0;
    char *egq_pfc_str = "egq_pfc_status";
    char *cal_rx_dst_strs[] = CAL_RX_DST_TYPE_STRs;

    /*** 0. print calendar mapping info ***/
    cli_out("map calendar entry %d to %s dst_id %d\n", fc_status_key->calendar_id, 
        cal_rx_dst_strs[fc_status_info->cal_rx_dst_type - SOC_TMC_FC_REC_CAL_DEST_PFC], 
        fc_status_info->cal_rx_dst_id);

    if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_NIF_LL) {
        /*** 1. print fc status in NBI ***/
        _dpp_fc_status_block_print(unit, FC_BLOCK_NBI);

        cli_out("|   %-30s = %-24d |\n", "llfc_stop_tx_from_cfc", fc_status_info->nbi_llfc_stop_tx_from_cfc);
        cli_out("|   %-30s = %-24d |\n", "llfc_stop_tx_to_mac", fc_status_info->nbi_llfc_stop_tx_to_mac);
    }

    /*** 2. print fc status in CFC ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_CFC);

    if (fc_status_key->intf_type == SOC_TMC_FC_INTF_TYPE_ILKN) {
        cli_out("|   %-30s = %-24d |\n", "ilkn_fc_status", fc_status_info->cfc_ilkn_fc_status);
        cli_out("|   %-30s = %-24d |\n", "ilkn_rx_crc_err_status", fc_status_info->cfc_ilkn_rx_crc_err_status);
        cli_out("|   %-30s = %-24d |\n", "ilkn_rx_crc_err_cnt", fc_status_info->cfc_ilkn_rx_crc_err_cnt);
        cli_out("|   %-30s = 0x%-22x |\n", "oob_rx_lanes_status", fc_status_info->cfc_oob_rx_lanes_status);
    }
    else if (fc_status_key->intf_type == SOC_TMC_FC_INTF_TYPE_SPI) {
        if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_NIF_LL) {
            cli_out("|   %-30s = %-24d |\n", "spi_rx_llfc_status", fc_status_info->cfc_spi_rx_llfc_status);
        }
        else if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PFC ||
            fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY ||
            fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY
        ) {
            cli_out("|   %-30s = 0x%-22x |\n", "spi_rx_pfc_status", fc_status_info->cfc_spi_rx_pfc_status);
        }
        else if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC) {
            cli_out("|   %-30s = %-24d |\n", "spi_rx_gen_pfc_status", fc_status_info->cfc_spi_rx_gen_pfc_status);
        }
        cli_out("|   %-30s = %-24d |\n", "spi_rx_crc_err_status", fc_status_info->cfc_spi_rx_crc_err_status);
        cli_out("|   %-30s = %-24d |\n", "spi_rx_frame_err_cnt", fc_status_info->cfc_spi_rx_frame_err_cnt);
        cli_out("|   %-30s = %-24d |\n", "spi_rx_dip_2_err_cnt", fc_status_info->cfc_spi_rx_dip_2_err_cnt);
    }
    else if (fc_status_key->intf_type == SOC_TMC_FC_INTF_TYPE_HCFC) {
        cli_out("|   %-30s = %-24d |\n", "hcfc_rx_crc_err_status", fc_status_info->cfc_hcfc_rx_crc_err_status);
        for (i = 0; i < 5; i++) {
            if (fc_status_info->cfc_hcfc_rx_crc_err_cnt[i] != 0) {
                sal_snprintf(indication_str, sizeof(indication_str), "hcfc_%d_rx_crc_err_cnt", i);
                cli_out("|   %-30s = %-24d |\n", indication_str, fc_status_info->cfc_hcfc_rx_crc_err_cnt[i]);
            }
        }
        cli_out("|   %-30s = %-24d |\n", "hcfc_rx_wd_err_status", fc_status_info->cfc_hcfc_rx_wd_err_status);
    }

    if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PFC ||
        fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY ||
        fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY
    ) {
        /* 8 bit status */
        cli_out("|   %-30s = 0x%-22x |\n", "egq_pfc_status", fc_status_info->cfc_egq_pfc_status[0]);
        cli_out("|   %-30s = 0x%-22x |\n", "egq_inf_fc", fc_status_info->cfc_egq_inf_fc);
        cli_out("|   %-30s = 0x%-22x |\n", "egq_dev_fc", fc_status_info->cfc_egq_dev_fc);
    }
    else if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC) 
    {
        _dpp_fc_256_egq_print(unit, egq_pfc_str, fc_status_info->cfc_egq_pfc_status_full[0]);
    }

    /*** 3. print fc status in EGQ ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_EGQ);

    /* LLFC */
    if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_NIF_LL) {
        cli_out("|   %-30s = %-24d |\n", "nif_fc_status", fc_status_info->egq_nif_fc_status[0]);
    }
    else if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PFC ||
        fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY ||
        fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY
    ) {    
        /* 8 bit status */
        cli_out("|   %-30s = 0x%-22x |\n", "cfc_pfc_status", fc_status_info->egq_cfc_fc_status[0]);
    }
    else if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC) 
    {
        _dpp_fc_256_egq_print(unit, egq_pfc_str, fc_status_info->egq_cfc_fc_status_full[0]);
    }

    /*** 4. print fc status in SCH ***/
    if (fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PFC ||
        fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY ||
        fc_status_info->cal_rx_dst_type == SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY
    ) { 
        _dpp_fc_status_block_print(unit, FC_BLOCK_SCH);

        for (i = 0; i < 8; i++) {
            if (fc_status_info->sch_fc_port_cnt[i] != 0) {
                sal_snprintf(indication_str, sizeof(indication_str), "fc_port_%d_cnt", i);
                cli_out("|   %-30s = %-24d |\n", indication_str, fc_status_info->sch_fc_port_cnt[i]);
            }
        }
        cli_out("|   %-30s = 0x%-22x |\n", "fc_inf_cnt", fc_status_info->sch_fc_inf_cnt);
        cli_out("|   %-30s = 0x%-22x |\n", "fc_dev_cnt", fc_status_info->sch_fc_dev_cnt);
    }

    /*** 5. print end ***/
    _dpp_fc_status_block_print(unit, FC_BLOCK_END);

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_fc_nif_gen_status_show(int unit, args_t *a)
{
    parse_table_t pt;
    soc_port_t arg_port = 0;
    char *arg_src_str = NULL, *arg_vsq_type_str = NULL, *arg_fc_type_str = NULL;
    int arg_vsq_id = 0;
    int fc_src = 0;
    int fc_vsq_type = 0;
    int fc_type = 0;
    int is_force_fc_type = 0;
    int fc_type_forced = 0;
    soc_error_t soc_rc;
    SOC_TMC_FC_STATUS_KEY fc_status_key;
    SOC_TMC_FC_STATUS_INFO fc_status_info;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "port", PQ_DFL|PQ_INT, NULL,
                    &arg_port, NULL);
    parse_table_add(&pt, "src", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_src_str, NULL);
    parse_table_add(&pt, "vsq_type", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_vsq_type_str, NULL);
    parse_table_add(&pt, "vsq_id", PQ_DFL|PQ_INT, 0, (void *)&arg_vsq_id, 0);
    parse_table_add(&pt, "fc_type", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_fc_type_str, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    sal_memset(&fc_status_key, 0, sizeof(fc_status_key));
    fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_NIF;
    fc_status_key.port = arg_port;
    fc_status_key.direction = SOC_TMC_FC_DIRECTION_GEN;
    if (CMD_OK != _fc_parse_str(PARSE_FC_SRC, arg_src_str, &fc_src)) {
        return CMD_USAGE;
    }
    fc_status_key.src_type = fc_src;

    if (fc_src == 1) {
        if (CMD_OK != _fc_parse_str(PARSE_FC_VSQ_TYPE, arg_vsq_type_str, &fc_vsq_type)) {
            return CMD_USAGE;
        }
        fc_status_key.vsq_type = fc_vsq_type;
        fc_status_key.vsq_id = arg_vsq_id;

        if (fc_vsq_type == SOC_TMC_ITM_VSQ_GROUP_LLFC) {      /* LLFC VSQ */
            is_force_fc_type = 1;
            fc_type_forced = SOC_TMC_FC_NIF_TYPE_LLFC;
        }
        else if (fc_vsq_type == SOC_TMC_ITM_VSQ_GROUP_PFC) { /* PFC VSQ */
            is_force_fc_type = 1;
            fc_type_forced = SOC_TMC_FC_NIF_TYPE_PFC;
        }
    }

    if (is_force_fc_type) {
        fc_status_key.nif_fc_type = fc_type_forced;
    }
    else {
        if (CMD_OK != _fc_parse_str(PARSE_NIF_FC_TYPE, arg_fc_type_str, &fc_type)) {
            return CMD_USAGE;
        }
        fc_status_key.nif_fc_type = fc_type;   
    }

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(&fc_status_info, 0, sizeof(fc_status_info));
    soc_rc = soc_dpp_fc_status_info_get(unit, &fc_status_key, &fc_status_info);
    if (SOC_FAILURE(soc_rc))
    {
        return CMD_FAIL;
    }

    _dpp_fc_nif_gen_print(unit, &fc_status_key, &fc_status_info); 

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_fc_nif_rec_status_show(int unit, args_t *a)
{
    parse_table_t pt;
    soc_port_t arg_port = 0;
    char *arg_fc_type_str = NULL;
    int fc_type = 0;
    soc_error_t soc_rc;
    SOC_TMC_FC_STATUS_KEY fc_status_key;
    SOC_TMC_FC_STATUS_INFO fc_status_info;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "port", PQ_DFL|PQ_INT, NULL,
                    &arg_port, NULL);
    parse_table_add(&pt, "fc_type", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_fc_type_str, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    sal_memset(&fc_status_key, 0, sizeof(fc_status_key));
    fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_NIF;
    fc_status_key.port = arg_port;
    fc_status_key.direction = SOC_TMC_FC_DIRECTION_REC;
    
    if (CMD_OK != _fc_parse_str(PARSE_NIF_FC_TYPE, arg_fc_type_str, &fc_type)) {
        return CMD_USAGE;
    }
    fc_status_key.nif_fc_type = fc_type;

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(&fc_status_info, 0, sizeof(fc_status_info));
    soc_rc = soc_dpp_fc_status_info_get(unit, &fc_status_key, &fc_status_info);
    if (SOC_FAILURE(soc_rc))
    {
        return CMD_FAIL;
    }

    _dpp_fc_nif_rec_print(unit, &fc_status_key, &fc_status_info); 

    return CMD_OK;

}

STATIC cmd_result_t
cmd_dpp_fc_ilkn_gen_status_show(int unit, args_t *a)
{
    parse_table_t pt;
    soc_port_t arg_port = 0;
    int arg_calendar_id = 0;
    soc_error_t soc_rc;
    SOC_TMC_FC_STATUS_KEY fc_status_key;
    SOC_TMC_FC_STATUS_INFO fc_status_info;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "port", PQ_DFL|PQ_INT, NULL,
                    &arg_port, NULL);
    parse_table_add(&pt, "calendar_id", PQ_DFL|PQ_INT, NULL,
                    &arg_calendar_id, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    sal_memset(&fc_status_key, 0, sizeof(fc_status_key));
    fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_ILKN;
    fc_status_key.is_oob = FALSE;
    fc_status_key.port = arg_port;
    fc_status_key.calendar_id = arg_calendar_id;
    fc_status_key.direction = SOC_TMC_FC_DIRECTION_GEN;

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(&fc_status_info, 0, sizeof(fc_status_info));
    soc_rc = soc_dpp_fc_status_info_get(unit, &fc_status_key, &fc_status_info);
    if (SOC_FAILURE(soc_rc))
    {
        return CMD_FAIL;
    }

    _dpp_fc_ilkn_gen_print(unit, &fc_status_key, &fc_status_info); 

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_fc_ilkn_rec_status_show(int unit, args_t *a)
{
    parse_table_t pt;
    soc_port_t arg_port = 0;
    char *arg_fc_type_str = NULL;
    int fc_type = 0;
    int arg_calendar_id = 0;

    soc_error_t soc_rc;
    SOC_TMC_FC_STATUS_KEY fc_status_key;
    SOC_TMC_FC_STATUS_INFO fc_status_info;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "port", PQ_DFL|PQ_INT, NULL,
                    &arg_port, NULL);
    parse_table_add(&pt, "fc_type", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_fc_type_str, NULL);
    parse_table_add(&pt, "calendar_id", PQ_DFL|PQ_INT, NULL,
                    &arg_calendar_id, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    sal_memset(&fc_status_key, 0, sizeof(fc_status_key));
    fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_ILKN;
    fc_status_key.is_oob = FALSE;
    fc_status_key.port = arg_port;
    fc_status_key.calendar_id = arg_calendar_id;
    fc_status_key.direction = SOC_TMC_FC_DIRECTION_REC;

    if (CMD_OK != _fc_parse_str(PARSE_CAL_FC_TYPE, arg_fc_type_str, &fc_type)) {
        return CMD_USAGE;
    }
    fc_status_key.calendar_fc_type = fc_type;

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(&fc_status_info, 0, sizeof(fc_status_info));
    soc_rc = soc_dpp_fc_status_info_get(unit, &fc_status_key, &fc_status_info);
    if (SOC_FAILURE(soc_rc))
    {
        return CMD_FAIL;
    }

    _dpp_fc_ilkn_rec_print(unit, &fc_status_key, &fc_status_info); 

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_fc_mub_gen_status_show(int unit, args_t *a)
{
    parse_table_t pt;
    soc_port_t arg_port = 0;
    int arg_calendar_id = 0;
    soc_error_t soc_rc;
    SOC_TMC_FC_STATUS_KEY fc_status_key;
    SOC_TMC_FC_STATUS_INFO fc_status_info;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "port", PQ_DFL|PQ_INT, NULL,
                    &arg_port, NULL);
    parse_table_add(&pt, "calendar_id", PQ_DFL|PQ_INT, NULL,
                    &arg_calendar_id, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    sal_memset(&fc_status_key, 0, sizeof(fc_status_key));
    fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_MUB;
    fc_status_key.is_oob = FALSE;
    fc_status_key.port = arg_port;
    fc_status_key.calendar_id = arg_calendar_id;
    fc_status_key.direction = SOC_TMC_FC_DIRECTION_GEN;

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(&fc_status_info, 0, sizeof(fc_status_info));
    soc_rc = soc_dpp_fc_status_info_get(unit, &fc_status_key, &fc_status_info);
    if (SOC_FAILURE(soc_rc))
    {
        return CMD_FAIL;
    }

    _dpp_fc_mub_gen_print(unit, &fc_status_key, &fc_status_info); 

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_fc_mub_rec_status_show(int unit, args_t *a)
{
    parse_table_t pt;
    soc_port_t arg_port = 0;
    char *arg_fc_type_str = NULL;
    int fc_type = 0;
    int arg_calendar_id = 0;

    soc_error_t soc_rc;
    SOC_TMC_FC_STATUS_KEY fc_status_key;
    SOC_TMC_FC_STATUS_INFO fc_status_info;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "port", PQ_DFL|PQ_INT, NULL,
                    &arg_port, NULL);
    parse_table_add(&pt, "fc_type", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_fc_type_str, NULL);
    parse_table_add(&pt, "calendar_id", PQ_DFL|PQ_INT, NULL,
                    &arg_calendar_id, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    sal_memset(&fc_status_key, 0, sizeof(fc_status_key));
    fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_MUB;
    fc_status_key.is_oob = FALSE;
    fc_status_key.port = arg_port;
    fc_status_key.calendar_id = arg_calendar_id;
    fc_status_key.direction = SOC_TMC_FC_DIRECTION_REC;

    if (CMD_OK != _fc_parse_str(PARSE_CAL_FC_TYPE, arg_fc_type_str, &fc_type)) {
        return CMD_USAGE;
    }
    fc_status_key.calendar_fc_type = fc_type;

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(&fc_status_info, 0, sizeof(fc_status_info));
    soc_rc = soc_dpp_fc_status_info_get(unit, &fc_status_key, &fc_status_info);
    if (SOC_FAILURE(soc_rc))
    {
        return CMD_FAIL;
    }

    _dpp_fc_mub_rec_print(unit, &fc_status_key, &fc_status_info); 

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_fc_oob_gen_status_show(int unit, args_t *a)
{
    parse_table_t pt;
    soc_port_t arg_port = 0;
    int arg_calendar_id = 0;
    char *arg_oob_type_str = NULL;
    int oob_type = 0;
    soc_error_t soc_rc;
    SOC_TMC_FC_STATUS_KEY fc_status_key;
    SOC_TMC_FC_STATUS_INFO fc_status_info;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "port", PQ_DFL|PQ_INT, NULL,
                    &arg_port, NULL);
    parse_table_add(&pt, "calendar_id", PQ_DFL|PQ_INT, NULL,
                    &arg_calendar_id, NULL);
    parse_table_add(&pt, "oob_type", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_oob_type_str, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    sal_memset(&fc_status_key, 0, sizeof(fc_status_key));
    fc_status_key.is_oob = TRUE;
    fc_status_key.port = arg_port;
    fc_status_key.calendar_id = arg_calendar_id;
    fc_status_key.direction = SOC_TMC_FC_DIRECTION_GEN;

    if (CMD_OK != _fc_parse_str(PARSE_OOB_TYPE, arg_oob_type_str, &oob_type)) {
        return CMD_USAGE;
    }

    if (oob_type == 0) {
        fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_ILKN;
    }
    else if (oob_type == 1) {
        fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_SPI;
    }
    else {
        fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_HCFC;        
    }
    fc_status_key.is_oob = TRUE;

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(&fc_status_info, 0, sizeof(fc_status_info));
    soc_rc = soc_dpp_fc_status_info_get(unit, &fc_status_key, &fc_status_info);
    if (SOC_FAILURE(soc_rc))
    {
        return CMD_FAIL;
    }

    _dpp_fc_oob_gen_print(unit, &fc_status_key, &fc_status_info); 

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_fc_oob_rec_status_show(int unit, args_t *a)
{
    parse_table_t pt;
    soc_port_t arg_port = 0;
    int arg_calendar_id = 0;
    char *arg_oob_type_str = NULL;
    int oob_type = 0;
    soc_error_t soc_rc;
    SOC_TMC_FC_STATUS_KEY fc_status_key;
    SOC_TMC_FC_STATUS_INFO fc_status_info;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "port", PQ_DFL|PQ_INT, NULL,
                    &arg_port, NULL);
    parse_table_add(&pt, "calendar_id", PQ_DFL|PQ_INT, NULL,
                    &arg_calendar_id, NULL);
    parse_table_add(&pt, "oob_type", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_oob_type_str, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    sal_memset(&fc_status_key, 0, sizeof(fc_status_key));
    fc_status_key.is_oob = TRUE;
    fc_status_key.port = arg_port;
    fc_status_key.calendar_id = arg_calendar_id;
    fc_status_key.direction = SOC_TMC_FC_DIRECTION_REC;

    if (CMD_OK != _fc_parse_str(PARSE_OOB_TYPE, arg_oob_type_str, &oob_type)) {
        return CMD_USAGE;
    }
    if (oob_type == 0) {
        fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_ILKN;
    }
    else if (oob_type == 1) {
        fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_SPI;
    }
    else {
        fc_status_key.intf_type = SOC_TMC_FC_INTF_TYPE_HCFC;        
    }

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(&fc_status_info, 0, sizeof(fc_status_info));
    soc_rc = soc_dpp_fc_status_info_get(unit, &fc_status_key, &fc_status_info);
    if (SOC_FAILURE(soc_rc))
    {
        return CMD_FAIL;
    }

    _dpp_fc_oob_rec_print(unit, &fc_status_key, &fc_status_info); 

    return CMD_OK;
}

STATIC cmd_t _dpp_fc_nif_gen_status_subcmds[] = {
        {"Show", cmd_dpp_fc_nif_gen_status_show, FC_NIF_GEN_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_nif_rec_status_subcmds[] = {
        {"Show", cmd_dpp_fc_nif_rec_status_show, FC_NIF_REC_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_ilkn_gen_status_subcmds[] = {
        {"Show", cmd_dpp_fc_ilkn_gen_status_show, FC_ILKN_GEN_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_ilkn_rec_status_subcmds[] = {
        {"Show", cmd_dpp_fc_ilkn_rec_status_show, FC_ILKN_REC_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_mub_gen_status_subcmds[] = {
        {"Show", cmd_dpp_fc_mub_gen_status_show, FC_MUB_GEN_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_mub_rec_status_subcmds[] = {
        {"Show", cmd_dpp_fc_mub_rec_status_show, FC_MUB_REC_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_oob_gen_status_subcmds[] = {
        {"Show", cmd_dpp_fc_oob_gen_status_show, FC_OOB_GEN_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_oob_rec_status_subcmds[] = {
        {"Show", cmd_dpp_fc_oob_rec_status_show, FC_OOB_REC_STATUS_USAGE, NULL}
    };

STATIC cmd_result_t
cmd_dpp_fc_nif_gen_status(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_nif_gen_status_subcmds, COUNTOF(_dpp_fc_nif_gen_status_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_nif_rec_status(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_nif_rec_status_subcmds, COUNTOF(_dpp_fc_nif_rec_status_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_ilkn_gen_status(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_ilkn_gen_status_subcmds, COUNTOF(_dpp_fc_ilkn_gen_status_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_ilkn_rec_status(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_ilkn_rec_status_subcmds, COUNTOF(_dpp_fc_ilkn_rec_status_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_mub_gen_status(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_mub_gen_status_subcmds, COUNTOF(_dpp_fc_mub_gen_status_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_mub_rec_status(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_mub_rec_status_subcmds, COUNTOF(_dpp_fc_mub_rec_status_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_oob_gen_status(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_oob_gen_status_subcmds, COUNTOF(_dpp_fc_oob_gen_status_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_oob_rec_status(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_oob_rec_status_subcmds, COUNTOF(_dpp_fc_oob_rec_status_subcmds));
}

STATIC cmd_t _dpp_fc_nif_gen_subcmds[] = {
        {"Status", cmd_dpp_fc_nif_gen_status, FC_NIF_GEN_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_nif_rec_subcmds[] = {
        {"Status", cmd_dpp_fc_nif_rec_status, FC_NIF_REC_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_ilkn_gen_subcmds[] = {
        {"Status", cmd_dpp_fc_ilkn_gen_status, FC_ILKN_GEN_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_ilkn_rec_subcmds[] = {
        {"Status", cmd_dpp_fc_ilkn_rec_status, FC_ILKN_REC_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_mub_gen_subcmds[] = {
        {"Status", cmd_dpp_fc_mub_gen_status, FC_MUB_GEN_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_mub_rec_subcmds[] = {
        {"Status", cmd_dpp_fc_mub_rec_status, FC_MUB_REC_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_oob_gen_subcmds[] = {
        {"Status", cmd_dpp_fc_oob_gen_status, FC_OOB_GEN_STATUS_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_oob_rec_subcmds[] = {
        {"Status", cmd_dpp_fc_oob_rec_status, FC_OOB_REC_STATUS_USAGE, NULL}
    };

STATIC cmd_result_t
cmd_dpp_fc_nif_gen(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_nif_gen_subcmds, COUNTOF(_dpp_fc_nif_gen_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_nif_rec(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_nif_rec_subcmds, COUNTOF(_dpp_fc_nif_rec_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_ilkn_gen(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_ilkn_gen_subcmds, COUNTOF(_dpp_fc_ilkn_gen_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_ilkn_rec(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_ilkn_rec_subcmds, COUNTOF(_dpp_fc_ilkn_rec_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_mub_gen(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_mub_gen_subcmds, COUNTOF(_dpp_fc_mub_gen_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_mub_rec(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_mub_rec_subcmds, COUNTOF(_dpp_fc_mub_rec_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_oob_gen(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_oob_gen_subcmds, COUNTOF(_dpp_fc_oob_gen_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_oob_rec(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_oob_rec_subcmds, COUNTOF(_dpp_fc_oob_rec_subcmds));
}

STATIC cmd_t _dpp_fc_nif_subcmds[] = {
        {"Gen", cmd_dpp_fc_nif_gen, FC_NIF_GEN_USAGE, NULL},
        {"Rec", cmd_dpp_fc_nif_rec, FC_NIF_REC_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_ilkn_subcmds[] = {
        {"Gen", cmd_dpp_fc_ilkn_gen, FC_ILKN_GEN_USAGE, NULL},
        {"Rec", cmd_dpp_fc_ilkn_rec, FC_ILKN_REC_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_mub_subcmds[] = {
        {"Gen", cmd_dpp_fc_mub_gen, FC_MUB_GEN_USAGE, NULL},
        {"Rec", cmd_dpp_fc_mub_rec, FC_MUB_REC_USAGE, NULL}
    };

STATIC cmd_t _dpp_fc_oob_subcmds[] = {
        {"Gen", cmd_dpp_fc_oob_gen, FC_OOB_GEN_USAGE, NULL},
        {"Rec", cmd_dpp_fc_oob_rec, FC_OOB_REC_USAGE, NULL}
    };

STATIC cmd_result_t
cmd_dpp_fc_nif(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_nif_subcmds, COUNTOF(_dpp_fc_nif_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_ilkn(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_ilkn_subcmds, COUNTOF(_dpp_fc_ilkn_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_mub(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_mub_subcmds, COUNTOF(_dpp_fc_mub_subcmds));
}

STATIC cmd_result_t
cmd_dpp_fc_oob(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_oob_subcmds, COUNTOF(_dpp_fc_oob_subcmds));
}

STATIC cmd_t _dpp_fc_subcmds[] = {
        {"Nif", cmd_dpp_fc_nif, FC_NIF_USAGE, NULL},
        {"Ilkn", cmd_dpp_fc_ilkn, FC_ILKN_USAGE, NULL},
        {"Mub", cmd_dpp_fc_mub, FC_MUB_USAGE, NULL},
        {"Oob", cmd_dpp_fc_oob, FC_OOB_USAGE, NULL}
    };

cmd_result_t
cmd_dpp_fc(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_fc_subcmds, COUNTOF(_dpp_fc_subcmds));
}

#endif

