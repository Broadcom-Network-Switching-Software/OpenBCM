/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utilities for Loopback Tests 
 */
#ifndef __LB_UTIL__H
#define __LB_UTIL__H

int lbu_setup_port(int unit, bcm_port_t port, 
                          int req_speed, int autoneg);

#endif /*!__LB_UTIL__H */
