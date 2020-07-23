/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IPMC Internal header
 */
#ifndef _BCM_INT_DPP_BFD_H_
#define _BCM_INT_DPP_BFD_H_

#include <bcm/bfd.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/oam_sw_db.h>
#include <bcm_int/dpp/oam_hw_db.h>
#include <bcm_int/dpp/oam_resource.h>
#include <bcm_int/dpp/oam_dissect.h>

/*ttl tos profile for bfd ipv4 single hop*/
#define _BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE 15
#define _BCM_BFD_IPV4_ECHO_MIN_INTERVAL_PROFILE 14

/* Seamless BFD macros */
#define _BCM_BFD_RESERVED_UDP_PORT_FOR_SBFD 7784
#define _BCM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit) ((SOC_DPP_CONFIG(unit))->pp.bfd_sbfd_enable == 1)
#define _BCM_BFD_SEAMLESS_BFD_REFLECTOR_CONFIG(unit) ((SOC_DPP_CONFIG(unit))->pp.sbfd_reflector_is_configured == 1)
#define _BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info) (_BCM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit) && ((endpoint_info->flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR) != 0))
#define _BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info) (_BCM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit) && ((endpoint_info->flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR) != 0))
#define _BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info) ((_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info))||(_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info)))
#define _BCM_BFD_NOF_SBFD_ENDPOINTS_GET(unit, num_sbfd_endpoints) do {\
                                                                if (_BCM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit)) {\
                                                                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_endpoints.get(unit, &num_sbfd_endpoints));\
                                                                    }\
                                                                }while(0)

#define _BCM_BFD_NOF_SBFD_ENDPOINTS_INCR(unit)                do {\
                                                                uint32 num_sbfd_endpoints = 0;\
                                                                if (_BCM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit)) {\
                                                                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_endpoints.get(unit, &num_sbfd_endpoints));\
                                                                    num_sbfd_endpoints++;\
                                                                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_endpoints.set(unit, num_sbfd_endpoints));\
                                                                }\
                                                                }while(0)

#define _BCM_BFD_NOF_SBFD_ENDPOINTS_DECR(unit)                do {\
                                                                uint32 num_sbfd_endpoints = 0;\
                                                                if (_BCM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit)) {\
                                                                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_endpoints.get(unit, &num_sbfd_endpoints));\
                                                                    num_sbfd_endpoints--;\
                                                                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_endpoints.set(unit, num_sbfd_endpoints));\
                                                                }\
                                                                }while(0)

/* Tx Rate is in 3.33 (JER - 1.67) ms units 
    value in HW is one lower then what was calculated, thus, incrementing
    In case transmit disable(bfd_period = 0) traffic still generated,but forwarded to invalid port,so we setting the maximal rate in order to create less traffic*/
#define _BCM_BFD_ENDPOINT_INFO_BFD_PERIOD_TO_MEP_DB_TX_RATE(bfd_period) \
    ((bfd_period == 0) ? 0x1ff : (SOC_SAND_DIV_ROUND(bfd_period * 100, SOC_IS_ARADPLUS_AND_BELOW(unit) ? 333 : 167) -1))

#define BFD_ACCESS        sw_state_access[unit].dpp.bcm.bfd

/*
 * Struct to maintians tx rates status
 */
typedef struct bcm_dpp_bfd_tx_rates_s {
    uint32       tx_rate[8];
    uint32       ref_count[8];
    sal_time_t   update_time_secs[8];
    sal_usecs_t  update_time_usecs[8];
} bcm_dpp_bfd_tx_rates_t;

volatile bcm_dpp_bfd_tx_rates_t update_bfd_tx_rates;
typedef struct {
    int ref_counter;
    int tos_value;
} bcm_dpp_bfd_pp_s_hop;

typedef struct {
    int                         YourDiscriminator_ref_count;
    bcm_dpp_bfd_pp_s_hop        s_hop;
    SOC_PPC_BFD_INIT_TRAP_INFO  trap_info;
    uint32                      mpls_udp_sport_ref_counter;
    ENDPOINT_LIST_PTR           _bcm_bfd_endpoint_list ;
    uint32                      ipv4_udp_sport_ref_counter;
    uint32                      ipv4_multi_hop_acc_ref_counter;
    uint32                      pdu_ref_counter;
    uint32                      mpls_tp_cc_ref_counter;
    uint32                      tx_rate_value[8];
    uint32                      tx_rate_ref_counter[8];
    uint32                      no_of_sbfd_endpoints;
    uint32                      reflector_trap_id;
    bcm_bfd_state_t             reflector_status;
    uint8                       no_of_sbfd_reflector;
} bcm_dpp_bfd_info_t;


typedef struct {
    uint8 free_tx_gport;
    uint8 free_src_ip_profile;
    uint8 free_tos_ttl_profile;
    uint8 free_bfd_tx_rate;
    uint8 free_req_interval_pointer_local_min_rx;
    uint8 free_req_interval_pointer_local_min_tx;
    uint8 free_pwe_push_profile;
    uint8 free_mhop_itmh_attributes;
    uint8 free_tx_itmh_attributes;
    uint8 free_flags;
} _bcm_bfd_mep_db_entry_pointers_to_free;


void bcm_dpp_bfd_endpoint_diag_print(bcm_bfd_endpoint_info_t* endpoint_info);


/*Function shared by OAM diagnostics and bfd diagnostics. 
   Simply prints information on an egress label*/
void bcm_dpp_print_mpls_egress_label(bcm_mpls_egress_label_t* egress_label);


int  _bcm_bfd_endpoint_info_to_lem_info(int unit, const bcm_bfd_endpoint_info_t *ep_info, ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO *lem_info, int trap_code, uint8 fwd_strenght, int snp_strength);

int _bcm_bfd_tx_rate_profile_alloc(int unit, uint32 tx_rate, uint8 *mep_db_ccm_interval);

int _bcm_bfd_tx_rate_profile_free(int unit, uint32 tx_rate);

int _bcm_bfd_tx_rate_profile_index_free(int unit, uint8 tx_rate_index);

int _bcm_bfd_set_and_allocate_bfd_rates(int unit, uint32 tx_rate, uint32 existing_tx_rate, uint8 *tx_rate_index, uint8 is_update);
#endif /*_BCM_INT_DPP_BFD_H_*/
