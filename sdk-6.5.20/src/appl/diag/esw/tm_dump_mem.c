/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diag CLI TMCheck command
 */

#include <soc/defs.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <bcm/error.h>

#include <soc/tomahawk3.h>

#ifdef __KERNEL__
#define atoi _shr_ctoi
#endif

char cmd_tm_dump_mem_usage[] =
    "tmdumpmem [-index] <port/queue> <memory_name> <port> <queue> ...\n";

STATIC int
th3_retrieve_port_mem_index (int unit, soc_mem_t mem, int port) {
    int mem_index;
    int mem_depth;


    if (mem == MMU_RQE_DEVICE_TO_MMU_PORT_MAPPINGm) {
        mem_index = port;
    } else {
        /* Index transformation for MMU memories based on port number */
        mem_depth = soc_mem_index_max(unit, mem) + 1;
        if (mem_depth == SOC_TH3_MAX_DEV_PORTS_PER_PIPE) {
            /* Mem indexed by local MMU port number */
            mem_index = SOC_TH3_MMU_LOCAL_PORT(unit, port);
        } else if (mem_depth == SOC_TH3_MAX_NUM_PORTS) {
            /* MMU mem indexed by MMU chip port number */
            mem_index = SOC_TH3_MMU_CHIP_PORT(unit, port);
        } else {
            mem_index = -1;
        }
    }
    return mem_index;
}


STATIC int
th3_dump_port_based_mem (int unit, char* memory_name, int port, int print_index)
{
    soc_mem_t *mem_list;
    int mem_tot = 1, mem_idx, memory_index;
    int copyno;
    soc_mem_t mem = INVALIDm;
    int block_info_index;
    int rv;
    uint32    entry[SOC_MAX_MEM_WORDS];

    /* Reserving array of length 64 for max combinations of EB/IPIPE in TH3 */
    mem_list = sal_alloc(64 * sizeof(soc_mem_t), "List of MMU logical views");
    if (mem_list == NULL) {
        return SOC_E_MEMORY;
    }

    /* Checking validity of passed memory */
    if (parse_memory_name(unit, &mem, memory_name, &copyno, 0) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n", memory_name);
        sal_free(mem_list);
        return SOC_E_PARAM;
    }
    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if ((SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_ITM) &&
        (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_EB) &&
        (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_GLB)) {
        /* index transformation not applicable for non-MMU memories */
        cli_out("ERROR: Command used for non-MMU memory \"%s\"\n", memory_name);
        sal_free(mem_list);
        return SOC_E_PARAM;
    }


    if (copyno == COPYNO_ALL) {
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }


    /* Expand based on port only if it is per-port memory */
    rv = soc_th3_retrieve_mmu_mem_list_for_port(unit, port, mem, -1,
                            mem_list, 64, &mem_tot);
    if (rv) {
        mem_list[0] = mem;
    }
    memory_index = th3_retrieve_port_mem_index(unit, mem_list[0], port);
    if (memory_index == -1) {
        sal_free(mem_list);
        cli_out("Memory: \"%s\" is not port based \n", memory_name);
        return SOC_E_PARAM;
    }
    for(mem_idx = 0; mem_idx < mem_tot; mem_idx++) {
        mem = mem_list[mem_idx];
        if (mem != INVALIDm) {
            if (!print_index) {
                cli_out("%s.%s[%d]: ",
                        SOC_MEM_UFNAME(unit, mem),
                        SOC_BLOCK_NAME(unit, copyno),
                        memory_index);
                soc_mem_entry_dump(unit, mem, entry, BSL_LSS_CLI);
            } else {
                cli_out("Memory: %s index: %d\n", SOC_MEM_UFNAME(unit, mem),
                        memory_index);
            }
        }
    }
    sal_free(mem_list);
    return SOC_E_NONE;
}

STATIC int
th3_dump_queue_based_mem (int unit, char* memory_name, int port, int queue,
        int print_index)
{
    soc_mem_t *mem_list;
    int mem_tot = 1, mem_idx;
    int copyno;
    soc_mem_t mem = INVALIDm;
    int block_info_index;
    int rv;
    uint32    entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);
    int mem_index;
    int mem_depth;

    /* Reserving array of length 64 for max combinations of EB/IPIPE in TH3 */
    mem_list = sal_alloc(64 * sizeof(soc_mem_t), "List of MMU logical views");
    if (mem_list == NULL) {
        return SOC_E_MEMORY;
    }

    /* Checking validity of passed memory */
    if (parse_memory_name(unit, &mem, memory_name, &copyno, 0) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n", memory_name);
        sal_free(mem_list);
        return SOC_E_PARAM;
    }
    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if ((SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_ITM) &&
        (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_EB)) {
        /* index transformation not applicable for non-MMU memories */
        cli_out("ERROR: Command used for unknown Queue based Memory \"%s\"\n",
                memory_name);
        sal_free(mem_list);
        return SOC_E_PARAM;
    }
    if (copyno == COPYNO_ALL) {
	    copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }

    /* Expand based on port number for queu based memories */
    rv = soc_th3_retrieve_mmu_mem_list_for_port(unit, port, mem, -1,
                        mem_list, 64, &mem_tot);
    if (rv) {
        mem_list[0] = mem;
    }

    for(mem_idx = 0; mem_idx < mem_tot; mem_idx++) {
        mem = mem_list[mem_idx];
        mem_depth = soc_mem_index_max(unit, mem) + 1;

        /*
         * Queue based memories in ITM have chip_q_number as index
         * Memory depths vary as 1920, 1968, & 2080
         * Queue memories which are pipe based have local queue number as index
         * Memory depth for pipe-local queue memories is 276
         */
        if (mem_depth == 1920 || mem_depth == 1968 || mem_depth == 2080) {
            /* Memories indexed by chip_q_number (global) */
            mem_index = si->port_uc_cosq_base[port] + queue;
        } else if (mem_depth == SOC_TH3_NUM_COS_QUEUES_PER_PIPE) {
            /* Memories indexed by pipe local Queue number */
            mem_index = (si->port_l2i_mapping[port] * SOC_TH3_NUM_GP_QUEUES)
                + queue;
        } else {
            /* Access meant for non-queue memory */
            cli_out("ERROR: Command used for unknown Queue based Memory: memory"
                    "depth not mtaching\"%s\"\n",
                memory_name);
            sal_free(mem_list);
            return SOC_E_PARAM;
        }
        if (mem != INVALIDm) {
            if (!print_index) {
                cli_out("%s.%s[%d]: ",
                        SOC_MEM_UFNAME(unit, mem),
                        SOC_BLOCK_NAME(unit, copyno),
                        mem_index);
                soc_mem_entry_dump(unit, mem, entry, BSL_LSS_CLI);
            } else {
                cli_out("Memory: %s index: %d\n", SOC_MEM_UFNAME(unit, mem),
                        mem_index);
            }
        }
    }
    sal_free(mem_list);
    return SOC_E_NONE;
}
cmd_result_t
cmd_tm_dump_mem(int unit, args_t *arg)
{
    char *index_type, *memory_name, *idx_param1, *idx_param2;
    int port;
    int rv = 0;
    int queue, user_q;
    int only_index = 0;
    soc_info_t *si = &SOC_INFO(unit);

    index_type = ARG_GET(arg);
    if (index_type == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(index_type, "-index")) {
        only_index = 1;
        index_type = ARG_GET(arg);
    }

    memory_name = ARG_GET(arg);
    if (memory_name == NULL) {
        return CMD_USAGE;
    }
    if (!sal_strcasecmp(index_type, "port")) {
        /* Accessing port based memories in MMU */
        idx_param1 = ARG_GET(arg);
        port = sal_atoi(idx_param1);
        if (SOC_PORT_VALID(unit, port)) {
            rv = th3_dump_port_based_mem (unit, memory_name, port, only_index);
            if (rv) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
                return CMD_FAIL;
            }
        } else {
            cli_out("Invalid logical port number\n");
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(index_type, "queue") ||
               !sal_strcasecmp(index_type, "uc_cos") ||
               !sal_strcasecmp(index_type, "mc_cos")) {
        /* Accessing queue based memories in MMU */
        idx_param1 = ARG_GET(arg);
        idx_param2 = ARG_GET(arg);
        port = sal_atoi(idx_param1);
        user_q = sal_atoi(idx_param2);

        if (!SOC_PORT_VALID(unit, port) && (port != -1)) {
            cli_out("Invalid logical port number\n");
            return CMD_FAIL;
        }
        /*
         * Queue number retrieval and validation based on user input of
         * queue/uc_cos/mc_cos
         */
        if (!sal_strcasecmp(index_type, "queue")) {
            queue = user_q;
        } else if(!sal_strcasecmp(index_type, "uc_cos")) {
            if ((IS_CPU_PORT(unit, port)) || (user_q > si->port_num_uc_cosq[1])) {
                cli_out ("Invalid Uc_cos number provided \n");
                return CMD_FAIL;
            }
            queue = user_q;
        } else {
            if ((!IS_CPU_PORT(unit, port) && (user_q > si->port_num_cosq[1])) ||
                (IS_CPU_PORT(unit, port) && (user_q > si->port_num_cosq[0]))) {
                cli_out ("Invalid mc_cos number provided \n");
                return CMD_FAIL;
            }
            queue = si->port_num_uc_cosq[1] + user_q;
        }

        if ((queue < 0) ||
            (IS_CPU_PORT(unit, port) && (queue >= SOC_TH3_NUM_CPU_QUEUES)) ||
            (!IS_CPU_PORT(unit, port) && (queue >= SOC_TH3_NUM_GP_QUEUES))) {
            cli_out("Invalid Queue number\n");
            return CMD_FAIL;
        }

        rv = th3_dump_queue_based_mem (unit, memory_name, port, queue,
                only_index);
        if (rv) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
