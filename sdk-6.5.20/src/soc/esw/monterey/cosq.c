/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * APACHE MMU/Cosq soc routines
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/trident2.h>
#include <soc/monterey.h>

#if defined(BCM_MONTEREY_SUPPORT)

#define _MN_COSQ_CPU_RESERVED_L0_BASE 261
#define _MN_COSQ_CPU_RESERVED_L0_NUM  4

#define _MN_HSP_PORT_MAX_COS 8
#define _MN_MAX_NUM_UC_QUEUE(u) (SOC_IS_MONTEREY(unit) ? 640 :16384)

typedef struct soc_monterey_shadow_memory_s {
    soc_mem_t mem;
    uint32  *mem_shadow;
}soc_monterey_shadow_memory_t;

typedef struct soc_monterey_shadow_s {
    soc_monterey_shadow_memory_t* shadow_array;
    int mem_count;
}soc_monterey_shadow_t;
typedef struct soc_monterey_obm_cb_entry_s {
    bcm_obm_callback_fn          cb;
    void                        *user_data;
}soc_monterey_obm_cb_entry_t;
soc_monterey_obm_cb_entry_t soc_monterey_obm_cb[SOC_MAX_NUM_DEVICES];



static soc_mem_t shadow_mems[] = {
    MMU_INTFI_XPIPE_FC_MAP_TBL0m,
    MMU_INTFI_XPIPE_FC_MAP_TBL1m,
    /* Followings OBM classifier tables would have two instances for two pgws */
    IDB_OBM0_DSCP_MAP_PORT0m,
    IDB_OBM0_DSCP_MAP_PORT1m,
    IDB_OBM0_DSCP_MAP_PORT2m,
    IDB_OBM0_DSCP_MAP_PORT3m,
    IDB_OBM1_DSCP_MAP_PORT0m,
    IDB_OBM1_DSCP_MAP_PORT1m,
    IDB_OBM1_DSCP_MAP_PORT2m,
    IDB_OBM1_DSCP_MAP_PORT3m,
    IDB_OBM2_DSCP_MAP_PORT0m,
    IDB_OBM2_DSCP_MAP_PORT1m,
    IDB_OBM2_DSCP_MAP_PORT2m,
    IDB_OBM2_DSCP_MAP_PORT3m,
    IDB_OBM3_DSCP_MAP_PORT0m,
    IDB_OBM3_DSCP_MAP_PORT1m,
    IDB_OBM3_DSCP_MAP_PORT2m,
    IDB_OBM3_DSCP_MAP_PORT3m,
    IDB_OBM4_DSCP_MAP_PORT0m,
    IDB_OBM4_DSCP_MAP_PORT1m,
    IDB_OBM4_DSCP_MAP_PORT2m,
    IDB_OBM4_DSCP_MAP_PORT3m,
    IDB_OBM5_DSCP_MAP_PORT0m,
    IDB_OBM5_DSCP_MAP_PORT1m,
    IDB_OBM5_DSCP_MAP_PORT2m,
    IDB_OBM5_DSCP_MAP_PORT3m,
    IDB_OBM6_DSCP_MAP_PORT0m,
    IDB_OBM6_DSCP_MAP_PORT1m,
    IDB_OBM6_DSCP_MAP_PORT2m,
    IDB_OBM6_DSCP_MAP_PORT3m,
    IDB_OBM7_DSCP_MAP_PORT0m,
    IDB_OBM7_DSCP_MAP_PORT1m,
    IDB_OBM7_DSCP_MAP_PORT2m,
    IDB_OBM7_DSCP_MAP_PORT3m,
    IDB_OBM0_PRI_MAP_PORT0m,
    IDB_OBM0_PRI_MAP_PORT1m,
    IDB_OBM0_PRI_MAP_PORT2m,
    IDB_OBM0_PRI_MAP_PORT3m,
    IDB_OBM1_PRI_MAP_PORT0m,
    IDB_OBM1_PRI_MAP_PORT1m,
    IDB_OBM1_PRI_MAP_PORT2m,
    IDB_OBM1_PRI_MAP_PORT3m,
    IDB_OBM2_PRI_MAP_PORT0m,
    IDB_OBM2_PRI_MAP_PORT1m,
    IDB_OBM2_PRI_MAP_PORT2m,
    IDB_OBM2_PRI_MAP_PORT3m,
    IDB_OBM3_PRI_MAP_PORT0m,
    IDB_OBM3_PRI_MAP_PORT1m,
    IDB_OBM3_PRI_MAP_PORT2m,
    IDB_OBM3_PRI_MAP_PORT3m,
    IDB_OBM4_PRI_MAP_PORT0m,
    IDB_OBM4_PRI_MAP_PORT1m,
    IDB_OBM4_PRI_MAP_PORT2m,
    IDB_OBM4_PRI_MAP_PORT3m,
    IDB_OBM5_PRI_MAP_PORT0m,
    IDB_OBM5_PRI_MAP_PORT1m,
    IDB_OBM5_PRI_MAP_PORT2m,
    IDB_OBM5_PRI_MAP_PORT3m,
    IDB_OBM6_PRI_MAP_PORT0m,
    IDB_OBM6_PRI_MAP_PORT1m,
    IDB_OBM6_PRI_MAP_PORT2m,
    IDB_OBM6_PRI_MAP_PORT3m,
    IDB_OBM7_PRI_MAP_PORT0m,
    IDB_OBM7_PRI_MAP_PORT1m,
    IDB_OBM7_PRI_MAP_PORT2m,
    IDB_OBM7_PRI_MAP_PORT3m
   };

static soc_monterey_shadow_t soc_monterey_shadow[SOC_MAX_NUM_DEVICES];

int
_soc_monterey_obm_cb_register(int unit , bcm_obm_callback_fn fn , void *fn_data)
{
    
    int pgw = 0 ;
    soc_reg_t  reg;
    int pgw_inst = 0 ;
    uint64 rval64; 

    if ( soc_monterey_obm_cb[unit].user_data != NULL) {
        return SOC_E_EXISTS;
    }
    for (pgw = 0; pgw < MONTEREY_PGWS_PER_DEV ;pgw++)
    {
         pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
         reg = PGW_CA_OVERFLOW_STATUSr;
         SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst ,0, &rval64));
         reg = PGW_OBM_DROP_STATUSr;
         SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst,0, &rval64));
    } 
    soc_monterey_obm_cb[unit].cb = fn;
    soc_monterey_obm_cb[unit].user_data = fn_data;
    return SOC_E_NONE;
}

int
_soc_monterey_obm_cb_unregister(int unit , bcm_obm_callback_fn fn , void *fn_data)
{
    if ( soc_monterey_obm_cb[unit].user_data  == NULL ) {
        return SOC_E_NOT_FOUND;
    }
    soc_monterey_obm_cb[unit].cb = NULL;
    soc_monterey_obm_cb[unit].user_data = NULL;
    return SOC_E_NONE;
}

STATIC int
soc_monterey_shadow_get_mem_idx(int unit, soc_mem_t mem, int *mem_idx)
{
    if (mem_idx == NULL) {
        return SOC_E_PARAM;
    }
    switch(mem){
        case MMU_INTFI_XPIPE_FC_MAP_TBL0m:
            *mem_idx = 0;
            break;
        case MMU_INTFI_XPIPE_FC_MAP_TBL1m:
            *mem_idx = 1;
            break;
        case IDB_OBM0_DSCP_MAP_PORT0m:
            *mem_idx = 2;
            break;
        case IDB_OBM0_DSCP_MAP_PORT1m:
            *mem_idx = 3;
            break;
        case IDB_OBM0_DSCP_MAP_PORT2m:
            *mem_idx = 4;
            break;
        case IDB_OBM0_DSCP_MAP_PORT3m:
            *mem_idx = 5;
            break;
        case IDB_OBM1_DSCP_MAP_PORT0m:
            *mem_idx = 6;
            break;
        case IDB_OBM1_DSCP_MAP_PORT1m:
            *mem_idx = 7;
            break;
        case IDB_OBM1_DSCP_MAP_PORT2m:
            *mem_idx = 8;
            break;
        case IDB_OBM1_DSCP_MAP_PORT3m:
            *mem_idx = 9;
            break;
        case IDB_OBM2_DSCP_MAP_PORT0m:
            *mem_idx = 10;
            break;
        case IDB_OBM2_DSCP_MAP_PORT1m:
            *mem_idx = 11;
            break;
        case IDB_OBM2_DSCP_MAP_PORT2m:
            *mem_idx = 12;
            break;
        case IDB_OBM2_DSCP_MAP_PORT3m:
            *mem_idx = 13;
            break;
        case IDB_OBM3_DSCP_MAP_PORT0m:
            *mem_idx = 14;
            break;
        case IDB_OBM3_DSCP_MAP_PORT1m:
            *mem_idx = 15;
            break;
        case IDB_OBM3_DSCP_MAP_PORT2m:
            *mem_idx = 16;
            break;
        case IDB_OBM3_DSCP_MAP_PORT3m:
            *mem_idx = 17;
            break;
        case IDB_OBM4_DSCP_MAP_PORT0m:
            *mem_idx = 18;
            break;
        case IDB_OBM4_DSCP_MAP_PORT1m:
            *mem_idx = 19;
            break;
        case IDB_OBM4_DSCP_MAP_PORT2m:
            *mem_idx = 20;
            break;
        case IDB_OBM4_DSCP_MAP_PORT3m:
            *mem_idx = 21;
            break;
        case IDB_OBM5_DSCP_MAP_PORT0m:
            *mem_idx = 22;
            break;
        case IDB_OBM5_DSCP_MAP_PORT1m:
            *mem_idx = 23;
            break;
        case IDB_OBM5_DSCP_MAP_PORT2m:
            *mem_idx = 24;
            break;
        case IDB_OBM5_DSCP_MAP_PORT3m:
            *mem_idx = 25;
            break;
        case IDB_OBM6_DSCP_MAP_PORT0m:
            *mem_idx = 26;
            break;
        case IDB_OBM6_DSCP_MAP_PORT1m:
            *mem_idx = 27;
            break;
        case IDB_OBM6_DSCP_MAP_PORT2m:
            *mem_idx = 28;
            break;
        case IDB_OBM6_DSCP_MAP_PORT3m:
            *mem_idx = 29;
            break;
        case IDB_OBM7_DSCP_MAP_PORT0m:
            *mem_idx = 30;
            break;
        case IDB_OBM7_DSCP_MAP_PORT1m:
            *mem_idx = 31;
            break;
        case IDB_OBM7_DSCP_MAP_PORT2m:
            *mem_idx = 32;
            break;
        case IDB_OBM7_DSCP_MAP_PORT3m:
            *mem_idx = 33;
            break;
        case IDB_OBM0_PRI_MAP_PORT0m:
            *mem_idx = 34;
            break;
        case IDB_OBM0_PRI_MAP_PORT1m:
            *mem_idx = 35;
            break;
        case IDB_OBM0_PRI_MAP_PORT2m:
            *mem_idx = 36;
            break;
        case IDB_OBM0_PRI_MAP_PORT3m:
            *mem_idx = 37;
            break;
        case IDB_OBM1_PRI_MAP_PORT0m:
            *mem_idx = 38;
            break;
        case IDB_OBM1_PRI_MAP_PORT1m:
            *mem_idx = 39;
            break;
        case IDB_OBM1_PRI_MAP_PORT2m:
            *mem_idx = 40;
            break;
        case IDB_OBM1_PRI_MAP_PORT3m:
            *mem_idx = 41;
            break;
        case IDB_OBM2_PRI_MAP_PORT0m:
            *mem_idx = 42;
            break;
        case IDB_OBM2_PRI_MAP_PORT1m:
            *mem_idx = 43;
            break;
        case IDB_OBM2_PRI_MAP_PORT2m:
            *mem_idx = 44;
            break;
        case IDB_OBM2_PRI_MAP_PORT3m:
            *mem_idx = 45;
            break;
        case IDB_OBM3_PRI_MAP_PORT0m:
            *mem_idx = 46;
            break;
        case IDB_OBM3_PRI_MAP_PORT1m:
            *mem_idx = 47;
            break;
        case IDB_OBM3_PRI_MAP_PORT2m:
            *mem_idx = 48;
            break;
        case IDB_OBM3_PRI_MAP_PORT3m:
            *mem_idx = 49;
            break;
        case IDB_OBM4_PRI_MAP_PORT0m:
            *mem_idx = 50;
            break;
        case IDB_OBM4_PRI_MAP_PORT1m:
            *mem_idx = 51;
            break;
        case IDB_OBM4_PRI_MAP_PORT2m:
            *mem_idx = 52;
            break;
        case IDB_OBM4_PRI_MAP_PORT3m:
            *mem_idx = 53;
            break;
        case IDB_OBM5_PRI_MAP_PORT0m:
            *mem_idx = 54;
            break;
        case IDB_OBM5_PRI_MAP_PORT1m:
            *mem_idx = 55;
            break;
        case IDB_OBM5_PRI_MAP_PORT2m:
            *mem_idx = 56;
            break;
        case IDB_OBM5_PRI_MAP_PORT3m:
            *mem_idx = 57;
            break;
        case IDB_OBM6_PRI_MAP_PORT0m:
            *mem_idx = 58;
            break;
        case IDB_OBM6_PRI_MAP_PORT1m:
            *mem_idx = 59;
            break;
        case IDB_OBM6_PRI_MAP_PORT2m:
            *mem_idx = 60;
            break;
        case IDB_OBM6_PRI_MAP_PORT3m:
            *mem_idx = 61;
            break;
        case IDB_OBM7_PRI_MAP_PORT0m:
            *mem_idx = 62;
            break;
        case IDB_OBM7_PRI_MAP_PORT1m:
            *mem_idx = 63;
            break;
        case IDB_OBM7_PRI_MAP_PORT2m:
            *mem_idx = 64;
            break;
        case IDB_OBM7_PRI_MAP_PORT3m:
            *mem_idx = 65;
            break;
        default:
            return SOC_E_NOT_FOUND;

    }
    return SOC_E_NONE;
}
static int _ap_invalid_ptr[SOC_MAX_NUM_DEVICES][SOC_MONTEREY_NODE_LVL_MAX];
#define INVALID_PARENT(unit, level)   _soc_monterey_invalid_parent_index((unit),(level))

STATIC int
soc_monterey_init_invalid_parents(int unit)
{

    
    _ap_invalid_ptr[unit][SOC_MONTEREY_NODE_LVL_ROOT] = -1;
    _ap_invalid_ptr[unit][SOC_MONTEREY_NODE_LVL_S1] = 0x7f ;
    _ap_invalid_ptr[unit][SOC_MONTEREY_NODE_LVL_L0] =
        soc_mem_index_max(unit, LLS_S1_PARENTm);
    _ap_invalid_ptr[unit][SOC_MONTEREY_NODE_LVL_L1] =
        soc_mem_index_max(unit, LLS_L0_PARENTm);
    _ap_invalid_ptr[unit][SOC_MONTEREY_NODE_LVL_L2] =
        soc_mem_index_max(unit, LLS_L1_PARENTm);
    return SOC_E_NONE;
}

int _soc_monterey_invalid_parent_index(int unit, int level)
{
    return _ap_invalid_ptr[unit][level];
}

#define _SOC_MONTEREY_DEF_SCHED_NODE_BASE(u,p,cfg,t)   \
            (cfg)[(t)].base_factor * (p)

soc_monterey_sched_type_t
_soc_monterey_port_sched_type_get(int unit, soc_port_t port)
{
    soc_info_t *si;

    si = &SOC_INFO(unit);
    /* On Monterey everything is HSP */
    return (SOC_PBMP_MEMBER(si->eq_pbm, port) || SOC_IS_MONTEREY(unit)) ?
        SOC_MONTEREY_SCHED_HSP : SOC_MONTEREY_SCHED_LLS;
}

int soc_monterey_port_common_attributes_get(int unit, int port, int *pipe,
                                       int *mmu_port, int *phy_port)
{
    int lphy_port, lmmu_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    if (pipe) {
        *pipe = 0;
    }

    lphy_port = si->port_l2p_mapping[port];
    lmmu_port = si->port_p2m_mapping[lphy_port];

    if (phy_port) {
        *phy_port = lphy_port;
    }

    if (mmu_port) {
        *mmu_port = lmmu_port;
    }
    return 0;
}

int _soc_monterey_root_scheduler_index(int unit, int port)
{
    int mmu_port, in_pipe;

    SOC_IF_ERROR_RETURN(
        soc_monterey_port_common_attributes_get(unit, port, &in_pipe, &mmu_port, NULL));

    return mmu_port;
}

int _soc_monterey_s1_scheduler_index(int unit, int port, int level, int hw_index)
{
    soc_mem_t mem;
    int c_parent;
    uint32 entry[SOC_MAX_MEM_WORDS];
    if(level == SOC_MONTEREY_NODE_LVL_S1) {
        return hw_index;
    } else if (level >= SOC_MONTEREY_NODE_LVL_L0) {
        mem = _SOC_MONTEREY_NODE_PARENT_MEM(unit, port, level);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
        c_parent = soc_mem_field32_get(unit, mem, entry, 
                C_PARENTf);
        if(level == SOC_MONTEREY_NODE_LVL_L0) {
            return c_parent;
        }
        mem = _SOC_MONTEREY_NODE_PARENT_MEM(unit, port, level - 1);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, c_parent, &entry));
        c_parent = soc_mem_field32_get(unit, mem, entry, 
                C_PARENTf);
        if(level == SOC_MONTEREY_NODE_LVL_L1) {
            return c_parent;
        }
        mem = _SOC_MONTEREY_NODE_PARENT_MEM(unit, port, level - 2);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, c_parent, &entry));
        c_parent = soc_mem_field32_get(unit, mem, entry, 
                C_PARENTf);
        if(level == SOC_MONTEREY_NODE_LVL_L2) {
            return c_parent;
        }

    }
    return -1;
}

STATIC int
_soc_monterey_flush_queue(int unit, int port, int queue_hw_index)
{
    return 0;
}

soc_error_t
soc_monterey_cosq_disable_hsp_sched(int unit, soc_port_t in_port)
{
    int mmu_port;
    uint32 fval, rval;

    SOC_IF_ERROR_RETURN
        (soc_monterey_port_common_attributes_get(unit, in_port, NULL, &mmu_port, NULL));
    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_GLOBAL_CONFIGr(unit, &rval));
    fval = soc_reg_field_get(unit, HSP_SCHED_GLOBAL_CONFIGr, rval, IS_HSP_PORT_IN_XPIPEf);
    fval &= ~(1 << mmu_port);
    soc_reg_field_set(unit, HSP_SCHED_GLOBAL_CONFIGr, &rval, IS_HSP_PORT_IN_XPIPEf, fval);
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_GLOBAL_CONFIGr(unit, rval));
    return SOC_E_NONE;
}

int
soc_monterey_cosq_enable_hsp_sched(int unit, soc_port_t in_port)
{
    int mmu_port;
    uint32 fval, rval;

    SOC_IF_ERROR_RETURN(
        soc_monterey_port_common_attributes_get(unit, in_port, NULL, &mmu_port, NULL));
    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_GLOBAL_CONFIGr(unit, &rval));
    fval = soc_reg_field_get(unit, HSP_SCHED_GLOBAL_CONFIGr, rval, 
            IS_HSP_PORT_IN_XPIPEf);
    fval |= (1 << mmu_port);
    soc_reg_field_set(unit, HSP_SCHED_GLOBAL_CONFIGr, &rval, 
            IS_HSP_PORT_IN_XPIPEf, fval);
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_GLOBAL_CONFIGr(unit, rval));
    return SOC_E_NONE;
}


int
soc_monterey_cosq_set_sched_parent(int unit, soc_port_t port,
                              int level, int hw_index,
                              int parent_hw_idx, int add)
{
    soc_monterey_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS], fval, rval;
    soc_mem_t mem;
    int l0off, l1off;
    uint32 *bmap = NULL;
    soc_port_t mmu_port;
    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%d : %d parent:%d\n"),
               port, level - 1, hw_index, parent_hw_idx));

    sched_type = _soc_monterey_port_sched_type_get(unit, port);

    if (sched_type == SOC_MONTEREY_SCHED_LLS) {
        mem = _SOC_MONTEREY_NODE_PARENT_MEM(unit, port, level);
        if (mem == INVALIDm) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
#if 0
        /* disconnecting the queue , flush it */
        if ((parent_hw_idx == INVALID_PARENT(unit, level)) && 
                (level == SOC_MONTEREY_NODE_LVL_L2)) {
            SOC_IF_ERROR_RETURN(_soc_monterey_flush_queue(unit, port, hw_index));
        }
#endif

        soc_mem_field32_set(unit, mem, entry,
                level == SOC_MONTEREY_NODE_LVL_S1 ? P_PORTf : 
                C_PARENTf, add ? parent_hw_idx: INVALID_PARENT(unit, level));
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
    } else if (sched_type == SOC_MONTEREY_SCHED_HSP) {
        if (level == SOC_MONTEREY_NODE_LVL_L1) {
            l0off = parent_hw_idx % 5;
            l1off = hw_index % 10;
            SOC_IF_ERROR_RETURN(
                    soc_monterey_port_common_attributes_get(unit, port, 
                        NULL, &mmu_port, NULL));


            if (parent_hw_idx == INVALID_PARENT(unit, level)) {
                SOC_IF_ERROR_RETURN(_soc_monterey_flush_queue(unit,mmu_port, hw_index));
            }
            if (l0off == 4) {
                SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr(
                        unit, port, &rval));

                fval = soc_reg_field_get(unit, HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr,
                        rval, CHILDREN_CONNECTION_MAP_4f);
            } else {
                
                SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                        unit, port, l0off - 1, &rval));

                fval = soc_reg_field_get(unit, HSP_SCHED_L0_NODE_CONNECTION_CONFIGr, 
                       rval, CHILDREN_CONNECTION_MAPf);
            }
            if (l1off >= 8) {
                l1off -= 8;
            }

            if (!add) {
                fval &= ~(1 << l1off);
            } else {
                fval |= 1 << l1off;
            }
            if (l0off == 4) {
                soc_reg_field_set(unit, HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr,
                        &rval, CHILDREN_CONNECTION_MAP_4f, fval);

                SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr(
                            unit, port, rval));
            } else {
                soc_reg_field_set(unit, HSP_SCHED_L0_NODE_CONNECTION_CONFIGr, 
                        &rval, CHILDREN_CONNECTION_MAPf, fval);

                SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                            unit, port, l0off - 1, rval));
            }
        }
        if (SOC_IS_MONTEREY(unit)) {
            return SOC_E_NONE;
        }
    }
    if (level == SOC_MONTEREY_NODE_LVL_S1) {
        bmap = SOC_CONTROL(unit)->port_lls_s1_bmap[port];
    } else if (level == SOC_MONTEREY_NODE_LVL_L0) {
        bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
    } else if (level == SOC_MONTEREY_NODE_LVL_L1) {
        bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
    } else if (level == SOC_MONTEREY_NODE_LVL_L2) {
        bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
    } else {
        return SOC_E_PARAM;
    }
    
    SOC_LLS_SCHEDULER_LOCK(unit);
    if (!add) {
        SHR_BITCLR(bmap, hw_index);
    } else {
        SHR_BITSET(bmap, hw_index);
    }
    SOC_LLS_SCHEDULER_UNLOCK(unit);

    return SOC_E_NONE;
}
STATIC int
soc_monterey_sched_weight_set(int unit, int port, int level,
                              int hw_index, int weight)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
   soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;

    if (level == SOC_MONTEREY_NODE_LVL_L0) {
       hw_index = hw_index;
        mem = HSP_SCHED_L0_NODE_WEIGHTm;
        field = L0_NODE_WEIGHTf;
    } else if (level == SOC_MONTEREY_NODE_LVL_L1) {
        hw_index = hw_index;
        mem = HSP_SCHED_L1_NODE_WEIGHTm;
        field  = L1_NODE_WEIGHTf;
    } else if (level == SOC_MONTEREY_NODE_LVL_L2) {
        if (hw_index < 640) {
            mem = HSP_SCHED_L2_UC_QUEUE_WEIGHTm;
            hw_index = hw_index;
            field = L2_UC_QUEUE_WEIGHTf;
        } else {
            mem = HSP_SCHED_L2_MC_QUEUE_WEIGHTm;
            hw_index = (hw_index - 640);
            field = L2_MC_QUEUE_WEIGHTf;
        }
   } else {
        /*Wrong level */
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
           (soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
    soc_mem_field32_set(unit, mem, &entry, field, weight);
    SOC_IF_ERROR_RETURN
           (soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
    return SOC_E_NONE;
}

STATIC int
soc_monterey_sched_weight_get(int unit, int port, int level,
                              int hw_index, int *weight)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;

    if (level == SOC_MONTEREY_NODE_LVL_L0) {
        hw_index = hw_index;
        mem = HSP_SCHED_L0_NODE_WEIGHTm;
        field = L0_NODE_WEIGHTf;
    } else if (level == SOC_MONTEREY_NODE_LVL_L1) {
        hw_index = hw_index;
        mem = HSP_SCHED_L1_NODE_WEIGHTm;
        field = L1_NODE_WEIGHTf;
    } else if (level == SOC_MONTEREY_NODE_LVL_L2) {
        if (hw_index < 640) {
            mem = HSP_SCHED_L2_UC_QUEUE_WEIGHTm;
            hw_index = hw_index;
            field = L2_UC_QUEUE_WEIGHTf;
        } else {
            mem = HSP_SCHED_L2_MC_QUEUE_WEIGHTm;
            hw_index = (hw_index - 640);
            field = L2_MC_QUEUE_WEIGHTf;
        }
    } else {
        /*Wrong level */
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
           (soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
    *weight = soc_mem_field32_get(unit, mem, &entry, field);
    return SOC_E_NONE;
}

int
soc_monterey_cosq_set_sched_mode(int unit, soc_port_t port, int level, int index,
                            soc_monterey_sched_mode_e mode, int weight)
{
    soc_monterey_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS], mfval = 0, rval = 0, wrr_mask, sp_mask;
    soc_mem_t mem;
    soc_reg_t reg, reg2;
    uint32 mc_group_mode;
    int fval, idx, parent_idx = -1, index_save = index;

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d MODE=%d WT=%d\n"),
               port, (level == 0) ? "r" : "", level - 1, 
               index, mode, weight));

    reg = INVALIDr;
    reg2 = INVALIDr;
    mem = INVALIDm;
    sched_type = _soc_monterey_port_sched_type_get(unit, port);
    if (sched_type == SOC_MONTEREY_SCHED_HSP) {
        /* get the port relative index / offset */
        if (level == SOC_MONTEREY_NODE_LVL_L0) {
            index = index % 5;
            reg = HSP_SCHED_PORT_CONFIGr;
            reg2 = HSP_SCHED_L0_NODE_CONFIGr;
            parent_idx = 0;
        } else if (level == SOC_MONTEREY_NODE_LVL_L1) {
            index = index % 10;
            reg = HSP_SCHED_L0_NODE_CONFIGr;
            reg2 = HSP_SCHED_L1_NODE_CONFIGr;
            /* Find the Parent index for the current child */
            for (idx = 1; idx < 5; idx++) {
                 if (idx == 4) {
                     SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr(
                                                         unit, port, &rval));
                     fval = soc_reg_field_get(unit,
                                 HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr,
                                 rval, CHILDREN_CONNECTION_MAP_4f);
                } else { 
                    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                                                       unit, port, idx - 1, &rval));
                    fval = soc_reg_field_get(unit,
                                HSP_SCHED_L0_NODE_CONNECTION_CONFIGr,
                               rval, CHILDREN_CONNECTION_MAPf);
                }
                if (idx == 4) {
                    if ((index >= 8) && (fval & (1 << (index - 8)))) {
                        parent_idx = idx;
                        break;
                    }
                } else {
                    if (fval & (1 << index)) {
                        parent_idx = idx;
                        break;
                    }
                }
            }
        } else if (level == SOC_MONTEREY_NODE_LVL_L2) {
            mc_group_mode = soc_reg_field_get( unit,
                    HSP_SCHED_PORT_CONFIGr, rval,
                    MC_GROUP_MODEf);
            reg = HSP_SCHED_L1_NODE_CONFIGr;
            if (index >= _MN_MAX_NUM_UC_QUEUE(unit)) {
                reg2 = HSP_SCHED_L2_MC_QUEUE_CONFIGr;
            } else {
                reg2 = HSP_SCHED_L2_UC_QUEUE_CONFIGr;
            }
            if (mc_group_mode && (index >= _MN_MAX_NUM_UC_QUEUE(unit))) {
                parent_idx = (index - _MN_MAX_NUM_UC_QUEUE(unit))  % 10;
                if ( parent_idx  < _MN_HSP_PORT_MAX_COS) {
                    reg = HSP_SCHED_L0_NODE_CONFIGr;
                    parent_idx = 0;
                }
            }else {
                parent_idx = index  % 10;
            }  
        } else {
            return SOC_E_PARAM;
        }

        if (parent_idx == -1) {
            return SOC_E_INTERNAL;
        }

        if (mode == SOC_MONTEREY_SCHED_MODE_STRICT) {
            weight = 0;
        } else if (mode == SOC_MONTEREY_SCHED_MODE_WRR) {
            mfval = 1;
        } else if (mode == SOC_MONTEREY_SCHED_MODE_WDRR) {
            mfval = 0;
        } else {
            return SOC_E_PARAM;
        }

        /* selection between SP and WxRR is based on weight property. */
        SOC_IF_ERROR_RETURN(soc_monterey_sched_weight_set(unit, port, 
                                                level, index_save, weight));
        if (mode != SOC_MONTEREY_SCHED_MODE_STRICT) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            wrr_mask = soc_reg_field_get(unit, reg, rval, ENABLE_WRRf);
            wrr_mask &= ~(1 << parent_idx);
            wrr_mask |= (mfval << parent_idx);
            soc_reg_field_set(unit, reg, &rval, ENABLE_WRRf, wrr_mask);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg2, port, 0, &rval));
        sp_mask = soc_reg_field_get(unit, reg2, rval, ENABLE_SP_IN_MINf);
        if (mode == SOC_MONTEREY_SCHED_MODE_STRICT) {
            sp_mask |= (1 << (index % 10));
        } else {
            sp_mask &= ~(1 << (index % 10));
        }
        soc_reg_field_set(unit, reg2, &rval, ENABLE_SP_IN_MINf, sp_mask);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg2, port, 0, rval));
    } else {
        if (mode == SOC_MONTEREY_SCHED_MODE_STRICT) {
            weight = 0;
        }
        
        SOC_IF_ERROR_RETURN(soc_monterey_sched_weight_set(unit, port, 
                                                level, index, weight));

        if (mode != SOC_MONTEREY_SCHED_MODE_STRICT) { 
            mem = _SOC_MONTEREY_NODE_CONFIG_MEM(unit, port, SOC_MONTEREY_NODE_LVL_S1);
            index = _soc_monterey_s1_scheduler_index(unit, port,  level, index);
            if (index == -1) {
                return SOC_E_INTERNAL;
            }
            SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
            soc_mem_field32_set(unit, mem, entry, 
                    PACKET_MODE_WRR_ACCOUNTING_ENABLEf, 
                    (mode == SOC_MONTEREY_SCHED_MODE_WRR) ? 1 : 0);
            SOC_IF_ERROR_RETURN(
                    soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_monterey_hsp_sp_node_init
 * Purpose:
 *     This function initialises ENABLE_SP_IN_MIN setting in
 *     HSP_SCHED_Lx_NODE_CONFIG.
 *     Hardware default value for sched mode is SP. SP nodes
 *     with min bandwidth set may be scheduled in WRR manner.
 *     To schedule SP nodes with strict priority, ENABLE_SP_IN_MIN
 *     must be set.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number
 * Returns:
 *     SOC_E_XXX
 */
int
soc_monterey_hsp_sp_node_init(int unit, soc_port_t port)
{
    uint32 index, rval, fval;
    soc_reg_t regs[4] = {
            HSP_SCHED_L0_NODE_CONFIGr,
            HSP_SCHED_L1_NODE_CONFIGr,
            HSP_SCHED_L2_UC_QUEUE_CONFIGr,
            HSP_SCHED_L2_MC_QUEUE_CONFIGr
        };

    for (index = 0; index < COUNTOF(regs); index++) {
        if (regs[index] == HSP_SCHED_L0_NODE_CONFIGr) {
            fval = 0x1f;
        } else {
            fval = 0x3ff;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, regs[index], port, 0, &rval));
        soc_reg_field_set(unit, regs[index], &rval, ENABLE_SP_IN_MINf, fval);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, regs[index], port, 0, rval));
    }

    return SOC_E_NONE;
}

/* number of registers in the set to support Dynamic Scheduling */
#define _SOC_MONTEREY_DYN_REGISTER_SET    4  

/* Structure to maintain the ports that are undergoing dyn sched*/
typedef struct _soc_monterey_port_dyn_sched_s {
    sal_mutex_t     lock;
    int             port[_SOC_MONTEREY_DYN_REGISTER_SET];
    int             _soc_monterey_init_done[SOC_MAX_NUM_DEVICES];
} _soc_monterey_port_dyn_sched_t;

static _soc_monterey_port_dyn_sched_t _monterey_dyn_sched_unit_data[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *     _soc_monterey_alloc_dyn_set
 * Purpose:
 *     This function allocates the register available from
 *     the set of SP_WERR_DYN_CHANGEnA/nB/nC (n=0,1,2,3) to the port and 
 *     updates the structure _monterey_dyn_sched_unit_data to keep track 
 *     of the used registers from the set.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number
 *     r_a        - (OUT) SP_WERR_DYN_CHANGEnA register
 *     r_b        - (OUT) SP_WERR_DYN_CHANGEnB register
 *     r_ctrl     - (OUT) SP_WERR_DYN_CHANGEnC register
 * Returns:
 *     SOC_E_XXX
 */
STATIC int _soc_monterey_alloc_dyn_set(int unit, int port, 
            soc_reg_t *r_a, soc_reg_t *r_b, soc_reg_t *r_ctrl)
{
    _soc_monterey_port_dyn_sched_t *pcb;
    int i, empty_at = -1, rv = SOC_E_NONE;

    soc_reg_t dyn_change_a_x[] = {
                                       LLS_SP_WERR_DYN_CHANGE_0Ar,
                                       LLS_SP_WERR_DYN_CHANGE_1Ar,
                                       LLS_SP_WERR_DYN_CHANGE_2Ar,
                                       LLS_SP_WERR_DYN_CHANGE_3Ar
                                 };
    soc_reg_t dyn_change_b_x[] = {
                                       LLS_SP_WERR_DYN_CHANGE_0Br,
                                       LLS_SP_WERR_DYN_CHANGE_1Br,
                                       LLS_SP_WERR_DYN_CHANGE_2Br,
                                       LLS_SP_WERR_DYN_CHANGE_3Br
                                 };
    
    soc_reg_t dyn_change_ctrl_x[] = {
                                       LLS_SP_WERR_DYN_CHANGE_0Cr,
                                       LLS_SP_WERR_DYN_CHANGE_1Cr,
                                       LLS_SP_WERR_DYN_CHANGE_2Cr,
                                       LLS_SP_WERR_DYN_CHANGE_3Cr
                                    };

    pcb = &_monterey_dyn_sched_unit_data[unit];
    
    sal_mutex_take(pcb->lock, sal_sem_FOREVER);
    for (i = 0; i < _SOC_MONTEREY_DYN_REGISTER_SET; i++) {
        if (pcb->port[i] == -1) {
            empty_at = i;
            break;
        } else if (pcb->port[i] == port) {
            rv = SOC_E_BUSY;
            break;
        }
    }

    if (!rv && (empty_at >= 0)) {
        pcb->port[empty_at] = port;
    }
    
    sal_mutex_give(pcb->lock);

    if (rv) {
        return rv;
    }

    if (empty_at == -1) {
        return SOC_E_BUSY;
    }
    *r_a = dyn_change_a_x[empty_at];
    *r_b = dyn_change_b_x[empty_at];
    *r_ctrl = dyn_change_ctrl_x[empty_at];
    return SOC_E_NONE;
}

/*
 * Function:
 *     _soc_monterey_free_dyn_set
 * Purpose:
 *     This function is called after the switchover is done 
 *     to update the structure _monterey_dyn_sched_unit_data by  
 *     removing the port from it. This makes the register
 *     used for the request available for the next request.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number
 * Returns:
 *     SOC_E_XXX
 */
STATIC int _soc_monterey_free_dyn_set(int unit, int port)
{
    _soc_monterey_port_dyn_sched_t *pcb;
    int i;

    pcb = &_monterey_dyn_sched_unit_data[unit];

    sal_mutex_take(pcb->lock, sal_sem_FOREVER);
    for (i = 0; i < _SOC_MONTEREY_DYN_REGISTER_SET; i++) {
        if (pcb->port[i] == port) {
            pcb->port[i] = -1;
        }
    }
    sal_mutex_give(pcb->lock);
    
    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_monterey_cosq_set_sched_child_config_dynamic
 * Purpose:
 *     This function enables the spri_vect_mode by setting the 
 *     spri_vect_mode_enable field to 1 in the LLS_CONFIG0 reg, which changes 
 *     p_num_spri[3:0] to p_vect_spri[7:0]. It allocates the register from set
 *     of SP_WERR_DYN_CHNG_nA/nB/nC registers to the port. It then sets the 
 *     SP_WERR_DYN_CHNG_nB/nC reg with the new value for the node id's parent.
 *     It sets the fields in SP_WERR_DYN_CHNG_nA with the info about the node
 *     to be switched like node_id, node_level and parent_id along with in_use
 *     field to 1, which triggers the request in H/W. The H/W clears in_use
 *     field to 0 once the switchover is complete, so we keep polling its
 *     to know about the completion of the request. 
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number
 *     level      - (IN) level of the node
 *     index      - (IN) index of the node
 *     num_spri   - (IN) number of SPRI child nodes
 *     first_child - (IN) first child node
 *     first_mc_child - (IN) first multicast child node
 *     ucmap        - (IN) unicast map
 *     spmap        - (IN) spri map
 *     child_index     - (IN) index number of the child
 * Returns:
 *     SOC_E_XXX
 */
int
soc_monterey_cosq_set_sched_child_config_dynamic(int unit, soc_port_t port,
                                            int level, int index, 
                                            int num_spri, int first_child,
                                            int first_mc_child, uint32 ucmap,
                                            uint32 spmap, int child_index)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval, d32, timeout_val, old_spmap, f1, f2;
    soc_mem_t mem;
    int rv = SOC_E_NONE;
    soc_reg_t r_a = INVALIDr, r_b = INVALIDr, r_ctrl = INVALIDr;
    soc_timeout_t timeout;

    if (level == SOC_MONTEREY_NODE_LVL_ROOT) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(_soc_monterey_alloc_dyn_set(unit, port, 
                                               &r_a, &r_b, &r_ctrl));

    mem = _SOC_MONTEREY_NODE_CONFIG_MEM(unit, port, level);
    if (mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }

    level -= 1;
    /* Getting the old spmap */    
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    f1 = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
    f2 = soc_mem_field32_get(unit, mem, &entry, P_VECT_SPRI_7_4f);
    old_spmap = f1 | (f2 << 4);

    LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit,
            "Port:%d L%s%d config : index=%d FC=%d FMC=%d UMAP=0x%x NUMSP=%d\n"), 
             port, (level==0) ? "r" : "", level, 
             index, first_child, first_mc_child, ucmap, num_spri));

    /* Setting the memory fields based on the new spmap */
    soc_mem_field32_set(unit, mem, &entry, P_NUM_SPRIf, spmap & 0xf);
    soc_mem_field32_set(unit, mem, &entry, P_VECT_SPRI_7_4f, (spmap >> 4) & 0xf);

    if (mem == LLS_L1_CONFIGm) {
        soc_mem_field32_set(unit, mem, &entry, P_START_UC_SPRIf, first_child);
        soc_mem_field32_set(unit, mem, &entry, P_START_MC_SPRIf, first_mc_child);
        soc_mem_field32_set(unit, mem, &entry, P_SPRI_SELECTf, 
                            (num_spri > 0) ? ucmap : 0);
    } else {
        soc_mem_field32_set(unit, mem, &entry, P_START_SPRIf, first_child);
    }

    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));

    rval = 0;
    soc_bits_get(entry, 0, 31, &d32);
    soc_reg_field_set(unit, r_ctrl, &rval, LLS_PARENT_CONFIGf, d32);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, r_ctrl, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_bits_get(entry, 32, 45, &d32);
    soc_reg_field_set(unit, r_b, &rval, LLS_PARENT_CONFIGf, d32);
    soc_reg_field_set(unit, r_b, &rval, LLS_PARENT_CONFIG_ORIGINAL_SP_VECTORf,
                                            old_spmap);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, r_b, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, r_a, &rval, NODE_LEVELf, level + 1);
    soc_reg_field_set(unit, r_a, &rval, NODE_IDf, child_index);
    soc_reg_field_set(unit, r_a, &rval, PARENT_IDf, index);
    soc_reg_field_set(unit, r_a, &rval, IN_USEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, r_a, REG_PORT_ANY, 0, rval));

    if (!SAL_BOOT_SIMULATION) {
        timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 20000);
        soc_timeout_init(&timeout, timeout_val, 0);

        do {
            SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, r_a, REG_PORT_ANY, 0, &rval));
                
            if (soc_timeout_check(&timeout)) {
                rv = SOC_E_TIMEOUT;
                break;
            }
        } while (soc_reg_field_get(unit, r_a, rval, IN_USEf));
    }

    SOC_IF_ERROR_RETURN(_soc_monterey_free_dyn_set(unit, port));

    return rv;
}

int
soc_monterey_cosq_set_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int num_spri, int first_child, 
                              int first_mc_child, uint32 ucmap, uint32 spmap,
                              soc_monterey_sched_mode_e mode, int weight)
{
    int child_level;

    if (level >= SOC_MONTEREY_NODE_LVL_L2) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
        soc_monterey_cosq_set_sched_child_config_dynamic(unit, port,
                                level, index, num_spri, first_child, 
                                first_mc_child, ucmap, spmap, child_index));
    if (child_index >= 0) {
        soc_monterey_get_child_type(unit, port, level, &child_level);
        SOC_IF_ERROR_RETURN(soc_monterey_cosq_set_sched_mode(unit, port, 
                    child_level, 
                    child_index, mode, weight));
    }

    return SOC_E_NONE;
}

int
soc_monterey_cosq_get_sched_child_config(int unit, soc_port_t port,
                              int level, int index,
                              int *pnum_spri, int *pfirst_sp_child, 
                              int *pfirst_sp_mc_child, uint32 *pucmap, uint32 *pspmap)
{
    soc_monterey_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 num_spri = 0, ucmap = 0, f1, f2;
    int first_sp_child = -1, first_sp_mc_child = -1,ii;
    soc_mem_t mem;

    sched_type = _soc_monterey_port_sched_type_get(unit, port);

    mem = INVALIDm;

    if (sched_type == SOC_MONTEREY_SCHED_HSP) {
        return SOC_E_PARAM;
    }

    mem = _SOC_MONTEREY_NODE_CONFIG_MEM(unit, port, level);
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    num_spri = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);

    f1 = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
    f2 = soc_mem_field32_get(unit, mem, &entry, P_VECT_SPRI_7_4f);
    *pspmap = f1 | (f2 << 4);
    for (ii = 0; ii < 8; ii++) {
        if ((1 << ii) & *pspmap) {
            num_spri++;
        }
    }
    if (mem == LLS_L1_CONFIGm) { 
        first_sp_child = soc_mem_field32_get(unit, mem, &entry, P_START_UC_SPRIf);
        first_sp_mc_child = soc_mem_field32_get(unit, mem, 
                                         &entry, P_START_MC_SPRIf);
        ucmap = soc_mem_field32_get(unit, mem, &entry, P_SPRI_SELECTf);
    } else {
        first_sp_child = soc_mem_field32_get(unit, mem, &entry, P_START_SPRIf);
        first_sp_mc_child = 0;
    }
    
    if (num_spri == 0) {
        ucmap = 0;
    }

    if (pnum_spri) {
        *pnum_spri = num_spri;
    }
       
    if (pucmap) {
        *pucmap = ucmap;
    }

    if (pfirst_sp_child) {
        *pfirst_sp_child = first_sp_child;
    }

    if (pfirst_sp_mc_child) {
        *pfirst_sp_mc_child = first_sp_mc_child;
    }

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d FC=%d FMC=%d UMAP=0x%x\n"),
               port, (level == 0) ? "r" : "", level - 1, 
               index, first_sp_child, first_sp_mc_child, ucmap));

    return SOC_E_NONE;
}

int
soc_monterey_cosq_get_sched_mode(int unit, soc_port_t port, int level, 
                            int index, soc_monterey_sched_mode_e *pmode, int *weight)
{
    soc_monterey_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS], rval, wrr_mask;
    soc_monterey_sched_mode_e mode = SOC_MONTEREY_SCHED_MODE_UNKNOWN;
    soc_mem_t mem;
    soc_reg_t reg;
    int parent_idx  = 0;
    int  idx, fval, index_save = index;   
    uint32 mc_group_mode = 0;
    reg = INVALIDr;
    mem = INVALIDm;
    sched_type = _soc_monterey_port_sched_type_get(unit, port);

    if (sched_type == SOC_MONTEREY_SCHED_HSP) {
        /* get the port relative index / offset */
        if (level == SOC_MONTEREY_NODE_LVL_L0) {
            index = index % 5;
            reg = HSP_SCHED_PORT_CONFIGr;
            parent_idx = 0;
        } else if (level == SOC_MONTEREY_NODE_LVL_L1) {
            index = index % 10;
            reg = HSP_SCHED_L0_NODE_CONFIGr;
            /* Find the Parent index for the current child */
            for (idx = 1; idx < 5; idx++) {
                 if (idx == 4) {
                     SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr(
                                                         unit, port, &rval));
                     fval = soc_reg_field_get(unit,
                                 HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr,
                                 rval, CHILDREN_CONNECTION_MAP_4f);
                } else {
                    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                                                        unit, port, idx - 1, &rval));
                    fval = soc_reg_field_get(unit,
                                HSP_SCHED_L0_NODE_CONNECTION_CONFIGr,
                                rval, CHILDREN_CONNECTION_MAPf);
                }
                if (idx == 4) {
                    if ((index >= 8) && (fval & (1 << (index - 8)))) {
                        parent_idx = idx;
                        break;
                    }
                } else {
                    if (fval & (1 << index)) {
                        parent_idx = idx;
                        break;
                    }
                }
            }
        } else if (level == SOC_MONTEREY_NODE_LVL_L2) {
            SOC_IF_ERROR_RETURN(
                    READ_HSP_SCHED_PORT_CONFIGr( unit, port, &rval));
            mc_group_mode = soc_reg_field_get( unit,
                                               HSP_SCHED_PORT_CONFIGr, rval,
                                               MC_GROUP_MODEf);
            reg = HSP_SCHED_L1_NODE_CONFIGr;

            if (mc_group_mode && (index >= _MN_MAX_NUM_UC_QUEUE(unit))) {
                parent_idx = (index - _MN_MAX_NUM_UC_QUEUE(unit))  % 10;
                if ( parent_idx  < _MN_HSP_PORT_MAX_COS) {
                    reg = HSP_SCHED_L0_NODE_CONFIGr;
                    parent_idx = 0;
                }
            }else {
                parent_idx = index  % 10;
            }

        } else {
            return SOC_E_PARAM;
        }

        /* selection between SP and WxRR is based on weight property. */
        SOC_IF_ERROR_RETURN(soc_monterey_sched_weight_get(unit, port, 
                                                level, index_save, weight));
        if (*weight == 0) {
            mode = SOC_MONTEREY_SCHED_MODE_STRICT;
        } else {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            wrr_mask = soc_reg_field_get(unit, reg, rval, ENABLE_WRRf);
            if (wrr_mask & (1 << parent_idx)) {
                mode = SOC_MONTEREY_SCHED_MODE_WRR;
            } else {
                mode = SOC_MONTEREY_SCHED_MODE_WDRR;
            }
        }
    } else {
        SOC_IF_ERROR_RETURN(soc_monterey_sched_weight_get(unit, port, 
                        level, index, weight));
        if (*weight == 0) {
            mode = SOC_MONTEREY_SCHED_MODE_STRICT;
        } else {
            mem = _SOC_MONTEREY_NODE_CONFIG_MEM(unit, port, SOC_MONTEREY_NODE_LVL_S1);
            index = _soc_monterey_s1_scheduler_index(unit, port,  level, index);
            if (index == -1) {
                return SOC_E_INTERNAL;
            }
            SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
            if (soc_mem_field32_get(unit, mem, 
                                entry, PACKET_MODE_WRR_ACCOUNTING_ENABLEf)) {
                mode = SOC_MONTEREY_SCHED_MODE_WRR;
            } else {
                mode = SOC_MONTEREY_SCHED_MODE_WDRR;
            }
        }
    }

    if (pmode) {
        *pmode = mode;
    }

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d MODE=%d WT=%d\n"),
               port, (level == 0) ? "r" : "", level - 1, index, mode, *weight));

    return SOC_E_NONE;
}

int
soc_monterey_cosq_get_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int *pnum_spri, int *pfirst_child, 
                              int *pfirst_mc_child, uint32 *pucmap, uint32 *pspmap,
                              soc_monterey_sched_mode_e *pmode, int *weight)
{
    int child_level;

    if (level >= SOC_MONTEREY_NODE_LVL_L2) {
        return SOC_E_PARAM;
    }

    if (level != SOC_MONTEREY_NODE_LVL_ROOT) {
        SOC_IF_ERROR_RETURN(soc_monterey_cosq_get_sched_child_config(unit, port, level,
                    index, pnum_spri, pfirst_child, pfirst_mc_child, pucmap, pspmap));
    }

    if (child_index >= 0) {
        soc_monterey_get_child_type(unit, port, level, &child_level);
        SOC_IF_ERROR_RETURN(
                soc_monterey_cosq_get_sched_mode(unit, port, child_level, child_index,
                                            pmode, weight));
    }
    
    return SOC_E_NONE;
}

int soc_monterey_get_child_type(int unit, soc_port_t port, int level, 
                                    int *child_type)
{
    *child_type = -1;
    if (level == SOC_MONTEREY_NODE_LVL_ROOT) {
        *child_type = SOC_MONTEREY_NODE_LVL_S1;
    } else if (level == SOC_MONTEREY_NODE_LVL_S1) {
            *child_type = SOC_MONTEREY_NODE_LVL_L0;
    } else if (level == SOC_MONTEREY_NODE_LVL_L0) {
        *child_type = SOC_MONTEREY_NODE_LVL_L1;
    } else if (level == SOC_MONTEREY_NODE_LVL_L1) {
        *child_type = SOC_MONTEREY_NODE_LVL_L2;
    }
    return SOC_E_NONE;
}
STATIC int
_soc_monterey_alloc_sched(int unit, soc_port_t in_port, soc_monterey_node_lvl_e lvl, 
                     int offset, int *hw_index)
{
    int in_pipe, tlm = -1;
    int mmu_port, max = -1;
    soc_monterey_sched_type_t stype;
    uint32  sched_bmap[32];
    
    sal_memset(sched_bmap, 0, sizeof(uint32)*32);

    SOC_IF_ERROR_RETURN(
        soc_monterey_port_common_attributes_get(unit, in_port, &in_pipe, &mmu_port, NULL));
    stype = _soc_monterey_port_sched_type_get(unit, in_port);
    if (SOC_IS_MONTEREY(unit)) {
        if (IS_CPU_PORT(unit, in_port)) {
            mmu_port++;
        } else if (IS_LB_PORT(unit, in_port)) {
            mmu_port--;
        }
    }
    if (lvl == SOC_MONTEREY_NODE_LVL_ROOT) {
        tlm = 1;
        max = 106;
    } else if (lvl == SOC_MONTEREY_NODE_LVL_L0) {
        tlm = 5; /* max 4 L0 under HSP */
        max = 272;
    } else if (lvl == SOC_MONTEREY_NODE_LVL_L1) {
        tlm = 10;
        max = 1024;
    }

    if ((max < 0) || (tlm < 0)) {
        return SOC_E_PARAM;
    }
        
    if (stype == SOC_MONTEREY_SCHED_HSP) {
        /* For HSP ports, the indexes of the scheduler and queues are fixed.*/
        if (offset >= tlm) {
            return SOC_E_PARAM;
        }
        mmu_port += (mmu_port >= 74) ? -74 : 0;
        *hw_index = (mmu_port * tlm) + offset;
        return SOC_E_NONE;
    }  

    return SOC_E_RESOURCE;
}

int soc_monterey_l2_hw_index(int unit, int qnum, int uc)
{
    if (uc) {
        qnum -= (qnum >= 640) ? 640 : 0;
    } else {
        qnum += 640;
    }
    return qnum;
}

/*
 * Function:
 *     soc_monterey_hw_index_logical_num
 * Purpose:
 *     Get the logical queue number according to hw index
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) local port number
 *     index         -(IN) The index of Queue management related memory Tables
 *     uc            - (IN) unicast indicator
 * Returns:
 *     logical queue number: queue number in logical view
 *     In logical view,
 *     the range of unicast queue number is:   [0,16384)
 *     the range of multicast queue number is: [0,778)
 */
int
soc_monterey_hw_index_logical_num(int unit,int port,int index,int uc)
{
    int qnum;
    if (uc) {
        qnum = index;
    } else {
        
        if (SOC_IS_MONTEREY(unit)) {
            qnum =  index - 640;
        } else { 
            qnum =  index - 16384;
        }
    }

    return qnum;
}


#define AP_DFL_HSP_SCHED_MODE  SOC_MONTEREY_SCHED_MODE_WDRR

int
soc_monterey_setup_hsp_port(int unit, int port)
{
    uint32 rval, fval = 0, pbmf;
    int mmu_port, pipe, l0_index, l1_index, l0_1, l0_4, hw_index, index;

   
    SOC_IF_ERROR_RETURN(
        soc_monterey_port_common_attributes_get(unit, port, &pipe, &mmu_port, NULL));

    for (index = 0; index < 5; index++) {
        SHR_BITSET(SOC_CONTROL(unit)->port_lls_l0_bmap[port],
                (mmu_port & 0x3f) * 5 + index);
    }
    for (index = 0; index < 10; index++) {
        SHR_BITSET(SOC_CONTROL(unit)->port_lls_l1_bmap[port],
                (mmu_port & 0x3f) * 10 + index);
    }
    for (index = 0; index < 10; index++) {
        SHR_BITSET(SOC_CONTROL(unit)->port_lls_l2_bmap[port],
                (mmu_port & 0x3f) * 10 + index);
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_PORT_CONFIGr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L0_NODE_CONFIGr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L1_NODE_CONFIGr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L2_UC_QUEUE_CONFIGr(unit, port, rval));

    l0_1 = 0;
    l0_4 = 0;
    for (l0_index = 0; l0_index < 5; l0_index++) {
        SOC_IF_ERROR_RETURN(soc_monterey_sched_hw_index_get(unit, port, 
                                    SOC_MONTEREY_NODE_LVL_L0, l0_index, &hw_index));
        SOC_IF_ERROR_RETURN(soc_monterey_cosq_set_sched_parent(unit, port,
                              SOC_MONTEREY_NODE_LVL_L0, hw_index, mmu_port, 1));

        if (l0_index == 1) {
            l0_1 = hw_index;
        } else if (l0_index == 4) {
            l0_4 = hw_index;
        }
    }

    for (l1_index = 0; l1_index < 10; l1_index++) {
        SOC_IF_ERROR_RETURN(soc_monterey_sched_hw_index_get(unit, port, 
                                    SOC_MONTEREY_NODE_LVL_L1, l1_index, &hw_index));

        SOC_IF_ERROR_RETURN(soc_monterey_cosq_set_sched_parent(unit, port,
                   SOC_MONTEREY_NODE_LVL_L1, hw_index, (l1_index < 8) ? l0_1 : l0_4, 1));

        SOC_IF_ERROR_RETURN(soc_monterey_cosq_set_sched_mode(unit, port, 
                 SOC_MONTEREY_NODE_LVL_L1, l1_index, SOC_MONTEREY_SCHED_MODE_WDRR, 1));
    }

    pbmf = IS_HSP_PORT_IN_XPIPEf;
    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_GLOBAL_CONFIGr(unit, &rval));
    fval = soc_reg_field_get(unit, HSP_SCHED_GLOBAL_CONFIGr, rval, pbmf);
    fval |= (1 << mmu_port);
    soc_reg_field_set(unit, HSP_SCHED_GLOBAL_CONFIGr, &rval, pbmf, fval);
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_GLOBAL_CONFIGr(unit, rval));
    return SOC_E_NONE;
}

int
soc_monterey_delete_hsp_port(int unit, int port)
{
    uint32 rval, fval = 0, pbmf;
    int mmu_port, pipe, l0_index, index;

    SOC_IF_ERROR_RETURN(
            soc_monterey_port_common_attributes_get(unit, port, &pipe, &mmu_port, NULL));

    for (index = 0; index < 5; index++) {
        SHR_BITCLR(SOC_CONTROL(unit)->port_lls_l0_bmap[port],
                (mmu_port & 0x3f) * 5 + index);
    }
    for (index = 0; index < 10; index++) {
        SHR_BITCLR(SOC_CONTROL(unit)->port_lls_l1_bmap[port],
                (mmu_port & 0x3f) * 10 + index);
    }
    for (index = 0; index < 10; index++) {
        SHR_BITCLR(SOC_CONTROL(unit)->port_lls_l2_bmap[port],
                (mmu_port & 0x3f) * 10 + index);
    }
  
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_PORT_CONFIGr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L0_NODE_CONFIGr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L1_NODE_CONFIGr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L2_UC_QUEUE_CONFIGr(unit, port, rval));

    for (l0_index = 1; l0_index < 5; l0_index++) {
        if (l0_index == 4) {
            SOC_IF_ERROR_RETURN(
                    WRITE_HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr(unit, port,
                        rval));
        } else { 
            SOC_IF_ERROR_RETURN(
                    WRITE_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(unit, port,
                        l0_index - 1, rval));
        }
    }

    pbmf = IS_HSP_PORT_IN_XPIPEf;
    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_GLOBAL_CONFIGr(unit, &rval));
    fval = soc_reg_field_get(unit, HSP_SCHED_GLOBAL_CONFIGr, rval, pbmf);
    fval &= (~(1 << mmu_port));
    soc_reg_field_set(unit, HSP_SCHED_GLOBAL_CONFIGr, &rval, pbmf, fval);
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_GLOBAL_CONFIGr(unit, rval));
    return SOC_E_NONE;
}

/*
 * Function:
 *     _soc_monterey_lls_init
 * Purpose:
 *     This function initialises the structure _monterey_dyn_sched_unit_data, which is
 *     used later to allocate the register available from  the set of 
 *     SP_WERR_DYN_CHANGEnA/nB/nC (n=0,1,2,3) to the port and  to keep track 
 *     of the used registers from the set for the port 
 * Parameters:
 *     unit       - (IN) unit number
 * Returns:
 *     SOC_E_XXX
 */
STATIC int _soc_monterey_lls_init(int unit)
{
    _soc_monterey_port_dyn_sched_t *pcb = NULL;
    int i;
    
    if (soc_feature(unit, soc_feature_vector_based_spri)) {
        pcb = &_monterey_dyn_sched_unit_data[unit];
        
        if (pcb->_soc_monterey_init_done[unit] == 1) {
            return SOC_E_NONE;
        }

        pcb->lock = sal_mutex_create("apache_dyn_lock");
        for (i = 0; i < _SOC_MONTEREY_DYN_REGISTER_SET; i++) {
            pcb->port[i] = -1;
        }
        pcb->_soc_monterey_init_done[unit] = 1;
    }

    return SOC_E_NONE;
}

int soc_monterey_lls_init(int unit)
{

    SOC_IF_ERROR_RETURN(_soc_monterey_lls_init(unit));


    SOC_IF_ERROR_RETURN(soc_monterey_init_invalid_parents(unit));
    
    return SOC_E_NONE;
}

int
soc_monterey_sched_hw_index_get(int unit, int port, int lvl, int offset, 
                            int *hw_index)
{
    int rv;

    rv = _soc_monterey_alloc_sched(unit, port, lvl, offset, hw_index);

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Alloced : port=%d lvl=%d ofst=%d Index=%d\n"),
                                port, lvl, offset, *hw_index));
    return rv;
}

int
soc_monterey_qcn_counter_hw_index_get(int unit, soc_port_t port,
                                 int index, int *qindex)
{
    soc_mem_t mem;
    mmu_qcn_enable_entry_t entry;
    int resolved_index = -1;    
    
    if (qindex == NULL) {
        return SOC_E_PARAM;
    }

    mem = MMU_QCN_ENABLE_0m;

    resolved_index = soc_monterey_l2_hw_index(unit, index, 1);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, resolved_index, &entry));
    if (soc_mem_field32_get(unit, mem, &entry, CPQ_ENf)) {
        *qindex = soc_mem_field32_get(unit, mem, &entry, CPQ_IDf);
    } else {
        *qindex = -1;
    }

    return SOC_E_NONE;
}


int 
soc_monterey_get_def_qbase(int unit, soc_port_t inport, int qtype, 
                       int *pbase, int *pnumq)
{
    soc_info_t *si;
    int base, numq;

    si = &SOC_INFO(unit);

    if (qtype == _SOC_TD2_INDEX_STYLE_UCAST_QUEUE) {
        base = si->port_uc_cosq_base[inport];
        numq = si->port_num_uc_cosq[inport];
    } else if (qtype == _SOC_TD2_INDEX_STYLE_MCAST_QUEUE) {
        base = si->port_cosq_base[inport];
        numq = si->port_num_cosq[inport];
    } else {
        return SOC_E_INTERNAL;
    }

    if (pbase) {
        *pbase = base;
    }
    if (pnumq) {
        *pnumq = numq;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_monterey_dump_hsp_sched_at(int unit, int port, int level, int offset, int index)
{
    uint32     rval = 0;
    uint32     conn_map = 0;
    uint32     l0_index, l1_index;
    uint32     mc_group_mode = 0;
    char       *lvl_name[] = { "Root","S1","L0", "L1", "UC", "MC" };
    char       *sched_modes[] = {"X", "SP", "WRR", "WERR"};
    uint32     num_spri =0;
    uint32     first_child =0;
    uint32     first_mc_child =0;
    uint32     ucmap =0;
    uint32     sched_mode =0;
    int        uc_cosq, mc_cosq;
    int        uc_cosq_base, mc_cosq_base;
    int        uc_hw_index, mc_hw_index;
    int        hw_index;
    int        wt =0;
    int        l2_index, l2_max_num = 1;
    int mmu_port;


    SOC_IF_ERROR_RETURN(soc_monterey_sched_hw_index_get(unit,
                port, SOC_MONTEREY_NODE_LVL_L1, 0, &uc_cosq_base));
    mc_cosq_base = uc_cosq_base;

    uc_cosq = soc_monterey_logical_qnum_hw_qnum(unit, port, uc_cosq_base, 1);
    mc_cosq = soc_monterey_logical_qnum_hw_qnum(unit, port, mc_cosq_base, 0);

    if (level != SOC_MONTEREY_NODE_LVL_ROOT) {
        return SOC_E_PARAM;
    }
    if (IS_CPU_PORT(unit, port)) {
        l2_max_num = 8; 
    }
    soc_monterey_port_common_attributes_get(unit, port, NULL, &mmu_port, NULL);
    /* Root Node */
    LOG_CLI((BSL_META_U(unit,
                        "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                        lvl_name[level], offset, index, num_spri, first_child,
                        sched_modes[sched_mode], wt));

    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_PORT_CONFIGr(unit, port, &rval));
    mc_group_mode = soc_reg_field_get(unit, HSP_SCHED_PORT_CONFIGr,
                                      rval, MC_GROUP_MODEf);
    /* L0 Node */
    for (l0_index = 0; l0_index < 5; l0_index++) {
        /* print the L0 Nodes*/
        level = SOC_MONTEREY_NODE_LVL_L0;
        SOC_IF_ERROR_RETURN(soc_monterey_sched_hw_index_get(unit,
                         port, level, l0_index, &hw_index));
        soc_monterey_cosq_get_sched_mode(unit, port, SOC_MONTEREY_NODE_LVL_L0,
                                     hw_index, &sched_mode, &wt);
        LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                            lvl_name[level], l0_index, hw_index, num_spri, first_child,
                            sched_modes[sched_mode], wt));

        /* read Connection map for the  L0.1, L0.2, L0.3, L0.4, L0.5
         * by default connection map should be set to all zeros for L0.0
         */
        if (l0_index == 4) {
            SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr(
                                     unit, port, &rval));
            conn_map = soc_reg_field_get(unit,
                                HSP_SCHED_L0_NODE_4_CONNECTION_CONFIGr,
                                rval, CHILDREN_CONNECTION_MAP_4f);
        } else if (l0_index > 0) { 
            SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                                     unit, port, l0_index - 1, &rval));
            conn_map = soc_reg_field_get(unit,
                                HSP_SCHED_L0_NODE_CONNECTION_CONFIGr,
                                rval, CHILDREN_CONNECTION_MAPf);
        }
        if ((l0_index >= 1) && (l0_index <= 4)) {
            for (l1_index = 0; l1_index < 8; l1_index++) {
                if (conn_map & (1U<<l1_index)) {
                    if (l0_index == 4) {
                        offset = l1_index + 8;
                    } else {
                        offset = l1_index;
                    }
                    /* print L1 node */
                    level = SOC_MONTEREY_NODE_LVL_L1;
                    SOC_IF_ERROR_RETURN(soc_monterey_sched_hw_index_get(unit,
                        port, level, offset, &hw_index));
                    soc_monterey_cosq_get_sched_mode(unit, port, SOC_MONTEREY_NODE_LVL_L1,
                                                hw_index, &sched_mode, &wt);
                    if (IS_CPU_PORT(unit, port)) {
                        num_spri = 0;
                        for (l2_index = 0; l2_index < l2_max_num; l2_index++) {
                             mc_hw_index = soc_monterey_l2_hw_index(unit, mc_cosq_base + l2_index, 0);
                             soc_monterey_cosq_get_sched_mode(unit, port, SOC_MONTEREY_NODE_LVL_L2,
                                                              mc_hw_index, &sched_mode, &wt);
                             if (wt == 0) {
                                 num_spri++;
                             }
                        }
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "  %s.%d : INDEX=%d NUM_SP=%d FC=%d "
                                        "FMC=%d UCMAP=0x%08x MODE=%s WT=%d\n"),
                                        lvl_name[level], offset, hw_index, num_spri,
                                        first_child, first_mc_child, ucmap,
                                        sched_modes[sched_mode], wt));
                    for (l2_index = 0; l2_index < l2_max_num; l2_index++) {
                         /* print L2 UC node attached to L1 */
                         uc_hw_index = soc_monterey_l2_hw_index(unit, uc_cosq_base, 1);
                         soc_monterey_cosq_get_sched_mode(unit, port,
                                                    SOC_MONTEREY_NODE_LVL_L2,
                                                    uc_hw_index, &sched_mode, &wt);
                         if (uc_cosq < 640) {
                             LOG_CLI((BSL_META_U(unit,
                                                 "    L2.uc : INDEX=%d Mode=%s WEIGHT=%d\n"),
                                                 uc_cosq, sched_modes[sched_mode], wt));
                         }
                         uc_cosq_base++;
                         uc_cosq++;

                         if (((mc_group_mode == 1) && (l0_index == 4)) ||
                             (mc_group_mode == 0)) {
                             /* print L2 MC node attached to L1 */
                             /* MC_GROUP_MODE = 1 then  8 MC Cos queues are
                              * grouped together and shared the parent (L0.0) bandwidth.
                              * MC_QM and MC_SC queues are still paired with
                              * UC_QM and UC_SC respectivly
                              */
                             mc_hw_index = soc_monterey_l2_hw_index(unit, mc_cosq_base, 0);
                             soc_monterey_cosq_get_sched_mode(unit, port,
                                                        SOC_MONTEREY_NODE_LVL_L2,
                                                        mc_hw_index, &sched_mode, &wt);
                             LOG_CLI((BSL_META_U(unit,
                                                 "    L2.mc : INDEX=%d Mode=%s WEIGHT=%d\n"),
                                                 mc_cosq, sched_modes[sched_mode], wt));
                             mc_cosq_base++;
                             mc_cosq++;
                          }
                      }
                   }
            }
        } else if (l0_index == 0) {
            if (mc_group_mode) {
                offset = 0;
                for (l1_index = 0; l1_index < 8; l1_index++) {
                    /* 8 MC Cos queues are grouped together and shared
                     * the parent (L0.0) bandwidth.
                     */
                    mc_hw_index = soc_monterey_l2_hw_index(unit, mc_cosq_base + offset, 0);
                    soc_monterey_cosq_get_sched_mode(unit, port,
                                                    SOC_MONTEREY_NODE_LVL_L2,
                                                    mc_hw_index, &sched_mode, &wt);
                    LOG_CLI((BSL_META_U(unit,
                                        "    L2.mc : INDEX=%d Mode=%s WEIGHT=%d\n"),
                                        mc_cosq + offset, sched_modes[sched_mode], wt));
                    offset++;
                }

            }
        }
    }

   return SOC_E_NONE;
}

STATIC int
_soc_monterey_child_num_get(int unit, int port, int level, int hw_index, int *count)
{
    soc_mem_t mem;
    int cindex, index_max, ii, child_level, num_child = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];

    soc_monterey_get_child_type(unit, port, level, &child_level);
    mem = _SOC_MONTEREY_NODE_PARENT_MEM(unit, port, child_level);

    if (mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }
    index_max = soc_mem_index_max(unit, mem);

    for (ii = 0; ii <= index_max; ii++) {
        SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, ii, &entry));

        cindex = soc_mem_field32_get(unit, mem, entry, C_PARENTf);

        if (cindex == hw_index) {
            num_child += 1;
        }
    }

    if (count != NULL) {
        *count = num_child;
    }

    return SOC_E_NONE;
}


STATIC int 
_soc_monterey_dump_sched_at(int unit, int port, int level, int offset, int hw_index)
{
    int num_spri = 0, first_child = 0, first_mc_child, rv, cindex;
    uint32 ucmap, spmap;
    soc_monterey_sched_mode_e sched_mode;
    soc_mem_t mem;
    int index_max, ii, ci, child_level, wt = 0, num_child;
    uint32 entry[SOC_MAX_MEM_WORDS];
    char *lvl_name[] = { "Root", "S1",  "L0", "L1", "L2" };
    char *sched_modes[] = {"X", "SP", "WRR", "WDRR"};
    int num_s1_children = 0;

    if (level < SOC_MONTEREY_NODE_LVL_L2) {
        soc_monterey_get_child_type(unit, port, level, &child_level);
        if (hw_index == INVALID_PARENT(unit, child_level)) {
            /* 
             * INVALID_PARENT(L0) may be zero. 
             * When this condition occurs, 
             * we need to do further check instead of returning.
             */
            if (!((child_level == SOC_MONTEREY_NODE_LVL_S1) &&
                (hw_index == 0))) {
                return SOC_E_NONE;
            }
        }
    }

    if (level != SOC_MONTEREY_NODE_LVL_ROOT) { 
        /* get sched config */
        SOC_IF_ERROR_RETURN(
                soc_monterey_cosq_get_sched_child_config(unit, port, level, hw_index, 
                    &num_spri, &first_child, &first_mc_child, &ucmap, &spmap));
    } 

    sched_mode = 0;
    if (level != SOC_MONTEREY_NODE_LVL_ROOT) { 
        SOC_IF_ERROR_RETURN(
          soc_monterey_cosq_get_sched_mode(unit, port, level, hw_index, &sched_mode, &wt));
    }
    
    if (level == SOC_MONTEREY_NODE_LVL_L1) {
        LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SP=%d FC=%d "
                            "FMC=%d UCMAP=0x%08x MODE=%s WT=%d\n"),
                            lvl_name[level], offset, hw_index, num_spri, 
                            first_child, first_mc_child, ucmap, 
                            sched_modes[sched_mode], wt));
    } else {
        if (soc_feature(unit, soc_feature_mmu_hqos_four_level))
        {
            LOG_CLI((BSL_META_U(unit,
                                "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                                lvl_name[level], offset, hw_index, num_spri, first_child,
                                sched_modes[sched_mode],
                                wt));
        } else 
        {    
            if (level != SOC_MONTEREY_NODE_LVL_ROOT)  
            {
                if (level == SOC_MONTEREY_NODE_LVL_S1) { 
                    LOG_CLI((BSL_META_U(unit,
                                     "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                                     lvl_name[level-1 ], offset, hw_index, num_spri, first_child,
                                     sched_modes[sched_mode],
                                     wt));
                }
                else 
                {
                    LOG_CLI((BSL_META_U(unit,
                                     "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                                     lvl_name[level], offset, hw_index, num_spri, first_child,
                                     sched_modes[sched_mode],
                                     wt));

               }
            }             
        }
    }

    soc_monterey_get_child_type(unit, port, level, &child_level);
    mem = _SOC_MONTEREY_NODE_PARENT_MEM(unit, port, child_level);
    
    if(mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }
    index_max = soc_mem_index_max(unit, mem);

    num_child = 0;
    for (ii = 0, ci = 0; ii <= index_max; ii++) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, ii, &entry);
        if (rv) {
            LOG_CLI((BSL_META_U(unit,
                                "Failed to read memory at index: %d\n"), ii));
            break;
        }
        
        cindex = soc_mem_field32_get(unit, mem, entry,
                child_level == SOC_MONTEREY_NODE_LVL_S1 ? P_PORTf : 
                C_PARENTf);

        /*
         * INVALID_PARENT(L0) may be zero.
         * When this condition occurs,
         * we should get the node`s child count to decide
         * if it is a valid node or not.
         */
        if ((cindex == 0) && (child_level == SOC_MONTEREY_NODE_LVL_S1)) {
            _soc_monterey_child_num_get(unit, port, child_level,
                                        ii, &num_s1_children);
            if (num_s1_children == 0) {
                continue;
            }
        }

        if (cindex == hw_index) {
            if (child_level == SOC_MONTEREY_NODE_LVL_L2) {
                SOC_IF_ERROR_RETURN(soc_monterey_cosq_get_sched_mode(unit, port,
                                    SOC_MONTEREY_NODE_LVL_L2, ii, &sched_mode, &wt));
                LOG_CLI((BSL_META_U(unit,
                                    "     L2.%s INDEX=%d Mode=%s WEIGHT=%d\n"), 
                                    ((ii < _MN_MAX_NUM_UC_QUEUE(unit)) ? "uc" : "mc"),
                                    ii, sched_modes[sched_mode], wt));
            } else {
                _soc_monterey_dump_sched_at(unit, port, child_level, ci, ii);
                ci += 1;
            }
            num_child += 1;
        }
    }
    if (num_child == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "*** No children \n")));
    }
    return SOC_E_NONE;
}

int soc_monterey_dump_port_lls(int unit, int port)
{
    int mmu_port, index;

    if (_soc_monterey_port_sched_type_get(unit, port) == SOC_MONTEREY_SCHED_HSP) {
        return SOC_E_NONE;
    }

    soc_monterey_port_common_attributes_get(unit, port, NULL, &mmu_port, NULL);

    LOG_CLI((BSL_META_U(unit,
                        "-------%s (LLS)------\n"), SOC_PORT_NAME(unit, (port))));
    index = _soc_monterey_root_scheduler_index(unit, port);
    _soc_monterey_dump_sched_at(unit, port, SOC_MONTEREY_NODE_LVL_ROOT, 0, index);
    return SOC_E_NONE;
}
int soc_monterey_dump_port_hsp(int unit, int port)
{
    int mmu_port, index;

    if (_soc_monterey_port_sched_type_get(unit, port) == SOC_MONTEREY_SCHED_LLS) {
        return SOC_E_NONE;
    }

    soc_monterey_port_common_attributes_get(unit, port, NULL, &mmu_port, NULL);

    LOG_CLI((BSL_META_U(unit,
                        "-------%s (HSP)------\n"), SOC_PORT_NAME(unit, (port))));
    index = _soc_monterey_root_scheduler_index(unit, port);
    _soc_monterey_dump_hsp_sched_at(unit, port, SOC_MONTEREY_NODE_LVL_ROOT, 0, index);
    return SOC_E_NONE;
}

int soc_monterey_logical_qnum_hw_qnum(int unit, int port, int lqnum, int uc)
{
    if (uc) {
        return lqnum;
    } else {
        return lqnum + 640;
    }
    return -1;
}

int soc_monterey_mmu_get_shared_size(int unit, int *shared_size)
{
    uint32 entry0[SOC_MAX_MEM_WORDS];

    sal_memset(entry0, 0, sizeof(mmu_thdu_xpipe_config_queue_entry_t));
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, MMU_THDU_XPIPE_CONFIG_QUEUEm, MEM_BLOCK_ALL, 0, entry0));
    *shared_size = soc_mem_field32_get(unit, MMU_THDU_XPIPE_CONFIG_QUEUEm, entry0, Q_SHARED_LIMIT_CELLf);
    return 1;
}

int soc_monterey_mmu_config_res_limits_update(int unit, uint32 spid, 
                                                      uint32 shared_size, int flags)
{
    return soc_monterey_mmu_config_shared_buf_recalc(unit, spid, shared_size, flags);
}

void
soc_monterey_shadow_free(int unit)
{
    int mem_idx;
    soc_monterey_shadow_memory_t * shadow_info;
    soc_monterey_shadow_t* shadow_ctrl = &soc_monterey_shadow[unit];
    
    if (shadow_ctrl->shadow_array != NULL) {
        for (mem_idx = 0; mem_idx < shadow_ctrl->mem_count; mem_idx++){
            shadow_info = &(shadow_ctrl->shadow_array[mem_idx]);
            if (shadow_info->mem_shadow != NULL){
                sal_free(shadow_info->mem_shadow);
            }
        }
    sal_free(shadow_ctrl->shadow_array);
    shadow_ctrl->shadow_array = NULL;
    shadow_ctrl->mem_count = 0;
    }
}

int
soc_monterey_shadow_create(int unit)
{
	  int mem_idx, alloc_size, num_entries, entry_size;
	  soc_monterey_shadow_memory_t* shadow_info = NULL;
	  uint32* shadow = NULL;
    
          if (soc_monterey_shadow[unit].shadow_array) {
              soc_monterey_shadow_free(unit); 
          } 
          alloc_size = sizeof(soc_monterey_shadow_memory_t) *
                      COUNTOF(shadow_mems);
          shadow_info = sal_alloc(alloc_size, "fc map shadow control");
          if (shadow_info == NULL) {
              return SOC_E_MEMORY;
          }
        
          sal_memset(shadow_info, 0, alloc_size);
          soc_monterey_shadow[unit].shadow_array = shadow_info;
    
          for (mem_idx = 0; mem_idx < COUNTOF(shadow_mems); mem_idx++) {
               num_entries = soc_mem_index_count(unit, shadow_mems[mem_idx]);
               /* for OBM classifier tables we need space for two pgws */
               if (mem_idx >= 2) {
               num_entries = num_entries * 2;
               }
               entry_size = soc_mem_entry_words(unit, shadow_mems[mem_idx]);
               alloc_size = entry_size * num_entries * sizeof(uint32);
               shadow = sal_alloc(alloc_size, "fc map shadow tbl");
               if (shadow == NULL) {
                   soc_monterey_shadow_free(unit);
                   return SOC_E_MEMORY;
               }
               sal_memset(shadow, 0, alloc_size);
               soc_monterey_shadow[unit].shadow_array[mem_idx].mem_shadow = shadow;
               soc_monterey_shadow[unit].shadow_array[mem_idx].mem = shadow_mems[mem_idx];
               soc_monterey_shadow[unit].mem_count++;
          }
    return SOC_E_NONE; 
}

int
soc_monterey_shadow_entry_get (int unit, soc_mem_t mem, int index,
                                      void* entry_data)
{
    int mem_idx, entry_words;
    uint32 * shadow = NULL;
    
    SOC_IF_ERROR_RETURN(soc_monterey_shadow_get_mem_idx(unit, mem, &mem_idx));
    shadow = soc_monterey_shadow[unit].shadow_array[mem_idx].mem_shadow;
    entry_words = soc_mem_entry_words(unit, mem);
    sal_memcpy(entry_data, shadow + index * entry_words, 
               entry_words * sizeof(uint32));
    return SOC_E_NONE;
}

int
soc_monterey_shadow_entry_set (int unit, soc_mem_t mem, int index,
                                      void* entry_data)
{
    int mem_idx, entry_words;
    uint32 * shadow = NULL;
    
    SOC_IF_ERROR_RETURN(soc_monterey_shadow_get_mem_idx(unit, mem, &mem_idx));
    shadow = soc_monterey_shadow[unit].shadow_array[mem_idx].mem_shadow;
    entry_words = soc_mem_entry_words(unit, mem);
    sal_memcpy(shadow + index * entry_words, entry_data, 
               entry_words * sizeof(uint32));
    return SOC_E_NONE;
}

int 
soc_monterey_shadow_clear (int unit, soc_mem_t mem)
{
    uint32 * shadow = NULL;
    int entry_words, tbl_size, mem_idx, num_entries;

    SOC_IF_ERROR_RETURN(soc_monterey_shadow_get_mem_idx(unit, mem, &mem_idx));
    entry_words = soc_mem_entry_words(unit, mem);
    shadow = soc_monterey_shadow[unit].shadow_array[mem_idx].mem_shadow;
    num_entries = soc_mem_index_count(unit, shadow_mems[mem_idx]);
    tbl_size = entry_words * num_entries;
    if (shadow != NULL) {
        sal_memset(shadow, 0, tbl_size * sizeof(uint32));
    }
    return SOC_E_NONE;
}

int _soc_monterey_obm_interrupt_cb(int unit) {

     bcm_obm_interrupt_info_t obm_info;
     int pgw = 0  ;
     bcm_obm_callback_fn cb;
     soc_info_t *si;
     soc_reg_t  reg;
     int pgw_inst = 0 ;
     uint64 rval64; 
     soc_field_t field;
     int port = 0,phy_port = 0  ;
     int temp =0 ;
     void *user_data =soc_monterey_obm_cb[unit].user_data;  
     si = &SOC_INFO(unit);
    for (pgw = 0; pgw < MONTEREY_PGWS_PER_DEV ;pgw++)
    {
         pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
         reg = PGW_CA_OVERFLOW_STATUSr;
         field = CA_PREEMPT_OVERFLOWf;
         SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst ,0, &rval64));
         temp = soc_reg64_field32_get(unit, reg, rval64, field);
         if(temp ) {
            obm_info.interrupt_type = bcmObmInterruptTypePreemptOverflow;
            break;
         }
         field = CA_EXP_OVERFLOWf;
         temp = soc_reg64_field32_get(unit, reg, rval64, field);
         if(temp ) {
            obm_info.interrupt_type = bcmObmInterruptTypeExpressOverflow;
            break;
         }

         reg = PGW_OBM_DROP_STATUSr;
         field = EXP_DROP_ERRf ;
         SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst,0, &rval64));
         temp = soc_reg64_field32_get(unit, reg, rval64, field);
         if(temp ) {
            obm_info.interrupt_type  = bcmObmInterruptTypeExpressDrop ;
            break;
         }
         field = PREEMPT_DROP_ERRf;
         temp = soc_reg64_field32_get(unit, reg, rval64, field);
         if(temp ) {
            obm_info.interrupt_type = bcmObmInterruptTypePreemptDrop;
            break;
         }
     }
     if (!temp) {
         return SOC_E_NONE;
     }
     PBMP_ALL_ITER(unit, port) {
        if (port == 0  ) {
            continue; 
        } 
        phy_port = si->port_l2p_mapping[port];
        if ((pgw == 1) && (phy_port <= 32)) 
        {
             continue; 
        }   
        if (phy_port > 32) {
            phy_port = phy_port - 32;
        }
        if (temp & (1 << (phy_port-1))) {
            obm_info.port = port;
            break;
        }
     }
     cb=soc_monterey_obm_cb[unit].cb;
     if (cb) {
            (void)(*cb)(unit ,&obm_info, user_data);
     }
     return SOC_E_NONE;

}

#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_monterey_shadow_size_get (int unit, uint32 * size)
{
    int alloc_size = 0, mem_idx, num_entries, entry_size;
    for (mem_idx = 0; mem_idx < COUNTOF(shadow_mems); mem_idx++) {
        num_entries = soc_mem_index_count(unit, shadow_mems[mem_idx]);
        entry_size = soc_mem_entry_words(unit, shadow_mems[mem_idx]);
        if (mem_idx >= 2) {
            num_entries = num_entries * 2;
        }
        alloc_size += entry_size * num_entries * sizeof(uint32);
    }
    *size = alloc_size;
    return SOC_E_NONE;
}

int
soc_monterey_shadow_sync (int unit, uint32 **sync_addr)
{
    int mem_idx, num_entries, tbl_size, entry_size;

    for (mem_idx = 0; mem_idx < COUNTOF(shadow_mems); mem_idx++) {
        num_entries = soc_mem_index_count(unit, shadow_mems[mem_idx]);
        if (mem_idx >= 2) {
            num_entries = num_entries * 2;
        }
        entry_size = soc_mem_entry_words(unit, shadow_mems[mem_idx]);
        tbl_size = entry_size * num_entries;
        sal_memcpy(*sync_addr, soc_monterey_shadow[unit].shadow_array[mem_idx].mem_shadow,
                   tbl_size * sizeof(uint32));
        *sync_addr += tbl_size;
    }
    return SOC_E_NONE;
}

int
soc_monterey_shadow_load (int unit, uint32 **load_addr)
{
    int blk, mem_idx, num_entries;
    int tbl_size, entry_size, vmap_size;
    uint32 *cache;
    uint32 *shadow;
    uint8 *vmap;
    int index_min;
    soc_mem_t mem;
    
    for (mem_idx = 0; mem_idx < COUNTOF(shadow_mems); mem_idx++) {
        mem = shadow_mems[mem_idx];
        num_entries = soc_mem_index_count(unit, mem);
        if (mem_idx >= 2) {
            num_entries = num_entries * 2;
        }
        entry_size = soc_mem_entry_words(unit, mem);
        index_min = soc_mem_index_min(unit, mem);
        tbl_size = entry_size * num_entries;
        sal_memcpy(soc_monterey_shadow[unit].shadow_array[mem_idx].mem_shadow, *load_addr,
                   tbl_size * sizeof(uint32));
        *load_addr += tbl_size;
        /* restore memory cache */
        vmap_size = (num_entries + 7) / 8;
        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
            if (SOC_MEM_STATE(unit, mem).cache[blk] == NULL) {
                continue;
            }
            cache = SOC_MEM_STATE(unit, mem).cache[blk];
            vmap = SOC_MEM_STATE(unit, mem).vmap[blk];
            shadow = soc_monterey_shadow[unit].shadow_array[mem_idx].mem_shadow;
            sal_memcpy(cache, shadow, tbl_size * sizeof(uint32));
        
            sal_memset(&vmap[index_min / 8], 0xff, 
                      (num_entries + 7 - index_min) / 8);
                
            /* Clear invalid bits at the left and right ends */
            vmap[index_min / 8] &= 0xff00 >> (8 - index_min % 8);
            vmap[vmap_size - 1] &= 0x00ff >> ((8 - num_entries % 8) % 8);
        }
    }
    return SOC_E_NONE;
}
#endif

#undef _ERR_MSG_MODULE_NAME /* debug */

#endif /* BCM_MONTEREY_SUPPORT */

