/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * MMU/Cosq soc routines
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/triumph3.h>

#if defined(BCM_TRIUMPH3_SUPPORT)

#define _SOC_TR3_ROOT_SCHEDULER_INDEX(u,p)   \
       SOC_INFO((u)).port_p2m_mapping[SOC_INFO((u)).port_l2p_mapping[(p)]]

#define _SOC_TR3_DEF_SCHED_NODE_BASE(u,p,cfg,t)   \
            (cfg)[(t)].base_factor * (p)

#define _SOC_TR3_IS_HSP_PORT(u,p)   \
        ((IS_CE_PORT((u),(p))) ||   \
        ((IS_HG_PORT((u),(p))) && (SOC_INFO((u)).port_speed_max[(p)] >= 100000)))

#define _SOC_TR3_LLS_OP_TEAR        0
#define _SOC_TR3_LLS_OP_SETUP       1
#define _SOC_TR3_LLS_OP_TRAVERSE    2

#define _SOC_TR3_DYN_SET    4

static int _soc_tr3_init_done[SOC_MAX_NUM_DEVICES] = {0};
static int _tr3_invalid_ptr[SOC_MAX_NUM_DEVICES][SOC_TR3_NODE_LVL_MAX];
static int _tr2_node_max[SOC_MAX_NUM_DEVICES][SOC_TR3_NODE_LVL_MAX];

typedef struct _soc_tr3_b0_cb_s {
    sal_mutex_t     lock;
    int             port[_SOC_TR3_DYN_SET];
} _soc_tr3_b0_cb_t;

static _soc_tr3_b0_cb_t _tr3_b0_unit_data[SOC_MAX_NUM_DEVICES];

int _bcm_tr3_port_sdyn[SOC_MAX_NUM_DEVICES][SOC_PBMP_PORT_MAX];  
int _bcm_tr3_port_numq[SOC_MAX_NUM_DEVICES][SOC_PBMP_PORT_MAX];  

STATIC int _soc_tr3_alloc_dyn_set(int unit, int port, 
            soc_reg_t *r_a, soc_reg_t *r_b, soc_reg_t *r_ctrl)
{
    _soc_tr3_b0_cb_t *pcb;
    int ii, empty_at = -1, rv = SOC_E_NONE;
    soc_reg_t dyn_change_a[] = {
                                   LLS_SP_WERR_DYN_CHANGE_0Ar,
                                   LLS_SP_WERR_DYN_CHANGE_1Ar,
                                   LLS_SP_WERR_DYN_CHANGE_2Ar,
                                   LLS_SP_WERR_DYN_CHANGE_3Ar
                               };
    soc_reg_t dyn_change_b[] = {
                                   LLS_SP_WERR_DYN_CHANGE_0Br,
                                   LLS_SP_WERR_DYN_CHANGE_1Br,
                                   LLS_SP_WERR_DYN_CHANGE_2Br,
                                   LLS_SP_WERR_DYN_CHANGE_3Br
                               };

    soc_reg_t dyn_change_ctrl[] = {
                                   LLS_SP_WERR_DYN_CHANGE_0r,
                                   LLS_SP_WERR_DYN_CHANGE_1r,
                                   LLS_SP_WERR_DYN_CHANGE_2r,
                                   LLS_SP_WERR_DYN_CHANGE_3r
                                  };

    pcb = &_tr3_b0_unit_data[unit];
    
    sal_mutex_take(pcb->lock, sal_sem_FOREVER);
    for (ii = 0; ii < _SOC_TR3_DYN_SET; ii++) {
        if (pcb->port[ii] == -1) {
            empty_at = ii;
        } else if (pcb->port[ii] == port) {
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

    *r_a = dyn_change_a[empty_at];
    *r_b = dyn_change_b[empty_at];
    *r_ctrl = dyn_change_ctrl[empty_at];

    return SOC_E_NONE;
}

STATIC int _soc_tr3_free_dyn_set(int unit, int port)
{
    _soc_tr3_b0_cb_t *pcb;
    int ii;

    pcb = &_tr3_b0_unit_data[unit];

    sal_mutex_take(pcb->lock, sal_sem_FOREVER);
    for (ii = 0; ii < _SOC_TR3_DYN_SET; ii++) {
        if (pcb->port[ii] == port) {
            pcb->port[ii] = -1;
        }
    }
    sal_mutex_give(pcb->lock);
    
    return SOC_E_NONE;
}

int
soc_tr3_cosq_set_sched_parent(int unit, soc_port_t port,
                               int level, int hw_index,
                               int parent_hw_idx)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    uint32 *bmap = NULL;

    mem = _SOC_TR3_NODE_PARENT_MEM(level);

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%d : %d parent:%d\n"),
              port, level - 1, hw_index, parent_hw_idx));
    /* For HSP port we don't need to set LLS mem */
    if (_SOC_TR3_IS_HSP_PORT(unit, port)) {
        return SOC_E_NONE;
    } 

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    
    soc_mem_field32_set(unit, mem, entry, C_PARENTf, parent_hw_idx);

    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));

    if (level == SOC_TR3_NODE_LVL_L0) {
        bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
    } else if (level == SOC_TR3_NODE_LVL_L1) {
        bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
    } else if (level == SOC_TR3_NODE_LVL_L2) {
        bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
    }
    if (bmap != NULL) {
        SOC_LLS_SCHEDULER_LOCK(unit);
        if (parent_hw_idx == _soc_tr3_invalid_parent_index(unit, level)) {
            SHR_BITCLR(bmap, hw_index);
        } else {
            SHR_BITSET(bmap, hw_index);
        }
        SOC_LLS_SCHEDULER_UNLOCK(unit);
    }
    return SOC_E_NONE;
}

int 
soc_tr3_sched_weight_set(int unit, int level, int index, int weight)
{
    soc_mem_t mem_weight;
    uint32 entry[SOC_MAX_MEM_WORDS];

    mem_weight = _SOC_TR3_NODE_WIEGHT_MEM(level);
    if (mem_weight <= 0) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem_weight, MEM_BLOCK_ALL, index, &entry));

    soc_mem_field32_set(unit, mem_weight, &entry, C_WEIGHTf, weight);

    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem_weight, MEM_BLOCK_ALL, index, &entry));

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "sched_weight_set L%d index=%d wt=%d\n"),
              level, index, weight));
    return SOC_E_NONE;
}

int 
soc_tr3_sched_weight_get(int unit, int level, int index, int *weight)
{
    soc_mem_t mem_weight;
    uint32 entry[SOC_MAX_MEM_WORDS];

    mem_weight = _SOC_TR3_NODE_WIEGHT_MEM(level);
    if (mem_weight <= 0) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem_weight, MEM_BLOCK_ALL, index, &entry));

    *weight = soc_mem_field32_get(unit, mem_weight, &entry, C_WEIGHTf);

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "sched_weight_get L%d index=%d wt=%d\n"),
              level, index, *weight));
    return SOC_E_NONE;
}

int
soc_tr3_cosq_set_sched_child_config_dynamic(int unit, soc_port_t port,
                                       int level, int index, 
                                       int num_spri, int first_child,
                                       int first_mc_child, uint32 ucmap,
                                       uint32 spmap, int child_index)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval, d32, timeout_val;
    soc_mem_t mem;
    int rv = SOC_E_NONE;
    soc_reg_t r_a = INVALIDr, r_b = INVALIDr, r_ctrl = INVALIDr;
    soc_timeout_t timeout;

    SOC_IF_ERROR_RETURN(READ_LLS_CONFIG0r(unit, &rval));
    if (!soc_reg_field_get(unit, LLS_CONFIG0r, rval, SPRI_VECT_MODE_ENABLEf)) {
        soc_reg_field_set(unit, LLS_CONFIG0r, &rval, SPRI_VECT_MODE_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_LLS_CONFIG0r(unit, rval)); 
    }

    mem = _SOC_TR3_NODE_CONFIG_MEM(level);
    if (mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    
    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d FC=%d FMC=%d UMAP=0x%x NUMSP=%d\n"),
              port, (level==0) ? "r" : "", level - 1, 
              index, first_child, first_mc_child, ucmap, num_spri));

    mem = _SOC_TR3_NODE_CONFIG_MEM(level);
    SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    soc_mem_field32_set(unit, mem, &entry, P_NUM_SPRIf, spmap & 0xf);
    soc_mem_field32_set(unit, mem, &entry, P_VECT_SPRI_7_4f, (spmap >> 4) & 0xf);

    if (mem == LLS_L1_CONFIGm) {
        soc_mem_field32_set(unit, mem, &entry, P_START_UC_SPRIf, first_child);
        soc_mem_field32_set(unit, mem, &entry, P_START_MC_SPRIf, 
                            first_mc_child);
        soc_mem_field32_set(unit, mem, &entry, P_SPRI_SELECTf, (num_spri > 0) ? ucmap : 0);
    } else {
        soc_mem_field32_set(unit, mem, &entry, P_START_SPRIf, first_child);
    }
        
    SOC_IF_ERROR_RETURN(_soc_tr3_alloc_dyn_set(unit, port, 
                                               &r_a, &r_b, &r_ctrl));

    rval = 0;
    soc_bits_get(entry, 0, 31, &d32);
    soc_reg_field_set(unit, r_b, &rval, LLS_PARENT_CONFIG_31_0f, d32);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, r_b, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, r_a, &rval, NODE_LEVELf, level + 1);
    soc_reg_field_set(unit, r_a, &rval, NODE_IDf, child_index);
    soc_reg_field_set(unit, r_a, &rval, PARENT_IDf, index);
    soc_bits_get(entry, 32, 39, &d32);
    soc_reg_field_set(unit, r_a, &rval, LLS_PARENT_CONFIG_39_32f, d32);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, r_a, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, r_ctrl, &rval, IN_USEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, r_ctrl, REG_PORT_ANY, 0, rval));

    if (!SAL_BOOT_SIMULATION) {
        timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 20000);
        soc_timeout_init(&timeout, timeout_val, 0);

        do {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, r_ctrl, REG_PORT_ANY, 0, &rval));
            if (soc_timeout_check(&timeout)) {
                rv = SOC_E_TIMEOUT;
                break;
            }
        } while (soc_reg_field_get(unit, r_ctrl, rval, IN_USEf));
    }

    SOC_IF_ERROR_RETURN(_soc_tr3_free_dyn_set(unit, port));

    return rv;
}

int
soc_tr3_cosq_set_sched_child_config(int unit, soc_port_t port,
                                    int level, int index, 
                                    int num_spri, int first_child,
                                    int first_mc_child, uint32 ucmap,
                                    uint32 spmap)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    int sch_index;
    int sp_vec = soc_feature(unit, soc_feature_tr3_sp_vector_mask);

    mem = _SOC_TR3_NODE_CONFIG_MEM(level);
    if (mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    
    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d FC=%d FMC=%d UMAP=0x%x NUMSP=%d\n"),
              port, (level==0) ? "r" : "", level - 1, 
              index, first_child, first_mc_child, ucmap, num_spri));

    sch_index = index;
    mem = _SOC_TR3_NODE_CONFIG_MEM(level);
    SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ALL, sch_index, &entry));
    if (sp_vec) {
        soc_mem_field32_set(unit, mem, &entry, P_NUM_SPRIf, spmap & 0xf);
        soc_mem_field32_set(unit, mem, &entry, P_VECT_SPRI_7_4f, (spmap >> 4) & 0xf);
    } else {
        soc_mem_field32_set(unit, mem, &entry, P_NUM_SPRIf, num_spri);
    }

    if (mem == LLS_L1_CONFIGm) {
        soc_mem_field32_set(unit, mem, &entry, P_START_UC_SPRIf, first_child);
        soc_mem_field32_set(unit, mem, &entry, P_START_MC_SPRIf, 
                            first_mc_child);
        soc_mem_field32_set(unit, mem, &entry, P_SPRI_SELECTf, (num_spri > 0) ? ucmap : 0);
    } else {
        soc_mem_field32_set(unit, mem, &entry, P_START_SPRIf, first_child);
    }
        
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY, sch_index, &entry));

    return SOC_E_NONE;
}

int
soc_tr3_cosq_set_sched_mode(int unit, soc_port_t port, int level, int index,
                            soc_tr3_sched_mode_e mode, int weight)
{
    uint32 entry[SOC_MAX_MEM_WORDS], fval;
    soc_mem_t mem;

    if (mode == SOC_TR3_SCHED_MODE_STRICT) {
        weight = 0;
    } 

    if ((!soc_property_get(unit, spn_BCM5664X_WRR_GRANULARITY_1, 0)) && 
        (mode == SOC_TR3_SCHED_MODE_WRR)) {
         weight *= 2;
    }

    SOC_IF_ERROR_RETURN(soc_tr3_sched_weight_set(unit, level, index, weight));

    if (mode != SOC_TR3_SCHED_MODE_STRICT) {
        mem = _SOC_TR3_NODE_CONFIG_MEM(SOC_TR3_NODE_LVL_ROOT);
        index = _SOC_TR3_ROOT_SCHEDULER_INDEX(unit, port);
        SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));

        fval = (mode == SOC_TR3_SCHED_MODE_WRR) ? 1 : 0;
        if (soc_mem_field32_get(unit, mem, entry, 
                        PACKET_MODE_WRR_ACCOUNTING_ENABLEf) != fval) {
            soc_mem_field32_set(unit, mem, entry, 
                                PACKET_MODE_WRR_ACCOUNTING_ENABLEf, fval);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));
        }
    }

    return SOC_E_NONE;
}

int
soc_tr3_cosq_set_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int num_spri, int first_child,
                              int first_mc_child, uint32 ucmap, uint32 spmap,
                              soc_tr3_sched_mode_e mode, int weight)
{
    int child_level;
    int sp_vec = soc_feature(unit, soc_feature_tr3_sp_vector_mask);
    int enable_dynamic = 1;

#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
        enable_dynamic = 0;
    }
#endif

    if (level >= SOC_TR3_NODE_LVL_L2) {
        return SOC_E_PARAM;
    }

    /*
     * COVERITY
     *
     * enable_dynamic variable is by default 1 and gets assigned to 0 if PLISIM is defined. 
     * So ignore the coverity warning below
     */
    if (sp_vec && enable_dynamic) {
        SOC_IF_ERROR_RETURN(soc_tr3_cosq_set_sched_child_config_dynamic(unit, port,
                                   level, index, num_spri, first_child, 
        /* coverity[dead_error_begin] */
                                   first_mc_child, ucmap, spmap, child_index));
    } else {
        SOC_IF_ERROR_RETURN(soc_tr3_cosq_set_sched_child_config(unit, port,
           level, index, num_spri, first_child, first_mc_child, ucmap, spmap));
    }

    if (child_index >= 0) {
        soc_tr3_get_child_type(unit, port, level, &child_level);
        SOC_IF_ERROR_RETURN(soc_tr3_cosq_set_sched_mode(unit, port, child_level, 
                                child_index, mode, weight));
    }

    return SOC_E_NONE;
}

int
soc_tr3_sched_get_node_config(int unit, soc_port_t port, int level, int index,
                              int *pnum_spri, int *pfirst_child,
                              int *pfirst_mc_child, uint32 *pucmap, 
                              uint32 *pspmap)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    uint32 num_spri = 0, ucmap = 0, f1, f2;
    int first_child = -1, first_mc_child = -1, ii;
    int sp_vec = soc_feature(unit, soc_feature_tr3_sp_vector_mask);

    *pspmap = 0;
    
    mem = _SOC_TR3_NODE_CONFIG_MEM(level);
    if (mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    if (sp_vec) {
        f1 = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
        f2 = soc_mem_field32_get(unit, mem, &entry, P_VECT_SPRI_7_4f);
        *pspmap = f1 | (f2 << 4);
        for (ii = 0; ii < 32; ii++) {
            if ((1 << ii) & *pspmap) {
                num_spri++;
            }
        }
    } else {
        num_spri = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
    }

    if (mem == LLS_L1_CONFIGm) {
        first_child = soc_mem_field32_get(unit, mem, &entry, P_START_UC_SPRIf);
        first_mc_child = soc_mem_field32_get(unit, mem, 
                                         &entry, P_START_MC_SPRIf);
        ucmap = soc_mem_field32_get(unit, mem, &entry, P_SPRI_SELECTf);
    } else {
        first_child = soc_mem_field32_get(unit, mem, &entry, P_START_SPRIf);
        first_mc_child = 0;
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

    if (pfirst_child) {
        *pfirst_child = first_child;
    }

    if (pfirst_mc_child) {
        *pfirst_mc_child = first_mc_child;
    }
    return SOC_E_NONE;
}

int
soc_tr3_cosq_get_sched_mode(int unit, soc_port_t port, int level, int index,
                              soc_tr3_sched_mode_e *pmode, int *weight)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    soc_tr3_sched_mode_e mode = SOC_TR3_SCHED_MODE_UNKNOWN;

    SOC_IF_ERROR_RETURN(soc_tr3_sched_weight_get(unit, level, index, weight));

    if (*weight == 0) {
        mode = SOC_TR3_SCHED_MODE_STRICT;
    } else {
        mem = _SOC_TR3_NODE_CONFIG_MEM(SOC_TR3_NODE_LVL_ROOT);
        index = _SOC_TR3_ROOT_SCHEDULER_INDEX(unit, port);
        SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
        if (soc_mem_field32_get(unit, mem, entry, PACKET_MODE_WRR_ACCOUNTING_ENABLEf)) {
            mode = SOC_TR3_SCHED_MODE_WRR;
            if (!soc_property_get(unit, spn_BCM5664X_WRR_GRANULARITY_1, 0)) {
                *weight /= 2;
            }
        } else {
            mode = SOC_TR3_SCHED_MODE_WDRR;
        }
    }

    if (pmode) {
        *pmode = mode;
    }
        
    return SOC_E_NONE;
}

int
soc_tr3_cosq_get_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int *pnum_spri, int *pfirst_child,
                              int *pfirst_mc_child, uint32 *pucmap, uint32 *pspmap,
                              soc_tr3_sched_mode_e *pmode, int *weight)
{
    int child_level;

    if (level >= SOC_TR3_NODE_LVL_L2) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_tr3_sched_get_node_config(unit, port, level,
                        index, pnum_spri, pfirst_child, pfirst_mc_child, pucmap,
                        pspmap));

    if (child_index >= 0) {
        soc_tr3_get_child_type(unit, port, level, &child_level);
        
        SOC_IF_ERROR_RETURN(
                soc_tr3_cosq_get_sched_mode(unit, port, child_level, child_index,
                                            pmode, weight));
    }
    
    return SOC_E_NONE;
}

int soc_tr3_get_child_type(int unit, soc_port_t port, int level, 
                                    int *child_type)
{
    *child_type = -1;

    if (level == SOC_TR3_NODE_LVL_ROOT) {
        *child_type = SOC_TR3_NODE_LVL_L0;
    } else if (level == SOC_TR3_NODE_LVL_L0) {
        *child_type = (_SOC_TR3_IS_HSP_PORT(unit, port)) ? 
                SOC_TR3_NODE_LVL_L2 : SOC_TR3_NODE_LVL_L1;
    } else if (level == SOC_TR3_NODE_LVL_L1) {
        *child_type = SOC_TR3_NODE_LVL_L2;
    }
    return SOC_E_NONE;
}

int 
soc_tr3_hsp_sched_weight_set(int unit, int port, int cosq, int weight, int adjust_cos)
{
    /* COS 0 reserved for MG group mode (currenly unsupported) */
    const int adjusted_cosq = cosq + adjust_cos;
    uint32 rval = 0;
    uint32 spmap;

    if (!_SOC_TR3_IS_HSP_PORT(unit, port)) {
        return SOC_E_PARAM;
    }
    /* Check if adjusted_cosq doesn't cross limit */
    if (adjusted_cosq >= _SOC_HSP_MAX_L0_NODES) {
        return SOC_E_PARAM;
    } 
    soc_reg_field_set(unit, L0_COSWEIGHTSr, &rval, COSWEIGHTSf, weight);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, 
                        L0_COSWEIGHTSr, port, adjusted_cosq, rval));

    /* Ensure that when scheduling minumum bandwidth, schedule disciple matches */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_PORT_CONFIGr, port, 0, &rval));
    spmap = soc_reg_field_get(unit, HES_PORT_CONFIGr, rval, MINSPCONFIGf);
    
    if (weight) {
        spmap &= ~(1 << adjusted_cosq);
    } else {
        spmap |= (1 << adjusted_cosq);
    }

    soc_reg_field_set(unit, HES_PORT_CONFIGr, &rval, MINSPCONFIGf, spmap);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HES_PORT_CONFIGr, port, 0, rval));

    return SOC_E_NONE;
}

int 
soc_tr3_hsp_sched_weight_get(int unit, int port, int cosq, int *weight)
{
    uint32 rval = 0;
    int    adjusted_cosq = cosq + _SOC_HSP_VALID_L0_START; 
    if (!_SOC_TR3_IS_HSP_PORT(unit, port)) {
        return SOC_E_PARAM;
    }
    if (adjusted_cosq >= _SOC_HSP_MAX_L0_NODES) {
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, L0_COSWEIGHTSr, port,
                                      adjusted_cosq, &rval));
    *weight = soc_reg_field_get(unit, L0_COSWEIGHTSr, rval, COSWEIGHTSf);
    return SOC_E_NONE;
}

int
soc_tr3_hsp_set_sched_config(int unit, soc_port_t port, int cosq,
                             soc_tr3_sched_mode_e mode, int adjust_cos)
{
    uint32 rval = 0;
   
    if (!_SOC_TR3_IS_HSP_PORT(unit, port)) {
        return SOC_E_PARAM;
    }
    if ((cosq + adjust_cos) >= _SOC_HSP_MAX_L0_NODES) {
        return SOC_E_PARAM;
    }
    if (mode == SOC_TR3_SCHED_MODE_STRICT) {
        soc_tr3_hsp_sched_weight_set(unit, port, cosq, 0, adjust_cos);
    } else if (mode == SOC_TR3_SCHED_MODE_WRR) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_L0_CONFIGr, 
                    port, 0, &rval));
        soc_reg_field_set(unit, HES_L0_CONFIGr, &rval, 
                          SCHEDULING_SELECTf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HES_L0_CONFIGr, port, 0, rval));
        rval = 0;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_PORT_CONFIGr,
                    port, 0, &rval));
        soc_reg_field_set(unit, HES_PORT_CONFIGr, &rval, 
                          SCHEDULING_SELECTf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HES_PORT_CONFIGr, port, 0, rval));

    } else if (mode == SOC_TR3_SCHED_MODE_WDRR) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_L0_CONFIGr, 
                    port, 0, &rval));
        soc_reg_field_set(unit, HES_L0_CONFIGr, &rval, 
                          SCHEDULING_SELECTf, 0x3FF);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HES_L0_CONFIGr, port, 0, rval));
        rval = 0;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_PORT_CONFIGr,
                    port, 0, &rval));
        soc_reg_field_set(unit, HES_PORT_CONFIGr, &rval, 
                          SCHEDULING_SELECTf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HES_PORT_CONFIGr, port, 0, rval));
    }

    return SOC_E_NONE;
}

int
soc_tr3_hsp_get_sched_config(int unit, soc_port_t port, int cosq,
                                    soc_tr3_sched_mode_e *mode)
{
    uint32 rval = 0, fval = 0;
    int wt = 0, adjusted_cosq = cosq + _SOC_HSP_VALID_L0_START;

    if (!_SOC_TR3_IS_HSP_PORT(unit, port)) {
        return SOC_E_PARAM;
    }
    if (adjusted_cosq >= _SOC_HSP_MAX_L0_NODES) {
        return SOC_E_PARAM;
    }
    soc_tr3_hsp_sched_weight_get(unit, port, cosq, &wt);
    if (wt == 0) {
        *mode = SOC_TR3_SCHED_MODE_STRICT;
    } else {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_L0_CONFIGr, port,
                                            0, &rval));
        fval = soc_reg_field_get(unit, HES_L0_CONFIGr, rval, SCHEDULING_SELECTf);
        *mode = (fval == 0) ? SOC_TR3_SCHED_MODE_WRR : SOC_TR3_SCHED_MODE_WDRR;
    }
    return SOC_E_NONE;
}
STATIC 
int _soc_tr3_hsp_get_cos_index(int unit, int port, int index, int cosq, int *ptr_cos)
{
    int cos;

    if (!_SOC_TR3_IS_HSP_PORT(unit, port)) {
        return SOC_E_PARAM;
    }
    if (ptr_cos == NULL) {
        return SOC_E_PARAM;
    } 
    /* Calculate cos. Allow only l0 node index 1 to 9 */
    if ((index < _SOC_HSP_VALID_L0_START) ||
        (index > _SOC_HSP_VALID_L0_END)) {
        return SOC_E_PARAM;
    }
    if ((cosq < _SOC_HSP_MIN_QUEUE_OFFSET) || 
        (cosq > _SOC_HSP_MAX_QUEUE_OFFSET)) {
        return SOC_E_PARAM;
    }

    if (index == _SOC_HSP_VALID_L0_END) {
        if ((cosq == 0) || (cosq == 1)) {
             /* Unicast Queue */
             cos = _SOC_HSP_LAST_L0_UC_START + cosq;
        } else {
            /* Multicast Queue */
            cos = _SOC_HSP_LAST_L0_MC_START + 
                  (cosq - _SOC_HSP_LAST_L0_MAX_UC_QUEUE );
        }
    } else {
        if (cosq > 1 ) {
            return SOC_E_PARAM;
        } 
        if (cosq == 0) {
            cos = index - _SOC_HSP_VALID_L0_START;
        } else {
            cos = _SOC_HSP_MAX_L0_NODES + (index - _SOC_HSP_VALID_L0_START);
        }
    }
    *ptr_cos = cos;
    return SOC_E_NONE;
}
int
_soc_tr3_hsp_sched_weight_set(int unit, int port, int index, int cosq, int weight)
{
    uint32 rval = 0;
    int cos; 

    SOC_IF_ERROR_RETURN(_soc_tr3_hsp_get_cos_index(unit, port, index, cosq, &cos)); 
    /* Now configure weight */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_COSWEIGHTSr, port, cos, &rval));
    soc_reg_field_set(unit, HES_Q_COSWEIGHTSr, &rval, COSWEIGHTSf, weight);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HES_Q_COSWEIGHTSr, port, cos, rval));

    return SOC_E_NONE;
}

int
_soc_tr3_hsp_sched_weight_get(int unit, int port, int index, int cosq, int *weight)
{
    uint32 rval = 0;
    int cos;

    SOC_IF_ERROR_RETURN(_soc_tr3_hsp_get_cos_index(unit, port, index, cosq, &cos));
    
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_COSWEIGHTSr, port, cos, &rval));
    *weight = soc_reg_field_get(unit, HES_Q_COSWEIGHTSr, rval, COSWEIGHTSf);
    
    return SOC_E_NONE;
}

int
_soc_tr3_hsp_set_sched_config(int unit, soc_port_t port, int index, int cosq,
                                    soc_tr3_sched_mode_e mode)
{
    uint32 rval = 0, temp = 0, q_spmap;
    int cos;

    SOC_IF_ERROR_RETURN(_soc_tr3_hsp_get_cos_index(unit, port, index, cosq, &cos));

    if (mode == SOC_TR3_SCHED_MODE_STRICT) {
        SOC_IF_ERROR_RETURN(_soc_tr3_hsp_sched_weight_set(unit, port, index, cosq, 0));
        temp = 1 << cos; 
    }
    /* Note that we don't differ b/w wrr and werr at queue level */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_MINSPr, port, 0, &rval));
    q_spmap = soc_reg_field_get(unit, HES_Q_MINSPr, rval, MINSPCONFIGf);
    q_spmap = q_spmap | temp;
    soc_reg_field_set(unit, HES_Q_MINSPr, &rval, MINSPCONFIGf, q_spmap);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HES_Q_MINSPr, port, 0,  rval));
    return SOC_E_NONE;
}

int
_soc_tr3_hsp_get_sched_config(int unit, soc_port_t port, int index, int cosq,
                             soc_tr3_sched_mode_e *mode)
{
    uint32 rval = 0, q_spmap;
    int cos;

    SOC_IF_ERROR_RETURN(_soc_tr3_hsp_get_cos_index(unit, port, index, cosq, &cos));

    /* Note that we don't differ b/w wrr and werr at queue level */

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_MINSPr, port, 0, &rval));
    q_spmap = soc_reg_field_get(unit, HES_Q_MINSPr, rval, MINSPCONFIGf);

    if ((q_spmap >> cos) & 1) {
        *mode = SOC_TR3_SCHED_MODE_STRICT; 
    } else {
        *mode = SOC_TR3_SCHED_MODE_WRR;
    }
    return SOC_E_NONE;
}
typedef struct _soc_tr3_lls_config_s {
    uint32 level;
    int    node_id;
    int    num_children;
    int    sched_mode;
    int    weights[48];
    /* Only for L2 nodes it denotes which of the children are UC (1), MC(0) */
    uint32 uc_mc_map;
} _soc_tr3_lls_config_t;

static int tr3_sm_port_num_sched_at_lvl[] = {
                /* root */ 1,
                /* L0 */   1,
                /* L1 */   1
                                       };

static int tr3_lb_port_num_sched_at_lvl[] = {
                /* root */ 1,
                /* L0 */   2,
                /* L1 */   9
                                       };

static int tr3_port_num_sched_at_lvl[] = {
                /* root */ 1,
                /* L0 */   1,
                /* L1 */   8
                                       };

static int tr3_hg_port_num_sched_at_lvl[] = {
                /* root */ 1,
                /* L0 */   1,
                /* L1 */   9
                                       };

static int tr3_cpu_num_sched_at_lvl[] = {
                /* root */ 1,
                /* L0 */   3,
                /* L1 */   10
                                       };

static _soc_tr3_lls_config_t _tr3_port_sm_lls_config[] = {
    /* SOC_TR3_NODE_LVL_ROOT */
    { SOC_TR3_NODE_LVL_ROOT, 0,
      1, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L0 L0.0 */
    { SOC_TR3_NODE_LVL_L0, 0,
      1, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.0 */
    { SOC_TR3_NODE_LVL_L1, 0,
      1, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x0 /* 1 UC nodes */},
    /* SOC_TR3_NODE_LVL_L2 */
    { -1, 0, 0, -1,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      1 }
};

static _soc_tr3_lls_config_t _tr3_lb_port_lls_config[] = {
    /* SOC_TR3_NODE_LVL_ROOT */
    { SOC_TR3_NODE_LVL_ROOT, 0,
      2, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
	/* SOC_TR3_NODE_LVL_L0 L0.0 */
    { SOC_TR3_NODE_LVL_L0, 0,
      1, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L0 L0.1 */
    { SOC_TR3_NODE_LVL_L0, 1,
      8, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.0 */
    { SOC_TR3_NODE_LVL_L1, 0,
      1, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
        0 /* 1 MC (QCN) node */},  
    /* SOC_TR3_NODE_LVL_L1 L1.1 */
    { SOC_TR3_NODE_LVL_L1, 1,
      2, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.2 */
    { SOC_TR3_NODE_LVL_L1, 2,
      2, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.3 */
    { SOC_TR3_NODE_LVL_L1, 3,
      2, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.4 */
    { SOC_TR3_NODE_LVL_L1, 4,
      2, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
     /* SOC_TR3_NODE_LVL_L1 L1.5 */
    { SOC_TR3_NODE_LVL_L1, 5,
      2, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.6 */
    { SOC_TR3_NODE_LVL_L1, 6,
      2, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.7 */
    { SOC_TR3_NODE_LVL_L1, 7,
      2, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.8 */
    { SOC_TR3_NODE_LVL_L1, 8,
      2, SOC_TR3_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L2 */
    { -1, 0, 0, -1,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      1 }
};

static _soc_tr3_lls_config_t _tr3_port_lls_config[] = {
    /* SOC_TR3_NODE_LVL_ROOT */
    { SOC_TR3_NODE_LVL_ROOT, 0,
      1, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L0 L0.0 */
    { SOC_TR3_NODE_LVL_L0, 0,
      8, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.0 */
    { SOC_TR3_NODE_LVL_L1, 0,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.1 */
    { SOC_TR3_NODE_LVL_L1, 1,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.2 */
    { SOC_TR3_NODE_LVL_L1, 2,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.3 */
    { SOC_TR3_NODE_LVL_L1, 3,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.4 */
    { SOC_TR3_NODE_LVL_L1, 4,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.5 */
    { SOC_TR3_NODE_LVL_L1, 5,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.6 */
    { SOC_TR3_NODE_LVL_L1, 6,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.7 */
    { SOC_TR3_NODE_LVL_L1, 7,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L2 */
    { -1, 0, 0, -1,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      1 }
};

static _soc_tr3_lls_config_t _tr3_port_dyn_lls_config[] = {
    /* SOC_TR3_NODE_LVL_ROOT */
    { SOC_TR3_NODE_LVL_ROOT, 0,
      1, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L0 L0.0 */
    { SOC_TR3_NODE_LVL_L0, 0,
      16, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.0 */
    { SOC_TR3_NODE_LVL_L1, 0,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.1 */
    { SOC_TR3_NODE_LVL_L1, 1,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.2 */
    { SOC_TR3_NODE_LVL_L1, 2,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.3 */
    { SOC_TR3_NODE_LVL_L1, 3,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.4 */
    { SOC_TR3_NODE_LVL_L1, 4,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.5 */
    { SOC_TR3_NODE_LVL_L1, 5,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.6 */
    { SOC_TR3_NODE_LVL_L1, 6,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.7 */
    { SOC_TR3_NODE_LVL_L1, 7,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.8 */
    { SOC_TR3_NODE_LVL_L1, 8,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.9 */
    { SOC_TR3_NODE_LVL_L1, 9,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.10 */
    { SOC_TR3_NODE_LVL_L1, 10,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.11 */
    { SOC_TR3_NODE_LVL_L1, 11,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.12 */
    { SOC_TR3_NODE_LVL_L1, 12,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.13 */
    { SOC_TR3_NODE_LVL_L1, 13,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.14 */
    { SOC_TR3_NODE_LVL_L1, 14,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.15 */
    { SOC_TR3_NODE_LVL_L1, 15,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L2 */
    { -1, 0, 0, -1,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      1 }
};

static _soc_tr3_lls_config_t _tr3_hg_port_lls_config[] = {
    /* SOC_TR3_NODE_LVL_ROOT */
    { SOC_TR3_NODE_LVL_ROOT, 0,
      1, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L0 L0.0 */
    { SOC_TR3_NODE_LVL_L0, 0,
      9, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.0 */
    { SOC_TR3_NODE_LVL_L1, 0,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.1 */
    { SOC_TR3_NODE_LVL_L1, 1,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.2 */
    { SOC_TR3_NODE_LVL_L1, 2,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.3 */
    { SOC_TR3_NODE_LVL_L1, 3,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.4 */
    { SOC_TR3_NODE_LVL_L1, 4,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.5 */
    { SOC_TR3_NODE_LVL_L1, 5,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.6 */
    { SOC_TR3_NODE_LVL_L1, 6,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.7 */
    { SOC_TR3_NODE_LVL_L1, 7,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.8 */
    { SOC_TR3_NODE_LVL_L1, 8,
      4, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x3 /* 2 UC nodes, 2 MC node */},
    /* SOC_TR3_NODE_LVL_L2 */
    { -1, 0, 0, -1,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      1 }
};

static _soc_tr3_lls_config_t _tr3_hg_port_dyn_lls_config[] = {
    /* SOC_TR3_NODE_LVL_ROOT */
    { SOC_TR3_NODE_LVL_ROOT, 0,
      1, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L0 L0.0 */
    { SOC_TR3_NODE_LVL_L0, 0,
      17, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.0 */
    { SOC_TR3_NODE_LVL_L1, 0,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.1 */
    { SOC_TR3_NODE_LVL_L1, 1,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.2 */
    { SOC_TR3_NODE_LVL_L1, 2,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.3 */
    { SOC_TR3_NODE_LVL_L1, 3,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.4 */
    { SOC_TR3_NODE_LVL_L1, 4,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.5 */
    { SOC_TR3_NODE_LVL_L1, 5,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.6 */
    { SOC_TR3_NODE_LVL_L1, 6,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.7 */
    { SOC_TR3_NODE_LVL_L1, 7,
      2, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.8 */
    { SOC_TR3_NODE_LVL_L1, 8,
      4, SOC_TR3_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x3 /* 2 UC nodes, 2 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.9 */
    { SOC_TR3_NODE_LVL_L1, 9,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TR3_NODE_LVL_L1 L1.10 */
    { SOC_TR3_NODE_LVL_L1, 10,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.11 */
    { SOC_TR3_NODE_LVL_L1, 11,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.12 */
    { SOC_TR3_NODE_LVL_L1, 12,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.13 */
    { SOC_TR3_NODE_LVL_L1, 13,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.14 */
    { SOC_TR3_NODE_LVL_L1, 14,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.15 */
    { SOC_TR3_NODE_LVL_L1, 15,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L1 L1.16 */
    { SOC_TR3_NODE_LVL_L1, 16,
      0, SOC_TR3_SCHED_MODE_STRICT, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TR3_NODE_LVL_L2 */
    { -1, 0, 0, -1,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      1 }
};

STATIC _soc_tr3_lls_config_t *
_soc_tr3_get_config_for_level(_soc_tr3_lls_config_t *cfg_tbl, int lvl, 
                              int offset)
{
    _soc_tr3_lls_config_t *en = cfg_tbl;

    while (en->level != -1) {
        if ((en->level == lvl) && (en->node_id == offset)) {
            return en;
        }
        en++;
    }

    return NULL;
}

int _soc_tr3_invalid_parent_index(int unit, int level)
{
    return _tr3_invalid_ptr[unit][level];
}

#define INVALID_PARENT(unit, level)   _soc_tr3_invalid_parent_index((unit),(level))

STATIC int
soc_tr3_port_lls_init(int unit, int port, _soc_tr3_lls_config_t *cfg_tbl,
                      int setup, _soc_tr3_lls_traverse_cb cb, void *cookie)
{
    _soc_tr3_lls_config_t *pinfo, *cinfo;
    struct _tr3_sched_pending_setup_s {
        int     parent;
        int     level;
        int     offset;
        int     hw_index;
    } pending_list[64], *ppending;
    int  list_size, lindex, child_lvl, uc;
    int  l2_lvl_offsets[2], numq, lvl_offsets[4];
    int spri, smcpri, num_spri, qnum, self_hw_index, c;
    uint32 ucmap, spmap, rval;
    int phy_port, mmu_port;
    soc_info_t *si;
    int sp_vec = soc_feature(unit, soc_feature_tr3_sp_vector_mask);
    int qcn_queue = 0;
    int rc = SOC_E_NONE;
    int weight;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (sp_vec) {
        SOC_IF_ERROR_RETURN(READ_LLS_CONFIG0r(unit, &rval));
        if (!soc_reg_field_get(unit, LLS_CONFIG0r, rval, SPRI_VECT_MODE_ENABLEf)) {
            soc_reg_field_set(unit, LLS_CONFIG0r, &rval, SPRI_VECT_MODE_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_LLS_CONFIG0r(unit, rval)); 
        }
    }

    /* To attach QCN cosq to AXP0*/
    if (mmu_port == 58) {
        qcn_queue = 1; 
    }

    l2_lvl_offsets[0] = l2_lvl_offsets[1] = 0;
    lvl_offsets[0] = lvl_offsets[1] = lvl_offsets[2] = lvl_offsets[3] = 0;

    /* setup port scheduler */
    pending_list[0].parent = -1;
    pending_list[0].level = SOC_TR3_NODE_LVL_ROOT;
    pending_list[0].offset = 0;
    pending_list[0].hw_index = _SOC_TR3_ROOT_SCHEDULER_INDEX(unit, port);
    list_size = 1;
    lindex = 0;
    do {
        ppending = &pending_list[lindex++];
        self_hw_index = ppending->hw_index;
        if (setup == _SOC_TR3_LLS_OP_TRAVERSE) {
            cb(unit, port, ppending->level, ppending->hw_index, cookie);
        } else if (ppending->parent != -1) {
            /* attach to parent */
            SOC_IF_ERROR_RETURN(
                soc_tr3_cosq_set_sched_parent(unit, port, ppending->level, self_hw_index, 
                    (setup) ? ppending->parent : INVALID_PARENT(unit, ppending->level)));
        }

        if (ppending->level == SOC_TR3_NODE_LVL_L2) {
            continue;
        }
        
        pinfo = _soc_tr3_get_config_for_level(cfg_tbl, 
                                              ppending->level, 
                                              ppending->offset);
        if (!pinfo) {
            return SOC_E_INTERNAL;
        }
            
        soc_tr3_get_child_type(unit, port, ppending->level, &child_lvl);
        ucmap = 0;
        spmap = 0;
        spri = -1;
        smcpri = -1;
        num_spri = 0;
        for (c = 0; c < pinfo->num_children; c++) {
            if ((setup != _SOC_TR3_LLS_OP_TRAVERSE) && 
                (pinfo->level == SOC_TR3_NODE_LVL_L1) && (mmu_port == 58) &&
                !qcn_queue) {
                /* connect EP redirect queues */
                if (pinfo->uc_mc_map & (1 << c)) {
                    continue;
                }
                weight = pinfo->weights[c]; 
                if (pinfo->sched_mode == SOC_TR3_SCHED_MODE_WRR) {
                    weight *= 2;
                }
                qnum = 560 + l2_lvl_offsets[0]; 
                SOC_IF_ERROR_RETURN(
                    soc_tr3_sched_weight_set(unit, SOC_TR3_NODE_LVL_L2,
                                         soc_tr3_l2_hw_index(unit, qnum, 0),
                                         weight));
                SOC_IF_ERROR_RETURN(
                    soc_tr3_cosq_set_sched_parent(unit, port, SOC_TR3_NODE_LVL_L2, 
                        soc_tr3_l2_hw_index(unit, qnum, 0),
                        (setup) ? self_hw_index :
                        INVALID_PARENT(unit, SOC_TR3_NODE_LVL_L2)));
            }
        
            pending_list[list_size].parent = self_hw_index;
            pending_list[list_size].level = child_lvl;
            pending_list[list_size].offset = lvl_offsets[child_lvl];
            lvl_offsets[child_lvl] += 1;
            if (child_lvl == SOC_TR3_NODE_LVL_L2) {
                uc = 0; /* default MC queue */
                if (!IS_CPU_PORT(unit, port)) {
                    uc = (pinfo->uc_mc_map & (1 << c)) ? 1 : 0;
                }

                if (qcn_queue != 1) {
                    rc = soc_tr3_get_def_qbase(unit, port, 
                                    (uc ? _SOC_TR3_INDEX_STYLE_UCAST_QUEUE : 
                           _SOC_TR3_INDEX_STYLE_MCAST_QUEUE), &qnum, &numq);

                    if (rc) {
                        return rc;
                    }

                    if ((numq == 0) || (qnum < 0)) {
                        continue;
                    }
                    qnum = soc_tr3_l2_hw_index(unit, qnum, uc);
                    pending_list[list_size].hw_index = qnum + l2_lvl_offsets[uc];
                    l2_lvl_offsets[uc]++;
                } else {
                    /* QCN queue details. */
                    qnum = 505; 
                    numq = 1;   
                    qcn_queue = 0;

                    qnum = soc_tr3_l2_hw_index(unit, qnum, uc);
                    pending_list[list_size].hw_index = qnum;
                }
                if (uc) {
                    if (spri == -1) {
                        spri = pending_list[list_size].hw_index;
                    }
                } else {
                    if (smcpri == -1) {
                        smcpri = pending_list[list_size].hw_index;
                    }
                }
                if ((pinfo->sched_mode == SOC_TR3_SCHED_MODE_STRICT) &&
                    (!IS_CPU_PORT(unit, port))) {
                    ucmap |= (uc) ? 0 : (1 << c);
                    if (num_spri == 0) {
                        if (uc) {
                            spri = pending_list[list_size].hw_index;
                        } else {
                            smcpri = pending_list[list_size].hw_index;
                        }
                    }
                    num_spri++;
                    spmap |= 1 << c;
                }
            
                if (setup == _SOC_TR3_LLS_OP_TRAVERSE) {
                    list_size++;
                } else {
                    SOC_IF_ERROR_RETURN(
                            soc_tr3_cosq_set_sched_parent(unit, port, 
                                  SOC_TR3_NODE_LVL_L2, 
                                  pending_list[list_size].hw_index, 
                                  (setup) ? self_hw_index :
                                  INVALID_PARENT(unit, SOC_TR3_NODE_LVL_L2)));

                    SOC_IF_ERROR_RETURN(
                        soc_tr3_cosq_set_sched_mode(unit, port, SOC_TR3_NODE_LVL_L2,
                                pending_list[list_size].hw_index,
                                pinfo->sched_mode, pinfo->weights[c]));
                }
            } else {
                cinfo = _soc_tr3_get_config_for_level(cfg_tbl, child_lvl, 
                                            pending_list[list_size].offset);
                if (!cinfo) {
                    return SOC_E_INTERNAL;
                }
                SOC_IF_ERROR_RETURN(soc_tr3_sched_hw_index_get(unit, 
                                    port, child_lvl, pending_list[list_size].offset, 
                                    &pending_list[list_size].hw_index));
                if (spri == -1) {
                    spri = pending_list[list_size].hw_index;
                }
                if (cinfo->sched_mode == SOC_TR3_SCHED_MODE_STRICT) {
                    if (num_spri == 0) {
                        spri = pending_list[list_size].hw_index;
                    }
                    num_spri++;
                    spmap |= 1 << c;
                }

                if (setup != _SOC_TR3_LLS_OP_TRAVERSE) {
                    SOC_IF_ERROR_RETURN(
                            soc_tr3_cosq_set_sched_parent(unit, port, child_lvl, 
                                pending_list[list_size].hw_index,
                                (setup) ? self_hw_index :
                                INVALID_PARENT(unit, child_lvl)));


                    SOC_IF_ERROR_RETURN(
                        soc_tr3_cosq_set_sched_mode(unit, port, child_lvl,
                                pending_list[list_size].hw_index,
                                cinfo->sched_mode, pinfo->weights[c]));
                }
                list_size++;
            }
        }

        if (spri == -1) {
            spri = 0;
        }
        if (smcpri == -1) {
            smcpri = 1024;
        }

        if (setup != _SOC_TR3_LLS_OP_TRAVERSE) {
            SOC_IF_ERROR_RETURN(
                soc_tr3_cosq_set_sched_child_config(unit, port, 
                                ppending->level, self_hw_index, 
                                num_spri, spri, smcpri, ucmap, spmap));
        }
    } while (lindex < list_size);

    return SOC_E_NONE;
}

STATIC int
soc_tr3_cpu_port_lls_init(int unit, int port, int setup, int reserve,
                            _soc_tr3_lls_traverse_cb cb, void *cookie)
{
    int l1_cnt[3], l2_cnt[3];
    _soc_tr3_lls_config_t *lnc, *pnode;
    int max_q = 48, nc, l1off = 0, rv = SOC_E_NONE, cmc;
    int num_l0, num_l1, l0_0, l0_1, l0_2, num_nodes, w, ii, jj;

    if (SOC_IS_HELIX4(unit)) { 
        max_q = (reserve==0) ? 48 : 44;
    } else {
        max_q = (reserve==0) ? 48 : 45;
    }

    /* alloc structure to hold lls node data, init it and kick off 
     * soc_tr3_port_lls_init */
    
    l2_cnt[1] = l0_1 = NUM_CPU_ARM_COSQ(unit, SOC_ARM_CMC(unit, 0));
    l2_cnt[2] = l0_2 = NUM_CPU_ARM_COSQ(unit, SOC_ARM_CMC(unit, 1));
    l2_cnt[0] = l0_0 = max_q - (l0_1 + l0_2);

    if (l0_0 <= 0) {
        return SOC_E_PARAM;
    }

    num_l0 = 1 + (l0_1 > 0 ? 1 : 0) + (l0_2 > 0 ? 1 : 0);

    /* keep a max of 8 nodes under each l1. */
    l1_cnt[0] = l1_cnt[1] = l1_cnt[2] = 0;

    l1_cnt[0] = (l0_0 + 7)/8;
    l1_cnt[1] = (l0_1 + 7)/8;
    l1_cnt[2] = (l0_2 + 7)/8;

    num_l1  = l1_cnt[0] + l1_cnt[1] + l1_cnt[2];

    num_nodes = 1 /* root */ + num_l0 + num_l1 + 1 /* null last node */;

    for (cmc = 0, ii = 0; cmc < 3; cmc++) {
        SHR_BITCLR_RANGE(CPU_ARM_QUEUE_BITMAP(unit, cmc), 0, 48);
        SHR_BITCLR_RANGE(CPU_ARM_RSVD_QUEUE_BITMAP(unit, cmc), 0, 48);
        for (jj = 0; jj < l2_cnt[cmc]; jj++, ii++) {
            SHR_BITSET(CPU_ARM_QUEUE_BITMAP(unit, cmc), ii);
        }
        NUM_CPU_ARM_COSQ(unit, cmc) = l2_cnt[cmc];
    }
    /* Setting the max_q again to 45 so that RESERVED QUEUE BITMAP
       is set always set for 45, 46,47 for TR3 and HX4 */
    if (SOC_IS_HELIX4(unit)) { 
        max_q = (reserve==0) ? 48 : 45;
    }
    for (ii = max_q ; ii < 48; ii++) {
        cmc = ii - max_q;
        if (l1_cnt[cmc] > 0) {
            SHR_BITSET(CPU_ARM_RSVD_QUEUE_BITMAP(unit, cmc), ii);
        }
    }
        
    lnc = sal_alloc(sizeof(_soc_tr3_lls_config_t)*num_nodes, "CPU LLS config");
    if (!lnc) {
        return SOC_E_MEMORY;
    }

    /* init root */
    pnode = lnc;
    pnode->level = SOC_TR3_NODE_LVL_ROOT;
    pnode->node_id = 0;
    pnode->sched_mode = SOC_TR3_SCHED_MODE_WRR;
    pnode->num_children = num_l0;
    pnode->uc_mc_map = 0;
    for (w = 0; w < 48; w++) {
        pnode->weights[w] = 1;
    }
   
    pnode++;
    l1off = 0;

    for (ii = 0; ii < num_l0; ii++) {
        pnode->level = SOC_TR3_NODE_LVL_L0;
        pnode->node_id = ii;
        pnode->num_children = l1_cnt[ii];
        pnode->sched_mode = SOC_TR3_SCHED_MODE_WRR;
        pnode->uc_mc_map = 0;
        for (w = 0; w < 48; w++) {
            pnode->weights[w] = 1;
        }
        pnode++;

        for (nc = 0, jj = 0; jj < l1_cnt[ii]; jj++) {
            pnode->level = SOC_TR3_NODE_LVL_L1;
            pnode->node_id = l1off++;

            if ((l2_cnt[ii] - nc) >= 8) {
                pnode->num_children = 8;
            } else {
                pnode->num_children = l2_cnt[ii] - nc;
            }
            nc += pnode->num_children;

            pnode->sched_mode = SOC_TR3_SCHED_MODE_WRR;
            pnode->uc_mc_map = 0;
            for (w = 0; w < 48; w++) {
                pnode->weights[w] = 1;
            }
            pnode++;
        }
    }

    /* null node */
    pnode->level = -1;
    pnode->node_id = -1;
    pnode->num_children = 0;
    pnode->uc_mc_map = 0;

    rv = soc_tr3_port_lls_init(unit, port, lnc, setup, cb, cookie);

    sal_free(lnc);
    return rv;
}

_soc_tr3_axp_lls_config _tr3_axp_lls_config[] = {
    {58, {-1,   1504,   -1}, {8, 8, 0}},
    {59, {-1,     -1,   -1}, {8, 0, 0}},
    {60, {-1,   1528,   -1}, {0, 1, 0}},
    {61, {1529, 1512, 1584}, {1, 8, 8}}
};

int
_soc_tr3_cosq_axp_node_validate(int unit, int local_port, int hw_index, int to_cos)
{
    int port, node_index = -1;
    int axp_min, axp_max, found;
    int count = 0;

    if (hw_index == -1) {
        return SOC_E_PARAM;
    }
    for (port = 0; port < 4; port++) {
        if (_tr3_axp_lls_config[port].local_port == local_port) {
            count = 0;
            found = 0;
            while (count < 3) {
                axp_min = _tr3_axp_lls_config[port].base[count];
                if ((axp_min == -1) && (hw_index < 1024)) {
                    /* Check if hw_index is a valid UC */
                    axp_max = _tr3_axp_lls_config[port].numq[count];
                    node_index = to_cos;
                    if (node_index < axp_max) {
                        found = 1;
                        break;
                    }
                } else if ((axp_min != -1) && (hw_index >= 1024)) {
                    /* Check if hw_index is valid AXP MC */
                    node_index = hw_index;
                    axp_max = axp_min + _tr3_axp_lls_config[port].numq[count];
                    if ((node_index >= axp_min) &&
                        (node_index < axp_max)) {
                        found = 1;
                        break;
                    }
                }
                count++;
            }
            if (found == 1) {
                return SOC_E_NONE;
            }
        }
    }
    return SOC_E_PARAM;
}

STATIC int
_soc_tr3_ce_port_index(int unit, int port)
{
    int phy_port, mmu_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (mmu_port == 48) {
        return 0;
    } else if (mmu_port == 52) {
        return 1;
    }
    return -1;
}

#define TR3_DFL_HSP_SCHED_MODE  SOC_TR3_SCHED_MODE_WDRR

STATIC int
soc_tr3_setup_hsp_port(int unit, int port)
{
    int ceid, hw_index, qnum;
    soc_field_t f;
    uint32 rval;
    static const soc_field_t hl0basef[] = {
        PORT0_L0_OFFSETf, PORT1_L0_OFFSETf
    };
    static const soc_field_t hl2basef[] = {
        PORT0_UCQ_OFFSETf, PORT1_UCQ_OFFSETf 
    };
    static const soc_field_t hspef[] = {
        HSP0f, HSP1f
    };

    if (!_SOC_TR3_IS_HSP_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_tr3_sched_hw_index_get(unit, port, 
                                    SOC_TR3_NODE_LVL_L0, 0, &hw_index));
    SOC_IF_ERROR_RETURN(soc_tr3_hsp_sched_hw_index_get(unit, port,
                                         SOC_TR3_NODE_LVL_L0, 0, 0, &hw_index));
    ceid = _soc_tr3_ce_port_index(unit, port);
    if (ceid == -1) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HSP_CONFIGr, port, 0, &rval));
    f = hl0basef[ceid];
    soc_reg_field_set(unit, HSP_CONFIGr, &rval, f, 
                ((hw_index - 1) >= 0) ? (hw_index - 1) : 0);
    soc_tr3_get_def_qbase(unit, port, _SOC_TR3_INDEX_STYLE_UCAST_QUEUE,
                          &qnum, NULL);
    f = hl2basef[ceid];
    soc_reg_field_set(unit, HSP_CONFIGr, &rval, f, qnum/4);
    
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HSP_CONFIGr, port, 0, rval));

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_L0_CONFIGr, port, 0, &rval));
    soc_reg_field_set(unit, HES_L0_CONFIGr, &rval, MCQ_MODEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HES_L0_CONFIGr, port, 0, rval));

    /* Ensure that when scheduling minumum bandwidth, SC and QM take precedence */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_PORT_CONFIGr, port, 0, &rval));
    soc_reg_field_set(unit, HES_PORT_CONFIGr, &rval, MINSPCONFIGf, (1 << 9));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, HES_PORT_CONFIGr, port, 0, rval));

    /* Set special queue group COS (SC and QM) to strict priority */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, L0_COSWEIGHTSr, port, 9, &rval));
    soc_reg_field_set(unit, L0_COSWEIGHTSr, &rval, COSWEIGHTSf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, L0_COSWEIGHTSr, port, 9, rval));

    /* Enable HSP */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, MMU_CFG_HSP_CONFIGr, port, 0, &rval));
    soc_reg_field_set(unit, MMU_CFG_HSP_CONFIGr, &rval, hspef[ceid], 1);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, MMU_CFG_HSP_CONFIGr, port, 0, rval));

    return SOC_E_NONE;
}

STATIC int _soc_tr3_lls_init(int unit)
{
    _soc_tr3_b0_cb_t *pcb;
    uint32 mmu_bmp[4];
    int ii, port;
    int phy_port, mmu_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    if (_soc_tr3_init_done[unit]) {
        return SOC_E_NONE;
    }
    
    if (soc_feature(unit, soc_feature_tr3_sp_vector_mask)) {
        pcb = &_tr3_b0_unit_data[unit];
        pcb->lock = sal_mutex_create("tr3_b0_dyn_lock");
        for (ii = 0; ii < _SOC_TR3_DYN_SET; ii++) {
            pcb->port[ii] = -1;
        }

        if (!soc_feature(unit, soc_feature_cmic_reserved_queues)) {
            tr3_cpu_num_sched_at_lvl[1] += 3;
        }
    }

    _tr3_invalid_ptr[unit][SOC_TR3_NODE_LVL_ROOT] = -1;
    _tr3_invalid_ptr[unit][SOC_TR3_NODE_LVL_L1] = 
                            soc_mem_index_max(unit, LLS_L0_PARENTm);
    _tr3_invalid_ptr[unit][SOC_TR3_NODE_LVL_L2] =
                            soc_mem_index_max(unit, LLS_L1_PARENTm);
    _tr2_node_max[unit][SOC_TR3_NODE_LVL_L0] = soc_mem_index_max(unit, LLS_L0_CONFIGm);
    _tr2_node_max[unit][SOC_TR3_NODE_LVL_L1] = soc_mem_index_max(unit, LLS_L1_CONFIGm);

    sal_memset(mmu_bmp, 0, sizeof(uint32)*4);
    PBMP_ALL_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        mmu_bmp[mmu_port/32] |= 1 << (mmu_port % 32);
    }

    for (ii = 0; ii < soc_mem_index_max(unit, LLS_L0_PARENTm); ii++) {
        if ((mmu_bmp[ii/32] & (1 << (ii % 32))) == 0) {
            _tr3_invalid_ptr[unit][SOC_TR3_NODE_LVL_L0] = ii;
            break;
        }
    }
    _soc_tr3_init_done[unit] = 1;
    return SOC_E_NONE;
}

int soc_tr3_lls_init(int unit)
{
    int dyn_mode, dyn_ports = 0, lvl, i;
    int port, rv = SOC_E_NONE;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    int phy_port, mmu_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN(_soc_tr3_lls_init(unit));

    PBMP_ALL_ITER(unit, port) {
        if (soc_feature(unit, soc_feature_tr3_sp_vector_mask)) {
            /* For TR3 >= B0 Rev, SW workaround for Dyn update is not needed. */
            _bcm_tr3_port_sdyn[unit][port] = 0;
        } else {
            _bcm_tr3_port_sdyn[unit][port] = soc_property_port_get(unit, port, 
                                                spn_PORT_SCHED_DYNAMIC, 0);
        }
        
        if (_SOC_TR3_IS_HSP_PORT(unit, port)) {
            continue;
        } else if (IS_CPU_PORT(unit, port)) {
            dyn_ports += 1;
        } else if (_bcm_tr3_port_sdyn[unit][port]) {
            dyn_ports += 16;
        } else {
            dyn_ports += 8;
        }
    }
    if (dyn_ports >= 512) {
        /* Too many dynamic ports for HW resources.
         * Use static mode instead.
         */
        LOG_ERROR(BSL_LS_SOC_COSQ,
                  (BSL_META_U(unit,
                              "unit %d : Cannot configure requested dynamic scheduler ports.\n"
                              "\tAvailable HW resources exhausted.\n"),
                   unit));
        return SOC_E_RESOURCE;
    }

    /* set the parent memories to a reserved scheduler instance at each level */
    for (lvl = SOC_TR3_NODE_LVL_L0; lvl <= SOC_TR3_NODE_LVL_L2; lvl++) {
        mem = _SOC_TR3_NODE_PARENT_MEM(lvl);
    
        sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
        soc_mem_field32_set(unit, mem, entry, C_PARENTf, INVALID_PARENT(unit, lvl));
    
        for (i = 0; i <= soc_mem_index_max(unit, mem); i++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &entry));
        }
    }

    PBMP_ALL_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        dyn_mode = _bcm_tr3_port_sdyn[unit][port];
        if (_SOC_TR3_IS_HSP_PORT(unit, port)) {
            rv = soc_tr3_setup_hsp_port(unit, port);
        } else if (IS_CPU_PORT(unit, port)) {
            if (soc_feature(unit, soc_feature_cmic_reserved_queues)) {
                rv = soc_tr3_cpu_port_lls_init(unit, port, _SOC_TR3_LLS_OP_SETUP,
                                               TRUE, NULL, NULL);
            } else {
                rv = soc_tr3_cpu_port_lls_init(unit, port, _SOC_TR3_LLS_OP_SETUP,
                                               FALSE, NULL, NULL);
            }
        } else if (mmu_port == 61) {
            rv = soc_tr3_port_lls_init(unit, port, _tr3_port_sm_lls_config,
                                        _SOC_TR3_LLS_OP_SETUP, NULL, NULL);
        } else {
            if (soc_port_hg_capable(unit, port)) {
                rv = soc_tr3_port_lls_init(unit, port, 
                 (dyn_mode) ?  _tr3_hg_port_dyn_lls_config : _tr3_hg_port_lls_config,
                               _SOC_TR3_LLS_OP_SETUP, NULL, NULL);
            } else if (mmu_port == 58) {
                rv = soc_tr3_port_lls_init(unit, port,
                                    _tr3_lb_port_lls_config,
                                    _SOC_TR3_LLS_OP_SETUP, NULL, NULL);
            } else {
                rv = soc_tr3_port_lls_init(unit, port, 
                    (dyn_mode) ? _tr3_port_dyn_lls_config : _tr3_port_lls_config, 
                                    _SOC_TR3_LLS_OP_SETUP, NULL, NULL);
            }
        }
        if (rv) {
            return SOC_E_INTERNAL;
        }
    }
    return SOC_E_NONE;
}

int soc_tr3_lls_port_uninit(int unit, soc_port_t port)
{
    int rv = SOC_E_INTERNAL;

    if (_SOC_TR3_IS_HSP_PORT(unit, port)) {
        rv = SOC_E_UNAVAIL;
    } else if (IS_CPU_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_cmic_reserved_queues)) {
            rv = soc_tr3_cpu_port_lls_init(unit, port, _SOC_TR3_LLS_OP_TEAR, 1, NULL, NULL);
        } else {
            rv = soc_tr3_cpu_port_lls_init(unit, port, _SOC_TR3_LLS_OP_TEAR, 0, NULL, NULL);
        }
    } else {
        if (soc_port_hg_capable(unit, port)) {
            rv = soc_tr3_port_lls_init(unit, port, _tr3_hg_port_lls_config, 
                                        _SOC_TR3_LLS_OP_TEAR, NULL, NULL);
        } else {
            rv = soc_tr3_port_lls_init(unit, port, _tr3_port_lls_config, 
                                        _SOC_TR3_LLS_OP_TEAR, NULL, NULL);
        }
    }

    return rv;
}

/* To reinstate Queue mapping for LB(Internal ports) */
int soc_tr3_lb_lls_init(int unit)
{
    int axp_port, phy_port, mmu_port, rv = SOC_E_NONE;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    PBMP_AXP_ITER(unit, axp_port) {
        phy_port = si->port_l2p_mapping[axp_port];
        mmu_port = si->port_p2m_mapping[phy_port];
        rv = soc_tr3_port_lls_init(unit, axp_port,
                (mmu_port == 61) ? _tr3_port_sm_lls_config : (mmu_port == 58) ?
                                _tr3_lb_port_lls_config : _tr3_port_lls_config,
                                            _SOC_TR3_LLS_OP_SETUP, NULL, NULL);
        if (rv < 0) {
            return SOC_E_INTERNAL;
        }
    }
    return 0;
}

int soc_tr3_lb_lls_port_uninit(int unit, int axp_port)
{
    int phy_port, mmu_port, rv = SOC_E_NONE;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    if (!SOC_PBMP_MEMBER(PBMP_AXP_ALL(unit), axp_port)) {
        return SOC_E_PARAM;
    }
    phy_port = si->port_l2p_mapping[axp_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    rv = soc_tr3_port_lls_init(unit, axp_port,
            (mmu_port == 61) ? _tr3_port_sm_lls_config : (mmu_port == 58) ?
                            _tr3_lb_port_lls_config : _tr3_port_lls_config,
                                        _SOC_TR3_LLS_OP_TEAR, NULL, NULL);
    if (rv < 0) {
        return SOC_E_INTERNAL;
    }
    return 0;
}

/* Disable Queuing on all L2 queues */
int soc_tr3_lls_l2_disable(int unit)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    int i, port, start_hw_index, numq = 0;
    soc_mem_t mem;

    mem = MMU_THDO_Q_TO_QGRP_MAPm;

    for (i = 0; i <= soc_mem_index_max(unit, mem); i++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                            MEM_BLOCK_ANY, i, &entry));
        soc_mem_field32_set(unit, mem , entry, DISABLE_QUEUINGf, 1);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &entry));
    }

    PBMP_ALL_ITER(unit, port) {
        if (!_SOC_TR3_IS_HSP_PORT(unit, port)) {
            continue;
        }
        start_hw_index = SOC_INFO(unit).port_uc_cosq_base[port];
        numq = SOC_INFO(unit).port_num_uc_cosq[port];
        for (i = start_hw_index; i < (start_hw_index + numq); i++) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                MEM_BLOCK_ANY, i, &entry));
            soc_mem_field32_set(unit, mem , entry, DISABLE_QUEUINGf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &entry));
        }
    }
    return 0;
}

int soc_tr3_lls_reset(int unit)
{
    int lvl, i;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    int alloc_size, port;
    uint32 *bmap = NULL;

    PBMP_ALL_ITER(unit, port) {
        for (lvl = SOC_TR3_NODE_LVL_L0; lvl <= SOC_TR3_NODE_LVL_L2; lvl++) {
            mem = _SOC_TR3_NODE_PARENT_MEM(lvl);
            /* coverity[negative_returns : FALSE] */
            alloc_size = SHR_BITALLOCSIZE(soc_mem_index_count(unit, mem));
            if (lvl == SOC_TR3_NODE_LVL_L0) {
                if (SOC_CONTROL(unit)->port_lls_l0_bmap[port] == NULL) {
                    SOC_CONTROL(unit)->port_lls_l0_bmap[port] = 
                                        sal_alloc(alloc_size, "LLS_L0_BMAP");
                }
                bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
            } else if (lvl == SOC_TR3_NODE_LVL_L1) {
                if (SOC_CONTROL(unit)->port_lls_l1_bmap[port] == NULL) {
                    SOC_CONTROL(unit)->port_lls_l1_bmap[port] = 
                                        sal_alloc(alloc_size, "LLS_L1_BMAP");
                }
                bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
            } else if (lvl == SOC_TR3_NODE_LVL_L2) {
                if (SOC_CONTROL(unit)->port_lls_l2_bmap[port] == NULL) {
                    SOC_CONTROL(unit)->port_lls_l2_bmap[port] = 
                                        sal_alloc(alloc_size, "LLS_L2_BMAP");
                }
                bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
            }
            sal_memset(bmap, 0, alloc_size);
        }
    }

    for (lvl = SOC_TR3_NODE_LVL_L0; lvl <= SOC_TR3_NODE_LVL_L2; lvl++) {
        mem = _SOC_TR3_NODE_PARENT_MEM(lvl);
    
        sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
        soc_mem_field32_set(unit, mem, entry, C_PARENTf, INVALID_PARENT(unit, lvl));
    
        for (i = 0; i <= soc_mem_index_max(unit, mem); i++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &entry));
        }
    }

    return 0;
}

int soc_tr3_port_lls_traverse(int unit, soc_port_t port, 
                              _soc_tr3_lls_traverse_cb cb, void *cookie)
{
    int rv = SOC_E_INTERNAL;
    int phy_port, mmu_port;
    soc_info_t *si;
    int dyn_mode;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    dyn_mode = _bcm_tr3_port_sdyn[unit][port];
    
    if (_SOC_TR3_IS_HSP_PORT(unit, port)) {
        rv = SOC_E_UNAVAIL;
    } else if (IS_CPU_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_cmic_reserved_queues)) {
            rv = soc_tr3_cpu_port_lls_init(unit, port, _SOC_TR3_LLS_OP_TRAVERSE,
                                           1, cb, cookie);
        } else {
            rv = soc_tr3_cpu_port_lls_init(unit, port, _SOC_TR3_LLS_OP_TRAVERSE,
                                           0, cb, cookie);
        }
    }else if (mmu_port == 61) {
            rv = soc_tr3_port_lls_init(unit, port, _tr3_port_sm_lls_config,
                                        _SOC_TR3_LLS_OP_TRAVERSE, cb, cookie);
    } else {
        if (soc_port_hg_capable(unit, port)) {
            rv = soc_tr3_port_lls_init(unit, port, 
                (dyn_mode) ? _tr3_hg_port_dyn_lls_config : _tr3_hg_port_lls_config, 
                                        _SOC_TR3_LLS_OP_TRAVERSE, cb, cookie);
        }else if (mmu_port == 58) {
            rv = soc_tr3_port_lls_init(unit, port, _tr3_lb_port_lls_config,
                                        _SOC_TR3_LLS_OP_TRAVERSE, cb, cookie);
        } else {
            rv = soc_tr3_port_lls_init(unit, port, 
                (dyn_mode) ? _tr3_port_dyn_lls_config : _tr3_port_lls_config, 
                                        _SOC_TR3_LLS_OP_TRAVERSE, cb, cookie);
        }
    }

    return rv;
}
int
soc_tr3_hsp_sched_hw_index_get(int unit, int port, int lvl, int index,
                           int offset, int *hw_index)
{
     int ceid, uc_queue_base, mc_queue_base;
     soc_field_t f;
     uint32 rval;
 
     static const soc_field_t hl2basef[] = {
        PORT0_UCQ_OFFSETf, PORT1_UCQ_OFFSETf
     };

     if (!_SOC_TR3_IS_HSP_PORT(unit, port)) {
         return SOC_E_PARAM;
     }
     if ((lvl != SOC_TR3_NODE_LVL_L0) && (lvl != SOC_TR3_NODE_LVL_L2)) {
          return SOC_E_PARAM;
     }
     
     if (lvl == SOC_TR3_NODE_LVL_L0) {
         ceid = _soc_tr3_ce_port_index(unit, port);
         if (ceid == -1) {
             return SOC_E_PARAM;
         }
         if (offset >= _SOC_HSP_VALID_L0_END) {
             *hw_index = -1;
             return SOC_E_PARAM;
         }
         *hw_index = _SOC_HSP_L0_BASE_HW_INDEX + 
                     ( _SOC_HSP_L0_PER_PORT_OFFSET * ceid ) + offset;
         return SOC_E_NONE;
    }
    ceid = _soc_tr3_ce_port_index(unit, port);
    if (ceid == -1) {
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HSP_CONFIGr, port, 0, &rval));
    f = hl2basef[ceid];
    uc_queue_base = soc_reg_field_get(unit, HSP_CONFIGr, rval, f);
    uc_queue_base = uc_queue_base * 4;
    soc_tr3_get_def_qbase(unit, port, _SOC_TR3_INDEX_STYLE_MCAST_QUEUE,
            &mc_queue_base, NULL);
    if ((index < _SOC_HSP_VALID_L0_START) || 
        (index > _SOC_HSP_VALID_L0_END)) {
        return SOC_E_PARAM;
    }
    if ((offset < _SOC_HSP_MIN_QUEUE_OFFSET) ||
        (offset > _SOC_HSP_MAX_QUEUE_OFFSET)) {
        return SOC_E_PARAM;
    } 
    if (index == _SOC_HSP_VALID_L0_END) {
       if ((offset == 0) || (offset == 1)) {
           /* UC Queue */
           *hw_index = uc_queue_base + _SOC_HSP_LAST_L0_UC_START + offset;
       } else {
           /* MC Queue */
           *hw_index = mc_queue_base + _SOC_HSP_LAST_L0_UC_START + 
                       (offset - _SOC_HSP_LAST_L0_MAX_UC_QUEUE );
       }
    } else {
        if (offset > 1) {
            return SOC_E_PARAM;
        } 
        if (offset == 0) {
            /* UC Queue */
            *hw_index = uc_queue_base + index - _SOC_HSP_VALID_L0_START;
        } else {
            /* MC Queue */
            *hw_index = mc_queue_base + index - _SOC_HSP_VALID_L0_START;
        }
   }
   return SOC_E_NONE; 
}          
int
soc_tr3_sched_hw_index_get(int unit, int port, int lvl, int offset, 
                            int *hw_index)
{
    int local_port, base = 0, ceid, num_at_lvl = 0;
    int phy_port, mmu_port;
    soc_info_t *si;
    int sp_vec = soc_feature(unit, soc_feature_tr3_sp_vector_mask);
    int top_offset = (_tr3_invalid_ptr[unit][lvl] == -1) ? 0 : 1;
    int dyn_mode = 0;

    si = &SOC_INFO(unit);

    if (_SOC_TR3_IS_HSP_PORT(unit, port)) {
        if (lvl != SOC_TR3_NODE_LVL_L0) {
            return SOC_E_PARAM;
        }
        ceid = _soc_tr3_ce_port_index(unit, port);
        if (ceid == -1) {
            return SOC_E_PARAM;
        }
        if (offset >= _SOC_HSP_VALID_L0_END) {
            *hw_index = -1;
            return SOC_E_PARAM;
        }
        *hw_index = _SOC_HSP_L0_BASE_HW_INDEX + 
                    ( _SOC_HSP_L0_PER_PORT_OFFSET * ceid ) + offset;
        return SOC_E_NONE;
    }

    PBMP_ALL_ITER(unit, local_port) {
        dyn_mode = _bcm_tr3_port_sdyn[unit][local_port];
        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (IS_CPU_PORT(unit, local_port)) {
            num_at_lvl = tr3_cpu_num_sched_at_lvl[lvl];
        } else if (_SOC_TR3_IS_HSP_PORT(unit, local_port)) {
            continue;
        } else if (mmu_port == 61) {
           num_at_lvl = tr3_sm_port_num_sched_at_lvl[lvl];
        } else if (mmu_port == 58) {
           num_at_lvl = tr3_lb_port_num_sched_at_lvl[lvl];
        } else {
            if (soc_port_hg_capable(unit, local_port)) {
                num_at_lvl = tr3_hg_port_num_sched_at_lvl[lvl];
            } else {
                num_at_lvl = tr3_port_num_sched_at_lvl[lvl];
            }
        }

        if ((lvl == SOC_TR3_NODE_LVL_L1) && (dyn_mode)) {
            num_at_lvl += 8;
        }

        if (local_port == port) {
            if (sp_vec && IS_CPU_PORT(unit, local_port) && 
                (lvl == SOC_TR3_NODE_LVL_L0) && (!soc_feature(unit, soc_feature_cmic_reserved_queues))) {
                offset += 3;
            }
            
            if (offset > num_at_lvl) {
                return SOC_E_PARAM;
            }
            
            if ((offset >= 8) && ((!dyn_mode))) {
                *hw_index = _tr2_node_max[unit][lvl] - (top_offset + 1);
                if ((base + 8) > *hw_index) {
                    return SOC_E_RESOURCE;
                }
            } else {
                *hw_index = base + offset;
            }
            return SOC_E_NONE;
        } else {
            if ((!dyn_mode) && (num_at_lvl > 8)) {
                top_offset += num_at_lvl - 8;
                num_at_lvl = 8;
            }
            base += num_at_lvl;
        }
    }

    return SOC_E_PARAM;
}

#define MMU_CMIC_PORT   59
#define MMU_SM_PORT     61

int soc_tr3_get_mcq_count(int unit)
{
    return 561;
}

/*The architectural limit on unicast queues is 1024*/
#define _BCM_TR3_NUM_L2_UC_LEAVES 1024

/*
 * Allow upto 32 unicast queues to be allocated per ports. Some customers 
 * have special queueing requirements that require many additional unicast
 * queues to be reserved at initialization time.
 */
#define _MAX_UNICAST_QUEUES_PER_PORT 32
int 
soc_tr3_get_def_qbase(int unit, soc_port_t inport, int qtype, 
                       int *pbase, int *pnumq)
{
    int base = 0, numq = 0, dflt_cnt;
    int port, phy_port, mmu_port = 0;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    if (qtype == _SOC_TR3_INDEX_STYLE_UCAST_QUEUE) {
        PBMP_ALL_ITER(unit, port) {
            phy_port = si->port_l2p_mapping[port];
            if (phy_port == -1) {
                continue;
            }
            mmu_port = si->port_p2m_mapping[phy_port];
            if (_SOC_TR3_IS_HSP_PORT(unit, port)) {
                numq = 16;
            } else if (mmu_port == MMU_CMIC_PORT) {
                numq = 0;
            } else if (mmu_port == MMU_SM_PORT) {
                numq = 8;
            } else {
                dflt_cnt = soc_port_hg_capable(unit, port) ? 10 : 8;
                if ((!IS_LB_PORT(unit, port)) &&
                    (_bcm_tr3_port_numq[unit][port] == 0)) {
                    numq = _bcm_tr3_port_numq[unit][port] =  
                        soc_property_port_get(unit, port, spn_LLS_NUM_L2UC, dflt_cnt);
                } else {
                    numq = _bcm_tr3_port_numq[unit][port];
                }

                if ((numq < dflt_cnt) || (numq > _MAX_UNICAST_QUEUES_PER_PORT)) {
                    numq = dflt_cnt;
                }
            }
            if (inport == port) {
                break;
            }
            numq = (numq + 3) & ~3;
            base += numq;
            if (base >= _BCM_TR3_NUM_L2_UC_LEAVES) {
                LOG_ERROR(BSL_LS_SOC_COSQ,
                  (BSL_META_U(unit,
                              "unit %d : Cannot configure requested number of unicast queues per port;\n"
                              "Available queue resources (%d queues) exhausted.\n"),
                              unit, _BCM_TR3_NUM_L2_UC_LEAVES));
                return SOC_E_CONFIG;
            }
        }
    } else if (qtype == _SOC_TR3_INDEX_STYLE_MCAST_QUEUE) {
        mmu_port = si->port_p2m_mapping[si->port_l2p_mapping[inport]];
        if (_SOC_TR3_IS_HSP_PORT(unit, inport)) {
            numq = 10;
        } else if (mmu_port == MMU_CMIC_PORT) {
            numq = soc_feature(unit, soc_feature_cmic_reserved_queues) ? 45 : 48;
        } else if (mmu_port == MMU_SM_PORT) {
            numq = 1;
        } else if ((mmu_port == 57) || (mmu_port == 62)) {
            numq = 0;
        } else {
            numq = ((mmu_port >= 40) && (mmu_port <= 55)) ? 10 : 8;
        }
        if (mmu_port <= 39) {
            base = mmu_port * 8;
        } else if (mmu_port <= 55) {
            base = 320 + (mmu_port - 40) * 10;
        } else if (mmu_port == 56) {
            base = 480;
        } else if (mmu_port <= 57) {
            base = -1;
        } else if (mmu_port == 58) {
            base = 488;
        } else if (mmu_port <= 59) {
            base = 512;
        } else if (mmu_port == 60) {
            base = 496;
        } else if (mmu_port == 61) {
            base = 504;
        } else if (mmu_port == 62) {
            base = 0;
        }
    }

    /* Pushing the queues used for LB ports to bottom */
    if (qtype == _SOC_TR3_INDEX_STYLE_UCAST_QUEUE) {
        if (mmu_port == 56) {
            base = 1008;
        } else if (mmu_port == 62) {
            base = 1016;
        } else if (mmu_port == 61) {
            base = 1000;
        }
    }

    if (pbase) {
        *pbase = base;
    }
    if (pnumq) {
        *pnumq = numq;
    }
    return SOC_E_NONE;
}

int soc_port_hg_capable(int unit, int port)
{
    int phy_port, mmu_port;
    soc_info_t *si;
    
    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if ((mmu_port >= 40) && (mmu_port <= 55)) {
        return 1;
    }
    return 0;
}

int soc_tr3_l2_hw_index(int unit, int qnum, int uc)
{
    return ((uc==0) ? 1024 : 0) + qnum;
}

STATIC int 
_soc_tr3_dump_sched_at(int unit, int port, int level, int offset, int hw_index)
{
    int num_spri, first_child, first_mc_child, rv, cindex;
    uint32 ucmap, spmap;
    soc_tr3_sched_mode_e sched_mode;
    soc_mem_t mem;
    int index_max, ii, ci, child_level, wt = 0, num_child;
    uint32 entry[SOC_MAX_MEM_WORDS];
    char *lvl_name[] = { "Root", "L0", "L1", "L2" };
    char *sched_modes[] = {"X", "SP", "WRR", "WDRR"};

    if ((level >= SOC_TR3_NODE_LVL_L0) &&
        (level < SOC_TR3_NODE_LVL_L2) && (hw_index == INVALID_PARENT(unit, level + 1))) {
        return SOC_E_NONE;
    }

    /* get sched config */
    SOC_IF_ERROR_RETURN(
            soc_tr3_sched_get_node_config(unit, port, level, hw_index, 
                       &num_spri, &first_child, &first_mc_child, &ucmap, &spmap));

    sched_mode = 0;
    if (level != SOC_TR3_NODE_LVL_ROOT) {
        SOC_IF_ERROR_RETURN(
          soc_tr3_cosq_get_sched_mode(unit, port, level, hw_index, &sched_mode, &wt));
    }
    
    if (level == SOC_TR3_NODE_LVL_L1) {
        LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SP=%d FC=%d FMC=%d "
                            "UCMAP=0x%08x SPMAP=0x%08x MODE=%s WT=%d\n"),
                 lvl_name[level], offset, hw_index, num_spri, 
                 first_child, first_mc_child, ucmap, spmap,
                 sched_modes[sched_mode], wt));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d SPMAP=0x%08x "
                            "MODE=%s WT=%d\n"),
                 lvl_name[level], offset, hw_index, num_spri, first_child,
                 spmap, sched_modes[sched_mode], wt));
    }

    soc_tr3_get_child_type(unit, port, level, &child_level);
    mem = _SOC_TR3_NODE_PARENT_MEM(child_level);
    
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
        
        cindex = soc_mem_field32_get(unit, mem, entry, C_PARENTf);
        if (cindex == hw_index) {
            if (child_level == SOC_TR3_NODE_LVL_L2) {
                SOC_IF_ERROR_RETURN(soc_tr3_cosq_get_sched_mode(unit, port,
                                    SOC_TR3_NODE_LVL_L2, ii, &sched_mode, &wt));
                LOG_CLI((BSL_META_U(unit,
                                    "     L2.%s INDEX=%d Mode=%s WEIGHT=%d\n"), 
                         ((ii < 1024) ? "uc" : "mc"),
                         ii, sched_modes[sched_mode], wt));
            } else {
                _soc_tr3_dump_sched_at(unit, port, child_level, ci, ii);
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

int soc_tr3_dump_port_lls(int unit, int port)
{
    if (_SOC_TR3_IS_HSP_PORT(unit, port)) {
        return SOC_E_NONE;
    }

    LOG_CLI((BSL_META_U(unit,
                        "-------%s (LLS)------\n"), SOC_PORT_NAME(unit, (port))));

    _soc_tr3_dump_sched_at(unit, port, SOC_TR3_NODE_LVL_ROOT, 0, 
                            _SOC_TR3_ROOT_SCHEDULER_INDEX(unit, port));
    return SOC_E_NONE;
}

int soc_tr3_dump_port_hsp(int unit, int port)
{
    soc_field_t f;
    int ceid;
    uint32 rval;
    int hw_index, mc_index, l2_queue, mcq_queue;
    int i, mcq_mode, q_spmap, wt, spmap, wrrmap , qnum, portwrr; 
    static const soc_field_t hl0basef[] = {
        PORT0_L0_OFFSETf, PORT1_L0_OFFSETf
    };
    static const soc_field_t hl2basef[] = {
        PORT0_UCQ_OFFSETf, PORT1_UCQ_OFFSETf 
    };

    if (!_SOC_TR3_IS_HSP_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    ceid = _soc_tr3_ce_port_index(unit, port);
    if (ceid == -1) {
        return SOC_E_PARAM;
    }

    LOG_CLI((BSL_META_U(unit,
                    "-------%s (HSP)------\n"), SOC_PORT_NAME(unit, (port))));

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HSP_CONFIGr, port, 0, &rval));
    f = hl0basef[ceid];
    hw_index = soc_reg_field_get(unit, HSP_CONFIGr, rval, f);
    f = hl2basef[ceid];
    qnum = soc_reg_field_get(unit, HSP_CONFIGr, rval, f);
    qnum *= 4;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_PORT_CONFIGr, port, 0, &rval));
    spmap = soc_reg_field_get(unit, HES_PORT_CONFIGr, rval, MINSPCONFIGf);
    portwrr = soc_reg_field_get(unit, HES_PORT_CONFIGr, rval, SCHEDULING_SELECTf);
    soc_tr3_get_def_qbase(unit, port, _SOC_TR3_INDEX_STYLE_MCAST_QUEUE,
            &mc_index, NULL);

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_L0_CONFIGr, port, 0, &rval));
    mcq_mode = soc_reg_field_get(unit, HES_L0_CONFIGr, rval, MCQ_MODEf);
    wrrmap = soc_reg_field_get(unit, HES_L0_CONFIGr, rval, SCHEDULING_SELECTf);
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_MINSPr, port, 0, &rval));
    q_spmap = soc_reg_field_get(unit, HES_Q_MINSPr, rval, MINSPCONFIGf);

    LOG_CLI((BSL_META_U(unit,
                    "   Root.0 : INDEX=%d PSPMAP=0x%03x QSPMAP=0x%05x FC=%d "
                    "MODE=%s WT=0\n"),
                _SOC_TR3_ROOT_SCHEDULER_INDEX(unit, port), spmap, q_spmap,
                hw_index,
                "X"));


    l2_queue = 0;
    mcq_queue = 0;
    for (i = 0; i < 10; i++ ) {

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, L0_COSWEIGHTSr, port, i, &rval));
        wt = soc_reg_field_get(unit, L0_COSWEIGHTSr, rval, COSWEIGHTSf);

        LOG_CLI((BSL_META_U(unit,
                        "   L0.%d : INDEX=%d FC=%d FMC=%d "
                        "MODE=%s WT=%d\n"),
                    i, hw_index + i ,  
                    (i == 0) ? 0 : qnum + l2_queue,
                    ((i == 0) && (mcq_mode == 1)) ?  0 : 
                    ((i > 0) && (i < 9) && (mcq_mode == 0)) ? 0 : mc_index + mcq_queue ,
                    (wt == 0) ? "SP" : (wrrmap & (1<<i)) ? "WDRR":"WRR",
                    wt));
        if (( i != 0) && (i != 9)) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_COSWEIGHTSr, port, l2_queue, &rval));
            wt = soc_reg_field_get(unit, HES_Q_COSWEIGHTSr, rval, COSWEIGHTSf);
            LOG_CLI((BSL_META_U(unit,
                            "       L1.uc : INDEX=%d  "
                            "MODE=%s WT=%d\n"),
                        qnum + l2_queue ,
                        (wt == 0) ? "SP" : (portwrr ? "DWRR" :"WRR"),
                        wt));
            l2_queue++;

        }
        if ((mcq_mode == 0)) {
            if ( i == 0 ) { 
                for (mcq_queue = 0; mcq_queue < 8 ; mcq_queue++) { 
                    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_COSWEIGHTSr, port, mcq_queue + 10, &rval));
                    wt = soc_reg_field_get(unit, HES_Q_COSWEIGHTSr, rval, COSWEIGHTSf);
                    LOG_CLI((BSL_META_U(unit,
                                    "       L1.mc : INDEX=%d "
                                    "MODE=%s WT=%d\n"),
                                mc_index + mcq_queue, 
                                (wt == 0) ? "SP" :(portwrr ? "DWRR" :"WRR"),
                                wt));
                }
            }
        } else {
            if (( i != 0) && (i != 9)) {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_COSWEIGHTSr, port, mcq_queue + 10, &rval));
                wt = soc_reg_field_get(unit, HES_Q_COSWEIGHTSr, rval, COSWEIGHTSf);
                LOG_CLI((BSL_META_U(unit,
                                "       L1.mc : INDEX=%d "
                                "MODE=%s WT=%d\n"),
                            mc_index + mcq_queue, 
                            (wt == 0) ? "SP" :(portwrr ? "DWRR" :"WRR"),
                            wt));
                mcq_queue++;
            }
        }
        if (i == 9) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_COSWEIGHTSr, port, l2_queue, &rval));
            wt = soc_reg_field_get(unit, HES_Q_COSWEIGHTSr, rval, COSWEIGHTSf);
            LOG_CLI((BSL_META_U(unit,
                            "       L1.uc : INDEX=%d  "
                            "MODE=%s WT=%d\n"),
                        qnum + l2_queue ,
                        (wt == 0) ? "SP" :(portwrr ? "DWRR" :"WRR"),
                        wt));
            l2_queue++;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_COSWEIGHTSr, port, l2_queue, &rval));
            wt = soc_reg_field_get(unit, HES_Q_COSWEIGHTSr, rval, COSWEIGHTSf);
            LOG_CLI((BSL_META_U(unit,
                            "       L1.uc : INDEX=%d  "
                            "MODE=%s WT=%d\n"),
                        qnum + l2_queue ,
                        (wt == 0) ? "SP" :(portwrr ? "DWRR" :"WRR"),
                        wt));
            l2_queue++;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_COSWEIGHTSr, port, mcq_queue + 10, &rval));
            wt = soc_reg_field_get(unit, HES_Q_COSWEIGHTSr, rval, COSWEIGHTSf);
            LOG_CLI((BSL_META_U(unit,
                            "       L1.mc : INDEX=%d "
                            "MODE=%s WT=%d\n"),
                        mc_index + mcq_queue, 
                        (wt == 0) ? "SP" :(portwrr ? "DWRR" :"WRR"),
                        wt));
            mcq_queue++;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, HES_Q_COSWEIGHTSr, port, mcq_queue + 10, &rval));
            wt = soc_reg_field_get(unit, HES_Q_COSWEIGHTSr, rval, COSWEIGHTSf);
            LOG_CLI((BSL_META_U(unit,
                            "       L1.mc : INDEX=%d "
                            "MODE=%s WT=%d\n"),
                        mc_index + mcq_queue, 
                        (wt == 0) ? "SP" :(portwrr ? "DWRR" :"WRR"),
                        wt));
            mcq_queue++;
        }

    }  
    return SOC_E_NONE;
}

#endif /* defined(BCM_TRIUMPH3_SUPPORT) */


