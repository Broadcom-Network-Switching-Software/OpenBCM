/*! \file stack.c
 *
 * BCM56780_A0 STACK Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/stack.h>
#include <bcm_int/ltsw/xfs/stack.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_STK

/******************************************************************************
 * Private functions
 */

static int
bcm56780_a0_ltsw_stack_set(int unit,
                           int my_modid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_set(unit, my_modid));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_modport_set(int unit,
                                   int modid,
                                   bcm_port_t dest_port,
                                   bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_modport_set(unit, modid, dest_port, port));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_modport_get(int unit,
                                   int modid,
                                   bcm_port_t dest_port,
                                   bcm_port_t *port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_modport_get(unit, modid, dest_port, port));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_modport_clear(int unit,
                                     int modid,
                                     bcm_port_t dest_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_modport_clear(unit, modid, dest_port));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_l2_if_map(int unit,
                                 int modid,
                                 bcm_port_t dest_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_l2_if_map(unit, modid, dest_port));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_l2_if_clear(int unit,
                                   int modid,
                                   bcm_port_t dest_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_l2_if_clear(unit, modid, dest_port));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_sys_port_get(int unit,
                                    int modid,
                                    bcm_port_t port,
                                    int *sys_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_sys_port_get(unit, modid, port, sys_port));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_sys_port_to_modport_get(int unit,
                                               int sys_port,
                                               int *modid,
                                               bcm_port_t *port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_sys_port_to_modport_get(unit, sys_port, modid, port));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_ifa_system_source_alloc(int unit,
                                               int *sys_source_ifa_probe_copy)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_ifa_system_source_alloc(unit, sys_source_ifa_probe_copy));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_stack_ifa_system_source_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_stack_ifa_system_source_destroy(unit));

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_stack_drv_t bcm56780_a0_ltsw_stack_drv = {
    bcm56780_a0_ltsw_stack_set,
    bcm56780_a0_ltsw_stack_modport_set,
    bcm56780_a0_ltsw_stack_modport_get,
    bcm56780_a0_ltsw_stack_modport_clear,
    bcm56780_a0_ltsw_stack_l2_if_map,
    bcm56780_a0_ltsw_stack_l2_if_clear,
    bcm56780_a0_ltsw_stack_init,
    bcm56780_a0_ltsw_stack_sys_port_get,
    bcm56780_a0_ltsw_stack_sys_port_to_modport_get,
    bcm56780_a0_ltsw_stack_ifa_system_source_alloc,
    bcm56780_a0_ltsw_stack_ifa_system_source_destroy,
};

/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_ltsw_stack_drv_attach(int unit)
{
    return mbcm_ltsw_stack_drv_set(unit, &bcm56780_a0_ltsw_stack_drv);
}

