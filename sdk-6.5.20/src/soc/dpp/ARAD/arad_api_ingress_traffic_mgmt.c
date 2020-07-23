#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_general.h>


























uint32
  arad_itm_dram_buffs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_DRAM_BUFFS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dram_buffs);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_dram_buffs_get_unsafe(
    unit,
    dram_buffs
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_dram_buffs_get()",0,0);
}


uint32
  arad_itm_glob_rcs_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_FC_TH  *info,
    SOC_SAND_OUT  ARAD_ITM_GLOB_RCS_FC_TH  *exact_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_FC_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = arad_itm_glob_rcs_fc_verify(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_glob_rcs_fc_set_unsafe(
    unit,
    info,
    exact_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_fc_set()",0,0);
}


uint32
  arad_itm_glob_rcs_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_FC_TH  *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_FC_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_glob_rcs_fc_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_fc_get()",0,0);
}


int
  arad_itm_glob_rcs_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *exact_info
  )
{
  uint32
    res;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);
  SOCDNX_NULL_CHECK(exact_info);

  res = arad_itm_glob_rcs_drop_verify(
    unit,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_itm_glob_rcs_drop_set_unsafe(
    unit,
    info,
    exact_info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  arad_itm_glob_rcs_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_glob_rcs_drop_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_get()",0,0);
}


int
  arad_itm_category_rngs_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info
  )
{
  uint32
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  res = arad_itm_category_rngs_verify(
    unit,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_itm_category_rngs_set_unsafe(
    unit,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_category_rngs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT ARAD_ITM_CATEGORY_RNGS *info
  )
{
  uint32
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  res = arad_itm_category_rngs_get_unsafe(
    unit,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_itm_admit_test_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_itm_admit_test_tmplt_verify(
    unit,
    admt_tst_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_admit_test_tmplt_set_unsafe(
    unit,
    admt_tst_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_set()",0,0);
}


uint32
  arad_itm_admit_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_admit_test_tmplt_get_unsafe(
    unit,
    admt_tst_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_get()",0,0);
}


uint32
  arad_itm_cr_request_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_INFO *info,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *exact_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = arad_itm_cr_request_verify(
    unit,
    qt_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_cr_request_set_unsafe(
    unit,
    qt_ndx,
    info,
    exact_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_request_set()",0,0);
}


uint32
  arad_itm_cr_request_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_cr_request_get_unsafe(
    unit,
    qt_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_request_get()", unit, qt_ndx);
}


uint32
  arad_itm_cr_discount_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_DISCOUNT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_itm_cr_discount_verify(
    unit,
    cr_cls_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_cr_discount_set_unsafe(
    unit,
    cr_cls_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_discount_set()",0,0);
}


uint32
  arad_itm_cr_discount_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_DISCOUNT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_cr_discount_get_unsafe(
    unit,
    cr_cls_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_discount_get()",0,0);
}


uint32
  arad_itm_queue_test_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TSTS      test_tmplt
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_TEST_TMPLT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_itm_queue_test_tmplt_verify(
    unit,
    rt_cls_ndx,
    drop_precedence_ndx,
    test_tmplt
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_queue_test_tmplt_set_unsafe(
    unit,
    rt_cls_ndx,
    drop_precedence_ndx,
    test_tmplt
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_test_tmplt_set()",0,0);
}


uint32
  arad_itm_queue_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TSTS      *test_tmplt
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_TEST_TMPLT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(test_tmplt);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_queue_test_tmplt_get_unsafe(
    unit,
    rt_cls_ndx,
    drop_precedence_ndx,
    test_tmplt
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_test_tmplt_get()",0,0);
}


int
  arad_itm_wred_exp_wq_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq,
    SOC_SAND_IN  uint8                   enable
  )
{
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_SAND_IF_ERR_EXIT(arad_itm_wred_exp_wq_verify(
    unit,
    rt_cls_ndx,
    exp_wq
                                  ));

  SOCDNX_SAND_IF_ERR_EXIT(arad_itm_wred_exp_wq_set_unsafe(
    unit,
    rt_cls_ndx,
    exp_wq,
    enable
                                  ));
exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_wred_exp_wq_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT  uint32                  *exp_wq
  )
{

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(exp_wq);

  SOCDNX_SAND_IF_ERR_EXIT(arad_itm_wred_exp_wq_get_unsafe(
    unit,
    rt_cls_ndx,
    exp_wq)
  );

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_itm_wred_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  )
{
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);
  SOCDNX_NULL_CHECK(exact_info);

  SOCDNX_SAND_IF_ERR_EXIT(arad_itm_wred_verify(
                                  unit,
                                  rt_cls_ndx,
                                  drop_precedence_ndx,
                                  info)
          );


  SOCDNX_SAND_IF_ERR_EXIT(arad_itm_wred_set_unsafe(
                                  unit,
                                  rt_cls_ndx,
                                  drop_precedence_ndx,
                                  info,
                                  exact_info)
                                  );
exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_wred_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  )
{

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  SOCDNX_SAND_IF_ERR_EXIT( arad_itm_wred_get_unsafe(
                                   unit,
                                   rt_cls_ndx,
                                   drop_precedence_ndx,
                                   info)
          );
exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT  ARAD_ITM_TAIL_DROP_INFO  *exact_info
  )
{

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);
  SOCDNX_NULL_CHECK(exact_info);

  SOCDNX_SAND_IF_ERR_EXIT(arad_itm_tail_drop_verify(
                                  unit,
                                  rt_cls_ndx,
                                  drop_precedence_ndx,
                                  info)
          );
  SOCDNX_SAND_IF_ERR_EXIT( arad_itm_tail_drop_set_unsafe(
                                   unit,
                                   rt_cls_ndx,
                                   drop_precedence_ndx,
                                   info,
                                   exact_info)
          );
exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_itm_tail_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *info
  )
{

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  SOCDNX_SAND_IF_ERR_EXIT(arad_itm_tail_drop_get_unsafe(
                                  unit,
                                  rt_cls_ndx,
                                  drop_precedence_ndx,
                                  info)
          );
exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_itm_fadt_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_FADT_DROP_INFO *info,
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO *exact_info
  )

{
    SOC_TMC_ITM_TAIL_DROP_INFO tail_drop_info, exact_tail_drop_info;
    int set_alpha;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_ITM_TAIL_DROP_INFO_clear(&tail_drop_info);
    SOC_TMC_ITM_TAIL_DROP_INFO_clear(&exact_tail_drop_info);

    
    tail_drop_info.max_inst_q_size = info->max_threshold[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES];
    tail_drop_info.max_inst_q_size_bds = info->max_inst_q_size_bds;

    SOCDNX_IF_ERR_EXIT(arad_itm_tail_drop_set(unit, rt_cls_ndx, drop_precedence_ndx, &tail_drop_info, &exact_tail_drop_info));
        
    exact_info->max_threshold[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] = exact_tail_drop_info.max_inst_q_size;
    exact_info->max_inst_q_size_bds = exact_tail_drop_info.max_inst_q_size_bds;
    
    exact_info->adjust_factor[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] = ARAD_PLUS_ALPHA_NOT_SUPPORTED;
    
    set_alpha = SOC_IS_ARADPLUS(unit);

    if (set_alpha) {
        SOCDNX_SAND_IF_ERR_EXIT (arad_plus_itm_alpha_set(unit, rt_cls_ndx, drop_precedence_ndx, info->adjust_factor_bds));
        exact_info->adjust_factor_bds = info->adjust_factor_bds;
    }
    else {
        exact_info->adjust_factor_bds = 0;
    }
exit:
    SOCDNX_FUNC_RETURN;

}

int
  arad_itm_fadt_tail_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO  *info
  )

{
    SOC_TMC_ITM_TAIL_DROP_INFO tail_drop_info;
    int alpha = 0;
    int set_alpha;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_TMC_ITM_TAIL_DROP_INFO_clear(&tail_drop_info);

    SOCDNX_SAND_IF_ERR_EXIT(arad_itm_tail_drop_get(unit,rt_cls_ndx,drop_precedence_ndx,&tail_drop_info));

    
    info->max_threshold[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] = tail_drop_info.max_inst_q_size;
    info->adjust_factor[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] = ARAD_PLUS_ALPHA_NOT_SUPPORTED;
    info->max_inst_q_size_bds = tail_drop_info.max_inst_q_size_bds ;

    set_alpha = SOC_IS_ARADPLUS(unit);
    if (set_alpha) {
        SOCDNX_SAND_IF_ERR_EXIT (arad_plus_itm_alpha_get(unit, rt_cls_ndx, drop_precedence_ndx, &alpha));
        info->adjust_factor_bds = alpha;
    } 
    else {
        info->adjust_factor_bds = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;

}





uint32
  arad_itm_cr_wd_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  ARAD_ITM_CR_WD_INFO      *info,
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *exact_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_WD_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = arad_itm_cr_wd_verify(
    unit,
    core_id,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_cr_wd_set_unsafe(
    unit,
    core_id,
    info,
    exact_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_wd_set()",0,0);
}


uint32
  arad_itm_cr_wd_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_WD_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_cr_wd_get_unsafe(
    unit,
    core_id,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_wd_get()",0,0);
}



uint32
  arad_itm_enable_ecn(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled 
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_enable_ecn_unsafe(unit, enabled);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_enable_ecn()", unit, enabled);
}


uint32
  arad_itm_get_ecn_enabled(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled 
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(enabled);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_get_ecn_enabled_unsafe(unit, enabled);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_get_ecn_enabled()", unit, 0);
}


int
  arad_itm_vsq_qt_rt_cls_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  )
{
  uint32
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_itm_vsq_qt_rt_cls_verify(
    unit,
    core_id,
    is_ocb_only,
    vsq_group_ndx,
    vsq_in_group_ndx,
    vsq_rt_cls
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_itm_vsq_qt_rt_cls_set_unsafe(
    unit,
    core_id,
    is_ocb_only,
    vsq_group_ndx,
    vsq_in_group_ndx,
    vsq_rt_cls
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_vsq_qt_rt_cls_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_OUT uint32                 *vsq_rt_cls
  )
{
  uint32
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(vsq_rt_cls);

  res = arad_itm_vsq_qt_rt_cls_get_unsafe(
    unit,
    core_id,
    is_ocb_only,
    vsq_group_ndx,
    vsq_in_group_ndx,
    vsq_rt_cls
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_vsq_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *exact_info
  )
{
    uint32
        res;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    res = arad_itm_vsq_fc_verify(
       unit,
       vsq_group_ndx,
       vsq_rt_cls_ndx,
       info
       );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_itm_vsq_fc_set_unsafe(
       unit,
       vsq_group_ndx,
       vsq_rt_cls_ndx,
       info,
       exact_info
       );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  arad_itm_vsq_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *info
  )
{
    uint32
        res;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    res = arad_itm_vsq_fc_get_unsafe(
        unit,
        vsq_group_ndx,
        vsq_rt_cls_ndx,
        info
        );
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  arad_itm_vsq_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  int                            is_headroom,
    SOC_SAND_IN  ARAD_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT  ARAD_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);
  SOCDNX_NULL_CHECK(exact_info);

  res = arad_itm_vsq_tail_drop_verify(
    unit,
    vsq_group_ndx,
    vsq_rt_cls_ndx,
    drop_precedence_ndx,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_itm_vsq_tail_drop_set_unsafe(
    unit,
    vsq_group_ndx,
    vsq_rt_cls_ndx,
    drop_precedence_ndx,
    info,
    exact_info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  arad_itm_vsq_tail_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  int                            is_headroom,
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
  uint32
    res;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);


  res = arad_itm_vsq_tail_drop_get_unsafe(
    unit,
    vsq_group_ndx,
    vsq_rt_cls_ndx,
    drop_precedence_ndx,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  arad_itm_vsq_tail_drop_default_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
  uint32
    res;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);

  
  res = arad_itm_vsq_tail_drop_get_default_unsafe(unit, info);
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}



int
  arad_itm_vsq_wred_gen_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_IN  ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
    uint32
        res;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info); 
    if (SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    res = arad_itm_vsq_wred_gen_verify(
       unit,
       vsq_group_ndx,
       vsq_rt_cls_ndx,
       info
       );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_itm_vsq_wred_gen_set_unsafe(
       unit,
       vsq_group_ndx,
       vsq_rt_cls_ndx,
       info
       );
    SOCDNX_SAND_IF_ERR_EXIT(res);
exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  arad_itm_vsq_wred_gen_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_OUT ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
    uint32
        res;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info); 
    if (SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = arad_itm_vsq_wred_gen_get_unsafe(
       unit,
       vsq_group_ndx,
       vsq_rt_cls_ndx,
       info
       );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  arad_itm_vsq_wred_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  )
{
    uint32
        res;
    SOCDNX_INIT_FUNC_DEFS; 

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    res = arad_itm_vsq_wred_verify(
      unit,
      vsq_group_ndx,
      vsq_rt_cls_ndx,
      drop_precedence_ndx,
      info
    );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_itm_vsq_wred_set_unsafe(
      unit,
      vsq_group_ndx,
      vsq_rt_cls_ndx,
      drop_precedence_ndx,
      info,
      exact_info
    );
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN; 
}


int
  arad_itm_vsq_wred_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  uint32
    res;
  SOCDNX_INIT_FUNC_DEFS; 
  SOCDNX_NULL_CHECK(info);

  res = arad_itm_vsq_wred_get_unsafe(
     unit,
     vsq_group_ndx,
     vsq_rt_cls_ndx,
     drop_precedence_ndx,
     info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN; 
}


uint32
  arad_itm_vsq_counter_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_cob_only,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = arad_itm_vsq_counter_verify(
    unit,
    core_id,
    is_cob_only,
    vsq_group_ndx,
    vsq_in_group_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_vsq_counter_set_unsafe(
    unit,
    core_id,
    is_cob_only,
    vsq_group_ndx,
    vsq_in_group_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_counter_set()",0,0);
}



uint32
  arad_itm_vsq_counter_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT  uint8                    *is_cob_only,
    SOC_SAND_OUT ARAD_ITM_VSQ_GROUP       *vsq_group_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_NDX         *vsq_in_group_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vsq_group_ndx);
  SOC_SAND_CHECK_NULL_INPUT(vsq_in_group_ndx);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_vsq_counter_get_unsafe(
    unit,
    core_id,
    is_cob_only,
    vsq_group_ndx,
    vsq_in_group_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_counter_get()",0,0);
}



uint32
  arad_itm_vsq_counter_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT uint32                  *pckt_count
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_READ);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pckt_count);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_vsq_counter_read_unsafe(
    unit,
    core_id,
    pckt_count
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_counter_read()",0,0);
}


uint32
  arad_itm_queue_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *old_info,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(old_info);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_itm_queue_info_verify(
    unit,
    core,
    queue_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_queue_info_set_unsafe(
    unit,
    core,
    queue_ndx,
    old_info,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_info_set()",0,0);
}


uint32
  arad_itm_queue_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT ARAD_ITM_QUEUE_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_queue_info_get_unsafe(
    unit,
    core,
    queue_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_info_get()",0,0);
}


int
  arad_itm_queue_dyn_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT ARAD_ITM_QUEUE_DYN_INFO      *info
  )
{
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);


  SOCDNX_SAND_IF_ERR_EXIT(arad_itm_queue_dyn_info_get_unsafe(
                                  unit,
                                  core,
                                  queue_ndx,
                                  info)
          );
exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_dyn_total_thresh_set(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  int   core_id,
    SOC_SAND_IN  uint8 is_ocb_only,
                 int32 reservation_increase[SOC_DPP_DEFS_MAX(NOF_CORES)][SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] 
  )
{
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(reservation_increase);
  SOCDNX_SAND_IF_ERR_EXIT(arad_itm_dyn_total_thresh_set_unsafe(unit, *reservation_increase[0]));

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_itm_ingress_shape_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    res;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  res = arad_itm_ingress_shape_verify(
    unit,
    info
  );
  SOCDNX_IF_ERR_EXIT(res);

  res = arad_itm_ingress_shape_set_unsafe(
    unit,
    info
  );
  SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_itm_ingress_shape_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    res;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);

  res = arad_itm_ingress_shape_get_unsafe(
    unit,
    info
  );
  SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_itm_priority_map_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_itm_priority_map_tmplt_verify(
    unit,
    map_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_priority_map_tmplt_set_unsafe(
    unit,
    map_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_set()",0,0);
}


uint32
  arad_itm_priority_map_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_OUT ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_priority_map_tmplt_get_unsafe(
    unit,
    map_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_get()",0,0);
}


uint32
  arad_itm_priority_map_tmplt_select_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = arad_itm_priority_map_tmplt_select_verify(
    unit,
    queue_64_ndx,
    priority_map
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_priority_map_tmplt_select_set_unsafe(
    unit,
    queue_64_ndx,
    priority_map
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_select_set()",0,0);
}


uint32
  arad_itm_priority_map_tmplt_select_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_OUT uint32                 *priority_map
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(priority_map);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_priority_map_tmplt_select_get_unsafe(
    unit,
    queue_64_ndx,
    priority_map
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_select_get()",0,0);
}


uint32
  arad_itm_sys_red_drop_prob_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_DROP_PROB_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_itm_sys_red_drop_prob_verify(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_sys_red_drop_prob_set_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_drop_prob_set()",0,0);
}


uint32
  arad_itm_sys_red_drop_prob_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_DROP_PROB_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_sys_red_drop_prob_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_drop_prob_get()",0,0);
}


int
  arad_itm_sys_red_queue_size_boundaries_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);
  SOCDNX_NULL_CHECK(exact_info);

  res = arad_itm_sys_red_queue_size_boundaries_verify(
    unit,
    rt_cls_ndx,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_itm_sys_red_queue_size_boundaries_set_unsafe(
    unit,
    rt_cls_ndx,
    info,
    exact_info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_sys_red_queue_size_boundaries_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  res = arad_itm_sys_red_queue_size_boundaries_get_unsafe(
    unit,
    rt_cls_ndx,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_sys_red_q_based_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  uint32
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  res = arad_itm_sys_red_q_based_verify(
    unit,
    rt_cls_ndx,
    sys_red_dp_ndx,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_itm_sys_red_q_based_set_unsafe(
    unit,
    rt_cls_ndx,
    sys_red_dp_ndx,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_sys_red_q_based_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  uint32
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  res = arad_itm_sys_red_q_based_get_unsafe(
    unit,
    rt_cls_ndx,
    sys_red_dp_ndx,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_itm_sys_red_eg_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_EG_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *exact_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_EG_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = arad_itm_sys_red_eg_verify(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_sys_red_eg_set_unsafe(
    unit,
    info,
    exact_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_eg_set()",0,0);
}


uint32
  arad_itm_sys_red_eg_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_EG_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_itm_sys_red_eg_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_eg_get()",0,0);
}


int
  arad_itm_sys_red_glob_rcs_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  res = arad_itm_sys_red_glob_rcs_verify(
    unit,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);


  res = arad_itm_sys_red_glob_rcs_set_unsafe(
    unit,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_itm_sys_red_glob_rcs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  res = arad_itm_sys_red_glob_rcs_get_unsafe(
    unit,
    info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_itm_vsq_index_group2global(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP vsq_group,
    SOC_SAND_IN  uint32            vsq_in_group_ndx,
    SOC_SAND_IN  uint8             is_ocb_only,
    SOC_SAND_OUT ARAD_ITM_VSQ_NDX   *vsq_ndx
  )
{
  ARAD_ITM_VSQ_NDX
    vsq_ndx_tmp = 0;
  uint32
    res;
  uint32
      vsq_in_group_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_CONVERT_GROUP_INDX_TO_GLOBAL_INDX);

  SOC_SAND_CHECK_NULL_INPUT(vsq_ndx);

  switch(vsq_group)
  {
  case ARAD_ITM_VSQ_GROUP_PFC:
    vsq_ndx_tmp += ARAD_ITM_VSQ_GROUP_LLFC_SIZE;
  case ARAD_ITM_VSQ_GROUP_LLFC:
    vsq_ndx_tmp += ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_SIZE;
  case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
    if (vsq_group == ARAD_ITM_VSQ_GROUP_STTSTCS_TAG && vsq_in_group_ndx > SOC_DPP_CONFIG(unit)->pdm_extension.max_st_vsq) {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_VSQ_INDEX_OUT_OF_RANGE, 5, exit);
    }
    vsq_ndx_tmp += ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_SIZE;
  case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
    vsq_ndx_tmp += ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_SIZE;
  case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
    vsq_ndx_tmp += ARAD_ITM_VSQ_GROUP_CTGRY_SIZE;
  case ARAD_ITM_VSQ_GROUP_CTGRY:
      vsq_ndx_tmp += 0;
  default:
      break;
  }
  vsq_ndx_tmp *= (1 + SOC_DPP_DEFS_GET(unit, vsq_ocb_only_support)); 
  if ((vsq_group != ARAD_ITM_VSQ_GROUP_PG) && (is_ocb_only)) {
      
      res = arad_itm_vsq_in_group_size_get(unit, vsq_group, &vsq_in_group_size);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      vsq_ndx_tmp += vsq_in_group_size;
  }
  vsq_ndx_tmp += vsq_in_group_ndx; 
  *vsq_ndx = vsq_ndx_tmp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_convert_group_indx_to_global_indx()",0,0);
}



uint32
  arad_itm_vsq_index_global2group(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX   vsq_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_GROUP *vsq_group,
    SOC_SAND_OUT uint32            *vsq_in_group_ndx,
    SOC_SAND_OUT uint8             *is_ocb_only
  )
{
  int 
      vsq_is_ocb_max = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_CONVERT_GLOBAL_INDX_TO_CTGRY_INDX);

  SOC_SAND_CHECK_NULL_INPUT(vsq_group);
  SOC_SAND_CHECK_NULL_INPUT(vsq_in_group_ndx);

  vsq_is_ocb_max = (1 + SOC_DPP_DEFS_GET(unit, vsq_ocb_only_support)); 

  if (vsq_ndx < ARAD_ITM_VSQ_GROUP_CTGRY_OFFSET * vsq_is_ocb_max)
  {
    *vsq_group = ARAD_ITM_VSQ_GROUP_CTGRY;
    *vsq_in_group_ndx = vsq_ndx % ARAD_ITM_VSQ_GROUP_CTGRY_SIZE;
    *is_ocb_only =      vsq_ndx / ARAD_ITM_VSQ_GROUP_CTGRY_SIZE ? TRUE : FALSE;
  }
  else if (vsq_ndx < ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_OFFSET * vsq_is_ocb_max)
  {
    *vsq_group = ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS;
    *vsq_in_group_ndx = (vsq_ndx - (ARAD_ITM_VSQ_GROUP_CTGRY_OFFSET * vsq_is_ocb_max)) % ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_SIZE;
    *is_ocb_only =      (vsq_ndx - (ARAD_ITM_VSQ_GROUP_CTGRY_OFFSET * vsq_is_ocb_max)) / ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_SIZE ? TRUE : FALSE;
  }
  else if (vsq_ndx < ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_OFFSET * vsq_is_ocb_max)
  {
    *vsq_group = ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS;
    *vsq_in_group_ndx = (vsq_ndx - (ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_OFFSET * vsq_is_ocb_max)) % ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_SIZE;
    *is_ocb_only =      (vsq_ndx - (ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_OFFSET * vsq_is_ocb_max)) / ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_SIZE ? TRUE : FALSE;
  }
  else if(vsq_ndx < ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_OFFSET * vsq_is_ocb_max)
  {
    *vsq_group = ARAD_ITM_VSQ_GROUP_STTSTCS_TAG;
    *vsq_in_group_ndx = (vsq_ndx - (ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_OFFSET * vsq_is_ocb_max)) % ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_SIZE;
    *is_ocb_only =      (vsq_ndx - (ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_OFFSET * vsq_is_ocb_max)) / ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_SIZE ? TRUE : FALSE;
    if (*vsq_in_group_ndx > SOC_DPP_CONFIG(unit)->pdm_extension.max_st_vsq) {
        SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_VSQ_INDEX_OUT_OF_RANGE, 5, exit);
    }
  }
  else if(vsq_ndx < ARAD_ITM_VSQ_GROUP_LLFC_OFFSET * vsq_is_ocb_max)
  {
    *vsq_group = ARAD_ITM_VSQ_GROUP_LLFC;
    *vsq_in_group_ndx = (vsq_ndx - (ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_OFFSET * vsq_is_ocb_max)) % ARAD_ITM_VSQ_GROUP_LLFC_SIZE;
    *is_ocb_only =      (vsq_ndx - (ARAD_ITM_VSQ_GROUP_STTSTCS_TAG_OFFSET * vsq_is_ocb_max)) / ARAD_ITM_VSQ_GROUP_LLFC_SIZE ? TRUE : FALSE;
  }
  else if(vsq_ndx < ARAD_ITM_VSQ_GROUP_PFC_OFFSET * vsq_is_ocb_max)
  {
    *vsq_group = ARAD_ITM_VSQ_GROUP_PFC;
    *vsq_in_group_ndx = (vsq_ndx - (ARAD_ITM_VSQ_GROUP_LLFC_OFFSET * vsq_is_ocb_max)) % ARAD_ITM_VSQ_GROUP_PFC_SIZE;
    *is_ocb_only =      (vsq_ndx - (ARAD_ITM_VSQ_GROUP_LLFC_OFFSET * vsq_is_ocb_max)) / ARAD_ITM_VSQ_GROUP_PFC_SIZE ? TRUE : FALSE;
  }
  else
  {
    
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_VSQ_INDEX_OUT_OF_RANGE, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_convert_global_indx_to_group_indx()",0,0);
}

void
  arad_ARAD_ITM_DRAM_BUFFERS_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_DRAM_BUFFERS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_DRAM_BUFFERS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_GLOB_RCS_FC_TYPE_clear(
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_FC_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_GLOB_RCS_FC_TH_clear(
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_FC_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_GLOB_RCS_FC_TH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_GLOB_RCS_DROP_TH_clear(
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_GLOB_RCS_DROP_TH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_QUEUE_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_QUEUE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_QUEUE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CATEGORY_RNGS_clear(
    SOC_SAND_OUT ARAD_ITM_CATEGORY_RNGS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CATEGORY_RNGS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_ADMIT_ONE_TEST_TMPLT_clear(
    SOC_SAND_OUT ARAD_ITM_ADMIT_ONE_TEST_TMPLT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_ADMIT_TEST_TMPLT_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_HUNGRY_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_HUNGRY_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_BACKOFF_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_BACKOFF_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_BACKLOG_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_BACKLOG_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_EMPTY_Q_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_EMPTY_Q_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_SATISFIED_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_SATISFIED_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_WD_Q_TH_clear(
    SOC_SAND_OUT ARAD_ITM_CR_WD_Q_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_WD_Q_TH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_DISCOUNT_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_DISCOUNT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_WRED_QT_DP_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_WRED_QT_DP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_TAIL_DROP_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_TAIL_DROP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_WD_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_WD_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_VSQ_FC_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_VSQ_FC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_VSQ_TAIL_DROP_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_VSQ_WRED_GEN_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_VSQ_WRED_GEN_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_INGRESS_SHAPE_Q_RANGE_clear(
    SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_Q_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_INGRESS_SHAPE_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_INGRESS_SHAPE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_PRIORITY_MAP_TMPLT_clear(
    SOC_SAND_OUT ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_PRIORITY_MAP_TMPLT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_DROP_PROB_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_DROP_PROB_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_QT_DP_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_QT_DP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_QT_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_QT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_EG_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_EG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_THS_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_THS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_VALS_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_VALS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_ITM_QT_NDX_to_string(
    SOC_SAND_IN  ARAD_ITM_QT_NDX enum_val
  )
{
  return SOC_TMC_ITM_QT_NDX_to_string(enum_val);
}

const char*
  arad_ARAD_ITM_CR_DISCNT_CLS_NDX_to_string(
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX enum_val
  )
{
  return SOC_TMC_ITM_CR_DISCNT_CLS_NDX_to_string(enum_val);
}

const char*
  arad_ARAD_ITM_DBUFF_SIZE_BYTES_to_string(
    SOC_SAND_IN  ARAD_ITM_DBUFF_SIZE_BYTES enum_val
  )
{
  return SOC_TMC_ITM_DBUFF_SIZE_BYTES_to_string(enum_val);
}

const char*
  arad_ARAD_ITM_ADMIT_TSTS_to_string(
    SOC_SAND_IN  ARAD_ITM_ADMIT_TSTS enum_val
  )
{
  return SOC_TMC_ITM_ADMIT_TSTS_to_string(enum_val);
}

const char*
  arad_ARAD_ITM_VSQ_GROUP_to_string(
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP enum_val
  )
{
  return SOC_TMC_ITM_VSQ_GROUP_to_string(enum_val);
}

const char*
  arad_ARAD_ITM_STAG_ENABLE_MODE_to_string(
    SOC_SAND_IN ARAD_ITM_STAG_ENABLE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_ITM_STAG_ENABLE_MODE_DISABLED:
    str = "DISABLED";
  break;

  case ARAD_ITM_STAG_ENABLE_MODE_STAT_IF_NO_DEQ:
    str = "STAT_IF_NO_DEQ";
  break;

  case ARAD_ITM_STAG_ENABLE_MODE_ENABLED_WITH_DEQ:
    str = "ENABLED_WITH_DEQ";
  break;

  case ARAD_ITM_NOF_STAG_ENABLE_MODES:
    str = "NOF_ENABLE_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  arad_ARAD_ITM_DRAM_BUFFERS_INFO_print(
    SOC_SAND_IN  ARAD_ITM_DRAM_BUFFERS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_DRAM_BUFFERS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_STAG_INFO_clear(
    SOC_SAND_OUT ARAD_ITM_STAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_ITM_STAG_INFO));
  info->enable_mode = ARAD_ITM_STAG_ENABLE_MODE_DISABLED;
  info->vsq_index_msb = 0;
  info->vsq_index_lsb = 0;
  info->dropp_en = 0;
  info->dropp_lsb = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_GLOB_RCS_FC_TYPE_print(
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_FC_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_GLOB_RCS_FC_TYPE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_GLOB_RCS_FC_TH_print(
    SOC_SAND_IN ARAD_ITM_GLOB_RCS_FC_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_GLOB_RCS_FC_TH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  arad_ARAD_ITM_GLOB_RCS_DROP_TH_print_no_table(
    SOC_SAND_IN ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<ARAD_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Bdbs[%u]: "),ind));
    arad_ARAD_THRESH_WITH_HYST_INFO_print(&(info->bdbs[ind]));
  }
  for (ind=0; ind<ARAD_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Bds[%u]: "),ind));
    arad_ARAD_THRESH_WITH_HYST_INFO_print(&(info->bds[ind]));
  }
  for (ind=0; ind<ARAD_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Unicast[%u]: "),ind));
    arad_ARAD_THRESH_WITH_HYST_INFO_print(&(info->unicast[ind]));
  }
  for (ind=0; ind<ARAD_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Full_mc[%u]: "),ind));
    arad_ARAD_THRESH_WITH_HYST_INFO_print(&(info->full_mc[ind]));
  }
  for (ind=0; ind<ARAD_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Mini_mc[%u]: "),ind));
    arad_ARAD_THRESH_WITH_HYST_INFO_print(&(info->mini_mc[ind]));
  }
  for (ind=0; ind<ARAD_NOF_DROP_PRECEDENCE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Mem_excess[%u]: "),ind));
    arad_ARAD_THRESH_WITH_HYST_INFO_print(&(info->mem_excess[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
   arad_ARAD_ITM_GLOB_RCS_DROP_TH_print(
    SOC_SAND_IN ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_GLOB_RCS_DROP_TH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_QUEUE_INFO_print(
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_QUEUE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CATEGORY_RNGS_print(
    SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CATEGORY_RNGS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_ADMIT_ONE_TEST_TMPLT_print(
    SOC_SAND_IN  ARAD_ITM_ADMIT_ONE_TEST_TMPLT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_ADMIT_ONE_TEST_TMPLT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_ADMIT_TEST_TMPLT_INFO_print(
    SOC_SAND_IN  ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_HUNGRY_TH_print(
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_HUNGRY_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_HUNGRY_TH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_BACKOFF_TH_print(
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_BACKOFF_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_BACKOFF_TH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_BACKLOG_TH_print(
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_BACKLOG_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_BACKLOG_TH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_EMPTY_Q_TH_print(
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_EMPTY_Q_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_EMPTY_Q_TH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_SATISFIED_TH_print(
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_SATISFIED_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_SATISFIED_TH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_WD_Q_TH_print(
    SOC_SAND_IN  ARAD_ITM_CR_WD_Q_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_WD_Q_TH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_REQUEST_INFO_print(
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_REQUEST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_DISCOUNT_INFO_print(
    SOC_SAND_IN  ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_DISCOUNT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_WRED_QT_DP_INFO_print(
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_WRED_QT_DP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_TAIL_DROP_INFO_print(
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_TAIL_DROP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_CR_WD_INFO_print(
    SOC_SAND_IN  ARAD_ITM_CR_WD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_CR_WD_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_VSQ_FC_INFO_print(
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_VSQ_FC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_VSQ_TAIL_DROP_INFO_print(
    SOC_SAND_IN  ARAD_ITM_VSQ_TAIL_DROP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_VSQ_WRED_GEN_INFO_print(
    SOC_SAND_IN  ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_VSQ_WRED_GEN_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_STAG_INFO_print(
    SOC_SAND_IN ARAD_ITM_STAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable_mode %s \n\r"),
           arad_ARAD_ITM_STAG_ENABLE_MODE_to_string(info->enable_mode)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "  Vsq_index_msb: %u\n\r"),info->vsq_index_msb));
  LOG_CLI((BSL_META_U(unit,
                      "  Vsq_index_lsb: %u\n\r"),info->vsq_index_lsb));
  LOG_CLI((BSL_META_U(unit,
                      "  Dropp_en: %u\n\r"),info->dropp_en));
  LOG_CLI((BSL_META_U(unit,
                      "  Dropp_lsb: %u\n\r"),info->dropp_lsb));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  arad_ARAD_ITM_INGRESS_SHAPE_Q_RANGE_print(
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_Q_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_INGRESS_SHAPE_Q_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_INGRESS_SHAPE_INFO_print(
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_INGRESS_SHAPE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_PRIORITY_MAP_TMPLT_print(
    SOC_SAND_IN  ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_PRIORITY_MAP_TMPLT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_DROP_PROB_print(
    SOC_SAND_IN  ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_DROP_PROB_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_QT_DP_INFO_print(
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_QT_DP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_QT_INFO_print(
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_QT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_EG_INFO_print(
    SOC_SAND_IN  ARAD_ITM_SYS_RED_EG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_EG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_THS_print(
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_THS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_GLOB_RCS_THS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_VALS_print(
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_VALS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_GLOB_RCS_VALS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ITM_SYS_RED_GLOB_RCS_INFO_print(
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ITM_SYS_RED_GLOB_RCS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#endif 


int
  arad_itm_committed_q_size_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    SOCDNX_SAND_IF_ERR_EXIT( arad_itm_committed_q_size_set_verify(
                                 unit,
                                 rt_cls_ndx,
                                 info
                                 ));
                             
    SOCDNX_SAND_IF_ERR_EXIT( arad_itm_committed_q_size_set_unsafe(
                                 unit,
                                 rt_cls_ndx,
                                 info,
                                 exact_info
                            ));

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_itm_committed_q_size_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(exact_info);

    SOCDNX_SAND_IF_ERR_EXIT( arad_itm_committed_q_size_get_verify(
                                 unit,
                                 rt_cls_ndx
                                 ));
                             
    SOCDNX_SAND_IF_ERR_EXIT( arad_itm_committed_q_size_get_unsafe(
                                 unit,
                                 rt_cls_ndx,
                                 exact_info
                                 ));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_itm_pfc_tc_map_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN int32                    tc_in,
    SOC_SAND_IN int32                    port_id,
    SOC_SAND_IN int32                    tc_out
  )
{
  soc_error_t
    rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PFC_TC_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  rv = arad_itm_pfc_tc_map_set_unsafe(
          unit,
          tc_in,
          port_id,
          tc_out
        );
  if(SOC_FAILURE(rv)) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit_semaphore);
  }

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_pfc_tc_map_set()", 0, 0);
}

uint32
  arad_itm_pfc_tc_map_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int32                   tc_in,
    SOC_SAND_IN  int32                   port_id,
    SOC_SAND_OUT int32                   *tc_out
  )
{
  soc_error_t
    rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PFC_TC_MAP_GET);

  SOC_SAND_CHECK_NULL_INPUT(tc_out);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  rv = arad_itm_pfc_tc_map_get_unsafe(
          unit,
          tc_in,
          port_id,
          tc_out
        );
  if(SOC_FAILURE(rv)) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit_semaphore);
  }

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_pfc_tc_map_get()", 0, 0);
}

int
  arad_itm_dp_discard_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN  uint32                  discard_dp
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_itm_dp_discard_set_verify(unit, discard_dp));

    SOCDNX_IF_ERR_EXIT(arad_itm_dp_discard_set_unsafe(unit, discard_dp));

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_itm_dp_discard_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(discard_dp);

    SOCDNX_IF_ERR_EXIT(arad_itm_dp_discard_get_unsafe(unit, discard_dp));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_plus_itm_alpha_set(
      SOC_SAND_IN  int       unit,
      SOC_SAND_IN  uint32       rt_cls_ndx,
      SOC_SAND_IN  uint32       drop_precedence_ndx,
      SOC_SAND_IN  int32        alpha 
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_ERR_IF_ABOVE_MAX(rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
      ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
      ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(alpha, ARAD_PLUS_MIN_ALPHA, ARAD_PLUS_MAX_ALPHA,
      SOC_SAND_GEN_ERR, 9, exit);
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    SOC_SAND_CHECK_FUNC_RESULT(arad_plus_itm_alpha_set_unsafe(unit, rt_cls_ndx, drop_precedence_ndx, alpha), 100, exit_semaphore);
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_itm_alpha_set()", rt_cls_ndx, drop_precedence_ndx);
}


uint32
    arad_plus_itm_alpha_get(
      SOC_SAND_IN  int       unit,
      SOC_SAND_IN  uint32       rt_cls_ndx,
      SOC_SAND_IN  uint32       drop_precedence_ndx,
      SOC_SAND_OUT int32        *alpha 
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_ERR_IF_ABOVE_MAX(rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
      ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
      ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit);
    SOC_SAND_CHECK_NULL_INPUT(alpha);
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    SOC_SAND_CHECK_FUNC_RESULT(arad_plus_itm_alpha_get_unsafe(unit, rt_cls_ndx, drop_precedence_ndx, alpha), 100, exit_semaphore);
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_itm_alpha_get()", rt_cls_ndx, drop_precedence_ndx);
}



uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_set(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint8    enabled 
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    SOC_SAND_CHECK_FUNC_RESULT(arad_plus_itm_fair_adaptive_tail_drop_enable_set_unsafe(unit, enabled), 100, exit_semaphore);
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_itm_fair_adaptive_tail_drop_enable_set()", unit, enabled);
}


uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint8    *enabled 
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_CHECK_NULL_INPUT(enabled);
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    SOC_SAND_CHECK_FUNC_RESULT(arad_plus_itm_fair_adaptive_tail_drop_enable_get_unsafe(unit, enabled), 100, exit_semaphore);
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_itm_fair_adaptive_tail_drop_enable_set()", unit, 0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
