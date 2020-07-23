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

#define _TH3_SPEED_TYPES 7

char cmd_check_tm_usage[] =
    "  tmcheck <idb | mmu> <portmap | drops> <port> \n"
    "  tmcheck mmu scheduler \n"
    "  tmcheck settings <file>\n"
    "  tmcheck health <all | port> \n";

#if !defined(NO_SAL_APPL) && (!defined(NO_FILEIO))
static
void chomp(char *str)
{
    while (*str && *str != '\n' && *str != '\r') {
        str++;
    }
    *str = 0;
}
#endif /* !defined(NO_SAL_APPL) && (!defined(NO_FILEIO)) */

STATIC int
compare_reg_value (int unit, soc_reg_t reg, int index, int inst, uint64 exp_value) {
#define UINT64_HEXA_STR_MAX_SIZE 20
    uint64 rval;
    int index_min, index_max, index_loop;
    char tmp[UINT64_HEXA_STR_MAX_SIZE];
    char tm1[UINT64_HEXA_STR_MAX_SIZE];
    COMPILER_64_ZERO(rval);

    if ((index < -1 || index >= SOC_REG_NUMELS(unit, reg)) &&
            (SOC_REG_INFO(unit, reg).regtype != soc_portreg)) {
        LOG_CLI((BSL_META_U(unit,
            "ERROR: register:%s index:%d is out of range\n"),
                   SOC_REG_NAME(unit, reg), index));
        return SOC_E_PARAM;
    }
    if (index == -1) {
        index_min = 0;
        index_max = SOC_REG_NUMELS(unit, reg);
    } else {
        index_min = index;
        index_max = index;
    }
    for (index_loop = index_min; index_loop <= index_max; index_loop++) {
        if (SOC_REG_INFO(unit, reg).regtype == soc_portreg) {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, index, inst, &rval));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, &rval));
        }
        _shr_format_uint64_hexa_string (rval, tmp);
        _shr_format_uint64_hexa_string (exp_value, tm1);
        if(!COMPILER_64_EQ(exp_value, rval)) {
            LOG_CLI((BSL_META_U(unit,
                "ERROR: register:%s index:%d  exp value:%s "
                " actual value:%s\n"),
                SOC_REG_NAME(unit, reg), index_loop, tm1, tmp));
        }
    }
#undef UINT64_HEXA_STR_MAX_SIZE
    return SOC_E_NONE;
}

STATIC int
compare_reg_field_value (int unit, soc_reg_t reg,
        soc_field_info_t* fld, int index, int inst, uint64 exp_value) {
#define UINT64_HEXA_STR_MAX_SIZE 20
    uint64 rval;
    uint64 act_value;
    soc_field_t field = fld->field;
    int index_min, index_max, index_loop;
    char tmp[UINT64_HEXA_STR_MAX_SIZE];
    char tm1[UINT64_HEXA_STR_MAX_SIZE];
    COMPILER_64_ZERO(rval);

    if ((index < -1 || index >= SOC_REG_NUMELS(unit, reg)) &&
            (SOC_REG_INFO(unit, reg).regtype != soc_portreg)) {
        LOG_CLI((BSL_META_U(unit,
            "ERROR: register:%s index:%d is out of range\n"),
                   SOC_REG_NAME(unit, reg), index));
        return SOC_E_PARAM;
    }
    if (index == -1) {
        index_min = 0;
        index_max = SOC_REG_NUMELS(unit, reg);
    } else {
        index_min = index;
        index_max = index;
    }
    for (index_loop = index_min; index_loop <= index_max; index_loop++) {
        if (SOC_REG_INFO(unit, reg).regtype == soc_portreg) {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, index, inst, &rval));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, &rval));
        }
        act_value = soc_reg64_field_get(unit, reg, rval, field);
        _shr_format_uint64_hexa_string (act_value, tmp);
        _shr_format_uint64_hexa_string (exp_value, tm1);
        if(!COMPILER_64_EQ(exp_value, act_value)) {
            LOG_CLI((BSL_META_U(unit,
                "ERROR: register:%s index:%d field_name : %s exp value:%s "
                " actual value:%s\n"),
                SOC_REG_NAME(unit, reg), index_loop, SOC_FIELD_NAME(unit,
                    fld->field), tm1, tmp));
        }
    }
#undef UINT64_HEXA_STR_MAX_SIZE
    return SOC_E_NONE;
}

STATIC int
compare_mem_field_value (int unit, soc_mem_t mem,
        soc_field_info_t* fld, int index, uint64 exp_value) {
#define UINT64_HEXA_STR_MAX_SIZE 20
    uint32 read_entry[SOC_MAX_MEM_WORDS];
    uint64 act_value;
    char tmp[UINT64_HEXA_STR_MAX_SIZE];
    char tm1[UINT64_HEXA_STR_MAX_SIZE];
    soc_field_t field = fld->field;

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                read_entry));
    soc_mem_field64_get(unit, mem, read_entry, field, &act_value);
    _shr_format_uint64_hexa_string (act_value, tmp);
    _shr_format_uint64_hexa_string (exp_value, tm1);

    if(!COMPILER_64_EQ(exp_value, act_value)) {
        LOG_CLI((BSL_META_U(unit,
            "ERROR: memory:%s index:%d field_name:%s, exp value:%s"
            " actual value:%s\n"),
            SOC_MEM_UFNAME(unit, mem), index, SOC_FIELD_NAME(unit, fld->field),
            tm1, tmp));
    }
#undef UINT64_HEXA_STR_MAX_SIZE
    return SOC_E_NONE;
}

STATIC int
exp_value_field_len_check (int unit, soc_field_info_t* fld,
        uint64 exp_value) {

#define UINT64_HEXA_STR_MAX_SIZE 20
    uint64 tmask;
    char tm1[UINT64_HEXA_STR_MAX_SIZE];
    COMPILER_64_MASK_CREATE(tmask, fld->len, 0);
	COMPILER_64_NOT(tmask);
	COMPILER_64_AND(tmask, exp_value);

    _shr_format_uint64_hexa_string (exp_value, tm1);
	if (!COMPILER_64_IS_ZERO(tmask)) {
	    cli_out("Value : %s too large for %d-bit field \"%s\".\n",
                    tm1, fld->len, SOC_FIELD_NAME(unit, fld->field));
#undef UINT64_HEXA_STR_MAX_SIZE
	    return SOC_E_PARAM;
	}
#undef UINT64_HEXA_STR_MAX_SIZE
    return SOC_E_NONE;
}

STATIC int
th3_override_index_based_on_port (int unit, soc_mem_t mem, int port) {
    int mem_index;
    int block_info_index;
    int mem_depth;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if ((SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_ITM) &&
        (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_EB) &&
        (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_GLB)) {
        /*index transformation not applicable for non-MMU memories*/
        return -1;
    } else {
        /*Index transformation for MMU memories based on port number*/
        mem_depth = soc_mem_index_max(unit, mem) + 1;
        if (mem_depth == 20) {
            /*Mem indexed by local MMU port number*/
            mem_index = SOC_TH3_MMU_LOCAL_PORT(unit, port);
        } else if (mem_depth == 160) {
            /*MMU mem indexed by MMU chip port number*/
            mem_index = SOC_TH3_MMU_CHIP_PORT(unit, port);
        } else {
            mem_index = -1;
        }
    }
    return mem_index;
}

STATIC int
memory_field_check (int unit, soc_mem_t mem, int index, soc_field_t field,
        uint64 exp_value, int port_num, int override_index) {
    soc_field_info_t *fld;
    soc_mem_info_t	*m;
    int mem_tot = 1, mem_idx, new_index = -1, memory_index;
    soc_mem_t *mem_list;
    int rv = SOC_E_NONE;

    /*Reserving array of length 64 for max combinations of EB/IPIPE in TH3*/
    mem_list = sal_alloc(64 * sizeof(soc_mem_t), "List of MMU logicla views");
    if (mem_list == NULL) {
        return SOC_E_MEMORY;
    }

    if (port_num != -1) {
        /*Expand based on port only if it is per-port memory*/
        rv = soc_th3_retrieve_mmu_mem_list_for_port(unit, port_num, mem, -1,
                                mem_list, 64, &mem_tot);
        if (rv) {
            mem_list[0] = mem;
        }
    } else {
        mem_list[0] = mem;
    }

    if (override_index == 1) {
        if (port_num != -1) {
            /*Expand based on port only if it is per-port memory*/
            new_index = th3_override_index_based_on_port(unit, mem_list[0],
                port_num);
        }
    }
    if (new_index != -1) {
        memory_index = new_index;
    } else {
        memory_index = index;
    }

    for(mem_idx = 0; mem_idx < mem_tot; mem_idx++) {
        mem = mem_list[mem_idx];
        if (mem != INVALIDm) {
            m = &SOC_MEM_INFO(unit, mem);
            for (fld = &m->fields[0]; fld < &m->fields[m->nFields]; fld++) {
                if (fld->field == field) {
                    break;
                }
            }
            if (fld == &m->fields[m->nFields]) {
                cli_out("No such field \"%s\" in memory \"%s\".\n",
                    SOC_FIELD_NAME(unit, fld->field), SOC_MEM_UFNAME(unit, mem));
                sal_free(mem_list);
                return SOC_E_PARAM;
            } else {
                rv = exp_value_field_len_check(unit, fld,
                            exp_value);
                if (rv) {
                    cli_out ("Field length check failed in memory:%s \n",
                            SOC_MEM_UFNAME(unit, mem));
                    sal_free(mem_list);
                    return rv;
                }
                rv = compare_mem_field_value (unit, mem,
                                fld, memory_index, exp_value);
            }
        }
    }
    sal_free(mem_list);
    return rv;
}

#if !defined(NO_SAL_APPL) && (!defined(NO_FILEIO))
STATIC int
memory_params_parse_check (int unit, char* mem_name, int index, char*
        field_name, uint64 exp_value, int port_num, int override_index) {
    soc_mem_t mem = INVALIDm;
    soc_field_info_t *fld;
    soc_mem_info_t	*m;
    int copyno;
    int mem_tot = 1, mem_idx, new_index = -1, memory_index;
    soc_mem_t *mem_list;
    int rv = SOC_E_NONE;

    /*Reserving array of length 64 for max combinations of EB/IPIPE in TH3*/
    mem_list = sal_alloc(64 * sizeof(soc_mem_t), "List of MMU logicla views");
    if (mem_list == NULL) {
        return SOC_E_MEMORY;
    }

    if (parse_memory_name(unit, &mem, mem_name, &copyno, 0) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n", mem_name);
        sal_free(mem_list);
        return SOC_E_PARAM;
    }
    if (port_num != -1) {
        /*Expand based on port only if it is per-port memory*/
        rv = soc_th3_retrieve_mmu_mem_list_for_port(unit, port_num, mem, -1,
                                mem_list, 64, &mem_tot);
        if (rv) {
            mem_list[0] = mem;
        }
    } else {
        mem_list[0] = mem;
    }

    if (override_index == 1) {
        if (port_num != -1) {
            /*Expand based on port only if it is per-port memory*/
            new_index = th3_override_index_based_on_port(unit, mem_list[0],
                    port_num);
        }
    }
    if (new_index != -1) {
        memory_index = new_index;
    } else {
        memory_index = index;
    }

    for(mem_idx = 0; mem_idx < mem_tot; mem_idx++) {
        mem = mem_list[mem_idx];
        if (mem != INVALIDm) {
            m = &SOC_MEM_INFO(unit, mem);
            for (fld = &m->fields[0]; fld < &m->fields[m->nFields]; fld++) {
                if (!sal_strcasecmp(field_name, SOC_FIELD_NAME(unit,
                                fld->field))) {
                break;
                }
            }
            if (fld == &m->fields[m->nFields]) {
                cli_out("No such field \"%s\" in memory \"%s\".\n",
                    field_name, SOC_MEM_UFNAME(unit, mem));
                sal_free(mem_list);
                return SOC_E_PARAM;
            } else {
                rv = exp_value_field_len_check(unit, fld,
                            exp_value);
                if (rv) {
                    cli_out ("Field length check failed in memory:%s \n",
                            mem_name);
                    sal_free(mem_list);
                    return rv;
                }
                rv = compare_mem_field_value (unit, mem,
                                fld, memory_index, exp_value);
            }
        }
    }
    sal_free(mem_list);
    return rv;
}

STATIC int
register_params_parse_check (int unit, char* reg_name, int index, char*
        field_name, uint64 exp_value) {
    soc_reg_t reg = INVALIDr;
    soc_regaddrlist_t alist;
    soc_regaddrinfo_t *ainfo;
    soc_reg_info_t *reg_info;
    soc_field_info_t *fld;
    int rv;

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
    }

    if (parse_symbolic_reference(unit, &alist, reg_name) < 0) {
        cli_out("Unknown register name \"%s\"\n", reg_name);
        soc_regaddrlist_free(&alist);
        return SOC_E_PARAM;
    }
    ainfo = &alist.ainfo[0];
    reg = ainfo->reg;
    reg_info = &SOC_REG_INFO(unit, reg);
    for (fld = &reg_info->fields[0]; fld <
            &reg_info->fields[reg_info->nFields]; fld++) {
        if (!sal_strcasecmp(field_name, SOC_FIELD_NAME(unit, fld->field))) {
            break;
        }
    }
    if (fld == &reg_info->fields[reg_info->nFields]) {
        cli_out("No such field \"%s\" in register \"%s\".\n",
                    field_name, SOC_REG_NAME(unit, reg));
        soc_regaddrlist_free(&alist);
        return SOC_E_PARAM;
    } else {
        rv = exp_value_field_len_check(unit, fld, exp_value);
        if (!rv) {
            compare_reg_field_value (unit, reg, fld, index, REG_PORT_ANY, exp_value);
        } else {
            soc_regaddrlist_free(&alist);
            return rv;
        }
    }
    soc_regaddrlist_free(&alist);
    return SOC_E_NONE;
}
#endif

void
th3_check_settings_from_file(int unit, char* file_name) {
#if !defined(NO_SAL_APPL) && (!defined(NO_FILEIO))
    FILE* fp;
    char line[1024];
    char *split1 = NULL;
    char *check_args[32] = {NULL};
    int arg_idx;
    int is_mem = -1, reg_mem_idx;
    uint64 exp_value;
    char *reg_mem_name = NULL, *field_name = NULL;
    int port_num = -1, override_index = -1;
    int err = 0;
    int port;

    fp = sal_fopen(file_name, "r");
    if (fp == NULL) {
        cli_out("*ERROR: Cannot open bcm config file: '%s'\n", file_name);
    } else {
        cli_out("\nRetrieveing expected settings from: '%s'", file_name);
        while (sal_fgets(line, sizeof(line), fp)) {
            split1 = sal_strtok(line,",");
            arg_idx = 0;
            while (split1) {
                check_args[arg_idx++] = split1;
                split1 = sal_strtok(NULL,",");
            }

            /*Expected input format (required args):
             * 1-bit to indicate reg/mem; (0-reg; 1-mem)
             * reg/memory name
             * index
             * field name
             * expected value
             */
            for (arg_idx = 0; arg_idx < 5; arg_idx++) {
                if (check_args[arg_idx] == NULL) {
                    cli_out("Fail: insufficient number of args in input file line");
                    err = 1;
                }
            }
            if (err == 0) {
                is_mem = sal_atoi(check_args[0]);
                reg_mem_name = check_args[1];
                reg_mem_idx = sal_atoi(check_args[2]);
                field_name = check_args[3];
                chomp(check_args[4]);
                exp_value = parse_uint64(check_args[4]);
                /*Additional optional args (only for memories)
                 *logical port num
                 *MMU queue num for a per-queue memory
                 */
                if (check_args[5] != NULL) {
                    chomp(check_args[5]);
                    port_num = sal_atoi(check_args[5]);
                }
                /*Optional param to indicate index override based on port number for
                 * MMU memories*/
                if (check_args[6] != NULL) {
                    chomp(check_args[6]);
                    override_index = sal_atoi(check_args[6]);
                }
                if (is_mem == 1) {
                    /*Process for memory*/
                    if (port_num != -1) {
                        memory_params_parse_check (unit, reg_mem_name, reg_mem_idx,
                            field_name, exp_value, port_num, override_index);
                    } else {
                        PBMP_ALL_ITER(unit, port) {
                            memory_params_parse_check (unit, reg_mem_name, reg_mem_idx,
                                field_name, exp_value, port, override_index);
                        }
                    }
                } else if (is_mem == 0) {
                    /*Process for register*/
                    register_params_parse_check (unit, reg_mem_name, reg_mem_idx,
                            field_name, exp_value);
                } else {
                    cli_out("Invalid input in register/memory indication");
                    continue;
                }
            }
        }
        sal_fclose(fp);
    }
#endif /*ifndef NO_SAL_APPL*/
}

STATIC void
th3_check_global_health_regs (int unit) {
    int reg_idx = 0;
    static const struct {
        soc_reg_t ref_reg;
        uint32 exp_value;
    } reg_val_list[] = {
         {MMU_GLBCFG_MEM_FAIL_INT_CTRr, 0x0},
         {MMU_GLBCFG_ECC_SINGLE_BIT_ERRORSr, 0x0},
         {MMU_GLBCFG_MEM_SER_FIFO_STSr, 0x1},
         {MMU_GLBCFG_CPU_INT_STATr, 0x0},
         {MMU_INTFO_THDR_TO_OOBFC_SP_STr, 0x0},
         {MMU_INTFO_THDI_TO_OOBFC_SP_STr, 0x0},
         {MMU_INTFO_THDO_TO_OOBFC_SP_STr, 0x0},
         {MMU_INTFO_THDO_TO_OOBFC_SP_MC_STr, 0x0},
         {MMU_INTFO_OOBFC_STSr, 0x0},
         {MMU_INTFO_TO_XPORT_BKPr, 0x0},
         {EDB_INTR_STATUSr, 0x0},
         {EDB_SER_FIFO_STATUSr, 0x0},
         {MMU_CFAP_STATUSr, 0x0},
         {INVALIDr, 0x0}
    };
    uint64 value;

    for (reg_idx = 0; reg_val_list[reg_idx].ref_reg != INVALIDr; reg_idx++) {
        COMPILER_64_ZERO(value);
        COMPILER_64_SET(value, 0x0, reg_val_list[reg_idx].exp_value);
        compare_reg_value (unit, reg_val_list[reg_idx].ref_reg, 0, 0,
                value);
    }

}

STATIC void
th3_check_per_pool_regs(int unit, int itm) {
    int reg_idx, pool;
    static const struct {
        soc_reg_t ref_reg;
        uint32 exp_value;
    } reg_val_list[] = {
        {MMU_THDO_SHARED_DB_POOL_SHARED_COUNT_DEBUG_MCr, 0x0},
        {MMU_THDI_POOL_SHARED_COUNT_SPr, 0x0},
        {MMU_THDO_SHARED_DB_POOL_SHARED_COUNTr, 0x0},
        {MMU_THDO_SHARED_DB_POOL_SHARED_COUNT_DEBUG_LOSSLESSr, 0x0},
        {MMU_THDO_MC_SHARED_CQE_POOL_SHARED_COUNTr, 0x0},
        {MMU_THDI_HDRM_POOL_COUNT_HPr, 0x0},
        {INVALIDr, 0x0}
    };
    uint64 value;
    for (reg_idx = 0; reg_val_list[reg_idx].ref_reg != INVALIDr; reg_idx++) {
        for (pool = 0; pool < _TH3_MMU_NUM_POOL; pool++) {
            COMPILER_64_ZERO(value);
            COMPILER_64_SET(value, 0x0, reg_val_list[reg_idx].exp_value);
            compare_reg_value (unit, reg_val_list[reg_idx].ref_reg, pool, itm,
                    value);
        }
    }
}

STATIC void
th3_check_per_rqe_queue_regs(int unit, int itm) {
    int reg_idx, rqe_q;
    static const struct {
        soc_reg_t ref_reg;
        uint32 exp_value;
    } reg_val_list[] = {
        {MMU_THDR_QE_SHARED_COUNT_PRIQr, 0x0},
        {MMU_THDR_QE_STATUS_PRIQr, 0x0},
        {MMU_THDR_QE_MIN_COUNT_PRIQr, 0x0},
        {MMU_THDR_QE_TOTAL_COUNT_PRIQr, 0x0},
        {INVALIDr, 0x0}
    };
    uint64 value;
    for (reg_idx = 0; reg_val_list[reg_idx].ref_reg != INVALIDr; reg_idx++) {
        for (rqe_q = 0; rqe_q < _TH3_MMU_NUM_RQE_QUEUES; rqe_q++) {
            COMPILER_64_ZERO(value);
            COMPILER_64_SET(value, 0x0, reg_val_list[reg_idx].exp_value);
            compare_reg_value (unit, reg_val_list[reg_idx].ref_reg, rqe_q, itm,
                    value);
        }
    }
}


STATIC void
th3_check_per_itm_health_regs (int unit, int itm) {
    int reg_idx;
    static const struct {
        soc_reg_t ref_reg;
        uint32 exp_value;
    } reg_val_list[] = {
        {MMU_THDO_SHARED_DB_POOL_DROP_STATESr, 0x0},
        {MMU_THDI_POOL_DROP_STATEr, 0x0},
        {MMU_THDO_CPU_QUEUE_DROP_STATES_47_36r, 0x0},
        {MMU_THDO_CPU_QUEUE_DROP_STATES_23_12r, 0x0},
        {MMU_THDO_CPU_QUEUE_DROP_STATES_35_24r, 0x0},
        {MMU_THDO_CPU_QUEUE_DROP_STATES_11_00r, 0x0},
        {MMU_THDR_QE_STATUS_SPr, 0x0},
        {MMU_THDO_MC_SHARED_CQE_POOL_DROP_STATESr, 0x0},
        {MMU_CFAP_INT_STATr, 0x0},
        {MMU_RL_CPU_INT_STATr, 0x0},
        {MMU_RL_DEBUG_PKT_CNTr, 0x0},
        {MMU_CRB_CPU_INT_STATr, 0x0},
        {MMU_ITMCFG_CPU_INT_STATr, 0x0},
        {MMU_ITMCFG_MEM_SER_FIFO_STSr, 0x1},
        {MMU_CFAP_STATUSr, 0x0},
        {MMU_CFAP_DROP_CBPr, 0x0},
        {MMU_CFAP_DROP_FULLr, 0x0},
        {MMU_CFAP_DROP_COLLISIONr, 0x0},
        {MMU_SCB_CPU_INT_STATr, 0x0},
        {MMU_RQE_INT_STATr, 0x0},
        {MMU_ITMCFG_ECC_SINGLE_BIT_ERRORSr, 0x0},
        {MMU_THDR_QE_CPU_INT_STATr, 0x0},
        {MMU_RQE_L3_PURGE_STATr, 0x0},
        {MMU_CRB_CPU_INT_STAT_LOGr, 0x0},
        {MMU_THDI_HDRM_POOL_STATUSr, 0x0},
        {MMU_THDR_QE_SHARED_COUNT_SPr, 0x0},
        {MMU_TOQ_CPU_INT_STATr, 0x0},
        {MMU_THDO_CPU_INT_STATr, 0x0},
        {MMU_ITMCFG_MEM_FAIL_INT_CTRr, 0x0},
        {MMU_OQS_CPU_INT_STATr, 0x0},
        {MMU_THDR_QE_COUNTER_OVERFLOWr, 0x0},
        {MMU_THDR_QE_COUNTER_UNDERFLOWr, 0x0},
        {MMU_THDO_COUNTER_UNDERFLOW_IDr, 0x0},
        {MMU_THDO_COUNTER_OVERFLOW_IDr, 0x0},
        {MMU_TOQ_STATUSr, 0x1},
         {INVALIDr, 0x0}
    };
    uint64 value;
    for (reg_idx = 0; reg_val_list[reg_idx].ref_reg != INVALIDr; reg_idx++) {
        COMPILER_64_ZERO(value);
        COMPILER_64_SET(value, 0x0, reg_val_list[reg_idx].exp_value);
        compare_reg_value (unit, reg_val_list[reg_idx].ref_reg, 0, itm,
                value);
    }
    th3_check_per_pool_regs(unit, itm);
    th3_check_per_rqe_queue_regs(unit, itm);
}


STATIC void
th3_check_per_pipe_health_regs (int unit, int pipe) {
    int reg_idx;
    static const struct {
        soc_reg_t ref_reg;
        uint32 exp_value;
    } reg_val_list[] = {
        {IDB_CA_CPU_ECC_STATUSr, 0x0},
        {IDB_CA_LPBK_ECC_STATUSr, 0x0},
        {CELL_QUEUE_ECC_STATUS_INTRr, 0x0},
        {EVENT_FIFO_ECC_STATUS_INTRr, 0x0},
        {SLOT_PIPELINE_ECC_STATUS_INTRr, 0x0},
        {MMU_EBCFG_MEM_FAIL_INT_CTRr, 0x0},
        {MMU_EBTOQ_CPU_INT_STATr, 0x0},
        {MMU_QSCH_PORT_EMPTY_STATUSr,  0xfffff},
        {MMU_EBCFG_ECC_SINGLE_BIT_ERRORSr, 0x0},
        {MMU_EBCFG_CPU_INT_STATr,  0x0},
        {MMU_INTFI_PORT_FC_BKPr, 0x0},
        {MMU_EBCFP_CPU_INT_STATr, 0x0},
        {MMU_EBCTM_PORT_EMPTY_STSr, 0xfffff},
        {MMU_EBCFG_MEM_SER_FIFO_STSr, 0x1},
        {MMU_EBPCC_COUNTER_OVERFLOWr, 0x1ff},
        {MMU_EBPCC_COUNTER_UNDERFLOWr, 0x1ff},
        {MMU_SCB_SCQE_CNT_OVERFLOWr, 0x0},
        {MMU_SCB_SCQE_CNT_UNDERFLOWr, 0x0},
        {MMU_SCB_SCQ_HP_FIFO_UNDERFLOWr, 0x0},
        {MMU_EBCFP_MTU_VIOLATION_STATUSr, 0x1ff},
        {MMU_EBCFP_POOL_COUNTERr, 0x0},
        {MMU_SCB_SCQ_PKT_CELL_CNT_STATUSr, 0x0},
        {MMU_SCB_SCQ_HP_FIFO_OVERFLOWr, 0x0},
        {MMU_SCB_SCQ_CELL_CNT_STATUSr, 0x0},
        {MMU_EBPCC_CPU_INT_STATr, 0x0},
        {MMU_MTRO_CPU_INT_STATr, 0x0},
        {MMU_EBCTM_CPU_INT_STATr, 0x0},
        {MMU_EBQS_CPU_INT_STATr, 0x0},
        {INVALIDr, 0x0}
    };
    uint64 value;
    for (reg_idx = 0; reg_val_list[reg_idx].ref_reg != INVALIDr; reg_idx++) {
        COMPILER_64_ZERO(value);
        COMPILER_64_SET(value, 0x0, reg_val_list[reg_idx].exp_value);
        compare_reg_value (unit, reg_val_list[reg_idx].ref_reg, 0, pipe,
                value);
    }

}

STATIC int
th3_get_pm_from_phy_port(int phy_port) {
    return (((phy_port - 1) & 0x1f) >> 3);
}

STATIC int
th3_get_subp_from_phy_port(int phy_port) {
    return ((phy_port - 1) & 0x7);
}

STATIC void
th3_check_idb_port_regs(int unit, int port) {

    static const soc_reg_t obm_usage_regs[][_TH3_PBLKS_PER_PIPE] = {
        {IDB_OBM0_USAGEr, IDB_OBM1_USAGEr, IDB_OBM2_USAGEr, IDB_OBM3_USAGEr},
        {IDB_OBM0_USAGE_1r, IDB_OBM1_USAGE_1r, IDB_OBM2_USAGE_1r,
            IDB_OBM3_USAGE_1r},
        {INVALIDr, INVALIDr, INVALIDr, INVALIDr}
    };
    static const soc_reg_t obm_hw_status_regs[_TH3_PBLKS_PER_PIPE] = {
        IDB_OBM0_HW_STATUSr, IDB_OBM1_HW_STATUSr, IDB_OBM2_HW_STATUSr,
        IDB_OBM3_HW_STATUSr
    };
    static const soc_field_t obm_hw_status_fields[_TH3_PORTS_PER_PBLK] = {
        RESIDUE_EMPTY_PORT0f, RESIDUE_EMPTY_PORT1f, RESIDUE_EMPTY_PORT2f,
        RESIDUE_EMPTY_PORT3f, RESIDUE_EMPTY_PORT4f, RESIDUE_EMPTY_PORT5f,
        RESIDUE_EMPTY_PORT6f, RESIDUE_EMPTY_PORT7f
    };
    static const soc_reg_t ca_hw_status_regs[_TH3_PBLKS_PER_PIPE] = {
        IDB_CA0_HW_STATUSr, IDB_CA1_HW_STATUSr, IDB_CA2_HW_STATUSr,
        IDB_CA3_HW_STATUSr
    };
    static const soc_field_t ca_fifo_full_fields[_TH3_PORTS_PER_PBLK] = {
        FIFO_FULL_PORT0f, FIFO_FULL_PORT1f, FIFO_FULL_PORT2f,
        FIFO_FULL_PORT3f, FIFO_FULL_PORT4f, FIFO_FULL_PORT5f,
        FIFO_FULL_PORT6f, FIFO_FULL_PORT7f
    };
    static const soc_field_t ca_fifo_ovrrun_fields[_TH3_PORTS_PER_PBLK] = {
        FIFO_OVERRUN_ERR_PORT0f, FIFO_OVERRUN_ERR_PORT1f, FIFO_OVERRUN_ERR_PORT2f,
        FIFO_OVERRUN_ERR_PORT3f, FIFO_OVERRUN_ERR_PORT4f, FIFO_OVERRUN_ERR_PORT5f,
        FIFO_OVERRUN_ERR_PORT6f, FIFO_OVERRUN_ERR_PORT7f
    };
    static const soc_reg_t ca_hw_status_2_regs[_TH3_PBLKS_PER_PIPE] = {
        IDB_CA0_HW_STATUS_2r, IDB_CA1_HW_STATUS_2r, IDB_CA2_HW_STATUS_2r,
        IDB_CA3_HW_STATUS_2r
    };
    static const soc_field_t ca_cell_len_err_fields[_TH3_PORTS_PER_PBLK] = {
        CELL_LEN_ERR_PORT0f, CELL_LEN_ERR_PORT1f, CELL_LEN_ERR_PORT2f,
        CELL_LEN_ERR_PORT3f, CELL_LEN_ERR_PORT4f, CELL_LEN_ERR_PORT5f,
        CELL_LEN_ERR_PORT6f, CELL_LEN_ERR_PORT7f
    };
    static const soc_field_t ca_sop_eop_err_fields[_TH3_PORTS_PER_PBLK] = {
        SOP_EOP_ERR_PORT0f, SOP_EOP_ERR_PORT1f, SOP_EOP_ERR_PORT2f,
        SOP_EOP_ERR_PORT3f, SOP_EOP_ERR_PORT4f, SOP_EOP_ERR_PORT5f,
        SOP_EOP_ERR_PORT6f, SOP_EOP_ERR_PORT7f
    };
    static const soc_field_t ca_fifo_underrun_fields[_TH3_PORTS_PER_PBLK] = {
        FIFO_UNDERRUN_ERR_PORT0f, FIFO_UNDERRUN_ERR_PORT1f, FIFO_UNDERRUN_ERR_PORT2f,
        FIFO_UNDERRUN_ERR_PORT3f, FIFO_UNDERRUN_ERR_PORT4f, FIFO_UNDERRUN_ERR_PORT5f,
        FIFO_UNDERRUN_ERR_PORT6f, FIFO_UNDERRUN_ERR_PORT7f
    };
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port = si->port_l2p_mapping[port];
    int pipe = si->port_pipe[port];
    int pm_num = th3_get_pm_from_phy_port(phy_port);
    int subp = th3_get_subp_from_phy_port(phy_port);
    int reg_idx;
    soc_reg_t reg;
    soc_field_t field;
    soc_reg_info_t *reg_info;
    soc_field_info_t *fld;
    uint64 value;

    for (reg_idx = 0; obm_usage_regs[reg_idx][0] != INVALIDr; reg_idx++) {
        reg = obm_usage_regs[reg_idx][pm_num];
        COMPILER_64_ZERO(value);
        compare_reg_value(unit, reg, subp, pipe, value);
    }

    reg = obm_hw_status_regs[pm_num];
    field = obm_hw_status_fields[subp];
    reg_info = &SOC_REG_INFO(unit, reg);
    for (fld = &reg_info->fields[0]; fld <
            &reg_info->fields[reg_info->nFields]; fld++) {
        if (field == fld->field) {
            break;
        }
    }
    COMPILER_64_SET(value, 0x0, 0x1);
    compare_reg_field_value(unit, reg, fld, 0, pipe, value);

    reg = ca_hw_status_regs[pm_num];
    reg_info = &SOC_REG_INFO(unit, reg);
    field = ca_fifo_full_fields[subp];
    for (fld = &reg_info->fields[0]; fld <
            &reg_info->fields[reg_info->nFields]; fld++) {
        if (field == fld->field) {
            break;
        }
    }
    COMPILER_64_ZERO(value);
    compare_reg_field_value(unit, reg, fld, 0, pipe, value);
    field = ca_fifo_ovrrun_fields[subp];
    for (fld = &reg_info->fields[0]; fld <
            &reg_info->fields[reg_info->nFields]; fld++) {
        if (field == fld->field) {
            break;
        }
    }
    COMPILER_64_ZERO(value);
    compare_reg_field_value(unit, reg, fld, 0, pipe, value);

    reg = ca_hw_status_2_regs[pm_num];
    reg_info = &SOC_REG_INFO(unit, reg);
    field = ca_cell_len_err_fields[subp];
    for (fld = &reg_info->fields[0]; fld <
            &reg_info->fields[reg_info->nFields]; fld++) {
        if (field == fld->field) {
            break;
        }
    }
    COMPILER_64_ZERO(value);
    compare_reg_field_value(unit, reg, fld, 0, pipe, value);
    field = ca_sop_eop_err_fields[subp];
    for (fld = &reg_info->fields[0]; fld <
            &reg_info->fields[reg_info->nFields]; fld++) {
        if (field == fld->field) {
            break;
        }
    }
    COMPILER_64_ZERO(value);
    compare_reg_field_value(unit, reg, fld, 0, pipe, value);
    field = ca_fifo_underrun_fields[subp];
    for (fld = &reg_info->fields[0]; fld <
            &reg_info->fields[reg_info->nFields]; fld++) {
        if (field == fld->field) {
            break;
        }
    }
    COMPILER_64_ZERO(value);
    compare_reg_field_value(unit, reg, fld, 0, pipe, value);
}

STATIC void
th3_check_mmu_port_regs(int unit, int port) {
    int reg_idx;
    static const struct {
        soc_reg_t ref_reg;
        uint32 exp_value;
    } reg_val_list[] = {
        {MMU_INTFI_PORT_PFC_STATEr, 0x0},
        {MMU_EBCTM_CNTR_1_STSr, 0x0},
        {MMU_THDI_PORT_LIMIT_STATESr, 0x0},
        {MMU_THDI_FLOW_CONTROL_XOFF_STATEr, 0x0},
        {MMU_THDI_SCR_CNT_STATUSr, 0x0},
        {MMU_INTFI_MMU_PORT_TO_MMU_QUEUES_FC_BKPr, 0x0},
        {MMU_EBPTS_PKSCH_CREDIT_STSr, 0x1},
        {IS_PKSCH_CREDIT_STSr, 0x1},
        {INVALIDr, 0x0}
    };
    uint64 value;
    for (reg_idx = 0; reg_val_list[reg_idx].ref_reg != INVALIDr; reg_idx++) {
        COMPILER_64_ZERO(value);
        COMPILER_64_SET(value, 0x0, reg_val_list[reg_idx].exp_value);
        compare_reg_value (unit, reg_val_list[reg_idx].ref_reg, 0, port,
                value);
    }
    if ((!IS_MANAGEMENT_PORT(unit, port)) &&
                 (!IS_CPU_PORT(unit, port)) &&
                 (!IS_LB_PORT(unit, port))) {
        /* Reg valid only for front panel ports */
        COMPILER_64_ZERO(value);
        compare_reg_value (unit, MMU_EBCTM_CNTR_0_STSr, port, 0,
                value);
    }
}

STATIC void
th3_check_mmu_port_pool_mems(int unit, int port) {
    int mem_idx;
    static const struct {
        soc_mem_t mem;
        soc_field_t field;
        uint32 exp_value;
    } mem_val_list[] = {
        {MMU_WRED_PORT_SP_SHARED_COUNTm, SHARED_COUNTf, 0x0},
        {MMU_THDO_COUNTER_PORTSP_MCm, SHARED_COUNTf, 0x0},
        {INVALIDm, INVALIDf, 0x0}
    };
    int mmu_chip_port = soc_th3_mmu_chip_port_num(unit, port);
    int pool, portsp_idx;
    uint64 value;

    for (mem_idx = 0;  mem_val_list[mem_idx].mem != INVALIDm; mem_idx++) {
        for (pool = 0; pool < _TH3_MMU_NUM_POOL; pool++) {
            portsp_idx = (mmu_chip_port * _TH3_MMU_NUM_POOL) + pool;
            COMPILER_64_ZERO(value);
            COMPILER_64_SET(value, 0x0, mem_val_list[mem_idx].exp_value);
            memory_field_check(unit, mem_val_list[mem_idx].mem, portsp_idx,
                    mem_val_list[mem_idx].field, value, port, 0);
        }
    }

}

STATIC void
th3_check_mmu_port_queue_mems(int unit, int port) {
    int mem_idx;
    static const struct {
        soc_mem_t mem;
        soc_field_t field;
        uint32 exp_value;
    } mem_val_list[] = {
        {MMU_WRED_UC_QUEUE_TOTAL_COUNTm, TOTAL_COUNTf, 0x0},
        {MMU_THDO_COUNTER_QUEUEm, TOTALCOUNTf, 0x0},
        {MMU_THDO_COUNTER_QUEUEm, SHARED_COUNTf, 0x0},
        {MMU_THDO_COUNTER_QUEUEm, MIN_COUNTf, 0x0},
        {INVALIDm, INVALIDf, 0x0}
    };
    soc_info_t *si = &SOC_INFO(unit);
    int q_loop, voq, num_queues;
    uint64 value;

    if (IS_CPU_PORT(unit, port)) {
        num_queues = SOC_TH3_NUM_CPU_QUEUES;
    } else {
        num_queues = SOC_TH3_NUM_GP_QUEUES;
    }

    for (mem_idx = 0;  mem_val_list[mem_idx].mem != INVALIDm; mem_idx++) {
        for (q_loop = 0; q_loop < num_queues; q_loop++) {
            voq =  si->port_uc_cosq_base[port] + q_loop;
            COMPILER_64_ZERO(value);
            COMPILER_64_SET(value, 0x0, mem_val_list[mem_idx].exp_value);
            memory_field_check(unit, mem_val_list[mem_idx].mem, voq,
                    mem_val_list[mem_idx].field, value, port, 0);
        }
    }

}

STATIC void
th3_check_mmu_port_mems(int unit, int port) {
    int mem_idx;
    static const struct {
        soc_mem_t mem;
        soc_field_t field;
        uint32 exp_value;
    } mem_val_list[] = {
        {MMU_THDO_TOTAL_PORT_COUNTERm, TOTAL_COUNTf, 0x0},
        {MMU_THDO_TOTAL_PORT_COUNTER_MCm, TOTAL_COUNTf, 0x0},
        {MMU_THDO_TOTAL_PORT_COUNTER_MC_SHm, TOTAL_COUNTf, 0x0},
        {MMU_THDO_TOTAL_PORT_COUNTER_SHm, TOTAL_COUNTf, 0x0},
        {MMU_THDO_COUNTER_UC_QGROUPm, MIN_COUNTf, 0x0},
        {MMU_THDO_COUNTER_MC_QGROUPm, MIN_COUNTf, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE0f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE1f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE2f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE3f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE4f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE5f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE6f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE7f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE8f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE9f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE10f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATEm, DS_QUEUE11f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATE_MCm, DS_QUEUE0f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATE_MCm, DS_QUEUE1f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATE_MCm, DS_QUEUE2f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATE_MCm, DS_QUEUE3f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATE_MCm, DS_QUEUE4f, 0x0},
        {MMU_THDO_PORT_Q_DROP_STATE_MCm, DS_QUEUE5f, 0x0},
        {MMU_THDO_PORT_SP_DROP_STATE_UCm,DATAf, 0x0},
        {MMU_THDO_PORT_SP_DROP_STATE_MCm,DATAf, 0x0},
        {MMU_THDO_COUNTER_PORT_UCm, SP0_SHARED_COUNTf, 0x0},
        {MMU_THDO_COUNTER_PORT_UCm, SP1_SHARED_COUNTf, 0x0},
        {MMU_THDO_COUNTER_PORT_UCm, SP2_SHARED_COUNTf, 0x0},
        {MMU_THDO_COUNTER_PORT_UCm, SP3_SHARED_COUNTf, 0x0},
        {MMU_THDO_COUNTER_PORT_UCm, SP0_TOTAL_COUNTf, 0x0},
        {MMU_THDO_COUNTER_PORT_UCm, SP1_TOTAL_COUNTf, 0x0},
        {MMU_THDO_COUNTER_PORT_UCm, SP2_TOTAL_COUNTf, 0x0},
        {MMU_THDO_COUNTER_PORT_UCm, SP3_TOTAL_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_MIN_COUNTERm, PG0_MIN_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_MIN_COUNTERm, PG1_MIN_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_MIN_COUNTERm, PG2_MIN_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_MIN_COUNTERm, PG3_MIN_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_MIN_COUNTERm, PG4_MIN_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_MIN_COUNTERm, PG5_MIN_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_MIN_COUNTERm, PG6_MIN_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_MIN_COUNTERm, PG7_MIN_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_SHARED_COUNTERm, PG0_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_SHARED_COUNTERm, PG1_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_SHARED_COUNTERm, PG2_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_SHARED_COUNTERm, PG3_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_SHARED_COUNTERm, PG4_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_SHARED_COUNTERm, PG5_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_SHARED_COUNTERm, PG6_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_SHARED_COUNTERm, PG7_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_HDRM_COUNTERm, PG0_HDRM_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_HDRM_COUNTERm, PG1_HDRM_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_HDRM_COUNTERm, PG2_HDRM_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_HDRM_COUNTERm, PG3_HDRM_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_HDRM_COUNTERm, PG4_HDRM_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_HDRM_COUNTERm, PG5_HDRM_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_HDRM_COUNTERm, PG6_HDRM_COUNTf, 0x0},
        {MMU_THDI_PORT_PG_HDRM_COUNTERm, PG7_HDRM_COUNTf, 0x0},
        {MMU_THDI_PORTSP_COUNTERm, PORTSP0_MIN_COUNTf, 0x0},
        {MMU_THDI_PORTSP_COUNTERm, PORTSP1_MIN_COUNTf, 0x0},
        {MMU_THDI_PORTSP_COUNTERm, PORTSP2_MIN_COUNTf, 0x0},
        {MMU_THDI_PORTSP_COUNTERm, PORTSP3_MIN_COUNTf, 0x0},
        {MMU_THDI_PORTSP_COUNTERm, PORTSP0_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORTSP_COUNTERm, PORTSP1_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORTSP_COUNTERm, PORTSP2_SHARED_COUNTf, 0x0},
        {MMU_THDI_PORTSP_COUNTERm, PORTSP3_SHARED_COUNTf, 0x0},
        {INVALIDm, INVALIDf, 0x0}
    };
    uint64 value;

    for (mem_idx = 0;  mem_val_list[mem_idx].mem != INVALIDm; mem_idx++) {
        COMPILER_64_ZERO(value);
        COMPILER_64_SET(value, 0x0, mem_val_list[mem_idx].exp_value);
        memory_field_check(unit, mem_val_list[mem_idx].mem, port,
                mem_val_list[mem_idx].field, value, port, 1);
    }

    th3_check_mmu_port_pool_mems(unit, port);
    th3_check_mmu_port_queue_mems(unit, port);

}

static const struct _soc_th3_port_ep_mmu_request_s {
    int port_speed;
    /*EP - MMU requests */
    int ep_mmu_req;
    /*EP - MMU PFC optimized setting */
    int ep_mmu_pfc_opt;
} _soc_th3_req_grant_tbl[_TH3_SPEED_TYPES] = {
    /* port_speed general    optimized settings*/
    { 10000,       8,           7},       /* 10GE    */
    { 25000,      15,          11},       /* 25GE    */
    { 40000,      20,          14},       /* 40GE    */
    { 50000,      28,          20},       /* 50GE    */
    {100000,      57,          42},       /* 100GE   */
    {200000,     100,          71},       /* 200GE   */
    {400000,     142,         110},        /* 400GE   */
};

STATIC void
th3_check_req_grant_per_port(int unit, int port) {
    int i;
    int phy_port;
    uint32 ep_mmu_req, port_ep_req = -1;
    uint32 mmu_credit, ep_credit;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 read_entry[SOC_MAX_MEM_WORDS];
    soc_reg_t reg;
    uint64 rval;
    uint32 rval_1 = 0;
    int speed_tbl_index = -1, port_speed = -1;
    int exp_port_ep_credit = 0, exp_ep_mmu_credit = 0;
    int num_lanes = si->port_num_lanes[port];
    int core_freq, dpr_freq;

    COMPILER_64_ZERO(rval);

    phy_port = si->port_l2p_mapping[port];
    port_speed = si->port_speed_max[port];

    dpr_freq = soc_property_get(unit, spn_DPR_CLOCK_FREQUENCY, -1);
    core_freq = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, -1);
    for (i = 0; i < _TH3_SPEED_TYPES; i++) {
        if (_soc_th3_req_grant_tbl[i].port_speed == port_speed) {
            speed_tbl_index = i;
            break;
        }
    }
    if ((!IS_CPU_PORT(unit, port)) && (!IS_LB_PORT(unit, port))) {
        if (IS_MANAGEMENT_PORT(unit, port)) {
            reg = XLMAC_TXFIFO_CELL_REQ_CNTr;
            exp_port_ep_credit = 8;
        } else {
            reg = CDMAC_TXFIFO_CELL_REQ_CNTr;
            exp_port_ep_credit = num_lanes * 16;
        }
        if (speed_tbl_index != -1) {
            if ((core_freq == 1325) && (dpr_freq == 1000)) {
                /* PFC optimized mode settings */
                exp_ep_mmu_credit =
                    _soc_th3_req_grant_tbl[speed_tbl_index].ep_mmu_pfc_opt;
            } else {
                exp_ep_mmu_credit =
                    _soc_th3_req_grant_tbl[speed_tbl_index].ep_mmu_req;
            }
        }
        /* coverity[checked_return:FALSE]*/
        (void)soc_reg_get(unit, reg, port, REG_PORT_ANY, &rval);
        /* coverity[checked_return:FALSE]*/
        port_ep_req = soc_reg64_field32_get(unit, reg, rval, REQ_CNTf);
        /* coverity[checked_return:FALSE]*/
        (void)soc_mem_read(unit, EGR_PORT_REQUESTSm, MEM_BLOCK_ANY, phy_port, read_entry);
        /* coverity[checked_return:FALSE]*/
        ep_credit = soc_mem_field32_get(unit, EGR_PORT_REQUESTSm, read_entry,
                OUTSTANDING_PORT_REQUESTSf);
        if (ep_credit != port_ep_req) {
            cli_out("ERROR: Mac to EP request credit mismatch for port:%d Mac"
                    "requests:%d EP credit:%d\n", port, port_ep_req, ep_credit);
        } else if (ep_credit == 0 && port_ep_req == 0) {
            cli_out("WARNING: MAc to EP requests credit 0 for port: %d"
                    " Please check if port is up\n", port);
        } else if (ep_credit != exp_port_ep_credit) {
             cli_out("ERROR: Mac to EP request credit not right for speed"
                     " port:%d speed:%d Mbps Mac requests:%d Expected:%d\n",
                     port, port_speed, ep_credit, exp_port_ep_credit);
        }
    }
    /* coverity[checked_return:FALSE]*/
    (void)soc_reg32_get(unit, MMU_EBPTS_EDB_CREDIT_COUNTERr, port, REG_PORT_ANY,
            &rval_1);
    /* coverity[checked_return:FALSE]*/
    mmu_credit = soc_reg_field_get(unit, MMU_EBPTS_EDB_CREDIT_COUNTERr, rval_1,
            NUM_EDB_CREDITSf);
    /* coverity[checked_return:FALSE]*/
    (void)soc_mem_read(unit, EGR_MMU_REQUESTSm, MEM_BLOCK_ANY, phy_port, read_entry);
    /* coverity[checked_return:FALSE]*/
    ep_mmu_req = soc_mem_field32_get(unit, EGR_MMU_REQUESTSm, read_entry,
                OUTSTANDING_MMU_REQUESTSf);
    if (mmu_credit != ep_mmu_req) {
        cli_out("ERROR: EP to MMU request credit mismatch for port:%d EP"
                "requests:%d MMU credit:%d\n", port, ep_mmu_req, mmu_credit);
    } else if (ep_mmu_req != exp_ep_mmu_credit) {
        if ((!IS_CPU_PORT(unit, port)) && (!IS_LB_PORT(unit, port)) &&
                port_ep_req != 0) {
                /*Do the check if port to Ep requests are non-zero for Front
                 * panel port*/
                cli_out("ERROR: EP to MMU request credit not right for speed"
                        " port:%d speed:%d Mbps EP requests:%d Expected:%d\n",
                        port, port_speed, ep_mmu_req, exp_ep_mmu_credit);
             }
    }
}

STATIC void
th3_check_port_health(int unit, int port) {
    th3_check_mmu_port_regs(unit, port);
    th3_check_mmu_port_mems(unit, port);
    th3_check_req_grant_per_port(unit, port);
    if ((!IS_MANAGEMENT_PORT(unit, port)) &&
       (!IS_CPU_PORT(unit, port)) &&
       (!IS_LB_PORT(unit, port))) {
        th3_check_idb_port_regs(unit, port);
    }
}

STATIC void
th3_check_health(int unit) {
    int itm, pipe;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map = si->itm_map;
    th3_check_global_health_regs(unit);

    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        if (itm_map & (1 << itm)) {
            th3_check_per_itm_health_regs(unit, itm);
        }
    }

    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        if (!SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            th3_check_per_pipe_health_regs(unit, pipe);
        }
    }
}

STATIC int
th3_check_idb_drops(int unit, int port) {
#define UINT64_HEXA_STR_MAX_SIZE 20
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    uint64 value, zero;
    int sync = 0; /*sync mode should be 0 for OBM counters*/
    char tmp[UINT64_HEXA_STR_MAX_SIZE];
    COMPILER_64_ZERO(zero);

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
    ctrl_info.instance = port;

    /*OBM lossy hi drops*/
    COMPILER_64_ZERO(value);
    ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI;
    SOC_IF_ERROR_RETURN(soc_counter_generic_get(unit, ctr_reg, ctrl_info, sync,
                                 0, &value));
    _shr_format_uint64_hexa_string (value, tmp);
    if (!COMPILER_64_EQ(zero, value)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Lossy hi IDB drops found for port :%d, pkt drop count : "
            "%s\n"), port, tmp));
    }

    /*OBM lossy lo drops*/
    COMPILER_64_ZERO(value);
    ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO;
    SOC_IF_ERROR_RETURN(
            soc_counter_generic_get(unit, ctr_reg, ctrl_info, sync,
                                 0, &value));
    _shr_format_uint64_hexa_string (value, tmp);
    if (!COMPILER_64_EQ(zero, value)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Lossy lo IDB drops found for port :%d, pkt drop count : "
            "%s\n"), port, tmp));
    }

    /*OBM lossless0 drops*/
    COMPILER_64_ZERO(value);
    ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0;
    SOC_IF_ERROR_RETURN(soc_counter_generic_get(unit, ctr_reg, ctrl_info, sync,
                                 0, &value));
    _shr_format_uint64_hexa_string (value, tmp);
    if (!COMPILER_64_EQ(zero, value)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Lossless 0 IDB drops found for port :%d, pkt drop count : "
            "%s \n"), port, tmp));
    }

    /*OBM lossless1 drops*/
    COMPILER_64_ZERO(value);
    ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1;
    SOC_IF_ERROR_RETURN(
            soc_counter_generic_get(unit, ctr_reg, ctrl_info, sync,
                                 0, &value));
    _shr_format_uint64_hexa_string (value, tmp);
    if (!COMPILER_64_EQ(zero, value)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Lossless 1 IDB drops found for port :%d, pkt drop count : "
            "%s \n"), port, tmp));
    }
#undef UINT64_HEXA_STR_MAX_SIZE
    return SOC_E_NONE;
}

STATIC int
th3_check_mmu_per_port_drops(int unit, int port) {
#define UINT64_HEXA_STR_MAX_SIZE 20
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg, ctr_reg_yel, ctr_reg_red;
    int sync = 0;
    uint64 value, red_drop, yel_drop, zero;
    char tmp[UINT64_HEXA_STR_MAX_SIZE], tmp_yel[UINT64_HEXA_STR_MAX_SIZE],
         tmp_red[UINT64_HEXA_STR_MAX_SIZE];
    int cosq;
    soc_info_t *si = &SOC_INFO(unit);
    COMPILER_64_ZERO(zero);

    /* Source port dropped packets*/
    COMPILER_64_ZERO(value);
    ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    ctrl_info.instance = port;
    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg,
                         ctrl_info, sync, -1, &value));
    _shr_format_uint64_hexa_string (value, tmp);
    if (!COMPILER_64_EQ(zero, value)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Ingress admission drops found for source port :%d,"
            " pkt drop count : %s \n"), port, tmp));
    }

    /*UC Egress Admission drops*/
    ctr_reg = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC;
    ctr_reg_yel = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW_UC;
    ctr_reg_red = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED_UC;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    ctrl_info.instance = port;
    for (cosq = 0; cosq < si->port_num_uc_cosq[port] ; cosq++) {
        COMPILER_64_ZERO(value);
        SOC_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, ctr_reg,
                             ctrl_info, sync, cosq, &value));
        _shr_format_uint64_hexa_string (value, tmp);
        if (!COMPILER_64_EQ(zero, value)) {
            LOG_CLI((BSL_META_U(unit,
                "Non-zero Egress admission drops found for port :%d,"
                " UC cos: %d, pkt drop count : %s \n"), port, cosq, tmp));
        }
    }
    COMPILER_64_ZERO(red_drop);
    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg_red,
                         ctrl_info, sync, 0, &red_drop));
    _shr_format_uint64_hexa_string (red_drop, tmp_red);
    if (!COMPILER_64_EQ(zero, red_drop)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Red Egress admission UC drops found for port :%d,"
            " pkt drop count : %s \n"), port, tmp_red));
    }
    COMPILER_64_ZERO(yel_drop);
    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg_yel,
                         ctrl_info, sync, 0, &yel_drop));
    _shr_format_uint64_hexa_string (yel_drop, tmp_yel);
    if (!COMPILER_64_EQ(zero, yel_drop)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Yellow Egress admission UC drops found for port :%d,"
            " pkt drop count : %s \n"), port, tmp_yel));
    }

    /*MC Egress Admission drops*/
    ctr_reg = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
    ctr_reg_yel = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW;
    ctr_reg_red = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    ctrl_info.instance = port;
    for (cosq = 0; cosq < si->port_num_cosq[port] ; cosq++) {
        COMPILER_64_ZERO(value);
        SOC_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, ctr_reg,
                             ctrl_info, sync, cosq, &value));
        _shr_format_uint64_hexa_string (value, tmp);
        if (!COMPILER_64_EQ(zero, value)) {
            LOG_CLI((BSL_META_U(unit,
                "Non-zero Egress admission drops found for port :%d,"
                " MC cos: %d, Total pkt drop count : %s \n"), port, cosq, tmp));
        }
    }
    COMPILER_64_ZERO(red_drop);
    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg_red,
                         ctrl_info, sync, 0, &red_drop));
    _shr_format_uint64_hexa_string (red_drop, tmp_red);
    if (!COMPILER_64_EQ(zero, red_drop)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Red Egress admission MC drops found for port :%d,"
            " pkt drop count : %s \n"), port, tmp_red));
    }
    COMPILER_64_ZERO(yel_drop);
    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg_yel,
                         ctrl_info, sync, 0, &yel_drop));
    _shr_format_uint64_hexa_string (yel_drop, tmp_yel);
    if (!COMPILER_64_EQ(zero, yel_drop)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Yellow Egress admission MC drops found for port :%d,"
            " pkt drop count : %s \n"), port, tmp_yel));
    }

    /*WRED drops*/
    ctr_reg = SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT;
    ctr_reg_yel = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW;
    ctr_reg_red = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    ctrl_info.instance = port;
    for (cosq = 0; cosq < si->port_num_uc_cosq[port] ; cosq++) {
        COMPILER_64_ZERO(value);
        SOC_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, ctr_reg,
                             ctrl_info, sync, cosq, &value));
        _shr_format_uint64_hexa_string (value, tmp);
        if (!COMPILER_64_EQ(zero, value)) {
            LOG_CLI((BSL_META_U(unit,
                "Non-zero WRED drops found for port :%d,"
                " UC cos: %d, Total pkt drop count : %s \n"), port, cosq, tmp));
        }
    }
    COMPILER_64_ZERO(red_drop);
    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg_red,
                         ctrl_info, sync, 0, &red_drop));
    _shr_format_uint64_hexa_string (red_drop, tmp_red);
    if (!COMPILER_64_EQ(zero, red_drop)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Red Egress admission MC drops found for port :%d,"
            " pkt drop count : %s \n"), port, tmp_red));
    }
    COMPILER_64_ZERO(yel_drop);
    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg_yel,
                         ctrl_info, sync, 0, &yel_drop));
    _shr_format_uint64_hexa_string (yel_drop, tmp_yel);
    if (!COMPILER_64_EQ(zero, yel_drop)) {
        LOG_CLI((BSL_META_U(unit,
            "Non-zero Yellow Egress admission MC drops found for port :%d,"
            " pkt drop count : %s \n"), port, tmp_yel));
    }

#undef UINT64_HEXA_STR_MAX_SIZE
    return SOC_E_NONE;
}

STATIC int
th3_check_mmu_global_drops(int unit) {
#define UINT64_HEXA_STR_MAX_SIZE 20
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg, ctr_reg_yel, ctr_reg_red;
    int sync = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map = si->itm_map;
    uint64 value64, sum, red_drop, yel_drop, zero;
    int itm, hdrm_pool_id, rqe_q_id;
    char tmp[UINT64_HEXA_STR_MAX_SIZE], tmp_gr[UINT64_HEXA_STR_MAX_SIZE],
         tmp_yel[UINT64_HEXA_STR_MAX_SIZE], tmp_red[UINT64_HEXA_STR_MAX_SIZE];
    COMPILER_64_ZERO(zero);

    /* Headroom pool drops*/
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    ctr_reg = SOC_COUNTER_NON_DMA_HDRM_POOL_DROP_PKT;
    for (hdrm_pool_id = 0; hdrm_pool_id < _TH3_MMU_NUM_POOL; hdrm_pool_id++) {
        COMPILER_64_ZERO(value64);
        ctrl_info.instance = 0;
        SOC_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg,
                             ctrl_info, sync, hdrm_pool_id, &value64));
        _shr_format_uint64_hexa_string (value64, tmp);
        if (!COMPILER_64_EQ(zero, value64)) {
            LOG_CLI((BSL_META_U(unit,
                "Non-zero Headroom Pool drops found for pool :%d,"
                " pkt drop count : %s \n"), hdrm_pool_id, tmp));
        }
    }

    /*RQE drops*/
    ctr_reg = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT;
    ctr_reg_yel = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_YELLOW;
    ctr_reg_red = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_RED;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    for (rqe_q_id = 0; rqe_q_id < _TH3_MMU_NUM_RQE_QUEUES; rqe_q_id++) {
        COMPILER_64_ZERO(value64);
        COMPILER_64_ZERO(red_drop);
        COMPILER_64_ZERO(yel_drop);
        ctrl_info.instance = 0;
        SOC_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, ctr_reg,
                             ctrl_info, sync, rqe_q_id, &value64));
        SOC_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, ctr_reg_yel,
                             ctrl_info, sync, rqe_q_id, &yel_drop));
        SOC_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, ctr_reg_red,
                             ctrl_info, sync, rqe_q_id, &red_drop));
        COMPILER_64_ZERO(sum);
        COMPILER_64_ADD_64(sum, value64);
        COMPILER_64_SUB_64(sum, yel_drop);
        COMPILER_64_SUB_64(sum, red_drop);
        _shr_format_uint64_hexa_string (value64, tmp);
        _shr_format_uint64_hexa_string (yel_drop, tmp_yel);
        _shr_format_uint64_hexa_string (red_drop, tmp_red);
        _shr_format_uint64_hexa_string (sum, tmp_gr);
        if (!COMPILER_64_EQ(zero, value64)) {
            LOG_CLI((BSL_META_U(unit,
                "Non-zero RQE queue drops found for queue :%d, "
                "Total pkt drop count: %s \n"), rqe_q_id, tmp));
            LOG_CLI((BSL_META_U(unit,
                "Color drops green : %s, yellow: %s, red: %s \n"),
                 tmp_gr, tmp_red, tmp_yel));
        }
    }

    /*CRB drops*/
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        if (itm_map & (1 << itm)) {
            COMPILER_64_ZERO(sum);
            COMPILER_64_ZERO(value64);
            SOC_IF_ERROR_RETURN(
                soc_reg_get(unit, MMU_CRB_PKT_DROP_CNTR_STATr, itm, 0, &value64));
            COMPILER_64_ADD_64(sum, value64);
            SOC_IF_ERROR_RETURN(
                soc_reg_get(unit, MMU_CRB_INVALID_DESTINATION_PKT_COUNTr, itm, 0, &value64));
            COMPILER_64_ADD_64(sum, value64);
            if (!COMPILER_64_EQ(zero, sum)) {
                _shr_format_uint64_hexa_string (sum, tmp);
                LOG_CLI((BSL_META_U(unit,
                    "Non-zero drops found for itm: %d due to invalid destination"
                    "port from IP, pkt drop count : %s\n"), itm, tmp));
            }
        }
    }

#undef UINT64_HEXA_STR_MAX_SIZE
    return SOC_E_NONE;
}

cmd_result_t
cmd_check_tm_settings(int unit, args_t *arg)
{
    char *subcmd, *component, *sub_comp;
    int port;
    int rv = 0;
    pbmp_t pbm;
    soc_info_t *si = &SOC_INFO(unit);

    subcmd = ARG_GET(arg);
    component = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "idb")) {
        if (!sal_strcasecmp(component, "portmap")) {
            soc_th3_check_idb_portmap(unit);
        } else if (!sal_strcasecmp(component, "drops")) {
            sub_comp = ARG_GET(arg);
            if(sub_comp == NULL) {
                PBMP_E_ITER(unit, port) {
                    if (!SOC_PBMP_MEMBER(si->management_pbm, port)) {
                        rv = th3_check_idb_drops(unit, port);
                        if (rv < 0) {
                            return CMD_FAIL;
                        }
                    }
                }
            } else {
                if (parse_bcm_pbmp(unit, sub_comp, &pbm) < 0) {
                    cli_out("%s: Error: unrecognized port bitmap: %s\n",
                            ARG_CMD(arg), sub_comp);
                    return CMD_FAIL;
                }
                if (BCM_PBMP_IS_NULL(pbm)) {
                    ARG_DISCARD(arg);
                    return CMD_OK;
                }
                SOC_PBMP_ITER(pbm, port) {
                    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
                            (SOC_PBMP_MEMBER(si->management_pbm, port))) {
                        cli_out ("Port :%d not valid for IDB drops \n", port);
                    } else {
                        rv = th3_check_idb_drops(unit, port);
                        if (rv < 0) {
                            return CMD_FAIL;
                        }
                    }
                }
            }
        } else {
            return CMD_USAGE;
        }
    } else if (!sal_strcasecmp(subcmd, "mmu")) {
        if (!sal_strcasecmp(component, "portmap")) {
            soc_th3_check_mmu_portmap(unit);
        } else if (!sal_strcasecmp(component, "drops")) {
            sub_comp = ARG_GET(arg);
            rv = th3_check_mmu_global_drops(unit);
            if (rv < 0) {
                return CMD_FAIL;
            }

            if(sub_comp == NULL) {
                PBMP_ALL_ITER(unit, port) {
                    rv = th3_check_mmu_per_port_drops(unit, port);
                    if (rv < 0) {
                        return CMD_FAIL;
                    }
                }
            } else {
                if (parse_bcm_pbmp(unit, sub_comp, &pbm) < 0) {
                    cli_out("%s: Error: unrecognized port bitmap: %s\n",
                            ARG_CMD(arg), sub_comp);
                    return CMD_FAIL;
                }
                if (BCM_PBMP_IS_NULL(pbm)) {
                    ARG_DISCARD(arg);
                    return CMD_OK;
                }
                SOC_PBMP_ITER(pbm, port) {
                    rv = th3_check_mmu_per_port_drops(unit, port);
                    if (rv < 0) {
                        return CMD_FAIL;
                    }
                }
            }

        } else if (!sal_strcasecmp(component, "scheduler")) {
            sub_comp = ARG_GET(arg);
            if (sub_comp == NULL) {
                PBMP_ALL_ITER(unit, port) {
                    rv = soc_tomahawk3_schduler_hier_show(unit, port);
                    if (rv < 0) {
                        return CMD_FAIL;
                    }
                }
            } else {
                if (parse_bcm_pbmp(unit, sub_comp, &pbm) < 0) {
                    cli_out("%s: Error: unrecognized port bitmap: %s\n",
                            ARG_CMD(arg), sub_comp);
                    return CMD_FAIL;
                }
                if (BCM_PBMP_IS_NULL(pbm)) {
                    ARG_DISCARD(arg);
                    return CMD_OK;
                }
                SOC_PBMP_ITER(pbm, port) {
                    rv = soc_tomahawk3_schduler_hier_show(unit, port);
                    if (rv < 0) {
                        return CMD_FAIL;
                    }
                }
            }
        } else {
            return CMD_USAGE;
        }
    } else if (!sal_strcasecmp(subcmd, "settings")) {
        /*Expect file name as seconf argument*/
        if (component == NULL) {
            cli_out("%s: ERROR: missing file name\n", ARG_CMD(arg));
            return CMD_USAGE;
        }

        th3_check_settings_from_file(unit, component);

    } else if (!sal_strcasecmp(subcmd, "health")) {
        if (component == NULL ) {
            th3_check_health(unit);
            PBMP_ALL_ITER(unit, port) {
                th3_check_port_health(unit, port);
            }
        } else {
           if (parse_bcm_pbmp(unit, component, &pbm) < 0) {
               cli_out("%s: Error: unrecognized port bitmap: %s\n",
                       ARG_CMD(arg), component);
               return CMD_FAIL;
           }
           if (BCM_PBMP_IS_NULL(pbm)) {
               ARG_DISCARD(arg);
               return CMD_OK;
           }
           SOC_PBMP_ITER(pbm, port) {
               if (SOC_PORT_VALID(unit, port)) {
                   th3_check_port_health(unit, port);
               }
           }
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
