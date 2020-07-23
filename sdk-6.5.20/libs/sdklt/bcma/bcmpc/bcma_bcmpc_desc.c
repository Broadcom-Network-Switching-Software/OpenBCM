/*! \file bcma_bcmpc_desc.c
 *
 * Utility functions for information convert.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/bcmpc/bcma_bcmpc.h>

int
bcma_bcmpc_desc2ovrr(bcma_bcmpc_parse_desc_t *desc, bcmpc_phy_acc_ovrr_t *ovrr)
{
    if (ovrr == NULL) {
        return -1;
    }

    bcmpc_phy_acc_ovrr_t_init(ovrr);

    if (desc == NULL) {
        return 0;
    }

    ovrr->lane_index = desc->lane_index;

    if (desc->pll_index < 2) {
        ovrr->pll_index = desc->pll_index;
    }
    return 0;
}

int
bcma_bcmpc_parse_desc_t_init(bcma_bcmpc_parse_desc_t *desc)
{
    if (desc == NULL) {
        return -1;
    }
    sal_memset(desc, 0, sizeof(*desc));
    desc->lane_index=NO_LN_IDX_SPECIFIED;

    return 0;
}
