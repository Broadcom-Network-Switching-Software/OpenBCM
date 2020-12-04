/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON COSQ
 */
 
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_COSQ 
#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#include <soc/error.h>
#include <soc/defs.h>


#include <soc/dnxc/error.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/ramon/ramon_cosq.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>

#include <sal/compiler.h>



typedef struct
{
    
    int nof_instances;
    int soc_block;

    
    soc_reg_t pipes_weights_register;
    soc_field_t wfq_weight_p_0;
    soc_field_t wfq_weight_p_1;
    soc_field_t wfq_weight_p_2;

    
    soc_reg_t dyn_pipes_weights_register;
    soc_field_t wfq_weight_p_0_p_0_gt_p_1;
    soc_field_t wfq_weight_p_1_p_0_gt_p_1;
    soc_field_t wfq_weight_p_0_p_1_gt_p_0;
    soc_field_t wfq_weight_p_1_p_1_gt_p_0;
} soc_ramon_cosq_pipe_weight_regs_info_t;

STATIC int
_soc_ramon_cosq_pipe_weight_validate(int unit, int pipe, soc_dnxf_cosq_weight_mode_t mode, int weight)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((mode == soc_dnxf_cosq_weight_mode_dynamic_0) || (mode == soc_dnxf_cosq_weight_mode_dynamic_1)) 
    {
        if (pipe > 1) 
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid pipe id, dynamic weights can be set only for pipe 0 or 1\n");
        }
    }

    if ((weight > SOC_RAMON_COSQ_WFQ_WEIGHT_MAX) || (weight <= 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid weight\n");
    }

exit:
    SHR_FUNC_EXIT;
}


STATIC void
soc_ramon_cosq_pipe_weight_regs_info_fill(int unit, soc_dnxf_cosq_pipe_type_t pipe_type, soc_ramon_cosq_pipe_weight_regs_info_t *info)
{
    if (soc_dnxf_cosq_pipe_type_ingress == pipe_type) 
    {
        info->nof_instances = dnxf_data_device.blocks.nof_instances_dch_get(unit);
        info->soc_block = SOC_BLK_DCH;

        info->pipes_weights_register = DCH_PIPES_WEIGHTS_REGISTERr;
        info->wfq_weight_p_0 = WFQ_WEIGHT_P_0f;
        info->wfq_weight_p_1 = WFQ_WEIGHT_P_1f;
        info->wfq_weight_p_2 = WFQ_WEIGHT_P_2f;

        info->dyn_pipes_weights_register = DCH_DYN_PIPES_WEIGHTS_REGISTERr;
        info->wfq_weight_p_0_p_0_gt_p_1 = WFQ_WEIGHT_P_0_P_0_GT_P_1f;
        info->wfq_weight_p_1_p_0_gt_p_1 = WFQ_WEIGHT_P_1_P_0_GT_P_1f;
        info->wfq_weight_p_0_p_1_gt_p_0 = WFQ_WEIGHT_P_0_P_1_GT_P_0f;
        info->wfq_weight_p_1_p_1_gt_p_0 = WFQ_WEIGHT_P_1_P_1_GT_P_0f;
    }
    else if (soc_dnxf_cosq_pipe_type_rtp == pipe_type) 
    {
        info->nof_instances = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
        info->soc_block = SOC_BLK_QRH;

        info->pipes_weights_register = QRH_DRH_WFQ_CONFIGURATIONSr;
        info->wfq_weight_p_0 = WFQ_WEIGHT_0f;
        info->wfq_weight_p_1 = WFQ_WEIGHT_1f;
        info->wfq_weight_p_2 = WFQ_WEIGHT_2f;

        info->dyn_pipes_weights_register = QRH_DYN_PIPES_WEIGHTS_REGISTERr;
        info->wfq_weight_p_0_p_0_gt_p_1 = WFQ_WEIGHT_P_0_P_0_GT_P_1f;
        info->wfq_weight_p_1_p_0_gt_p_1 = WFQ_WEIGHT_P_1_P_0_GT_P_1f;
        info->wfq_weight_p_0_p_1_gt_p_0 = WFQ_WEIGHT_P_0_P_1_GT_P_0f;
        info->wfq_weight_p_1_p_1_gt_p_0 = WFQ_WEIGHT_P_1_P_1_GT_P_0f;
    }
    else if (soc_dnxf_cosq_pipe_type_middle == pipe_type) 
    {
        info->nof_instances = dnxf_data_device.blocks.nof_instances_lcm_get(unit);
        info->soc_block = SOC_BLK_LCM;

        info->pipes_weights_register = LCM_DTM_PIPES_WEIGHTS_REGISTERr;
        info->wfq_weight_p_0 = DTM_WFQ_WEIGHT_P_0f;
        info->wfq_weight_p_1 = DTM_WFQ_WEIGHT_P_1f;
        info->wfq_weight_p_2 = DTM_WFQ_WEIGHT_P_2f;

        info->dyn_pipes_weights_register = LCM_DTM_DYN_PIPES_WEIGHTS_REGISTERr;
        info->wfq_weight_p_0_p_0_gt_p_1 = DTM_WFQ_WEIGHT_P_0_P_0_GT_P_1f;
        info->wfq_weight_p_1_p_0_gt_p_1 = DTM_WFQ_WEIGHT_P_1_P_0_GT_P_1f;
        info->wfq_weight_p_0_p_1_gt_p_0 = DTM_WFQ_WEIGHT_P_0_P_1_GT_P_0f;
        info->wfq_weight_p_1_p_1_gt_p_0 = DTM_WFQ_WEIGHT_P_1_P_1_GT_P_0f;
    }
    else if (soc_dnxf_cosq_pipe_type_egress == pipe_type) 
    {
        info->nof_instances = dnxf_data_device.blocks.nof_instances_lcm_get(unit);
        info->soc_block = SOC_BLK_LCM;

        info->pipes_weights_register = LCM_DTL_PIPES_WEIGHTS_REGISTERr;
        info->wfq_weight_p_0 = WFQ_WEIGHT_P_0f;
        info->wfq_weight_p_1 = WFQ_WEIGHT_P_1f;
        info->wfq_weight_p_2 = WFQ_WEIGHT_P_2f;

        info->dyn_pipes_weights_register = LCM_DYN_PIPES_WEIGHTS_REGISTERr;
        info->wfq_weight_p_0_p_0_gt_p_1 = WFQ_WEIGHT_P_0_P_0_GT_P_1f;
        info->wfq_weight_p_1_p_0_gt_p_1 = WFQ_WEIGHT_P_1_P_0_GT_P_1f;
        info->wfq_weight_p_0_p_1_gt_p_0 = WFQ_WEIGHT_P_0_P_1_GT_P_0f;
        info->wfq_weight_p_1_p_1_gt_p_0 = WFQ_WEIGHT_P_1_P_1_GT_P_0f;
    }
}


shr_error_e
soc_ramon_cosq_pipe_weight_set(int unit, soc_dnxf_cosq_pipe_type_t pipe_type, int pipe, int port, soc_dnxf_cosq_weight_mode_t mode, int weight)
{
    shr_error_e rc;
	int i = 0;
    uint32 reg_val32 = 0;
    soc_reg_above_64_val_t reg_val_above_64;
    int instance = 0, inner_link = 0;
    int loop_start = 0, loop_end = 0;
    soc_ramon_cosq_pipe_weight_regs_info_t info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&info, 0, sizeof(info));

    
    rc = _soc_ramon_cosq_pipe_weight_validate(unit, pipe, mode, weight);
    SHR_IF_ERR_EXIT(rc);

    
    soc_ramon_cosq_pipe_weight_regs_info_fill(unit, pipe_type, &info);

    if (port == -1) {
        loop_start = 0;
        loop_end = info.nof_instances;
    } else {
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &instance, &inner_link, info.soc_block));
        SHR_IF_ERR_EXIT(rc);
        loop_start = instance;
        loop_end = instance + 1;
    }

    
    if (mode == soc_dnxf_cosq_weight_mode_regular) 
    {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);

        for (i = loop_start ; i < loop_end ; i++)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, info.pipes_weights_register, i, 0, reg_val_above_64));

            if (pipe == 0)
            {
                soc_reg_above_64_field32_set(unit, info.pipes_weights_register, reg_val_above_64, info.wfq_weight_p_0, weight);                    
            }
            else if (pipe == 1) 
            {
                soc_reg_above_64_field32_set(unit, info.pipes_weights_register, reg_val_above_64, info.wfq_weight_p_1, weight);
            }
            else if (pipe == 2) 
            {
                soc_reg_above_64_field32_set(unit, info.pipes_weights_register, reg_val_above_64, info.wfq_weight_p_2, weight);
            }

            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, info.pipes_weights_register, i, 0, reg_val_above_64));
        }
    }
    else if (mode == soc_dnxf_cosq_weight_mode_dynamic_0) 
    {
        for (i=loop_start ; i < loop_end ; i++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, info.dyn_pipes_weights_register, i, 0, &reg_val32));

            if (pipe == 0)
            {
                soc_reg_field_set(unit, info.dyn_pipes_weights_register, &reg_val32, info.wfq_weight_p_0_p_0_gt_p_1, weight);
            }
        
            else if (pipe == 1) 
            {
                soc_reg_field_set(unit, info.dyn_pipes_weights_register, &reg_val32, info.wfq_weight_p_1_p_0_gt_p_1, weight);
            }

            SHR_IF_ERR_EXIT(soc_reg32_set(unit, info.dyn_pipes_weights_register, i, 0, reg_val32));
        }
    }
    else if (mode == soc_dnxf_cosq_weight_mode_dynamic_1) 
    {
        for (i=loop_start ; i < loop_end ; i++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, info.dyn_pipes_weights_register, i, 0, &reg_val32));

            if (pipe == 0)
            {
                soc_reg_field_set(unit, info.dyn_pipes_weights_register, &reg_val32, info.wfq_weight_p_0_p_1_gt_p_0, weight);
            }
        
            else if (pipe == 1) 
            {
                soc_reg_field_set(unit, info.dyn_pipes_weights_register, &reg_val32, info.wfq_weight_p_1_p_1_gt_p_0, weight);
            }

            SHR_IF_ERR_EXIT(soc_reg32_set(unit, info.dyn_pipes_weights_register, i, 0, reg_val32));
        }
    }
    
exit:
    SHR_FUNC_EXIT; 
}


shr_error_e
soc_ramon_cosq_pipe_weight_get(int unit, soc_dnxf_cosq_pipe_type_t pipe_type, int pipe, int port, soc_dnxf_cosq_weight_mode_t mode, int *weight)
{
    shr_error_e rc;
    uint32 reg_val32;
    int instance, inner_link;
    soc_ramon_cosq_pipe_weight_regs_info_t info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&info, 0, sizeof(info));

    
    rc = _soc_ramon_cosq_pipe_weight_validate(unit, pipe, mode, SOC_RAMON_COSQ_WFQ_WEIGHT_IGNORE);
    SHR_IF_ERR_EXIT(rc);

    
    soc_ramon_cosq_pipe_weight_regs_info_fill(unit, pipe_type, &info);

    if (port == -1) {
        instance = 0;
    } else {
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &instance, &inner_link, info.soc_block));
        SHR_IF_ERR_EXIT(rc);
    }

    
    if (mode == soc_dnxf_cosq_weight_mode_regular) 
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, info.pipes_weights_register, instance, 0, &reg_val32));

        if (pipe == 0)
        {
            *weight = soc_reg_field_get(unit, info.pipes_weights_register, reg_val32, info.wfq_weight_p_0);
        }
        else if (pipe == 1) 
        {
            *weight = soc_reg_field_get(unit, info.pipes_weights_register, reg_val32, info.wfq_weight_p_1);
        }
        else if (pipe == 2) 
        {
            *weight = soc_reg_field_get(unit, info.pipes_weights_register, reg_val32, info.wfq_weight_p_2);
        }
    }
    else if (mode == soc_dnxf_cosq_weight_mode_dynamic_0) 
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, info.dyn_pipes_weights_register, instance, 0, &reg_val32));

        if (pipe == 0)
        {
            *weight = soc_reg_field_get(unit, info.dyn_pipes_weights_register, reg_val32, info.wfq_weight_p_0_p_0_gt_p_1);
        }
        else if (pipe == 1) 
        {
            *weight = soc_reg_field_get(unit, info.dyn_pipes_weights_register, reg_val32, info.wfq_weight_p_1_p_0_gt_p_1);
        }
    }
    else if (mode == soc_dnxf_cosq_weight_mode_dynamic_1) 
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, info.dyn_pipes_weights_register, instance, 0, &reg_val32));

        if (pipe == 0)
        {
            *weight = soc_reg_field_get(unit, info.dyn_pipes_weights_register, reg_val32, info.wfq_weight_p_0_p_1_gt_p_0);
        }
        else if (pipe == 1) 
        {
            *weight = soc_reg_field_get(unit, info.dyn_pipes_weights_register, reg_val32, info.wfq_weight_p_1_p_1_gt_p_0);
        }
    }

exit:
    SHR_FUNC_EXIT; 
}

#undef BSL_LOG_MODULE
