/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include "merlin16/tier1/merlin16_diag.h"
#include "merlin16/tier1/merlin16_config.h"


#ifdef PHYMOD_MERLIN16_SUPPORT


STATIC int merlin16_diagnostics_eyescan_run_uc(const phymod_phy_access_t* phy, uint32_t flags)
{
    int                 rc = PHYMOD_E_NONE;
    uint32_t            stripe[64];
    uint16_t            status;
    int                 i,ii;
    phymod_phy_access_t phy_copy;
    struct merlin16_uc_core_config_st core_cfg;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* do not run eyescan for 10Gx4 case */
    if ((PHYMOD_EYESCAN_F_ENABLE_GET(flags)) ||
        (PHYMOD_EYESCAN_F_PROCESS_GET(flags))) {
        PHYMOD_IF_ERR_RETURN(merlin16_get_uc_core_config(&(phy_copy.access), &core_cfg));
        if ((core_cfg.vco_rate_in_Mhz == 12500) && (phy->access.lane_mask == 0xf)) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("eyescan is not supported")));
        }
    }

    for (i = 0; i < PHYMOD_CONFIG_MAX_LANES_PER_CORE; i++) { /* Loop for all lanes. */
        if ((phy->access.lane_mask & (1 << i)) == 0) continue;

        phy_copy.access.lane_mask = (phy->access.lane_mask & (1 << i));

        if (PHYMOD_EYESCAN_F_ENABLE_GET(flags)) {
            rc = merlin16_meas_eye_scan_start(&(phy_copy.access), EYE_SCAN_HORIZ);
            if (rc != PHYMOD_E_NONE) {
                PHYMOD_IF_ERR_RETURN(merlin16_meas_eye_scan_done(&(phy_copy.access)));
            }
            if (!PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_GET(flags)) {
                PHYMOD_USLEEP(100000);
            }
        }

        if (PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {
            for (ii = 31; ii >= -31; ii = ii-1) {
                if (ii == 31) {
                    PHYMOD_DIAG_OUT(("\n\n\n"));
                    PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                    PHYMOD_DIAG_OUT(("    | EYESCAN Phy: 0x%03x lane_mask: 0x%02x                                 |\n", phy_copy.access.addr, phy_copy.access.lane_mask));
                    PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                    /*display eyescan header*/
                    PHYMOD_IF_ERR_RETURN(merlin16_display_eye_scan_header(1));
                }

                rc  = merlin16_read_eye_scan_stripe(&(phy_copy.access), stripe, &status);
                if (rc != PHYMOD_E_NONE) {
                    PHYMOD_IF_ERR_RETURN(rc);
                }
                PHYMOD_IF_ERR_RETURN(merlin16_display_eye_scan_stripe(&(phy_copy.access), ii, &stripe[0]));

                PHYMOD_DIAG_OUT(("\n"));
            }

            if (rc == PHYMOD_E_NONE) {
                PHYMOD_IF_ERR_RETURN(merlin16_display_eye_scan_footer(1));
                PHYMOD_DIAG_OUT(("\n"));
            }
        }

        if (PHYMOD_EYESCAN_F_DONE_GET(flags)) {
            PHYMOD_IF_ERR_RETURN(merlin16_meas_eye_scan_done(&(phy_copy.access)));
        }
    }

    return PHYMOD_E_NONE;
}

int merlin16_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{        

    switch(mode) {
        case phymodEyescanModeFast:
             return merlin16_diagnostics_eyescan_run_uc(phy, flags);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }
        
    return PHYMOD_E_NONE;
    
}

#endif /* PHYMOD_MERLIN16_SUPPORT */
