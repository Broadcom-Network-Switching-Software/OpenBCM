/*
 * ! \file ctest_dnx_port_util.h Contains common dnx ctest port utilities
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef CTEST_DNX_PORT_UTILS_H_INCLUDED
#define CTEST_DNX_PORT_UTILS_H_INCLUDED

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_rhlist.h>
#include <bcm/port.h>

shr_error_e ctest_dnx_port_ucode_port_type_group_get(
    int unit,
    int speed,
    int lanes,
    char **ucode_port_type_p,
    char **port_group_p);

void ctest_dnx_pm_first_last_phy_get(
    int unit,
    int pm_id,
    int *first_phy,
    int *last_phy);

shr_error_e ctest_dnx_port_free_pms_get(
    int unit,
    int is_cdu,
    int core,
    bcm_pbmp_t reserved_port_pbmp,
    bcm_pbmp_t reserved_phy_pbmp,
    bcm_pbmp_t * free_pms);

shr_error_e ctest_dnx_port_first_last_phy_get(
    int unit,
    int port,
    int *first_phy,
    int *last_phy);

shr_error_e ctest_dnx_port_type_pbmp_get(
    int unit,
    char *port_type,
    bcm_pbmp_t * pbmp);

#endif /* CTEST_DNX_PORT_UTILS_H_INCLUDED */
