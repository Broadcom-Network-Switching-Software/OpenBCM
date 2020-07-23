/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        lb.h
 * Purpose:     LB internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_LB_H_
#define   _BCM_INT_DPP_LB_H_

#include <soc/dpp/TMC/tmc_api_link_bonding.h>

#define BCM_LB_PORT_INVALID          -1
#define BCM_LB_LBG_INVALID           0xFFFFFFFF

#define BCM_LB_VALUE_MAX_CHECK(val, max, para_name) {\
    if ((val) > (max)) { \
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, \
            (_BSL_SOCDNX_MSG("Fail(%s) parameter(%s=%d) should be less than %d\n"), \
                             soc_errmsg(BCM_E_PARAM), para_name, val, (max+1)));\
    } \
}

#define BCM_LB_VALUE_CHECK(val, min, max, para_name) \
    if ((val) < (min) || (val) > (max)) { \
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, \
            (_BSL_SOCDNX_MSG("Fail(%s) parameter(%s=%d) should be in range of [%d, %d]\n"), \
                             soc_errmsg(BCM_E_PARAM), para_name, val, min, max));\
    }

#define LB_INFO_ACCESS               sw_state_access[unit].dpp.bcm.linkbonding

typedef struct bcm_dpp_lb_info_s {
    SHR_BITDCL   *lbg_valid_bmp;
    int           modem_to_ports[SOC_TMC_LB_NOF_MODEM];
    uint32        port_to_lbg[SOC_MAX_NUM_PORTS];
    uint32        modem_to_ing_lbg[SOC_TMC_LB_NOF_MODEM];
    uint32        modem_to_egr_lbg[SOC_TMC_LB_NOF_MODEM];
} bcm_dpp_lb_info_t;

/* sw_state_access[unit].dpp.bcm.lb.cosq_res_info.queue_unicast_total_dynamic_res.bit_set */

extern int bcm_petra_lb_init(int unit);
extern int bcm_petra_lb_detach(int unit);

#endif /* _BCM_INT_DPP_LB_H_ */

