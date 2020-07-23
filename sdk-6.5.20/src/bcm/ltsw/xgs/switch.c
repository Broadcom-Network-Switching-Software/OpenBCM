/*! \file switch.c
 *
 * Switch control interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/switch.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/xgs/switch.h>
#include <bcm_int/ltsw/xgs/switch_defs.h>
#include <bcm_int/ltsw/ser.h>
#include <bcm_int/ltsw/dlb.h>
#include <bcm_int/ltsw/pfc.h>
#include <bcm_int/ltsw/rate.h>
#include <bcm_int/ltsw/bst.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/tunnel.h>
#include <bcm_int/ltsw/lb_hash.h>
#include <bcm_int/ltsw/uft.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/link.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/ipmc.h>
#include <bcm_int/ltsw/mpls.h>
#include <bcm_int/ltsw/generated/switch_ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/ltsw/ecn.h>
#include <bcm_int/ltsw/int.h>
#include <bcm_int/ltsw/flow.h>
#include <bcm_int/ltsw/time.h>
#include <bcm_int/ltsw/qos.h>

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

#define BSL_LOG_MODULE      BSL_LS_BCM_SWITCH

#ifndef BCM_SWITCH_FORWARD_VALUE
#define BCM_SWITCH_FORWARD_VALUE        0
#define BCM_SWITCH_DROP_VALUE           1
#define BCM_SWITCH_FLOOD_VALUE          2
#define BCM_SWITCH_RESERVED_VALUE       3
#endif

/*! Switch control ID field size */
#define scIdBits            16

/*! Switch control ID mask */
#define scIdMask            ((1 << scIdBits) - 1)
/*! Value type switch control */
#define scValueCtrl         (2 << scIdBits)
/*! Symbol type switch control */
#define scSymbolCtrl        (3 << scIdBits)
/*! Bit type switch control */
#define scBitCtrl           (4 << scIdBits)
/*! Symbol type switch control accessing by key */
#define scKeySymbolCtrl     (5 << scIdBits)
/*! PORT_PKT_CONTROL type switch control */
#define scPortPktCtrl       (6 << scIdBits)

/*! Software value type switch control, this type only update SW database */
#define scSoftwareValueCtrl (7 << scIdBits)

/*! Ignore port ID for device wide configuration */
#define PORT_IGNORE         -1

/*! HA subcomponent id for software value type switch control. */
#define HA_SC_SOFTWARE_SUB_COMP_ID   1

/*! HA subcomponent id for port based software value type switch control. */
#define HA_SCP_SOFTWARE_SUB_COMP_ID  2

/*!
 * \brief Type function definition structure.
 *
 * The \c set implements bcm_switch_control_set() function.
 * The \c get implements bcm_switch_control_get() function.
 */
typedef const struct sc_func_s {

    /*! Switch control type */
    int type;

    /*! Device-wide switch control set function */
    sc_set_f set;

    /*! Device-wide switch control get function */
    sc_get_f get;

} sc_func_t;

/*!
 * \brief Type function definition structure.
 *
 * The \c set implements bcm_switch_control_port_set() function.
 * The \c get implements bcm_switch_control_port_get() function.
 */
typedef const struct scp_func_s {

    /*! Switch control type */
    int type;

    /*! Port base switch control set function */
    scp_set_f set;

    /*! Port base switch control get function */
    scp_get_f get;

} scp_func_t;

/*!
 * \brief Symbol map structure
 *
 * Map a tring to a value.
 */
typedef struct sc_symbol_map_s {

    /*! Enum name. */
    const char *name;

    /*! Enum value. */
    int val;

} sc_symbol_map_t;

/*!
 * \brief Direct value configuration map
 *
 * This structure defines LT field (\c table and \c field), argument tranlation
 * function (\c encode and \c decode), per port or device-wide attributes for a
 * switch control type for sc_sc_value_ctrl_set/get() function to implement
 * the function.
 */
typedef struct sc_value_ctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! TRUE - per-port setting, FALSE - device-wide setting */
    bool perport;

    /*! Logic Table name */
    const char *table;

    /*! Logic Table field */
    const char *field;

    /*! Encode argument to setting value */
    sc_arg_encode_f encode;

    /*! Decode getting value to argument */
    sc_arg_decode_f decode;

} sc_value_ctrl_t;

/*!
 * \brief Direct symbol configuration map
 */
typedef struct sc_symbol_ctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! TRUE - per-port setting, FALSE - device-wide setting */
    bool perport;

    /*! Logic Table name */
    const char *table;

    /*! Logic Table field */
    const char *field;

    /*! String to enum value map */
    sc_symbol_map_t *maps;

} sc_symbol_ctrl_t;

/*!
 * \brief Symbol control access by key
 */
typedef struct sc_symbol_kctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! TRUE - per-port setting, FALSE - device-wide setting */
    bool perport;

    /*! Logic Table name */
    const char *table;

    /*! Logic Table key field */
    const char *kfield;

    /*! Logic Table key field value */
    const char *kval;

    /*! Logic Table field */
    const char *field;

    /*! String to enum value map */
    sc_symbol_map_t *maps;

} sc_symbol_kctrl_t;

/*!
 * \brief Per bit value access map
 *
 * This is for flag type argument. The \c maps defines LT field to flag mapping.
 * A flag bit set means to set the correspondent LT field to 1. Otherwise, set
 * it to 0.
 */
typedef struct sc_bit_ctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! TRUE - per-port setting, FALSE - device-wide setting */
    bool perport;

    /*! Logic Table name */
    const char *table;

    /*! Field to mask value map */
    sc_symbol_map_t *maps;

} sc_bit_ctrl_t;

/*!
 * \brief Special bool string LT map
 *
 * This structure defines LT \c field's bool string (\c ture_str and
 * \c false_str) map.
 */
typedef struct sc_bool_str_ctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! Logic Table field */
    const char *field;

    /*! Enum string for true */
    const char *true_str;

    /*! Enum string for false */
    const char *false_str;

} sc_bool_str_ctrl_t;

/*!
 * \brief Switch control type map
 *
 * This structure intended for mapping switch control enum type to SW value.
 * The type does not require accessing SDKLT LTs.
 */
typedef struct sc_value_map_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! Switch control value */
    int value;

} sc_value_map_t;

/*!
 * \brief Switch port control type map
 *
 * This structure intended for mapping switch control enum type to SW value.
 * The type does not require accessing SDKLT LTs.
 */
typedef struct scp_value_map_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! switch control value array with number of elements equals to BCM_PBMP_PORT_MAX */
    int value[BCM_PBMP_PORT_MAX];

} scp_value_map_t;

/*!
 * \brief Switch control infomation structure.
 */
typedef struct sc_info_s {

    /* Software value type switch control. */
    sc_value_map_t *sc_value_map;

    /* Port based software value type switch control. */
    scp_value_map_t *scp_value_map;

} sc_info_t;

static sc_info_t sc_info[BCM_MAX_NUM_UNITS];

/*! switch_control_set/get index mapping table */
static int sc_idx[bcmSwitch__Count];

/*! switch_control_port_set/get index mapping table */
static int scp_idx[bcmSwitch__Count];

/* CodeGen map table section begin
 * DO NOT CHANGE anything from here to "CodeGen map table end".
 */
/* CodeGen map table section end */

static int
sc_bcmlt_entry_lookup(int unit, bcmlt_entry_handle_t handle)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    rv = bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_INSERT,
                                      BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL));
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_value_ctrl_set(int unit, int port, sc_value_ctrl_t *ctrl, int arg)
{
    uint64_t val;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    dunit = bcmi_ltsw_dev_dunit(unit);

    if (ctrl->encode) {
        SHR_IF_ERR_EXIT
            (ctrl->encode(unit, port, arg, &val));
    } else {
        val = (unsigned int)arg;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(handle, ctrl->field, val));
    if (ctrl->perport) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
sc_value_ctrl_get(int unit, int port, sc_value_ctrl_t *ctrl, int *arg)
{
    uint64_t val;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    if (ctrl->perport) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, handle));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(handle, ctrl->field, &val));

    if (ctrl->decode) {
        SHR_IF_ERR_EXIT
            (ctrl->decode(unit, port, val, arg));
    } else {
        *arg = val & 0xffffffff;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
sc_symbol_ctrl_set(int unit, int port, sc_symbol_ctrl_t *ctrl, int arg)
{
    int i;
    const char *str = NULL;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    i = 0;
    while (ctrl->maps[i].name) {
        if (arg == ctrl->maps[i].val) {
            str = ctrl->maps[i].name;
            break;
        }
        i++;
    }
    /* Configuration enum is not support */
    SHR_NULL_CHECK(str, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(handle, ctrl->field, str));
    if (ctrl->perport) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
sc_symbol_ctrl_get(int unit, int port, sc_symbol_ctrl_t *ctrl, int *arg)
{
    int i;
    const char *str = NULL;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    if (ctrl->perport) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(handle, ctrl->field, &str));
    SHR_NULL_CHECK(str, SHR_E_INTERNAL);

    i = 0;
    while (ctrl->maps[i].name) {
        if (!sal_strcasecmp(ctrl->maps[i].name, str)) {
            *arg = ctrl->maps[i].val;
            break;
        }
        i++;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


/* Support
 * 1. Multiple entries for a same control type
 * 2. Both 1 bit mask and mask block (consecutive 1s)
 */
static int
sc_bit_ctrl_set(int unit, int port, bcm_switch_control_t type,
                sc_bit_ctrl_t *ctrls, int arg)
{
    int i;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrls, SHR_E_PARAM);

    i= 0;
    while (ctrls[i].table) {
        if (type == ctrls[i].type) {
            int j;
            uint64_t val;
            sc_bit_ctrl_t *ctrl = &ctrls[i];
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
            j = 0;
            while (ctrl->maps[j].name) {
                val = (arg & ctrl->maps[j].val) ? 1 : 0;
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(handle, ctrl->maps[j].name, val));
                j++;
            }
            if (ctrl->perport) {
                if (port == PORT_IGNORE) {
                    SHR_IF_ERR_MSG_EXIT
                        (SHR_E_PARAM,
                         (BSL_META_U(unit, "Abnormal port ID %d\n"), port));
                }
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(handle, PORT_IDs, port));
            }
            SHR_IF_ERR_EXIT
                (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_free(handle));
            handle = BCMLT_INVALID_HDL;
        }
        i++;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/* Support
 * 1. Multiple entries for a same control type
 * 2. Both 1 bit mask and mask block (consecutive 1s)
 */
static int
sc_bit_ctrl_get(int unit, int port, bcm_switch_control_t type,
                sc_bit_ctrl_t *ctrls, int *arg)
{
    int i;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrls, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;
    i= 0;
    while (ctrls[i].table) {
        if (type == ctrls[i].type) {
            int j;
            uint64_t val;
            sc_bit_ctrl_t *ctrl = &ctrls[i];
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
            if (ctrl->perport) {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(handle, PORT_IDs, port));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (sc_bcmlt_entry_lookup(unit, handle));
            j = 0;
            while (ctrl->maps[j].name) {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_get(handle, ctrl->maps[j].name, &val));
                if (val) {
                    *arg |= ctrl->maps[j].val;
                }
                j++;
            }
            SHR_IF_ERR_EXIT
                (bcmlt_entry_free(handle));
            handle = BCMLT_INVALID_HDL;
        }
        i++;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
sc_symbol_kctrl_set(int unit, int port, sc_symbol_kctrl_t *ctrl, int arg)
{
    int i;
    const char *str = NULL;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    i = 0;
    while (ctrl->maps[i].name) {
        if (arg == ctrl->maps[i].val) {
            str = ctrl->maps[i].name;
            break;
        }
        i++;
    }
    /* Configuration enum is not support */
    SHR_NULL_CHECK(str, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(handle, ctrl->kfield, ctrl->kval));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(handle, ctrl->field, str));
    if (ctrl->perport) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
sc_symbol_kctrl_get(int unit, int port, sc_symbol_kctrl_t *ctrl, int *arg)
{
    int i;
    const char *str = NULL;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(handle, ctrl->kfield, ctrl->kval));
    if (ctrl->perport) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(handle, ctrl->field, &str));
    SHR_NULL_CHECK(str, SHR_E_INTERNAL);

    i = 0;
    while (ctrl->maps[i].name) {
        if (!sal_strcasecmp(ctrl->maps[i].name, str)) {
            *arg = ctrl->maps[i].val;
            break;
        }
        i++;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
sc_port_pkt_ctrl_set(int unit, int port, sc_bool_str_ctrl_t *ctrl, int arg)
{
    uint64_t val;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, PORTs, &handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(handle, PORT_IDs, port));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(handle, PORT_PKT_CONTROL_IDs, &val));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(handle));
    handle = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, PORT_PKT_CONTROLs, &handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(handle, PORT_PKT_CONTROL_IDs, val));

    if (ctrl->true_str && ctrl->false_str) {
        if (arg == 1) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_symbol_add(handle, ctrl->field,
                                              ctrl->true_str));
        } else if (arg == 0) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_symbol_add(handle, ctrl->field,
                                              ctrl->false_str));
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (!(ctrl->true_str || ctrl->false_str)){
        val = (unsigned int)arg;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(handle, ctrl->field, val));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
sc_port_pkt_ctrl_get(int unit, int port, sc_bool_str_ctrl_t *ctrl, int *arg)
{
    uint64_t val;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, PORTs, &handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(handle, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(handle, PORT_PKT_CONTROL_IDs, &val));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(handle));
    handle = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, PORT_PKT_CONTROLs, &handle));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(handle, PORT_PKT_CONTROL_IDs, val));
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, handle));
    if (ctrl->true_str && ctrl->false_str) {
        const char *str = NULL;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_get(handle, ctrl->field, &str));
        if (!sal_strcasecmp(ctrl->true_str, str)) {
            *arg = 1;
        } else {
            *arg = 0;
        }
    } else if (!(ctrl->true_str || ctrl->false_str)){
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(handle, ctrl->field, &val));
        *arg = val & 0xffffffff;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
        handle = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/* CodeGen function section begin
 * Don't change or remove this comments.
 * Each interface is generated by code-gen. The content should be
 * maintained by developer;
 */
static int
sc_bcmSwitchAlternateStoreForward_set(int unit, int arg)
{
    uint64_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_UNAVAIL);
    COMPILER_REFERENCE(val);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchAlternateStoreForward_get(int unit, int *arg)
{
    uint64_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT(SHR_E_UNAVAIL);

    *arg = val & 0xffffffff;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEccSingleBitErrorEvent_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_ser_single_bit_error_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEccSingleBitErrorEvent_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_ser_single_bit_error_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicSampleRate_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    if (arg <= 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeSampleRate,
                                   value));

exit:
    SHR_FUNC_EXIT();

}

static int
sc_bcmSwitchEcmpDynamicSampleRate_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeSampleRate,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();

}

static int
sc_bcmSwitchEcmpDynamicEgressBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold_get(int unit, int *arg)
{

   uint64_t value;

   SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicRandomSeed_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeRandomSeed,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicRandomSeed_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeRandomSeed,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchDlbMonitorIngressRandomSeed_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeMonitorIngressRandomSeed,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchDlbMonitorIngressRandomSeed_get(int unit, int *arg)
{
    uint64_t value;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeMonitorIngressRandomSeed,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultipath_set(int unit, int arg)
{
    uint32_t val = arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_control_set(unit,
                                      bcmiL3ControlEcmpHashMultipath,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultipath_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_control_get(unit,
                                      bcmiL3ControlEcmpHashMultipath,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashControl_set(int unit, int arg)
{
    uint32_t val = arg;
    uint32_t trunk_hash = 0;

    SHR_FUNC_ENTER(unit);

    if (arg & BCM_HASH_CONTROL_TRUNK_NUC_DST) {
        trunk_hash |= BCM_HASH_CONTROL_TRUNK_NUC_DST;
    }
    if (arg & BCM_HASH_CONTROL_TRUNK_NUC_SRC) {
        trunk_hash |= BCM_HASH_CONTROL_TRUNK_NUC_SRC;
    }
    if (arg & BCM_HASH_CONTROL_TRUNK_NUC_MODPORT) {
        trunk_hash |= BCM_HASH_CONTROL_TRUNK_NUC_MODPORT;
    }
    if (arg & BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE) {
        trunk_hash |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_trunk_sc_hash_control_set(unit, trunk_hash));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlEcmpHashControl,
                                  val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_control_set(unit, bcmiHashControlEcmp, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashControl_get(int unit, int *arg)
{
    uint32_t val = 0;
    uint32_t trunk_hash = 0;
    int lb_hash = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_CONT
        (bcmi_ltsw_trunk_sc_hash_control_get(unit, &trunk_hash));

    SHR_IF_ERR_CONT
            (bcmi_ltsw_l3_control_get(unit,
                                      bcmiL3ControlEcmpHashControl,
                                      &val));
    if (trunk_hash) {
        val |= trunk_hash;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_control_get(unit, bcmiHashControlEcmp, &lb_hash));

    if (lb_hash) {
        val |= lb_hash;
    }

    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmp_set(int unit, int arg)
{
    uint32_t val = arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_control_set(unit,
                                      bcmiL3ControlEcmpHashUseFlowSelEcmp,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmp_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_control_get(unit,
                                      bcmiL3ControlEcmpHashUseFlowSelEcmp,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEntropy_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_control_set(unit, bcmiHashControlEcmp, arg));


exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEntropy_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_control_get(unit, bcmiHashControlEcmp, arg));


exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPHashBitCountSelect_set(int unit, int arg)
{
    uint32_t val = arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_control_set(unit,
                                      bcmiL3ControlEcmpHashBitCountSelect,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPHashBitCountSelect_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_control_get(unit,
                                      bcmiL3ControlEcmpHashBitCountSelect,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpOverlay_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlFlowSelOverlayEcmp,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpOverlay_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlFlowSelOverlayEcmp,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelL2Ecmp_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlFlowSelL2Ecmp,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelL2Ecmp_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlFlowSelL2Ecmp,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelL2EcmpOverlay_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlFlowSelOverlayL2Ecmp,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelL2EcmpOverlay_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlFlowSelOverlayL2Ecmp,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpResilientHash_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlFlowSelRhEcmp,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpResilientHash_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlFlowSelRhEcmp,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpOverlayResilientHash_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlFlowSelOverlayRhEcmp,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpOverlayResilientHash_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlFlowSelOverlayRhEcmp,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpWeightedOverlay_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlFlowSelOverlayWeightedEcmp,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpWeightedOverlay_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlFlowSelOverlayWeightedEcmp,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchHashUseFlowSelMplsEcmpOverlay_set(int unit, int arg)
{
    uint32_t val = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
             bcmiTunnelControlHashUseFlowSelMplsEcmpOverlay, val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelMplsEcmpOverlay_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
             bcmiTunnelControlHashUseFlowSelMplsEcmpOverlay, &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcnNonIpResponsive_set(int unit, int arg)
{
    int enable;

    SHR_FUNC_ENTER(unit);

    enable = arg ? 1 : 0;

    SHR_ERR_EXIT
        (bcmi_ltsw_ecn_ctrl_set(unit, ECN_CONTROLs, NON_IP_RESPONSIVEs, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcnNonIpResponsive_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_ecn_ctrl_get(unit, ECN_CONTROLs, NON_IP_RESPONSIVEs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcnNonIpIntCongestionNotification_set(int unit, int arg)
{
    int data = 0;

    SHR_FUNC_ENTER(unit);

    bcmi_ltsw_ecn_ctrl_get(unit, ECN_CONTROLs, NON_IP_RESPONSIVEs, &data);

    if (data == 1) {
        SHR_ERR_EXIT
            (bcmi_ltsw_ecn_ctrl_set(unit, ECN_CONTROLs,
                                    RESPONSIVE_NON_IP_ECN_CNGs, arg));
    } else {
        SHR_ERR_EXIT
            (bcmi_ltsw_ecn_ctrl_set(unit, ECN_CONTROLs,
                                    NON_RESPONSIVE_NON_IP_ECN_CNGs, arg));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcnNonIpIntCongestionNotification_get(int unit, int *arg)
{
    int data = 0;

    SHR_FUNC_ENTER(unit);

    bcmi_ltsw_ecn_ctrl_get(unit, ECN_CONTROLs, NON_IP_RESPONSIVEs, &data);

    if (data == 1) {
        SHR_ERR_EXIT
            (bcmi_ltsw_ecn_ctrl_get(unit, ECN_CONTROLs,
                                    RESPONSIVE_NON_IP_ECN_CNGs, arg));
    } else {
        SHR_ERR_EXIT
            (bcmi_ltsw_ecn_ctrl_get(unit, ECN_CONTROLs,
                                    NON_RESPONSIVE_NON_IP_ECN_CNGs, arg));
    }

exit:
    SHR_FUNC_EXIT();

}

/*
 * Not supported in the xgs devices.
 */
static int
sc_bcmSwitchMplsNonIpPayloadDefaultEcn_set(int unit, int arg)
{
    uint64_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL);
    COMPILER_REFERENCE(val);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Not supported in the xgs devices.
 */
static int
sc_bcmSwitchMplsNonIpPayloadDefaultEcn_get(int unit, int *arg)
{
    uint64_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT(BCM_E_UNAVAIL);

    *arg = val & 0xffffffff;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcnIpPayloadResponsive_set(int unit, int arg)
{
    int enable;

    SHR_FUNC_ENTER(unit);

    enable = arg ? 1 : 0;

    SHR_ERR_EXIT
        (bcmi_ltsw_ecn_ctrl_set(unit, ECN_CONTROLs,
                                USE_PAYLOAD_IP_PROTO_FOR_RESPONSIVEs, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcnIpPayloadResponsive_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_ecn_ctrl_get(unit, ECN_CONTROLs,
                                USE_PAYLOAD_IP_PROTO_FOR_RESPONSIVEs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2StaticMoveToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, STATIC_MAC_MOVE_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2StaticMoveToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, STATIC_MAC_MOVE_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownMcastToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, UNKNOWN_MC_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownMcastToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, UNKNOWN_MC_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownUcastToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, UNKNOWN_UC_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownUcastToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, UNKNOWN_UC_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSourceRouteToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, SRC_MC_MAC_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSourceRouteToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, SRC_MC_MAC_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2NonStaticMoveToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, NON_STATIC_MAC_MOVE_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2NonStaticMoveToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, NON_STATIC_MAC_MOVE_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSourceMacZeroDrop_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, DROP_ALL_ZERO_SRC_MACs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSourceMacZeroDrop_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, DROP_ALL_ZERO_SRC_MACs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchDhcpLearn_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, SKIP_LEARNING_DHCPs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchDhcpLearn_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, SKIP_LEARNING_DHCPs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2HitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg) {
        SHR_ERR_EXIT
            (bcmi_ltsw_l2_addr_hit_clear(unit, 1, 1));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2HitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2SrcHitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg) {
        SHR_ERR_EXIT
            (bcmi_ltsw_l2_addr_hit_clear(unit, 1, 0));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2SrcHitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2DstHitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg) {
        SHR_ERR_EXIT
            (bcmi_ltsw_l2_addr_hit_clear(unit, 0, 1));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2DstHitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2SourceDiscardMoveToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, SKIP_SRC_DROPs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2SourceDiscardMoveToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, SKIP_SRC_DROPs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2DstHitEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, SKIP_HIT_DST_MACs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2DstHitEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, SKIP_HIT_DST_MACs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHWL2Freeze_set(int unit, int arg)
{

    SHR_FUNC_ENTER(unit);

    if (arg) {
        SHR_ERR_EXIT
            (bcmi_ltsw_l2_hw_freeze(unit));
    } else {
        SHR_ERR_EXIT
            (bcmi_ltsw_l2_hw_thaw(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHWL2Freeze_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_hw_is_frozen(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchParityErrorToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, PARITY_ERR_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchParityErrorToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, PARITY_ERR_TO_CPUs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSharedVlanL2McastEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_set(unit, L2_CONTROLs, L2_MC_FID_LOOKUPs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSharedVlanL2McastEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_l2_ctrl_get(unit, L2_CONTROLs, L2_MC_FID_LOOKUPs, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMcastUnknownErrToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_control_set(unit,
                                      bcmiL3ControlMcastUnknownErrToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMcastUnknownErrToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_control_get(unit,
                                      bcmiL3ControlMcastUnknownErrToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchNonIpL3ErrToCpu_set(int unit, int arg)
{
    uint32_t val = arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_control_set(unit,
                                      bcmiL3ControlNonIpL3ErrToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchNonIpL3ErrToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlNonIpL3ErrToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownL3DestToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlUnknownL3DestToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownL3DestToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                   bcmiL3ControlUnknownL3DestToCpu,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcPortMissToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlIpmcPortMissToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcPortMissToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlIpmcPortMissToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcErrorToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlIpmcErrorToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcErrorToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlIpmcErrorToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV6L3ErrToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlV6L3ErrToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV6L3ErrToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlV6L3ErrToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV6L3DstMissToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlV6L3DstMissToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV6L3DstMissToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlV6L3DstMissToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV6L3LocalLinkDrop_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlV6L3LocalLinkDrop,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV6L3LocalLinkDrop_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlV6L3LocalLinkDrop,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV4L3ErrToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlV4L3ErrToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV4L3ErrToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlV4L3ErrToCpu,
                                  &val));


    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV4L3DstMissToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlV4L3DstMissToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV4L3DstMissToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlV4L3DstMissToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMartianAddrToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlMartianAddrToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMartianAddrToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlV4L3DstMissToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3UcTtlErrToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlL3UcTtlErrToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3UcTtlErrToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlL3UcTtlErrToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3SlowpathToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlL3SlowpathToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3SlowpathToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlL3SlowpathToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcTtlErrToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlIpmcTtlErrToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcTtlErrToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlIpmcTtlErrToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3MtuFailToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlL3MtuFailToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3MtuFailToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlL3MtuFailToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIgmpReservedMcastEnable_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlIgmpReservedMcastEnable,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIgmpReservedMcastEnable_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlIgmpReservedMcastEnable,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMldReservedMcastEnable_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlMldReservedMcastEnable,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMldReservedMcastEnable_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlMldReservedMcastEnable,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMldDirectAttachedOnly_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlMldDirectAttachedOnly,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMldDirectAttachedOnly_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlMldDirectAttachedOnly,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownIpmcAsMcast_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlUnknownIpmcAsMcast,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownIpmcAsMcast_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlUnknownIpmcAsMcast,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTunnelUnknownIpmcDrop_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlTunnelUnknownIpmcDrop,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTunnelUnknownIpmcDrop_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlTunnelUnknownIpmcDrop,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcTtl1ToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlIpmcTtl1ToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcTtl1ToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlIpmcTtl1ToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3UcastTtl1ToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlL3UcastTtl1ToCpu,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3UcastTtl1ToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlL3UcastTtl1ToCpu,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3RouteHitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_route_hit_clear(unit));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3RouteHitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3HostHitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_host_hit_clear(unit));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3HostHitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}
static int
sc_bcmSwitchIpmcSameVlanPruning_set(int unit, int arg)
{
    bool enable = FALSE;

    SHR_FUNC_ENTER(unit);
    if (arg < 0) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /* Revert the L2/L3 combinded mode. */
    enable = (arg > 0) ? FALSE : TRUE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_l2_l3_combined_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcSameVlanPruning_get(int unit, int *arg)
{
    bool enable = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_l2_l3_combined_get(unit, &enable));

    /* Revert the L2/L3 combinded mode. */
    *arg = (enable == TRUE) ? 0 : 1;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3SrcHitEnable_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlL3SrcHitEnable,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3SrcHitEnable_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlL3SrcHitEnable,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFlexIP6ExtHdr_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlFlexIP6ExtHdr,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFlexIP6ExtHdr_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlFlexIP6ExtHdr,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFlexIP6ExtHdrEgress_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlFlexIP6ExtHdrEgress,
                                  val));


exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFlexIP6ExtHdrEgress_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlFlexIP6ExtHdrEgress,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV6L3SrcDstLocalLinkDropCancel_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlV6L3SrcDstLocalLinkDropCancel,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchV6L3SrcDstLocalLinkDropCancel_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlV6L3SrcDstLocalLinkDropCancel,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIgmpUcastEnable_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlIgmpUcastEnable,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIgmpUcastEnable_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlIgmpUcastEnable,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMldUcastEnable_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlMldUcastEnable,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMldUcastEnable_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlMldUcastEnable,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIp4McastL2DestCheck_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlIp4McastL2DestCheck,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIp4McastL2DestCheck_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlIp4McastL2DestCheck,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIp6McastL2DestCheck_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlIp6McastL2DestCheck,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIp6McastL2DestCheck_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlIp6McastL2DestCheck,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcSameVlanL3Route_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlIpmcSameVlanL3Route,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcSameVlanL3Route_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlIpmcSameVlanL3Route,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchWESPProtocolEnable_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlWESPProtocolEnable,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchWESPProtocolEnable_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlWESPProtocolEnable,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchWESPProtocol_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlWESPProtocol,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchWESPProtocol_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlWESPProtocol,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField0PreProcessEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_set(unit,
                                    bcmiHashBinField0PreProcessEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField0PreProcessEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_get(unit,
                                    bcmiHashBinField0PreProcessEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField1PreProcessEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_set(unit,
                                    bcmiHashBinField1PreProcessEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField1PreProcessEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_get(unit,
                                    bcmiHashBinField1PreProcessEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchHashField0Config_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_set(unit,
                                         bcmiAlgorithmField0Inst0,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField0Config_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_get(unit,
                                         bcmiAlgorithmField0Inst0,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField0Config1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_set(unit,
                                         bcmiAlgorithmField0Inst1,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField0Config1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_get(unit,
                                         bcmiAlgorithmField0Inst1,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField1Config_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_set(unit,
                                         bcmiAlgorithmField1Inst0,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField1Config_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_get(unit,
                                         bcmiAlgorithmField1Inst0,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField1Config1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_set(unit,
                                         bcmiAlgorithmField1Inst1,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField1Config1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_get(unit,
                                         bcmiAlgorithmField1Inst1,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashFieldConfig_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_set(unit,
                                         bcmiAlgorithmMacroFlow,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashFieldConfig_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_algorithm_get(unit,
                                         bcmiAlgorithmMacroFlow,
                                         arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashSelectControl_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_pkt_hdr_sel_set(unit,
                                           bcmiPktHdrSelControl,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashSelectControl_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_pkt_hdr_sel_get(unit,
                                           bcmiPktHdrSelControl,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchHashIP4Field0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash0FieldSelectTypeIP,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4Field0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash0FieldSelectTypeIP,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4Field1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash1FieldSelectTypeIP,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4Field1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash1FieldSelectTypeIP,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4TcpUdpField0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash0FieldSelectTypeTcpUdp,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4TcpUdpField0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash0FieldSelectTypeTcpUdp,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4TcpUdpField1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash1FieldSelectTypeTcpUdp,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4TcpUdpField1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash1FieldSelectTypeTcpUdp,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4TcpUdpPortsEqualField0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash0FieldSelectTypeL4Port,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4TcpUdpPortsEqualField0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash0FieldSelectTypeL4Port,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4TcpUdpPortsEqualField1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash1FieldSelectTypeL4Port,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP4TcpUdpPortsEqualField1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash1FieldSelectTypeL4Port,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6Field0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash0FieldSelectTypeIP6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6Field0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash0FieldSelectTypeIP6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6Field1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash1FieldSelectTypeIP6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6Field1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash1FieldSelectTypeIP6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6TcpUdpField0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash0FieldSelectTypeTcpUdpIp6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6TcpUdpField0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash0FieldSelectTypeTcpUdpIp6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6TcpUdpField1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash1FieldSelectTypeTcpUdpIp6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchHashIP6TcpUdpField1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash1FieldSelectTypeTcpUdpIp6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6TcpUdpPortsEqualField0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash0FieldSelectTypeL4PortIp6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchHashIP6TcpUdpPortsEqualField0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash0FieldSelectTypeL4PortIp6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6TcpUdpPortsEqualField1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash1FieldSelectTypeL4PortIp6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6TcpUdpPortsEqualField1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash1FieldSelectTypeL4PortIp6,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashL2Field0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash0FieldSelectTypeL2,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashL2Field0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash0FieldSelectTypeL2,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashL2Field1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash1FieldSelectTypeL2,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashL2Field1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash1FieldSelectTypeL2,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}



static int
sc_bcmSwitchHashField0OverlayCntagRpidEnable_set(int unit, int arg)
{
    uint64_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL);
    COMPILER_REFERENCE(val);

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchHashField0OverlayCntagRpidEnable_get(int unit, int *arg)
{
    uint64_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT(BCM_E_UNAVAIL);

    *arg = val & 0xffffffff;

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchHashField1OverlayCntagRpidEnable_set(int unit, int arg)
{
    uint64_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL);
    COMPILER_REFERENCE(val);

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchHashField1OverlayCntagRpidEnable_get(int unit, int *arg)
{
    uint64_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT(BCM_E_UNAVAIL);

    *arg = val & 0xffffffff;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField0Ip6FlowLabel_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_set(unit,
                                    bcmiHashBinField0Ip6FlowLabel,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField0Ip6FlowLabel_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_get(unit,
                                    bcmiHashBinField0Ip6FlowLabel,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField1Ip6FlowLabel_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_set(unit,
                                    bcmiHashBinField1Ip6FlowLabel,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashField1Ip6FlowLabel_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_get(unit,
                                    bcmiHashBinField1Ip6FlowLabel,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpUnderlayHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpUnderlayHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashL3MPLSField0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash0FieldSelectTypeL3Mpls,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashL3MPLSField0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash0FieldSelectTypeL3Mpls,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashL3MPLSField1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash1FieldSelectTypeL3Mpls,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashL3MPLSField1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash1FieldSelectTypeL3Mpls,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMPLSTunnelField0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash0FieldSelectTypeMplsTransit,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMPLSTunnelField0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash0FieldSelectTypeMplsTransit,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMPLSTunnelField1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_set(unit,
                                             bcmiHash1FieldSelectTypeMplsTransit,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMPLSTunnelField1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_fields_select_get(unit,
                                             bcmiHash1FieldSelectTypeMplsTransit,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPortIndependentLowerRange1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_set(unit,
                                               0,
                                               1,
                                               arg));

exit:
    SHR_FUNC_EXIT();

}

static int
sc_bcmSwitchMplsPortIndependentLowerRange1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_get(unit,
                                               0,
                                               arg,
                                               NULL));

exit:
    SHR_FUNC_EXIT();

}

static int
sc_bcmSwitchMplsPortIndependentUpperRange1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_set(unit,
                                               0,
                                               0,
                                               arg));

exit:
    SHR_FUNC_EXIT();

}

static int
sc_bcmSwitchMplsPortIndependentUpperRange1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_get(unit,
                                               0,
                                               NULL,
                                               arg));

exit:
    SHR_FUNC_EXIT();

}

static int
sc_bcmSwitchMplsPortIndependentLowerRange2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_set(unit,
                                               1,
                                               1,
                                               arg));

exit:
    SHR_FUNC_EXIT();

}

static int
sc_bcmSwitchMplsPortIndependentLowerRange2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_get(unit,
                                               1,
                                               arg,
                                               NULL));

exit:
    SHR_FUNC_EXIT();

}

static int
sc_bcmSwitchMplsPortIndependentUpperRange2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_set(unit,
                                               1,
                                               0,
                                               arg));

exit:
    SHR_FUNC_EXIT();

}

static int
sc_bcmSwitchMplsPortIndependentUpperRange2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_get(unit,
                                               1,
                                               NULL,
                                               arg));

exit:
    SHR_FUNC_EXIT();

}


static int
sc_bcmSwitchMacroFlowLoadBalanceHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeEntropy,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchMacroFlowLoadBalanceHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeEntropy,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeTrunk,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeTrunk,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6AddrCollapseSelect0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_ip6_collapse_set(unit,
                                            bcmiIp6CollapseField0,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6AddrCollapseSelect0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_ip6_collapse_get(unit,
                                            bcmiIp6CollapseField0,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6AddrCollapseSelect1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_ip6_collapse_set(unit,
                                            bcmiIp6CollapseField1,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6AddrCollapseSelect1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_ip6_collapse_get(unit,
                                            bcmiIp6CollapseField1,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSymmetricHashControl_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_symmetric_control_set(unit,
                                                 arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSymmetricHashControl_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_symmetric_control_get(unit,
                                                 arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUdfHashEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_set(unit,
                                    bcmiHashBinUdfHashEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUdfHashEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_get(unit,
                                    bcmiHashBinUdfHashEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMPLSUseLabelStack_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_pkt_hdr_sel_set(unit,
                                           bcmiPktHdrSelMplsUseLabelStack,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMPLSUseLabelStack_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_pkt_hdr_sel_get(unit,
                                           bcmiPktHdrSelMplsUseLabelStack,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6NextHeaderUseExtensionHeader0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_pkt_hdr_sel_set(unit,
                                           bcmiPktHdrSelIp6ExtensionHeader,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashIP6NextHeaderUseExtensionHeader0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_pkt_hdr_sel_get(unit,
                                           bcmiPktHdrSelIp6ExtensionHeader,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchHashGtpTeidEnable0_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_set(unit,
                                    bcmiHashBinGtpTeidEnable0,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashGtpTeidEnable0_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_get(unit,
                                    bcmiHashBinGtpTeidEnable0,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashGtpTeidEnable1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_set(unit,
                                    bcmiHashBinGtpTeidEnable1,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashGtpTeidEnable1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_bins_get(unit,
                                    bcmiHashBinGtpTeidEnable1,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeTrunk,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchMacroFlowTrunkHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeTrunk,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeTrunk,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeTrunk,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeTrunk,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeTrunk,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeEntropy,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}



static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeEntropy,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeEntropy,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeEntropy,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeEntropy,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeEntropy,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMPLSExcludeReservedLabel_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_pkt_hdr_sel_set(unit,
                                           bcmiPktHdrSelplsExcludeReservedLabel,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMPLSExcludeReservedLabel_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_pkt_hdr_sel_get(unit,
                                           bcmiPktHdrSelplsExcludeReservedLabel,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeDlbEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeDlbEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeDlbEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeDlbEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeDlbEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeDlbEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeDlbEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeDlbEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpWeighted_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlFlowSelWeightedEcmp,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpWeighted_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlFlowSelWeightedEcmp,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeWECMP,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeWECMP,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeWECMP,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeWECMP,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeWECMP,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeWECMP,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeWECMP,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeWECMP,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPRandomSeed_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlECMPRandomSeed,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPRandomSeed_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlECMPRandomSeed,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashVersatileInitValue0_set(int unit, int arg)
{
    int val = arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_set(unit,
                                                 bcmiHashVersatileControlInitValue0,
                                                 val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashVersatileInitValue0_get(int unit, int *arg)
{
    int val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_get(unit,
                                                 bcmiHashVersatileControlInitValue0,
                                                 &val));

    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashVersatileInitValue1_set(int unit, int arg)
{
    int val = arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_set(unit,
                                                 bcmiHashVersatileControlInitValue1,
                                                 val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashVersatileInitValue1_get(int unit, int *arg)
{
    int val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_get(unit,
                                                 bcmiHashVersatileControlInitValue1,
                                                 &val));

    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHash0VersatileInitValue0_set(int unit, int arg)
{
    int val = arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_set(unit,
                                                 bcmiHashVersatileControlBlockAInitValue0,
                                                 val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHash0VersatileInitValue0_get(int unit, int *arg)
{
    int val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_get(unit,
                                                 bcmiHashVersatileControlBlockAInitValue0,
                                                 &val));

    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHash0VersatileInitValue1_set(int unit, int arg)
{
    int val = arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_set(unit,
                                                 bcmiHashVersatileControlBlockAInitValue1,
                                                 val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHash0VersatileInitValue1_get(int unit, int *arg)
{
    int val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_get(unit,
                                                 bcmiHashVersatileControlBlockAInitValue1,
                                                 &val));

    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHash1VersatileInitValue0_set(int unit, int arg)
{
    int val = arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_set(unit,
                                                 bcmiHashVersatileControlBlockBInitValue0,
                                                 val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHash1VersatileInitValue0_get(int unit, int *arg)
{
    int val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_get(unit,
                                                 bcmiHashVersatileControlBlockBInitValue0,
                                                 &val));

    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHash1VersatileInitValue1_set(int unit, int arg)
{
    int val = arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_set(unit,
                                                 bcmiHashVersatileControlBlockBInitValue1,
                                                 val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHash1VersatileInitValue1_get(int unit, int *arg)
{
    int val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_versatile_control_get(unit,
                                                 bcmiHashVersatileControlBlockBInitValue1,
                                                 &val));

    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashSeed0_set(int unit, int arg)
{
    int val = arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_control_seed_set(unit,
                                            bcmiHashControlSeed0,
                                            val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashSeed0_get(int unit, int *arg)
{
    int val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_control_seed_get(unit,
                                            bcmiHashControlSeed0,
                                            &val));

    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashSeed1_set(int unit, int arg)
{
    int val = arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_control_seed_set(unit,
                                            bcmiHashControlSeed1,
                                            val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashSeed1_get(int unit, int *arg)
{
    int val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_control_seed_get(unit,
                                            bcmiHashControlSeed1,
                                            &val));

    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPLevel1RandomSeed_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlECMPLevel1RandomSeed,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPLevel1RandomSeed_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlECMPLevel1RandomSeed,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPLevel2RandomSeed_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_set(unit,
                                  bcmiL3ControlECMPLevel2RandomSeed,
                                  val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPLevel2RandomSeed_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_control_get(unit,
                                  bcmiL3ControlECMPLevel2RandomSeed,
                                  &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpResilientHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeRhEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchMacroFlowEcmpResilientHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeRhEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpResilientHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeRhEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpResilientHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeRhEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpResilientHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeRhEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpResilientHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeRhEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpResilientHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeRhEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpResilientHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeRhEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayResilientHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayRhEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayResilientHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayRhEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayResilientHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayRhEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayResilientHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayRhEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayResilientHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayRhEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayResilientHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayRhEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayResilientHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayRhEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayResilientHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayRhEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpMplsHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeMplsEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpMplsHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeMplsEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpMplsHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeMplsEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpMplsHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeMplsEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpMplsHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeMplsEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpMplsHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeMplsEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpMplsHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeMplsEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpMplsHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeMplsEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayMplsHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeMplsOverlayEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayMplsHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeMplsOverlayEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayMplsHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeMplsOverlayEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayMplsHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeMplsOverlayEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayMplsHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeMplsOverlayEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayMplsHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeMplsOverlayEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayMplsHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeMplsOverlayEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpOverlayMplsHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeMplsOverlayEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypePlfs,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypePlfs,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypePlfs,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypePlfs,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypePlfs,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypePlfs,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypePlfs,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypePlfs,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeL2Ecmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeL2Ecmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeL2Ecmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeL2Ecmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeL2Ecmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeL2Ecmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeL2Ecmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeL2Ecmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpOverlayHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeL2OverlayEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpOverlayHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeL2OverlayEcmp,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpOverlayHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeL2OverlayEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpOverlayHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeL2OverlayEcmp,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpOverlayHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeL2OverlayEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpOverlayHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeL2OverlayEcmp,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpOverlayHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeL2OverlayEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowL2EcmpOverlayHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeL2OverlayEcmp,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeUcTrunk,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeUcTrunk,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeUcTrunk,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeUcTrunk,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeUcTrunk,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeUcTrunk,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeUcTrunk,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeUcTrunk,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeNonUcTrunk,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeNonUcTrunk,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeNonUcTrunk,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeNonUcTrunk,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeNonUcTrunk,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeNonUcTrunk,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeNonUcTrunk,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeNonUcTrunk,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedOverlayHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayWECMP,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedOverlayHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayWECMP,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedOverlayHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayWECMP,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedOverlayHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayWECMP,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedOverlayHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayWECMP,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedOverlayHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayWECMP,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedOverlayHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayWECMP,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpWeightedOverlayHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayWECMP,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpHashOverlayConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayECMP,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpHashOverlayConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayECMP,
                                               bcmiLbHosFlowScConcat,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayECMP,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayECMP,
                                               bcmiLbHosFlowScMin,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayECMP,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayECMP,
                                               bcmiLbHosFlowScMax,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_set(unit,
                                               bcmiLbHosFlowTypeOverlayECMP,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_flow_control_get(unit,
                                               bcmiLbHosFlowTypeOverlayECMP,
                                               bcmiLbHosFlowScStride,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowECMPHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_macro_flow_seed_set(unit, bcmiMacroFlowEcmp, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowECMPHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowECMPOverlayHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_macro_flow_seed_set(unit, bcmiMacroFlowEcmpOverlay, arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchMacroFlowECMPOverlayHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_macro_flow_seed_set(unit, bcmiMacroFlowLoadBalance, arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchMacroFlowLoadBalanceHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowECMPUnderlayHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_macro_flow_seed_set(unit, bcmiMacroFlowECMPUnderlay, arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchMacroFlowECMPUnderlayHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_macro_flow_seed_set(unit, bcmiMacroFlowTrunk, arg));

exit:
    SHR_FUNC_EXIT();
}


static int
sc_bcmSwitchMacroFlowTrunkHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSampleVersion_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sample_version_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSampleVersion_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sample_version_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSampleIngressRandomSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sampler_seed_set(unit,
                                           bcmiSamplerSeedIngress,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSampleIngressRandomSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sampler_seed_get(unit,
                                           bcmiSamplerSeedIngress,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSampleEgressRandomSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sampler_seed_set(unit,
                                           bcmiSamplerSeedEgress,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSampleEgressRandomSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sampler_seed_get(unit,
                                           bcmiSamplerSeedEgress,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorPktChecksEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_pkt_checks_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorPktChecksEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_pkt_checks_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorUnicastCosq_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_uc_cos_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorUnicastCosq_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_uc_cos_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorMulticastCosq_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_mc_cos_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorMulticastCosq_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_mc_cos_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSampleFlexRandomSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sampler_seed_set(unit,
                                           bcmiSamplerSeedFlex,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSampleFlexRandomSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sampler_seed_get(unit,
                                           bcmiSamplerSeedFlex,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorPsampFormat2Epoch_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_psamp_epoch_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorPsampFormat2Epoch_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_psamp_epoch_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorCpuCosq_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_cpu_cos_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorCpuCosq_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_cpu_cos_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorPsampIpfixVersion_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_psamp_ipfix_version_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorPsampIpfixVersion_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_psamp_ipfix_version_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

int
sc_bcmSwitchPFCDeadlockRecoveryAction_set(int unit, int arg)
{
    return bcmi_ltsw_pfc_deadlock_recovery_action_set(unit, arg);
}

static int
sc_bcmSwitchPFCDeadlockRecoveryAction_get(int unit, int *arg)
{
    return bcmi_ltsw_pfc_deadlock_recovery_action_get(unit, arg);
}

static int
sc_bcmSwitchBstSnapshotEnable_set(int unit, int arg)
{
    return bcmi_ltsw_cosq_bst_snapshot_enable_set(unit, arg);
}

static int
sc_bcmSwitchBstSnapshotEnable_get(int unit, int *arg)
{
    return bcmi_ltsw_cosq_bst_snapshot_enable_get(unit, arg);
}

static int
sc_bcmSwitchBstEnable_set(int unit, int arg)
{
    return bcmi_ltsw_cosq_bst_enable_set(unit, arg);
}

static int
sc_bcmSwitchBstEnable_get(int unit, int *arg)
{
    return bcmi_ltsw_cosq_bst_enable_get(unit, arg);
}

static int
sc_bcmSwitchBstTrackingMode_set(int unit, int arg)
{
    return bcmi_ltsw_cosq_bst_tracking_mode_set(unit, arg);
}

static int
sc_bcmSwitchBstTrackingMode_get(int unit, int *arg)
{
    return bcmi_ltsw_cosq_bst_tracking_mode_get(unit, arg);
}

static int
sc_bcmSwitchRateLimitLinear_set(int unit, int arg)
{
    /*
     * Linear mode or ITU mode can only be configured during boot time and
     * can't be updated during runtime.
     */
    return SHR_E_UNAVAIL;
}

static int
sc_bcmSwitchRateLimitLinear_get(int unit, int *arg)
{
    return bcmi_ltsw_cosq_shaper_config_get(unit, NULL, arg, NULL);
}

static int
sc_bcmSwitchTunnelIp4IdShared_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlTunnelIp4IdShared,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTunnelIp4IdShared_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlTunnelIp4IdShared,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTunnelErrToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlTunnelErrToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTunnelErrToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlTunnelErrToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsLabelMissToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsLabelMissToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsLabelMissToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsLabelMissToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsTtlErrToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsTtlErrToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsTtlErrToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsTtlErrToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsInvalidL3PayloadToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsInvalidL3PayloadToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsInvalidL3PayloadToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsInvalidL3PayloadToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsInvalidActionToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsInvalidActionToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsInvalidActionToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsInvalidActionToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsGalAlertLabelToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsGalAlertLabelToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsGalAlertLabelToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsGalAlertLabelToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsRalAlertLabelToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsRalAlertLabelToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsRalAlertLabelToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsRalAlertLabelToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsIllegalReservedLabelToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsIllegalReservedLabelToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsIllegalReservedLabelToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsIllegalReservedLabelToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsUnknownAchTypeToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsUnknownAchTypeToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsUnknownAchTypeToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsUnknownAchTypeToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsUnknownAchVersionToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsUnknownAchVersionToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsUnknownAchVersionToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsUnknownAchVersionToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPWControlWordToCpu_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlMplsPWControlWordToCpu,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPWControlWordToCpu_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlMplsPWControlWordToCpu,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelMplsEcmp_set(int unit, int arg)
{
    uint32_t val = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_set(unit,
                                      bcmiTunnelControlHashUseFlowSelMplsEcmp,
                                      val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelMplsEcmp_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_tunnel_control_get(unit,
                                      bcmiTunnelControlHashUseFlowSelMplsEcmp,
                                      &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkUc_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_sc_lb_control_unicast_set(unit, val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkUc_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_sc_lb_control_unicast_get(unit, &val));

    *arg = val & 0xffffffff;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkNonUnicast_set(int unit, int arg)
{
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_sc_lb_control_nonunicast_set(unit, val));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkNonUnicast_get(int unit, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_sc_lb_control_nonunicast_get(unit, &val));

    *arg = val & 0xffffffff;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownVlanToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_set(unit,
                                    bcmiVlanControlUnknownVlanToCpu,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchUnknownVlanToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_get(unit,
                                    bcmiVlanControlUnknownVlanToCpu,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSharedVlanMismatchToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_set(unit,
                                    bcmiVlanControlSharedVlanMismatchToCpu,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSharedVlanMismatchToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_get(unit,
                                    bcmiVlanControlSharedVlanMismatchToCpu,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMcastFloodDefault_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_flood_default_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMcastFloodDefault_get(int unit, int *arg)
{
    bcm_vlan_mcast_flood_t mode;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_flood_default_get(unit, &mode));
    *arg = mode;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchBpduInvalidVlanDrop_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_set(unit,
                                    bcmiVlanControlBpduInvalidVlanDrop,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchBpduInvalidVlanDrop_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_get(unit,
                                    bcmiVlanControlBpduInvalidVlanDrop,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorInvalidVlanDrop_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mirror_invalid_vlan_drop_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorInvalidVlanDrop_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mirror_invalid_vlan_drop_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSharedVlanEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_set(unit,
                                    bcmiVlanControlSharedVlanEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSharedVlanEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_get(unit,
                                    bcmiVlanControlSharedVlanEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSnapNonZeroOui_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_set(unit,
                                    bcmiVlanControlSnapNonZeroOui,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSnapNonZeroOui_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_get(unit,
                                    bcmiVlanControlSnapNonZeroOui,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEgressStgCheckIngressEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_set(unit,
                                    bcmiVlanControlEgressStgCheckIngressEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEgressStgCheckIngressEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_control_get(unit,
                                    bcmiVlanControlEgressStgCheckIngressEnable,
                                    arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthL2,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthL2,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthMpls_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthMpls,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthMpls_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthMpls,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL3Tunnel_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthL3Tunnel,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL3Tunnel_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthL3Tunnel,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthExactMatch_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthExactMatch,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthExactMatch_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthExactMatch,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIntVectorMatchMissAction_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlVectorMatchMissAction,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntVectorMatchMissAction_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlVectorMatchMissAction,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntEgressTimeDelta_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlEgressTimeDelta,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntEgressTimeDelta_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlEgressTimeDelta,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntSwitchId_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlSwitchId,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntSwitchId_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlSwitchId,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIfaVersion_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIfaVersion,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIfaVersion_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIfaVersion,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIfaProtocol_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIfaProtocol,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIfaProtocol_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIfaProtocol,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamGreProtocolParse_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIoamGreProtocolParse,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamGreProtocolParse_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIoamGreProtocolParse,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamGreProtocol_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIoamGreProtocol,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamGreProtocol_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIoamGreProtocol,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamOptionIncrementalParse_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIoamOptionIncrementalParse,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamOptionIncrementalParse_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIoamOptionIncrementalParse,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamOptionIncremental_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIoamOptionIncremental,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamOptionIncremental_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIoamOptionIncremental,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamType_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIoamType,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIoamType_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIoamType,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntVersion_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIntDpVersion,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntVersion_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIntDpVersion,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntL4DestPort1_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIntDpL4DestPort1,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntL4DestPort1_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIntDpL4DestPort1,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntL4DestPort2_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIntDpL4DestPort2,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntL4DestPort2_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIntDpL4DestPort2,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntProbeMarker1_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIntDpProbeMarker1,
                                   val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIntDpUseProbeMarker1,
                                   1));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntProbeMarker1_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIntDpUseProbeMarker1,
                                   &val));

    if (val) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_control_get(unit,
                                       bcmiIntControlIntDpProbeMarker1,
                                       &val));
        *arg = val;
    }

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntProbeMarker2_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIntDpProbeMarker2,
                                   val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIntDpUseProbeMarker2,
                                   1));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntProbeMarker2_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIntDpUseProbeMarker2,
                                   &val));

    if (val) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_int_control_get(unit,
                                       bcmiIntControlIntDpProbeMarker2,
                                       &val));
        *arg = val;
    }

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntTurnAroundCpuEnable_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIntTurnAroundCpuEnable,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
        return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntTurnAroundCpuEnable_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIntTurnAroundCpuEnable,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntHopLimitCpuEnable_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t val = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlIntHopLimitCpuEnable,
                                   val));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntHopLimitCpuEnable_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlIntHopLimitCpuEnable,
                                   &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchVxlanUdpSrcPortSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanUdpSrcPortSelect,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanUdpSrcPortSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanUdpSrcPortSelect,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanUdpDestPortSet_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanUdpDestPortSet,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanUdpDestPortSet_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanUdpDestPortSet,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanTunnelMissToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanTunnelMissToCpu,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanTunnelMissToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanTunnelMissToCpu,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanVnIdMissToCpu_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanVnIdMissToCpu,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanVnIdMissToCpu_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanVnIdMissToCpu,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderFlags_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanHeaderFlags,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderFlags_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanHeaderFlags,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderReserved1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanHeaderReserved1,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderReserved1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanHeaderReserved1,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderReserved2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanHeaderReserved2,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderReserved2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanHeaderReserved2,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderMatchFlags_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanHeaderMatchFlags,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderMatchFlags_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanHeaderMatchFlags,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderFlagsMask_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanHeaderFlagsMask,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderFlagsMask_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanHeaderFlagsMask,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderFlagsMismatchDrop_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_set(unit,
                                           bcmSwitchVxlanHeaderFlagsMismatchDrop,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanHeaderFlagsMismatchDrop_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_switch_control_get(unit,
                                           bcmSwitchVxlanHeaderFlagsMismatchDrop,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncLiveWireToD_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_set(unit, bcmSwitchTimesyncLiveWireToD,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncLiveWireToD_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_get(unit, bcmSwitchTimesyncLiveWireToD,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncEgressVersion_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_set(unit,
                                           bcmSwitchTimesyncEgressVersion,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncEgressVersion_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_get(unit,
                                           bcmSwitchTimesyncEgressVersion,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncIngressVersion_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_set(unit,
                                           bcmSwitchTimesyncIngressVersion,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncIngressVersion_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_get(unit,
                                           bcmSwitchTimesyncIngressVersion,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchNtpL4DestPort_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_set(unit,
                                           bcmSwitchNtpL4DestPort,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchNtpL4DestPort_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_get(unit,
                                           bcmSwitchNtpL4DestPort,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFlowCountFlexCtrIngAction_set(int unit, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {CTR_ING_EFLEX_ACTIONs,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_ltsw_flexctr_counter_info_t counter_info = {0};

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;

    if (arg != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_counter_id_info_get(unit, arg,
                                                   &counter_info));
        fields[0].u.val = counter_info.action_index;
    } else {
        fields[0].u.val = arg;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_FLOWCOUNT_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFlowCountFlexCtrIngAction_get(int unit, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {CTR_ING_EFLEX_ACTIONs,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_flexctr_counter_info_t ci = {0};

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_FLOWCOUNT_CONTROLs,
                           &entry, NULL, NULL));

    if (fields[0].u.val == 0) {
        *arg = fields[0].u.val;
    } else {
        ci.action_index = fields[0].u.val;
        ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
        ci.stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, (uint32_t *)arg));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchArpReplyToCpu_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     ARP_REPLY_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchArpReplyToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        ARP_REPLY_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchArpReplyDrop_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     ARP_REPLY_DROPs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchArpReplyDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        ARP_REPLY_DROPs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchArpRequestToCpu_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     ARP_REQUEST_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchArpRequestToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        ARP_REQUEST_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchArpRequestDrop_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     ARP_REQUEST_DROPs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchArpRequestDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        ARP_REQUEST_DROPs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchNdPktToCpu_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     ND_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchNdPktToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        ND_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchNdPktDrop_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     ND_DROPs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchNdPktDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        ND_DROPs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMcastFloodBlocking_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     PFM_RULE_APPLYs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMcastFloodBlocking_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        PFM_RULE_APPLYs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpPktToCpu_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_REPORT_LEAVE_TO_CPUs,
                                                     field_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_QUERY_TO_CPUs,
                                                     field_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_UNKNOWN_MSG_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpPktToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_REPORT_LEAVE_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpPktDrop_set(int unit, int port, int arg)
{
    int  field_value = arg ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_REPORT_LEAVE_FWD_ACTIONs,
                                                     field_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_QUERY_FWD_ACTIONs,
                                                     field_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_UNKNOWN_MSG_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpPktDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_REPORT_LEAVE_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchDhcpPktToCpu_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     DHCP_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchDhcpPktToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        DHCP_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchDhcpPktDrop_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     DHCP_DROPs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}


static int
scp_bcmSwitchDhcpPktDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        DHCP_DROPs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldPktToCpu_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_REPORT_DONE_TO_CPUs,
                                                     field_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_QUERY_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldPktToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MLD_REPORT_DONE_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldPktDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_REPORT_DONE_FWD_ACTIONs,
                                                     field_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_QUERY_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldPktDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MLD_REPORT_DONE_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV4ResvdMcPktToCpu_set(int unit, int port, int arg)
{
    int  field_value = arg ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV4_RESVD_MC_PKT_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV4ResvdMcPktToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV4_RESVD_MC_PKT_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV4ResvdMcPktDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV4_RESVD_MC_PKT_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV4ResvdMcPktDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV4_RESVD_MC_PKT_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV4ResvdMcPktFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV4_RESVD_MC_PKT_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV4ResvdMcPktFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV4_RESVD_MC_PKT_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV6ResvdMcPktToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV6_RESVD_MC_PKT_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV6ResvdMcPktToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV6_RESVD_MC_PKT_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV6ResvdMcPktDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV6_RESVD_MC_PKT_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV6ResvdMcPktDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV6_RESVD_MC_PKT_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV6ResvdMcPktFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV6_RESVD_MC_PKT_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchV6ResvdMcPktFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV6_RESVD_MC_PKT_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpReportLeaveToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_REPORT_LEAVE_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpReportLeaveToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_REPORT_LEAVE_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpReportLeaveDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_REPORT_LEAVE_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpReportLeaveDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_REPORT_LEAVE_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpReportLeaveFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_REPORT_LEAVE_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpReportLeaveFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_REPORT_LEAVE_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpQueryToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_QUERY_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpQueryToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_QUERY_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpQueryDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_QUERY_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpQueryDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_QUERY_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpQueryFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_QUERY_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpQueryFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_QUERY_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpUnknownToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_UNKNOWN_MSG_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpUnknownToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_UNKNOWN_MSG_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpUnknownDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_UNKNOWN_MSG_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpUnknownDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_UNKNOWN_MSG_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpUnknownFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IGMP_UNKNOWN_MSG_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIgmpUnknownFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IGMP_UNKNOWN_MSG_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldReportDoneToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_REPORT_DONE_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldReportDoneToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MLD_REPORT_DONE_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldReportDoneDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_REPORT_DONE_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}


static int
scp_bcmSwitchMldReportDoneDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MLD_REPORT_DONE_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldReportDoneFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_REPORT_DONE_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldReportDoneFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MLD_REPORT_DONE_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldQueryToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_QUERY_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldQueryToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MLD_QUERY_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldQueryDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_QUERY_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldQueryDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MLD_QUERY_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldQueryFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MLD_QUERY_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMldQueryFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MLD_QUERY_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV4RouterDiscoveryToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV4_MC_ROUTER_ADV_PKT_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV4RouterDiscoveryToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV4_MC_ROUTER_ADV_PKT_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV4RouterDiscoveryDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV4RouterDiscoveryDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV4RouterDiscoveryFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONs,
                                                     field_value));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV4RouterDiscoveryFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV6RouterDiscoveryToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV6_MC_ROUTER_ADV_PKT_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV6RouterDiscoveryToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV6_MC_ROUTER_ADV_PKT_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV6RouterDiscoveryDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV6RouterDiscoveryDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV6RouterDiscoveryFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONs,
                                                     field_value));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchIpmcV6RouterDiscoveryFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchPortEgressBlockL2_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = EGR_BLOCK_L2s;
    fields[0].u.val = arg ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, PORT_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchPortEgressBlockL2_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = EGR_BLOCK_L2s;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, PORT_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, PORT_CONTROLs, EGR_BLOCK_L2s, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    *arg = fields[0].u.val ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchPortEgressBlockL3_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = EGR_BLOCK_L3s;
    fields[0].u.val = arg ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, PORT_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchPortEgressBlockL3_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = EGR_BLOCK_L3s;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, PORT_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, PORT_CONTROLs, EGR_BLOCK_L3s, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    *arg = fields[0].u.val ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMmrpPktToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MMRP_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMmrpPktToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MMRP_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMmrpPktDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MMRP_FWD_ACTIONs,
                                                     field_value));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMmrpPktDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MMRP_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMmrpPktFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     MMRP_FWD_ACTIONs,
                                                     field_value));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMmrpPktFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        MMRP_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSrpPktToCpu_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? 1 : 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     SRP_TO_CPUs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSrpPktToCpu_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        SRP_TO_CPUs,
                                                        arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSrpPktDrop_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     SRP_FWD_ACTIONs,
                                                     field_value));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSrpPktDrop_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        SRP_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_DROP_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSrpPktFlood_set(int unit, int port, int arg)
{
    int  field_value = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_update(unit, port,
                                                     SRP_FWD_ACTIONs,
                                                     field_value));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSrpPktFlood_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(unit, port,
                                                        SRP_FWD_ACTIONs,
                                                        arg));
    *arg = (*arg == BCM_SWITCH_FLOOD_VALUE) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSRPEthertype_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = SRP_ETHERTYPEs;
    fields[0].u.val = arg;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, L2_PARSER_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSRPEthertype_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = SRP_ETHERTYPEs;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, L2_PARSER_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, L2_PARSER_CONTROLs, SRP_ETHERTYPEs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    *arg = fields[0].u.val;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMMRPEthertype_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = MMRP_ETHERTYPEs;
    fields[0].u.val = arg;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, L2_PARSER_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMMRPEthertype_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = MMRP_ETHERTYPEs;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, L2_PARSER_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, L2_PARSER_CONTROLs, MMRP_ETHERTYPEs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    *arg = fields[0].u.val;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSRPDestMacOui_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    uint32_t f_lo = (uint32_t)arg << 24;
    uint32_t f_hi = ((uint32_t)arg >> 8) & 0xffff;
    uint64_t mac = 0;

    SHR_FUNC_ENTER(unit);

    mac = f_hi;
    mac = (mac << 32) | f_lo;

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = SRP_DST_MACs;
    fields[0].u.val = mac;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, L2_PARSER_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSRPDestMacOui_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;
    uint64_t mac = 0;
    uint32_t f_lo = 0;
    uint32_t f_hi = 0;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = SRP_DST_MACs;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, L2_PARSER_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, L2_PARSER_CONTROLs, SRP_DST_MACs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    mac = fields[0].u.val;
    f_lo = (uint32_t)mac;
    f_lo = f_lo >> 24;
    f_hi = mac >> 32;
    f_hi = f_hi << 8;
    *arg = f_hi | f_lo;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMMRPDestMacOui_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    uint32_t f_lo = (uint32_t)arg << 24;
    uint32_t f_hi = ((uint32_t)arg >> 8) & 0xffff;
    uint64_t mac = 0;

    SHR_FUNC_ENTER(unit);

    mac = f_hi;
    mac = (mac << 32) | f_lo;

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = MMRP_DST_MACs;
    fields[0].u.val = mac;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, L2_PARSER_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMMRPDestMacOui_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;
    uint64_t mac = 0;
    uint32_t f_lo = 0;
    uint32_t f_hi = 0;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = MMRP_DST_MACs;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, L2_PARSER_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, L2_PARSER_CONTROLs, MMRP_DST_MACs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    mac = fields[0].u.val;
    f_lo = (uint32_t)mac;
    f_lo = f_lo >> 24;
    f_hi = mac >> 32;
    f_hi = f_hi << 8;
    *arg = f_hi | f_lo;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSRPDestMacNonOui_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    uint64_t mac_old = 0, mac_new = 0;
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = SRP_DST_MACs;
    if (SHR_FAILURE(bcmi_lt_entry_get(unit, L2_PARSER_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, L2_PARSER_CONTROLs, SRP_DST_MACs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    mac_old = fields[0].u.val;
    mac_new = (mac_old >> 24) << 24;
    mac_new |= ((arg << 8) >> 8);
    fields[0].flags = BCMI_LT_FIELD_F_SET;
    fields[0].u.val = mac_new;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, L2_PARSER_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchSRPDestMacNonOui_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;
    uint64_t mac = 0;
    uint32_t f_lo = 0;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = SRP_DST_MACs;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, L2_PARSER_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, L2_PARSER_CONTROLs, SRP_DST_MACs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    mac = fields[0].u.val;
    f_lo = (uint32_t)mac;
    *arg = (f_lo << 8) >> 8;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMMRPDestMacNonOui_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    uint64_t mac_old = 0, mac_new = 0;
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = MMRP_DST_MACs;
    if (SHR_FAILURE(bcmi_lt_entry_get(unit, L2_PARSER_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, L2_PARSER_CONTROLs, MMRP_DST_MACs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    mac_old = fields[0].u.val;
    mac_new = (mac_old >> 24) << 24;
    mac_new |= ((arg << 8) >> 8);
    fields[0].flags = BCMI_LT_FIELD_F_SET;
    fields[0].u.val = mac_new;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, L2_PARSER_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchMMRPDestMacNonOui_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;
    uint64_t mac = 0;
    uint32_t f_lo = 0;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = MMRP_DST_MACs;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, L2_PARSER_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, L2_PARSER_CONTROLs, MMRP_DST_MACs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    mac = fields[0].u.val;
    f_lo = (uint32_t)mac;
    *arg = (f_lo << 8) >> 8;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchCpuToCpuEnable_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = DO_NOT_COPY_FROM_CPU_TO_CPUs;
    fields[0].u.val = arg ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, PORT_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchCpuToCpuEnable_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = DO_NOT_COPY_FROM_CPU_TO_CPUs;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, PORT_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, PORT_CONTROLs, DO_NOT_COPY_FROM_CPU_TO_CPUs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    *arg = fields[0].u.val ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEgressBlockUcast_set(int unit, int port, int arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = EGR_BLOCK_UCASTs;
    fields[0].u.val = arg ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, PORT_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEgressBlockUcast_get(int unit, int port, int *arg)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {NULL,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].fld_name = EGR_BLOCK_UCASTs;

    if (SHR_FAILURE(bcmi_lt_entry_get(unit, PORT_CONTROLs, &entry, NULL, NULL))) {
        sal_memset(&fld_def, 0, sizeof(fld_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, PORT_CONTROLs, EGR_BLOCK_UCASTs, &fld_def));
        fields[0].u.val = fld_def.def;
    }
    *arg = fields[0].u.val ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchLinkDownInfoSkip_set(int unit, int port, int arg)
{
    return bcmi_ltsw_link_port_info_skip_set(unit, port, arg);
}

static int
scp_bcmSwitchLinkDownInfoSkip_get(int unit, int port, int *arg)
{
    return bcmi_ltsw_link_port_info_skip_get(unit, port, arg);
}

static int
scp_bcmSwitchMeterAdjust_set(int unit, int port, int arg)
{
    return bcmi_ltsw_rate_meter_adjust_set(unit, port, arg);
}

static int
scp_bcmSwitchMeterAdjust_get(int unit, int port, int *arg)
{
    return bcmi_ltsw_rate_meter_adjust_get(unit, port, arg);
}

static int
scp_bcmSwitchShaperAdjust_set(int unit, int port, int arg)
{
    return bcmi_ltsw_rate_shaper_adjust_set(unit, port, arg);
}

static int
scp_bcmSwitchShaperAdjust_get(int unit, int port, int *arg)
{
    return bcmi_ltsw_rate_shaper_adjust_get(unit, port, arg);
}

static int
scp_bcmSwitchTrunkHashSet0UnicastOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeUcTrunk,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkHashSet0UnicastOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeUcTrunk,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkHashSet0NonUnicastOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeNonUcTrunk,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkHashSet0NonUnicastOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeNonUcTrunk,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkFailoverHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypePlfs,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkFailoverHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypePlfs,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchECMPHashSet0Offset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchECMPHashSet0Offset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEntropyHashSet0Offset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeEntropy,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEntropyHashSet0Offset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeEntropy,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpDynamicHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeDlbEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpDynamicHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeDlbEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchECMPMplsHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeMplsEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchECMPMplsHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeMplsEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpResilientHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeRhEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpResilientHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeRhEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpWeightedHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeWEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpWeightedHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeWEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}


static int
scp_bcmSwitchFieldMultipathHashSelect_set(int unit, int port, int arg)
{
    uint64_t val = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_UNAVAIL);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(val);

exit:
    SHR_FUNC_EXIT();
}


static int
scp_bcmSwitchFieldMultipathHashSelect_get(int unit, int port, int *arg)
{
    uint64_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT(SHR_E_UNAVAIL);
    COMPILER_REFERENCE(port);

    *arg = val & 0xffffffff;

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchAlternateStoreForward_set(int unit, int port, int arg)
{
    return bcmi_ltsw_port_asf_mode_set(unit, port, arg);
}

static int
scp_bcmSwitchAlternateStoreForward_get(int unit, int port, int *arg)
{
    return bcmi_ltsw_port_asf_mode_get(unit, port, arg);
}

static int
scp_bcmSwitchEcmpOverlayResilientHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeOverlayRhEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpOverlayResilientHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeOverlayRhEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpOverlayMplsHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeMplsOverlayEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpOverlayMplsHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeMplsOverlayEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpOverlayHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeOverlayEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpOverlayHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeOverlayEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpOverlayWeightedHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeOverlayWEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpOverlayWeightedHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeOverlayWEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchL2EcmpHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeL2Ecmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchL2EcmpHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeL2Ecmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchL2EcmpOverlayHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_set(unit,
                                               bcmiLbHosPortTypeL2OverlayEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchL2EcmpOverlayHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_os_port_control_get(unit,
                                               bcmiLbHosPortTypeL2OverlayEcmp,
                                               port,
                                               arg));

exit:
    SHR_FUNC_EXIT();
}


static int
scp_bcmSwitchTimesyncEgressTimestampingMode_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_port_set
            (unit, port, bcmSwitchTimesyncEgressTimestampingMode, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTimesyncEgressTimestampingMode_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_port_get
            (unit, port, bcmSwitchTimesyncEgressTimestampingMode, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchNtpResidenceTimeUpdate_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_port_set
            (unit, port, bcmSwitchNtpResidenceTimeUpdate, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchNtpResidenceTimeUpdate_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_port_get
            (unit, port, bcmSwitchNtpResidenceTimeUpdate, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchLayeredQoSResolution_set(int unit, int port, int arg)
{
    uint32_t val = arg ? 0 : 1; /* layered resolution  : serial resolution */

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_qos_control_set(unit,
                                   bcmiQosControlLayeredQoSResolution,
                                   val));

exit:
    SHR_FUNC_EXIT();

}

static int
scp_bcmSwitchLayeredQoSResolution_get(int unit, int port, int *arg)
{
    uint32_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_qos_control_get(unit,
                                   bcmiQosControlLayeredQoSResolution,
                                   &val));

    *arg = val ? 0 : 1;

exit:
    SHR_FUNC_EXIT();
}

/* CodeGen function section end
 * Don't change or remove this comments.
 */

static sc_value_ctrl_t sc_vctrls[] = {
    SC_VALUE_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL, NULL}
};

static sc_bit_ctrl_t sc_mctrls[] ={
    SC_BIT_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL}
};

static sc_symbol_ctrl_t sc_sctrls[] = {
    SC_SYMBOL_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL}
};

static sc_value_ctrl_t scp_vctrls[] = {
    SCP_VALUE_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL, NULL}
};

static sc_bit_ctrl_t scp_mctrls[] ={
    SCP_BIT_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL}
};

static sc_symbol_ctrl_t scp_sctrls[] = {
    SCP_SYMBOL_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL}
};

static sc_symbol_kctrl_t scp_key_sctrls[] = {
    SCP_KEY_SYMBOL_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL}
};

static sc_bool_str_ctrl_t scp_port_pkt_ctrls[] = {
    SCP_PORT_PKT_CONTROLS
    {bcmSwitch__Count, NULL, NULL, NULL}
};

/* Software value type switch control. */
static sc_value_map_t sc_value_maps[] = {
    {bcmSwitchUseGport, 0},
    {bcmSwitchL2McIdxRetType, 0},
    {bcmSwitchBypassMode, 0},
    {bcmSwitchL2PortBlocking, 0},
    {bcmSwitchL2FreezeMode, 1},
    {bcmSwitchL2ApplCallbackSuppress, 0},
    {bcmSwitch__Count, 0}
};

/* Port based software value type switch control. */
static scp_value_map_t scp_value_maps[] = {
    {bcmSwitch__Count, { 0 }}
};

/*
 * SC type function list.
 */
static sc_func_t sc_funcs[] = {
    SC_FUNC_LIST
    {bcmSwitch__Count, NULL, NULL}
};

/*
 * SCP type function list.
 */
static scp_func_t scp_funcs[] = {
    SCP_FUNC_LIST
    {bcmSwitch__Count, NULL, NULL}
};

static int
sc_info_init(int unit)
{
    int rv = SHR_E_NONE;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    sc_info_t *sc = &sc_info[unit];
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    ha_array_size = COUNTOF(sc_value_maps);
    ha_instance_size = sizeof(bcmint_switch_control_value_map_t);
    ha_req_size = COUNTOF(sc_value_maps) * sizeof(sc_value_map_t);
    ha_alloc_size = ha_req_size;
    sc->sc_value_map = bcmi_ltsw_ha_mem_alloc(unit,
                                              BCMI_HA_COMP_ID_SWITCH,
                                              HA_SC_SOFTWARE_SUB_COMP_ID,
                                              "bcmSwitchSW",
                                              &ha_alloc_size);
    SHR_NULL_CHECK(sc->sc_value_map, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memcpy(sc->sc_value_map, sc_value_maps, ha_alloc_size);
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit, BCMI_HA_COMP_ID_SWITCH,
                                           HA_SC_SOFTWARE_SUB_COMP_ID,
                                           0, ha_instance_size,
                                           ha_array_size,
                                           BCMINT_SWITCH_CONTROL_VALUE_MAP_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    ha_req_size = COUNTOF(scp_value_maps) * sizeof(scp_value_map_t);
    ha_alloc_size = ha_req_size;
    sc->scp_value_map = bcmi_ltsw_ha_mem_alloc(unit,
                                               BCMI_HA_COMP_ID_SWITCH,
                                               HA_SCP_SOFTWARE_SUB_COMP_ID,
                                               "bcmSwitchPortSW",
                                               &ha_alloc_size);
    SHR_NULL_CHECK(sc->scp_value_map, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memcpy(sc->scp_value_map, scp_value_maps, ha_alloc_size);
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit, BCMI_HA_COMP_ID_SWITCH,
                                           HA_SC_SOFTWARE_SUB_COMP_ID,
                                           0, ha_instance_size,
                                           ha_array_size,
                                           BCMINT_SWITCH_CONTROL_PORT_VALUE_MAP_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!warm && sc->sc_value_map) {
            (void)bcmi_ltsw_ha_mem_free(unit, sc->sc_value_map);
        }
        if (!warm && sc->scp_value_map) {
            (void)bcmi_ltsw_ha_mem_free(unit, sc->scp_value_map);
        }
    }
    SHR_FUNC_EXIT();
}

static void
scp_init(void) {
    int num, i = 1;

    while (scp_funcs[i].set) {
        scp_idx[scp_funcs[i].type] = i;
        i++;
    }
    num = COUNTOF(scp_vctrls);
    for (i = 0; i < num; i++) {
        if (scp_vctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_vctrls[i].type] = scValueCtrl + i;
    }
    num = COUNTOF(scp_mctrls);
    for (i = 0; i < num; i++) {
        if(scp_mctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_mctrls[i].type] = scBitCtrl + i;
    }
    num = COUNTOF(scp_sctrls);
    for (i = 0; i < num; i++) {
        if(scp_sctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_sctrls[i].type] = scSymbolCtrl + i;
    }
    num = COUNTOF(scp_key_sctrls);
    for (i = 0; i < num; i++) {
        if(scp_key_sctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_key_sctrls[i].type] = scKeySymbolCtrl + i;
    }
    num = COUNTOF(scp_port_pkt_ctrls);
    for (i = 0; i < num; i++) {
        if(scp_port_pkt_ctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_port_pkt_ctrls[i].type] = scPortPktCtrl + i;
    }
}

static int
sc_init(int unit) {
    static bool inited = false;
    int num, i = 1;

    SHR_FUNC_ENTER(unit);

    if (inited) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (sc_info_init(unit));

    scp_init();

    while (sc_funcs[i].set) {
        sc_idx[sc_funcs[i].type] = i;
        i++;
    }
    num = COUNTOF(sc_vctrls);
    for (i = 0; i < num; i++) {
        if(sc_vctrls[i].type == bcmSwitch__Count) {
            break;
        }
        sc_idx[sc_vctrls[i].type] = scValueCtrl + i;
    }
    num = COUNTOF(sc_mctrls);
    for (i = 0; i < num; i++) {
        if(sc_mctrls[i].type == bcmSwitch__Count) {
            break;
        }
        sc_idx[sc_mctrls[i].type] = scBitCtrl + i;
    }
    num = COUNTOF(sc_sctrls);
    for (i = 0; i < num; i++) {
        if(sc_sctrls[i].type == bcmSwitch__Count) {
            break;
        }
        sc_idx[sc_sctrls[i].type] = scSymbolCtrl + i;
    }
    num = COUNTOF(sc_value_maps);
    for (i = 0; i < num; i++) {
        if(sc_value_maps[i].type == bcmSwitch__Count) {
            break;
        }
        sc_idx[sc_value_maps[i].type] = scSoftwareValueCtrl + i;
    }
    inited = TRUE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xgs_scp_set(int unit, int port, bcm_switch_control_t type, int arg)
{
    sc_info_t *sc = &sc_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(sc_init(unit));
    if (type < bcmSwitch__Count) {
        int table_type = scp_idx[type] & (~scIdMask);
        int idx = scp_idx[type] & scIdMask;

        if (table_type == scSoftwareValueCtrl) {
            if (idx >= COUNTOF(scp_value_maps)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            sc->scp_value_map[idx].value[port] = arg;
        } else if (table_type == scValueCtrl) {
            if (idx >= sizeof(scp_vctrls) / sizeof(scp_vctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_value_ctrl_set(unit, port, &scp_vctrls[idx], arg));
        } else if (table_type == scBitCtrl) {
            /* Support multiple fields setting. Ignore idx. */
            SHR_ERR_EXIT
                (sc_bit_ctrl_set(unit, port, type, scp_mctrls, arg));
        } else if (table_type == scSymbolCtrl) {
            if (idx >= sizeof(scp_sctrls) / sizeof(scp_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_ctrl_set(unit, port, &scp_sctrls[idx], arg));
        } else if (table_type == scKeySymbolCtrl) {
            if (idx >= sizeof(scp_key_sctrls) / sizeof(scp_key_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_kctrl_set(unit, port, &scp_key_sctrls[idx], arg));
        } else if (table_type == scPortPktCtrl) {
            if (idx >=
                sizeof(scp_port_pkt_ctrls) / sizeof(scp_port_pkt_ctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_port_pkt_ctrl_set(unit, port, &scp_port_pkt_ctrls[idx],
                                      arg));
        } else {
            if (idx >= sizeof(scp_funcs) / sizeof(scp_funcs[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (scp_funcs[idx].set) {
                SHR_IF_ERR_EXIT
                    (scp_funcs[idx].set(unit, port, arg));
            } else {
                SHR_ERR_EXIT(BCM_E_UNAVAIL);
            }
        }
    } else {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xgs_scp_get(int unit, int port, bcm_switch_control_t type, int *arg)
{
    sc_info_t *sc = &sc_info[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(sc_init(unit));
    if (type < bcmSwitch__Count) {
        int table_type = scp_idx[type] & (~scIdMask);
        int idx = scp_idx[type] & scIdMask;
        if (table_type == scSoftwareValueCtrl) {
            if (idx >= COUNTOF(scp_value_maps)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            *arg = sc->scp_value_map[idx].value[port];
        } else if (table_type == scValueCtrl) {
            if (idx >= sizeof(scp_vctrls) / sizeof(scp_vctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_value_ctrl_get(unit, port, &scp_vctrls[idx], arg));
        } else if (table_type == scBitCtrl) {
            /* Support multiple fields setting. Ignore idx. */
            SHR_ERR_EXIT
                (sc_bit_ctrl_get(unit, port, type, scp_mctrls, arg));
        } else if (table_type == scSymbolCtrl) {
            if (idx >= sizeof(scp_sctrls) / sizeof(scp_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_ctrl_get(unit, port, &scp_sctrls[idx], arg));
        } else if (table_type == scKeySymbolCtrl) {
            if (idx >= sizeof(scp_key_sctrls) / sizeof(scp_key_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_kctrl_get(unit, port, &scp_key_sctrls[idx], arg));
        } else if (table_type == scPortPktCtrl) {
            if (idx >=
                sizeof(scp_port_pkt_ctrls) / sizeof(scp_port_pkt_ctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_port_pkt_ctrl_get(unit, port, &scp_port_pkt_ctrls[idx],
                                      arg));
        } else {
            if (idx >= sizeof(scp_funcs) / sizeof(scp_funcs[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (scp_funcs[idx].get) {
                SHR_ERR_EXIT
                    (scp_funcs[idx].get(unit, port, arg));
            } else {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xgs_sc_set(int unit, bcm_switch_control_t type, int arg)
{
    int port = PORT_IGNORE;
    sc_info_t *sc = &sc_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(sc_init(unit));
    if (type < bcmSwitch__Count) {
        int table_type = sc_idx[type] & (~scIdMask);
        int idx = sc_idx[type] & scIdMask;
        if (table_type == scSoftwareValueCtrl) {
            if (idx >= COUNTOF(sc_value_maps)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            sc->sc_value_map[idx].value = arg;
        } else if (table_type == scValueCtrl) {
            if (idx >= sizeof(sc_vctrls) / sizeof(sc_vctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_value_ctrl_set(unit, port, &sc_vctrls[idx], arg));
        } else if (table_type == scBitCtrl) {
            /* Support multiple fields setting. Ignore idx. */
            SHR_ERR_EXIT
                (sc_bit_ctrl_set(unit, port, type, sc_mctrls, arg));
        } else if (table_type == scSymbolCtrl) {
            if (idx >= sizeof(sc_sctrls) / sizeof(sc_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_ctrl_set(unit, port, &sc_sctrls[idx], arg));
        } else {
            if (idx >= sizeof(sc_funcs) / sizeof(sc_funcs[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (sc_funcs[idx].set) {
                SHR_IF_ERR_EXIT
                    (sc_funcs[idx].set(unit, arg));
            } else {/* idx == 0 */
                bcm_pbmp_t pbmp;
                bool found = false;

                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT,
                                           &pbmp));
                BCM_PBMP_ITER(pbmp, port) {
                    SHR_IF_ERR_EXIT
                        (bcmint_xgs_scp_set(unit, port, type, arg));
                    found = true;
                }

                if (!found) {
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xgs_sc_get(int unit, bcm_switch_control_t type, int *arg)
{
    int port = PORT_IGNORE;
    sc_info_t *sc = &sc_info[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(sc_init(unit));
    if (type < bcmSwitch__Count) {
        int table_type = sc_idx[type] & (~scIdMask);
        int idx = sc_idx[type] & scIdMask;
        if (table_type == scSoftwareValueCtrl) {
            if (idx >= COUNTOF(sc_value_maps)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            *arg = sc->sc_value_map[idx].value;
        } else if (table_type == scValueCtrl) {
            if (idx >= sizeof(sc_vctrls) / sizeof(sc_vctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_value_ctrl_get(unit, port, &sc_vctrls[idx], arg));
        } else if (table_type == scBitCtrl) {
            /* Support multiple fields setting. Ignore idx. */
            SHR_ERR_EXIT
                (sc_bit_ctrl_get(unit, port, type, sc_mctrls, arg));
        } else if (table_type == scSymbolCtrl) {
            if (idx >= sizeof(sc_sctrls) / sizeof(sc_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_ctrl_get(unit, port, &sc_sctrls[idx], arg));
        } else {
            if (idx >= sizeof(sc_funcs) / sizeof(sc_funcs[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (sc_funcs[idx].get) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (sc_funcs[idx].get(unit, arg));
            } else {
                bcm_pbmp_t pbmp;
                bool found = false;

                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT,
                                           &pbmp));
                BCM_PBMP_ITER(pbmp, port) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_xgs_scp_get(unit, port, type, arg));
                    found = true;
                    break;
                }

                if (!found) {
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xgs_switch_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (sc_info_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xgs_switch_detach(int unit)
{
    return SHR_E_NONE;
}

