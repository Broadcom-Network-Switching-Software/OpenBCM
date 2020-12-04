/*! \file vlan.c
 *
 * LTSW VLAN command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(BCM_LTSW_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/ltsw/cmdlist.h>
#include <shared/bsl.h>
#include <bcm/vlan.h>
#include <bcm_int/ltsw/port.h>

/******************************************************************************
 * Private functions
 */

#define VLAN_IF_ERROR_RETURN(op)                        \
  do {                                                  \
        int __rv__;                                     \
        if ((__rv__ = (op)) < 0) {                      \
            cli_out("Error: %s\n", bcm_errmsg(__rv__));  \
            return CMD_FAIL;                            \
        }                                               \
  } while (0)

/*
 * Structure used internally to get vlan action arguments.
 * This structure corresponds to bcm_vlan_action_t but
 * has the action name strings obtained from user input.
 */
typedef struct ivlan_action_set_s {
    int   new_outer_vlan;
    int   new_inner_vlan;
    int   priority;
    char *dt_outer;
    char *dt_outer_prio;
    char *dt_inner;
    char *dt_inner_prio;
    char *ot_outer;
    char *ot_outer_prio;
    char *ot_inner;
    char *it_outer;
    char *it_inner;
    char *it_inner_prio;
    char *ut_outer;
    char *ut_inner;
    int  policer;
    int  forwarding_domain;
    char *outer_tag;
    char *inner_tag;
} ivlan_action_set_t;

static struct vlan_action_names {
    char              *name;
    bcm_vlan_action_t  action;
} vlan_action_names[] = {
    {  "None",     bcmVlanActionNone    },
    {  "Add",      bcmVlanActionAdd     },
    {  "Replace",  bcmVlanActionReplace },
    {  "Delete",   bcmVlanActionDelete  }
};

static struct vlan_key_type_names {
    char                     *name;
    bcm_vlan_translate_key_t  key;
} vlan_key_type_names[] = {
    { "",              bcmVlanTranslateKeyInvalid       },
    { "Double",        bcmVlanTranslateKeyDouble        },
    { "Outer",         bcmVlanTranslateKeyOuter         },
    { "Inner",         bcmVlanTranslateKeyInner         },
    { "OuterTag",      bcmVlanTranslateKeyOuterTag      },
    { "InnerTag",      bcmVlanTranslateKeyInnerTag      },
    { "OuterPri",      bcmVlanTranslateKeyOuterPri      },
    { "PortDouble",    bcmVlanTranslateKeyPortDouble    },
    { "PortOuter",     bcmVlanTranslateKeyPortOuter     },
    { "PortInner",     bcmVlanTranslateKeyPortInner     },
    { "PortOuterTag",  bcmVlanTranslateKeyPortOuterTag  },
    { "PortInnerTag",  bcmVlanTranslateKeyPortInnerTag  },
    { "PortOuterPri",  bcmVlanTranslateKeyPortOuterPri  },
    { "PortGroupDouble",    bcmVlanTranslateKeyPortGroupDouble    },
    { "PortGroupOuter",     bcmVlanTranslateKeyPortGroupOuter     },
    { "PortGroupInner",     bcmVlanTranslateKeyPortGroupInner     }
};

static void
_ivlan_action_set_t_init(ivlan_action_set_t *data)
{
    if (data != NULL) {
        sal_memset(data, 0, sizeof(ivlan_action_set_t));
    }
}

static INLINE void
_parse_table_vlan_action_set_add(parse_table_t *pt, ivlan_action_set_t *act)
{
    if (pt == NULL || act == NULL) {
        return;
    }
    parse_table_add(pt, "OuterVlan",   PQ_HEX, 0,
                    &(act->new_outer_vlan), NULL);
    parse_table_add(pt, "InnerVlan",   PQ_HEX, 0,
                    &(act->new_inner_vlan), NULL);
    parse_table_add(pt, "Prio",        PQ_INT, 0,
                    &(act->priority), NULL);
    parse_table_add(pt, "DtOuter",     PQ_STRING, "None",
                    &(act->dt_outer), NULL);
    parse_table_add(pt, "DtOuterPrio", PQ_STRING, "None",
                    &(act->dt_outer_prio), NULL);
    parse_table_add(pt, "DtInner",     PQ_STRING, "None",
                    &(act->dt_inner), NULL);
    parse_table_add(pt, "DtInnerPrio", PQ_STRING, "None",
                    &(act->dt_inner_prio), NULL);
    parse_table_add(pt, "OtOuter",     PQ_STRING, "None",
                    &(act->ot_outer), NULL);
    parse_table_add(pt, "OtOuterPrio", PQ_STRING, "None",
                    &(act->ot_outer_prio), NULL);
    parse_table_add(pt, "OtInner",     PQ_STRING, "None",
                    &(act->ot_inner), NULL);
    parse_table_add(pt, "ItOuter",     PQ_STRING, "None",
                    &(act->it_outer), NULL);
    parse_table_add(pt, "ItInner",     PQ_STRING, "None",
                    &(act->it_inner), NULL);
    parse_table_add(pt, "ItInnerPrio", PQ_STRING, "None",
                    &(act->it_inner_prio), NULL);
    parse_table_add(pt, "UtOuter",     PQ_STRING, "None",
                    &(act->ut_outer), NULL);
    parse_table_add(pt, "UtInner",     PQ_STRING, "None",
                    &(act->ut_inner), NULL);
    parse_table_add(pt, "Policer",     PQ_HEX, 0,
                    &(act->policer), NULL);
    parse_table_add(pt, "ForwardingDomain",     PQ_HEX, 0,
                    &(act->forwarding_domain), NULL);
    parse_table_add(pt, "OuterTag",    PQ_STRING, "None",
                    &(act->outer_tag), NULL);
    parse_table_add(pt, "InnerTag",    PQ_STRING, "None",
                    &(act->inner_tag), NULL);
}

/*
 * Return bcm_vlan_action_t action from action name string
 */
static int
_vlan_action_get(const char *action_name)
{
    int i, names;

    if (action_name == NULL || *action_name == '\0') {
        return 0;
    }

    names = sizeof(vlan_action_names) / sizeof(struct vlan_action_names);
    for (i = 0; i < names; i++) {
        if (sal_strcasecmp(action_name, vlan_action_names[i].name) == 0) {
            return vlan_action_names[i].action;
        }
    }
    cli_out("Invalid action <%s>. Valid actions are:\n   ", action_name);
    for (i = 0; i < names; i++) {
        cli_out("%s ", vlan_action_names[i].name);
    }
    cli_out("\n");
    return -1;
}

/*
 * Return the action name string from bcm_vlan_action_t action
 */
static INLINE char *
_vlan_action_name_get(bcm_vlan_action_t action)
{
    int names = sizeof(vlan_action_names) / sizeof(struct vlan_action_names);
    return ((action >= 0 && action < names) ?  vlan_action_names[action].name :
            "Invalid action");
}

static INLINE void
_vlan_action_set_print(const bcm_vlan_action_set_t *action)
{
    if (action == NULL) {
        return;
    }
    cli_out("  DT: Outer=%-7s  OuterPrio=%-7s  Inner=%-7s  InnerPrio=%-7s\n",
            _vlan_action_name_get(action->dt_outer),
            _vlan_action_name_get(action->dt_outer_prio),
            _vlan_action_name_get(action->dt_inner),
            _vlan_action_name_get(action->dt_inner_prio));
    cli_out("  OT: Outer=%-7s  OuterPrio=%-7s  Inner=%-7s\n",
            _vlan_action_name_get(action->ot_outer),
            _vlan_action_name_get(action->ot_outer_prio),
            _vlan_action_name_get(action->ot_inner));
    cli_out("  IT: Outer=%-7s  Inner=%-7s      InnerPrio=%-7s\n",
            _vlan_action_name_get(action->it_outer),
            _vlan_action_name_get(action->it_inner),
            _vlan_action_name_get(action->it_inner_prio));
    cli_out("  UT: Outer=%-7s  Inner=%-7s\n",
            _vlan_action_name_get(action->ut_outer),
            _vlan_action_name_get(action->ut_inner));
    cli_out("  Policer=0x%x\n", action->policer_id);
    cli_out("  ForwardingDomain=0x%x\n", action->forwarding_domain);
    cli_out("  ALL tagged: Outer=%-7s  Inner=%-7s\n",
            _vlan_action_name_get(action->outer_tag),
            _vlan_action_name_get(action->inner_tag));
}

static int
_vlan_translate_egress_action_print(int unit, int port_class,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    bcm_vlan_action_set_t *action,
                                    void *user_data)
{
    cli_out("PortClass=%d, OldOuterVlan=%d, OldInnerVlan=%d\n",
            port_class, outer_vlan, inner_vlan);
    _vlan_action_set_print(action);

    return CMD_OK;
}

#define VLAN_IF_BAD_ACTION_RETURN(a, op)     \
    if (((a = (op)) == -1)) { return CMD_FAIL; }

static cmd_result_t
_ivlan_to_vlan_action_set(bcm_vlan_action_set_t *action, ivlan_action_set_t *act)
{
    if (action == NULL || act == NULL) {
        return CMD_FAIL;
    }
    action->new_outer_vlan = act->new_outer_vlan;
    action->new_inner_vlan = act->new_inner_vlan;
    action->priority = act->priority;
    VLAN_IF_BAD_ACTION_RETURN(action->dt_outer,
                              _vlan_action_get(act->dt_outer));
    VLAN_IF_BAD_ACTION_RETURN(action->dt_outer_prio,
                              _vlan_action_get(act->dt_outer_prio));
    VLAN_IF_BAD_ACTION_RETURN(action->dt_inner,
                              _vlan_action_get(act->dt_inner));
    VLAN_IF_BAD_ACTION_RETURN(action->dt_inner_prio,
                              _vlan_action_get(act->dt_inner_prio));
    VLAN_IF_BAD_ACTION_RETURN(action->ot_outer,
                              _vlan_action_get(act->ot_outer));
    VLAN_IF_BAD_ACTION_RETURN(action->ot_outer_prio,
                              _vlan_action_get(act->ot_outer_prio));
    VLAN_IF_BAD_ACTION_RETURN(action->ot_inner,
                              _vlan_action_get(act->ot_inner));
    VLAN_IF_BAD_ACTION_RETURN(action->it_outer,
                              _vlan_action_get(act->it_outer));
    VLAN_IF_BAD_ACTION_RETURN(action->it_inner,
                              _vlan_action_get(act->it_inner));
    VLAN_IF_BAD_ACTION_RETURN(action->it_inner_prio,
                              _vlan_action_get(act->it_inner_prio));
    VLAN_IF_BAD_ACTION_RETURN(action->ut_outer,
                              _vlan_action_get(act->ut_outer));
    VLAN_IF_BAD_ACTION_RETURN(action->ut_inner,
                              _vlan_action_get(act->ut_inner));
    action->policer_id = act->policer;
    action->forwarding_domain = act->forwarding_domain;
    VLAN_IF_BAD_ACTION_RETURN(action->outer_tag,
                              _vlan_action_get(act->outer_tag));
    VLAN_IF_BAD_ACTION_RETURN(action->inner_tag,
                              _vlan_action_get(act->inner_tag));
    return CMD_OK;
}

static int
_vlan_translate_range_action_print(int unit, bcm_gport_t gport,
                                   bcm_vlan_t outer_vlan_lo,
                                   bcm_vlan_t outer_vlan_hi,
                                   bcm_vlan_t inner_vlan_lo,
                                   bcm_vlan_t inner_vlan_hi,
                                   bcm_vlan_action_set_t *action,
                                   void *user_data)
{
    int             id;
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     trunk;

    VLAN_IF_ERROR_RETURN(
        bcmi_ltsw_port_gport_resolve(unit, gport, &modid, &port, &trunk, &id));
    if (BCM_GPORT_IS_TRUNK(gport)) {
        cli_out("TGID=%d, ", trunk);
    } else {
        cli_out("Port=%d, Modid=%d, ", port, modid);
    }
    cli_out("OuterVlanLo=%d, OuterVlanHi=%d, InnerVlanLo=%d, "
            "InnerVlanHi=%d\n", outer_vlan_lo, outer_vlan_hi,
            inner_vlan_lo, inner_vlan_hi);
    return CMD_OK;
}

static int
_vlan_action_translate_key_get(const char *key_str)
{
    int  i, names;

    names = sizeof(vlan_key_type_names) / sizeof(struct vlan_key_type_names);

    /* Get bcm_vlan_translate_key_t key type from key type string */
    for (i = 1; i < names; i++) {
        if (key_str &&
            (sal_strcasecmp(key_str, vlan_key_type_names[i].name) == 0)) {
            return vlan_key_type_names[i].key;
        }
    }
    cli_out("Invalid key type <%s>. Valid key types are:\n   ",
            key_str ? key_str : "");
    for (i = 1;  i < names; i++) {
        cli_out("%s ", vlan_key_type_names[i].name);
        if (i % 7 == 0) {
            cli_out("\n   ");
        }
    }
    cli_out("\n");
    return bcmVlanTranslateKeyInvalid;
}

static INLINE char *
_vlan_action_translate_key_name_get(bcm_vlan_translate_key_t key_type)
{
    int names = sizeof(vlan_key_type_names) / sizeof(struct vlan_key_type_names);
    int i;
    for (i = 0; i < names; i++) {
        if (key_type == vlan_key_type_names[i].key) {
            return vlan_key_type_names[i].name;
        }
    }
    return vlan_key_type_names[0].name;
}

static int
_vlan_translate_action_print(int unit, bcm_gport_t gport,
                             bcm_vlan_translate_key_t key_type,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcm_vlan_action_set_t *action,
                             void *user_data)
{
    if (action == NULL) {
        return CMD_FAIL;
    }

    cli_out("PortGroup=%d, ", gport);
    cli_out("KeyType=%s, ",_vlan_action_translate_key_name_get(key_type));
    if (outer_vlan == BCM_VLAN_INVALID) {
        cli_out("OldOuterVlan=--, ");
    } else {
        cli_out("OldOuterVlan=%d, ", outer_vlan);
    }
    if (inner_vlan == BCM_VLAN_INVALID) {
        cli_out("OldInnerVlan=--\n");
    } else {
        cli_out("OldInnerVlan=%d\n", inner_vlan);
    }
    _vlan_action_set_print(action);

    return CMD_OK;
}

static cmd_result_t
_vlan_action_translate_range(int unit, args_t *a)
{
    char       *subcmd;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_t       pt;
        bcm_gport_t         gport;
        int                 port,
                            outer_vlan_lo = BCM_VLAN_INVALID,
                            outer_vlan_hi = BCM_VLAN_INVALID,
                            inner_vlan_lo = BCM_VLAN_INVALID,
                            inner_vlan_hi = BCM_VLAN_INVALID;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "OuterVLanLo", PQ_DFL | PQ_INT, 0,
                        &outer_vlan_lo, NULL);
        parse_table_add(&pt, "OuterVLanHi", PQ_DFL | PQ_INT, 0,
                        &outer_vlan_hi, NULL);
        parse_table_add(&pt, "InnerVLanLo", PQ_DFL | PQ_INT, 0,
                        &inner_vlan_lo, NULL);
        parse_table_add(&pt, "InnerVLanHi", PQ_DFL | PQ_INT, 0,
                        &inner_vlan_hi, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("Error: invalid option %s\n",
                    ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /* Done with string parameters */
        parse_arg_eq_done(&pt);
        BCM_GPORT_LOCAL_SET(gport, port);
        VLAN_IF_ERROR_RETURN(
            bcm_vlan_translate_action_range_add(unit, gport,
                                                outer_vlan_lo,
                                                outer_vlan_hi,
                                                inner_vlan_lo,
                                                inner_vlan_hi,
                                                NULL));
        return CMD_OK;

    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        parse_table_t       pt;
        cmd_result_t        r;
        bcm_gport_t         gport;
        int                 port = 0,
                            outer_vlan_lo = BCM_VLAN_INVALID,
                            outer_vlan_hi = BCM_VLAN_INVALID,
                            inner_vlan_lo = BCM_VLAN_INVALID,
                            inner_vlan_hi = BCM_VLAN_INVALID;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "OuterVLanLo", PQ_DFL | PQ_INT, 0,
                        &outer_vlan_lo, NULL);
        parse_table_add(&pt, "OuterVLanHi", PQ_DFL | PQ_INT, 0,
                        &outer_vlan_hi, NULL);
        parse_table_add(&pt, "InnerVLanLo", PQ_DFL | PQ_INT, 0,
                        &inner_vlan_lo, NULL);
        parse_table_add(&pt, "InnerVLanHi", PQ_DFL | PQ_INT, 0,
                        &inner_vlan_hi, NULL);
        if (!parseEndOk( a, &pt, &r)) {
            return r;
        }
        BCM_GPORT_LOCAL_SET(gport, port);
        VLAN_IF_ERROR_RETURN(
            bcm_vlan_translate_action_range_delete(unit, gport,
                                                   outer_vlan_lo,
                                                   outer_vlan_hi,
                                                   inner_vlan_lo,
                                                   inner_vlan_hi));

        return CMD_OK;
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
         VLAN_IF_ERROR_RETURN(
             bcm_vlan_translate_action_range_traverse(unit,
                                             _vlan_translate_range_action_print,
                                             NULL));
         return CMD_OK;
     } else if (sal_strcasecmp(subcmd, "clear") == 0) {
         VLAN_IF_ERROR_RETURN(
             bcm_vlan_translate_action_range_delete_all(unit));
         return CMD_OK;
     }

    return CMD_USAGE;
}

static cmd_result_t
_vlan_action_translate_egress(int unit, args_t *a)
{
    char       *subcmd;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_t       pt;
        cmd_result_t        r;
        int                 old_outer_vlan, old_inner_vlan, port_class = 0;
        ivlan_action_set_t  act;
        bcm_vlan_action_set_t action;

        _ivlan_action_set_t_init(&act);
        parse_table_init(unit, &pt);
        /*
         * The bcmPortClassVlanTranslateEgress port class is created with
         * bcm_port_class_set(). The default bcmPortClassVlanTransateEgress
         * class for a port is its own port number.
         */
        parse_table_add(&pt, "PortClass", PQ_INT, 0, &port_class, NULL);
        parse_table_add(&pt, "OldOuterVLan", PQ_INT, 0, &old_outer_vlan, NULL);
        parse_table_add(&pt, "OldInnerVLan", PQ_INT, 0, &old_inner_vlan, NULL);
        _parse_table_vlan_action_set_add(&pt, &act);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("Error: invalid option %s\n",
                    ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        bcm_vlan_action_set_t_init(&action);
        r = _ivlan_to_vlan_action_set(&action, &act);
        if (r != CMD_OK) {
            parse_arg_eq_done(&pt);
            cli_out("Error: invalid action\n");
            return r;
        }
        /* Done with string parameters */
        parse_arg_eq_done(&pt);
        VLAN_IF_ERROR_RETURN(
            bcm_vlan_translate_egress_action_add(unit, port_class,
                                                 old_outer_vlan,
                                                 old_inner_vlan,
                                                 &action));
        return CMD_OK;

    } else if ((sal_strcasecmp(subcmd, "delete") == 0) ||
               (sal_strcasecmp(subcmd, "get") == 0)) {
        parse_table_t       pt;
        cmd_result_t        r;
        int                 old_outer_vlan, old_inner_vlan, port_class = 0;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortClass", PQ_INT, 0, &port_class, NULL);
        parse_table_add(&pt, "OldOuterVLan", PQ_INT, 0, &old_outer_vlan, NULL);
        parse_table_add(&pt, "OldInnerVLan", PQ_INT, 0, &old_inner_vlan, NULL);
        if (!parseEndOk( a, &pt, &r)) {
            return r;
        }
        if (sal_strcasecmp(subcmd, "delete") == 0) {
            VLAN_IF_ERROR_RETURN
                (bcm_vlan_translate_egress_action_delete(unit, port_class,
                                                         old_outer_vlan,
                                                         old_inner_vlan));
            return CMD_OK;
        } else { /* get */
            bcm_vlan_action_set_t action;
            VLAN_IF_ERROR_RETURN
                (bcm_vlan_translate_egress_action_get(unit, port_class,
                                                      old_outer_vlan,
                                                      old_inner_vlan,
                                                      &action));
            _vlan_translate_egress_action_print(unit, port_class,
                                                old_outer_vlan,
                                                old_inner_vlan, &action, NULL);
            return CMD_OK;
        }
    } else  if (sal_strcasecmp(subcmd, "show") == 0) {
        VLAN_IF_ERROR_RETURN
            (bcm_vlan_translate_egress_action_traverse(unit,
                 _vlan_translate_egress_action_print, NULL));
        return CMD_OK;
    }

    return CMD_USAGE;
}

static cmd_result_t
_vlan_action_translate_ingress(int unit, args_t *a, char* subcmd)
{
    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_t       pt;
        cmd_result_t        r;
        int                 old_outer_vlan, old_inner_vlan, port = 0;
        char               *key_str = NULL;
        bcm_vlan_translate_key_t key_type;
        ivlan_action_set_t  act;
        bcm_vlan_action_set_t action;

        _ivlan_action_set_t_init(&act);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "KeyType", PQ_STRING, 0, &key_str, NULL);
        parse_table_add(&pt, "OldOuterVLan", PQ_INT, 0, &old_outer_vlan, NULL);
        parse_table_add(&pt, "OldInnerVLan", PQ_INT, 0, &old_inner_vlan, NULL);
        _parse_table_vlan_action_set_add(&pt, &act);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("Error: invalid option %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        bcm_vlan_action_set_t_init(&action);
        r = _ivlan_to_vlan_action_set(&action, &act);
        if (r != CMD_OK) {
            parse_arg_eq_done(&pt);
            cli_out("Error: invalid action\n");
            return r;
        }
        key_type = _vlan_action_translate_key_get(key_str);
        parse_arg_eq_done(&pt);
        if (key_type == bcmVlanTranslateKeyInvalid) {
            cli_out("Error: invalid key\n");
            return CMD_FAIL;
        }

        VLAN_IF_ERROR_RETURN(
            bcm_vlan_translate_action_add(unit, port, key_type,
                                          old_outer_vlan,
                                          old_inner_vlan, &action));
        return CMD_OK;

    } else if ((sal_strcasecmp(subcmd, "delete") == 0) ||
               (sal_strcasecmp(subcmd, "get") == 0)) {
        parse_table_t       pt;
        int                 old_outer_vlan, old_inner_vlan, port = 0;
        char               *key_str = NULL;
        bcm_vlan_translate_key_t key_type;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "KeyType", PQ_STRING, 0, &key_str, NULL);
        parse_table_add(&pt, "OldOuterVLan", PQ_INT, 0, &old_outer_vlan, NULL);
        parse_table_add(&pt, "OldInnerVLan", PQ_INT, 0, &old_inner_vlan, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("Error: invalid option %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        key_type = _vlan_action_translate_key_get(key_str);
        if (key_type == bcmVlanTranslateKeyInvalid) {
            cli_out("Error: invalid key\n");
            return CMD_FAIL;
        }
        if (sal_strcasecmp(subcmd, "delete") == 0) {
            VLAN_IF_ERROR_RETURN(
                bcm_vlan_translate_action_delete(unit, port, key_type,
                                                 old_outer_vlan,
                                                 old_inner_vlan));
            parse_arg_eq_done(&pt);
            return CMD_OK;

        } else {
            bcm_vlan_action_set_t action;
            VLAN_IF_ERROR_RETURN(
                bcm_vlan_translate_action_get(unit, port, key_type,
                                              old_outer_vlan,
                                              old_inner_vlan,
                                              &action));
            cli_out("Port=%d, KeyType=%s, OldOuterVlan=%d, OldInnerVlan=%d\n",
                    port, key_str, old_outer_vlan, old_inner_vlan);
            _vlan_action_set_print(&action);
            parse_arg_eq_done(&pt);
            return CMD_OK;
        }

    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        VLAN_IF_ERROR_RETURN(
            bcm_vlan_translate_action_traverse(unit,
                                               _vlan_translate_action_print,
                                               NULL));
        return CMD_OK;
    }
    return CMD_USAGE;
}

static cmd_result_t
_vlan_action_translate(int unit, args_t *a)
{
    char       *subcmd;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "egress") == 0) {
        return _vlan_action_translate_egress(unit, a);
    }
    if (sal_strcasecmp(subcmd, "range") == 0) {
        return _vlan_action_translate_range(unit, a);
    }

    return _vlan_action_translate_ingress(unit, a, subcmd);
}

static cmd_result_t
_vlan_action_port(int unit, args_t *a)
{
    char       *subcmd;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd, "default") == 0) {
        if ((subcmd = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        if (sal_strcasecmp(subcmd, "add") == 0) {
            int                     port;
            parse_table_t           pt;
            cmd_result_t            r;
            bcm_vlan_action_set_t   action;
            ivlan_action_set_t      act;

            _ivlan_action_set_t_init(&act);
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
            _parse_table_vlan_action_set_add(&pt, &act);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("Error: Invalid option %s\n", ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            bcm_vlan_action_set_t_init(&action);
            r = _ivlan_to_vlan_action_set(&action, &act);
            parse_arg_eq_done(&pt);
            if (r != CMD_OK) {
                cli_out("Error: invalid action\n");
                return r;
            }

            VLAN_IF_ERROR_RETURN(
                bcm_vlan_port_default_action_set(unit, port, &action));
            return CMD_OK;

        } else if (sal_strcasecmp(subcmd, "get") == 0) {
            int                     port = 1;
            parse_table_t           pt;
            cmd_result_t            r;
            bcm_vlan_action_set_t   action;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
            if (!parseEndOk( a, &pt, &r)) {
                return r;
            }

            VLAN_IF_ERROR_RETURN(
                bcm_vlan_port_default_action_get(unit, port, &action));
            cli_out("Port %d: New OVLAN=%d, New IVLAN=%d, New Prio=%d\n",
                    port, action.new_outer_vlan, action.new_inner_vlan,
                    action.priority);
            _vlan_action_set_print(&action);
            return CMD_OK;
        }
    }

    return CMD_USAGE;
}

static cmd_result_t
cmd_ltsw_vlan_create(int unit, args_t *a)
{
    char *c;
    int r = 0;
    vlan_id_t id = VLAN_ID_INVALID;
    bcm_pbmp_t      arg_ubmp;
    bcm_pbmp_t      arg_pbmp;
    parse_table_t   pt;

    BCM_PBMP_CLEAR(arg_ubmp);
    BCM_PBMP_CLEAR(arg_pbmp);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
    return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!isint(c)) {
        cli_out("ERROR: Invalid VLAN ID \"%s\"\n", c);
        return CMD_FAIL;
    }

    id = parse_integer(c);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap",  PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &arg_pbmp, NULL);
    parse_table_add(&pt, "UntagBitMap",     PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &arg_ubmp, NULL);
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if ((r = bcm_vlan_create(unit, id)) < 0) {
        goto bcm_err;
    }
    if ((r = bcm_vlan_port_add(unit, id, arg_pbmp, arg_ubmp)) < 0) {
        goto bcm_err;
    }

    return CMD_OK;

bcm_err:
   cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
   return CMD_FAIL;
}

static cmd_result_t
cmd_ltsw_vlan_destroy(int unit, args_t *a)
{
    char *c;
    int r = 0;
    vlan_id_t id = VLAN_ID_INVALID;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
    return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!isint(c)) {
        cli_out("ERROR: Invalid VLAN ID \"%s\"\n", c);
        return CMD_FAIL;
    }

    id = parse_integer(c);
    if ((r = bcm_vlan_destroy(unit, id)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }
    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_vlan_destroy_all(int unit, args_t *a)
{
    int r = 0;

    if ((r = bcm_vlan_destroy_all(unit)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }
    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_vlan_port_add(int unit, args_t *a)
{
    char *c;
    int r = 0;
    vlan_id_t id = VLAN_ID_INVALID;
    bcm_pbmp_t      arg_ubmp;
    bcm_pbmp_t      arg_pbmp;
    parse_table_t   pt;

    BCM_PBMP_CLEAR(arg_ubmp);
    BCM_PBMP_CLEAR(arg_pbmp);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
    return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!isint(c)) {
        cli_out("ERROR: Invalid VLAN ID \"%s\"\n", c);
        return CMD_FAIL;
    }

    id = parse_integer(c);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap",  PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &arg_pbmp, NULL);
    parse_table_add(&pt, "UntagBitMap",     PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &arg_ubmp, NULL);
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if ((r = bcm_vlan_port_add(unit, id, arg_pbmp, arg_ubmp)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_vlan_port_remove(int unit, args_t *a)
{
    char *c;
    int r = 0;
    vlan_id_t id = VLAN_ID_INVALID;
    bcm_pbmp_t      arg_pbmp;
    parse_table_t   pt;

    BCM_PBMP_CLEAR(arg_pbmp);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
    return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!isint(c)) {
        cli_out("ERROR: Invalid VLAN ID \"%s\"\n", c);
        return CMD_FAIL;
    }

    id = parse_integer(c);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap",  PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &arg_pbmp, NULL);
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if ((r = bcm_vlan_port_remove(unit, id, arg_pbmp)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_vlan_show(int unit, args_t *a)
{
    char *c;
    bcm_vlan_data_t *list;
    int     count, i, alloc_bufp, alloc_pfmtp;
    char        *bufp = NULL;
    char        *pfmtp = NULL;
    vlan_id_t id = VLAN_ID_INVALID;
    int r;
    static char *bcm_vlan_mcast_flood_str[] = BCM_VLAN_MCAST_FLOOD_STR;

    if ((c = ARG_GET(a)) != NULL) {
        id = parse_integer(c);
    }

    if ((r = bcm_vlan_list(unit, &list, &count)) < 0) {
        goto bcm_err;
    }

    /* Don't put giant port arrays on the stack */
    alloc_bufp = FORMAT_PBMP_MAX * sizeof(*bufp);
    bufp = sal_alloc(alloc_bufp, "CLI VLAN show port format");
    if (NULL == bufp) {
        r = BCM_E_MEMORY;
    }

    if (BCM_SUCCESS(r)) {
        alloc_pfmtp = SOC_PBMP_FMT_LEN * sizeof(*pfmtp);
        pfmtp = sal_alloc(alloc_pfmtp, "CLI VLAN show pbmp format");
        if (NULL == pfmtp) {
            r = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(r)) {
        for (i = 0; i < count; i++) {
            if (id == VLAN_ID_INVALID || list[i].vlan_tag == id) {
                bcm_vlan_mcast_flood_t  mode;
                if ((r = bcm_vlan_mcast_flood_get(unit,
                                                  list[i].vlan_tag,
                                                  &mode)) < 0) {
                    if (r == BCM_E_UNAVAIL) {
                        mode = BCM_VLAN_MCAST_FLOOD_COUNT;
                        r = BCM_E_NONE;
                    } else {
                        break;
                    }
                }

                format_bcm_pbmp(unit, bufp, alloc_bufp,
                                list[i].port_bitmap);
                cli_out("vlan %d\tports %s (%s), ",
                        list[i].vlan_tag,
                        bufp, SOC_PBMP_FMT(list[i].port_bitmap, pfmtp));

                format_bcm_pbmp(unit, bufp, alloc_bufp,
                                list[i].ut_port_bitmap);
                cli_out("untagged %s (%s) %s\n",
                        bufp, SOC_PBMP_FMT(list[i].ut_port_bitmap, pfmtp),
                        bcm_vlan_mcast_flood_str[mode]);
            }
        }
    }

    if (NULL != pfmtp) {
        sal_free(pfmtp);
    }
    if (NULL != bufp) {
        sal_free(bufp);
    }
    bcm_vlan_list_destroy(unit, list, count);
    if (BCM_FAILURE(r)) {
        return CMD_FAIL;
    }

    return CMD_OK;
bcm_err:
   cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
   return CMD_FAIL;

}

static cmd_result_t
cmd_ltsw_vlan_property_set(int unit, vlan_id_t id, args_t *a)
{
    int r = BCM_E_NONE;
    cmd_result_t cmd_r = CMD_OK;
    bcm_vlan_control_vlan_t vlan_control, default_control;
    int outer_tpid, learn_disable, unknown_ip6_mcast_to_cpu;
    int def_outer_tpid, def_learn_disable, def_unknown_ip6_mcast_to_cpu;
    int unknown_ip4_mcast_to_cpu, def_unknown_ip4_mcast_to_cpu;
    int def_ip4_disable, ip4_disable;
    int def_ip6_disable, ip6_disable;
    int def_ip4_mcast_disable, ip4_mcast_disable;
    int def_ip6_mcast_disable, ip6_mcast_disable;
    int def_ip4_mcast_l2_disable, ip4_mcast_l2_disable;
    int def_ip6_mcast_l2_disable, ip6_mcast_l2_disable;
    int def_mpls_disable, mpls_disable;
    int def_cosq_enable, cosq_enable;
    int def_l3_vrf_global_disable, l3_vrf_global_disable;
    int def_mim_term_disable, mim_term_disable;
    bcm_cos_queue_t def_cosq, cosq;
    static char *bcm_vlan_mcast_flood_str[] = BCM_VLAN_MCAST_FLOOD_STR;
    parse_table_t pt;
    ivlan_action_set_t  act;
    _ivlan_action_set_t_init(&act);

    if (VLAN_ID_VALID(id)) {
        sal_memset(&default_control, 0, sizeof(bcm_vlan_control_vlan_t));

        r = bcm_vlan_control_vlan_get(unit, id, &default_control);
        if (r < 0) {
            goto bcm_err;
        }

        sal_memcpy(&vlan_control, &default_control, sizeof(vlan_control));

        def_outer_tpid    = default_control.outer_tpid;
        outer_tpid        = def_outer_tpid;

        def_learn_disable = (default_control.flags &
                            BCM_VLAN_LEARN_DISABLE) ? 1 : 0;
        learn_disable     = def_learn_disable;

        def_unknown_ip6_mcast_to_cpu = (default_control.flags &
                                  BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU) ? 1 : 0;
        unknown_ip6_mcast_to_cpu     =  def_unknown_ip6_mcast_to_cpu;

        def_unknown_ip4_mcast_to_cpu = (default_control.flags &
                                  BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU) ? 1 : 0;
        unknown_ip4_mcast_to_cpu     = def_unknown_ip4_mcast_to_cpu;

        def_ip4_disable = (default_control.flags &
                            BCM_VLAN_IP4_DISABLE) ? 1 : 0;
        ip4_disable     = def_ip4_disable;

        def_ip6_disable = (default_control.flags &
                            BCM_VLAN_IP6_DISABLE) ? 1 : 0;
        ip6_disable     = def_ip6_disable;

        def_ip4_mcast_disable = (default_control.flags &
                            BCM_VLAN_IP4_MCAST_DISABLE) ? 1 : 0;
        ip4_mcast_disable     = def_ip4_mcast_disable;

        def_ip6_mcast_disable = (default_control.flags &
                            BCM_VLAN_IP6_MCAST_DISABLE) ? 1 : 0;
        ip6_mcast_disable     = def_ip6_mcast_disable;

        def_l3_vrf_global_disable = (default_control.flags &
                            BCM_VLAN_L3_VRF_GLOBAL_DISABLE) ? 1 : 0;
        l3_vrf_global_disable = def_l3_vrf_global_disable;

        def_ip4_mcast_l2_disable = (default_control.flags &
                            BCM_VLAN_IP4_MCAST_L2_DISABLE) ? 1 : 0;
        ip4_mcast_l2_disable     = def_ip4_mcast_l2_disable;

        def_ip6_mcast_l2_disable = (default_control.flags &
                            BCM_VLAN_IP6_MCAST_L2_DISABLE) ? 1 : 0;
        ip6_mcast_l2_disable     = def_ip6_mcast_l2_disable;

        def_mpls_disable = (default_control.flags &
                            BCM_VLAN_MPLS_DISABLE) ? 1 : 0;
        mpls_disable     = def_mpls_disable;

        def_mim_term_disable = (default_control.flags &
                            BCM_VLAN_MIM_TERM_DISABLE) ? 1 : 0;
        mim_term_disable     = def_mim_term_disable;

        def_cosq_enable = (default_control.flags &
                           BCM_VLAN_COSQ_ENABLE) ? 1 : 0;
        cosq_enable     = def_cosq_enable;

        def_cosq        = default_control.cosq;
        cosq            = def_cosq;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "VRF", PQ_INT | PQ_DFL, &default_control.vrf,
                        &vlan_control.vrf, NULL);
        parse_table_add(&pt, "OuterTPID", PQ_HEX | PQ_DFL, &def_outer_tpid,
                        &outer_tpid, NULL);
        parse_table_add(&pt, "LearnDisable", PQ_INT | PQ_DFL,
                        &def_learn_disable, &learn_disable, NULL);
        parse_table_add(&pt, "UnknownIp6McastToCpu", PQ_INT | PQ_DFL,
                        &def_unknown_ip6_mcast_to_cpu,
                        &unknown_ip6_mcast_to_cpu, NULL);
        parse_table_add(&pt, "UnknownIp4McastToCpu", PQ_INT | PQ_DFL,
                        &def_unknown_ip4_mcast_to_cpu,
                        &unknown_ip4_mcast_to_cpu, NULL);
        parse_table_add(&pt, "Ip4Disable", PQ_INT | PQ_DFL,
                        &def_ip4_disable, &ip4_disable, NULL);
        parse_table_add(&pt, "Ip6Disable", PQ_INT | PQ_DFL,
                        &def_ip6_disable, &ip6_disable, NULL);
        parse_table_add(&pt, "Ip4McastDisable", PQ_INT | PQ_DFL,
                        &def_ip4_mcast_disable, &ip4_mcast_disable, NULL);
        parse_table_add(&pt, "Ip6McastDisable", PQ_INT | PQ_DFL,
                        &def_ip6_mcast_disable, &ip6_mcast_disable, NULL);
        parse_table_add(&pt, "Ip4McastL2Disable", PQ_INT | PQ_DFL,
                        &def_ip4_mcast_l2_disable, &ip4_mcast_l2_disable,
                        NULL);
        parse_table_add(&pt, "Ip6McastL2Disable", PQ_INT | PQ_DFL,
                        &def_ip6_mcast_l2_disable, &ip6_mcast_l2_disable,
                        NULL);
        parse_table_add(&pt, "L3VrfGlobalDisable", PQ_INT | PQ_DFL,
                        &def_l3_vrf_global_disable, &l3_vrf_global_disable,
                        NULL);
        parse_table_add(&pt, "MplsDisable", PQ_INT | PQ_DFL,
                        &def_mpls_disable, &mpls_disable, NULL);
        parse_table_add(&pt, "CosqEnable", PQ_INT | PQ_DFL,
                        &def_cosq_enable, &cosq_enable, NULL);
        parse_table_add(&pt, "MiMTermDisable", PQ_INT | PQ_DFL,
                        &def_mim_term_disable, &mim_term_disable, NULL);
        parse_table_add(&pt, "Cosq", PQ_INT | PQ_DFL,
                        &def_cosq, &cosq, NULL);
        parse_table_add(&pt, "Ip6McastFloodMode", PQ_MULTI | PQ_DFL,
                        &default_control.ip6_mcast_flood_mode,
                        &vlan_control.ip6_mcast_flood_mode,
                        bcm_vlan_mcast_flood_str);
        parse_table_add(&pt, "Ip4McastFloodMode", PQ_MULTI | PQ_DFL,
                        &default_control.ip4_mcast_flood_mode,
                        &vlan_control.ip4_mcast_flood_mode,
                        bcm_vlan_mcast_flood_str);
        parse_table_add(&pt, "L2McastFloodMode", PQ_MULTI | PQ_DFL,
                        &default_control.l2_mcast_flood_mode,
                        &vlan_control.l2_mcast_flood_mode,
                        bcm_vlan_mcast_flood_str);
        parse_table_add(&pt, "IfClass", PQ_INT | PQ_DFL,
                        &default_control.if_class,
                        &vlan_control.if_class, NULL);
        parse_table_add(&pt, "BCgroup", PQ_INT | PQ_DFL,
                        &default_control.broadcast_group,
                        &vlan_control.broadcast_group, NULL);
        parse_table_add(&pt, "UMCgroup", PQ_INT | PQ_DFL,
                        &default_control.unknown_multicast_group,
                        &vlan_control.unknown_multicast_group, NULL);
        parse_table_add(&pt, "UUCgroup", PQ_INT | PQ_DFL,
                        &default_control.unknown_unicast_group,
                        &vlan_control.unknown_unicast_group, NULL);
        _parse_table_vlan_action_set_add(&pt, &act);
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: ERROR: Unknown option: %s\n",
                        ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        cmd_r = _ivlan_to_vlan_action_set(&vlan_control.egress_action, &act);
        if (cmd_r != CMD_OK) {
            parse_arg_eq_done(&pt);
            cli_out("Error: invalid action\n");
            return cmd_r;
        }
        parse_arg_eq_done(&pt);

        vlan_control.outer_tpid     = (uint16) outer_tpid;
        vlan_control.cosq           = cosq;
        vlan_control.flags = (learn_disable ?
                                      BCM_VLAN_LEARN_DISABLE : 0);
        vlan_control.flags |= (unknown_ip6_mcast_to_cpu ?
                                      BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU : 0);
        vlan_control.flags |= (unknown_ip4_mcast_to_cpu?
                                      BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU : 0);
        vlan_control.flags |= (ip4_disable ?
                                      BCM_VLAN_IP4_DISABLE : 0);
        vlan_control.flags |= (ip6_disable ?
                                      BCM_VLAN_IP6_DISABLE : 0);
        vlan_control.flags |= (ip4_mcast_disable ?
                                      BCM_VLAN_IP4_MCAST_DISABLE : 0);
        vlan_control.flags |= (ip6_mcast_disable ?
                                      BCM_VLAN_IP6_MCAST_DISABLE : 0);
        vlan_control.flags |= (l3_vrf_global_disable ?
                                      BCM_VLAN_L3_VRF_GLOBAL_DISABLE : 0);
        vlan_control.flags |= (ip4_mcast_l2_disable ?
                                      BCM_VLAN_IP4_MCAST_L2_DISABLE : 0);
        vlan_control.flags |= (ip6_mcast_l2_disable ?
                                      BCM_VLAN_IP6_MCAST_L2_DISABLE : 0);
        vlan_control.flags |= (mpls_disable ?
                                      BCM_VLAN_MPLS_DISABLE : 0);
        vlan_control.flags |= (cosq_enable ?
                                      BCM_VLAN_COSQ_ENABLE : 0);
        vlan_control.flags |= (mim_term_disable ?
                                      BCM_VLAN_MIM_TERM_DISABLE : 0);
        r = bcm_vlan_control_vlan_set(unit, id, vlan_control);
        if (r < 0) {
            return CMD_FAIL;
        }
        return CMD_OK;
    }
bcm_err:
   cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
   return CMD_FAIL;
}

static cmd_result_t
cmd_ltsw_vlan_gport(int unit, args_t *a)
{
    int r = BCM_E_NONE;
    cmd_result_t retCode;
    char *subcmd, *c;
    bcm_vlan_t id;
    bcm_gport_t gport = BCM_GPORT_INVALID;
    parse_table_t pt;
    int is_untagged = 0;
    subcmd = ARG_GET(a);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing gport subcommand\n", ARG_CMD(a));
        return CMD_FAIL;
    }
    /* Check against all valid "vlan gport" subcommands. */
    if ((sal_strcasecmp(subcmd, "delete") != 0) &&
        (sal_strcasecmp(subcmd, "add") != 0) &&
        (sal_strcasecmp(subcmd, "get") != 0) &&
        (sal_strcasecmp(subcmd, "clear") != 0) &&
        (sal_strcasecmp(subcmd, "show") != 0)) {
        cli_out("%s: ERROR: Invalid gport subcommand\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    /* Fetch next argument, the VLAN id */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    /* "id" gets the vlan ID */
    id = parse_integer(c);

    if ((sal_strcasecmp(subcmd, "clear") != 0) &&
        (sal_strcasecmp(subcmd, "show") != 0)) {
        /* "clear" and "show" do not need a gport parameter, the others do */
        gport = 0;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortID", PQ_DFL | PQ_PORT,
                        (void *) (0), &gport, NULL);

        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
    }

    if (sal_strcasecmp(subcmd, "delete") == 0) {
        if ((r = bcm_vlan_gport_delete(unit, id, gport)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "add") == 0) {
        if ((r = bcm_vlan_gport_add(unit, id, gport, 0)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        if ((r = bcm_vlan_gport_get(unit, id, gport, &is_untagged)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "clear") == 0) {
        if ((r = bcm_vlan_gport_delete_all(unit, id)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        const int max = BCM_PBMP_PORT_MAX;
        int i, count, alloc_size;
        bcm_gport_t *port_array;
        int *is_untagged_array;

        /* Don't put giant port arrays on the stack */
        alloc_size = BCM_PBMP_PORT_MAX * sizeof(*port_array);
        port_array = sal_alloc(alloc_size, "CLI VLAN show ports");
        if (NULL == port_array) {
            r = BCM_E_MEMORY;
            goto bcm_err;
        }
        alloc_size = BCM_PBMP_PORT_MAX * sizeof(*is_untagged_array);
        is_untagged_array = sal_alloc(alloc_size,
                                       "CLI VLAN show untagged ports");
        if (NULL == is_untagged_array) {
            sal_free(port_array);
            r = BCM_E_MEMORY;
            goto bcm_err;
        }

        /* Get all virtual and physical ports from the specified VLAN */
        r = bcm_vlan_gport_get_all(unit, id, max, port_array,
                                   is_untagged_array, &count);

        if (0 <= r) {
            /*
             * Count will always be non-zero
             * (BCM_E_NOT_FOUND returned when
             * VLAN is valid but has no associated ports).
             */
            cli_out("Virtual and physical ports defined for VLAN: %d\n",
                    id);
            for (i = 0; i < count; i++) {
                /* port name is empty string if port is not local */
                const char *port_name = mod_port_name(unit, -1,
                                                      port_array[i]);

                if (*port_name) {
                    cli_out("  %4s (%stagged)\n", port_name,
                            is_untagged_array[i] ? "un" : "");
                }
            }
        }

        sal_free(port_array);
        sal_free(is_untagged_array);
        if (r < 0) {
            goto bcm_err;
        }
    }
    return CMD_OK;

bcm_err:
   cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
   return CMD_FAIL;
}

static cmd_result_t
cmd_ltsw_vlan_control(int unit, args_t *a)
{
    int r = BCM_E_NONE;
    char *subcmd;
    char    *value, *tname;
    int ttype, i, varg, matched;

    static struct {         /* match enum from bcm/vlan.h */
        int     type;
        char    *name;
    } typenames[] = {
        { bcmVlanTranslate,   "translate" },
        { 0,            NULL }      /* LAST ENTRY */
    };

    subcmd = ARG_GET(a);
    value = ARG_GET(a);

    matched = 0;

    for (i = 0; typenames[i].name != NULL; i++) {
        tname = typenames[i].name;
        if (subcmd == NULL || sal_strcasecmp(subcmd, tname) == 0) {
        matched += 1;
        ttype = typenames[i].type;
        if (value == NULL) {
            r = bcm_vlan_control_get(unit, ttype, &varg);
            if (r < 0) {
            cli_out("%-20s-\t%s\n", tname, bcm_errmsg(r));
            } else {
            cli_out("%-20s%d\n", tname, varg);
            }
        } else {
            varg = parse_integer(value);
            r = bcm_vlan_control_set(unit, ttype, varg);
            if (r < 0) {
            cli_out("%s\tERROR: %s\n", tname, bcm_errmsg(r));
            }
        }
        }
    }

    if (matched == 0) {
        cli_out("%s: ERROR: Unknown control name\n", subcmd);
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_vlan_stat(int unit, args_t *a)
{
    char *c;
    int rv, attach = 0, stat_id = 0;
    vlan_id_t id = VLAN_ID_INVALID;
    parse_table_t   pt;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
    return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!isint(c)) {
        cli_out("ERROR: Invalid VLAN ID \"%s\"\n", c);
        return CMD_FAIL;
    }

    id = parse_integer(c);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Attach", PQ_DFL|PQ_BOOL, (void *)(0), &attach, NULL);
    parse_table_add(&pt, "StatId", PQ_DFL|PQ_INT, (void *)(0), &stat_id, NULL);
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if (attach) {
        rv = bcm_vlan_stat_attach(unit, id, stat_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        rv = bcm_vlan_stat_detach_with_id(unit, id, stat_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_vlan_default(int unit, args_t *a)
{
    char *c;
    vlan_id_t id = VLAN_ID_INVALID;
    int r = 0;

    if ((c = ARG_GET(a)) != NULL) {
        id = parse_integer(c);
    }

    if (id == VLAN_ID_INVALID) {
        if ((r = bcm_vlan_default_get(unit, &id)) < 0) {
        goto bcm_err;
        }

        cli_out("Default VLAN ID is %d\n", id);
    } else {
        if ((r = bcm_vlan_default_set(unit, id)) < 0) {
        goto bcm_err;
        }

        cli_out("Default VLAN ID set to %d\n", id);
    }

    return CMD_OK;
bcm_err:
   cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
   return CMD_FAIL;

}

static cmd_result_t
cmd_ltsw_vlan_translate(int unit, args_t *a)
{
    char *subcmd;
    int r = 0;

    subcmd = ARG_GET(a);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing translate subcommand\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "On") == 0) {
        if ((r = bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE)) < 0) {
            goto bcm_err;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "Off") == 0) {
        if ((r = bcm_vlan_control_set(unit, bcmVlanTranslate, FALSE)) < 0) {
            goto bcm_err;
        }
        return CMD_OK;
    }

    cli_out("%s: ERROR: Unknown translate subcommand: %s\n",
            ARG_CMD(a), subcmd);
    return CMD_FAIL;

bcm_err:
   cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
   return CMD_FAIL;
}

static cmd_result_t
cmd_ltsw_vlan_port(int unit, args_t *a)
{
    char *c, *subcmd;
    int r = 0;
    char *value, *tname;
    int ttype, i, varg, matched;
    bcm_port_t port;
    pbmp_t       pbm, pbmp_port_all;
    static struct { /* match enum from bcm/vlan.h */
        int      type;
        char    *name;
    } typenames[] = {
            { bcmVlanPortPreferIP4,            "preferip4" },
            { bcmVlanPortPreferMAC,            "prefermac" },
            { bcmVlanTranslateIngressEnable,   "translateingress" },
            { bcmVlanTranslateIngressMissDrop, "translateingressmissdrop" },
            { bcmVlanTranslateEgressEnable,    "translateegress" },
            { bcmVlanTranslateEgressMissDrop,  "translateegressmissdrop" },
     { bcmVlanTranslateEgressMissUntaggedDrop, "translateegressmissuntagdrop" },
     { bcmVlanTranslateEgressMissUntag,        "translateegressmissuntag"},
            { bcmVlanLookupMACEnable,          "lookupmac" },
            { bcmVlanLookupIPEnable,           "lookupip" },
            { bcmVlanPortUseInnerPri,          "useinnerpri" },
            { bcmVlanPortUseOuterPri,          "useouterpri" },
            { bcmVlanPortVerifyOuterTpid,      "verifyoutertpid" },
            { bcmVlanPortOuterTpidSelect,      "outertpidselect"},
            { bcmVlanPortTranslateKeyFirst,    "translatekeyfirst"},
            { bcmVlanPortTranslateKeySecond,   "translatekeysecond"},
            { bcmVlanPortPayloadTagsDelete,         "PayloadTagsDelete" },
            { bcmVlanPortTranslateEgressVnidEnable, "TranslateEgressVnidEnable" },
            { 0,                                     NULL }   /* LAST ENTRY */
    };

    if ((r = bcmi_ltsw_port_bitmap(unit,
                  BCMI_LTSW_PORT_TYPE_ALL, &pbmp_port_all)) < 0) {
        return CMD_FAIL;
    }
    if ((c = ARG_GET(a)) == NULL) {
        BCM_PBMP_ASSIGN(pbm, pbmp_port_all);
    } else if (parse_bcm_pbmp(unit, c, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n",
                ARG_CMD(a), c);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, pbmp_port_all);
    if (BCM_PBMP_IS_NULL(pbm)) {
        cli_out("No ports specified.\n");
        return CMD_OK;
    }

    subcmd = ARG_GET(a);
    value = ARG_GET(a);
    matched = 0;

    BCM_PBMP_ITER(pbm, port) {
        cli_out("\nVlan Control on Port=%s\n", BCM_PORT_NAME(unit, port));
        for (i = 0; typenames[i].name != NULL; i++) {
            tname = typenames[i].name;
            if (subcmd == NULL || sal_strcasecmp(subcmd, tname) == 0) {
                matched += 1;
                ttype = typenames[i].type;
                if (value == NULL) {
                    r = bcm_vlan_control_port_get(unit, port, ttype, &varg);
                    if (r < 0) {
                        cli_out("%-30s-\t%s\n", tname, bcm_errmsg(r));
                    } else {
                        cli_out("%-30s%d\n", tname, varg);
                    }
                } else {
                    varg = parse_integer(value);
                    r = bcm_vlan_control_port_set(unit, port, ttype, varg);
                    if (r < 0) {
                        cli_out("%s\tERROR: %s\n", tname, bcm_errmsg(r));
                    }
                }
            }
        }
    }

    if (matched == 0) {
        cli_out("%s: ERROR: Unknown port control name\n", subcmd);
        return CMD_FAIL;
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_vlan_action(int unit, args_t *a)
{
    char *subcmd;

    subcmd = ARG_GET(a);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd, "port") == 0) {
        return _vlan_action_port(unit, a);
    }
    if (sal_strcasecmp(subcmd, "translate") == 0) {
        return _vlan_action_translate(unit, a);
    }

    return CMD_USAGE;
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief VLAN cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
cmd_result_t
cmd_ltsw_vlan(int unit, args_t *arg)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "create")) {
        return cmd_ltsw_vlan_create(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "destroy")) {
        return cmd_ltsw_vlan_destroy(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "clear")) {
        return cmd_ltsw_vlan_destroy_all(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "add")) {
        return cmd_ltsw_vlan_port_add(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "remove")) {
        return cmd_ltsw_vlan_port_remove(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "show")) {
        return cmd_ltsw_vlan_show(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "gport")) {
        return cmd_ltsw_vlan_gport(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "control") ||
               !sal_strcasecmp(subcmd, "ctrl")) {
        return cmd_ltsw_vlan_control(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "stat")) {
        return cmd_ltsw_vlan_stat(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "default")) {
        return cmd_ltsw_vlan_default(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "translate")) {
        return cmd_ltsw_vlan_translate(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "port")) {
        return cmd_ltsw_vlan_port(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "action")) {
        return cmd_ltsw_vlan_action(unit, arg);
    } else if (isint(subcmd)) {
        /* Set per Vlan property (Must be the last)*/
        vlan_id_t id = VLAN_ID_INVALID;
        id = parse_integer(subcmd);
        return cmd_ltsw_vlan_property_set(unit, id, arg);
    } else {
        return CMD_NOTIMPL;
    }
    return CMD_OK;
}

cmd_result_t
cmd_ltsw_pvlan(int u, args_t *a)
{
    char *subcmd, *argpbm, *argvid;
    vlan_id_t vid = BCM_VLAN_INVALID;
    soc_port_t port;
    int rv;
    bcm_pbmp_t          pbmp_port_all;
    bcm_pbmp_t pbm;

    UNSUPPORTED_COMMAND(u, SOC_CHIP_BCM5670, a);

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    if ((rv = bcmi_ltsw_port_bitmap(u,
              BCMI_LTSW_PORT_TYPE_ALL, &pbmp_port_all)) < 0) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        subcmd = "show";
    }

    if ((argpbm = ARG_GET(a)) == NULL) {
        BCM_PBMP_ASSIGN(pbm, pbmp_port_all);
    } else {
        if (parse_bcm_pbmp(u, argpbm, &pbm) < 0) {
            cli_out("%s: ERROR: unrecognized port bitmap: %s\n",
                    ARG_CMD(a), argpbm);
            return CMD_FAIL;
        }

        BCM_PBMP_AND(pbm, pbmp_port_all);
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        rv = BCM_E_NONE;

        /* coverity[overrun-local] */
        BCM_PBMP_ITER(pbm, port) {
        if ((rv = bcm_port_untagged_vlan_get(u, port, &vid)) < 0) {
                cli_out("Error retrieving info for port %s: %s\n",
                        BCM_PORT_NAME(u, port), bcm_errmsg(rv));
            break;
        }

        cli_out("Port %s default VLAN is %d\n",
                    BCM_PORT_NAME(u, port), vid);
        }

        return (rv < 0) ? CMD_FAIL : CMD_OK;
    } else if (sal_strcasecmp(subcmd, "set") == 0) {
        if ((argvid = ARG_GET(a)) == NULL) {
            cli_out("Missing VID for set.\n");
            return CMD_USAGE;
        }

        vid = sal_ctoi(argvid, 0);
    } else {
        return CMD_USAGE;
    }

    /* Set default VLAN as indicated */

    rv = BCM_E_NONE;

    BCM_PBMP_ITER(pbm, port) {
        if ((rv = bcm_port_untagged_vlan_set(u, port, vid)) < 0) {
            cli_out("Error setting port %s default VLAN to %d: %s\n",
                        BCM_PORT_NAME(u, port), vid, bcm_errmsg(rv));

            if ((rv == BCM_E_NOT_FOUND) ||
                (rv == BCM_E_CONFIG)) {
                cli_out("VLAN %d must be created and contain the ports "
                            "before being used for port default VLAN.\n", vid);
            }

             break;
        }
    }

    return CMD_OK;
}

#endif /* BCM_LTSW_SUPPORT */
