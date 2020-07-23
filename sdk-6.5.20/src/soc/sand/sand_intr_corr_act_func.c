/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_INTR

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/drv.h>
#include <soc/error.h>
 
#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <shared/bitop.h>
#include <bcm/error.h>

#include <soc/sand/sand_intr_corr_act_func.h>
#include <soc/sand/sand_ser_correction.h>
#include <soc/sand/sand_mem.h>



static sand_block_control_info_t sand_intr_blocks_control_info[SOC_MAX_NUM_DEVICES][NOF_BCM_BLOCKS];
static uint32 sand_intr_block_control_init_flag[SOC_MAX_NUM_DEVICES] = {0};

static sand_intr_action_t *sand_intr_action_info[SOC_MAX_NUM_DEVICES];
static char* sand_intr_corr_act_name[] = {
    "Clear Check",
    "Config DRAM",
    "ECC 1b Correct",
    "EM Soft Recovery",
    "Force",
    "Handle CRC Delete Buffer FIFO",
    "Handle MACT Event FIFO",
    "Handle OAMP Event Fifo",
    "Handle OAMP Statistics Event Fifo",
    "Hard Reset",
    "Hard Reset without Fabric",
    "EM Soft Recovery",
    "EM Soft Recovery",
    "Ingress Hard Reset",
    "IPS QDESC Clear Unused",
    "None",
    "EM Soft Recovery",
    "Print",
    "Reprogram Resource",
    "RTP Link Mask Change",
    "RX LOS Handle",
    "According to SER algorithm",
    "Shadow and Soft Reset",
    "Shutdown link",
    "Shutdown Unreachable Destination",
    "Soft Reset",
    "TCAM Shadow from SW DB",
    "RTP SLSCT",
    "Shutdown links",
    "MC RTP Correct",
    "UC RTP Correct",
    "All Rechable fix",
    "Event Ready",
    "IPS_QSZ Correct",
    "EM Soft Recovery",
    "XOR Fix",
    "Dynamic CallBration",
    "EM ECC 1B FIX",
    "Ingress Soft Reset",
    "XOR ECC 1Bit SW correction",
    "XOR Fix and soft reset",

    "MDB ECC 1B FIX",
    "MDB ECC 2B FIX",
    "Delete BDB FIFO FULL",   
    "Delete BDB FIFO NOT EMPTY",
    "Soft Init",
    "PFC deadlock breaking",
    "Kaps TECC",
    "Unknown"
};



void sand_intr_action_info_set(int unit, void* sand_intr_action)
{
    sand_intr_action_info[unit] = (sand_intr_action_t*)sand_intr_action;
    return;
}

sand_intr_action_t* sand_intr_action_info_get(int unit)
{
    return sand_intr_action_info[unit];
}

int sand_is_ser_action_support(int unit)
{
    int     rc = FALSE;
    
    if (sand_intr_action_info[unit] != NULL)
    {
        rc = TRUE;
    }

    return rc;
}

int sand_is_ser_intr_cnt_reg(int unit, soc_reg_t reg)
{
    int rc = FALSE;
    soc_reg_info_t reg_info;
    soc_block_type_t block;
    sand_block_control_info_t* sand_block_info;

    if (SOC_REG_IS_VALID(unit, reg))
    {
        reg_info = SOC_REG_INFO(unit, reg);
        block = sand_blktype_to_index(SOC_REG_FIRST_BLK_TYPE(reg_info.block));
        sand_block_info = sand_get_block_control_info(unit);
        if ((reg == sand_block_info[block].ecc1_cnt_reg) ||
            (reg == sand_block_info[block].ecc2_cnt_reg) ||
            (reg == sand_block_info[block].parity_cnt_reg))
        {
            rc = TRUE;
        }
    }

    return rc;
}

char* sand_intr_action_name_by_func(int unit, soc_handle_interrupt_func intr_func)
{
    int                     ii;
    sand_intr_action_t*     sand_intr_action;
    int                     act_action = SAND_INT_CORR_ACT_NONE;


    sand_intr_action = sand_intr_action_info_get(unit);
    if (sand_intr_action != NULL)
    {
        for (ii = 0; sand_intr_action[ii].func_arr != NULL; ii++)
        {
            if (intr_func == sand_intr_action[ii].func_arr)
            {
                act_action = sand_intr_action[ii].corr_action;
                break;
            }
        }
    }

    if (act_action >= COUNTOF(sand_intr_corr_act_name))
    {
        act_action = COUNTOF(sand_intr_corr_act_name) - 1;
    }
    return sand_intr_corr_act_name[act_action];
}


sand_memory_dc_t sand_get_cnt_reg_type(int unit, soc_reg_t cnt_reg)
{
    if (SOC_REG_FIELD_VALID(unit,cnt_reg, PARITY_ERR_ADDR_VALIDf)) 
    {
        return SAND_ECC_PARITY_DC;
    }

    if (SOC_REG_FIELD_VALID(unit,cnt_reg, ECC_2B_ERR_ADDR_VALIDf)) 
    {
        return SAND_ECC_ECC2B_DC;
    }

    if (SOC_REG_FIELD_VALID(unit,cnt_reg,ECC_1B_ERR_ADDR_VALIDf))
    {
        return SAND_ECC_ECC1B_DC;
    }

    if (SOC_REG_FIELD_VALID(unit,cnt_reg,P_1_ECC_2B_ERR_ADDR_VALIDf))
    {
        return SAND_P_1_ECC_ECC2B_DC;
    }

    if (SOC_REG_FIELD_VALID(unit,cnt_reg,P_1_ECC_1B_ERR_ADDR_VALIDf))
    {
        return SAND_P_1_ECC_ECC1B_DC;
    }

    if (SOC_REG_FIELD_VALID(unit,cnt_reg,P_2_ECC_2B_ERR_ADDR_VALIDf))
    {
        return SAND_P_2_ECC_ECC2B_DC;
    }

    if (SOC_REG_FIELD_VALID(unit,cnt_reg,P_2_ECC_1B_ERR_ADDR_VALIDf))
    {
        return SAND_P_2_ECC_ECC1B_DC;
    }

    if (SOC_REG_FIELD_VALID(unit,cnt_reg,P_3_ECC_2B_ERR_ADDR_VALIDf))
    {
        return SAND_P_3_ECC_ECC2B_DC;
    }

    if (SOC_REG_FIELD_VALID(unit,cnt_reg,P_3_ECC_1B_ERR_ADDR_VALIDf))
    {
        return SAND_P_3_ECC_ECC1B_DC;
    }

    return SAND_INVALID_DC;
}



int sand_get_cnt_reg_values(
        int unit,
        sand_memory_dc_t type,
        soc_reg_t cnt_reg,
        int copyno,
        uint32 *cntf, 
        uint32 *cnt_overflowf, 
        uint32 *addrf, 
        uint32 *addr_validf)
{
    uint64 counter;
    int rc;
    soc_field_t         
        error_addr_valid_field=INVALIDf, 
        error_addr_field=INVALIDf,
        error_cnt_overflow_field=INVALIDf, 
        error_cnt_field=INVALIDf;
    SHR_FUNC_INIT_VARS(unit);

    switch (type) {
        case SAND_ECC_PARITY_DC:
            error_addr_valid_field = PARITY_ERR_ADDR_VALIDf;
            error_addr_field = PARITY_ERR_ADDRf;
            error_cnt_overflow_field = PARITY_ERR_CNT_OVERFLOWf;
            error_cnt_field = PARITY_ERR_CNTf;
            break;
        case SAND_ECC_ECC2B_DC:
            error_addr_valid_field = ECC_2B_ERR_ADDR_VALIDf;
            error_addr_field = ECC_2B_ERR_ADDRf;
            error_cnt_overflow_field = ECC_2B_ERR_CNT_OVERFLOWf;
            error_cnt_field = ECC_2B_ERR_CNTf;
            break;
        case SAND_ECC_ECC1B_DC:
            error_addr_valid_field = ECC_1B_ERR_ADDR_VALIDf;
            error_addr_field = ECC_1B_ERR_ADDRf;
            error_cnt_overflow_field = ECC_1B_ERR_CNT_OVERFLOWf;
            error_cnt_field = ECC_1B_ERR_CNTf;
            break;
        case SAND_P_1_ECC_ECC1B_DC:
            error_addr_valid_field = P_1_ECC_1B_ERR_ADDR_VALIDf;
            error_addr_field = P_1_ECC_1B_ERR_ADDRf;
            error_cnt_overflow_field = P_1_ECC_1B_ERR_CNT_OVERFLOWf;
            error_cnt_field = P_1_ECC_1B_ERR_CNTf;
            break;
        case SAND_P_1_ECC_ECC2B_DC:
            error_addr_valid_field = P_1_ECC_2B_ERR_ADDR_VALIDf;
            error_addr_field = P_1_ECC_2B_ERR_ADDRf;
            error_cnt_overflow_field = P_1_ECC_2B_ERR_CNT_OVERFLOWf;
            error_cnt_field = P_1_ECC_2B_ERR_CNTf;
            break;
        case SAND_P_2_ECC_ECC1B_DC:
            error_addr_valid_field = P_2_ECC_1B_ERR_ADDR_VALIDf;
            error_addr_field = P_2_ECC_1B_ERR_ADDRf;
            error_cnt_overflow_field = P_2_ECC_1B_ERR_CNT_OVERFLOWf;
            error_cnt_field = P_2_ECC_1B_ERR_CNTf;
            break;
        case SAND_P_2_ECC_ECC2B_DC:
            error_addr_valid_field = P_2_ECC_2B_ERR_ADDR_VALIDf;
            error_addr_field = P_2_ECC_2B_ERR_ADDRf;
            error_cnt_overflow_field = P_2_ECC_2B_ERR_CNT_OVERFLOWf;
            error_cnt_field = P_2_ECC_2B_ERR_CNTf;
            break;
        case SAND_P_3_ECC_ECC1B_DC:
            error_addr_valid_field = P_3_ECC_1B_ERR_ADDR_VALIDf;
            error_addr_field = P_3_ECC_1B_ERR_ADDRf;
            error_cnt_overflow_field = P_3_ECC_1B_ERR_CNT_OVERFLOWf;
            error_cnt_field = P_3_ECC_1B_ERR_CNTf;
            break;
        case SAND_P_3_ECC_ECC2B_DC:
            error_addr_valid_field = P_3_ECC_2B_ERR_ADDR_VALIDf;
            error_addr_field = P_3_ECC_2B_ERR_ADDRf;
            error_cnt_overflow_field = P_3_ECC_2B_ERR_CNT_OVERFLOWf;
            error_cnt_field = P_3_ECC_2B_ERR_CNTf;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit, "Unknown type %d"),type));
            rc = _SHR_E_NOT_FOUND;
            SHR_EXIT();
    }

    rc = soc_reg_get(unit, cnt_reg, copyno, 0, &counter);
    SHR_IF_ERR_EXIT(rc);

    
    *addr_validf = soc_reg64_field32_get(unit, cnt_reg, counter, error_addr_valid_field);

    
    *addrf = soc_reg64_field32_get(unit, cnt_reg, counter, error_addr_field);

    
    *cnt_overflowf = soc_reg64_field32_get(unit, cnt_reg, counter, error_cnt_overflow_field);

    
    *cntf = soc_reg64_field32_get(unit, cnt_reg, counter, error_cnt_field);

exit:
    SHR_FUNC_EXIT;
}




int
sand_interrupt_print_info(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    int recurring_action,
    sand_int_corr_act_type corr_act,
    char *special_msg)
{
    int rc;
    uint32 flags=1;
    soc_interrupt_db_t* interrupt;
    char cur_special_msg[SAND_INTERRUPT_SPECIAL_MSG_SIZE];
    char cur_corr_act_msg[SAND_INTERRUPT_COR_ACT_MSG_SIZE];
    char print_msg[SAND_INTERRUPT_PRINT_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No interrupts for device");
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt]);
    if (special_msg == NULL) {
        sal_snprintf(cur_special_msg, sizeof(cur_special_msg), "None");
    } else {
        sal_strncpy(cur_special_msg, special_msg, sizeof(cur_special_msg) - 1);
    }

    rc = soc_interrupt_flags_get(unit, en_interrupt, &flags);
    SHR_IF_ERR_EXIT(rc);

    
    if(((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0) && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = SAND_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
        case SAND_INT_CORR_ACT_HARD_RESET:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Hard Reset");
            break;
        case SAND_INT_CORR_ACT_NONE:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "None");
            break;
        case SAND_INT_CORR_ACT_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Soft Reset");
            break;
        case SAND_INT_CORR_ACT_PRINT:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Print");
            break;
        case SAND_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
            sal_sprintf(cur_corr_act_msg, "Handle OAMP Event Fifo");
            break;
        case SAND_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO:
            sal_sprintf(cur_corr_act_msg, "Handle OAMP Statistics Event Fifo");
            break;
        case SAND_INT_CORR_ACT_SHADOW:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Shadow");
            break;
        case SAND_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown link");
            break;
        case SAND_INT_CORR_ACT_CLEAR_CHECK:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Clear Check");
            break;
        case SAND_INT_CORR_ACT_CONFIG_DRAM:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Config DRAM");
            break;
        case SAND_INT_CORR_ACT_ECC_1B_FIX:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "ECC 1b Correct");
            break;
        case SAND_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY:
        case SAND_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY:
        case SAND_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY:
        case SAND_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY:
        case SAND_INT_CORR_ACT_EM_SOFT_RECOVERY:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "EM Soft Recovery");
            break;
        case SAND_INT_CORR_ACT_FORCE:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Force");
            break;
        case SAND_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Handle CRC Delete Buffer FIFO");
            break;
        case SAND_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Handle MACT Event FIFO");
            break;
        case SAND_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Hard Reset without Fabric");
            break;
        case SAND_INT_CORR_ACT_INGRESS_HARD_RESET:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Ingress Hard Reset");
            break;
        case SAND_INT_CORR_ACT_IPS_QDESC:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "IPS QDESC Clear Unused");
            break;
        case SAND_INT_CORR_ACT_REPROGRAM_RESOURCE:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Reprogram Resource");
            break;
        case SAND_INT_CORR_ACT_RTP_LINK_MASK_CHANGE:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "RTP Link Mask Change");
            break;
        case SAND_INT_CORR_ACT_RX_LOS_HANDLE:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "RX LOS Handle");
            break;
        case SAND_INT_CORR_ACT_SHADOW_AND_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Shadow and Soft Reset");
            break;
        case SAND_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown Unreachable Destination");
            break;
        case SAND_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "TCAM Shadow from SW DB");
            break;
        case SAND_INT_CORR_ACT_RTP_SLSCT:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "RTP SLSCT");
            break;
        case SAND_INT_CORR_ACT_SHUTDOWN_LINKS:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown links");
            break;
        case SAND_INT_CORR_ACT_MC_RTP_CORRECT:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "MC RTP Correct");
            break;
        case SAND_INT_CORR_ACT_UC_RTP_CORRECT:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "UC RTP Correct");
            break;
        case SAND_INT_CORR_ACT_ALL_REACHABLE_FIX:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "All Rechable fix");
            break;
        case SAND_INT_CORR_ACT_IPS_QSZ_CORRECT:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "IPS QSZ Correct");
            break;
        case SAND_INT_CORR_ACT_XOR_FIX:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "XOR Fix");
            break;
        case SAND_INT_CORR_ACT_DYNAMIC_CALIBRATION:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Dynamic calibration");
            break;
        default:
            sal_snprintf(cur_corr_act_msg, SAND_INTERRUPT_COR_ACT_MSG_SIZE, "Unknown");
    }

    
#if defined(SOC_NO_NAMES)
    sal_snprintf(print_msg, SAND_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ",
                en_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#else
    sal_snprintf(print_msg, SAND_INTERRUPT_PRINT_MSG_SIZE,"name=%s, id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ",
                interrupt->name, en_interrupt, interrupt->reg_index, block_instance, unit, recurring_action, cur_special_msg, cur_corr_act_msg);
#endif

    
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE)) {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,"%s"), print_msg));
    }

exit:
    SHR_FUNC_EXIT;
}



int sand_blktype_to_index(soc_block_t blktype)
{
    int index = blktype;

    if (blktype>SOC_BLK_END_NOT_DNX) {
       index -=(SOC_BLK_FIRST_DNX - SOC_BLK_END_NOT_DNX) ;
    }
    return index;
}



int sand_set_block_control_regs_info_init(int unit, void* data)
{
    sand_block_control_info_t* map = (sand_block_control_info_t*)data;
    soc_reg_t reg;
    soc_reg_info_t reg_info;   
    soc_block_type_t block;

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        if (!SOC_REG_IS_VALID(unit, reg))
        {
            continue;
        }
        if (!SOC_REG_IS_ENABLED(unit, reg))
        {
            continue;
        }

        reg_info = SOC_REG_INFO(unit, reg);
        block = sand_blktype_to_index(SOC_REG_FIRST_BLK_TYPE(reg_info.block));
        
        if (SOC_REG_FIELD_VALID(unit, reg, CPU_BYPASS_ECC_PARf))
        {
            map[block].gmo_reg = reg;
        }
        else if (sal_strstr(SOC_REG_NAME(unit,reg), "ECC_ERR_1B_MONITOR_MEM_MASK"))
        {
            map[block].ecc1_monitor_mem_reg = reg;
        }
        else if (sal_strstr(SOC_REG_NAME(unit,reg), "ECC_ERR_2B_MONITOR_MEM_MASK"))
        {
            map[block].ecc2_monitor_mem_reg = reg;
        }
        else
        {
            if (sal_strstr(SOC_REG_NAME(unit,reg), "PARITY_ERR_MONITOR_MEM_MASK"))
            {
                map[block].parity_monitor_mem_reg = reg;
            }
            if (sal_strstr(SOC_REG_NAME(unit,reg), "PAR_ERR_MEM_MASK"))
            {
                map[block].parity_monitor_mem_reg = reg;
            }
        }
    }

    return _SHR_E_NONE;
}



int sand_collect_blocks_control_info(int unit)
{
    soc_interrupt_db_t* ints_db = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    soc_block_type_t block;
    sand_block_control_info_t* map = sand_intr_blocks_control_info[unit];
    soc_reg_t def_gmo_reg;
    int i, nof_ints;
    soc_reg_t reg;
    soc_reg_info_t reg_info;

    SHR_FUNC_INIT_VARS(unit);

    
    for (block=0; block<NOF_BCM_BLOCKS; block++)
    {
        map[block].ecc1_monitor_mem_reg     = INVALIDr;
        map[block].ecc2_monitor_mem_reg     = INVALIDr;
        map[block].parity_monitor_mem_reg   = INVALIDr;
        map[block].gmo_reg = INVALIDr;

        map[block].ecc1_cnt_reg     = INVALIDr;
        map[block].ecc2_cnt_reg     = INVALIDr;
        map[block].parity_cnt_reg   = INVALIDr;
    }
    
    soc_nof_interrupts(unit, &nof_ints);
    for (i=0; i < nof_ints; i++) 
    {
        reg = ints_db[i].cnt_reg;
        if ((reg == INVALIDr) || (!SOC_REG_IS_VALID(unit,reg)))
        {
            continue;
        }
        reg_info = SOC_REG_INFO(unit, reg);
        block = sand_blktype_to_index(SOC_REG_FIRST_BLK_TYPE(reg_info.block));

        if (SOC_REG_FIELD_VALID(unit, reg, PARITY_ERR_ADDR_VALIDf)) 
        {
            map[block].parity_int = ints_db[i].id;
            map[block].parity_cnt_reg = reg;
        }
        else if (SOC_REG_FIELD_VALID(unit, reg, ECC_1B_ERR_ADDR_VALIDf))
        {
            map[block].ecc1_int = ints_db[i].id;
            map[block].ecc1_cnt_reg = reg;
        }
        else if (SOC_REG_FIELD_VALID(unit, reg, ECC_2B_ERR_ADDR_VALIDf))
        {
            map[block].ecc2_int = ints_db[i].id;
            map[block].ecc2_cnt_reg = reg;
        }
    }

    if (SOC_IS_FE1600(unit))
    {
        def_gmo_reg = ECI_GLOBAL_1r;
    }
    else if (SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        def_gmo_reg = ECI_GLOBALFr;
    }
    else
    {
        def_gmo_reg = ECI_GLOBAL_MEM_OPTIONSr;
    }

    for (i=0; i<SOC_BLK_LAST_DNX; i++) 
    {
        if (i>=SOC_BLK_END_NOT_DNX && i < SOC_BLK_FIRST_DNX)
        {
            continue;
        }
        block = sand_blktype_to_index(i); 
        map[block].gmo_reg = def_gmo_reg;
    }

    SHR_IF_ERR_EXIT(sand_set_block_control_regs_info_init(unit, map)); 

    sand_intr_block_control_init_flag[unit] = 1;
exit:
    SHR_FUNC_EXIT;
}



int sand_dump_block_control_info(int unit, sand_block_control_info_t* map)
{
    int     index, blk_type;
    char*   blk_name;

    if (map == NULL)
    {
        map = sand_intr_blocks_control_info[unit];
    }

    cli_out("%-6s%-6s%-16s%-24s\n", "block", "index", "name", "gmo_reg");
    for (blk_type=0; blk_type<SOC_BLK_END_NOT_DNX; blk_type++)
    {
        index = sand_blktype_to_index(blk_type);
        blk_name = soc_block_name_lookup_ext(blk_type, unit);
        cli_out("%-6d%-6d%-10s%-20s\n", blk_type, index, blk_name,
            SOC_REG_IS_VALID(unit, map[index].gmo_reg)? SOC_REG_NAME(unit, map[index].gmo_reg) : "INVALIDr");
    }
    cli_out("------------------------------------------------------------------\n");
    for (blk_type=SOC_BLK_FIRST_DNX; blk_type<SOC_BLK_LAST_DNX; blk_type++)
    {
        index = sand_blktype_to_index(blk_type);
        blk_name = soc_block_name_lookup_ext(blk_type, unit);
        cli_out("%-6d%-6d%-16s%-24s\n", blk_type, index, blk_name,
            SOC_REG_IS_VALID(unit, map[index].gmo_reg)? SOC_REG_NAME(unit, map[index].gmo_reg) : "INVALIDr");
    }

    return SOC_E_NONE;
}



int sand_read_mem_gmo_reg(int unit, soc_mem_t mem, int copyno, uint32* regval)
{
    soc_block_t block;
    soc_reg_t   gmo_reg;

    SHR_FUNC_INIT_VARS(unit);

    block = sand_blktype_to_index(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)));
    gmo_reg = sand_intr_blocks_control_info[unit][block].gmo_reg;

    SHR_IF_ERR_EXIT(soc_reg32_get(unit, gmo_reg, copyno, 0, regval));

exit:
    SHR_FUNC_EXIT;
}


int sand_write_mem_gmo_reg(int unit, soc_mem_t mem, int copyno, uint32 regval)
{
    soc_block_t block;
    soc_reg_t   gmo_reg;

    SHR_FUNC_INIT_VARS(unit);

    block = sand_blktype_to_index(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)));
    gmo_reg = sand_intr_blocks_control_info[unit][block].gmo_reg;

    SHR_IF_ERR_EXIT(soc_reg32_set(unit, gmo_reg, copyno, 0, regval));

exit:
    SHR_FUNC_EXIT;
}




int sand_disable_block_mem_monitor(
        int unit,
        soc_mem_t mem,
        int copyno,
        soc_reg_above_64_val_t* monitor_mask)
{
    soc_block_t             block;
    soc_reg_t               monitor_mem_reg;
    soc_reg_above_64_val_t  value_zero;

    SHR_FUNC_INIT_VARS(unit);
    if (SOC_IS_FE1600(unit) || SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        SHR_EXIT();
    }

    block = sand_blktype_to_index(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)));
    SOC_REG_ABOVE_64_CLEAR(value_zero);
    
    monitor_mem_reg = sand_intr_blocks_control_info[unit][block].ecc1_monitor_mem_reg;
    if (SOC_REG_IS_VALID(unit, monitor_mem_reg))
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, monitor_mem_reg, copyno, 0, monitor_mask[0]));
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, monitor_mem_reg, copyno, 0, value_zero));
    }

    
    monitor_mem_reg = sand_intr_blocks_control_info[unit][block].ecc2_monitor_mem_reg;
    if (SOC_REG_IS_VALID(unit, monitor_mem_reg))
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, monitor_mem_reg, copyno, 0, monitor_mask[1]));
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, monitor_mem_reg, copyno, 0, value_zero));
    }

    
    monitor_mem_reg = sand_intr_blocks_control_info[unit][block].parity_monitor_mem_reg;
    if (SOC_REG_IS_VALID(unit, monitor_mem_reg))
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, monitor_mem_reg, copyno, 0, monitor_mask[2]));
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, monitor_mem_reg, copyno, 0, value_zero));
    }

    
    monitor_mem_reg = sand_intr_blocks_control_info[unit][block].gmo_reg;
    if ((SOC_REG_IS_VALID(unit, monitor_mem_reg)) && (SOC_REG_FIELD_VALID(unit, monitor_mem_reg, CPU_BYPASS_ECC_PARf)))
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, monitor_mem_reg, copyno, 0, monitor_mask[3]));
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, monitor_mem_reg, copyno, CPU_BYPASS_ECC_PARf, 0x0));
    }

exit:
    SHR_FUNC_EXIT;

}



int sand_re_enable_block_mem_monitor(int unit, int interrupt_id, soc_mem_t mem, int copyno, soc_reg_above_64_val_t* monitor_mask)
{
    soc_block_t             block;
    soc_reg_t               monitor_mem_reg, cnt_reg;
    soc_interrupt_db_t*     ints_db;
    uint32                  cntf, cnt_overflowf, addrf, addr_validf;

    SHR_FUNC_INIT_VARS(unit);
    if (SOC_IS_FE1600(unit) || SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        SHR_EXIT();
    }

    block = sand_blktype_to_index(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)));
    
    monitor_mem_reg = sand_intr_blocks_control_info[unit][block].ecc1_monitor_mem_reg;
    if (SOC_REG_IS_VALID(unit, monitor_mem_reg))
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, monitor_mem_reg, copyno, 0, monitor_mask[0]));
    }

    
    monitor_mem_reg = sand_intr_blocks_control_info[unit][block].ecc2_monitor_mem_reg;
    if (SOC_REG_IS_VALID(unit, monitor_mem_reg))
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, monitor_mem_reg, copyno, 0, monitor_mask[1]));
    }

    
    monitor_mem_reg = sand_intr_blocks_control_info[unit][block].parity_monitor_mem_reg;
    if (SOC_REG_IS_VALID(unit, monitor_mem_reg))
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, monitor_mem_reg, copyno, 0, monitor_mask[2]));
    }

    
    monitor_mem_reg = sand_intr_blocks_control_info[unit][block].gmo_reg;
    if ((SOC_REG_IS_VALID(unit, monitor_mem_reg)) && (SOC_REG_FIELD_VALID(unit, monitor_mem_reg, CPU_BYPASS_ECC_PARf)))
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, monitor_mem_reg, copyno, 0, monitor_mask[3]));
    }

    
    ints_db = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    cnt_reg = ints_db[interrupt_id].cnt_reg;
    SHR_IF_ERR_EXIT(sand_get_cnt_reg_values(unit, sand_get_cnt_reg_type(unit,cnt_reg), cnt_reg, copyno, &cntf, &cnt_overflowf, &addrf, &addr_validf));
    if (addr_validf) 
    {
        SHR_IF_ERR_EXIT(sand_get_cnt_reg_values(unit, sand_get_cnt_reg_type(unit,cnt_reg), cnt_reg, copyno, &cntf, &cnt_overflowf, &addrf, &addr_validf));
        if (addr_validf) 
        {
            cli_out("mem %s cnt_reg was not cleared as expected\n", SOC_MEM_NAME(unit,mem));
        }
        else
        {
            cli_out("mem %s cnt_reg was not cleared at first time\n", SOC_MEM_NAME(unit,mem));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int sand_disable_block_ecc_check(
        int unit, 
        int interrupt_id,
        soc_mem_t mem,
        int copyno,
        soc_reg_above_64_val_t value,
        soc_reg_above_64_val_t orig_value)
{
    soc_block_t block ;
    soc_reg_t   ecc1_monitor_mem_reg ;

    SHR_FUNC_INIT_VARS(unit);
    block = sand_blktype_to_index(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem))) ;
    ecc1_monitor_mem_reg = sand_intr_blocks_control_info[unit][block].ecc1_monitor_mem_reg;
    if (!SOC_REG_IS_VALID(unit,ecc1_monitor_mem_reg))
    {
        cli_out("cant find ecc1_monitor_mem_reg for mem %s\n",SOC_MEM_NAME(unit,mem));
        SHR_EXIT();
    }

    if (orig_value != NULL)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ecc1_monitor_mem_reg,  copyno, 0, orig_value));
    }
    else
    {
        soc_interrupt_db_t *ints_db = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
        uint32  cntf, cnt_overflowf, addrf, addr_validf;
        soc_reg_t cnt_reg = ints_db[interrupt_id].cnt_reg;
        SHR_IF_ERR_EXIT(sand_get_cnt_reg_values(unit, sand_get_cnt_reg_type(unit,cnt_reg), cnt_reg, copyno, &cntf, &cnt_overflowf, &addrf, &addr_validf));
        if (addr_validf)
        {
            cli_out("mem %s   cnt_reg doesnt cleared as expected\n", SOC_MEM_NAME(unit,mem));
        }
    }
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ecc1_monitor_mem_reg, copyno, 0, value));
exit:
    SHR_FUNC_EXIT;
}

sand_block_control_info_t* sand_get_block_control_info(int unit)
{
    if (!sand_intr_block_control_init_flag[unit])
    {
        sand_collect_blocks_control_info(unit);
    }

    return sand_intr_blocks_control_info[unit];
}

static int sand_interrupt_flags_parse(
    int unit,
    uint32 flags,
    char* str,
    int size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(str, _SHR_E_PARAM, "str is null");
    
    sal_memset(str, 0, size);
    if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE))
    {
        sal_snprintf(str + sal_strlen(str), size - sal_strlen(str), "LOG");
    }
    else
    {
        sal_snprintf(str + sal_strlen(str), size - sal_strlen(str), "notLOG");
    }
    if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB_BIT))
    {
        sal_snprintf(str + sal_strlen(str), size - sal_strlen(str), " ACT_BCM_AND_USR_CB");
    }
    else if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE))
    {
        sal_snprintf(str + sal_strlen(str), size - sal_strlen(str), " ACT_OVERRIDE");
    }
    else
    {
        sal_snprintf(str + sal_strlen(str), size - sal_strlen(str), " ACT_BCM_ONLY");
    }
    sal_snprintf(str + sal_strlen(str), size - sal_strlen(str), " Priority-%d", 
                 ((flags & SOC_INTERRUPT_DB_FLAGS_PRIORITY_MASK) >> SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB));
    if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS))
    {
        sal_snprintf(str + sal_strlen(str), size - sal_strlen(str), " FORCE_UNMASK");
    }
exit:
    SHR_FUNC_EXIT;
}

int
sand_single_interrupt_info_dump(
    int unit,
    int intr)
{
    int blk, port, nof_interrupts;
    int intr_assert, intr_enable;
    soc_interrupt_db_t *interrupts;
    uint32 flags, storm_timed_count, storm_timed_period;
    soc_block_info_t *bi = NULL;
    char str_flags[256];
    
    SHR_FUNC_INIT_VARS(unit);

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (NULL == interrupts)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    
    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    if ((intr >= nof_interrupts) || (intr < 0))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (!SOC_REG_IS_VALID(unit, interrupts[intr].reg))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(soc_interrupt_flags_get(unit, intr, &flags));
    SHR_IF_ERR_EXIT(soc_interrupt_storm_timed_count_get(unit, intr, &storm_timed_count));
    SHR_IF_ERR_EXIT(soc_interrupt_storm_timed_period_get(unit, intr, &storm_timed_period));
    SHR_IF_ERR_EXIT(sand_interrupt_flags_parse(unit, flags, str_flags, sizeof(str_flags)));

#if !defined(SOC_NO_NAMES)

    cli_out("%-6d%s\n", intr, interrupts[intr].name);

#else

    cli_out("%-6d%d\n", intr, interrupts[intr].id);

#endif
    cli_out("    %-12s%s\n", "FLAGS:", str_flags);
    cli_out("    %-12stimed_period=%d, timed_count=%d\n", "storms:", storm_timed_period, storm_timed_count);
    cli_out("--------------------------------------------------------------------------------------\n");
    SOC_BLOCKS_ITER(unit, blk, SOC_REG_INFO(unit, interrupts[intr].reg).block)
    {
        if (!SOC_INFO(unit).block_valid[blk])
        {
            continue;
        }
        bi = &(SOC_BLOCK_INFO(unit, blk));
        port = (bi->type == SOC_BLK_CLP || bi->type == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : bi->number;

        SHR_IF_ERR_EXIT(soc_interrupt_get(unit, port, &(interrupts[intr]), &intr_assert));
        SHR_IF_ERR_EXIT(soc_interrupt_is_enabled(unit, port, &(interrupts[intr]), &intr_enable));
        cli_out("\t%-2d:    %s(%s), stat=%d, nominal_count=%d, occurrences=%d, start_time=%d\n", 
                bi->number, 
                (intr_enable == 1) ? "Enable" : "Disable",
                (intr_assert == 1) ? "Assert" : "!Assert",
                interrupts[intr].statistics_count[bi->number],
                interrupts[intr].storm_nominal_count[bi->number],
                interrupts[intr].storm_detection_occurrences[bi->number],
                interrupts[intr].storm_detection_start_time[bi->number]);
    }
    cli_out("--------------------------------------------------------------------------------------\n");

exit:
    SHR_FUNC_EXIT;
}


#undef BSL_LOG_MODULE

