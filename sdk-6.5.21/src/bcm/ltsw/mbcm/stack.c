/*! \file stack.c
 *
 * BCM STACK module multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/mbcm/stack.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_STK

static mbcm_ltsw_stack_drv_t *mbcm_ltsw_stack_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_stack_drv_set(int unit, mbcm_ltsw_stack_drv_t *drv)
{
    mbcm_ltsw_stack_drv[unit] = drv;

    return SHR_E_NONE;
}

int
mbcm_ltsw_stack_set(int unit,
                         int my_modid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_set(unit, my_modid));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_modport_set(int unit,
                                    int modid,
                                    bcm_port_t dest_port,
                                    bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_modport_set(unit, modid, dest_port, port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_modport_get(int unit,
                                    int modid,
                                    bcm_port_t dest_port,
                                    bcm_port_t *port)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_modport_get(unit, modid, dest_port, port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_modport_clear(int unit,
                                      int modid,
                                      bcm_port_t dest_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_modport_clear(unit, modid, dest_port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_l2_if_map(int unit,
                          int modid,
                          bcm_port_t dest_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_l2_if_map(unit, modid, dest_port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_l2_if_clear(int unit,
                            int modid,
                            bcm_port_t dest_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_l2_if_clear(unit, modid, dest_port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_init ?
         mbcm_ltsw_stack_drv[unit]->stack_init(unit) :
         SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_sys_port_get(int unit,
                             int modid,
                             bcm_port_t port,
                             int *sys_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_sys_port_get(
                                      unit, modid, port, sys_port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_sys_port_to_modport_get(int unit,
                                        int sys_port,
                                        int *modid,
                                        bcm_port_t *port)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_sys_port_to_modport_get(
                                      unit, sys_port, modid, port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_ifa_system_source_alloc(int unit,
                                        int *sys_source_ifa_probe_copy)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_ifa_system_source_alloc
            (unit, sys_source_ifa_probe_copy));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_stack_ifa_system_source_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mbcm_ltsw_stack_drv[unit], SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_drv[unit]->stack_ifa_system_source_destroy(unit));

exit:
    SHR_FUNC_EXIT();
}
