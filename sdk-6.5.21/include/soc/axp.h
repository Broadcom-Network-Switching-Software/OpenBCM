/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * AXP management routines
 */

#ifndef _SOC_AXP_H
#define _SOC_AXP_H

#include <soc/types.h>


typedef enum soc_axp_nlf_type_e {
    SOC_AXP_NLF_PASSTHRU = 0,
    SOC_AXP_NLF_WLAN_DECAP,
    SOC_AXP_NLF_WLAN_ENCAP,
    SOC_AXP_NLF_SM,         /* Signature match */
    SOC_AXP_NLF_NUM         /* Always last */
} soc_axp_nlf_type_t;

/* Loopback packet type, TR3 internal HW encoding */
typedef enum soc_axp_lpt_e {
    SOC_AXP_LPT_NOP = 0,
    SOC_AXP_LPT_MIM,
    SOC_AXP_LPT_TRILL_NETWORK_PORT,
    SOC_AXP_LPT_TRILL_ACCESS_PORT,
    SOC_AXP_LPT_WLAN_ENCAP,
    SOC_AXP_LPT_WLAN_DECAP,
    SOC_AXP_LPT_DPI_SM,
    SOC_AXP_LPT_MPLS_P2MP,
    SOC_AXP_LPT_IFP_GENERIC,
    SOC_AXP_LPT_MPLS_EXTENDED_LOOKUP,
    SOC_AXP_LPT_L2GRE,
    SOC_AXP_LPT_QCN,
    SOC_AXP_LPT_EP_REDIRECT,
    SOC_AXP_LPT_NUM
} soc_axp_lpt_t;

/* Loopback packet type, TR3 internal */
typedef enum soc_axp_egr_port_prop_e {
    SOC_AXP_EPP_LPORT_0 = 0,
    SOC_AXP_EPP_LPORT_1,
    SOC_AXP_EPP_LPORT_2,
    SOC_AXP_EPP_LPORT_3,
    SOC_AXP_EPP_LPORT_4,
    SOC_AXP_EPP_LPORT_5,
    SOC_AXP_EPP_USE_SGLP,
    SOC_AXP_EPP_USE_SVP,
    SOC_AXP_EPP_NUM
} soc_axp_egr_port_prop_t;

#endif  /* _SOC_AXP_H */

