 /** \file bcm_int/dnx/stat/stat_pp.h
 *
 * Internal DNX STAT PP
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_STAT_PP_INCLUDED__
/*
 * {
 */
#define _DNX_STAT_PP_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MACROs
 * {
 */

/** Statistics profile is composed of 2 bits representing the
 *  source engine, and profile ID. Below macros help in set
 *  or get for the profile IDs and source engine */
#define STAT_PP_PROFILE_ID_SHIFT   0
#define STAT_PP_PROFILE_ID_MASK    0x3fffffff
#define STAT_PP_PROFILE_ID_GET(profile)   \
    (((profile) >> STAT_PP_PROFILE_ID_SHIFT) & STAT_PP_PROFILE_ID_MASK)
#define STAT_PP_IRPP_PROFILE_MAX_VALUE 31
#define STAT_PP_ETPP_PROFILE_MAX_VALUE 31
#define STAT_PP_ERPP_PROFILE_MAX_VALUE 7
#define STAT_PP_IRPP_META_DATA_MAX_SIZE 16
#define STAT_PP_IRPP_META_DATA_UPDATE_MAX_SIZE 3
#define STAT_PP_IRPP_META_DATA_UPDATE_MAX_SHIFT (STAT_PP_IRPP_META_DATA_MAX_SIZE-1)

#define STAT_PP_IRPP_MAX_CPRS_ITF_ID 9
#define STAT_PP_ETPP_MAX_CPRS_ITF_ID 5


#define STAT_PP_PROFILE_INVALID 0

#define STAT_PP_STAT_IF_NO_STATISTICS_INDICATION 0xF

/** Invalid meter command id */
#define STAT_PP_INGRESS_METER_COMMAND_ID_INVALID    0
#define STAT_PP_METER_MIN_ID 0
#define STAT_PP_METER_IRPP_MAX_ID 2
#define STAT_PP_METER_ETPP_MAX_ID 1

/** statistics engine source origin is stored on 2 of the
 *  profile ID MSBs as internal values */
#define STAT_PP_PROFILE_IRPP_ENCODED_VALUE 0
#define STAT_PP_PROFILE_ERPP_ENCODED_VALUE 1
#define STAT_PP_PROFILE_ETPP_ENCODED_VALUE 2
#define STAT_PP_PROFILE_ENGINE_SHIFT   30
#define STAT_PP_PROFILE_ENGINE_MASK    3
#define STAT_PP_PROFILE_ENCODED_ENGINE_GET(profile)   \
    (((profile) >> STAT_PP_PROFILE_ENGINE_SHIFT) & STAT_PP_PROFILE_ENGINE_MASK)
/** Macros for translating internal engine source values to
 *  the BCM values */
#define STAT_PP_PROFILE_ENGINE_IS_IRPP(profile)   \
        (STAT_PP_PROFILE_ENCODED_ENGINE_GET(profile) == STAT_PP_PROFILE_IRPP_ENCODED_VALUE)
#define STAT_PP_PROFILE_ENGINE_IS_ERPP(profile)   \
        (STAT_PP_PROFILE_ENCODED_ENGINE_GET(profile) == STAT_PP_PROFILE_ERPP_ENCODED_VALUE)
#define STAT_PP_PROFILE_ENGINE_IS_ETPP(profile)   \
        (STAT_PP_PROFILE_ENCODED_ENGINE_GET(profile) == STAT_PP_PROFILE_ETPP_ENCODED_VALUE)
#define STAT_PP_PROFILE_ENGINE_GET(profile) \
        ((STAT_PP_PROFILE_ENGINE_IS_IRPP(profile)) ? bcmStatCounterInterfaceIngressReceivePp : \
        (STAT_PP_PROFILE_ENGINE_IS_ERPP(profile)) ? bcmStatCounterInterfaceEgressReceivePp : \
        bcmStatCounterInterfaceEgressTransmitPp)
#define STAT_PP_ENGINE_PROFILE_SET(encoded_profile, profile, engine) \
    do \
    { \
        if (engine == bcmStatCounterInterfaceIngressReceivePp) \
        { \
            encoded_profile = ((profile & STAT_PP_PROFILE_ID_MASK) << STAT_PP_PROFILE_ID_SHIFT) | \
            (STAT_PP_PROFILE_IRPP_ENCODED_VALUE << STAT_PP_PROFILE_ENGINE_SHIFT); \
        } \
        else if (engine == bcmStatCounterInterfaceEgressReceivePp) \
        { \
            encoded_profile = ((profile & STAT_PP_PROFILE_ID_MASK) << STAT_PP_PROFILE_ID_SHIFT) | \
            (STAT_PP_PROFILE_ERPP_ENCODED_VALUE << STAT_PP_PROFILE_ENGINE_SHIFT); \
        } \
        else if (engine == bcmStatCounterInterfaceEgressTransmitPp) \
        { \
            encoded_profile = ((profile & STAT_PP_PROFILE_ID_MASK) << STAT_PP_PROFILE_ID_SHIFT) | \
            (STAT_PP_PROFILE_ETPP_ENCODED_VALUE << STAT_PP_PROFILE_ENGINE_SHIFT); \
        } \
    } while (0)

#define STAT_PP_ETPP_SWITCH_PROFILE_MAX_PROFILE dnx_data_stat.stat_pp.max_etpp_counting_profile_value_get(unit)
#define STAT_PP_ETPP_SWITCH_PROFILE_SET_MASK 0x1F
#define STAT_PP_ETPP_SWITCH_PROFILE_GET_MASK 0xF8000
#define STAT_PP_ETPP_SWITCH_ID_SHIFT 15
#define STAT_PP_ETPP_SWITCH_ID_GET_SHIFT 0x7FFF
#define STAT_PP_ETPP_SWITCH_CMD_SET(stat_id,stat_pp_profile) \
    do \
    { \
        int orig_stat_pp_profile = STAT_PP_PROFILE_ID_GET(stat_pp_profile); \
        stat_pp_profile = stat_id & STAT_PP_ETPP_SWITCH_PROFILE_SET_MASK; \
        STAT_PP_ENGINE_PROFILE_SET(stat_pp_profile, stat_pp_profile, bcmStatCounterInterfaceEgressTransmitPp); \
        stat_id = (orig_stat_pp_profile << STAT_PP_ETPP_SWITCH_ID_SHIFT) | (stat_id >> 5); \
    } while (0)
#define STAT_PP_ETPP_SWITCH_CMD_GET(stat_id,stat_pp_profile) \
    do \
    { \
        int orig_stat_pp_profile = stat_pp_profile; \
        stat_pp_profile = (stat_id & STAT_PP_ETPP_SWITCH_PROFILE_GET_MASK) >> STAT_PP_ETPP_SWITCH_ID_SHIFT; \
        stat_id = ((stat_id & STAT_PP_ETPP_SWITCH_ID_GET_SHIFT) << 5) | orig_stat_pp_profile;\
    } while (0)

/* Macro for checking if a stat_flag is given and if so --> set stat pp lif fields to 0,
 * indicating the lif table manager to allocate stat_pp result type (to be later set by bcm_stat_gport_set()*/
#define STAT_PP_CHECK_AND_SET_STAT_OBJECT_LIF_FIELDS(flags,stat_flag) \
    do \
    { \
        if (_SHR_IS_FLAG_SET(flags, stat_flag)) \
        { \
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 0); \
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0); \
        } \
    } while (0)

/**
 * \brief - verify function - make sure statistics are present
 *        in the relevant result type
 *
 * \param [in] unit - relevant unit
 * \param [in] dbal_table_id - relevant dbal table id
 * \param [in] dbal_result_type - dbal result type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stat_pp_result_type_verify(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 dbal_result_type);

/**
 * \brief - Verify function, crps validation
 *          If the pp_profile id is valid - get profile info
 *          from HW, call crps module for interface_type,
 *          profile_id, type_id, stat_id validation
 *
 * \param [in] unit - relevant unit
 * \param [in] core_id - relevant core
 * \param [in] stat_id - Object's statistics id
 * \param [in] stat_pp_profile - Statistics profile
 * \param [in] interface_type - Counter interface type
 *
 * \return
 *   Negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_stat_pp_crps_verify(
    int unit,
    bcm_core_t core_id,
    uint32 stat_id,
    int stat_pp_profile,
    int interface_type);

/**
 * \brief - init function, performed during bcm init stage.
 *          Indicate no statistics on default invalid statistic profile 0
 * \param [in] unit - relevant unit
 *
 * \return
 *   Negative in case of an error. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stat_pp_init(
    int unit);

/*
 * }
 */
#endif  /*_DNX_STAT_PP_INCLUDED__*/
