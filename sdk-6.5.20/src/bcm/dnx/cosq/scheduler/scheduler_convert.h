/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/cosq/scheduler_dbal.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_CONVERT_H_INCLUDED_
#define _DNX_SCHEDULER_CONVERT_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <bcm/port.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>

/* DEFINES
 * {
 */

#define DNX_SCH_SE_QUARTER_SIZE(unit) (dnx_data_sch.flow.nof_se_get(unit) / DNX_SCHEDULER_QUARTET_SIZE)

/*
 * SE IDs are organized as the following:
 * - all flow at offset 0 in quartet, then
 * - all flows at offset 1 in quartet, then
 * - all flows at offset 2 in quartet, then
 * - all flows at offset 3 in quartet
 * 
 * So,  offset in quartet = SE_ID / (quarter of Number_of_SEs)
 * quartet index = SE_ID % (quarter of Number_of_SEs)
 */

#define DNX_SCH_CONVERT_SE_ID_TO_QUARTET_NUMBER(unit, se_id) ((se_id) % DNX_SCH_SE_QUARTER_SIZE(unit))
#define DNX_SCH_CONVERT_SE_ID_TO_QUARTET_OFFSET(unit, se_id) ((se_id) / DNX_SCH_SE_QUARTER_SIZE(unit))

/*
 * \brief - convert port+TC to SE ID
 */
shr_error_e dnx_sch_convert_port_tc_to_se_id(
    int unit,
    bcm_port_t logical_port,
    int tc,
    int *core,
    int *se_id);

#endif /** _DNX_SCHEDULER_CONVERT_H_INCLUDED_ */
