/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tx.c
 * Purpose: Implementation of bcm_petra_tx* API for dune devices
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif



#define _ERR_MSG_MODULE_NAME BSL_BCM_POLICER

#include <shared/bsl.h>

#include <bcm/policer.h>
#include <soc/mem.h>

#include <bcm_int/control.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>

#include <soc/dpp/PPD/ppd_api_metering.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/policer.h>

#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/rate.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/switch.h>

#include <bcm/debug.h>
#include <bcm_int/common/debug.h>

#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

/***************************************************************/

/*
 * Local defines
 */


#define DPP_POLICER_MSG(string)   _ERR_MSG_MODULE_NAME, unit, "%s[%d]: " string, __FILE__, __LINE__

#define DPP_POLICER_INIT_CHECK(unit) \
do { \
    BCM_DPP_UNIT_CHECK(unit); \
    if (!sw_state_sync_db[(unit)].dpp.policer_lock) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,  (_BSL_BCM_MSG("%s: Policers unitialized on unit:%d \n"), \
                           FUNCTION_NAME(), unit)); \
    } \
} while (0)

#define DPP_POLICER_UNIT_LOCK(unit) \
do { \
    if (sal_mutex_take(sw_state_sync_db[(unit)].dpp.policer_lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d. \n"), \
                    FUNCTION_NAME(), unit)); \
    } \
    _lock_taken = 1;  \
} while (0)


#define DPP_POLICER_UNIT_UNLOCK(unit) \
do { \
    if(1 == _lock_taken) { \
        _lock_taken = 0;  \
        if (sal_mutex_give(sw_state_sync_db[(unit)].dpp.policer_lock)) { \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_give failed for unit %d. \n"), \
                        FUNCTION_NAME(), unit)); \
        } \
    } \
} while (0)

#define DPP_POLICER_LEGAL_FLAGS_ARAD (  BCM_POLICER_WITH_ID                 | \
                                        /* Note: REPLACE is an internal */    \
                                        /* flag. */                           \
                                        BCM_POLICER_REPLACE                 | \
                                        BCM_POLICER_MACRO                   | \
                                        BCM_POLICER_COLOR_BLIND             | \
                                        BCM_POLICER_REPLACE_SHARED)

#ifdef BCM_88660_A0
#   define DPP_POLICER_LEGAL_FLAGS_ARAD_PLUS (DPP_POLICER_LEGAL_FLAGS_ARAD | BCM_POLICER_MODE_PACKETS )
#endif

/* implemented for PB:
 *  - assumption, meter groups: 0,1
 *  - meter encoding type: 0 
 */


/* convert defines*/
/* kbits to bytes, for burst BCM TO DPP */
#define _DPP_POLICER_KBITS_TO_BYTES(__bcm_rate) ((__bcm_rate == 0) ? 64 :((__bcm_rate)*125))
/* bytes to kbits , for burst DPP TO BCM */
#define _DPP_POLICER_BYTES_TO_KBITS(__bcm_rate) ((__bcm_rate)/125)

/* is high rate meter needed, according to rate/burst volume*/
#define _DPP_POLICER_IS_HIGH_RATE(__unit,__pol_cfg)    \
    ((__pol_cfg->ckbits_sec > SOC_DPP_CONFIG(unit)->meter.lr_max_rate) ||    \
    (__pol_cfg->pkbits_sec > SOC_DPP_CONFIG(unit)->meter.lr_max_rate) ||    \
    (_DPP_POLICER_KBITS_TO_BYTES((__pol_cfg)->ckbits_burst) > SOC_DPP_CONFIG(unit)->meter.lr_max_burst) || \
    (_DPP_POLICER_KBITS_TO_BYTES((__pol_cfg)->pkbits_burst) > SOC_DPP_CONFIG(unit)->meter.lr_max_burst))

/* this is only SW representation */
/* bit 12 tells if this is group 0 or 1*/

/* start index of High rate */
#define _DPP_POLICER_METER_HR_PROFILE_MIN(__unit) ((SOC_DPP_CONFIG(__unit)->meter.nof_meter_profiles - SOC_DPP_CONFIG(__unit)->meter.nof_high_rate_profiles))


/* 
 * format meter id 
 * 0-12: meter/policer id 
 * 13/15: group (for meter) 
 * 14/16: core (for meter) 
 * 29: type 0:meter 1:ethernet-policer , 
 *     needed as same APIs and ID used for both meter types
 * Notes: 
 *    - meter type: has to be zero, as pointer by FP
 *    - meter core: currently up to two cores are supported by the macros
 */


/* check if profile is high rate*/
#define _DPP_POLICER_PROFILE_IS_HR(_unit, __profile_id ) ((__profile_id) >= (SOC_DPP_CONFIG(unit)->meter.nof_meter_profiles - SOC_DPP_CONFIG(unit)->meter.nof_high_rate_profiles))


#define POLICER_ACCESS  sw_state_access[unit].dpp.bcm.policer


#define _DPP_POLICER_GROUP_MODES (14)

#define _DPP_POLICER_KBITS_TO_BYTES_FACTOR   125
#define _DPP_POLICER_PACKETS_TO_BYTES_FACTOR 64

static int _dpp_policer_group_num_policers[_DPP_POLICER_GROUP_MODES] = {
    1,  /* bcmPolicerGroupModeSingle - all                              */
    3,  /* bcmPolicerGroupModeTrafficType - kn/uk uc, kn/uc mc, bc      */
    2,  /* bcmPolicerGroupModeDlfAll - <unknow-uc/mc, mc/uc/bc>         */
    -1, /* bcmPolicerGroupModeDlfIntPri - Unsupported                   */
    4,  /* bcmPolicerGroupModeTyped- uk-uc,k-uc, kn/uk mc, bc           */
    -1, /* bcmPolicerGroupModeTypedAll- uk-uc,k-uc, mc, bc, glbl        */
    -1, /* bcmPolicerGroupModeTypedIntPri 
           based on configured cos levels                               */
    -1, /* bcmPolicerGroupModeSingleWithControl-  all, glbl             */
    -1, /* bcmPolicerGroupModeTrafficTypeWithControl - uc, mc, bc, glbl */
    -1, /* bcmPolicerGroupModeDlfAllWithControl - Unsupported           */
    -1, /* bcmPolicerGroupModeDlfIntPriWithControl - <unknow-uc/mc, mc/uc/bc. glbl>                               */
    -1, /* bcmPolicerGroupModeTypedWithControl uk-uc,k-uc, mc, bc, glbl */
    -1, /* bcmPolicerGroupModeTypedAllWithControl                       */
    -1  /* bcmPolicerGroupModeTypedIntPriWithControl 
           based on configured cos levels                               */
};

/* policer utilities */

STATIC uint32 _bcm_petra_policer_translate_kbits_to_packets(uint32 kbits)
{
    uint32 packets= kbits*_DPP_POLICER_KBITS_TO_BYTES_FACTOR;
    packets = SOC_SAND_DIV_ROUND(packets,_DPP_POLICER_PACKETS_TO_BYTES_FACTOR);
    return packets;
}

STATIC uint32 _bcm_petra_policer_translate_packets_to_kbits(uint32 packets)
{
    uint32 kbits = packets*_DPP_POLICER_PACKETS_TO_BYTES_FACTOR;
    kbits = SOC_SAND_DIV_ROUND(kbits,_DPP_POLICER_KBITS_TO_BYTES_FACTOR);
    return kbits;
}

STATIC uint32 _bcm_petra_policer_translate_packets_to_bytes(uint32 packets)
{
    uint32 bytes = packets*_DPP_POLICER_PACKETS_TO_BYTES_FACTOR;
    return bytes;
}

STATIC uint32 _bcm_petra_policer_translate_bytes_to_packets(uint32 bytes)
{
    uint32 packets = bytes/_DPP_POLICER_PACKETS_TO_BYTES_FACTOR;
    return packets;
}


#ifdef BCM_88660_A0

/** 
 *  Correct all rates in pol_cfg by the specified multiplier.
 *  
 *  @pol_cfg The configuration to correct.
 *  @multiplier The amount to multiply or divide by.
 *  @divide If TRUE then divide the rates by @multiplier.
 *          Otherwise multiply by it.
 */
STATIC int _bcm_dpp_policer_correct_config_rates(int unit, bcm_dpp_am_meter_entry_t *db_info, bcm_policer_config_t *pol_cfg, int divide)
{
    int i = 0;
    uint32 *rates[6];

    BCMDNX_INIT_FUNC_DEFS;

    if(db_info != NULL) {
        rates[0] = &db_info->cbs;
        rates[1] = &db_info->cir;
        rates[2] = &db_info->max_cir;
        rates[3] = &db_info->ebs;
        rates[4] = &db_info->eir;
        rates[5] = &db_info->max_eir;    
    } else if(pol_cfg != NULL) {
        rates[0] = &pol_cfg->ckbits_burst;
        rates[1] = &pol_cfg->ckbits_sec;
        rates[2] = &pol_cfg->max_ckbits_sec;
        rates[3] = &pol_cfg->pkbits_burst;
        rates[4] = &pol_cfg->pkbits_sec;
        rates[5] = &pol_cfg->max_pkbits_sec;     
    } else {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("_bcm_dpp_policer_correct_config_rates icalled with null arguments")));
    }

    for (i = 0; i < sizeof(rates) / sizeof(rates[0]); i++) {
        /* for the max_rates, 0xFFFFFFFF is an 'unlimited' value so it cannot be changed*/
        if ((i==2 || i==5)&&(*rates[i] == 0xFFFFFFFF)) {
            continue;
        }
        if (divide) {
            *(rates[i]) = _bcm_petra_policer_translate_packets_to_kbits(*(rates[i]));
        } else {
            *(rates[i]) = _bcm_petra_policer_translate_kbits_to_packets(*(rates[i]));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

#endif /* BCM_88660_A0 */

STATIC int bcm_petra_meter_max_info(
   int unit,
   bcm_policer_config_t *pol_cfg) 
{
    BCMDNX_INIT_FUNC_DEFS;
    bcm_policer_config_t_init(pol_cfg);

    /* sw state allocate unique profile according to this infomration */
    pol_cfg->ckbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
    pol_cfg->pkbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
    pol_cfg->max_pkbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
    pol_cfg->max_ckbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
    pol_cfg->ckbits_burst = _DPP_POLICER_BYTES_TO_KBITS(SOC_DPP_CONFIG(unit)->meter.max_burst);
    pol_cfg->pkbits_burst = _DPP_POLICER_BYTES_TO_KBITS(SOC_DPP_CONFIG(unit)->meter.max_burst);
    pol_cfg->flags = 0;
    pol_cfg->mode = bcmPolicerModeTrTcmDs;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int bcm_petra_policer_max_info(
   int unit,
   bcm_policer_config_t *pol_cfg
    )
{
    BCMDNX_INIT_FUNC_DEFS;
    bcm_policer_config_t_init(pol_cfg);
    pol_cfg->ckbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
    pol_cfg->pkbits_sec = 0;
    pol_cfg->max_ckbits_sec = 0;
    pol_cfg->max_pkbits_sec = 0;
    pol_cfg->ckbits_burst = _DPP_POLICER_BYTES_TO_KBITS(SOC_DPP_CONFIG(unit)->meter.max_burst);
    pol_cfg->pkbits_burst = 0;
    pol_cfg->flags = 0;
    pol_cfg->mode = bcmPolicerModeSrTcm;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int bcm_petra_max_info_check(
   int unit,
   bcm_policer_config_t *pol_cfg,
        bcm_policer_config_t *max_pol_cfg)
{
    bcm_policer_config_t pol_cfg_rates;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_policer_config_t_init(&pol_cfg_rates);
    pol_cfg_rates.ckbits_sec = pol_cfg->ckbits_sec;
    pol_cfg_rates.ckbits_burst = pol_cfg->ckbits_burst;
    pol_cfg_rates.pkbits_sec = pol_cfg->pkbits_sec;
    pol_cfg_rates.pkbits_burst = pol_cfg->pkbits_burst;
    pol_cfg_rates.max_pkbits_sec = pol_cfg->max_pkbits_sec;
    pol_cfg_rates.max_ckbits_sec = pol_cfg->max_ckbits_sec;

    if (pol_cfg->flags & BCM_POLICER_MODE_PACKETS) {
        rv = _bcm_dpp_policer_correct_config_rates(unit, NULL, &pol_cfg_rates, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* sw state allocate unique profile according to this infomration */
    if (pol_cfg_rates.ckbits_sec > max_pol_cfg->ckbits_sec ||
        pol_cfg_rates.pkbits_sec > max_pol_cfg->pkbits_sec ||
        pol_cfg_rates.ckbits_burst > max_pol_cfg->ckbits_burst ||
        pol_cfg_rates.pkbits_burst > max_pol_cfg->pkbits_burst) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("one or more of the rates/bursts is above max")));
    }
    if (pol_cfg_rates.max_pkbits_sec != _DPP_POLICER_MAX_KBITS_SEC_UNLIMITED &&
        (pol_cfg_rates.max_pkbits_sec > max_pol_cfg->max_pkbits_sec ||
         pol_cfg_rates.max_pkbits_sec < pol_cfg_rates.pkbits_sec)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("max_pkbits_sec is above max or < pkbits_sec")));
    }
    if (pol_cfg->mode == bcmPolicerModeCascade ||
        pol_cfg->mode == bcmPolicerModeCoupledCascade) {
        if (pol_cfg_rates.max_ckbits_sec != _DPP_POLICER_MAX_KBITS_SEC_UNLIMITED &&
            (pol_cfg_rates.max_ckbits_sec > max_pol_cfg->max_ckbits_sec ||
             pol_cfg_rates.max_ckbits_sec < pol_cfg_rates.ckbits_sec)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("max_ckbits_sec is above max or < ckbits_sec")));
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_petra_polcer_check_config
 * Purpose
 *      general check function for policer params
 * Parameters
 *      (in) unit       = unit number
 *      (in) pol_cfg       = policer configuration
 *      ((in)) is_eth_policer = is ethernet policer or meter
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_petra_policer_check_config(
   int unit,
   bcm_policer_config_t      *pol_cfg,
   int                        is_eth_policer,
   int                        policer_id)
{
    int rv = BCM_E_NONE;
    bcm_policer_config_t      max_pol_cfg;
    uint32 legal_flags;

    BCMDNX_INIT_FUNC_DEFS;

    legal_flags = DPP_POLICER_LEGAL_FLAGS_ARAD;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        legal_flags = DPP_POLICER_LEGAL_FLAGS_ARAD_PLUS;
    }
    if (SOC_IS_JERICHO(unit)) {
        legal_flags |= BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE;
    }
#endif /* BCM_88660_A0 */

    /* replace without ID! */
    if ((pol_cfg->flags & BCM_POLICER_REPLACE) && (!(pol_cfg->flags & BCM_POLICER_WITH_ID))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("replace flag can not be present when with_id is not present \n")));
    }
    if (pol_cfg->kbits_current != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("current kbits has to be zero \n")));
    }

    if (pol_cfg->action_id != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("action id has to be zero \n")));
    }

    /*
    if (pol_cfg->sharing_mode != BCM_POLICER_GLOBAL_METER_SHARING_MODE_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("in case of sharing only minimum is supported \n")));
    } 
    */ 



    if (_DPP_POLICER_METER_IS_SINGLE_CORE(unit)) {
        if ((SOC_DPP_CONFIG(unit)->meter.meter_range_mode == 64) && (pol_cfg->flags & BCM_POLICER_MACRO)) {
            /* in 64K range mode with single meter core, meters are not divided into 2 groups, so user cannot specify a group */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("BCM_POLICER_MACRO flag is not supported when policer_ingress_count is 64K\n")));
        }
    }
    else /*dual meter cores*/
    {
        if (SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit)) {
            if ((SOC_DPP_CONFIG(unit)->meter.meter_range_mode == 128) && (pol_cfg->flags & BCM_POLICER_MACRO)) {
                /* in 128K range mode, meters are not divided into 2 groups, so user cannot specify a group */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("BCM_POLICER_MACRO flag is not supported when policer_ingress_count is 128K\n")));
            }
            if (pol_cfg->core_id != 0) {
                /* in single core mode device, core-id is not supported */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("core_id is not supported in single core mode\n")));
            }
        }
        else {/*dual device core*/
            if ((SOC_DPP_CONFIG(unit)->meter.meter_range_mode == 64) && (pol_cfg->flags & BCM_POLICER_MACRO)) {
                /* in 64K range mode, meters are not divided into 2 groups, so user cannot specify a group */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("BCM_POLICER_MACRO flag is not supported when policer_ingress_count is 64K\n")));
            }
        }
        
    }

    if ((pol_cfg->flags & BCM_POLICER_REPLACE_SHARED) && (pol_cfg->entropy_id == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("BCM_POLICER_REPLACE_SHARED is valid only with non-zero entropy_id\n")));
    }

    if (pol_cfg->entropy_id >= SOC_DPP_CONFIG(unit)->meter.nof_meter_profiles) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("entropy_id is out of bounds (max %d).\n"), SOC_DPP_CONFIG(unit)->meter.nof_meter_profiles));
    }

    if (pol_cfg->core_id > _DPP_POLICER_METER_MAX_CORE_ID(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Invalid core_id (max %d).\n"), _DPP_POLICER_METER_MAX_CORE_ID(unit)));
    }

    /* verify parameters according to mode */
    if (!is_eth_policer) {
        switch (pol_cfg->mode) {
        case bcmPolicerModeSrTcm: 
            if (pol_cfg->pkbits_sec != 0) { /* eir has to be zero in single rate */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("In single rate, EIR have to be equal to 0 \n")));
            }
            if (pol_cfg->max_pkbits_sec == 0) { /* max eir shouldn't be zero in SrTcm */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("In single-rate two-colors, Max EIR shouldn't be equal to zero \n")));
            }
            if (pol_cfg->ckbits_sec !=0 && pol_cfg->ckbits_burst == 0) { /* if cir isn't zero, cbs can't be zero */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Zero burst size isn't supported, 64 bytes is the minimal value \n")));
            }
            break; 
        case bcmPolicerModeTrTcmDs: /* two rate three colors */
        case bcmPolicerModeCoupledTrTcmDs: /* two rates three colors with coupling */
            if (pol_cfg->ckbits_sec !=0 && pol_cfg->ckbits_burst == 0) { /* if cir isn't zero, cbs can't be zero */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Zero burst size isn't supported, 64 bytes is the minimal value \n")));
            }
            break;
        case  bcmPolicerModeCommitted: /* single rate single burst */
            if (pol_cfg->pkbits_sec != 0 || pol_cfg->pkbits_burst != 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("In single rate, EIR and Excess Burst size have to be equal to 0 \n")));
            }
            break;
            /* modes that are supported only in ARAD */
        case bcmPolicerModeCascade: /* 4 meters work in hierarchical mode */
        case bcmPolicerModeCoupledCascade: /* 4 meters work in hierarchical mode with coupling */
            /* groups of 4 hierarchical meters */
            if (pol_cfg->flags & BCM_POLICER_WITH_ID) {
                /* meter has to be: meter % 4 = 0 */
                if ((policer_id % 4) & !(pol_cfg->flags & BCM_POLICER_REPLACE)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_BADID, (_BSL_BCM_MSG("In cascade mode policer_id must be: policer_id %% 4 = 0")));
                }
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (DPP_POLICER_MSG("Invalid policer mode: %d \n"), pol_cfg->mode));
        }

        /* only these flags are supported for meters */
        if ((pol_cfg->flags & ~legal_flags) != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("illegal flags specified")));
        }

        rv = bcm_petra_meter_max_info(
           unit,
           &max_pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* 
         * ethernet policer, has one bucket (commited), 
         *   in-profile: don't change color
         *   out-of-profile: red
         */

        if (pol_cfg->pkbits_burst != 0 || pol_cfg->pkbits_sec != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("pkbits_burst and pkbits_sec must be zero"))); /* has to be zero */
        }
        if (pol_cfg->mode != bcmPolicerModeSrTcm) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("only bcmPolicerModeSrTcm mode is supported"))); /* only this mode is supported for ethernet policer */
        }

        /* only these flags are supported for ethernet policer */
        if (SOC_IS_JERICHO(unit)) {
            if (pol_cfg->flags & (~(BCM_POLICER_REPLACE | BCM_POLICER_WITH_ID | BCM_POLICER_MODE_BYTES | BCM_POLICER_MODE_PACKETS | BCM_POLICER_COLOR_BLIND | BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("one or more of the flags is not supported"))); /* not supported */
            }
        }else{
            if (pol_cfg->flags & (~(BCM_POLICER_REPLACE | BCM_POLICER_WITH_ID | BCM_POLICER_MODE_BYTES | BCM_POLICER_MODE_PACKETS | BCM_POLICER_COLOR_BLIND))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("one or more of the flags is not supported"))); /* not supported */
            }
        }

        /* only these flags are supported for ethernet policer */
        if ((pol_cfg->flags & BCM_POLICER_MODE_PACKETS) && (pol_cfg->flags & BCM_POLICER_MODE_BYTES)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags BCM_POLICER_MODE_PACKETS and BCM_POLICER_MODE_BYTES cannot be both set"))); /* cannot be both */
        }

        rv = bcm_petra_policer_max_info(
           unit,
           &max_pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = bcm_petra_max_info_check(unit, pol_cfg, &max_pol_cfg);
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_QUX(unit) && (pol_cfg->mark_drop_as_black != 0 && pol_cfg->mark_drop_as_black != 1))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mark_drop_as_black valid values are: 0 or 1")));        
    }

    if(((pol_cfg->mode == bcmPolicerModeCascade) || (pol_cfg->mode == bcmPolicerModeCoupledCascade)) && 
        (pol_cfg->flags & BCM_POLICER_REPLACE) && 
        (pol_cfg->entropy_id !=0))
    {
        LOG_WARN(BSL_LS_BCM_POLICER,
                 (BSL_META_U(unit,
                             "policer replace for mode=%d and entropy_id=%d. cascade mode requires that all meters will have the same Rev-Exp. \n"
                             "driver cannot garentee it once the user take ownership of the profile managements. Recommended to work with entropy_id=0 if the mode is cascade\n"), 
                  pol_cfg->mode, pol_cfg->entropy_id));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function
 *      _bcm_dpp_policer_group_num_get
 * Purpose
 *      Get the number of policers in the group mode
 * Parameters
 *      (in) unit       = unit number
 *      (in) mode       = group mode
 *      (out) npolicers = number of policers
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_dpp_policer_group_num_get(int unit, bcm_policer_group_mode_t mode,
                                  int *npolicers)
{
    int num;
    BCMDNX_INIT_FUNC_DEFS;

    if (mode >= _DPP_POLICER_GROUP_MODES) {
        /* Unsupported mode specified */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: not supported policer mode #%d  \n"), FUNCTION_NAME(),mode));
    }
    num = _dpp_policer_group_num_policers[mode];

    if (num < 0) {
        /* Unsupported mode specified */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: not supported policer mode #%d  \n"), FUNCTION_NAME(),mode));
    } else {
        *npolicers = num;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_dpp_policer_group_core_check
 * Purpose
 *      Check the validity of the core number
 * Parameters
 *      (in) unit       = unit number
 *      (in) core       = core number
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_dpp_policer_group_core_check(int unit, bcm_core_t core)
{
    BCMDNX_INIT_FUNC_DEFS;
    if(SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit) && core != 0){
        /* Unsupported mode specified */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: SINGLE_CORE_MODE: core is out of range #%d  \n"), FUNCTION_NAME(),core));
    }else{
        if ((core >= SOC_DPP_CONFIG(unit)->meter.nof_meter_cores)||(core < 0)) {
            /* Unsupported mode specified */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: core is out of range #%d  \n"), FUNCTION_NAME(),core));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_dpp_meter_to_db_info(
   int unit,
   const bcm_policer_config_t      *pol_cfg_user,
   bcm_dpp_am_meter_entry_t  *sw_state_info
   ) 
{
    /* Copy the config to a temp variable since it may change (times 64) if PACKET_MODE is set. */
    bcm_policer_config_t pol_cfg = *pol_cfg_user;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(pol_cfg_user);
    BCMDNX_NULL_CHECK(sw_state_info);

    /* sw state allocate unique profile according to this infomration */
    sw_state_info->cir = pol_cfg.ckbits_sec;
    sw_state_info->eir = pol_cfg.pkbits_sec;
    sw_state_info->cbs = pol_cfg.ckbits_burst;
    sw_state_info->ebs = pol_cfg.pkbits_burst;
    /* store flags except SW flags*/
    sw_state_info->flags = pol_cfg.flags & (~(BCM_POLICER_WITH_ID | BCM_POLICER_REPLACE | BCM_POLICER_REPLACE_SHARED));
    sw_state_info->mode = pol_cfg.mode;
    sw_state_info->entropy_id = pol_cfg.entropy_id;
    sw_state_info->max_cir = pol_cfg.max_ckbits_sec;
    sw_state_info->max_eir = pol_cfg.max_pkbits_sec;
    sw_state_info->ir_rev_exp = ARAD_PP_INVALID_IR_REV_EXP; /* will be update later for cascade mode */    
    sw_state_info->mark_black = SOC_IS_QUX(unit) ? pol_cfg.mark_drop_as_black : 0;
exit:
    BCMDNX_FUNC_RETURN;
}



/* given API meter info, set
 *   - SW-state meter info
 *   - ppd meter info
 */
STATIC int bcm_petra_meter_to_inter_info(
   int unit,
   const bcm_dpp_am_meter_entry_t *pol_cfg_user,
   SOC_PPC_MTR_BW_PROFILE_INFO    *dpp_profile_info
   ) 
{
    int rv;
    /* Copy the config to a temp variable since it may change (times 64) if PACKET_MODE is set. */
    bcm_dpp_am_meter_entry_t pol_cfg = *pol_cfg_user;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(dpp_profile_info);
    BCMDNX_NULL_CHECK(pol_cfg_user);

    /* Set the packet mode in case of Arad+ only and change the rates from */
    /* kbits per sec to packets (with the defined resolution) per sec */
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        if (pol_cfg.flags & BCM_POLICER_MODE_PACKETS) {
            /* Divide the values by _DPP_POLICER_PACKET_MODE_CORRECTION to fool bcm_petra_meter_to_inter_info into thinking */
            /* that the user has made the multiplication. */
            rv = _bcm_dpp_policer_correct_config_rates(unit, &pol_cfg, NULL, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
#endif /* BCM_88660_A0 */

    /* DPP state, configuration of the device */
    SOC_PPC_MTR_BW_PROFILE_INFO_clear(dpp_profile_info);

    if (pol_cfg.mode == bcmPolicerModeCascade || pol_cfg.mode == bcmPolicerModeCoupledCascade) {
        dpp_profile_info->is_sharing_enabled = TRUE;
    }

    /* commited rate set */
    if (pol_cfg.cir == 0) { /* zero, means disable credits, yet set to minimum cir */
        dpp_profile_info->cir = 0;
        dpp_profile_info->disable_cir = TRUE;
    } else {
        dpp_profile_info->cir = pol_cfg.cir;
        dpp_profile_info->disable_cir = FALSE;
    }

    /* commited max rate, applicable only in hierarchical modes*/
    if (dpp_profile_info->is_sharing_enabled) {
        /* max_cir can exist also when disable_cir = TRUE since it will get extra credits from higher buckets */
        dpp_profile_info->max_cir = pol_cfg.max_cir;
    }

    /* coupling depends on policer mode */
    if (pol_cfg.mode == bcmPolicerModeSrTcm || pol_cfg.mode ==  bcmPolicerModeCoupledTrTcmDs) {
        dpp_profile_info->is_coupling_enabled = TRUE;
    } else {
        dpp_profile_info->is_coupling_enabled = FALSE;
    }
    dpp_profile_info->is_mtr_enabled = 1;

    /* excess/peak rate set */
    if (pol_cfg.eir == 0) { /* zero, means disable credits, yet set to minimum cir */
        dpp_profile_info->eir = 0;
        dpp_profile_info->disable_eir = TRUE;
    } else {
        dpp_profile_info->disable_eir = FALSE;
        dpp_profile_info->eir = pol_cfg.eir;
    }

    if ((dpp_profile_info->disable_eir == FALSE) || (dpp_profile_info->is_coupling_enabled == TRUE) || 
        (pol_cfg.mode == bcmPolicerModeCoupledCascade) || (pol_cfg.mode == bcmPolicerModeCascade)) {
        /* max_eir can exist also when disable_eir = TRUE, because if coupling/(sharing) is enabled the bucket gets extra credits from cir */
        dpp_profile_info->max_eir = pol_cfg.max_eir;
    } else {
        dpp_profile_info->max_eir = dpp_profile_info->eir;
    }

    dpp_profile_info->ebs = _DPP_POLICER_KBITS_TO_BYTES(pol_cfg.ebs); /* kbits to bytes*/
    dpp_profile_info->cbs = _DPP_POLICER_KBITS_TO_BYTES(pol_cfg.cbs); /* kbits to bytes*/
    dpp_profile_info->color_mode = (pol_cfg.flags & BCM_POLICER_COLOR_BLIND) ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;
    dpp_profile_info->is_pkt_truncate = (pol_cfg.flags & BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE) ? TRUE : FALSE;

    /* fix for dpp access */
    if (dpp_profile_info->ebs < SOC_DPP_CONFIG(unit)->meter.min_burst) {
        dpp_profile_info->ebs = SOC_DPP_CONFIG(unit)->meter.min_burst;
    }

    if (dpp_profile_info->cbs < SOC_DPP_CONFIG(unit)->meter.min_burst) {
        dpp_profile_info->cbs = SOC_DPP_CONFIG(unit)->meter.min_burst;
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
      dpp_profile_info->is_packet_mode = (pol_cfg.flags & BCM_POLICER_MODE_PACKETS) ? TRUE : FALSE;
    }
#endif /* BCM_88660_A0 */
    dpp_profile_info->ir_rev_exp = ARAD_PP_INVALID_IR_REV_EXP;

    dpp_profile_info->mark_black = SOC_IS_QUX(unit) ? pol_cfg.mark_black : 0;
exit:
    BCMDNX_FUNC_RETURN;
}


/* given DPP meter info, set
 *   - API info
 */
STATIC int bcm_petra_meter_from_inter_info(
   int unit,
   int core_id,
   SOC_PPC_MTR_METER_ID          meter_id,
   bcm_policer_config_t      *pol_cfg,
   SOC_PPC_MTR_BW_PROFILE_INFO   *dpp_profile_info
   ) 
{
    uint32 global_sharing = 0;
    uint32 rv_soc;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(dpp_profile_info);
    BCMDNX_NULL_CHECK(pol_cfg);

    bcm_policer_config_t_init(pol_cfg);

    if (!_DPP_POLICER_METER_IS_SINGLE_CORE(unit) && SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit)) {
        /* In dual meter core devices (Jericho) working in single device core mode,
           Usage of the second MRPS core is reflected using the MACRO flag*/
        if (core_id == 1) {
            pol_cfg->flags |= BCM_POLICER_MACRO;
        }
    }
    else {
        if (meter_id.group == 1) {
            pol_cfg->flags |= BCM_POLICER_MACRO;
        }
        pol_cfg->core_id = core_id;
    }

    /* commited rate set */
    if (dpp_profile_info->disable_cir == TRUE) { /* zero, means disable credits, yet set to minimum cir */
        pol_cfg->ckbits_sec = 0;
    } else {
        pol_cfg->ckbits_sec = dpp_profile_info->cir;
    }
    if (dpp_profile_info->is_sharing_enabled) {
        pol_cfg->max_ckbits_sec = dpp_profile_info->max_cir;
    }

    /* excess/peak rate set */
    if (dpp_profile_info->disable_eir == TRUE) { /* zero, means disable credits, yet set to minimum cir */
        pol_cfg->pkbits_sec = 0;
    } else {
        pol_cfg->pkbits_sec = dpp_profile_info->eir;
    }

    pol_cfg->pkbits_burst = _DPP_POLICER_BYTES_TO_KBITS(dpp_profile_info->ebs); /* bytes to kbits */
    pol_cfg->ckbits_burst = _DPP_POLICER_BYTES_TO_KBITS(dpp_profile_info->cbs); /* bytes to kbits */
    if (dpp_profile_info->color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND) {
        pol_cfg->flags |= BCM_POLICER_COLOR_BLIND;
    }

    rv_soc = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_policer_global_sharing_get,(unit,core_id,meter_id.id,meter_id.group,&global_sharing)));
    BCMDNX_IF_ERR_EXIT(rv_soc);

    if ((dpp_profile_info->disable_eir == FALSE) || (dpp_profile_info->is_coupling_enabled == TRUE) || 
        (dpp_profile_info->is_sharing_enabled == TRUE)) {
        /* max_eir is valid either when eir is enabled or when coupling/sharing is set */
        pol_cfg->max_pkbits_sec = dpp_profile_info->max_eir;
    }

    /* coupling depends on policer mode */
    if (pol_cfg->pkbits_sec == 0 && dpp_profile_info->is_coupling_enabled == TRUE) {
        pol_cfg->mode = bcmPolicerModeSrTcm;
    } else if (pol_cfg->pkbits_sec == 0 && dpp_profile_info->ebs <= SOC_DPP_CONFIG(unit)->meter.min_burst) {
        pol_cfg->mode = bcmPolicerModeCommitted;
    } else if (dpp_profile_info->is_coupling_enabled == TRUE) {

        pol_cfg->mode = bcmPolicerModeCoupledTrTcmDs;
    } else {
        pol_cfg->mode = bcmPolicerModeTrTcmDs;
    }
    if ((dpp_profile_info->is_sharing_enabled == TRUE)) { /* cascade */
        if (global_sharing) {
            pol_cfg->mode = bcmPolicerModeCoupledCascade;
        } else {
            pol_cfg->mode = bcmPolicerModeCascade;
        }
    }

    if (dpp_profile_info->is_packet_mode) {
        pol_cfg->flags |= BCM_POLICER_MODE_PACKETS;
    }
    if (dpp_profile_info->is_pkt_truncate) {
        pol_cfg->flags |= BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE;
    }
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        /* In packet mode the values are artifically divided by _DPP_POLICER_PACKET_MODE_CORRECTION and so must be scaled back to user values */
        if (dpp_profile_info->is_packet_mode) {
            rv = _bcm_dpp_policer_correct_config_rates(unit, NULL, pol_cfg, FALSE);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
#endif /* BCM_88660_A0 */

    pol_cfg->mark_drop_as_black = SOC_IS_QUX(unit) ? dpp_profile_info->mark_black : 0;
exit:
    BCMDNX_FUNC_RETURN;
}



/* 
 * Given ppd info, allocate bw profile and map meter to it.
 *  
 * change_profile - Should the data of the profile be set ? 
 *  
 * if *new_profile is not 0 then:
 * 1) a. If the profile is not allocated and change_profile then allocate and set data.
 *    b. If the profile is not allocated and !change_profile then return BCM_E_NOT_FOUND.
 *    c. If allocated and change_profile then change the profile data.
 * 2) Change the profile of the meter to *new_profile.
 * 
 * If *new_profile != 0 and the old profile was user managed (entropy_id == 0) then
 * BCM_E_PARAM is returned. 
 * This is to protect the user from accidentally reusing auto managed profiles for 
 * user managed profiles. 
 */
STATIC int bcm_petra_meter_profile_alloc(
   int                            unit,
   int                            core,
   int                            group,
   int                            meter,
   bcm_dpp_am_meter_entry_t      *sw_state_info,
   SOC_PPC_MTR_BW_PROFILE_INFO   *dpp_profile_info,
   int                           *new_profile,
   int                           *old_profile,
   int                            change_profile) 
{
    SOC_PPC_MTR_METER_ID meter_id;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    int rv = BCM_E_NONE;
    SOC_PPC_MTR_BW_PROFILE_INFO exact_dpp_profile_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int is_last = 0;
    int is_allocated = 0;
    uint32 tmp_old_profile;

    BCMDNX_INIT_FUNC_DEFS;

    /* change_profile only valid with new_profile */
    if (change_profile && (*new_profile == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("error: change_profile is only valid in combination with new_profile\n")));
    }
    
    BCMDNX_NULL_CHECK(sw_state_info);

    soc_sand_dev_id = (unit);

    SOC_PPC_MTR_METER_ID_clear(&meter_id);
    meter_id.group = group;
    meter_id.id = meter;

    /* check if old setting was to high rate */
    soc_sand_rv = soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
       soc_sand_dev_id,
       core,
       &meter_id,
       &tmp_old_profile
       );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *old_profile = tmp_old_profile;

    /* if use same high/low rate use exchange */
    /* This is always true for arad */
    /* Check that the profile we need to set isn't being used for auto management. */
    if (*new_profile != 0) {
        bcm_dpp_am_meter_entry_t old_sw_state_info;
        rv = _bcm_dpp_am_template_meter_data_get(unit, core, *new_profile, group, &old_sw_state_info);
        if (rv != BCM_E_NOT_FOUND) {
            BCMDNX_IF_ERR_EXIT(rv);

            if (old_sw_state_info.entropy_id == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Profile %d can not be used since it is already in use by auto management. \n"),
                                                               *new_profile));
            }
        }

    }
    rv = _bcm_dpp_am_template_meter_exchange(unit, core, *old_profile, group, sw_state_info, &is_last, new_profile, change_profile, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    /* update profile:
       porfile 0 cannot be allocated/deallocated (it exists from the start)
       no need to add profile 0 is was already done in init */
    if ((is_allocated == 1) && (*new_profile != 0)) {
        soc_sand_rv = soc_ppd_mtr_bw_profile_add(
           soc_sand_dev_id,
           core,
           group,
           *new_profile,
           dpp_profile_info,           
           &exact_dpp_profile_info,
           &failure_indication);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(failure_indication);
    }

    /* map meter to new profile */
    soc_sand_rv = soc_ppd_mtr_meter_ins_to_bw_profile_map_set(
       soc_sand_dev_id,
       core,
       &meter_id,
       *new_profile
       );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * calculate the ir_rev_exp for sharing mode. Since the rev exp must be the same for all buckets, 
 * it should be calculate according to all rates of the MEF (4 meters)
 */
STATIC int _bcm_petra_rev_exp_for_sharing_enable_mode_calc(
   int unit,
   int size_of_profiles,
   SOC_PPC_MTR_BW_PROFILE_INFO   *dpp_profile_info,
   uint32 *ir_reverse_exponent_2
   ) 
{
    int i;
    uint32 rates[SOC_PPC_BW_PROFILE_MAX_NUMBER_OF_RATES];
    uint32 rates_idx, nof_rates = -1;
    uint32 max_cir_for_calc, max_eir_for_calc;
    uint32 sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    rates_idx = 0;
    for(i=0;i<size_of_profiles;i++)
    {
        /* at least one is used*/
        
        if (FALSE == dpp_profile_info[i].disable_cir){
            /* CIR used*/
            rates[rates_idx] = dpp_profile_info[i].cir;
            ++rates_idx;
        }
        if (FALSE == dpp_profile_info[i].disable_eir){
            /* EIR used*/
            rates[rates_idx] = dpp_profile_info[i].eir;
            ++rates_idx;
        }    
        if (dpp_profile_info[i].max_cir == SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED) { /* max cir unlimited */
            /* max_cir is not used in calculations */
            max_cir_for_calc = dpp_profile_info[i].cir;
        }
        else 
        {
            max_cir_for_calc = dpp_profile_info[i].max_cir;
        }        
        if (max_cir_for_calc >= SOC_DPP_CONFIG(unit)->meter.meter_min_rate)
        {
            /* Max-CIR used */
            rates[rates_idx] = max_cir_for_calc;
            ++rates_idx;
        }
        if (dpp_profile_info[i].max_eir == SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED) { /* max eir unlimited */
            /* max_eir is not used in calculations */
            max_eir_for_calc = dpp_profile_info[i].eir;
        }
        else 
        {
            max_eir_for_calc = dpp_profile_info[i].max_eir;
        }        
        if (max_eir_for_calc >= SOC_DPP_CONFIG(unit)->meter.meter_min_rate){
            /* Max-EIR used */
            rates[rates_idx] = max_eir_for_calc;
            ++rates_idx;
        }
        if(rates_idx == SOC_PPC_BW_PROFILE_MAX_NUMBER_OF_RATES)
        {
            LOG_ERROR(BSL_LS_BCM_POLICER,(BSL_META_U(unit,"rates_idx=%d, out of range \n"),rates_idx));
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
        }
    }
    nof_rates = rates_idx;    
    if(nof_rates == 0)
    {
        *ir_reverse_exponent_2 = 0;
    }
    else
    {
        /* Calculate the best rev_exp (with respect to both rate accuracy and bucket update) for: CIR, EIR, max CIR and max EIR */
        sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate,(unit, rates, nof_rates, ir_reverse_exponent_2));
        BCM_SAND_IF_ERR_EXIT(sand_rv);           
    }
exit:
    BCMDNX_FUNC_RETURN;    
}


/* 
 * get the min/max rates of the 3 other meters
 * since changing the rate of one of the meters, affects all 4 meters (rev-exp is shared)
 * the function will recalculate the rates of the 3 other meters.
 */
STATIC int _bcm_petra_hierarchical_meters_rev_exp_set(
   int unit,
   int core_id,
   int meter_id, /* one of the 4 hierarchical_meters. min/max rates will be of the 3 other meters */
   int group,
   bcm_dpp_am_meter_entry_t *sw_state_info,
   SOC_PPC_MTR_BW_PROFILE_INFO   *dpp_profile_info,
   int *reconfigure_hierarchical_meters
   ) 
{
    int i;
    uint32 profile;
    SOC_PPC_MTR_METER_ID other_meter;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 other_ir_rev_exp = ARAD_PP_INVALID_IR_REV_EXP, current_ir_rev_exp;
    bcm_dpp_am_meter_entry_t sw_state_info_array[4];
    SOC_PPC_MTR_BW_PROFILE_INFO dpp_profile_info_array[4];
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    SOC_PPC_MTR_METER_ID_clear(&other_meter);

    other_meter.group = group;
       
    for (i = 0; i < 4; i++) 
    {
        other_meter.id = meter_id - (meter_id % 4) + i;
        if (other_meter.id == meter_id) 
        {
            memcpy(&dpp_profile_info_array[i], &dpp_profile_info[0], sizeof(SOC_PPC_MTR_BW_PROFILE_INFO));
            memcpy(&sw_state_info_array[i], &sw_state_info[0], sizeof(bcm_dpp_am_meter_entry_t));
        }
        else
        {
            /* get the meters profile */
            soc_sand_rv = soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
               soc_sand_dev_id,
               core_id,
               &other_meter,
               &profile
               );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            soc_sand_rv = _bcm_dpp_am_template_meter_data_get(unit, core_id, profile, group, &sw_state_info_array[i]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* it doesn't matter which one of the other profiles we hold the rev_exp, because they all should be the same */
            other_ir_rev_exp = sw_state_info_array[i].ir_rev_exp;
            /* calc the inter info for all other meters */
            BCMDNX_IF_ERR_EXIT(bcm_petra_meter_to_inter_info(unit,&sw_state_info_array[i],&dpp_profile_info_array[i]));
        }
    }
    BCMDNX_IF_ERR_EXIT(_bcm_petra_rev_exp_for_sharing_enable_mode_calc(unit, 4, dpp_profile_info_array, &current_ir_rev_exp));
    /* case that rev_exp is different for profiles belong to the same group, 
           need to exchange the profile in order to have same rev_exp.
           Here, we just update a flag and the relevant structures. the exchange will be made later*/
    if(other_ir_rev_exp != current_ir_rev_exp)
    {
        * reconfigure_hierarchical_meters = TRUE;  
        
        for (i = 0; i < 4; i++)
        {
            other_meter.id = meter_id - (meter_id % 4) + i;
            if (other_meter.id == meter_id)             
            {
                /* at this point all 4 meters are configure according tothe original one. */
                /* therefore, need to update only the rev exp */
                /* update the original meter with the rev_exp */
                sw_state_info[i].ir_rev_exp = current_ir_rev_exp;
                dpp_profile_info[i].ir_rev_exp = current_ir_rev_exp;
            }
            else
            {
                /* calc the BW_PROFILE_INFO for all other meters */
                memcpy(&sw_state_info[i], &sw_state_info_array[i], sizeof(bcm_dpp_am_meter_entry_t));
                memcpy(&dpp_profile_info[i], &dpp_profile_info_array[i], sizeof(SOC_PPC_MTR_BW_PROFILE_INFO));
                sw_state_info[i].ir_rev_exp = current_ir_rev_exp;
                dpp_profile_info[i].ir_rev_exp = current_ir_rev_exp;
            }
        }
    }
    else
    {
        * reconfigure_hierarchical_meters = FALSE;
        sw_state_info[0].ir_rev_exp = current_ir_rev_exp;
        dpp_profile_info[0].ir_rev_exp = current_ir_rev_exp;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * configure MRPS meter (vs ethernet policer)
 */
STATIC int _bcm_petra_meter_set(
   int unit,
   bcm_policer_config_t *pol_cfg,
   bcm_policer_t *policer_id) 
{
    int 
        rv = BCM_E_NONE,
        am_flags = 0,
        group = 0,
        meter = 0,
        i,
        core = 0, /* default core*/
        act_meter_group, 
        act_meter_core,
        meter_id, 
        meter_id_incr,
        old_profile,
        nof_meters = 1,
        new_profile,
        reconfigure_hierarchical_meters = FALSE;
    uint32 global_sharing = FALSE;
    uint32 rv_soc;
    SOC_PPC_MTR_BW_PROFILE_INFO   dpp_profile_info[4];
    bcm_dpp_am_meter_entry_t  sw_state_info[4];

    uint32 soc_sand_rv;

    /* We expect a non-zero value here only for Arad. */
    int change_profile = (pol_cfg->flags & BCM_POLICER_REPLACE_SHARED) ? TRUE : FALSE;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get meter, group and core from meter_id if BCM_POLICER_WITH_ID */
    if (pol_cfg->flags & BCM_POLICER_WITH_ID) {
        am_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;

        /* meter id: bits 0-12 in PB, 0-14 in ARAD and above, 0-12 for QUX.
           group (0/1): bit 13 in PB, 15 in ARAD and above, 13 for QUX
           core (0/1): bit 16 (Jericho only) */
        meter_id = *policer_id;
        group = _DPP_POLICER_ID_TO_GROUP(unit, meter_id);
        meter = _DPP_POLICER_ID_TO_METER(unit, meter_id);
        core = _DPP_POLICER_ID_TO_CORE(unit, meter_id);
    }

     /* Explicit defenition of group/core when not replacing an existing meter*/
    if (!(pol_cfg->flags & BCM_POLICER_REPLACE)) {
        if (pol_cfg->core_id) {
            core = pol_cfg->core_id;
        }

        if (pol_cfg->flags & BCM_POLICER_MACRO) {
            if (!_DPP_POLICER_METER_IS_SINGLE_CORE(unit) && SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit)) {
                /* In dual meter core devices (Jericho) working in single device core mode,
                   MACRO indicates usage of the second MRPS core*/
                core = 1;
            }
            else {
                group = 1;
            }
        }
    }

    /* Identify modes in which meters are not divided into 2 groups/cores */
    if (!(pol_cfg->flags & BCM_POLICER_WITH_ID)) {

        if (SOC_DPP_CONFIG(unit)->meter.meter_range_mode == 64) {
            group = BCM_DPP_AM_METER_GROUP_ANY;
        }

        if (SOC_DPP_CONFIG(unit)->meter.meter_range_mode == 128) {
            core = BCM_DPP_AM_METER_CORE_ANY;
        }
    }

    if (pol_cfg->mode == bcmPolicerModeCascade || pol_cfg->mode == bcmPolicerModeCoupledCascade) {
        nof_meters = 4; /* allocate 4 meters, first one % 4 = 0 */
        if (pol_cfg->mode == bcmPolicerModeCoupledCascade) {
            global_sharing = 1; /* 4 meters are in hierarchical mode */
        }
    }

    if (pol_cfg->flags & BCM_POLICER_REPLACE) {
        if (nof_meters == 4) {
            rv = bcm_dpp_am_meter_is_alloced(
               unit,
               core,
               group,
               nof_meters,
               meter - (meter % 4));
        } else {
            rv = bcm_dpp_am_meter_is_alloced(
               unit,
               core,
               group,
               nof_meters,
               meter);
        }
        if (rv == BCM_E_NOT_FOUND) {
            LOG_ERROR(BSL_LS_BCM_POLICER,
                      (BSL_META_U(unit,
                                 " replace/destroy nonexist policer %d \n"),
                       *policer_id));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        act_meter_group = group;
        act_meter_core = core;
    
    } else { /* allocate or mark as allocated */
        rv = bcm_dpp_am_meter_alloc(
           unit,
           am_flags,
           core,
           group,
           nof_meters,
           &act_meter_core,
           &act_meter_group,
           &meter);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* if the user just want to modify the profile that the meter use - read the profile property from HW */
    if((pol_cfg->entropy_id != 0) && ((pol_cfg->flags & BCM_POLICER_REPLACE_SHARED) == 0))
    {
        soc_sand_rv = _bcm_dpp_am_template_meter_data_get(unit, act_meter_core, pol_cfg->entropy_id, act_meter_group, &sw_state_info[0]);
        BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        /* calc the inter info for the meter */
        BCMDNX_IF_ERR_EXIT(bcm_petra_meter_to_inter_info(unit,&sw_state_info[0],&dpp_profile_info[0]));
    }
    else
    {
        /* conver API parameters to internal hw/sw structures */
        rv = _bcm_dpp_meter_to_db_info(
           unit,
           pol_cfg,
           &sw_state_info[0]);
        rv = bcm_petra_meter_to_inter_info(
           unit,
           &sw_state_info[0],
           &dpp_profile_info[0]);
        BCMDNX_IF_ERR_EXIT(rv);
    }	

    if (pol_cfg->mode == bcmPolicerModeCascade || pol_cfg->mode == bcmPolicerModeCoupledCascade) {
        /* configure all 4 meters the same */
        for(i = 1 ; i < nof_meters; i++)
        {
            memcpy(&sw_state_info[i], &sw_state_info[0], sizeof(bcm_dpp_am_meter_entry_t));
            memcpy(&dpp_profile_info[i], &dpp_profile_info[0], sizeof(SOC_PPC_MTR_BW_PROFILE_INFO));
        }
        /* IMPORTNAY NOTE: in case the user wants to control the profile id, we cannot match the rev exponent of all 4 meters. */
        /* the reason for that is that we are not allowed to allocate new profile, becaue it is controled by the user */ 
        /* Rev exponent match responsibility is moving to the user */
        if ((pol_cfg->flags & BCM_POLICER_REPLACE) && (pol_cfg->entropy_id == 0))
        {
            /* get 3 other members to determine the rev exp that is most accurate for all meters in the group.
                         HW limitation - all buckets must have the same rev_exp in cascade mode */
            soc_sand_rv = _bcm_petra_hierarchical_meters_rev_exp_set(
               unit,
               act_meter_core,
               meter,
               act_meter_group,
               sw_state_info,
               dpp_profile_info,
               &reconfigure_hierarchical_meters);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);    
        }
        else
        {
            /* calc the rev exp */
            BCMDNX_IF_ERR_EXIT(_bcm_petra_rev_exp_for_sharing_enable_mode_calc(unit, 1, &dpp_profile_info[0], &dpp_profile_info[0].ir_rev_exp));
            sw_state_info[0].ir_rev_exp = dpp_profile_info[0].ir_rev_exp;
            /* update the DB */
            for(i = 1 ; i < nof_meters; i++)
            {
                sw_state_info[i].ir_rev_exp = dpp_profile_info[i].ir_rev_exp = dpp_profile_info[0].ir_rev_exp;    
            }
        }
    }

    /* at this point meter id is allocated */
    *policer_id = _DPP_POLICER_ID_FROM_METER_GROUP_CORE(unit, meter, act_meter_group, act_meter_core);

    /* set profile id to store this information */
    if (pol_cfg->flags & BCM_POLICER_REPLACE) {
        /* in case of replace, only chage the rates of one of the 4 meters group */
        nof_meters = 1;
        if(reconfigure_hierarchical_meters == TRUE)
        {
            nof_meters = 4;
            meter = meter - (meter % 4); /* align to the first meter in the group */
        }
    }
    for (meter_id_incr = 0; meter_id_incr < nof_meters; ++meter_id_incr) {
        new_profile = pol_cfg->entropy_id; /* Expect non-zero only for Arad. */
        rv = bcm_petra_meter_profile_alloc(unit, act_meter_core, act_meter_group, meter + meter_id_incr, &sw_state_info[meter_id_incr], 
                                           &dpp_profile_info[meter_id_incr], &new_profile, &old_profile, change_profile);
        if (rv == BCM_E_NOT_FOUND) {
            /* This means that the old profile did not exist and change_profile != 1, which means there is no way */
            /* to know how to allocate a new profile. */
            LOG_WARN(BSL_LS_BCM_POLICER,
                     (BSL_META_U(unit,
                                 "Cannot set the profile of meter %d to profile %d - profile is not allocated.\n"
                                 "HINT: To allocate it use BCM_POLICER_REPLACE_SHARED with the new configuration for the profile.\n"), 
                      meter, *policer_id));
            
        }

        if (rv != BCM_E_NONE) { /* if some reason fail to allocate profile, release policer if needed */
            LOG_ERROR(BSL_LS_BCM_POLICER,
                      (BSL_META_U(unit,
                                  " fail to allocate profile for policer %d \n"),
                       *policer_id));
            if (!(pol_cfg->flags & BCM_POLICER_REPLACE)) {
                bcm_dpp_am_meter_dealloc(
                   unit,
                   act_meter_core,
                   0,
                   act_meter_group,
                   nof_meters,
                   meter);
            }
        }
    }

    rv_soc = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_policer_global_sharing_set,(unit,act_meter_core,meter,act_meter_group,&global_sharing)));
    BCMDNX_IF_ERR_EXIT(rv_soc);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
Used in init
*/
int
bcm_petra_policer_detach(int unit) 
{
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    /* get rid of allocated structure */
    if(sw_state_sync_db[(unit)].dpp.policer_lock) {
        sal_mutex_destroy(sw_state_sync_db[(unit)].dpp.policer_lock);
    }

    sw_state_sync_db[(unit)].dpp.policer_lock = NULL;

    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_policer_init(int unit) 
{
    int rv = BCM_E_NONE;
    SOC_PPC_MTR_BW_PROFILE_INFO   dpp_profile_info;
    SOC_PPC_MTR_BW_PROFILE_INFO   exact_dpp_profile_info;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    SOC_PPC_MTR_METER_ID meter_id;
    bcm_dpp_am_meter_entry_t  sw_state_info;
    int group_indx;
    SOC_PPC_MTR_GROUP_INFO
       mtr_group_info;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    uint32 bw_profile = 0;
    int core_index;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    sw_state_sync_db[(unit)].dpp.policer_lock = sal_mutex_create("_dpp_meter_unit_lock");
    if (!sw_state_sync_db[(unit)].dpp.policer_lock) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("%s: fail to allocate lock \n"), FUNCTION_NAME()));
    }
    rv = bcm_petra_rate_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);
	
    if (SOC_WARM_BOOT(unit)) {
        BCM_EXIT;
    }

    /* take lock */
    DPP_POLICER_INIT_CHECK(unit);
    DPP_POLICER_UNIT_LOCK(unit);

    SOC_PPC_MTR_BW_PROFILE_INFO_clear(&dpp_profile_info);
    SOC_PPC_MTR_METER_ID_clear(&meter_id);
    SOC_PPC_MTR_GROUP_INFO_clear(&mtr_group_info);

    soc_sand_dev_id = (unit);

    if (!SOC_WARM_BOOT(unit)) {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            /* enable policing
               Jericho: Enable by default*/
            soc_sand_rv = soc_ppd_mtr_eth_policer_enable_set(soc_sand_dev_id, TRUE);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }


        /* allocate profile zero, from high rate with maximum bndwidth */
        /* map all meters to max rate */
        dpp_profile_info.color_mode = SOC_PPC_MTR_COLOR_MODE_AWARE;
        dpp_profile_info.disable_cir = FALSE;
        dpp_profile_info.disable_eir = FALSE;
        dpp_profile_info.ebs = SOC_DPP_CONFIG(unit)->meter.max_burst;
        dpp_profile_info.eir = SOC_DPP_CONFIG(unit)->meter.max_rate;
        dpp_profile_info.cbs = SOC_DPP_CONFIG(unit)->meter.max_burst;
        dpp_profile_info.cir = SOC_DPP_CONFIG(unit)->meter.max_rate;
        dpp_profile_info.max_eir = SOC_DPP_CONFIG(unit)->meter.max_rate;
        dpp_profile_info.max_cir = SOC_DPP_CONFIG(unit)->meter.max_rate;		
        dpp_profile_info.is_fairness_enabled = FALSE;
        dpp_profile_info.is_sharing_enabled = FALSE;
        dpp_profile_info.is_coupling_enabled = TRUE;
        dpp_profile_info.ir_rev_exp = ARAD_PP_INVALID_IR_REV_EXP;
        
        /* sw state allocate unique profile according to this infomration */
        sal_memset(&sw_state_info,0,sizeof(bcm_dpp_am_meter_entry_t));
        sw_state_info.cir = dpp_profile_info.cir;
        sw_state_info.eir = dpp_profile_info.eir;
        sw_state_info.max_cir = dpp_profile_info.max_cir;
        sw_state_info.max_eir = dpp_profile_info.max_eir;
        sw_state_info.cbs = _DPP_POLICER_BYTES_TO_KBITS(dpp_profile_info.cbs);
        sw_state_info.ebs = _DPP_POLICER_BYTES_TO_KBITS(dpp_profile_info.ebs);
        sw_state_info.flags = 0;
        sw_state_info.mode = bcmPolicerModeTrTcmDs;
        sw_state_info.entropy_id = 0;
        sw_state_info.ir_rev_exp = ARAD_PP_INVALID_IR_REV_EXP;
        _DPP_POLICER_METER_CORES_ITER(core_index){
            for (group_indx = 0; group_indx <= 1; ++group_indx) {

                /* set first profile in high rate, for full rate, 'as no disable' */
                    /* there are no high-rate profiles, so profile 0 is configured */
                bw_profile = 0;

                soc_sand_rv = soc_ppd_mtr_bw_profile_add(
                       soc_sand_dev_id,
                       core_index,
                       group_indx,
                       bw_profile,
                       &dpp_profile_info,
                       &exact_dpp_profile_info,
                       &failure_indication
                    );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                SOC_SAND_IF_FAIL_EXIT(failure_indication);

                meter_id.group = group_indx;
            }
        }

        if(SOC_IS_JERICHO(unit)){
            /* init the Eth. poliser local and global default profile*/
            _DPP_POLICER_METER_CORES_ITER(core_index){
                int 
                  policer_indx = 0;

                /* map all meters to max rate */
                BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mtr_eth_policer_glbl_profile_set, (soc_sand_dev_id, core_index, policer_indx, &dpp_profile_info)));
            }
        }
    }

    rv = _bcm_dpp_am_template_meter_init(unit, bw_profile, &sw_state_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * use to create "meter" to create ethernet policer use  group_create
 */
int
bcm_petra_policer_create(int unit, bcm_policer_config_t *pol_cfg,
                         bcm_policer_t *policer_id) 
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    /* verify parameters */
    DPP_POLICER_INIT_CHECK(unit);

    DPP_POLICER_UNIT_LOCK(unit);

    BCMDNX_NULL_CHECK(pol_cfg);

    rv = _bcm_petra_policer_check_config(unit, pol_cfg, 0, *policer_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_meter_set(unit, pol_cfg, policer_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_policer_group_create(
   int unit,
   bcm_policer_group_mode_t mode,
   bcm_policer_t *policer_id,
   int *npolicers) 
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(policer_id); 
    BCMDNX_NULL_CHECK(npolicers); 

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: NOT SUPPORTED ON JERICHO AND QAX!!\nUse: bcm_policer_aggregate_group_create.\n"), FUNCTION_NAME()));
    }

    /* how many policers needed for this mode */
    rv = _bcm_dpp_policer_group_num_get(unit, mode, npolicers);
    BCMDNX_IF_ERR_EXIT(rv);

    DPP_POLICER_UNIT_LOCK(unit);
    rv = bcm_dpp_am_policer_alloc(unit, 0, 0, *npolicers, policer_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(POLICER_ACCESS.policer_group.mode.set(unit, 0, *policer_id, mode));

    *policer_id = _DPP_ETH_POLICER_ID_SET(unit, *policer_id, 0);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_policer_aggregate_group_create(
    int unit, 
    bcm_policer_aggregate_group_info_t *info, 
    bcm_policer_t *policer_id, 
    int *npolicers)
{
    int rv = BCM_E_NONE;
    int core_id, policer_indx;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(policer_id);
    BCMDNX_NULL_CHECK(npolicers);

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("%s: NOT SUPPORTED ON ARAD AND ARAD+!!\nUse: bcm_policer_group_create.\n"), FUNCTION_NAME()));
    }

    /* how many policers needed for this mode */
    rv = _bcm_dpp_policer_group_num_get(unit, info->mode, npolicers);
    BCMDNX_IF_ERR_EXIT(rv);

    /* check core validity */
    rv = _bcm_dpp_policer_group_core_check(unit, info->core);
    BCMDNX_IF_ERR_EXIT(rv);

    DPP_POLICER_UNIT_LOCK(unit);
    core_id = info->core;

    rv = bcm_dpp_am_policer_alloc(unit, core_id, 0, *npolicers, &policer_indx);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(POLICER_ACCESS.policer_group.mode.set(unit, core_id, policer_indx, info->mode));

    *policer_id = _DPP_ETH_POLICER_ID_SET(unit, policer_indx, core_id);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/* get policer-id from opposit group */
int bcm_petra_policer_envelop_create(
   int unit,
   uint32 flag,
   bcm_policer_t macro_flow_policer_id,
   bcm_policer_t *policer_id) 
{

    int act_meter_group, act_meter_core;
    int meter;
    int group;
    int rv = BCM_E_NONE;
    int core;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);

    core = 0;
    group = _DPP_POLICER_ID_TO_GROUP(unit, macro_flow_policer_id);
    meter = _DPP_POLICER_ID_TO_METER(unit, macro_flow_policer_id);

    /* check parameters */
    if (_DPP_IS_ETH_POLICER_ID(unit, macro_flow_policer_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("envelop is not supported for ethernet policer \n")));
    }

    /* flags is ingnored, as micro/macro determined according to meter usage */

    /* take semaphore */
    DPP_POLICER_UNIT_LOCK(unit);

    /* check if policer is allocated */
    rv = bcm_dpp_am_meter_is_alloced(
       unit,
       core,
       group,
       1,
       meter);
    if (rv == BCM_E_NOT_FOUND) {
        LOG_ERROR(BSL_LS_BCM_POLICER,
                  (BSL_META_U(unit, 
                              " replace/destroy nonexist policer %d \n"),
                   macro_flow_policer_id));
            BCMDNX_IF_ERR_EXIT(rv);
    }

    /* alloc from inverse group */
    rv = bcm_dpp_am_meter_alloc(
       unit,
       core,
       0,
       1 - group, /* assuming group: 0,1*/
       1,
       &act_meter_core,
       &act_meter_group,
       &meter);
    BCMDNX_IF_ERR_EXIT(rv);

    *policer_id = _DPP_POLICER_ID_FROM_METER_GROUP_CORE(unit, meter, act_meter_group, act_meter_core);


exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/* used for both, policer/meter according to policer-id*/
int
bcm_petra_policer_set(int unit, bcm_policer_t policer_id, bcm_policer_config_t *pol_cfg) 
{
    int policer_indx, core_id;
    SOC_PPC_MTR_BW_PROFILE_INFO policer_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    /* check parameters */
    DPP_POLICER_INIT_CHECK(unit);

    BCMDNX_NULL_CHECK(pol_cfg);

    core_id = _DPP_ETH_POLICER_CORE_GET(unit, policer_id);
    policer_indx = _DPP_ETH_POLICER_ID_GET(unit, policer_id);

    /* if not ethenet policer then call policer_create with_id*/
    if (!_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
        pol_cfg->flags |= (BCM_POLICER_WITH_ID | BCM_POLICER_REPLACE);
        BCMDNX_IF_ERR_EXIT(bcm_petra_policer_create(unit, pol_cfg, &policer_id));
        BCM_EXIT;
    }

    soc_sand_dev_id = (unit);
    if (policer_indx > _DPP_POLICER_MAX_POLICER_ID || policer_indx < _DPP_POLICER_MIN_POLICER_ID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("policer_id is invalid")));
    }
    /* common check */
    rv = _bcm_petra_policer_check_config(unit, pol_cfg, 1, policer_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* take lock */
    DPP_POLICER_UNIT_LOCK(unit);

    rv = bcm_dpp_am_policer_is_alloc(unit, core_id, 0, policer_indx);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), policer_id));
    }
    /* reset rv */
    rv = BCM_E_NONE;

    SOC_PPC_MTR_BW_PROFILE_INFO_clear(&policer_info);
    if ((pol_cfg->ckbits_sec != 0) && (pol_cfg->ckbits_burst != 0)) {
        policer_info.is_packet_mode = pol_cfg->flags & BCM_POLICER_MODE_PACKETS ? TRUE : FALSE;
        policer_info.is_pkt_truncate = pol_cfg->flags & BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE ? TRUE : FALSE;
        if (policer_info.is_packet_mode == TRUE) {
            policer_info.cir = _bcm_petra_policer_translate_packets_to_kbits(pol_cfg->ckbits_sec);
            policer_info.cbs = _bcm_petra_policer_translate_packets_to_bytes(pol_cfg->ckbits_burst);
        }else{
            policer_info.cir = pol_cfg->ckbits_sec;
            policer_info.cbs = _DPP_POLICER_KBITS_TO_BYTES(pol_cfg->ckbits_burst);
        }
        policer_info.disable_cir = 0;
        if (SOC_IS_ARADPLUS(unit)) { 
          policer_info.color_mode = pol_cfg->flags & BCM_POLICER_COLOR_BLIND ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;
        } 
    }else{
        policer_info.cir = 0;
        policer_info.cbs = 0;
        policer_info.is_packet_mode = 0;
        policer_info.disable_cir = 1;
        policer_info.color_mode = 0;
        policer_info.is_pkt_truncate = 0;
    }
    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mtr_eth_policer_glbl_profile_set, (soc_sand_dev_id, core_id, policer_indx, &policer_info)));
    }else{
        soc_sand_rv = soc_ppd_mtr_eth_policer_glbl_profile_set(soc_sand_dev_id, policer_indx, &policer_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

STATIC int
bcm_petra_eth_policer_destroy(int unit, bcm_policer_t policer_id) {

    bcm_policer_config_t pol_cfg;
    int policer_indx, core_id;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);

    if (!_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("0x%x is not a policer id\n"), policer_id));
    }

    core_id = _DPP_ETH_POLICER_CORE_GET(unit, policer_id);
    policer_indx = _DPP_ETH_POLICER_ID_GET(unit, policer_id);

    if (policer_indx > _DPP_POLICER_MAX_POLICER_ID || policer_indx < _DPP_POLICER_MIN_POLICER_ID) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("incorrect policer id: %d \n"), policer_id));
    }
    bcm_policer_config_t_init(&pol_cfg);
    if (SOC_IS_JERICHO(unit)) {

        pol_cfg.ckbits_sec = 0;
        pol_cfg.ckbits_burst = 0;
        /* These params are irrelevant for global ethernet policer. */
        pol_cfg.pkbits_sec = 0;
        pol_cfg.pkbits_burst = 0;
        pol_cfg.max_ckbits_sec = 0;
        pol_cfg.max_pkbits_sec = 0;

        pol_cfg.mode = bcmPolicerModeSrTcm;

        pol_cfg.flags = BCM_POLICER_REPLACE | BCM_POLICER_WITH_ID;

        DPP_POLICER_UNIT_LOCK(unit);

        rv = bcm_petra_policer_set(unit, policer_id, &pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    }else{
        /* update policer to maximum rate */
        rv = bcm_petra_meter_max_info(
           unit,
           &pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);

        /* These params are irrelevant for global ethernet policer. */
        pol_cfg.pkbits_sec = 0;
        pol_cfg.pkbits_burst = 0;
        pol_cfg.max_ckbits_sec = 0;
        pol_cfg.max_pkbits_sec = 0;
        pol_cfg.mode = bcmPolicerModeSrTcm;

        pol_cfg.flags = BCM_POLICER_REPLACE | BCM_POLICER_WITH_ID;

        DPP_POLICER_UNIT_LOCK(unit);

        rv = bcm_petra_policer_set(unit, policer_id, &pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* MARK AS UNUSED */
    rv = bcm_dpp_am_policer_dealloc(unit, core_id, 0, 1, policer_indx);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

STATIC int
bcm_petra_meter_destroy(int unit, bcm_policer_t policer_id) 
{
    bcm_policer_config_t pol_cfg;
    int rv;
    int meter_id, group_id, core_id;
    int nof_meters = 1, meter_idx;
    bcm_policer_t current_policer_id;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* get the mode to see how many meter_ids to dealloc */
    rv = bcm_petra_policer_get(unit, policer_id, &pol_cfg);
    BCMDNX_IF_ERR_EXIT(rv);

    if (pol_cfg.mode == bcmPolicerModeCascade || pol_cfg.mode == bcmPolicerModeCoupledCascade) {
        nof_meters = 4; /* 4 meters, first one % 4 = 0 */
    }

    /* update policer to maximum rate */
    rv = bcm_petra_meter_max_info(
       unit,
       &pol_cfg);
    BCMDNX_IF_ERR_EXIT(rv);
    pol_cfg.flags  =  BCM_POLICER_REPLACE | BCM_POLICER_WITH_ID;

    DPP_POLICER_UNIT_LOCK(unit);

    /** for cascade mode, need to update 4 meters. (expected that  first one % 4 = 0) */
    for(meter_idx = 0; meter_idx < nof_meters; meter_idx++)
    {
        group_id = _DPP_POLICER_ID_TO_GROUP(unit, policer_id);
        meter_id = _DPP_POLICER_ID_TO_METER(unit, policer_id) + meter_idx;
        core_id = _DPP_POLICER_ID_TO_CORE(unit, policer_id);
        current_policer_id = _DPP_POLICER_ID_FROM_METER_GROUP_CORE(unit, meter_id, group_id, core_id);

        rv = _bcm_petra_meter_set(
           unit,
           &pol_cfg,
           &current_policer_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* mark meter as not used */
    rv = bcm_dpp_am_meter_dealloc(
       unit,
       _DPP_POLICER_ID_TO_CORE(unit, policer_id),
       0,
       _DPP_POLICER_ID_TO_GROUP(unit, policer_id),
       nof_meters,
       _DPP_POLICER_ID_TO_METER(unit, policer_id));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_policer_destroy(int unit, bcm_policer_t policer_id) 
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* lock is taken in called functions */
    if (_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_eth_policer_destroy(unit, policer_id));
    } else {
        BCMDNX_IF_ERR_EXIT(bcm_petra_meter_destroy(unit, _DPP_ETH_METER_ID_GET(unit, policer_id)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/* for eth policer id */
STATIC int
bcm_petra_eth_policer_get(int unit, bcm_policer_t policer_id,
                          bcm_policer_config_t *pol_cfg) 
{
    int core_id,policer_indx;
    SOC_PPC_MTR_BW_PROFILE_INFO policer_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    /* check parameters */

    bcm_policer_config_t_init(pol_cfg);

    soc_sand_dev_id = (unit);

    BCM_DPP_UNIT_CHECK(unit);

    core_id = _DPP_ETH_POLICER_CORE_GET(unit, policer_id);
    policer_indx = _DPP_ETH_POLICER_ID_GET(unit, policer_id);

    if (policer_indx > _DPP_POLICER_MAX_POLICER_ID || policer_indx < _DPP_POLICER_MIN_POLICER_ID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("policer not exist \n")));
    }

    DPP_POLICER_UNIT_LOCK(unit);

    rv = bcm_dpp_am_policer_is_alloc(unit, core_id, 0, policer_indx);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), policer_id));
    }
    /* reset rv */
    rv = BCM_E_NONE;

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mtr_eth_policer_glbl_profile_get, (soc_sand_dev_id, core_id, policer_indx, &policer_info)));
    }else{
        soc_sand_rv = soc_ppd_mtr_eth_policer_glbl_profile_get(soc_sand_dev_id, policer_indx, &policer_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }


    if(policer_info.disable_cir == 0) {
        if (policer_info.is_packet_mode == TRUE) {
            pol_cfg->ckbits_sec   = _bcm_petra_policer_translate_kbits_to_packets(policer_info.cir);
            pol_cfg->ckbits_burst = _bcm_petra_policer_translate_bytes_to_packets(policer_info.cbs);
            pol_cfg->flags |= BCM_POLICER_MODE_PACKETS;
        }else{
            pol_cfg->ckbits_sec   = policer_info.cir;
            pol_cfg->ckbits_burst = _DPP_POLICER_BYTES_TO_KBITS(policer_info.cbs);
        }
    }else{
        pol_cfg->ckbits_sec   = 0;
        pol_cfg->ckbits_burst = 0;
    }
    if (policer_info.is_pkt_truncate) {
        pol_cfg->flags |= BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE;  
    }

    pol_cfg->mode = bcmPolicerModeSrTcm;
    if (policer_info.color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND) {
        pol_cfg->flags |= BCM_POLICER_COLOR_BLIND;
        /* This can only be set in Arad+. */
        BCMDNX_VERIFY(SOC_IS_ARADPLUS(unit));
    }
    
exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * no_exist_print - If 1 then print error if the meter does not exist.
 */
STATIC int
_bcm_petra_meter_get_with_opt(int unit, bcm_policer_t policer_id,
                     bcm_policer_config_t *pol_cfg, int no_exist_print) 
{
    uint32 bw_profile;
    SOC_PPC_MTR_METER_ID meter_id;
    SOC_PPC_MTR_BW_PROFILE_INFO   dpp_profile_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    uint32 tmp_flags = 0;
    int core_id;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_MTR_METER_ID_clear(&meter_id);
    meter_id.group = _DPP_POLICER_ID_TO_GROUP(unit, policer_id);
    meter_id.id = _DPP_POLICER_ID_TO_METER(unit, policer_id);
    core_id = _DPP_POLICER_ID_TO_CORE(unit, policer_id);

    DPP_POLICER_UNIT_LOCK(unit);

    /* check if meter is exist */
    rv = bcm_dpp_am_meter_is_alloced(
       unit,
       core_id,
       meter_id.group,
       1,
       meter_id.id);
    if (rv != BCM_E_EXISTS) { /* meter is not exist */
        if (!no_exist_print) {
            BCM_ERR_EXIT_NO_MSG(rv);
        } else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    soc_sand_dev_id = (unit);

    /* get profile of meter */
    soc_sand_rv = soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
       soc_sand_dev_id,
       core_id,
       &meter_id,
       &bw_profile
       );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* get profile data */
    soc_sand_rv = soc_ppd_mtr_bw_profile_get(
       soc_sand_dev_id,
       core_id,
       meter_id.group,
       bw_profile,
       &dpp_profile_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* IMPORTANT NOTE: pol_cfg is OUT and we use it as INOUT. */
    /* bcm_petra_meter_from_inter_info inits the config, so we must record the flags. */
    tmp_flags = pol_cfg->flags;

    /* map profile data to bcm */
    rv = bcm_petra_meter_from_inter_info(
       unit,
       core_id,
       meter_id,
       pol_cfg,
       &dpp_profile_info);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((tmp_flags & BCM_POLICER_REPLACE_SHARED)) {
        pol_cfg->entropy_id = bw_profile;
    }

exit:
    DPP_POLICER_UNIT_UNLOCK(unit);
    BCMDNX_FUNC_RETURN;
}

/* Default wrapper _bcm_petra_meter_get_with_opt */
STATIC int 
_bcm_petra_meter_get(int unit, bcm_policer_t policer_id,
                     bcm_policer_config_t *pol_cfg) 
{
    return _bcm_petra_meter_get_with_opt(unit, policer_id, pol_cfg, TRUE /* no_exist_print */);
}

/* for meter and policer according to policer-id */
int
bcm_petra_policer_get(int unit, bcm_policer_t policer_id,
                      bcm_policer_config_t *pol_cfg) 
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);

    /* if ethenet policer */
    if (_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
        rv = bcm_petra_eth_policer_get(unit, policer_id, pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* if meter */
        rv = _bcm_petra_meter_get(unit, _DPP_ETH_METER_ID_GET(unit, policer_id), pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_policer_traverse(int unit, bcm_policer_traverse_cb traverse_callback,
                           void *cookie) 
{
    int policer_id;
    bcm_policer_config_t pol_cfg;
    int group_indx;
    int meter;
    int rv = BCM_E_NONE;
    int policer_indx;
    int core_index;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(traverse_callback);
    BCM_DPP_UNIT_CHECK(unit);

    /* iterate over all meters */
    _DPP_POLICER_METER_CORES_ITER(core_index){
        for (group_indx = 0; group_indx <= 1; ++group_indx) {
            /* map all meters to use this profile */
            for (meter = 0; meter < SOC_DPP_CONFIG(unit)->meter.nof_meter_a; ++meter) {

                policer_id = _DPP_POLICER_ID_FROM_METER_GROUP_CORE(unit, meter, group_indx, core_index);
                rv = _bcm_petra_meter_get_with_opt(unit, policer_id, &pol_cfg, FALSE /* no_exist_print */);
                if (rv != BCM_E_NONE) {
                    continue;
                }
                rv = (*traverse_callback)(unit, policer_id, &pol_cfg, cookie);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    
    for (policer_indx = _DPP_POLICER_MIN_POLICER_ID; policer_indx < _DPP_POLICER_NOF_POLICERS; ++policer_indx) {

        policer_id = _DPP_ETH_POLICER_ID_SET(unit, policer_indx, 0);

        rv = bcm_petra_eth_policer_get(unit, policer_id, &pol_cfg);
        if (rv != BCM_E_NONE) {
            continue;
        }
        rv = (*traverse_callback)(unit, policer_id, &pol_cfg, cookie);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int bcm_petra_policer_destroy_cb(
   int unit,
   bcm_policer_t policer_id,
   bcm_policer_config_t *info,
   void *user_data) {

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_policer_destroy(unit, policer_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_policer_destroy_all_by_callback(int unit) 
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_policer_traverse(unit, bcm_petra_policer_destroy_cb, 0));
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_policer_destroy_all(int unit) 
{
    int policer_id;
    int group_indx;
    int meter;
    int rv = BCM_E_NONE;
    int policer_indx;
    int core_indx;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* over meters */
    _DPP_POLICER_METER_CORES_ITER(core_indx){
        for (group_indx = 0; group_indx <= 1; ++group_indx) {
            /* map all meters to use this profile */
            for (meter = 0; meter < SOC_DPP_CONFIG(unit)->meter.nof_meter_a; ++meter) {

                rv = bcm_dpp_am_meter_is_alloced(
                   unit,
                   core_indx,
                   group_indx,
                   1,
                   meter);
                if (rv != BCM_E_EXISTS) { /* meter is not exist */
                    continue;
                }
                policer_id = _DPP_POLICER_ID_FROM_METER_GROUP_CORE(unit, meter, group_indx, core_indx);

                rv = bcm_petra_meter_destroy(unit, policer_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    /* destroy all policers aloocated by the user
       policer_id=0 is default so it is not destroyed */
    _DPP_POLICER_METER_CORES_ITER(core_indx){
        for (policer_indx = _DPP_POLICER_MIN_POLICER_ID; policer_indx < _DPP_POLICER_NOF_POLICERS; ++policer_indx) {

            rv = bcm_dpp_am_policer_is_alloc(unit, core_indx, 0, policer_indx);
            if (rv != BCM_E_EXISTS) {
                continue;
            }

            policer_id = _DPP_ETH_POLICER_ID_SET(unit, policer_indx, core_indx);

            rv = bcm_petra_eth_policer_destroy(unit, policer_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_policer_port_set(
   int unit,
   bcm_port_t port,
   bcm_policer_t policer_id) 
{
    int policer_indx, core_id, core_id_by_port;
    int num_to_alloc = 0;
    bcm_policer_group_mode_t mode;
    int  type_offset[SOC_PPC_NOF_MTR_ETH_TYPES] = { 0 };
    int port_i;
    int plc_indx;
    int soc_sand_rv = 0;
    SOC_PPC_MTR_ETH_TYPE pkt_type;
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;

    uint32 pp_port;

    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core_id_by_port)));

    core_id = _DPP_ETH_POLICER_CORE_GET(unit, policer_id);
    policer_indx = _DPP_ETH_POLICER_ID_GET(unit, policer_id);

    /* policer id means no policing */
    if (policer_id != 0) {
        /* verify parameters */
        if (core_id != core_id_by_port) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG(" port and policer are not from the same core. policer_id=%d, port = %d \n"), policer_id, port));
        }

        /* can be only ethernet policer */
        if (!_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("this is not ethernet policer %d \n"), policer_id));
        }

        DPP_POLICER_UNIT_LOCK(unit);

        /* is this policer allocated */
        rv = bcm_dpp_am_policer_is_alloc(unit, core_id, 0, policer_indx);
        if (rv != BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), policer_id));
        }
        /* reset rv */
        rv = BCM_E_NONE;

        /* policer to mode */
        BCMDNX_IF_ERR_EXIT(POLICER_ACCESS.policer_group.mode.get(unit, core_id, policer_indx, &mode));

        rv = _bcm_dpp_policer_group_num_get(unit, mode, &num_to_alloc);
        BCMDNX_IF_ERR_EXIT(rv);

        /* check that all policers in group is allocated */
        for (plc_indx = policer_indx + 1; plc_indx < policer_indx + num_to_alloc; ++plc_indx) {
            rv = bcm_dpp_am_policer_is_alloc(unit, core_id, 0, plc_indx);
            if (rv != BCM_E_EXISTS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), _DPP_ETH_POLICER_ID_SET(unit, plc_indx, core_id)));
            }
            /* reset rv */
            rv = BCM_E_NONE;
        }

        /* map traffic type to policer offset */
        switch (mode) {
        case bcmPolicerGroupModeSingle: /* all types to base policer */
            break;
        case bcmPolicerGroupModeTrafficType:
            type_offset[SOC_PPC_MTR_ETH_TYPE_UNKNOW_UC] = 0;
            type_offset[SOC_PPC_MTR_ETH_TYPE_KNOW_UC] = 0;
            type_offset[SOC_PPC_MTR_ETH_TYPE_UNKNOW_MC] = 1;
            type_offset[SOC_PPC_MTR_ETH_TYPE_KNOW_MC] = 1;
            type_offset[SOC_PPC_MTR_ETH_TYPE_BC] = 2;
            break;
        case bcmPolicerGroupModeDlfAll:
            type_offset[SOC_PPC_MTR_ETH_TYPE_UNKNOW_UC] = 0;
            type_offset[SOC_PPC_MTR_ETH_TYPE_UNKNOW_MC] = 0;
            type_offset[SOC_PPC_MTR_ETH_TYPE_KNOW_UC] = 1;
            type_offset[SOC_PPC_MTR_ETH_TYPE_KNOW_MC] = 1;
            type_offset[SOC_PPC_MTR_ETH_TYPE_BC] = 1;
            break;
        case bcmPolicerGroupModeTyped:
            type_offset[SOC_PPC_MTR_ETH_TYPE_UNKNOW_UC] = 0;
            type_offset[SOC_PPC_MTR_ETH_TYPE_KNOW_UC] = 1;
            type_offset[SOC_PPC_MTR_ETH_TYPE_UNKNOW_MC] = 2;
            type_offset[SOC_PPC_MTR_ETH_TYPE_KNOW_MC] = 2;
            type_offset[SOC_PPC_MTR_ETH_TYPE_BC] = 3;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("policer is not allocated %d \n"), _DPP_ETH_POLICER_ID_SET(unit, policer_indx, core_id)));
        }
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO(unit)) {
        for (pkt_type = 0; pkt_type < SOC_PPC_NOF_MTR_ETH_TYPES; ++pkt_type) {
            BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mtr_eth_policer_glbl_profile_map_set, (unit, core_id_by_port, pp_port, pkt_type, policer_indx + type_offset[pkt_type])));
        }
    }else{
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            for (pkt_type = 0; pkt_type < SOC_PPC_NOF_MTR_ETH_TYPES; ++pkt_type) {

                soc_sand_rv = soc_ppd_mtr_eth_policer_glbl_profile_map_set(
                   unit,
                   port_i,
                   pkt_type,
                   policer_indx + type_offset[pkt_type]
                   );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_policer_port_get(
   int unit,
   bcm_port_t port,
   bcm_policer_t *policer_id) 
{
    int policer_indx, core_id;
    int port_i;
    int soc_sand_rv = 0;
    int min_policer = _DPP_POLICER_NOF_POLICERS;
    uint32 policer_profile;
    SOC_PPC_MTR_ETH_TYPE pkt_type;
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;
    uint32 pp_port;

    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core_id)));

    DPP_POLICER_UNIT_LOCK(unit);

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO(unit)) {
        for (pkt_type = 0; pkt_type < SOC_PPC_NOF_MTR_ETH_TYPES; ++pkt_type) {
            BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mtr_eth_policer_glbl_profile_map_get, (unit, core_id, pp_port, pkt_type, &policer_profile)));
            if (policer_profile < min_policer) {
                min_policer = policer_profile;
            }
        }
    }else{
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            for (pkt_type = 0; pkt_type < SOC_PPC_NOF_MTR_ETH_TYPES; ++pkt_type) {

                soc_sand_rv = soc_ppd_mtr_eth_policer_glbl_profile_map_get(
                   unit,
                   port_i,
                   pkt_type,
                   &policer_profile
                   );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                if (policer_profile < min_policer) {
                    min_policer = policer_profile;
                }
            }
            break; /* get info from first port */
        }
    }

    /* this is the policer of the port group */
    policer_indx = min_policer;

    /* is this policer allocated, 0 is default */
    if (policer_indx != 0) {
        rv = bcm_dpp_am_policer_is_alloc(unit, core_id, 0, policer_indx);
        if (rv != BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), _DPP_ETH_POLICER_ID_SET(unit, policer_indx, core_id)));
        }
        /* reset rv */
        rv = BCM_E_NONE;
        *policer_id = _DPP_ETH_POLICER_ID_SET(unit, policer_indx, core_id);
    } else {
        *policer_id = 0;
    }


exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}


/*
 * Function
 *      _bcm_petra_policer_color_decision_check
 * Purpose
 *      general check function for color decision param
 * Parameters
 *      (in) unit                      = unit number
 *      (in) *policer_color_decision   = color decision entry
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_petra_policer_color_decision_check(
   int unit,
   bcm_policer_color_decision_t *policer_color_decision)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(policer_color_decision);

    if (SOC_DPP_CONFIG(unit)->meter.sharing_mode == SOC_PPC_MTR_SHARING_MODE_NONE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Color decision could be used only in Serial or Parallel Policer Sharing modes.\n"
                                                          "Change the value of the policer_ingress_sharing_mode soc_property.\n")));
    }

    if (policer_color_decision->flags){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Color decision flags are not supported\n")));
    }

    if (policer_color_decision->policer0_color >= _DPP_POLICER_NOF_COLORS ||
        policer_color_decision->policer1_color >= _DPP_POLICER_NOF_COLORS ||
        policer_color_decision->policer0_update_bucket >= _DPP_POLICER_NOF_COLORS ||
        policer_color_decision->policer1_update_bucket >= _DPP_POLICER_NOF_COLORS ||
        policer_color_decision->policer_color >= _DPP_POLICER_NOF_COLORS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Invalid input, valid color range 0-3\n")));
    }

    if (policer_color_decision->policer0_other_bucket_has_credits >= 2 
        || policer_color_decision->policer1_other_bucket_has_credits >= 2) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Invalid other_bucket_has_credits, valid range 0-1\n")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if (policer_color_decision->policer0_color == bcmPolicerColorInvalid ||
            policer_color_decision->policer1_color == bcmPolicerColorInvalid ||
            policer_color_decision->policer0_update_bucket == bcmPolicerColorInvalid ||
            policer_color_decision->policer1_update_bucket == bcmPolicerColorInvalid) {
        LOG_WARN(BSL_LS_BCM_POLICER,(BSL_META_U(unit,"Warining: bcmPolicerColorInvalid isn't used in Arad+\n")));
        }
    }

    if (SOC_DPP_CONFIG(unit)->meter.color_resolution_mode == SOC_PPC_MTR_COLOR_RESOLUTION_MODE_RED_DIFF &&
        policer_color_decision->policer_color == bcmColorRed) {
            LOG_WARN(BSL_LS_BCM_POLICER,(BSL_META_U(unit,"Warning: policer_color_resolution_mode is set to diffrentiate between Ethernet Policer Red\n"
                                                         "and Metering Red. In this mode, policer_color=3 will be overide by 2.")));
    }
    if(SOC_IS_QUX(unit) && ((policer_color_decision->policer0_mark_drop_as_black != 0 && policer_color_decision->policer0_mark_drop_as_black != 1) || 
        (policer_color_decision->policer1_mark_drop_as_black != 0 && policer_color_decision->policer1_mark_drop_as_black != 1)))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("mark_drop_as_black valid values are: 0 or 1\n")));     
    }
	
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_petra_policer_color_decision_to_inter
 * Purpose
 *      convert color decision to internal struct
 * Parameters
 *      (in) unit                      = unit number
 *      (in) *policer_color_decision   = color decision entry
 *      (out) *internal_info           = internal color decision entry
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_petra_policer_color_decision_to_inter(
   int unit,
   bcm_policer_color_decision_t *policer_color_decision,
   SOC_PPC_MTR_COLOR_DECISION_INFO *internal_info)

{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(policer_color_decision);
    BCMDNX_NULL_CHECK(internal_info);

    internal_info->flags = policer_color_decision->flags;
    internal_info->policer0_color = policer_color_decision->policer0_color;
    internal_info->policer0_other_bucket_has_credits = policer_color_decision->policer0_other_bucket_has_credits;
    internal_info->policer0_mark_black = SOC_IS_QUX(unit) ? policer_color_decision->policer0_mark_drop_as_black : 0;    
    internal_info->policer1_color = policer_color_decision->policer1_color;
    internal_info->policer1_other_bucket_has_credits = policer_color_decision->policer1_other_bucket_has_credits;
    internal_info->policer1_mark_black = SOC_IS_QUX(unit) ? policer_color_decision->policer1_mark_drop_as_black : 0;    
    internal_info->policer0_update_bucket = policer_color_decision->policer0_update_bucket;    
    internal_info->policer1_update_bucket = policer_color_decision->policer1_update_bucket;
    internal_info->policer_color = policer_color_decision->policer_color;
	
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_petra_policer_color_decision_from_inter
 * Purpose
 *      convert color decision to internal struct
 * Parameters
 *      (in) unit                      = unit number
 *      (in) *internal_info            = internal color decision entry
 *      (out) *policer_color_decision  = color decision entry
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_petra_policer_color_decision_from_inter(
   int unit,
   SOC_PPC_MTR_COLOR_DECISION_INFO *internal_info,
   bcm_policer_color_decision_t *policer_color_decision)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(policer_color_decision);
    BCMDNX_NULL_CHECK(internal_info);

    policer_color_decision->flags = internal_info->flags;
    policer_color_decision->policer0_color = internal_info->policer0_color;
    policer_color_decision->policer0_other_bucket_has_credits = (uint8)internal_info->policer0_other_bucket_has_credits;
    policer_color_decision->policer0_mark_drop_as_black = SOC_IS_QUX(unit) ? internal_info->policer0_mark_black : 0;
    policer_color_decision->policer1_color = internal_info->policer1_color;
    policer_color_decision->policer1_other_bucket_has_credits = (uint8)internal_info->policer1_other_bucket_has_credits;
    policer_color_decision->policer1_mark_drop_as_black = SOC_IS_QUX(unit) ? internal_info->policer1_mark_black : 0;	
    policer_color_decision->policer0_update_bucket = internal_info->policer0_update_bucket;
    policer_color_decision->policer1_update_bucket = internal_info->policer1_update_bucket;
    policer_color_decision->policer_color = internal_info->policer_color;

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_policer_color_decision_get(
      int unit,
      bcm_policer_color_decision_t *policer_color_decision)
{
    SOC_PPC_MTR_COLOR_DECISION_INFO internal_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(policer_color_decision);

    /* common checks*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_decision_check(unit, policer_color_decision));
    
    /* check output parameters*/
    if (policer_color_decision->policer0_update_bucket ||
        policer_color_decision->policer1_update_bucket ||
        policer_color_decision->policer_color) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("policer0_update_bucket, policer1_update_bucket and policer_color"
                                                          " shouldn't be set when calling bcm_policer_color_decision_get\n")));
    }

    SOC_PPC_MTR_COLOR_DECISION_INFO_clear(&internal_info);
    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_decision_to_inter(unit, policer_color_decision, &internal_info));

    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_metering_pcd_entry_get,(unit,&internal_info)));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_decision_from_inter(unit, &internal_info, policer_color_decision));

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_policer_color_decision_set(
      int unit,
      bcm_policer_color_decision_t *policer_color_decision)
{
    SOC_PPC_MTR_COLOR_DECISION_INFO internal_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    DPP_POLICER_UNIT_LOCK(unit);
    BCMDNX_NULL_CHECK(policer_color_decision);

    /* common checks*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_decision_check(unit, policer_color_decision));

    SOC_PPC_MTR_COLOR_DECISION_INFO_clear(&internal_info);
    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_decision_to_inter(unit, policer_color_decision, &internal_info));

    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_metering_pcd_entry_set,(unit,&internal_info)));

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_petra_policer_color_resolution_check
 * Purpose
 *      general check function for color resolution param
 * Parameters
 *      (in) unit                           = unit number
 *      (in) *policer_resolution_decision   = color resolution entry
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_petra_policer_color_resolution_check(
   int unit,
   bcm_policer_color_resolution_t *policer_color_resolution)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(policer_color_resolution);

    if (policer_color_resolution->flags){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Color resolution flags are not supported\n")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if (policer_color_resolution->ethernet_policer_color) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Ethernet Policer Color is not supported\n")));
        }
    }

    if (policer_color_resolution->incoming_color >= _DPP_POLICER_NOF_COLORS ||
        policer_color_resolution->ethernet_policer_color >= _DPP_POLICER_NOF_COLORS ||
        policer_color_resolution->policer_color >= _DPP_POLICER_NOF_COLORS ||
        policer_color_resolution->ingress_color >= _DPP_POLICER_NOF_COLORS ||
        policer_color_resolution->egress_color >= _DPP_POLICER_NOF_COLORS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Invalid input, valid color range 0-3\n")));
    }

    if (policer_color_resolution->policer_action >= _DPP_POLICER_NOF_METER_ACTIONS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Invalid input, valid policer action range 0-3\n")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_petra_policer_color_resolution_to_inter
 * Purpose
 *      convert color resolution to internal struct
 * Parameters
 *      (in) unit                      = unit number
 *      (in) *policer_color_resoltuion = color resolution entry
 *      (out) *internal_info           = internal color resolution entry
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_petra_policer_color_resolution_to_inter(
   int unit,
   bcm_policer_color_resolution_t *policer_color_resolution,
   SOC_PPC_MTR_COLOR_RESOLUTION_INFO *internal_info)

{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(policer_color_resolution);
    BCMDNX_NULL_CHECK(internal_info);

    internal_info->flags = policer_color_resolution->flags;
    internal_info->incoming_color = policer_color_resolution->incoming_color;
    internal_info->policer_action = policer_color_resolution->policer_action;
    internal_info->ethernet_policer_color = policer_color_resolution->ethernet_policer_color;
    internal_info->policer_color = policer_color_resolution->policer_color;
    internal_info->egress_color = policer_color_resolution->egress_color;
    internal_info->ingress_color = policer_color_resolution->ingress_color;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_petra_policer_color_resolution_from_inter
 * Purpose
 *      convert color resolution to internal struct
 * Parameters
 *      (in) unit                       = unit number
 *      (in) *internal_info             = internal color resolution entry
 *      (out) *policer_color_resolution = color resolution entry
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_petra_policer_color_resolution_from_inter(
   int unit,
   SOC_PPC_MTR_COLOR_RESOLUTION_INFO *internal_info,
   bcm_policer_color_resolution_t *policer_color_resolution)

{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(policer_color_resolution);
    BCMDNX_NULL_CHECK(internal_info);

    policer_color_resolution->flags = internal_info->flags;
    policer_color_resolution->incoming_color = internal_info->incoming_color;
    policer_color_resolution->policer_action = internal_info->policer_action;
    policer_color_resolution->ethernet_policer_color = internal_info->ethernet_policer_color;
    policer_color_resolution->policer_color = internal_info->policer_color;
    policer_color_resolution->egress_color = internal_info->egress_color;
    policer_color_resolution->ingress_color = internal_info->ingress_color;

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_policer_color_resolution_get(
      int unit,
      bcm_policer_color_resolution_t *policer_color_resolution)
{
    SOC_PPC_MTR_COLOR_RESOLUTION_INFO internal_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(policer_color_resolution);

    /* common checks*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_resolution_check(unit, policer_color_resolution));
    
    /* check output parameters*/
    if (policer_color_resolution->egress_color || policer_color_resolution->ingress_color) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("egress_color and ingress_color shouldn't be set when calling bcm_policer_color_resolution_get\n")));
    }

    SOC_PPC_MTR_COLOR_RESOLUTION_INFO_clear(&internal_info);
    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_resolution_to_inter(unit, policer_color_resolution, &internal_info));

    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_metering_dp_map_entry_get,(unit,&internal_info)));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_resolution_from_inter(unit, &internal_info, policer_color_resolution));

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_policer_color_resolution_set(
      int unit,
      bcm_policer_color_resolution_t *policer_color_resolution)
{
    SOC_PPC_MTR_COLOR_RESOLUTION_INFO internal_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    DPP_POLICER_UNIT_LOCK(unit);

    BCMDNX_NULL_CHECK(policer_color_resolution);

    /* common checks*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_resolution_check(unit, policer_color_resolution));

    SOC_PPC_MTR_COLOR_RESOLUTION_INFO_clear(&internal_info);
    BCMDNX_IF_ERR_EXIT(_bcm_petra_policer_color_resolution_to_inter(unit, policer_color_resolution, &internal_info));

    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_metering_dp_map_entry_set,(unit,&internal_info)));

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}
