/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l3.c
 * Purpose: Tomahawk3 L3 function implementations
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(INCLUDE_L3) && defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/drv.h>
#include <soc/tomahawk2.h>
#include <soc/profile_mem.h>

#include <bcm/vlan.h>
#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/tomahawk2.h>
#include <bcm_int/esw/tomahawk3.h>

#define WRITE_DLB_MEMBER_PORT_0_15(_unit, _i, _port_ent, _val, _intf_count) {\
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m, \
                        _port_ent,\
                        MEMBER_ ## _i ## _PORTf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m, \
                        _port_ent,\
                        MEMBER_ ## _i ## _PORT_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_PORT_16_31(_unit, _i, _port_ent, _val, _intf_count) {\
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_16_TO_31m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORTf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_16_TO_31m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORT_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_PORT_32_47(_unit, _i, _port_ent, _val, _intf_count) {\
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_32_TO_47m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORTf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_32_TO_47m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORT_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_PORT_48_63(_unit, _i, _port_ent, _val, _intf_count) {\
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_48_TO_63m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORTf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_48_TO_63m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORT_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_NHI_0_12(_unit, _i, _nhi_ent, _val, _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m, _nhi_ent, \
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m, _nhi_ent, \
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_NHI_13_25(_unit, _i, _nhi_ent, _val, _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_13_TO_25m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_13_TO_25m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_NHI_26_38(_unit, _i, _nhi_ent, _val, _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_26_TO_38m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_26_TO_38m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_NHI_39_51(_unit, _i, _nhi_ent, _val, _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_39_TO_51m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_39_TO_51m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_NHI_52_63(_unit, _i, _nhi_ent, _val, _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_52_TO_63m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_52_TO_63m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define READ_DLB_MEMBER_PORT_0_15(_unit, _i, _port_ent, _val) {\
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m, _port_ent,\
                        MEMBER_ ## _i ## _PORT_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m, _port_ent,\
                            MEMBER_ ## _i ## _PORTf); \
    } \
}

#define READ_DLB_MEMBER_PORT_16_31(_unit, _i, _port_ent, _val) {\
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_16_TO_31m, _port_ent,\
                        MEMBER_ ## _i ## _PORT_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_16_TO_31m, _port_ent,\
                            MEMBER_ ## _i ## _PORTf); \
    } \
}

#define READ_DLB_MEMBER_PORT_32_47(_unit, _i, _port_ent, _val) {\
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_32_TO_47m, _port_ent,\
                        MEMBER_ ## _i ## _PORT_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_32_TO_47m, _port_ent,\
                            MEMBER_ ## _i ## _PORTf); \
    } \
}

#define READ_DLB_MEMBER_PORT_48_63(_unit, _i, _port_ent, _val) {\
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_48_TO_63m, _port_ent,\
                        MEMBER_ ## _i ## _PORT_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_PORT_MEMBERS_48_TO_63m, _port_ent,\
                            MEMBER_ ## _i ## _PORTf); \
    } \
}

#define READ_DLB_MEMBER_ALT_PORT_0_15(_unit, _i, _port_ent, _val) {\
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m, _port_ent,\
                        MEMBER_ ## _i ## _PORT_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m, _port_ent,\
                            MEMBER_ ## _i ## _PORTf); \
    } \
}

#define READ_DLB_MEMBER_ALT_PORT_16_31(_unit, _i, _port_ent, _val) {\
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_16_TO_31m, _port_ent,\
                        MEMBER_ ## _i ## _PORT_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_16_TO_31m, _port_ent,\
                            MEMBER_ ## _i ## _PORTf); \
    } \
}

#define READ_DLB_MEMBER_ALT_PORT_32_47(_unit, _i, _port_ent, _val) {\
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_32_TO_47m, _port_ent,\
                        MEMBER_ ## _i ## _PORT_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_32_TO_47m, _port_ent,\
                            MEMBER_ ## _i ## _PORTf); \
    } \
}

#define READ_DLB_MEMBER_ALT_PORT_48_63(_unit, _i, _port_ent, _val) {\
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_48_TO_63m, _port_ent,\
                        MEMBER_ ## _i ## _PORT_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_48_TO_63m, _port_ent,\
                            MEMBER_ ## _i ## _PORTf); \
    } \
}

#define READ_DLB_MEMBER_NHI_0_12(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}

#define READ_DLB_MEMBER_NHI_13_25(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_13_TO_25m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_13_TO_25m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}

#define READ_DLB_MEMBER_NHI_26_38(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_26_TO_38m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_26_TO_38m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}

#define READ_DLB_MEMBER_NHI_39_51(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_39_TO_51m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_39_TO_51m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}

#define READ_DLB_MEMBER_NHI_52_63(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_52_TO_63m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_NHI_MEMBERS_52_TO_63m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}

#define READ_DLB_MEMBER_ALT_NHI_0_12(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}

#define READ_DLB_MEMBER_ALT_NHI_13_25(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_13_TO_25m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_13_TO_25m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}

#define READ_DLB_MEMBER_ALT_NHI_26_38(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_26_TO_38m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_26_TO_38m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}

#define READ_DLB_MEMBER_ALT_NHI_39_51(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_39_TO_51m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_39_TO_51m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}

#define READ_DLB_MEMBER_ALT_NHI_52_63(_unit, _i, _nhi_ent, _val) { \
    _val = 0; \
    if (soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_52_TO_63m, _nhi_ent,\
                        MEMBER_ ## _i ## _NHI_VALIDf)) { \
        _val = soc_mem_field32_get(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_52_TO_63m, _nhi_ent,\
                            MEMBER_ ## _i ## _NHIf); \
    } \
}


#define WRITE_DLB_MEMBER_ALT_PORT_0_15(_unit, _i, _port_ent, _val, \
                                                    _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORTf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORT_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_ALT_PORT_16_31(_unit, _i, _port_ent, _val, \
                                        _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_16_TO_31m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORTf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_16_TO_31m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORT_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_ALT_PORT_32_47(_unit, _i, _port_ent, _val, \
                                        _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_32_TO_47m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORTf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_32_TO_47m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORT_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_ALT_PORT_48_63(_unit, _i, _port_ent, _val, \
                                        _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_48_TO_63m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORTf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_48_TO_63m, \
                        _port_ent, \
                        MEMBER_ ## _i ## _PORT_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_ALT_NHI_0_12(_unit, _i, _nhi_ent, _val, \
                                      _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_ALT_NHI_13_25(_unit, _i, _nhi_ent, _val, \
                                       _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_13_TO_25m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_13_TO_25m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_ALT_NHI_26_38(_unit, _i, _nhi_ent, _val, \
                                       _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_26_TO_38m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_26_TO_38m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_ALT_NHI_39_51(_unit, _i, _nhi_ent, _val, \
                                       _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_39_TO_51m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_39_TO_51m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define WRITE_DLB_MEMBER_ALT_NHI_52_63(_unit, _i, _nhi_ent, _val, \
                                       _intf_count) { \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_52_TO_63m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHIf, (_i < _intf_count)?_val:0); \
    soc_mem_field32_set(_unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_52_TO_63m, \
                        _nhi_ent, \
                        MEMBER_ ## _i ## _NHI_VALIDf, (_i < _intf_count)); \
}

#define _DLB_SET_GROUP_PTR_0_12(_M_) {\
        int port_index = 0; \
        READ_DLB_MEMBER_NHI_0_12(unit, _M_, dlb_nhi_members.entry_data, nh_index); \
        if (nh_index) { \
            READ_DLB_MEMBER_PORT_0_15(unit, _M_, dlb_port_members.entry_data, port_index); \
            if (port_index) {\
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
        READ_DLB_MEMBER_ALT_NHI_0_12(unit, _M_, dlb_alt_nhi_members.entry_data, nh_index); \
        if (nh_index) { \
            READ_DLB_MEMBER_ALT_PORT_0_15(unit, _M_, dlb_alt_port_members.entry_data, port_index); \
            if (port_index) { \
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
}

#define _DLB_SET_GROUP_PTR_13_25(_M_) {\
        int port_index = 0; \
        READ_DLB_MEMBER_NHI_13_25(unit, _M_, dlb_nhi_members_13.entry_data, nh_index); \
        if (nh_index) { \
            if (_M_ <= 15) { \
                READ_DLB_MEMBER_PORT_0_15(unit, _M_, dlb_port_members.entry_data, port_index); \
            } else { \
                READ_DLB_MEMBER_PORT_16_31(unit, _M_, dlb_port_members_16.entry_data, port_index); \
            } \
            if (port_index) {\
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
        READ_DLB_MEMBER_ALT_NHI_13_25(unit, _M_, dlb_alt_nhi_members_13.entry_data, nh_index); \
        if (nh_index) { \
            if (_M_ <= 15) { \
                READ_DLB_MEMBER_ALT_PORT_0_15(unit, _M_, dlb_alt_port_members.entry_data, port_index); \
            } else { \
                READ_DLB_MEMBER_ALT_PORT_16_31(unit, _M_, dlb_alt_port_members_16.entry_data, port_index); \
            } \
            if (port_index) {\
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
}


#define _DLB_SET_GROUP_PTR_26_38(_M_) {\
        int port_index = 0; \
        READ_DLB_MEMBER_NHI_26_38(unit, _M_, dlb_nhi_members_26.entry_data, nh_index); \
        if (nh_index) { \
            if (_M_ <= 31) { \
                READ_DLB_MEMBER_PORT_16_31(unit, _M_, dlb_port_members_16.entry_data, port_index); \
            } else { \
                READ_DLB_MEMBER_PORT_32_47(unit, _M_, dlb_port_members_32.entry_data, port_index); \
            } \
            if (port_index) {\
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
        READ_DLB_MEMBER_ALT_NHI_26_38(unit, _M_, dlb_alt_nhi_members_26.entry_data, nh_index); \
        if (nh_index) { \
            if (_M_ <= 31) { \
                READ_DLB_MEMBER_ALT_PORT_16_31(unit, _M_, dlb_alt_port_members_16.entry_data, port_index); \
            } else { \
                READ_DLB_MEMBER_ALT_PORT_32_47(unit, _M_, dlb_alt_port_members_32.entry_data, port_index); \
            } \
            if (port_index) {\
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
}


#define _DLB_SET_GROUP_PTR_39_51(_M_) {\
        int port_index = 0; \
        READ_DLB_MEMBER_NHI_39_51(unit, _M_, dlb_nhi_members_39.entry_data, nh_index); \
        if (nh_index) { \
            if (_M_ <= 47) { \
                READ_DLB_MEMBER_PORT_32_47(unit, _M_, dlb_port_members_32.entry_data, port_index); \
            } else { \
                READ_DLB_MEMBER_PORT_48_63(unit, _M_, dlb_port_members_48.entry_data, port_index); \
            } \
            if (port_index) {\
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
        READ_DLB_MEMBER_ALT_NHI_39_51(unit, _M_, dlb_alt_nhi_members_39.entry_data, nh_index); \
        if (nh_index) { \
            if (_M_ <= 47) { \
                READ_DLB_MEMBER_ALT_PORT_32_47(unit, _M_, dlb_alt_port_members_32.entry_data, port_index); \
            } else { \
                READ_DLB_MEMBER_ALT_PORT_48_63(unit, _M_, dlb_alt_port_members_48.entry_data, port_index); \
            } \
            if (port_index) {\
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
}


#define _DLB_SET_GROUP_PTR_52_63(_M_) {\
        int port_index = 0; \
        READ_DLB_MEMBER_NHI_52_63(unit, _M_, dlb_nhi_members_52.entry_data, nh_index); \
        if (nh_index) { \
            READ_DLB_MEMBER_PORT_48_63(unit, _M_, dlb_port_members_48.entry_data, port_index); \
            if (port_index) {\
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
        READ_DLB_MEMBER_ALT_NHI_52_63(unit, _M_, dlb_alt_nhi_members_52.entry_data, nh_index); \
        if (nh_index) { \
            READ_DLB_MEMBER_ALT_PORT_48_63(unit, _M_, dlb_alt_port_members_48.entry_data, port_index); \
            if (port_index) {\
                _bcm_th3_set_port_list(unit, port_index, dlb_id, nh_index); \
            } \
        } \
}

#define ECMP_DLB_INFO(_unit_) (_th2_ecmp_dlb_bk[_unit_])
#define ECMP_DLB_PORT_INFO(_unit_, _idx_) \
           (_th2_ecmp_dlb_bk[_unit_]->ecmp_dlb_port_info[_idx_])
#define ECMP_DLB_PORT_LIST(_unit_, _idx_) \
            ECMP_DLB_PORT_INFO(unit, _idx_).port_membership_list

#define _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT      8
#define _BCM_ECMP_DLB_FLOWSET_BLOCK_TEST_RANGE(_u_, _idx_, _count_, _result_) \
    SHR_BITTEST_RANGE(ECMP_DLB_INFO(_u_)->ecmp_dlb_flowset_block_bitmap, _idx_, _count_, _result_)

extern _th2_ecmp_dlb_bookkeeping_t *_th2_ecmp_dlb_bk[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_th3_l3_ecmp_dlb_port_get
 * Purpose:
 *      Get ports for each ECMP member.
 * Parameters:
 *      unit                - (IN) SOC unit number.
 *      ecmp                - (IN) ECMP group info.
 *      dlb_id              - (IN) DLB group ID.
 *      primary_count       - (IN) ECMP primary count..
 *      intf_count          - (IN) Number of elements in intf_array.
 *      intf_array          - (IN) Array of Egress forwarding objects.
 *      port_array          - (OUT) Array of ports.
 *      ecmp_member_bitmap  - (IN) ECMP DLB members.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_l3_ecmp_dlb_port_get(int unit, bcm_l3_egress_ecmp_t *ecmp,
                                 int dlb_id, int primary_count, int intf_count,
                                 bcm_if_t *intf_array, int *port_array)
{
    int rv = BCM_E_NONE;
    int i;
    ing_l3_next_hop_entry_t ing_nh_entry;
    int port;
    int nh_index;

    for (i = 0; i < intf_count; i++) {

        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf_array[i])) {
            nh_index =  intf_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            return BCM_E_PARAM;
        }

        /* Derive port from nh_index */
        rv = (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                    nh_index, &ing_nh_entry));
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry, Tf)) {
            /* Next hop should correspond to a single physical port. */
            return BCM_E_CONFIG;
        }
        port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh_entry,
                PORT_NUMf);
        port_array[i] = port;
    }
    return rv;
}
/*
 *    Function:
 *       bcm_th3_l3_egress_ecmp_dlb_resource_validate
 *    Purpose:
 *       Checks for free resources for the Replace operation. Also,
 *       checks for the contiguous space based on the number of blocks
 *       used for the replacement ecmp dlb group.
 *    Parameters:
 *         unit       - (IN) bcm device.
 *         ecmp       - (IN) ECMP group info.
 *         dlb_id   -   (IN) ECMP DLB id.
 *    Returns:
 *         BCM_E_XXX
 */
int _bcm_th3_l3_egress_ecmp_dlb_resource_validate(int unit, bcm_l3_egress_ecmp_t *ecmp,
                                  int dlb_id)
{
    bcm_l3_egress_ecmp_t dlb_ecmp;
    int dlb_ecmp_count = 0;
    int num_blocks;
    int block_base_ptr;
    int occupied;
    int total_blocks;
    int max_block_base_ptr;
  
    bcm_l3_egress_ecmp_t_init(&dlb_ecmp);
    dlb_ecmp.ecmp_intf = ecmp->ecmp_intf;
    BCM_IF_ERROR_RETURN(
        _bcm_esw_l3_egress_ecmp_get(unit, &dlb_ecmp, 0, NULL,
                                    &dlb_ecmp_count, TRUE));

    num_blocks = ecmp->dynamic_size >> _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    total_blocks = soc_mem_index_count(unit, DLB_ECMP_FLOWSETm) >>
                   _BCM_ECMP_DLB_FLOWSET_BLOCK_SIZE_SHIFT;
    max_block_base_ptr = total_blocks - num_blocks;

    /* Check if a free contiguous region of flow set table exists
     * starting from (dlb_id+1) to (block_base_ptr + num_blocks - 1).
     */
    for (block_base_ptr = dlb_id + 1;
          block_base_ptr <= max_block_base_ptr;
          block_base_ptr++) {
        _BCM_ECMP_DLB_FLOWSET_BLOCK_TEST_RANGE(unit, block_base_ptr, num_blocks,
                occupied);
        if (!occupied) {
            break;
        }
    }
    if (block_base_ptr > max_block_base_ptr) {
        /* A contiguous region of the desired size could not be found in
         * flow set table.
         */
        return BCM_E_RESOURCE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_l3_ecmp_dlb_get_id
 * Purpose:
 *      Get ECMP dlb id from ecmp group.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      ecmp_grp_idx  - (IN) ECMP group index.
 *                         -1: free all DLB resources,
 *                         others: will not disable DLB and free flowset.
 * Returns:
 *      dlb_id - DLB group id.
 */
int
_bcm_th3_l3_ecmp_dlb_get_id(int unit, int ecmp_grp_idx)
{
    int dlb_id = 0;
    dlb_id = ecmp_grp_idx - BCM_TH3_L3_ECMP_DLB_START(unit);

    return dlb_id;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_port_member_set
 * Purpose:
 *      Set dlb port numbers corresponding to each ecmp member NH.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      port_array      - (IN) port array for each NH.
 *      intf_count      - (IN) INTF count.
 *      primary_count   - (IN) Primary INTF count.
 *      dlb_id          - (IN) DLB group id.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_l3_ecmp_dlb_port_member_set(int unit, int *port_array,
                                  int primary_count, int unique_intfs, int dlb_id)
{
    int alt_nh_start;
    int rv = BCM_E_NONE;
    int alt_count = unique_intfs - primary_count;

    dlb_ecmp_group_port_members_0_to_15_entry_t dlb_port_members;
    dlb_ecmp_group_port_members_16_to_31_entry_t dlb_port_members_16;
    dlb_ecmp_group_port_members_32_to_47_entry_t dlb_port_members_32;
    dlb_ecmp_group_port_members_48_to_63_entry_t dlb_port_members_48;
    dlb_ecmp_group_alt_port_members_0_to_15_entry_t dlb_alt_port_members;
    dlb_ecmp_group_alt_port_members_16_to_31_entry_t dlb_alt_port_members_16;
    dlb_ecmp_group_alt_port_members_32_to_47_entry_t dlb_alt_port_members_32;
    dlb_ecmp_group_alt_port_members_48_to_63_entry_t dlb_alt_port_members_48;

    sal_memset(&dlb_port_members, 0,
            sizeof(dlb_ecmp_group_port_members_0_to_15_entry_t));
    sal_memset(&dlb_port_members_16, 0,
            sizeof(dlb_ecmp_group_port_members_16_to_31_entry_t));
    sal_memset(&dlb_port_members_32, 0,
            sizeof(dlb_ecmp_group_port_members_32_to_47_entry_t));
    sal_memset(&dlb_port_members_48, 0,
            sizeof(dlb_ecmp_group_port_members_48_to_63_entry_t));
    WRITE_DLB_MEMBER_PORT_0_15(unit, 0, &dlb_port_members,
                               port_array[0], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 1, &dlb_port_members,
                               port_array[1], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 2, &dlb_port_members,
                               port_array[2], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 3, &dlb_port_members,
                               port_array[3], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 4, &dlb_port_members,
                               port_array[4], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 5, &dlb_port_members,
                               port_array[5], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 6, &dlb_port_members,
                               port_array[6], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 7, &dlb_port_members,
                               port_array[7], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 8, &dlb_port_members,
                               port_array[8], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 9, &dlb_port_members,
                               port_array[9], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 10, &dlb_port_members
                              , port_array[10], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 11, &dlb_port_members,
                               port_array[11], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 12, &dlb_port_members,
                               port_array[12], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 13, &dlb_port_members,
                               port_array[13], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 14, &dlb_port_members,
                               port_array[14], primary_count);
    WRITE_DLB_MEMBER_PORT_0_15(unit, 15, &dlb_port_members,
                               port_array[15], primary_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_port_members);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 16, &dlb_port_members_16,
                               port_array[16], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 17, &dlb_port_members_16,
                               port_array[17], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 18, &dlb_port_members_16,
                               port_array[18], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 19, &dlb_port_members_16,
                               port_array[19], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 20, &dlb_port_members_16,
                               port_array[20], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 21, &dlb_port_members_16,
                               port_array[21], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 22, &dlb_port_members_16,
                               port_array[22], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 23, &dlb_port_members_16,
                                port_array[23], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 24, &dlb_port_members_16,
                                port_array[24], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 25, &dlb_port_members_16,
                                port_array[25], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 26, &dlb_port_members_16,
                                port_array[26], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 27, &dlb_port_members_16,
                                port_array[27], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 28, &dlb_port_members_16,
                                port_array[28], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 29, &dlb_port_members_16,
                                port_array[29], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 30, &dlb_port_members_16,
                                port_array[30], primary_count);
    WRITE_DLB_MEMBER_PORT_16_31(unit, 31, &dlb_port_members_16,
                                port_array[31], primary_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_PORT_MEMBERS_16_TO_31m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_port_members_16);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 32, &dlb_port_members_32,
                                port_array[32], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 33, &dlb_port_members_32,
                                port_array[33], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 34, &dlb_port_members_32,
                                port_array[34], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 35, &dlb_port_members_32,
                                port_array[35], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 36, &dlb_port_members_32,
                                port_array[36], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 37, &dlb_port_members_32,
                                port_array[37], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 38, &dlb_port_members_32,
                                port_array[38], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 39, &dlb_port_members_32,
                                port_array[39], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 40, &dlb_port_members_32,
                                port_array[40], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 41, &dlb_port_members_32,
                                port_array[41], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 42, &dlb_port_members_32,
                                port_array[42], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 43, &dlb_port_members_32,
                                port_array[43], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 44, &dlb_port_members_32,
                                port_array[44], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 45, &dlb_port_members_32,
                                port_array[45], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 46, &dlb_port_members_32,
                                port_array[46], primary_count);
    WRITE_DLB_MEMBER_PORT_32_47(unit, 47, &dlb_port_members_32,
                                port_array[47], primary_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_PORT_MEMBERS_32_TO_47m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_port_members_32);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 48, &dlb_port_members_48,
                                port_array[48], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 49, &dlb_port_members_48,
                                port_array[49], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 50, &dlb_port_members_48,
                                port_array[50], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 51, &dlb_port_members_48,
                                port_array[51], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 52, &dlb_port_members_48,
                                port_array[52], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 53, &dlb_port_members_48,
                                port_array[53], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 54, &dlb_port_members_48,
                                port_array[54], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 55, &dlb_port_members_48,
                                port_array[55], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 56, &dlb_port_members_48,
                                port_array[56], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 57, &dlb_port_members_48,
                                port_array[57], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 58, &dlb_port_members_48,
                                port_array[58], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 59, &dlb_port_members_48,
                                port_array[59], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 60, &dlb_port_members_48,
                                port_array[60], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 61, &dlb_port_members_48,
                                port_array[61], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 62, &dlb_port_members_48,
                                port_array[62], primary_count);
    WRITE_DLB_MEMBER_PORT_48_63(unit, 63, &dlb_port_members_48,
                                port_array[63], primary_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_PORT_MEMBERS_48_TO_63m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_port_members_48);
    if (alt_count <= 0) {
        return rv;
    }
    alt_nh_start = primary_count;
    sal_memset(&dlb_alt_port_members, 0,
            sizeof(dlb_ecmp_group_alt_port_members_0_to_15_entry_t));
    sal_memset(&dlb_alt_port_members_16, 0,
            sizeof(dlb_ecmp_group_alt_port_members_16_to_31_entry_t));
    sal_memset(&dlb_alt_port_members_32, 0,
            sizeof(dlb_ecmp_group_alt_port_members_32_to_47_entry_t));
    sal_memset(&dlb_alt_port_members_48, 0,
            sizeof(dlb_ecmp_group_alt_port_members_48_to_63_entry_t));

    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 0, &dlb_alt_port_members,
                                port_array[alt_nh_start], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 1, &dlb_alt_port_members,
                                port_array[alt_nh_start + 1], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 2, &dlb_alt_port_members,
                                port_array[alt_nh_start + 2], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 3, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 3], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 4, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 4], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 5, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 5], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 6, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 6], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 7, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 7], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 8, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 8], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 9, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 9], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 10, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 10], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 11, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 11], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 12, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 12], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 13, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 13], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 14, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 14], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_0_15(unit, 15, &dlb_alt_port_members,
                                   port_array[alt_nh_start + 15], alt_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_alt_port_members);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 16, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 16], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 17, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 17], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 18, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 18], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 19, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 19], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 20, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 20], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 21, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 21], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 22, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 22], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 23, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 23], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 24, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 24], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 25, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 25], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 26, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 26], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 27, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 27], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 28, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 28], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 29, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 29], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 30, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 30], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_16_31(unit, 31, &dlb_alt_port_members_16,
                                   port_array[alt_nh_start + 31], alt_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_16_TO_31m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_alt_port_members_16);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 32, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 32], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 33, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 33], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 34, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 34], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 35, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 35], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 36, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 36], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 37, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 37], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 38, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 38], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 39, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 39], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 40, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 40], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 41, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 41], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 42, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 42], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 43, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 43], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 44, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 44], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 45, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 45], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 46, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 46], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_32_47(unit, 47, &dlb_alt_port_members_32,
                                   port_array[alt_nh_start + 47], alt_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_32_TO_47m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_alt_port_members_32);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 48, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 48], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 49, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 49], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 50, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 50], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 51, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 51], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 52, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 52], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 53, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 53], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 54, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 54], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 55, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 55], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 56, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 56], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 57, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 57], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 58, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 58], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 59, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 59], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 60, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 60], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 61, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 61], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 62, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 62], alt_count);
    WRITE_DLB_MEMBER_ALT_PORT_48_63(unit, 63, &dlb_alt_port_members_48,
                                   port_array[alt_nh_start + 63], alt_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_48_TO_63m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_alt_port_members_48);
    return rv;
}

/*
 * Function:
 *      _bcm_th2_ecmp_dlb_nhi_member_set
 * Purpose:
 *      Set dlb port numbers corresponding to each ecmp member NH.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      intf_array      - (IN) intf array for each NH.
 *      intf_count      - (IN) INTF count.
 *      primary_count   - (IN) Primary INTF count.
 *      dlb_id          - (IN) DLB group id.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_l3_ecmp_dlb_nhi_member_set(int unit, int *intf_array, int primary_count, int unique_intfs, int dlb_id)
{
    int alt_nh_start;
    int rv = BCM_E_NONE;
    int alt_count = unique_intfs - primary_count;

    dlb_ecmp_group_nhi_members_0_to_12_entry_t dlb_nhi_members;
    dlb_ecmp_group_nhi_members_13_to_25_entry_t dlb_nhi_members_13;
    dlb_ecmp_group_nhi_members_26_to_38_entry_t dlb_nhi_members_26;
    dlb_ecmp_group_nhi_members_39_to_51_entry_t dlb_nhi_members_39;
    dlb_ecmp_group_nhi_members_52_to_63_entry_t dlb_nhi_members_52;

    dlb_ecmp_group_alt_nhi_members_0_to_12_entry_t dlb_alt_nhi_members;
    dlb_ecmp_group_alt_nhi_members_13_to_25_entry_t dlb_alt_nhi_members_13;
    dlb_ecmp_group_alt_nhi_members_26_to_38_entry_t dlb_alt_nhi_members_26;
    dlb_ecmp_group_alt_nhi_members_39_to_51_entry_t dlb_alt_nhi_members_39;
    dlb_ecmp_group_alt_nhi_members_52_to_63_entry_t dlb_alt_nhi_members_52;

    sal_memset(&dlb_nhi_members, 0,
            sizeof(dlb_ecmp_group_nhi_members_0_to_12_entry_t));
    sal_memset(&dlb_nhi_members_13, 0,
            sizeof(dlb_ecmp_group_nhi_members_13_to_25_entry_t));
    sal_memset(&dlb_nhi_members_26, 0,
            sizeof(dlb_ecmp_group_nhi_members_26_to_38_entry_t));
    sal_memset(&dlb_nhi_members_39, 0,
            sizeof(dlb_ecmp_group_nhi_members_39_to_51_entry_t));
    sal_memset(&dlb_nhi_members_52, 0,
            sizeof(dlb_ecmp_group_nhi_members_52_to_63_entry_t));

    WRITE_DLB_MEMBER_NHI_0_12(unit, 0, &dlb_nhi_members,
            (intf_array[0] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 1, &dlb_nhi_members,
            (intf_array[1] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 2, &dlb_nhi_members,
            (intf_array[2] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 3, &dlb_nhi_members,
            (intf_array[3] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 4, &dlb_nhi_members,
            (intf_array[4] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 5, &dlb_nhi_members,
            (intf_array[5] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 6, &dlb_nhi_members,
            (intf_array[6] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 7, &dlb_nhi_members,
            (intf_array[7] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 8, &dlb_nhi_members,
            (intf_array[8] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 9, &dlb_nhi_members,
            (intf_array[9] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 10, &dlb_nhi_members,
            (intf_array[10] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 11, &dlb_nhi_members,
            (intf_array[11] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_0_12(unit, 12, &dlb_nhi_members,
            (intf_array[12] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_nhi_members);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 13, &dlb_nhi_members_13,
            (intf_array[13] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 14, &dlb_nhi_members_13,
            (intf_array[14] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 15, &dlb_nhi_members_13,
            (intf_array[15] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 15, &dlb_nhi_members_13,
            (intf_array[15] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 16, &dlb_nhi_members_13,
            (intf_array[16] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 17, &dlb_nhi_members_13,
            (intf_array[17] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 18, &dlb_nhi_members_13,
            (intf_array[18] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 19, &dlb_nhi_members_13,
            (intf_array[19] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 20, &dlb_nhi_members_13,
            (intf_array[20] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 21, &dlb_nhi_members_13,
            (intf_array[21] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 22, &dlb_nhi_members_13,
            (intf_array[22] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 23, &dlb_nhi_members_13,
            (intf_array[23] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 24, &dlb_nhi_members_13,
            (intf_array[24] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_13_25(unit, 25, &dlb_nhi_members_13,
            (intf_array[25] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_NHI_MEMBERS_13_TO_25m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_nhi_members_13);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 26, &dlb_nhi_members_26,
            (intf_array[26] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 27, &dlb_nhi_members_26,
            (intf_array[27] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 28, &dlb_nhi_members_26,
            (intf_array[28] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 29, &dlb_nhi_members_26,
            (intf_array[29] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 30, &dlb_nhi_members_26,
            (intf_array[30] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 31, &dlb_nhi_members_26,
            (intf_array[31] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 32, &dlb_nhi_members_26,
            (intf_array[32] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 33, &dlb_nhi_members_26,
            (intf_array[33] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 34, &dlb_nhi_members_26,
            (intf_array[34] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 35, &dlb_nhi_members_26,
            (intf_array[35] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 36, &dlb_nhi_members_26,
            (intf_array[36] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 37, &dlb_nhi_members_26,
            (intf_array[37] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_26_38(unit, 38, &dlb_nhi_members_26,
            (intf_array[38] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_NHI_MEMBERS_26_TO_38m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_nhi_members_26);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 39, &dlb_nhi_members_39,
            (intf_array[39] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 40, &dlb_nhi_members_39,
            (intf_array[40] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 41, &dlb_nhi_members_39,
            (intf_array[41] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 42, &dlb_nhi_members_39,
            (intf_array[42] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 43, &dlb_nhi_members_39,
            (intf_array[43] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 44, &dlb_nhi_members_39,
            (intf_array[44] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 45, &dlb_nhi_members_39,
            (intf_array[45] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 46, &dlb_nhi_members_39,
            (intf_array[46] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 47, &dlb_nhi_members_39,
            (intf_array[47] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 48, &dlb_nhi_members_39,
            (intf_array[48] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 49, &dlb_nhi_members_39,
            (intf_array[49] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 50, &dlb_nhi_members_39,
            (intf_array[50] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_39_51(unit, 51, &dlb_nhi_members_39,
            (intf_array[51] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_NHI_MEMBERS_39_TO_51m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_nhi_members_39);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 52, &dlb_nhi_members_52,
            (intf_array[52] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 53, &dlb_nhi_members_52,
            (intf_array[53] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 54, &dlb_nhi_members_52,
            (intf_array[54] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 55, &dlb_nhi_members_52,
            (intf_array[55] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 56, &dlb_nhi_members_52,
            (intf_array[56] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 57, &dlb_nhi_members_52,
            (intf_array[57] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 58, &dlb_nhi_members_52,
            (intf_array[58] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 59, &dlb_nhi_members_52,
            (intf_array[59] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 60, &dlb_nhi_members_52,
            (intf_array[60] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 61, &dlb_nhi_members_52,
            (intf_array[61] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 62, &dlb_nhi_members_52,
            (intf_array[62] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    WRITE_DLB_MEMBER_NHI_52_63(unit, 63, &dlb_nhi_members_52,
            (intf_array[63] - BCM_XGS3_EGRESS_IDX_MIN(unit)), primary_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_NHI_MEMBERS_52_TO_63m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_nhi_members_52);

    if (alt_count <= 0) {
        return rv;
    }
    alt_nh_start = primary_count;

    sal_memset(&dlb_alt_nhi_members, 0,
            sizeof(dlb_ecmp_group_alt_nhi_members_0_to_12_entry_t));
    sal_memset(&dlb_alt_nhi_members_13, 0,
            sizeof(dlb_ecmp_group_alt_nhi_members_13_to_25_entry_t));
    sal_memset(&dlb_alt_nhi_members_26, 0,
            sizeof(dlb_ecmp_group_alt_nhi_members_26_to_38_entry_t));
    sal_memset(&dlb_alt_nhi_members_39, 0,
            sizeof(dlb_ecmp_group_alt_nhi_members_39_to_51_entry_t));
    sal_memset(&dlb_alt_nhi_members_52, 0,
            sizeof(dlb_ecmp_group_alt_nhi_members_52_to_63_entry_t));

    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 0, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 1, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 1] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 2, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 2] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 3, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 3] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 4, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 4] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 5, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 5] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 6, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 6] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 7, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 7] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 8, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 8] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 9, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 9] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 10, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 10] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 11, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 11] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_0_12(unit, 12, &dlb_alt_nhi_members,
       (intf_array[alt_nh_start + 12] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_alt_nhi_members);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 13, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 13] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 14, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 14] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 15, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 15] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 15, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 15] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 16, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 16] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 17, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 17] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 18, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 18] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 19, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 19] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 20, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 20] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 21, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 21] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 22, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 22] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 23, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 23] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 24, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 24] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_13_25(unit, 25, &dlb_alt_nhi_members_13,
       (intf_array[alt_nh_start + 25] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_13_TO_25m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_alt_nhi_members_13);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 26, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 26] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 27, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 27] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 28, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 28] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 29, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 29] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 30, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 30] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 31, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 31] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 32, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 32] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 33, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 33] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 34, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 34] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 35, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 35] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 36, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 36] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 37, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 37] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_26_38(unit, 38, &dlb_alt_nhi_members_26,
       (intf_array[alt_nh_start + 38] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_26_TO_38m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_alt_nhi_members_26);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 39, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 39] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 40, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 40] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 41, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 41] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 42, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 42] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 43, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 43] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 44, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 44] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 45, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 45] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 46, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 46] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 47, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 47] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 48, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 48] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 49, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 49] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 50, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 50] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_39_51(unit, 51, &dlb_alt_nhi_members_39,
       (intf_array[alt_nh_start + 51] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_39_TO_51m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_alt_nhi_members_39);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 52, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 52] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 53, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 53] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 54, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 54] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 55, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 55] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 56, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 56] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 57, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 57] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 58, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 58] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 59, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 59] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 60, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 60] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 61, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 61] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 62, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 62] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    WRITE_DLB_MEMBER_ALT_NHI_52_63(unit, 63, &dlb_alt_nhi_members_52,
       (intf_array[alt_nh_start + 63] - BCM_XGS3_EGRESS_IDX_MIN(unit)), alt_count);
    rv = soc_mem_write(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_52_TO_63m,
                       MEM_BLOCK_ALL, (dlb_id), &dlb_alt_nhi_members_52);

    return rv;
}

/*
 * Function:
 *      _bcm_th3_l3_ecmp_dlb_scaling_factor_encode
 * Purpose:
 *      Check valid scaling factors, and encode them.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      scaling_factor  - (IN) Threshold scaling factor.
 * Returns:
 *      1: check success
 *      0: check fail
 */
int
_bcm_th3_l3_ecmp_dlb_scaling_factor_encode(int unit, int scaling_factor,
                                        int *scaling_factor_hw)
{
    int bcm_factors[] = {10, 25, 40, 50, 75, 100, 200, 400};
    int hw_factors[]  = { 0,  1,  2,  3,  4,   5,   6,   7};
    int i;

    for (i = 0; i < COUNTOF(bcm_factors); i++ ) {
        if (scaling_factor == bcm_factors[i]) {
            break;
        }
    }

    if (i < COUNTOF(hw_factors)) {
        if (scaling_factor_hw) {
            *scaling_factor_hw = hw_factors[i];
        }
        return 1;
    }
    return 0;
}


/*
 * Function:
 *      _bcm_th3_l3_ecmp_dlb_scaling_factor_decode
 * Purpose:
 *      Decode scaling factors.
 * Parameters:
 *      unit            - (IN) SOC unit number.
 *      scaling_factor  - (IN) Threshold scaling factor.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_l3_ecmp_dlb_scaling_factor_decode(int unit, int scaling_factor_hw,
                                        int *scaling_factor)
{
    int bcm_factors[] = {10, 25, 40, 50, 75, 100, 200, 400};
    int hw_factors[]  = { 0,  1,  2,  3,  4,   5,   6,   7};
    int i;

    for (i = 0; i < COUNTOF(hw_factors); i++ ) {
        if (scaling_factor_hw == hw_factors[i]) {
            break;
        }
    }

    if (i == COUNTOF(hw_factors)) {
        return BCM_E_INTERNAL;
    }

    *scaling_factor = bcm_factors[i];
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_l3_ecmp_dlb_mon_config_set
 * Purpose:
 *      This function configures DLB flow monitoring parameters for a DLB id
 *      derived from ECMP group id (the second argument). Property
 *      spn_DLB_FLOW_MONITOR_EN must be enabled before invoking this function.
 *      See Notes.
 * Parameters:
 *      unit :        (IN) Device's unit number
 *      ecmp_intf :   (IN) ECMP interface id obtained from bcm_l3_ecmp_create or
 *                         bcm_l3_egress_ecmp_create
 *      dlb_mon_cfg : (IN) DLB flow monitoring configuration
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      1. If 'sample_rate' is specified as 0, monitoring will not be enabled,
 *         irrespective of 'enable' setting in 'dlb_mon_cfg'.
 *      2. If 'enable' field in dlb_mon_cfg is set to 0, we still go ahead and
 *         program the monitoring information because IFP uses the threshold and
 *         action values programmed in h/w (it is shared information)
 */
int
bcm_th3_l3_ecmp_dlb_mon_config_set(int unit,
                                   bcm_if_t ecmp_intf,
                                   bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg)
{
    int dlb_id;
    int dlb_mode;
    uint64 dlb_en_bitmap;
    uint32 dlb_enabled;
    dlb_ecmp_group_control_entry_t dlb_ecmp_group_control_entry;
    uint32 threshold = 0;
    uint32 enable = 0;
    uint32 copy_to_cpu = 0;
    uint32 mirror = 0;
    dlb_ecmp_group_monitor_control_entry_t mon_ctrl;
    int max_val;
    int ecmp_group_idx;

    /* If application has not defined this feature, or not enabled this feature,
     * return error
     */
    if (!soc_property_get(unit, spn_DLB_FLOW_MONITOR_EN, 0)) {
        return BCM_E_UNAVAIL;
    }

    if (dlb_mon_cfg == NULL) {
        return BCM_E_PARAM;
    }

    ecmp_group_idx = ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    if (ecmp_group_idx < BCM_TH3_L3_ECMP_DLB_START(unit)) {
        return BCM_E_CONFIG;
    }

    /* Get the DLB id associated with the Ecmp group */
    dlb_id = _bcm_th3_l3_ecmp_dlb_get_id(unit, ecmp_group_idx);

    /* DLB id of 0 is not supported for DLB flow monitoring. If we obtained
     * it (0), return error
     */
    if (dlb_id <= 0) {
        return BCM_E_INTERNAL;
    }

    /* Check if the DLB obtained is in enabled state */
    if (dlb_id < soc_reg_field_length(unit, DLB_ID_0_TO_63_ENABLEr, BITMAPf)) {
        SOC_IF_ERROR_RETURN( \
          READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap));
        dlb_enabled = COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id);
    } else {
        SOC_IF_ERROR_RETURN( \
          READ_DLB_ID_64_TO_127_ENABLEr(unit, &dlb_en_bitmap));
         dlb_enabled = COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id -
                               (soc_reg_field_length(unit,
                               DLB_ID_64_TO_127_ENABLEr, BITMAPf)));
    }

    if (!dlb_enabled) {
        return BCM_E_CONFIG;
    }

    /* Get DLB operational mode */
    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY,
                            dlb_id, &dlb_ecmp_group_control_entry));

    dlb_mode = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                   &dlb_ecmp_group_control_entry, PORT_ASSIGNMENT_MODEf);

    /* If DLB group is in PACKET_SPRAY mode, return error. Refer arch doc
     * for details
     */
    if (dlb_mode == 2) {
        return BCM_E_CONFIG;
    }

    /* Validate configuration info passed */
    if ((dlb_mon_cfg->action < BCM_L3_ECMP_DLB_MON_COPY_TO_CPU) ||
        (dlb_mon_cfg->action > BCM_L3_ECMP_DLB_MON_ACTION_NONE)) {
        return BCM_E_PARAM;
    }


    max_val = (1 << soc_mem_field_length(unit, DLB_ECMP_GROUP_MONITOR_CONTROLm,
                                         SAMPLE_THRESHOLDf));

    enable = dlb_mon_cfg->enable ? 1 : 0;

    /* Program h/w only if a non-zero rate is specified. Rate of 0 means
     * 'do not sample', which means effectively disabling monitoring
     */
    if (dlb_mon_cfg->sample_rate > 0) {

        /* Calculate threshold based on sampling rate provided. We complement
         * the value calculated since packet sampling happens when h/w's
         * random number generator is >= the programmed threshold
         * (In case of sflow, sampling happens if RNG value <= threshold)
         */

        /* Rate of 1 means sample each packet. In this case we need to adjust
         * value to be the max value that can be fitted in SAMPLE_THRESHOLDf
         */
        if (dlb_mon_cfg->sample_rate == 1) {
            max_val = max_val - 1;
        }

        threshold = max_val / dlb_mon_cfg->sample_rate;

        /* Complement the value */
        /* Note 'max_val - 1' is used here as mask to zero out bits 31:29 */
        threshold = ~threshold & (max_val - 1);
    } else {
        threshold = 0;
        enable = 0;
    }

    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_GROUP_MONITOR_CONTROLm(unit,
                                                             MEM_BLOCK_ANY,
                                                             dlb_id,
                                                             &mon_ctrl));

    /* Set Monitor enable and threshold fields */
    soc_DLB_ECMP_GROUP_MONITOR_CONTROLm_field32_set(unit, &mon_ctrl,
                                                    MONITOR_ENf, enable);

    soc_DLB_ECMP_GROUP_MONITOR_CONTROLm_field32_set(unit, &mon_ctrl,
                                                    SAMPLE_THRESHOLDf,
                                                    threshold);

    /* If application is enabling monitoring, only then program actions
     * passed by the application, else reset the action
     */
    if (dlb_mon_cfg->sample_rate > 0) {
        /* Set Copy to CPU and/or Mirror bit(s) */
        switch (dlb_mon_cfg->action) {
            case BCM_L3_ECMP_DLB_MON_COPY_TO_CPU:
                copy_to_cpu = 1;
                mirror = 0;
                break;

            case BCM_L3_ECMP_DLB_MON_MIRROR:
                copy_to_cpu = 0;
                mirror = 1;
                break;

            case BCM_L3_ECMP_DLB_MON_COPY_TO_CPU_AND_MIRROR:
                copy_to_cpu = 1;
                mirror = 1;
                break;

            case BCM_L3_ECMP_DLB_MON_ACTION_NONE:
                copy_to_cpu = 0;
                mirror = 0;
                break;

            default:
                /* This case should not be reached */
                return BCM_E_CONFIG;
                break;
        }
    } else {
        copy_to_cpu = 0;
        mirror = 0;
    }

    soc_DLB_ECMP_GROUP_MONITOR_CONTROLm_field32_set(unit, &mon_ctrl,
                                                    COPY_TO_CPUf, copy_to_cpu);

    soc_DLB_ECMP_GROUP_MONITOR_CONTROLm_field32_set(unit, &mon_ctrl,
                                                    MIRRORf, mirror);

    BCM_IF_ERROR_RETURN(WRITE_DLB_ECMP_GROUP_MONITOR_CONTROLm(unit,
                                                              MEM_BLOCK_ANY,
                                                              dlb_id,
                                                              &mon_ctrl));

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcm_th3_l3_ecmp_dlb_mon_config_get
 * Purpose:
 *    This function reads DLB monitoring parameters from hardware, for a DLB id
 *    derived from ECMP group id. Property spn_DLB_FLOW_MONITOR_EN must be
 *    enabled before invoking this function. See Notes.
 * Parameters:
 *    unit :        (IN) Device's unit number
 *    ecmp_intf :   (IN) ECMP interface id obtained from bcm_l3_ecmp_create or,
 *                       bcm_l3_egress_ecmp_create
 *    dlb_mon_cfg : (OUT) DLB flow monitoring configuration read from h/w for
 *                        specified DLB
 * Returns:
 *    BCM_E_XXX
 * Notes:
 * 1. Even if MONITOR_EN is set to 0 for a DLB id, IFP can use the threshold and
 *    action values. So we return the value read from h/w to the user even if
 *    MONITOR_EN is set to 0.
 * 2. Sample rate calculated from threshold may not exactly match the sample
 *    rate programmed via config_set API. This is because we do integer division
 *    to calculate the threhold in the config_set, and store the value obtained
 *    in the threshold field. And in this function, we perform integer division
 *    again.
 */
int bcm_th3_l3_ecmp_dlb_mon_config_get(int unit,
                                       bcm_if_t ecmp_intf,
                                       bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg)
{
    int dlb_id;
    int dlb_mode;
    uint64 dlb_en_bitmap;
    uint32 dlb_enabled;
    dlb_ecmp_group_control_entry_t dlb_ecmp_group_control_entry;
    uint32 threshold = 0;
    uint32 enable = 0;
    uint32 copy_to_cpu = 0;
    uint32 mirror = 0;
    dlb_ecmp_group_monitor_control_entry_t mon_ctrl;
    int max_val;
    int ecmp_group_idx;

    /* If application has not defined this feature, or not enabled this feature,
     * return error
     */
    if (!soc_property_get(unit, spn_DLB_FLOW_MONITOR_EN, 0)) {
        return BCM_E_UNAVAIL;
    }

    if (dlb_mon_cfg == NULL) {
        return BCM_E_PARAM;
    }

    ecmp_group_idx = ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    if (ecmp_group_idx < BCM_TH3_L3_ECMP_DLB_START(unit)) {
        return BCM_E_CONFIG;
    }

    /* Get the DLB id associated with the Ecmp group */
    dlb_id = _bcm_th3_l3_ecmp_dlb_get_id(unit, ecmp_group_idx);

    /* DLB id of 0 is not supported for DLB flow monitoring. If we obtained
     * it (0), return error
     */
    if (dlb_id <= 0) {
        return BCM_E_INTERNAL;
    }

    /* Check if the DLB obtained is in enabled state */
    if (dlb_id < soc_reg_field_length(unit, DLB_ID_0_TO_63_ENABLEr, BITMAPf)) {
        SOC_IF_ERROR_RETURN( \
          READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap));
        dlb_enabled = COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id);
    } else {
        SOC_IF_ERROR_RETURN( \
          READ_DLB_ID_64_TO_127_ENABLEr(unit, &dlb_en_bitmap));
         dlb_enabled = COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id -
                               (soc_reg_field_length(unit,
                               DLB_ID_64_TO_127_ENABLEr, BITMAPf)));
    }

    if (!dlb_enabled) {
        return BCM_E_CONFIG;
    }

   /* Get DLB operational mode */
    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY,
                            dlb_id, &dlb_ecmp_group_control_entry));

    dlb_mode = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                   &dlb_ecmp_group_control_entry, PORT_ASSIGNMENT_MODEf);

    /* If DLB group is in PACKET_SPRAY mode, return error. Refer arch doc
     * for details
     */
    if (dlb_mode == 2) {
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN(READ_DLB_ECMP_GROUP_MONITOR_CONTROLm(unit, MEM_BLOCK_ANY, dlb_id, &mon_ctrl));

    enable = soc_DLB_ECMP_GROUP_MONITOR_CONTROLm_field32_get(unit, &mon_ctrl, MONITOR_ENf);

    threshold = soc_DLB_ECMP_GROUP_MONITOR_CONTROLm_field32_get(unit, &mon_ctrl, SAMPLE_THRESHOLDf);

    copy_to_cpu = soc_DLB_ECMP_GROUP_MONITOR_CONTROLm_field32_get(unit, &mon_ctrl, COPY_TO_CPUf);

    mirror = soc_DLB_ECMP_GROUP_MONITOR_CONTROLm_field32_get(unit, &mon_ctrl, MIRRORf);

    dlb_mon_cfg->enable = enable;

    if (copy_to_cpu && mirror) {
        dlb_mon_cfg->action = BCM_L3_ECMP_DLB_MON_COPY_TO_CPU_AND_MIRROR;
    } else {
        if (copy_to_cpu) {
            dlb_mon_cfg->action = BCM_L3_ECMP_DLB_MON_COPY_TO_CPU;
        } else {
            if (mirror) {
                dlb_mon_cfg->action = BCM_L3_ECMP_DLB_MON_MIRROR;
            } else {
                dlb_mon_cfg->action = BCM_L3_ECMP_DLB_MON_ACTION_NONE;
            }
        }
    }

    max_val = (1 << soc_mem_field_length(unit,
                                         DLB_ECMP_GROUP_MONITOR_CONTROLm,
                                         SAMPLE_THRESHOLDf));

    /* Calculate sample_rate from sample threshold */
    /* 'max_val - 1' is used as mask here */
    threshold = ~threshold & (max_val - 1);

    if (threshold == 0) {
        dlb_mon_cfg->sample_rate = max_val;
    } else {
        dlb_mon_cfg->sample_rate = max_val / threshold;
    }

    return BCM_E_NONE;
}

int _bcm_th3_set_port_list(int unit, int port, int dlb_id, int nh_index)
{

    _th2_ecmp_dlb_port_membership_t *current_ptr, *first_ptr;
    _th2_ecmp_dlb_port_membership_t *membership;
    /* now have nh_index (ecmp_member[k]), port (port), dlb_id  */
    /* find it nexthop exists in specific port list*/
    current_ptr = ECMP_DLB_PORT_LIST(unit, port);
    first_ptr = NULL;
    while (current_ptr) {
        if (current_ptr->nh_index != nh_index) {
            current_ptr = current_ptr->next;
            continue;
        }

        if (dlb_id == current_ptr->dlb_id) {
            current_ptr->ref_count++;
            break;
        }
        if (first_ptr == NULL) {
            first_ptr = current_ptr;
        }

        current_ptr = current_ptr->next;
    }

    if (current_ptr == NULL) {
        /* Insert member ID and group to the next hop's linked list of member
         * IDs and groups.
         */
        membership = sal_alloc(sizeof(_th2_ecmp_dlb_port_membership_t),
                "ecmp dlb nh membership");
        if (NULL == membership) {
            return BCM_E_MEMORY;
        }
        sal_memset(membership, 0, sizeof(_th2_ecmp_dlb_port_membership_t));
        membership->nh_index = nh_index;
        membership->ref_count = 1;
        membership->dlb_id    = dlb_id;

        if (first_ptr != NULL) {
        /* didnt find exact same node(nh_index, dlb_id) ,
         * but find same nh_index (A).
         * create B after A.
         * first_ptr points to A.
         */
            membership->next = first_ptr->next;
            first_ptr->next = membership;
            ECMP_DLB_PORT_INFO(unit, port).dlb_count++;
        } else {
        /* didnt find same nh_index*/
        /* this is new nh_index, insert it at head */
            membership->next = ECMP_DLB_PORT_LIST(unit, port);
            ECMP_DLB_PORT_LIST(unit, port) = membership;
            ECMP_DLB_PORT_INFO(unit, port).nh_count++;
            ECMP_DLB_PORT_INFO(unit, port).dlb_count++;
        }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_th3_ecmp_dlb_member_recover
 * Purpose:
 *      Recover ECMP DLB ports usage and the next hop indices associated
 *      with the ports.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th3_ecmp_dlb_member_recover(int unit)
{
    int rv = BCM_E_NONE;
    int port_map_width;
    int alloc_size_port_map;
    SHR_BITDCL *port_map = NULL;
    SHR_BITDCL *port_map2 = NULL;
    int dlb_id;
    dlb_ecmp_group_membership_entry_t group_membership_entry;
    int ecmp_count = 0;           /* Next hop count in the group.        */
    bcm_if_t *ecmp_member = NULL; /* Ecmp group from hw.                 */
    dlb_ecmp_group_control_entry_t group_control_entry;
    int j;
    int membership_pointer;
    int offset;
    int count;
    int nh_index;
    bcm_l3_egress_ecmp_t tmp_ecmp;
    uint64 dlb_en_bitmap;

    dlb_ecmp_group_nhi_members_0_to_12_entry_t dlb_nhi_members;
    dlb_ecmp_group_nhi_members_13_to_25_entry_t dlb_nhi_members_13;
    dlb_ecmp_group_nhi_members_26_to_38_entry_t dlb_nhi_members_26;
    dlb_ecmp_group_nhi_members_39_to_51_entry_t dlb_nhi_members_39;
    dlb_ecmp_group_nhi_members_52_to_63_entry_t dlb_nhi_members_52;

    dlb_ecmp_group_alt_nhi_members_0_to_12_entry_t dlb_alt_nhi_members;
    dlb_ecmp_group_alt_nhi_members_13_to_25_entry_t dlb_alt_nhi_members_13;
    dlb_ecmp_group_alt_nhi_members_26_to_38_entry_t dlb_alt_nhi_members_26;
    dlb_ecmp_group_alt_nhi_members_39_to_51_entry_t dlb_alt_nhi_members_39;
    dlb_ecmp_group_alt_nhi_members_52_to_63_entry_t dlb_alt_nhi_members_52;

    dlb_ecmp_group_port_members_0_to_15_entry_t dlb_port_members;
    dlb_ecmp_group_port_members_16_to_31_entry_t dlb_port_members_16;
    dlb_ecmp_group_port_members_32_to_47_entry_t dlb_port_members_32;
    dlb_ecmp_group_port_members_48_to_63_entry_t dlb_port_members_48;

    dlb_ecmp_group_alt_port_members_0_to_15_entry_t dlb_alt_port_members;
    dlb_ecmp_group_alt_port_members_16_to_31_entry_t dlb_alt_port_members_16;
    dlb_ecmp_group_alt_port_members_32_to_47_entry_t dlb_alt_port_members_32;
    dlb_ecmp_group_alt_port_members_48_to_63_entry_t dlb_alt_port_members_48;

    /* Allocate port map */
    port_map_width = soc_mem_field_length(unit,
            DLB_ECMP_GROUP_MEMBERSHIPm, PORT_MAPf);
    alloc_size_port_map = SHR_BITALLOCSIZE(port_map_width);
    port_map = sal_alloc(alloc_size_port_map, "DLB ECMP port map");
    if (NULL == port_map) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    port_map2 = sal_alloc(alloc_size_port_map, "DLB ECMP port map 2");
    if (NULL == port_map2) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    /* Allocate ECMP group members */
    ecmp_member = sal_alloc(128 * sizeof(bcm_if_t),
            "ecmp member array");
    if (NULL == ecmp_member) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    for (dlb_id = 0; dlb_id < 128; dlb_id++) {
        /* Enable bits for the first 64 groups are in
           DLB_ID_0_TO_63_ENABLE register and the next 64 are in
           DLB_ID_64_TO_127_ENABLE. */
        if (dlb_id < soc_reg_field_length(unit,
                     DLB_ID_0_TO_63_ENABLEr, BITMAPf)) {
            rv = READ_DLB_ID_0_TO_63_ENABLEr(unit, &dlb_en_bitmap);
            if (rv < 0) {
                goto cleanup;
            }
            if (!(COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id))) {
                continue;
            }
        } else {
            rv = READ_DLB_ID_64_TO_127_ENABLEr(unit, &dlb_en_bitmap);
            if (rv < 0) {
                goto cleanup;
            }
            if (!(COMPILER_64_BITTEST(dlb_en_bitmap, dlb_id -
                soc_reg_field_length(unit, DLB_ID_0_TO_63_ENABLEr, BITMAPf)))) {
                continue;
            }
        }

        rv = READ_DLB_ECMP_GROUP_CONTROLm(unit, MEM_BLOCK_ANY, dlb_id,
                                          &group_control_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        membership_pointer = soc_DLB_ECMP_GROUP_CONTROLm_field32_get(unit,
                &group_control_entry, GROUP_MEMBERSHIP_POINTERf);

        rv = READ_DLB_ECMP_GROUP_MEMBERSHIPm(unit, MEM_BLOCK_ANY,
                                             membership_pointer,
                                             &group_membership_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* We have the DLB_ID here,
         * now there are 2 ways to recover these info from hw to sw:
         * 1) Use the DLB_ID to get port_map,
         *    the port map will indicate how many port2member entries are set.
         *    traverse the valid port2member entries via port map.
         *    Each port2member entry can be parsed to get the valid bits and then
         *    the nexthops. Got the nexthop, the DLB_ID, the port. It's enough
         *    to recover the sw state.
         * 2) The first method has a side effect, which is traverse all valid
         *    port2member entries, and then traverse the valid bits of each.
         *    That's portnumber * 64 loops in total.
         *    An alternative way is to get the ecmp members (64 members at most)
         *    Parse the entries and then get the ports. Same as above, we got
         *    all the needed info: nexthop, DLB_ID, port.
         *
         *    Actually, the port2member entries are not necessary, if DLB can
         *    traverse the ecmp member table and get all nexthops pointing
         *    to the chosen port, it's done. In fact, port2member is just another
         *    angle of ecmp member table.
         */

        /* Get port map */
        sal_memset(port_map, 0, alloc_size_port_map);
        soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_get(unit,
                &group_membership_entry, PORT_MAPf, port_map);

        sal_memset(port_map2, 0, alloc_size_port_map);
        soc_DLB_ECMP_GROUP_MEMBERSHIPm_field_get(unit,
                &group_membership_entry, PORT_MAP_ALTERNATEf, port_map2);

        /* Get ECMP members */
        /* Check if the ECMP group is DLB enabled. */
        bcm_l3_egress_ecmp_t_init(&tmp_ecmp);
        tmp_ecmp.ecmp_intf = dlb_id + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit) + BCM_TH3_L3_ECMP_DLB_START(unit);
        /*
         * Increase entry reference count to avoid the failure of calling
         * ecmp_get since the reference count of ecmp group is recovered along
         * with hash value after the recovery of DLB info.
         */
        BCM_XGS3_L3_ENT_REF_CNT_INC(
            BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp), dlb_id + BCM_TH3_L3_ECMP_DLB_START(unit), _BCM_SINGLE_WIDE);
        rv = _bcm_esw_l3_egress_ecmp_get(unit, &tmp_ecmp, 0,
                                         NULL, &ecmp_count, TRUE);
        BCM_XGS3_L3_ENT_REF_CNT_DEC(
            BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp), dlb_id + BCM_TH3_L3_ECMP_DLB_START(unit), _BCM_SINGLE_WIDE);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if (tmp_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_NORMAL &&
            tmp_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED &&
            tmp_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL &&
            tmp_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        if (ecmp_count > 128) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        sal_memset(ecmp_member, 0, 128 * sizeof(bcm_if_t));
        /*
         * Increase entry reference count to avoid the failure of calling
         * ecmp_get since the reference count of ecmp group is recovered along
         * with hash value after the recovery of DLB info.
         */
        BCM_XGS3_L3_ENT_REF_CNT_INC(
            BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp), dlb_id + BCM_TH3_L3_ECMP_DLB_START(unit), _BCM_SINGLE_WIDE);
        rv = _bcm_esw_l3_egress_ecmp_get(unit, &tmp_ecmp, ecmp_count,
                                         ecmp_member, &count, TRUE);
        BCM_XGS3_L3_ENT_REF_CNT_DEC(
            BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp), dlb_id + BCM_TH3_L3_ECMP_DLB_START(unit), _BCM_SINGLE_WIDE);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        for (j = 0; j < ecmp_count; j++) {
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, ecmp_member[j])) {
                offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
            } else {
                rv = BCM_E_PARAM;
                goto cleanup;
            }
            ecmp_member[j] -= offset;
        }

        SHR_BITOR_RANGE(port_map, port_map2, 0, port_map_width, port_map);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_nhi_members);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_NHI_MEMBERS_13_TO_25m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_nhi_members_13);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_NHI_MEMBERS_26_TO_38m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_nhi_members_26);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_NHI_MEMBERS_39_TO_51m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_nhi_members_39);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_NHI_MEMBERS_52_TO_63m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_nhi_members_52);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_port_members);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_PORT_MEMBERS_16_TO_31m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_port_members_16);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_PORT_MEMBERS_32_TO_47m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_port_members_32);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_PORT_MEMBERS_48_TO_63m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_port_members_48);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_alt_port_members);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_16_TO_31m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_alt_port_members_16);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_32_TO_47m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_alt_port_members_32);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_ALT_PORT_MEMBERS_48_TO_63m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_alt_port_members_48);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_alt_nhi_members);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_13_TO_25m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_alt_nhi_members_13);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_26_TO_38m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_alt_nhi_members_26);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_39_TO_51m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_alt_nhi_members_39);

        rv = soc_mem_read(unit, DLB_ECMP_GROUP_ALT_NHI_MEMBERS_52_TO_63m,
                           SOC_BLOCK_ANY, (dlb_id), &dlb_alt_nhi_members_52);

        _DLB_SET_GROUP_PTR_0_12(0);
        _DLB_SET_GROUP_PTR_0_12(1);
        _DLB_SET_GROUP_PTR_0_12(2);
        _DLB_SET_GROUP_PTR_0_12(3);
        _DLB_SET_GROUP_PTR_0_12(4);
        _DLB_SET_GROUP_PTR_0_12(5);
        _DLB_SET_GROUP_PTR_0_12(6);
        _DLB_SET_GROUP_PTR_0_12(7);
        _DLB_SET_GROUP_PTR_0_12(8);
        _DLB_SET_GROUP_PTR_0_12(9);
        _DLB_SET_GROUP_PTR_0_12(10);
        _DLB_SET_GROUP_PTR_0_12(11);
        _DLB_SET_GROUP_PTR_0_12(12);

        _DLB_SET_GROUP_PTR_13_25(13);
        _DLB_SET_GROUP_PTR_13_25(14);
        _DLB_SET_GROUP_PTR_13_25(15);
        _DLB_SET_GROUP_PTR_13_25(16);
        _DLB_SET_GROUP_PTR_13_25(17);
        _DLB_SET_GROUP_PTR_13_25(18);
        _DLB_SET_GROUP_PTR_13_25(19);
        _DLB_SET_GROUP_PTR_13_25(20);
        _DLB_SET_GROUP_PTR_13_25(21);
        _DLB_SET_GROUP_PTR_13_25(22);
        _DLB_SET_GROUP_PTR_13_25(23);
        _DLB_SET_GROUP_PTR_13_25(24);
        _DLB_SET_GROUP_PTR_13_25(25);

        _DLB_SET_GROUP_PTR_26_38(26);
        _DLB_SET_GROUP_PTR_26_38(27);
        _DLB_SET_GROUP_PTR_26_38(28);
        _DLB_SET_GROUP_PTR_26_38(29);
        _DLB_SET_GROUP_PTR_26_38(30);
        _DLB_SET_GROUP_PTR_26_38(31);
        _DLB_SET_GROUP_PTR_26_38(32);
        _DLB_SET_GROUP_PTR_26_38(33);
        _DLB_SET_GROUP_PTR_26_38(34);
        _DLB_SET_GROUP_PTR_26_38(35);
        _DLB_SET_GROUP_PTR_26_38(36);
        _DLB_SET_GROUP_PTR_26_38(37);
        _DLB_SET_GROUP_PTR_26_38(38);

        _DLB_SET_GROUP_PTR_39_51(39);
        _DLB_SET_GROUP_PTR_39_51(40);
        _DLB_SET_GROUP_PTR_39_51(41);
        _DLB_SET_GROUP_PTR_39_51(42);
        _DLB_SET_GROUP_PTR_39_51(43);
        _DLB_SET_GROUP_PTR_39_51(44);
        _DLB_SET_GROUP_PTR_39_51(45);
        _DLB_SET_GROUP_PTR_39_51(46);
        _DLB_SET_GROUP_PTR_39_51(47);
        _DLB_SET_GROUP_PTR_39_51(48);
        _DLB_SET_GROUP_PTR_39_51(49);
        _DLB_SET_GROUP_PTR_39_51(50);
        _DLB_SET_GROUP_PTR_39_51(51);

        _DLB_SET_GROUP_PTR_52_63(52);
        _DLB_SET_GROUP_PTR_52_63(53);
        _DLB_SET_GROUP_PTR_52_63(54);
        _DLB_SET_GROUP_PTR_52_63(55);
        _DLB_SET_GROUP_PTR_52_63(56);
        _DLB_SET_GROUP_PTR_52_63(57);
        _DLB_SET_GROUP_PTR_52_63(58);
        _DLB_SET_GROUP_PTR_52_63(59);
        _DLB_SET_GROUP_PTR_52_63(60);
        _DLB_SET_GROUP_PTR_52_63(61);
        _DLB_SET_GROUP_PTR_52_63(62);
        _DLB_SET_GROUP_PTR_52_63(63);

    }

cleanup:
    if (port_map) {
        sal_free(port_map);
    }
    if (port_map2) {
        sal_free(port_map2);
    }
    if (ecmp_member) {
        sal_free(ecmp_member);
    }
    return BCM_E_NONE;
}


#endif
