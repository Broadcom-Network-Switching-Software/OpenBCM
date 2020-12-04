/*! \file bcma_testutil_flex.c
 *
 * Flex port operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcma/test/util/bcma_testutil_flex.h>
#include <bcma/test/util/bcma_testutil_drv.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST


int
bcma_testutil_drv_flex_pc_port_add(int unit, uint32_t lport, uint32_t speed,
                                   uint8_t num_lanes, uint32_t max_frame_size,
                                   char *fec_mode, char *speed_vco)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_flex_lt_op_t *flex_lt_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->flex_lt_op_get, SHR_E_UNAVAIL);

    flex_lt_ops = drv->flex_lt_op_get(unit);
    SHR_NULL_CHECK(flex_lt_ops->flex_pc_port_add, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (flex_lt_ops->flex_pc_port_add(unit, lport, speed, num_lanes,
                                       max_frame_size, fec_mode, speed_vco));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_flex_pc_port_delete(int unit, uint32_t lport)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_flex_lt_op_t *flex_lt_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->flex_lt_op_get, SHR_E_UNAVAIL);

    flex_lt_ops = drv->flex_lt_op_get(unit);
    SHR_NULL_CHECK(flex_lt_ops->flex_pc_port_delete, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (flex_lt_ops->flex_pc_port_delete(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_flex_pc_port_phys_map_add(int unit, uint32_t lport,
                                            uint32_t pport)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_flex_lt_op_t *flex_lt_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->flex_lt_op_get, SHR_E_UNAVAIL);

    flex_lt_ops = drv->flex_lt_op_get(unit);
    SHR_NULL_CHECK(flex_lt_ops->flex_pc_port_phys_map_add, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (flex_lt_ops->flex_pc_port_phys_map_add(unit, lport, pport));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_flex_pc_port_phys_map_delete(int unit, uint32_t lport)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_flex_lt_op_t *flex_lt_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->flex_lt_op_get, SHR_E_UNAVAIL);

    flex_lt_ops = drv->flex_lt_op_get(unit);
    SHR_NULL_CHECK(flex_lt_ops->flex_pc_port_phys_map_delete, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (flex_lt_ops->flex_pc_port_phys_map_delete(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_flex_l2p_map_get(int unit, bcmdrd_pbmp_t pbmp,
                                   uint32_t array_size, uint32_t *l2p_map)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_flex_lt_op_t *flex_lt_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->flex_lt_op_get, SHR_E_UNAVAIL);

    flex_lt_ops = drv->flex_lt_op_get(unit);
    SHR_NULL_CHECK(flex_lt_ops->flex_l2p_map_get, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (flex_lt_ops->flex_l2p_map_get(unit, pbmp, array_size, l2p_map));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_flex_do_flex(int unit, bcmdrd_pbmp_t lgc_pbmp,
                               bcmdrd_pbmp_t phy_pbmp_down,
                               bcmdrd_pbmp_t phy_pbmp_up,
                               uint32_t num_pms, uint32_t str_size,
                               char *pm_mode_str,
                               bcmdrd_pbmp_t *lgc_pbmp_down,
                               bcmdrd_pbmp_t *lgc_pbmp_up)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_flex_traffic_op_t *flex_traffic_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->flex_traffic_op_get, SHR_E_UNAVAIL);

    flex_traffic_ops = drv->flex_traffic_op_get(unit);
    SHR_NULL_CHECK(flex_traffic_ops->flex_do_flex, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (flex_traffic_ops->flex_do_flex(unit, lgc_pbmp,
                                        phy_pbmp_down, phy_pbmp_up,
                                        num_pms, str_size, pm_mode_str,
                                        lgc_pbmp_down, lgc_pbmp_up));

exit:
    SHR_FUNC_EXIT();
}
