/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * VLAN CLI commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/l3.h>
#include <bcm/debug.h>
#include <bcm_int/esw/port.h>


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
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int  policer;
#endif
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
    { "PortOuterPri",  bcmVlanTranslateKeyPortOuterPri  }
};

STATIC void 
_ivlan_action_set_t_init(ivlan_action_set_t *data)
{
    if (data != NULL) {
        sal_memset(data, 0, sizeof(ivlan_action_set_t));
    }
}

STATIC INLINE void
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
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    parse_table_add(pt, "Policer",     PQ_HEX, 0, 
                    &(act->policer), NULL);   
#endif
}

/*
 * Return bcm_vlan_action_t action from action name string
 */
STATIC int
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
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    cli_out("  Policer=0x%x\n", action->policer_id);
#endif
}

STATIC int 
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

STATIC cmd_result_t
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
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    action->policer_id = act->policer;
#endif
    return CMD_OK;
}

STATIC cmd_result_t
_vlan_action_port(int unit, args_t *a)
{
    char       *subcmd;
    int         egress = 0;
 
    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd, "egress") == 0) {
        if ((subcmd = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        egress = 1;
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
            
            if (egress == 0) {
                VLAN_IF_ERROR_RETURN(
                    bcm_vlan_port_default_action_set(unit, port, &action));
            } else {
                VLAN_IF_ERROR_RETURN(
                    bcm_vlan_port_egress_default_action_set(unit, port, 
                                                            &action));
            }
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

            if (egress == 0) {
                VLAN_IF_ERROR_RETURN(
                    bcm_vlan_port_default_action_get(unit, port, &action));
            } else {
                VLAN_IF_ERROR_RETURN(
                    bcm_vlan_port_egress_default_action_get(unit, port, 
                                                            &action));
            }
            cli_out("Port %d: New OVLAN=%d, New IVLAN=%d, New Prio=%d\n",
                    port, action.new_outer_vlan, action.new_inner_vlan,
                    action.priority);
            _vlan_action_set_print(&action);
            return CMD_OK;
        }
    } 

    return CMD_USAGE;
}

STATIC cmd_result_t
_vlan_action_mac(int unit, args_t *a)
{
    char       *subcmd;
 
    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_t       pt;
        bcm_mac_t           mac;
        ivlan_action_set_t  act;
        cmd_result_t        r;
        bcm_vlan_action_set_t action;            
        
        _ivlan_action_set_t_init(&act);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress", PQ_MAC, 0, &mac, NULL);
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
            cli_out("Error: Invalid action\n");
            return r;
        }
        VLAN_IF_ERROR_RETURN(bcm_vlan_mac_action_add(unit, mac, &action));
        return CMD_OK;

    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        parse_table_t       pt;
        bcm_mac_t           mac;
        cmd_result_t        r;
        bcm_vlan_action_set_t action;            

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress", PQ_MAC, 0, &mac, NULL);
        if (!parseEndOk( a, &pt, &r)) {
            return r;
        }
        VLAN_IF_ERROR_RETURN(bcm_vlan_mac_action_get(unit, mac, &action));
        cli_out("MAC=%02x:%02x:%02x:%02x:%02x:%02x\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        _vlan_action_set_print(&action);
        return CMD_OK;

    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        parse_table_t       pt;
        bcm_mac_t           mac;
        cmd_result_t        r;
        
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress", PQ_MAC, 0, &mac, NULL);
        if (!parseEndOk( a, &pt, &r)) {
            return r;
        }
        VLAN_IF_ERROR_RETURN(bcm_vlan_mac_action_delete(unit, mac));
        return CMD_OK;

    } else if (sal_strcasecmp(subcmd, "clear") == 0) {
        VLAN_IF_ERROR_RETURN(bcm_vlan_mac_action_delete_all(unit));
        return CMD_OK;
    } 

    return CMD_USAGE;
}
    
STATIC cmd_result_t
_vlan_action_protocol(int unit, args_t *a)
{
    char       *subcmd;
 
    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_t       pt;
        ivlan_action_set_t  act;
        cmd_result_t        r;
        bcm_pbmp_t          pbmp;
        bcm_port_t          port, dport;
        int                 frame, ether;
        bcm_vlan_action_set_t action;            

        _ivlan_action_set_t_init(&act);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_PBMP|PQ_BCM, 0, &pbmp, NULL);
        parse_table_add(&pt, "Frame", PQ_INT, 0, &frame, NULL);
        parse_table_add(&pt, "Ether", PQ_HEX, 0, &ether, NULL);
        _parse_table_vlan_action_set_add(&pt, &act);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(a));
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
        
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            /* convert to gport? */
            VLAN_IF_ERROR_RETURN(
                bcm_vlan_port_protocol_action_add(unit, port, frame, ether, 
                                                  &action));
        }
        return CMD_OK;
    } 

    return CMD_USAGE;
}

STATIC cmd_result_t
_vlan_action_ip(int unit, args_t *a, int ip6)
{
    char       *subcmd;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_t       pt;
        ivlan_action_set_t  act;
        cmd_result_t        r;
        bcm_vlan_ip_t       vlan_ip;
        bcm_vlan_action_set_t action;            
        
        _ivlan_action_set_t_init(&act);
        bcm_vlan_ip_t_init(&vlan_ip);
        parse_table_init(unit, &pt);
        if (ip6) {
            vlan_ip.flags = BCM_VLAN_SUBNET_IP6;
            parse_table_add(&pt, "IPaddr", PQ_IP6, 0, &vlan_ip.ip6, NULL);
            parse_table_add(&pt, "prefiX", PQ_INT, 0, &vlan_ip.prefix, NULL);
        } else {
            parse_table_add(&pt, "IPaddr", PQ_IP, 0, &vlan_ip.ip4, NULL);
            parse_table_add(&pt, "NetMask", PQ_IP, 0, &vlan_ip.mask, NULL);
        }
        _parse_table_vlan_action_set_add(&pt, &act);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(a));
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
        
        VLAN_IF_ERROR_RETURN(bcm_vlan_ip_action_add(unit, &vlan_ip, &action));
        return CMD_OK;
    } 

    return CMD_USAGE;
}


STATIC cmd_result_t
_vlan_action_translate_egress(int unit, args_t *a)
{
    char       *subcmd;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_t	    pt;
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
        parse_table_t	    pt;
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
            VLAN_IF_ERROR_RETURN(
                                 bcm_vlan_translate_egress_action_delete(unit, port_class,
                                                                         old_outer_vlan, 
                                                                         old_inner_vlan));
            return CMD_OK;
        } else { /* get */
            bcm_vlan_action_set_t action;            
            VLAN_IF_ERROR_RETURN(
                                 bcm_vlan_translate_egress_action_get(unit, port_class,
                                                                      old_outer_vlan, 
                                                                      old_inner_vlan,
                                                                      &action));
            _vlan_translate_egress_action_print(unit, port_class,
                                                old_outer_vlan,
                                                old_inner_vlan, &action, NULL);
            return CMD_OK;
        }
    } else  if (sal_strcasecmp(subcmd, "show") == 0) {
        VLAN_IF_ERROR_RETURN(
                             bcm_vlan_translate_egress_action_traverse(unit,
                                                                       _vlan_translate_egress_action_print, 
                                                                       NULL));
        return CMD_OK;
    }

    return CMD_USAGE;
}


STATIC int 
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

    if (action == NULL) {
        return CMD_FAIL;
    }

    VLAN_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, gport, &modid, &port, &trunk, &id));
    if (BCM_GPORT_IS_TRUNK(gport)) {
        cli_out("TGID=%d, ", trunk);
    } else {
        cli_out("Port=%d, Modid=%d, ", port, modid);
    }
    cli_out("OuterVlanLo=%d, OuterVlanHi=%d, InnerVlanLo=%d, "
            "InnerVlanHi=%d\n", outer_vlan_lo, outer_vlan_hi,
            inner_vlan_lo, inner_vlan_hi);
    _vlan_action_set_print(action);

    return CMD_OK;
}

STATIC cmd_result_t
_vlan_action_translate_range(int unit, args_t *a)
{
    char       *subcmd;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_t	    pt;
        cmd_result_t        r;
        bcm_gport_t         gport;
        int                 port, 
                            outer_vlan_lo = BCM_VLAN_INVALID, 
                            outer_vlan_hi = BCM_VLAN_INVALID, 
                            inner_vlan_lo = BCM_VLAN_INVALID, 
                            inner_vlan_hi = BCM_VLAN_INVALID; 
        ivlan_action_set_t  act;
        bcm_vlan_action_set_t action;            
   
        _ivlan_action_set_t_init(&act);
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
        BCM_GPORT_LOCAL_SET(gport, port);
        VLAN_IF_ERROR_RETURN(
            bcm_vlan_translate_action_range_add(unit, gport,
                                                outer_vlan_lo, 
                                                outer_vlan_hi, 
                                                inner_vlan_lo,
                                                inner_vlan_hi,
                                                &action));
        return CMD_OK;

    } else if ((sal_strcasecmp(subcmd, "delete") == 0) ||
               (sal_strcasecmp(subcmd, "get") == 0)) {
        parse_table_t	    pt;
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
        if (sal_strcasecmp(subcmd, "delete") == 0) {
            VLAN_IF_ERROR_RETURN(
                bcm_vlan_translate_action_range_delete(unit, gport,
                                                       outer_vlan_lo, 
                                                       outer_vlan_hi, 
                                                       inner_vlan_lo,
                                                       inner_vlan_hi));

            return CMD_OK;

        } else { /* get */
            bcm_vlan_action_set_t action;            
            VLAN_IF_ERROR_RETURN(
                bcm_vlan_translate_action_range_get(unit, gport,
                                                    outer_vlan_lo, 
                                                    outer_vlan_hi, 
                                                    inner_vlan_lo,
                                                    inner_vlan_hi,
                                                    &action));
            cli_out("Port=%d, OuterVlanLo=%d, OuterVlanHi=%d, InnerVlanLo=%d, "
                    "InnerVlanHi=%d\n", port, outer_vlan_lo, outer_vlan_hi,
                    inner_vlan_lo, inner_vlan_hi);
            _vlan_action_set_print(&action);
            return CMD_OK;
        }
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

STATIC int
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
    return (key_type >= names ? vlan_key_type_names[0].name : 
            vlan_key_type_names[key_type].name);
}

STATIC int 
_vlan_translate_action_print(int unit, bcm_gport_t gport, 
                             bcm_vlan_translate_key_t key_type, 
                             bcm_vlan_t outer_vlan, 
                             bcm_vlan_t inner_vlan, 
                             bcm_vlan_action_set_t *action, 
                             void *user_data)
{
    int             id;
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     trunk;

    if (action == NULL) {
        return CMD_FAIL;
    }

    if (key_type >= bcmVlanTranslateKeyPortDouble) {
        /* Port information only valid with Port+VID keys */
        VLAN_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, gport, &modid, &port, &trunk, &id));
        if (BCM_GPORT_IS_TRUNK(gport)) {
            cli_out("TGID=%d, ", trunk);
        } else {
            cli_out("Port=%d, Modid=%d, ", port, modid);
        }
    }
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


STATIC cmd_result_t
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


    if (sal_strcasecmp(subcmd, "add") == 0) {
        parse_table_t       pt;
        cmd_result_t        r;
        int                 old_outer_vlan, old_inner_vlan, port = 0; 
        bcm_gport_t         gport;
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
        
        BCM_GPORT_LOCAL_SET(gport, port);
        VLAN_IF_ERROR_RETURN(
            bcm_vlan_translate_action_add(unit, gport, key_type, 
                                          old_outer_vlan,
                                          old_inner_vlan, &action));
        return CMD_OK;

    } else if ((sal_strcasecmp(subcmd, "delete") == 0) ||
               (sal_strcasecmp(subcmd, "get") == 0)) {
        parse_table_t       pt;
        int                 old_outer_vlan, old_inner_vlan, port = 0; 
        char               *key_str = NULL;
        bcm_gport_t         gport;
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
        parse_arg_eq_done(&pt);
        if (key_type == bcmVlanTranslateKeyInvalid) {
            cli_out("Error: invalid key\n");
            return CMD_FAIL;
        }
        BCM_GPORT_LOCAL_SET(gport, port);
        if (sal_strcasecmp(subcmd, "delete") == 0) {
            VLAN_IF_ERROR_RETURN(
                bcm_vlan_translate_action_delete(unit, gport, key_type, 
                                                 old_outer_vlan,
                                                 old_inner_vlan));
            return CMD_OK; 

        } else {
            bcm_vlan_action_set_t action;            
            VLAN_IF_ERROR_RETURN(
                bcm_vlan_translate_action_get(unit, gport, key_type, 
                                              old_outer_vlan,
                                              old_inner_vlan,
                                              &action));
            cli_out("Port=%d, KeyType=%s, OldOuterVlan=%d, OldInnerVlan=%d\n",
                    port, key_str, old_outer_vlan, old_inner_vlan);
            _vlan_action_set_print(&action);
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


cmd_result_t
if_esw_pvlan(int u, args_t *a)
{
    char *subcmd, *argpbm, *argvid;
    vlan_id_t vid = BCM_VLAN_INVALID;
    soc_port_t port, dport;
    int rv;
    bcm_pbmp_t          pbmp_port_all;
    bcm_pbmp_t pbm;

    UNSUPPORTED_COMMAND(u, SOC_CHIP_BCM5670, a);

    if (!sh_check_attached(ARG_CMD(a), u)) {
	return CMD_FAIL;
    }

    BCM_PBMP_ASSIGN(pbmp_port_all, PBMP_PORT_ALL(u));

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
        DPORT_BCM_PBMP_ITER(u, pbm, dport, port) {
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

    DPORT_BCM_PBMP_ITER(u, pbm, dport, port) {
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



STATIC int 
_vlan_translate_print(int unit, bcm_port_t gport, bcm_vlan_t old_vid,
                      bcm_vlan_t new_vid, int prio, void *user_data)
{
    int             rv, id;
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     trunk;

    rv = _bcm_esw_gport_resolve(unit, gport, &modid, &port, &trunk, &id);
    if (BCM_FAILURE(rv)) {
        cli_out("ERROR: Invalid gport =0x%x\n", gport);
        return CMD_FAIL;
    }
    if (BCM_GPORT_IS_TRUNK(gport)) {
        cli_out("u = %d, tgid = %d, old_vid = %d, new_vid = %d, prio = %d \n",
                unit, trunk, old_vid, new_vid, prio);
    } else {
        cli_out("u = %d, modid = %d port = %d, old_vid = %d, new_vid = %d, prio = %d \n",
                unit, modid, port, old_vid, new_vid, prio);
    }

    return CMD_OK;
}

STATIC int 
_vlan_translate_range_print(int unit, bcm_port_t gport, bcm_vlan_t old_vid_lo,
                            bcm_vlan_t old_vid_hi, bcm_vlan_t new_vid, 
                            int prio, void *user_data)
{
    int             rv, id;
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     trunk;

    rv = _bcm_esw_gport_resolve(unit, gport, &modid, &port, &trunk, &id);
    if (BCM_FAILURE(rv)) {
        cli_out("ERROR: Invalid gport =0x%x\n", gport);
        return CMD_FAIL;
    }
    if (BCM_GPORT_IS_TRUNK(gport)) {
        cli_out("u = %d, tgid = %d, old_vid_lo = %d, old_vid_hi = %d, new_vid = %d, prio = %d \n",
                unit, trunk, old_vid_lo, old_vid_hi, new_vid, prio);
    } else {
        cli_out("u = %d, modid = %d port = %d, old_vid_lo = %d, old_vid_hi = %d, new_vid = %d, prio = %d \n",
                unit, modid, port, old_vid_lo, old_vid_hi, new_vid, prio);
    }

    return CMD_OK;
}


cmd_result_t
if_esw_vlan(int unit, args_t *a)
{
    char		*subcmd, *c;
    int			is_untagged = 0;
    int                 r = 0;
    vlan_id_t		id = VLAN_ID_INVALID;
    bcm_pbmp_t		arg_ubmp;
    bcm_pbmp_t		arg_pbmp;
    bcm_pbmp_t          pbmp_port_all;
    parse_table_t	pt;
    cmd_result_t	retCode;
    bcm_gport_t         gport = BCM_GPORT_INVALID;
    static char
        *bcm_vlan_mcast_flood_str[] = BCM_VLAN_MCAST_FLOOD_STR;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_policer_t       policer;
#endif

    BCM_PBMP_CLEAR(arg_ubmp);
    BCM_PBMP_CLEAR(arg_pbmp);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    /* This was previously a call to bcm_port_config_get, but which was
     * used both to verify the port module init and to get this
     * port bitmap.  But the port config now has 18 pbmp's in it,
     * which is just too many to toss on the stack.
     */
    BCM_PBMP_ASSIGN(pbmp_port_all, PBMP_PORT_ALL(unit));

    if ((subcmd = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "create") == 0) {
	if ((c = ARG_GET(a)) == NULL) {
	    return CMD_USAGE;
        }

        if (!isint(c)) { 
            cli_out("ERROR: Malformed VLAN ID \"%s\"\n", c); 
            return CMD_FAIL; 
        } 

	id = parse_integer(c);

	parse_table_init(unit, &pt);
	parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &arg_pbmp, NULL);
	parse_table_add(&pt, "UntagBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
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
    }

    if (sal_strcasecmp(subcmd, "destroy") == 0) {
	if ((c = ARG_GET(a)) == NULL)
	    return CMD_USAGE;

	id = parse_integer(c);

	if ((r = bcm_vlan_destroy(unit, id)) < 0)
	    goto bcm_err;

	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "clear") == 0) {
	if ((r = bcm_vlan_destroy_all(unit)) < 0) {
	    goto bcm_err;
	}

	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "add") == 0 ||
	sal_strcasecmp(subcmd, "remove") == 0) {
	if ((c = ARG_GET(a)) == NULL) {
	    return CMD_USAGE;
	}

	id = parse_integer(c);

	parse_table_init(unit, &pt);
	parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &arg_pbmp, NULL);
	if (sal_strcasecmp(subcmd, "add") == 0) {
	    parse_table_add(&pt, "UntagBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
			    (void *)(0), &arg_ubmp, NULL);
	}

	if (!parseEndOk( a, &pt, &retCode)) {
	    return retCode;
	}

	if (sal_strcasecmp(subcmd, "remove") == 0) {
	    if ((r = bcm_vlan_port_remove(unit, id, arg_pbmp)) < 0) {
		goto bcm_err;
	    }
	} else {
	    if ((r = bcm_vlan_port_add(unit, id, arg_pbmp, arg_ubmp)) < 0) {
		goto bcm_err;
	    }
	}

	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "gport") == 0) {
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
    }

    if (sal_strcasecmp(subcmd, "MulticastFlood") == 0) {
        bcm_vlan_mcast_flood_t  mode;
	if ((c = ARG_GET(a)) == NULL) {
	    return CMD_USAGE;
	}

	id = parse_integer(c);
	if ((c = ARG_GET(a)) == NULL) {
	    if ((r = bcm_vlan_mcast_flood_get(unit, id, &mode)) < 0) {
		goto bcm_err;
            }
            cli_out("vlan %d Multicast Flood Mode is %s\n",
                    id, bcm_vlan_mcast_flood_str[mode]);
            return CMD_OK;
	}
	mode = parse_integer(c);
        if ((r = bcm_vlan_mcast_flood_set(unit, id, mode)) < 0) {
            goto bcm_err;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
	bcm_vlan_data_t	*list;
	int		count, i, alloc_bufp, alloc_pfmtp;
	char		*bufp = NULL;
	char		*pfmtp = NULL;

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
            goto bcm_err;
        }

	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "default") == 0) {
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
    }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (sal_strcasecmp(subcmd, "policer") == 0) { 
	subcmd = ARG_GET(a);
	if (subcmd == NULL) {
	    cli_out("%s: ERROR: missing policer subcommand\n", ARG_CMD(a));
	    return CMD_FAIL;
	}

	if (sal_strcasecmp(subcmd, "set") == 0) {
	    if ((c = ARG_GET(a)) != NULL) {
	        id = parse_integer(c);
	    } else {
                return CMD_FAIL;
            }
	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "POLICER", PQ_HEX, 0, &policer, NULL);
	    if (!parseEndOk( a, &pt, &retCode)) {
                cli_out("Usages: vlan policer set <id> POLICER=<policer>\n");
		return retCode;
	    }
            r = bcm_vlan_policer_set(unit, id, policer);
            if( 0 == r) {
                cli_out(" Policer id=%x is set to Vlan %x\n", policer,id);
            } else {
                cli_out(" Policer id=%x set to Vlan %x failed\n", policer,id);
            }
	    return CMD_OK;
	}
	if (sal_strcasecmp(subcmd, "get") == 0) {
	    if ((c = ARG_GET(a)) != NULL) {
	        id = parse_integer(c);
	    } else {
                return CMD_FAIL;
            }
            r = bcm_vlan_policer_get(unit, id, &policer);
            if(0 == r) {
                cli_out(" Policer id=%x is set in Vlan %x\n", policer,id);
            } else {
                cli_out(" Get Policer from Vlan %x failed\n", id);
            }
	    return CMD_OK;
	}

	cli_out("%s: ERROR: unknown policer subcommand: %s\n",
                ARG_CMD(a), subcmd);

	return CMD_FAIL;
    }


#endif

    /* Protocol vlan selection */
    if (sal_strcasecmp(subcmd, "protocol") == 0 ||
	sal_strcasecmp(subcmd, "proto") == 0) {

	subcmd = ARG_GET(a);
	if (subcmd == NULL) {
	    cli_out("%s: ERROR: missing protocol subcommand\n", ARG_CMD(a));
	    return CMD_FAIL;
	}

	if (sal_strcasecmp(subcmd, "add") == 0) {
	    bcm_pbmp_t	pbmp;
	    bcm_port_t	port, dport;
	    int		frame, ether, vlan, prio;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "PortBitMap", PQ_PBMP|PQ_BCM, 0, &pbmp, NULL);
	    parse_table_add(&pt, "Frame", PQ_INT, 0, &frame, NULL);
	    parse_table_add(&pt, "Ether", PQ_HEX, 0, &ether, NULL);
	    parse_table_add(&pt, "VLan", PQ_INT, 0, &vlan, NULL);
	    parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
	    if (!parseEndOk( a, &pt, &retCode)) {
		return retCode;
	    }

            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		if ((r = bcm_port_protocol_vlan_add(unit, port, frame,
						    ether, vlan)) < 0) {
		    goto bcm_err;
		}
	    }
	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "delete") == 0) {
	    bcm_pbmp_t	pbmp;
	    bcm_port_t	port, dport;
	    int		frame, ether;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "PortBitMap", PQ_PBMP|PQ_BCM, 0, &pbmp, NULL);
	    parse_table_add(&pt, "Frame", PQ_INT, 0, &frame, NULL);
	    parse_table_add(&pt, "Ether", PQ_HEX, 0, &ether, NULL);
	    if (!parseEndOk( a, &pt, &retCode)) {
		return retCode;
	    }

            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		if ((r = bcm_port_protocol_vlan_delete(unit, port, frame,
						       ether)) < 0) {
		    goto bcm_err;
		}
	    }
	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "clear") == 0) {
	    bcm_pbmp_t	pbmp;
	    bcm_port_t	port, dport;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "PortBitMap", PQ_PBMP|PQ_BCM, 0, &pbmp, NULL);
	    if (!parseEndOk( a, &pt, &retCode)) {
                cli_out("Usages: vlan protocol clear PortBitMap=<pbmp>\n");
		return retCode;
	    }

            /* coverity[overrun-local] */
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		if ((r = bcm_port_protocol_vlan_delete_all(unit,
							   port)) < 0) {
		    goto bcm_err;
		}
	    }
	    return CMD_OK;
	}

	cli_out("%s: ERROR: unknown protocol subcommand: %s\n",
                ARG_CMD(a), subcmd);

	return CMD_FAIL;
    }

    /* MAC address vlan selection */
    if (sal_strcasecmp(subcmd, "mac") == 0) {
	subcmd = ARG_GET(a);
	if (subcmd == NULL) {
	    cli_out("%s: ERROR: missing mac subcommand\n", ARG_CMD(a));
	    return CMD_FAIL;
	}

	if (sal_strcasecmp(subcmd, "add") == 0) {
	    bcm_mac_t	mac;
	    int		vlan, prio, cng;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "MACaddress", PQ_MAC, 0, &mac, NULL);
	    parse_table_add(&pt, "VLan", PQ_INT, 0, &vlan, NULL);
	    parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
	    parse_table_add(&pt, "Cng", PQ_INT, 0, &cng, NULL);

	    if (!parseEndOk( a, &pt, &retCode)) {
		return retCode;
	    }

	    if (cng) {
		prio |= BCM_PRIO_DROP_FIRST;
	    }

	    if ((r = bcm_vlan_mac_add(unit, mac, vlan, prio)) < 0) {
		goto bcm_err;
	    }

	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "delete") == 0) {
	    bcm_mac_t	mac;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "MACaddress", PQ_MAC, 0, &mac, NULL);

	    if (!parseEndOk( a, &pt, &retCode)) {
		return retCode;
	    }

	    if ((r = bcm_vlan_mac_delete(unit, mac)) < 0) {
		goto bcm_err;
	    }

	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "clear") == 0) {
	    if ((r = bcm_vlan_mac_delete_all(unit)) < 0) {
		goto bcm_err;
	    }

	    return CMD_OK;
	}

	cli_out("%s: ERROR: Unknown MAC subcommand: %s\n", ARG_CMD(a), subcmd);

	return CMD_FAIL;
    }

    /* VLAN translate selection */
    if (sal_strcasecmp(subcmd, "translate") == 0) {
	subcmd = ARG_GET(a);
	if (subcmd == NULL) {
	    cli_out("%s: ERROR: Missing translate subcommand\n", ARG_CMD(a));
	    return CMD_FAIL;
	}

	if (sal_strcasecmp(subcmd, "On") == 0) {
	    if ((r = bcm_vlan_control_set(unit, bcmVlanTranslate,
					  TRUE)) < 0) {
		goto bcm_err;
	    }

	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "Off") == 0) {
	    if ((r = bcm_vlan_control_set(unit, bcmVlanTranslate,
					  FALSE)) < 0) {
		goto bcm_err;
	    }

	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "add") == 0) {
        int port = 0, old_vlan = 1, new_vlan = 1, prio = 0, cng = 0;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
	    parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);
	    parse_table_add(&pt, "NewVLan", PQ_INT, 0, &new_vlan, NULL);
	    parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
	    parse_table_add(&pt, "Cng", PQ_INT, 0, &cng, NULL);

	    if (!parseEndOk( a, &pt, &retCode)) {
		return retCode;
	    }

	    if (cng) {
		prio |= BCM_PRIO_DROP_FIRST;
	    }

	    if ((r = bcm_vlan_translate_add(unit, port,
					    old_vlan, new_vlan, prio)) < 0) {
		goto bcm_err;
	    }

	    return CMD_OK;
	}

        if (sal_strcasecmp(subcmd, "get") == 0) {
            int		port = 0, prio = 0;
            int     old_vlan = 1;
            bcm_vlan_t new_vlan = 1;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
            parse_table_add(&pt, "OldVlan", PQ_INT, 0, &old_vlan, NULL);
            
            if (!parseEndOk( a, &pt, &retCode)) {
                return retCode;
            }
            
            if (!BCM_VLAN_VALID(old_vlan)) {
                goto bcm_err;
            }
            if ((r = bcm_vlan_translate_get(unit, port,
                                            (bcm_vlan_t)old_vlan, 
                                            &new_vlan, 
                                            &prio)) < 0) {
                goto bcm_err;
            }
            
            cli_out("New Vlan ID = %d, Priority = %d\n", new_vlan, prio);
            return CMD_OK;
        }
        
        if (sal_strcasecmp(subcmd, "show") == 0) {
            
            parse_table_init(unit, &pt);
            
            if ((r = bcm_vlan_translate_traverse(unit, _vlan_translate_print, 
                                                 NULL)) < 0) {
                goto bcm_err;
            }
            return CMD_OK;
        }
        
	if (sal_strcasecmp(subcmd, "delete") == 0) {
        int port = 0, old_vlan = 1;
            
	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
	    parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);
            
	    if (!parseEndOk( a, &pt, &retCode)) {
		return retCode;
	    }
            
	    if ((r = bcm_vlan_translate_delete(unit, port, old_vlan)) < 0) {
		goto bcm_err;
	    }
            
	    return CMD_OK;
	}
        
	if (sal_strcasecmp(subcmd, "clear") == 0) {
	    if ((r = bcm_vlan_translate_delete_all(unit)) < 0) {
		goto bcm_err;
	    }
            
	    return CMD_OK;
	}
        
        if (sal_strcasecmp(subcmd, "egress") == 0) {
            char * subsubcmd = ARG_GET(a);
            if (subsubcmd == NULL) {
                cli_out("%s: ERROR: Missing translate egress subcommand\n",
                        ARG_CMD(a));
                return CMD_FAIL;
            }
            
            if (sal_strcasecmp(subsubcmd, "add") == 0) {
                int port = 0, old_vlan = 1, new_vlan = 1, prio = 0, cng = 0;
                
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);
                parse_table_add(&pt, "NewVLan", PQ_INT, 0, &new_vlan, NULL);
                parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
                parse_table_add(&pt, "Cng", PQ_INT, 0, &cng, NULL);
                
                if (!parseEndOk( a, &pt, &retCode)) {
                    return retCode;
                }
                
                if (cng) {
                    prio |= BCM_PRIO_DROP_FIRST;
                }
                
                if ((r = bcm_vlan_translate_egress_add(unit, port,
                                                       old_vlan, new_vlan, prio)) < 0) {
                    goto bcm_err;
                }
                
                return CMD_OK;
            }
            if (sal_strcasecmp(subsubcmd, "get") == 0) {
                int         port = 0, prio = 0;
                int         old_vlan = 1;
                bcm_vlan_t  new_vlan = 1;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);

                if (!parseEndOk( a, &pt, &retCode)) {
                    return retCode;
                }

                if (!BCM_VLAN_VALID(old_vlan)) {
                    goto bcm_err;
                }
                if ((r = bcm_vlan_translate_egress_get(unit, port,
                                                       (bcm_vlan_t)old_vlan, 
                                                       &new_vlan, 
                                                       &prio)) < 0) {
                    goto bcm_err;
                }

                cli_out("New Vlan ID = %d, Priority = %d\n", new_vlan, prio);
                return CMD_OK;
            }
            
            if (sal_strcasecmp(subsubcmd, "show") == 0) {
                
                parse_table_init(unit, &pt);
                
                if ((r = bcm_vlan_translate_egress_traverse(unit,
                                                            _vlan_translate_print, 
                                                            NULL)) < 0) {
                    goto bcm_err;
                }
                return CMD_OK;
            }
            
            if (sal_strcasecmp(subsubcmd, "delete") == 0) {
                int port = 0, old_vlan = 1;
                
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);
                
                if (!parseEndOk( a, &pt, &retCode)) {
                    return retCode;
                }
                
                if ((r = bcm_vlan_translate_egress_delete(unit, port,
							  old_vlan)) < 0) {
                    goto bcm_err;
                }
                
                return CMD_OK;
            }
            
            if (sal_strcasecmp(subsubcmd, "clear") == 0) {
                if ((r = bcm_vlan_translate_egress_delete_all(unit)) < 0) {
                    goto bcm_err;
                }
                
                return CMD_OK;
            }
        }
        
        if (sal_strcasecmp(subcmd, "dtag") == 0) {
            char * subsubcmd = ARG_GET(a);
            if (subsubcmd == NULL) {
                cli_out("%s: ERROR: Missing translate dtag subcommand\n",
                        ARG_CMD(a));
                return CMD_FAIL;
            }

            if (sal_strcasecmp(subsubcmd, "add") == 0) {
                int port = 0, old_vlan = 1, new_vlan = 1, prio = 0, cng = 0;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);
                parse_table_add(&pt, "NewVLan", PQ_INT, 0, &new_vlan, NULL);
                parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
                parse_table_add(&pt, "Cng", PQ_INT, 0, &cng, NULL);

                if (!parseEndOk( a, &pt, &retCode)) {
                    return retCode;
                }

                if (cng) {
                    prio |= BCM_PRIO_DROP_FIRST;
                }

                if ((r = bcm_vlan_dtag_add(unit, port,
                                           old_vlan, new_vlan, prio)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "get") == 0) {
                int         port = 0, prio = 0;
                int         old_vlan = 1;
                bcm_vlan_t  new_vlan = 1;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);

                if (!parseEndOk( a, &pt, &retCode)) {
                    return retCode;
                }

                if (!BCM_VLAN_VALID(old_vlan)) {
                    goto bcm_err;
                }
                if ((r = bcm_vlan_dtag_get(unit, port, (bcm_vlan_t)old_vlan, 
                                           &new_vlan, &prio)) < 0) {
                    goto bcm_err;
                }

                cli_out("New Vlan ID = %d, Priority = %d\n", new_vlan, prio);
                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "show") == 0) {

                parse_table_init(unit, &pt);

                if ((r = bcm_vlan_dtag_traverse(unit, _vlan_translate_print, 
                                                NULL)) < 0) {
                    goto bcm_err;
                }
                return CMD_OK;
            }
            if (sal_strcasecmp(subsubcmd, "delete") == 0) {
                int		port=0, old_vlan=1;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLan", PQ_INT, 0, &old_vlan, NULL);

                if (!parseEndOk( a, &pt, &retCode)) {
                    return retCode;
                }

                if ((r = bcm_vlan_dtag_delete(unit, port, old_vlan)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "clear") == 0) {
                if ((r = bcm_vlan_dtag_delete_all(unit)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }
        }


        if (sal_strcasecmp(subcmd, "range") == 0) {
            char * subsubcmd = ARG_GET(a);
            if (subsubcmd == NULL) {
                cli_out("%s: ERROR: Missing translate range subcommand\n",
                        ARG_CMD(a));
                return CMD_FAIL;
            }

            if (sal_strcasecmp(subsubcmd, "add") == 0) {
                int old_vlan_lo = 1, old_vlan_hi = 1;
                int port = 0, new_vlan = 1, prio = 0, cng = 0;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLanLo", PQ_INT, 0, &old_vlan_lo, NULL);
                parse_table_add(&pt, "OldVLanHi", PQ_INT, 0, &old_vlan_hi, NULL);
                parse_table_add(&pt, "NewVLan", PQ_INT, 0, &new_vlan, NULL);
                parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
                parse_table_add(&pt, "Cng", PQ_INT, 0, &cng, NULL);

                if (!parseEndOk( a, &pt, &retCode)) {
                    return retCode;
                }

                if (cng) {
                    prio |= BCM_PRIO_DROP_FIRST;
                }

                if (!BCM_VLAN_VALID(old_vlan_lo) || 
                    !BCM_VLAN_VALID(old_vlan_hi) ||
                    !BCM_VLAN_VALID(new_vlan)) {
                    goto bcm_err;
                }

                if ((r = bcm_vlan_translate_range_add(unit, port, old_vlan_lo, 
                                                      old_vlan_hi, new_vlan, 
                                                      prio)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "get") == 0) {
                int         port = 0, prio = 0;
                int         old_vlan_lo = 1, old_vlan_hi = 1;
                bcm_vlan_t  new_vlan = 1;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLanLo", PQ_INT, 0, &old_vlan_lo, NULL);
                parse_table_add(&pt, "OldVLanHi", PQ_INT, 0, &old_vlan_hi, NULL);

                if (!parseEndOk( a, &pt, &retCode)) {
                    return retCode;
                }

                if (!BCM_VLAN_VALID(old_vlan_lo) || 
                    !BCM_VLAN_VALID(old_vlan_hi)) {
                    goto bcm_err;
                }
                if ((r = bcm_vlan_translate_range_get(unit, port, 
                                                      (bcm_vlan_t)old_vlan_lo,
                                                      (bcm_vlan_t)old_vlan_hi,
                                                      &new_vlan, &prio)) < 0) {
                    goto bcm_err;
                }

                cli_out("New Vlan ID = %d, Priority = %d\n", new_vlan, prio);
                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "show") == 0) {
                if ((r = bcm_vlan_translate_range_traverse(unit, 
                                                  _vlan_translate_range_print, 
                                                           NULL)) < 0) {
                    goto bcm_err;
                }
                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "delete") == 0) {
                int port = 0, old_vlan_lo = 1, old_vlan_hi = 1;

                parse_table_init(unit, &pt);
                parse_table_add(&pt, "Port", PQ_PORT, 0, &port, NULL);
                parse_table_add(&pt, "OldVLanLo", PQ_INT, 0, &old_vlan_lo, NULL);
                parse_table_add(&pt, "OldVLanHi", PQ_INT, 0, &old_vlan_hi, NULL);

                if (!parseEndOk( a, &pt, &retCode)) {
                    return retCode;
                }

                if (!BCM_VLAN_VALID(old_vlan_lo) || 
                    !BCM_VLAN_VALID(old_vlan_hi)) {
                    goto bcm_err;
                }

                if ((r = bcm_vlan_translate_range_delete(unit, port, old_vlan_lo, 
                                                         old_vlan_hi)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }

            if (sal_strcasecmp(subsubcmd, "clear") == 0) {
                if ((r = bcm_vlan_translate_range_delete_all(unit)) < 0) {
                    goto bcm_err;
                }

                return CMD_OK;
            }
        }

/**************************************************************************************************/

	cli_out("%s: ERROR: Unknown translate subcommand: %s\n",
                ARG_CMD(a), subcmd);

	return CMD_FAIL;
    }

    /* IP4 subnet based vlan selection */
    if (sal_strcasecmp(subcmd, "ip4") == 0 ||
	sal_strcasecmp(subcmd, "ip") == 0) {
	subcmd = ARG_GET(a);
	if (subcmd == NULL) {
	    cli_out("%s: ERROR: missing ip4 subcommand\n", ARG_CMD(a));
	    return CMD_FAIL;
	}

	if (sal_strcasecmp(subcmd, "add") == 0) {
	    bcm_ip_t	ipaddr, subnet;
	    int		vlan, prio, cng;
            bcm_vlan_ip_t vlan_ip;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "IPaddr", PQ_IP, 0, &ipaddr, NULL);
	    parse_table_add(&pt, "NetMask", PQ_IP, 0, &subnet, NULL);
	    parse_table_add(&pt, "VLan", PQ_INT, 0, &vlan, NULL);
	    parse_table_add(&pt, "Prio", PQ_INT, 0, &prio, NULL);
	    parse_table_add(&pt, "Cng", PQ_INT, 0, &cng, NULL);

	    if (!parseEndOk( a, &pt, &retCode)) {
		return retCode;
	    }
	    if (cng) {
		prio |= BCM_PRIO_DROP_FIRST;
	    }

            bcm_vlan_ip_t_init(&vlan_ip);
            vlan_ip.ip4 = ipaddr;
            vlan_ip.mask = subnet;
            vlan_ip.vid = vlan;
            vlan_ip.prio = prio;

	    if ((r = bcm_vlan_ip_add(unit, &vlan_ip)) < 0) {
		goto bcm_err;
	    }

	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "delete") == 0) {
	    bcm_ip_t	ipaddr, subnet;
            bcm_vlan_ip_t vlan_ip;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "IPaddr", PQ_IP, 0, &ipaddr, NULL);
	    parse_table_add(&pt, "NetMask", PQ_IP, 0, &subnet, NULL);

	    if (!parseEndOk( a, &pt, &retCode)) {
		return retCode;
	    }

            bcm_vlan_ip_t_init(&vlan_ip);
            vlan_ip.ip4 = ipaddr;
            vlan_ip.mask = subnet;

	    if ((r = bcm_vlan_ip_delete(unit, &vlan_ip)) < 0) {
		goto bcm_err;
	    }

	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "clear") == 0) {
	    if ((r = bcm_vlan_ip_delete_all(unit)) < 0) {
		goto bcm_err;
	    }

	    return CMD_OK;
	}

	cli_out("%s: ERROR: Unknown ip4 subcommand: %s\n", ARG_CMD(a), subcmd);

	return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "ip6") == 0) {
        subcmd = ARG_GET(a);
	if (subcmd == NULL) {
	    cli_out("%s: ERROR: missing ip6 subcommand\n", ARG_CMD(a));
	    return CMD_FAIL;
	}

	if (sal_strcasecmp(subcmd, "add") == 0) {
	    bcm_vlan_ip_t	ipaddr;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "IPaddr", PQ_IP6, 0, &ipaddr.ip6, NULL);
	    parse_table_add(&pt, "prefiX", PQ_INT, 0, &ipaddr.prefix, NULL);
	    parse_table_add(&pt, "VLan", PQ_INT, 0, &ipaddr.vid, NULL);
	    parse_table_add(&pt, "Prio", PQ_INT, 0, &ipaddr.prio, NULL);

	    if (!parseEndOk( a, &pt, &retCode)) {
		    return retCode;
	    }
        ipaddr.flags = BCM_VLAN_SUBNET_IP6;
	    if ((r = bcm_vlan_ip_add(unit, &ipaddr)) < 0) {
		    goto bcm_err;
	    }

	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "delete") == 0) {
	    bcm_vlan_ip_t	ipaddr;

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "IPaddr", PQ_IP6, 0, &ipaddr.ip6, NULL);
	    parse_table_add(&pt, "prefiX", PQ_INT, 0, &ipaddr.prefix, NULL);

	    if (!parseEndOk( a, &pt, &retCode)) {
		    return retCode;
	    }

        ipaddr.flags = BCM_VLAN_SUBNET_IP6;
	    if ((r = bcm_vlan_ip_delete(unit, &ipaddr)) < 0) {
		    goto bcm_err;
	    }

	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "clear") == 0) {
	    if ((r = bcm_vlan_ip_delete_all(unit)) < 0) {
		    goto bcm_err;
	    }

	    return CMD_OK;
	}

	cli_out("%s: ERROR: Unknown ip6 subcommand: %s\n", ARG_CMD(a), subcmd);

	return CMD_FAIL;
    }

    /* Vlan control */
    if (sal_strcasecmp(subcmd, "control") == 0 ||
	sal_strcasecmp(subcmd, "ctrl") == 0) {
	char	*value, *tname;
	int	ttype, i, varg, matched;

	static struct {			/* match enum from bcm/vlan.h */
	    int		type;
	    char	*name;
	} typenames[] = {
	    { bcmVlanDropUnknown,	"dropunknown" },
	    { bcmVlanPreferIP4,		"preferip4" },
	    { bcmVlanPreferMAC,		"prefermac" },
	    { bcmVlanShared,		"shared" },
	    { bcmVlanSharedID,		"sharedid" },
            { bcmVlanPreferEgressTranslate, "preferegresstranslate" },
	    { 0,			NULL }		/* LAST ENTRY */
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

    /* Vlan port control */
    if (sal_strcasecmp(subcmd, "port") == 0) {
	char	*value, *tname;
	int	ttype, i, varg, matched;
        bcm_port_t port, dport;
        pbmp_t       pbm;
	static struct {			/* match enum from bcm/vlan.h */
	    int		type;
	    char	*name;
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
	    { 0,			       NULL }   /* LAST ENTRY */
	};

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

        DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
            cli_out("\nVlan Control on Port=%s\n", BCM_PORT_NAME(unit, port));
	for (i = 0; typenames[i].name != NULL; i++) {
	    tname = typenames[i].name;
	    if (subcmd == NULL || sal_strcasecmp(subcmd, tname) == 0) {
		matched += 1;
		ttype = typenames[i].type;
		if (value == NULL) {
                        r = bcm_vlan_control_port_get(unit, port, 
                                                      ttype, &varg);
		    if (r < 0) {
                            cli_out("%-30s-\t%s\n", tname, bcm_errmsg(r));
		    } else {
                            cli_out("%-30s%d\n", tname, varg);
                        }  
		} else {
		    varg = parse_integer(value);
                        r = bcm_vlan_control_port_set(unit, port, 
                                                      ttype, varg);
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


    /* Vlan inner tag */
    if (sal_strcasecmp(subcmd, "innertag") == 0) {
        int          priority, cfi;
        int          vid;
        bcm_port_t   port, dport;
        uint16       innertag;
        pbmp_t       pbm;

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

        priority  = -1;
        cfi       = -1;
        vid       = -1;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Cfi", PQ_INT | PQ_DFL, 0, &cfi, NULL);
        parse_table_add(&pt, "Vlan", PQ_INT | PQ_DFL, 0, &vid, NULL);
        parse_table_add(&pt, "Priority", PQ_INT | PQ_DFL, 0, &priority, NULL);

        if (0 > parse_arg_eq(a, &pt)) {
            cli_out("%s: Error: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return (CMD_FAIL);
        }
        parse_arg_eq_done(&pt);
        
       if (priority == -1 || cfi == -1 || vid == -1) {
            /* coverity[overrun-local] */
            DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
                if ((r = bcm_port_vlan_inner_tag_get(unit, port, &innertag)) 
                                                      < 0) {
                    goto bcm_err;
                }
                cli_out("Port=%s, Priority=%d, CFI=%d, VLAN=%d\n",
                        BCM_PORT_NAME(unit, port),
                        BCM_VLAN_CTRL_PRIO(innertag),
                        BCM_VLAN_CTRL_CFI(innertag),
                        BCM_VLAN_CTRL_ID(innertag));
           }
        } else {
            innertag = BCM_VLAN_CTRL(priority, cfi, vid);

            DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
                if ((r = bcm_port_vlan_inner_tag_set(unit, port, innertag)) 
                             < 0) {
                    goto bcm_err;
                }
            }
        }
        return CMD_OK;
    }

    /* Vlan action */
    if (sal_strcasecmp(subcmd, "action") == 0) {
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
	if (sal_strcasecmp(subcmd, "mac") == 0) {
            return _vlan_action_mac(unit, a);
        } 
	if (sal_strcasecmp(subcmd, "protocol") == 0 ||
            sal_strcasecmp(subcmd, "proto") == 0) {
            return _vlan_action_protocol(unit, a);
        } 
        if (sal_strcasecmp(subcmd, "ip4") == 0 ||
            sal_strcasecmp(subcmd, "ip") == 0) {
            return _vlan_action_ip(unit, a, 0);
        } 
        if (sal_strcasecmp(subcmd, "ip6") == 0) {
            return _vlan_action_ip(unit, a, BCM_VLAN_SUBNET_IP6);
        } 
        return CMD_USAGE;
    }


    /* Set per Vlan property (Must be the last)*/
    {
        bcm_vlan_control_vlan_t	vlan_control, default_control;
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

        id = parse_integer(subcmd);


	if (VLAN_ID_VALID(id)) {
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
                            &def_ip4_mcast_l2_disable, &ip4_mcast_l2_disable, NULL);
            parse_table_add(&pt, "Ip6McastL2Disable", PQ_INT | PQ_DFL, 
                            &def_ip6_mcast_l2_disable, &ip6_mcast_l2_disable, NULL);
            parse_table_add(&pt, "L3VrfGlobalDisable", PQ_INT | PQ_DFL, 
                            &def_l3_vrf_global_disable, &l3_vrf_global_disable, NULL);
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

            if (!parseEndOk( a, &pt, &retCode)) {
                return retCode;
            }

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
            
            if ((r = bcm_vlan_control_vlan_set(unit, id, vlan_control)) < 0) {
                goto bcm_err;
            }

            return CMD_OK;
        }
    }


    return CMD_USAGE;

 bcm_err:

    cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));

    return CMD_FAIL;
}

