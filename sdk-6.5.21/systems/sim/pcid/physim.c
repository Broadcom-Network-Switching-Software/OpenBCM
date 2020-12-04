/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The part of PCID that simulates a PHY
 *
 * Requires:
 *     
 * Provides:
 *     soc_internal_miim_op
 */

#include <shared/bsl.h>

#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>

#include <soc/cmic.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <bde/pli/verinet.h>

#include "pcid.h"
#include "cmicsim.h"

#define PHY_ID_COUNT		256
#define PHY_ADDR_COUNT		32

static uint16 phy_resetval_5228[PHY_ADDR_COUNT] = {
    0x3000, 0x7809, 0x0040, 0x61d4, 0x01e1, 0x0000, 0x0004, 0x2001,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x1000, 0x0001, 0x0000, 0x0000, 0x0200, 0x0600, 0x0100, 0x0000,
    0x003c, 0x0002, 0x1f00, 0x009a, 0x002c, 0x0000, 0x0000, 0x000b,
};

static uint16 phy_resetval_5402[PHY_ADDR_COUNT] = {
    0x1100, 0x7949, 0x0020, 0x6061, 0x0061, 0x0000, 0x0004, 0x2001,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3000,
    0x0002, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0420, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000,
};

static uint16 phy_regs[PHY_ID_COUNT][PHY_ADDR_COUNT];

static void
phy_reset(uint8 phy_id)
{
    /* This only handles (kind of) the 24+2 switches */

    sal_memcpy(&phy_regs[phy_id],
	       (phy_id < 25) ? phy_resetval_5228 : phy_resetval_5402,
	       sizeof (phy_regs[phy_id]));
}

/*
 * soc_internal_miim_op
 *
 * Look at miim parameters and make up a response.
 */

void
soc_internal_miim_op(pcid_info_t *pcid_info, int read)
{
    uint8           phy_id;
    uint8           phy_addr;
    uint16          phy_wr_data;

    phy_addr = PCIM(pcid_info, CMIC_MIIM_PARAM) >> 24 & 0x1f;
    phy_id = PCIM(pcid_info, CMIC_MIIM_PARAM) >> 16 & 0xff;

    /* Simulate power-on reset */

    if (phy_regs[phy_id][2] == 0 && phy_regs[phy_id][3] == 0) {
	phy_reset(phy_id);
    }

    if (read) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Responding to MIIM read: id=0x%02x addr=0x%02x data=0x%04x\n"),
                     phy_id, phy_addr, phy_regs[phy_id][phy_addr]));
	PCIM(pcid_info, CMIC_MIIM_READ_DATA) = phy_regs[phy_id][phy_addr];
    } else {
	phy_wr_data = PCIM(pcid_info, CMIC_MIIM_PARAM) & 0xffff;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Responding to MIIM write: "
                              "id=0x%02x addr=0x%02x data=0x%04x\n"),
                     phy_id, phy_addr, phy_wr_data));
	if (phy_addr == 0 && (phy_wr_data & 0x8000) != 0) {
	    phy_reset(phy_id);
	} else {
	    phy_regs[phy_id][phy_addr] = phy_wr_data;
	}
    }

    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MIIM_OP_DONE_TST;
}
