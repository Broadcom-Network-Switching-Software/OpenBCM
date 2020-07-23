
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_internal_hw_entity_porting.h>

shr_error_e
dbal_init_hw_entity_porting_init(
int unit,
hl_porting_info_t * hl_porting_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_init_hw_entity_porting_hw_entity_porting_init(unit, hl_porting_info));
exit:
    SHR_FUNC_EXIT;
}
