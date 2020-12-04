/*
 * File: cint_api_logger_filter_tests.c
 *
 * Purpose:
 *
 * Demonstrate the filter/formatter capabilities of the API logger
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

char* bcm_module_list[] = {
    "auth",
    "bcmi2c",
    "bfd",
    "bregex",
    "collector",
    "cosq",
    "cpri",
    "custom",
    "eav",
    "ecn",
    "error",
    "extender",
    "fabric",
    "failover",
    "fcmap",
    "fcoe",
    "field",
    "flexctr",
    "flexdigest",
    "flexstate",
    "flow",
    "flowtracker",
    "gdpll",
    "hash",
    "ifa",
    "init",
    "instru",
    "int",
    "ipfix",
    "ipmc",
    "ipsec",
    "knet",
    "knetsync",
    "l2",
    "l2gre",
    "l3",
    "latency_monitor",
    "lb",
    "link",
    "macsec",
    "mcast",
    "mim",
    "mirror",
    "module",
    "mpls",
    "multicast",
    "nat",
    "niv",
    "oam",
    "oob",
    "pkt",
    "pktio",
    "pktio_defs",
    "policer",
    "port",
    "ppp",
    "proxy",
    "pstats",
    "ptp",
    "qos",
    "range",
    "rate",
    "rx",
    "sat",
    "srv6",
    "stack",
    "stat",
    "stg",
    "subport",
    "sum",
    "switch",
    "tdm",
    "telemetry",
    "time",
    "topo",
    "trill",
    "trunk",
    "tsn",
    "tunnel",
    "tx",
    "txbeacon",
    "types",
    "udf",
    "vlan",
    "vswitch",
    "vxlan",
    "wlan",
    "xflow_macsec",
};

char*
bcm_api_to_module_name (char *api)
{
    int nmodules;
    int start, end, cur, res, *ptr1, *ptr3;
    void *ptr2;
    char *module = NULL;

    if (!api) {
        return NULL;
    }

    nmodules = sizeof(bcm_module_list) / sizeof(bcm_module_list[0]);

    /*
     * binary search...
     */
    start = 0;
    end = nmodules -1;

    do {

        cur = (start + end)/2;

        module = bcm_module_list[cur];

        /*
        ptr1 = sal_strncmp(&api[4], module, sal_strlen(module));
        ptr2 = &ptr1;
        ptr3 = ptr2;
        res = *ptr3;
        */
        res = sal_strncmp(&api[4], module, sal_strlen(module));

        if (res == 0) {
            break;
        } else if (res > 0) {
            start = cur + 1;
        } else {
            end = cur - 1;
        }

        module = NULL;

    } while (start <= end);

    return module;
}

/*
 * The variable "this" should exist auto-magically. Test that by referring to
 * this.flags without prototype.
 */
int CINT_LOGGER_IS_BEFORE ()
{
    uint32 flags = this.flags;

    return (((flags & 0x100) == 0x100) &&
            ((flags & 0x100) == flags));
}

/*
 * The variable "this" should exist auto-magically. Test that by referring to
 * this.flags without prototype.
 */
int CINT_LOGGER_IS_AFTER ()
{
    uint32 flags = this.flags;

    return (((flags & 0x200) == 0x200) &&
            ((flags & 0x200) == flags));
}

/*
 * A no-op inline filtering function. In order to add some processing into it
 * bcm-module name of the API is computed and simply ignored. This function
 * filters out nothing by always returning FALSE. Intended for internal testing
 * purposes.
 */
int
inline_filter_no_filtering ()
{
    bcm_api_to_module_name(this.api);
    return 0;
}

/*
 * A formatter that prints a line as follows for every call to it. This can be
 * used as-is for offline logfile filtering, and in the show and decode
 * commands.
 *
 * <full timetamp> <bcm-module>: <{Entering| Exiting}> <api-name>
 *
 * returns TRUE, indicating that the CINT-like log should be filtered out, i.e.
 * not generated
 */
int ts_module_formatter ()
{
    char str[256];
    int len;

    cint_strftime(str, sizeof(str), "%F %T", cint_localtime(&this.timestamp.tv_sec));
    len = sal_strlen(str);
    snprintf(&str[len], sizeof(str)-len, ".%ld", this.timestamp.tv_usec);

    printf("%s %s: %s %s\n",
           str, bcm_api_to_module_name(this.api),
           CINT_LOGGER_IS_BEFORE() ? "{Entering" : " Exitting}",
           this.api);

    return 1;
}

int dummy_call ()
{
    return 0;
}
