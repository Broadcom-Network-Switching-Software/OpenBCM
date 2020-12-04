/** \file tdm.c
 * 
 *
 * TDM common procedures
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/core/libc.h>
#include <bcm/tdm.h>

/*
 * Function:
 *     bcm_tdm_ingress_editing_context_t_init
 * Description:
 *     Initialize an ingress context structure
 * Parameters:
 *     ingress_context - pointer to ingress context structure
 * Return: none
 */
void
bcm_tdm_ingress_editing_context_t_init(bcm_tdm_ingress_context_t *ingress_context)
{
    sal_memset(ingress_context, 0, sizeof(bcm_tdm_ingress_context_t));
}
