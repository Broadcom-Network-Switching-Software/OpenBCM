

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SAT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sat.h>



extern shr_error_e jr2_a0_data_sat_attach(
    int unit);
extern shr_error_e jr2_b0_data_sat_attach(
    int unit);
extern shr_error_e j2c_a0_data_sat_attach(
    int unit);
extern shr_error_e q2a_a0_data_sat_attach(
    int unit);
extern shr_error_e j2p_a0_data_sat_attach(
    int unit);



static shr_error_e
dnx_data_sat_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "SAT general data";
    
    submodule_data->nof_features = _dnx_data_sat_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sat general features");

    submodule_data->features[dnx_data_sat_general_is_sat_supported].name = "is_sat_supported";
    submodule_data->features[dnx_data_sat_general_is_sat_supported].doc = "indicate if SAT is supported";
    submodule_data->features[dnx_data_sat_general_is_sat_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_sat_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sat general defines");

    submodule_data->defines[dnx_data_sat_general_define_granularity].name = "granularity";
    submodule_data->defines[dnx_data_sat_general_define_granularity].doc = "granularity (64kbits per sec)";
    submodule_data->defines[dnx_data_sat_general_define_granularity].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_sat_general_define_granularity].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_sat_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sat general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sat_general_feature_get(
    int unit,
    dnx_data_sat_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_general, feature);
}


uint32
dnx_data_sat_general_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_general, dnx_data_sat_general_define_granularity);
}










static shr_error_e
dnx_data_sat_generator_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "generator";
    submodule_data->doc = "SAT gtf data";
    
    submodule_data->nof_features = _dnx_data_sat_generator_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sat generator features");

    
    submodule_data->nof_defines = _dnx_data_sat_generator_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sat generator defines");

    submodule_data->defines[dnx_data_sat_generator_define_nof_evc_ids].name = "nof_evc_ids";
    submodule_data->defines[dnx_data_sat_generator_define_nof_evc_ids].doc = "Number of EVC IDs";
    
    submodule_data->defines[dnx_data_sat_generator_define_nof_evc_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_nof_gtf_ids].name = "nof_gtf_ids";
    submodule_data->defines[dnx_data_sat_generator_define_nof_gtf_ids].doc = "Number of GTF IDs";
    
    submodule_data->defines[dnx_data_sat_generator_define_nof_gtf_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_header_max_length].name = "gtf_pkt_header_max_length";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_header_max_length].doc = "SAT GTF packet header max length";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_header_max_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_length_max_patn].name = "gtf_pkt_length_max_patn";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_length_max_patn].doc = "SAT GTF packet length max pattern";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_length_max_patn].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_max_seq_period].name = "gtf_max_seq_period";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_max_seq_period].doc = "SAT GTF max sequence period";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_max_seq_period].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_stamp_max_inc_step].name = "gtf_stamp_max_inc_step";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_stamp_max_inc_step].doc = "SAT GTF stamp max increment step";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_stamp_max_inc_step].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_stamp_max_inc_period].name = "gtf_stamp_max_inc_period";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_stamp_max_inc_period].doc = "SAT GTF stamp max increment period";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_stamp_max_inc_period].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_bw_max_value].name = "gtf_bw_max_value";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_bw_max_value].doc = "GTF bandwidth max value";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_bw_max_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_rate_max].name = "gtf_rate_max";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_rate_max].doc = "GTF max rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_rate_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_pps_rate_max].name = "gtf_pps_rate_max";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pps_rate_max].doc = "GTF max pps rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pps_rate_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_header_entries].name = "gtf_pkt_header_entries";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_header_entries].doc = "Gtf packet header entries";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_header_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_header_length].name = "gtf_pkt_header_length";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_header_length].doc = "gtf packet header length";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_header_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_len_patten_size].name = "gtf_pkt_len_patten_size";
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_len_patten_size].doc = "Packet length pattern size";
    
    submodule_data->defines[dnx_data_sat_generator_define_gtf_pkt_len_patten_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_64B].name = "max_bit_rate_in_64B";
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_64B].doc = "Gtf max bit rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_64B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_128B].name = "max_bit_rate_in_128B";
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_128B].doc = "Gtf max bit rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_128B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_256B].name = "max_bit_rate_in_256B";
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_256B].doc = "Gtf max bit rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_256B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_512B].name = "max_bit_rate_in_512B";
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_512B].doc = "Gtf max bit rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_512B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_1024B].name = "max_bit_rate_in_1024B";
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_1024B].doc = "Gtf max bit rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_1024B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_1280B].name = "max_bit_rate_in_1280B";
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_1280B].doc = "Gtf max bit rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_1280B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_1518B].name = "max_bit_rate_in_1518B";
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_1518B].doc = "Gtf max bit rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_1518B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_9000B].name = "max_bit_rate_in_9000B";
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_9000B].doc = "Gtf max bit rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_bit_rate_in_9000B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_64B].name = "max_packet_rate_in_64B";
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_64B].doc = "Gtf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_64B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_128B].name = "max_packet_rate_in_128B";
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_128B].doc = "Gtf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_128B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_256B].name = "max_packet_rate_in_256B";
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_256B].doc = "Gtf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_256B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_512B].name = "max_packet_rate_in_512B";
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_512B].doc = "Gtf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_512B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_1024B].name = "max_packet_rate_in_1024B";
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_1024B].doc = "Gtf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_1024B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_1280B].name = "max_packet_rate_in_1280B";
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_1280B].doc = "Gtf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_1280B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_1518B].name = "max_packet_rate_in_1518B";
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_1518B].doc = "Gtf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_1518B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_9000B].name = "max_packet_rate_in_9000B";
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_9000B].doc = "Gtf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_packet_rate_in_9000B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_64B].name = "max_ctf_recv_packet_rate_in_64B";
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_64B].doc = "Ctf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_64B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_128B].name = "max_ctf_recv_packet_rate_in_128B";
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_128B].doc = "Ctf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_128B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_256B].name = "max_ctf_recv_packet_rate_in_256B";
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_256B].doc = "Ctf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_256B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_512B].name = "max_ctf_recv_packet_rate_in_512B";
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_512B].doc = "Ctf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_512B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1024B].name = "max_ctf_recv_packet_rate_in_1024B";
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1024B].doc = "Ctf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1024B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1280B].name = "max_ctf_recv_packet_rate_in_1280B";
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1280B].doc = "Ctf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1280B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1518B].name = "max_ctf_recv_packet_rate_in_1518B";
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1518B].doc = "Ctf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1518B].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_9000B].name = "max_ctf_recv_packet_rate_in_9000B";
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_9000B].doc = "Ctf max packet rate";
    
    submodule_data->defines[dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_9000B].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_sat_generator_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sat generator tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sat_generator_feature_get(
    int unit,
    dnx_data_sat_generator_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, feature);
}


uint32
dnx_data_sat_generator_nof_evc_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_nof_evc_ids);
}

uint32
dnx_data_sat_generator_nof_gtf_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_nof_gtf_ids);
}

uint32
dnx_data_sat_generator_gtf_pkt_header_max_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_pkt_header_max_length);
}

uint32
dnx_data_sat_generator_gtf_pkt_length_max_patn_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_pkt_length_max_patn);
}

uint32
dnx_data_sat_generator_gtf_max_seq_period_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_max_seq_period);
}

uint32
dnx_data_sat_generator_gtf_stamp_max_inc_step_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_stamp_max_inc_step);
}

uint32
dnx_data_sat_generator_gtf_stamp_max_inc_period_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_stamp_max_inc_period);
}

uint32
dnx_data_sat_generator_gtf_bw_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_bw_max_value);
}

uint32
dnx_data_sat_generator_gtf_rate_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_rate_max);
}

uint32
dnx_data_sat_generator_gtf_pps_rate_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_pps_rate_max);
}

uint32
dnx_data_sat_generator_gtf_pkt_header_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_pkt_header_entries);
}

uint32
dnx_data_sat_generator_gtf_pkt_header_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_pkt_header_length);
}

uint32
dnx_data_sat_generator_gtf_pkt_len_patten_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_gtf_pkt_len_patten_size);
}

uint32
dnx_data_sat_generator_max_bit_rate_in_64B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_bit_rate_in_64B);
}

uint32
dnx_data_sat_generator_max_bit_rate_in_128B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_bit_rate_in_128B);
}

uint32
dnx_data_sat_generator_max_bit_rate_in_256B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_bit_rate_in_256B);
}

uint32
dnx_data_sat_generator_max_bit_rate_in_512B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_bit_rate_in_512B);
}

uint32
dnx_data_sat_generator_max_bit_rate_in_1024B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_bit_rate_in_1024B);
}

uint32
dnx_data_sat_generator_max_bit_rate_in_1280B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_bit_rate_in_1280B);
}

uint32
dnx_data_sat_generator_max_bit_rate_in_1518B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_bit_rate_in_1518B);
}

uint32
dnx_data_sat_generator_max_bit_rate_in_9000B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_bit_rate_in_9000B);
}

uint32
dnx_data_sat_generator_max_packet_rate_in_64B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_packet_rate_in_64B);
}

uint32
dnx_data_sat_generator_max_packet_rate_in_128B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_packet_rate_in_128B);
}

uint32
dnx_data_sat_generator_max_packet_rate_in_256B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_packet_rate_in_256B);
}

uint32
dnx_data_sat_generator_max_packet_rate_in_512B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_packet_rate_in_512B);
}

uint32
dnx_data_sat_generator_max_packet_rate_in_1024B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_packet_rate_in_1024B);
}

uint32
dnx_data_sat_generator_max_packet_rate_in_1280B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_packet_rate_in_1280B);
}

uint32
dnx_data_sat_generator_max_packet_rate_in_1518B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_packet_rate_in_1518B);
}

uint32
dnx_data_sat_generator_max_packet_rate_in_9000B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_packet_rate_in_9000B);
}

uint32
dnx_data_sat_generator_max_ctf_recv_packet_rate_in_64B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_64B);
}

uint32
dnx_data_sat_generator_max_ctf_recv_packet_rate_in_128B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_128B);
}

uint32
dnx_data_sat_generator_max_ctf_recv_packet_rate_in_256B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_256B);
}

uint32
dnx_data_sat_generator_max_ctf_recv_packet_rate_in_512B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_512B);
}

uint32
dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1024B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1024B);
}

uint32
dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1280B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1280B);
}

uint32
dnx_data_sat_generator_max_ctf_recv_packet_rate_in_1518B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_1518B);
}

uint32
dnx_data_sat_generator_max_ctf_recv_packet_rate_in_9000B_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_generator, dnx_data_sat_generator_define_max_ctf_recv_packet_rate_in_9000B);
}










static shr_error_e
dnx_data_sat_collector_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "collector";
    submodule_data->doc = "SAT ctf data";
    
    submodule_data->nof_features = _dnx_data_sat_collector_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sat collector features");

    
    submodule_data->nof_defines = _dnx_data_sat_collector_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sat collector defines");

    submodule_data->defines[dnx_data_sat_collector_define_nof_ctf_ids].name = "nof_ctf_ids";
    submodule_data->defines[dnx_data_sat_collector_define_nof_ctf_ids].doc = "Number of CTF IDs";
    
    submodule_data->defines[dnx_data_sat_collector_define_nof_ctf_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_nof_oam_ids].name = "nof_oam_ids";
    submodule_data->defines[dnx_data_sat_collector_define_nof_oam_ids].doc = "Number of OAM IDs";
    
    submodule_data->defines[dnx_data_sat_collector_define_nof_oam_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_mef_payload_offset].name = "mef_payload_offset";
    submodule_data->defines[dnx_data_sat_collector_define_mef_payload_offset].doc = "SAT MEF payload offset";
    
    submodule_data->defines[dnx_data_sat_collector_define_mef_payload_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_mef_seq_num_offset].name = "mef_seq_num_offset";
    submodule_data->defines[dnx_data_sat_collector_define_mef_seq_num_offset].doc = "SAT MEF seq num offset";
    
    submodule_data->defines[dnx_data_sat_collector_define_mef_seq_num_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_mef_time_stamp_offset].name = "mef_time_stamp_offset";
    submodule_data->defines[dnx_data_sat_collector_define_mef_time_stamp_offset].doc = "SAT MEF time stamp offset";
    
    submodule_data->defines[dnx_data_sat_collector_define_mef_time_stamp_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_mef_crc_byte_offset].name = "mef_crc_byte_offset";
    submodule_data->defines[dnx_data_sat_collector_define_mef_crc_byte_offset].doc = "SAT MEF crc byte offset";
    
    submodule_data->defines[dnx_data_sat_collector_define_mef_crc_byte_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_y1731_payload_offset].name = "y1731_payload_offset";
    submodule_data->defines[dnx_data_sat_collector_define_y1731_payload_offset].doc = "SAT Y1731 payload offset";
    
    submodule_data->defines[dnx_data_sat_collector_define_y1731_payload_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_y1731_seq_num_offset].name = "y1731_seq_num_offset";
    submodule_data->defines[dnx_data_sat_collector_define_y1731_seq_num_offset].doc = "SAT Y1731 seq num offset";
    
    submodule_data->defines[dnx_data_sat_collector_define_y1731_seq_num_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_y1731_time_stamp_offset].name = "y1731_time_stamp_offset";
    submodule_data->defines[dnx_data_sat_collector_define_y1731_time_stamp_offset].doc = "SAT Y1731 time stamp offset";
    
    submodule_data->defines[dnx_data_sat_collector_define_y1731_time_stamp_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_y1731_crc_byte_offset].name = "y1731_crc_byte_offset";
    submodule_data->defines[dnx_data_sat_collector_define_y1731_crc_byte_offset].doc = "SAT Y1731 crc byte offset";
    
    submodule_data->defines[dnx_data_sat_collector_define_y1731_crc_byte_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_ctf_bins_limit_entries].name = "ctf_bins_limit_entries";
    submodule_data->defines[dnx_data_sat_collector_define_ctf_bins_limit_entries].doc = "ctf bins limit entries";
    
    submodule_data->defines[dnx_data_sat_collector_define_ctf_bins_limit_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_trap_id_entries].name = "trap_id_entries";
    submodule_data->defines[dnx_data_sat_collector_define_trap_id_entries].doc = "Trap id entries";
    
    submodule_data->defines[dnx_data_sat_collector_define_trap_id_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_sat_collector_define_trap_data_entries].name = "trap_data_entries";
    submodule_data->defines[dnx_data_sat_collector_define_trap_data_entries].doc = "Trap data entries";
    
    submodule_data->defines[dnx_data_sat_collector_define_trap_data_entries].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_sat_collector_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sat collector tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sat_collector_feature_get(
    int unit,
    dnx_data_sat_collector_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, feature);
}


uint32
dnx_data_sat_collector_nof_ctf_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_nof_ctf_ids);
}

uint32
dnx_data_sat_collector_nof_oam_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_nof_oam_ids);
}

uint32
dnx_data_sat_collector_mef_payload_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_mef_payload_offset);
}

uint32
dnx_data_sat_collector_mef_seq_num_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_mef_seq_num_offset);
}

uint32
dnx_data_sat_collector_mef_time_stamp_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_mef_time_stamp_offset);
}

uint32
dnx_data_sat_collector_mef_crc_byte_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_mef_crc_byte_offset);
}

uint32
dnx_data_sat_collector_y1731_payload_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_y1731_payload_offset);
}

uint32
dnx_data_sat_collector_y1731_seq_num_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_y1731_seq_num_offset);
}

uint32
dnx_data_sat_collector_y1731_time_stamp_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_y1731_time_stamp_offset);
}

uint32
dnx_data_sat_collector_y1731_crc_byte_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_y1731_crc_byte_offset);
}

uint32
dnx_data_sat_collector_ctf_bins_limit_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_ctf_bins_limit_entries);
}

uint32
dnx_data_sat_collector_trap_id_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_trap_id_entries);
}

uint32
dnx_data_sat_collector_trap_data_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_collector, dnx_data_sat_collector_define_trap_data_entries);
}










static shr_error_e
dnx_data_sat_rx_flow_param_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rx_flow_param";
    submodule_data->doc = "SAT rx flow param";
    
    submodule_data->nof_features = _dnx_data_sat_rx_flow_param_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sat rx_flow_param features");

    submodule_data->features[dnx_data_sat_rx_flow_param_natural_id].name = "natural_id";
    submodule_data->features[dnx_data_sat_rx_flow_param_natural_id].doc = "indicate if the rx flow param table index is natural id or odd/even id";
    submodule_data->features[dnx_data_sat_rx_flow_param_natural_id].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_sat_rx_flow_param_odd_even_id].name = "odd_even_id";
    submodule_data->features[dnx_data_sat_rx_flow_param_odd_even_id].doc = "indicate if the rx flow param table index is natural id or odd/even id";
    submodule_data->features[dnx_data_sat_rx_flow_param_odd_even_id].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_sat_rx_flow_param_twamp_mode].name = "twamp_mode";
    submodule_data->features[dnx_data_sat_rx_flow_param_twamp_mode].doc = "indicate if TWAMP modei enable.";
    submodule_data->features[dnx_data_sat_rx_flow_param_twamp_mode].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_sat_rx_flow_param_twamp_rx_time_stamp].name = "twamp_rx_time_stamp";
    submodule_data->features[dnx_data_sat_rx_flow_param_twamp_rx_time_stamp].doc = "indicate if TWAMP RX time stamp enable.";
    submodule_data->features[dnx_data_sat_rx_flow_param_twamp_rx_time_stamp].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_sat_rx_flow_param_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sat rx_flow_param defines");

    
    submodule_data->nof_tables = _dnx_data_sat_rx_flow_param_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sat rx_flow_param tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sat_rx_flow_param_feature_get(
    int unit,
    dnx_data_sat_rx_flow_param_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_rx_flow_param, feature);
}











static shr_error_e
dnx_data_sat_tx_flow_param_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tx_flow_param";
    submodule_data->doc = "SAT tx param";
    
    submodule_data->nof_features = _dnx_data_sat_tx_flow_param_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sat tx_flow_param features");

    submodule_data->features[dnx_data_sat_tx_flow_param_seq_number_wrap_around].name = "seq_number_wrap_around";
    submodule_data->features[dnx_data_sat_tx_flow_param_seq_number_wrap_around].doc = " SEQ number wrap-around ";
    submodule_data->features[dnx_data_sat_tx_flow_param_seq_number_wrap_around].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_sat_tx_flow_param_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sat tx_flow_param defines");

    
    submodule_data->nof_tables = _dnx_data_sat_tx_flow_param_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sat tx_flow_param tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sat_tx_flow_param_feature_get(
    int unit,
    dnx_data_sat_tx_flow_param_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sat, dnx_data_sat_submodule_tx_flow_param, feature);
}








shr_error_e
dnx_data_sat_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "sat";
    module_data->nof_submodules = _dnx_data_sat_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data sat submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_sat_general_init(unit, &module_data->submodules[dnx_data_sat_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_sat_generator_init(unit, &module_data->submodules[dnx_data_sat_submodule_generator]));
    SHR_IF_ERR_EXIT(dnx_data_sat_collector_init(unit, &module_data->submodules[dnx_data_sat_submodule_collector]));
    SHR_IF_ERR_EXIT(dnx_data_sat_rx_flow_param_init(unit, &module_data->submodules[dnx_data_sat_submodule_rx_flow_param]));
    SHR_IF_ERR_EXIT(dnx_data_sat_tx_flow_param_init(unit, &module_data->submodules[dnx_data_sat_submodule_tx_flow_param]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sat_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_sat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sat_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_sat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sat_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_sat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sat_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_sat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sat_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_sat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sat_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_sat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sat_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_sat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sat_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_sat_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

