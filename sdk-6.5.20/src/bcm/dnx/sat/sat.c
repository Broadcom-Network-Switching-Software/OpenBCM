
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SAT

 
#include <shared/bsl.h>
#include <bcm/sat.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <bcm_int/dnx/sat/sat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/dnxc_ha.h>

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_err_recovery_manager.h>












 





static shr_error_e
dnx_sat_timer_cfg_init(
    int unit,
    dnx_sat_init_t * sat_init)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TIMER_CONFIG, &entry_handle_id));

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLKS_IN_CYCLE, INST_SINGLE,
                                 sat_init->rate_num_clks_cycle);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLOCKS_SEC, INST_SINGLE, sat_init->num_clks_sec);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



static shr_error_e
dnx_sat_credit_cfg_set(
    int unit,
    int credit_backpressure)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (int i = 0; i < 4; i++)
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_CONFIG, &entry_handle_id));

        
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SAT_PIPE_ID, i);

        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_BACKPRESSURE, INST_SINGLE,
                                     credit_backpressure);

        
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



static shr_error_e
dnx_sat_evc_paring_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 evc_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (evc_id = 0; evc_id < DNX_DATA_MAX_SAT_GENERATOR_NOF_EVC_IDS; evc_id++)
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_EVC_PAIR_CONFIG, &entry_handle_id));

        
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_EVC_ID, evc_id);

        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVC_PAIR_SET, INST_SINGLE, 0);

        
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_sat_init_interrupt(
    int unit)
{
    bcm_switch_event_control_t type;
    bcm_switch_event_t switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
    uint32 enable = 1;

    SHR_FUNC_INIT_VARS(unit);

    
    type.event_id = dnx_data_oam.oamp.intr_oamp_stat_pending_event_get(unit);
    type.action = bcmSwitchEventForceUnmask;
    type.index = BCM_CORE_ALL;

    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, type, enable));

    type.action = bcmSwitchEventMask;
    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, type, !enable));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
dnx_sat_init_internal(
    int unit)
{
    uint32 freq_hz = 0;
    int granularity = 1000;     
    uint8 dma_event_interface_enable = 0;
    int gtf_id;

    
    dnx_sat_init_t dnx_sat_init;
    dnx_sat_gtf_pkt_hdr_tbl_info_t pkt_hdr_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    
    freq_hz = dnx_data_device.general.core_clock_khz_get(unit) * 1000;  

    granularity = DNX_DATA_MAX_SAT_GENERAL_GRANULARITY;
    dnx_sat_init.num_clks_sec = freq_hz;
    dnx_sat_init.rate_num_clks_cycle = freq_hz / (granularity / DNX_SAT_BITS_PER_BYTE); 

    SHR_IF_ERR_EXIT(dnx_sat_timer_cfg_init(unit, &dnx_sat_init));
    SHR_IF_ERR_EXIT(dnx_sat_credit_cfg_set(unit, DNX_SAT_DEFAULT_CREDIT_BACKPRESSURE));

    SHR_IF_ERR_EXIT(dnx_sat_evc_paring_init(unit));

    sal_memset(&pkt_hdr_info, 0x0, sizeof(dnx_sat_gtf_pkt_hdr_tbl_info_t));
    SHR_IF_ERR_EXIT(dnx_am_template_sat_pkt_header_create(unit, DNX_SAT_PKT_HEADER_DEFAULT_PROFILE, &pkt_hdr_info));

    dma_event_interface_enable = dnx_data_oam.oamp.oamp_fifo_dma_report_interface_enable_get(unit);
    
    if (dma_event_interface_enable)
    {
        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_stat_dma_init(unit));
    }
    else
    {
        
        if (!soc_is(unit, J2P_DEVICE))
        {
            SHR_IF_ERR_EXIT(dnx_sat_init_interrupt(unit));
        }
    }
    for (gtf_id = DNX_SAT_GTF_ID_MIN; gtf_id <= DNX_SAT_GTF_ID_MAX; gtf_id++)
    {
        SHR_IF_ERR_EXIT(dnx_sat_gtf_config_rate_set(unit, gtf_id, 0, DNX_SAT_GTF_RATE_CMP_CONVERSION, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

 
shr_error_e
dnx_sat_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sat_init_internal(unit));
exit:
    SHR_FUNC_EXIT;
}

 
shr_error_e
dnx_sat_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_CONT(dnx_oam_oamp_stat_dma_deinit(unit));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

 
static shr_error_e
dnx_sat_gen_cfg_get(
    int unit,
    bcm_sat_config_t * conf)
{
    uint32 entry_handle_id;
    uint32 field_data_crc = 0;
    uint32 field_rev_crc = 0;
    uint32 field_crc_high = 0;
    uint32 field_tod = 0;
    uint32 field_prbs = 0;
    uint32 field_twamp_mode = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_GEN_CONFIG, &entry_handle_id));

    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RVERSED_DATA_FOR_CRC, INST_SINGLE, &field_data_crc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INVERT_REVERSED_CRC, INST_SINGLE, &field_rev_crc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RST_CRC_TO_HIGH, INST_SINGLE, &field_crc_high);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_USE_NTP_TOD, INST_SINGLE, &field_tod);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_USE_NXOR_FOR_PRBS, INST_SINGLE, &field_prbs);
    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_twamp_mode))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TWAMP_MODE, INST_SINGLE, &field_twamp_mode);
    }
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (field_data_crc)
    {
        conf->config_flags |= BCM_SAT_CONFIG_CRC_REVERSED_DATA;
    }
    if (field_rev_crc)
    {
        conf->config_flags |= BCM_SAT_CONFIG_CRC_INVERT;
    }
    if (field_crc_high)
    {
        conf->config_flags |= BCM_SAT_CONFIG_CRC_HIGH_RESET;
    }
    conf->timestamp_format = (field_tod ? bcmSATTimestampFormatNTP : bcmSATTimestampFormatIEEE1588v1);
    if (field_twamp_mode)
    {
        conf->config_flags |= BCM_SAT_CONFIG_TWAMP_MODE;
    }
    if (field_prbs)
    {
        conf->config_flags |= BCM_SAT_CONFIG_PRBS_USE_NXOR;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 
static shr_error_e
dnx_sat_config_get_verify(
    int unit,
    bcm_sat_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(conf, _SHR_E_PARAM, "conf ");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_sat_config_get(
    int unit,
    bcm_sat_config_t * conf)
{
    bcm_sat_config_t dnx_conf;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    
    SHR_INVOKE_VERIFY_DNX(dnx_sat_config_get_verify(unit, conf));

    sal_memset(&dnx_conf, 0, sizeof(bcm_sat_config_t));
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) \n", BSL_FUNC, BSL_LINE, unit));

    SHR_IF_ERR_EXIT(dnx_sat_gen_cfg_get(unit, &dnx_conf));

    conf->config_flags = dnx_conf.config_flags;
    conf->timestamp_format = dnx_conf.timestamp_format;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

 
static shr_error_e
dnx_sat_gen_cfg_set(
    int unit,
    bcm_sat_config_t * conf)
{
    uint32 entry_handle_id;
    uint32 field = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_GEN_CONFIG, &entry_handle_id));

    
    field = (conf->config_flags & BCM_SAT_CONFIG_CRC_REVERSED_DATA ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RVERSED_DATA_FOR_CRC, INST_SINGLE, field);

    field = (conf->config_flags & BCM_SAT_CONFIG_CRC_INVERT ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INVERT_REVERSED_CRC, INST_SINGLE, field);

    field = (conf->config_flags & BCM_SAT_CONFIG_CRC_HIGH_RESET ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RST_CRC_TO_HIGH, INST_SINGLE, field);

    field = (conf->timestamp_format == bcmSATTimestampFormatNTP ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NTP_TOD, INST_SINGLE, field);

    if (dnx_data_sat.rx_flow_param.feature_get(unit, dnx_data_sat_rx_flow_param_twamp_mode))
    {
        field = (conf->config_flags & BCM_SAT_CONFIG_TWAMP_MODE ? 1 : 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TWAMP_MODE, INST_SINGLE, field);
    }

    field = (conf->config_flags & BCM_SAT_CONFIG_PRBS_USE_NXOR ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NXOR_FOR_PRBS, INST_SINGLE, field);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    
    if (dnx_data_sat.tx_flow_param.feature_get(unit, dnx_data_sat_tx_flow_param_seq_number_wrap_around))
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_SEQ_NUM_WRAP_AROUND, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEQ_NUM_WRAP_AROUND, INST_SINGLE,
                                     conf->seq_number_wrap_around);
        
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 
static shr_error_e
dnx_sat_config_set_verify(
    int unit,
    bcm_sat_config_t * conf)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(conf, _SHR_E_PARAM, "conf ");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_sat_config_set(
    int unit,
    bcm_sat_config_t * conf)
{
    bcm_sat_config_t dnx_conf;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    
    SHR_INVOKE_VERIFY_DNX(dnx_sat_config_set_verify(unit, conf));

    SHR_RANGE_VERIFY(conf->timestamp_format, bcmSATTimestampFormatIEEE1588v1, bcmSATTimestampFormatNTP, _SHR_E_PARAM,
                     "timestamp_format");
    LOG_VERBOSE(BSL_LS_BCM_SAT,
                ("%s %d: u(%d) timestamp_format (%d) config_flags(%d)\n", BSL_FUNC, BSL_LINE, unit,
                 conf->timestamp_format, conf->config_flags));

    sal_memset(&dnx_conf, 0, sizeof(bcm_sat_config_t));
    dnx_conf.config_flags = conf->config_flags;
    dnx_conf.timestamp_format = conf->timestamp_format;

    SHR_IF_ERR_EXIT(dnx_sat_gen_cfg_set(unit, &dnx_conf));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_sat_trap_data_set(
    int unit,
    dnx_sat_data_t * psat_data,
    int index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TRAP_DATA_SW, &entry_handle_id));

    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TRAP_INDEX, index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SSID, INST_SINGLE, psat_data->trap_data[index].ssid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_DATA, INST_SINGLE,
                                 psat_data->trap_data[index].trap_data);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_MASK, INST_SINGLE,
                                 psat_data->trap_data[index].trap_mask);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 
shr_error_e
dnx_sat_data_set(
    int unit,
    dnx_sat_data_t * psat_data)
{
    uint32 entry_handle_id;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_STATE_SW, &entry_handle_id));

    
    for (index = 0; index < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_ID_ENTRIES; index++)
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_ID, index,
                                     psat_data->ctf_trap_id[index]);
    }

    for (index = 0; index <= DNX_SAT_GTF_ID_MAX; index++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GTF_HEADER_ID, index,
                                     psat_data->gtf_header_id[index]);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_MODE, INST_SINGLE, psat_data->rate_mode);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GRANULARITY_SET, INST_SINGLE,
                                 psat_data->granularity_flag);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    for (index = 0; index < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_DATA_ENTRIES; index++)
    {
        SHR_IF_ERR_EXIT(dnx_sat_trap_data_set(unit, psat_data, index));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 
static shr_error_e
dnx_sat_trap_data_get(
    int unit,
    dnx_sat_data_t * psat_data,
    int index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TRAP_DATA_SW, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_TRAP_INDEX, index);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SSID, INST_SINGLE, &psat_data->trap_data[index].ssid);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_DATA, INST_SINGLE,
                               &psat_data->trap_data[index].trap_data);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_MASK, INST_SINGLE,
                               &psat_data->trap_data[index].trap_mask);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

  
shr_error_e
dnx_sat_data_get(
    int unit,
    dnx_sat_data_t * psat_data)
{
    uint32 entry_handle_id;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_STATE_SW, &entry_handle_id));

     
    for (index = 0; index < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_ID_ENTRIES; index++)
    {
        
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CTF_TRAP_ID, index,
                                   &psat_data->ctf_trap_id[index]);
    }
    for (index = 0; index <= DNX_SAT_GTF_ID_MAX; index++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GTF_HEADER_ID, index,
                                   &psat_data->gtf_header_id[index]);
    }

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_RATE_MODE, INST_SINGLE, &(psat_data->rate_mode));

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_GRANULARITY_SET, INST_SINGLE,
                              &(psat_data->granularity_flag));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    for (index = 0; index < DNX_DATA_MAX_SAT_COLLECTOR_TRAP_DATA_ENTRIES; index++)
    {
        SHR_IF_ERR_EXIT(dnx_sat_trap_data_get(unit, psat_data, index));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_sat_event_register_verify(
    int unit,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "user_data ");

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_sat_event_register(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb,
    void *user_data)
{
    bcm_sat_event_type_t type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_sat_event_register_verify(unit, user_data));

    for (type = 0; type < bcmSATEventCount; type++)
    {
        if (type == event_type)
        {
            if (_g_dnx_sat_event_cb[unit][type] && (_g_dnx_sat_event_cb[unit][type] != cb))
            {
                
                SHR_ERR_EXIT(_SHR_E_EXISTS, "EVENT %d already has a registered callback ", type);
            }
            _g_dnx_sat_event_cb[unit][type] = cb;
            _g_dnx_sat_event_ud[unit][type] = user_data;
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_sat_event_unregister_verify(
    int unit,
    bcm_sat_event_type_t event_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_RANGE_VERIFY(event_type, bcmSATEventReport, bcmSATEventCount, _SHR_E_PARAM, "event_type");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_sat_ctf_report_process(
    int unit,
    dnx_sat_event_data_t * event)
{
    bcm_sat_report_event_data_t report;
    bcm_sat_event_type_t type = bcmSATEventReport;
    SHR_FUNC_INIT_VARS(unit);

    if (!event || (!event->is_valid))
    {
        SHR_IF_ERR_EXIT(BCM_E_PARAM);
    }
    sal_memset(&report, 0, sizeof(bcm_sat_report_event_data_t));

    report.ctf_id = event->flow_id;
    if (event->d_set)
    {
        report.delay = event->frame_delay;
        report.delay_valid = 1;
    }

    if (event->s_set)
    {
        report.sequence_number = event->seq_num;
        report.sequence_number_valid = 1;
    }

    if (_g_dnx_sat_event_cb[unit][type])
    {
        _g_dnx_sat_event_cb[unit][type] (unit, type, (void *) &report, _g_dnx_sat_event_ud[unit][type]);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_sat_event_unregister(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb)
{
    bcm_sat_event_type_t type;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    
    SHR_INVOKE_VERIFY_DNX(dnx_sat_event_unregister_verify(unit, event_type));

    for (type = 0; type < bcmSATEventCount; type++)
    {
        if (type == event_type)
        {
            if (_g_dnx_sat_event_cb[unit][type] && (_g_dnx_sat_event_cb[unit][type] != cb))
            {
                
                SHR_ERR_EXIT(_SHR_E_EXISTS, "EVENT %d already has a registered callback ", type);
            }
            _g_dnx_sat_event_cb[unit][type] = NULL;
            _g_dnx_sat_event_ud[unit][type] = NULL;
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
