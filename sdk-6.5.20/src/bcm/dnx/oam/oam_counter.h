/** \file oam_counter.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _OAM_COUNTER_INCLUDED__
/*
 * {
 */
#define _OAM_COUNTER_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * }
 */

/*
 *MP_TYPE definitions for ease of use
 */
    /** MDL of the packet is below the lowest MEP level on lif  */
#define BELOW_LOWER_MEP          DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP
    /** MDL of the packet matches the 3rd MEP level on lif  */
#define ACTIVE_MATCH_LOW         DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW
    /** MDL of the packet matches the 3rd MEP level on lif on the opposite direction  */
#define PASSIVE_MATCH_LOW        DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW
    /** MDL of the packet is between the 2nd and 3rd MEP level on lif  */
#define BETWEEN_MIDDLE_AND_LOW   DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIDDLE_AND_LOW
    /** MDL of the packet matches the 2nd MEP level on lif  */
#define ACTIVE_MATCH_MIDDLE      DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE
    /** MDL of the packet matches the 2nd MEP level on lif on the opposite direction  */
#define PASSIVE_MATCH_MIDDLE     DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE
    /** MDL of the packet is between the Highest and the 2nd MEP level on lif  */
#define BETWEEN_HIGH_AND_MIDDLE  DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE
    /** MDL of the packet matches the highest MEP level on lif  */
#define ACTIVE_MATCH_HIGH        DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH
    /** MDL of the packet matches the highest MEP level on lif on the opposite direction  */
#define PASSIVE_MATCH_HIGH       DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH
    /** MDL of the packet is higher than the highest MEP level on lif  */
#define ABOVE_UPPER_MEP          DBAL_ENUM_FVAL_MP_TYPE_ABOVE_UPPER_MEP
    /** MDL of the packet matches than the highest MIP level on lif  */
#define MIP_MATCH                DBAL_ENUM_FVAL_MP_TYPE_MIP_MATCH
    /** MDL of the packet is between a MIP and a MEP */
#define BETWEEN_MIP_AND_MEP      DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIP_AND_MEP
    /** The packet matches the BFD endpoint on lif  */
#define MP_TYPE_BFD              DBAL_ENUM_FVAL_MP_TYPE_BFD
    /** MDL of the packet matches a MEP without a counter on lif */
#define ACTIVE_MATCH_NO_COUNTER  DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_NO_COUNTER
    /** MDL of the packet matches a MEP without a counter on lif on the opposite direction  */
#define PASSIVE_MATCH_NO_COUNTER DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_NO_COUNTER
    /** Reserved  */
#define RESERVED                 DBAL_ENUM_FVAL_MP_TYPE_RESERVED

/** May be used in  dnx_oam_lif_db_access_profile_en_set().
 *  Signifies to set the parameters for all
 *  OAM-LIF-DB-ACCESS */
#define DNX_OAM_COUNTER_OAM_LIF_DB_ACCESS_ALL 3

/**
 * \brief -
 * Initialize all oam counters functionality 
 *  
 * Call further init functions for different 
 * counter resolution stages if required
 *
 * \par DIRECT INPUT:
 *    \param [in] unit -     Relevant unit.
 * 
 * \par DIRECT OUTPUT:
 *    \retval Error indication according to shr_error_e enum
 */

shr_error_e dnx_oam_counters_init(
    int unit);

/**
 * \brief -
 * Get the counter range. The counter index from the lifs will 
 * be checked against this range. According to the result, 
 * counting per PCP functionality will be applied. 
 *  
 * \param [in] unit -     Relevant unit.
 * \param [in] is_ingress - Selects the table ( may be 
 *        ingress or egress table )
 * \param [out] counter_range_max - upper boundary of OAM counter range
 * \param [out] counter_range_min - lower boundary of OAM counter range
 *
 * \retval Counter range register value 
 *  
 * \see dnx_oam_counter_global_range_set()
 */
shr_error_e dnx_oam_counter_global_range_get(
    int unit,
    int is_ingress,
    uint32 *counter_range_min,
    uint32 *counter_range_max);

/**
 * \brief -
 * Set the counter range. The counter index from the lifs will 
 * be checked against this range. According to the result, 
 * counting per PCP functionality will be applied. 
 * This function will not update values of counter base, shift 
 * and use_PCP fields.
 *  
 * \param [in] unit -     Relevant unit.
 * \param [in] is_ingress - Selects the table ( may be 
 *        ingress or egress table )
 * \param [in] counter_range_max - upper boundary of OAM counter
 *        range
 * \param [in] counter_range_min - lower boundary of OAM counter
 *        range
 *
 * \retval Counter range register value 
 *  
 * \see dnx_oam_counter_global_range_get()
 */

shr_error_e dnx_oam_counter_global_range_set(
    int unit,
    int is_ingress,
    int counter_range_min,
    int counter_range_max);

/**
 * \brief -
 * Set the counter_disable (AKA profile_en AKA 
 * profile_en_counter_disable) configuration. Per packet, 
 * disable-counter is determined by: 
 * profile_en_counter_disable | counter_disable | OAMA.dis_cnt
 *  
 * For example to allow SLM on a given LIF, set profile_en =1 
 * for the LIF's mp-profile for each LIF-DB access (0,1,2), for 
 * both ingress and egress (in other words 6 calls). 
 *  
 * \param [in] unit -     Relevant unit.
 * \param [in] is_ingress - ingress or egress table
 * \param [in] lif_db_access - access number to OAM-LIF-DB. May 
 *        be 0, 1, 2, or for setting on all 3 use
 *        DNX_OAM_COUNTER_OAM_LIF_DB_ACCESS_ALL
 * \param [in] mp_profile - output of OAM-LIF-DB. possible 
 *        values: 0-16.
 * \param [in] packet_is_oam - packet is oam 
 * \param [in] counter_disable - 1 indicates disable counter. 
 *  
 * \retval Error indication according to shr_error_e enum 
 *  
 * \see dnx_oam_lif_db_access_profile_en_get()
 */
shr_error_e dnx_oam_lif_db_access_profile_en_counter_disable_set(
    int unit,
    int is_ingress,
    int lif_db_access,
    int mp_profile,
    int is_oam,
    uint32 counter_disable);

/**
 * \brief -
 * Get the counter_disable (AKA profile_en AKA profile_en_counter_disable) 
 * configuration according to input. Per packet (and 
 * LIF_DB_access), disable-counter is determined by: 
 * profile_en_counter_disable | counter_disable | OAMA.dis_cnt 
 *  
 *  
 * \param [in] unit -     Relevant unit.
 * \param [in] is_ingress - Selects the table ( may be 
 *        ingress or egress table )
 * \param [in] lif_db_access access number to OAM-LIF-DB. May 
 *        be 0, 1, or 2.
 * \param [in] mp_profile - output of OAM-LIF-DB, possible 
 *        values: 0-16.
 * \param [in] packet_is_oam - packet is oam 
 * \param [out] counter_disable - 1 indicates disable 
 *        counter.
 *  
 * \retval Error indication according to shr_error_e enum 
 *  
 * \see dnx_oam_lif_db_access_profile_en_set()
 */
shr_error_e dnx_oam_lif_db_access_profile_en_counter_disable_get(
    int unit,
    int is_ingress,
    int lif_db_access,
    int mp_profile,
    int packet_is_oam,
    uint32 *counter_disable);

#endif/*_OAM_COUNTER_INCLUDED__*/
