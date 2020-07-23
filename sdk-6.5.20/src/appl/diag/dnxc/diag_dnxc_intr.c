/** \file diag_dnxc_intr.c
 *
 * diagnostic pack for interrupt
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

/*bcm*/
#include <bcm/switch.h>
/*soc*/
#include <soc/intr.h>
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

 /*
  * LOCAL DEFINEs:
  * {
  */

 /*
  * LOCAL DEFINEs:
  * }
  */

 /*
  * LOCAL TYPEs:
  * }
  */

 /*
  * LOCAL TYPEs:
  * }
  */

/*
 * LOCAL FUNCTIONs:
 * {
 */

/**
 * \brief - display device interrupt
 */
static shr_error_e
cmd_dnxc_intr_enable(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 intr_id;
    int32 port = 0;
    bcm_switch_event_control_t event;
    int32 nof_interrupts;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    /*
     * Get filter input if used
     */
    SH_SAND_GET_UINT32("id", intr_id);
    SH_SAND_GET_INT32("port", port);

    if (intr_id >= nof_interrupts)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid Interrupt number specified! interrupt should be between 0 and %d \n",
                     nof_interrupts - 1);
    }
    event.event_id = intr_id;
    event.index = port;
    event.action = bcmSwitchEventMask;

    SHR_IF_ERR_EXIT(bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_intr_disable(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 intr_id;
    int32 port = 0;
    bcm_switch_event_control_t event;
    int32 nof_interrupts;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    /*
     * Get filter input if used
     */
    SH_SAND_GET_UINT32("id", intr_id);
    SH_SAND_GET_INT32("port", port);

    if (intr_id >= nof_interrupts)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid Interrupt number specified! interrupt should be between 0 and %d \n",
                     nof_interrupts - 1);
    }
    event.event_id = intr_id;
    event.index = port;
    event.action = bcmSwitchEventMask;

    SHR_IF_ERR_EXIT(bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 1));

exit:
    SHR_FUNC_EXIT;
}
/*
 * LOCAL FUNCTIONs:
 * }
 */

/*
 * LOCAL DIAG PACK:
 * {
 */

 /**
  * List of the supported commands, pointer to command function and command usage function.
  */
 /* *INDENT-OFF* */
 static sh_sand_option_t dnxc_intr_enable_options[] = {
    {"id",    SAL_FIELD_TYPE_UINT32,  "Interrupt ID",    NULL, NULL, NULL, SH_SAND_ARG_FREE},
    {"Port",        SAL_FIELD_TYPE_INT32,   "port number",     "0"},
    {NULL}
 };

 static sh_sand_option_t dnxc_intr_disable_options[] = {
    {"id",    SAL_FIELD_TYPE_UINT32,  "Interrupt ID",    NULL, NULL, NULL, SH_SAND_ARG_FREE},
    {"Port",        SAL_FIELD_TYPE_INT32,   "port number",     "0"},
    {NULL}
 };
 static sh_sand_man_t dnxc_intr_enable_man = {
        .brief     = "Enable device interrupt",
        .full      = "Diagnostic to enable interrupts by given interrupt number",
        .synopsis  = "<intr_id> [port]",
        .examples  = "12\n" "122 p=0\n" "id=3"
 };
 static sh_sand_man_t dnxc_intr_disable_man = {
    .brief    = "Disable device interrupt",
    .full     = "Diagnostic to disable interrupts by given interrupt number",
    .synopsis = "<intr_id> [port]",
    .examples = "3\n" "2 p=0\n" "id=12"
};
sh_sand_man_t sh_dnxc_intr_man = {
    .brief    = "Misc facilities to interrupts related diagnostic",
};

sh_sand_cmd_t sh_dnxc_intr_cmds[] = {
    /*keyword,   action,                  command,   options,                     man */
    {"ENAble",   cmd_dnxc_intr_enable,    NULL,      dnxc_intr_enable_options,    &dnxc_intr_enable_man},
    {"DISaBle",  cmd_dnxc_intr_disable,   NULL,      dnxc_intr_disable_options,   &dnxc_intr_disable_man},
    {NULL}
};

#undef BSL_LOG_MODULE
