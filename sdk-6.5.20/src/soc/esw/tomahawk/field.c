/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC Routines related to Field module.
 */

#include <soc/debug.h>
#include <soc/tomahawk.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/xgs4.h>
#if defined(BCM_TOMAHAWK_SUPPORT)

/*
 * Function:
 *      soc_th_field_mem_mode_get
 * Purpose:
 *      Retrieve Field Processor Group operational mode
 *      for the given field memory.
 * Parameters:
 *      unit    - (IN)  BCM Device number.
 *      mem     - (IN)  Field TCAM Memory.
 *      mode    - (OUT) Reference to SER memory mode.
 *
 * Returns:
 *      SOC_E_NONE   - Operation successful.
 *      SOC_E_PARAM  - Invalid parameter.
 *      SOC_E_INIT   - BCM unit not initialized.
 *
 * Notes:
 *      Valid "mem" input parameter values supported by
 *      this routine:
 *          "VFP_TCAMm, EFP_TCAMm, IFP_TCAMm,"
 *          "IFP_TCAM_WIDEm, IFP_LOGICAL_TABLE_TCAMm"
 *          "EXACT_MATCH_LOGICAL_TABLE_SELECTm."
 */
int
soc_th_field_mem_mode_get(int unit, soc_mem_t mem, int *mode)
{
   bcm_field_qualify_t          stage;      /* Field Stage Qualifier Id */
   bcm_field_group_oper_mode_t  oper_mode;  /* Field Operation Mode */
   int                          rv;         /* Return Value */

   /* Validate input parameters */
   if (mode == NULL) {
      return SOC_E_PARAM;
   }

   switch (mem) {
     /* Field lookup TCAMs and SRAMs */
     case VFP_TCAMm:
     case VFP_POLICY_TABLEm:
          stage = bcmFieldQualifyStageLookup;
          break;

     /* Field Egress TCAMs and SRAMs */
     case EFP_TCAMm:
     case EFP_POLICY_TABLEm:
     case EFP_COUNTER_TABLEm:
     case EFP_METER_TABLEm:
          stage = bcmFieldQualifyStageEgress;
          break;

     /* Field Ingress TCAMs */
     case IFP_LOGICAL_TABLE_SELECTm:
     case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
     case IFP_TCAMm:
     /* Field Ingress SRAMs */
     case IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm:
     case IFP_LOGICAL_TABLE_ACTION_PRIORITYm:
     case IFP_KEY_GEN_PROGRAM_PROFILEm:
     case IFP_KEY_GEN_PROGRAM_PROFILE2m:
     case IFP_POLICY_TABLEm:
     case IFP_METER_TABLEm:
          stage = bcmFieldQualifyStageIngress;
          break;

     /* Exact Match TCAMs */ 
     case EXACT_MATCH_LOGICAL_TABLE_SELECTm:
     case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
     /* Exact Match SRAMs */ 
     case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm:
     case EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm:
     case EXACT_MATCH_ACTION_PROFILEm:
     case EXACT_MATCH_QOS_ACTIONS_PROFILEm:
     case EXACT_MATCH_DEFAULT_POLICYm:
     case EXACT_MATCH_KEY_GEN_MASKm:
     case EXACT_MATCH_2m:
     case EXACT_MATCH_4m:
     case EXACT_MATCH_2_ENTRY_ONLYm:
     case EXACT_MATCH_4_ENTRY_ONLYm:
          stage = bcmFieldQualifyStageIngressExactMatch;
          break;

     /* Field Ingress Compression TCAMs and SRAMs */
     case SRC_COMPRESSIONm:
     case SRC_COMPRESSION_TCAM_ONLYm:
     case SRC_COMPRESSION_DATA_ONLYm:
     case DST_COMPRESSIONm:
     case DST_COMPRESSION_TCAM_ONLYm:
     case DST_COMPRESSION_DATA_ONLYm:
          stage = bcmFieldQualifyStageClass;
          break;
     case TTL_FNm:
     case TOS_FNm:
     case TCP_FNm:
     case IP_PROTO_MAPm:
          stage = bcmFieldQualifyStageClassExactMatch;
          break;
     case FP_UDF_TCAMm:
     case FP_UDF_OFFSETm:
          /* 
           * When udf support is not available or the memory is not being used
           * by either UDF or FIELD module, return BCM_E_UNAVAIL
           */
          if (soc_feature(unit, soc_feature_udf_support) && UDF_CTRL(unit)) {
             if (UDF_CTRL(unit)->udf_used_by_module == UDF_USED_BY_UDF_MODULE) {
                  bcm_udf_oper_mode_t udf_oper_mode = 0;
                  rv = bcmi_xgs4_udf_oper_mode_get(unit, &udf_oper_mode);
                  if (BCM_SUCCESS(rv)) {
                     *mode = (udf_oper_mode == bcmUdfOperModeGlobal) ?
                      _SOC_SER_MEM_MODE_GLOBAL : _SOC_SER_MEM_MODE_PIPE_UNIQUE;
                  }
              } else if(UDF_CTRL(unit)->udf_used_by_module ==
                                                     UDF_USED_BY_FIELD_MODULE) {
                  /*
                   *  Currently Field Module is not supporting per-pipe mode
                   *  for this memory. Hence, returning it as Global mode.
                   */
                  *mode = _SOC_SER_MEM_MODE_GLOBAL;
                  rv = BCM_E_NONE;
              } else {
                  /* Not used by any of the module. */
                  rv = BCM_E_UNAVAIL;
              }
          } else {
              /* Feature doesn't support */
              rv = BCM_E_UNAVAIL;
          }

          if (BCM_FAILURE(rv)) {
              LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                      "Failed to get Operation Mode[%d] for UDF Memory[%d].\n"),
                       rv, mem));
          }
          return rv; 
     case IFP_TCAM_WIDEm:
          if (soc_property_get(unit, spn_FIELD_ATOMIC_UPDATE, FALSE) == TRUE) {
             stage = bcmFieldQualifyStageIngress;
          } else {
            /*
             *  Inorder to support bcmFieldQualifyInPorts in Global mode,
             *  IP_TCAM_WIDE should always be in pipe local mode.
             */
             *mode = _SOC_SER_MEM_MODE_PIPE_UNIQUE;
            return BCM_E_NONE;
          }
          break;

     default:
          return SOC_E_UNAVAIL;
          break;
   }

   rv = _bcm_field_th_group_oper_mode_get(unit, stage, &oper_mode);
   if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "Failed to get Group Operation Mode[%d] for "
                             "Stage Qualifier[%d]/TCAM Memory[%d].\n"),
                             rv, stage, mem));
       return rv;
   }

   *mode = (oper_mode == bcmFieldGroupOperModeGlobal) ?
               _SOC_SER_MEM_MODE_GLOBAL:_SOC_SER_MEM_MODE_PIPE_UNIQUE;

   return SOC_E_NONE;
}

/*
 * Function:
 *     soc_mem_field_nw_tcam_prio_order_index_get
 *
 * Purpose:
 *     To retrieve the right priority index for the given index for the groups
 *     operated in narrow wide (80-bit) TCAMs.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     mem      - (IN) Memory
 *     tcam_idx - (IN) Reference to TCAM index.
 *
 * Returns:
 *     SOC_E_NONE      - Success
 */
int soc_mem_field_nw_tcam_prio_order_index_get(int unit,
                                               soc_mem_t mem,
                                               int *tcam_idx)
{
   int slice_num;
   soc_reg_t reg = INVALIDr;

   if (soc_feature(unit, soc_feature_fp_nw_tcam_prio_order_war) &&
       ((mem == IFP_TCAMm) || (mem == IFP_TCAM_PIPE0m) ||
        (mem == IFP_TCAM_PIPE1m) || (mem == IFP_TCAM_PIPE2m) ||
        (mem == IFP_TCAM_PIPE3m) || (mem == IFP_POLICY_TABLEm) ||
        (mem == IFP_POLICY_TABLE_PIPE0m) || (mem == IFP_POLICY_TABLE_PIPE1m) ||
        (mem == IFP_POLICY_TABLE_PIPE2m) || (mem == IFP_POLICY_TABLE_PIPE3m))) {
   
      if (tcam_idx == NULL) {
         return SOC_E_PARAM;
      }
   
      /*
       * As IFP_POLICY_TABLE is common among multiple IFP_TCAM modes,
       * retrieve the field mode in-case of Policy Table.
       */              
      if (mem == IFP_POLICY_TABLEm) {
         reg = IFP_CONFIGr;
      } else if (mem == IFP_POLICY_TABLE_PIPE0m) {
         reg = IFP_CONFIG_PIPE0r;
      } else if (mem == IFP_POLICY_TABLE_PIPE1m) {
         reg = IFP_CONFIG_PIPE1r;
      } else if (mem == IFP_POLICY_TABLE_PIPE2m) {
         reg = IFP_CONFIG_PIPE2r;
      } else if (mem == IFP_POLICY_TABLE_PIPE3m) {
         reg = IFP_CONFIG_PIPE3r;
      }

      /* Calculate slice number of the given tcam_idx. */
      slice_num = (*tcam_idx)/512;

      if (reg != INVALIDr) {
         uint32 regval = 0;
         uint32 mode; 

         SOC_IF_ERROR_RETURN
           (soc_reg32_get(unit, reg, REG_PORT_ANY, slice_num, &regval));

         if (soc_feature(unit, soc_feature_ifp_no_narrow_mode_support)) {
            mode = 1;
         } else {
            mode = soc_reg_field_get(unit, reg, regval, IFP_SLICE_MODEf);
         }

         /* Return if the TCAM mode is WIDE. */
         if (mode == 1) {
            return SOC_E_NONE;
         }
      }

      LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Changed TCAM_ID: %d -"), *tcam_idx));

      if (*tcam_idx % 2) {
         *tcam_idx = 256 + (slice_num * 256) + ((*tcam_idx)/2);
      } else {
         *tcam_idx = (slice_num * 256) + ((*tcam_idx)/2);
      }

      LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                " %d for slice:%d\n\r"), *tcam_idx, slice_num));
   }

   return SOC_E_NONE;
}

/*
 * Function:
 *    soc_th_ifp_mem_write
 * Purpose:
 *    Write a memory internal to the SOC for TH IFP.
 * Notes:
 *    GBP/CBP memory should only accessed when MMU is in DEBUG mode.
 */
int
soc_th_ifp_mem_write(int unit, soc_mem_t mem, int copyno, int index,
                     void *entry_data)
{
    int rv;

    if (soc_feature(unit, soc_feature_fp_nw_tcam_prio_order_war) &&
        ((mem == IFP_TCAMm) || (mem == IFP_TCAM_PIPE0m) ||
        (mem == IFP_TCAM_PIPE1m) || (mem == IFP_TCAM_PIPE2m) ||
        (mem == IFP_TCAM_PIPE3m) || (mem == IFP_POLICY_TABLEm) ||
        (mem == IFP_POLICY_TABLE_PIPE0m) || (mem == IFP_POLICY_TABLE_PIPE1m) ||
        (mem == IFP_POLICY_TABLE_PIPE2m) || (mem == IFP_POLICY_TABLE_PIPE3m))) {

         rv = soc_mem_field_nw_tcam_prio_order_index_get(unit, mem, &index);
         SOC_IF_ERROR_RETURN(rv);

    }

    return soc_mem_write(unit, mem, copyno, index, entry_data);
}

/*
 * Function:
 *    soc_th_ifp_mem_read
 * Purpose:
 *    Read a memory internal to the SOC for TH IFP.
 * Notes:
 *    GBP/CBP memory should only accessed when MMU is in DEBUG mode.
 */
int
soc_th_ifp_mem_read(int unit, soc_mem_t mem, int copyno, int index,
                    void *entry_data)
{
    int rv;

    if (soc_feature(unit, soc_feature_fp_nw_tcam_prio_order_war) &&
        ((mem == IFP_TCAMm) || (mem == IFP_TCAM_PIPE0m) ||
        (mem == IFP_TCAM_PIPE1m) || (mem == IFP_TCAM_PIPE2m) ||
        (mem == IFP_TCAM_PIPE3m) || (mem == IFP_POLICY_TABLEm) ||
        (mem == IFP_POLICY_TABLE_PIPE0m) || (mem == IFP_POLICY_TABLE_PIPE1m) ||
        (mem == IFP_POLICY_TABLE_PIPE2m) || (mem == IFP_POLICY_TABLE_PIPE3m))) {

           rv = soc_mem_field_nw_tcam_prio_order_index_get(unit, mem, &index);
           SOC_IF_ERROR_RETURN(rv);

    }

    return soc_mem_read(unit, mem, copyno, index, entry_data);
}

/*
 * Function:
 *      soc_th_ifp_slice_mode_get
 * Purpose:
 *      Retrieve the IFP TCAM slice mode and enable status.
 * Parameters:
 *      unit       - (IN) BCM Device number.
 *      pipe       - (IN) Pipe Instance. (pipe < 0 indicates Global.)
 *      slice_num  - (IN) IFP TCAM Slice number.
 *      slice_mode - (OUT) IFP TCAM mode.
 *      slice_enabled - (OUT) Whether the slice is enabled.
 *
 * Returns:
 *      SOC_E_NONE   - Operation successful.
 *      SOC_E_PARAM  - Invalid parameter.
 *      SOC_E_INIT   - Field Module is not initialized.
 */
int
soc_th_ifp_slice_mode_get(int unit, int pipe, int slice_num, int *slice_mode,
                          int *slice_enabled)
{
   int    rv;         /* Return Value */

   /* Validate input parameters */
   if (slice_mode == NULL || slice_enabled == NULL) {
      return SOC_E_PARAM;
   }

   rv = _bcm_field_th_ifp_slice_mode_get(unit, pipe, slice_num,
                                    slice_mode, slice_enabled);
   return rv;
}

#endif /* BCM_TOMAHAWK_SUPPORT */
