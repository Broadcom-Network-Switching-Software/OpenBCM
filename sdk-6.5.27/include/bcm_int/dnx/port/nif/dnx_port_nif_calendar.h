/** \file dnx_port_nif_calendar.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _PORT_NIF_CALENDAR_INTERNAL_H_
#define _PORT_NIF_CALENDAR_INTERNAL_H_

#include <bcm/types.h>
#include <soc/sand/shrextend/shrextend_error.h>

typedef struct
{
    int level;
    float deviation;
} dnx_port_nif_calendar_distance_check_t;

typedef struct
{
    int limit_value;
    int max_occurrences;
} dnx_port_nif_calendar_min_distance_check_t;

typedef union
{
    dnx_port_nif_calendar_distance_check_t distance_data;
    dnx_port_nif_calendar_min_distance_check_t min_distance_data;
} dnx_port_nif_calendar_check_t;

typedef enum
{
    DNX_PORT_NIF_CALENDAR_CHECK_FIRST = 0,

    DNX_PORT_NIF_CALENDAR_CHECK_SINGLE_DISTANCE = DNX_PORT_NIF_CALENDAR_CHECK_FIRST,

    DNX_PORT_NIF_CALENDAR_CHECK_DOUBLE_DISTANCE = 1,

    DNX_PORT_NIF_CALENDAR_CHECK_TRIPLE_DISTANCE = 2,

    DNX_PORT_NIF_CALENDAR_CHECK_MINIMAL_DISTANCE = 3,

    DNX_PORT_NIF_CALENDAR_CHECK_MACSEC_DISTANCE = 4,

    DNX_PORT_NIF_CALENDAR_CHECK_WB_SINGLE_DISTANCE = 5,

    DNX_PORT_NIF_CALENDAR_CHECK_NOF
} dnx_port_nif_calendar_check_type_e;

typedef enum
{
    DNX_PORT_NIF_CALENDAR_FIRST = 0,

    DNX_PORT_NIF_ARB_RX_QPM_CALENDAR = DNX_PORT_NIF_CALENDAR_FIRST,

    DNX_PORT_NIF_ARB_RX_SCH_CALENDAR = 1,

    DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR = 2,

    DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR = 3,

    DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR = 4,

    DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR = 5,

    DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR = 6,

    DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR = 7,

    DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR = 8,

    DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR = 9,

    DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR = 10,

    DNX_PORT_NIF_OFT_CALENDAR = 11,

    DNX_PORT_NIF_MACSEC0_CALENDAR = 12,

    DNX_PORT_NIF_MACSEC1_CALENDAR = 13,

    DNX_PORT_NIF_FRAMER_SB_RX_CALENDAR = 14,

    DNX_PORT_NIF_FRAMER_SB_TX_CALENDAR = 15,

    DNX_PORT_NIF_FLEXE_PHY_CALENDAR = 16,

    DNX_PORT_NIF_CALENDAR_NOF
} dnx_port_nif_calendar_type_e;

shr_error_e dnx_port_calendar_init(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type);
shr_error_e dnx_port_nif_calendars_db_init(
    int unit);
shr_error_e dnx_port_calendar_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type);
shr_error_e dnx_port_shadow_calendar_build(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type);
shr_error_e dnx_port_calendar_build_only(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar_length,
    uint32 *calendar);
shr_error_e dnx_port_calendar_switch_active_id(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type);
shr_error_e dnx_port_nif_calendar_verify(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *nof_slots_per_port,
    int nof_calendar_clients,
    uint32 *calendar,
    uint32 calendar_length,
    bcm_pbmp_t * clients_tests,
    bcm_pbmp_t check_old_calendar,
    dnx_port_nif_calendar_check_t tests_data[DNX_PORT_NIF_CALENDAR_CHECK_NOF]);
shr_error_e dnx_port_calendar_length_get(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type,
    uint32 *calendar_length);

#endif /* _PORT_NIF_CALENDAR_INTERNAL_H_ */
