/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 COSQ
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ 
#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#include <soc/error.h>
#include <soc/defs.h>


#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_config_defs.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/fe3200/fe3200_cosq.h>

#include <sal/compiler.h>


STATIC int
_soc_fe3200_cosq_pipe_weight_validate(int unit, int pipe, soc_dfe_cosq_weight_mode_t mode, int weight)
{
    SOCDNX_INIT_FUNC_DEFS;

    if ((mode == soc_dfe_cosq_weight_mode_dynamic_0) || (mode == soc_dfe_cosq_weight_mode_dynamic_1)) 
    {
        if (pipe > 1) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid pipe id, dynamic weights can be set only for pipe 0 or 1\n")));
        }
    }

    if ((weight > SOC_FE3200_COSQ_WFQ_WEIGHT_MAX) || (weight <= 0))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid weight\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_fe3200_cosq_pipe_threshold_validate(int unit, int pipe, int threshold, int max_threshold)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (pipe > 1) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid pipe id, dynamic thresholds can be set only for pipe 0 or 1\n")));
    }

    if ((threshold > max_threshold) || (threshold < 0))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid threshold\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_cosq_pipe_rx_weight_set(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int weight)
{
    soc_error_t rc;
	int  i, nof_instances_dch;
    uint32 reg_val32;
    int dch_instance, dch_inner_link;
    int dch_loop_start, dch_loop_end;

    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_weight_validate(unit, pipe, mode, weight);
    SOCDNX_IF_ERR_EXIT(rc);

    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);

    if (port == -1) {
        dch_loop_start = 0;
        dch_loop_end = nof_instances_dch;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
        SOCDNX_IF_ERR_EXIT(rc);
        dch_loop_start = dch_instance;
        dch_loop_end = dch_instance + 1;
    }

    
    if (mode == soc_dfe_cosq_weight_mode_regular) 
    {
        for (i=dch_loop_start ; i < dch_loop_end ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_PIPES_WEIGHTS_REGISTERr(unit, i, &reg_val32));

            if (pipe == 0)
            {
                soc_reg_field_set(unit, DCH_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_0f, weight);                    
            }
            else if (pipe == 1) 
            {
                soc_reg_field_set(unit, DCH_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_1f, weight);
            }
            else if (pipe == 2) 
            {
                soc_reg_field_set(unit, DCH_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_2f, weight);
            }

            SOCDNX_IF_ERR_EXIT(WRITE_DCH_PIPES_WEIGHTS_REGISTERr(unit, i, reg_val32));
        }
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_0) 
    {
        for (i=dch_loop_start ; i < dch_loop_end ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DYN_PIPES_WEIGHTS_REGISTERr(unit, i, &reg_val32));

            if (pipe == 0)
            {
                soc_reg_field_set(unit, DCH_DYN_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_0_P_0_GT_P_1f, weight);
            }
        
            else if (pipe == 1) 
            {
                soc_reg_field_set(unit, DCH_DYN_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_1_P_0_GT_P_1f, weight);
            }

            SOCDNX_IF_ERR_EXIT(WRITE_DCH_DYN_PIPES_WEIGHTS_REGISTERr(unit, i, reg_val32));
        }
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_1) 
    {
        for (i=dch_loop_start ; i < dch_loop_end ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DYN_PIPES_WEIGHTS_REGISTERr(unit, i, &reg_val32));

            if (pipe == 0)
            {
                soc_reg_field_set(unit, DCH_DYN_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_0_P_1_GT_P_0f, weight);
            }
        
            else if (pipe == 1) 
            {
                soc_reg_field_set(unit, DCH_DYN_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_1_P_1_GT_P_0f, weight);
            }

            SOCDNX_IF_ERR_EXIT(WRITE_DCH_DYN_PIPES_WEIGHTS_REGISTERr(unit, i, reg_val32));
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_fe3200_cosq_pipe_rx_weight_get(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int *weight)
{
    soc_error_t rc;
    uint32 reg_val32;
    int dch_instance, dch_inner_link;

    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_weight_validate(unit, pipe, mode, SOC_FE3200_COSQ_WFQ_WEIGHT_IGNORE);
    SOCDNX_IF_ERR_EXIT(rc);

    if (port == -1) {
        dch_instance = 0;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
        SOCDNX_IF_ERR_EXIT(rc);
    }


    
    if (mode == soc_dfe_cosq_weight_mode_regular) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_PIPES_WEIGHTS_REGISTERr(unit, dch_instance, &reg_val32));

        if (pipe == 0)
        {
            *weight = soc_reg_field_get(unit, DCH_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_0f);
        }
        else if (pipe == 1) 
        {
            *weight = soc_reg_field_get(unit, DCH_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_1f);
        }
        else if (pipe == 2) 
        {
            *weight = soc_reg_field_get(unit, DCH_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_2f);
        }
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_0) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_DYN_PIPES_WEIGHTS_REGISTERr(unit, dch_instance, &reg_val32));

        if (pipe == 0)
        {
            *weight = soc_reg_field_get(unit, DCH_DYN_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_0_P_0_GT_P_1f);
        }
        else if (pipe == 1) 
        {
            *weight = soc_reg_field_get(unit, DCH_DYN_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_1_P_0_GT_P_1f);
        }
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_1) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_DYN_PIPES_WEIGHTS_REGISTERr(unit, dch_instance, &reg_val32));

        if (pipe == 0)
        {
            *weight = soc_reg_field_get(unit, DCH_DYN_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_0_P_1_GT_P_0f);
        }
        else if (pipe == 1) 
        {
            *weight = soc_reg_field_get(unit, DCH_DYN_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_1_P_1_GT_P_0f);
        }
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t  
soc_fe3200_cosq_pipe_mid_weight_set(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int weight)
{
    soc_error_t rc;
	int  i, nof_instances_dcm;
    uint32 reg_val32;
    int dcm_instance, dcm_inner_link;
    int dcm_loop_start, dcm_loop_end;

    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_weight_validate(unit, pipe, mode, weight);
    SOCDNX_IF_ERR_EXIT(rc);

    nof_instances_dcm = SOC_DFE_DEFS_GET(unit, nof_instances_dcm);

    if (port == -1) {
        dcm_loop_start = 0;
        dcm_loop_end = nof_instances_dcm;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcm_instance, &dcm_inner_link, SOC_BLK_DCM));
        SOCDNX_IF_ERR_EXIT(rc);
        dcm_loop_start = dcm_instance;
        dcm_loop_end = dcm_instance + 1;
    }

    
    if (mode == soc_dfe_cosq_weight_mode_regular) 
    {
        for (i=dcm_loop_start ; i < dcm_loop_end ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_PIPES_WEIGHTS_REGISTERr(unit, i, &reg_val32));

            if (pipe == 0)
            {
                soc_reg_field_set(unit, DCM_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_0f, weight);                    
            }
            else if (pipe == 1) 
            {
                soc_reg_field_set(unit, DCM_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_1f, weight);
            }
            else if (pipe == 2) 
            {
                soc_reg_field_set(unit, DCM_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_2f, weight);
            }

            SOCDNX_IF_ERR_EXIT(WRITE_DCM_PIPES_WEIGHTS_REGISTERr(unit, i, reg_val32));
        }
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_0) 
    {
        for (i=dcm_loop_start ; i < dcm_loop_end ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_DYN_PIPES_WEIGHTS_REGISTERr(unit, i, &reg_val32));

            if (pipe == 0)
            {
                soc_reg_field_set(unit, DCM_DYN_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_0_P_0_GT_P_1f, weight);
            }
        
            else if (pipe == 1) 
            {
                soc_reg_field_set(unit, DCM_DYN_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_1_P_0_GT_P_1f, weight);
            }

            SOCDNX_IF_ERR_EXIT(WRITE_DCM_DYN_PIPES_WEIGHTS_REGISTERr(unit, i, reg_val32));
        }
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_1) 
    {
        for (i=dcm_loop_start ; i < dcm_loop_end ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_DYN_PIPES_WEIGHTS_REGISTERr(unit, i, &reg_val32));

            if (pipe == 0)
            {
                soc_reg_field_set(unit, DCM_DYN_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_0_P_1_GT_P_0f, weight);
            }
        
            else if (pipe == 1) 
            {
                soc_reg_field_set(unit, DCM_DYN_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_WEIGHT_P_1_P_1_GT_P_0f, weight);
            }

            SOCDNX_IF_ERR_EXIT(WRITE_DCM_DYN_PIPES_WEIGHTS_REGISTERr(unit, i, reg_val32));
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t  
soc_fe3200_cosq_pipe_mid_weight_get(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int *weight)
{
    soc_error_t rc;
    uint32 reg_val32;
    int dcm_instance, dcm_inner_link;

    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_weight_validate(unit, pipe, mode, SOC_FE3200_COSQ_WFQ_WEIGHT_IGNORE);
    SOCDNX_IF_ERR_EXIT(rc);


    if (port == -1) {
        dcm_instance = 0;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcm_instance, &dcm_inner_link, SOC_BLK_DCM));
        SOCDNX_IF_ERR_EXIT(rc);
    }

    
    if (mode == soc_dfe_cosq_weight_mode_regular) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCM_PIPES_WEIGHTS_REGISTERr(unit, dcm_instance, &reg_val32));

        if (pipe == 0)
        {
            *weight = soc_reg_field_get(unit, DCM_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_0f);
        }
        else if (pipe == 1) 
        {
            *weight = soc_reg_field_get(unit, DCM_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_1f);
        }
        else if (pipe == 2) 
        {
            *weight = soc_reg_field_get(unit, DCM_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_2f);
        }
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_0) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCM_DYN_PIPES_WEIGHTS_REGISTERr(unit, dcm_instance, &reg_val32));

        if (pipe == 0)
        {
            *weight = soc_reg_field_get(unit, DCM_DYN_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_0_P_0_GT_P_1f);
        }
        else if (pipe == 1) 
        {
            *weight = soc_reg_field_get(unit, DCM_DYN_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_1_P_0_GT_P_1f);
        }
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_1) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCM_DYN_PIPES_WEIGHTS_REGISTERr(unit, dcm_instance, &reg_val32));

        if (pipe == 0)
        {
            *weight = soc_reg_field_get(unit, DCM_DYN_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_0_P_1_GT_P_0f);
        }
        else if (pipe == 1) 
        {
            *weight = soc_reg_field_get(unit, DCM_DYN_PIPES_WEIGHTS_REGISTERr, reg_val32, WFQ_WEIGHT_P_1_P_1_GT_P_0f);
        }
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t  
soc_fe3200_cosq_pipe_tx_weight_set(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int weight)
{
    soc_error_t rc;
    int dcl_inner_link, dcl_instance;
    uint32 reg_val32;
    uint32 field_val32;
    uint32 temp_weight;


    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_weight_validate(unit, pipe, mode, weight);
    SOCDNX_IF_ERR_EXIT(rc);
    DFE_LINK_INPUT_CHECK(unit, port);

    
    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
    SOCDNX_IF_ERR_EXIT(rc);

    temp_weight = (uint32)weight;

    
    if (mode == soc_dfe_cosq_weight_mode_regular) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_LINKS_WEIGHT_CONFIGr(unit, dcl_instance, dcl_inner_link ,&reg_val32));

        
        field_val32 = soc_reg_field_get(unit, DCL_LINKS_WEIGHT_CONFIGr, reg_val32, LINK_W__Nf);
        SHR_BITCOPY_RANGE(&field_val32, (pipe*SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS), &temp_weight, 0, SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS);
        soc_reg_field_set(unit, DCL_LINKS_WEIGHT_CONFIGr, &reg_val32, LINK_W__Nf, field_val32);

        SOCDNX_IF_ERR_EXIT(WRITE_DCL_LINKS_WEIGHT_CONFIGr(unit, dcl_instance, dcl_inner_link, reg_val32));
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_0) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_LINKS_WEIGHT_CONFIG_P_0_GT_P_1r(unit, dcl_instance, dcl_inner_link ,&reg_val32));

        
        field_val32 = soc_reg_field_get(unit, DCL_LINKS_WEIGHT_CONFIG_P_0_GT_P_1r, reg_val32, LINK_W_P_0_GT_P_1__Nf);
        SHR_BITCOPY_RANGE(&field_val32, (pipe*SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS), &temp_weight, 0, SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS);
        soc_reg_field_set(unit, DCL_LINKS_WEIGHT_CONFIG_P_0_GT_P_1r, &reg_val32, LINK_W_P_0_GT_P_1__Nf, field_val32);

        SOCDNX_IF_ERR_EXIT(WRITE_DCL_LINKS_WEIGHT_CONFIG_P_0_GT_P_1r(unit, dcl_instance, dcl_inner_link, reg_val32));
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_1) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_LINKS_WEIGHT_CONFIG_P_1_GT_P_0r(unit, dcl_instance, dcl_inner_link ,&reg_val32));

        
        field_val32 = soc_reg_field_get(unit, DCL_LINKS_WEIGHT_CONFIG_P_1_GT_P_0r, reg_val32, LINK_W_P_1_GT_P_0__Nf);
        SHR_BITCOPY_RANGE(&field_val32, (pipe*SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS), &temp_weight, 0, SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS);
        soc_reg_field_set(unit, DCL_LINKS_WEIGHT_CONFIG_P_1_GT_P_0r, &reg_val32, LINK_W_P_1_GT_P_0__Nf, field_val32);

        SOCDNX_IF_ERR_EXIT(WRITE_DCL_LINKS_WEIGHT_CONFIG_P_1_GT_P_0r(unit, dcl_instance, dcl_inner_link, reg_val32));
    }
    
exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t  
soc_fe3200_cosq_pipe_tx_weight_get(int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int *weight)
{
    soc_error_t rc;
	int dcl_inner_link, dcl_instance;
    uint32 reg_val32, temp_weight;
    uint32 field_val32;

    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_weight_validate(unit, pipe, mode, SOC_FE3200_COSQ_WFQ_WEIGHT_IGNORE);
    SOCDNX_IF_ERR_EXIT(rc);
    DFE_LINK_INPUT_CHECK(unit, port);

    
    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
    SOCDNX_IF_ERR_EXIT(rc);

    temp_weight = 0;

    
    if (mode == soc_dfe_cosq_weight_mode_regular) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_LINKS_WEIGHT_CONFIGr(unit, dcl_instance, dcl_inner_link ,&reg_val32));

        
        field_val32 = soc_reg_field_get(unit, DCL_LINKS_WEIGHT_CONFIGr, reg_val32, LINK_W__Nf);
        SHR_BITCOPY_RANGE(&temp_weight, 0, &field_val32, (pipe*SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS), SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS);
    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_0) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_LINKS_WEIGHT_CONFIG_P_0_GT_P_1r(unit, dcl_instance, dcl_inner_link ,&reg_val32));

        
        field_val32 = soc_reg_field_get(unit, DCL_LINKS_WEIGHT_CONFIG_P_0_GT_P_1r, reg_val32, LINK_W_P_0_GT_P_1__Nf);
        SHR_BITCOPY_RANGE(&temp_weight, 0, &field_val32, (pipe*SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS), SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS);

    }
    else if (mode == soc_dfe_cosq_weight_mode_dynamic_1) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_LINKS_WEIGHT_CONFIG_P_1_GT_P_0r(unit, dcl_instance, dcl_inner_link ,&reg_val32));

        
        field_val32 = soc_reg_field_get(unit, DCL_LINKS_WEIGHT_CONFIG_P_1_GT_P_0r, reg_val32, LINK_W_P_1_GT_P_0__Nf);
        SHR_BITCOPY_RANGE(&temp_weight, 0, &field_val32, (pipe*SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS), SOC_FE3200_COSQ_WFQ_WEIGHT_NOF_BITS);
    }

    *weight = (int)temp_weight;
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t  
soc_fe3200_cosq_pipe_rx_threshold_set(int unit, int pipe, int port, int threshold)
{
    soc_error_t rc;
    int         nof_instances_dch, i;
    uint32      reg_val32;
    int dch_instance, dch_inner_link;
    int dch_loop_start, dch_loop_end;

    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_threshold_validate(unit, pipe, threshold, SOC_FE3200_COSQ_WFQ_DCH_THRESH_MAX);
    SOCDNX_IF_ERR_EXIT(rc);

    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);

    if (port == -1) {
        dch_loop_start = 0;
        dch_loop_end = nof_instances_dch;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
        SOCDNX_IF_ERR_EXIT(rc);
        dch_loop_start = dch_instance;
        dch_loop_end = dch_instance + 1;
    }

    
    for (i=dch_loop_start; i < dch_loop_end; i++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_DYN_WFQ_WEIGHT_THD_REGISTEr(unit, i, &reg_val32));

        if (pipe == 0)
        {
            soc_reg_field_set(unit, DCH_DYN_WFQ_WEIGHT_THD_REGISTEr, &reg_val32, DYN_WFQ_WEIGHT_THD_P_0f, threshold);
        }

        else if (pipe == 1) 
        {
            soc_reg_field_set(unit, DCH_DYN_WFQ_WEIGHT_THD_REGISTEr, &reg_val32, DYN_WFQ_WEIGHT_THD_P_1f, threshold);
        }

        SOCDNX_IF_ERR_EXIT(WRITE_DCH_DYN_WFQ_WEIGHT_THD_REGISTEr(unit, i, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
soc_error_t  
soc_fe3200_cosq_pipe_rx_threshold_get(int unit, int pipe, int port, int *threshold)
{
    soc_error_t rc;
    uint32 reg_val32;
    int dch_instance, dch_inner_link;

    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_threshold_validate(unit, pipe, 0, SOC_FE3200_COSQ_WFQ_DCH_THRESH_MAX);
    SOCDNX_IF_ERR_EXIT(rc);

    if (port == -1) {
        dch_instance = 0;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
        SOCDNX_IF_ERR_EXIT(rc);
    }


    SOCDNX_IF_ERR_EXIT(READ_DCH_DYN_WFQ_WEIGHT_THD_REGISTEr(unit, dch_instance, &reg_val32));

    if (pipe == 0)
    {
        *threshold = soc_reg_field_get(unit, DCH_DYN_WFQ_WEIGHT_THD_REGISTEr, reg_val32, DYN_WFQ_WEIGHT_THD_P_0f);
    }
    else if (pipe == 1) 
    {
        *threshold = soc_reg_field_get(unit, DCH_DYN_WFQ_WEIGHT_THD_REGISTEr, reg_val32, DYN_WFQ_WEIGHT_THD_P_1f);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t  
soc_fe3200_cosq_pipe_mid_threshold_set(int unit, int pipe, int port, int threshold)
{
    soc_error_t rc;
    int         nof_instances_dcm, i;
    uint32      reg_val32;
    int dcm_instance, dcm_inner_link;
    int dcm_loop_start, dcm_loop_end;

    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_threshold_validate(unit, pipe, threshold, SOC_FE3200_COSQ_WFQ_DCM_THRESH_MAX);
    SOCDNX_IF_ERR_EXIT(rc);

    nof_instances_dcm = SOC_DFE_DEFS_GET(unit, nof_instances_dcm);

    if (port == -1) {
        dcm_loop_start = 0;
        dcm_loop_end = nof_instances_dcm;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcm_instance, &dcm_inner_link, SOC_BLK_DCM));
        SOCDNX_IF_ERR_EXIT(rc);
        dcm_loop_start = dcm_instance;
        dcm_loop_end = dcm_instance + 1;
    }

    
    for (i=dcm_loop_start; i < dcm_loop_end; i++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCM_DYN_WFQ_WEIGHT_THD_REGISTERr(unit, i, &reg_val32));

        if (pipe == 0)
        {
            soc_reg_field_set(unit, DCM_DYN_WFQ_WEIGHT_THD_REGISTERr, &reg_val32, DYN_WFQ_WEIGHT_THD_P_0f, threshold);
        }

        else if (pipe == 1) 
        {
            soc_reg_field_set(unit, DCM_DYN_WFQ_WEIGHT_THD_REGISTERr, &reg_val32, DYN_WFQ_WEIGHT_THD_P_1f, threshold);
        }

        SOCDNX_IF_ERR_EXIT(WRITE_DCM_DYN_WFQ_WEIGHT_THD_REGISTERr(unit, i, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t  
soc_fe3200_cosq_pipe_mid_threshold_get(int unit, int pipe, int port, int *threshold)
{
    soc_error_t rc;
    uint32 reg_val32;
    int dcm_instance, dcm_inner_link;

    SOCDNX_INIT_FUNC_DEFS;

    
    rc = _soc_fe3200_cosq_pipe_threshold_validate(unit, pipe, 0, SOC_FE3200_COSQ_WFQ_DCM_THRESH_MAX);
    SOCDNX_IF_ERR_EXIT(rc);

    if (port == -1) {
        dcm_instance = 0;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcm_instance, &dcm_inner_link, SOC_BLK_DCM));
        SOCDNX_IF_ERR_EXIT(rc);
    }


    SOCDNX_IF_ERR_EXIT(READ_DCM_DYN_WFQ_WEIGHT_THD_REGISTERr(unit, dcm_instance, &reg_val32));

    if (pipe == 0)
    {
        *threshold = soc_reg_field_get(unit, DCM_DYN_WFQ_WEIGHT_THD_REGISTERr, reg_val32, DYN_WFQ_WEIGHT_THD_P_0f);
    }
    else if (pipe == 1) 
    {
        *threshold = soc_reg_field_get(unit, DCM_DYN_WFQ_WEIGHT_THD_REGISTERr, reg_val32, DYN_WFQ_WEIGHT_THD_P_1f);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t  
soc_fe3200_cosq_pipe_tx_threshold_set(int unit, int pipe, int port, int threshold)
{
    soc_error_t rc;
    int         nof_instances_dcl, i;
    uint32      reg_val32;
    int dcl_instance, dcl_inner_link;  
    int   dcl_loop_start, dcl_loop_end;

    SOCDNX_INIT_FUNC_DEFS;

    rc = _soc_fe3200_cosq_pipe_threshold_validate(unit, pipe, threshold, SOC_FE3200_COSQ_WFQ_DCL_THRESH_MAX);
    SOCDNX_IF_ERR_EXIT(rc);

    nof_instances_dcl = SOC_DFE_DEFS_GET(unit, nof_instances_dcl);

    if (port == -1) {
        dcl_loop_start = 0;
        dcl_loop_end = nof_instances_dcl;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
        SOCDNX_IF_ERR_EXIT(rc);
        dcl_loop_start = dcl_instance;
        dcl_loop_end = dcl_instance + 1;
    }

    
    for (i=dcl_loop_start; i < dcl_loop_end; i++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_LINK_WEIGHT_CONFIG_THRESHOLDSr(unit, i, &reg_val32));

        if (pipe == 0)
        {
            soc_reg_field_set(unit, DCL_LINK_WEIGHT_CONFIG_THRESHOLDSr, &reg_val32, LINK_W_P_0_THDf, threshold);
        }

        else if (pipe == 1) 
        {
            soc_reg_field_set(unit, DCL_LINK_WEIGHT_CONFIG_THRESHOLDSr, &reg_val32, LINK_W_P_1_THDf, threshold);
        }

        SOCDNX_IF_ERR_EXIT(WRITE_DCL_LINK_WEIGHT_CONFIG_THRESHOLDSr(unit, i, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t  
soc_fe3200_cosq_pipe_tx_threshold_get(int unit, int pipe, int port, int *threshold)
{
    soc_error_t rc;
    uint32 reg_val32;
    int dcl_instance, dcl_inner_link;

    SOCDNX_INIT_FUNC_DEFS;



    
    rc = _soc_fe3200_cosq_pipe_threshold_validate(unit, pipe, 0, SOC_FE3200_COSQ_WFQ_DCL_THRESH_MAX);
    SOCDNX_IF_ERR_EXIT(rc);

    if (port == -1) {
        dcl_instance = 0;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
        SOCDNX_IF_ERR_EXIT(rc);
    }


    SOCDNX_IF_ERR_EXIT(READ_DCL_LINK_WEIGHT_CONFIG_THRESHOLDSr(unit, dcl_instance, &reg_val32));

    if (pipe == 0)
    {
        *threshold = soc_reg_field_get(unit, DCL_LINK_WEIGHT_CONFIG_THRESHOLDSr, reg_val32, LINK_W_P_0_THDf);
    }
    else if (pipe == 1) 
    {
        *threshold = soc_reg_field_get(unit, DCL_LINK_WEIGHT_CONFIG_THRESHOLDSr, reg_val32, LINK_W_P_1_THDf);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t  
soc_fe3200_cosq_pipe_rx_rate_set(int unit, int pipe, int port, soc_dfe_cosq_shaper_mode_t shaper_mode, uint32 rate)
{
    uint32 period, reg_val32, max_cells_per_sec;
    int nof_instances_dch, i;
    int dch_instance, dch_inner_link;
    int dch_loop_start, dch_loop_end;
    int rc;



    SOCDNX_INIT_FUNC_DEFS;

    
    if (shaper_mode != soc_dfe_cosq_shaper_mode_packet) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid shaping mode, only packet based shaper is allowed for dch\n")));
    }



     

    max_cells_per_sec = SOC_DFE_CONFIG(unit).core_clock_speed * 1000;                     
    
    if (rate == 0) 
    {
        period = 0;
    }
    else
    {
        period = max_cells_per_sec/(rate + 1);
    }

    if (period > SOC_FE3200_COSQ_RATE_DCH_PERIOD_MAX) 
    {
        period = SOC_FE3200_COSQ_RATE_DCH_PERIOD_MAX;
    }


    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);

    if (port == -1) {
        dch_loop_start = 0;
        dch_loop_end = nof_instances_dch;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
        SOCDNX_IF_ERR_EXIT(rc);
        dch_loop_start = dch_instance;
        dch_loop_end = dch_instance + 1;
    }

    
    for (i=dch_loop_start; i < dch_loop_end; i++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LP_PIPE_SHAPER_REGISTERr(unit, i, &reg_val32));

        soc_reg_field_set(unit, DCH_LP_PIPE_SHAPER_REGISTERr, &reg_val32, LP_PIPE_SHAPER_IDXf, pipe);
        soc_reg_field_set(unit, DCH_LP_PIPE_SHAPER_REGISTERr, &reg_val32, LP_PIPE_SHAPER_PERIODf, period);
   
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_LP_PIPE_SHAPER_REGISTERr(unit, i, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t  
soc_fe3200_cosq_pipe_rx_rate_get(int unit, int pipe, int port, soc_dfe_cosq_shaper_mode_t *shaper_mode, uint32 *rate)
{
    uint32 period, reg_val32, max_cells_per_sec, current_pipe;
    int dch_instance, dch_inner_link;
    int rc;
    SOCDNX_INIT_FUNC_DEFS;



    
    
    if (port == -1) {
        dch_instance = 0;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
        SOCDNX_IF_ERR_EXIT(rc);
    } 

    
    max_cells_per_sec = SOC_DFE_CONFIG(unit).core_clock_speed * 1000;                     

    SOCDNX_IF_ERR_EXIT(READ_DCH_LP_PIPE_SHAPER_REGISTERr(unit, dch_instance, &reg_val32));

    current_pipe = soc_reg_field_get(unit, DCH_LP_PIPE_SHAPER_REGISTERr, reg_val32, LP_PIPE_SHAPER_IDXf);
    period = soc_reg_field_get(unit, DCH_LP_PIPE_SHAPER_REGISTERr, reg_val32, LP_PIPE_SHAPER_PERIODf);

    

    if ((pipe == current_pipe) && (period != 0))
    {
        *rate = max_cells_per_sec/period - 1;
    }
    else
    {
        *rate = 0;
    }

    *shaper_mode = soc_dfe_cosq_shaper_mode_packet;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t  
soc_fe3200_cosq_pipe_tx_rate_set(int unit, int port, soc_dfe_cosq_shaper_mode_t shaper_mode, uint32 rate)
{
    uint32 inc_size, shaper_rate, reg_val32, core_clock_speed;
    uint64 val64, shaper_rate_val64;
    int nof_instances_dcl = 0, i = 0;
    int dcl_inner_link, dcl_instance;
    int dcl_loop_start, dcl_loop_end;
    int rc;


    SOCDNX_INIT_FUNC_DEFS;

     




    core_clock_speed = SOC_DFE_CONFIG(unit).core_clock_speed;                   
    
    if (shaper_mode == soc_dfe_cosq_shaper_mode_byte) 
    { 
        inc_size = 0;
        if (rate != 0)
        {
            COMPILER_64_SET(shaper_rate_val64, 0, rate);
            COMPILER_64_SET(val64, 0, core_clock_speed);
            COMPILER_64_UMUL_32(shaper_rate_val64, 16);
            COMPILER_64_UDIV_64(shaper_rate_val64, val64);
        }
        else
        {
            COMPILER_64_SET(shaper_rate_val64, 0, 0);
        }

        COMPILER_64_SET(val64, 0, SOC_FE3200_COSQ_RATE_DCL_DEC_INC_VAL_MAX);
        if (COMPILER_64_GT(shaper_rate_val64, val64))
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid rate\n")));
        }
        else
        {
            COMPILER_64_TO_32_LO(shaper_rate, shaper_rate_val64);
        }
    }
    else if (shaper_mode == soc_dfe_cosq_shaper_mode_packet) 
    {
        rate = rate/1000; 
        inc_size = SOC_FE3200_COSQ_RATE_DCL_DEC_INC_VAL_MAX + 1;

        if (rate != 0) 
        {
            COMPILER_64_SET(shaper_rate_val64, 0, rate);
            COMPILER_64_SET(val64, 0, core_clock_speed);
            COMPILER_64_UMUL_32(shaper_rate_val64, (16*inc_size));
            COMPILER_64_UDIV_64(shaper_rate_val64, val64);
        }
        else
        {
            COMPILER_64_SET(shaper_rate_val64, 0, 0);            
        }
        COMPILER_64_SET(val64, 0, SOC_FE3200_COSQ_RATE_DCL_DEC_INC_VAL_MAX);
        while ( COMPILER_64_GT(shaper_rate_val64, val64) && (i < SOC_FE3200_COSQ_RATE_DCL_DEC_INC_NOF_BITS) ) 
        {
            COMPILER_64_SHR(shaper_rate_val64, 1);
            inc_size >>= 1;
            i++;
        }

        if ( COMPILER_64_GT(shaper_rate_val64, val64) ) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid rate\n")));
        }

        COMPILER_64_TO_32_LO(shaper_rate, shaper_rate_val64);
        inc_size -= 1;
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid shaping mode\n")));
    }

    nof_instances_dcl = SOC_DFE_DEFS_GET(unit, nof_instances_dcl);

    if (port == -1) {
        dcl_loop_start = 0;
        dcl_loop_end = nof_instances_dcl;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
        SOCDNX_IF_ERR_EXIT(rc);
        dcl_loop_start = dcl_instance;
        dcl_loop_end = dcl_instance + 1;
    }

    
    for (i=dcl_loop_start; i < dcl_loop_end; i++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_SHAPER_AND_WFQ_CFGr(unit, i, &reg_val32));

        soc_reg_field_set(unit, DCL_SHAPER_AND_WFQ_CFGr, &reg_val32, SHAPER_RATEf, shaper_rate);
        soc_reg_field_set(unit, DCL_SHAPER_AND_WFQ_CFGr, &reg_val32, SHAPER_INC_SIZEf, inc_size);
   
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_SHAPER_AND_WFQ_CFGr(unit, i, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_cosq_pipe_tx_rate_get(int unit, int port, soc_dfe_cosq_shaper_mode_t *shaper_mode, uint32 *rate)
{
    uint32 inc_size, shaper_rate, reg_val32, core_clock_speed;
    int dcl_instance, dcl_inner_link;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    if (port == -1) {
        dcl_instance = 0;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
        SOCDNX_IF_ERR_EXIT(rc);
    }

    core_clock_speed = SOC_DFE_CONFIG(unit).core_clock_speed; 

    SOCDNX_IF_ERR_EXIT(READ_DCL_SHAPER_AND_WFQ_CFGr(unit, dcl_instance, &reg_val32));

    shaper_rate = soc_reg_field_get(unit, DCL_SHAPER_AND_WFQ_CFGr, reg_val32, SHAPER_RATEf);
    inc_size = soc_reg_field_get(unit, DCL_SHAPER_AND_WFQ_CFGr, reg_val32, SHAPER_INC_SIZEf);

    if (inc_size == 0)
    {
        *shaper_mode = soc_dfe_cosq_shaper_mode_byte;
    }
    else
    {
        *shaper_mode = soc_dfe_cosq_shaper_mode_packet;
    }


    *rate = (core_clock_speed * shaper_rate )/(16*( 1 + inc_size)); 

    if (*shaper_mode == soc_dfe_cosq_shaper_mode_packet)
    {
        *rate *= 1000; 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t  
soc_fe3200_cosq_pipe_tx_rate_enable_set(int unit, int pipe, int port, int enable)
{
    uint32 reg_val32, pipes_bmp;
    int rc;
    int nof_instances_dcl, i;
    int dcl_instance, dcl_inner_link; 
    int  dcl_loop_start, dcl_loop_end;  

    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_dcl = SOC_DFE_DEFS_GET(unit, nof_instances_dcl);

    if (port == -1) {
        dcl_loop_start = 0;
        dcl_loop_end = nof_instances_dcl;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
        SOCDNX_IF_ERR_EXIT(rc);
        dcl_loop_start = dcl_instance;
        dcl_loop_end = dcl_instance + 1;
    }   

    
    for (i=dcl_loop_start; i < dcl_loop_end; i++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_SHAPER_AND_WFQ_CFGr(unit, i, &reg_val32));

        pipes_bmp = soc_reg_field_get(unit, DCL_SHAPER_AND_WFQ_CFGr, reg_val32, SHAPER_INC_SOURCEf);

        enable ? SHR_BITSET_RANGE (&pipes_bmp, pipe, 1) : SHR_BITCLR_RANGE(&pipes_bmp, pipe, 1);

        soc_reg_field_set(unit, DCL_SHAPER_AND_WFQ_CFGr, &reg_val32, SHAPER_INC_SOURCEf, pipes_bmp);
   
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_SHAPER_AND_WFQ_CFGr(unit, i, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t  
soc_fe3200_cosq_pipe_tx_rate_enable_get(int unit, int pipe, int port, int *enable)
{
    uint32 reg_val32, pipes_bmp;
    int rc;
    int dcl_instance, dcl_inner_link;

    SOCDNX_INIT_FUNC_DEFS;

    if (port == -1) {
        dcl_instance = 0;
    } else {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
        SOCDNX_IF_ERR_EXIT(rc);
    }

    SOCDNX_IF_ERR_EXIT(READ_DCL_SHAPER_AND_WFQ_CFGr(unit, dcl_instance, &reg_val32));

    pipes_bmp = soc_reg_field_get(unit, DCL_SHAPER_AND_WFQ_CFGr, reg_val32, SHAPER_INC_SOURCEf);

    SHR_BITTEST_RANGE(&pipes_bmp, pipe, 1, *enable);

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
