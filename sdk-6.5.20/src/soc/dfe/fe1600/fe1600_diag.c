/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 DEFS H
 */
#include <shared/bsl.h>
#include <soc/dfe/fe1600/fe1600_diag.h>
#include <soc/dcmn/error.h>
#include <soc/drv.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_config_defs.h>
#include <soc/dfe/fe1600/fe1600_defs.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG



soc_error_t 
soc_fe1600_counters_get_info(int unit,soc_dfe_counters_info_t* fe_counters_info){
    
    
    
    int i,j;
   

    uint32   reg_val,reg_val2,reg_val3,reg_val4,reg_val5,reg_val6,reg_val7;
    uint64   reg_val64;
    
    uint32   curr_max=0;
    uint32 temp1=0,temp2=0;
    uint32 dcm_total=0,dcm_drp=0;
    uint32 mask_9_bits=0x1FF;
    
    uint32 nof_pipes;
   
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);
     

    if(SOC_DFE_CONFIG(unit).is_dual_mode)
    {
        nof_pipes=2;
        fe_counters_info->nof_pipes=2;
    }
    else{
        nof_pipes=1;
        fe_counters_info->nof_pipes=1;
    }
   
   
    
        
   
        curr_max=0;

        for (i=0; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++) 
        {
             SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_Pr(unit,i,&reg_val));
             SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_OUT_CELL_CNT_Pr(unit,i,&reg_val2));
             SOCDNX_IF_ERR_EXIT(READ_DCH_FIFO_DISCARD_COUNTER_Pr(unit,i,&reg_val3));
             SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_REORD_DISCARD_COUNTER_Pr(unit,i,&reg_val4));
             SOCDNX_IF_ERR_EXIT(READ_DCH_UN_REACH_DEST_0_Pr(unit,i,&reg_val64));
             SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr(unit,i,&reg_val6));

             fe_counters_info->dch_counters_info[0].dch_total_in.value+=soc_reg_field_get(unit,DCH_DCH_TOTAL_CELL_CNT_Pr,reg_val,DCH_TOTAL_CELL_CNT_Pf);
             fe_counters_info->dch_counters_info[0].dch_total_in.overflow+=soc_reg_field_get(unit,DCH_DCH_TOTAL_CELL_CNT_Pr,reg_val,DCH_TOTAL_CELL_CNT_OPf);
             
             fe_counters_info->dch_counters_info[0].dch_total_out.value+=soc_reg_field_get(unit,DCH_DCH_TOTAL_OUT_CELL_CNT_Pr,reg_val2,DCH_TOTAL_OUT_CELL_CNT_Pf);
             fe_counters_info->dch_counters_info[0].dch_total_out.overflow+=soc_reg_field_get(unit,DCH_DCH_TOTAL_OUT_CELL_CNT_Pr,reg_val2,DCH_TOTAL_OUT_CELL_CNT_OPf);

             fe_counters_info->dch_counters_info[0].dch_fifo_discard.value+=soc_reg_field_get(unit,DCH_FIFO_DISCARD_COUNTER_Pr,reg_val3,DCH_DESC_CNT_Pf);
             fe_counters_info->dch_counters_info[0].dch_fifo_discard.overflow+=soc_reg_field_get(unit,DCH_FIFO_DISCARD_COUNTER_Pr,reg_val3,DCH_DESC_CNT_OPf);

             fe_counters_info->dch_counters_info[0].reorder_discard.value+=soc_reg_field_get(unit,DCH_DCH_REORD_DISCARD_COUNTER_Pr,reg_val4,DCH_REORD_DESC_CNT_Pf);
             fe_counters_info->dch_counters_info[0].reorder_discard.overflow+=soc_reg_field_get(unit,DCH_DCH_REORD_DISCARD_COUNTER_Pr,reg_val4,DCH_REORD_DESC_CNT_OPf);

             fe_counters_info->dch_counters_info[0].sum_dch_unreach_discard.value+=soc_reg64_field32_get(unit,DCH_UN_REACH_DEST_0_Pr,reg_val64,UNREACH_DEST_CNT_Pf);
             fe_counters_info->dch_counters_info[0].sum_dch_unreach_discard.overflow+=soc_reg64_field32_get(unit,DCH_UN_REACH_DEST_0_Pr,reg_val64,UNREACH_DEST_CNT_OPf); 
             
             temp1=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr,reg_val6,MAX_IFMAFPf);
             temp2=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr,reg_val6,MAX_IFMBFPf);
             if (temp1>curr_max)
             {
                 curr_max=temp1;
                 
             }
             if (temp2>curr_max)
             {
                 curr_max=temp2;
             }
        }
        fe_counters_info->dch_counters_info[0].dch_max_q_size=curr_max;

        if (nof_pipes==2)    
        {
            curr_max=0;

                for (i=0;i< SOC_DFE_DEFS_GET(unit, nof_instances_dch) ;i++) 
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_Sr(unit,i,&reg_val));
                    SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_OUT_CELL_CNT_Sr(unit,i,&reg_val2));
                    SOCDNX_IF_ERR_EXIT(READ_DCH_FIFO_DISCARD_COUNTER_Sr(unit,i,&reg_val3));
                    SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_REORD_DISCARD_COUNTER_Sr(unit,i,&reg_val4));
                    SOCDNX_IF_ERR_EXIT(READ_DCH_UN_REACH_DEST_0_Sr(unit,i,&reg_val64));
                    SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr(unit,i,&reg_val6));

                    fe_counters_info->dch_counters_info[1].dch_total_in.value+=soc_reg_field_get(unit,DCH_DCH_TOTAL_CELL_CNT_Sr,reg_val,DCH_TOTAL_CELL_CNT_Sf);
                    fe_counters_info->dch_counters_info[1].dch_total_in.overflow+=soc_reg_field_get(unit,DCH_DCH_TOTAL_CELL_CNT_Sr,reg_val,DCH_TOTAL_CELL_CNT_OSf);

                    fe_counters_info->dch_counters_info[1].dch_total_out.value+=soc_reg_field_get(unit,DCH_DCH_TOTAL_OUT_CELL_CNT_Sr,reg_val2,DCH_TOTAL_OUT_CELL_CNT_Sf);
                    fe_counters_info->dch_counters_info[1].dch_total_out.overflow+=soc_reg_field_get(unit,DCH_DCH_TOTAL_OUT_CELL_CNT_Sr,reg_val2,DCH_TOTAL_OUT_CELL_CNT_OSf);

                    fe_counters_info->dch_counters_info[1].dch_fifo_discard.value+=soc_reg_field_get(unit,DCH_FIFO_DISCARD_COUNTER_Sr,reg_val3,DCH_DESC_CNT_Sf);
                    fe_counters_info->dch_counters_info[1].dch_fifo_discard.overflow+=soc_reg_field_get(unit,DCH_FIFO_DISCARD_COUNTER_Sr,reg_val3,DCH_DESC_CNT_OSf);

                    fe_counters_info->dch_counters_info[1].reorder_discard.value+=soc_reg_field_get(unit,DCH_DCH_REORD_DISCARD_COUNTER_Sr,reg_val4,DCH_REORD_DESC_CNT_Sf);
                    fe_counters_info->dch_counters_info[1].reorder_discard.overflow+=soc_reg_field_get(unit,DCH_DCH_REORD_DISCARD_COUNTER_Sr,reg_val4,DCH_REORD_DESC_CNT_OSf);

                    fe_counters_info->dch_counters_info[1].sum_dch_unreach_discard.value+=soc_reg64_field32_get(unit,DCH_UN_REACH_DEST_0_Sr,reg_val64,UNREACH_DEST_CNT_Sf);
                    fe_counters_info->dch_counters_info[1].sum_dch_unreach_discard.overflow+=soc_reg64_field32_get(unit,DCH_UN_REACH_DEST_0_Sr,reg_val64,UNREACH_DEST_CNT_OSf); 
             
                    temp1=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr,reg_val6,MAX_IFMAFSf);
                    temp2=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr,reg_val6,MAX_IFMBFSf);
                    if (temp1>curr_max)
                    {
                        curr_max=temp1;
                 
                    }
                    if (temp2>curr_max)
                    {
                        curr_max=temp2;
                    }
                }
                fe_counters_info->dch_counters_info[1].dch_max_q_size=curr_max;

            }
    

    

    
            

    

            
            curr_max=0;
            for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCMA;i++)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMUA_TOTAL_IN_CELL_CNT_0r(unit,i,&reg_val));
                dcm_total+=soc_reg_field_get(unit,DCMA_DCMUA_TOTAL_IN_CELL_CNT_0r,reg_val,TOTAL_IN_CELL_CNT_Af);
                dcm_total+=soc_reg_field_get(unit,DCMA_DCMUA_TOTAL_IN_CELL_CNT_0r,reg_val,TOTAL_IN_CELL_CNT_Bf);

                SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMUA_TOTAL_IN_CELL_CNT_1r(unit,i,&reg_val2));
                dcm_total+=soc_reg_field_get(unit,DCMA_DCMUA_TOTAL_IN_CELL_CNT_1r,reg_val2,TOTAL_IN_CELL_CNT_Cf);
                dcm_total+=soc_reg_field_get(unit,DCMA_DCMUA_TOTAL_IN_CELL_CNT_1r,reg_val2,TOTAL_IN_CELL_CNT_Df);

                SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMUA_DROPPED_TOTAL_CNTr(unit,i,&reg_val3));

                dcm_drp+=soc_reg_field_get(unit,DCMA_DCMUA_DROPPED_TOTAL_CNTr,reg_val3,DROPPED_TOTALf);

                SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMUA_DCM_MAX_OCUPP_PRIMERYr(unit,i,&reg_val64));


                for (j=0;j<=27;j=j+9)                   
                {                                       
                    COMPILER_64_SHR(reg_val64,j? 9:0);  

                    temp1=COMPILER_64_LO(reg_val64);                  
                    
                    temp1=temp1 & mask_9_bits;
                    
                    if (temp1>curr_max)
                    {
                        curr_max=temp1;
                    }
                    
                }
            }


            fe_counters_info->dcm_counters_info[0].dcma_tot=dcm_total;
            fe_counters_info->dcm_counters_info[0].dcma_drp=dcm_drp;
            fe_counters_info->dcm_counters_info[0].dcma_max=curr_max;


            
            dcm_total=0;
            dcm_drp=0;
            curr_max=0;
            for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCMB;i++)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMUB_TOTAL_IN_CELL_CNT_0r(unit,i,&reg_val));
                dcm_total+=soc_reg_field_get(unit,DCMB_DCMUB_TOTAL_IN_CELL_CNT_0r,reg_val,TOTAL_IN_CELL_CNT_Af);
                dcm_total+=soc_reg_field_get(unit,DCMB_DCMUB_TOTAL_IN_CELL_CNT_0r,reg_val,TOTAL_IN_CELL_CNT_Bf);

                SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMUB_TOTAL_IN_CELL_CNT_1r(unit,i,&reg_val2));
                dcm_total+=soc_reg_field_get(unit,DCMB_DCMUB_TOTAL_IN_CELL_CNT_1r,reg_val2,TOTAL_IN_CELL_CNT_Cf);
                dcm_total+=soc_reg_field_get(unit,DCMB_DCMUB_TOTAL_IN_CELL_CNT_1r,reg_val2,TOTAL_IN_CELL_CNT_Df);

                SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMUB_DROPPED_TOTAL_CNTr(unit,i,&reg_val3));

                dcm_drp+=soc_reg_field_get(unit,DCMB_DCMUB_DROPPED_TOTAL_CNTr,reg_val3,DROPPED_TOTALf);

                SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMUB_DCM_MAX_OCUPP_PRIMERYr(unit,i,&reg_val64));

                for (j=0;j<=27;j=j+9)
                {
                    COMPILER_64_SHR(reg_val64,j? 9:0); 

                    temp1=COMPILER_64_LO(reg_val64);                  
                    
                    temp1=temp1 & mask_9_bits;
                    
                    if (temp1>curr_max)
                    {
                        curr_max=temp1;
                    }
                    
                }
            }


            fe_counters_info->dcm_counters_info[0].dcmb_tot=dcm_total;
            fe_counters_info->dcm_counters_info[0].dcmb_drp=dcm_drp;
            fe_counters_info->dcm_counters_info[0].dcmb_max=curr_max;


            if (nof_pipes==2) 
            {
                dcm_total=0;
                dcm_drp=0;
                curr_max=0;
                for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCMA;i++)
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMMA_TOTAL_IN_CELL_CNT_0r(unit,i,&reg_val));
                    dcm_total+=soc_reg_field_get(unit,DCMA_DCMUA_TOTAL_IN_CELL_CNT_0r,reg_val,TOTAL_IN_CELL_CNT_Af);
                    dcm_total+=soc_reg_field_get(unit,DCMA_DCMUA_TOTAL_IN_CELL_CNT_0r,reg_val,TOTAL_IN_CELL_CNT_Bf);

                    SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMMA_TOTAL_IN_CELL_CNT_1r(unit,i,&reg_val2));
                    dcm_total+=soc_reg_field_get(unit,DCMA_DCMUA_TOTAL_IN_CELL_CNT_1r,reg_val2,TOTAL_IN_CELL_CNT_Cf);
                    dcm_total+=soc_reg_field_get(unit,DCMA_DCMUA_TOTAL_IN_CELL_CNT_1r,reg_val2,TOTAL_IN_CELL_CNT_Df);

                    SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMMA_DROPPED_TOTAL_CNTr(unit,i,&reg_val3));

                    dcm_drp+=soc_reg_field_get(unit,DCMA_DCMMA_DROPPED_TOTAL_CNTr,reg_val3,DROPPED_TOTALf);

                    SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMMA_DCM_MAX_OCUPP_SECONDARYr(unit,i,&reg_val64));


                    for (j=0;j<=27;j=j+9)
                    {
                        COMPILER_64_SHR(reg_val64,j? 9:0); 

                        temp1=COMPILER_64_LO(reg_val64);                  
                    
                        temp1=temp1 & mask_9_bits;
                    
                        if (temp1>curr_max)
                        {
                            curr_max=temp1;
                        }
                    
                    }
                }


                fe_counters_info->dcm_counters_info[1].dcma_tot=dcm_total;
                fe_counters_info->dcm_counters_info[1].dcma_drp=dcm_drp;
                fe_counters_info->dcm_counters_info[1].dcma_max=curr_max;


                
                dcm_total=0;
                dcm_drp=0;
                curr_max=0;
                for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCMB;i++)
                {
                    SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMMB_TOTAL_IN_CELL_CNT_0r(unit,i,&reg_val));
                    dcm_total+=soc_reg_field_get(unit,DCMB_DCMMB_TOTAL_IN_CELL_CNT_0r,reg_val,TOTAL_IN_CELL_CNT_Af);
                    dcm_total+=soc_reg_field_get(unit,DCMB_DCMMB_TOTAL_IN_CELL_CNT_0r,reg_val,TOTAL_IN_CELL_CNT_Bf);

                    SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMMB_TOTAL_IN_CELL_CNT_1r(unit,i,&reg_val2));
                    dcm_total+=soc_reg_field_get(unit,DCMB_DCMMB_TOTAL_IN_CELL_CNT_1r,reg_val2,TOTAL_IN_CELL_CNT_Cf);
                    dcm_total+=soc_reg_field_get(unit,DCMB_DCMMB_TOTAL_IN_CELL_CNT_1r,reg_val2,TOTAL_IN_CELL_CNT_Df);

                    SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMMB_DROPPED_TOTAL_CNTr(unit,i,&reg_val3));

                    dcm_drp+=soc_reg_field_get(unit,DCMB_DCMMB_DROPPED_TOTAL_CNTr,reg_val3,DROPPED_TOTALf);

                    SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMMB_DCM_MAX_OCUPP_SECONDARYr(unit,i,&reg_val64));

                    for (j=0;j<=27;j=j+9)
                    {
                        COMPILER_64_SHR(reg_val64,j? 9:0); 

                        temp1=COMPILER_64_LO(reg_val64);                  
                    
                        temp1=temp1 & mask_9_bits;
                    
                        if (temp1>curr_max)
                        {
                            curr_max=temp1;
                        }
                    
                    }
                }


                fe_counters_info->dcm_counters_info[1].dcmb_tot=dcm_total;
                fe_counters_info->dcm_counters_info[1].dcmb_drp=dcm_drp;
                fe_counters_info->dcm_counters_info[1].dcmb_max=curr_max;
            }

            
     

            
            
            curr_max=0;
            for (i=0;i<SOC_DFE_DEFS_GET(unit, nof_instances_dcl);i++) 
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_TOTAL_IN_CELL_CNT_Pr(unit,i,&reg_val));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_TOTAL_OUT_CELL_CNT_Pr(unit,i,&reg_val2));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_DROPPED_P_0_CNT_Pr(unit,i,&reg_val3));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_DROPPED_P_1_CNT_Pr(unit,i,&reg_val4));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_DROPPED_P_2_CNT_Pr(unit,i,&reg_val5));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_DROPPED_P_3_CNT_Pr(unit,i,&reg_val6));
                SOCDNX_IF_ERR_EXIT(READ_DCL_MAX_OCUPANCY_FIFO_Pr(unit,i,&reg_val7));

                fe_counters_info->dcl_counters_info[0].dcl_tot_in.value+=soc_reg_field_get(unit,DCL_DCL_TOTAL_IN_CELL_CNT_Pr,reg_val,DCL_TOTAL_IN_CELL_CNT_Pf);
                fe_counters_info->dcl_counters_info[0].dcl_tot_in.overflow+=soc_reg_field_get(unit,DCL_DCL_TOTAL_IN_CELL_CNT_Pr,reg_val,DCL_TOTAL_IN_CELL_CNT_OPf);
                
                fe_counters_info->dcl_counters_info[0].dcl_tot_out.value+=soc_reg_field_get(unit,DCL_DCL_TOTAL_OUT_CELL_CNT_Pr,reg_val2,DCL_TOTAL_OUT_CELL_CNT_Pf);
                fe_counters_info->dcl_counters_info[0].dcl_tot_out.overflow+=soc_reg_field_get(unit,DCL_DCL_TOTAL_OUT_CELL_CNT_Pr,reg_val2,DCL_TOTAL_OUT_CELL_CNT_OPf);
                    
                fe_counters_info->dcl_counters_info[0].dcl_dropped.value+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_0_CNT_Pr,reg_val3,DCL_DROPED_P_0_CNT_Pf);
                fe_counters_info->dcl_counters_info[0].dcl_dropped.overflow+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_0_CNT_Pr,reg_val3,DCL_DROPED_P_0_CNT_OPf);

                fe_counters_info->dcl_counters_info[0].dcl_dropped.value+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_1_CNT_Pr,reg_val4,DCL_DROPED_P_1_CNT_Pf);
                fe_counters_info->dcl_counters_info[0].dcl_dropped.overflow+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_1_CNT_Pr,reg_val4,DCL_DROPED_P_1_CNT_OPf);

                fe_counters_info->dcl_counters_info[0].dcl_dropped.value+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_2_CNT_Pr,reg_val5,DCL_DROPED_P_2_CNT_Pf);
                fe_counters_info->dcl_counters_info[0].dcl_dropped.overflow+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_2_CNT_Pr,reg_val5,DCL_DROPED_P_2_CNT_OPf);

                fe_counters_info->dcl_counters_info[0].dcl_dropped.value+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_3_CNT_Pr,reg_val6,DCL_DROPED_P_3_CNT_Pf);
                fe_counters_info->dcl_counters_info[0].dcl_dropped.overflow+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_3_CNT_Pr,reg_val6,DCL_DROPED_P_3_CNT_OPf);
                
                temp1=soc_reg_field_get(unit,DCL_MAX_OCUPANCY_FIFO_Pr,reg_val7,MAX_OPf);
                if (temp1>curr_max)
                {
                    curr_max=temp1;
                }
            }
            fe_counters_info->dcl_counters_info[0].dcl_max=curr_max;

            
            curr_max=0;
            for (i=0;i<SOC_DFE_DEFS_GET(unit, nof_instances_dcl);i++)   
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_TOTAL_IN_CELL_CNT_Sr(unit,i,&reg_val));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_TOTAL_OUT_CELL_CNT_Sr(unit,i,&reg_val2));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_DROPPED_P_0_CNT_Sr(unit,i,&reg_val3));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_DROPPED_P_1_CNT_Sr(unit,i,&reg_val4));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_DROPPED_P_2_CNT_Sr(unit,i,&reg_val5));
                SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_DROPPED_P_3_CNT_Sr(unit,i,&reg_val6));
                SOCDNX_IF_ERR_EXIT(READ_DCL_MAX_OCUPANCY_FIFO_Sr(unit,i,&reg_val7));

                fe_counters_info->dcl_counters_info[1].dcl_tot_in.value+=soc_reg_field_get(unit,DCL_DCL_TOTAL_IN_CELL_CNT_Sr,reg_val,DCL_TOTAL_IN_CELL_CNT_Sf);
                fe_counters_info->dcl_counters_info[1].dcl_tot_in.overflow+=soc_reg_field_get(unit,DCL_DCL_TOTAL_IN_CELL_CNT_Sr,reg_val,DCL_TOTAL_IN_CELL_CNT_OSf);
                
                fe_counters_info->dcl_counters_info[1].dcl_tot_out.value+=soc_reg_field_get(unit,DCL_DCL_TOTAL_OUT_CELL_CNT_Sr,reg_val2,DCL_TOTAL_OUT_CELL_CNT_Sf);
                fe_counters_info->dcl_counters_info[1].dcl_tot_out.overflow+=soc_reg_field_get(unit,DCL_DCL_TOTAL_OUT_CELL_CNT_Sr,reg_val2,DCL_TOTAL_OUT_CELL_CNT_OSf);
                    
                fe_counters_info->dcl_counters_info[1].dcl_dropped.value+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_0_CNT_Sr,reg_val3,DCL_DROPED_P_0_CNT_Sf);
                fe_counters_info->dcl_counters_info[1].dcl_dropped.overflow+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_0_CNT_Sr,reg_val3,DCL_DROPED_P_0_CNT_OSf);

                fe_counters_info->dcl_counters_info[1].dcl_dropped.value+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_1_CNT_Sr,reg_val4,DCL_DROPED_P_1_CNT_Sf);
                fe_counters_info->dcl_counters_info[1].dcl_dropped.overflow+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_1_CNT_Sr,reg_val4,DCL_DROPED_P_1_CNT_OSf);

                fe_counters_info->dcl_counters_info[1].dcl_dropped.value+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_2_CNT_Sr,reg_val5,DCL_DROPED_P_2_CNT_Sf);
                fe_counters_info->dcl_counters_info[1].dcl_dropped.overflow+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_2_CNT_Sr,reg_val5,DCL_DROPED_P_2_CNT_OSf);

                fe_counters_info->dcl_counters_info[1].dcl_dropped.value+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_3_CNT_Sr,reg_val6,DCL_DROPED_P_3_CNT_Sf);
                fe_counters_info->dcl_counters_info[1].dcl_dropped.overflow+=soc_reg_field_get(unit,DCL_DCL_DROPPED_P_3_CNT_Sr,reg_val6,DCL_DROPED_P_3_CNT_OSf);
                
                temp1=soc_reg_field_get(unit,DCL_MAX_OCUPANCY_FIFO_Sr,reg_val7,MAX_OSf);
                if (temp1>curr_max)
                {
                    curr_max=temp1;
                }
            }
            fe_counters_info->dcl_counters_info[1].dcl_max=curr_max;



            
            

            
        
            for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCH;i++)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCH_DCMU_FLOW_CNTROL_CNTr(unit,i,&reg_val));  

                fe_counters_info->dch_dcm_fc[0].value+=soc_reg_field_get(unit,DCH_DCMU_FLOW_CNTROL_CNTr,reg_val,DCMU_FLOW_CNTROL_CNTf);
                fe_counters_info->dch_dcm_fc[0].overflow+=soc_reg_field_get(unit,DCH_DCMU_FLOW_CNTROL_CNTr,reg_val,DCMU_FLOW_CNTROL_CNT_OVERFLOWf);
            }
            

            for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCMA;i++)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCMA_DCLU_FLOW_CNTROL_CNTr(unit,i,&reg_val));
                SOCDNX_IF_ERR_EXIT(READ_DCMB_DCLU_FLOW_CNTROL_CNTr(unit,i,&reg_val2));

                fe_counters_info->dcm_dcl_fc[0].value+=soc_reg_field_get(unit,DCMA_DCLU_FLOW_CNTROL_CNTr,reg_val,DCLU_FLOW_CNTROL_CNTf);
                fe_counters_info->dcm_dcl_fc[0].value+=soc_reg_field_get(unit,DCMB_DCLU_FLOW_CNTROL_CNTr,reg_val2,DCLU_FLOW_CNTROL_CNTf);

                fe_counters_info->dcm_dcl_fc[0].overflow+=soc_reg_field_get(unit,DCMA_DCLU_FLOW_CNTROL_CNTr,reg_val,DCLU_FLOW_CNTROL_CNT_OVERFLOWf);
                fe_counters_info->dcm_dcl_fc[0].overflow+=soc_reg_field_get(unit,DCMB_DCLU_FLOW_CNTROL_CNTr,reg_val2,DCLU_FLOW_CNTROL_CNT_OVERFLOWf);
                  
            } 
              

            for (i=0;i<SOC_FE1600_NOF_INSTANCES_CCS;i++)
            {
                SOCDNX_IF_ERR_EXIT(READ_CCS_TOTAL_CELLS_CNTr(unit,i,&reg_val));
                SOCDNX_IF_ERR_EXIT(READ_CCS_FLOW_STATUS_CELLS_CNTr(unit,i,&reg_val2));
                SOCDNX_IF_ERR_EXIT(READ_CCS_CREDIT_CELLS_CNTr(unit,i,&reg_val3));
                SOCDNX_IF_ERR_EXIT(READ_CCS_REACHABILITY_CELLS_CNTr(unit,i,&reg_val4));
                SOCDNX_IF_ERR_EXIT(READ_CCS_UNREACHABLE_DESTINATION_CELLS_CNTr(unit,i,&reg_val5));

                fe_counters_info->ccs_counters_info.ccs_total_control.value+=soc_reg_field_get(unit,CCS_TOTAL_CELLS_CNTr,reg_val,TOTAL_CELLS_CNTf);
                fe_counters_info->ccs_counters_info.ccs_total_control.overflow+=soc_reg_field_get(unit,CCS_TOTAL_CELLS_CNTr,reg_val,TOTAL_CELLS_CNT_OVERFLOWf);

                fe_counters_info->ccs_counters_info.ccs_flow_stat.value+=soc_reg_field_get(unit,CCS_FLOW_STATUS_CELLS_CNTr,reg_val2,FLOW_STATUS_CELLS_CNTf);
                fe_counters_info->ccs_counters_info.ccs_flow_stat.overflow+=soc_reg_field_get(unit,CCS_FLOW_STATUS_CELLS_CNTr,reg_val2,FLOW_STATUS_CELLS_CNT_OVERFLOWf);

                fe_counters_info->ccs_counters_info.ccs_credits.value+=soc_reg_field_get(unit,CCS_CREDIT_CELLS_CNTr,reg_val3,CREDIT_CELLS_CNTf);
                fe_counters_info->ccs_counters_info.ccs_credits.overflow+=soc_reg_field_get(unit,CCS_CREDIT_CELLS_CNTr,reg_val3,CREDIT_CELLS_CNT_OVERFLOWf);

                fe_counters_info->ccs_counters_info.ccs_reachability.value+=soc_reg_field_get(unit,CCS_REACHABILITY_CELLS_CNTr,reg_val4,REACHABILITY_CELLS_CNTf);
                fe_counters_info->ccs_counters_info.ccs_reachability.overflow+=soc_reg_field_get(unit,CCS_REACHABILITY_CELLS_CNTr,reg_val4,REACHABILITY_CELLS_CNT_OVERFLOWf);

                fe_counters_info->ccs_counters_info.ccs_unreach_dest.value+=soc_reg_field_get(unit,CCS_UNREACHABLE_DESTINATION_CELLS_CNTr,reg_val5,UNREACHABLE_DESTINATION_CELLS_CNTf);
                fe_counters_info->ccs_counters_info.ccs_unreach_dest.overflow+=soc_reg_field_get(unit,CCS_UNREACHABLE_DESTINATION_CELLS_CNTr,reg_val5,UNREACHABLE_DESTINATION_CELLS_CNT_OVERFLOWf);
            }
          
            
exit:
   SOCDNX_FUNC_RETURN;

}



soc_error_t 
soc_fe1600_queues_get_info(int unit,soc_dfe_queues_info_t* fe_queues_info)
{
    int i,j;
    uint32 nof_pipes;
    uint32 reg_val;
    uint32 nof_link;
    uint32 nof_fifo;
    uint32 temp1;
    uint32 mask_9_bits=0x1FF;

    uint64 reg_val64;

    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);
     

    if(SOC_DFE_CONFIG(unit).is_dual_mode)
    {
        nof_pipes=2;
        fe_queues_info->nof_pipes=2;
        
    }
    else{
        nof_pipes=1;
        fe_queues_info->nof_pipes=1;
        
    }

    
    
    for (i=0;i<SOC_DFE_DEFS_GET(unit, nof_instances_dch);i++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr(unit,i,&reg_val));

        nof_fifo=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr,reg_val,MAX_IFMF_NUMA_Pf);
        nof_link=i*SOC_FE1600_NOF_LINKS_IN_DCQ+nof_fifo;

        fe_queues_info->dch_queues_info[0].nof_dch[i].nof_link_a=nof_link;
        fe_queues_info->dch_queues_info[0].nof_dch[i].value_a=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr,reg_val,MAX_IFMAFPf);

        nof_fifo=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr,reg_val,MAX_IFMF_NUMB_Pf);
        nof_link=i*SOC_FE1600_NOF_LINKS_IN_DCQ+SOC_FE1600_NOF_LINKS_IN_DCQ/2+nof_fifo;

        fe_queues_info->dch_queues_info[0].nof_dch[i].nof_link_b=nof_link;
        fe_queues_info->dch_queues_info[0].nof_dch[i].value_b=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr,reg_val,MAX_IFMBFPf);

    }
    
    if (nof_pipes==2) 
    {
        for (i=0;i<SOC_DFE_DEFS_GET(unit, nof_instances_dch);i++) 
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr(unit,i,&reg_val));

            nof_fifo=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr,reg_val,MAX_IFMF_NUMA_Sf);
            nof_link=i*SOC_FE1600_NOF_LINKS_IN_DCQ+nof_fifo;

            fe_queues_info->dch_queues_info[1].nof_dch[i].nof_link_a=nof_link;
            fe_queues_info->dch_queues_info[1].nof_dch[i].value_a=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr,reg_val,MAX_IFMAFSf);

            nof_fifo=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr,reg_val,MAX_IFMF_NUMB_Sf);
            nof_link=i*SOC_FE1600_NOF_LINKS_IN_DCQ+SOC_FE1600_NOF_LINKS_IN_DCQ/2+nof_fifo;

            fe_queues_info->dch_queues_info[1].nof_dch[i].nof_link_b=nof_link;
            fe_queues_info->dch_queues_info[1].nof_dch[i].value_b=soc_reg_field_get(unit,DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr,reg_val,MAX_IFMBFSf);

        }
    }

    
    
    for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCMA;i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMUA_DCM_MAX_OCUPP_PRIMERYr(unit,i,&reg_val64));

        for (j=0;j<4;j++)
        {
            COMPILER_64_SHR(reg_val64,j? 9:0);
            temp1=COMPILER_64_LO(reg_val64);
            temp1=temp1 & mask_9_bits;
            fe_queues_info->dcm_queues_info[0].nof_dcma[i].nof_fifo[j]=temp1;    
        }
    
    }

    if(nof_pipes==2)
    {
        for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCMA;i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMMA_DCM_MAX_OCUPP_SECONDARYr(unit,i,&reg_val64));

            for (j=0;j<4;j++) 
            {
                COMPILER_64_SHR(reg_val64,j? 9:0); 
                temp1=COMPILER_64_LO(reg_val64);
                temp1=temp1 & mask_9_bits;
                fe_queues_info->dcm_queues_info[1].nof_dcma[i].nof_fifo[j]=temp1;    
            }
    
        }
    }

    
    for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCMB;i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMUB_DCM_MAX_OCUPP_PRIMERYr(unit,i,&reg_val64));

        for (j=0;j<4;j++)
        {
            COMPILER_64_SHR(reg_val64,j? 9:0);
            temp1=COMPILER_64_LO(reg_val64);
            temp1=temp1 & mask_9_bits;
            fe_queues_info->dcm_queues_info[0].nof_dcmb[i].nof_fifo[j]=temp1;    
        }
    
    }

    if(nof_pipes==2)
    {
        for (i=0;i<SOC_FE1600_NOF_INSTANCES_DCMB;i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMMB_DCM_MAX_OCUPP_SECONDARYr(unit,i,&reg_val64));

            for (j=0;j<4;j++) 
            {
                COMPILER_64_SHR(reg_val64,j? 9:0); 
                temp1=COMPILER_64_LO(reg_val64);
                temp1=temp1 & mask_9_bits;
                fe_queues_info->dcm_queues_info[1].nof_dcmb[i].nof_fifo[j]=temp1;    
            }
    
        }
    }





    
    
    for (i=0;i<SOC_DFE_DEFS_GET(unit, nof_instances_dcl);i++)
    {   
        SOCDNX_IF_ERR_EXIT(READ_DCL_MAX_OCUPANCY_FIFO_Pr(unit,i,&reg_val));

        nof_fifo=soc_reg_field_get(unit,DCL_MAX_OCUPANCY_FIFO_Pr,reg_val,MAX_O_FF_Pf);
        nof_link=i*SOC_FE1600_NOF_LINKS_IN_DCQ+nof_fifo;

        fe_queues_info->dcl_queues_info[0].nof_dcl[i].nof_link=nof_link;
        fe_queues_info->dcl_queues_info[0].nof_dcl[i].value=soc_reg_field_get(unit,DCL_MAX_OCUPANCY_FIFO_Pr,reg_val,MAX_OPf);

    }

    if (nof_pipes==2) 
    {   
         for (i=0;i<SOC_DFE_DEFS_GET(unit, nof_instances_dcl);i++) 
         {   
             SOCDNX_IF_ERR_EXIT(READ_DCL_MAX_OCUPANCY_FIFO_Sr(unit,i,&reg_val));

             nof_fifo=soc_reg_field_get(unit,DCL_MAX_OCUPANCY_FIFO_Sr,reg_val,MAX_O_FF_Sf);
             nof_link=i*SOC_FE1600_NOF_LINKS_IN_DCQ+nof_fifo;

             fe_queues_info->dcl_queues_info[1].nof_dcl[i].nof_link=nof_link;
             fe_queues_info->dcl_queues_info[1].nof_dcl[i].value=soc_reg_field_get(unit,DCL_MAX_OCUPANCY_FIFO_Sr,reg_val,MAX_OSf);

         }

    }

exit:
   SOCDNX_FUNC_RETURN;
}


const soc_dfe_diag_flag_str_t soc_fe1600_diag_flag_str_interrupts_names[] = 
{
    {0, "ECI_GLOBAL"},
    {1, ""},
    {2, "DCH0"},
    {3, "DCH1"},
    {4, "DCH2"},
    {5, ""},
    {6, "DCH3"},
    {7, "DCL0"},
    {8, "DCL1"},
    {9, "DCL2"},
    {10, "DCL3"},
    {11, "DCM0"},
    {12, "DCM1"},
    {13, "DCM2"},
    {14, "DCM3"},
    {15, "DCMC"},
    {16, "CCS0"},
    {17, "CCS1"},
    {18, "RTP"},
    {19, ""},
    {20, "FMAC0"},
    {21, "FMAC1"},
    {22, "FMAC2"},
    {23, "FMAC3"},
    {24, "FMAC4"},
    {25, "FMAC5"},
    {26, "FMAC6"},
    {27, "FMAC7"},
    {28, "FMAC8"},
    {29, "FMAC9"},
    {30, "FMAC10"},
    {31, "FMAC11"},
    {32, "FMAC12"},
    {33, "FMAC13"},
    {34, "FMAC14"},
    {35, "FMAC15"},
    {36, "FMAC16"},
    {37, "FMAC17"},
    {38, "FMAC18"},
    {39, "FMAC19"},
    {40, "FMAC20"},
    {41, "FMAC21"},
    {42, "FMAC22"},
    {43, "FMAC23"},
    {44, "FMAC24"},
    {45, "FMAC25"},
    {46, "FMAC26"},
    {47, "FMAC27"},
    {48, "FMAC28"},
    {49, "FMAC29"},
    {50, "FMAC30"},
    {51, "FMAC31"},
    {52, "FSRD0"},
    {53, "FSRD1"},
    {54, "FSRD2"},
    {55, "FSRD3"},
    {56, "FSRD4"},
    {57, "FSRD5"},
    {58, "FSRD6"},
    {59, "FSRD7"},
    {59, "ECI_INTERNAL"},
    {-1,  NULL}
};
 
soc_error_t
soc_fe1600_diag_fabric_cell_snake_test_interrupts_name_get(int unit, const soc_dfe_diag_flag_str_t **intr_names)
{
    SOCDNX_INIT_FUNC_DEFS;

    *intr_names = soc_fe1600_diag_flag_str_interrupts_names;

    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME
