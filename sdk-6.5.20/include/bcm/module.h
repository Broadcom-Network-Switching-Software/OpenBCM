/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_MODULE_H__
#define __BCM_MODULE_H__

#include <bcm/types.h>

#define BCM_MODULE_PORT             0          
#define BCM_MODULE_L2               1          
#define BCM_MODULE_VLAN             2          
#define BCM_MODULE_TRUNK            3          
#define BCM_MODULE_COSQ             4          
#define BCM_MODULE_MCAST            5          
#define BCM_MODULE_LINKSCAN         6          
#define BCM_MODULE_STAT             7          
#define BCM_MODULE_VIRTUAL          8          
#define BCM_MODULE_COMMON           9          
#define BCM_MODULE_MIRROR           10         
#define BCM_MODULE_L3               11         
#define BCM_MODULE_STACK            12         
#define BCM_MODULE_IPMC             13         
#define BCM_MODULE_STG              14         
#define BCM_MODULE_TX               15         
#define BCM_MODULE_L2GRE            16         
#define BCM_MODULE_AUTH             17         
#define BCM_MODULE_RX               18         
#define BCM_MODULE_FIELD            19         
#define BCM_MODULE_TIME             20         
#define BCM_MODULE_SUBPORT          21         
#define BCM_MODULE_MPLS             22         
#define BCM_MODULE_FABRIC           23         
#define BCM_MODULE_MIM              24         
#define BCM_MODULE_POLICER          25         
#define BCM_MODULE_OAM              26         
#define BCM_MODULE_FAILOVER         27         
#define BCM_MODULE_VSWITCH          28         
#define BCM_MODULE_WLAN             29         
#define BCM_MODULE_QOS              30         
#define BCM_MODULE_MULTICAST        31         
#define BCM_MODULE_TRILL            32         
#define BCM_MODULE_IPFIX            33         
#define BCM_MODULE_NIV              34         
#define BCM_MODULE_PTP              36         
#define BCM_MODULE_BFD              37         
#define BCM_MODULE_REGEX            38         
#define BCM_MODULE_VXLAN            39         
#define BCM_MODULE_EXTENDER         40         
#define BCM_MODULE_FCOE             41         
#define BCM_MODULE_UDF              42         
#define BCM_MODULE_SAT              43         
#define BCM_MODULE_RANGE            44         
#define BCM_MODULE_SWITCH           45         
#define BCM_MODULE_LB               46         
#define BCM_MODULE_PROXY            47         
#define BCM_MODULE_FLOW             48         
#define BCM_MODULE_TSN              49         
#define BCM_MODULE_XFLOW_MACSEC     50         
#define BCM_MODULE_FLOWTRACKER      51         
#define BCM_MODULE_GDPLL            52         
#define BCM_MODULE_IFA              53         
#define BCM_MODULE_COLLECTOR        54         
#define BCM_MODULE_TELEMETRY        55         
#define BCM_MODULE_LATENCY_MONITOR  56         
#define BCM_MODULE_INT              57         
#define BCM_MODULE_SUM              58         
#define BCM_MODULE_INSTRU           59         
#define BCM_MODULE_SRV6             60         
#define BCM_MODULE_HASH             61         
#define BCM_MODULE_TDM              62         
#define BCM_MODULE_FLEXCTR          63         
#define BCM_MODULE_RATE             64         
#define BCM_MODULE_PKTIO            65         
#define BCM_MODULE_FLEXDIGEST       66         
#define BCM_MODULE_FLEXSTATE        67         
#define BCM_MODULE_PKTIO_DEFS       68         
#define BCM_MODULE_PPP              69         
#define BCM_MODULE_FLOW_LIF         70         
#define BCM_MODULE_CPRI             71         
#define BCM_MODULE__COUNT           72         /* Always last plus one. */

#define BCM_MODULE_NAMES_INITIALIZER    \
{ \
    "port", \
    "l2", \
    "vlan", \
    "trunk", \
    "cosq", \
    "mcast", \
    "linkscan", \
    "stat", \
    "virtual", \
    "common", \
    "mirror", \
    "l3", \
    "stack", \
    "ipmc", \
    "stg", \
    "tx", \
    "l2gre", \
    "auth", \
    "rx", \
    "field", \
    "rcpu", \
    "subport", \
    "mpls", \
    "fabric", \
    "mim", \
    "policer", \
    "oam", \
    "failover", \
    "vswitch", \
    "wlan", \
    "qos", \
    "multicast", \
    "trill", \
    "ipfix", \
    "niv", \
    "ces", \
    "ptp", \
    "bfd", \
    "regex", \
    "vxlan", \
    "extender", \
    "fcoe", \
    "udf", \
    "sat", \
    "range", \
    "switch", \
    "lb", \
    "proxy", \
    "flow", \
    "tsn", \
    "xflow_macsec", \
    "flowtracker", \
    "gdpll", \
    "ifa", \
    "collector", \
    "telemetry", \
    "latency_monitor", \
    "int", \
    "sum", \
    "instru", \
    "srv6", \
    "hash", \
    "tdm", \
    "flexctr", \
    "rate", \
    "pktio", \
    "flexdigest", \
    "flexstate", \
    "pktio_defs", \
    "ppp", \
    "flow_lif", \
    "cpri" \
} 

/* bcm_module_name */
extern char *bcm_module_name(
    int unit, 
    int bcm_module);

#endif /* __BCM_MODULE_H__ */
