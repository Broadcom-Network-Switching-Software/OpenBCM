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

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_multicast_fabric.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/port_sw_db.h>









#define ARAD_MULT_NOF_INGRESS_SHAPINGS             (2)
#define ARAD_MULT_TC_MAPPING_FABRIC_MULT_NO_IS     (16)
#define ARAD_MULT_TC_MAPPING_FABRIC_MULT_WITH_IS   (17)





















uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  ARAD_MULT_FABRIC_CLS     new_mult_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_VERIFY);

  res = arad_traffic_class_verify(tr_cls_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  
  if ((new_mult_cls < ARAD_MULT_FABRIC_CLS_MIN)||
    (new_mult_cls > ARAD_MULT_FABRIC_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_traffic_class_to_multicast_cls_map_verify()",0,0);
}


uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  ARAD_MULT_FABRIC_CLS     new_mult_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_UNSAFE);
  res = arad_ipq_traffic_class_profile_map_set_unsafe(
            unit,
            SOC_CORE_ALL,
            0 ,
            FALSE,
            TRUE ,
            tr_cls_ndx,
            new_mult_cls);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe()",0,0);
}

uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_TR_CLS           tr_cls_ndx,
    SOC_SAND_OUT ARAD_MULT_FABRIC_CLS  *new_mult_cls
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(new_mult_cls);
  res = arad_ipq_traffic_class_profile_map_get_unsafe(
            unit,
            SOC_CORE_ALL,
            0 ,
            FALSE,
            TRUE ,
            tr_cls_ndx,
            new_mult_cls);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe()",0,0);
}

uint32
  arad_mult_fabric_base_queue_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_BASE_QUEUE_VERIFY);

  if (!arad_is_queue_valid(unit, queue_id))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_MULT_FABRIC_ILLEGAL_NUMBER_OF_QUEUE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_base_queue_verify()",0,0);
}


uint32
  arad_mult_fabric_base_queue_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  )
{
  uint32
    res;
  soc_reg_t reg;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_BASE_QUEUE_SET_UNSAFE);

  if (SOC_IS_QAX(unit)) {
      reg = TAR_FABRIC_MC_BASE_QUEUEr;
  } else {
      reg = IRR_FABRIC_MULTICAST_BASE_QUEUEr;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, FABRIC_MC_BASE_QUEUEf,  queue_id));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_base_queue_set_unsafe()",0,0);
}


uint32
  arad_mult_fabric_base_queue_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *queue_id
  )
{
  uint32
    res;
  soc_reg_t reg;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_BASE_QUEUE_GET_UNSAFE);

  if (SOC_IS_QAX(unit)) {
      reg = TAR_FABRIC_MC_BASE_QUEUEr;
  } else {
      reg = IRR_FABRIC_MULTICAST_BASE_QUEUEr;
  }

  SOC_SAND_CHECK_NULL_INPUT(queue_id);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, FABRIC_MC_BASE_QUEUEf, queue_id));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_base_queue_get_unsafe()",0,0);
}


STATIC int
  arad_mult_fabric_credit_source_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_INFO    *info
  )
{
    uint8
        sch_in = FALSE,
        sch_ou = FALSE;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_MAGIC_NUM_VERIFY(info);

    sch_in = TRUE;
    sch_in = sch_in && (info->best_effort.be_sch_port[ARAD_MULTICAST_CLASS_0].be_sch_port.multicast_class_valid);
    sch_in = sch_in && (info->best_effort.be_sch_port[ARAD_MULTICAST_CLASS_1].be_sch_port.multicast_class_valid);
    sch_in = sch_in && (info->best_effort.be_sch_port[ARAD_MULTICAST_CLASS_2].be_sch_port.multicast_class_valid);
    sch_in = sch_in && (info->guaranteed.gr_sch_port.multicast_class_valid);

    sch_ou = TRUE;
    sch_ou = sch_ou && (info->credits_via_sch);

    if (((sch_in) && (!sch_ou)) ||
        ((!sch_in) && (sch_ou)))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("ARAD_MULT_FABRIC_ILLEGAL_CONF_ERR")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}




STATIC
  int
    arad_mult_fabric_credit_source_port_get(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  int                          core,
      SOC_SAND_IN  ARAD_MULTICAST_CLASS         multicast_class_ndx,
      SOC_SAND_OUT ARAD_MULT_FABRIC_PORT_INFO   *info
    )
{
    uint32
        multicast_class_valid = 0,
        tm_port,
        buffer = 0,
        base_port_tc,
        flags;
    soc_reg_t
        mcast_reg;
    uint32 
        nof_prio;
    soc_pbmp_t 
        ports_bm;
    int
        core_i;
    soc_port_t
        port;
    soc_field_t
        mcast_class_port_id_fld[ARAD_NOF_MULTICAST_CLASSES] 
            = {MCAST_GFMC_PORT_IDf, MCAST_BFMC_1_PORT_IDf, MCAST_BFMC_2_PORT_IDf, MCAST_BFMC_3_PORT_IDf},
        multicast_class_valid_fld[ARAD_NOF_MULTICAST_CLASSES] 
            = {MULTICAST_GFMC_ENABLEf, MULTICAST_BFMC_1_ENABLEf, MULTICAST_BFMC_2_ENABLEf, MULTICAST_BFMC_3_ENABLEf};

    SOCDNX_INIT_FUNC_DEFS;

    switch (multicast_class_ndx) {
        case ARAD_MULTICAST_CLASS_0:
        case ARAD_MULTICAST_CLASS_1:
            mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r;
            break;
        case ARAD_MULTICAST_CLASS_2:
        case ARAD_MULTICAST_CLASS_3:
            mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ARAD_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR")));
            break;
    }
  
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, mcast_reg, core,  0, &buffer));

    base_port_tc = soc_reg_field_get(unit, mcast_reg, buffer, mcast_class_port_id_fld[multicast_class_ndx]);

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));

    SOC_PBMP_ITER(ports_bm, port)
    {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core_i));

        if(core != core_i) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags)) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_i, tm_port, &nof_prio));

        if (base_port_tc == nof_prio) {
            info->mcast_class_port_id = tm_port;
        }
    } 

    multicast_class_valid = soc_reg_field_get(unit, mcast_reg, buffer, multicast_class_valid_fld[multicast_class_ndx]);
    info->multicast_class_valid = multicast_class_valid == 1 ? TRUE : FALSE;


exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  int
    arad_mult_fabric_credit_source_port_set(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  int                          core,
      SOC_SAND_IN  ARAD_MULTICAST_CLASS         multicast_class_ndx,
      SOC_SAND_IN  ARAD_MULT_FABRIC_PORT_INFO   *info
    )
{
    uint32
        mcast_class_port_id = info->mcast_class_port_id,
        multicast_class_valid = info->multicast_class_valid == TRUE ? 1 : 0,
        buffer = 0,
        base_port_tc;
    soc_reg_t
        mcast_reg;
    soc_field_t
        mcast_class_port_id_fld[ARAD_NOF_MULTICAST_CLASSES] 
            = {MCAST_GFMC_PORT_IDf, MCAST_BFMC_1_PORT_IDf, MCAST_BFMC_2_PORT_IDf, MCAST_BFMC_3_PORT_IDf},
        multicast_class_valid_fld[ARAD_NOF_MULTICAST_CLASSES] 
            = {MULTICAST_GFMC_ENABLEf, MULTICAST_BFMC_1_ENABLEf, MULTICAST_BFMC_2_ENABLEf, MULTICAST_BFMC_3_ENABLEf};
    int
        rv;

    SOCDNX_INIT_FUNC_DEFS;

    switch (multicast_class_ndx) {
        case ARAD_MULTICAST_CLASS_0:
        case ARAD_MULTICAST_CLASS_1:
            mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r;
            break;
        case ARAD_MULTICAST_CLASS_2:
        case ARAD_MULTICAST_CLASS_3:
            mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("ARAD_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR")));
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, mcast_reg, core, 0, &buffer));

    if(multicast_class_valid) {
        rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, mcast_class_port_id,  &base_port_tc);  
        SOCDNX_IF_ERR_EXIT(rv);

        soc_reg_field_set(unit, mcast_reg, &buffer, mcast_class_port_id_fld[multicast_class_ndx], base_port_tc);
    }

    soc_reg_field_set(unit, mcast_reg, &buffer, multicast_class_valid_fld[multicast_class_ndx], multicast_class_valid);

    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, mcast_reg, core, 0, buffer));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  int
    arad_mult_fabric_credit_source_shaper_set(
      SOC_SAND_IN  int                              unit,
      SOC_SAND_IN  int                              core,
      SOC_SAND_IN  soc_reg_t                        reg_desc,
      SOC_SAND_IN  soc_field_t                      max_burst_f,
      SOC_SAND_IN  soc_field_t                      shaper_rate_f,
      SOC_SAND_IN  ARAD_MULT_FABRIC_SHAPER_INFO     *info
    )
{
    uint32
        res;
    uint32
        rate = 0,
        buffer = 0;
    int 
        rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_reg32_get(unit, reg_desc, core, 0, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_reg_field_set(unit ,reg_desc, &buffer, max_burst_f, info->max_burst);

    res = arad_intern_rate2clock(
          unit,
          info->rate,
          FALSE, 
          &rate
        );
    SOC_SAND_IF_ERR_EXIT(res);

    soc_reg_field_set(unit ,reg_desc, &buffer, shaper_rate_f, rate);

    rv = soc_reg32_set(unit, reg_desc, core, 0, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  int
    arad_mult_fabric_credit_source_shaper_get(
      SOC_SAND_IN  int                              unit,
      SOC_SAND_IN  int                              core,
      SOC_SAND_IN  soc_reg_t                        reg_desc,
      SOC_SAND_IN  soc_field_t                      max_burst_f,
      SOC_SAND_IN  soc_field_t                      shaper_rate_f,
      SOC_SAND_OUT ARAD_MULT_FABRIC_SHAPER_INFO     *info
    )
{
    uint32
        res;
    uint32
        rate = 0,
        buffer = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg_desc, core, 0, &buffer));

    info->max_burst = soc_reg_field_get(unit , reg_desc, buffer, max_burst_f);
    rate = soc_reg_field_get(unit , reg_desc, buffer, shaper_rate_f);

    res = arad_intern_clock2rate(
          unit,
          rate,
          FALSE, 
          &(info->rate)
        );
    SOC_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  int
    arad_mult_fabric_credit_source_be_wfq_set(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  int                          core,
      SOC_SAND_IN  ARAD_MULT_FABRIC_BE_INFO     *info
    )
{
    uint32
        wfq_enable = info->wfq_enable == TRUE ? 0x1 : 0x0,
        buffer = 0;
    int
        rv;

    SOCDNX_INIT_FUNC_DEFS;

    

    rv = READ_IPS_BFMC_CLASS_CONFIGr(unit, core, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_reg_field_set(unit, IPS_BFMC_CLASS_CONFIGr, &buffer, BFMC_WFQ_ENf, wfq_enable);

    if (wfq_enable)
    {
        
        soc_reg_field_set(unit, IPS_BFMC_CLASS_CONFIGr, &buffer, BFMC_CLASS_2_Wf, info->be_sch_port[ARAD_MULTICAST_CLASS_0].weight);
        soc_reg_field_set(unit, IPS_BFMC_CLASS_CONFIGr, &buffer, BFMC_CLASS_1_Wf, info->be_sch_port[ARAD_MULTICAST_CLASS_1].weight);
        soc_reg_field_set(unit, IPS_BFMC_CLASS_CONFIGr, &buffer, BFMC_CLASS_0_Wf, info->be_sch_port[ARAD_MULTICAST_CLASS_2].weight);
    }

    rv = WRITE_IPS_BFMC_CLASS_CONFIGr(unit, core, buffer);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
  int
    arad_mult_fabric_credit_source_be_wfq_get(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  int                           core,
      SOC_SAND_OUT ARAD_MULT_FABRIC_BE_INFO       *info
    )
{
    uint32
        wfq_enable = 0,
        buffer = 0;
    int
        rv;

    SOCDNX_INIT_FUNC_DEFS;

    info->be_sch_port[ARAD_MULTICAST_CLASS_0].weight = 0;
    info->be_sch_port[ARAD_MULTICAST_CLASS_1].weight = 0;
    info->be_sch_port[ARAD_MULTICAST_CLASS_2].weight = 0;

    
    rv = READ_IPS_BFMC_CLASS_CONFIGr(unit, core, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    wfq_enable = soc_reg_field_get(unit, IPS_BFMC_CLASS_CONFIGr, buffer, BFMC_WFQ_ENf);

    info->wfq_enable = (wfq_enable==0x1 ? TRUE : FALSE);

    if (info->wfq_enable)
    {
        
        info->be_sch_port[ARAD_MULTICAST_CLASS_0].weight = soc_reg_field_get(unit, IPS_BFMC_CLASS_CONFIGr, buffer, BFMC_CLASS_2_Wf);
        info->be_sch_port[ARAD_MULTICAST_CLASS_1].weight = soc_reg_field_get(unit, IPS_BFMC_CLASS_CONFIGr, buffer, BFMC_CLASS_1_Wf);
        info->be_sch_port[ARAD_MULTICAST_CLASS_2].weight = soc_reg_field_get(unit, IPS_BFMC_CLASS_CONFIGr, buffer, BFMC_CLASS_0_Wf);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  int
    arad_mult_fabric_credit_source_be_set(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  int                          core,
      SOC_SAND_IN  ARAD_MULT_FABRIC_BE_INFO    *info
    )
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = arad_mult_fabric_credit_source_be_wfq_set(
          unit,
          core,
          info
        );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = arad_mult_fabric_credit_source_shaper_set(
          unit,
          core,
          IPS_BFMC_SHAPER_CONFIGr,
          BFMC_MAX_BURSTf,
          BFMC_MAX_CR_RATEf,
          &(info->be_shaper)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = arad_mult_fabric_credit_source_port_set(
          unit,
          core,
          ARAD_MULTICAST_CLASS_3,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_0].be_sch_port)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_mult_fabric_credit_source_port_set(
          unit,
          core,
          ARAD_MULTICAST_CLASS_2,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_1].be_sch_port)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_mult_fabric_credit_source_port_set(
          unit,
          core,
          ARAD_MULTICAST_CLASS_1,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_2].be_sch_port)
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  int
    arad_mult_fabric_credit_source_be_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  int                       core,
      SOC_SAND_OUT ARAD_MULT_FABRIC_BE_INFO *info
    )
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = arad_mult_fabric_credit_source_be_wfq_get(
          unit,
          core,
          info
        );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = arad_mult_fabric_credit_source_shaper_get(
          unit,
          core,
          IPS_BFMC_SHAPER_CONFIGr,
          BFMC_MAX_BURSTf,
          BFMC_MAX_CR_RATEf,
          &(info->be_shaper)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = arad_mult_fabric_credit_source_port_get(
          unit,
          core,
          ARAD_MULTICAST_CLASS_3,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_0].be_sch_port)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_mult_fabric_credit_source_port_get(
          unit,
          core,
          ARAD_MULTICAST_CLASS_2,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_1].be_sch_port)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_mult_fabric_credit_source_port_get(
          unit,
          core,
          ARAD_MULTICAST_CLASS_1,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_2].be_sch_port)
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  int
    arad_mult_fabric_credit_source_gu_set(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  int                       core,
      SOC_SAND_IN  ARAD_MULT_FABRIC_GR_INFO *info
    )
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = arad_mult_fabric_credit_source_shaper_set(
          unit,
          core,
          IPS_GFMC_SHAPER_CONFIGr,
          GFMC_MAX_BURSTf,
          GFMC_MAX_CR_RATEf,
          &(info->gr_shaper)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = arad_mult_fabric_credit_source_port_set(
          unit,
          core,
          ARAD_MULTICAST_CLASS_0,
          &(info->gr_sch_port)
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  int
    arad_mult_fabric_credit_source_gu_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  int                       core,
      SOC_SAND_OUT ARAD_MULT_FABRIC_GR_INFO *info
    )
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = arad_mult_fabric_credit_source_shaper_get(
          unit,
          core,
          IPS_GFMC_SHAPER_CONFIGr,
          GFMC_MAX_BURSTf,
          GFMC_MAX_CR_RATEf,
          &(info->gr_shaper)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = arad_mult_fabric_credit_source_port_get(
          unit,
          core,
          ARAD_MULTICAST_CLASS_0,
          &(info->gr_sch_port)
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}




int
  arad_mult_fabric_credit_source_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  ARAD_MULT_FABRIC_INFO      *info
  )
{
    int
        rv;
    uint32
        buffer,
        rate = 0,
        res;
   
    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_mult_fabric_credit_source_verify(unit, info);
    SOCDNX_IF_ERR_EXIT(rv);

    res = arad_intern_rate2clock(
          unit,
          info->max_rate,
          FALSE, 
          &rate
        );
    SOC_SAND_IF_ERR_EXIT(res);

    if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, core, FMC_CREDITS_FROM_SCHf,  info->credits_via_sch));
    } else {
        SOCDNX_IF_ERR_EXIT(READ_IPS_IPS_GENERAL_CONFIGURATIONSr_REG32(unit, core, &buffer));
        soc_reg_field_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &buffer, FMC_CREDITS_FROM_SCHf, info->credits_via_sch);
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_IPS_GENERAL_CONFIGURATIONSr_REG32(unit, core, buffer));
    }


    SOCDNX_IF_ERR_EXIT(READ_IPS_FMC_SHAPER_CONFIGr(unit, core ,&buffer));
    soc_reg_field_set(unit, IPS_FMC_SHAPER_CONFIGr, &buffer, FMC_MAX_CR_RATEf, rate);
    soc_reg_field_set(unit, IPS_FMC_SHAPER_CONFIGr, &buffer, FMC_MAX_BURSTf, info->max_burst);
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_FMC_SHAPER_CONFIGr(unit, core ,buffer));

    rv = arad_mult_fabric_credit_source_be_set(
          unit,
          core,
          &(info->best_effort)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_mult_fabric_credit_source_gu_set(
          unit,
          core,
          &(info->guaranteed)
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_mult_fabric_credit_source_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_OUT ARAD_MULT_FABRIC_INFO      *info
  )
{
    int
        rv;
    uint32
        buffer,
        credits_via_sch = 0,
        rate = 0,
        res;
   
    SOCDNX_INIT_FUNC_DEFS;

    SOC_TMC_MULT_FABRIC_INFO_clear(info);

    rv = arad_mult_fabric_credit_source_gu_get(
          unit,
          core,
          &(info->guaranteed)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_mult_fabric_credit_source_be_get(
          unit,
          core,
          &(info->best_effort)
        );
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_IPS_FMC_SHAPER_CONFIGr(unit, core ,&buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    rate = soc_reg_field_get(unit, IPS_FMC_SHAPER_CONFIGr, buffer, FMC_MAX_CR_RATEf);

    res = arad_intern_clock2rate(
          unit,
          rate,
          FALSE, 
          &(info->max_rate)
        );
    SOC_SAND_IF_ERR_EXIT(res);

    info->max_burst = soc_reg_field_get(unit, IPS_FMC_SHAPER_CONFIGr, buffer, FMC_MAX_BURSTf);

    if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, core, FMC_CREDITS_FROM_SCHf, &credits_via_sch));
    } else {
        rv = READ_IPS_IPS_GENERAL_CONFIGURATIONSr_REG32(unit, core, &buffer);
        SOCDNX_IF_ERR_EXIT(rv);

        credits_via_sch = soc_reg_field_get(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, buffer, FMC_CREDITS_FROM_SCHf);
    }
    info->credits_via_sch = (credits_via_sch ? TRUE : FALSE);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_mult_fabric_enhanced_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  )
{
  int
    res;
  uint32
    reg_val = 0; 
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(queue_range);

  res = READ_ECI_GLOBAL_17r(unit, &reg_val);
  SOCDNX_IF_ERR_EXIT(res);

  soc_reg_field_set(unit, ECI_GLOBAL_17r, &reg_val, FMC_QNUM_LOWf, queue_range->start);

  res = WRITE_ECI_GLOBAL_17r(unit, reg_val);
  SOCDNX_IF_ERR_EXIT(res);

  res = READ_ECI_GLOBAL_18r(unit, &reg_val);
  SOCDNX_IF_ERR_EXIT(res);

  soc_reg_field_set(unit, ECI_GLOBAL_18r, &reg_val, FMC_QNUM_HIGHf, queue_range->end);

  res = WRITE_ECI_GLOBAL_18r(unit, reg_val);
  SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_mult_fabric_enhanced_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  )
{
  SOCDNX_INIT_FUNC_DEFS;
  if (queue_range->start > (SOC_DPP_DEFS_GET(unit, nof_queues) - 1)) {
    LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Queue start %d out of range\n"), queue_range->start));
    SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (queue_range->end > (SOC_DPP_DEFS_GET(unit, nof_queues) - 1)) {
    LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Queue end %d out of range\n"), queue_range->end));
    SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }

  if (queue_range->start > queue_range->end) {
      LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Queue start %d is higher than Queue end %d\n"), queue_range->start, queue_range->end));
      SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_mult_fabric_enhanced_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE                  *queue_range
  )
{
  int
    res;
  uint32
    reg_val = 0;
  
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(queue_range);
 
  res = READ_ECI_GLOBAL_17r(unit, &reg_val);
  SOCDNX_IF_ERR_EXIT(res);

  queue_range->start = soc_reg_field_get(unit, ECI_GLOBAL_17r, reg_val, FMC_QNUM_LOWf);

  res = READ_ECI_GLOBAL_18r(unit, &reg_val);
  SOCDNX_IF_ERR_EXIT(res);

  queue_range->end = soc_reg_field_get(unit, ECI_GLOBAL_18r, reg_val, FMC_QNUM_HIGHf);

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_mult_fabric_flow_control_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(fc_map);
  SOC_SAND_MAGIC_NUM_VERIFY(fc_map);

  if(fc_map->gfmc_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      SOC_SAND_ERR_IF_ABOVE_MAX(fc_map->gfmc_lb_fc_map, 0xf, ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 1, exit);
  }

  if(fc_map->bfmc0_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      SOC_SAND_ERR_IF_ABOVE_MAX(fc_map->bfmc0_lb_fc_map, 0xf, ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 2, exit);
  }

  if(fc_map->bfmc1_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      SOC_SAND_ERR_IF_ABOVE_MAX(fc_map->bfmc1_lb_fc_map, 0xf, ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 3, exit);
  }

  if(fc_map->bfmc2_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      SOC_SAND_ERR_IF_ABOVE_MAX(fc_map->bfmc2_lb_fc_map, 0xf, ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 4, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_flow_control_set_verify()", 0, 0);
}

  
uint32
  arad_mult_fabric_flow_control_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  )
{
  uint32 fld_value, res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(fc_map->gfmc_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      fld_value = fc_map->gfmc_lb_fc_map & 0xf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, GFMC_FC_MAPf,  fld_value));
  }

  if(fc_map->bfmc0_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      fld_value = fc_map->bfmc0_lb_fc_map & 0xf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_0_FC_MAPf,  fld_value));
  }

  if(fc_map->bfmc1_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      fld_value = fc_map->bfmc1_lb_fc_map & 0xf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_1_FC_MAPf,  fld_value));
  }

  if(fc_map->bfmc2_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      fld_value = fc_map->bfmc2_lb_fc_map & 0xf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_2_FC_MAPf,  fld_value));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_flow_control_set_unsafe()",0,0);
}

uint32
  arad_mult_fabric_flow_control_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(fc_map);
  SOC_SAND_MAGIC_NUM_VERIFY(fc_map);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_flow_control_set_verify()", 0, 0);
}

uint32
  arad_mult_fabric_flow_control_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_FLOW_CONTROL_MAP     *fc_map
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, GFMC_FC_MAPf, &fc_map->gfmc_lb_fc_map));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_0_FC_MAPf, &fc_map->bfmc0_lb_fc_map));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_1_FC_MAPf, &fc_map->bfmc1_lb_fc_map));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_2_FC_MAPf, &fc_map->bfmc2_lb_fc_map));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_flow_control_get_unsafe()",0,0);
}


soc_error_t 
arad_multicast_table_size_get(int unit, uint32* mc_table_size)
{
    SOCDNX_INIT_FUNC_DEFS;

    *mc_table_size = SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;

    SOCDNX_FUNC_RETURN;    
}

#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
