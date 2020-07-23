

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm/stat.h>
#include <bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <shared/utilex/utilex_integer_arithmetic.h>







static shr_error_e
jr2_a0_dnx_data_stif_config_stif_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int feature_index = dnx_data_stif_config_stif_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_STAT_IF_ENABLE;
    feature->property.doc = 
        "\n"
        "Enable / Disable STIF:\n"
        "stat_if_enable = 0 / 1\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_stif_report_mc_single_copy_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int feature_index = dnx_data_stif_config_stif_report_mc_single_copy;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_STAT_IF_REPORT_MULTICAST_SINGLE_COPY;
    feature->property.doc = 
        "\n"
        "Billing - count all ingress MC copies as one.:\n"
        "stat_if_report_multicast_single_copy = 0/1\n"
        "Default: 1\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_mixing_ports_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int feature_index = dnx_data_stif_config_mixing_ports_limitation;
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
jr2_a0_dnx_data_stif_config_is_stif_record_updated_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int feature_index = dnx_data_stif_config_is_stif_record_updated;
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
jr2_a0_dnx_data_stif_config_sram_buffers_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_sram_buffers_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_sram_pdbs_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_sram_pdbs_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_dram_bdb_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_dram_bdb_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_stif_packet_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_packet_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_PKT_SIZE;
    define->property.doc = 
        "\n"
        "Configure packet size (same for all NIF interfaces):\n"
        "stat_if_pkt_size = 512/ 1024/ 1536\n"
        "Default: 980\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "512";
    define->property.mapping[0].val = 512;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "1024";
    define->property.mapping[1].val = 1024;
    define->property.mapping[2].name = "1536";
    define->property.mapping[2].val = 1536;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_stif_scrubber_queue_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_scrubber_queue_min;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_SCRUBBER_QUEUE_MIN;
    define->property.doc = 
        "\n"
        "Scrubber queue minimum\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = dnx_data_ipq.queues.nof_queues_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_stif_scrubber_queue_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_scrubber_queue_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_ipq.queues.nof_queues_get(unit)-1;

    
    define->data = dnx_data_ipq.queues.nof_queues_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_SCRUBBER_QUEUE_MAX;
    define->property.doc = 
        "\n"
        "Scrubber queue maximum\n"
        "Default: 65535\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = dnx_data_ipq.queues.nof_queues_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_stif_scrubber_rate_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_scrubber_rate_min;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_SCRUBBER_RATE_MIN;
    define->property.doc = 
        "\n"
        "Minimal period between two consecutive scrubber in nanoseconds\n"
        "Default: 0 - disabled\n"
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
jr2_a0_dnx_data_stif_config_stif_scrubber_rate_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_scrubber_rate_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 65535;

    
    define->data = 65535;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_SCRUBBER_RATE_MAX;
    define->property.doc = 
        "\n"
        "Maximal period between two consecutive scrubber in nanoseconds\n"
        "Default: 0 - disabled\n"
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
jr2_a0_dnx_data_stif_config_stif_report_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_report_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_stat_stif_mgmt_report_mode_billing;

    
    define->data = dnx_stat_stif_mgmt_report_mode_billing;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_REPORT_MODE;
    define->property.doc = 
        "\n"
        "STIF report mode - queue size / billing/ billing ingress\n"
        "Default: billing\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "QSIZE";
    define->property.mapping[0].val = dnx_stat_stif_mgmt_report_mode_qsize;
    define->property.mapping[1].name = "BILLING";
    define->property.mapping[1].val = dnx_stat_stif_mgmt_report_mode_billing;
    define->property.mapping[1].is_default = 1 ;
    define->property.mapping[2].name = "INGRESS_BILLING";
    define->property.mapping[2].val = dnx_stat_stif_mgmt_report_mode_billing_ingress;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_stif_report_size_ingress_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_report_size_ingress;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS;

    
    define->data = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_REPORT_SIZE;
    define->property.doc = 
        "\n"
        "Ingress STIF report size in bits\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_direct_map;
    define->property.method_str = "suffix_direct_map";
    define->property.suffix = "ingress";
    define->property.nof_mapping = 4;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "64";
    define->property.mapping[0].val = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "72";
    define->property.mapping[1].val = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_72_BITS;
    define->property.mapping[2].name = "80";
    define->property.mapping[2].val = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_80_BITS;
    define->property.mapping[3].name = "96";
    define->property.mapping[3].val = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_96_BITS;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_stif_report_size_egress_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_report_size_egress;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS;

    
    define->data = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_REPORT_SIZE;
    define->property.doc = 
        "\n"
        "Egress STIF report size in bits\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_direct_map;
    define->property.method_str = "suffix_direct_map";
    define->property.suffix = "egress";
    define->property.nof_mapping = 4;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "64";
    define->property.mapping[0].val = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "72";
    define->property.mapping[1].val = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_72_BITS;
    define->property.mapping[2].name = "80";
    define->property.mapping[2].val = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_80_BITS;
    define->property.mapping[3].name = "96";
    define->property.mapping[3].val = DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_96_BITS;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_stif_idle_report_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_stif_idle_report;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_STAT_IF_IDLE_REPORTS_PRESENT;
    define->property.doc = 
        "\n"
        "Configure if empty reports will be sent:\n"
        "stat_if_idle_report = 0/1\n"
        "if empty reports will be sent the report period will be set to 256\n"
        "Default: 1\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 0;
    define->property.mapping[1].name = "1";
    define->property.mapping[1].val = 256;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_nof_valid_sources_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int define_index = dnx_data_stif_config_define_nof_valid_sources;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_device.general.nof_cores_get(unit)*3;

    
    define->data = dnx_data_device.general.nof_cores_get(unit)*3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_stif_config_scrubber_buffers_set(
    int unit)
{
    int thresh_id_index;
    dnx_data_stif_config_scrubber_buffers_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int table_index = dnx_data_stif_config_table_scrubber_buffers;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 16;
    table->info_get.key_size[0] = 16;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_stif_config_scrubber_buffers_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_stif_config_table_scrubber_buffers");

    
    default_data = (dnx_data_stif_config_scrubber_buffers_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->sram_buffers = 0;
    default_data->sram_pdbs = 0;
    default_data->dram_bdb = 0;
    
    for (thresh_id_index = 0; thresh_id_index < table->keys[0].size; thresh_id_index++)
    {
        data = (dnx_data_stif_config_scrubber_buffers_t *) dnxc_data_mgmt_table_data_get(unit, table, thresh_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_STAT_IF_SCRUBBER_SRAM_BUFFERS_TH;
    table->values[0].property.doc =
        "\n"
        "scrubber sram buffers threshold\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_range;
    table->values[0].property.method_str = "suffix_range";
    table->values[0].property.suffix = "";
    table->values[0].property.range_min = 0;
    table->values[0].property.range_max = utilex_power_of_2(dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit)) - 1;
    
    table->values[1].property.name = spn_STAT_IF_SCRUBBER_SRAM_PDBS_TH;
    table->values[1].property.doc =
        "\n"
        "scrubber sram pdbs threshold\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_range;
    table->values[1].property.method_str = "suffix_range";
    table->values[1].property.suffix = "";
    table->values[1].property.range_min = 0;
    table->values[1].property.range_max = utilex_power_of_2(dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit)) - 1;
    
    table->values[2].property.name = spn_STAT_IF_SCRUBBER_BDB_TH;
    table->values[2].property.doc =
        "\n"
        "scrubber dram bdb threshold\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_suffix_range;
    table->values[2].property.method_str = "suffix_range";
    table->values[2].property.suffix = "";
    table->values[2].property.range_min = 0;
    table->values[2].property.range_max = utilex_power_of_2(dnx_data_ingr_congestion.dbal.dram_bdbs_th_nof_bits_get(unit)) - 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (thresh_id_index = 0; thresh_id_index < table->keys[0].size; thresh_id_index++)
    {
        data = (dnx_data_stif_config_scrubber_buffers_t *) dnxc_data_mgmt_table_data_get(unit, table, thresh_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, thresh_id_index, &data->sram_buffers));
    }
    for (thresh_id_index = 0; thresh_id_index < table->keys[0].size; thresh_id_index++)
    {
        data = (dnx_data_stif_config_scrubber_buffers_t *) dnxc_data_mgmt_table_data_get(unit, table, thresh_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, thresh_id_index, &data->sram_pdbs));
    }
    for (thresh_id_index = 0; thresh_id_index < table->keys[0].size; thresh_id_index++)
    {
        data = (dnx_data_stif_config_scrubber_buffers_t *) dnxc_data_mgmt_table_data_get(unit, table, thresh_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, thresh_id_index, &data->dram_bdb));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_config_selective_report_range_set(
    int unit)
{
    int range_id_index;
    dnx_data_stif_config_selective_report_range_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_config;
    int table_index = dnx_data_stif_config_table_selective_report_range;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "DATA(ipq,queues,nof_queues)-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_stif_config_selective_report_range_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_stif_config_table_selective_report_range");

    
    default_data = (dnx_data_stif_config_selective_report_range_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->min = 0;
    default_data->max = dnx_data_ipq.queues.nof_queues_get(unit)-1;
    
    for (range_id_index = 0; range_id_index < table->keys[0].size; range_id_index++)
    {
        data = (dnx_data_stif_config_selective_report_range_t *) dnxc_data_mgmt_table_data_get(unit, table, range_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_STAT_IF_SELECTIVE_REPORT_QUEUE_MIN;
    table->values[0].property.doc =
        "\n"
        "range of queues for qsize reports - min value\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_range;
    table->values[0].property.method_str = "suffix_range";
    table->values[0].property.suffix = "range";
    table->values[0].property.range_min = 0;
    table->values[0].property.range_max = dnx_data_ipq.queues.nof_queues_get(unit)-1;
    
    table->values[1].property.name = spn_STAT_IF_SELECTIVE_REPORT_QUEUE_MAX;
    table->values[1].property.doc =
        "\n"
        "range of queues for qsize reports - max value\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_range;
    table->values[1].property.method_str = "suffix_range";
    table->values[1].property.suffix = "range";
    table->values[1].property.range_min = 0;
    table->values[1].property.range_max = dnx_data_ipq.queues.nof_queues_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (range_id_index = 0; range_id_index < table->keys[0].size; range_id_index++)
    {
        data = (dnx_data_stif_config_selective_report_range_t *) dnxc_data_mgmt_table_data_get(unit, table, range_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, range_id_index, &data->min));
    }
    for (range_id_index = 0; range_id_index < table->keys[0].size; range_id_index++)
    {
        data = (dnx_data_stif_config_selective_report_range_t *) dnxc_data_mgmt_table_data_get(unit, table, range_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, range_id_index, &data->max));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_stif_port_max_nof_instances_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_port;
    int define_index = dnx_data_stif_port_define_max_nof_instances;
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
jr2_a0_dnx_data_stif_port_stif_ethu_select_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_port;
    int define_index = dnx_data_stif_port_define_stif_ethu_select_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_port_stif_lane_mapping_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_port;
    int define_index = dnx_data_stif_port_define_stif_lane_mapping_size;
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
jr2_a0_dnx_data_stif_port_ethu_id_stif_valid_set(
    int unit)
{
    int ethu_id_index;
    dnx_data_stif_port_ethu_id_stif_valid_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_port;
    int table_index = dnx_data_stif_port_table_ethu_id_stif_valid;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.ethu_nof_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.ethu_nof_get(unit);

    
    table->values[0].default_val = "1";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_stif_port_ethu_id_stif_valid_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_stif_port_table_ethu_id_stif_valid");

    
    default_data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 1;
    default_data->hw_select_value = 0;
    
    for (ethu_id_index = 0; ethu_id_index < table->keys[0].size; ethu_id_index++)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, ethu_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->valid = 1;
        data->hw_select_value = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->valid = 1;
        data->hw_select_value = 2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->valid = 1;
        data->hw_select_value = 4;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->valid = 1;
        data->hw_select_value = 8;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->valid = 1;
        data->hw_select_value = 16;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->valid = 1;
        data->hw_select_value = 32;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->valid = 1;
        data->hw_select_value = 1;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->valid = 1;
        data->hw_select_value = 2;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->valid = 1;
        data->hw_select_value = 4;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->valid = 1;
        data->hw_select_value = 8;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->valid = 1;
        data->hw_select_value = 16;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_stif_port_ethu_id_stif_valid_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->valid = 1;
        data->hw_select_value = 32;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_stif_report_etpp_core_in_portmetadata_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int feature_index = dnx_data_stif_report_etpp_core_in_portmetadata;
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
jr2_a0_dnx_data_stif_report_stif_record_is_last_copy_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int feature_index = dnx_data_stif_report_stif_record_is_last_copy;
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
jr2_a0_dnx_data_stif_report_qsize_idle_report_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_qsize_idle_report_size;
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
jr2_a0_dnx_data_stif_report_qsize_idle_report_period_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_qsize_idle_report_period;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_qsize_idle_report_period_invalid_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_qsize_idle_report_period_invalid;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 127;

    
    define->data = 127;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_qsize_scrubber_report_cycles_unit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_max_thresholds_per_buffer_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_max_thresholds_per_buffer;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_incoming_tc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_incoming_tc;
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
jr2_a0_dnx_data_stif_report_record_two_msb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_record_two_msb;
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
jr2_a0_dnx_data_stif_report_nof_source_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_nof_source_types;
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
jr2_a0_dnx_data_stif_report_stat_id_max_count_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_stat_id_max_count;
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
jr2_a0_dnx_data_stif_report_stat_object_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_stat_object_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_billing_format_element_packet_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_billing_format_element_packet_size;
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
jr2_a0_dnx_data_stif_report_billing_format_msb_opcode_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_billing_format_msb_opcode_size;
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
jr2_a0_dnx_data_stif_report_billing_format_etpp_metadata_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_billing_format_etpp_metadata_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_billing_opsize_use_one_type_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_billing_opsize_use_one_type_mask;
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
jr2_a0_dnx_data_stif_report_billing_opsize_use_two_types_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_billing_opsize_use_two_types_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_billing_opsize_use_three_types_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_billing_opsize_use_three_types_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 63;

    
    define->data = 63;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_billing_opsize_use_four_types_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_billing_opsize_use_four_types_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 255;

    
    define->data = 255;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_billing_enq_ing_size_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int define_index = dnx_data_stif_report_define_billing_enq_ing_size_shift;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_stif_report_etpp_metadata_set(
    int unit)
{
    int element_index;
    dnx_data_stif_report_etpp_metadata_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int table_index = dnx_data_stif_report_table_etpp_metadata;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 31;
    table->info_get.key_size[0] = 31;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_stif_report_etpp_metadata_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_stif_report_table_etpp_metadata");

    
    default_data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->size = 0;
    
    for (element_index = 0; element_index < table->keys[0].size; element_index++)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, element_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmStatStifRecordElementEgressMetaDataMultiCast < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataMultiCast, 0);
        data->size = 1;
    }
    if (bcmStatStifRecordElementEgressMetaDataPpDropReason < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataPpDropReason, 0);
        data->size = 2;
    }
    if (bcmStatStifRecordElementEgressMetaDataPort < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataPort, 0);
        data->size = 8;
    }
    if (bcmStatStifRecordElementEgressMetaDataEcnEligibleAndCni < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataEcnEligibleAndCni, 0);
        data->size = 1;
    }
    if (bcmStatStifRecordElementEgressMetaDataTrafficClass < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataTrafficClass, 0);
        data->size = 3;
    }
    if (bcmStatStifRecordElementEgressMetaDataDropPrecedence < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataDropPrecedence, 0);
        data->size = 2;
    }
    if (bcmStatStifRecordElementEgressMetaDataObj0 < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataObj0, 0);
        data->size = 4;
    }
    if (bcmStatStifRecordElementEgressMetaDataObj1 < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataObj1, 0);
        data->size = 4;
    }
    if (bcmStatStifRecordElementEgressMetaDataObj2 < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataObj2, 0);
        data->size = 4;
    }
    if (bcmStatStifRecordElementEgressMetaDataObj3 < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataObj3, 0);
        data->size = 4;
    }
    if (bcmStatStifRecordElementEgressMetaDataCore < table->keys[0].size)
    {
        data = (dnx_data_stif_report_etpp_metadata_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatStifRecordElementEgressMetaDataCore, 0);
        data->size = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_report_report_size_set(
    int unit)
{
    int dbal_enum_index;
    dnx_data_stif_report_report_size_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_report;
    int table_index = dnx_data_stif_report_table_report_size;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_STIF_BILLING_REPORT_SIZE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_STIF_BILLING_REPORT_SIZE_VALUES;

    
    table->values[0].default_val = "64";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_stif_report_report_size_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_stif_report_table_report_size");

    
    default_data = (dnx_data_stif_report_report_size_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->size = 64;
    
    for (dbal_enum_index = 0; dbal_enum_index < table->keys[0].size; dbal_enum_index++)
    {
        data = (dnx_data_stif_report_report_size_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_enum_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS < table->keys[0].size)
    {
        data = (dnx_data_stif_report_report_size_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_64_BITS, 0);
        data->size = 64;
    }
    if (DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_72_BITS < table->keys[0].size)
    {
        data = (dnx_data_stif_report_report_size_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_72_BITS, 0);
        data->size = 72;
    }
    if (DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_80_BITS < table->keys[0].size)
    {
        data = (dnx_data_stif_report_report_size_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_80_BITS, 0);
        data->size = 80;
    }
    if (DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_96_BITS < table->keys[0].size)
    {
        data = (dnx_data_stif_report_report_size_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_96_BITS, 0);
        data->size = 96;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_stif_flow_control_instance_0_must_be_on_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int feature_index = dnx_data_stif_flow_control_instance_0_must_be_on;
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
jr2_a0_dnx_data_stif_flow_control_etpp_pipe_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_etpp_pipe_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 770;

    
    define->data = 770;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_flow_control_etpp_deq_size_of_fifo_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo;
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
jr2_a0_dnx_data_stif_flow_control_cgm_pipe_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_cgm_pipe_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_flow_control_cgm_size_of_fifo_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_cgm_size_of_fifo;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 208;

    
    define->data = 208;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_flow_control_billing_level_sampling_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_billing_level_sampling;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_flow_control_fc_disable_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_fc_disable_value;
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
jr2_a0_dnx_data_stif_flow_control_billing_etpp_flow_control_high_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_billing_etpp_flow_control_high;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_stif.flow_control.etpp_deq_size_of_fifo_get(unit)*0.9 - dnx_data_stif.flow_control.etpp_pipe_length_get(unit) - dnx_data_stif.flow_control.billing_level_sampling_get(unit);

    
    define->data = dnx_data_stif.flow_control.etpp_deq_size_of_fifo_get(unit)*0.9 - dnx_data_stif.flow_control.etpp_pipe_length_get(unit) - dnx_data_stif.flow_control.billing_level_sampling_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_flow_control_billing_etpp_flow_control_low_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_billing_etpp_flow_control_low;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_stif.flow_control.billing_etpp_flow_control_high_get(unit) - 1;

    
    define->data = dnx_data_stif.flow_control.billing_etpp_flow_control_high_get(unit) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_flow_control_billing_cgm_flow_control_high_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_billing_cgm_flow_control_high;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_stif.flow_control.cgm_size_of_fifo_get(unit)*0.9 - dnx_data_stif.flow_control.cgm_pipe_length_get(unit) - dnx_data_stif.flow_control.billing_level_sampling_get(unit);

    
    define->data = dnx_data_stif.flow_control.cgm_size_of_fifo_get(unit)*0.9 - dnx_data_stif.flow_control.cgm_pipe_length_get(unit) - dnx_data_stif.flow_control.billing_level_sampling_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_flow_control_billing_cgm_flow_control_low_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_billing_cgm_flow_control_low;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_stif.flow_control.billing_cgm_flow_control_high_get(unit) - 1;

    
    define->data = dnx_data_stif.flow_control.billing_cgm_flow_control_high_get(unit) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_flow_control_qsize_cgm_flow_control_high_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_stif.flow_control.cgm_size_of_fifo_get(unit)*0.9 - dnx_data_stif.flow_control.cgm_pipe_length_get(unit);

    
    define->data = dnx_data_stif.flow_control.cgm_size_of_fifo_get(unit)*0.9 - dnx_data_stif.flow_control.cgm_pipe_length_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stif_flow_control_qsize_cgm_flow_control_low_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stif;
    int submodule_index = dnx_data_stif_submodule_flow_control;
    int define_index = dnx_data_stif_flow_control_define_qsize_cgm_flow_control_low;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_stif.flow_control.qsize_cgm_flow_control_high_get(unit) - 1;

    
    define->data = dnx_data_stif.flow_control.qsize_cgm_flow_control_high_get(unit) - 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_stif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_stif;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_stif_submodule_config;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_stif_config_define_sram_buffers_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_sram_buffers_resolution_set;
    data_index = dnx_data_stif_config_define_sram_pdbs_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_sram_pdbs_resolution_set;
    data_index = dnx_data_stif_config_define_dram_bdb_resolution;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_dram_bdb_resolution_set;
    data_index = dnx_data_stif_config_define_stif_packet_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_stif_packet_size_set;
    data_index = dnx_data_stif_config_define_stif_scrubber_queue_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_stif_scrubber_queue_min_set;
    data_index = dnx_data_stif_config_define_stif_scrubber_queue_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_stif_scrubber_queue_max_set;
    data_index = dnx_data_stif_config_define_stif_scrubber_rate_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_stif_scrubber_rate_min_set;
    data_index = dnx_data_stif_config_define_stif_scrubber_rate_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_stif_scrubber_rate_max_set;
    data_index = dnx_data_stif_config_define_stif_report_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_stif_report_mode_set;
    data_index = dnx_data_stif_config_define_stif_report_size_ingress;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_stif_report_size_ingress_set;
    data_index = dnx_data_stif_config_define_stif_report_size_egress;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_stif_report_size_egress_set;
    data_index = dnx_data_stif_config_define_stif_idle_report;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_stif_idle_report_set;
    data_index = dnx_data_stif_config_define_nof_valid_sources;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_config_nof_valid_sources_set;

    
    data_index = dnx_data_stif_config_stif_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_stif_config_stif_enable_set;
    data_index = dnx_data_stif_config_stif_report_mc_single_copy;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_stif_config_stif_report_mc_single_copy_set;
    data_index = dnx_data_stif_config_mixing_ports_limitation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_stif_config_mixing_ports_limitation_set;
    data_index = dnx_data_stif_config_is_stif_record_updated;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_stif_config_is_stif_record_updated_set;

    
    data_index = dnx_data_stif_config_table_scrubber_buffers;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_stif_config_scrubber_buffers_set;
    data_index = dnx_data_stif_config_table_selective_report_range;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_stif_config_selective_report_range_set;
    
    submodule_index = dnx_data_stif_submodule_port;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_stif_port_define_max_nof_instances;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_port_max_nof_instances_set;
    data_index = dnx_data_stif_port_define_stif_ethu_select_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_port_stif_ethu_select_size_set;
    data_index = dnx_data_stif_port_define_stif_lane_mapping_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_port_stif_lane_mapping_size_set;

    

    
    data_index = dnx_data_stif_port_table_ethu_id_stif_valid;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_stif_port_ethu_id_stif_valid_set;
    
    submodule_index = dnx_data_stif_submodule_report;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_stif_report_define_qsize_idle_report_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_qsize_idle_report_size_set;
    data_index = dnx_data_stif_report_define_qsize_idle_report_period;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_qsize_idle_report_period_set;
    data_index = dnx_data_stif_report_define_qsize_idle_report_period_invalid;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_qsize_idle_report_period_invalid_set;
    data_index = dnx_data_stif_report_define_qsize_scrubber_report_cycles_unit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_qsize_scrubber_report_cycles_unit_set;
    data_index = dnx_data_stif_report_define_max_thresholds_per_buffer;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_max_thresholds_per_buffer_set;
    data_index = dnx_data_stif_report_define_incoming_tc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_incoming_tc_set;
    data_index = dnx_data_stif_report_define_record_two_msb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_record_two_msb_set;
    data_index = dnx_data_stif_report_define_nof_source_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_nof_source_types_set;
    data_index = dnx_data_stif_report_define_stat_id_max_count;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_stat_id_max_count_set;
    data_index = dnx_data_stif_report_define_stat_object_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_stat_object_size_set;
    data_index = dnx_data_stif_report_define_billing_format_element_packet_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_billing_format_element_packet_size_set;
    data_index = dnx_data_stif_report_define_billing_format_msb_opcode_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_billing_format_msb_opcode_size_set;
    data_index = dnx_data_stif_report_define_billing_format_etpp_metadata_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_billing_format_etpp_metadata_size_set;
    data_index = dnx_data_stif_report_define_billing_opsize_use_one_type_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_billing_opsize_use_one_type_mask_set;
    data_index = dnx_data_stif_report_define_billing_opsize_use_two_types_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_billing_opsize_use_two_types_mask_set;
    data_index = dnx_data_stif_report_define_billing_opsize_use_three_types_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_billing_opsize_use_three_types_mask_set;
    data_index = dnx_data_stif_report_define_billing_opsize_use_four_types_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_billing_opsize_use_four_types_mask_set;
    data_index = dnx_data_stif_report_define_billing_enq_ing_size_shift;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_report_billing_enq_ing_size_shift_set;

    
    data_index = dnx_data_stif_report_etpp_core_in_portmetadata;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_stif_report_etpp_core_in_portmetadata_set;
    data_index = dnx_data_stif_report_stif_record_is_last_copy;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_stif_report_stif_record_is_last_copy_set;

    
    data_index = dnx_data_stif_report_table_etpp_metadata;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_stif_report_etpp_metadata_set;
    data_index = dnx_data_stif_report_table_report_size;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_stif_report_report_size_set;
    
    submodule_index = dnx_data_stif_submodule_flow_control;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_stif_flow_control_define_etpp_pipe_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_etpp_pipe_length_set;
    data_index = dnx_data_stif_flow_control_define_etpp_deq_size_of_fifo;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_etpp_deq_size_of_fifo_set;
    data_index = dnx_data_stif_flow_control_define_cgm_pipe_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_cgm_pipe_length_set;
    data_index = dnx_data_stif_flow_control_define_cgm_size_of_fifo;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_cgm_size_of_fifo_set;
    data_index = dnx_data_stif_flow_control_define_billing_level_sampling;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_billing_level_sampling_set;
    data_index = dnx_data_stif_flow_control_define_fc_disable_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_fc_disable_value_set;
    data_index = dnx_data_stif_flow_control_define_billing_etpp_flow_control_high;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_billing_etpp_flow_control_high_set;
    data_index = dnx_data_stif_flow_control_define_billing_etpp_flow_control_low;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_billing_etpp_flow_control_low_set;
    data_index = dnx_data_stif_flow_control_define_billing_cgm_flow_control_high;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_billing_cgm_flow_control_high_set;
    data_index = dnx_data_stif_flow_control_define_billing_cgm_flow_control_low;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_billing_cgm_flow_control_low_set;
    data_index = dnx_data_stif_flow_control_define_qsize_cgm_flow_control_high;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_qsize_cgm_flow_control_high_set;
    data_index = dnx_data_stif_flow_control_define_qsize_cgm_flow_control_low;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stif_flow_control_qsize_cgm_flow_control_low_set;

    
    data_index = dnx_data_stif_flow_control_instance_0_must_be_on;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_stif_flow_control_instance_0_must_be_on_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

