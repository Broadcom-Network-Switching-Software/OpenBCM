/*! \file bcma_bcmpc_phy_reg.c
 *
 * Internal PHY registers access functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmpc/bcmpc_phy.h>

#include <bcma/bcmpc/bcma_bcmpc.h>

int
bcma_bcmpc_phy_reg(int unit, int port, bcma_bcmpc_parse_desc_t *desc,
                   uint32_t reg, uint32_t *data_ptr)
{
    int rv = SHR_E_NONE;
    bcmpc_phy_acc_ovrr_t acc_ovrr, *ovrr = &acc_ovrr;
    uint32_t data = 0;

    if (data_ptr != NULL) {
        data = *data_ptr;
    }

    bcma_bcmpc_desc2ovrr(desc, ovrr);

    if (data_ptr == NULL) {
        /* Register read */
        rv = bcmpc_phy_reg_read(unit, port, reg, &data, ovrr);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFail to read from address %08"PRIx32"\n",
                    BCMA_CLI_CONFIG_ERROR_STR, reg);
            return rv;
        }
        cli_out("    0x%"PRIx32": 0x%04"PRIx32"\n", reg, data);
    } else {
        /* Register write */
        rv = bcmpc_phy_reg_write(unit, port, reg, data, ovrr);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFail to write to address %08"PRIx32"\n",
                    BCMA_CLI_CONFIG_ERROR_STR, reg);
            return rv;
        }
    }

    return rv;
}

int
bcma_bcmpc_phy_raw_reg(int unit, int pm_id, bcma_bcmpc_parse_desc_t *desc,
                       uint32_t reg, uint32_t *data_ptr)
{
    int rv = SHR_E_NONE;
    bcmpc_phy_acc_ovrr_t acc_ovrr, *ovrr = &acc_ovrr;
    uint32_t data = 0;

    if (data_ptr != NULL) {
        data = *data_ptr;
    }
    bcma_bcmpc_desc2ovrr(desc, ovrr);

    if (data_ptr == NULL) {
        /* Register read */
        rv = bcmpc_phy_raw_reg_read(unit, pm_id, reg, &data, ovrr);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFail to read from address %08"PRIx32"\n",
                    BCMA_CLI_CONFIG_ERROR_STR, reg);
            return rv;
        }
        cli_out("    0x%"PRIx32": 0x%04"PRIx32"\n", reg, data);
    } else {
        /* Register write */
        rv = bcmpc_phy_raw_reg_write(unit, pm_id, reg, data, ovrr);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFail to write to address %08"PRIx32"\n",
                    BCMA_CLI_CONFIG_ERROR_STR, reg);
            return rv;
        }
    }

    return rv;
}
