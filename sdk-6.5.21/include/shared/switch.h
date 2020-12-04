/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SHR_SWITCH_H
#define _SHR_SWITCH_H

#define _SHR_SWITCH_MAX_PIPES                 8  /* Max number of pipes (or slices) */
#define _SHR_SWITCH_MAX_PP_PIPES              8  /* Max number of PP pipes */

#define _SHR_SWITCH_STABLE_NONE               0  /* No storage */
#define _SHR_SWITCH_STABLE_DEVICE_NEXT_HOP    1  /* Use next hop table */
#define _SHR_SWITCH_STABLE_DEVICE_EXT_MEM     2  /* Use external TCAM/SRAM */
#define _SHR_SWITCH_STABLE_APPLICATION        3  /* Use application storage */
#define _SHR_SWITCH_STABLE_SHARED_MEM         4  /* Use Linux shmem for internal proccess NV storage */ 

/*
 * Defines:
 *	_SHR_SWITCH_FILTER_*
 * Purpose:
 *	Defines the bypass control items (bitmap format)
 */

typedef enum _shr_switch_filter_control_e {

    _SHR_SWITCH_FILTER_NONE           = 0,          /* CRC check */
    /* bypass for RX group */
    _SHR_SWITCH_FILTER_RX_CRCCHK      = (1 << 0),   /* CRC check */
    /* bypass for DOS group */
    _SHR_SWITCH_FILTER_DOS_TOCPU      = (1 << 1),
    /* bypass for STP group */
    _SHR_SWITCH_FILTER_STP_USERADDR   = (1 << 2),   /* L2 User MAC Addr */
    _SHR_SWITCH_FILTER_STP_MAC0X      = (1 << 3),   /* 0180c2-000000x */
    /* bypass for VLAN group */
    _SHR_SWITCH_FILTER_VLAN_IGMP_MLD  = (1 << 4),
    _SHR_SWITCH_FILTER_VLAN_ARP_DHCP  = (1 << 5),
    _SHR_SWITCH_FILTER_VLAN_MIIM      = (1 << 6),   /* IMP ingress frame */
    _SHR_SWITCH_FILTER_VLAN_MCAST     = (1 << 7),
    _SHR_SWITCH_FILTER_VLAN_RSV_MCAST = (1 << 8),   /* 0180c2-000000x */
                                                    /* 0180c2-000001x */
                                                    /* 0180c2-000002x */
    _SHR_SWITCH_FILTER_VLAN_USERADDR  = (1 << 9),   /* L2 User MAC Addr */
    /* bypass for EAP group */
    _SHR_SWITCH_FILTER_EAP_USERADDR   = (1 << 10),  /* L2 User MAC Addr */
    _SHR_SWITCH_FILTER_EAP_DHCP       = (1 << 11),
    _SHR_SWITCH_FILTER_EAP_ARP        = (1 << 12),
    _SHR_SWITCH_FILTER_EAP_MAC_22_2F  = (1 << 13),  /* 0180c2-000002x */
    _SHR_SWITCH_FILTER_EAP_MAC_21     = (1 << 14),  /* 0180c2-0000021 */
    _SHR_SWITCH_FILTER_EAP_MAC_20     = (1 << 15),  /* 0180c2-0000020 */
    _SHR_SWITCH_FILTER_EAP_MAC_11_1F  = (1 << 16),  /* 0180c2-000001x */
    _SHR_SWITCH_FILTER_EAP_MAC_10     = (1 << 17),  /* 0180c2-0000010 */
    _SHR_SWITCH_FILTER_EAP_MAC_0X     = (1 << 18),  /* 0180c2-000000x */
    _SHR_SWITCH_FILTER_EAP_BPDU       = (1 << 19),  /* 0180c2-0000000 */
    _SHR_SWITCH_FILTER_CONTROL_COUNT  = 20
} _shr_switch_filter_control_t;

#define _SHR_SWITCH_FILTER_TYPE_GET(_type, _id)  \
            (_type) = (((_id) == 0) ? _SHR_SWITCH_FILTER_RX_CRCCHK : \
                        ((_id) == 1) ? _SHR_SWITCH_FILTER_DOS_TOCPU : \
                        ((_id) == 2) ? _SHR_SWITCH_FILTER_STP_USERADDR : \
                        ((_id) == 3) ? _SHR_SWITCH_FILTER_STP_MAC0X : \
                        ((_id) == 4) ? _SHR_SWITCH_FILTER_VLAN_IGMP_MLD : \
                        ((_id) == 5) ? _SHR_SWITCH_FILTER_VLAN_ARP_DHCP : \
                        ((_id) == 6) ? _SHR_SWITCH_FILTER_VLAN_MIIM : \
                        ((_id) == 7) ? _SHR_SWITCH_FILTER_VLAN_MCAST : \
                        ((_id) == 8) ? _SHR_SWITCH_FILTER_VLAN_RSV_MCAST : \
                        ((_id) == 9) ? _SHR_SWITCH_FILTER_VLAN_USERADDR : \
                        ((_id) == 10) ? _SHR_SWITCH_FILTER_EAP_USERADDR : \
                        ((_id) == 11) ? _SHR_SWITCH_FILTER_EAP_DHCP : \
                        ((_id) == 12) ? _SHR_SWITCH_FILTER_EAP_ARP : \
                        ((_id) == 13) ? _SHR_SWITCH_FILTER_EAP_MAC_22_2F : \
                        ((_id) == 14) ? _SHR_SWITCH_FILTER_EAP_MAC_21 : \
                        ((_id) == 15) ? _SHR_SWITCH_FILTER_EAP_MAC_20 : \
                        ((_id) == 16) ? _SHR_SWITCH_FILTER_EAP_MAC_11_1F : \
                        ((_id) == 17) ? _SHR_SWITCH_FILTER_EAP_MAC_10 : \
                        ((_id) == 18) ? _SHR_SWITCH_FILTER_EAP_MAC_0X : \
                        ((_id) == 19) ? _SHR_SWITCH_FILTER_EAP_BPDU : \
                        _SHR_SWITCH_FILTER_NONE)

/* Switch event action controls. */
typedef enum _shr_switch_event_control_action_e {
    _SHR_SWITCH_EVENT_MASK = 0x0,
    _SHR_SWITCH_EVENT_CLEAR = 0x1,
    _SHR_SWITCH_EVENT_READ = 0x2,
    _SHR_SWITCH_EVENT_FORCE = 0x3,
    _SHR_SWITCH_EVENT_STORM_TIMED_COUNT = 0x4,
    _SHR_SWITCH_EVENT_STORM_TIMED_PERIOD = 0x5,
    _SHR_SWITCH_EVENT_STORM_NOMINAL = 0x6,
    _SHR_SWITCH_EVENT_PRIORITY = 0x7,
    _SHR_SWITCH_EVENT_LOG = 0x8,
    _SHR_SWITCH_EVENT_STAT = 0x9,
    _SHR_SWITCH_EVENT_CORR_ACT_OVERRIDE = 0xA,
    _SHR_SWITCH_EVENT_UNMASK_AND_CLEAR_DISABLE = 0xB,
    _SHR_SWITCH_EVENT_FORCE_UNMASK = 0xC
} _shr_switch_event_control_action_t;

/*
 * Defines:
 *	_SHR_SWITCH_LED_FUNCSET_*
 * Purpose:
 *	Defines the LED function groups
 */
#define _SHR_SWITCH_LED_FUNCGRP_0       0
#define _SHR_SWITCH_LED_FUNCGRP_1       1

/*
 * Defines:
 *	_SHR_SWITCH_LED_FUNC_*
 * Purpose:
 *	Defines the LED function items
 */
#define _SHR_SWITCH_LED_FUNC_NONE       0
#define _SHR_SWITCH_LED_FUNC_PHYLED4    (1 << 0)
#define _SHR_SWITCH_LED_FUNC_LNK        (1 << 1)
#define _SHR_SWITCH_LED_FUNC_DPX        (1 << 2)
#define _SHR_SWITCH_LED_FUNC_ACT        (1 << 3)
#define _SHR_SWITCH_LED_FUNC_COL        (1 << 4)
#define _SHR_SWITCH_LED_FUNC_LINK_ACT   (1 << 5)
#define _SHR_SWITCH_LED_FUNC_DPX_COL    (1 << 6)
#define _SHR_SWITCH_LED_FUNC_SP_10      (1 << 7)
#define _SHR_SWITCH_LED_FUNC_SP_100     (1 << 8)
#define _SHR_SWITCH_LED_FUNC_SP_1G      (1 << 9)
#define _SHR_SWITCH_LED_FUNC_10_ACT     (1 << 10)
#define _SHR_SWITCH_LED_FUNC_100_ACT    (1 << 11)
#define _SHR_SWITCH_LED_FUNC_10_100_ACT (1 << 12)
#define _SHR_SWITCH_LED_FUNC_1G_ACT     (1 << 13)
#define _SHR_SWITCH_LED_FUNC_EAV_LINK   (1 << 14)
#define _SHR_SWITCH_LED_FUNC_PHYLED3    (1 << 15)

#define _SHR_SWITCH_LED_FUNC_SP_100_200     (1 << 16)
#define _SHR_SWITCH_LED_FUNC_100_200_ACT    (1 << 17)
#define _SHR_SWITCH_LED_FUNC_LNK_ACT_SP     (1 << 18)

/*
 * Defines:
 *	_SHR_SWITCH_LED_MODE_*
 * Purpose:
 *	Defines the LED mode
 */
#define _SHR_SWITCH_LED_MODE_OFF        0
#define _SHR_SWITCH_LED_MODE_ON         1
#define _SHR_SWITCH_LED_MODE_BLINK      2
#define _SHR_SWITCH_LED_MODE_AUTO       3

/*
 * structure:
 *      _shr_temperature_monitor_t 
 * Purpose:
 *      entry type for retrieving temperature monitor value 
 */
typedef struct _shr_switch_temperature_monitor_s {
    int curr;
    int peak;
} _shr_switch_temperature_monitor_t;

#endif  /* !_SHR_SWITCH_H */
