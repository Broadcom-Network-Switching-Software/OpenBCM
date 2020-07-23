/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * diag_dnx_group.h
 *
 *  Created on: Aug 21, 2018
 *      Author: si888124
 */

#ifndef DIAG_DNX_GROUP_H_INCLUDED
#define DIAG_DNX_GROUP_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */

extern sh_sand_cmd_t sh_dnx_tm_sch_group_cmds[];
extern sh_sand_man_t sh_dnx_tm_sch_group_man;

/*************
 * FUNCTIONS *
 */

extern shr_error_e dnx_sch_dbal_credit_counter_config_set(
    int unit,
    int core_id,
    dbal_enum_value_field_sch_credit_counter_type_e counter_type,
    uint32 is_group_match,
    uint32 is_flow_match,
    uint32 group,
    uint32 flow);

extern shr_error_e dnx_sch_dbal_credit_counter_get(
    int unit,
    int core_id,
    uint32 *counter_val,
    uint32 *is_ovf);

#endif /* DIAG_DNX_GROUP_H_INCLUDED */
