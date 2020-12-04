/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/dport.h>
#ifdef BCM_HURRICANE3_SUPPORT
#include <soc/wolfhound2.h>
#endif
/*
 * Static port maps are arrays of physical port numbers.
 * The array index is the dport number.
 */

static soc_port_t bcm5665_dport_map[] = {
    28,
     0,  1,  2,  3,  4,    5,  6,  7,  8,  9,   /*  1-10 */
    10, 11, 12, 13, 14,   15, 16, 17, 18, 19,   /* 11-20 */
    20, 21, 22, 23, 32,   33, 34, 35, 36, 37,   /* 21-30 */
    38, 39, 40, 41, 42,   43, 44, 45, 46, 47,   /* 31-40 */
    48, 49, 50, 51, 52,   53, 54, 55, 24, 25,   /* 41-50 */
    26, 27, 56
};

static soc_port_t bcm56524_dport_map[] = {
     0,
    30, 31, 32, 33, 34,   35, 36, 37, 38, 39,   /*  1-10 */
    40, 41, 42, 43, 44,   45, 46, 47, 48, 49,   /* 11-20 */
    50, 51, 52, 53, -1,   -1, -1, -1,  1, -1,   /* 21-30 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 31-40 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
    26, 27, 28, 29
};

static soc_port_t bcm56526_dport_map[] = {
     0,
     2,  3,  4,  5,  6,    7,  8,  9, 38, 39,   /*  1-10 */
    40, 41, 10, 11, 12,   13, 14, 15, 16, 17,   /* 11-20 */
    18, 19, 20, 21, 22,   23, 24, 25,  1, -1,   /* 21-30 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 31-40 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
    46, 50, 26, 27, 28, 29
};

static soc_port_t bcm56624_dport_map[] = {
     0, 
     2,  3,  4,  5,  6,    7, 18, 19,  8,  9,   /*  1-10 */
    10, 11, 12, 13, 20,   21, 14, 15, 16, 17,   /* 11-20 */
    22, 23, 24, 25, 37,   38, 39, 40, 26, 32,   /* 21-30 */
    33, 34, 41, 42, 48,   49, 50, 51, 52, 53,   /* 31-40 */
    35, 36, 46, 47, 27,   43, 44, 45,  1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
    28, 29, 30, 31
};

static soc_port_t bcm56626_dport_map[] = {
     0, 
     2,  3,  4,  5,  6,    7, 18, 19,  8,  9,   /*  1-10 */
    10, 11, 12, 13, 20,   21, 14, 15, 16, 17,   /* 11-20 */
    22, 23, 24, 25,  1,   -1, -1, -1, -1, -1,   /* 21-30 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 31-40 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
    26, 27, 28, 29, 30, 31
};

static soc_port_t bcm56628_dport_map[] = {
     0, 
     1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /*  1-10 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 11-20 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 21-30 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 31-40 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
     2, 14, 26, 27, 28, 29, 30, 31
};

static soc_port_t bcm56629_dport_map[] = {
     0, 
     8,  9, 10, 11, 12,   13, 20, 21, 22, 23,   /*  1-10 */
    24, 25, 37, 38, 39,   40, 41, 42, 48, 49,   /* 11-20 */
    50, 51, 52, 53,  1,   -1, -1, -1, -1, -1,   /* 21-30 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 31-40 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
     2, 14, 26, 27, 28, 29, 30, 31
};

static soc_port_t bcm56629_40ge_dport_map[] = {
     0, 
     2,  3,  4,  5,  8,    9, 10, 11, 12, 13,   /*  1-10 */
    14, 15, 16, 17, 20,   21, 22, 23, 24, 25,   /* 11-20 */
    26, 32, 33, 34, 37,   38, 39, 40, 41, 42,   /* 21-30 */
    27, 43, 44, 45, 48,   49, 50, 51, 52, 53,   /* 31-40 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
    28, 29, 30, 31
};

static soc_port_t bcm56680_dport_map[] = {
     0, 
     2,  3,  4,  5,  6,    7, 18, 19, 14, 15,   /*  1-10 */
    16, 17, 26, 32, 33,   34, 35, 36, 46, 47,   /* 11-20 */
    27, 43, 44, 45,  1,   -1, -1, -1, -1, -1,   /* 21-30 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 31-40 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
    28, 29, 30, 31
};

static soc_port_t bcm56620_dport_map[] = {
     0, 
     2,  3,  4,  5,  6,    7, 18, 19, 14, 15,   /*  1-10 */
    16, 17, 26, 32, 33,   34, 35, 36, 46, 47,   /* 11-20 */
    27, 43, 44, 45, -1,   -1, -1, -1, -1, -1,   /* 21-30 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 31-40 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
    28, 29, 30, 31
};

static soc_port_t bcm56634_dport_map[] = {
     0, 
    30, 31, 32, 33, 34,   35, 36, 37,  2,  3,   /*  1-10 */
     4,  5,  6,  7,  8,    9, 38, 39, 40, 41,   /* 11-20 */
    10, 11, 12, 13, 14,   15, 16, 17, 42, 43,   /* 21-30 */
    44, 45, 18, 19, 20,   21, 22, 23, 24, 25,   /* 31-40 */
    46, 47, 48, 49, 50,   51, 52, 53,  1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
    26, 27, 28, 29
};

static soc_port_t bcm56636_dport_map[] = {
     0, 
    30, 31, 32, 33, 34,   35, 36, 37,  2,  3,   /*  1-10 */
     4,  5,  6,  7,  8,    9, 38, 39, 40, 41,   /* 11-20 */
    10, 11, 12, 13,  1,   43, 44, 45, 51, 52,   /* 21-30 */
    53, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 31-40 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 41-50 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 51-60 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 61-70 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 71-80 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 81-90 */
    -1, -1, -1, -1, -1,   -1, -1, -1, -1, -1,   /* 91-100 */
     /* HG/XE ports start at 101 */
    42, 50, 26, 27, 28, 29
};

static soc_port_t bcm5339x_dport_map[] = { 
      0, 
     -1,  2, -1, -1, -1,  6, -1, -1, -1, 10, /*  1-10 */
     -1, -1, -1, 14, -1, -1, -1, 18, -1, -1, /* 11-20 */
     22, 21, 23, 24, 25, 26, 27, 28, 29, -1, /* 21-30 */
     -1, -1, -1
};

static soc_port_t bcm56160_dport_map[] = {
     0,
     2,  3,  4,  5, 13,   12, 11, 10,  9,  8,   /*  1-10 */
     7,  6, 18, 19, 20,   21, 22, 23, 24, 25,   /* 11-20 */
    14, 15, 16, 17, 26,   27, 28, 29, 30, 31   /* 21-30 */
};

static soc_port_t bcm56160_non_gphy_dport_map[] = {
     0,
     2,  3,  4,  5,  6,    7,  8,  9, 18, 19,   /*  1-10 */
    20, 21, 22, 23, 24,   25, 10, 11, 12, 13,   /* 11-20 */
    14, 15, 16, 17, 26,   27, 28, 29, 30, 31   /* 21-30 */
};

static soc_port_t bcm56162_dport_map[] = {
     0,
     9,  8,  7,  6,  5,    4,  3,  2, 18, 19,   /*  1-10 */
    20, 21, 22, 23, 24,   25, 10, 11, 12, 13,   /* 11-20 */
    14, 15, 16, 17, 26,   27, 28, 29, 30, 31    /* 21-30 */
};


STATIC int
_soc_esw_dport_map_array(int unit, soc_port_t *ports, int size)
{
    int idx;

    for (idx = 0; idx < size; idx++) {
        if (ports[idx] >= 0) {
            soc_dport_map_port(unit, idx, ports[idx]);
        }
    }
    return 0;
}

#define MAP_ARRAY(_u,_m) _soc_esw_dport_map_array(_u,_m,COUNTOF(_m))

int
soc_esw_dport_init(int unit)
{
    uint16 dev_id;
    uint8 rev_id;
    int idx, port, dport, port_offset;
    int hu2_config_id = 0;

    hu2_config_id = soc_property_get(unit, spn_BCM5615X_CONFIG, 0);
	
    /* Cache the configuration settings */
    SOC_DPORT_MAP_FLAGS(unit) = 0;
    if (soc_property_get(unit, spn_DPORT_MAP_ENABLE, TRUE)) {
        SOC_DPORT_MAP_FLAGS(unit) |= SOC_DPORT_MAP_F_ENABLE;
    }
    if (soc_property_get(unit, spn_DPORT_MAP_INDEXED, TRUE)) {
        SOC_DPORT_MAP_FLAGS(unit) |= SOC_DPORT_MAP_F_INDEXED;
    }

    for (idx = 0; idx < COUNTOF(SOC_DPORT_MAP(unit)); idx++) {
        SOC_DPORT_MAP(unit)[idx] = -1;
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);

    switch (dev_id) {
    case BCM5665_DEVICE_ID:
        MAP_ARRAY(unit, bcm5665_dport_map);
        break;
    case BCM56524_DEVICE_ID:
        MAP_ARRAY(unit, bcm56524_dport_map);
        break;
    case BCM56526_DEVICE_ID:
        MAP_ARRAY(unit, bcm56526_dport_map);
        break;
    case BCM56620_DEVICE_ID:
        MAP_ARRAY(unit, bcm56620_dport_map);
        break;
    case BCM56624_DEVICE_ID:
        MAP_ARRAY(unit, bcm56624_dport_map);
        break;
    case BCM56629_DEVICE_ID:
        if (soc_feature(unit, soc_feature_xgport_one_xe_six_ge)) {
            if (soc_property_get(unit, spn_BCM56629_40GE, 0)) {
                MAP_ARRAY(unit, bcm56629_40ge_dport_map);
            } else {
                MAP_ARRAY(unit, bcm56629_dport_map);
            }
            break;
        }
        /* Fall through - BCM56629_A0 uses BCM56626 port layout */
    case BCM56626_DEVICE_ID:
        MAP_ARRAY(unit, bcm56626_dport_map);
        break;
    case BCM56628_DEVICE_ID:
        MAP_ARRAY(unit, bcm56628_dport_map);
        break;
    case BCM56680_DEVICE_ID:
    case BCM56684_DEVICE_ID:
        MAP_ARRAY(unit, bcm56680_dport_map);
        break;
    case BCM56634_DEVICE_ID:
    case BCM56538_DEVICE_ID:
        MAP_ARRAY(unit, bcm56634_dport_map);
        if (soc_feature(unit, soc_feature_internal_loopback)) {
            SOC_PBMP_ITER(PBMP_LB(unit), port) {
                SOC_DPORT_MAP(unit)[port] = port;
            }
            SOC_PBMP_ITER(PBMP_MMU(unit), port) {
                SOC_DPORT_MAP(unit)[port] = port;
            }
        }
        break;
    case BCM56636_DEVICE_ID:
        MAP_ARRAY(unit, bcm56636_dport_map);
        if (soc_feature(unit, soc_feature_internal_loopback)) {
            SOC_PBMP_ITER(PBMP_LB(unit), port) {
                SOC_DPORT_MAP(unit)[port] = port;
            }
            SOC_PBMP_ITER(PBMP_MMU(unit), port) {
                SOC_DPORT_MAP(unit)[port] = port;
            }
        }
        break;
    
    case BCM56160_DEVICE_ID:
    case BCM53440_DEVICE_ID:
        if (soc_feature(unit, soc_feature_gphy)) {
            MAP_ARRAY(unit, bcm56160_dport_map);
        } else {
            MAP_ARRAY(unit, bcm56160_non_gphy_dport_map);
        }
        break;
    case BCM56162_DEVICE_ID:
    case BCM56163_DEVICE_ID:
    case BCM56164_DEVICE_ID:
    case BCM56166_DEVICE_ID:
    case BCM53443_DEVICE_ID:
    case BCM53442_DEVICE_ID:
    case BCM53434_DEVICE_ID: 
        if (soc_feature(unit, soc_feature_gphy)) {
            MAP_ARRAY(unit, bcm56162_dport_map);
        } else {
            MAP_ARRAY(unit, bcm56160_non_gphy_dport_map);
        }    
        break; 
    case BCM53393_DEVICE_ID:
        MAP_ARRAY(unit, bcm5339x_dport_map);
        break;
    case BCM53394_DEVICE_ID:
        if ((hu2_config_id == 0) || (hu2_config_id == 1)) {
            MAP_ARRAY(unit, bcm5339x_dport_map);
            break;
        }
        /* fall through */
    default:
        port_offset = (CMIC_PORT(unit) == 0) ? 0 : 1;
        PBMP_ALL_ITER(unit, port) {
            /* coverity[overrun-local : FALSE] */
            SOC_DPORT_MAP(unit)[port + port_offset] = port;
        }
        SOC_PBMP_ITER(PBMP_MMU(unit), port) {
            /* coverity[overrun-local : FALSE] */
            SOC_DPORT_MAP(unit)[port + port_offset] = port;
        }
        break;
    }

#ifdef BCM_HURRICANE3_SUPPORT
    if (soc_feature(unit, soc_feature_wh2)) {
        (void)soc_wolfhound2_dport_init(unit);
    }
#endif /* BCM_HURRICANE3_SUPPORT */
    PBMP_ALL_ITER(unit, port) {
        dport = soc_property_port_get(unit, port, spn_DPORT_MAP_PORT, -1);
        if (dport >= 0) {
            soc_dport_map_port(unit, dport, port);
        }
    }
    SOC_PBMP_ITER(PBMP_MMU(unit), port) {
        dport = soc_property_port_get(unit, port, spn_DPORT_MAP_PORT, -1);
        if (dport >= 0) {
            soc_dport_map_port(unit, dport, port);
        }
    }

    return soc_dport_map_update(unit);
}

