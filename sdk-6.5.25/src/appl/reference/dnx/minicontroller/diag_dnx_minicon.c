/**
 * \file diag_dnx_minicon.c
 */

#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDES
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <appl/diag/diag.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <src/appl/reference/dnx/minicontroller/diag_dnx_minicon.h>
#include <src/appl/reference/dnx/minicontroller/armor/diag_dnx_armor.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

sh_sand_man_t sh_dnx_minicon_man = {
    .brief = "minicon appps commands\n",
};

sh_sand_cmd_t sh_dnx_minicon_cmds[] = {
    /*
     * Name | Leaf Action | Node | Options for Leaf | Usage
     */
    {"ARMor", NULL, sh_dnx_armor_cmds, NULL, &sh_dnx_armor_man, NULL, NULL, SH_CMD_CONDITIONAL,
     diag_armor_image_check}
    ,

    {NULL}
};

shr_error_e
diag_minicom_bare_metal_check(
    int unit,
    rhlist_t * list)
{
    uint8 is_default_image = TRUE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_default_image));

    if (is_default_image)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT;
}
