/** \file diag_dnx_oam.c
 *
 * Main diagnostics for oam application
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Structure of inputs to diag oam cl
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/sand/sand_signals.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for access*/
#include <appl/diag/diag.h>
#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_signals.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_signals.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/appl/diag/dnx/dbal/diag_dnx_dbal.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/oam/oam_counter.h>
#include <src/bcm/dnx/stat/crps/crps_mgmt_internal.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <include/soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "src/bcm/dnx/oam/oamp_pe_infra.h"

#include "diag_dnx_oam.h"

/** To be used int rmep_traverse call back function */
#define MAX_RMEPS_TO_DISPLAY 10

/** Display options for oam ep   */
#define DISPLAY_OFF 0
#define DISPLAY_ON 1

/** Pipeline options for oam id   */
#define PIPE_TYPE_INGRESS 1
#define PIPE_TYPE_EGRESS 2
#define PIPE_TYPE_BOTH 3

/** To be used in oam cntrs */
#define MAX_NOF_DATABASES 34
#define MAX_NOF_ENGINES 34

#define DIAG_DNX_OAM_READ_SIGNAL(func)\
do                      \
    {                       \
      int _rv = func;      \
      if (SHR_FAILURE(_rv)) \
      {                     \
        LOG_ERROR_EX(BSL_LOG_MODULE,                       \
          " Signal read failed '%s' indicated. Resuming %s%s%s\r\n" ,           \
          shrextend_errmsg_get(_rv) ,EMPTY,EMPTY,EMPTY) ;  \
      }                     \
} while (0)

static int rmep_index;
static bcm_oam_endpoint_t rmep_list[MAX_RMEPS_TO_DISPLAY];

/*
 * Structure of inputs of "oam oamid"
 */
typedef struct
{
    uint32 core_id;
    uint32 is_acc;
    uint32 pipe;
    uint32 oam_id;
    uint32 ing;
    uint32 profile_id;
    uint32 opcode;
    uint32 pkt_is_bfd;
    uint32 mymac;
    uint32 mp_type;
    uint32 mep_id;
} oam_oamid_params_t;

/*
 * Structure of inputs of "oam cf"
 */
typedef struct
{
    uint32 core_id;
    uint32 lif;
    uint32 lif_1;
    uint32 lif_2;
    uint32 lif_3;
    uint32 opcode;
    uint32 mdl;
    uint32 mymac;
    uint32 inj;
    uint32 da_is_mc;
    uint32 ydv;
    uint32 ing;
    uint32 pkt_is_bfd;
    uint32 color;
    uint32 last_run;
} oam_cf_inputs_t;

/*
 * Structure of outputs of "oam cl"
 */
typedef struct
{
    uint32 local_lif_1;
    uint32 local_lif_2;
    uint32 local_lif_3;
    uint32 local_lif;
    uint32 lif_key_1;
    uint32 lif_key_2;
    uint32 lif_key_3;
    uint32 oam_1_key;
    uint32 oam_2_key;
    uint32 oam_subtype;
    uint32 trap_code;
    uint32 trap_strength;
    uint32 snoop_strength;
    uint32 meter_disable;
    uint32 up_mep;
    uint32 oam_id;
    uint32 oama_dis_1;
    uint32 oama_dis_2;
    uint32 oama_dis_3;
    uint32 oamb_dis;
    uint32 acc_mp_profile;
    uint32 acc_entry_found;
    dnx_oam_lif_entry_value_t lif_db_access_1;
    dnx_oam_lif_entry_value_t lif_db_access_2;
    dnx_oam_lif_entry_value_t lif_db_access_3;
    uint32 lif_1_entry_found;
    uint32 lif_2_entry_found;
    uint32 lif_3_entry_found;
    uint32 counter_inc_1;
    uint32 counter_inc_2;
    uint32 counter_inc_3;
    uint32 counter_read_idx;
    uint32 counter_disable_map_1;
    uint32 counter_disable_map_2;
    uint32 counter_disable_map_3;
    uint32 profile_en_map_1;
    uint32 profile_en_map_2;
    uint32 profile_en_map_3;
    uint32 pkt_is_oam;
    uint32 inlif_valid4lm;
    uint32 nof_lm_lifs;
    uint32 mp_type;
} oam_cf_results_t;

/*
 * Structure of outputs of "oamp ctrs"
 */
typedef struct
{
    uint32 tx_counter;
    uint32 rx_counter;
    uint32 itr_drop;
    uint32 oversized_rx_pkt;
    uint32 itr_count;
    uint32 txm_dropped_count;
    uint32 txo_dropped_count;
} oamp_ctrs_diag_output_t;

/*
 * Structure of outputs of "oamp status"
 */
typedef struct
{
    uint8 type_err;
    uint8 trap_code_err;
    uint8 timestamp_err;
    uint8 source_udp_port_err;
    uint8 rmep_state_err;
    uint8 rfc_6374_err;
    uint8 parity_err;
    uint8 mdl_err;
    uint8 maid_err;
    uint8 flex_crc_err;
    uint8 your_discriminator_err;
    uint8 source_ip_err;
    uint8 egress_err;
    uint8 my_discriminator_err;
    uint8 emc_err;
    uint8 channel_type_err;
    uint8 ccm_interval_err;
    uint16 trap_code_rcv;
    uint8 trap_code_match;
    uint16 udp_source_port;
    uint8 mdl_rcv;
    uint8 mdl_exp;
    uint8 maid_type;
    uint8 maid_name_rcv[BCM_OAM_GROUP_NAME_LENGTH];
    uint8 maid_name_exp[BCM_OAM_GROUP_NAME_LENGTH];
    uint16 flex_crc_rcv;
    uint16 flex_crc_exp;
    uint32 your_disc_rcv;
    uint32 your_disc_exp;
    uint32 src_ip_rcv;
    uint32 src_ip_exp;
    uint32 subnet_mask;
    uint8 mep_type;
    uint32 my_disc_rcv;
    uint32 my_disc_exp;
    uint8 ccm_interval_rcv;
    uint8 ccm_interval_exp;
} oamp_status_diag_output_t;

/*
 * Structure of inputs of "oam ep"
 */
typedef struct
{
    int32 core_id;
    uint32 ep_id;
} oam_ep_inputs_t;

/*
 * Structure of outputs of "oam ep"
 */
typedef struct
{
    int32 no_ep;
    int32 no_mips;
    int32 no_ups;
    int32 no_accs;
    int32 no_errs;
    int32 no_rmeps;
    int32 no_eth;
    int32 no_bhh_mpls;
    int32 no_bhh_pwe;
    int32 no_bhh_pwe_gal;
    int32 no_mpls;
    int32 no_bhh_sec;
    bcm_oam_endpoint_info_t endpoint_info;
} oam_ep_results_t;

/*
 * Structure of endpoint parameters for "oam counters"
 */
typedef struct
{
    int id;
    int counter_if;
    int counter_base;
    int is_pcp;
    uint64 ing_counter_value[8];
    uint64 eg_counter_value[8];
} endpoint_results_t;

/*
 * Structure of counter database information for "oam counters"
 */
typedef struct
{
    int is_oam_ing;
    int is_oam_eg;
    int is_oam_oamp;
    int cntr_if;
    int start_counter;
    int end_counter;
    int nof_engines;
    int is_oam;
    int engine[MAX_NOF_ENGINES];
} cntrs_database_t;

/*
 * Structure of endpoints counter information, for "oam counters"
 */
typedef struct
{
    int ing_engine_id;
    int ing_index;
    int eg_engine_id;
    int eg_index;
} mep_counter_info_t;

/*
 * Structure of outputs of "oam counters"
 */
typedef struct
{
    cntrs_database_t database[MAX_NOF_DATABASES];
    endpoint_results_t endpoint_results;
} oam_cntrs_results_t;

/*
 * Structure of inputs of "oam cntrs"
 */
typedef struct
{
    int32 core_id;
    uint32 ep_id;
} oam_cntrs_inputs_t;

/*
 * Structure of outputs of "oam ep"
 */
typedef struct
{
    int32 no_ep;
    int32 no_ups;
    int32 no_accs;
    int32 no_errs;
    int32 no_udp_over_ipv4;
    int32 no_udp_over_ipv6;
    int32 no_mpls;
    int32 no_pwe_ctr_wrd;
    int32 no_pwe_ttl;
    int32 no_mpls_tp_cc;
    int32 no_mpls_tp_cc_cv;
    int32 no_pwe_gal;
    int32 no_sbfd_initiator_over_ipv4;
    int32 no_sbfd_initiator_over_ipv6;
    int32 no_sbfd_initiator_over_mpls;
    int32 no_sbfd_reflector_over_ipv4;
    int32 no_sbfd_reflector_over_mpls;
    int32 no_pwe_rtr_alrt;
    int32 no_pwe_rtr_alrt_ach;
    bcm_bfd_endpoint_info_t endpoint_info;
} oam_bfdep_results_t;

/*
 * Structure of outputs of "oam grp"
 */
typedef struct
{
    int32 no_grp;
    int32 no_err;
    int32 no_meps;
    sh_sand_control_t *sand_control;
} oam_grp_results_t;

/* Structures for OAMP MEP DB diag */
typedef struct _oamp_mep_db_diag_input_s
{
    /*
     * partition subcommand is true/false
     */
    uint8 partition;
    /*
     * allocation subcommand is true/false
     */
    uint8 allocation;
} oamp_mep_db_diag_input_t;

/* Pool types - short/full/extra */
#define SHORT_TYPE 0
#define FULL_TYPE  1
#define EXTRA_TYPE 2

/* Maid types - None/ICC/2B */
#define NONE_TYPE 0
#define ICC_TYPE  1
#define TWO_BYTE_TYPE 2

#define POOL_TYPE_STR(pool_type) ((pool_type == SHORT_TYPE)?"Short":\
                                  (pool_type == FULL_TYPE)?"Full":\
                                  (pool_type == EXTRA_TYPE)?"Extra":"None")

#define MAID_TYPE_STR(maid_type) ((maid_type == NONE_TYPE)?"":\
                                  (maid_type == ICC_TYPE)?"may use ICC MAID":\
                     (maid_type == TWO_BYTE_TYPE)?"may only use 2-byte MAID":"None")

#define MAID_TYPE_ALLOC_STR(maid_type) ((maid_type == NONE_TYPE)?"":\
                                  (maid_type == ICC_TYPE)?"uses ICC MAID":\
                     (maid_type == TWO_BYTE_TYPE)?"uses 2-byte MAID":"None")

/* ICC short, 2B short, ICC full, 2B full, Extra pool */
#define MAX_OAMP_MEP_DB_POOLS 5

/* Max number of MEPs in one pool possible is 64k -UMC (8k)
 * when threshold is set to 64k
 * when all entries are quarter entries.
 */
#define MAX_NUM_MEPS (56 * 1024)

/* Max MEP ID is 96k which will be 5 chars.
 * +1 char for comma character.
 */
#define MAX_CHARS_PER_MEP (6)

#define MAX_MEP_INFO_STR_SIZE (MAX_CHARS_PER_MEP * MAX_NUM_MEPS)

typedef char mep_info_str_t[MAX_MEP_INFO_STR_SIZE];

typedef struct _oamp_mep_db_diag_output_s
{
    /*
     * Number of OAMP MEP DB pools
     */
    uint8 num_pools;
    /*
     * pool type - short or full
     */
    uint8 pool_type[MAX_OAMP_MEP_DB_POOLS];
    /*
     * maid type - icc_maid or 2B maid
     */
    uint8 maid_type[MAX_OAMP_MEP_DB_POOLS];
    /*
     * Total number of MEP IDs in the pool
     */
    int num_meps[MAX_OAMP_MEP_DB_POOLS];
    /*
     * Number of MEP IDs filled in the mep_info below
     */
    int num_filled_meps[MAX_OAMP_MEP_DB_POOLS];
    /*
     * MEP Info - partition info or allocated mep info
     */
    mep_info_str_t mep_info_str[MAX_OAMP_MEP_DB_POOLS];
} oamp_mep_db_diag_output_t;

/* Max OAM_LIF_DB keys */
#define MAX_OAM_LIF_DB_KEYS 3

/* Max OAM_LIF_DB/ACC_MEP_DB keys */
#define MAX_OEM_KEYS 4

/*
   Function check for endianness:
   * returns 1, if architecture is little endian
   * return 0 in case of big endian.
 */
static int
check_for_endianness(
    void)
{
    unsigned int x = 1;
    char *c = (char *) &x;
    return (int) *c;
}

typedef uint32 result_t[2];

/*
 * Structure of outputs of "oam lu"
 */
typedef struct
{
    uint32 core_id;
    /*
     * IOEM-1/2 keys and results
     */
    uint32 ing_keys[MAX_OEM_KEYS];
    uint32 ing_found[MAX_OEM_KEYS];
    result_t ing_results[MAX_OEM_KEYS];
    /*
     * IOEM-1 keys
     */
    uint32 ing_key_prefix[MAX_OAM_LIF_DB_KEYS];
    uint32 ing_key_base[MAX_OAM_LIF_DB_KEYS];
    /*
     * IOEM-1 results
     */
    uint32 ing_mp_type[DNX_OAM_MAX_MDL + 1];
    uint32 ing_mp_profile[MAX_OAM_LIF_DB_KEYS];
    uint32 ing_counter_base[MAX_OAM_LIF_DB_KEYS];
    uint32 ing_counter_intf[MAX_OAM_LIF_DB_KEYS];
    /*
     * IOEM-2 keys
     */
    uint32 ing_oam_lif;
    uint32 ing_mdl;
    uint32 ing_your_disc_valid;
    /*
     * IOEM-2 results
     */
    uint32 ing_oam_id;
    uint32 ing_acc_mp_profile;

    /*
     * EOEM-1/2 keys and results
     */
    uint32 egr_keys[MAX_OEM_KEYS];
    uint32 egr_found[MAX_OEM_KEYS];
    result_t egr_results[MAX_OEM_KEYS];
    /*
     * EOEM-1 keys
     */
    uint32 egr_key_prefix[MAX_OAM_LIF_DB_KEYS];
    uint32 egr_key_base[MAX_OAM_LIF_DB_KEYS];
    /*
     * EOEM-1 results
     */
    uint32 egr_mp_type[DNX_OAM_MAX_MDL + 1];
    uint32 egr_mp_profile[MAX_OAM_LIF_DB_KEYS];
    uint32 egr_counter_base[MAX_OAM_LIF_DB_KEYS];
    uint32 egr_counter_intf[MAX_OAM_LIF_DB_KEYS];
    /*
     * EOEM-2 keys
     */
    uint32 egr_oam_lif;
    uint32 egr_mdl;
    /*
     * EOEM-2 results
     */
    uint32 egr_oam_id;
    uint32 egr_acc_mp_profile;
} oam_lu_params_t;

/**
 * \brief -
 * oam ClassiFier command - Input verification function.
 * Most of the verifications are being performed on HW access
 * This function verifies core id, and allows entering the
 * oam-lif, as lif or lif-1.
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_cf_inputs - pointer to inputs to oam cf
 *        command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cf_verify(
    int unit,
    oam_cf_inputs_t * oam_cf_inputs)
{
    int num_of_cores;
    char disp_line[14][128];
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Verify core_id
     *  Get number of cores for this device
     */
    num_of_cores = dnx_data_device.general.nof_cores_get(unit);
    if (oam_cf_inputs->core_id > num_of_cores)
    {
        /*
         * Default core_id
         */
        oam_cf_inputs->core_id = 0;
    }

    if ((oam_cf_inputs->last_run == 1) && (oam_cf_inputs->ing == 0))
    {
        LOG_CLI((BSL_META_U
                 (unit,
                  "\n\n*************************************\n\nLast EGRESS classifier display is not supported\n\n*************************************\n\n")));
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    /*
     *  Verify lifs
     *  First lif may be entered as lif or lif_1
     */
    if (oam_cf_inputs->lif == DNX_OAM_INVALID_LIF)
    {
        if (oam_cf_inputs->lif_1 == DNX_OAM_INVALID_LIF)
        {

            /*
             *  Print usage
             */
            LOG_CLI((BSL_META_U(unit, "\nInvalid lif value\nUsage:\n------\noam cf lif=xxx <options>...<options>\n")));
            sal_sprintf(disp_line[13], "LAST     - Display classifier state from last packet\n");
            sal_sprintf(disp_line[0], "lif      - OAM-LIF - Global for egress, local for ingress\n");
            sal_sprintf(disp_line[1], "lif_1    - Global for egress, local for ingress\n");
            sal_sprintf(disp_line[2], "lif_2    - Global for egress, local for ingress\n");
            sal_sprintf(disp_line[3], "lif_3    - Global for egress, local for ingress\n");
            sal_sprintf(disp_line[4], "OPcode   - oam-opcode of the incoming packet\n");
            sal_sprintf(disp_line[5], "mdl      - oam level of the incoming packet\n");
            sal_sprintf(disp_line[6], "mymac    - Set if packet's dst equals mep's mac\n");
            sal_sprintf(disp_line[7], "inj      - Set for injected packet\n");
            sal_sprintf(disp_line[8], "da_is_mc - Set if multicast\n");
            sal_sprintf(disp_line[9], "ydv      - Your Disc is Valid\n");
            sal_sprintf(disp_line[10], "INGress - Set for ingress classifier\n");
            sal_sprintf(disp_line[11], "BFD     - Set for BFD packet\n");
            sal_sprintf(disp_line[12], "CoLoR   - Display in color. (0 for no-color)\n");
            LOG_CLI((BSL_META_U(unit, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s"), disp_line[13], disp_line[0], disp_line[1],
                     disp_line[2], disp_line[3], disp_line[4], disp_line[5], disp_line[6], disp_line[7], disp_line[8],
                     disp_line[9], disp_line[10], disp_line[11], disp_line[12]));
            SHR_IF_ERR_EXIT(_SHR_E_NONE);
        }
        else
        {
            /*
             *  If lif was entered as lif_1, accept it
             */
            oam_cf_inputs->lif = oam_cf_inputs->lif_1;
        }
    }
    else
    {
        /*
         *  Assuming oam header is valid so lif_1 = lif
         */
        if ((oam_cf_inputs->lif_1 != oam_cf_inputs->lif) && (oam_cf_inputs->lif_1 != DNX_OAM_INVALID_LIF)
            && (oam_cf_inputs->lif_1 != 0))
        {
            LOG_CLI((BSL_META_U(unit, "Invalid lif value. lif_1 should be equal to lif.\n")));
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
        oam_cf_inputs->lif_1 = oam_cf_inputs->lif;
    }
    if (oam_cf_inputs->ing > 1)
    {
        LOG_CLI((BSL_META_U(unit, "Invalid INGress value. Should be 0 or 1.\n")));
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (oam_cf_inputs->inj > 1)
    {
        LOG_CLI((BSL_META_U(unit, "Invalid inject value. Should be 0 or 1.\n")));
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    /*
     * In case ydv = 1, packet will be considered BFD
     * In case of PWE and ydv=0, pkt_is_bfd (from input) will be used
     */
    if (oam_cf_inputs->ydv == 1)
    {
        oam_cf_inputs->pkt_is_bfd = 1;
    }
    if (oam_cf_inputs->pkt_is_bfd == 1)
    {
        oam_cf_inputs->opcode = 0;
        oam_cf_inputs->mymac = 0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam ClassiFier command - Read inputs from signals
 * Used when LAST option is on.
 * \param [in] unit -     Relevant unit.
 * \param [in] core -     Relevant core.
 * \param [in] *oam_cf_inputs - pointer to inputs to oam cf
 *        command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cf_read_signals(
    int unit,
    int core,
    oam_cf_inputs_t * oam_cf_inputs)
{
    int core_num;

    SHR_FUNC_INIT_VARS(unit);

    core_num = dnx_data_device.general.nof_cores_get(unit);

    if ((core < 0) || (core >= core_num))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal core ID:%d for device\n", core);
    }
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "OAM_LIF", &oam_cf_inputs->lif, 1));
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "VTT_OAM_LIF_0",
                              &oam_cf_inputs->lif_1, 1));
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "VTT_OAM_LIF_1",
                              &oam_cf_inputs->lif_2, 1));
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "VTT_OAM_LIF_2",
                              &oam_cf_inputs->lif_3, 1));
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "Mdl", &oam_cf_inputs->mdl, 1));
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "OAM_Opcode",
                              &oam_cf_inputs->opcode, 1));
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "OAM_Your_Discr",
                              &oam_cf_inputs->ydv, 1));
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "Pkt_is_BFD",
                              &oam_cf_inputs->pkt_is_bfd, 1));
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "My_CFM_MAC", &oam_cf_inputs->mymac,
                              1));
    DIAG_DNX_OAM_READ_SIGNAL(dpp_dsig_read
                             (unit, oam_cf_inputs->core_id, "IRPP", "FWD1", "FWD2", "Pkt_is_Compatible_MC",
                              &oam_cf_inputs->da_is_mc, 1));

    if (oam_cf_inputs->lif == 0)
    {
        oam_cf_inputs->lif = DNX_OAM_INVALID_LIF;
    }
    if (oam_cf_inputs->lif_1 == 0)
    {
        oam_cf_inputs->lif_1 = DNX_OAM_INVALID_LIF;
    }
    if (oam_cf_inputs->lif_2 == 0)
    {
        oam_cf_inputs->lif_2 = DNX_OAM_INVALID_LIF;
    }
    if (oam_cf_inputs->lif_3 == 0)
    {
        oam_cf_inputs->lif_3 = DNX_OAM_INVALID_LIF;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam ClassiFier command - helper function.
 * Gets local oam-lifs if needed (for egress)
 * Prepares  inlif_valid4lm and nof_lm_lifs for key select
 * \param [in] unit -     Relevant unit.
 * \param [in] oam_cf_inputs - inputs to oam cf command
 * \param [in] *oam_cf_results - pointer to the outputs of the
 *        classification. The functions fills in the relevant
 *        fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cf_get_lifs_validity_4lm(
    int unit,
    oam_cf_inputs_t oam_cf_inputs,
    oam_cf_results_t * oam_cf_results)
{
    int local_lif;
    int rv;
    /*
     *  Assuming oam header is valid so lif_1 = lif
     */
    int local_lif2;
    int local_lif3;
    uint32 temp_inlif_valid4lm;
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    uint32 dbal_result_type;
    dnx_algo_gpm_gport_hw_resources_t hw_res;
    bcm_gport_t lif_in_tunnel;
    dbal_fields_e lif_field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    rv = _SHR_E_NONE;
    dbal_table_id = DBAL_TABLE_EMPTY;
    local_lif = oam_cf_inputs.lif;

    /*
     * Ingress - Input to diag is global
     * Egress  - Input to diag is local
     */
    if (oam_cf_inputs.ydv == 0)
    {
        if (oam_cf_inputs.ing == 1)
        {
            /*
             * Turn the global lif into a tunnel, then call gport to hw resources.
             */
            sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
            BCM_GPORT_TUNNEL_ID_SET(lif_in_tunnel, oam_cf_inputs.lif);

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, lif_in_tunnel,
                                                               DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                               &hw_res));

            local_lif = hw_res.local_in_lif;
            dbal_table_id = hw_res.inlif_dbal_table_id;
            dbal_result_type = hw_res.inlif_dbal_result_type;
            lif_field = DBAL_FIELD_IN_LIF;
        }
        else
        {
            local_lif = oam_cf_inputs.lif;

            rv = dnx_lif_mngr_outlif_sw_info_get(unit, local_lif, &dbal_table_id, &dbal_result_type, NULL, NULL, NULL,
                                                 NULL);
            lif_field = DBAL_FIELD_OUT_LIF;
        }
    }

    if (rv == _SHR_E_NOT_FOUND)
    {
        dbal_table_id = DBAL_NOF_TABLES;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
    if ((oam_cf_inputs.ing == 0) && (oam_cf_inputs.pkt_is_bfd == 0) && (dbal_table_id == DBAL_TABLE_EEDB_OUT_AC))
    {
        local_lif = oam_cf_inputs.lif;
        local_lif2 = oam_cf_inputs.lif_2;
        local_lif3 = oam_cf_inputs.lif_3;
    }
    else
    {
        if (oam_cf_inputs.ydv == 1)
        {
            local_lif = oam_cf_inputs.lif;
        }
        local_lif2 = DNX_OAM_INVALID_LIF;
        local_lif3 = DNX_OAM_INVALID_LIF;
        if (oam_cf_inputs.lif_2 != DNX_OAM_INVALID_LIF)
        {
            sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
            BCM_GPORT_TUNNEL_ID_SET(lif_in_tunnel, oam_cf_inputs.lif_2);

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, lif_in_tunnel,
                                                               DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                               &hw_res));

            local_lif2 = hw_res.local_in_lif;
        }
        if (oam_cf_inputs.lif_3 != DNX_OAM_INVALID_LIF)
        {
            sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
            BCM_GPORT_TUNNEL_ID_SET(lif_in_tunnel, oam_cf_inputs.lif_3);

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, lif_in_tunnel,
                                                               DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                               &hw_res));

            local_lif3 = hw_res.local_in_lif;
        }
    }
    oam_cf_results->local_lif = local_lif;
    oam_cf_results->local_lif_1 = local_lif;
    oam_cf_results->local_lif_2 = local_lif2;
    oam_cf_results->local_lif_3 = local_lif3;

    /*
     *  Calculate nof_lm_lifs
     */
    oam_cf_results->nof_lm_lifs = 0;
    oam_cf_results->inlif_valid4lm = 0;
    if ((oam_cf_inputs.ydv == 0) && (oam_cf_inputs.pkt_is_bfd == 0)
        && ((dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB) || (dbal_table_id == DBAL_TABLE_EEDB_OUT_AC)))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, lif_field, local_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, dbal_result_type);
        rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        if (rv != _SHR_E_NONE)
        {
            printf
                ("\n\n*************************************\n\nIllegal or unallocated lif.\n\n*************************************\n\n");
            return rv;
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE,
                                                            &(oam_cf_results->inlif_valid4lm)));
        oam_cf_results->nof_lm_lifs += oam_cf_results->inlif_valid4lm;
        if (oam_cf_inputs.lif_2 != DNX_OAM_INVALID_LIF)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, lif_field, local_lif2);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, &temp_inlif_valid4lm));
            oam_cf_results->nof_lm_lifs += temp_inlif_valid4lm;
        }
        if (oam_cf_inputs.lif_3 != DNX_OAM_INVALID_LIF)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, lif_field, local_lif3);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, &temp_inlif_valid4lm));
            oam_cf_results->nof_lm_lifs += temp_inlif_valid4lm;
        }
    }
    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam ClassiFier command - counter resolution function
 * Accesses to HW and performs required calculations to produce
 * oam counter resolution. Also produces HW table miss/hit
 * indications for convenient display.
 * \param [in] unit -     Relevant unit.
 * \param [in] oam_cf_inputs - inputs to oam cf command
 * \param [in] *oam_cf_results - pointer to the outputs of the
 *        classification. The functions fills in the relevant
 *        fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cf_get_counter_resolution(
    int unit,
    oam_cf_inputs_t oam_cf_inputs,
    oam_cf_results_t * oam_cf_results)
{
    int table;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     *  Get Counter_disable_map results
     */
    if (oam_cf_inputs.ing)
    {
        table = DBAL_TABLE_INGRESS_OAM_COUNTER_GENERAL_ENABLE;
    }
    else
    {
        table = DBAL_TABLE_EGRESS_OAM_COUNTER_GENERAL_ENABLE;
    }

    /*
     * If acc_entry is not found oamb_dis should be 0
     */
    SHR_IF_ERR_EXIT(dnx_oam_counter_disable_map_entry_get(unit, table, oam_cf_results->nof_lm_lifs,
                                                          (oam_cf_results->oamb_dis & oam_cf_results->acc_entry_found),
                                                          oam_cf_results->inlif_valid4lm, oam_cf_results->mp_type,
                                                          &(oam_cf_results->counter_disable_map_1),
                                                          &(oam_cf_results->counter_disable_map_2),
                                                          &(oam_cf_results->counter_disable_map_3)));

    if (oam_cf_results->lif_1_entry_found)
    {
        SHR_IF_ERR_EXIT(dnx_oam_lif_db_access_profile_en_counter_disable_get
                        (unit, oam_cf_inputs.ing, 0, oam_cf_results->lif_db_access_1.mp_profile, 1,
                         &(oam_cf_results->profile_en_map_1)));
    }
    else
    {
        oam_cf_results->profile_en_map_1 = 0;
    }
    if (oam_cf_results->lif_2_entry_found)
    {
        SHR_IF_ERR_EXIT(dnx_oam_lif_db_access_profile_en_counter_disable_get
                        (unit, oam_cf_inputs.ing, 1, oam_cf_results->lif_db_access_2.mp_profile, 1,
                         &(oam_cf_results->profile_en_map_2)));
    }
    else
    {
        oam_cf_results->profile_en_map_2 = 0;
    }
    if (oam_cf_results->lif_3_entry_found)
    {
        SHR_IF_ERR_EXIT(dnx_oam_lif_db_access_profile_en_counter_disable_get
                        (unit, oam_cf_inputs.ing, 2, oam_cf_results->lif_db_access_3.mp_profile, 1,
                         &(oam_cf_results->profile_en_map_3)));
    }
    else
    {
        oam_cf_results->profile_en_map_3 = 0;
    }

   /** Assume - no PCP. */
    oam_cf_results->counter_inc_1 =
        (!(oam_cf_results->oama_dis_1 | oam_cf_results->counter_disable_map_1 | oam_cf_results->profile_en_map_1) & 1);
    oam_cf_results->counter_inc_2 =
        (!(oam_cf_results->oama_dis_2 | oam_cf_results->counter_disable_map_2 | oam_cf_results->profile_en_map_2) & 1);
    oam_cf_results->counter_inc_3 =
        (!(oam_cf_results->oama_dis_3 | oam_cf_results->counter_disable_map_3 | oam_cf_results->profile_en_map_3) & 1);
    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam ClassiFier command - classifier resolution function
 * Accesses to HW and performs required calculations to produce
 * oam classifier resolution. Also produces HW table miss/hit
 * indications for convenient display.
 * \param [in] unit -     Relevant unit.
 * \param [in] oam_cf_inputs - inputs to oam cf command
 * \param [in] *oam_cf_results - pointer to the outputs of the
 *        classification. The functions fills in the relevant
 *        fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cf_get_results(
    int unit,
    oam_cf_inputs_t oam_cf_inputs,
    oam_cf_results_t * oam_cf_results)
{
    uint32 key_pre_1;
    uint32 key_pre_2;
    uint32 key_pre_3;
    uint32 key_base_0;
    uint32 key_base_1;
    uint32 key_base_2;
    uint32 oam_base_0;
    uint32 oam_base_1;
    uint32 oam_base_2;
    uint32 nof_mep_above_mdl;
    uint32 nof_mep_below_mdl;
    uint32 mip_above_mdl;
    uint32 level;
    uint32 internal_opcode;
    uint32 flags;
    uint32 packet_is_bfd;
    shr_error_e rv;
    dnx_oam_lif_entry_key_t oam_lif_entry_key;
    oam_action_key_t action_key;
    oam_action_content_t action_content;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     *  Get is_inlif_valid_for_lm and nof_valid_lm
     */
    rv = diag_oam_cf_get_lifs_validity_4lm(unit, oam_cf_inputs, oam_cf_results);
    if (rv != _SHR_E_NONE)
    {
        printf("\n\n****************************\n\nIllegal inputs.\n\n****************************\n\n");
        SHR_EXIT();
    }

    /*
     *  KEY SELECT
     */
    if (oam_cf_inputs.ing)
    {
        SHR_IF_ERR_EXIT(dnx_oam_key_select_ingress_get
                        (unit, oam_cf_results->nof_lm_lifs, oam_cf_results->inlif_valid4lm, oam_cf_inputs.ydv,
                         &key_pre_1, &key_pre_2, &key_pre_3, &key_base_0, &key_base_1, &key_base_2));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_key_select_egress_get
                        (unit, oam_cf_results->nof_lm_lifs, 1, oam_cf_inputs.inj, 1, &key_pre_1, &key_pre_2,
                         &key_pre_3, &key_base_0, &key_base_1, &key_base_2));
    }

    /*
     *  oam key base selection
     */
    if (oam_cf_inputs.ing)
    {
        {
            if (key_base_0 == 1)
                oam_base_0 = oam_cf_inputs.lif;
            else if (key_base_0 == 2)
                oam_base_0 = oam_cf_inputs.lif_3;
            else if (key_base_0 == 3)
                oam_base_0 = oam_cf_inputs.lif_2;
            else if (key_base_0 == 4)
                oam_base_0 = oam_cf_inputs.lif_1;
            else
            {
                oam_base_0 = 0;
            }
        }
        {
            if (key_base_1 == 1)
                oam_base_1 = oam_cf_inputs.lif;
            else if (key_base_1 == 2)
                oam_base_1 = oam_cf_inputs.lif_3;
            else if (key_base_1 == 3)
                oam_base_1 = oam_cf_inputs.lif_2;
            else if (key_base_1 == 4)
                oam_base_1 = oam_cf_inputs.lif_1;
            else
            {
                oam_base_1 = 0;
            }
        }
        {
            if (key_base_2 == 1)
                oam_base_2 = oam_cf_inputs.lif;
            else if (key_base_2 == 2)
                oam_base_2 = oam_cf_inputs.lif_3;
            else if (key_base_2 == 3)
                oam_base_2 = oam_cf_inputs.lif_2;
            else if (key_base_2 == 4)
                oam_base_2 = oam_cf_inputs.lif_1;
            else
            {
                oam_base_2 = 0;
            }
        }
    }
    else
    {
        {
            if (key_base_0 == 1)
                oam_base_0 = oam_cf_results->local_lif_1;
            else if (key_base_0 == 2)
                oam_base_0 = oam_cf_results->local_lif_2;
            else if (key_base_0 == 3)
                oam_base_0 = oam_cf_results->local_lif_3;
            else if (key_base_0 == 4)
                oam_base_0 = oam_cf_results->local_lif;
            else
            {
                oam_base_0 = 0;
            }
        }
        {
            if (key_base_1 == 1)
                oam_base_1 = oam_cf_results->local_lif_1;
            else if (key_base_1 == 2)
                oam_base_1 = oam_cf_results->local_lif_2;
            else if (key_base_1 == 3)
                oam_base_1 = oam_cf_results->local_lif_3;
            else if (key_base_1 == 4)
                oam_base_1 = oam_cf_results->local_lif;
            else
            {
                oam_base_1 = 0;
            }
        }
        {
            if (key_base_2 == 1)
                oam_base_2 = oam_cf_results->local_lif_1;
            else if (key_base_2 == 2)
                oam_base_2 = oam_cf_results->local_lif_2;
            else if (key_base_2 == 3)
                oam_base_2 = oam_cf_results->local_lif_3;
            else if (key_base_2 == 4)
                oam_base_2 = oam_cf_results->local_lif;
            else
            {
                oam_base_2 = 0;
            }
        }
    }

    /*
     *  End of oam key base selection
     */

    /*
     *  oam-lif-db : 3 accesses
     */
    oam_lif_entry_key.oam_key_prefix = key_pre_1;
    oam_lif_entry_key.oam_lif_id = oam_base_0;
    oam_lif_entry_key.flags = oam_cf_inputs.ing == 1 ? DNX_OAM_CLASSIFIER_INGRESS : DNX_OAM_CLASSIFIER_EGRESS;
    oam_lif_entry_key.core_id = oam_cf_inputs.core_id;
    oam_cf_results->lif_key_1 = key_pre_1 << 22 | (oam_base_0 & 0x3fffff);
    rv = dnx_oam_lif_get(unit, &oam_lif_entry_key, &(oam_cf_results->lif_db_access_1));
    if (rv == _SHR_E_NOT_FOUND)
    {
        oam_cf_results->lif_1_entry_found = 0;
        oam_cf_results->lif_db_access_1.counter_base = 0xdead;
        oam_cf_results->lif_db_access_1.counter_interface = 0;
        oam_cf_results->lif_db_access_1.mdl_mp_type[oam_cf_inputs.mdl] = 0;
        oam_cf_results->lif_db_access_1.mp_profile = 0xdead;
    }
    else
    {
        oam_cf_results->lif_1_entry_found = 1;
    }
    if (oam_base_1 != 0)
    {
        oam_lif_entry_key.oam_key_prefix = key_pre_2;
        oam_lif_entry_key.oam_lif_id = oam_base_1;
        oam_cf_results->lif_key_2 = key_pre_2 << 22 | (oam_base_1 & 0x3fffff);
        rv = dnx_oam_lif_get(unit, &oam_lif_entry_key, &(oam_cf_results->lif_db_access_2));
        if (rv == _SHR_E_NOT_FOUND)
        {
            oam_cf_results->lif_2_entry_found = 0;
            oam_cf_results->lif_db_access_2.counter_base = 0xdead;
            oam_cf_results->lif_db_access_2.counter_interface = 0;
            oam_cf_results->lif_db_access_2.mdl_mp_type[oam_cf_inputs.mdl] = 0;
            oam_cf_results->lif_db_access_2.mp_profile = 0xdead;
        }
        else
        {
            oam_cf_results->lif_2_entry_found = 1;
        }
    }
    else
    {
        oam_cf_results->lif_2_entry_found = 0;
        oam_cf_results->lif_db_access_2.counter_base = 0xdead;
        oam_cf_results->lif_db_access_2.counter_interface = 0;
        oam_cf_results->lif_db_access_2.mdl_mp_type[oam_cf_inputs.mdl] = 0;
        oam_cf_results->lif_db_access_2.mp_profile = 0xdead;
    }
    if (oam_base_2 != 0)
    {
        oam_lif_entry_key.oam_key_prefix = key_pre_3;
        oam_lif_entry_key.oam_lif_id = oam_base_2;
        oam_cf_results->lif_key_3 = key_pre_3 << 22 | (oam_base_2 & 0x3fffff);
        rv = dnx_oam_lif_get(unit, &oam_lif_entry_key, &(oam_cf_results->lif_db_access_3));
        if (rv == _SHR_E_NOT_FOUND)
        {
            oam_cf_results->lif_3_entry_found = 0;
            oam_cf_results->lif_db_access_3.counter_base = 0xdead;
            oam_cf_results->lif_db_access_3.counter_interface = 0;
            oam_cf_results->lif_db_access_3.mdl_mp_type[oam_cf_inputs.mdl] = 0;
            oam_cf_results->lif_db_access_3.mp_profile = 0xdead;
        }
        else
        {
            oam_cf_results->lif_3_entry_found = 1;
        }
    }
    else
    {
        oam_cf_results->lif_3_entry_found = 0;
        oam_cf_results->lif_db_access_3.counter_base = 0xdead;
        oam_cf_results->lif_db_access_3.counter_interface = 0;
        oam_cf_results->lif_db_access_3.mdl_mp_type[oam_cf_inputs.mdl] = 0;
        oam_cf_results->lif_db_access_3.mp_profile = 0xdead;

    }

    /*
     *  Calculate mp_type table keys
     */
    nof_mep_above_mdl = 0;
    nof_mep_below_mdl = 0;
    mip_above_mdl = 0;
    for (level = 0; level <= DNX_OAM_MAX_MDL; level++)
    {
        if (oam_cf_inputs.mdl < level)
        {
            if ((oam_cf_results->lif_db_access_1.mdl_mp_type[level] == DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP) ||
                (oam_cf_results->lif_db_access_1.mdl_mp_type[level] == DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP))

            {
                nof_mep_above_mdl++;
            }
            mip_above_mdl |= oam_cf_results->lif_db_access_1.mdl_mp_type[level] == DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP;
        }
        if (oam_cf_inputs.mdl > level)
        {
            if ((oam_cf_results->lif_db_access_1.mdl_mp_type[level] == DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP) ||
                (oam_cf_results->lif_db_access_1.mdl_mp_type[level] == DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP))
            {
                nof_mep_below_mdl++;
            }
        }
    }

    /*
     *  MP TYPE table access
     */
    SHR_IF_ERR_EXIT(dnx_oam_mp_type_get
                    (unit, oam_cf_inputs.ing, nof_mep_above_mdl,
                     oam_cf_results->lif_db_access_1.mdl_mp_type[oam_cf_inputs.mdl], nof_mep_below_mdl,
                     oam_cf_inputs.pkt_is_bfd, mip_above_mdl, &oam_cf_results->mp_type));

    /*
     *  if oem-1 was hit, access oam action table
     */
    if (oam_cf_results->lif_1_entry_found == 1)
    {
        /*
         *  LIF-Action-Table Access
         */
        SHR_IF_ERR_EXIT(dnx_oam_opcode_map_get(unit, oam_cf_inputs.ing, oam_cf_inputs.opcode, &internal_opcode));
        if (oam_cf_inputs.ing)
        {
            flags = DNX_OAM_CLASSIFIER_INGRESS | DNX_OAM_CLASSIFIER_LIF_ACTION;
        }
        else
        {
            flags = DNX_OAM_CLASSIFIER_EGRESS | DNX_OAM_CLASSIFIER_LIF_ACTION;
        }
        action_key.da_is_mc = oam_cf_inputs.da_is_mc;
        action_key.is_inject = oam_cf_inputs.inj;
        action_key.is_my_mac = oam_cf_inputs.mymac;
        action_key.mp_profile = oam_cf_results->lif_db_access_1.mp_profile;
        action_key.mp_type = oam_cf_results->mp_type;
        action_key.oam_internal_opcode = internal_opcode;
        oam_cf_results->oam_1_key = ((action_key.mp_type & 0xf) << 11) |
            ((internal_opcode & 0xf) << 7) |
            ((action_key.mp_profile & 0xf) << 3) |
            ((action_key.is_my_mac & 0x1) << 2) | ((action_key.is_inject & 0x1) << 1) | (action_key.da_is_mc & 0x1);
        SHR_IF_ERR_EXIT(dnx_oam_action_get(unit, flags, &action_key, &action_content));
        oam_cf_results->trap_strength = action_content.forwarding_strength;
        oam_cf_results->oam_subtype = action_content.oam_sub_type;
        oam_cf_results->oama_dis_1 = action_content.oama_counter_disable_access_0;
        oam_cf_results->oama_dis_2 = action_content.oama_counter_disable_access_1;
        oam_cf_results->oama_dis_3 = action_content.oama_counter_disable_access_2;
        oam_cf_results->snoop_strength = action_content.snoop_strength;
        if (oam_cf_inputs.ing)
        {
            oam_cf_results->trap_code = action_content.table_specific_fields.ingress_only_fields.ingress_trap_code;
            oam_cf_results->up_mep = action_content.table_specific_fields.ingress_only_fields.is_up_mep;
            oam_cf_results->meter_disable = action_content.table_specific_fields.ingress_only_fields.meter_disable;
        }
        else
        {
            oam_cf_results->trap_code = action_content.table_specific_fields.egress_only_fields.egress_action_profile;
            oam_cf_results->up_mep = 0xdead;
            oam_cf_results->meter_disable = 0xdead;
        }
    }
    flags = oam_cf_inputs.ing;

    if (oam_cf_results->mp_type == DBAL_ENUM_FVAL_MP_TYPE_BFD)
    {
        flags |= DNX_OAM_CLASSIFIER_BFD;
    }

    /*
     *  Acc-MEP-DB Access
     */
    rv = dnx_oam_acc_mep_get(unit, oam_cf_inputs.core_id, flags, oam_base_0, oam_cf_inputs.mdl,
                             &(oam_cf_results->oam_id), &(oam_cf_results->acc_mp_profile));
    if (rv == _SHR_E_NOT_FOUND)
    {
        oam_cf_results->acc_entry_found = 0;
        oam_cf_results->oamb_dis = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
        oam_cf_results->acc_entry_found = 1;

        /*
         *  Acc-MEP-Action-Table Access
         */
        SHR_IF_ERR_EXIT(dnx_oam_opcode_map_get(unit, oam_cf_inputs.ing, oam_cf_inputs.opcode, &internal_opcode));
        if (oam_cf_inputs.ing)
        {
            flags = DNX_OAM_CLASSIFIER_INGRESS | DNX_OAM_CLASSIFIER_ACC_MEP_ACTION;
        }
        else
        {
            flags = DNX_OAM_CLASSIFIER_EGRESS | DNX_OAM_CLASSIFIER_ACC_MEP_ACTION;
        }
        action_key.da_is_mc = oam_cf_inputs.da_is_mc;
        action_key.is_inject = oam_cf_inputs.inj;
        action_key.is_my_mac = oam_cf_inputs.mymac;
        action_key.mp_profile = oam_cf_results->acc_mp_profile;
        action_key.oam_internal_opcode = internal_opcode;
        action_key.mp_type = 0; /* Not relevant for ACC MEP action DB */
        packet_is_bfd = (oam_cf_results->mp_type == DBAL_ENUM_FVAL_MP_TYPE_BFD) ? 1 : 0;
        oam_cf_results->oam_2_key = (action_key.da_is_mc & 0x1) |
            ((action_key.is_inject & 0x1) << 1) |
            ((action_key.is_my_mac & 0x1) << 2) |
            ((internal_opcode & 0xf) << 3) | ((action_key.mp_profile & 0xf) << 7) | ((packet_is_bfd & 0x1) << 11);
        SHR_IF_ERR_EXIT(dnx_oam_action_get(unit, flags, &action_key, &action_content));
        oam_cf_results->trap_strength = action_content.forwarding_strength;
        oam_cf_results->oam_subtype = action_content.oam_sub_type;
        oam_cf_results->oamb_dis = action_content.oamb_counter_disable;
        oam_cf_results->snoop_strength = action_content.snoop_strength;
        oam_cf_results->trap_code = action_content.table_specific_fields.ingress_only_fields.ingress_trap_code;
        oam_cf_results->up_mep = action_content.table_specific_fields.ingress_only_fields.is_up_mep;
        oam_cf_results->meter_disable = action_content.table_specific_fields.ingress_only_fields.meter_disable;
    }
    SHR_IF_ERR_EXIT(diag_oam_cf_get_counter_resolution(unit, oam_cf_inputs, oam_cf_results));
    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam ClassiFier command - display function
 * Displays the output of the claffication on a drawing. Uses
 * colors to represent valid/relevant outputs.
 * \param [in] unit -     Relevant unit.
 * \param [in] oam_cf_inputs - inputs to oam cf command
 * \param [in] oam_cf_results - outputs of the classification
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cf_display(
    int unit,
    oam_cf_inputs_t * oam_cf_inputs,
    oam_cf_results_t * oam_cf_results)
{
    char disp_line[61][100];
    char color_line[10];
    char reg_color_line[10];
    SHR_FUNC_INIT_VARS(unit);
    if (oam_cf_inputs->color)
    {
        sal_sprintf(color_line, "\033[96m");
        sal_sprintf(reg_color_line, "\033[0m");
        LOG_CLI((BSL_META_U(unit, "\n\033[92mOAM - CLASSIFIER \n----------------\nInputs:\033[0m\n")));
        if (oam_cf_inputs->last_run == 1)
        {
            LOG_CLI((BSL_META_U(unit, "\033[92m LAST PACKET\033[0m\n")));
        }
        LOG_CLI((BSL_META_U
                 (unit,
                  "\n\033[92mLAST=%d lif=0x%x lif1=0x%x lif2=0x%x lif3=0x%x opcode=%d INGress=%d INJect=%d mdl=%d mymac=%d da_is_mc=%d ydv=%d BFD=%d CoLoR=%d\033[0m\n"),
                 oam_cf_inputs->last_run, oam_cf_inputs->lif, oam_cf_inputs->lif_1, oam_cf_inputs->lif_2,
                 oam_cf_inputs->lif_3, oam_cf_inputs->opcode, oam_cf_inputs->ing, oam_cf_inputs->inj,
                 oam_cf_inputs->mdl, oam_cf_inputs->mymac, oam_cf_inputs->da_is_mc, oam_cf_inputs->ydv,
                 oam_cf_inputs->pkt_is_bfd, oam_cf_inputs->color));
        LOG_CLI((BSL_META_U(unit, "\n\033[92mLegend:\033[0m HIT - Yellow, \033[96mMISS - Blue\033[0m\n\n")));
    }
    else
    {
        LOG_CLI((BSL_META_U(unit, "\nOAM - CLASSIFIER \n----------------\nInputs:\n")));
        if (oam_cf_inputs->last_run == 1)
        {
            LOG_CLI((BSL_META_U(unit, " LAST PACKET\n")));
        }
        LOG_CLI((BSL_META_U
                 (unit,
                  "\nLAST=%d lif=0x%x lif1=0x%x lif2=0x%x lif3=0x%x opcode=%d INGress=%d INJect=%d mdl=%d mymac=%d da_is_mc=%d ydv=%d BFD=%d CoLoR=%d\n"),
                 oam_cf_inputs->last_run, oam_cf_inputs->lif, oam_cf_inputs->lif_1, oam_cf_inputs->lif_2,
                 oam_cf_inputs->lif_3, oam_cf_inputs->opcode, oam_cf_inputs->ing, oam_cf_inputs->inj,
                 oam_cf_inputs->mdl, oam_cf_inputs->mymac, oam_cf_inputs->da_is_mc, oam_cf_inputs->ydv,
                 oam_cf_inputs->pkt_is_bfd, oam_cf_inputs->color));
    }

    /*
     *  Left section
     */
    sal_sprintf(disp_line[60], "pkt_is_bfd(0x%08x)-|                               ", oam_cf_inputs->pkt_is_bfd);
    sal_sprintf(disp_line[0], "opcode..(0x%08x)---|                               ", oam_cf_inputs->opcode);
    sal_sprintf(disp_line[1], "mymac...(0x%08x)---|                               ", oam_cf_inputs->mymac);
    sal_sprintf(disp_line[2], "inject..(0x%08x)---|-------------------------------", oam_cf_inputs->inj);
    sal_sprintf(disp_line[3], "da_is_mc(0x%08x)---|                               ", oam_cf_inputs->da_is_mc);
    sal_sprintf(disp_line[4], "                                                       ");
    sal_sprintf(disp_line[5], "                     _______                           ");
    sal_sprintf(disp_line[6], "                    |       |                          ");
    if (oam_cf_results->lif_1_entry_found)
    {
        sal_sprintf(disp_line[7], "LIF-1(0x%08x)-->|       |--lif_key_1(0x%08x)---", oam_cf_inputs->lif_1,
                    oam_cf_results->lif_key_1);
    }
    else
    {
        sal_sprintf(disp_line[7], "LIF-1(0x%08x)-->|       |%s--lif_key_1(0x%08x)---%s",
                    oam_cf_inputs->lif_1, color_line, oam_cf_results->lif_key_1, reg_color_line);
    }
    if (oam_cf_results->lif_2_entry_found)
    {
        sal_sprintf(disp_line[8], "LIF-2(0x%08x)-->|       |--lif_key_2(0x%08x)---", oam_cf_inputs->lif_2,
                    oam_cf_results->lif_key_2);
    }
    else
    {
        sal_sprintf(disp_line[8], "LIF-2(0x%08x)-->|       |%s--lif_key_2(0x%08x)---%s",
                    oam_cf_inputs->lif_2, color_line, oam_cf_results->lif_key_2, reg_color_line);
    }
    if (oam_cf_results->lif_3_entry_found)
    {
        sal_sprintf(disp_line[9], "LIF-3(0x%08x)-->|  KEY  |--lif_key_3(0x%08x)---", oam_cf_inputs->lif_3,
                    oam_cf_results->lif_key_3);
    }
    else
    {
        sal_sprintf(disp_line[9], "LIF-3(0x%08x)-->|  KEY  |%s--lif_key_3(0x%08x)---%s",
                    oam_cf_inputs->lif_3, color_line, oam_cf_results->lif_key_3, reg_color_line);
    }
    sal_sprintf(disp_line[10], "LIF..(0x%08x)-->|       |                          ", oam_cf_inputs->lif);
    sal_sprintf(disp_line[11], "                    |  SEL  |                          ");
    sal_sprintf(disp_line[12], "ydv..(0x%08x)-->|       |                          ", oam_cf_inputs->ydv);
    sal_sprintf(disp_line[13], "                    |_______|                          ");
    sal_sprintf(disp_line[14], "                                                       ");
    sal_sprintf(disp_line[15], "                                                       ");
    if (oam_cf_results->acc_entry_found)
    {
        sal_sprintf(disp_line[16], "LIF..(0x%08x)--------------------------------------", oam_cf_inputs->lif);
        sal_sprintf(disp_line[17], "ydv..(0x%08x)--------------------------------------", oam_cf_inputs->ydv);
        sal_sprintf(disp_line[18], "mdl..(0x%08x)--------------------------------------", oam_cf_inputs->mdl);
    }
    else
    {
        sal_sprintf(disp_line[16], "%sLIF..(0x%08x)--------------------------------------%s",
                    color_line, oam_cf_inputs->lif, reg_color_line);
        sal_sprintf(disp_line[17], "%sydv..(0x%08x)--------------------------------------%s",
                    color_line, oam_cf_inputs->ydv, reg_color_line);
        sal_sprintf(disp_line[18], "%smdl..(0x%08x)--------------------------------------%s",
                    color_line, oam_cf_inputs->mdl, reg_color_line);
    }
    sal_sprintf(disp_line[19], "                                                       ");
    if (oam_cf_results->acc_entry_found)
    {
        /*
         *  Middle section
         */
        sal_sprintf(disp_line[20], "                                                                  ");
        sal_sprintf(disp_line[21], "                                                                  ");
        sal_sprintf(disp_line[22], "------------------------------                                    ");
        sal_sprintf(disp_line[23], "                              |                                   ");
        sal_sprintf(disp_line[24], "     mp-type...(0x%08x)-> |                                   ",
                    oam_cf_results->mp_type);
        if (oam_cf_results->lif_1_entry_found)
        {
            sal_sprintf(disp_line[25], "    _______                   |___________                        ");
            sal_sprintf(disp_line[26], "   |       |                              |                       ");
            sal_sprintf(disp_line[27], "-->|  LIF  |-                             |                       ");
            if (oam_cf_results->lif_2_entry_found)
            {
                sal_sprintf(disp_line[28], "-->|   DB  | |-                           |--oam-2-key(0x%08x)",
                            oam_cf_results->oam_2_key);
            }
            else
            {
                sal_sprintf(disp_line[28],
                            "%s-->%s|   DB  | |-                           |--oam-2-key(0x%08x)",
                            color_line, reg_color_line, oam_cf_results->oam_2_key);
            }
            if (oam_cf_results->lif_3_entry_found)
            {
                sal_sprintf(disp_line[29], "-->|  1-3  | | |                          |                       ");
            }
            else
            {
                sal_sprintf(disp_line[29],
                            "%s-->%s|  1-3  | | |                          |                       ", color_line,
                            reg_color_line);
            }
            sal_sprintf(disp_line[30], "   |_______| | |                          |                       ");
            sal_sprintf(disp_line[31], "    |________| |                          |                       ");
            sal_sprintf(disp_line[32], "      |________|                          |                       ");
        }
        else
        {
            sal_sprintf(disp_line[25],
                        "    %s_______%s                   |___________                        ", color_line,
                        reg_color_line);
            sal_sprintf(disp_line[26], "   %s|       |%s                              |                       ",
                        color_line, reg_color_line);
            sal_sprintf(disp_line[27], "%s-->|  LIF  |-%s                             |                       ",
                        color_line, reg_color_line);
            if (oam_cf_results->lif_2_entry_found)
            {
                sal_sprintf(disp_line[28],
                            "-->%s|   DB  | |-%s                           |--oam-2-key(0x%08x)",
                            color_line, reg_color_line, oam_cf_results->oam_2_key);
            }
            else
            {
                sal_sprintf(disp_line[28],
                            "%s-->|   DB  | |-%s                           |--oam-2-key(0x%08x)",
                            color_line, reg_color_line, oam_cf_results->oam_2_key);
            }
            if (oam_cf_results->lif_3_entry_found)
            {
                sal_sprintf(disp_line[29],
                            "-->%s|  1-3  | | |%s                          |                       ", color_line,
                            reg_color_line);
            }
            else
            {
                sal_sprintf(disp_line[29],
                            "%s-->|  1-3  | | |%s                          |                       ", color_line,
                            reg_color_line);
            }
            sal_sprintf(disp_line[30],
                        "   %s|_______| | |%s                          |                       ", color_line,
                        reg_color_line);
            sal_sprintf(disp_line[31], "    %s|________| |%s                          |                       ",
                        color_line, reg_color_line);
            sal_sprintf(disp_line[32], "      %s|________|%s                          |                       ",
                        color_line, reg_color_line);
        }
        sal_sprintf(disp_line[33], "                                          |                       ");
        if (oam_cf_results->acc_entry_found)
        {
            sal_sprintf(disp_line[34], "    _______                               |                       ");
            sal_sprintf(disp_line[35], "   |       |                              |                       ");
            sal_sprintf(disp_line[36], "-->|  ACC  |-acc-mp_profile(0x%08x)-->|                       ",
                        oam_cf_results->acc_mp_profile);
            sal_sprintf(disp_line[37], "-->|  LIF  |                                                      ");
            sal_sprintf(disp_line[38], "-->|  DB   |------------------------------------------------------");
            sal_sprintf(disp_line[39], "   |_______|                                                      ");
        }
        else
        {
            sal_sprintf(disp_line[34],
                        "    %s_______%s                               |                       ", color_line,
                        reg_color_line);
            sal_sprintf(disp_line[35], "   %s|       |%s                              |                       ",
                        color_line, reg_color_line);
            sal_sprintf(disp_line[36], "%s-->|  ACC  |-acc-mp_profile(0x%08x)-->|%s                       ", color_line,
                        oam_cf_results->acc_mp_profile, reg_color_line);
            sal_sprintf(disp_line[37], "%s-->|  LIF  |%s                                                      ",
                        color_line, reg_color_line);
            sal_sprintf(disp_line[38], "%s-->|  DB   |------------------------------------------------------%s",
                        color_line, reg_color_line);
            sal_sprintf(disp_line[39], "   %s|_______|%s                                                      ",
                        color_line, reg_color_line);
        }

        /*
         *  Right section
         */
        sal_sprintf(disp_line[40], "                                             ");
        sal_sprintf(disp_line[41], "                                             ");
        sal_sprintf(disp_line[42], "                                             ");
        sal_sprintf(disp_line[43], "                                             ");
        sal_sprintf(disp_line[44], "                                             ");
        sal_sprintf(disp_line[45], "     _______                                 ");
        sal_sprintf(disp_line[46], "    |       | --- oam_subtype----(0x%08x)--->", oam_cf_results->oam_subtype);
        if (oam_cf_inputs->ing)
        {
            sal_sprintf(disp_line[47], "    |  LIF  | --- trap_code------(0x%08x)--->", oam_cf_results->trap_code);
        }
        else
        {
            sal_sprintf(disp_line[47], "    |  LIF  | --- action-profile-(0x%08x)--->", oam_cf_results->trap_code);
        }
        sal_sprintf(disp_line[48], "--->|  MEP  | --- trap_strength--(0x%08x)--->", oam_cf_results->trap_strength);
        sal_sprintf(disp_line[49], "    | Action| --- snoop_strength-(0x%08x)--->", oam_cf_results->snoop_strength);
        if (oam_cf_inputs->ing)
        {
            sal_sprintf(disp_line[50], "    |  OAMB | --- meter_disable--(0x%08x)--->", oam_cf_results->meter_disable);
            sal_sprintf(disp_line[51], "    |_______| --- up_mep---------(0x%08x)--->", oam_cf_results->up_mep);
        }
        else
        {
            sal_sprintf(disp_line[50], "    |  OAMB |                                ");
            sal_sprintf(disp_line[51], "    |_______|                                ");
        }
        sal_sprintf(disp_line[52], "                                             ");
        sal_sprintf(disp_line[53], "                                             ");
        sal_sprintf(disp_line[54], "                                             ");
        sal_sprintf(disp_line[55], "                                             ");
        sal_sprintf(disp_line[56], "                                             ");
        sal_sprintf(disp_line[57], "                                             ");
        sal_sprintf(disp_line[58], "----------------- oam_id---------(0x%08x)--->", oam_cf_results->oam_id);
        sal_sprintf(disp_line[59], "                                             ");
    }
    else
    {
        /*
         * Acc-MEP-DB Miss
         */
        /*
         *  Middle section
         */
        sal_sprintf(disp_line[20], "                                                                  ");
        sal_sprintf(disp_line[21], "                                                                  ");
        sal_sprintf(disp_line[22], "------------------------------                                    ");
        sal_sprintf(disp_line[23], "                              |                                   ");
        sal_sprintf(disp_line[24], "     mp-type...(0x%08x)-> |                                   ",
                    oam_cf_results->mp_type);
        if (oam_cf_results->lif_1_entry_found)
        {
            sal_sprintf(disp_line[25], "    _______                   |___________                        ");
            sal_sprintf(disp_line[26], "   |       |                              |                       ");
            sal_sprintf(disp_line[27], "-->|  LIF  |-                             |                       ");
            if (oam_cf_results->lif_2_entry_found)
            {
                sal_sprintf(disp_line[28], "-->|   DB  | |-                           |--oam-1-key(0x%08x)",
                            oam_cf_results->oam_1_key);
            }
            else
            {
                sal_sprintf(disp_line[28],
                            "%s-->%s|   DB  | |-                           |--oam-1-key(0x%08x)",
                            color_line, reg_color_line, oam_cf_results->oam_1_key);
            }
            if (oam_cf_results->lif_3_entry_found)
            {
                sal_sprintf(disp_line[29], "-->|  1-3  | | |--mp_profile(0x%08x)->|                       ",
                            oam_cf_results->lif_db_access_1.mp_profile);
            }
            else
            {
                sal_sprintf(disp_line[29],
                            "%s-->%s|  1-3  | | |--mp_profile(0x%08x)->|                       ",
                            color_line, reg_color_line, oam_cf_results->lif_db_access_1.mp_profile);
            }
            sal_sprintf(disp_line[30], "   |_______| | |                                                  ");
            sal_sprintf(disp_line[31], "    |________| |                                                  ");
            sal_sprintf(disp_line[32], "      |________|                                                  ");
        }
        else
        {
            /*
             *  entry for lif_1 not found
             */
            sal_sprintf(disp_line[25],
                        "    %s_______%s                   |___________                        ", color_line,
                        reg_color_line);
            sal_sprintf(disp_line[26], "   %s|       |%s                              |                       ",
                        color_line, reg_color_line);
            sal_sprintf(disp_line[27], "%s-->|  LIF  |-%s                             |                       ",
                        color_line, reg_color_line);
            if (oam_cf_results->lif_2_entry_found)
            {
                sal_sprintf(disp_line[28],
                            "-->%s|   DB  | |-%s                           |%s--oam-1-key(0x%08x)%s",
                            color_line, reg_color_line, color_line, oam_cf_results->oam_1_key, reg_color_line);
            }
            else
            {
                sal_sprintf(disp_line[28],
                            "%s-->|   DB  | |-%s                           |%s--oam-1-key(0x%08x)%s",
                            color_line, reg_color_line, color_line, oam_cf_results->oam_1_key, reg_color_line);
            }
            if (oam_cf_results->lif_3_entry_found)
            {
                sal_sprintf(disp_line[29],
                            "-->%s|  1-3  | | |--mp_profile(0x%08x)->%s|                       ",
                            color_line, oam_cf_results->lif_db_access_1.mp_profile, reg_color_line);
            }
            else
            {
                sal_sprintf(disp_line[29],
                            "%s-->|  1-3  | | |--mp_profile(0x%08x)->%s|                       ",
                            color_line, oam_cf_results->lif_db_access_1.mp_profile, reg_color_line);
            }
            sal_sprintf(disp_line[30],
                        "   %s|_______| | |%s                                                  ", color_line,
                        reg_color_line);
            sal_sprintf(disp_line[31], "    %s|________| |%s                                                  ",
                        color_line, reg_color_line);
            sal_sprintf(disp_line[32], "      %s|________|%s                                                  ",
                        color_line, reg_color_line);
        }
        sal_sprintf(disp_line[33], "                                                                  ");
        sal_sprintf(disp_line[34], "    %s_______%s                                                       ", color_line,
                    reg_color_line);
        sal_sprintf(disp_line[35], "   %s|       |%s                                                      ", color_line,
                    reg_color_line);
        sal_sprintf(disp_line[36], "%s-->|  ACC  |%s                                                      ", color_line,
                    reg_color_line);
        sal_sprintf(disp_line[37], "%s-->|  LIF  |%s                                                      ", color_line,
                    reg_color_line);
        sal_sprintf(disp_line[38], "%s-->|  DB   |------------------------------------------------------%s", color_line,
                    reg_color_line);
        sal_sprintf(disp_line[39], "   %s|_______|%s                                                      ", color_line,
                    reg_color_line);

        /*
         *  Right section
         */
        sal_sprintf(disp_line[40], "                                             ");
        sal_sprintf(disp_line[41], "                                             ");
        sal_sprintf(disp_line[42], "                                             ");
        sal_sprintf(disp_line[43], "                                             ");
        sal_sprintf(disp_line[44], "                                             ");
        if (oam_cf_results->lif_1_entry_found)
        {
            sal_sprintf(disp_line[45], "     _______                                 ");
            sal_sprintf(disp_line[46], "    |       | --- oam_subtype----(0x%08x)--->", oam_cf_results->oam_subtype);
            if (oam_cf_inputs->ing)
            {
                sal_sprintf(disp_line[47], "    |  LIF  | --- trap_code------(0x%08x)--->", oam_cf_results->trap_code);
            }
            else
            {
                sal_sprintf(disp_line[47], "    |  LIF  | --- action_profile-(0x%08x)--->", oam_cf_results->trap_code);
            }
            sal_sprintf(disp_line[48], "--->| Action| --- trap_strength--(0x%08x)--->", oam_cf_results->trap_strength);
            sal_sprintf(disp_line[49], "    |  OAMA | --- snoop_strength-(0x%08x)--->", oam_cf_results->snoop_strength);
            if (oam_cf_inputs->ing)
            {
                sal_sprintf(disp_line[50], "    |       | --- meter_disable--(0x%08x)--->",
                            oam_cf_results->meter_disable);
                sal_sprintf(disp_line[51], "    |_______| --- up_mep---------(0x%08x)--->", oam_cf_results->up_mep);
            }
            else
            {
                sal_sprintf(disp_line[50], "    |       |                                ");
                sal_sprintf(disp_line[51], "    |_______|                                ");
            }
        }
        else
        {
            sal_sprintf(disp_line[45], "%s     _______                                 %s", color_line, reg_color_line);
            sal_sprintf(disp_line[46], "%s    |       | --- oam_subtype----(0x%08x)--->%s",
                        color_line, oam_cf_results->oam_subtype, reg_color_line);
            if (oam_cf_inputs->ing)
            {
                sal_sprintf(disp_line[47], "%s    |  LIF  | --- trap_code------(0x%08x)--->%s",
                            color_line, oam_cf_results->trap_code, reg_color_line);
            }
            else
            {
                sal_sprintf(disp_line[47], "%s    |  LIF  | --- action_profile-(0x%08x)--->%s",
                            color_line, oam_cf_results->trap_code, reg_color_line);
            }
            sal_sprintf(disp_line[48], "%s--->| Action| --- trap_strength--(0x%08x)--->%s",
                        color_line, oam_cf_results->trap_strength, reg_color_line);
            sal_sprintf(disp_line[49], "%s    |  OAMA | --- snoop_strength-(0x%08x)--->%s",
                        color_line, oam_cf_results->snoop_strength, reg_color_line);
            if (oam_cf_inputs->ing)
            {
                sal_sprintf(disp_line[50], "%s    |       | --- meter_disable--(0x%08x)--->%s",
                            color_line, oam_cf_results->meter_disable, reg_color_line);
                sal_sprintf(disp_line[51], "%s    |_______| --- up_mep---------(0x%08x)--->%s",
                            color_line, oam_cf_results->up_mep, reg_color_line);
            }
            else
            {
                sal_sprintf(disp_line[50], "%s    |       |                                       ", color_line);
                sal_sprintf(disp_line[51], "%s    |_______|                                       ", color_line);
            }
        }
        sal_sprintf(disp_line[52], "                                             ");
        sal_sprintf(disp_line[53], "                                             ");
        sal_sprintf(disp_line[54], "                                             ");
        sal_sprintf(disp_line[55], "                                             ");
        sal_sprintf(disp_line[56], "                                             ");
        sal_sprintf(disp_line[57], "                                             ");
        sal_sprintf(disp_line[58], "%s----------------- oam_id---------(0x%08x)--->%s",
                    color_line, oam_cf_results->oam_id, reg_color_line);
        sal_sprintf(disp_line[59], "                                             ");
    }

    /*
     * Print classifier with data
     */
/* *INDENT-OFF* */
    LOG_CLI((BSL_META_U
             (unit,
              "\n\n\n%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n"),
             disp_line[60],
             disp_line[0], disp_line[20], disp_line[40], disp_line[1], disp_line[21], disp_line[41],
             disp_line[2], disp_line[22], disp_line[42], disp_line[3], disp_line[23], disp_line[43],
             disp_line[4], disp_line[24], disp_line[44], disp_line[5], disp_line[25], disp_line[45],
             disp_line[6], disp_line[26], disp_line[46], disp_line[7], disp_line[27], disp_line[47],
             disp_line[8], disp_line[28], disp_line[48], disp_line[9], disp_line[29], disp_line[49],
             disp_line[10], disp_line[30], disp_line[50], disp_line[11], disp_line[31], disp_line[51],
             disp_line[12], disp_line[32], disp_line[52], disp_line[13], disp_line[33], disp_line[53],
             disp_line[14], disp_line[34], disp_line[54], disp_line[15], disp_line[35], disp_line[55],
             disp_line[16], disp_line[36], disp_line[56], disp_line[17], disp_line[37], disp_line[57],
             disp_line[18], disp_line[38], disp_line[58], disp_line[19], disp_line[39], disp_line[59]));
/* *INDENT-ON* */

    /*
     *  Counter Resolution Display
     */
    sal_sprintf(disp_line[0], "                                                      ");
    sal_sprintf(disp_line[1], "                                                      ");
    if (oam_cf_results->oama_dis_1)
    {
        sal_sprintf(disp_line[2], "                %sOAMA-dis_1---------(0x%08x)------>%s",
                    color_line, oam_cf_results->oama_dis_1, reg_color_line);
    }
    else
    {
        sal_sprintf(disp_line[2], "                OAMA-dis_1---------(0x%08x)------>", oam_cf_results->oama_dis_1);
    }
    if (oam_cf_results->oama_dis_2)
    {
        sal_sprintf(disp_line[3], "                %sOAMA-dis_2---------(0x%08x)------>%s",
                    color_line, oam_cf_results->oama_dis_2, reg_color_line);
    }
    else
    {
        sal_sprintf(disp_line[3], "                OAMA-dis_2---------(0x%08x)------>", oam_cf_results->oama_dis_2);
    }
    if (oam_cf_results->oama_dis_3)
    {
        sal_sprintf(disp_line[4], "                %sOAMA-dis_3---------(0x%08x)------>%s",
                    color_line, oam_cf_results->oama_dis_3, reg_color_line);
    }
    else
    {
        sal_sprintf(disp_line[4], "                OAMA-dis_3---------(0x%08x)------>", oam_cf_results->oama_dis_3);
    }
    if (oam_cf_results->oamb_dis)
    {
        sal_sprintf(disp_line[5], "                %sOAMB-dis-----------(0x%08x)------>%s",
                    color_line, oam_cf_results->oamb_dis, reg_color_line);
    }
    else
    {
        sal_sprintf(disp_line[5], "                OAMB-dis-----------(0x%08x)------>", oam_cf_results->oamb_dis);
    }
    if (oam_cf_results->lif_1_entry_found)
    {
        sal_sprintf(disp_line[6], "                mp_profile_1-------(0x%08x)------>",
                    oam_cf_results->lif_db_access_1.mp_profile);
    }
    else
    {
        sal_sprintf(disp_line[6], "                %smp_profile_1-------(0x%08x)------>%s",
                    color_line, oam_cf_results->lif_db_access_1.mp_profile, reg_color_line);
    }
    if (oam_cf_results->lif_2_entry_found)
    {
        sal_sprintf(disp_line[7], "                mp_profile_2-------(0x%08x)------>",
                    oam_cf_results->lif_db_access_2.mp_profile);
    }
    else
    {
        sal_sprintf(disp_line[7], "                %smp_profile_2-------(0x%08x)------>%s",
                    color_line, oam_cf_results->lif_db_access_2.mp_profile, reg_color_line);
    }
    if (oam_cf_results->lif_3_entry_found)
    {
        sal_sprintf(disp_line[8], "                mp_profile_3-------(0x%08x)------>",
                    oam_cf_results->lif_db_access_3.mp_profile);
    }
    else
    {
        sal_sprintf(disp_line[8], "                %smp_profile_3-------(0x%08x)------>%s",
                    color_line, oam_cf_results->lif_db_access_3.mp_profile, reg_color_line);
    }
    sal_sprintf(disp_line[9], "                pkt_is_oam---------(0x%08x)------>", oam_cf_results->pkt_is_oam);
    sal_sprintf(disp_line[10], "                is_inlif_valid4LM--(0x%08x)------>", oam_cf_results->inlif_valid4lm);
    sal_sprintf(disp_line[11], "                nof_LM_lifs--------(0x%08x)------>", oam_cf_results->nof_lm_lifs);
    sal_sprintf(disp_line[12], "                mp_type------------(0x%08x)------>", oam_cf_results->mp_type);
    sal_sprintf(disp_line[13], "                                                      ");
    sal_sprintf(disp_line[14], " _________                                 ");
    sal_sprintf(disp_line[15], "|         |                                ");
    sal_sprintf(disp_line[16], "|         |                                ");
    sal_sprintf(disp_line[17], "|         |                                ");
    sal_sprintf(disp_line[18], "|         |                                ");
    if (oam_cf_results->lif_1_entry_found)
    {
        sal_sprintf(disp_line[19], "|         |--Counter_if,idx_1--(%01d,0x%08x)----->",
                    oam_cf_results->lif_db_access_1.counter_interface, oam_cf_results->lif_db_access_1.counter_base);
    }
    else
    {
        sal_sprintf(disp_line[19], "|         |%s--Counter_if,idx_1--(%01d,0x%08x)----->%s",
                    color_line, oam_cf_results->lif_db_access_1.counter_interface,
                    oam_cf_results->lif_db_access_1.counter_base, reg_color_line);
    }
    if (oam_cf_results->counter_inc_1)
    {
        sal_sprintf(disp_line[20], "| Counter |--Counter_inc_1----------------(%01d)----->",
                    oam_cf_results->counter_inc_1);
    }
    else
    {
        sal_sprintf(disp_line[20], "| Counter |%s--Counter_inc_1----------------(%01d)----->%s",
                    color_line, oam_cf_results->counter_inc_1, reg_color_line);
    }
    if (oam_cf_results->lif_2_entry_found)
    {
        sal_sprintf(disp_line[21], "|         |--Counter_if,idx_2--(%01d,0x%08x)----->",
                    oam_cf_results->lif_db_access_2.counter_interface, oam_cf_results->lif_db_access_2.counter_base);
    }
    else
    {
        sal_sprintf(disp_line[21], "|         |%s--Counter_if,idx_2--(%01d,0x%08x)----->%s",
                    color_line, oam_cf_results->lif_db_access_2.counter_interface,
                    oam_cf_results->lif_db_access_2.counter_base, reg_color_line);
    }
    if (oam_cf_results->counter_inc_2)
    {
        sal_sprintf(disp_line[22], "|   Res   |--Counter_inc_2----------------(%01d)----->",
                    oam_cf_results->counter_inc_2);
    }
    else
    {
        sal_sprintf(disp_line[22], "|   Res   |%s--Counter_inc_2----------------(%01d)----->%s",
                    color_line, oam_cf_results->counter_inc_2, reg_color_line);
    }
    if (oam_cf_results->lif_3_entry_found)
    {
        sal_sprintf(disp_line[23], "|         |--Counter_if,idx_3--(%01d,0x%08x)----->",
                    oam_cf_results->lif_db_access_3.counter_interface, oam_cf_results->lif_db_access_3.counter_base);
    }
    else
    {
        sal_sprintf(disp_line[23], "|         |%s--Counter_if,idx_3--(%01d,0x%08x)----->%s",
                    color_line, oam_cf_results->lif_db_access_3.counter_interface,
                    oam_cf_results->lif_db_access_3.counter_base, reg_color_line);
    }
    if (oam_cf_results->counter_inc_3)
    {
        sal_sprintf(disp_line[24], "|         |--Counter_inc_3----------------(%01d)----->",
                    oam_cf_results->counter_inc_3);
    }
    else
    {
        sal_sprintf(disp_line[24], "|         |%s--Counter_inc_3----------------(%01d)----->%s",
                    color_line, oam_cf_results->counter_inc_3, reg_color_line);
    }
    sal_sprintf(disp_line[25], "|         |--Counter_Read_idx----(0x%08x)----->", oam_cf_results->counter_read_idx);
    sal_sprintf(disp_line[26], "|         |                                ");
    sal_sprintf(disp_line[27], "|_________|                                ");
    sal_sprintf(disp_line[28], "                                                                 ");
    LOG_CLI((BSL_META_U
             (unit,
              "%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n%s%s%s\n\n"),
             disp_line[28], disp_line[0], disp_line[14], disp_line[28], disp_line[1], disp_line[15],
             disp_line[28], disp_line[2], disp_line[16], disp_line[28], disp_line[3], disp_line[17],
             disp_line[28], disp_line[4], disp_line[18], disp_line[28], disp_line[5], disp_line[19],
             disp_line[28], disp_line[6], disp_line[20], disp_line[28], disp_line[7], disp_line[21],
             disp_line[28], disp_line[8], disp_line[22], disp_line[28], disp_line[9], disp_line[23],
             disp_line[28], disp_line[10], disp_line[24], disp_line[28], disp_line[11], disp_line[25],
             disp_line[28], disp_line[12], disp_line[26], disp_line[28], disp_line[13], disp_line[27]));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam ClassiFier command - inti results
 * \param [in] unit -     Relevant unit.
 * \param [in,out] oam_cf_results - results structure to be
 *        initialized.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cf_init_results(
    int unit,
    oam_cf_results_t * oam_cf_results)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    oam_cf_results->local_lif_1 = 0;
    oam_cf_results->local_lif_2 = 0;
    oam_cf_results->local_lif_3 = 0;
    oam_cf_results->local_lif = 0;
    oam_cf_results->lif_key_1 = 0;
    oam_cf_results->lif_key_2 = 0;
    oam_cf_results->lif_key_3 = 0;
    oam_cf_results->oam_1_key = 0;
    oam_cf_results->oam_2_key = 0;
    oam_cf_results->oam_subtype = 0;
    oam_cf_results->trap_code = 0;
    oam_cf_results->trap_strength = 0;
    oam_cf_results->snoop_strength = 0;
    oam_cf_results->meter_disable = 0;
    oam_cf_results->up_mep = 0;
    oam_cf_results->oam_id = 0;
    oam_cf_results->oama_dis_1 = 1;
    oam_cf_results->oama_dis_2 = 1;
    oam_cf_results->oama_dis_3 = 1;
    oam_cf_results->oamb_dis = 1;
    oam_cf_results->acc_mp_profile = 0;
    oam_cf_results->acc_entry_found = 0;
    oam_cf_results->lif_1_entry_found = 0;
    oam_cf_results->lif_2_entry_found = 0;
    oam_cf_results->lif_3_entry_found = 0;
    oam_cf_results->counter_inc_1 = 0;
    oam_cf_results->counter_inc_2 = 0;
    oam_cf_results->counter_inc_3 = 0;
    oam_cf_results->counter_read_idx = 0;
    oam_cf_results->counter_disable_map_1 = 1;
    oam_cf_results->counter_disable_map_2 = 1;
    oam_cf_results->counter_disable_map_3 = 1;
    oam_cf_results->profile_en_map_1 = 0;
    oam_cf_results->profile_en_map_2 = 0;
    oam_cf_results->profile_en_map_3 = 0;
    oam_cf_results->pkt_is_oam = 0;
    oam_cf_results->inlif_valid4lm = 0;
    oam_cf_results->nof_lm_lifs = 0;
    for (i = 0; i <= DNX_OAM_MAX_MDL; i++)
    {
        oam_cf_results->mp_type = DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam ClassiFier command - main function
 * Gets input and calls verify, get_results and dosplay
 * functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_classifier_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oam_cf_inputs_t oam_cf_inputs;
    oam_cf_results_t oam_cf_results;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_UINT32("LAST", oam_cf_inputs.last_run);
    SH_SAND_GET_UINT32("INGress", oam_cf_inputs.ing);
    SH_SAND_GET_INT32("core", oam_cf_inputs.core_id);
    SH_SAND_GET_UINT32("CoLoR", oam_cf_inputs.color);
    SH_SAND_GET_UINT32("lif", oam_cf_inputs.lif);
    SH_SAND_GET_UINT32("lif1", oam_cf_inputs.lif_1);
    SH_SAND_GET_UINT32("lif2", oam_cf_inputs.lif_2);
    SH_SAND_GET_UINT32("lif3", oam_cf_inputs.lif_3);
    SH_SAND_GET_UINT32("OPcode", oam_cf_inputs.opcode);
    SH_SAND_GET_UINT32("mdl", oam_cf_inputs.mdl);
    SH_SAND_GET_UINT32("mymac", oam_cf_inputs.mymac);
    SH_SAND_GET_UINT32("INJect", oam_cf_inputs.inj);
    SH_SAND_GET_UINT32("da_is_mc", oam_cf_inputs.da_is_mc);
    SH_SAND_GET_UINT32("ydv", oam_cf_inputs.ydv);
    SH_SAND_GET_UINT32("BFD", oam_cf_inputs.pkt_is_bfd);

    SHR_IF_ERR_EXIT(diag_oam_cf_init_results(unit, &oam_cf_results));

    if (oam_cf_inputs.core_id == BCM_CORE_ALL)
    {
        /*
         * Default core
         */
        oam_cf_inputs.core_id = 0;
    }
    if (oam_cf_inputs.last_run == 1)
    {
        SHR_IF_ERR_EXIT(diag_oam_cf_read_signals(unit, oam_cf_inputs.core_id, &oam_cf_inputs));
    }
    /*
     *  Verify inputs
     */
    SHR_INVOKE_VERIFY_DNX(diag_oam_cf_verify(unit, &oam_cf_inputs));
    /*
     *  Get classifier resolution
     */
    SHR_IF_ERR_EXIT(diag_oam_cf_get_results(unit, oam_cf_inputs, &oam_cf_results));

    /*
     *  Display classifier with resolutions
     */
     /* coverity[stack_use_overflow:FALSE]  */
    SHR_IF_ERR_EXIT(diag_oam_cf_display(unit, &oam_cf_inputs, &oam_cf_results));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get and display information regarding last packet
 * \param [in] unit         - Relevant unit.
 * \param [in] sand_control - Controlling structure for shell comamnd framework
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_pe_last_packet(
    int unit,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    int core_idx, core_in;
    uint32 prog_id = 0, prog_enum = 0;
    uint32 tcam_id = 0, tcam_enum = 0;
    uint32 tcam_key = 0, hit = 0;
    uint32 pkt_vars[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core_in);

    PRT_TITLE_SET("OAMP-PE last packet diagnostics:");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Property");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Values");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_STATUS, &entry_handle_id));

    DNXCMN_CORES_ITER(unit, core_in, core_idx)
    {
        char prog_last_name[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
        char tcam_last_name[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
        char key_buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
        char pkt_vars_buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
        char hit_buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_idx);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OAMP_PE_LAST_KEY, INST_SINGLE, &tcam_key));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LAST_PROG_IDX, INST_SINGLE, &prog_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LAST_TCAM_IDX, INST_SINGLE, &tcam_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LAST_TCAM_HIT, INST_SINGLE, &hit));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_LAST_PKT_VARS, INST_SINGLE, pkt_vars));

        SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_hw2enum_get(unit, prog_id, &prog_enum));
        SHR_IF_ERR_EXIT(dnx_oamp_pe_tcam_sw_get(unit, tcam_id, &tcam_enum));

        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_OAMP_PE_PROGRAM_SW_HW2ENUM, DBAL_FIELD_OAMP_PE_PROGRAM_ENUM, &prog_enum, NULL,
                         0, FALSE, prog_last_name));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_OAMP_PE_TCAM_SW, DBAL_FIELD_OAMP_PE_PROG_TCAM_ENUM, &tcam_enum, NULL, 0,
                         FALSE, tcam_last_name));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_OAMP_PE_STATUS, DBAL_FIELD_OAMP_PE_LAST_KEY, &tcam_key, NULL, 0, FALSE,
                         key_buffer));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_OAMP_PE_STATUS, DBAL_FIELD_LAST_TCAM_HIT, &hit, NULL, 0, FALSE, hit_buffer));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_OAMP_PE_STATUS, DBAL_FIELD_LAST_PKT_VARS, pkt_vars, NULL, 0, FALSE,
                         pkt_vars_buffer));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Core ID");
        PRT_CELL_SET("%d", core_idx);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Last TCAM lookup key");
        PRT_CELL_SET("%s", key_buffer);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Last packet vars");
        PRT_CELL_SET("%s", pkt_vars_buffer);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("TCAM hit");
        PRT_CELL_SET("%s", hit_buffer);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Last program selected");
        PRT_CELL_SET("%s", prog_last_name);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Last TCAM selected");
        PRT_CELL_SET("%s", tcam_last_name);
        PRT_ROW_SET_MODE(PRT_ROW_SEP_UNDERSCORE);
    }

    PRT_COMMITX;

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get and display information regarding all loaded TCAMs
 * \param [in] unit -     Relevant unit.
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_pe_tcams(
    int unit,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id_sw, entry_handle_id_hw;
    int is_end;
    uint32 tcam_enum;
    uint32 prog_id, valid;
    uint32 access_idx = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Loaded TCAMs");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Access ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Program ID");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_TCAM_SW, &entry_handle_id_sw));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_TCAM, &entry_handle_id_hw));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id_sw, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_sw, &is_end));

    while (!is_end)
    {
        char tcam_name[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
        uint32 tcam_key_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        uint32 tcam_key_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        char hw_tcam_buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

        /*
         * get TCAM name and access id
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id_sw, DBAL_FIELD_OAMP_PE_PROG_TCAM_ID, &access_idx));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id_sw, DBAL_FIELD_OAMP_PE_PROG_TCAM_ENUM, INST_SINGLE, &tcam_enum));

        /*
         * convert TCAM enum to string
         */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_OAMP_PE_TCAM_SW, DBAL_FIELD_OAMP_PE_PROG_TCAM_ENUM, &tcam_enum, NULL, 0,
                         FALSE, tcam_name));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_sw, &is_end));

        /** set access ID (index) */
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id_hw, access_idx));
        /** get HW TCAM key+mask */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get(unit, entry_handle_id_hw,
                                                                     DBAL_FIELD_OAMP_PE_PROG_TCAM,
                                                                     tcam_key_val, tcam_key_mask));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_OAMP_PE_PROGRAMS_TCAM, DBAL_FIELD_OAMP_PE_PROG_TCAM, tcam_key_val,
                         tcam_key_mask, 0, TRUE, hw_tcam_buffer));

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_hw, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id_hw, DBAL_FIELD_OAMP_PE_PROGRAM, INST_SINGLE, &prog_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id_hw, DBAL_FIELD_VALID, INST_SINGLE, &valid));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", tcam_name);
        PRT_CELL_SET("%d", access_idx);
        PRT_CELL_SET("%s", hw_tcam_buffer);
        PRT_CELL_SET("%d", valid);
        PRT_CELL_SET("%d", prog_id);
    }

    PRT_COMMITX;

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get and display information regarding all loaded programs
 * \param [in] unit -     Relevant unit.
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_pe_programs(
    int unit,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id_sw, entry_handle_id_hw;
    int is_end;
    uint32 prog_enum[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    char program_enum_name[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    uint32 hw_key = 0, const_val, first_inst;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Loaded programs");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "First Instructions");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Const");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAM_SW_HW2ENUM, &entry_handle_id_sw));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_PROPERTIES, &entry_handle_id_hw));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id_sw, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_sw, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id_sw, DBAL_FIELD_OAMP_PE_PROGRAM, &hw_key));

        /*
         * get program enum and convert to string
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id_sw, DBAL_FIELD_OAMP_PE_PROGRAM_ENUM, INST_SINGLE, prog_enum));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_sw, &is_end));

        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_OAMP_PE_PROGRAM_SW_HW2ENUM, DBAL_FIELD_OAMP_PE_PROGRAM_ENUM, prog_enum,
                         NULL, 0, FALSE, program_enum_name));

        /*
         * get program properties - first instruction and cont value
         */
        dbal_entry_key_field32_set(unit, entry_handle_id_hw, DBAL_FIELD_OAMP_PE_PROGRAM_INDEX, hw_key);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_hw, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id_hw, DBAL_FIELD_CONST_VALUE, INST_SINGLE, &const_val));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id_hw, DBAL_FIELD_PROGRAM_ADRESS, INST_SINGLE, &first_inst));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", program_enum_name);
        PRT_CELL_SET("%d", hw_key);
        PRT_CELL_SET("%d", first_inst);
        PRT_CELL_SET("0x%x", const_val);
    }

    PRT_COMMITX;

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * OAMP-PE command - main function
 * Gets input and calls get_results and display functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oamp_pe_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int last, prog, tcam, all;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_BOOL("LAST", last);
    SH_SAND_GET_BOOL("PRoGram", prog);
    SH_SAND_GET_BOOL("TCam", tcam);
    SH_SAND_GET_BOOL("ALL", all);

    if (!last && !tcam && !prog)
    {
        /*
         * if no option is set, then output all PE related info 
         */
        all = 1;
    }

    if (last || all)
    {
        /*
         * diagnostics of last packet
         */
        SHR_IF_ERR_EXIT(diag_oamp_pe_last_packet(unit, sand_control));
    }

    if (tcam || all)
    {
        /*
         * loaded TCAM diagnostics
         */
        SHR_IF_ERR_EXIT(diag_oamp_pe_tcams(unit, sand_control));
    }

    if (prog || all)
    {
        /*
         * loaded programs diagnostics
         */
        SHR_IF_ERR_EXIT(diag_oamp_pe_programs(unit, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get all information from OAMP status
 * \param [in] unit -     Relevant unit.
 * \param [out] *oamp_status_outputs - values to be displayed.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_status_get_results(
    int unit,
    oamp_status_diag_output_t * oamp_status_outputs)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_STATUS, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_TYPE_MISS, INST_SINGLE, &(oamp_status_outputs->type_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_TRAP_CODE_MISS, INST_SINGLE,
                     &(oamp_status_outputs->trap_code_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_TRAP_CODE_RECEIVED, INST_SINGLE,
                     &(oamp_status_outputs->trap_code_rcv)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_TRAP_CODE_MATCH, INST_SINGLE,
                     &(oamp_status_outputs->trap_code_match)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_TIMESTAMP_MISS, INST_SINGLE,
                     &(oamp_status_outputs->timestamp_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_SRC_PORT_MISS, INST_SINGLE,
                     &(oamp_status_outputs->source_udp_port_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_UDP_SRC_PORT_RECEIVED, INST_SINGLE,
                     &(oamp_status_outputs->udp_source_port)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_RMEP_STATE_CHANGE, INST_SINGLE,
                     &(oamp_status_outputs->rmep_state_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_RFC_PUNT, INST_SINGLE, &(oamp_status_outputs->rfc_6374_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_PARITY_ERROR, INST_SINGLE, &(oamp_status_outputs->parity_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_MDL_MISS, INST_SINGLE, &(oamp_status_outputs->mdl_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_MDL_RECEIVED, INST_SINGLE, &(oamp_status_outputs->mdl_rcv)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_MDL_EXPECTED, INST_SINGLE, &(oamp_status_outputs->mdl_exp)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_MAID_MISS, INST_SINGLE, &(oamp_status_outputs->maid_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_MAID_RECEIVED, INST_SINGLE,
                     oamp_status_outputs->maid_name_rcv));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_MAID_EXPECTED, INST_SINGLE,
                     oamp_status_outputs->maid_name_exp));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_MAID_TYPE, INST_SINGLE, &(oamp_status_outputs->maid_type)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FLEX_CRC_MISS, INST_SINGLE,
                     &(oamp_status_outputs->flex_crc_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_FLEX_CRC_RECEIVED, INST_SINGLE,
                     &(oamp_status_outputs->flex_crc_rcv)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_FLEX_CRC_EXPECTED, INST_SINGLE,
                     &(oamp_status_outputs->flex_crc_exp)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_ERR_YOUR_DISC_MISS, INST_SINGLE,
                     &(oamp_status_outputs->your_discriminator_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_YOUR_DISC_RECEIVED, INST_SINGLE,
                     &(oamp_status_outputs->your_disc_rcv)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_YOUR_DISC_EXPECTED, INST_SINGLE,
                     &(oamp_status_outputs->your_disc_exp)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_ERR_SRC_IP_MISS, INST_SINGLE,
                     &(oamp_status_outputs->source_ip_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_SRC_IP_RECEIVED, INST_SINGLE,
                     &(oamp_status_outputs->src_ip_rcv)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_SRC_IP_EXPECTED, INST_SINGLE,
                     &(oamp_status_outputs->src_ip_exp)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_SUBNET_MASK, INST_SINGLE,
                     &(oamp_status_outputs->subnet_mask)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_TYPE, INST_SINGLE, &(oamp_status_outputs->mep_type)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_ERR_PACKET, INST_SINGLE, &(oamp_status_outputs->egress_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_ERR_MY_DISC_MISS, INST_SINGLE,
                     &(oamp_status_outputs->my_discriminator_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_MY_DISC_RECEIVED, INST_SINGLE,
                     &(oamp_status_outputs->my_disc_rcv)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_MY_DISC_EXPECTED, INST_SINGLE,
                     &(oamp_status_outputs->my_disc_exp)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_EMC_MISS, INST_SINGLE, &(oamp_status_outputs->emc_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_CHAN_TYPE_MISS, INST_SINGLE,
                     &(oamp_status_outputs->channel_type_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_CCM_INTERVAL_MISS, INST_SINGLE,
                     &(oamp_status_outputs->ccm_interval_err)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_CCM_INTERVAL_RECEIVED, INST_SINGLE,
                     &(oamp_status_outputs->ccm_interval_rcv)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_PKT_CCM_INTERVAL_EXPECTED, INST_SINGLE,
                     &(oamp_status_outputs->ccm_interval_exp)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oamp ctrs command - display function
 * Displays the OAMP status.
 * \param [in] unit -     Relevant unit.
 * \param [in] *oamp_status_outputs - values to be displayed.
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_status_display(
    int unit,
    oamp_status_diag_output_t * oamp_status_outputs,
    sh_sand_control_t * sand_control)
{

    char maid[16];
    uint32 maid32[BYTES2WORDS(16)];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("OAMP Status(only occured errors presented)");
    PRT_COLUMN_ADD("Error");
    PRT_COLUMN_ADD("Additional Information");
    PRT_COLUMN_ADD("Value");

    if (oamp_status_outputs->type_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "MEP Type");
    }

    if (oamp_status_outputs->trap_code_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "TRAP Code");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Received TRAP Code");
        PRT_CELL_SET("0x%x", (oamp_status_outputs->trap_code_rcv));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "TCAM match");
        if (oamp_status_outputs->trap_code_match)
        {
            PRT_CELL_SET("%s", "YES");
        }
        else
        {
            PRT_CELL_SET("%s", "NO");
        }
    }
    if (oamp_status_outputs->timestamp_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Timestamp");
    }

    if (oamp_status_outputs->source_udp_port_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "UDP Source Port(BFD)");

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Received UDP Source Port");
        PRT_CELL_SET("0x%x", (oamp_status_outputs->udp_source_port));
    }

    if (oamp_status_outputs->rmep_state_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Remote State");
    }

    if (oamp_status_outputs->rfc_6374_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "RFC 6374 not supported");
    }

    if (oamp_status_outputs->parity_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Parity Error");
    }

    if (oamp_status_outputs->mdl_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "MDL");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Received MDL");
        PRT_CELL_SET("%d", (oamp_status_outputs->mdl_rcv));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Expected MDL");
        PRT_CELL_SET("%d", (oamp_status_outputs->mdl_exp));
    }

    if (oamp_status_outputs->maid_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "MAID");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "MAID Type");
        if (oamp_status_outputs->maid_type)
        {
            PRT_CELL_SET("%s", "ICC");
        }
        else
        {
            PRT_CELL_SET("%s", "short");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Received MAID");
        utilex_U8_to_U32(oamp_status_outputs->maid_name_rcv, 16, maid32);
        sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, maid32, maid, 16 * 8, PRINT_BIG_ENDIAN);
        PRT_CELL_SET("%s", maid);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Expected MAID");
        utilex_U8_to_U32(oamp_status_outputs->maid_name_exp, 16, maid32);
        sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, maid32, maid, 16 * 8, PRINT_BIG_ENDIAN);
        PRT_CELL_SET("%s", maid);
    }

    if (oamp_status_outputs->flex_crc_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Flexible CRC");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Received CRC");
        PRT_CELL_SET("0x%x", oamp_status_outputs->flex_crc_rcv);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Expected CRC");
        PRT_CELL_SET("0x%x", oamp_status_outputs->flex_crc_exp);
    }

    if (oamp_status_outputs->source_ip_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Source IP");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Received Source IP");
        PRT_CELL_SET("%d.%d.%d.%d", (oamp_status_outputs->src_ip_rcv >> 24) & 0xff,
                     (oamp_status_outputs->src_ip_rcv >> 16) & 0xff,
                     (oamp_status_outputs->src_ip_rcv >> 8) & 0xff, (oamp_status_outputs->src_ip_rcv) & 0xff);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Expected Source IP");
        PRT_CELL_SET("%d.%d.%d.%d", (oamp_status_outputs->src_ip_exp >> 24) & 0xff,
                     (oamp_status_outputs->src_ip_exp >> 16) & 0xff,
                     (oamp_status_outputs->src_ip_exp >> 8) & 0xff, (oamp_status_outputs->src_ip_exp) & 0xff);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Subnet mask");
        PRT_CELL_SET("%d.%d.%d.%d", (oamp_status_outputs->subnet_mask >> 24) & 0xff,
                     (oamp_status_outputs->subnet_mask >> 16) & 0xff,
                     (oamp_status_outputs->subnet_mask >> 8) & 0xff, (oamp_status_outputs->subnet_mask) & 0xff);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "MEP Type");
        PRT_CELL_SET("%d", oamp_status_outputs->mep_type);
    }

    if (oamp_status_outputs->egress_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Egress error");
    }

    if (oamp_status_outputs->my_discriminator_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "My Discriminator");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Received My Discriminator");
        PRT_CELL_SET("%d", oamp_status_outputs->my_disc_rcv);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Expected My Discriminator");
        PRT_CELL_SET("%d", oamp_status_outputs->my_disc_exp);
    }

    if (oamp_status_outputs->your_discriminator_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Your Discriminator");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Received your Discriminator");
        PRT_CELL_SET("%d", oamp_status_outputs->your_disc_rcv);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Expected your Discriminator");
        PRT_CELL_SET("%d", oamp_status_outputs->your_disc_exp);
    }

    if (oamp_status_outputs->emc_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "EMC");
    }

    if (oamp_status_outputs->channel_type_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Channel Type");
    }

    if (oamp_status_outputs->ccm_interval_err)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "CCM Interval");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Received CCM Interval");
        PRT_CELL_SET("%d", oamp_status_outputs->ccm_interval_rcv);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%s", "Expected CCM Interval");
        PRT_CELL_SET("%d", oamp_status_outputs->ccm_interval_exp);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * oamp status command - main function
 * Calls get_results and display functions
 * \param [in] unit - Relevant unit
 * \param [in] args - Arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oamp_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oamp_status_diag_output_t oamp_status_outputs;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get requested information
     */
    SHR_IF_ERR_EXIT(diag_oamp_status_get_results(unit, &oamp_status_outputs));

    /*
     *  Display requested information
     */
    SHR_IF_ERR_EXIT(diag_oamp_status_display(unit, &oamp_status_outputs, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Form a sequence of MEP IDs in the  MEP DB pool and put
 * it on the string. The sequence will be as follows.
 *
 * If print_part_list is TRUE:
 *      [First 6 MEPs in the sequence....Last 6 MEPs in the sequence]
 * Else:
 *      [All MEPs in the sequence]
 *
 * \param [in] num_meps - Number of MEPs in that pool.
 * \param [in] list_of_meps - the MEP IDs in that pool
 * \param [in] print_part_list - Print partial list or full list
 * \param [out] mep_info_str - string with the sequence.
 *                             It is assumed that string is big enough
 *                             to hold the above sequence.
 *
 * \retval None
 */

static void
diag_oamp_mep_db_compute_mep_str_from_list(
    int num_meps,
    int *list_of_meps,
    uint8 print_part_list,
    char *mep_info_str)
{
    int num_meps_in_first_set = 6;
    int num_meps_in_second_set = 6;
    int i = 0;
    int num_chars = 0;

    if (num_meps <= 12 || print_part_list == FALSE)
    {
        /*
         * If there are less than or equal to 12 MEPs, we will print all of them in one set anyway. Or if we want to
         * print all of them.
         */
        num_meps_in_first_set = num_meps;
        num_meps_in_second_set = 0;
    }
    num_chars = 0;

    /*
     * First set of meps
     */
    for (i = 0; i < num_meps_in_first_set; i++)
    {
        /*
         * Skip comma for last one.
         */
        if (i == num_meps_in_first_set - 1)
        {
            num_chars += sal_sprintf(&mep_info_str[num_chars], "%d", list_of_meps[i]);
        }
        else
        {
            num_chars += sal_sprintf(&mep_info_str[num_chars], "%d,", list_of_meps[i]);
        }
    }

    if (num_meps_in_second_set != 0)
    {
        /*
         * Print dots.
         */
        num_chars += sal_sprintf(&mep_info_str[num_chars], "....");
        /*
         * Second set of meps
         */
        for (i = num_meps_in_second_set; i > 0; i--)
        {
            if (i == 1)
            {
                /*
                 * Comma not required for last MEP
                 */
                num_chars += sal_sprintf(&mep_info_str[num_chars], "%d", list_of_meps[num_meps - i]);
            }
            else
            {
                num_chars += sal_sprintf(&mep_info_str[num_chars], "%d,", list_of_meps[num_meps - i]);
            }
        }
    }
}

/**
 * \brief
 * Get the list of MEP IDs in the  MEP DB pool based on
 * number of meps in the pool, pool type and maid type
 *
 * \param [in] unit - relevant unit.
 * \param [in] pool_index - Index of the pool in outputs structure.
 * \param [in] is_partition - partition/allocation
 * \param [in] num_meps - Number of MEPs in that pool.
 * \param [in] pool_type - Pool type - Short, full or extra.
 * \param [in] maid_type - MA ID type - None, ICC or 2Byte
 * \param [out] list_of_meps - the MEP IDs in that pool
 * \param [out] outputs - Output structure where num_filled_meps and mep_info_str
 *                        needs to be filled.
 *
 * \retval Error indication
 */

static shr_error_e
diag_oamp_mep_db_get_list_of_meps_and_print(
    int unit,
    uint8 pool_index,
    uint8 is_partition,
    int num_meps,
    uint8 pool_type,
    uint8 maid_type,
    int *list_of_meps,
    oamp_mep_db_diag_output_t * outputs)
{
    int mep_db_threshold;
    int nof_umc;
    int oamp_max_nof_ep_id;
    int nof_mep_db_short_entries;
    int nof_short_lines;
    int mep_index = 0;
    int mep_id = 0;
    int alloced_count = 0;
    uint8 is_allocated = 0;
    uint8 in_range = TRUE;
    int first_mep_id_in_range = -1;
    int last_mep_id_in_range = -1;
    char *mep_info_str = outputs->mep_info_str[pool_index];
    int num_chars = 0;
    SHR_FUNC_INIT_VARS(unit);

    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_umc = dnx_data_oam.oamp.nof_umc_get(unit);
    oamp_max_nof_ep_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_short_lines = mep_db_threshold / nof_mep_db_short_entries;

    if (list_of_meps == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "List of meps pointer is NULL\n");
    }

    for (mep_index = 0; mep_index < num_meps; mep_index++)
    {
        /*
         * Calculate the mep id from mep index based on pool type and MAID type
         */
        switch (pool_type)
        {
            case SHORT_TYPE:
            {
                {
                    if (maid_type == ICC_TYPE)
                    {
                        /*
                         * 0, 4, 8... = idx * 4
                         */
                        mep_id = mep_index * nof_mep_db_short_entries;
                    }
                    else if (maid_type == TWO_BYTE_TYPE)
                    {
                        if (mep_index / DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE > nof_umc)
                        {
                            /*
                             * Crossed the UMC requirement. So now every entry corresponds to 4 2byte MEP IDs. Example
                             * consider threshold = 33k. Until 32k entries which translates to 24K mep indexes considering
                             * 3 indexes every 4 entries for 2B so far. At this point, every mep index will translate to 1
                             * 2B mep id. i.e Mep indexes 24K , 24K + 1, 24 K+2, ..... 25 K -1 = MEP IDs 32K, 32K + 1...
                             * 33 K -1 now simply add no_of_umc with mep_index to get the actual index.
                             */
                            mep_id = mep_index + nof_umc;
                        }
                        else
                        {
                            /*
                             * 1, 2, 3, 5, 7, 9... = ((4*idx)/3) + 1
                             */
                            mep_id =
                                ((nof_mep_db_short_entries * mep_index) / DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE) +
                                1;
                        }
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown maid type is used %d\n", maid_type);
                    }
                }
                break;
            }
            case FULL_TYPE:
            {
                {
                    if (maid_type == ICC_TYPE)
                    {
                        if (nof_short_lines < nof_umc)
                        {
                            /*
                             * Thr +4, Thr + 8, .... = threshold + (4 * idx)
                             */
                            mep_id = mep_db_threshold + (mep_index * nof_mep_db_short_entries);
                        }
                        else
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "ICC FULL should not be possible when nof_short_lines is greater than UMC\n");
                        }
                    }
                    else if (maid_type == TWO_BYTE_TYPE)
                    {
                        if (nof_short_lines < nof_umc)
                        {
                            /*
                             * 32K + 4,32K + 8,... = 32K + (4 *idx)
                             */
                            mep_id = (nof_umc * nof_mep_db_short_entries) + (mep_index * nof_mep_db_short_entries);
                        }
                        else
                        {
                            /*
                             * Thr +4, Thr + 8, .... = threshold + (4 * idx)
                             */
                            mep_id = mep_db_threshold + (mep_index * nof_mep_db_short_entries);
                        }
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown maid type is used %d\n", maid_type);
                    }
                }
                break;
            }
            case EXTRA_TYPE:
            {
                if (maid_type == NONE_TYPE)
                {
                    /*
                     * 64K, 64K+4, 64K+4... = 64K + (4*idx)
                     */
                    mep_id = oamp_max_nof_ep_id + (mep_index * nof_mep_db_short_entries);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid maid type is used for extra pool %d\n", maid_type);
                }
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown pool type is used %d\n", pool_type);
                break;
            }
        }
        if (is_partition)
        {
            /*
             * Store it in the list of meps
             */
            list_of_meps[mep_index] = mep_id;
        }
        else
        {
            is_allocated = 0;
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_is_allocated(unit, mep_id, &is_allocated));
            /*
             * Check if it is allocated before filling
             */
            if (is_allocated)
            {
                list_of_meps[alloced_count++] = mep_id;
                if (in_range == TRUE)
                {
                    /*
                     * In range. Continue without printing
                     */
                    /*
                     * Update last mep id in range to my mep_id
                     */
                    last_mep_id_in_range = mep_id;
                    if (first_mep_id_in_range == -1)
                    {
                        first_mep_id_in_range = mep_id;
                    }
                }
                else
                {
                    /*
                     * Start of the range. Update the first person.
                     */
                    first_mep_id_in_range = mep_id;
                    in_range = TRUE;
                }
            }
            else
            {
                if (in_range == TRUE)
                {
                    if (first_mep_id_in_range != -1)
                    {
                        /*
                         * Range broken. print the previous range into mep str
                         */
                        if (last_mep_id_in_range != -1)
                        {
                            num_chars += sal_sprintf(&(mep_info_str[num_chars]), "%d - %d,",
                                                     first_mep_id_in_range, last_mep_id_in_range);
                        }
                        else
                        {
                            num_chars += sal_sprintf(&(mep_info_str[num_chars]), "%d,", first_mep_id_in_range);
                        }
                    }
                    first_mep_id_in_range = -1;
                    last_mep_id_in_range = -1;
                    in_range = FALSE;
                }
            }
        }
    }

    outputs->num_meps[pool_index] = num_meps;
    if (is_partition)
    {
        outputs->num_filled_meps[pool_index] = num_meps;
        /*
         * Compute the display string from list of MEP IDs.
         */
        diag_oamp_mep_db_compute_mep_str_from_list(num_meps, list_of_meps, is_partition,
                                                   outputs->mep_info_str[pool_index]);
    }
    else
    {
        outputs->num_filled_meps[pool_index] = alloced_count;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Fill up MEP DB pool partition info based on
 * number of meps in the pool, pool type and maid type
 * \param [in] unit -     Relevant unit.
 * \param [in] is_partition - partition/allocation.
 * \param [in] pool_index - Index of the pool to be filled.
 * \param [in] num_meps - Number of MEPs in that pool.
 * \param [in] pool_type - Pool type - Short, full or extra.
 * \param [in] maid_type - MA ID type - None, ICC or 2Byte
 * \param [out] outputs - values to be displayed
 *
 * \retval Error indication
 */

static shr_error_e
diag_oamp_mep_db_fill_pool_info(
    int unit,
    uint8 is_partition,
    uint8 pool_index,
    int num_meps,
    uint8 pool_type,
    uint8 maid_type,
    oamp_mep_db_diag_output_t * outputs)
{
    int *list_of_meps = NULL;
    SHR_FUNC_INIT_VARS(unit);

    outputs->pool_type[pool_index] = pool_type;
    outputs->maid_type[pool_index] = maid_type;

    SHR_ALLOC_SET_ZERO(list_of_meps, sizeof(int) * num_meps, "OAMP MEPs list", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Get the list of MEP IDs and print them into string.
     */
    SHR_IF_ERR_EXIT(diag_oamp_mep_db_get_list_of_meps_and_print
                    (unit, pool_index, is_partition, num_meps, pool_type, maid_type, list_of_meps, outputs));

exit:
    SHR_FREE(list_of_meps);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * provide info about MEP_DB partitioning scheme
 * or allocated MEPs
 * \param [in] unit -     Relevant unit.
 * \param [in] is_partition - partition or allocation
 * \param [out] outputs - values to be displayed
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_mep_db_mep_info_fill(
    int unit,
    uint8 is_partition,
    oamp_mep_db_diag_output_t * outputs)
{
    int mep_db_threshold;
    int nof_umc;
    int oamp_max_nof_mep_id;
    int oamp_max_nof_ep_id;
    int nof_mep_db_short_entries;
    int nof_short_lines;
    uint8 num_pools;
    int num_meps = 0;

    SHR_FUNC_INIT_VARS(unit);

    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_umc = dnx_data_oam.oamp.nof_umc_get(unit);
    oamp_max_nof_mep_id = dnx_data_oam.oamp.max_nof_mep_id_get(unit);
    oamp_max_nof_ep_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_short_lines = mep_db_threshold / nof_mep_db_short_entries;

    num_pools = 0;
    /*
     * Configure the size of all EP pools.
     */
    if (nof_short_lines < nof_umc)
    {
        /*
         * ICC SHORT pool
         */
        num_meps = nof_short_lines;
        if (num_meps > 0)
        {
            SHR_IF_ERR_EXIT(diag_oamp_mep_db_fill_pool_info
                            (unit, is_partition, num_pools, num_meps, SHORT_TYPE, ICC_TYPE, outputs));
            num_pools++;
        }

        /*
         * 2B SHORT pool
         */
        num_meps = nof_short_lines * DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE;
        if (num_meps > 0)
        {
            SHR_IF_ERR_EXIT(diag_oamp_mep_db_fill_pool_info
                            (unit, is_partition, num_pools, num_meps, SHORT_TYPE, TWO_BYTE_TYPE, outputs));
            num_pools++;
        }

        /*
         * ICC Full pool
         */
        num_meps = nof_umc - nof_short_lines;
        if (num_meps > 0)
        {
            SHR_IF_ERR_EXIT(diag_oamp_mep_db_fill_pool_info
                            (unit, is_partition, num_pools, num_meps, FULL_TYPE, ICC_TYPE, outputs));
            num_pools++;
        }

        /*
         * 2B Full pool
         */
        num_meps = oamp_max_nof_ep_id / nof_mep_db_short_entries - nof_umc;
        if (num_meps > 0)
        {
            SHR_IF_ERR_EXIT(diag_oamp_mep_db_fill_pool_info
                            (unit, is_partition, num_pools, num_meps, FULL_TYPE, TWO_BYTE_TYPE, outputs));
            num_pools++;
        }
    }
    else
    {
        /*
         * ICC SHORT pool
         */
        num_meps = nof_umc;
        if (num_meps > 0)
        {
            SHR_IF_ERR_EXIT(diag_oamp_mep_db_fill_pool_info
                            (unit, is_partition, num_pools, num_meps, SHORT_TYPE, ICC_TYPE, outputs));
            num_pools++;
        }

        /*
         * 2B SHORT pool
         */
        num_meps =
            DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE * nof_umc + (mep_db_threshold -
                                                                     nof_umc * nof_mep_db_short_entries);
        if (num_meps > 0)
        {
            SHR_IF_ERR_EXIT(diag_oamp_mep_db_fill_pool_info
                            (unit, is_partition, num_pools, num_meps, SHORT_TYPE, TWO_BYTE_TYPE, outputs));
            num_pools++;
        }

        /*
         * 2B Full pool
         */
        num_meps = (oamp_max_nof_ep_id / nof_mep_db_short_entries) - nof_short_lines;
        if (num_meps > 0)
        {
            SHR_IF_ERR_EXIT(diag_oamp_mep_db_fill_pool_info
                            (unit, is_partition, num_pools, num_meps, FULL_TYPE, TWO_BYTE_TYPE, outputs));
            num_pools++;
        }
    }

    num_meps = (oamp_max_nof_mep_id - oamp_max_nof_ep_id) / nof_mep_db_short_entries;
    SHR_IF_ERR_EXIT(diag_oamp_mep_db_fill_pool_info
                    (unit, is_partition, num_pools, num_meps, EXTRA_TYPE, NONE_TYPE, outputs));
    num_pools++;

    outputs->num_pools = num_pools;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Based on inputs, provide info about MEP_DB partitioning scheme or
 * allocated MEPs
 * \param [in] unit -     Relevant unit.
 * \param [in] inputs -  'inputs' configuration
 * \param [out] outputs - values to be displayed per 'inputs' configuration
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_mep_db_results(
    int unit,
    oamp_mep_db_diag_input_t * inputs,
    oamp_mep_db_diag_output_t * outputs)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_oamp_mep_db_mep_info_fill(unit, inputs->partition, outputs));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Displays the output of OAMP MEP DB partition/allocation
 * \param [in] unit -     Relevant unit.
 * \param [in] sand_control - control structure
 * \param [in] inputs - input flags
 * \param [in] outputs - values to be displayed per 'inputs' configuration
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_mep_db_display(
    int unit,
    sh_sand_control_t * sand_control,
    oamp_mep_db_diag_input_t * inputs,
    oamp_mep_db_diag_output_t * outputs)
{
    int pool = 0;
    int total_num_meps = 0;
    int total_alloc_meps = 0;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (inputs->partition == TRUE)
    {
        /*
         * TITLE
         */
        PRT_TITLE_SET("OAMP-MEP-DB diagnotics:Partitioning scheme");
        /*
         * COLUMNS
         */
        PRT_COLUMN_ADD("Type of MEP DB entries");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MEP IDs list");
        PRT_COLUMN_ADD("MAID type usage");

        for (pool = 0; pool < outputs->num_pools; pool++)
        {
            /*
             * ROWS
             */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s MEP DB entries", POOL_TYPE_STR(outputs->pool_type[pool]));
            PRT_CELL_SET("MEPs %s", outputs->mep_info_str[pool]);
            PRT_CELL_SET("%s", MAID_TYPE_STR(outputs->maid_type[pool]));
        }
    }
    else if (inputs->allocation == TRUE)
    {
        /*
         * TITLE
         */
        PRT_TITLE_SET("OAMP-MEP-DB diagnotics:Allocated MEPs");
        /*
         * COLUMNS
         */
        PRT_COLUMN_ADD("Type of MEP DB entries");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Allocated MEP IDs list");
        PRT_COLUMN_ADD("MAID type usage");

        for (pool = 0; pool < outputs->num_pools; pool++)
        {
            /*
             * ROWS
             */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s MEP DB entries", POOL_TYPE_STR(outputs->pool_type[pool]));
            if (outputs->num_filled_meps[pool] == 0)
            {
                PRT_CELL_SET("No MEPs are allocated in this pool");
            }
            else
            {
                PRT_CELL_SET("MEPs %s are allocated", outputs->mep_info_str[pool]);
            }
            PRT_CELL_SET("%s", MAID_TYPE_ALLOC_STR(outputs->maid_type[pool]));
            total_num_meps += outputs->num_meps[pool];
            total_alloc_meps += outputs->num_filled_meps[pool];
        }
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("Total number of MEP IDs = %d", total_num_meps);
        PRT_CELL_SET("Number of allocated MEP IDs = %d", total_alloc_meps);
        PRT_CELL_SET("Number of free MEP IDs = %d", total_num_meps - total_alloc_meps);

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Both partition and allocation are not true\n");
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * OAMP_MEP_DB command - partition main function
 * Gets input and calls get_results and display functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oamp_mep_db_cmd_part(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oamp_mep_db_diag_input_t inputs = { 0 };
    oamp_mep_db_diag_output_t *outputs = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(outputs, sizeof(oamp_mep_db_diag_output_t), "OAMP MEP DB DIAG OP", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);
    inputs.partition = TRUE;
    inputs.allocation = FALSE;
    /*
     *  Get requested information
     */
    SHR_IF_ERR_EXIT(diag_oamp_mep_db_results(unit, &inputs, outputs));

    /*
     *  Display requested information
     */
    SHR_IF_ERR_EXIT(diag_oamp_mep_db_display(unit, sand_control, &inputs, outputs));

exit:
    SHR_FREE(outputs);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * OAMP_MEP_DB command - allocation main function
 * Gets input and calls get_results and display functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oamp_mep_db_cmd_alloc(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oamp_mep_db_diag_input_t inputs = { 0 };
    oamp_mep_db_diag_output_t *outputs = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(outputs, sizeof(oamp_mep_db_diag_output_t), "OAMP MEP DB DIAG OP", "%s%s%s\r\n", EMPTY, EMPTY,
                       EMPTY);
    inputs.partition = FALSE;
    inputs.allocation = TRUE;
    /*
     *  Get requested information
     */
    SHR_IF_ERR_EXIT(diag_oamp_mep_db_results(unit, &inputs, outputs));

    /*
     *  Display requested information
     */
    SHR_IF_ERR_EXIT(diag_oamp_mep_db_display(unit, sand_control, &inputs, outputs));

exit:
    SHR_FREE(outputs);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam BFD Endpoint command - Initialize statistics variables
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_bfdep_results - pointer to the retrieved
 *        parameters of the endpoint. The functions fills in the
 *        relevant fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_bfdep_init(
    int unit,
    oam_bfdep_results_t * oam_bfdep_results)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init statistics
     */
    oam_bfdep_results->no_ep = 0;
    oam_bfdep_results->no_ups = 0;
    oam_bfdep_results->no_accs = 0;
    oam_bfdep_results->no_errs = 0;
    oam_bfdep_results->no_udp_over_ipv4 = 0;
    oam_bfdep_results->no_udp_over_ipv6 = 0;
    oam_bfdep_results->no_mpls = 0;
    oam_bfdep_results->no_pwe_ctr_wrd = 0;
    oam_bfdep_results->no_pwe_ttl = 0;
    oam_bfdep_results->no_mpls_tp_cc = 0;
    oam_bfdep_results->no_mpls_tp_cc_cv = 0;
    oam_bfdep_results->no_pwe_gal = 0;
    oam_bfdep_results->no_sbfd_initiator_over_ipv4 = 0;
    oam_bfdep_results->no_sbfd_initiator_over_ipv6 = 0;
    oam_bfdep_results->no_sbfd_initiator_over_mpls = 0;
    oam_bfdep_results->no_sbfd_reflector_over_ipv4 = 0;
    oam_bfdep_results->no_sbfd_reflector_over_mpls = 0;
    oam_bfdep_results->no_pwe_rtr_alrt = 0;
    oam_bfdep_results->no_pwe_rtr_alrt_ach = 0;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam GRP command - Initialize statistics variables
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_grp_results - pointer to the retrieved
 *        parameters of the group. The functions fills in the
 *        relevant fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_grp_init(
    int unit,
    oam_grp_results_t * oam_grp_results)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init statistics
     */
    oam_grp_results->no_grp = 0;
    oam_grp_results->no_err = 0;
    oam_grp_results->no_meps = 0;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam BFD Endpoint command - display function
 * Displays the endpoint parameters
 * \param [in] unit -     Relevant unit.
 * \param [in] ep - ID of the endpoint to be displayed
 * \param [in] oam_bfdep_results - outputs of the oam bfdep command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_bfdep_display(
    int unit,
    uint32 ep,
    oam_bfdep_results_t * oam_bfdep_results,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("BFD ENDPOINT");
    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "ID");
    PRT_CELL_SET("0x%x = %d", ep, ep);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Type");
    switch (oam_bfdep_results->endpoint_info.type)
    {
        case bcmBFDTunnelTypeUdp:
        {
            char *ip_version = oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_IPV6 ? "IPv6" : "IPv4";
            {
                if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR)
                {
                    PRT_CELL_SET("%s %s", "Seamless BFD Initiator over", ip_version);
                }
                else if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR)
                {
                    PRT_CELL_SET("%s %s", "Seamless BFD Reflector over", ip_version);
                }
                else if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_MULTIHOP)
                {
                    PRT_CELL_SET("%s %s %s", "BFD over", ip_version, "MultiHOP");
                }
                else if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_MICRO_BFD)
                {
                    PRT_CELL_SET("%s %s %s", "BFD over", ip_version, "MicroBFD");
                }
                else
                {
                    PRT_CELL_SET("%s %s %s", "BFD over", ip_version, "SingleHOP");
                }
            }
            if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_IPV6)
            {
                if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR)
                {
                    oam_bfdep_results->no_sbfd_initiator_over_ipv6++;
                }
                else
                {
                    oam_bfdep_results->no_udp_over_ipv6++;
                }
            }
            else
            {
                if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR)
                {
                    oam_bfdep_results->no_sbfd_initiator_over_ipv4++;
                }
                else if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR)
                {
                    oam_bfdep_results->no_sbfd_reflector_over_ipv4++;
                }
                else
                {
                    oam_bfdep_results->no_udp_over_ipv4++;
                }
            }

            break;
        }
        case bcmBFDTunnelTypeMpls:
        {
            if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR)
            {
                PRT_CELL_SET("%s", "Seamless BFD Initiator over MPLS LSP");
                oam_bfdep_results->no_sbfd_initiator_over_mpls++;
            }
            else if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR)
            {
                PRT_CELL_SET("%s", "Seamless BFD Reflector over MPLS LSP");
                oam_bfdep_results->no_sbfd_reflector_over_mpls++;
            }
            else
            {
                PRT_CELL_SET("%s", "BFD over MPLS LSP");
                oam_bfdep_results->no_mpls++;
            }
            break;
        }
        case bcmBFDTunnelTypePweControlWord:
        {
            PRT_CELL_SET("%s", "BFD over PWE control word (VCCV Type 1)");
            oam_bfdep_results->no_pwe_ctr_wrd++;
            break;
        }
        case bcmBFDTunnelTypePweTtl:
        {
            PRT_CELL_SET("%s", "BFD over PWE ttl 1 (VCCV Type 3)");
            oam_bfdep_results->no_pwe_ttl++;
            break;
        }
        case bcmBFDTunnelTypeMplsTpCc:
        {
            PRT_CELL_SET("%s", "BFD over MPLS-TP proactive CC");
            oam_bfdep_results->no_mpls_tp_cc++;
            break;
        }
        case bcmBFDTunnelTypeMplsTpCcCv:
        {
            PRT_CELL_SET("%s", "BFD over MPLS-TP proactive CC&CV");
            oam_bfdep_results->no_mpls_tp_cc_cv++;
            break;
        }
        case bcmBFDTunnelTypePweGal:
        {
            PRT_CELL_SET("%s", "BFD over PWE With GAL");
            oam_bfdep_results->no_pwe_gal++;
            break;
        }
        case bcmBFDTunnelTypePweRouterAlert:
        {
            if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_PWE_ACH)
            {
                PRT_CELL_SET("%s", "BFD over PWE With Router Alert (VCCV Type 2) including PWE-ACH");
                oam_bfdep_results->no_pwe_rtr_alrt_ach++;
            }
            else
            {
                PRT_CELL_SET("%s", "BFD over PWE With Router Alert (VCCV Type 2)");
                oam_bfdep_results->no_pwe_rtr_alrt++;
            }
            break;
        }
        default:
        {
            PRT_CELL_SET("%s", "ERROR: Unsupported endpoint type");
            oam_bfdep_results->no_errs++;
            break;
        }
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Remote Gport");
    if (oam_bfdep_results->endpoint_info.remote_gport != BCM_GPORT_INVALID)
    {
        PRT_CELL_SET("0x%x", oam_bfdep_results->endpoint_info.remote_gport);
    }
    else
    {
        PRT_CELL_SET("%s", "Disabled");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Gport");
    if (oam_bfdep_results->endpoint_info.gport != BCM_GPORT_INVALID)
    {
        PRT_CELL_SET("0x%x", oam_bfdep_results->endpoint_info.gport);
    }
    else
    {
        PRT_CELL_SET("%s", "Disabled");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Local discriminator");
    PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.local_discr);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Accelerated BFD Session");
    if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_IN_HW)
    {
        PRT_CELL_SET("%s", "YES");
        oam_bfdep_results->no_accs++;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "TX Gport");

        PRT_CELL_SET("0x%x", oam_bfdep_results->endpoint_info.tx_gport);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Local state");
        switch (oam_bfdep_results->endpoint_info.local_state)
        {
            case bcmBFDStateAdminDown:
            {
                PRT_CELL_SET("%s", "Admin down");
                break;
            }
            case bcmBFDStateDown:
            {
                PRT_CELL_SET("%s", "down");
                break;
            }
            case bcmBFDStateUp:
            {
                PRT_CELL_SET("%s", "up");
                break;
            }
            case bcmBFDStateInit:
            {
                PRT_CELL_SET("%s", "Init");
                break;
            }
            default:
            {
                PRT_CELL_SET("%s", "INVALID LOCAL STATE");
                oam_bfdep_results->no_errs++;
                break;
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Remote state");
        switch (oam_bfdep_results->endpoint_info.remote_state)
        {
            case bcmBFDStateAdminDown:
            {
                PRT_CELL_SET("%s", "Admin down");
                break;
            }
            case bcmBFDStateDown:
            {
                PRT_CELL_SET("%s", "down");
                break;
            }
            case bcmBFDStateUp:
            {
                PRT_CELL_SET("%s", "up");
                break;
            }
            case bcmBFDStateInit:
            {
                PRT_CELL_SET("%s", "Init");
                break;
            }
            default:
            {
                PRT_CELL_SET("%s", "INVALID REMOTE STATE");
                oam_bfdep_results->no_errs++;
                break;
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Local diagnostic");
        switch (oam_bfdep_results->endpoint_info.local_diag)
        {
            case bcmBFDDiagCodeNone:
            {
                PRT_CELL_SET("%s", "No Diagnostic");
                break;
            }
            case bcmBFDDiagCodeCtrlDetectTimeExpired:
            {
                PRT_CELL_SET("%s", "Control Detection Time Expired");
                break;
            }
            case bcmBFDDiagCodeEchoFailed:
            {
                PRT_CELL_SET("%s", "Echo Function Failed");
                break;
            }
            case bcmBFDDiagCodeNeighbourSignalledDown:
            {
                PRT_CELL_SET("%s", "Neighbor Signaled Session Down");
                break;
            }
            case bcmBFDDiagCodeForwardingPlaneReset:
            {
                PRT_CELL_SET("%s", "Forwarding Plane Reset");
                break;
            }
            case bcmBFDDiagCodePathDown:
            {
                PRT_CELL_SET("%s", "Path Down");
                break;
            }
            case bcmBFDDiagCodeConcatPathDown:
            {
                PRT_CELL_SET("%s", "Concatenated Path Down");
                break;
            }
            case bcmBFDDiagCodeAdminDown:
            {
                PRT_CELL_SET("%s", "Administratively Down");
                break;
            }
            case bcmBFDDiagCodeRevConcatPathDown:
            {
                PRT_CELL_SET("%s", "Reverse Concatenated Path Down");
                break;
            }
            default:
            {
                PRT_CELL_SET("%s", "INVALID DIAG");
                oam_bfdep_results->no_errs++;
                break;
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Remote diagnostic");
        switch (oam_bfdep_results->endpoint_info.remote_diag)
        {
            case bcmBFDDiagCodeNone:
            {
                PRT_CELL_SET("%s", "No Diagnostic");
                break;
            }
            case bcmBFDDiagCodeCtrlDetectTimeExpired:
            {
                PRT_CELL_SET("%s", "Control Detection Time Expired");
                break;
            }
            case bcmBFDDiagCodeEchoFailed:
            {
                PRT_CELL_SET("%s", "Echo Function Failed");
                break;
            }
            case bcmBFDDiagCodeNeighbourSignalledDown:
            {
                PRT_CELL_SET("%s", "Neighbor Signaled Session Down");
                break;
            }
            case bcmBFDDiagCodeForwardingPlaneReset:
            {
                PRT_CELL_SET("%s", "Forwarding Plane Reset");
                break;
            }
            case bcmBFDDiagCodePathDown:
            {
                PRT_CELL_SET("%s", "Path Down");
                break;
            }
            case bcmBFDDiagCodeConcatPathDown:
            {
                PRT_CELL_SET("%s", "Concatenated Path Down");
                break;
            }
            case bcmBFDDiagCodeAdminDown:
            {
                PRT_CELL_SET("%s", "Administratively Down");
                break;
            }
            case bcmBFDDiagCodeRevConcatPathDown:
            {
                PRT_CELL_SET("%s", "Reverse Concatenated Path Down");
                break;
            }
            default:
            {
                PRT_CELL_SET("%s", "INVALID DIAG");
                oam_bfdep_results->no_errs++;
                break;
            }
        }
        if (oam_bfdep_results->endpoint_info.type == bcmBFDTunnelTypeUdp
            || oam_bfdep_results->endpoint_info.type == bcmBFDTunnelTypeMpls)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "UDP source port");
            PRT_CELL_SET("0x%x", oam_bfdep_results->endpoint_info.udp_src_port);
        }
        if (oam_bfdep_results->endpoint_info.type == bcmBFDTunnelTypeUdp)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "Destination IP address");
            if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_IPV6)
            {
                PRT_CELL_SET(IPV6_PRINT_FMT, IPV6_PRINT_ARG(oam_bfdep_results->endpoint_info.dst_ip6_addr));
            }
            else
            {
                if (check_for_endianness())
                {
                    PRT_CELL_SET(IPV4_PRINT_FMT, IPV4_PRINT_ARG_LE(oam_bfdep_results->endpoint_info.dst_ip_addr));
                }
                else
                {
                    PRT_CELL_SET(IPV4_PRINT_FMT, IPV4_PRINT_ARG_BE(oam_bfdep_results->endpoint_info.dst_ip_addr));
                }
            }
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "Source address");
            if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_IPV6)
            {
                PRT_CELL_SET(IPV6_PRINT_FMT, IPV6_PRINT_ARG(oam_bfdep_results->endpoint_info.src_ip6_addr));
            }
            else
            {
                if (check_for_endianness())
                {
                    PRT_CELL_SET(IPV4_PRINT_FMT, IPV4_PRINT_ARG_LE(oam_bfdep_results->endpoint_info.src_ip_addr));
                }
                else
                {
                    PRT_CELL_SET(IPV4_PRINT_FMT, IPV4_PRINT_ARG_BE(oam_bfdep_results->endpoint_info.src_ip_addr));
                }
            }
            if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_MULTIHOP)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET_SHIFT(1, "%s", "IP TTL");
                PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.ip_ttl);

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET_SHIFT(1, "%s", "IP TOS");
                PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.ip_tos);
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Remote Flags");
        PRT_CELL_SET("0x%x", oam_bfdep_results->endpoint_info.remote_flags);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Local Flags");
        PRT_CELL_SET("0x%x", oam_bfdep_results->endpoint_info.local_flags);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Int priority");
        PRT_CELL_SET("%d", oam_bfdep_results->endpoint_info.int_pri);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "BFD rate (ms)");
        PRT_CELL_SET("%d", oam_bfdep_results->endpoint_info.bfd_period);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Desired local min TX interval");
        PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.local_min_tx);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Required local RX interval");
        PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.local_min_rx);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Local detection multiplier");
        PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.local_detect_mult);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(1, "%s", "Remote discriminator");
        PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.remote_discr);

        if (oam_bfdep_results->endpoint_info.type != bcmBFDTunnelTypeUdp)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "MPLS label on MPLS stack");
            PRT_CELL_SKIP(1);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "label");
            PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.egress_label.label);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "exp");
            PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.egress_label.exp);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "TTL");
            PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.egress_label.ttl);
        }

        if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "Remote detection explicit timeout");
            PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.bfd_detection_time);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "Remote detection multiplier");
            PRT_CELL_SET("%u", oam_bfdep_results->endpoint_info.remote_detect_mult);
        }
    }
    else
    {
        /*
         * Not accelerated
         */
        PRT_CELL_SET("%s", "NO");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Source address");
        if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_IPV6)
        {
            PRT_CELL_SET(IPV6_PRINT_FMT, IPV6_PRINT_ARG(oam_bfdep_results->endpoint_info.src_ip6_addr));
        }
        else
        {
            if (check_for_endianness())
            {
                PRT_CELL_SET(IPV4_PRINT_FMT, IPV4_PRINT_ARG_LE(oam_bfdep_results->endpoint_info.src_ip_addr));
            }
            else
            {
                PRT_CELL_SET(IPV4_PRINT_FMT, IPV4_PRINT_ARG_BE(oam_bfdep_results->endpoint_info.src_ip_addr));
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam BFD Endpoint command - statistics function Gathers
 * statistics of the endpoint without displaying theparameters
 * \param [in] unit -     Relevant unit.
 * \param [in] ep - ID of the endpoint to be displayed
 * \param [in] oam_bfdep_results - outputs of the oam ep command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_bfdep_statistics(
    int unit,
    bcm_oam_endpoint_t ep,
    oam_bfdep_results_t * oam_bfdep_results)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (oam_bfdep_results->endpoint_info.type)
    {
        case bcmBFDTunnelTypeUdp:
        {
            if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_IPV6)
            {
                if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR)
                {
                    oam_bfdep_results->no_sbfd_initiator_over_ipv6++;
                }
                else
                {
                    oam_bfdep_results->no_udp_over_ipv6++;
                }
            }
            else
            {
                if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR)
                {
                    oam_bfdep_results->no_sbfd_initiator_over_ipv4++;
                }
                else if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR)
                {
                    oam_bfdep_results->no_sbfd_reflector_over_ipv4++;
                }
                else
                {
                    oam_bfdep_results->no_udp_over_ipv4++;
                }
            }
            break;
        }
        case bcmBFDTunnelTypeMpls:
        {
            if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR)
            {
                oam_bfdep_results->no_sbfd_initiator_over_mpls++;
            }
            else if (oam_bfdep_results->endpoint_info.flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR)
            {
                oam_bfdep_results->no_sbfd_reflector_over_mpls++;
            }
            else
            {
                oam_bfdep_results->no_mpls++;
            }
            break;
        }
        case bcmBFDTunnelTypePweControlWord:
        {
            oam_bfdep_results->no_pwe_ctr_wrd++;
            break;
        }
        case bcmBFDTunnelTypePweTtl:
        {
            oam_bfdep_results->no_pwe_ttl++;
            break;
        }
        case bcmBFDTunnelTypeMplsTpCc:
        {
            oam_bfdep_results->no_mpls_tp_cc++;
            break;
        }
        case bcmBFDTunnelTypeMplsTpCcCv:
        {
            oam_bfdep_results->no_mpls_tp_cc_cv++;
            break;
        }
        case bcmBFDTunnelTypePweGal:
        {
            oam_bfdep_results->no_pwe_gal++;
            break;
        }
        case bcmBFDTunnelTypePweRouterAlert:
        {
            if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_PWE_ACH)
            {
                oam_bfdep_results->no_pwe_rtr_alrt_ach++;
            }
            else
            {
                oam_bfdep_results->no_pwe_rtr_alrt++;
            }
            break;
        }
        default:
        {
            oam_bfdep_results->no_errs++;
            break;
        }
    }
    if (oam_bfdep_results->endpoint_info.flags & BCM_BFD_ENDPOINT_IN_HW)
    {
        oam_bfdep_results->no_accs++;

        switch (oam_bfdep_results->endpoint_info.local_state)
        {
            case bcmBFDStateAdminDown:
            case bcmBFDStateDown:
            case bcmBFDStateUp:
            case bcmBFDStateInit:
            {
                break;
            }
            default:
            {
                oam_bfdep_results->no_errs++;
                break;
            }
        }
        switch (oam_bfdep_results->endpoint_info.remote_state)
        {
            case bcmBFDStateAdminDown:
            case bcmBFDStateDown:
            case bcmBFDStateUp:
            case bcmBFDStateInit:
            {
                break;
            }
            default:
            {
                oam_bfdep_results->no_errs++;
                break;
            }
        }

        switch (oam_bfdep_results->endpoint_info.local_diag)
        {
            case bcmBFDDiagCodeNone:
            case bcmBFDDiagCodeCtrlDetectTimeExpired:
            case bcmBFDDiagCodeEchoFailed:
            case bcmBFDDiagCodeNeighbourSignalledDown:
            case bcmBFDDiagCodeForwardingPlaneReset:
            case bcmBFDDiagCodePathDown:
            case bcmBFDDiagCodeConcatPathDown:
            case bcmBFDDiagCodeAdminDown:
            case bcmBFDDiagCodeRevConcatPathDown:
            {
                break;
            }
            default:
            {
                oam_bfdep_results->no_errs++;
                break;
            }
        }
        switch (oam_bfdep_results->endpoint_info.remote_diag)
        {
            case bcmBFDDiagCodeNone:
            case bcmBFDDiagCodeCtrlDetectTimeExpired:
            case bcmBFDDiagCodeEchoFailed:
            case bcmBFDDiagCodeNeighbourSignalledDown:
            case bcmBFDDiagCodeForwardingPlaneReset:
            case bcmBFDDiagCodePathDown:
            case bcmBFDDiagCodeConcatPathDown:
            case bcmBFDDiagCodeAdminDown:
            case bcmBFDDiagCodeRevConcatPathDown:
            {
                break;
            }
            default:
            {
                oam_bfdep_results->no_errs++;
                break;
            }
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam BFD Endpoint command - Statistics display function
 * Displays the statistics of the endpoints
 * \param [in] unit -     Relevant unit.
 * \param [in] oam_bfdep_results - outputs of the oam ep command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_bfdep_statistics_display(
    int unit,
    oam_bfdep_results_t * oam_bfdep_results,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("ENDPOINT STATISTICS");
    PRT_COLUMN_ADD("Direction");
    PRT_COLUMN_ADD("Count");
    PRT_COLUMN_ADD("Acceleration");
    PRT_COLUMN_ADD("Count");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Count");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "DOWNMEPs");
    PRT_CELL_SET("%d", (oam_bfdep_results->no_ep - oam_bfdep_results->no_ups));

    PRT_CELL_SET("%s", "Accelerated");
    PRT_CELL_SET("%d", oam_bfdep_results->no_accs);

    PRT_CELL_SET("%s", "BFD over IPV4");
    PRT_CELL_SET("%d", oam_bfdep_results->no_udp_over_ipv4);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "UPMEPs");
    PRT_CELL_SET("%d", oam_bfdep_results->no_ups);

    PRT_CELL_SET("%s", "Non-Accelerated");
    PRT_CELL_SET("%d", oam_bfdep_results->no_ep - oam_bfdep_results->no_accs);

    PRT_CELL_SET("%s", "BFD over IPV6");
    PRT_CELL_SET("%d", oam_bfdep_results->no_udp_over_ipv6);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "SBFD initiator over IPV4");
    PRT_CELL_SET("%d", oam_bfdep_results->no_sbfd_initiator_over_ipv4);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "SBFD initiator over IPV6");
    PRT_CELL_SET("%d", oam_bfdep_results->no_sbfd_initiator_over_ipv6);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "SBFD reflector over IPV4");
    PRT_CELL_SET("%d", oam_bfdep_results->no_sbfd_reflector_over_ipv4);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "BFD over MPLS LSP");
    PRT_CELL_SET("%d", oam_bfdep_results->no_mpls);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "SBFD initiator over MPLS LSP");
    PRT_CELL_SET("%d", oam_bfdep_results->no_sbfd_initiator_over_mpls);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "SBFD reflector over MPLS LSP");
    PRT_CELL_SET("%d", oam_bfdep_results->no_sbfd_reflector_over_mpls);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "BFD over PWE control word (VCCV Type 1)");
    PRT_CELL_SET("%d", oam_bfdep_results->no_pwe_ctr_wrd);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "BFD over PWE ttl 1 (VCCV Type 3)");
    PRT_CELL_SET("%d", oam_bfdep_results->no_pwe_ttl);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "BFD over MPLS-TP proactive CC");
    PRT_CELL_SET("%d", oam_bfdep_results->no_mpls_tp_cc);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "BFD over MPLS-TP proactive CC&CV");
    PRT_CELL_SET("%d", oam_bfdep_results->no_mpls_tp_cc_cv);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "BFD over PWE With GAL");
    PRT_CELL_SET("%d", oam_bfdep_results->no_pwe_gal);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "BFD over PWE With Router Alert (VCCV Type 2)");
    PRT_CELL_SET("%d", oam_bfdep_results->no_pwe_rtr_alrt);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SKIP(4);

    PRT_CELL_SET("%s", "BFD over PWE With Router Alert (VCCV Type 2) including PWE-ACH");
    PRT_CELL_SET("%d", oam_bfdep_results->no_pwe_rtr_alrt_ach);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%s", "Total Endpoints");
    PRT_CELL_SET("%d", oam_bfdep_results->no_ep);

    PRT_CELL_SKIP(1);
    PRT_CELL_SET("%d", oam_bfdep_results->no_ep);

    PRT_CELL_SKIP(1);
    PRT_CELL_SET("%d", oam_bfdep_results->no_ep);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%s", "Errors");
    PRT_CELL_SET("%d", oam_bfdep_results->no_errs);
    PRT_CELL_SKIP(4);

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam GRP command - Statistics display function Displays the
 * statistics of the groups
 * \param [in] unit -     Relevant unit.
 * \param [in] oam_grp_results - outputs of the oam grp command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_grp_statistics_display(
    int unit,
    oam_grp_results_t * oam_grp_results,
    sh_sand_control_t * sand_control)
{

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("GROUP STATISTICS");
    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD("Value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Number of Groups");
    PRT_CELL_SET("%d", oam_grp_results->no_grp);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Number of Errors");
    PRT_CELL_SET("%d", oam_grp_results->no_err);

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam BFD Endpoint command - Gets Endpoint parameters
 * function Accesses to HW and prepares parameters for display
 * \param [in] unit -     Relevant unit.
 * \param [in] ep - ID of the requested endpoint
 * \param [in] *oam_bfdep_results - pointer to the retrieved
 *        parameters of the endpoint. The functions fills in the
 *        relevant fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_bfdep_get_results(
    int unit,
    uint32 ep,
    oam_bfdep_results_t * oam_bfdep_results)
{
    SHR_FUNC_INIT_VARS(unit);

    bcm_bfd_endpoint_info_t_init(&oam_bfdep_results->endpoint_info);

    /*
     * Get endpoint_info
     */
    SHR_IF_ERR_EXIT(bcm_dnx_bfd_endpoint_get(unit, ep, &(oam_bfdep_results->endpoint_info)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam BFD EnpPoint command - display properties of a single
 * endpoint.
 * \param [in] unit - Relevant unit.
 * \param [in] display_on - When set the endpoint parameters
 *        will be displayed, other wise only statistics will be
 *        gathered.
 * \param [in] id -   Endpoint ID
 * \param [in] oam_bfdep_results - outputs of the oam bfdep command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_bfdep_display_single_endpoint(
    int unit,
    int display_on,
    uint32 id,
    oam_bfdep_results_t * oam_bfdep_results,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get Endpoint Parameters
     */
    SHR_IF_ERR_EXIT(diag_oam_bfdep_get_results(unit, id, oam_bfdep_results));
    if (display_on)
    {
        /*
         *  Display Endpoint parameters
         */
        SHR_IF_ERR_EXIT(diag_oam_bfdep_display(unit, id, oam_bfdep_results, sand_control));
    }
    else
    {
        /*
         *  Gather statistics
         */
        SHR_IF_ERR_EXIT(diag_oam_bfdep_statistics(unit, id, oam_bfdep_results));
    }
    oam_bfdep_results->no_ep++;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam BFD EnpPoint command - traverse all endpoints and display
 * their properties one by one.
 * \param [in] unit -     Relevant unit.
 * \param [in] id -   Endpoint ID
 * \param [in] oam_bfdep_results - outputs of the oam bfdep
 *        command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_bfdep_display_all(
    int unit,
    int32 id,
    oam_bfdep_results_t * oam_bfdep_results,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id_ep;
    int is_ep_end;
    int display;
    uint32 ep;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

       /** Get handle to table   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_ENDPOINT_SW_INFO, &entry_handle_id_ep));

      /** Create iterator   */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id_ep, DBAL_ITER_MODE_ALL));

      /** Read first entry   */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ep, &is_ep_end));
    while (!is_ep_end)
    {
          /** Get endpoint ID from the entry key  */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id_ep, DBAL_FIELD_BFD_ENDPOINT_ID, &ep));

        /*
         * If specific endpoint was requested display it else display all endpoints
         */
        display = DISPLAY_OFF;
        if ((ep == id) || (id == -1))
        {
            display = DISPLAY_ON;
        }
        /*
         * Display Parameters of the Endpoint
         */
        SHR_IF_ERR_EXIT(diag_oam_bfdep_display_single_endpoint(unit, display, ep, oam_bfdep_results, sand_control));

          /** Read next entry   */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ep, &is_ep_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam GRP command - display single group. Called either from
 * grp_traverse as call-back, or from grp_display_all if display
 * of a specific group was requested.
 * \param [in] unit -     Relevant unit.
 * \param [in] group_info -   Group propertied structure
 * \param [in] user_data - outputs of the oam grp command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_grp_display_single_group(
    int unit,
    bcm_oam_group_info_t * group_info,
    void *user_data)
{
    oam_grp_results_t *oam_grp_results;
    uint8 temp_group_name[BCM_OAM_GROUP_NAME_LENGTH];
    sw_state_ll_node_t node;
    bcm_oam_group_t mep_id = 0;
    sh_sand_control_t *sand_control;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    oam_grp_results = (oam_grp_results_t *) user_data;
    oam_grp_results->no_grp++;
    sand_control = oam_grp_results->sand_control;

    /*
     * Print table header
     */
    PRT_TITLE_SET("OAM GROUP");

    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "ID");
    PRT_CELL_SET("0x%x = %d", group_info->id, group_info->id);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Group Name");
    PRT_CELL_SET(MAID_PRINT_FMT, MAID_PRINT_ARG(group_info->name));

    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Type");
        PRT_CELL_SET("%s", "48 Byte MAID");
    }
    else
    {
        /** Not 48B MAID */
        switch (group_info->name[0])
        {       /* MD Format */
            case DNX_OAMP_OAM_GROUP_MD_FORMAT:
                /*
                 * Two formats supported: short MA name: {1,3,2,{two byte name},0,0,...,0} and long ICC based:
                 * {1,32,13,{ICC data-6bytes},{UMC data - 5 bytes},{two byte name},0,0...0}
                 */
                switch (group_info->name[1])
                {
                    case DNX_OAMP_OAM_GROUP_MA_NAME_FORMAT_SHORT:      /* MA_NAME_FORMAT SHORT */
                        if (group_info->name[2] == 2)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", "Type");
                            PRT_CELL_SET("%s", "Short MA name");
                        }
                        else
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", "Type");
                            PRT_CELL_SET("%s", "ERROR in Group Name");
                            oam_grp_results->no_err++;
                        }
                        break;
                    case DNX_OAMP_OAM_GROUP_MA_NAME_FORMAT_ICC:        /* MA_NAME_FORMAT ICC_BASED */
                        if (group_info->name[2] == 13)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", "Type");
                            PRT_CELL_SET("%s", "ICC based MA name");
                        }
                        else
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", "Type");
                            PRT_CELL_SET("%s", "ERROR in Group Name");
                            oam_grp_results->no_err++;
                        }
                        break;
                    default:
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Type");
                        PRT_CELL_SET("%s", "Unsupported in Group Name");
                        oam_grp_results->no_err++;
                        break;
                }

                break;
            case DNX_OAM_GROUP_NON_ACCELERATED_MD_FORMAT:
                sal_memset(temp_group_name, 0, BCM_OAM_GROUP_NAME_LENGTH);
                if (sal_memcmp(group_info->name, temp_group_name, sizeof(group_info->name)) != 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Type");
                    PRT_CELL_SET("%s", "Unsupported in Group Name");
                    oam_grp_results->no_err++;

                }
                else
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Type");
                    PRT_CELL_SET("%s", "Non Accelerated");
                }
                break;
            default:
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", "Type");
                PRT_CELL_SET("%s", "Unsupported in Group Name");
                oam_grp_results->no_err++;
                break;
        }
    }

    sal_memset(temp_group_name, 0, BCM_OAM_GROUP_NAME_LENGTH);
    if (sal_memcmp(group_info->rx_name, temp_group_name, sizeof(group_info->name)) != 0)
    {

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Remote Group Name");
        PRT_CELL_SET(MAID_PRINT_FMT, MAID_PRINT_ARG(group_info->rx_name));
    }

    /*
     * Display MEPs that are registered on the group
     */
    oam_grp_results->no_meps = 0;

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "MEPs List");
    PRT_CELL_SKIP(1);

    /** Get the first element of Oam group linked list*/
    SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.
                    oam_group_array_of_linked_lists.get_first(unit, group_info->id, &node));

    /** While there are elements in the Linked list */
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        /** Get mep_id based on Node and Group */
        SHR_IF_ERR_EXIT(oam_sw_db_info.oam_group_sw_db_info.
                        oam_group_array_of_linked_lists.node_value(unit, node, &mep_id));

        /** Get next Node from the linked list*/
        oam_sw_db_info.oam_group_sw_db_info.oam_group_array_of_linked_lists.next_node(unit, group_info->id, node,
                                                                                      &node);

        /** Count number of MEPs */
        oam_grp_results->no_meps++;

        /*
         * Display first 20 MEPs' IDs
         */
        if (oam_grp_results->no_meps <= 21)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SKIP(1);
            PRT_CELL_SET("0x%x = %d", mep_id, mep_id);
        }
    }

    if (oam_grp_results->no_meps > 21)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("Additional %d MEPs exist", oam_grp_results->no_meps - 21);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Number of MEPs");
    PRT_CELL_SET("%d", oam_grp_results->no_meps);

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam GRP command - traverse all groups and display
 * their properties one by one.
 * \param [in] unit -     Relevant unit.
 * \param [in] id -   Group ID
 * \param [in] oam_grp_results - outputs of the oam grpep
 *        command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_grp_display_all(
    int unit,
    int32 id,
    oam_grp_results_t * oam_grp_results,
    sh_sand_control_t * sand_control)
{
    bcm_oam_group_info_t grp_info;

    SHR_FUNC_INIT_VARS(unit);

    if (id == -1)
    {
        SHR_IF_ERR_EXIT(bcm_oam_group_traverse(unit, diag_oam_grp_display_single_group, oam_grp_results));
    }
    else
    {
        bcm_oam_group_info_t_init(&grp_info);

        SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, id, &grp_info));

        SHR_IF_ERR_EXIT(diag_oam_grp_display_single_group(unit, &grp_info, oam_grp_results));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam (oam and bfd) EndPoint command - Input verification
 * function.
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_ep_inputs - pointer to inputs to oam cf
 *        command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_ep_verify(
    int unit,
    oam_ep_inputs_t * oam_ep_inputs)
{
    int num_of_cores;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Verify core_id
     *  Get number of cores for this device
     */
    num_of_cores = dnx_data_device.general.nof_cores_get(unit);
    if (oam_ep_inputs->core_id > num_of_cores)
    {
        /*
         * Default core_id
         */
        oam_ep_inputs->core_id = 0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam Counters command - Input verification function.
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_cntrs_inputs - pointer to inputs to oam
 *        counters command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cntrs_verify(
    int unit,
    oam_cntrs_inputs_t * oam_cntrs_inputs)
{
    int num_of_cores;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Verify core_id
     *  Get number of cores for this device
     */
    num_of_cores = dnx_data_device.general.nof_cores_get(unit);
    if ((oam_cntrs_inputs->core_id < 0) || (oam_cntrs_inputs->core_id > num_of_cores))
    {
        /*
         * Default core_id
         */
        oam_cntrs_inputs->core_id = 0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam GRouP command - main function
 * Gets input and calls verify, get_results and display
 * functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_grp_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oam_ep_inputs_t oam_ep_inputs;
    oam_grp_results_t oam_grp_results;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_INT32("ID", oam_ep_inputs.ep_id);
    SH_SAND_GET_INT32("core", oam_ep_inputs.core_id);

    /*
     *  Verify inputs
     */
    SHR_INVOKE_VERIFY_DNX(diag_oam_ep_verify(unit, &oam_ep_inputs));

    SHR_IF_ERR_EXIT(diag_oam_grp_init(unit, &oam_grp_results));

    oam_grp_results.sand_control = sand_control;
    /*
     * Display all requested endpoints and gather statistics information
     */
     /* coverity[stack_use_overflow:FALSE]  */
    SHR_IF_ERR_EXIT(diag_oam_grp_display_all(unit, oam_ep_inputs.ep_id, &oam_grp_results, sand_control));

    /*
     *  Display Endpoint Statistics
     */
    SHR_IF_ERR_EXIT(diag_oam_grp_statistics_display(unit, &oam_grp_results, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam BFDEndPoint command - main function
 * Gets input and calls verify, get_results and dosplay
 * functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oam_bfdep_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oam_ep_inputs_t oam_ep_inputs;
    oam_bfdep_results_t oam_bfdep_results;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_INT32("ID", oam_ep_inputs.ep_id);
    SH_SAND_GET_INT32("core", oam_ep_inputs.core_id);

    /*
     *  Verify inputs
     */
    SHR_INVOKE_VERIFY_DNX(diag_oam_ep_verify(unit, &oam_ep_inputs));

    SHR_IF_ERR_EXIT(diag_oam_bfdep_init(unit, &oam_bfdep_results));

    /*
     * Display all requested endpoints and gather statistics information
     */
    SHR_IF_ERR_EXIT(diag_oam_bfdep_display_all(unit, oam_ep_inputs.ep_id, &oam_bfdep_results, sand_control));

    /*
     *  Display Endpoint Statistics
     */
    SHR_IF_ERR_EXIT(diag_oam_bfdep_statistics_display(unit, &oam_bfdep_results, sand_control));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 * oam Endpoint command - Gets Endpoint parameters
 * function Accesses to HW and prepares parameters for display
 * \param [in] unit -     Relevant unit.
 * \param [in] ep - ID of the requested endpoint
 * \param [in] *oam_ep_results - pointer to the retrieved
 *        parameters of the endpoint. The functions fills in the
 *        relevant fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_ep_get_results(
    int unit,
    uint32 ep,
    oam_ep_results_t * oam_ep_results)
{
    SHR_FUNC_INIT_VARS(unit);

    bcm_oam_endpoint_info_t_init(&(oam_ep_results->endpoint_info));
    /*
     * Get endpoint_info
     */
    SHR_IF_ERR_EXIT(bcm_oam_endpoint_get(unit, ep, &(oam_ep_results->endpoint_info)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam Endpoint command - Initialize statistics variables
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_ep_results - pointer to the retrieved
 *        parameters of the endpoint. The functions fills in the
 *        relevant fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_ep_init(
    int unit,
    oam_ep_results_t * oam_ep_results)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init statistics
     */
    oam_ep_results->no_ep = 0;
    oam_ep_results->no_mips = 0;
    oam_ep_results->no_ups = 0;
    oam_ep_results->no_accs = 0;
    oam_ep_results->no_errs = 0;
    oam_ep_results->no_rmeps = 0;
    oam_ep_results->no_eth = 0;
    oam_ep_results->no_bhh_mpls = 0;
    oam_ep_results->no_bhh_pwe = 0;
    oam_ep_results->no_bhh_pwe_gal = 0;
    oam_ep_results->no_mpls = 0;
    oam_ep_results->no_bhh_sec = 0;
    rmep_index = 0;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam Counters command - Get Counter Database information
 * Database information will be used to calculate the engine_id
 * and the memory address of a specific counter
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] core -     Relevant core.
 * \param [in] *oam_cntrs_results - pointer to the retrieved
 *        database information. The functions fills in the
 *        relevant fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cntrs_get_database_info(
    int unit,
    int core,
    oam_cntrs_results_t * oam_cntrs_results)
{
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t config;
    int database_idx;
    int engine_no;
    int flags;
    int is_oam;
    int engine_id;
    int nof_databases;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    flags = 0;

    /*
     * Check all databases
     */
    nof_databases = dnx_data_crps.engine.nof_databases_get(unit);
    for (database_idx = 0; database_idx < nof_databases; database_idx++)
    {
        database.database_id = database_idx;
        database.core_id = core;
        oam_cntrs_results->database[database_idx].is_oam = 0;

        SHR_IF_ERR_EXIT(dnx_crps_db.database_res.is_allocated(unit, core, database.database_id, &is_allocated));

        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, flags, &database, &config));
            oam_cntrs_results->database[database_idx].is_oam_ing =
                config.source == bcmStatCounterInterfaceIngressOam ? 1 : 0;
            oam_cntrs_results->database[database_idx].is_oam_eg =
                config.source == bcmStatCounterInterfaceEgressOam ? 1 : 0;
            oam_cntrs_results->database[database_idx].is_oam_oamp =
                config.source == bcmStatCounterInterfaceOamp ? 1 : 0;
            is_oam = 0;
            {
                if (oam_cntrs_results->database[database_idx].is_oam_ing)
                {
                    /*
                     * counter_if 0-2 in ingress are mapped to command_id 7-9
                     */
                    if ((config.command_id >= 7) && (config.command_id <= 9))
                    {
                        oam_cntrs_results->database[database_idx].cntr_if = config.command_id - 7;
                        is_oam = 1;
                    }
                }
                else if (oam_cntrs_results->database[database_idx].is_oam_eg)
                {
                    /*
                     * counter_if 0-2 in egress are mapped to command_id 0-2
                     */
                    if ((config.command_id >= 0) && (config.command_id <= 2))
                    {
                        oam_cntrs_results->database[database_idx].cntr_if = config.command_id;
                        is_oam = 1;
                    }
                }
                else if ((oam_cntrs_results->database[database_idx].is_oam_oamp) && (config.command_id == 0))
                {
                    /*
                     * counter_if 0-2 in ingress are mapped to command_id 7-9
                     */
                    oam_cntrs_results->database[database_idx].cntr_if = 0;
                    is_oam = 1;
                }
                else
                {
                    /*
                     * Not OAM
                     */
                    is_oam = 0;
                }
            }
            if (is_oam)
            {
                /*
                 * Get first counter_id of the database
                 */
                oam_cntrs_results->database[database_idx].is_oam = 1;
                oam_cntrs_results->database[database_idx].start_counter = config.type_config[0].start;
                oam_cntrs_results->database[database_idx].end_counter = config.type_config[0].end;
                /*
                 * Get First engine_id
                 */
                SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core, database_idx, &engine_id));
                if (engine_id == DNX_CRPS_MGMT_ENGINE_INVALID)
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "ERROR: No engines attached to the OAM database.\n");
                }
                /*
                 * Get list of engines that are attached to the database
                 */
                engine_no = 0;
                while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
                {
                    oam_cntrs_results->database[database_idx].engine[engine_no] = engine_id;
                    engine_no++;
                    SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.next_engine_id.get(unit, core, engine_id, &engine_id));
                }
                oam_cntrs_results->database[database_idx].nof_engines = engine_no;
            }
        }
        else
        {
            /** Database is not allocated */
            oam_cntrs_results->database[database_idx].is_oam_ing = 0;
            oam_cntrs_results->database[database_idx].is_oam_eg = 0;
            oam_cntrs_results->database[database_idx].is_oam_oamp = 0;
            oam_cntrs_results->database[database_idx].cntr_if = 3;
            oam_cntrs_results->database[database_idx].nof_engines = 0;
            oam_cntrs_results->database[database_idx].start_counter = config.type_config[0].start = 0;
            oam_cntrs_results->database[database_idx].start_counter = config.type_config[0].end = 0;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam Endpoint command - Callback function for rmep traverse
 * \param [in] unit -     Relevant unit.
 * \param [in] ep - ID of the endpoint
 * \param [in] rmep - ID of the rmep
 *
 * \retval Error indication
 */
shr_error_e
dnx_oam_get_rmep(
    int unit,
    bcm_oam_endpoint_t ep,
    bcm_oam_endpoint_t rmep)
{
    SHR_FUNC_INIT_VARS(unit);

    if (rmep_index < MAX_RMEPS_TO_DISPLAY)
    {
        rmep_list[rmep_index] = rmep;
    }
    rmep_index++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam Endpoint command - display function
 * Displays the endpoint parameters
 * \param [in] unit -     Relevant unit.
 * \param [in] ep - ID of the endpoint to be displayed
 * \param [in] oam_ep_results - outputs of the oam ep command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_ep_display(
    int unit,
    uint32 ep,
    oam_ep_results_t * oam_ep_results,
    sh_sand_control_t * sand_control)
{
    char display_str[100];
    int is_up;
    int is_mip;
    int is_acc;
    int is_rfc_6374;
    int rmep_cnt;
    int rmep_no;
    int loc;
    int rdi;
    int name;
    uint32 endpoint_id;
    bcm_oam_endpoint_info_t rmep_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("OAM ENDPOINT");
    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "ID");
    PRT_CELL_SET("0x%x = %d", ep, ep);

    is_up = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(&oam_ep_results->endpoint_info);
    is_mip = DNX_OAM_DISSECT_IS_ENDPOINT_MIP(&oam_ep_results->endpoint_info);
    is_rfc_6374 = DNX_OAM_IS_ENDPOINT_RFC6374(&oam_ep_results->endpoint_info);

    /** Not relevant for MplsLmDm */
    if (!is_rfc_6374)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Group");
        PRT_CELL_SET("0x%x = %d", oam_ep_results->endpoint_info.group, oam_ep_results->endpoint_info.group);

        if (is_mip)
        {
            sal_sprintf(display_str, "MIP");
            oam_ep_results->no_mips++;
        }
        else
        {
            if (is_up)
            {
                sal_sprintf(display_str, "UPMEP");
                oam_ep_results->no_ups++;
            }
            else
            {
                sal_sprintf(display_str, "DOWNMEP");
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Direction");
        PRT_CELL_SET("%s", display_str);
    }

    switch (oam_ep_results->endpoint_info.type)
    {
        case bcmOAMEndpointTypeEthernet:
        {
            sal_sprintf(display_str, "OAM over Ethernet");
            oam_ep_results->no_eth++;
            break;
        }
        case bcmOAMEndpointTypeBHHMPLS:
        {
            sal_sprintf(display_str, "BHH MPLS");
            oam_ep_results->no_bhh_mpls++;
            break;
        }
        case bcmOAMEndpointTypeBHHPwe:
        {
            sal_sprintf(display_str, "BHH - PWE");
            oam_ep_results->no_bhh_pwe++;
            break;
        }
        case bcmOAMEndpointTypeBHHPweGAL:
        {
            sal_sprintf(display_str, "BHH - PWEoGAL");
            oam_ep_results->no_bhh_pwe_gal++;
            break;
        }
        case bcmOAMEndpointTypeMPLSNetwork:
        {
            sal_sprintf(display_str, "MPLSNetwork");
            oam_ep_results->no_mpls++;
            break;
        }
        case bcmOAMEndpointTypeBhhSection:
        {
            sal_sprintf(display_str, "BHH - Section");
            oam_ep_results->no_bhh_sec++;
            break;
        }
        case bcmOAMEndpointTypeMplsLmDmLsp:
        {
            sal_sprintf(display_str, "RFC 6374 LM DM - LSP");
            oam_ep_results->no_bhh_mpls++;
            break;
        }
        case bcmOAMEndpointTypeMplsLmDmPw:
        {
            sal_sprintf(display_str, "RFC 6374 LM DM - PWE");
            oam_ep_results->no_bhh_pwe++;
            break;
        }
        case bcmOAMEndpointTypeMplsLmDmSection:
        {
            sal_sprintf(display_str, "RFC 6374 LM DM - Section");
            oam_ep_results->no_bhh_sec++;
            break;
        }
        default:
        {
            sal_sprintf(display_str, "ERROR: Unsupported endpoint type");
            oam_ep_results->no_errs++;
            break;
        }
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Type");
    PRT_CELL_SET("%s", display_str);

    if (!is_rfc_6374)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Level");
        PRT_CELL_SET("%d", oam_ep_results->endpoint_info.level);
    }
    else
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Timestamp format");
        if (oam_ep_results->endpoint_info.timestamp_format)
        {
            PRT_CELL_SET("%s", "bcmOAMTimestampFormatIEEE1588v1");
        }
        else
        {
            PRT_CELL_SET("%s", "bcmOAMTimestampFormatNTP");
        }
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Counter Interface");
    PRT_CELL_SET("%d", oam_ep_results->endpoint_info.lm_counter_if);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Counter Base");
    PRT_CELL_SET("0x%x = %d", oam_ep_results->endpoint_info.lm_counter_base_id,
                 oam_ep_results->endpoint_info.lm_counter_base_id);

    if (oam_ep_results->endpoint_info.lm_flags & BCM_OAM_LM_PCP)
    {
        sal_sprintf(display_str, "ON");
    }
    else
    {
        sal_sprintf(display_str, "OFF");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "LM - PCP");
    PRT_CELL_SET("%s", display_str);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Gport");
    PRT_CELL_SET("0x%x", oam_ep_results->endpoint_info.gport);

    if (oam_ep_results->endpoint_info.mpls_out_gport != BCM_GPORT_INVALID)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "MPLS out gport");
        PRT_CELL_SET("0x%x", oam_ep_results->endpoint_info.mpls_out_gport);
    }

    is_acc = DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(&oam_ep_results->endpoint_info);
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Accelerated in OAMP");
    if (is_acc)
    {
        oam_ep_results->no_accs++;
        PRT_CELL_SET("%s", "YES");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        if (is_rfc_6374)
        {
            PRT_CELL_SET("%s", "Session ID");
            PRT_CELL_SET("%d", oam_ep_results->endpoint_info.session_id);
        }
        else
        {
            PRT_CELL_SET("%s", "Name");
            PRT_CELL_SET("%d", oam_ep_results->endpoint_info.name);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Memory Type");
        switch (oam_ep_results->endpoint_info.endpoint_memory_type)
        {
            case bcmOamEndpointMemoryTypeSelfContained:
                PRT_CELL_SET("%s", "Self Contained");
                break;
            case bcmOamEndpointMemoryTypeLmDmOffloadedEntry:
                PRT_CELL_SET("%s", "Offloaded");
                break;
            case bcmOamEndpointMemoryTypeShortEntry:
                PRT_CELL_SET("%s", "Short");
                break;
            default:
                PRT_CELL_SET("%s", "ERROR: Unsupported memory type");
                oam_ep_results->no_errs++;
                break;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "Tx Gport");
        PRT_CELL_SET("0x%x", oam_ep_results->endpoint_info.tx_gport);

        if (!is_rfc_6374)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "Source MAC");
            PRT_CELL_SET(MAC_PRINT_FMT, MAC_PRINT_ARG(oam_ep_results->endpoint_info.src_mac_address));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "CCM Period");
            PRT_CELL_SET("%d", oam_ep_results->endpoint_info.ccm_period);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "COS of outgoing CCMs");
            PRT_CELL_SET("%d", oam_ep_results->endpoint_info.int_pri);

            if (oam_ep_results->endpoint_info.flags & BCM_OAM_ENDPOINT_RDI_TX)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", "Update and transmit RDI");
                PRT_CELL_SKIP(1);
            }
            if (oam_ep_results->endpoint_info.vlan)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", "Outer Vlan");
                PRT_CELL_SET("%d", oam_ep_results->endpoint_info.vlan);

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", "OuterPacket Priority");
                PRT_CELL_SET("%d", oam_ep_results->endpoint_info.pkt_pri);

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", "Outer TPID");
                PRT_CELL_SET("0x%x", oam_ep_results->endpoint_info.outer_tpid);

                if (oam_ep_results->endpoint_info.inner_vlan)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Inner Vlan");
                    PRT_CELL_SET("0x%x", oam_ep_results->endpoint_info.outer_tpid);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Inner Packet Priority");
                    PRT_CELL_SET("%d", oam_ep_results->endpoint_info.inner_pkt_pri);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Inner TPID");
                    PRT_CELL_SET("0x%x", oam_ep_results->endpoint_info.inner_tpid);
                }
            }
            /*
             * Print the Remote Meps associated to the Mep
 */
            rmep_index = 0;
            SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_traverse_list(unit, ep, dnx_oam_get_rmep));
            oam_ep_results->no_rmeps += rmep_index;
            if (rmep_index > 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", "Assocoated RMEPs");
                rmep_cnt = rmep_index;
                if (rmep_index >= MAX_RMEPS_TO_DISPLAY)
                {
                    rmep_cnt = MAX_RMEPS_TO_DISPLAY;
                }
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                for (rmep_no = 0; rmep_no < rmep_cnt; rmep_no++)
                {
                    DNX_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmep_list[rmep_no], endpoint_id);
                    SHR_IF_ERR_EXIT(bcm_oam_endpoint_get(unit, endpoint_id, &rmep_info));
                    loc =
                        (rmep_info.faults & BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT) ==
                        BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT ? 1 : 0;
                    rdi = (rmep_info.faults & BCM_OAM_ENDPOINT_FAULT_REMOTE) == BCM_OAM_ENDPOINT_FAULT_REMOTE ? 1 : 0;
                    name = rmep_info.name;
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(2);
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("   ID");
                    PRT_CELL_SET("0x%x (0x%x)\n    Name 0x%x\n    LOC %d\n   RDI %d\n   IF State %d\n   Port State %d",
                                 endpoint_id, rmep_list[rmep_no], name, loc, rdi, rmep_info.interface_state,
                                 rmep_info.port_state);
                }
            }
        }
        else
        {
            /** RFC6374 entry */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "MPLS label on MPLS stack");
            PRT_CELL_SKIP(1);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "label");
            PRT_CELL_SET("%u", oam_ep_results->endpoint_info.egress_label.label);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "exp");
            PRT_CELL_SET("%u", oam_ep_results->endpoint_info.egress_label.exp);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET_SHIFT(1, "%s", "TTL");
            PRT_CELL_SET("%u", oam_ep_results->endpoint_info.egress_label.ttl);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "Int priority");
            PRT_CELL_SET("%d", oam_ep_results->endpoint_info.int_pri);
        }
    }
    else
    {
        PRT_CELL_SET("%s", "NO");
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam Endpoint command - display function
 * Displays the endpoint parameters
 * \param [in] unit -     Relevant unit.
 * \param [in] ep - ID of the endpoint to be displayed
 * \param [in] oam_ep_results - outputs of the oam ep command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_ep_statistics(
    int unit,
    bcm_oam_endpoint_t ep,
    oam_ep_results_t * oam_ep_results)
{
    int is_up;
    int is_mip;
    int is_acc;

    SHR_FUNC_INIT_VARS(unit);

    is_up = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(&oam_ep_results->endpoint_info);
    is_mip = DNX_OAM_DISSECT_IS_ENDPOINT_MIP(&oam_ep_results->endpoint_info);
    if (is_mip)
    {
        oam_ep_results->no_mips++;
    }
    else
    {
        if (is_up)
        {
            oam_ep_results->no_ups++;
        }
    }

    switch (oam_ep_results->endpoint_info.type)
    {
        case bcmOAMEndpointTypeEthernet:
        {
            oam_ep_results->no_eth++;
            break;
        }
        case bcmOAMEndpointTypeBHHMPLS:
        {
            oam_ep_results->no_bhh_mpls++;
            break;
        }
        case bcmOAMEndpointTypeBHHPwe:
        {
            oam_ep_results->no_bhh_pwe++;
            break;
        }
        case bcmOAMEndpointTypeBHHPweGAL:
        {
            oam_ep_results->no_bhh_pwe_gal++;
            break;
        }
        case bcmOAMEndpointTypeMPLSNetwork:
        {
            oam_ep_results->no_mpls++;
            break;
        }
        case bcmOAMEndpointTypeBhhSection:
        {
            oam_ep_results->no_bhh_sec++;
            break;
        }
        default:
        {
            oam_ep_results->no_errs++;
            break;
        }
    }

    is_acc = DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(&oam_ep_results->endpoint_info);
    if (is_acc)
    {
        oam_ep_results->no_accs++;
    }
/*
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_traverse_list(unit, ep, dnx_oam_get_rmep));

    oam_ep_results->no_rmeps += rmep_index;
*/
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam Endpoint command - Statistics display function
 * Displays the statistics of the endpoints
 * \param [in] unit -     Relevant unit.
 * \param [in] oam_ep_results - outputs of the oam ep command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_ep_statistics_display(
    int unit,
    oam_ep_results_t * oam_ep_results,
    sh_sand_control_t * sand_control)
{

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("ENDPOINT STATISTICS");
    PRT_COLUMN_ADD("Direction");
    PRT_COLUMN_ADD("MEPs");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("MEPs");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("MEPs");

    {

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "DOWNMEPs");
        PRT_CELL_SET("%d", (oam_ep_results->no_ep - oam_ep_results->no_ups - oam_ep_results->no_mips));

        PRT_CELL_SET("%s", "Accelerated");
        PRT_CELL_SET("%d", oam_ep_results->no_accs);

        PRT_CELL_SET("%s", "Ethernet");
        PRT_CELL_SET("%d", oam_ep_results->no_eth);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "UPMEPs");
        PRT_CELL_SET("%d", oam_ep_results->no_ups);

        PRT_CELL_SET("%s", "Non-Accelerated");
        PRT_CELL_SET("%d", oam_ep_results->no_ep - oam_ep_results->no_accs);

        PRT_CELL_SET("%s", "MPLS");
        PRT_CELL_SET("%d", oam_ep_results->no_mpls);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "MIPs");
        PRT_CELL_SET("%d", oam_ep_results->no_mips);

        PRT_CELL_SKIP(2);

        PRT_CELL_SET("%s", "BHH-MPLS");
        PRT_CELL_SET("%d", oam_ep_results->no_bhh_mpls);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(4);

        PRT_CELL_SET("%s", "BHH-PWE");
        PRT_CELL_SET("%d", oam_ep_results->no_bhh_pwe);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(4);

        PRT_CELL_SET("%s", "BHH-PWEoGAL");
        PRT_CELL_SET("%d", oam_ep_results->no_bhh_pwe_gal);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(4);

        PRT_CELL_SET("%s", "BHH-SEC");
        PRT_CELL_SET("%d", oam_ep_results->no_bhh_sec);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Total Endpoints");
        PRT_CELL_SET("%d", oam_ep_results->no_ep);

        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%d", oam_ep_results->no_ep);

        PRT_CELL_SKIP(1);
        PRT_CELL_SET("%d", oam_ep_results->no_ep);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "RMEPs");
        PRT_CELL_SET("%d", oam_ep_results->no_rmeps);
        PRT_CELL_SKIP(4);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Errors");
        PRT_CELL_SET("%d", oam_ep_results->no_errs);
        PRT_CELL_SKIP(4);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam EnpPoint command - display properties of a single
 * endpoint.
 * \param [in] unit - Relevant unit.
 * \param [in] display_on - When set the endpoint parameters
 *        will be displayed, other wise only statistics will be
 *        gathered.
 * \param [in] id -   Endpoint ID
 * \param [in] oam_ep_results - outputs of the oam ep command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_ep_display_single_endpoint(
    int unit,
    int display_on,
    uint32 id,
    oam_ep_results_t * oam_ep_results,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get Endpoint Parameters
     */
    SHR_IF_ERR_EXIT(diag_oam_ep_get_results(unit, id, oam_ep_results));
    if (display_on)
    {
        /*
         *  Display Endpoint parameters
         */
        SHR_IF_ERR_EXIT(diag_oam_ep_display(unit, id, oam_ep_results, sand_control));
    }
    else
    {
        /*
         *  Gather statistics
         */
        SHR_IF_ERR_EXIT(diag_oam_ep_statistics(unit, id, oam_ep_results));
    }
    oam_ep_results->no_ep++;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam COUnters command - Get number of counters in an engine
 * \param [in] unit - Relevant unit.
 * \param [in] id -   engine_id
 * \param [out] num_of_counters - number of counters in the
 *        engine
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cntrs_get_engine_size(
    int unit,
    int id,
    int *num_of_counters)
{
    SHR_FUNC_INIT_VARS(unit);
    {
        if ((id >= 0) && (id < 8))
        {
            *num_of_counters = 8192;
        }
        else if (((id >= 8) && (id < 15)))
        {
            *num_of_counters = 16384;
        }
        else if (((id >= 16) && (id < 22)))
        {
            *num_of_counters = 32768;
        }
        else
        {
            *num_of_counters = 0;
            SHR_ERR_EXIT(_SHR_E_PARAM, "");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam COUnters command - Calculate the engine_id and the memory
 * index of a given counter
 * \param [in] unit - Relevant unit.
 * \param [in] oam_cntrs_results - outputs of the oam counters
 * \param [in] counter_base -  Requested counter id
 * \param [in] counter_if -  Requested counter if
 * \param [out] counter_info - endpoint's counter information
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cntrs_get_engine_and_index(
    int unit,
    oam_cntrs_results_t * oam_cntrs_results,
    int counter_base,
    int counter_if,
    mep_counter_info_t * counter_info)
{
    int found_index;
    int found_engine_id;
    int engine_no;
    int database_id;
    int engine_first_counter;
    int engine_last_counter;
    int num_of_counters;
    int counter_index;

    SHR_FUNC_INIT_VARS(unit);

    database_id = 0;
    counter_info->ing_engine_id = 0;
    counter_info->ing_index = 0;
    counter_info->eg_engine_id = 0;
    counter_info->eg_index = 0;
    while (database_id < MAX_NOF_DATABASES)
    {
        if (oam_cntrs_results->database[database_id].is_oam)
        {
            if (oam_cntrs_results->database[database_id].cntr_if == counter_if)
            {
                counter_index = counter_base / 4;
                if ((counter_index >= oam_cntrs_results->database[database_id].start_counter)
                    && (counter_index <= oam_cntrs_results->database[database_id].end_counter))
                {
                  /** Database found. Now find the engine */
                    found_index = 0;
                    found_engine_id = 0;
                    engine_first_counter = oam_cntrs_results->database[database_id].start_counter;
                    engine_last_counter = oam_cntrs_results->database[database_id].start_counter;
                    for (engine_no = 0; engine_no < oam_cntrs_results->database[database_id].nof_engines; engine_no++)
                    {
                        SHR_IF_ERR_EXIT(diag_oam_cntrs_get_engine_size
                                        (unit, oam_cntrs_results->database[database_id].engine[engine_no],
                                         &num_of_counters));
                        engine_last_counter += num_of_counters;
                        if (counter_base <= engine_last_counter)
                        {
                         /** Engine found */
                            found_engine_id = oam_cntrs_results->database[database_id].engine[engine_no];
                            found_index = counter_base - engine_first_counter;
                            break;
                        }
                        engine_first_counter += num_of_counters;
                    }
                    if (oam_cntrs_results->database[database_id].is_oam_ing)
                    {
                        counter_info->ing_engine_id = found_engine_id;
                        counter_info->ing_index = found_index;
                    }
                    else if (oam_cntrs_results->database[database_id].is_oam_eg)
                    {
                        counter_info->eg_engine_id = found_engine_id;
                        counter_info->eg_index = found_index;
                    }
                }
            }
        }
        database_id++;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  oam COUnters command - display counters of a single endpoint.
 * \param [in] unit - Relevant unit.
 * \param [in] core - Relevant core.
 * \param [in] id -   Endpoint ID
 * \param [in] oam_cntrs_results - outputs of the oam counters
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_ep_display_single_endpoint_cntrs(
    int unit,
    int core,
    uint32 id,
    oam_cntrs_results_t * oam_cntrs_results)
{
    bcm_oam_endpoint_info_t endpoint_info;
    mep_counter_info_t counter_info;
    int ing_index;
    int eg_index;
    int pcp_index;
    int msb_bits;
    int mask;
    int counter_bank;
    int ing_engine;
    int eg_engine;
    int block;
    soc_reg_above_64_val_t ing_entry_above_64, eg_entry_above_64;
    uint64 temp_64;
    uint64 temp_64_a;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(ing_entry_above_64);
    SOC_REG_ABOVE_64_CLEAR(eg_entry_above_64);

    /*
     *  Get Endpoint Parameters
     */
    bcm_oam_endpoint_info_t_init(&endpoint_info);
    /*
     * Get endpoint_info
     */
    SHR_IF_ERR_EXIT(bcm_oam_endpoint_get(unit, id, &endpoint_info));

    oam_cntrs_results->endpoint_results.is_pcp = (endpoint_info.lm_flags & BCM_OAM_LM_PCP);
    oam_cntrs_results->endpoint_results.counter_base = endpoint_info.lm_counter_base_id;
    oam_cntrs_results->endpoint_results.counter_if = endpoint_info.lm_counter_if;

    /*
     * Get counter engines and indexes to these engines
     */
    SHR_IF_ERR_EXIT(diag_oam_cntrs_get_engine_and_index
                    (unit, oam_cntrs_results, endpoint_info.lm_counter_base_id, endpoint_info.lm_counter_if,
                     &counter_info));

    ing_index = counter_info.ing_index >> 2;
    eg_index = counter_info.eg_index >> 2;

    ing_engine = counter_info.ing_engine_id;
    eg_engine = counter_info.eg_engine_id;
    counter_bank = MCP_ENG_DB_A_MEMm;
    if (counter_info.ing_engine_id > 7)
    {
        counter_bank = MCP_ENG_DB_B_MEMm;
        ing_engine -= 8;
        eg_engine -= 8;
    }
    if (counter_info.ing_engine_id > 15)
    {
        counter_bank = MCP_ENG_DB_C_MEMm;
        ing_engine -= 8;
        eg_engine -= 8;
    }

    /*
     * Read counter values (136 bytes for each 8 counters (for pcp))
     */
    block = MCP_BLOCK(unit, core);
    SHR_IF_ERR_EXIT(soc_mem_array_read(unit, counter_bank, ing_engine, block, ing_index, &ing_entry_above_64));
    SHR_IF_ERR_EXIT(soc_mem_array_read(unit, counter_bank, eg_engine, block, eg_index, &eg_entry_above_64));

    /*
     * Prepare values of pcp=0 counter. If no-pcp, this is the only value needed.
     */
    COMPILER_64_SET(temp_64, 0, ing_entry_above_64[1]);
    COMPILER_64_SET(temp_64_a, 0, 3);
    COMPILER_64_AND(temp_64, temp_64_a);
    COMPILER_64_SHL(temp_64, 32);
    COMPILER_64_ADD_32(oam_cntrs_results->endpoint_results.ing_counter_value[0], ing_entry_above_64[0]);
    COMPILER_64_ADD_64(oam_cntrs_results->endpoint_results.ing_counter_value[0], temp_64);

    COMPILER_64_SET(temp_64, 0, eg_entry_above_64[1]);
    COMPILER_64_SET(temp_64_a, 0, 3);
    COMPILER_64_AND(temp_64, temp_64_a);
    COMPILER_64_SHL(temp_64, 32);
    COMPILER_64_ADD_32(oam_cntrs_results->endpoint_results.eg_counter_value[0], eg_entry_above_64[0]);
    COMPILER_64_ADD_64(oam_cntrs_results->endpoint_results.eg_counter_value[0], temp_64);
    /*
     * In case of pcp, prepare values of all counters
     */
    if (oam_cntrs_results->endpoint_results.is_pcp)
    {
        for (pcp_index = 1; pcp_index < 4; pcp_index++)
        {
            msb_bits = 2 * (pcp_index + 1);
            mask = (1 << msb_bits) - 1;
            COMPILER_64_SET(temp_64_a, 0, mask);

            COMPILER_64_SET(temp_64, 0, ing_entry_above_64[pcp_index + 1]);
            COMPILER_64_AND(temp_64, temp_64_a);
            COMPILER_64_SHL(temp_64, 32);
            COMPILER_64_ADD_32(oam_cntrs_results->endpoint_results.ing_counter_value[pcp_index],
                               (ing_entry_above_64[pcp_index] >> (pcp_index * 2)));
            COMPILER_64_ADD_64(oam_cntrs_results->endpoint_results.ing_counter_value[pcp_index], temp_64);

            COMPILER_64_SET(temp_64, 0, eg_entry_above_64[pcp_index + 1]);
            COMPILER_64_AND(temp_64, temp_64_a);
            COMPILER_64_SHL(temp_64, 32);
            COMPILER_64_ADD_32(oam_cntrs_results->endpoint_results.eg_counter_value[pcp_index],
                               (eg_entry_above_64[pcp_index] >> (pcp_index * 2)));
            COMPILER_64_ADD_64(oam_cntrs_results->endpoint_results.eg_counter_value[pcp_index], temp_64);
        }
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, counter_bank, ing_engine, block, ing_index + 1, &ing_entry_above_64));
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, counter_bank, eg_engine, block, eg_index + 1, &eg_entry_above_64));
        for (pcp_index = 0; pcp_index < 4; pcp_index++)
        {
            msb_bits = 2 * (pcp_index + 1);
            mask = (1 << msb_bits) - 1;
            COMPILER_64_SET(temp_64_a, 0, mask);

            COMPILER_64_SET(temp_64, 0, ing_entry_above_64[pcp_index + 1]);
            COMPILER_64_AND(temp_64, temp_64_a);
            COMPILER_64_SHL(temp_64, 32);
            COMPILER_64_ADD_32(oam_cntrs_results->endpoint_results.ing_counter_value[pcp_index + 4],
                               (ing_entry_above_64[pcp_index] >> (pcp_index * 2)));
            COMPILER_64_ADD_64(oam_cntrs_results->endpoint_results.ing_counter_value[pcp_index + 4], temp_64);

            COMPILER_64_SET(temp_64, 0, eg_entry_above_64[pcp_index + 1]);
            COMPILER_64_AND(temp_64, temp_64_a);
            COMPILER_64_SHL(temp_64, 32);
            COMPILER_64_ADD_32(oam_cntrs_results->endpoint_results.eg_counter_value[pcp_index + 4],
                               (eg_entry_above_64[pcp_index] >> (pcp_index * 2)));
            COMPILER_64_ADD_64(oam_cntrs_results->endpoint_results.eg_counter_value[pcp_index + 4], temp_64);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam EnpPoint command - traverse all endpoints and display
 * their properties one by one.
 * \param [in] unit -     Relevant unit.
 * \param [in] id -  ID of the requested endpoint
 * \param [in] oam_ep_results - outputs of the oam ep command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_ep_display_all(
    int unit,
    int32 id,
    oam_ep_results_t * oam_ep_results,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id_ep;
    int is_ep_end;
    int display;
    uint32 ep;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

       /** Get handle to table   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ENDPOINT_INFO_SW, &entry_handle_id_ep));

      /** Create iterator   */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id_ep, DBAL_ITER_MODE_ALL));

      /** Read first entry   */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ep, &is_ep_end));
    while (!is_ep_end)
    {
          /** Get endpoint ID from the entry key  */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id_ep, DBAL_FIELD_OAM_ENDPOINT_ID, &ep));

        /*
         * If specific endpoint was requested display it else display all endpoints
         */
        display = DISPLAY_OFF;
        if ((ep == id) || (id == -1))
        {
            display = DISPLAY_ON;
        }
        /*
         * Display Parameters of the Endpoint
         */
        SHR_IF_ERR_EXIT(diag_oam_ep_display_single_endpoint(unit, display, ep, oam_ep_results, sand_control));

          /** Read next entry   */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ep, &is_ep_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam CounTeRS command - traverse all endpoints, get their
 * counter_if and counter_base, and display their values one
 * by one.
 * \param [in] unit -     Relevant unit.
 * \param [in] core -     Relevant core.
 * \param [in] id -  ID of the requested endpoint
 * \param [in] oam_cntrs_results - outputs of the oam counters
 *        command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cntrs_display_all(
    int unit,
    int core,
    int32 id,
    oam_cntrs_results_t * oam_cntrs_results,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id_ep;
    int is_ep_end;
    uint32 ep;
    int pcp_index;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Display Title and Columns */
    PRT_TITLE_SET("ENDPOINT COUNTERS");
    PRT_COLUMN_ADD("Endpoint ID");
    PRT_COLUMN_ADD("Counter If");
    PRT_COLUMN_ADD("Counter Base");
    for (pcp_index = 0; pcp_index < 8; pcp_index++)
    {
        PRT_COLUMN_ADD("Counter PCP=%d", pcp_index);
    }

       /** Get handle to table   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ENDPOINT_INFO_SW, &entry_handle_id_ep));

      /** Create iterator   */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id_ep, DBAL_ITER_MODE_ALL));

      /** Read first entry   */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ep, &is_ep_end));
    while (!is_ep_end)
    {
          /** Get endpoint ID from the entry key  */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id_ep, DBAL_FIELD_OAM_ENDPOINT_ID, &ep));

        /*
         * If specific endpoint was requested display it else display all endpoints
         */
        if ((ep == id) || (id == -1))
        {
            /*
             * Display Counters of the Endpoint
             */
            SHR_IF_ERR_EXIT(diag_oam_ep_display_single_endpoint_cntrs(unit, core, ep, oam_cntrs_results));

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("0x%x = %d INGRESS", ep, ep);
            PRT_CELL_SET("%d", oam_cntrs_results->endpoint_results.counter_if);
            PRT_CELL_SET("%d", oam_cntrs_results->endpoint_results.counter_base);
            PRT_CELL_SET(COMPILER_64_PRINTF_FORMAT,
                         COMPILER_64_PRINTF_VALUE(oam_cntrs_results->endpoint_results.ing_counter_value[0]));
            for (pcp_index = 1; pcp_index < 8; pcp_index++)
            {
                if (oam_cntrs_results->endpoint_results.is_pcp)
                {
                    PRT_CELL_SET(COMPILER_64_PRINTF_FORMAT,
                                 COMPILER_64_PRINTF_VALUE(oam_cntrs_results->
                                                          endpoint_results.ing_counter_value[pcp_index]));
                }
                else
                {
                    PRT_CELL_SET("--");
                }
            }
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("0x%x = %d EGRESS", ep, ep);
            PRT_CELL_SET("%d", oam_cntrs_results->endpoint_results.counter_if);
            PRT_CELL_SET("%d", oam_cntrs_results->endpoint_results.counter_base);
            PRT_CELL_SET(COMPILER_64_PRINTF_FORMAT,
                         COMPILER_64_PRINTF_VALUE(oam_cntrs_results->endpoint_results.eg_counter_value[0]));
            for (pcp_index = 1; pcp_index < 8; pcp_index++)
            {
                if (oam_cntrs_results->endpoint_results.is_pcp)
                {
                    PRT_CELL_SET(COMPILER_64_PRINTF_FORMAT,
                                 COMPILER_64_PRINTF_VALUE(oam_cntrs_results->
                                                          endpoint_results.eg_counter_value[pcp_index]));
                }
                else
                {
                    PRT_CELL_SET("--");
                }
            }
        }

          /** Read next entry   */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ep, &is_ep_end));
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam EnpPoint command - main function
 * Gets input and calls verify, get_results and display
 * functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oam_oamep_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oam_ep_inputs_t oam_ep_inputs;
    oam_ep_results_t oam_ep_results;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_INT32("ID", oam_ep_inputs.ep_id);
    SH_SAND_GET_INT32("core", oam_ep_inputs.core_id);

    /*
     *  Verify inputs
     */
    SHR_INVOKE_VERIFY_DNX(diag_oam_ep_verify(unit, &oam_ep_inputs));

    SHR_IF_ERR_EXIT(diag_oam_ep_init(unit, &oam_ep_results));

    /*
     * Display all requested endpoints and gather statistics information
     */
    SHR_IF_ERR_EXIT(diag_oam_ep_display_all(unit, oam_ep_inputs.ep_id, &oam_ep_results, sand_control));

    /*
     *  Display Endpoint Statistics
     */
    SHR_IF_ERR_EXIT(diag_oam_ep_statistics_display(unit, &oam_ep_results, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam CounTeRS command - Initialize result variables
 * \param [in] unit -     Relevant unit.
 * \param [in] oam_cntrs_results - outputs of the oam counters
 *        command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_cntrs_init_results(
    int unit,
    oam_cntrs_results_t * oam_cntrs_results)
{
    int index;
    SHR_FUNC_INIT_VARS(unit);

    oam_cntrs_results->endpoint_results.counter_if = 0;
    oam_cntrs_results->endpoint_results.counter_base = 0;
    oam_cntrs_results->endpoint_results.is_pcp = 0;

    for (index = 0; index < 8; ++index)
    {
        COMPILER_64_ZERO(oam_cntrs_results->endpoint_results.ing_counter_value[index]);
        COMPILER_64_ZERO(oam_cntrs_results->endpoint_results.eg_counter_value[index]);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam CounTeRS command - main function Gets input and calls
 * verify, gets database info, and display functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oam_cntrs_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oam_cntrs_inputs_t oam_cntrs_inputs;
    oam_cntrs_results_t oam_cntrs_results;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_INT32("ID", oam_cntrs_inputs.ep_id);
    SH_SAND_GET_INT32("core", oam_cntrs_inputs.core_id);

    /*
     *  Verify inputs
     */
    SHR_INVOKE_VERIFY_DNX(diag_oam_cntrs_verify(unit, &oam_cntrs_inputs));

    /*
     *  Initialize outputs
     */
    SHR_IF_ERR_EXIT(diag_oam_cntrs_init_results(unit, &oam_cntrs_results));

    /*
     * Get counter database information
     */
    SHR_IF_ERR_EXIT(diag_oam_cntrs_get_database_info(unit, oam_cntrs_inputs.core_id, &oam_cntrs_results));

    /*
     * Display all requested endpoints and gather statistics information
     */
    SHR_IF_ERR_EXIT(diag_oam_cntrs_display_all
                    (unit, oam_cntrs_inputs.core_id, oam_cntrs_inputs.ep_id, &oam_cntrs_results, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get all information from OAMP_RX_TX_COUNTERS
 * \param [in] unit -     Relevant unit.
 * \param [out] *oamp_ctrs_outputs - values to be displayed.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_ctrs_get_results(
    int unit,
    oamp_ctrs_diag_output_t * oamp_ctrs_outputs)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_TX_RX_COUNTERS, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TX_COUNTER, INST_SINGLE, &(oamp_ctrs_outputs->tx_counter)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RX_COUNTER, INST_SINGLE, &(oamp_ctrs_outputs->rx_counter)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITR_DROP, INST_SINGLE, &(oamp_ctrs_outputs->itr_drop)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OVERSIZED_RX_PACKET, INST_SINGLE,
                     &(oamp_ctrs_outputs->oversized_rx_pkt)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ITR_COUNT, INST_SINGLE, &(oamp_ctrs_outputs->itr_count)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TXM_DROPPED_COUNT, INST_SINGLE,
                     &(oamp_ctrs_outputs->txm_dropped_count)));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TXO_DROPPED_COUNT, INST_SINGLE,
                     &(oamp_ctrs_outputs->txo_dropped_count)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oamp ctrs command - display function
 * Displays the OAMP TX/RX counters.
 * \param [in] unit -     Relevant unit.
 * \param [in] *oamp_ctrs_outputs - values to be displayed.
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oamp_ctrs_display(
    int unit,
    oamp_ctrs_diag_output_t * oamp_ctrs_outputs,
    sh_sand_control_t * sand_control)
{

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("OAMP TX/RX counters");
    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD("Value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "TxCounter");
    PRT_CELL_SET("%d", (oamp_ctrs_outputs->tx_counter));

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "RxCounter");
    PRT_CELL_SET("%d", (oamp_ctrs_outputs->rx_counter));

    if (oamp_ctrs_outputs->itr_drop)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "InterruptDrop");
        PRT_CELL_SET("%d", (oamp_ctrs_outputs->itr_drop));
    }

    if (oamp_ctrs_outputs->itr_count)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "InterruptCount");
        PRT_CELL_SET("%d", (oamp_ctrs_outputs->itr_count));
    }

    if (oamp_ctrs_outputs->oversized_rx_pkt)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "OversizedRxPacket");
        PRT_CELL_SET("%d", (oamp_ctrs_outputs->oversized_rx_pkt));
    }

    if (oamp_ctrs_outputs->txm_dropped_count)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "TxmDroppedCount");
        PRT_CELL_SET("%d", (oamp_ctrs_outputs->txm_dropped_count));
    }

    if (oamp_ctrs_outputs->txo_dropped_count)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "TxoDroppedCount");
        PRT_CELL_SET("%d", (oamp_ctrs_outputs->txo_dropped_count));
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * oamp ctrs command - main function
 * Calls get_results and display functions
 * \param [in] unit - Relevant unit
 * \param [in] args - Arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oamp_ctrs_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oamp_ctrs_diag_output_t oamp_ctrs_outputs;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get requested information
     */
    SHR_IF_ERR_EXIT(diag_oamp_ctrs_get_results(unit, &oamp_ctrs_outputs));

    /*
     *  Display requested information
     */
    SHR_IF_ERR_EXIT(diag_oamp_ctrs_display(unit, &oamp_ctrs_outputs, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command - Read OAM ID signal
 * \param [in] unit -     Relevant unit.
 * \param [in] side -     Relevant pipeline.
 * \param [in] *oam_oamid_params - pointer to params to oam oam_id
 *        command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_oamid_read_signals(
    int unit,
    int side,
    oam_oamid_params_t * oam_oamid_params)
{
    int rv;
    uint32 is_non_acc;
    SHR_FUNC_INIT_VARS(unit);

    if (side == PIPE_TYPE_INGRESS)
    {
        /** Receive the value of the OAM_ID signal. Set it to 0 if it couldn't be received. */
        rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "OAM_ID", &oam_oamid_params->oam_id,
                           3);
        if (rv != BCM_E_NONE)
        {
            oam_oamid_params->oam_id = 0;
            SHR_EXIT();
        }
        else
        {
            rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "OAM_ID_NON_ACC_indication",
                               &is_non_acc, 1);
            if (is_non_acc == 1)
            {
                /**
                * Structure of OAM ID for non-acc MEPs
                * 1'b1,2'b0,1'b1,my_cfm_mac(1),ingress(1),packet_is_bfd(1),oam_opcode(4),mp_type(4), mp_profile(4)
                */
                oam_oamid_params->is_acc = 0;
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "My_CFM_MAC",
                                   &oam_oamid_params->mymac, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->mymac = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "is_ingress",
                                   &oam_oamid_params->ing, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->ing = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "Packet_is_bfd",
                                   &oam_oamid_params->pkt_is_bfd, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->pkt_is_bfd = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "OPCODE",
                                   &oam_oamid_params->opcode, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->opcode = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "MP_Type",
                                   &oam_oamid_params->mp_type, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->mp_type = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "MP_profile",
                                   &oam_oamid_params->profile_id, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->profile_id = 0;
                }
            }
            else
            {
                /**
                * Structure of OAM ID for acc MEPs
                * 1'b0,acc_mep_db.mp_profile[1:0],acc_mep_db.oam_id[15:0]
                */
                oam_oamid_params->is_acc = 1;
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "MP_profile",
                                   &oam_oamid_params->profile_id, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->profile_id = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "IRPP", "FWD2", NULL, "MEP_ID",
                                   &oam_oamid_params->mep_id, 2);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->mep_id = 0;
                }
            }
        }
    }

    if (side == PIPE_TYPE_EGRESS)
    {
        /** Receive the value of the OAM_ID signal. Set it to 0 if it couldn't be received. */
        rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "OAM_ID", &oam_oamid_params->oam_id,
                           3);
        if (rv != BCM_E_NONE)
        {
            oam_oamid_params->oam_id = 0;
            SHR_EXIT();
        }
        else
        {
            rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "OAM_ID_NON_ACC_indication",
                               &is_non_acc, 1);
            if (rv != BCM_E_NONE)
            {
                oam_oamid_params->mep_id = 0;
            }
            if (is_non_acc == 1)
            {
                /**
                * Structure of OAM ID for non-acc MEPs
                * 1'b1,2'b0,1'b1,my_cfm_mac(1),ingress(1),packet_is_bfd(1),oam_opcode(4),mp_type(4), mp_profile(4)
                */
                oam_oamid_params->is_acc = 0;
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "My_CFM_MAC",
                                   &oam_oamid_params->mymac, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->mymac = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "is_ingress",
                                   &oam_oamid_params->ing, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->ing = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "Packet_is_bfd",
                                   &oam_oamid_params->pkt_is_bfd, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->pkt_is_bfd = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "OPCODE",
                                   &oam_oamid_params->opcode, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->opcode = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "MP_Type",
                                   &oam_oamid_params->mp_type, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->mp_type = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "MP_profile",
                                   &oam_oamid_params->profile_id, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->profile_id = 0;
                }
            }
            else
            {
                /**
                * Structure of OAM ID for acc MEPs
                * 1'b0,acc_mep_db.mp_profile[1:0],acc_mep_db.oam_id[15:0]
                */
                oam_oamid_params->is_acc = 1;
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "MP_profile",
                                   &oam_oamid_params->profile_id, 1);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->profile_id = 0;
                }
                rv = dpp_dsig_read(unit, oam_oamid_params->core_id, "ETPP", "Term", NULL, "MEP_ID",
                                   &oam_oamid_params->mep_id, 2);
                if (rv != BCM_E_NONE)
                {
                    oam_oamid_params->mep_id = 0;
                }
            }
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command - Decode given oam_id to inputs.
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_oamid_params - pointer to params to oam oam_id
 *        command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_oamid_decode(
    int unit,
    oam_oamid_params_t * oam_oamid_params)
{
    uint32 acc_mask = 0x40000;
    uint32 non_acc_mask = 0x48000;
    uint32 mac_mask = 0x4000;
    uint32 ing_mask = 0x2000;
    uint32 bfd_mask = 0x1000;
    uint8 op_prof_type_mask = 0xf;
    uint32 acc_mep_id_mask = 0xffff;
    uint8 acc_prof_mask = 0x3;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get case
     */
    if (!(oam_oamid_params->oam_id & acc_mask))
    {
        oam_oamid_params->is_acc = 1;
        oam_oamid_params->profile_id = (oam_oamid_params->oam_id >> 16) & acc_prof_mask;
        oam_oamid_params->mep_id = oam_oamid_params->oam_id & acc_mep_id_mask;
    }
    else
    {
        if (!(oam_oamid_params->oam_id & non_acc_mask))
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "The OAM ID is invalid - 0x%x\n", oam_oamid_params->oam_id);
        }
        oam_oamid_params->is_acc = 0;
        oam_oamid_params->mymac = (oam_oamid_params->oam_id & mac_mask) >> 14;
        oam_oamid_params->ing = (oam_oamid_params->oam_id & ing_mask) >> 13;
        oam_oamid_params->pkt_is_bfd = (oam_oamid_params->oam_id & bfd_mask) >> 12;
        oam_oamid_params->opcode = (oam_oamid_params->oam_id >> 8) & op_prof_type_mask;
        oam_oamid_params->mp_type = (oam_oamid_params->oam_id >> 4) & op_prof_type_mask;
        oam_oamid_params->profile_id = oam_oamid_params->oam_id & op_prof_type_mask;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command - Statistics display function
 * Displays the statistics of the OAM ID
 * \param [in] unit -     Relevant unit.
 * \param [out] oam_oamid_inputs - expected oam id
 * \param [out] oam_oamid_results - actual oam id
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_oamid_cmp_and_display(
    int unit,
    oam_oamid_params_t * oam_oamid_inputs,
    oam_oamid_params_t * oam_oamid_results,
    sh_sand_control_t * sand_control)
{
    uint32 opcode_arr[2];
    uint32 mp_type_arr[2];
    int indx = 0;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    opcode_arr[0] = oam_oamid_inputs->opcode;
    opcode_arr[1] = oam_oamid_results->opcode;
    mp_type_arr[0] = oam_oamid_inputs->mp_type;
    mp_type_arr[1] = oam_oamid_results->mp_type;

    /** Cover corner case when provided and actual ID could be for different meps */
    if (oam_oamid_inputs->is_acc != oam_oamid_results->is_acc)
    {
        if (oam_oamid_inputs->is_acc)
        {
            LOG_CLI((BSL_META_U(unit, "Provided ID is for \033[91m Accelerated hit\033[0m, but actual is not!\n")));
        }
        else
        {
            LOG_CLI((BSL_META_U(unit, "Actual ID is for \033[91m Accelerated hit\033[0m, but provided is not!\n")));
        }
        SHR_EXIT();
    }
    /*
     * Print table header
     */
    if (oam_oamid_inputs->oam_id != oam_oamid_results->oam_id)
    {
        PRT_TITLE_SET("Expected OAM_ID DOESN'T MATCH actual");
    }
    else
    {
        PRT_TITLE_SET("Expected OAM_ID MATCH actual");
    }

    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD("Expected Value");
    PRT_COLUMN_ADD("Actual Value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "OAM ID");
    PRT_CELL_SET("%x", oam_oamid_inputs->oam_id);
    PRT_CELL_SET("%x", oam_oamid_results->oam_id);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    if (oam_oamid_inputs->is_acc && oam_oamid_results->is_acc)
    {
        PRT_CELL_SET("%s", "MEP ID");
        PRT_CELL_SET("%d", oam_oamid_inputs->mep_id);
        PRT_CELL_SET("%d", oam_oamid_results->mep_id);
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "Profile ID");
        PRT_CELL_SET("0x%x", oam_oamid_inputs->profile_id);
        PRT_CELL_SET("0x%x", oam_oamid_results->profile_id);
    }
    else
    {
        PRT_CELL_SET("%s", "MyMac set");
        if (oam_oamid_inputs->mymac)
        {
            PRT_CELL_SET("%s", "Yes");
        }
        else
        {
            PRT_CELL_SET("%s", "No");
        }
        if (oam_oamid_results->mymac)
        {
            PRT_CELL_SET("%s", "Yes");
        }
        else
        {
            PRT_CELL_SET("%s", "No");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "Side");
        if (oam_oamid_inputs->ing)
        {
            PRT_CELL_SET("%s", "Ingress");
        }
        else
        {
            PRT_CELL_SET("%s", "Egress");
        }
        if (oam_oamid_results->ing)
        {
            PRT_CELL_SET("%s", "Ingress");
        }
        else
        {
            PRT_CELL_SET("%s", "Egress");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "Packet is BFD");
        if (oam_oamid_inputs->pkt_is_bfd)
        {
            PRT_CELL_SET("%s", "Yes");
        }
        else
        {
            PRT_CELL_SET("%s", "No");
        }
        if (oam_oamid_results->pkt_is_bfd)
        {
            PRT_CELL_SET("%s", "Yes");
        }
        else
        {
            PRT_CELL_SET("%s", "No");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "OpCode");
        for (indx = 0; indx < 2; indx++)
        {
            switch (opcode_arr[indx])
            {
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_BFD:
                {
                    PRT_CELL_SET("%s", "BFD");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_CCM:
                {
                    PRT_CELL_SET("%s", "CCM(1)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBR:
                {
                    PRT_CELL_SET("%s", "LBR(2)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBM:
                {
                    PRT_CELL_SET("%s", "LBM(3)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTR:
                {
                    PRT_CELL_SET("%s", "LTR(4)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTM:
                {
                    PRT_CELL_SET("%s", "LTM(5)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMR:
                {
                    PRT_CELL_SET("%s", "LMR(42)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMM:
                {
                    PRT_CELL_SET("%s", "LMM(43)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMR:
                {
                    PRT_CELL_SET("%s", "DMR(46)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM:
                {
                    PRT_CELL_SET("%s", "DMM(47)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_1DM:
                {
                    PRT_CELL_SET("%s", "1DM(45)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLM:
                {
                    PRT_CELL_SET("%s", "SLM(55)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLR:
                {
                    PRT_CELL_SET("%s", "SLR(54)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_AIS:
                {
                    PRT_CELL_SET("%s", "AIS(33)");
                    break;
                }
                case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LINEAR_APS:
                {
                    PRT_CELL_SET("%s", "LINEAR_APS(39)");
                    break;
                }
                default:
                {
                    PRT_CELL_SET("%s", "OTHER");
                    break;
                }
            }
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "MP Type");
        for (indx = 0; indx < 2; indx++)
        {
            switch (mp_type_arr[indx])
            {
                case DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP:
                {
                    PRT_CELL_SET("%s", "BELOW_LOWER_MEP");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW:
                {
                    PRT_CELL_SET("%s", "ACTIVE_MATCH_LOW");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW:
                {
                    PRT_CELL_SET("%s", "PASSIVE_MATCH_LOW");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIDDLE_AND_LOW:
                {
                    PRT_CELL_SET("%s", "BETWEEN_MIDDLE_AND_LOW");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE:
                {
                    PRT_CELL_SET("%s", "ACTIVE_MATCH_MIDDLE");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE:
                {
                    PRT_CELL_SET("%s", "PASSIVE_MATCH_MIDDLE");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE:
                {
                    PRT_CELL_SET("%s", "BETWEEN_HIGH_AND_MIDDLE");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH:
                {
                    PRT_CELL_SET("%s", "ACTIVE_MATCH_HIGH");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH:
                {
                    PRT_CELL_SET("%s", "PASSIVE_MATCH_HIGH");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_ABOVE_UPPER_MEP:
                {
                    PRT_CELL_SET("%s", "ABOVE_UPPER_MEP");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_MIP_MATCH:
                {
                    PRT_CELL_SET("%s", "MIP_MATCH");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_BFD:
                {
                    PRT_CELL_SET("%s", "BFD");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIP_AND_MEP:
                {
                    PRT_CELL_SET("%s", "BETWEEN_MIP_AND_MEP");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_NO_COUNTER:
                {
                    PRT_CELL_SET("%s", "ACTIVE_MATCH_NO_COUNTER");
                    break;
                }
                case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_NO_COUNTER:
                {
                    PRT_CELL_SET("%s", "PASSIVE_MATCH_NO_COUNTER");
                    break;
                }
                default:
                {
                    PRT_CELL_SET("%s", "RESERVED");
                    break;
                }
            }
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "Profile ID");
        PRT_CELL_SET("0x%x", oam_oamid_inputs->profile_id);
        PRT_CELL_SET("0x%x", oam_oamid_results->profile_id);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command - Statistics display function
 * Displays the statistics of the OAM ID
 * \param [in] unit -     Relevant unit.
 * \param [in] oam_oamid_params - outputs of the oam ep command
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_oamid_display(
    int unit,
    oam_oamid_params_t oam_oamid_params,
    sh_sand_control_t * sand_control)
{

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("OAM ID INFORMATION");

    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD("Value");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    if (oam_oamid_params.core_id > (-1))
    {
        PRT_CELL_SET("%s", "Core");
        PRT_CELL_SET("%d", oam_oamid_params.core_id);
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    }

    PRT_CELL_SET("%s", "OAM ID");
    PRT_CELL_SET("0x%x", oam_oamid_params.oam_id);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    if (oam_oamid_params.is_acc)
    {
        PRT_CELL_SET("%s", "MEP ID");
        PRT_CELL_SET("%d", oam_oamid_params.mep_id);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "Last 2'b of Profile ID");
        PRT_CELL_SET("0x%x", oam_oamid_params.profile_id);
    }
    else
    {
        PRT_CELL_SET("%s", "MyMac set");
        if (oam_oamid_params.mymac)
        {
            PRT_CELL_SET("%s", "Yes");
        }
        else
        {
            PRT_CELL_SET("%s", "No");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "Pipeline");
        if (oam_oamid_params.ing)
        {
            PRT_CELL_SET("%s", "Ingress");
        }
        else
        {
            PRT_CELL_SET("%s", "Egress");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "Packet is BFD");
        if (oam_oamid_params.pkt_is_bfd)
        {
            PRT_CELL_SET("%s", "Yes");
        }
        else
        {
            PRT_CELL_SET("%s", "No");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "OpCode");
        switch (oam_oamid_params.opcode)
        {
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_BFD:
            {
                PRT_CELL_SET("%s", "BFD");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_CCM:
            {
                PRT_CELL_SET("%s", "CCM(1)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBR:
            {
                PRT_CELL_SET("%s", "LBR(2)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBM:
            {
                PRT_CELL_SET("%s", "LBM(3)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTR:
            {
                PRT_CELL_SET("%s", "LTR(4)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTM:
            {
                PRT_CELL_SET("%s", "LTM(5)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMR:
            {
                PRT_CELL_SET("%s", "LMR(42)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMM:
            {
                PRT_CELL_SET("%s", "LMM(43)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMR:
            {
                PRT_CELL_SET("%s", "DMR(46)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM:
            {
                PRT_CELL_SET("%s", "DMM(47)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_1DM:
            {
                PRT_CELL_SET("%s", "1DM(45)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLM:
            {
                PRT_CELL_SET("%s", "SLM(55)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLR:
            {
                PRT_CELL_SET("%s", "SLR(54)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_AIS:
            {
                PRT_CELL_SET("%s", "AIS(33)");
                break;
            }
            case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LINEAR_APS:
            {
                PRT_CELL_SET("%s", "LINEAR_APS(39)");
                break;
            }
            default:
            {
                PRT_CELL_SET("%s", "OTHER");
                break;
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "MP Type");
        switch (oam_oamid_params.mp_type)
        {
            case DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP:
            {
                PRT_CELL_SET("%s", "BELOW_LOWER_MEP");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW:
            {
                PRT_CELL_SET("%s", "ACTIVE_MATCH_LOW");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW:
            {
                PRT_CELL_SET("%s", "PASSIVE_MATCH_LOW");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIDDLE_AND_LOW:
            {
                PRT_CELL_SET("%s", "BETWEEN_MIDDLE_AND_LOW");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE:
            {
                PRT_CELL_SET("%s", "ACTIVE_MATCH_MIDDLE");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE:
            {
                PRT_CELL_SET("%s", "PASSIVE_MATCH_MIDDLE");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE:
            {
                PRT_CELL_SET("%s", "BETWEEN_HIGH_AND_MIDDLE");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH:
            {
                PRT_CELL_SET("%s", "ACTIVE_MATCH_HIGH");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH:
            {
                PRT_CELL_SET("%s", "PASSIVE_MATCH_HIGH");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_ABOVE_UPPER_MEP:
            {
                PRT_CELL_SET("%s", "ABOVE_UPPER_MEP");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_MIP_MATCH:
            {
                PRT_CELL_SET("%s", "MIP_MATCH");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_BFD:
            {
                PRT_CELL_SET("%s", "BFD");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIP_AND_MEP:
            {
                PRT_CELL_SET("%s", "BETWEEN_MIP_AND_MEP");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_NO_COUNTER:
            {
                PRT_CELL_SET("%s", "ACTIVE_MATCH_NO_COUNTER");
                break;
            }
            case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_NO_COUNTER:
            {
                PRT_CELL_SET("%s", "PASSIVE_MATCH_NO_COUNTER");
                break;
            }
            default:
            {
                PRT_CELL_SET("%s", "RESERVED");
                break;
            }
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", "Profile ID");
        PRT_CELL_SET("0x%x", oam_oamid_params.profile_id);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command -Input verification
 * Displays the statistics of the OAM ID
 * \param [in] unit -     Relevant unit.
 * \param [out] oam_oamid_params - input of the oam id command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_oamid_get_verify(
    int unit,
    oam_oamid_params_t * oam_oamid_params)
{
    int num_of_cores;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Verify core_id
     *  Get number of cores for this device
     */
    num_of_cores = dnx_data_device.general.nof_cores_get(unit);

    if (oam_oamid_params->core_id > num_of_cores)
    {
        /*
         * Default core_id
         */
        oam_oamid_params->core_id = 0;
        LOG_CLI((BSL_META_U(unit, "\n* The diag will present information for CORE 0\n")));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command - main function
 * Gets input and calls read_signals, decode, and display
 * functions
 * \param [in] unit -       Relevant unit.
 * \param [in] args -       arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oam_oamid_cmd_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oam_oamid_params_t oam_oamid_params;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_UINT32("PIPE", oam_oamid_params.pipe);
    SH_SAND_GET_INT32("core", oam_oamid_params.core_id);

    /** Validate the inputs */
    SHR_IF_ERR_EXIT(diag_oam_oamid_get_verify(unit, &oam_oamid_params));

    if (oam_oamid_params.pipe & PIPE_TYPE_INGRESS)
    {
        /** Read OAM ID at the FLP at the ingress*/
        SHR_IF_ERR_EXIT(diag_oam_oamid_read_signals(unit, PIPE_TYPE_INGRESS, &oam_oamid_params));
        /** Display the result*/
        SHR_IF_ERR_EXIT(diag_oam_oamid_display(unit, oam_oamid_params, sand_control));
    }
    if (oam_oamid_params.pipe & PIPE_TYPE_EGRESS)
    {
        /** Read OAM ID at the FWD at the egress */
        SHR_IF_ERR_EXIT(diag_oam_oamid_read_signals(unit, PIPE_TYPE_EGRESS, &oam_oamid_params));
        /** Display the result*/
        SHR_IF_ERR_EXIT(diag_oam_oamid_display(unit, oam_oamid_params, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command - main function
 * Gets input and calls read_signals, decode, and display
 * functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oam_oamid_cmd_dcd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oam_oamid_params_t oam_oamid_params;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_UINT32("ID", oam_oamid_params.oam_id);

    /** Parse the input */
    SHR_IF_ERR_EXIT(diag_oam_oamid_decode(unit, &oam_oamid_params));

    /** Display the result*/
    SHR_IF_ERR_EXIT(diag_oam_oamid_display(unit, oam_oamid_params, sand_control));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command -Input verification
 * Displays the statistics of the OAM ID
 * \param [in] unit -     Relevant unit.
 * \param [out] oam_oamid_params - input of the oam id command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_oamid_verify_cmp(
    int unit,
    oam_oamid_params_t * oam_oamid_params)
{

    int num_of_cores;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Verify core_id
     *  Get number of cores for this device
     */
    num_of_cores = dnx_data_device.general.nof_cores_get(unit);

    if (oam_oamid_params->core_id > num_of_cores)
    {
        /*
         * Default core_id
         */
        oam_oamid_params->core_id = 0;
        LOG_CLI((BSL_META_U(unit, "\n* The diag will present information for CORE 0\n")));
    }

    if (oam_oamid_params->oam_id == (-1))
    {
        LOG_CLI((BSL_META_U(unit, "\n* ID is mandatory for CoMPare! Please provide ID\n")));
    }

    if (oam_oamid_params->pipe == (-1))
    {
        LOG_CLI((BSL_META_U(unit, "\n* PIPE is mandatory for CoMPare! Please provide PIPE\n")));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command - Initialize statistics variables
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_oamid_params - pointer to the retrieved
 *        parameters of the oam_id. The functions fills in the
 *        relevant fields of this structure.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_oamid_init(
    int unit,
    oam_oamid_params_t * oam_oamid_params)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init statistics
     */
    oam_oamid_params->core_id = 0;
    oam_oamid_params->is_acc = 0;
    oam_oamid_params->pipe = 0;
    oam_oamid_params->oam_id = 0;
    oam_oamid_params->ing = 0;
    oam_oamid_params->profile_id = 0;
    oam_oamid_params->opcode = 0;
    oam_oamid_params->pkt_is_bfd = 0;
    oam_oamid_params->mymac = 0;
    oam_oamid_params->mp_type = 0;
    oam_oamid_params->mep_id = 0;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam OAM_ID command - main function
 * Gets input and calls read_signals, decode, compare, and display
 * functions
 * \param [in] unit -     Relevant unit.
 * \param [in] args - arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oam_oamid_cmd_cmp(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oam_oamid_params_t oam_oamid_inputs;
    oam_oamid_params_t oam_oamid_results;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_oam_oamid_init(unit, &oam_oamid_inputs));
    SHR_IF_ERR_EXIT(diag_oam_oamid_init(unit, &oam_oamid_results));
    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_INT32("core", oam_oamid_inputs.core_id);
    SH_SAND_GET_UINT32("ID", oam_oamid_inputs.oam_id);
    SH_SAND_GET_UINT32("PIPE", oam_oamid_inputs.pipe);

    /** Validate the inputs */
    SHR_IF_ERR_EXIT(diag_oam_oamid_verify_cmp(unit, &oam_oamid_inputs));

    /** Read OAM ID at the FLP at the ingress*/
    oam_oamid_results.core_id = oam_oamid_inputs.core_id;
    SHR_IF_ERR_EXIT(diag_oam_oamid_read_signals(unit, oam_oamid_inputs.pipe, &oam_oamid_results));

    /** Decode the input where we have oam_id */
    SHR_IF_ERR_EXIT(diag_oam_oamid_decode(unit, &oam_oamid_inputs));
    SHR_IF_ERR_EXIT(diag_oam_oamid_cmp_and_display(unit, &oam_oamid_inputs, &oam_oamid_results, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * oam lu command - Read Ingress/Egress OEM key signals
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_lu_params - parameters of the oam lu command
 *        command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_lu_read_signals(
    int unit,
    oam_lu_params_t * oam_lu_params)
{
    bcm_error_t rv;
    uint32 key[5];
    uint32 result[3];
    SHR_FUNC_INIT_VARS(unit);

    /** Read the values of the Ingress OEM key signals. */
    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_0", key, 5);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    oam_lu_params->ing_keys[0] = (key[0] & 0x1ffffff);

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_0.Key_Prefix",
                       &oam_lu_params->ing_key_prefix[0], 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_0.Key_Base",
                     &oam_lu_params->ing_key_base[0], 1));

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_1", key, 5);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    oam_lu_params->ing_keys[1] = (key[0] & 0x1ffffff);

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_1.Key_Prefix",
                       &oam_lu_params->ing_key_prefix[1], 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_1.Key_Base",
                     &oam_lu_params->ing_key_base[1], 1));

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_2", key, 5);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    oam_lu_params->ing_keys[2] = (key[0] & 0x1ffffff);

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_2.Key_Prefix",
                       &oam_lu_params->ing_key_prefix[2], 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_2.Key_Base",
                     &oam_lu_params->ing_key_base[2], 1));

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Key_3", key, 5);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    oam_lu_params->ing_keys[3] = (key[0] & 0x3ffffff);

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "Your_Disc_Valid",
                       &oam_lu_params->ing_your_disc_valid, 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "OAM_LIF", &oam_lu_params->ing_oam_lif, 1));
    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, oam_lu_params->core_id, "IRPP", "FWD2", "IOEM", "MDL", &oam_lu_params->ing_mdl, 1));

    /** Read the values of the Ingress OEM found signals. */
    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Hit_0", &oam_lu_params->ing_found[0], 1);

    if (rv != BCM_E_NONE)
    {
        oam_lu_params->ing_found[0] = 0;
        SHR_EXIT();
    }

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Hit_1", &oam_lu_params->ing_found[1], 1);

    if (rv != BCM_E_NONE)
    {
        oam_lu_params->ing_found[1] = 0;
        SHR_EXIT();
    }

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Hit_2", &oam_lu_params->ing_found[2], 1);

    if (rv != BCM_E_NONE)
    {
        oam_lu_params->ing_found[2] = 0;
        SHR_EXIT();
    }

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Hit_3", &oam_lu_params->ing_found[3], 1);

    if (rv != BCM_E_NONE)
    {
        oam_lu_params->ing_found[3] = 0;
        SHR_EXIT();
    }

    /*
     * Read the values of the IOEM-1 payload signals:
     * result[0..2] = {{32'b0}, {10'b Counter-Base(LSB), 2'b Counter-Intf, 20'b0},
     *                  {2'b0, 16'b MP-Type-Vec, 4'b MP-Profile, 10'b Counter-Base(MSB)}
     */
    if (oam_lu_params->ing_found[0])
    {
        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0", result, 3);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        oam_lu_params->ing_results[0][0] = (result[2] & 0x3fffffff);
        oam_lu_params->ing_results[0][1] = (result[1] >> 20);

        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.MDL_0_MP_Type",
                           &oam_lu_params->ing_mp_type[0], 1);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.MDL_1_MP_Type",
                         &oam_lu_params->ing_mp_type[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.MDL_2_MP_Type",
                         &oam_lu_params->ing_mp_type[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.MDL_3_MP_Type",
                         &oam_lu_params->ing_mp_type[3], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.MDL_4_MP_Type",
                         &oam_lu_params->ing_mp_type[4], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.MDL_5_MP_Type",
                         &oam_lu_params->ing_mp_type[5], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.MDL_6_MP_Type",
                         &oam_lu_params->ing_mp_type[6], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.MDL_7_MP_Type",
                         &oam_lu_params->ing_mp_type[7], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.MP_Profile",
                         &oam_lu_params->ing_mp_profile[0], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.Counter_Base",
                         &oam_lu_params->ing_counter_base[0], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_0.Counter_Interface",
                         &oam_lu_params->ing_counter_intf[0], 1));
    }

    if (oam_lu_params->ing_found[1])
    {
        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1", result, 3);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        oam_lu_params->ing_results[1][0] = (result[2] & 0x3fffffff);
        oam_lu_params->ing_results[1][1] = (result[1] >> 20);

        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.MDL_0_MP_Type",
                           &oam_lu_params->ing_mp_type[0], 1);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.MDL_1_MP_Type",
                         &oam_lu_params->ing_mp_type[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.MDL_2_MP_Type",
                         &oam_lu_params->ing_mp_type[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.MDL_3_MP_Type",
                         &oam_lu_params->ing_mp_type[3], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.MDL_4_MP_Type",
                         &oam_lu_params->ing_mp_type[4], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.MDL_5_MP_Type",
                         &oam_lu_params->ing_mp_type[5], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.MDL_6_MP_Type",
                         &oam_lu_params->ing_mp_type[6], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.MDL_7_MP_Type",
                         &oam_lu_params->ing_mp_type[7], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.MP_Profile",
                         &oam_lu_params->ing_mp_profile[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.Counter_Base",
                         &oam_lu_params->ing_counter_base[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_1.Counter_Interface",
                         &oam_lu_params->ing_counter_intf[1], 1));
    }

    if (oam_lu_params->ing_found[2])
    {
        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2", result, 3);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        oam_lu_params->ing_results[2][0] = (result[2] & 0x3fffffff);
        oam_lu_params->ing_results[2][1] = (result[1] >> 20);
        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.MDL_0_MP_Type",
                           &oam_lu_params->ing_mp_type[0], 1);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.MDL_1_MP_Type",
                         &oam_lu_params->ing_mp_type[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.MDL_2_MP_Type",
                         &oam_lu_params->ing_mp_type[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.MDL_3_MP_Type",
                         &oam_lu_params->ing_mp_type[3], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.MDL_4_MP_Type",
                         &oam_lu_params->ing_mp_type[4], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.MDL_5_MP_Type",
                         &oam_lu_params->ing_mp_type[5], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.MDL_6_MP_Type",
                         &oam_lu_params->ing_mp_type[6], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.MDL_7_MP_Type",
                         &oam_lu_params->ing_mp_type[7], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.MP_Profile",
                         &oam_lu_params->ing_mp_profile[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.Counter_Base",
                         &oam_lu_params->ing_counter_base[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_2.Counter_Interface",
                         &oam_lu_params->ing_counter_intf[2], 1));
    }

    /** Read the values of the IOEM-2 payload signals. */
    /** result[0..2] = {{32'b0}, {32'b0}, {2'b0, 16'b OAM-ID, 4'b MP-Profile, 10'b0}} */
    if (oam_lu_params->ing_found[3])
    {
        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Result_3", result, 3);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        oam_lu_params->ing_results[3][0] = ((result[2] >> 10) & 0xfffff);

        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "OAM_ID",
                           &oam_lu_params->ing_oam_id, 1);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "IRPP", "IOEM", "FWD2", "Acc_MP_Profile",
                         &oam_lu_params->ing_acc_mp_profile, 1));
    }

    /** Read the values of the Egress OEM key signals. */
    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_0", key, 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    oam_lu_params->egr_keys[0] = (key[0] & 0x7fffff);

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_0.Key_Prefix",
                       &oam_lu_params->egr_key_prefix[0], 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_0.Key_Base",
                     &oam_lu_params->egr_key_base[0], 1));

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_1", key, 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    oam_lu_params->egr_keys[1] = (key[0] & 0x7fffff);

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_1.Key_Prefix",
                       &oam_lu_params->egr_key_prefix[1], 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_1.Key_Base",
                     &oam_lu_params->egr_key_base[1], 1));

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_2", key, 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    oam_lu_params->egr_keys[2] = (key[0] & 0x7fffff);

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_2.Key_Prefix",
                       &oam_lu_params->egr_key_prefix[2], 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_2.Key_Base",
                     &oam_lu_params->egr_key_base[2], 1));

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "Key_3", key, 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    oam_lu_params->egr_keys[3] = (key[0] & 0x7fffff);

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "OAM_LIF", &oam_lu_params->egr_oam_lif, 1);

    if (rv != BCM_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dpp_dsig_read
                    (unit, oam_lu_params->core_id, "ETPP", "Term", "EOEM", "MDL", &oam_lu_params->egr_mdl, 1));

    /** Read the values of the Egress OEM found signals. */
    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Found_0",
                       &oam_lu_params->egr_found[0], 1);

    if (rv != BCM_E_NONE)
    {
        oam_lu_params->egr_found[0] = 0;
        SHR_EXIT();
    }

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Found_1",
                       &oam_lu_params->egr_found[1], 1);

    if (rv != BCM_E_NONE)
    {
        oam_lu_params->egr_found[1] = 0;
        SHR_EXIT();
    }

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Found_2",
                       &oam_lu_params->egr_found[2], 1);

    if (rv != BCM_E_NONE)
    {
        oam_lu_params->egr_found[2] = 0;
        SHR_EXIT();
    }

    rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Found_3",
                       &oam_lu_params->egr_found[3], 1);

    if (rv != BCM_E_NONE)
    {
        oam_lu_params->egr_found[3] = 0;
        SHR_EXIT();
    }

    /*
     * Read the values of the EOEM-1 payload signals:
     * result[0..2] = {{2'b MP-Type-Vec(LSB), 4'b MP-Profile, 20'b Counter-Base, 2'b Counter-Intf, 4'b0},
     *                  {18'b0, 14'b MP-Type-Vec(MSB)}, {32'b0}}
     */
    if (oam_lu_params->egr_found[0])
    {
        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0", result, 2);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        oam_lu_params->egr_results[0][0] = (result[1] & 0x3fff);
        oam_lu_params->egr_results[0][1] = (result[0] >> 4);

        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.MDL_0_MP_Type",
                           &oam_lu_params->egr_mp_type[0], 1);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.MDL_1_MP_Type",
                         &oam_lu_params->egr_mp_type[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.MDL_2_MP_Type",
                         &oam_lu_params->egr_mp_type[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.MDL_3_MP_Type",
                         &oam_lu_params->egr_mp_type[3], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.MDL_4_MP_Type",
                         &oam_lu_params->egr_mp_type[4], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.MDL_5_MP_Type",
                         &oam_lu_params->egr_mp_type[5], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.MDL_6_MP_Type",
                         &oam_lu_params->egr_mp_type[6], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.MDL_7_MP_Type",
                         &oam_lu_params->egr_mp_type[7], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.MP_Profile",
                         &oam_lu_params->egr_mp_profile[0], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.Counter_Base",
                         &oam_lu_params->egr_counter_base[0], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_0.Counter_Interface",
                         &oam_lu_params->egr_counter_intf[0], 1));
    }

    if (oam_lu_params->egr_found[1])
    {
        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1", result, 2);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        oam_lu_params->egr_results[1][0] = (result[1] & 0x3fff);
        oam_lu_params->egr_results[1][1] = (result[0] >> 4);

        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.MDL_0_MP_Type",
                           &oam_lu_params->egr_mp_type[0], 1);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.MDL_1_MP_Type",
                         &oam_lu_params->egr_mp_type[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.MDL_2_MP_Type",
                         &oam_lu_params->egr_mp_type[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.MDL_3_MP_Type",
                         &oam_lu_params->egr_mp_type[3], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.MDL_4_MP_Type",
                         &oam_lu_params->egr_mp_type[4], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.MDL_5_MP_Type",
                         &oam_lu_params->egr_mp_type[5], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.MDL_6_MP_Type",
                         &oam_lu_params->egr_mp_type[6], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.MDL_7_MP_Type",
                         &oam_lu_params->egr_mp_type[7], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.MP_Profile",
                         &oam_lu_params->egr_mp_profile[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.Counter_Base",
                         &oam_lu_params->egr_counter_base[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_1.Counter_Interface",
                         &oam_lu_params->egr_counter_intf[1], 1));
    }

    if (oam_lu_params->egr_found[2])
    {
        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2", result, 2);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        oam_lu_params->egr_results[2][0] = (result[1] & 0x3fff);
        oam_lu_params->egr_results[2][1] = (result[0] >> 4);

        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.MDL_0_MP_Type",
                           &oam_lu_params->egr_mp_type[0], 1);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.MDL_1_MP_Type",
                         &oam_lu_params->egr_mp_type[1], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.MDL_2_MP_Type",
                         &oam_lu_params->egr_mp_type[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.MDL_3_MP_Type",
                         &oam_lu_params->egr_mp_type[3], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.MDL_4_MP_Type",
                         &oam_lu_params->egr_mp_type[4], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.MDL_5_MP_Type",
                         &oam_lu_params->egr_mp_type[5], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.MDL_6_MP_Type",
                         &oam_lu_params->egr_mp_type[6], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.MDL_7_MP_Type",
                         &oam_lu_params->egr_mp_type[7], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.MP_Profile",
                         &oam_lu_params->egr_mp_profile[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.Counter_Base",
                         &oam_lu_params->egr_counter_base[2], 1));
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_2.Counter_Interface",
                         &oam_lu_params->egr_counter_intf[2], 1));
    }

    /** Read the values of the EOEM-2 payload signals. */
    /** result[0..2] = {{2'b OAM-ID(LSB), 4'b MP-Profile, 26'b0}, {18'b0, 14'b OAM-ID}, {32'b0}} */
    if (oam_lu_params->egr_found[3])
    {
        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Payload_3", result, 2);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        oam_lu_params->egr_results[3][0] = (result[1] & 0x3fff);
        oam_lu_params->egr_results[3][1] = (result[0] >> 26);

        rv = dpp_dsig_read(unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "OAM_ID",
                           &oam_lu_params->egr_oam_id, 1);

        if (rv != BCM_E_NONE)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, oam_lu_params->core_id, "ETPP", "EOEM", "Term", "Acc_MP_Profile",
                         &oam_lu_params->egr_acc_mp_profile, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * oam lu command - display function
 * Displays the last lookup results to Ingress/Egress OAM LIF DB
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_lu_params - values to be displayed
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_lu_display(
    int unit,
    oam_lu_params_t * oam_lu_params,
    sh_sand_control_t * sand_control)
{
    int index;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("OAM Last Lookup Statistics");
    PRT_COLUMN_ADD("Database");
    PRT_COLUMN_ADD("Key");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Payload");
    PRT_COLUMN_ADD("Value");

    for (index = 0; index < MAX_OAM_LIF_DB_KEYS; index++)
    {
        /*
         * Display Ingress OAM LIF DB last lookup statistics
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "IOEM-1");

        PRT_CELL_SET("%s", "Full");
        PRT_CELL_SET("0x%x", (oam_lu_params->ing_keys[index]));

        PRT_CELL_SET("%s", "Full");

        if (oam_lu_params->ing_found[index])
        {
            PRT_CELL_SET("0x%x%x", (oam_lu_params->ing_results[index][0]), (oam_lu_params->ing_results[index][1]));
        }
        else
        {
            PRT_CELL_SET("%s", "Not found");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);

        PRT_CELL_SET("%s", "Prefix");
        PRT_CELL_SET("0x%x", oam_lu_params->ing_key_prefix[index]);

        if (oam_lu_params->ing_found[index])
        {
            PRT_CELL_SET("%s", "MP Profile");
            PRT_CELL_SET("0x%x", oam_lu_params->ing_mp_profile[index]);
        }
        else
        {
            PRT_CELL_SKIP(2);
        }

        if (oam_lu_params->ing_found[index])
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        }

        PRT_CELL_SKIP(1);

        PRT_CELL_SET("%s", "Base");
        PRT_CELL_SET("0x%x", oam_lu_params->ing_key_base[index]);

        if (oam_lu_params->ing_found[index])
        {
            PRT_CELL_SET("%s", "Counter Base");
            PRT_CELL_SET("0x%x", oam_lu_params->ing_counter_base[index]);
        }
        else
        {
            PRT_CELL_SKIP(2);
        }

        if (oam_lu_params->ing_found[index])
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SKIP(3);

            PRT_CELL_SET("%s", "Counter Interface");
            PRT_CELL_SET("0x%x", oam_lu_params->ing_counter_intf[index]);
        }
    }

    /*
     * Display Ingress ACC MEP DB last lookup statistics
     */
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%s", "IOEM-2");

    PRT_CELL_SET("%s", "Full");
    PRT_CELL_SET("0x%x", (oam_lu_params->ing_keys[index]));

    PRT_CELL_SET("%s", "Full");

    if (oam_lu_params->ing_found[index])
    {
        PRT_CELL_SET("0x%x", oam_lu_params->ing_results[index][0]);
    }
    else
    {
        PRT_CELL_SET("%s", "Not found");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);

    PRT_CELL_SET("%s", "Your Disc Valid");
    PRT_CELL_SET("%d", oam_lu_params->ing_your_disc_valid);

    if (oam_lu_params->ing_found[index])
    {
        PRT_CELL_SET("%s", "OAM ID");
        PRT_CELL_SET("0x%x", oam_lu_params->ing_oam_id);
    }
    else
    {
        PRT_CELL_SKIP(2);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);

    PRT_CELL_SET("%s", "OAM LIF");
    PRT_CELL_SET("0x%x", oam_lu_params->ing_oam_lif);

    if (oam_lu_params->ing_found[index])
    {
        PRT_CELL_SET("%s", "Acc MP Profile");
        PRT_CELL_SET("0x%x", oam_lu_params->ing_acc_mp_profile);
    }
    else
    {
        PRT_CELL_SKIP(2);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SKIP(1);

    PRT_CELL_SET("%s", "MDL");
    PRT_CELL_SET("%d", oam_lu_params->ing_mdl);

    PRT_CELL_SKIP(2);

    for (index = 0; index < MAX_OEM_KEYS - 1; index++)
    {
        /*
         * Display Egress OAM LIF DB last lookup statistics
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "EOEM-1");

        PRT_CELL_SET("%s", "Full");
        PRT_CELL_SET("0x%x", (oam_lu_params->egr_keys[index]));

        PRT_CELL_SET("%s", "Full");

        if (oam_lu_params->egr_found[index])
        {
            PRT_CELL_SET("0x%x%x", oam_lu_params->egr_results[index][0], oam_lu_params->egr_results[index][1]);
        }
        else
        {
            PRT_CELL_SET("%s", "Not found");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(1);

        PRT_CELL_SET("%s", "Prefix");
        PRT_CELL_SET("0x%x", oam_lu_params->egr_key_prefix[index]);

        if (oam_lu_params->egr_found[index])
        {
            PRT_CELL_SET("%s", "MP Profile");
            PRT_CELL_SET("0x%x", oam_lu_params->egr_mp_profile[index]);
        }
        else
        {
            PRT_CELL_SKIP(2);
        }

        if (oam_lu_params->egr_found[index])
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        }

        PRT_CELL_SKIP(1);

        PRT_CELL_SET("%s", "Base");
        PRT_CELL_SET("0x%x", oam_lu_params->egr_key_base[index]);

        if (oam_lu_params->egr_found[index])
        {
            PRT_CELL_SET("%s", "Counter Base");
            PRT_CELL_SET("0x%x", oam_lu_params->egr_counter_base[index]);
        }
        else
        {
            PRT_CELL_SKIP(2);
        }

        if (oam_lu_params->egr_found[index])
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SKIP(3);

            PRT_CELL_SET("%s", "Counter Interface");
            PRT_CELL_SET("0x%x", oam_lu_params->egr_counter_intf[index]);
        }
    }

    /*
     * Display Egress ACC MEP DB last lookup statistics
     */
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%s", "EOEM-2");

    PRT_CELL_SET("%s", "Full");
    PRT_CELL_SET("0x%x", (oam_lu_params->egr_keys[index]));

    PRT_CELL_SET("%s", "Full");

    if (oam_lu_params->egr_found[index])
    {
        PRT_CELL_SET("0x%x%x", oam_lu_params->egr_results[index][0], oam_lu_params->egr_results[index][1]);
    }
    else
    {
        PRT_CELL_SET("%s", "Not found");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SKIP(1);

    PRT_CELL_SET("%s", "OAM LIF");
    PRT_CELL_SET("0x%x", oam_lu_params->egr_oam_lif);

    if (oam_lu_params->egr_found[index])
    {
        PRT_CELL_SET("%s", "OAM ID");
        PRT_CELL_SET("0x%x", oam_lu_params->egr_oam_id);
    }
    else
    {
        PRT_CELL_SKIP(2);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SKIP(1);

    PRT_CELL_SET("%s", "MDL");
    PRT_CELL_SET("%d", oam_lu_params->egr_mdl);

    if (oam_lu_params->egr_found[index])
    {
        PRT_CELL_SET("%s", "Acc MP Profile");
        PRT_CELL_SET("0x%x", oam_lu_params->egr_acc_mp_profile);
    }
    else
    {
        PRT_CELL_SKIP(2);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * oam lu command - display function
 * Displays the last lookup results MP Type vectors
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_lu_params - values to be displayed
 * \param [in] sand_control - pointer to framework control structure
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_lu_mp_type_display(
    int unit,
    oam_lu_params_t * oam_lu_params,
    sh_sand_control_t * sand_control)
{
    int level;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("OAM Last Lookup Statistics - MP Type");
    PRT_COLUMN_ADD("Pipe");
    PRT_COLUMN_ADD("MDL_MP_TYPE_0");
    PRT_COLUMN_ADD("MDL_MP_TYPE_1");
    PRT_COLUMN_ADD("MDL_MP_TYPE_2");
    PRT_COLUMN_ADD("MDL_MP_TYPE_3");
    PRT_COLUMN_ADD("MDL_MP_TYPE_4");
    PRT_COLUMN_ADD("MDL_MP_TYPE_5");
    PRT_COLUMN_ADD("MDL_MP_TYPE_6");
    PRT_COLUMN_ADD("MDL_MP_TYPE_7");

    /*
     * Display Ingress MP Type Vector
     */
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%s", "INGRESS");

    for (level = 0; level < DNX_OAM_MAX_MDL + 1; level++)
    {
        switch (oam_lu_params->ing_mp_type[level])
        {
            case DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP:
            {
                PRT_CELL_SET("%s", "NO MEP");
                break;
            }
            case DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP:
            {
                PRT_CELL_SET("%s", "MIP");
                break;
            }
            case DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP:
            {
                PRT_CELL_SET("%s", "ACTIVE MEP");
                break;
            }
            case DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP:
            {
                PRT_CELL_SET("%s", "PASSIVE MEP");
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown MP Type %d\n", oam_lu_params->ing_mp_type[level]);
                break;
            }
        }
    }

    /*
     * Display Egress MP Type Vector
     */
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%s", "EGRESS");

    for (level = 0; level < DNX_OAM_MAX_MDL + 1; level++)
    {
        switch (oam_lu_params->egr_mp_type[level])
        {
            case DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP:
            {
                PRT_CELL_SET("%s", "NO MEP");
                break;
            }
            case DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP:
            {
                PRT_CELL_SET("%s", "MIP");
                break;
            }
            case DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP:
            {
                PRT_CELL_SET("%s", "ACTIVE MEP");
                break;
            }
            case DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP:
            {
                PRT_CELL_SET("%s", "PASSIVE MEP");
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown MP Type %d\n", oam_lu_params->egr_mp_type[level]);
                break;
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * oam lu command - Input verification
 * \param [in] unit -     Relevant unit.
 * \param [out] oam_lu_params - input of oam lu command
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_lu_verify(
    int unit,
    oam_lu_params_t * oam_lu_params)
{
    int num_of_cores;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Verify core_id
     *  Get number of cores for this device
     */
    num_of_cores = dnx_data_device.general.nof_cores_get(unit);

    if (oam_lu_params->core_id > num_of_cores)
    {
        /*
         * Default core_id
         */
        oam_lu_params->core_id = 0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * oam lu command - Initialize statistics variables
 * \param [in] unit -     Relevant unit.
 * \param [in] *oam_lu_params - parameters of the oam lu command
 *        command.
 *
 * \retval Error indication
 */
static shr_error_e
diag_oam_lu_init(
    int unit,
    oam_lu_params_t * oam_lu_params)
{
    int index;
    int level;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init statistics
     */
    oam_lu_params->ing_oam_lif = 0;
    oam_lu_params->ing_mdl = 0;
    oam_lu_params->ing_your_disc_valid = 0;
    oam_lu_params->ing_oam_id = 0;
    oam_lu_params->ing_acc_mp_profile = 0;

    oam_lu_params->egr_oam_lif = 0;
    oam_lu_params->egr_mdl = 0;
    oam_lu_params->egr_oam_id = 0;
    oam_lu_params->egr_acc_mp_profile = 0;

    for (level = 0; level < DNX_OAM_MAX_MDL + 1; level++)
    {
        oam_lu_params->ing_mp_type[level] = 0;
        oam_lu_params->egr_mp_type[level] = 0;
    }

    for (index = 0; index < MAX_OEM_KEYS; index++)
    {
        oam_lu_params->ing_keys[index] = 0;
        oam_lu_params->ing_found[index] = 0;
        oam_lu_params->ing_results[index][0] = 0;
        oam_lu_params->ing_results[index][1] = 0;

        oam_lu_params->egr_keys[index] = 0;
        oam_lu_params->egr_found[index] = 0;
        oam_lu_params->egr_results[index][0] = 0;
        oam_lu_params->egr_results[index][1] = 0;

        if (index < MAX_OAM_LIF_DB_KEYS)
        {
            oam_lu_params->ing_key_prefix[index] = 0;
            oam_lu_params->ing_key_base[index] = 0;
            oam_lu_params->ing_mp_profile[index] = 0;
            oam_lu_params->ing_counter_base[index] = 0;
            oam_lu_params->ing_counter_intf[index] = 0;

            oam_lu_params->egr_key_prefix[index] = 0;
            oam_lu_params->egr_key_base[index] = 0;
            oam_lu_params->egr_mp_profile[index] = 0;
            oam_lu_params->egr_counter_base[index] = 0;
            oam_lu_params->egr_counter_intf[index] = 0;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * oam lu command - main function
 * Calls get_results and display functions
 * \param [in] unit - Relevant unit
 * \param [in] args - Arguments from command line
 * \param [in] sand_control -
 *
 * \retval Error indication
 */
static shr_error_e
sh_dnx_oam_lu_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    oam_lu_params_t oam_lu_params;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_INT32("core", oam_lu_params.core_id);

    /** Validate the inputs */
    SHR_IF_ERR_EXIT(diag_oam_lu_verify(unit, &oam_lu_params));

    /** Initialize statistics variables */
    SHR_IF_ERR_EXIT(diag_oam_lu_init(unit, &oam_lu_params));

    /*
     *  Get requested information
     */
    SHR_IF_ERR_EXIT(diag_oam_lu_read_signals(unit, &oam_lu_params));

    /*
     *  Display requested information
     */
    SHR_IF_ERR_EXIT(diag_oam_lu_display(unit, &oam_lu_params, sand_control));

    SHR_IF_ERR_EXIT(diag_oam_lu_mp_type_display(unit, &oam_lu_params, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief DNX OAM CLassifier diagnostic manual
 * This is the manual for "oam"
 */
sh_sand_man_t sh_dnx_oam_man = {
    .brief = "OAM diagnostic commands"
};

static bcm_vlan_port_t vp1,vp2,vp3;

/**
 * \brief DNX OAM CLassifier diagnostic init function
 */
shr_error_e init_oam(int unit)
{
   bcm_oam_endpoint_info_t ep;
   bcm_oam_group_info_t group_info;
   bcm_oam_profile_t enc_ing_profile_id;
   bcm_oam_profile_t enc_eg_profile_id;
   int port1;
   int port2;
   int port3;
   uint32 flags;

   SHR_FUNC_INIT_VARS(unit);

   port1 = 200;
   port2 = 201;
   port3 = 202;

   bcm_vlan_port_t_init(&vp1);
   vp1.flags = 0;
   vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
   vp1.port = port1;
   vp1.match_vlan = 10;
   SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit,&vp1));

   bcm_vlan_port_t_init(&vp2);
   vp2.flags = 0;
   vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
   vp2.port = port2;
   vp2.match_vlan = 12;
   SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit,&vp2));

   bcm_vlan_port_t_init(&vp3);
   vp3.flags = 0;
   vp3.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
   vp3.port = port3;
   vp3.match_vlan = 14;
   SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit,&vp3));

   /*
    * Create MEG
    */
   bcm_oam_group_info_t_init(&group_info);

   group_info.id = 7;
   group_info.flags = BCM_OAM_GROUP_WITH_ID;
   SHR_IF_ERR_EXIT(bcm_oam_group_create(unit, &group_info));

   flags = 0;
   bcm_oam_profile_id_get_by_type(unit,1,bcmOAMProfileIngressLIF,&flags,&enc_ing_profile_id);
   bcm_oam_profile_id_get_by_type(unit,1,bcmOAMProfileEgressLIF,&flags,&enc_eg_profile_id);

   SHR_IF_ERR_EXIT(bcm_oam_lif_profile_set(unit, 0, vp1.vlan_port_id,enc_ing_profile_id,enc_eg_profile_id));
   SHR_IF_ERR_EXIT(bcm_oam_lif_profile_set(unit, 0, vp2.vlan_port_id,enc_ing_profile_id,enc_eg_profile_id));
   SHR_IF_ERR_EXIT(bcm_oam_lif_profile_set(unit, 0, vp3.vlan_port_id,enc_ing_profile_id,enc_eg_profile_id));

   bcm_oam_endpoint_info_t_init(&ep);
   ep.gport = vp1.vlan_port_id;
   ep.type = bcmOAMEndpointTypeEthernet;
   ep.level = 3;
   ep.group = group_info.id;
   ep.lm_counter_base_id = 6;
   ep.lm_counter_if = 0;

   SHR_IF_ERR_EXIT(bcm_oam_endpoint_create(unit, &ep));

   bcm_oam_endpoint_info_t_init(&ep);
   ep.gport = vp2.vlan_port_id;
   ep.type = bcmOAMEndpointTypeEthernet;
   ep.level = 4;
   ep.group = group_info.id;
   ep.lm_counter_base_id = 23;
   ep.lm_counter_if = 1;

   SHR_IF_ERR_EXIT(bcm_oam_endpoint_create(unit, &ep));

   bcm_oam_endpoint_info_t_init(&ep);
   ep.gport = vp3.vlan_port_id;
   ep.type = bcmOAMEndpointTypeEthernet;
   ep.level = 5;
   ep.group = group_info.id;
   ep.lm_counter_base_id = 14;
   ep.lm_counter_if = 2;

   SHR_IF_ERR_EXIT(bcm_oam_endpoint_create(unit, &ep));

   SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief DNX OAM CLassifier diagnostic deinit function
 */
shr_error_e deinit_oam(int unit)
{
   SHR_FUNC_INIT_VARS(unit);

   SHR_IF_ERR_EXIT(bcm_oam_endpoint_destroy_all(unit,7));
   SHR_IF_ERR_EXIT(bcm_oam_group_destroy_all(unit));

   SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit,vp1.vlan_port_id));
   SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit,vp2.vlan_port_id));
   SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit,vp3.vlan_port_id));

   SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief DNX OAM BFDEndPoint diagnostic init function
 */
shr_error_e init_bfd(int unit)
{

    int port2 = 201;
    int out_vlan = 10;
    int in_vlan = 10;
    int vrf = 1;
    bcm_if_t l3egid_null;
    bcm_mac_t mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
    bcm_mac_t next_hop_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
    bcm_bfd_endpoint_info_t bfd_endpoint_info;

    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_egress_t l3eg;
    SHR_FUNC_INIT_VARS(unit);

    /** L2 configuration */
    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, in_vlan));
    /** L3 configuration */
    bcm_l3_intf_t_init(&l3if);
    bcm_l3_ingress_t_init(&l3_ing_if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    l3if.l3a_vid = in_vlan;
    l3if.l3a_ttl = 31;
    l3if.l3a_mtu = 0;
    l3if.l3a_mtu_forwarding = 0;
    l3if.l3a_intf_id = in_vlan;     /* In DNX Arch VSI always equal RIF */
    sal_memcpy(l3if.l3a_mac_addr, mac_address, 6);
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3if));

    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;   /* must, as we update exist RIF */
    l3_ing_if.vrf = vrf;
    l3_ing_if.urpf_mode = bcmL3IngressUrpfDisable;
    SHR_IF_ERR_EXIT(bcm_l3_ingress_create(unit, &l3_ing_if, &l3if.l3a_intf_id));

    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, next_hop_mac, 6);
    l3eg.vlan = out_vlan;
    if (BCM_L3_ITF_TYPE_IS_LIF(l3if.l3a_intf_id))
    {
        l3eg.intf = l3if.l3a_intf_id;
    }
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null));

    /** BFD configuration */
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);
    bfd_endpoint_info.type = bcmBFDTunnelTypeUdp;
    bfd_endpoint_info.flags = BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP;
    bfd_endpoint_info.src_ip_addr = 0x01020364;
    bfd_endpoint_info.dst_ip_addr = 0x04050617;
    BCM_GPORT_SYSTEM_PORT_ID_SET(bfd_endpoint_info.tx_gport, port2);
    bfd_endpoint_info.loc_clear_threshold = 1;
    bfd_endpoint_info.ip_ttl = 255;
    bfd_endpoint_info.ip_tos = 255;
    bfd_endpoint_info.udp_src_port = 0xC001;
    bfd_endpoint_info.int_pri = 1;
    bfd_endpoint_info.egress_if = l3eg.encap_id;
    bfd_endpoint_info.local_discr = 32772;
    bfd_endpoint_info.remote_discr = 32776;
    bfd_endpoint_info.local_min_tx = 10000;
    bfd_endpoint_info.local_min_rx = 10000;
    bfd_endpoint_info.local_detect_mult = 30;
    bfd_endpoint_info.remote_detect_mult = 30;
    bfd_endpoint_info.loc_clear_threshold = 1;
    bfd_endpoint_info.remote_state = 2;
    bfd_endpoint_info.local_state = 2;
    bfd_endpoint_info.local_diag = 2;
    bfd_endpoint_info.remote_diag = 2;

    bfd_endpoint_info.remote_gport = BCM_GPORT_INVALID;
    SHR_IF_ERR_EXIT(bcm_bfd_endpoint_create(unit, &bfd_endpoint_info));

    bfd_endpoint_info.local_discr = 4;
    SHR_IF_ERR_EXIT(bcm_bfd_endpoint_create(unit, &bfd_endpoint_info));

   SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief DNX OAM BFDEndPoint diagnostic deinit function
 */
shr_error_e deinit_bfd(int unit)
{
   SHR_FUNC_INIT_VARS(unit);

   SHR_IF_ERR_EXIT(bcm_bfd_endpoint_destroy_all(unit));
   SHR_IF_ERR_EXIT(bcm_vlan_destroy_all(unit));
   SHR_IF_ERR_EXIT(bcm_l3_intf_delete_all(unit));

   SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief DNX OAM CLassifier diagnostic manual
 * This is the manual for "diag oam cl"
 */
static sh_sand_man_t sh_dnx_oam_cf_man =
{
    .brief  = "OAM Clasifier Diagnostic",
    .full   = "Presents OAM classifier simulation and display outcomes for a given set of inputs.\n \
                * lif is mandatory input.\n \
                * All other inputs are optional. If optional input was not supplied, default value will be used.\n \
                * Keys of HW tables that result in a hit and its result fields will be colored.\n \
                * Counters that are enabled will be colored.",
    .examples = "lif=0x1000\n"
                "last=0 lif=0x1000 lif1=0x1000 lif2=0x1001 lif3=0x1002 opcode=1 mdl=3 ing=0 mymac=1 inject=0 da_is_mc=0 ydv=0 bfd=0 color=0\n",
    .init_cb = init_oam,
    .deinit_cb = deinit_oam
};

/**
 * \brief DNX OAM ClassiFier options
 * List of the supported options for oam cf command
 */
static sh_sand_option_t dnx_oam_cf_list_options[] = {
    /**name        type                  desc                                               default*/
    {"LAST",     SAL_FIELD_TYPE_UINT32, "Take Input from Last Run",                         "0"},
    {"lif",      SAL_FIELD_TYPE_UINT32, "OAM-LIF - Global for egress, local for ingress",   "-1"},
    {"lif1",     SAL_FIELD_TYPE_UINT32, "OAM-LIF-1 - Global for egress, local for ingress", "-1"},
    {"lif2",     SAL_FIELD_TYPE_UINT32, "OAM-LIF-2 - Global for egress, local for ingress", "-1"},
    {"lif3",     SAL_FIELD_TYPE_UINT32, "OAM-LIF-3 - Global for egress, local for ingress", "-1"},
    {"OPcode",   SAL_FIELD_TYPE_UINT32, "oam-opcode of the incoming packet",                "1"},
    {"mdl",      SAL_FIELD_TYPE_UINT32, "oam level of the incoming packet",                 "0"},
    {"mymac",    SAL_FIELD_TYPE_UINT32, "Set if packet's dst equals mep's mac",             "1"},
    {"INJect",   SAL_FIELD_TYPE_UINT32, "Set for injected packet",                          "0"},
    {"da_is_mc", SAL_FIELD_TYPE_UINT32, "Set if multicast",                                 "0"},
    {"ydv",      SAL_FIELD_TYPE_UINT32, "Your Disc is Valid",                               "0"},
    {"INGress",  SAL_FIELD_TYPE_UINT32, "Set for ingress classifier",                       "1"},
    {"BFD",      SAL_FIELD_TYPE_UINT32, "BFD packet indication",                            "0"},
    {"CoLoR",    SAL_FIELD_TYPE_UINT32, "Display in color.(0 for no-color)",                "1"},
    {NULL}
};

/**
 * \brief DNX OAM ID diagnostic manual
 */
static sh_sand_man_t sh_dnx_oam_oamid_man =
{
    .brief  = "OAM_ID Diagnostic",
    .full   = "OAM_ID signals contatins one of two indictations: \n \
              *In case there is a match in the OEM-2: the actual endpoint ID (OAM-ID) in the OAMP  \n \
              *Otherwise: Debug signals from the OAM-classifier.",
};

/**
 * \brief DNX OAM LU diagnostic manual
 */
static sh_sand_man_t sh_dnx_oam_lu_man =
{
    .brief  = "OAM Last LookUp Diagnostic",
};

/** Supported endpoint types   */
static sh_sand_enum_t dnx_oam_pipeline_type_enum_table[] = {
    {"INGress", PIPE_TYPE_INGRESS , "Ingress Pipe"},
    {"EGRess", PIPE_TYPE_EGRESS , "Egress Pipe"},
    {"BOTH", PIPE_TYPE_BOTH , "Both sides"},
    {NULL}
};

/**
 * \brief DNX OAM ID options
 * List of the supported options for oam_id get command
 */
static sh_sand_option_t dnx_oam_oamid_get_list_options[] = {
    /**name        type                  desc                                               default*/
    {"PIPE",     SAL_FIELD_TYPE_UINT32, "Present selected side",   "BOTH", (void *) dnx_oam_pipeline_type_enum_table},
    {NULL}
};

/**
 * \brief DNX OAM ID options
 * List of the supported options for oam_id dcd command
 */
static sh_sand_option_t dnx_oam_oamid_dcd_list_options[] = {
    /**name        type                  desc                      default*/
    {"ID",      SAL_FIELD_TYPE_UINT32, "ID to be decoded", NULL},

    {NULL}
};

/**
 * \brief DNX OAM ClassiFier options
 * List of the supported options for oam_id cmp command
 */
static sh_sand_option_t dnx_oam_oamid_cmp_list_options[] = {
    /**name        type                  desc                                               default*/
    {"ID",      SAL_FIELD_TYPE_UINT32,  "ID which will be compared",    "-1"},
    {"PIPE",    SAL_FIELD_TYPE_UINT32,  "Present selected side",    "-1", (void *) dnx_oam_pipeline_type_enum_table},
    {NULL}
};

/**
 * \brief DNX OAM BFDEndPoint diagnostic manual
 * This is the manual for "oam oamqf"
 */
static sh_sand_man_t sh_dnx_oam_oamid_get_man =
{
    .brief  = "OAM OAM_ID GET Diagnostic",
    .full   = "Get and present the OAM_ID of the last packet at the Ingress or Egress.\n",
    .examples = "\n"
                "PIPE=ingress CORE=0"
};
/**
 * \brief DNX OAM ID diagnostic manual
 * This is the manual for "oam oamqf"
 */
static sh_sand_man_t sh_dnx_oam_oamid_dcd_man =
{
    .brief  = "OAM OAM_ID DeCoDe Diagnostic",
    .full   = "Parse given OAM_ID\n",
    .examples = "ID=0x11388"
};
/**
 * \brief DNX OAM ID diagnostic manual
 * This is the manual for "oam oamqf"
 */
static sh_sand_man_t sh_dnx_oam_oamid_cmp_man =
{
    .brief  = "OAM OAM_ID CoMPare Diagnostic",
    .full   = "Parse given OAM_ID and compare with actual value at Ingress or Egress pipe\n",
    .examples = "PIPE=ingress CORE=0 id=0x11388"

};

/**
 * \brief DNX OAM_ID diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for OAM ID diagnostic commands
 */
sh_sand_cmd_t sh_dnx_oam_oamid_cmds[] =
{
    /**keyword,        action,           command, options, man */
#ifdef ADAPTER_SERVER_MODE
    {"Get",    sh_dnx_oam_oamid_cmd_get, NULL, dnx_oam_oamid_get_list_options,  &sh_dnx_oam_oamid_get_man,  NULL, NULL, SH_CMD_NO_XML_VERIFY},
#else
    {"Get",    sh_dnx_oam_oamid_cmd_get, NULL, dnx_oam_oamid_get_list_options,  &sh_dnx_oam_oamid_get_man,  NULL, NULL},
#endif
    {"DeCoDe", sh_dnx_oam_oamid_cmd_dcd, NULL, dnx_oam_oamid_dcd_list_options,  &sh_dnx_oam_oamid_dcd_man,  NULL, NULL},
    {"CoMPare",sh_dnx_oam_oamid_cmd_cmp, NULL, dnx_oam_oamid_cmp_list_options,  &sh_dnx_oam_oamid_cmp_man,  NULL, NULL},
    {NULL}
};


/**
 * \brief DNX OAM CouNTeRS diagnostic manual
 * This is the manual for "oam cntrs"
 */
static sh_sand_man_t sh_dnx_oam_oamep_man =
{
    .brief  = "OAM EndPoint Diagnostic",
    .full   = "Presents Configured OAM Endpoints and their properties.\n \
                * When ID is supplied, specific endpoint will be presented.\n \
                * When used without specific ID, all configured endpoints will be presented.\n",
    .examples = "\n"
                "ID=3"
};

/**
 * \brief DNX OAM CounteRs diagnostic manual
 * This is the manual for "oam cntrs"
 */
static sh_sand_man_t sh_dnx_oam_cntrs_man =
{
    .brief  = "OAM EndPoint LM Counters Diagnostic",
    .full   = "Presents LM Counters and values of configured OAM Endpoints.\n \
                * When ID is supplied, specific endpoint's counters will be presented.\n \
                * When used without specific ID, counters of configured endpoints will be presented.\n",
    .examples = "\n"
                "ID=3"
};

/**
 * \brief DNX OAM GRouP diagnostic manual
 * This is the manual for "oam grp"
 */
static sh_sand_man_t sh_dnx_oam_grp_man =
{
    .brief  = "OAM GRouP Diagnostic",
    .full   = "Presents Configured OAM Groups, their properties, and  endpoints allocated on that group.\n \
                * When ID is supplied, specific group will be presented.\n \
                * When used without specific ID, all configured groups will be presented.\n",
    .examples = "ID=7\n",
    .init_cb = init_oam,
    .deinit_cb = deinit_oam
};

/**
 * \brief DNX OAM BFDEndPoint diagnostic manual
 * This is the manual for "oam bfdep"
 */
static sh_sand_man_t sh_dnx_oam_bfdep_man =
{
    .brief  = "OAM BFDEndPoint Diagnostic",
    .full   = "Presents Configured OAM BFD Endpoints and their properties\n \
                * When ID is supplied, specific Endpoint will be presented.\n \
                * When used without specific ID, all configured BFD endpoints will be presented.\n",
    .examples = "ID=4\n",
    .init_cb = init_bfd,
    .deinit_cb = deinit_bfd

};

/**
 * \brief DNX OAM OAMEndPoint options
 * List of the supported options for oam cf command
 */
static sh_sand_option_t dnx_oam_oamep_list_options[] = {
    /**name        type                  desc                                               default*/
    {"ID",      SAL_FIELD_TYPE_INT32, "ID of the requested OAM endpoint",   "-1"},
    {NULL}
};

/**
 * \brief DNX OAM CounteRs options
 * List of the supported options for oam CounteRs command
 */
static sh_sand_option_t dnx_oam_cntrs_list_options[] = {
    /**name        type                  desc                                      default*/
    {"ID",      SAL_FIELD_TYPE_INT32, "ID of the OAM endpoint whose counters are requested",   "-1"},
    {NULL}
};

/**
 * \brief DNX OAM Group options
 * List of the supported options for oam cf command
 */
static sh_sand_option_t dnx_oam_grp_list_options[] = {
    /**name        type                  desc                                               default*/
    {"ID",      SAL_FIELD_TYPE_INT32, "ID of the requested OAM group",   "-1"},
    {NULL}
};

/**
 * \brief DNX OAM BFDEP options
 * List of the supported options for oam cf command
 */
static sh_sand_option_t dnx_oam_bfdep_list_options[] = {
    /**name        type                  desc                                               default*/
    {"ID",      SAL_FIELD_TYPE_INT32, "ID of the requested BFD Endpoint",   "-1"},
    {NULL}
};

/**
 * \brief DNX OAM BFDEP options
 * List of the supported options for oam cf command
 */
static sh_sand_option_t dnx_oamp_pe_list_options[] = {
    /**name        type                  desc                                               default*/
    {"LAST",      SAL_FIELD_TYPE_BOOL, "diagnostics regarding last packet",   "0"},
    {"PRoGram",   SAL_FIELD_TYPE_BOOL, "all loaded programs",   "0"},
    {"TCam",      SAL_FIELD_TYPE_BOOL, "all loaded TCAMs",   "0"},
    {"ALL",       SAL_FIELD_TYPE_BOOL, "all diagnostics",   "0"},
    {NULL}
};

/**
 * \brief DNX OAMP-PE diagnostic manual
 * This is the manual for "oam OAMP-PE"
 */
static sh_sand_man_t sh_dnx_oamp_pe_man =
{
    .brief  = "OAMP-PE Diagnostic",
    .full   = "Presents OAMP programmable engine diagnostics regarding last packet\n",
    .examples = "last=1\n"
                "program=0\n"
                "tcam=0\n"
                "all\n"
};

/**
 * \brief DNX OAMP TX/RX Counters diagnostic manual
 * This is the manual for "oam oamp ctrs"
 */
static sh_sand_man_t sh_dnx_oamp_ctrs_man =
{
    .brief  = "OAMP TX/RX Counters Diagnostic",
    .full   = "Presents OAMP TX/RX counters diagnostics.\n"
};

/**
 * \brief DNX OAMP_MEP_DB allocation diagnostic manual
 * This is the manual for "oam OAMP_MEP_DB allocation"
 */
static sh_sand_man_t sh_dnx_oamp_mep_db_alloc_man =
{
    .brief  = "OAMP_MEP_DB Diagnostic Allocation command",
    .full   = "Presents OAMP MEP DB diagnostics regarding Allocated MEP IDs\n",
};
/**
 * \brief DNX OAMP_MEP_DB partition diagnostic manual
 * This is the manual for "oam OAMP_MEP_DB partition"
 */
static sh_sand_man_t sh_dnx_oamp_mep_db_part_man =
{
    .brief  = "OAMP_MEP_DB Diagnostic Partition command",
    .full   = "Presents OAMP MEP DB diagnostics regarding MEP ID partition scheme\n",
};

/**
 * \brief DNX OAMP_MEP_DB diagnostic manual
 * This is the manual for "oam OAMP_MEP_DB"
 */
static sh_sand_man_t sh_dnx_oamp_mep_db_man =
{
    .brief  = "OAMP_MEP_DB Diagnostic",
    .full   = "Presents OAMP MEP DB diagnostics regarding partition and allocation\n"
};

/**
 * \brief DNX OAMP status manual
 * This is the manual for "oam OAMP_MEP_DB"
 */
static sh_sand_man_t sh_dnx_oamp_status_man =
{
    .brief  = "OAMP sticky status",
    .full   = "OAMP status\n"
              "Status represents sticky bits and additional information if exist\n"
              "Error priority in parenthesis"
              "    MEP Type - MEP-DB type missmatch(4)\n"
              "    Trap Code - Trap code not as expected(3)\n"
              "        Received Trap Code - Recieved Trap code\n"
              "        TCAM match  -  No match from Trap code TCAM\n"
              "    Timestamp  -  Timestamp missmatch\n"
              "    UDP source port  - Source Port missmatch - BFD only\n"
              "        Received UDP source port -  Received Source Port - BFD only\n"
              "    Remote State -  RMEP state missmatch\n"
              "    RFC 6374 not supported  -  Got packet on RFC 6374 format\n"
              "    Parity Error  -  MEP-DB or RMEP-DB hit with parity error(2)\n"
              "    MDL -  MDL missmatch(6) - OMA only\n"
              "        Received MDL  -  Received MDL\n"
              "        Expected MDL  -  Expected MDL from MEP-DB\n"
              "    MAID  -  MAID missmatch(5) - OAM only\n"
              "        MAID Type  -  '0' - short, '1' - icc\n"
              "        Received MAID  -  Received MAID\n"
              "        Expected MAID  -  Ecpected MAID\n"
              "    Flexible CRC  -  Flexible CRC missmatch\n"
              "        Recieved CRC  -  Recieved Flexible CRC\n"
              "        Expected CRC  -  Expected Flexible CRC\n"
              "    Your Discriminator  -  Your discriminator not in range(9) BFD only\n"
              "        Received Your Discriminator  -  Received your Discriminator - BFD only\n"
              "        Expected Your Discriminator  -  Expected your Discriminator - BFD only\n"
              "    Source IP  -  Source IP missmatch(10) - BFD only\n"
              "        Received Sourrce IP  -  Received Source IP - BFD only\n"
              "        Expected Source IP   -  Expected Source IP - BFD only\n"
              "        Subnet Mask  -  Subnet mask  - BFD only\n"
              "        MEP Type  -  MEP Type form MEP-DB - BFD only\n"
              "    Egress Error  -  Packet arrived with error from egress(1)\n"
              "    My Discriminator  -  Packet my Discriminator missmatch MEP-DB Your doscriminator(8) - BFD only\n"
              "        Received My Discriminator  -  Received my Discriminator - BFD only\n"
              "        Expected My Discriminator  -  Expected my Discriminator - BFD only\n"
              "    EMC -  EMC missmatch(Remote MEP not found)(11)\n"
              "    Channel Type  -  Channel type missmatch\n"
              "    CCM Interval -  CCM interval missmatch(7) - OMA only\n"
              "        Received CCM Interval  -  Expected CCM interval\n"
              "        Expected CCM Interval  -  Received CCM interval\n"
};

/**
 * \brief DNX OAMP_MEP_DB diagnostic manual
 * This is the manual for "oam OAMP_MEP_DB"
 */
static sh_sand_man_t sh_dnx_oamp_man =
{
    .brief  = "OAMP Diagnostic",
    .full   = "Presents OAMP Diagnostic (OAMP_PE/OAMP Counters/MEP_DB/Errors(sticky)\n"
};

/**
 * \brief DNX OAMP MEP DB diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for OAMP MEP DB diagnostic commands
 */
static sh_sand_cmd_t sh_dnx_oamp_mep_db_cmds[] =
{
    /**keyword,        action,           command, options, man */
    {"PARTition",  sh_dnx_oamp_mep_db_cmd_part, NULL, NULL,  &sh_dnx_oamp_mep_db_part_man,  NULL, NULL},
    {"ALLocaTion", sh_dnx_oamp_mep_db_cmd_alloc, NULL, NULL,  &sh_dnx_oamp_mep_db_alloc_man,  NULL, NULL},
    {NULL}
};

/**
 * \brief DNX OAMP diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for OAM Processor diagnostic commands
 */
static sh_sand_cmd_t sh_dnx_oamp_cmds[] =
{
    /**keyword,  action,                  command,                 options,                  man */
    {"COUnter" , sh_dnx_oamp_ctrs_cmd   , NULL,                    NULL                    , &sh_dnx_oamp_ctrs_man  , NULL, NULL},
    {"OAMP_PE" , sh_dnx_oamp_pe_cmd     , NULL,                    dnx_oamp_pe_list_options, &sh_dnx_oamp_pe_man    , NULL, NULL},
    {"MEP_DB"  , NULL                   , sh_dnx_oamp_mep_db_cmds, NULL                    , &sh_dnx_oamp_mep_db_man, NULL, NULL},
    {"StaTus"  , sh_dnx_oamp_status_cmd , NULL,                    NULL                    , &sh_dnx_oamp_status_man, NULL, NULL},
    {NULL}
};

/**
 * \brief DNX OAM diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for OAM diagnostic commands
 */
sh_sand_cmd_t sh_dnx_oam_cmds[] =
{
    /*
     *  keyword,        action,           command,                  options,                        man
     */
    {"ClassiFier",  sh_dnx_classifier_cmd, NULL,                    dnx_oam_cf_list_options,    &sh_dnx_oam_cf_man,  NULL, NULL},
    {"OAM_ID",      NULL,                  sh_dnx_oam_oamid_cmds,   NULL,                       &sh_dnx_oam_oamid_man,  NULL, NULL},
    {"GRouP",       sh_dnx_grp_cmd,        NULL,                    dnx_oam_grp_list_options,   &sh_dnx_oam_grp_man, NULL, NULL},
    {"OAMEndPoint", sh_dnx_oam_oamep_cmd,  NULL,                    dnx_oam_oamep_list_options, &sh_dnx_oam_oamep_man, NULL, NULL},
    {"COUnter",     sh_dnx_oam_cntrs_cmd,  NULL,                    dnx_oam_cntrs_list_options, &sh_dnx_oam_cntrs_man, NULL, NULL},
    {"BFDEndPoint", sh_dnx_oam_bfdep_cmd,  NULL,                    dnx_oam_bfdep_list_options, &sh_dnx_oam_bfdep_man, NULL, NULL},
    {"OAMP"         , NULL                , sh_dnx_oamp_cmds, NULL,                             &sh_dnx_oamp_man, NULL, NULL},
    {"Last_lookUp", sh_dnx_oam_lu_cmd,      NULL,                    NULL,                       &sh_dnx_oam_lu_man, NULL, NULL},
    {NULL}
};
/* *INDENT-ON* */
