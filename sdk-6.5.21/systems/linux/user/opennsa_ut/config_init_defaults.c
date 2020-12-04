/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        config_init_defaults.c
 * Purpose:     Declaration for compiled-in config variables, to be 
 *              used by Diag Shell if config.bcm file is not specified
 *              or not available.
 *
 *              Typically contains a sequence of multiple invocations
 *              of sal_config_set($attr,$val) for various configuration
 *              properties defined in $SDK/include/soc/property.h
 */

void
sal_config_init_defaults(void)
{
}
