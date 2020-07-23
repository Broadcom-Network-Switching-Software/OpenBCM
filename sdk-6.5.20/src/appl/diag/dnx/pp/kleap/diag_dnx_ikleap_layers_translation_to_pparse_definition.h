/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_IKLEAP_LAYERS_TRANSLATION_TO_PPARSE_DEFINITION_H_INCLUDED
#  define DIAG_DNX_IKLEAP_LAYERS_TRANSLATION_TO_PPARSE_DEFINITION_H_INCLUDED

/** \brief Number of layers in lookups packet */
#define KLEAP_NOF_RELATIVE_LAYERS                   4

typedef struct
{
    int nof_signals;
    char signal_mame[5][64];
    char signal_prefix[5][64];

} kleap_layer_to_pparse_t;

#endif
