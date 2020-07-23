

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_FC_H_

#define _DNX_DATA_FC_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fc.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_fc_init(
    int unit);







typedef enum
{
    
    dnx_data_fc_general_fc_to_nif_status,
    
    dnx_data_fc_general_ilu_fc_status,
    
    dnx_data_fc_general_inb_ilkn_rx_status,
    
    dnx_data_fc_general_glb_res_lp_to_llfc,
    
    dnx_data_fc_general_vsqd_to_llfc_pfc,
    
    dnx_data_fc_general_vsq_thr_en,
    
    dnx_data_fc_general_oob_tx_reset,
    
    dnx_data_fc_general_additional_enablers,
    
    dnx_data_fc_general_ilkn_inb_llfc,
    
    dnx_data_fc_general_flexe_flow_control,
    
    dnx_data_fc_general_glb_rsc_oob_en,
    
    dnx_data_fc_general_cat2_oob_en,
    
    dnx_data_fc_general_pfc_rec_cdu_shift,
    
    dnx_data_fc_general_pfc_status_regs_double_read,
    
    dnx_data_fc_general_ilkn_oob_fast_llfc,
    
    dnx_data_fc_general_pp_inb_enable,
    
    dnx_data_fc_general_fc_from_llfc_vsq_status_sel,
    
    dnx_data_fc_general_if_2_cfc_enablers,
    
    dnx_data_fc_general_ilkn_oob_en,
    
    dnx_data_fc_general_ilkn_polarity_set,
    
    dnx_data_fc_general_additional_rx_pfc_bmp_types,

    
    _dnx_data_fc_general_feature_nof
} dnx_data_fc_general_feature_e;



typedef int(
    *dnx_data_fc_general_feature_get_f) (
    int unit,
    dnx_data_fc_general_feature_e feature);


typedef uint32(
    *dnx_data_fc_general_nof_glb_rsc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_general_nof_gen_bmps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_general_nof_af_nif_indications_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_general_nof_cal_llfcs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_general_nof_entries_qmlf_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_general_bcm_cos_count_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_fc_general_feature_get_f feature_get;
    
    dnx_data_fc_general_nof_glb_rsc_get_f nof_glb_rsc_get;
    
    dnx_data_fc_general_nof_gen_bmps_get_f nof_gen_bmps_get;
    
    dnx_data_fc_general_nof_af_nif_indications_get_f nof_af_nif_indications_get;
    
    dnx_data_fc_general_nof_cal_llfcs_get_f nof_cal_llfcs_get;
    
    dnx_data_fc_general_nof_entries_qmlf_get_f nof_entries_qmlf_get;
    
    dnx_data_fc_general_bcm_cos_count_get_f bcm_cos_count_get;
} dnx_data_if_fc_general_t;






typedef struct
{
    
    int mode;
} dnx_data_fc_coe_calendar_mode_t;


typedef struct
{
    
    uint8 mac[6];
} dnx_data_fc_coe_mac_address_t;


typedef struct
{
    
    int len;
} dnx_data_fc_coe_calender_length_t;



typedef enum
{

    
    _dnx_data_fc_coe_feature_nof
} dnx_data_fc_coe_feature_e;



typedef int(
    *dnx_data_fc_coe_feature_get_f) (
    int unit,
    dnx_data_fc_coe_feature_e feature);


typedef uint32(
    *dnx_data_fc_coe_nof_coe_cal_instances_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_nof_supported_vlan_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_nof_coe_vid_mask_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_nof_coe_vid_right_shift_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_max_coe_ticks_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_fc_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_calendar_pause_resolution_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_data_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_ethertype_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_control_opcode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_coe_vid_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_coe_vid_mask_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_coe_coe_vid_right_shift_get_f) (
    int unit);


typedef const dnx_data_fc_coe_calendar_mode_t *(
    *dnx_data_fc_coe_calendar_mode_get_f) (
    int unit);


typedef const dnx_data_fc_coe_mac_address_t *(
    *dnx_data_fc_coe_mac_address_get_f) (
    int unit);


typedef const dnx_data_fc_coe_calender_length_t *(
    *dnx_data_fc_coe_calender_length_get_f) (
    int unit,
    int cal_id);



typedef struct
{
    
    dnx_data_fc_coe_feature_get_f feature_get;
    
    dnx_data_fc_coe_nof_coe_cal_instances_get_f nof_coe_cal_instances_get;
    
    dnx_data_fc_coe_nof_supported_vlan_ids_get_f nof_supported_vlan_ids_get;
    
    dnx_data_fc_coe_nof_coe_vid_mask_width_get_f nof_coe_vid_mask_width_get;
    
    dnx_data_fc_coe_nof_coe_vid_right_shift_width_get_f nof_coe_vid_right_shift_width_get;
    
    dnx_data_fc_coe_max_coe_ticks_get_f max_coe_ticks_get;
    
    dnx_data_fc_coe_fc_support_get_f fc_support_get;
    
    dnx_data_fc_coe_calendar_pause_resolution_get_f calendar_pause_resolution_get;
    
    dnx_data_fc_coe_data_offset_get_f data_offset_get;
    
    dnx_data_fc_coe_ethertype_get_f ethertype_get;
    
    dnx_data_fc_coe_control_opcode_get_f control_opcode_get;
    
    dnx_data_fc_coe_coe_vid_offset_get_f coe_vid_offset_get;
    
    dnx_data_fc_coe_coe_vid_mask_get_f coe_vid_mask_get;
    
    dnx_data_fc_coe_coe_vid_right_shift_get_f coe_vid_right_shift_get;
    
    dnx_data_fc_coe_calendar_mode_get_f calendar_mode_get;
    
    dnxc_data_table_info_get_f calendar_mode_info_get;
    
    dnx_data_fc_coe_mac_address_get_f mac_address_get;
    
    dnxc_data_table_info_get_f mac_address_info_get;
    
    dnx_data_fc_coe_calender_length_get_f calender_length_get;
    
    dnxc_data_table_info_get_f calender_length_info_get;
} dnx_data_if_fc_coe_t;






typedef struct
{
    
    int tx;
    
    int rx;
} dnx_data_fc_oob_calender_length_t;


typedef struct
{
    
    int tx;
    
    int rx;
} dnx_data_fc_oob_calender_rep_count_t;


typedef struct
{
    
    int ratio;
} dnx_data_fc_oob_tx_freq_t;


typedef struct
{
    
    int type;
    
    int mode;
} dnx_data_fc_oob_port_t;


typedef struct
{
    
    int ilkn_tx;
    
    int ilkn_rx;
    
    int spi;
} dnx_data_fc_oob_indication_invert_t;



typedef enum
{

    
    _dnx_data_fc_oob_feature_nof
} dnx_data_fc_oob_feature_e;



typedef int(
    *dnx_data_fc_oob_feature_get_f) (
    int unit,
    dnx_data_fc_oob_feature_e feature);


typedef uint32(
    *dnx_data_fc_oob_nof_oob_ids_get_f) (
    int unit);


typedef const dnx_data_fc_oob_calender_length_t *(
    *dnx_data_fc_oob_calender_length_get_f) (
    int unit,
    int oob_id);


typedef const dnx_data_fc_oob_calender_rep_count_t *(
    *dnx_data_fc_oob_calender_rep_count_get_f) (
    int unit,
    int oob_id);


typedef const dnx_data_fc_oob_tx_freq_t *(
    *dnx_data_fc_oob_tx_freq_get_f) (
    int unit,
    int oob_id);


typedef const dnx_data_fc_oob_port_t *(
    *dnx_data_fc_oob_port_get_f) (
    int unit,
    int oob_id);


typedef const dnx_data_fc_oob_indication_invert_t *(
    *dnx_data_fc_oob_indication_invert_get_f) (
    int unit,
    int oob_id);



typedef struct
{
    
    dnx_data_fc_oob_feature_get_f feature_get;
    
    dnx_data_fc_oob_nof_oob_ids_get_f nof_oob_ids_get;
    
    dnx_data_fc_oob_calender_length_get_f calender_length_get;
    
    dnxc_data_table_info_get_f calender_length_info_get;
    
    dnx_data_fc_oob_calender_rep_count_get_f calender_rep_count_get;
    
    dnxc_data_table_info_get_f calender_rep_count_info_get;
    
    dnx_data_fc_oob_tx_freq_get_f tx_freq_get;
    
    dnxc_data_table_info_get_f tx_freq_info_get;
    
    dnx_data_fc_oob_port_get_f port_get;
    
    dnxc_data_table_info_get_f port_info_get;
    
    dnx_data_fc_oob_indication_invert_get_f indication_invert_get;
    
    dnxc_data_table_info_get_f indication_invert_info_get;
} dnx_data_if_fc_oob_t;






typedef struct
{
    
    int tx;
    
    int rx;
} dnx_data_fc_inband_calender_length_t;


typedef struct
{
    
    int mode;
    
    int llfc_mode;
} dnx_data_fc_inband_port_t;



typedef enum
{

    
    _dnx_data_fc_inband_feature_nof
} dnx_data_fc_inband_feature_e;



typedef int(
    *dnx_data_fc_inband_feature_get_f) (
    int unit,
    dnx_data_fc_inband_feature_e feature);


typedef uint32(
    *dnx_data_fc_inband_nof_inband_intlkn_cal_instances_get_f) (
    int unit);


typedef const dnx_data_fc_inband_calender_length_t *(
    *dnx_data_fc_inband_calender_length_get_f) (
    int unit,
    int ilkn_id);


typedef const dnx_data_fc_inband_port_t *(
    *dnx_data_fc_inband_port_get_f) (
    int unit,
    int ilkn_id);



typedef struct
{
    
    dnx_data_fc_inband_feature_get_f feature_get;
    
    dnx_data_fc_inband_nof_inband_intlkn_cal_instances_get_f nof_inband_intlkn_cal_instances_get;
    
    dnx_data_fc_inband_calender_length_get_f calender_length_get;
    
    dnxc_data_table_info_get_f calender_length_info_get;
    
    dnx_data_fc_inband_port_get_f port_get;
    
    dnxc_data_table_info_get_f port_info_get;
} dnx_data_if_fc_inband_t;







typedef enum
{

    
    _dnx_data_fc_pfc_deadlock_feature_nof
} dnx_data_fc_pfc_deadlock_feature_e;



typedef int(
    *dnx_data_fc_pfc_deadlock_feature_get_f) (
    int unit,
    dnx_data_fc_pfc_deadlock_feature_e feature);


typedef uint32(
    *dnx_data_fc_pfc_deadlock_cdum_intr_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_pfc_deadlock_cdu_intr_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fc_pfc_deadlock_clu_intr_id_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_fc_pfc_deadlock_feature_get_f feature_get;
    
    dnx_data_fc_pfc_deadlock_cdum_intr_id_get_f cdum_intr_id_get;
    
    dnx_data_fc_pfc_deadlock_cdu_intr_id_get_f cdu_intr_id_get;
    
    dnx_data_fc_pfc_deadlock_clu_intr_id_get_f clu_intr_id_get;
} dnx_data_if_fc_pfc_deadlock_t;





typedef struct
{
    
    dnx_data_if_fc_general_t general;
    
    dnx_data_if_fc_coe_t coe;
    
    dnx_data_if_fc_oob_t oob;
    
    dnx_data_if_fc_inband_t inband;
    
    dnx_data_if_fc_pfc_deadlock_t pfc_deadlock;
} dnx_data_if_fc_t;




extern dnx_data_if_fc_t dnx_data_fc;


#endif 

