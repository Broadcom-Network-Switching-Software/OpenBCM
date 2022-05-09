/*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
* 
*/



#include <phymod/phymod.h>
#include <phymod/phymod_util.h>
#include "d5_cfg_seq.h"

int d5_identify(phymod_access_t *sa__, d5_rev_id0_t *rev_id0, d5_rev_id1_t *rev_id1)
{

  return PHYMOD_E_NONE;
}

int d5_tx_rx_polarity_set(phymod_access_t *sa__, uint32_t tx_pol, uint32_t rx_pol)
{

  return PHYMOD_E_NONE;
}

int d5_tx_rx_polarity_get(phymod_access_t *sa__, uint32_t *tx_pol, uint32_t *rx_pol)
{

  return PHYMOD_E_NONE;
}

int d5_dig_lpbk_get(phymod_access_t *sa__, uint32_t *lpbk)
{

  return PHYMOD_E_NONE;
}

int d5_rmt_lpbk_get(phymod_access_t *sa__, uint32_t *lpbk)
{

  return PHYMOD_E_NONE;
}

int d5_lane_soft_reset_read(phymod_access_t *sa__, uint32_t *enable)
{

  return PHYMOD_E_NONE;
}

int d5_pwrdn_set(phymod_access_t *sa__, int tx_rx, int pwrdn)
{

  return PHYMOD_E_NONE;
}

int d5_pwrdn_get(phymod_access_t *sa__, power_status_t *pwrdn)
{

  return PHYMOD_E_NONE;
}

int d5_pmd_loopback_get(phymod_access_t *sa__, uint32_t *enable)
{

  return PHYMOD_E_NONE;
}

int d5_lane_soft_reset_release(phymod_access_t *sa__, uint32_t enable)
{

    return PHYMOD_E_NONE;
}

int d5_lane_soft_reset_release_get(phymod_access_t *sa__, uint32_t *enable)
{

    return PHYMOD_E_NONE;
}


int d5_lane_hard_soft_reset_release(phymod_access_t *sa__, uint32_t enable)
{

    return PHYMOD_E_NONE;
}

int d5_clause72_control(phymod_access_t *sa__, uint32_t cl_72_en)
{

  return PHYMOD_E_NONE;
}

int d5_clause72_control_get(phymod_access_t *sa__, uint32_t *cl_72_en)
{

  return PHYMOD_E_NONE;
}

int d5_pmd_cl72_enable_get(phymod_access_t *sa__, uint32_t *enable)
{

  return PHYMOD_E_NONE;
}

int d5_pmd_cl72_receiver_status(phymod_access_t *sa__, uint32_t *status)
{

  return PHYMOD_E_NONE;
}

int d5_pmd_reset_seq(phymod_access_t *sa__, int pmd_touched)
{
    return (PHYMOD_E_NONE);
}

int d5_signal_detect (phymod_access_t *sa__, uint32_t *signal_detect)
{

  return (PHYMOD_E_NONE);
}

int d5_electrical_idle_set(phymod_access_t *sa__, uint32_t en)
{

  return PHYMOD_E_NONE;
}

int d5_tx_disable_get (phymod_access_t *sa__, uint8_t *enable)
{

  return PHYMOD_E_NONE;
}

int d5_rescal_val_set(phymod_access_t *sa__, uint8_t enable, uint32_t rescal_val)
{

    return PHYMOD_E_NONE;
}

int d5_rescal_val_get(phymod_access_t *sa__, uint32_t *rescal_val)
{

    return PHYMOD_E_NONE;
}

int d5_rx_pmd_restart(phymod_access_t *sa__)
{

    return PHYMOD_E_NONE;
}
