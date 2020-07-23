

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/cosq/flow_control/cosq_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/intr/auto_generated/j2p/j2p_intr.h>







static shr_error_e
j2p_a0_dnx_data_fc_general_fc_to_nif_status_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_fc_to_nif_status;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_glb_res_lp_to_llfc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_glb_res_lp_to_llfc;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_vsqd_to_llfc_pfc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_vsqd_to_llfc_pfc;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_vsq_thr_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_vsq_thr_en;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_oob_tx_reset_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_oob_tx_reset;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_additional_enablers_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_additional_enablers;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_glb_rsc_oob_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_glb_rsc_oob_en;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_cat2_oob_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_cat2_oob_en;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_pfc_rec_cdu_shift_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_pfc_rec_cdu_shift;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_pfc_status_regs_double_read_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_pfc_status_regs_double_read;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_ilkn_oob_fast_llfc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_ilkn_oob_fast_llfc;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_pp_inb_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_pp_inb_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_fc_from_llfc_vsq_status_sel_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_fc_from_llfc_vsq_status_sel;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_if_2_cfc_enablers_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_if_2_cfc_enablers;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_ilkn_oob_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_ilkn_oob_en;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_ilkn_polarity_set_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_ilkn_polarity_set;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_additional_rx_pfc_bmp_types_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_additional_rx_pfc_bmp_types;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_fc_general_nof_glb_rsc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int define_index = dnx_data_fc_general_define_nof_glb_rsc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_nof_af_nif_indications_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int define_index = dnx_data_fc_general_define_nof_af_nif_indications;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_general_nof_cal_llfcs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int define_index = dnx_data_fc_general_define_nof_cal_llfcs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit) + 4;

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit) + 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_fc_coe_nof_supported_vlan_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_nof_supported_vlan_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_coe_nof_coe_vid_mask_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_nof_coe_vid_mask_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_coe_nof_coe_vid_right_shift_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_nof_coe_vid_right_shift_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_coe_max_coe_ticks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_max_coe_ticks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1048575;

    
    define->data = 1048575;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_coe_fc_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_fc_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}












static shr_error_e
j2p_a0_dnx_data_fc_pfc_deadlock_cdum_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_pfc_deadlock;
    int define_index = dnx_data_fc_pfc_deadlock_define_cdum_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2P_INT_CDBM_PFC_DEADLOCK_BREAKING_MECHANISM_INT;

    
    define->data = J2P_INT_CDBM_PFC_DEADLOCK_BREAKING_MECHANISM_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_pfc_deadlock_cdu_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_pfc_deadlock;
    int define_index = dnx_data_fc_pfc_deadlock_define_cdu_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2P_INT_CDB_PFC_DEADLOCK_BREAKING_MECHANISM_INT;

    
    define->data = J2P_INT_CDB_PFC_DEADLOCK_BREAKING_MECHANISM_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fc_pfc_deadlock_clu_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_pfc_deadlock;
    int define_index = dnx_data_fc_pfc_deadlock_define_clu_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2P_INT_LAST;

    
    define->data = J2P_INT_LAST;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2p_a0_data_fc_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_fc;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_fc_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fc_general_define_nof_glb_rsc;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_general_nof_glb_rsc_set;
    data_index = dnx_data_fc_general_define_nof_af_nif_indications;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_general_nof_af_nif_indications_set;
    data_index = dnx_data_fc_general_define_nof_cal_llfcs;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_general_nof_cal_llfcs_set;

    
    data_index = dnx_data_fc_general_fc_to_nif_status;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_fc_to_nif_status_set;
    data_index = dnx_data_fc_general_glb_res_lp_to_llfc;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_glb_res_lp_to_llfc_set;
    data_index = dnx_data_fc_general_vsqd_to_llfc_pfc;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_vsqd_to_llfc_pfc_set;
    data_index = dnx_data_fc_general_vsq_thr_en;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_vsq_thr_en_set;
    data_index = dnx_data_fc_general_oob_tx_reset;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_oob_tx_reset_set;
    data_index = dnx_data_fc_general_additional_enablers;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_additional_enablers_set;
    data_index = dnx_data_fc_general_glb_rsc_oob_en;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_glb_rsc_oob_en_set;
    data_index = dnx_data_fc_general_cat2_oob_en;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_cat2_oob_en_set;
    data_index = dnx_data_fc_general_pfc_rec_cdu_shift;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_pfc_rec_cdu_shift_set;
    data_index = dnx_data_fc_general_pfc_status_regs_double_read;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_pfc_status_regs_double_read_set;
    data_index = dnx_data_fc_general_ilkn_oob_fast_llfc;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_ilkn_oob_fast_llfc_set;
    data_index = dnx_data_fc_general_pp_inb_enable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_pp_inb_enable_set;
    data_index = dnx_data_fc_general_fc_from_llfc_vsq_status_sel;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_fc_from_llfc_vsq_status_sel_set;
    data_index = dnx_data_fc_general_if_2_cfc_enablers;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_if_2_cfc_enablers_set;
    data_index = dnx_data_fc_general_ilkn_oob_en;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_ilkn_oob_en_set;
    data_index = dnx_data_fc_general_ilkn_polarity_set;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_ilkn_polarity_set_set;
    data_index = dnx_data_fc_general_additional_rx_pfc_bmp_types;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fc_general_additional_rx_pfc_bmp_types_set;

    
    
    submodule_index = dnx_data_fc_submodule_coe;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fc_coe_define_nof_supported_vlan_ids;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_coe_nof_supported_vlan_ids_set;
    data_index = dnx_data_fc_coe_define_nof_coe_vid_mask_width;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_coe_nof_coe_vid_mask_width_set;
    data_index = dnx_data_fc_coe_define_nof_coe_vid_right_shift_width;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_coe_nof_coe_vid_right_shift_width_set;
    data_index = dnx_data_fc_coe_define_max_coe_ticks;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_coe_max_coe_ticks_set;
    data_index = dnx_data_fc_coe_define_fc_support;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_coe_fc_support_set;

    

    
    
    submodule_index = dnx_data_fc_submodule_oob;
    submodule = &module->submodules[submodule_index];

    

    

    
    
    submodule_index = dnx_data_fc_submodule_pfc_deadlock;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fc_pfc_deadlock_define_cdum_intr_id;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_pfc_deadlock_cdum_intr_id_set;
    data_index = dnx_data_fc_pfc_deadlock_define_cdu_intr_id;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_pfc_deadlock_cdu_intr_id_set;
    data_index = dnx_data_fc_pfc_deadlock_define_clu_intr_id;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fc_pfc_deadlock_clu_intr_id_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

