/** \file oam_counter.c
 * 
 *
 * OAM counters procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_counter.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * Structure for initialization of table for IPPB_OAM_COUNTER_DISABLE_MAP, ETPPC_OAM_COUNTER_DISABLE_MAP
 * The tables maps the keys to counter disable bit
 */
typedef struct
{
    /*
     * Table keys
     */
    int nof_valid_lm_lifs;
    int oamb_dis_cnt;
    int in_lif_valid_for_lm;
    dbal_enum_value_field_mp_type_e mp_type;
    /*
     * Table Value
     */
    int dis_cnt_access_0;
    int dis_cnt_access_1;
    int dis_cnt_access_2;
} oam_counter_disable_table_table_entry_t;

/** Counter is Enabled / Disabled */
#define OAM_COUNTER_EN  0
#define OAM_COUNTER_DIS 1

/**
 * \brief
 * Counter interface accepted from the OEM accesses
 */

    /** Select counter if of the first lif-db access  */
#define COUNTER_IF_0 DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_0
    /** Select counter if of the second lif-db access  */
#define COUNTER_IF_1 DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_1
    /** Select counter if of the third lif-db access  */
#define COUNTER_IF_2 DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_2
    /** Number of enum valid values for Counter interfaces */
#define NUM_COUNTER_IF DBAL_NOF_ENUM_OAM_LM_COUNTER_READ_IDX_SEL_VALUES

/** Default Values for Counters Range */
#define DEFAULT_COUNTER_RANGE_MIN 0
#define DEFAULT_COUNTER_RANGE_MAX 0

/* Default in-range counter resolution parameters Used for counter resolution for counter in range counter indexes
 *  Used by dnx_oam_counter_default_global_range_set()
 */
#define DEFAULT_IN_RANGE_COUNTER_BASE 0
#define DEFAULT_IN_RANGE_USE_PRIORITY 0
#define DEFAULT_IN_RANGE_SHIFT_VALUE  0

/*
 * Default out-of-range counter resolution parameters Used for counter resolution for counter out-of-range counter
 * indexes. Used by dnx_oam_counter_default_global_range_set()
 */
#define DEFAULT_NOT_IN_RANGE_COUNTER_BASE 0
#define DEFAULT_NOT_IN_RANGE_USE_PRIORITY 1
#define DEFAULT_NOT_IN_RANGE_SHIFT_VALUE  0

/* Maximum allowed number of lifs that have MEPs with a valid
 * counter assigned.
 * Used by dnx_oam_counter_disable_map_reset()
 */
#define MAX_NOF_VALID_LM_LIFS 3

/* OAM PRIORITY MAP PROFILES
   4 different TC to PCP maps may be defined.
   Each map defines a TC to PCP mapping.
   The following 4 values define the specific map
   to be chosen */
#define OAM_PRIORITY_MAP_0 0
#define OAM_PRIORITY_MAP_1 1
#define OAM_PRIORITY_MAP_2 2
#define OAM_PRIORITY_MAP_3 3
/**
 * \brief
 * Counter interface accepted from the OEM accesses (OAM-LIF-DB)
 */
typedef enum counter_interface_e
{
     /**
      *  Only counter interfaces 0,1,2 are allowed
      */
    INVALID_COUNTER_IF_E = -1,
    FIRST_COUNTER_IF_E = 0,
    /*
     * Counter interface accepted from the first OEM access
     */
    COUNTER_IF_0_E = FIRST_COUNTER_IF_E,
    /*
     * Counter interface accepted from the second OEM access
     */
    COUNTER_IF_1_E,
    /*
     * Counter interface accepted from the third OEM access
     */
    COUNTER_IF_2_E,
    NUM_COUNTER_IF_E,
    COUNTER_IF_ALL = NUM_COUNTER_IF_E
} counter_interface_t;

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */

 /*
  * Initialization table for oam_counter_disable_map The HW tables (IPPB_OAM_COUNTER_DISABLE_MAP,
  * ETPPC_OAM_COUNTER_DISABLE_MAP ) enables each counter according to nof_valid_lm_lifs, oamb_dis_cnt,
  * in_lif_valid_for_lm, mp_type. This array is used to initialize the HW table. Each entry initializes an entry
  * in the HW table. The entries in the array organized as: nof_valid_lm_lifs, oamb_dis_cnt, in_lif_valid_for_lm,
  * mp_type, value
  */
  /* *INDENT-OFF* */
static const oam_counter_disable_table_table_entry_t oam_counter_disable_map[] = {
   /** HLM by LIF (2 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {1, 0, 0, BELOW_LOWER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {1, 0, 0, ACTIVE_MATCH_HIGH,          OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {1, 0, 0, PASSIVE_MATCH_HIGH,         OAM_COUNTER_EN, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {1, 0, 0, ABOVE_UPPER_MEP,            OAM_COUNTER_EN, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {1, 0, 0, MIP_MATCH,                  OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {1, 0, 0, MP_TYPE_BFD,                OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), InnerLIF no-LM, outerLIF with LM.*/
    {1, 0, 0, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS,OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), InnerLIF no-LM, outerLIF with LM.*/
    {1, 0, 0, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS,OAM_COUNTER_EN, OAM_COUNTER_EN},

   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {2, 0, 0, BELOW_LOWER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {2, 0, 0, ACTIVE_MATCH_HIGH,          OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM.  We only can get MP_TYPE Above-upper-MEP*/
    {2, 0, 0, PASSIVE_MATCH_HIGH,         OAM_COUNTER_EN, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {2, 0, 0, ABOVE_UPPER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {2, 0, 0, MIP_MATCH,                  OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {2, 0, 0, MP_TYPE_BFD,                OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM.*/
    {2, 0, 0, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS,OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM.*/
    {2, 0, 0, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS,OAM_COUNTER_EN, OAM_COUNTER_EN},

   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {3, 0, 0, BELOW_LOWER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {3, 0, 0, ACTIVE_MATCH_HIGH,          OAM_COUNTER_EN, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {3, 0, 0, PASSIVE_MATCH_HIGH,         OAM_COUNTER_EN, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {3, 0, 0, ABOVE_UPPER_MEP,            OAM_COUNTER_EN, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM. We only can get MP_TYPE Above-upper-MEP*/
    {3, 0, 0, MIP_MATCH,                  OAM_COUNTER_EN, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM.*/
    {3, 0, 0, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS,OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), InnerLIF no-LM, outerLIF with LM.*/
    {3, 0, 0, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS,OAM_COUNTER_EN, OAM_COUNTER_EN},

   /** HLM by MDL */
    {1, 0, 1, ACTIVE_MATCH_LOW,           OAM_COUNTER_DIS, OAM_COUNTER_DIS, OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, PASSIVE_MATCH_LOW,          OAM_COUNTER_DIS, OAM_COUNTER_DIS, OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, BETWEEN_MIDDLE_AND_LOW,     OAM_COUNTER_DIS, OAM_COUNTER_DIS, OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, ACTIVE_MATCH_MIDDLE,        OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, PASSIVE_MATCH_MIDDLE,       OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, BETWEEN_HIGH_AND_MIDDLE,    OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, ACTIVE_MATCH_HIGH,          OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, PASSIVE_MATCH_HIGH,         OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, ABOVE_UPPER_MEP,            OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, BETWEEN_MIP_AND_MEP,        OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, MIP_MATCH,                  OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by MDL */
    {1, 0, 1, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** MPLS_LM_DM - counting lm packets */
    {1, 0, 1, MP_TYPE_BFD,                OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},

   /** HLM by LIF (2 LIFs), innerLIF hit */
    {2, 0, 1, BELOW_LOWER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF hit */
    {2, 0, 1, ACTIVE_MATCH_HIGH,          OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF hit passive */
    {2, 0, 1, PASSIVE_MATCH_HIGH,         OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {2, 0, 1, ABOVE_UPPER_MEP,            OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {2, 0, 1, MIP_MATCH,                  OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {2, 0, 1, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {2, 0, 1, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** MPLS_LM_DM - counting lm packets */
    {2, 0, 1, MP_TYPE_BFD,                OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},

   /** HLM by LIF (3 LIFs), innerLIF hit */
    {3, 0, 1, BELOW_LOWER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF hit */
    {3, 0, 1, ACTIVE_MATCH_HIGH,          OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF hit passive */
    {3, 0, 1, PASSIVE_MATCH_HIGH,         OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 0, 1, ABOVE_UPPER_MEP,            OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 0, 1, MIP_MATCH,                  OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 0, 1, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 0, 1, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** MPLS_LM_DM - counting lm packets */
    {3, 0, 1, MP_TYPE_BFD,                OAM_COUNTER_EN , OAM_COUNTER_EN , OAM_COUNTER_EN},

   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {1, 1, 0, BELOW_LOWER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {1, 1, 0, ACTIVE_MATCH_HIGH,          OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {1, 1, 0, PASSIVE_MATCH_HIGH,         OAM_COUNTER_EN, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {1, 1, 0, ABOVE_UPPER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_DIS},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {1, 1, 0, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {1, 1, 0, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},

   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {2, 1, 0, BELOW_LOWER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {2, 1, 0, ACTIVE_MATCH_HIGH,          OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {2, 1, 0, PASSIVE_MATCH_HIGH,         OAM_COUNTER_EN, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {2, 1, 0, ABOVE_UPPER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {2, 1, 0, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs), innerLIF data packet */
    {2, 1, 0, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},

   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 1, 0, BELOW_LOWER_MEP,            OAM_COUNTER_EN , OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 1, 0, ACTIVE_MATCH_HIGH,          OAM_COUNTER_EN , OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 1, 0, PASSIVE_MATCH_HIGH,         OAM_COUNTER_EN , OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 1, 0, ABOVE_UPPER_MEP,            OAM_COUNTER_EN , OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 1, 0, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs), innerLIF data packet */
    {3, 1, 0, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},

   /** HLM by MDL (1 LIF) */
    {1, 1, 1, PASSIVE_MATCH_HIGH,         OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},

   /** HLM by LIF (2 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {2, 1, 1, ACTIVE_MATCH_HIGH,          OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {2, 1, 1, PASSIVE_MATCH_HIGH,         OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {2, 1, 1, ABOVE_UPPER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {2, 1, 0, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (2 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {2, 1, 0, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},

    /** HLM by LIF (3 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {3, 1, 1, ACTIVE_MATCH_HIGH,          OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {3, 1, 1, PASSIVE_MATCH_HIGH,         OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {3, 1, 1, ABOVE_UPPER_MEP,            OAM_COUNTER_DIS, OAM_COUNTER_EN, OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {3, 1, 0, ACTIVE_MATCH_NO_COUNTER,    OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
   /** HLM by LIF (3 LIFs, innerLIF hit action-set OAMb_dis_cnt=1 */
    {3, 1, 0, PASSIVE_MATCH_NO_COUNTER,   OAM_COUNTER_DIS, OAM_COUNTER_EN , OAM_COUNTER_EN},
};
/* *INDENT-ON* */

/*
 * }
 */

/*
 */

/** see prototype definition in oam_internal.h for information */
shr_error_e
dnx_oam_priority_map_set(
    int unit,
    uint32 oam_priority_map,
    uint32 oam_tc,
    uint32 oam_pcp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_PRIORITY_MAP, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PRIORITY_MAP_PROFILE, oam_priority_map);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, oam_tc);

    /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PCP, INST_SINGLE, oam_pcp);

 /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init Priority mapping table:
 *    EGRESS_OAM_PRIORITY_MAP
 * By default tc is mapped Priority Map Profile 0 is set, and
 * PCP is taken directly from TC
 *
 * \param [in] unit         -   Relevant unit.
 *
 * \return error indication.
 */
shr_error_e
dnx_oam_priority_map_init(
    int unit)
{
    uint32 tc;
    uint32 pcp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (tc = 0; tc < 8; tc++)
    {
        pcp = tc;
        SHR_IF_ERR_EXIT(dnx_oam_priority_map_set(unit, OAM_PRIORITY_MAP_0, tc, pcp));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init Priority mapping tables:
 *    EGRESS_OAM_PRIORITY_MAP_PROFILE
 *    EGRESS_OAM_PRIORITY_MAP
 * By default tc is mapped Priority Map Profile 0 is set, and
 * PCP is taken directly from TC
 *
 * \param [in] unit         -   Relevant unit.
 *
 * \return error indication.
 */
shr_error_e
dnx_oam_priority_resolution_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_priority_map_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set the counter range and index resolution parameters.
 * Parameters of either a specific counter interface or all the
 * three counter interfaces may be set. The counter index from
 * the lifs will be checked against the counter range. According
 * to the result counting per PCP functionality will be applied.
 *
 * Counter Resolution:
 *
 *  Lm_counter_index[counter_if] = counter_base[counter_if] +
 *                                 counter_pointer[counter_if]<<shift_value[counter_if] +
 *                                 use_priority[counter_if]*priority[counter_if] *
 *
 * \param [in] unit         -   Relevant unit.
 * \param [in] is_ingress   -   Counter side to be
 *                              configured. Ingress and
 *                              egress have different
 *                              physical register
 * \param [in] interface_index - index of the counter interface to be configured:
 *                                Use interface_index =
 *                                COUNTER_IF_ALL for configuring
 *                                all interfaces
 * \param [in] counter_range_min - lower boundary of non-PCP
 *                            counter values
 * \param [in] counter_range_max - upper boundary of non-PCP
 *                            counter values
 * \param [in] in_range_counter_base - Counter base of the
 *                                configured counters for
 *                                counters in range.
 * \param [in] in_range_use_priority - Use Priority for
 *                                counter resolution for
 *                                counters in range.
 * \param [in] in_range_shift_value - Shift value for counter
 *                               resolution for counters in
 *                               range.
 * \param [in] not_in_range_counter_base - Counter base of the
 *                                    configured counters for
 *                                    counters out of range.
 * \param [in] not_in_range_use_priority - Use Priority for
 *                                    counter resolution for
 *                                    counters out of range.
 * \param [in] not_in_range_shift_value - Shift value for
 *                                   counter resolution for
 *                                   counters out of range.
 *
 *
 * \return error indication.
 *
 * \remark
 *
 * Indirect output :  Counter range register value is set.
 *
 * By default:
 * PCP will be used for:
 *      min=max=0, counter > MAX, counter < MIN,
 * PCP will NOT be used for:
 *      MIN < counter < MAX
 *
 *  In-Range:
 *   counter_base = 0
 *   counter_shift= 0
 *   use_priority = 0
 *
 *  Out-of-Range: (PCP)
 *   counter_base = 0
 *   counter_shift= 3
 *   use_priority = 1
 *
 * DEFAULT_IN_RANGE_COUNTER_BASE = 0
 * DEFAULT_IN_RANGE_USE_PRIORITY = 0
 * DEFAULT_IN_RANGE_SHIFT_VALUE  = 0
 * DEFAULT_NOT_IN_RANGE_COUNTER_BASE = 0
 * DEFAULT_NOT_IN_RANGE_USE_PRIORITY = 1
 * DEFAULT_NOT_IN_RANGE_SHIFT_VALUE  = 3
 *
 * \see
 *      dnx_oam_counter_global_range_get(),dnx_oam_counter_global_range_set()
 */
shr_error_e
dnx_oam_counter_global_resolution_parameters_set(
    int unit,
    int is_ingress,
    int interface_index,
    uint32 counter_range_min,
    uint32 counter_range_max,
    uint32 in_range_counter_base,
    uint32 in_range_shift_value,
    uint32 in_range_use_priority,
    uint32 not_in_range_counter_base,
    uint32 not_in_range_shift_value,
    uint32 not_in_range_use_priority)
{
    int table;
    int instance_type;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_ingress)
    {
        table = DBAL_TABLE_INGRESS_OAM_COUNTER_GLOBAL_RANGE;
    }
    else
    {
        table = DBAL_TABLE_EGRESS_OAM_COUNTER_GLOBAL_RANGE;
    }

    instance_type = interface_index;
    if (interface_index == COUNTER_IF_ALL)
    {
        instance_type = INST_ALL;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

 /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_MIN, INST_SINGLE, counter_range_min);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_MAX, INST_SINGLE, counter_range_max);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_IN_RANGE_CFG_BASE, instance_type,
                                 in_range_counter_base);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_IN_RANGE_CFG_SHIFT, instance_type,
                                 in_range_shift_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_IN_RANGE_CFG_ADD_PCP, instance_type,
                                 in_range_use_priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_NOT_IN_RANGE_CFG_BASE, instance_type,
                                 not_in_range_counter_base);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_NOT_IN_RANGE_CFG_SHIFT, instance_type,
                                 not_in_range_shift_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_NOT_IN_RANGE_CFG_ADD_PCP, instance_type,
                                 not_in_range_use_priority);

 /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see prototype defintion for information */
shr_error_e
dnx_oam_counter_global_range_get(
    int unit,
    int is_ingress,
    uint32 *counter_range_min,
    uint32 *counter_range_max)
{

    uint32 entry_handle_id;
    int table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_ingress)
    {
        table = DBAL_TABLE_INGRESS_OAM_COUNTER_GLOBAL_RANGE;
    }
    else
    {
        table = DBAL_TABLE_EGRESS_OAM_COUNTER_GLOBAL_RANGE;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

 /** setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_MIN, INST_SINGLE, counter_range_min);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_MAX, INST_SINGLE, counter_range_max);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see prototype defintion for information */
shr_error_e
dnx_oam_counter_global_range_set(
    int unit,
    int is_ingress,
    int counter_range_min,
    int counter_range_max)
{

    uint32 entry_handle_id;
    int table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_ingress)
    {
        table = DBAL_TABLE_INGRESS_OAM_COUNTER_GLOBAL_RANGE;
    }
    else
    {
        table = DBAL_TABLE_EGRESS_OAM_COUNTER_GLOBAL_RANGE;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

 /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_MIN, INST_SINGLE, counter_range_min);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_MAX, INST_SINGLE, counter_range_max);

 /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see prototype defintion  in oam_counter.h for information */
shr_error_e
dnx_oam_lif_db_access_profile_en_counter_disable_set(
    int unit,
    int is_ingress,
    int lif_db_access,
    int mp_profile,
    int packet_is_oam,
    uint32 counter_disable)
{
    uint32 entry_handle_id;
    int dbal_table = is_ingress ? DBAL_TABLE_INGRESS_OAM_LIF_DB_PROFILE_EN_COUNT_DIS :
        DBAL_TABLE_EGRESS_OAM_LIF_DB_PROFILE_EN_COUNT_DIS;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
  /** setting key fields */
    if (lif_db_access == DNX_OAM_COUNTER_OAM_LIF_DB_ACCESS_ALL)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_LIF_DB_ACCESS, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_DB_ACCESS, lif_db_access);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_OAM, packet_is_oam);

    /*
     * setting value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_EN_COUNTER_DISABLE, INST_SINGLE,
                                 counter_disable);

 /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see prototype defintion  in oam_counter.h for information */
shr_error_e
dnx_oam_lif_db_access_profile_en_counter_disable_get(
    int unit,
    int is_ingress,
    int lif_db_access,
    int mp_profile,
    int packet_is_oam,
    uint32 *counter_disable)
{
    uint32 entry_handle_id;
    int dbal_table = is_ingress ? DBAL_TABLE_INGRESS_OAM_LIF_DB_PROFILE_EN_COUNT_DIS :
        DBAL_TABLE_EGRESS_OAM_LIF_DB_PROFILE_EN_COUNT_DIS;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

     /** setting key fields */

  /** setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_DB_ACCESS, lif_db_access);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_OAM, packet_is_oam);

    /*
     * getting value
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_EN_COUNTER_DISABLE, INST_SINGLE,
                               counter_disable);

     /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See oam_internal.h for details */
shr_error_e
dnx_oam_enable_data_counting(
    int unit,
    int profile_id,
    uint8 is_ingress,
    int enable)
{
    uint32 oam_pkt_counting_enable;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Since the API expects counter disable indication, we need to pass !enable
     */
    SHR_IF_ERR_EXIT(dnx_oam_lif_db_access_profile_en_counter_disable_set
                    (unit, is_ingress, DNX_OAM_COUNTER_OAM_LIF_DB_ACCESS_ALL, profile_id, DNX_OAM_PACKET_IS_DATA,
                     !enable));
    /*
     * Enable counting of oam packets
     */
    oam_pkt_counting_enable = DNX_OAM_ENABLE_COUNTING;

    /*
     * Since the API expects counter disable indication, we need to pass !oam_pkt_counting_enable
     */
    SHR_IF_ERR_EXIT(dnx_oam_lif_db_access_profile_en_counter_disable_set
                    (unit, is_ingress, DNX_OAM_COUNTER_OAM_LIF_DB_ACCESS_ALL, profile_id, DNX_OAM_PACKET_IS_OAM,
                     !oam_pkt_counting_enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set oam counter range and counter resolution parameters to
 * default values ( used for init )
 *
 * Default values for counter range are:
 *  counter_range_min = 0
 *  counter_range_max = 0
 *
 * DEFAULT_COUNTER_RANGE_MIN = 0
 * DEFAULT_COUNTER_RANGE_MAX = 0
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication.
 * \remark
 * Indirect return value: Counter range register value is set
 */
static shr_error_e
dnx_oam_counter_default_global_range_set(
    int unit)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set Inress Counter Range
     */
    rv = dnx_oam_counter_global_resolution_parameters_set(unit, DNX_OAM_CLASSIFIER_INGRESS, COUNTER_IF_ALL,
                                                          DEFAULT_COUNTER_RANGE_MIN,
                                                          DEFAULT_COUNTER_RANGE_MAX,
                                                          DEFAULT_IN_RANGE_COUNTER_BASE,
                                                          DEFAULT_IN_RANGE_SHIFT_VALUE,
                                                          DEFAULT_IN_RANGE_USE_PRIORITY,
                                                          DEFAULT_NOT_IN_RANGE_COUNTER_BASE,
                                                          DEFAULT_NOT_IN_RANGE_SHIFT_VALUE,
                                                          DEFAULT_NOT_IN_RANGE_USE_PRIORITY);
    SHR_IF_ERR_EXIT(rv);

    /*
     * Set Egress Counter Range
     */
    rv = dnx_oam_counter_global_resolution_parameters_set(unit, DNX_OAM_CLASSIFIER_EGRESS, COUNTER_IF_ALL,
                                                          DEFAULT_COUNTER_RANGE_MIN,
                                                          DEFAULT_COUNTER_RANGE_MAX,
                                                          DEFAULT_IN_RANGE_COUNTER_BASE,
                                                          DEFAULT_IN_RANGE_SHIFT_VALUE,
                                                          DEFAULT_IN_RANGE_USE_PRIORITY,
                                                          DEFAULT_NOT_IN_RANGE_COUNTER_BASE,
                                                          DEFAULT_NOT_IN_RANGE_SHIFT_VALUE,
                                                          DEFAULT_NOT_IN_RANGE_USE_PRIORITY);

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set bitmap of the counter selection index for counter read.
 * We have up to 3 counter interfaces from the 3 accesses to
 * LIF-DB.
 * This bitmap selects the specific counter interface to be read
 * for counter stamping. This function should be called during
 * init.
 *
 *  \param [in] unit -         Relevant unit.
 *
 *  \return
 *    \retval None. HW Register value is set.
 *
 *   \remark
 *   Usually the counter value should be read from the
 *   counter-if-1.
 *   In case of Hierarchical LM by MDL, the counter
 *   interface should be selected according to the MEP
 *   level that was hit i.e. according to mp-type
 */
static shr_error_e
dnx_oam_counter_read_select_set(
    int unit)
{
    uint32 entry_handle_id;
    int table_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

        /** Init both ingress and egress tables */
    for (table_index = 0; table_index < 2; table_index++)
    {
        if (table_index == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_OAM_LM_COUNTER_READ_IDX_SEL, &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LM_COUNTER_READ_IDX_SEL, &entry_handle_id));
        }

        /*
         * First set all entries to select interface-1
         */
         /** setting key fields */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_VALID_FOR_LM, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
         /** setting value field */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LM_COUNTER_READ_IDX_SEL, INST_SINGLE,
                                     COUNTER_IF_0);

         /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Now set for HLM by MDL cases
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE,
                                         DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE,
                                         DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LM_COUNTER_READ_IDX_SEL, INST_SINGLE,
                                     COUNTER_IF_1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE,
                                         DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW,
                                         DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LM_COUNTER_READ_IDX_SEL, INST_SINGLE,
                                     COUNTER_IF_2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Now set for HLM by LIF with IN_LIF_VALID_FOR_LM==0 for oam injeced over mpls
         */
        if (table_index == 1)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_VALID_FOR_LM, 0);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE,
                                       DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH);
            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, 1, 2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LM_COUNTER_READ_IDX_SEL, INST_SINGLE,
                                         COUNTER_IF_2);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Reset oam_counter_disable_map ( used for init )
 *
 * The HW table (IPPB_OAM_COUNTER_DISABLE_MAP) enables each counter
 * according to nof_valid_lm_lifs, oamb_dis_cnt, in_lif_valid_for_lm, mp_type.
 *
 * This function resets all entries in
 * IPPB_OAM_COUNTER_DISABLE_MAP table and defines all
 * counters to be disabled for all possible sets of
 * nof_valid_lm_lifs, oamb_dis_cnt, in_lif_valid_for_lm,
 * mp_type.
 *
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] is_ingress - Selects the table ( may be
 *        ingress or egress table )
 *
 * \retval Error indication according to shr_error_e enum
 *
 * \remark Indirect output: Entry of the HW table
 */

static shr_error_e
dnx_oam_counter_disable_map_reset(
    int unit,
    int is_ingress)
{

    int table;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Key:
     * -------------------------------------------------------------------------------- |
     * nof_valid_lm_lifs(2b) | oamb_dis_cnt(1b) | in_lif_valid_for_lm(1b) | mp_type(4b) |
     * ---------------------------------------------------------------------------------|
     */

    if (is_ingress)
    {
        table = DBAL_TABLE_INGRESS_OAM_COUNTER_GENERAL_ENABLE;
    }
    else
    {
        table = DBAL_TABLE_EGRESS_OAM_COUNTER_GENERAL_ENABLE;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

 /** setting key fields range */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAMB_DIS_CNT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_VALID_FOR_LM, DBAL_RANGE_ALL,
                                     DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /*
     * setting value fields
     * reset all entries
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_0, INST_SINGLE,
                                 OAM_COUNTER_DIS);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_1, INST_SINGLE,
                                 OAM_COUNTER_DIS);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_2, INST_SINGLE,
                                 OAM_COUNTER_DIS);

 /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set oam_counter_disable_map entry
 *
 * The HW table (IPPB_OAM_COUNTER_DISABLE_MAP) enables each counter
 * according to nof_valid_lm_lifs, oamb_dis_cnt, in_lif_valid_for_lm, mp_type.
 *
 * This function sets an entry in IPPB_OAM_COUNTER_DISABLE_MAP table and
 * defines which countes will be enabled for the given set of
 * nof_valid_lm_lifs, oamb_dis_cnt, in_lif_valid_for_lm, mp_type.
 *
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] table -    table name ( may be ingress or egress table )
 * \param [in] nof_valid_lm_lifs -     Number of lifs with valid lm meps (used as key)
 *             nof_valid_lm_lifs = oam_lif_0_valid + oam_lif_1_valid + oam_lif_2_valid
 * \param [in] oamb_dis_cnt -          OAMB dis_cnt bit from acc action table (used as key)
 * \param [in] in_lif_valid_for_lm -   Indication for lm mep on most inner lif (used as key)
 *             in_lif_valid_for_lm = in_lif[0]==OAM_LIF_0
 * \param [in] mp_type -               mp_type (used as key)
 * \param [in] dis_cnt_access_0 -      (result) value to be
 *        written to the table (enables/disables counter from
 *        first access).
 * \param [in] dis_cnt_access_1 -      (result) value to be
 *        written to the table (enables/disables counter from
 *        second access).
 * \param [in] dis_cnt_access_2 -      (result) value to be
 *        written to the table (enables/disables counter from
 *        third access).
 *
 * \retval Error indication according to shr_error_e enum
 * \remark Entry of the HW table will be set
 */

static shr_error_e
dnx_oam_counter_disable_map_entry_set(
    int unit,
    int table,
    int nof_valid_lm_lifs,
    int oamb_dis_cnt,
    int in_lif_valid_for_lm,
    dbal_enum_value_field_mp_type_e mp_type,
    int dis_cnt_access_0,
    int dis_cnt_access_1,
    int dis_cnt_access_2)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Key:
     * -------------------------------------------------------------------------------- |
     * nof_valid_lm_lifs(2b) | oamb_dis_cnt(1b) | in_lif_valid_for_lm(1b) | mp_type(4b) |
     * ---------------------------------------------------------------------------------|
     */

    /*
     * write IPPB_OAM_COUNTER_DISABLE_MAP,key,dis_cnt_access_0,dis_cnt_access_1,dis_cnt_access_2
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

  /** setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMB_DIS_CNT, oamb_dis_cnt);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_VALID_FOR_LM, in_lif_valid_for_lm);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE, mp_type);

  /** setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_0, INST_SINGLE,
                                 dis_cnt_access_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_1, INST_SINGLE,
                                 dis_cnt_access_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_2, INST_SINGLE,
                                 dis_cnt_access_2);

  /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see prototype defintion for information */
shr_error_e
dnx_oam_counter_disable_map_entry_get(
    int unit,
    int table,
    int nof_valid_lm_lifs,
    int oamb_dis_cnt,
    int in_lif_valid_for_lm,
    dbal_enum_value_field_mp_type_e mp_type,
    uint32 *dis_cnt_1,
    uint32 *dis_cnt_2,
    uint32 *dis_cnt_3)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Key:
     * -------------------------------------------------------------------------------- |
     * nof_valid_lm_lifs(2b) | oamb_dis_cnt(1b) | in_lif_valid_for_lm(1b) | mp_type(4b) |
     * ---------------------------------------------------------------------------------|
     */

    /*
     * write IPPB_OAM_COUNTER_DISABLE_MAP,key,dis_cnt_high,dis_cnt_middle,dis_cnt_low
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

 /** setting value fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMB_DIS_CNT, oamb_dis_cnt);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_VALID_FOR_LM, in_lif_valid_for_lm);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE, mp_type);

  /** setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_0, INST_SINGLE, dis_cnt_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_1, INST_SINGLE, dis_cnt_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_2, INST_SINGLE, dis_cnt_3);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init oam_counter_disable_map table.
 * First disables all counters for all entries then
 * reads static table and inits relevant entries in HW table
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 * \remark   oam_counter_disable_map - Static array of
 *           entries for initializing the HW table is used
 */
static shr_error_e
dnx_oam_counter_disable_map_init(
    int unit)
{
    int rv = _SHR_E_NONE;
    int nof_valid_lm_lifs;
    int oamb_dis_cnt;
    int in_lif_valid_for_lm;
    dbal_enum_value_field_mp_type_e mp_type;
    int dis_cnt_access_0, dis_cnt_access_1, dis_cnt_access_2;
    int entry;
    int nof_oam_counter_disable_table_entries;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Disable counters for all entries
     */
    rv = dnx_oam_counter_disable_map_reset(unit, DNX_OAM_CLASSIFIER_INGRESS);
    SHR_IF_ERR_EXIT(rv);
    rv = dnx_oam_counter_disable_map_reset(unit, DNX_OAM_CLASSIFIER_EGRESS);
    SHR_IF_ERR_EXIT(rv);

    /*
     * Enable counters according to table
     */
    nof_oam_counter_disable_table_entries =
        sizeof(oam_counter_disable_map) / sizeof(oam_counter_disable_table_table_entry_t);

    for (entry = 0; entry < nof_oam_counter_disable_table_entries; entry++)
    {
        nof_valid_lm_lifs = oam_counter_disable_map[entry].nof_valid_lm_lifs;
        oamb_dis_cnt = oam_counter_disable_map[entry].oamb_dis_cnt;
        in_lif_valid_for_lm = oam_counter_disable_map[entry].in_lif_valid_for_lm;
        mp_type = oam_counter_disable_map[entry].mp_type;

        dis_cnt_access_0 = oam_counter_disable_map[entry].dis_cnt_access_0;
        dis_cnt_access_1 = oam_counter_disable_map[entry].dis_cnt_access_1;
        dis_cnt_access_2 = oam_counter_disable_map[entry].dis_cnt_access_2;

        rv = dnx_oam_counter_disable_map_entry_set(unit, DBAL_TABLE_INGRESS_OAM_COUNTER_GENERAL_ENABLE,
                                                   nof_valid_lm_lifs, oamb_dis_cnt, in_lif_valid_for_lm, mp_type,
                                                   dis_cnt_access_0, dis_cnt_access_1, dis_cnt_access_2);
        SHR_IF_ERR_EXIT(rv);
        rv = dnx_oam_counter_disable_map_entry_set(unit, DBAL_TABLE_EGRESS_OAM_COUNTER_GENERAL_ENABLE,
                                                   nof_valid_lm_lifs, oamb_dis_cnt, in_lif_valid_for_lm, mp_type,
                                                   dis_cnt_access_0, dis_cnt_access_1, dis_cnt_access_2);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * General initializations for OAM counters
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 * \remark
 */
static shr_error_e
dnx_oam_counters_general_init(
    int unit)
{
    uint32 entry_handle_id;
    int counter_if;
    int new_counter_if;
    int nof_cores;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_oam.general.oam_lm_read_index_field_exists_get(unit))
    {
        nof_cores = dnx_data_device.general.nof_cores_get(unit);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_STAT_LM_READ_INDEX, &entry_handle_id));

        for (core_id = 0; core_id < nof_cores; core_id++)
        {
            for (counter_if = COUNTER_IF_0; counter_if <= NUM_COUNTER_IF; counter_if++)
            {
                new_counter_if = (counter_if + 1) & 0x3;
                /*
                 * key construction
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLP_COUNTER_READ_INDEX, counter_if);

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CGM_COUNTER_READ_INDEX, INST_SINGLE,
                                             new_counter_if);

                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

           /** Clear current entry to config other next entries */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAM_STAT_LM_READ_INDEX, entry_handle_id));

            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See h file for description
 */
shr_error_e
dnx_oam_counters_init(
    int unit)
{
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    rv = dnx_oam_counters_general_init(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = dnx_oam_counter_disable_map_init(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = dnx_oam_counter_read_select_set(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = dnx_oam_counter_default_global_range_set(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = dnx_oam_priority_resolution_init(unit);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}
