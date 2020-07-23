
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD
#include <shared/bsl.h>




#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_lpm_mngr.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_test.h>

#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>




















typedef struct rule_payload_t {
  SOC_SAND_PP_IPV4_SUBNET rule;
  uint32 payload;
} rule_payload_t;

static ARAD_PP_IPV4_LPM_MNGR_INFO  *Lpm_mngr = 0;
static rule_payload_t *rule_list=NULL; 
static uint32 rule_count = 0; 









#define ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK 0xFFFFFFF
#define ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT 0

#define ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(payload) \
  (((payload) >> ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT) & ARAD_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK)

STATIC int32 arad_pp_ipv4_test_rule_payload_compare(void *a, void *b) {
  const rule_payload_t *a1 = a;
  const rule_payload_t *b1 = b;

  if(a1->rule.prefix_len != b1->rule.prefix_len) {
    return(a1->rule.prefix_len - b1->rule.prefix_len);
  } else {
    return(a1->rule.ip_address - b1->rule.ip_address);
  }
}


uint32 arad_pp_frwrd_ipv4_test_clear_vrf(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN ARAD_PP_FRWRD_IPV4_CLEAR_INFO *reset_info
  )
{
  uint32
    res;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  if (rule_list)
  {
    soc_sand_os_free_any_size(rule_list);
    rule_count = 0;
    rule_list = NULL;
  }

  if (reset_info->vrf == SOC_PPC_FRWRD_IP_ALL_VRFS_ID)
  {
    res = soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear(unit,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    goto exit;
  }

  if (reset_info->vrf == 0)
  {
    res = soc_ppd_frwrd_ipv4_uc_routing_table_clear(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    res = soc_ppd_frwrd_ipv4_vrf_routing_table_clear(unit,reset_info->vrf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_test_reset_vrf()", 0, 0);
}

uint32 
  arad_pp_frwrd_ipv4_test_lpm_lkup_get(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY      *lpm_key,
    SOC_SAND_OUT  uint32                  *fec_ptr,
    SOC_SAND_OUT  uint8                 *found
  )
{
    uint32
      res;
    uint64 
        reg_64_val;
    uint32
        fld_val,
        reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_JERICHO(unit)) {
#if defined(INCLUDE_KBP)
      uint8 hit_bit = 0;
      res = arad_pp_frwrd_ipv4_uc_rpf_kaps_dbal_route_get(unit, 
                                                         lpm_key->vrf, 
                                                         &lpm_key->key.subnet, 
                                                         0 ,
                                                         &hit_bit,
                                                         fec_ptr,
                                                         found);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, res);
#endif
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {



      COMPILER_64_SET(reg_64_val,lpm_key->vrf,lpm_key->key.subnet.ip_address);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHB_LPM_QUERY_KEYr(unit,reg_64_val));

      fld_val = 0x1;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LPM_QUERY_PAYLOADr, REG_PORT_ANY, 0, QUERY_TRIGGERf,  fld_val));

      res = arad_polling(
             unit,
             ARAD_TIMEOUT,
             ARAD_MIN_POLLS,
             IHB_LPM_QUERY_PAYLOADr,
             REG_PORT_ANY,
             0,
             QUERY_TRIGGERf,
             0
           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHB_LPM_QUERY_PAYLOADr(unit,&reg_val));
      ARAD_FLD_FROM_REG(IHB_LPM_QUERY_PAYLOADr, QUERY_PAYLOADf, fld_val, reg_val, 50, exit);

      *fec_ptr = fld_val;
  }
  *found = 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_test_print_mem_lest()", lpm_key->vrf, lpm_key->key.subnet.ip_address);
}

uint32 
  arad_pp_frwrd_ipv4_test_init(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN ARAD_PP_FRWRD_IPV4_CLEAR_INFO *reset_info
  )
{
    uint32
      res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(rule_list) {
      res = arad_pp_frwrd_ipv4_test_clear_vrf(unit,reset_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      if(Lpm_mngr) {
          soc_sand_os_free(Lpm_mngr);
          Lpm_mngr = 0;
      }
  }

  if (!rule_list)
  {
    rule_list = (rule_payload_t*)soc_sand_os_malloc_any_size(sizeof(rule_payload_t)*10000, "rule_list 1");
    rule_count = 1;
    rule_list[0].rule.ip_address = 0;
    rule_list[0].rule.prefix_len = 0;
    rule_list[0].payload = 0; 
  }

  if (!Lpm_mngr){
      Lpm_mngr = (ARAD_PP_IPV4_LPM_MNGR_INFO*) soc_sand_os_malloc(sizeof(ARAD_PP_IPV4_LPM_MNGR_INFO), "Lpm_mngr frwrd_ipv4_test ");
      arad_pp_ARAD_PP_IPV4_LPM_MNGR_INFO_clear(Lpm_mngr);

      res = arad_pp_sw_db_ipv4_lpm_mngr_get(
              unit,
              Lpm_mngr
            );
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_test_init()", 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1
uint32 arad_pp_frwrd_ipv4_test_print_mem(
  SOC_SAND_IN int                   unit,
  SOC_SAND_IN  uint32                    bnk_bmp,
  SOC_SAND_IN  uint32                    print_level
  )
{
  uint32
    indx;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_PPC_FRWRD_IPV4_MEM_STATUS
    mem_status;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          0,
          &lpm_mngr
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for (indx = 1; indx < lpm_mngr.init_info.nof_mems; ++indx)
    {
      if (!(SOC_SAND_BIT(indx) & bnk_bmp))
      {
        continue;
      }
      if (print_level >= 1)
      {
        LOG_CLI((BSL_META_U(unit,
                            "\n\r\n\r bank: %u \n\r"),indx));
        res = arad_pp_arr_mem_allocator_print_free_by_order(
            &lpm_mngr.init_info.mem_allocators[indx]);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }

      res = soc_ppd_frwrd_ipv4_mem_status_get(unit,indx,&mem_status);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      SOC_PPC_FRWRD_IPV4_MEM_STATUS_print(&mem_status);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_test_print_mem_lest()", 0, 0);
}
#endif


uint32 arad_pp_frwrd_ipv4_test_defrag_mem(
  SOC_SAND_IN int                   unit,
  SOC_SAND_IN  uint32                    bnk_bmp,
  SOC_SAND_IN  uint32                    print_level
  )
{
  uint32
    indx;
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO
    defrag_info;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_sw_db_ipv4_lpm_mngr_get(
          0,
          &lpm_mngr
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for (indx = 1; indx < lpm_mngr.init_info.nof_mems; ++indx)
    {
      if (!(SOC_SAND_BIT(indx) & bnk_bmp))
      {
        continue;
      }
      SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO_clear(&defrag_info);
      LOG_INFO(BSL_LS_SOC_FORWARD,
               (BSL_META_U(unit,
                           "\n\r\n\r Defrage bank: %u \n\r"),indx));

      res = soc_ppd_frwrd_ipv4_mem_defrage (unit,indx,&defrag_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_test_print_mem_lest()", 0, 0);
}


uint32 arad_pp_frwrd_ipv4_test_list_lookup(
    SOC_SAND_IN int                                unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT  SOC_PPC_FEC_ID                             *fec_id
  )
{
  uint32 best_rule, j;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  for(best_rule=0, j=1; j < rule_count; j++) {
    if((rule_list[j].rule.prefix_len == 0) ||
      ((rule_list[j].rule.ip_address ^ route_key->subnet.ip_address) >> (32 - rule_list[j].rule.prefix_len)) == 0) {
        best_rule = j;
    }
  }
  *fec_id = rule_list[best_rule].payload;


  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_test_list_lookup()", 0, 0);
}



uint32 arad_pp_frwrd_ipv4_test_lookup(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO *lkup_info
  )
{
  ARAD_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_PPC_FEC_ID
    fec_id;
  uint32
    res;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  if(rule_list) {
      res = arad_pp_frwrd_ipv4_test_list_lookup(unit,lkup_info->key.vrf,&(lkup_info->key.key),&fec_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      LOG_INFO(BSL_LS_SOC_FORWARD,
               (BSL_META_U(unit,
                           "***Lookup vrf:%u %08x/%d in LIST gives payload:  0x%x\n"), lkup_info->key.vrf,lkup_info->key.key.subnet.ip_address, lkup_info->key.key.subnet.prefix_len, fec_id));
  }
  else{
      LOG_INFO(BSL_LS_SOC_FORWARD,
               (BSL_META_U(unit,
                           "*** Test DB is not initialized \n\r")));
  }

  
  arad_pp_frwrd_ipv4_test_lpm_lkup_get(unit,&lkup_info->key,&fec_id, &found);
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "***Lookup vrf:%u %08x/%d in HW-diag gives payload:  0x%x\n"), lkup_info->key.vrf,lkup_info->key.key.subnet.ip_address, lkup_info->key.key.subnet.prefix_len, fec_id));



  res = arad_pp_sw_db_ipv4_lpm_mngr_get(
        unit,
        &lpm_mngr
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ipv4_lpm_mngr_lookup0(&lpm_mngr, unit, lkup_info->key.vrf, lkup_info->key.key.subnet.ip_address, &fec_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "***Lookup vrf:%u %08x/%d in SW-simulation gives payload:  0x%x\n"), lkup_info->key.vrf,lkup_info->key.key.subnet.ip_address, lkup_info->key.key.subnet.prefix_len, fec_id));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_test_print_mem_lest()", 0, 0);
}


uint32 arad_pp_frwrd_ipv4_test_vrf(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                    vrf_ndx,
    SOC_SAND_IN  uint32                  nof_iterations
  )
{
  uint32
    res;
  uint32 i;
  ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO key;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO_clear(&key);
  key.key.vrf = vrf_ndx;

      for(i=0; i < rule_count; i++) {
        if(rule_list[i].rule.prefix_len == 0) {
          key.key.key.subnet.ip_address = soc_sand_os_rand();
        } else {
          key.key.key.subnet.ip_address = rule_list[i].rule.ip_address;
          key.key.key.subnet.ip_address &= ~((1 << (32 - rule_list[i].rule.prefix_len))-1);
          key.key.key.subnet.ip_address |= soc_sand_os_rand() & ((1 << (32 - rule_list[i].rule.prefix_len))-1);
        }
        key.key.key.subnet.prefix_len = 32;
        
        
      res = arad_pp_frwrd_ipv4_test_lookup(unit,&key);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
exit:
      SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_test_vrf()", 0, 0);
}
#if SOC_PPC_DEBUG_IS_LVL1
uint32 arad_pp_frwrd_ipv4_test_run(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN ARAD_PP_FRWRD_IPV4_TEST_INFO *tes_info
  )
{
  SOC_SAND_PP_IPV4_SUBNET key;
  uint8 success;
  uint32
    indx;
    SOC_SAND_PAT_TREE_NODE_KEY node_key;
    SOC_SAND_PAT_TREE_NODE_INFO   lpm_info;
    SOC_SAND_PP_SYSTEM_FEC_ID        sys_fec_id=0,add_sys_fec_id=0,sys_fec_id2=0;
    ARAD_PP_DIAG_IPV4_TEST_VPN_KEY
      hw_key;
    uint32 *check_rule_list=NULL,check_rule_idx=0;
    uint32 i,print_level=tes_info->print_level;
    uint32 in_cache = 0;
    uint32 remove_prob=tes_info->remove_prob, till_cache=tes_info->cache_change, till_commit = tes_info->nof_cached, test = tes_info->test,run_traffic_sim=tes_info->hw_test;
    SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             route_key;
    SOC_SAND_SUCCESS_FAILURE                    fail_inidcation;
    uint32 last_cached_index = 0, last_commit_index=0;
    uint32 nof_routes=tes_info->nof_routes;
    uint32
      nof_adds=0;
    uint32
      nof_removes=0;
    uint32
      res;
    uint8
      hw_ok=TRUE,
      found;
    ARAD_PP_FRWRD_IPV4_CLEAR_INFO reset_info;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);






  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_clear(&route_key);


  ARAD_PP_FRWRD_IPV4_CLEAR_INFO_clear(&reset_info);
  reset_info.vrf = 0;

  res = arad_pp_frwrd_ipv4_test_init(unit,&reset_info);

  
  
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


    soc_sand_os_srand(tes_info->seed);
    for(indx = 0; indx < nof_routes && rule_count < 10000 ; indx++) {
      key.ip_address = soc_sand_os_rand();
      key.prefix_len = (uint8)soc_sand_os_rand() % 32 + 1;
       key.ip_address &= key.prefix_len >= SOC_SAND_NOF_BITS_IN_UINT32 ? SOC_SAND_NOF_BITS_IN_UINT32 :  ~(((uint32) -1) >> key.prefix_len);
      if (indx % 100 == 0)
      {
        LOG_INFO(BSL_LS_SOC_FORWARD,
                 (BSL_META_U(unit,
                             "%d\n\r"),indx));
      }
      
      
      if(soc_sand_os_rand() % 2 == 0 && rule_count > 0) {
        uint32 selected_rule;
        
        selected_rule = soc_sand_os_rand() % rule_count;
        if(rule_list[selected_rule].rule.prefix_len < 32 && rule_list[selected_rule].rule.prefix_len > 0 && rule_list[selected_rule].rule.prefix_len > 0) {
          key.ip_address = rule_list[selected_rule].rule.ip_address;
          key.ip_address &= ~((1 << (32 - rule_list[selected_rule].rule.prefix_len))-1);
          key.ip_address |= soc_sand_os_rand() & ((1 << (32 - rule_list[selected_rule].rule.prefix_len))-1);
          key.prefix_len = (uint8)soc_sand_os_rand() % 32 + 1;
        }
      }
      
      if (tes_info->fec_id == 0xFFFFFFFF)
      {
        add_sys_fec_id = soc_sand_os_rand() & 0xff;
      }
      else
      {
        add_sys_fec_id = tes_info->fec_id;
      }

      route_key.subnet.ip_address = key.ip_address;
      route_key.subnet.prefix_len = key.prefix_len;
      if (!in_cache && (till_cache == 0 ||((indx - last_commit_index) % till_cache == till_cache-1) ))
      {
        if (print_level > 0)
        {
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***Enable Caching \n **")));
        }
        
        last_cached_index = indx;
        soc_ppd_frwrd_ip_routes_cache_mode_enable_set(unit,SOC_PPC_FRWRD_IP_ALL_VRFS_ID,SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM);
        in_cache = TRUE;
      }
      else if (in_cache &&  ((last_cached_index + till_commit == indx)))
      {
        if (print_level > 0)
        {
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***start commit cached \n **")));
        }
        if (print_level > 1)
        {
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***cached from %u until %u \n **"), last_cached_index, indx-1));
        }
        last_commit_index = indx;
        
        soc_ppd_frwrd_ip_routes_cache_commit(unit,SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,SOC_PPC_FRWRD_IP_ALL_VRFS_ID,&fail_inidcation);
        soc_ppd_frwrd_ip_routes_cache_mode_enable_set(unit,SOC_PPC_FRWRD_IP_ALL_VRFS_ID,SOC_PPC_FRWRD_IP_CACHE_MODE_NONE);
        if (print_level > 0)
        {
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***Caching Disabled\n **")));
        }
        in_cache = FALSE;
      }

      if (print_level > 0)
      {
        LOG_INFO(BSL_LS_SOC_FORWARD,
                 (BSL_META_U(unit,
                             "***Adding %08x/%d, 0x%x\n"), key.ip_address, key.prefix_len, add_sys_fec_id));
      }
      if (tes_info->add_prob != 0)
      {
        res = soc_ppd_frwrd_ipv4_uc_route_add(unit,&route_key,add_sys_fec_id,&fail_inidcation);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        if (fail_inidcation != SOC_SAND_SUCCESS)
        {
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***ERROR in ADD***\n\r")));
        }
        ++nof_adds;
      }
      
      
      if (test && tes_info->add_prob != 0)
      {
        for(i=0; i < rule_count; i++) {
          if(rule_list[i].rule.prefix_len == key.prefix_len &&
             ((key.prefix_len == 0) ||
              ((rule_list[i].rule.ip_address ^ key.ip_address) >> (32 - key.prefix_len)) == 0)) {
            
            rule_list[i].rule.ip_address = key.ip_address;
            rule_list[i].rule.prefix_len = key.prefix_len;
            rule_list[i].payload = add_sys_fec_id;
            break; 
          }
        }
        if(i >= rule_count) { 
          rule_list[rule_count].rule.ip_address = key.ip_address;
          rule_list[rule_count].rule.prefix_len = key.prefix_len;
          rule_list[rule_count].payload = add_sys_fec_id;
          rule_count++;
          
          soc_sand_os_qsort(rule_list, rule_count, sizeof(*rule_list),
            arad_pp_ipv4_test_rule_payload_compare);
        }
      }
      
      
      if (remove_prob > 0 && tes_info->add_prob == 0 && rule_count == 1)
      {
        LOG_INFO(BSL_LS_SOC_FORWARD,
                 (BSL_META_U(unit,
                             "\n\r Nothing to remove \n\r ")));
        goto exit;
      }
      if(rule_count > 0 && soc_sand_os_rand() % 101 >= 100 - remove_prob ) {
        uint32 selected_rule;

        selected_rule = soc_sand_os_rand() % rule_count;
        if(selected_rule != 0) {
          node_key.val = rule_list[selected_rule].rule.ip_address;
          node_key.prefix = rule_list[selected_rule].rule.prefix_len;
          key.ip_address = node_key.val;
          key.prefix_len = node_key.prefix;
          if (print_level > 0)
          {
            LOG_INFO(BSL_LS_SOC_FORWARD,
                     (BSL_META_U(unit,
                                 "***Removing %08x/%d\n"), key.ip_address, key.prefix_len));
          }
          
          route_key.subnet.ip_address = key.ip_address;
          route_key.subnet.prefix_len = key.prefix_len;
          res = soc_ppd_frwrd_ipv4_uc_route_remove(unit,&route_key,&fail_inidcation);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          if (fail_inidcation != SOC_SAND_SUCCESS)
          {
            LOG_INFO(BSL_LS_SOC_FORWARD,
                     (BSL_META_U(unit,
                                 "***ERROR in Remove***\n\r")));
          }
          ++nof_removes;
          rule_count--;
          rule_list[selected_rule].rule.ip_address = rule_list[rule_count].rule.ip_address;
          rule_list[selected_rule].rule.prefix_len = rule_list[rule_count].rule.prefix_len;
          rule_list[selected_rule].payload = rule_list[rule_count].payload;
          
          soc_sand_os_qsort(rule_list, rule_count, sizeof(*rule_list), arad_pp_ipv4_test_rule_payload_compare);
        }
      }

      if (in_cache &&  ((indx == nof_routes-1 &&till_cache==0)))
      {
        if (print_level > 0)
        {
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***Disable Caching \n **")));
        }
        
        soc_ppd_frwrd_ip_routes_cache_commit(unit,SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,0,&fail_inidcation);
        soc_ppd_frwrd_ip_routes_cache_mode_enable_set(unit,SOC_PPC_FRWRD_IP_ALL_VRFS_ID,SOC_PPC_FRWRD_IP_CACHE_MODE_NONE);
        in_cache = FALSE;
      }

      

      
      if (((nof_adds) % tes_info->defragment_rate) >= tes_info->defragment_rate - 1)
      {
        res = arad_pp_frwrd_ipv4_test_print_mem(unit,tes_info->defragment_banks_bmp,tes_info->print_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        res = arad_pp_frwrd_ipv4_test_defrag_mem(unit,tes_info->defragment_banks_bmp,tes_info->print_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        res = arad_pp_frwrd_ipv4_test_print_mem(unit,tes_info->defragment_banks_bmp,tes_info->print_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }

      
      if (!test && !run_traffic_sim)
      {
        continue;
      }
      
      for(i=0; i < rule_count; i++) {
        uint32 best_rule, j;

        if(rule_list[i].rule.prefix_len == 0) {
          key.ip_address = soc_sand_os_rand();
        } else {
          key.ip_address = rule_list[i].rule.ip_address;
          key.ip_address &= ~((1 << (32 - rule_list[i].rule.prefix_len))-1);
          key.ip_address |= soc_sand_os_rand() & ((1 << (32 - rule_list[i].rule.prefix_len))-1);
        }
        key.prefix_len = 32;
        node_key.val = key.ip_address;
        node_key.prefix = key.prefix_len;

        
        if (in_cache)
        {
          
          continue;;
        }
        else
        {
          if ( 0)
          {
            key.prefix_len = 32;
            node_key.val = check_rule_list[--check_rule_idx];
            node_key.prefix = key.prefix_len;
            i = 0;
          }
        }
        
        
        for(best_rule=0, j=1; j < rule_count; j++) {
          if((rule_list[j].rule.prefix_len == 0) ||
             ((rule_list[j].rule.ip_address ^ key.ip_address) >> (32 - rule_list[j].rule.prefix_len)) == 0) {
            best_rule = j;
          }
        }
        soc_sand_pat_tree_lpm_get(&Lpm_mngr->init_info.lpms[0], &node_key, 0, &lpm_info, &success);
        if ((test & 2))
        {
          res = arad_pp_ipv4_lpm_mngr_lookup0(Lpm_mngr, 0, 0, key.ip_address, &sys_fec_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
        
        if (run_traffic_sim || (test & 1))
        {
          ARAD_PP_DIAG_IPV4_TEST_VPN_KEY_clear(&hw_key);
          hw_key.vrf = 0;
          hw_key.key.subnet.ip_address = key.ip_address;
          hw_key.key.subnet.prefix_len = 32;

          arad_pp_frwrd_ipv4_test_lpm_lkup_get(unit,&hw_key,&sys_fec_id2, &found);
          if (!(test & 2))
          {
            sys_fec_id = sys_fec_id2;
          }
          if (sys_fec_id2 != sys_fec_id)
          {
            hw_ok = FALSE;
          }
        }
        else
        {
          hw_ok = TRUE;
        }
        if (print_level > 1)
        {
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***Lookup %08x/%d in list gives payload: %x\n"), key.ip_address, key.prefix_len, rule_list[best_rule].payload));
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***Lookup %08x/%d in pat gives payload:  %x\n"), key.ip_address, key.prefix_len, ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(lpm_info.data.payload)));
          if (run_traffic_sim || (test & 1))
          {
            LOG_INFO(BSL_LS_SOC_FORWARD,
                     (BSL_META_U(unit,
                                 "***Lookup %08x/%d in hw-lkup gives payload: %x\n"), key.ip_address, key.prefix_len, sys_fec_id2));
          }
          if ((test & 2))
          {
            LOG_INFO(BSL_LS_SOC_FORWARD,
                     (BSL_META_U(unit,
                                 "***Lookup %08x/%d in lpm gives payload:  %x\n"), key.ip_address, key.prefix_len, sys_fec_id));
          }
        }
        if(rule_list[best_rule].payload != sys_fec_id || ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(lpm_info.data.payload) != sys_fec_id || !hw_ok) {
          soc_sand_pat_tree_lpm_get(&Lpm_mngr->init_info.lpms[0], &node_key, 0, &lpm_info, &success);
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "indx = %d, rule_count = %d\n"), indx, rule_count));
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***Lookup %08x/%d in list gives payload: %x\n"), key.ip_address, key.prefix_len, rule_list[best_rule].payload));
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "***Lookup %08x/%d in pat gives payload:  %x\n"), key.ip_address, key.prefix_len, ARAD_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(lpm_info.data.payload)));
          if (run_traffic_sim || (test & 1))
          {
            LOG_INFO(BSL_LS_SOC_FORWARD,
                     (BSL_META_U(unit,
                                 "***Lookup %08x/%d in hw lookup: %x\n"), key.ip_address, key.prefix_len, sys_fec_id2));
          }
          if ((test & 2))
          {
            LOG_INFO(BSL_LS_SOC_FORWARD,
                     (BSL_META_U(unit,
                                 "***Lookup %08x/%d in lpm gives payload:  %x\n"), key.ip_address, key.prefix_len, sys_fec_id));
          }
          for(j=0; j < rule_count; j++) {
            LOG_INFO(BSL_LS_SOC_FORWARD,
                     (BSL_META_U(unit,
                                 "Rule: %08x/%d, %x\n"), rule_list[j].rule.ip_address, rule_list[j].rule.prefix_len, rule_list[j].payload));
          }
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "error\n")));
          return(1);
        }
      }
      if (print_level > 0)
      {
        LOG_INFO(BSL_LS_SOC_FORWARD,
                 (BSL_META_U(unit,
                             "indx = %d, rule_count = %d\n"), indx, rule_count));
      }
      
    }
    if (print_level > 0)
    {
      LOG_INFO(BSL_LS_SOC_FORWARD,
               (BSL_META_U(unit,
                           "indx = %d, rule_count = %d\n"), indx, rule_count));
    }

    if (check_rule_list)
    {
      soc_sand_os_free_any_size(check_rule_list);
    }

    if (print_level == 0)
    {
      arad_pp_arr_mem_allocator_print_free(
        &Lpm_mngr->init_info.mem_allocators[5]);
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ipv4_test_run()", 0, 0);
}
#endif


void
  ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO));
  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY_clear(&(info->key));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_IPV4_TEST_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_TEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_FRWRD_IPV4_TEST_INFO));
  info->seed = 50;
  info->nof_routes = 0;
  info->cache_change = 0;
  info->test = 0;
  info->hw_test = 0;
  info->print_level = 0;
  info->remove_prob = 20;
  info->add_prob = 100;
  info->fec_id = 0xFFFFFFFF;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_IPV4_CLEAR_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_CLEAR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_FRWRD_IPV4_CLEAR_INFO));
  info->vrf = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_IPV4_TEST_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_DIAG_IPV4_TEST_VPN_KEY));
  info->vrf = 0;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&(info->key));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PPC_DEBUG_IS_LVL1
void
  ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_TEST_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "key:")));
  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY_print(&(info->key));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_IPV4_TEST_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_TEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "seed: %u\n\r"),info->seed));
  LOG_CLI((BSL_META_U(unit,
                      "nof_routes: %u\n\r"),info->nof_routes));
  LOG_CLI((BSL_META_U(unit,
                      "cache_change: %u\n\r"),info->cache_change));
  LOG_CLI((BSL_META_U(unit,
                      "test: %u\n\r"),info->test));
  LOG_CLI((BSL_META_U(unit,
                      "hw_test: %u\n\r"),info->hw_test));
  LOG_CLI((BSL_META_U(unit,
                      "print_level: %u\n\r"),info->print_level));
  LOG_CLI((BSL_META_U(unit,
                      "remove_prob: %u\n\r"),info->remove_prob));
  LOG_CLI((BSL_META_U(unit,
                      "add_prob: %u\n\r"),info->add_prob));
  LOG_CLI((BSL_META_U(unit,
                      "fec_id: %u\n\r"),info->fec_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_IPV4_CLEAR_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_CLEAR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vrf: %u\n\r"),info->vrf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY_print(
    SOC_SAND_IN  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vrf: %u\n\r"),info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "key:\n\r")));
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_print(&(info->key));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

