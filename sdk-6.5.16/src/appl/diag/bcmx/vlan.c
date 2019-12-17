/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        vlan.c
 * Purpose:
 * Requires:    
 */


#include <bcm/vlan.h>

#include <bcm/custom.h>
#include <bcm/error.h>
#include <bcmx/vlan.h>
#include <bcmx/bcmx.h>

#include <sal/appl/io.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include "bcmx.h"
#include <appl/diag/diag.h>

char bcmx_cmd_vlan_usage[] =
    "Usages:  \n"
    "    vlan {create | destroy} <id>    - Create/destroy a VLAN\n"
    "    vlan add <id> <uport-list> [<untagged-uport-list>]\n"
    "                                    - Add port(s) to a VLAN\n"
    "    vlan remove <id> <uport-list>   - Remove ports from a VLAN\n"
    "    vlan show [<id>]                - Display VLAN info\n"
    "    vlan clear                      - Destroy all VLANs\n"
    "    vlan default [<id>]             - Set/show default VLAN\n";

typedef enum _vlan_op_e {
    _VO_CREATE,
    _VO_DESTROY,
    _VO_ADD,
    _VO_REMOVE,
    _VO_CLEAR,
    _VO_SHOW,
    _VO_CUSTOM_SHOW,
    _VO_DEFAULT,
    _VO_NONE
} _vlan_op_t;

/*
 * VLAN id represented as bit field in an array
 * of 32-bit words
 */
#define VID_VEC_WORDS (4096/32)

_vlan_op_t
get_op(char *subcmd)
{
    if (sal_strcasecmp(subcmd, "create") == 0) {
        return _VO_CREATE;
    }
    if (sal_strcasecmp(subcmd, "destroy") == 0) {
        return _VO_DESTROY;
    }
    if (sal_strcasecmp(subcmd, "add") == 0) {
        return _VO_ADD;
    }
    if (sal_strcasecmp(subcmd, "remove") == 0) {
        return _VO_REMOVE;
    }
    if (sal_strcasecmp(subcmd, "clear") == 0) {
        return _VO_CLEAR;
    }
    if (sal_strcasecmp(subcmd, "show") == 0) {
        return _VO_SHOW;
    }
    if (sal_strcasecmp(subcmd, "custom_show") == 0) {
        return _VO_CUSTOM_SHOW;
    }
    if (sal_strcasecmp(subcmd, "default") == 0) {
        return _VO_DEFAULT;
    }

    return _VO_NONE;
}

#define _VID_BAD 0xffff

#define _CLEAN_UP_RETURN(rv) do { \
    bcmx_lplist_free(&ports); \
   return (rv); \
} while (0)

#define _IF_BAD_VID_RETURN(vid) if (vid == _VID_BAD) { \
    sal_printf("VLAN:  Bad vid or not specified\n"); \
    _CLEAN_UP_RETURN(CMD_USAGE); \
}

#define _IF_BAD_PL_RETURN(pl_given) if (!pl_given) { \
    sal_printf("VLAN:  Bad port list or not specified\n"); \
    _CLEAN_UP_RETURN(CMD_USAGE); \
}

int
display_vlan_lplist(int unit, int vec_count, uint32 vid_vec[VID_VEC_WORDS]) 
{
    int i, j, vid, vid_count = 0;
    int rv = BCM_E_NONE;
    int lport, count = 0;
    bcmx_lplist_t t_ports, ut_ports;

    if (bcmx_lplist_init(&t_ports, 0, 0) < 0) {
        sal_printf("Could not init tagged port list\n");
        return rv; 
    }

    if (bcmx_lplist_init(&ut_ports, 0, 0) < 0) {
        sal_printf("Could not init untagged port list\n");
        bcmx_lplist_free(&t_ports);
        return rv;
    }

    for (i=0; i < VID_VEC_WORDS; i++) {
        for (j=0; j<32; j++) {
            if (vid_vec[i] & (1 << j)) {
                vid = 32*i + j;
                vid_count++;

                sal_printf("\nVLAN %d: ", vid);

                /*
                 * Get tagged and untagged lplist
                 */
                 
                rv = bcmx_vlan_port_get(vid, &t_ports, &ut_ports);
                bcmx_lplist_sort(&t_ports);
                bcmx_lplist_sort(&ut_ports);
                sal_printf("\n\tTagged Uports: ");
                if (!(BCMX_LPLIST_IS_EMPTY(&t_ports))) {  
                    count = BCMX_LPLIST_COUNT(&t_ports);
                    BCMX_LPLIST_ITER(t_ports, lport, count) {
                       sal_printf("%s ", bcmx_lport_to_uport_str(lport));
                    }
                } else {
                    sal_printf("None");
                }
                 
                sal_printf("\n\n\tUntagged Uports: ");
                if (!(BCMX_LPLIST_IS_EMPTY(&ut_ports))) {
                    count = BCMX_LPLIST_COUNT(&ut_ports);
                    BCMX_LPLIST_ITER(ut_ports, lport, count) {
                       sal_printf("%s ", bcmx_lport_to_uport_str(lport));
                    }
                } else {
                    sal_printf("None");
                }
                sal_printf("\n");
                if (vid_count == vec_count) {
                    return rv;
                }
            } /* if vlan is valid */
        } /* for 32 bits */
    } /* for 0..127 */

    if (vid_count != vec_count) {
        return -1;
    }
        
    return rv;
}

cmd_result_t
bcmx_cmd_vlan(int unit, args_t *args)
{
    char *subcmd, *ch;
    bcmx_lplist_t ports;
    int rv = BCM_E_NONE;
    cmd_result_t cmd_rv = CMD_OK;
    bcm_vlan_t id;
    int portlist_given = 1;
    int count;
    _vlan_op_t op;
    uint32 vid_vector[VID_VEC_WORDS];

    if ((subcmd = ARG_GET(args)) == NULL) {
        sal_printf("Subcommand required\n");
        return CMD_USAGE;
    }

    if ((ch = ARG_GET(args)) == NULL) {
        id = _VID_BAD;
    } else {
        id = parse_integer(ch);
    }

    if (bcmx_lplist_init(&ports, 0, 0) < 0) {
        sal_printf("Could not init port list\n");
        return CMD_FAIL;
    }

    if ((ch = ARG_GET(args)) == NULL) {
        portlist_given = 0;
    } else {
        if (bcmx_lplist_parse(&ports, ch) < 0) {
            sal_printf("%s: Error: could not parse portlist: %s\n",
                         ARG_CMD(args), ch);
            _CLEAN_UP_RETURN(CMD_FAIL);
        }
    }

    op = get_op(subcmd);
    switch (op) {
    case _VO_CREATE:
        _IF_BAD_VID_RETURN(id);

        rv = bcmx_vlan_create(id);

        /* Add option to add ports here */
        break;

    case _VO_DESTROY:
        _IF_BAD_VID_RETURN(id);
        rv = bcmx_vlan_destroy(id);
        break;

    case _VO_ADD:
    {
        bcmx_lplist_t ut_ports;

        _IF_BAD_VID_RETURN(id);
        _IF_BAD_PL_RETURN(portlist_given);

        if (bcmx_lplist_init(&ut_ports, 0, 0) != BCM_E_NONE) {
            sal_printf("VLAN: Could not allocate untagged port list\n");
            _CLEAN_UP_RETURN(CMD_FAIL);
        }

        if ((ch = ARG_GET(args)) != NULL) {
            if (bcmx_lplist_parse(&ut_ports, ch) < 0) {
                sal_printf("%s: Error: could not parse untagged "
                             "portlist: %s\n", ARG_CMD(args), ch);
                bcmx_lplist_free(&ut_ports);
                _CLEAN_UP_RETURN(CMD_USAGE);
            }
        }

        rv = bcmx_vlan_port_add(id, ports, ut_ports);
        if (rv != BCM_E_NONE) {
            sal_printf("VLAN: Add failed %d: %s\n", rv, bcm_errmsg(rv));
            break;
        }

        bcmx_lplist_free(&ut_ports);
        break;
    }

    case _VO_REMOVE:
        _IF_BAD_VID_RETURN(id);
        rv = bcmx_vlan_port_remove(id, ports);
        if (rv != BCM_E_NONE) {
            sal_printf("VLAN: Remove failed %d: %s\n", rv, bcm_errmsg(rv));
        }
        break;

    case _VO_CUSTOM_SHOW:
    {
        int bcm_unit, i, j, count = 0;
        uint32 cu_args[BCM_CUSTOM_ARGS_MAX];
        int actual;

        sal_memset(vid_vector, 0, VID_VEC_WORDS *sizeof(uint32));
        sal_memset(cu_args, 0, BCM_CUSTOM_ARGS_MAX *sizeof(uint32));

        BCMX_UNIT_ITER(bcm_unit, i) {
          rv = bcm_custom_port_get(bcm_unit, -1,
                                   CUSTOM_HANDLER_VLAN_GET,
                                   BCM_CUSTOM_ARGS_MAX,
                                   cu_args, &actual);  
          if (rv < 0) {
              sal_printf("\nVLAN: vlan vector fetch failed at unit %d", bcm_unit);
              continue;
          } 
          
          /*
           * Some devices may not have VLANs configured;
           * OR them with the existing vlan vector for each unit
           */
          for (j = 0; j < VID_VEC_WORDS; j++) {
              vid_vector[j] |= cu_args[j+1];
          }

          if (count < cu_args[0]) {
              count =  cu_args[0];
          }
        }

        rv = display_vlan_lplist(bcm_unit, count, vid_vector);
        if (rv < 0) {
            sal_printf("\nVLAN: failure in displaying active VLANs\n");
            return CMD_FAIL;
        }
        break;
    }

    case _VO_SHOW:
    {
        bcmx_lplist_t t_ports;
        bcmx_lplist_t ut_ports;

        if (bcmx_lplist_init(&t_ports, 0, 0) < 0) {
            sal_printf("Could not init tagged port list\n");
            _CLEAN_UP_RETURN(CMD_FAIL);
        }

        if (bcmx_lplist_init(&ut_ports, 0, 0) < 0) {
            sal_printf("Could not init port list\n");
            bcmx_lplist_free(&t_ports);
            _CLEAN_UP_RETURN(CMD_FAIL);
        }

        if (id == _VID_BAD) {
            int vid;
            int count = 0;

            sal_printf("Current VLANS:  \n");
            for (vid = 0; vid < 4096; vid++) {
                rv = bcmx_vlan_port_get(vid, &t_ports, &ut_ports);
                BCM_IF_ERROR_RETURN(rv);
                bcmx_lplist_sort(&t_ports);
                bcmx_lplist_sort(&ut_ports);
                if (!(BCMX_LPLIST_IS_EMPTY(&t_ports))) {
                    sal_printf("%d ", vid);
                    if ((++count % 12) == 0) {
                        sal_printf("\n");
                    }
                }
            }
            sal_printf("\n");
            rv = BCM_E_NONE;
        } else {
            bcmx_lport_t lport;
            int out_count_t = 0;
            int out_count_ut = 0;

            rv = bcmx_vlan_port_get(id, &t_ports, &ut_ports);
            bcmx_lplist_sort(&t_ports);
            bcmx_lplist_sort(&ut_ports);

            if (!BCMX_LPLIST_IS_EMPTY(&t_ports)) {
                BCMX_LPLIST_ITER(t_ports, lport, count) {
                    if (!out_count_t) {
                        sal_printf("Tagged uports for vlan %d:\n", id);
                    }
                    sal_printf("%s ", bcmx_lport_to_uport_str(lport));
                    if (!(++out_count_t % 10)) {
                        sal_printf("\n");
                    }
                }
                sal_printf("\n");
            }

            if (!BCMX_LPLIST_IS_EMPTY(&ut_ports)) {
                BCMX_LPLIST_ITER(ut_ports, lport, count) {
                    if (!out_count_ut) {
                        sal_printf("Untagged uports for vlan %d:\n",
                                   id);
                    }
                    sal_printf("%s ", bcmx_lport_to_uport_str(lport));
                    if (!(++out_count_ut % 10)) {
                        sal_printf("\n");
                    }
                }
                sal_printf("\n");
            }

            if (!out_count_t && !out_count_ut) {
                if (rv < 0) {
                    sal_printf("VLAN port get returns error %d: %s\n",
                               rv, bcm_errmsg(rv));
                    cmd_rv = CMD_FAIL;
                    rv = 0;
                } else {
                    sal_printf("VLAN %d contains no ports\n", id);
                }
            } else if (rv < 0) {
                sal_printf("NOTE: VLAN get failed on some units (%d): %s\n",
                           rv, bcm_errmsg(rv));
                rv = 0;
            }
        }
        bcmx_lplist_free(&t_ports);
        bcmx_lplist_free(&ut_ports);
        break;
    }

    case _VO_CLEAR:
        rv = bcmx_vlan_destroy_all();
        break;

    case _VO_DEFAULT:
	if (id == _VID_BAD) {
	    rv = bcmx_vlan_default_get(&id);
	    if (rv >= 0) {
		sal_printf("VLAN default is %d\n", id);
	    }
	} else {
	    rv = bcmx_vlan_default_set(id);
	}
	break;
    case _VO_NONE:
    default:
        sal_printf("VLAN: Unknown subcommand %s\n", subcmd);
        cmd_rv = CMD_FAIL;
        break;
    }  /* End of sub-operation */

    if (rv < 0) {
        sal_printf("Error: Command returned %s\n", bcm_errmsg(rv));
        cmd_rv = CMD_FAIL;
    }

    bcmx_lplist_free(&ports);
    return cmd_rv;
}

