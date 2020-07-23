

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
#include <bcm/types.h>
#include <bcm_int/dnx/cosq/flow_control/cosq_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr.h>







static shr_error_e
jr2_a0_dnx_data_fc_general_fc_to_nif_status_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_fc_to_nif_status;
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
jr2_a0_dnx_data_fc_general_ilu_fc_status_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_ilu_fc_status;
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
jr2_a0_dnx_data_fc_general_inb_ilkn_rx_status_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_inb_ilkn_rx_status;
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
jr2_a0_dnx_data_fc_general_glb_res_lp_to_llfc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_glb_res_lp_to_llfc;
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
jr2_a0_dnx_data_fc_general_vsqd_to_llfc_pfc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_vsqd_to_llfc_pfc;
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
jr2_a0_dnx_data_fc_general_vsq_thr_en_set(
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
jr2_a0_dnx_data_fc_general_oob_tx_reset_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_oob_tx_reset;
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
jr2_a0_dnx_data_fc_general_additional_enablers_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_additional_enablers;
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
jr2_a0_dnx_data_fc_general_ilkn_inb_llfc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_ilkn_inb_llfc;
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
jr2_a0_dnx_data_fc_general_flexe_flow_control_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_flexe_flow_control;
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
jr2_a0_dnx_data_fc_general_glb_rsc_oob_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_glb_rsc_oob_en;
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
jr2_a0_dnx_data_fc_general_cat2_oob_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_cat2_oob_en;
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
jr2_a0_dnx_data_fc_general_pfc_rec_cdu_shift_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_pfc_rec_cdu_shift;
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
jr2_a0_dnx_data_fc_general_pfc_status_regs_double_read_set(
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
jr2_a0_dnx_data_fc_general_ilkn_oob_fast_llfc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_ilkn_oob_fast_llfc;
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
jr2_a0_dnx_data_fc_general_pp_inb_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_pp_inb_enable;
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
jr2_a0_dnx_data_fc_general_fc_from_llfc_vsq_status_sel_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_fc_from_llfc_vsq_status_sel;
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
jr2_a0_dnx_data_fc_general_if_2_cfc_enablers_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_if_2_cfc_enablers;
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
jr2_a0_dnx_data_fc_general_ilkn_oob_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_ilkn_oob_en;
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
jr2_a0_dnx_data_fc_general_ilkn_polarity_set_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int feature_index = dnx_data_fc_general_ilkn_polarity_set;
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
jr2_a0_dnx_data_fc_general_nof_glb_rsc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int define_index = dnx_data_fc_general_define_nof_glb_rsc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_general_nof_gen_bmps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int define_index = dnx_data_fc_general_define_nof_gen_bmps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_general_nof_af_nif_indications_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int define_index = dnx_data_fc_general_define_nof_af_nif_indications;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_general_nof_cal_llfcs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int define_index = dnx_data_fc_general_define_nof_cal_llfcs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_per_core_get(unit) + 4;

    
    define->data = dnx_data_nif.phys.nof_phys_per_core_get(unit) + 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_general_nof_entries_qmlf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int define_index = dnx_data_fc_general_define_nof_entries_qmlf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32767;

    
    define->data = 32767;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_general_bcm_cos_count_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_general;
    int define_index = dnx_data_fc_general_define_bcm_cos_count;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = BCM_COS_COUNT;

    
    define->data = BCM_COS_COUNT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_fc_coe_nof_coe_cal_instances_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_nof_coe_cal_instances;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_nof_supported_vlan_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_nof_supported_vlan_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_nof_coe_vid_mask_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_nof_coe_vid_mask_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_nof_coe_vid_right_shift_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_nof_coe_vid_right_shift_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_max_coe_ticks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_max_coe_ticks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8191;

    
    define->data = 8191;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_fc_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_fc_support;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_calendar_pause_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_calendar_pause_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FC_CALENDAR_PAUSE_RESOLUTION;
    define->property.doc = 
        "\n"
        "PP COE Pause counter rate. Unit: usec.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 4294967295;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_data_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_data_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FC_COE_DATA_OFFSET;
    define->property.doc = 
        "\n"
        "Define offset in bytes from after Ethernet Frame for COE FC data. Valid range = [0-31]\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 31;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_ethertype_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_ethertype;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FC_COE_ETHERTYPE;
    define->property.doc = 
        "\n"
        "Set ethertype of COE FC packet.(16bits)\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 65535;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_control_opcode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_control_opcode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x10;

    
    define->data = 0x10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FC_CALENDAR_COE_MODE;
    define->property.doc = 
        "\n"
        "Set PP COE Control Opcode used to identify COE.(16bits)\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "PAUSE";
    define->property.mapping[0].val = 0x10;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "PFC";
    define->property.mapping[1].val = 0x101;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_coe_vid_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_coe_vid_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3;

    
    define->data = 0x3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_coe_vid_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_coe_vid_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xff;

    
    define->data = 0xff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_coe_vid_right_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int define_index = dnx_data_fc_coe_define_coe_vid_right_shift;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x2;

    
    define->data = 0x2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_fc_coe_calendar_mode_set(
    int unit)
{
    dnx_data_fc_coe_calendar_mode_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int table_index = dnx_data_fc_coe_table_calendar_mode;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_fc_coe_calendar_mode_t, (1 + 1  ), "data of dnx_data_fc_coe_table_calendar_mode");

    
    default_data = (dnx_data_fc_coe_calendar_mode_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mode = 0;
    
    data = (dnx_data_fc_coe_calendar_mode_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_FC_CALENDAR_COE_MODE;
    table->values[0].property.doc =
        "\n"
        "PP COE Mode\n"
        "Valid value: PAUSE/PFC\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_direct_map;
    table->values[0].property.method_str = "direct_map";
    table->values[0].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_fc_coe_calendar_mode_t property mapping");

    table->values[0].property.mapping[0].name = "PAUSE";
    table->values[0].property.mapping[0].val = DNX_FC_COE_MODE_PAUSE;
    table->values[0].property.mapping[1].name = "PFC";
    table->values[0].property.mapping[1].val = DNX_FC_COE_MODE_PFC;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_fc_coe_calendar_mode_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->mode));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_mac_address_set(
    int unit)
{
    dnx_data_fc_coe_mac_address_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int table_index = dnx_data_fc_coe_table_mac_address;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0, 0, 0, 0, 0, 0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_fc_coe_mac_address_t, (1 + 1  ), "data of dnx_data_fc_coe_table_mac_address");

    
    default_data = (dnx_data_fc_coe_mac_address_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mac[0] = 0;
    default_data->mac[1] = 0;
    default_data->mac[2] = 0;
    default_data->mac[3] = 0;
    default_data->mac[4] = 0;
    default_data->mac[5] = 0;
    
    data = (dnx_data_fc_coe_mac_address_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->property.name = "fc_coe_mac_address";
    table->property.doc =
        "\n"
        "Set the mac address of COE FC packet (48 bits)\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_fc_coe_mac_address_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnx_data_property_fc_coe_mac_address_read(unit, data));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_coe_calender_length_set(
    int unit)
{
    int cal_id_index;
    dnx_data_fc_coe_calender_length_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_coe;
    int table_index = dnx_data_fc_coe_table_calender_length;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "256";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fc_coe_calender_length_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fc_coe_table_calender_length");

    
    default_data = (dnx_data_fc_coe_calender_length_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->len = 256;
    
    for (cal_id_index = 0; cal_id_index < table->keys[0].size; cal_id_index++)
    {
        data = (dnx_data_fc_coe_calender_length_t *) dnxc_data_mgmt_table_data_get(unit, table, cal_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_FC_COE_CALENDER_LENGTH;
    table->values[0].property.doc =
        "\n"
        "Flow Control COE RX calender length.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_range_signed;
    table->values[0].property.method_str = "port_range_signed";
    table->values[0].property.range_min = 1;
    table->values[0].property.range_max = 256;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (cal_id_index = 0; cal_id_index < table->keys[0].size; cal_id_index++)
    {
        data = (dnx_data_fc_coe_calender_length_t *) dnxc_data_mgmt_table_data_get(unit, table, cal_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, cal_id_index, &data->len));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_fc_oob_nof_oob_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_oob;
    int define_index = dnx_data_fc_oob_define_nof_oob_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_fc_oob_calender_length_set(
    int unit)
{
    int oob_id_index;
    dnx_data_fc_oob_calender_length_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_oob;
    int table_index = dnx_data_fc_oob_table_calender_length;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "512";
    table->values[1].default_val = "512";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fc_oob_calender_length_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fc_oob_table_calender_length");

    
    default_data = (dnx_data_fc_oob_calender_length_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->tx = 512;
    default_data->rx = 512;
    
    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_calender_length_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "fc_oob_calender_length";
    table->property.doc =
        "\n"
        "Flow Control Out-Of-Band TX/RX calender length.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_calender_length_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_fc_oob_calender_length_read(unit, oob_id_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_oob_calender_rep_count_set(
    int unit)
{
    int oob_id_index;
    dnx_data_fc_oob_calender_rep_count_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_oob;
    int table_index = dnx_data_fc_oob_table_calender_rep_count;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "1";
    table->values[1].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fc_oob_calender_rep_count_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fc_oob_table_calender_rep_count");

    
    default_data = (dnx_data_fc_oob_calender_rep_count_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->tx = 1;
    default_data->rx = 1;
    
    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_calender_rep_count_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "fc_oob_calender_rep_count";
    table->property.doc =
        "\n"
        "Flow Control Out-Of-Band TX/RX repeat count.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_calender_rep_count_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_fc_oob_calender_rep_count_read(unit, oob_id_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_oob_tx_freq_set(
    int unit)
{
    int oob_id_index;
    dnx_data_fc_oob_tx_freq_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_oob;
    int table_index = dnx_data_fc_oob_table_tx_freq;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fc_oob_tx_freq_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fc_oob_table_tx_freq");

    
    default_data = (dnx_data_fc_oob_tx_freq_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->ratio = 1;
    
    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_tx_freq_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_FC_OOB_TX_FREQ_RATIO;
    table->values[0].property.doc =
        "\n"
        "Sets the Flow Control OOB TX Speed relatively to the Core Clock. Possible values:\n"
        "1 - Same as Core Clock\n"
        "2 - Core Clock / 2\n"
        "4 - Core Clock / 4\n"
        "6 - Core Clock / 6\n"
        "8 - Core Clock / 8\n"
        "10 - Core Clock / 10\n"
        "12 - Core Clock / 12\n"
        "14 - Core Clock / 14\n"
        "16 - Core Clock / 16\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_direct_map;
    table->values[0].property.method_str = "port_direct_map";
    table->values[0].property.nof_mapping = 8;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_fc_oob_tx_freq_t property mapping");

    table->values[0].property.mapping[0].name = "2";
    table->values[0].property.mapping[0].val = 0;
    table->values[0].property.mapping[1].name = "4";
    table->values[0].property.mapping[1].val = 1;
    table->values[0].property.mapping[1].is_default = 1 ;
    table->values[0].property.mapping[2].name = "6";
    table->values[0].property.mapping[2].val = 2;
    table->values[0].property.mapping[3].name = "8";
    table->values[0].property.mapping[3].val = 3;
    table->values[0].property.mapping[4].name = "10";
    table->values[0].property.mapping[4].val = 4;
    table->values[0].property.mapping[5].name = "12";
    table->values[0].property.mapping[5].val = 5;
    table->values[0].property.mapping[6].name = "14";
    table->values[0].property.mapping[6].val = 6;
    table->values[0].property.mapping[7].name = "16";
    table->values[0].property.mapping[7].val = 7;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_tx_freq_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, oob_id_index, &data->ratio));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_oob_port_set(
    int unit)
{
    int oob_id_index;
    dnx_data_fc_oob_port_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_oob;
    int table_index = dnx_data_fc_oob_table_port;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "1";
    table->values[1].default_val = "DNX_FC_CAL_MODE_DISABLE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fc_oob_port_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fc_oob_table_port");

    
    default_data = (dnx_data_fc_oob_port_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->type = 1;
    default_data->mode = DNX_FC_CAL_MODE_DISABLE;
    
    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_port_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_FC_OOB_TYPE;
    table->values[0].property.doc =
        "\n"
        "Flow Control Out-Of-Band port type. Values:\n"
        "0x1 for SPI,  0x2 for ILKN\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_direct_map;
    table->values[0].property.method_str = "port_direct_map";
    table->values[0].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_fc_oob_port_t property mapping");

    table->values[0].property.mapping[0].name = "1";
    table->values[0].property.mapping[0].val = 1;
    table->values[0].property.mapping[0].is_default = 1 ;
    table->values[0].property.mapping[1].name = "2";
    table->values[0].property.mapping[1].val = 2;
    
    table->values[1].property.name = spn_FC_OOB_MODE;
    table->values[1].property.doc =
        "\n"
        "Flow Control Out-Of-Band port mode.. Values:\n"
        "0x0 for disable, 0x1 for RX, 0x2 for TX, 0x3 for both\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_port_direct_map;
    table->values[1].property.method_str = "port_direct_map";
    table->values[1].property.nof_mapping = 4;
    DNXC_DATA_ALLOC(table->values[1].property.mapping, dnxc_data_property_map_t, table->values[1].property.nof_mapping, "dnx_data_fc_oob_port_t property mapping");

    table->values[1].property.mapping[0].name = "0";
    table->values[1].property.mapping[0].val = DNX_FC_CAL_MODE_DISABLE;
    table->values[1].property.mapping[0].is_default = 1 ;
    table->values[1].property.mapping[1].name = "1";
    table->values[1].property.mapping[1].val = DNX_FC_CAL_MODE_RX_ENABLE;
    table->values[1].property.mapping[2].name = "2";
    table->values[1].property.mapping[2].val = DNX_FC_CAL_MODE_TX_ENABLE;
    table->values[1].property.mapping[3].name = "3";
    table->values[1].property.mapping[3].val = DNX_FC_CAL_MODE_TX_RX_ENABLE;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_port_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, oob_id_index, &data->type));
    }
    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_port_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, oob_id_index, &data->mode));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_oob_indication_invert_set(
    int unit)
{
    int oob_id_index;
    dnx_data_fc_oob_indication_invert_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_oob;
    int table_index = dnx_data_fc_oob_table_indication_invert;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fc_oob_indication_invert_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fc_oob_table_indication_invert");

    
    default_data = (dnx_data_fc_oob_indication_invert_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->ilkn_tx = 0;
    default_data->ilkn_rx = 0;
    default_data->spi = 0;
    
    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_indication_invert_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_FC_INTLKN_INDICATION_INVERT;
    table->values[0].property.doc =
        "\n"
        "This option controls the meaning of '0'/'1' in the of Calendar FC indications per interface.\n"
        "If unset (default), use the standard FC indication. If set, use an inverted FC indication.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_enable;
    table->values[0].property.method_str = "suffix_enable";
    table->values[0].property.suffix = "tx";
    
    table->values[1].property.name = spn_FC_INTLKN_INDICATION_INVERT;
    table->values[1].property.doc =
        "\n"
        "This option controls the meaning of '0'/'1' in the of Calendar FC indications per interface.\n"
        "If unset (default), use the standard FC indication. If set, use an inverted FC indication.\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_enable;
    table->values[1].property.method_str = "suffix_enable";
    table->values[1].property.suffix = "rx";
    
    table->values[2].property.name = spn_FC_SPI_INDICATION_INVERT;
    table->values[2].property.doc =
        "\n"
        "This option controls the meaning of '0'/'1' in the of Calendar FC indications per interface.\n"
        "If unset (default), use the standard FC indication. If set, use an inverted FC indication.\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_enable;
    table->values[2].property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_indication_invert_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, oob_id_index, &data->ilkn_tx));
    }
    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_indication_invert_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, oob_id_index, &data->ilkn_rx));
    }
    for (oob_id_index = 0; oob_id_index < table->keys[0].size; oob_id_index++)
    {
        data = (dnx_data_fc_oob_indication_invert_t *) dnxc_data_mgmt_table_data_get(unit, table, oob_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, oob_id_index, &data->spi));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_fc_inband_nof_inband_intlkn_cal_instances_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_inband;
    int define_index = dnx_data_fc_inband_define_nof_inband_intlkn_cal_instances;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_fc_pfc_deadlock_cdum_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_pfc_deadlock;
    int define_index = dnx_data_fc_pfc_deadlock_define_cdum_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = JR2_INT_CDUM_PFC_DEADLOCK_BREAKING_MECHANISM_INT;

    
    define->data = JR2_INT_CDUM_PFC_DEADLOCK_BREAKING_MECHANISM_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_pfc_deadlock_cdu_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_pfc_deadlock;
    int define_index = dnx_data_fc_pfc_deadlock_define_cdu_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = JR2_INT_CDU_PFC_DEADLOCK_BREAKING_MECHANISM_INT;

    
    define->data = JR2_INT_CDU_PFC_DEADLOCK_BREAKING_MECHANISM_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fc_pfc_deadlock_clu_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fc;
    int submodule_index = dnx_data_fc_submodule_pfc_deadlock;
    int define_index = dnx_data_fc_pfc_deadlock_define_clu_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = JR2_INT_LAST;

    
    define->data = JR2_INT_LAST;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_fc_attach(
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
    define->set = jr2_a0_dnx_data_fc_general_nof_glb_rsc_set;
    data_index = dnx_data_fc_general_define_nof_gen_bmps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_general_nof_gen_bmps_set;
    data_index = dnx_data_fc_general_define_nof_af_nif_indications;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_general_nof_af_nif_indications_set;
    data_index = dnx_data_fc_general_define_nof_cal_llfcs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_general_nof_cal_llfcs_set;
    data_index = dnx_data_fc_general_define_nof_entries_qmlf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_general_nof_entries_qmlf_set;
    data_index = dnx_data_fc_general_define_bcm_cos_count;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_general_bcm_cos_count_set;

    
    data_index = dnx_data_fc_general_fc_to_nif_status;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_fc_to_nif_status_set;
    data_index = dnx_data_fc_general_ilu_fc_status;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_ilu_fc_status_set;
    data_index = dnx_data_fc_general_inb_ilkn_rx_status;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_inb_ilkn_rx_status_set;
    data_index = dnx_data_fc_general_glb_res_lp_to_llfc;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_glb_res_lp_to_llfc_set;
    data_index = dnx_data_fc_general_vsqd_to_llfc_pfc;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_vsqd_to_llfc_pfc_set;
    data_index = dnx_data_fc_general_vsq_thr_en;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_vsq_thr_en_set;
    data_index = dnx_data_fc_general_oob_tx_reset;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_oob_tx_reset_set;
    data_index = dnx_data_fc_general_additional_enablers;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_additional_enablers_set;
    data_index = dnx_data_fc_general_ilkn_inb_llfc;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_ilkn_inb_llfc_set;
    data_index = dnx_data_fc_general_flexe_flow_control;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_flexe_flow_control_set;
    data_index = dnx_data_fc_general_glb_rsc_oob_en;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_glb_rsc_oob_en_set;
    data_index = dnx_data_fc_general_cat2_oob_en;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_cat2_oob_en_set;
    data_index = dnx_data_fc_general_pfc_rec_cdu_shift;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_pfc_rec_cdu_shift_set;
    data_index = dnx_data_fc_general_pfc_status_regs_double_read;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_pfc_status_regs_double_read_set;
    data_index = dnx_data_fc_general_ilkn_oob_fast_llfc;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_ilkn_oob_fast_llfc_set;
    data_index = dnx_data_fc_general_pp_inb_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_pp_inb_enable_set;
    data_index = dnx_data_fc_general_fc_from_llfc_vsq_status_sel;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_fc_from_llfc_vsq_status_sel_set;
    data_index = dnx_data_fc_general_if_2_cfc_enablers;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_if_2_cfc_enablers_set;
    data_index = dnx_data_fc_general_ilkn_oob_en;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_ilkn_oob_en_set;
    data_index = dnx_data_fc_general_ilkn_polarity_set;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_fc_general_ilkn_polarity_set_set;

    
    
    submodule_index = dnx_data_fc_submodule_coe;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fc_coe_define_nof_coe_cal_instances;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_nof_coe_cal_instances_set;
    data_index = dnx_data_fc_coe_define_nof_supported_vlan_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_nof_supported_vlan_ids_set;
    data_index = dnx_data_fc_coe_define_nof_coe_vid_mask_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_nof_coe_vid_mask_width_set;
    data_index = dnx_data_fc_coe_define_nof_coe_vid_right_shift_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_nof_coe_vid_right_shift_width_set;
    data_index = dnx_data_fc_coe_define_max_coe_ticks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_max_coe_ticks_set;
    data_index = dnx_data_fc_coe_define_fc_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_fc_support_set;
    data_index = dnx_data_fc_coe_define_calendar_pause_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_calendar_pause_resolution_set;
    data_index = dnx_data_fc_coe_define_data_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_data_offset_set;
    data_index = dnx_data_fc_coe_define_ethertype;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_ethertype_set;
    data_index = dnx_data_fc_coe_define_control_opcode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_control_opcode_set;
    data_index = dnx_data_fc_coe_define_coe_vid_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_coe_vid_offset_set;
    data_index = dnx_data_fc_coe_define_coe_vid_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_coe_vid_mask_set;
    data_index = dnx_data_fc_coe_define_coe_vid_right_shift;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_coe_coe_vid_right_shift_set;

    

    
    data_index = dnx_data_fc_coe_table_calendar_mode;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fc_coe_calendar_mode_set;
    data_index = dnx_data_fc_coe_table_mac_address;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fc_coe_mac_address_set;
    data_index = dnx_data_fc_coe_table_calender_length;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fc_coe_calender_length_set;
    
    submodule_index = dnx_data_fc_submodule_oob;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fc_oob_define_nof_oob_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_oob_nof_oob_ids_set;

    

    
    data_index = dnx_data_fc_oob_table_calender_length;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fc_oob_calender_length_set;
    data_index = dnx_data_fc_oob_table_calender_rep_count;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fc_oob_calender_rep_count_set;
    data_index = dnx_data_fc_oob_table_tx_freq;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fc_oob_tx_freq_set;
    data_index = dnx_data_fc_oob_table_port;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fc_oob_port_set;
    data_index = dnx_data_fc_oob_table_indication_invert;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fc_oob_indication_invert_set;
    
    submodule_index = dnx_data_fc_submodule_inband;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fc_inband_define_nof_inband_intlkn_cal_instances;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_inband_nof_inband_intlkn_cal_instances_set;

    

    
    
    submodule_index = dnx_data_fc_submodule_pfc_deadlock;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fc_pfc_deadlock_define_cdum_intr_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_pfc_deadlock_cdum_intr_id_set;
    data_index = dnx_data_fc_pfc_deadlock_define_cdu_intr_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_pfc_deadlock_cdu_intr_id_set;
    data_index = dnx_data_fc_pfc_deadlock_define_clu_intr_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fc_pfc_deadlock_clu_intr_id_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

