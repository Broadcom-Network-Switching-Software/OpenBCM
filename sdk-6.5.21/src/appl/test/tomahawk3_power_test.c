/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The TH3 power test test excercises the WCS power scenario
 * streaming MPLS/IPv6 packets on all ports at maximum rate. All ports are
 * configured in MAC/PHY/EXT loopback mode and each port is paired with a same speed
 * port. The test calculates the number of packets needed to saturate the ports
 * and send the packets from the CPU to each port pairs initially.
 * Then the packets are switched between the port pairs indefinitely. Once
 * the traffic reaches steady state, rate calculation is done by dividing the
 * transmit packet count changes and transmit byte count changes over a
 * programmable interval. The rates are checked against expected rates based on
 * port configuration and oversubscription ratio. Finally, packet integrity
 * check is achieved by redirecting the packets back to the CPU and compared
 * against expected packets.
 *
 * Configuration parameters passed from CLI:
 *          See power_test_usage[] array
 */

#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/cmicm.h>
#include <soc/cmic.h>

#include <sal/types.h>
#include <appl/diag/parse.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/stat.h>

#include <soc/init/tomahawk3_flexport_defines.h>
#include "testlist.h"
#include "gen_pkt.h"
#include "streaming_lib.h"

