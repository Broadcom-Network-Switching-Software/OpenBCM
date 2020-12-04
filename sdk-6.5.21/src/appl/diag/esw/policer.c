/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Service meter(global meters) CLI commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/hash.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/policer.h>
#include <bcm_int/esw/policer.h>

/*
 * SVC METER command catagory
 */
enum _svc_meter_cmd_t {
    GLOBAL_METER_MODE_CREATE = 1,
    GLOBAL_METER_MODE_DELETE,
    GLOBAL_METER_ACTION_CREATE,
    GLOBAL_METER_ACTION_DESTROY,
    GLOBAL_METER_ACTION_ADD,
    GLOBAL_METER_ACTION_GET,
    GLOBAL_METER_ACTION_ATTACH,
    GLOBAL_METER_ACTION_DETACH,
    GLOBAL_METER_ACTION_ATTACH_GET,
    GLOBAL_METER_POLICER_GROUP_CREATE,
    GLOBAL_METER_POLICER_ENVELOP_CREATE,
    GLOBAL_METER_POLICER_DESTROY,
    GLOBAL_METER_POLICER_DESTROY_ALL,
    GLOBAL_METER_POLICER_SET,
    GLOBAL_METER_POLICER_GET,
    GLOBAL_METER_POLICER_TRAVERSE
};


static struct policer_group_type_names {
    char                     *name;
    bcm_policer_group_mode_t  mode;
} policer_group_type_names[] = {
    { "Single",        bcmPolicerGroupModeSingle                    },
    { "TrafficType",   bcmPolicerGroupModeTrafficType               },
    { "DlfAll",        bcmPolicerGroupModeDlfAll                    },
    { "DlfIntPri",     bcmPolicerGroupModeDlfIntPri                 },
    { "Typed",         bcmPolicerGroupModeTyped                     },
    { "TypedAll",      bcmPolicerGroupModeTypedAll                  },
    { "TypedIntPri",   bcmPolicerGroupModeTypedIntPri               },
    { "SingleWithControl", bcmPolicerGroupModeSingleWithControl     },
    { "TrafficTypeWithControl", bcmPolicerGroupModeTrafficTypeWithControl},
    { "DlfAllWithControl",      bcmPolicerGroupModeDlfAllWithControl     },
    { "DlfIntPriWithControl",   bcmPolicerGroupModeDlfIntPriWithControl  },
    { "TypedWithControl",       bcmPolicerGroupModeTypedWithControl      },
    { "TypedAllWithControl",    bcmPolicerGroupModeTypedAllWithControl   },
    { "TypedIntPriWithControl", bcmPolicerGroupModeTypedIntPriWithControl},
    { "Dot1P",                  bcmPolicerGroupModeDot1P                 },
    { "IntPri",                 bcmPolicerGroupModeIntPri                },
    { "IntPriCng",              bcmPolicerGroupModeIntPriCng             },
    { "SvpType",                bcmPolicerGroupModeSvpType               },
    { "Dscp",                   bcmPolicerGroupModeDscp                  },
    { "Cascade",                bcmPolicerGroupModeCascade               },
    { "CascadeWithCoupling",    bcmPolicerGroupModeCascadeWithCoupling   },
    { "InnerDot1P",             bcmPolicerGroupModeInnerDot1P            },
    { "IntPriCascade",          bcmPolicerGroupModeIntPriCascade         },
    { "IntPriCascadeWithCoupling", bcmPolicerGroupModeIntPriCascadeWithCoupling}
};

static struct policer_type_names {
    char                     *name;
    bcm_policer_mode_t       mode;
} policer_type_names[] = {
    { "SrTcm",                bcmPolicerModeSrTcm               },
    { "Committed",            bcmPolicerModeCommitted           }, 
    { "Peak",                 bcmPolicerModePeak                }, 
    { "TrTcm",                bcmPolicerModeTrTcm               },
    { "TrTcmDs",              bcmPolicerModeTrTcmDs             },          
    { "Green",                bcmPolicerModeGreen               }, 
    { "PassThrough",          bcmPolicerModePassThrough         }, 
    { "SrTcmModified",        bcmPolicerModeSrTcmModified       }, 
    { "CoupledTrTcmDs",       bcmPolicerModeCoupledTrTcmDs      }, 
    { "Cascade",              bcmPolicerModeCascade             }, 
    { "CoupledCascade",       bcmPolicerModeCoupledCascade      },
    { "SrTcmTsn",             bcmPolicerModeSrTcmTsn            }
};

static struct policer_action_names {
    char                     *name;
    bcm_policer_action_t     action;
} policer_action_names[] = {
    { "GpDrop",                bcmPolicerActionGpDrop            },
    { "GpDscpNew",             bcmPolicerActionGpDscpNew         },     
    { "GpEcnNew",              bcmPolicerActionGpEcnNew          },       
    { "GpPrioIntNew",          bcmPolicerActionGpPrioIntNew      },   
    { "GpCngNew",              bcmPolicerActionGpCngNew          },       
    { "GpVlanPrioNew",         bcmPolicerActionGpVlanPrioNew     },  
    { "YpDrop",                bcmPolicerActionYpDrop            },         
    { "YpDscpNew",             bcmPolicerActionYpDscpNew         },      
    { "YpEcnNew",              bcmPolicerActionYpEcnNew          },       
    { "YpPrioIntNew",          bcmPolicerActionYpPrioIntNew      },   
    { "YpCngNew",              bcmPolicerActionYpCngNew          },       
    { "YpVlanPrioNew",         bcmPolicerActionYpVlanPrioNew     },  
    { "RpDrop",                bcmPolicerActionRpDrop            },         
    { "RpDscpNew",             bcmPolicerActionRpDscpNew         },      
    { "RpEcnNew",              bcmPolicerActionRpEcnNew          },       
    { "RpPrioIntNew",          bcmPolicerActionRpPrioIntNew      },   
    { "RpCngNew",              bcmPolicerActionRpCngNew          },      
    { "RpVlanPrioNew",         bcmPolicerActionRpVlanPrioNew     }
};

static int _bcm_esw_handle_policer_commands(int unit, args_t *a);   

char cmd_esw_policer_global_meter_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  GlobalMeter [args...]\n"
#else
    "  GlobalMeter Policer CreateGroup <GROUP>=<group_name>\n\t"
    "              - Create global meter policers with given group\n\t"
    "  GlobalMeter Policer CreateEnvelop <Flag>=<1/2> \n\t" 
    "              -           <MacroMeterPolicer> = <policer>\n\t"
    "              - Create micro/macro meter policer \n\t"
    "  GlobalMeter Policer Destroy <POLICER>=<policer_id>\n\t"
    "              - Destroy global meter policers given base policer id \n\t"
    "  GlobalMeter Policer DestroyAll \n\t"
    "              - Destroy all global meter policers \n\t"
    "  GlobalMeter Policer Set <POLICER>=<policer_id> \n\t"
    "              -           <POLICER_TYPE>=<policer_type> <FLAG>=<flag> \n\t"
    "              -           [<CommittedRate>=<committed_rate>]\n\t"
    "              -           [<CommittedBurst>=<committed_burst_rate>]\n\t"
    "              -           [<PeakRate>=<Peak_rate>]\n\t"
    "              -           [<PeakBurst>=<Peak_burst_rate>]\n\t"
    "              -           [<ActionId>=<action_id>]\n\t"
    "              -           [<SharingMode>=<sharing_mode>]\n\t"
    "              - Set parameters of the policer \n\t"
    "  GlobalMeter Policer Get <POLICER>=<policer_id> \n\t"
    "              - Get parameters of the policer \n\t"
    "  GlobalMeter Policer Traverse \n\t"
    "              - Traverse through all the policers \n\t"
    "  GlobalMeter Policer Action ATtach <POLICER>=<policer_id> \n\t"
    "              -                     <ACTION_ID>=<action_id> \n\t"
    "              - Attach policer action to policer specified \n\t"
    "  GlobalMeter Policer Action Detach <POLICER>=<policer_id> \n\t"
    "              - Detach policer action from policer specified \n\t"
    "  GlobalMeter Policer Action AttachGet <POLICER>=<policer_id> \n\t"
    "              - Get the policer action index associated with policer \n\t"
    "  GlobalMeter Policer Action Create \n\t"
    "              - Create an  policer action entry \n\t"
    "  GlobalMeter Policer Action Destroy <ACTION_ID>=<action_id> \n\t"
    "              - Destroy an policer action entry \n\t"
    "  GlobalMeter Policer Action Add <ACTION_ID>=<action_id><ACTION>=<action> \n\t"
    "              -           [<PARAM>=<action_param>]\n\t"
    "              - Add an action to policer action entry specified \n\t"
    "  GlobalMeter Policer Action Get <ACTION_ID>=<action_id> <ACTION>=<action> \n\t"
    "              - Get parameter associated with an action entry action \n\t"
#endif
    ;

STATIC int
_bcm_esw_policer_traverse_print(int unit, bcm_policer_t policer_id,
                                    bcm_policer_config_t *cfg,
                                    void *user_data)
{
    cli_out("Policer with id %x configuration params are \n",policer_id);
    cli_out("Policer Type =%d \n",cfg->mode);
    cli_out("Flag =%d \n",cfg->flags);
    cli_out("Committed rate =%d \n",cfg->ckbits_sec);
    cli_out("Committed Burst =%d \n",cfg->ckbits_burst);
    cli_out("Peak rate =%d \n",cfg->pkbits_sec);
    cli_out("Peak Burst =%d \n",cfg->pkbits_burst);
    cli_out("Action ID =%d \n",cfg->action_id);
    cli_out("Sharing Mode =%d \n",cfg->sharing_mode);

    return CMD_OK;
}

STATIC int
_policer_group_mode_get(const char *mode_str, bcm_policer_group_mode_t *mode)
{
    int  i, names;
    names = sizeof(policer_group_type_names) /  \
                        sizeof(struct policer_group_type_names);
    /* Get bcm_policer_group_mode from mode type string */
    for (i = 0; i < names; i++) {
        if (mode_str &&
            (sal_strcasecmp(mode_str, policer_group_type_names[i].name) == 0)) {
            *mode = policer_group_type_names[i].mode;
            return BCM_E_NONE;
        }
    }
    cli_out("Invalid group mode type <%s>. Valid key types are:\n   ",
            mode_str ? mode_str : "");
    for (i = 0;  i < names; i++) {
        cli_out("%s ", policer_group_type_names[i].name);
        if (i % 7 == 0) {
            cli_out("\n   ");
        }
    }
    cli_out("\n");
    return BCM_E_PARAM;
}

STATIC int
_policer_mode_get(const char *mode_str, bcm_policer_mode_t *mode)
{
    int  i, names;
    names = sizeof(policer_type_names) /  \
                        sizeof(struct policer_type_names);
    /* Get bcm_policer_mode from mode type string */
    for (i = 0; i < names; i++) {
        if (mode_str &&
            (sal_strcasecmp(mode_str, policer_type_names[i].name) == 0)) {
            *mode = policer_type_names[i].mode;
            return BCM_E_NONE;
        }
    }
    cli_out("Invalid group mode type <%s>. Valid key types are:\n   ",
            mode_str ? mode_str : "");
    for (i = 0;  i < names; i++) {
        cli_out("%s ", policer_type_names[i].name);
        if (i % 7 == 0) {
            cli_out("\n   ");
        }
    }
    cli_out("\n");
    return BCM_E_PARAM;
}

static INLINE int
_policer_mode_name_get(bcm_policer_mode_t key_type, char *mode_str)
{
    int names = sizeof(policer_type_names) / sizeof(struct policer_type_names);
    if (key_type >= names) {
        return BCM_E_INTERNAL;
    }

    /* coverity[secure_coding] */
    strcpy(mode_str, policer_type_names[key_type].name);
    return BCM_E_NONE;
}

STATIC int
_policer_action_get(const char *action_str, bcm_policer_action_t *action)
{
    int  i, names;
    names = sizeof(policer_action_names) /  \
                        sizeof(struct policer_action_names);
    /* Get bcm_policer_group_mode from mode type string */
    for (i = 0; i < names; i++) {
        if (action_str &&
            (sal_strcasecmp(action_str, policer_action_names[i].name) == 0)) {
            *action = policer_action_names[i].action;
            return BCM_E_NONE;
        }
    }
    cli_out("Invalid action type <%s>. Valid types are:\n   ",
            action_str ? action_str : "");
    for (i = 0;  i < names; i++) {
        cli_out("%s ", policer_action_names[i].name);
        if (i % 7 == 0) {
            cli_out("\n   ");
        }
    }
    cli_out("\n");
    return BCM_E_PARAM;
}

cmd_result_t
cmd_esw_policer_global_meter(int unit, args_t *a)
{
    char *cmd;

    /* Check valid device to operation on ...*/
    if (! sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if ((cmd = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    if (sal_strcasecmp(cmd, "policer") == 0) {
        return(_bcm_esw_handle_policer_commands(unit, a));   
    } else {
	return CMD_USAGE;
    }
}

static int _bcm_esw_handle_policer_action_commands(int unit, args_t *a)
{
    char *subcmd_s;
    int subcmd = 0;
    int rv=0;
    parse_table_t   pt;
    cmd_result_t    retCode;
    uint32 action_id;
    bcm_policer_t policer_id;
    char *action_str = NULL;
    bcm_policer_action_t action;
    uint32 param=0;

    if ((subcmd_s = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd_s, "create") == 0) {
        subcmd = GLOBAL_METER_ACTION_CREATE;
    }
    if (sal_strcasecmp(subcmd_s, "destroy") == 0) {
        subcmd = GLOBAL_METER_ACTION_DESTROY;
    }
    if (sal_strcasecmp(subcmd_s, "add") == 0) {
        subcmd = GLOBAL_METER_ACTION_ADD;
    }
    if (sal_strcasecmp(subcmd_s, "get") == 0) {
        subcmd = GLOBAL_METER_ACTION_GET;
    }
    if (sal_strcasecmp(subcmd_s, "attach") == 0) {
        subcmd = GLOBAL_METER_ACTION_ATTACH;
    }
    if (sal_strcasecmp(subcmd_s, "detach") == 0) {
        subcmd = GLOBAL_METER_ACTION_DETACH;
    }
    if (sal_strcasecmp(subcmd_s, "attachget") == 0) {
        subcmd = GLOBAL_METER_ACTION_ATTACH_GET;
    }

    parse_table_init(unit, &pt);
    switch (subcmd) {
	case GLOBAL_METER_ACTION_CREATE:
            rv = bcm_policer_action_create(unit, &action_id); 
            if (rv == BCM_E_NONE) {
                cli_out("Created METER ACtion with ID %x\n",action_id);
            } else {
                cli_out("%s Creation of METER action failed with error \
                        %s\n",ARG_CMD(a),bcm_errmsg(rv));
            }
            break;

	case GLOBAL_METER_ACTION_DESTROY:
            parse_table_add(&pt, "ACTION_ID", PQ_HEX, 0, &action_id, 0);
	    if (!parseEndOk(a, &pt, &retCode)) {
	        return retCode;
            }
            rv = bcm_policer_action_destroy(unit, action_id); 
            if (rv == BCM_E_NONE) {
                cli_out("Destroy METER ACtion with ID %x SUCCESS\n",action_id);
            } else {
                cli_out("%s Destroy of METER action failed with error \
                        %s\n",ARG_CMD(a),bcm_errmsg(rv));
            }
            break;

	case GLOBAL_METER_ACTION_ADD:
            parse_table_add(&pt, "ACTION_ID", PQ_HEX, 0, &action_id, 0);
            parse_table_add(&pt, "ACTION", PQ_STRING, 0, &action_str, NULL);
            parse_table_add(&pt, "PARAM", PQ_INT, 0, &param, 0);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("Error: invalid option %s\n", ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if (action_str != NULL) {
                rv = _policer_action_get(action_str, &action);
                if (rv != BCM_E_NONE) {
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
            } else {
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            } 
            rv = bcm_policer_action_add(unit, action_id, action, param); 
            if (rv == BCM_E_NONE) {
                cli_out("Added action %s to action id %x\n",action_str,action_id);
            } else {
                cli_out("%s Addition of action to action_id %x failed with error \
                        %s\n",ARG_CMD(a),action_id,bcm_errmsg(rv));
            }
            parse_arg_eq_done(&pt);
            break;

	case GLOBAL_METER_ACTION_GET:
            parse_table_add(&pt, "ACTION_ID", PQ_HEX, 0, &action_id, 0);
            parse_table_add(&pt, "ACTION", PQ_STRING, 0, &action_str, NULL);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("Error: invalid option %s\n", ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if(action_str != NULL) {
                rv = _policer_action_get(action_str, &action);
                if (rv != BCM_E_NONE) {
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
            } else {
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            rv = bcm_policer_action_get(unit, action_id, action, &param); 
            if (rv == BCM_E_NONE) {
                cli_out("Param for Action %s of action_id %x is %d SUCCESS\n", 
                        action_str, action_id, param);
            } else {
                cli_out("%s  Action get failed with error \
                        %s\n",ARG_CMD(a),bcm_errmsg(rv));
            }
            parse_arg_eq_done(&pt);
            break;

	case GLOBAL_METER_ACTION_ATTACH:
            parse_table_add(&pt, "POLICER", PQ_HEX, 0, &policer_id, 0);
            parse_table_add(&pt, "ACTION_ID", PQ_HEX, 0, &action_id, 0);
	    if (!parseEndOk( a, &pt, &retCode)) {
	        return retCode;
            }
            rv = bcm_policer_action_attach(unit, policer_id, action_id); 
            if (rv == BCM_E_NONE) {
                cli_out("Attach action id %x to policer %x SUCCESS\n", 
                        action_id, policer_id);
            } else {
                cli_out("%s Attach action_id %x failed with error \
                        %s\n",ARG_CMD(a),action_id,bcm_errmsg(rv));
            }
            break;

	case GLOBAL_METER_ACTION_DETACH:
            parse_table_add(&pt, "POLICER", PQ_HEX, 0, &policer_id, 0);
            parse_table_add(&pt, "ACTION_ID", PQ_HEX, 0, &action_id, 0);
	    if (!parseEndOk( a, &pt, &retCode)) {
	        return retCode;
            }
            rv = bcm_policer_action_detach(unit, policer_id, action_id); 
            if (rv == BCM_E_NONE) {
                cli_out("Detach action id %x from policer %x SUCCESS\n",
                        action_id, policer_id);
            } else {
                cli_out("%s detach action_id %x failed with error \
                        %s\n", ARG_CMD(a), action_id, bcm_errmsg(rv));
            }
	    break;

	case GLOBAL_METER_ACTION_ATTACH_GET:
            parse_table_add(&pt, "POLICER", PQ_HEX, 0, &policer_id, 0);
	    if (!parseEndOk( a, &pt, &retCode)) {
	        return retCode;
            }
            rv = bcm_policer_action_attach_get(unit, policer_id, &action_id); 
            if (rv == BCM_E_NONE) {
                cli_out("Action id associated with policer id %x is %x \n",
                        policer_id, action_id);
            } else {
                cli_out("%s action Attach get failed with error \
                        %s\n",ARG_CMD(a),bcm_errmsg(rv));
            }
	    break;

	default:
	    return CMD_USAGE;
	    break;
    }
    return CMD_OK;
}

static int _bcm_esw_handle_policer_commands(int unit, args_t *a)   
{
    char *subcmd_s;
    int subcmd = 0;
    char *mode_str = NULL;
    char mode_name[20];
    bcm_policer_group_mode_t mode = 0;
    bcm_policer_t policer_id = 0;
    bcm_policer_t macro_meter_policer = 0;
    int flag = 0;
    int numbers = 0;
    int rv = 0;
    parse_table_t   pt;
    cmd_result_t    retCode;

    bcm_policer_config_t pol_cfg;

    if ((subcmd_s = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd_s, "action") == 0) {
        return(_bcm_esw_handle_policer_action_commands(unit, a));
    }
    if (sal_strcasecmp(subcmd_s, "creategroup") == 0) {
        subcmd = GLOBAL_METER_POLICER_GROUP_CREATE;

    }
    if (sal_strcasecmp(subcmd_s, "CreateEnvelop") == 0) {
        subcmd = GLOBAL_METER_POLICER_ENVELOP_CREATE;
    }

    if (sal_strcasecmp(subcmd_s, "set") == 0) {
        subcmd = GLOBAL_METER_POLICER_SET;

    }
    if (sal_strcasecmp(subcmd_s, "get") == 0) {
        subcmd = GLOBAL_METER_POLICER_GET;
    }
    if (sal_strcasecmp(subcmd_s, "Destroy") == 0) {
        subcmd = GLOBAL_METER_POLICER_DESTROY;
    }
    if (sal_strcasecmp(subcmd_s, "DestroyAll") == 0) {
        subcmd = GLOBAL_METER_POLICER_DESTROY_ALL;
    }
    if (sal_strcasecmp(subcmd_s, "Traverse") == 0) {
        subcmd = GLOBAL_METER_POLICER_TRAVERSE;
    }
    parse_table_init(unit, &pt);

    switch (subcmd) {
	case GLOBAL_METER_POLICER_GROUP_CREATE:
            parse_table_add(&pt, "GROUP", PQ_STRING, 0, &mode_str, NULL);
            parse_table_add(&pt, "NUM", PQ_INT, 0, &numbers, 0);

            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("Error: invalid option %s\n", ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if (mode_str != NULL) {
                rv = _policer_group_mode_get(mode_str, &mode);
                if (rv != BCM_E_NONE) {
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
            } 
            parse_arg_eq_done(&pt);
            rv = bcm_policer_group_create(unit, mode, &policer_id, &numbers); 
            if (rv == BCM_E_NONE) {
                cli_out("Created policer group with %d policers and Base policer \
                        ID %x\n",numbers,policer_id);
            } else {
                cli_out("%s Creation of policer group failed with error \
                        %s\n",ARG_CMD(a),bcm_errmsg(rv));
            }
            break;

	case GLOBAL_METER_POLICER_ENVELOP_CREATE:
            parse_table_add(&pt, "Flag", PQ_INT, 0, &flag, 0);
            parse_table_add(&pt, "MacroMeterPolicer", PQ_INT, 0, &macro_meter_policer, 0);
	    if (!parseEndOk( a, &pt, &retCode)) {
	        return retCode;
            }
            rv = bcm_policer_envelop_create(unit, flag, macro_meter_policer, &policer_id); 
            if (rv == BCM_E_NONE) {
                cli_out("Created Envelop policer of type %d with ID %x\n", flag, policer_id);
            } else {
                cli_out("%s Creation of envelop policer falied with error \
                        %s\n",ARG_CMD(a),bcm_errmsg(rv));
            }
            break;

	case GLOBAL_METER_POLICER_DESTROY:
            parse_table_add(&pt, "POLICER", PQ_HEX, 0, &policer_id, 0);
	    if (!parseEndOk( a, &pt, &retCode)) {
	        return retCode;
            }
            rv = bcm_policer_destroy(unit, policer_id); 
            if (rv == BCM_E_NONE) {
                cli_out("Destroy policer group with Base policer \
                        ID %x SUCCESS\n",policer_id);
            } else {
                cli_out("%s Destroy policer group failed with error \
                        %s\n",ARG_CMD(a),bcm_errmsg(rv));
            }
            break;

	case GLOBAL_METER_POLICER_DESTROY_ALL:
            rv = bcm_policer_destroy_all(unit); 
            if (rv == BCM_E_NONE) {
                cli_out("Destroy ALL policer SUCCESS\n");
            } else {
                cli_out("%s Destroy ALL Failed with error \
                        %s\n",ARG_CMD(a),bcm_errmsg(rv));
            }
            break;

	case GLOBAL_METER_POLICER_SET:
            parse_table_add(&pt, "POLICER", PQ_HEX, 0, &policer_id, 0);
            parse_table_add(&pt, "POLICER_TYPE", PQ_STRING, 0, &mode_str, NULL);
            parse_table_add(&pt, "FLAG", PQ_INT, 0, &(pol_cfg.flags), 0);
            parse_table_add(&pt, "CommittedRate", 
                                          PQ_INT, 0, &(pol_cfg.ckbits_sec), 0);
            parse_table_add(&pt, "CommittedBurst", 
                                        PQ_INT, 0, &(pol_cfg.ckbits_burst), 0);
            parse_table_add(&pt, "PeakRate", 
                                          PQ_INT, 0, &(pol_cfg.pkbits_sec), 0);
            parse_table_add(&pt, "PeakBurst", 
                                        PQ_INT, 0, &(pol_cfg.pkbits_burst), 0);
            parse_table_add(&pt, "ACTION_ID", 
                                           PQ_INT, 0, &(pol_cfg.action_id), 0);
            parse_table_add(&pt, "SharingMode", 
                                        PQ_INT, 0, &(pol_cfg.sharing_mode), 0);

            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("Error: invalid option %s\n", ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if(mode_str != NULL) {
                rv = _policer_mode_get(mode_str, &(pol_cfg.mode));
                if (rv != BCM_E_NONE) {
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
            } 
            parse_arg_eq_done(&pt);
            rv = bcm_policer_set(unit, policer_id, &pol_cfg); 
            if (rv == BCM_E_NONE) {
                cli_out("Configured policer with id %x \n",policer_id);
            } else {
                cli_out("%s Configure policer failed with error \
                        %s\n",ARG_CMD(a),bcm_errmsg(rv));
            }
	    break;

	case GLOBAL_METER_POLICER_GET:
            parse_table_add(&pt, "POLICER", PQ_HEX, 0, &policer_id, 0);
	    if (!parseEndOk(a, &pt, &retCode)) {
	        return retCode;
            }

            rv = bcm_policer_get(unit, policer_id, &pol_cfg); 
            if (rv == BCM_E_NONE) {
                cli_out("Policer with id %x configuration params are \n",
                        policer_id);
                rv = _policer_mode_name_get(pol_cfg.mode, &mode_name[0]);
                if (rv == BCM_E_NONE) {
                    cli_out("Policer Type =%s \n",mode_name);
                }
                cli_out("Flag =%d \n", pol_cfg.flags);
                cli_out("Committed rate =%d \n", pol_cfg.ckbits_sec);
                cli_out("Committed Burst =%d \n", pol_cfg.ckbits_burst);
                cli_out("Peak rate =%d \n", pol_cfg.pkbits_sec);
                cli_out("Peak Burst =%d \n", pol_cfg.pkbits_burst);
                cli_out("Action ID =%d \n", pol_cfg.action_id);
                cli_out("Sharing Mode =%d \n", pol_cfg.sharing_mode);
            } else {
                cli_out("%s Configure policer failed with error \
                        %s\n",ARG_CMD(a), bcm_errmsg(rv));
            }
            break;

	case GLOBAL_METER_POLICER_TRAVERSE:
            rv = bcm_policer_traverse(unit, 
                                    _bcm_esw_policer_traverse_print, NULL); 
            if (rv == BCM_E_NONE) {
                cli_out("policer traverse SUCCESS\n");
            } else {
                cli_out("%s Destroy ALL Failed with error \
                        %s\n",ARG_CMD(a), bcm_errmsg(rv));
            }
            break;

	default:
	    return CMD_USAGE;
	    break;
    }
    return CMD_OK;
}

