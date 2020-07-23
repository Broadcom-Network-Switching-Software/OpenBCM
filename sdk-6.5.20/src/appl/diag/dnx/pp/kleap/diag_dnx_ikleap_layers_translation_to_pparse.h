/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_IKLEAP_LAYERS_TRANSLATION_TO_PPARSE_H_INCLUDED
#define DIAG_DNX_IKLEAP_LAYERS_TRANSLATION_TO_PPARSE_H_INCLUDED
#include "diag_dnx_ikleap_layers_translation_to_pparse_definition.h"

/** Network Headers Signals */
static kleap_layer_to_pparse_t General_header = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t BIER_TI_header = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t FCoE_header = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t PPPoE_header = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t L2TP_header = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t GTP_header = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t BIER_MPLS_header = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t ETHERNET_header = {
    1,
    {"ETH0"},
    {""}
};
static kleap_layer_to_pparse_t RCH_header = {
    1,
    {"RCH"},
    {""}
};
static kleap_layer_to_pparse_t IPv4_header = {
    1,
    {"IPv4"},
    {""}
};
static kleap_layer_to_pparse_t IPv6_header = {
    1,
    {"IPv6"},
    {""}
};

static kleap_layer_to_pparse_t MPLS_header = {
    1,
    {"MPLS"},
    {""}
};

static kleap_layer_to_pparse_t INITIALIZATION_header = {
    0,
    {""},
    {""}
};

#if 0
static kleap_layer_to_pparse_t UDP_header = {
    1,
    {"UDP"},
    {""}
};

static kleap_layer_to_pparse_t SRv6_BEYOND_header = {
    1,
    {"SRv6"},
    {""}
};

static kleap_layer_to_pparse_t ARP_header = {
    1,
    {"ARP"},
    {""}
};

static kleap_layer_to_pparse_t TCP_header = {
    1,
    {"TCP"},
    {""}
};

#endif
/** Layer Records Signals */
static kleap_layer_to_pparse_t General_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t ETHERNET_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t RCH_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t PPPoE_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t L2TP_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t GTP_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t FCoE_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t MPLS_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t BIER_MPLS_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t BIER_TI_layer = {
    0,
    {""},
    {""}
};
static kleap_layer_to_pparse_t IPv4_layer = {
    1,
    {"IPP_IPv4_Layer_Record"},
    {""}
};
static kleap_layer_to_pparse_t IPv6_layer = {
    1,
    {"IPP_IPv6_Layer_Record"},
    {""}
};
static kleap_layer_to_pparse_t INITIALIZATION_layer = {
    0,
    {""},
    {""}
};
#endif
