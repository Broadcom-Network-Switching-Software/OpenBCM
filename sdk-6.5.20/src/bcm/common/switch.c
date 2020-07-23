/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       switch.c
 * Purpose:    Switch common APIs
 */

#include <soc/drv.h>
#include <bcm/switch.h>
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <appl/diag/shell.h>
#include <bcm_int/control.h>

#if defined(BCM_LTSW_SUPPORT)
#include <bcm_int/ltsw/issu.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager.h>
#endif

#if (defined(BCM_DPP_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#define SOC_SYNC_WHEN_NOT_DIRTY ((SOC_IS_DPP(unit) || SOC_IS_DFE(unit)) ? 1 : 0)
#else
#define SOC_SYNC_WHEN_NOT_DIRTY 0
#endif

#ifdef BCM_DNX_SUPPORT
/*
 * indicates that the API does not require error recovery support
 */
#define SWITCH_ERR_RECOVERY_NOT_NEEDED(_unit)\
if (SOC_IS_DNX(_unit))                       \
{                                            \
    DNX_ERR_RECOVERY_NOT_NEEDED(_unit);      \
}
#else /* BCM_DNX_SUPPORT */
#define SWITCH_ERR_RECOVERY_NOT_NEEDED(_unit)
#endif /* BCM_DNX_SUPPORT */

/*
 * Function:
 *     bcm_switch_pkt_info_t_init
 * Description:
 *     Initialize an switch packet parameter strucutre
 * Parameters:
 *     pkt_info - pointer to switch packet parameter strucutre
 * Return: none
 */
void
bcm_switch_pkt_info_t_init(bcm_switch_pkt_info_t *pkt_info)
{
    sal_memset(pkt_info, 0, sizeof(bcm_switch_pkt_info_t));
}

/*
 * Function:
 *     bcm_switch_network_group_config_t_init
 * Description:
 *     Initialize a bcm_switch_network_group_config_t structure.
 * Parameters:
 *     config - pointer to switch network group config parameter strucutre
 * Return: none
 */
extern void bcm_switch_network_group_config_t_init(
    bcm_switch_network_group_config_t *config)
{
    sal_memset(config, 0, sizeof(bcm_switch_network_group_config_t));
}

/*
 * Function:
 *     bcm_switch_service_config_t_init
 * Description:
 *     Initialize a bcm_switch_service_config_t structure.
 * Parameters:
 *     config - pointer to service config parameter strucutre
 * Return: none
 */
extern void bcm_switch_service_config_t_init(
    bcm_switch_service_config_t *config)
{
    sal_memset(config, 0, sizeof(bcm_switch_service_config_t));
}

void bcm_switch_profile_mapping_t_init(bcm_switch_profile_mapping_t *profile_mapping)
{
    if (profile_mapping != NULL) {
        sal_memset(profile_mapping, 0, sizeof(bcm_switch_profile_mapping_t));
    }
}

/*
 * Function:
 *     bcm_switch_hash_entry_config_t_init
 * Description:
 *     Initialize a bcm_switch_reflector_data_t structure.
 * Parameters:
 *     data - pointer to data parameter structure
 * Return: none
 */

void
bcm_switch_reflector_data_t_init(bcm_switch_reflector_data_t *data)
{
    if (data != NULL) {
        sal_memset(data, 0, sizeof(bcm_switch_reflector_data_t));
    }
}

/*
 * Function:
 *     bcm_switch_hash_entry_config_t_init
 * Description:
 *     Initialize a bcm_switch_hash_entry_config_t structure.
 * Parameters:
 *     config - pointer to flex hash config parameter strucutre
 * Return: none
 */

void
bcm_switch_hash_entry_config_t_init(bcm_switch_hash_entry_config_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(bcm_switch_hash_entry_config_t));
    }
}

/* Initialize the structure bcm_switch_olp_l2_addr_t to default values */
void bcm_switch_olp_l2_addr_t_init(
    bcm_switch_olp_l2_addr_t *l2_addr)
{
    if (l2_addr != NULL) {
        sal_memset(l2_addr, 0, sizeof(bcm_switch_olp_l2_addr_t));
    }
}    

/*
 * Function:
 *     bcm_switch_agm_info_t_init
 * Description:
 *     Initialize an switch agm info parameter strucutre
 * Parameters:
 *     agm_info - pointer to switch agm info parameter strucutre
 * Return: none
 */
void
bcm_switch_agm_info_t_init(bcm_switch_agm_info_t *agm_info)
{
    sal_memset(agm_info, 0, sizeof(bcm_switch_agm_info_t));
}

/*
 * Function:
 *     bcm_switch_agm_stat_t_init
 * Description:
 *     Initialize an switch agm stat parameter strucutre
 * Parameters:
 *     agm_info - pointer to switch agm stat parameter strucutre
 * Return: none
 */
void
bcm_switch_agm_stat_t_init(bcm_switch_agm_stat_t *agm_stat)
{
    sal_memset(agm_stat, 0, sizeof(bcm_switch_agm_stat_t));
}

/*
 * Function:
 *     bcm_switch_match_config_info_t_init
 * Description:
 *     Initialize a bcm_switch_match_config_info_t structure.
 * Parameters:
 *     config_info - pointer to match config parameter strucutre
 * Return: none
 */
void bcm_switch_match_config_info_t_init(
    bcm_switch_match_config_info_t *config_info)
{
    if (config_info != NULL) {
        sal_memset(config_info, 0, sizeof(bcm_switch_match_config_info_t));
    }
}

/*
 * Function:
 *     bcm_switch_match_control_info_t_init
 * Description:
 *     Initialize a bcm_switch_match_control_info_t structure.
 * Parameters:
 *     control_info - pointer to match control parameter strucutre
 * Return: none
 */
void bcm_switch_match_control_info_t_init(
    bcm_switch_match_control_info_t *control_info)
{
    if (control_info != NULL) {
        sal_memset(control_info, 0, sizeof(bcm_switch_match_control_info_t));
    }
}

/*
 * Function:
 *     bcm_switch_encap_info_t_init
 * Description:
 *     Initialize a bcm_switch_encap_info_t structure.
 * Parameters:
 *     encap_info - pointer to switch encap parameter strucutre
 * Return: none
 */
void
bcm_switch_encap_info_t_init(bcm_switch_encap_info_t *encap_info)
{
    if (encap_info != NULL) {
        sal_memset(encap_info, 0, sizeof(bcm_switch_encap_info_t));
    }
}


/*
 * Function:
 *     bcm_switch_tpid_class_t_init
 * Description:
 *     Initialize a bcm_switch_tpid_class_t structure.
 * Parameters:
 *     tpid_class - pointer to tpid class parameter strucutre
 * Return: none
 */
void
bcm_switch_tpid_class_t_init(bcm_switch_tpid_class_t *tpid_class)
{
    if (tpid_class != NULL) {
        sal_memset(tpid_class, 0, sizeof(bcm_switch_tpid_class_t));
    }
}

/*
 * Function:
 *     bcm_switch_ser_log_info_t_init
 * Description:
 *     Initialize a bcm_switch_ser_log_info_t structure.
 * Parameters:
 *     info - pointer to ser log info parameter strucutre
 * Return: none
 */
void
bcm_switch_ser_log_info_t_init(bcm_switch_ser_log_info_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(bcm_switch_ser_log_info_t));
    }
}

/*
 * Function:
 *     bcm_switch_ser_error_stat_type_t_init
 * Description:
 *     Initialize a bcm_switch_ser_error_stat_type_t_init structure.
 * Parameters:
 *     stat_type - pointer to ser statistics type parameter strucutre
 * Return: none
 */
void
bcm_switch_ser_error_stat_type_t_init(bcm_switch_ser_error_stat_type_t *stat_type)
{
    if (stat_type != NULL) {
        sal_memset(stat_type, 0, sizeof(bcm_switch_ser_error_stat_type_t));
    }
}

/*
 * Function:
 *     bcm_switch_opaque_tag_config_t_init
 * Description:
 *     Initialize a bcm_switch_opaque_tag_params_t structure
 * Parameters:
 *     info - pointer to opaque tag parameters structure
 * Return: none
 */
void
bcm_switch_opaque_tag_config_t_init(bcm_switch_opaque_tag_params_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(bcm_switch_opaque_tag_params_t));
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT

/* Externs for bcm_<arch>_switch_control_get functions */
#define BCM_DLIST_ENTRY(_dtype) \
extern int bcm_##_dtype##_switch_control_set( \
    int unit, bcm_switch_control_t type, int arg);
#include <bcm_int/bcm_dlist.h>

/* Dispatch table for this function, which uses the externs above */
#define BCM_DLIST_ENTRY(_dtype)\
bcm_##_dtype##_switch_control_set,
static int (*_switch_control_set_disp[])(
    int unit, 
    bcm_switch_control_t type, 
    int arg) =
{
#include <bcm_int/bcm_dlist.h>
0
};

/* Externs for bcm_<arch>_switch_control_get functions */
#define BCM_DLIST_ENTRY(_dtype) \
extern int bcm_##_dtype##_switch_control_get( \
    int unit, bcm_switch_control_t type, int *arg);
#include <bcm_int/bcm_dlist.h>

/* Dispatch table for this function, which uses the externs above */
#define BCM_DLIST_ENTRY(_dtype)\
bcm_##_dtype##_switch_control_get,
static int (*_switch_control_get_disp[])(
    int unit, 
    bcm_switch_control_t type, 
    int *arg) =
{
#include <bcm_int/bcm_dlist.h>
0
};


/**
 * Function:
 *      _bcm_switch_state_sync
 * Purpose:
 *      Sync warm boot cache if necessary
 * Parameters:
 *      unit - (IN) Unit number.
 *      dtype - (IN) Dispatch index
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

int
_bcm_switch_state_sync(int unit, bcm_dtype_t dtype)
{
    int rv = BCM_E_UNIT;
    int arg = 0;

    if (SOC_UNIT_VALID(unit)) {
#if defined(BCM_LTSW_SUPPORT)
        if (SOC_IS_LTSW(unit)) {
            return BCM_E_UNAVAIL;
        }
#endif
        rv = _switch_control_get_disp[dtype](unit,
                                             bcmSwitchControlAutoSync, &arg);
        if (BCM_SUCCESS(rv) && (arg != 0)) {
            if (SOC_CONTROL(unit)->scache_dirty || SOC_SYNC_WHEN_NOT_DIRTY) {
                rv = _switch_control_set_disp[dtype](unit,
                                                     bcmSwitchControlSync, 1);
            }
        }
    }

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_switch_obm_classifier_t_init
 * Purpose:
 *      Initialize the OBM  classifier struct
 * Parameters:
 *      switch_classifier - Pointer to the struct to be init'ed
 */

void
bcm_switch_obm_classifier_t_init(bcm_switch_obm_classifier_t *switch_classifier)
{
    if (switch_classifier != NULL) {
        sal_memset(switch_classifier, 0, sizeof(*switch_classifier));
    }
}

/*
 * Function:
 *      bcm_switch_obm_classifier_t_init
 * Purpose:
 *      Initialize the OBM  classifier struct
 * Parameters:
 *      switch_classifier - Pointer to the struct to be init'ed
 */

void
bcm_switch_appl_signature_info_t_init(
        bcm_switch_appl_signature_info_t *appl_signature_info)
{
    if (appl_signature_info != NULL) {
        sal_memset(appl_signature_info, 0, sizeof(*appl_signature_info));
    }
}
/*!
 * \brief Initialize a packet control action structure.
 *
 * \param [in|out] action Pointer to a packet control action structure.
 *
 * \retval NONE.
 */
void
bcm_switch_pkt_control_action_t_init(bcm_switch_pkt_control_action_t *action)
{
    if (action != NULL) {
        sal_memset(action, 0, sizeof(*action));
    }
}

/*!
 * \brief Initialize a packet integrity match attribute structure.
 *
 * \param [in|out] match Pointer to a packet integrity match attribute structure.
 *
 * \retval NONE.
 */
void
bcm_switch_pkt_integrity_match_t_init(bcm_switch_pkt_integrity_match_t *match)

{
    if (match != NULL) {
        sal_memset(match, 0, sizeof(*match));
    }
}

/*!
 * \brief Initialize a protocol match structure.
 *
 * \param [in|out] match Pointer to a protocol match structure.
 *
 * \retval NONE.
 */

void
bcm_switch_pkt_protocol_match_t_init(bcm_switch_pkt_protocol_match_t  *match)
{
    if (match != NULL) {
        sal_memset(match, 0, sizeof(*match));
    }
}

/*
 * Function:
 *      bcm_switch_drop_event_mon_t_init
 * Purpose:
 *      Initialize a bcm_switch_drop_event_mon_t structure.
 * Parameters:
 *      monitor - Pointer to a switch drop event monitor structure.
 * Return: none
 */
void
bcm_switch_drop_event_mon_t_init(bcm_switch_drop_event_mon_t *monitor)
{
    if (monitor != NULL) {
        sal_memset(monitor, 0, sizeof(*monitor));
        monitor->drop_event = -1;
    }
}

/*
 * Function:
 *      bcm_switch_trace_event_mon_t_init
 * Purpose:
 *      Initialize a bcm_switch_trace_event_mon_t structure.
 * Parameters:
 *      monitor - Pointer to a switch trace event monitor structure.
 * Return: none
 */
void
bcm_switch_trace_event_mon_t_init(bcm_switch_trace_event_mon_t *monitor)
{
    if (monitor != NULL) {
        sal_memset(monitor, 0, sizeof(*monitor));
        monitor->trace_event = -1;
    }
}

/*!
 * \brief Initialize a hash bank information structure.
 *
 * \param [in|out] bank_info Pointer to a hash bank information structure.
 *
 * \retval NONE.
 */
void
bcm_switch_hash_bank_info_init(bcm_switch_hash_bank_info_t *bank_info)
{
    if (bank_info != NULL) {
        sal_memset(bank_info, 0, sizeof(*bank_info));
        bank_info->bank_num = -1;
    }
}

/*!
 * \brief Initialize a hash bank control structure.
 *
 * \param [in|out] bank_control Pointer to a hash bank control structure.
 *
 * \retval NONE.
 */
void
bcm_switch_hash_bank_control_init(bcm_switch_hash_bank_control_t *bank_control)
{
    if (bank_control != NULL) {
        sal_memset(bank_control, 0, sizeof(*bank_control));
        bank_control->bank_num = -1;
    }
}

/*!
 * \brief Initialize an EP Recirculate NIH residence time field config structure.
 *
 * \param [in/out] nih_residence_time_field The EP Recirculate NIH residence time field config structure.
 *
 * \retval NONE.
 */
void
bcm_switch_ep_recirc_nih_residence_time_field_config_t_init(
    bcm_switch_ep_recirc_nih_residence_time_field_config_t *nih_residence_time_field)
{
    if (nih_residence_time_field != NULL) {
        sal_memset(nih_residence_time_field, 0, sizeof(*nih_residence_time_field));
    }
}

/*!
 * \brief Initialize an EP Recirculate NIH config structure.
 *
 * \param [in/out] nih_config The EP Recirculate NIH config structure.
 *
 * \retval NONE.
 */
void
bcm_switch_ep_recirc_nih_config_t_init(
    bcm_switch_ep_recirc_nih_config_t *nih_config)
{
    if (nih_config != NULL) {
        sal_memset(nih_config, 0, sizeof(*nih_config));
        (void)bcm_switch_ep_recirc_nih_residence_time_field_config_t_init(
            &(nih_config->res_time_field_cfg));
    }
}

/*!
 * \brief Initialize an EP Recirculate CPU DMA header timestamp field config structure.
 *
 * \param [in/out] cpu_dma_timestamp_field The EP Recirculate CPU DMA header timestamp field config structure.
 *
 * \retval NONE.
 */
void
bcm_switch_ep_recirc_cpu_dma_timestamp_field_config_t_init(
    bcm_switch_ep_recirc_cpu_dma_timestamp_field_config_t *cpu_dma_timestamp_field)
{
    if (cpu_dma_timestamp_field != NULL) {
        sal_memset(cpu_dma_timestamp_field, 0, sizeof(*cpu_dma_timestamp_field));
    }
}

/*!
 * \brief Initialize an EP Recirculate CPU DMA header config structure.
 *
 * \param [in/out] cpu_dma_config The EP Recirculate CPU DMA header config structure.
 *
 * \retval NONE.
 */
void
bcm_switch_ep_recirc_cpu_dma_config_t_init(
    bcm_switch_ep_recirc_cpu_dma_config_t *cpu_dma_config)
{
    if (cpu_dma_config!= NULL) {
        sal_memset(cpu_dma_config, 0, sizeof(*cpu_dma_config));
        (void)bcm_switch_ep_recirc_cpu_dma_timestamp_field_config_t_init(
                &(cpu_dma_config->timestamp_field_cfg));
    }
}

/*!
 * \brief Initialize an EP Recirculate profile structure.
 *
 * \param [in/out] recirc_profile The EP Recirculate profile config structure.
 *
 * \retval NONE.
 */
void
bcm_switch_ep_recirc_profile_t_init(
    bcm_switch_ep_recirc_profile_t *recirc_profile)
{
    if (recirc_profile != NULL) {
        sal_memset(recirc_profile, 0, sizeof(*recirc_profile));
        (void)bcm_switch_ep_recirc_nih_config_t_init(&(recirc_profile->nih_cfg));
        (void)bcm_switch_ep_recirc_cpu_dma_config_t_init(&(recirc_profile->cpu_dma_cfg));
    }
}

/*!
 * \brief Initialize an EP Recirculate drop event control structure.
 *
 * \param [in] drop_event_ctrl The EP recirculate drop event control structure.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
void
bcm_switch_ep_recirc_drop_event_control_t_init(
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl)
{
    if (drop_event_ctrl != NULL) {
        sal_memset(drop_event_ctrl, 0, sizeof(*drop_event_ctrl));
    }
}

/*!
 * \brief Initialize an EP Recirculate trace event control structure.
 *
 * \param [in] trace_event_ctrl The EP recirculate trace event control structure.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
void
bcm_switch_ep_recirc_trace_event_control_t_init(
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl)
{
    if (trace_event_ctrl != NULL) {
        sal_memset(trace_event_ctrl, 0, sizeof(*trace_event_ctrl));
    }
}

/*!
 * \brief ISSU upgrade start.
 *
 * This function initiates the ISSU process. It starts by loading the ISSU DLL.
 * It laters informs the system manager engine about the ongoing update by
 * assigning values to callback functions being executed during the pre config
 * stage.
 *
 * \param [in] from_ver Is the previous version of software that was running.
 * \param [in] to_ver Is the version of the current software.
  *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_switch_issu_upgrade_start(const char *from_ver, const char *to_ver)
{
#if defined(BCM_LTSW_SUPPORT)
    return bcmi_ltsw_issu_upgrade_start(from_ver, to_ver);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*!
 * \brief ISSU upgrade done.
 *
 * This function releases all the resources that were used by the ISSU. In
 * particular it unloads the ISSU DLL. It is assume that this function is being
 * called once all the units had completed their upgrade.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm_switch_issu_upgrade_done(void)
{
#if defined(BCM_LTSW_SUPPORT)
    return bcmi_ltsw_issu_upgrade_done();
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *     bcm_common_switch_led_uc_num_get
 * Purpose:
 *     Get number of LED microcontrollers.
 * Parameters:
 *     unit - (IN) Unit number.
 *     led_uc_num (OUT) Number of LED microcontrollers.
 * Returns:
 *     BCM_E_NONE               - Operation successful
 *     BCM_E_PARAM              - Invalid input parameter value
 *     BCM_E_UNAVAIL            - Feature Not supported
 */
int
bcm_common_switch_led_uc_num_get(int unit,
                                 int *led_uc_num)
{
    return soc_led_uc_num_get(unit, led_uc_num);
}

/*
 * Function:
 *     bcm_common_switch_led_control_data_read
 * Purpose:
 *     Read control data of LED firmware.
 * Parameters:
 *     unit - (IN) Unit number.
 *     led_uc - (IN) LED microcontroller number.
 *     offset - (IN) Position to start led control data read.
 *     data - (OUT) Read data buffer.
 *     len - (IN) Length of read data.
 * Returns:
 *      BCM_E_NONE               - Operation successful
 *      BCM_E_PARAM              - Invalid input parameter value
 *      BCM_E_UNAVAIL            - Feature Not supported
 */
int
bcm_common_switch_led_control_data_read(int unit,
                                        int led_uc,
                                        int offset,
                                        uint8 *data,
                                        int len)
{
    /* indicates that the API does not require error recovery support */
    SWITCH_ERR_RECOVERY_NOT_NEEDED(unit);
    return soc_led_control_data_read(unit, led_uc, offset, data, len);
}

/*
 * Function:
 *     bcm_common_switch_led_control_data_write
 * Purpose:
 *     Write control data of LED firmware.
 * Parameters:
 *     unit - (IN) Unit number.
 *     led_uc - (IN) LED microcontroller number.
 *     offset - (IN) Position to start led control data write.
 *     data - (IN) Write data buffer.
 *     len - (IN) Length of write data.
 * Returns:
 *     BCM_E_NONE               - Operation successful
 *     BCM_E_PARAM              - Invalid input parameter value
 *     BCM_E_UNAVAIL            - Feature Not supported
 */
int
bcm_common_switch_led_control_data_write(int unit,
                                         int led_uc,
                                         int offset,
                                         const uint8 *data,
                                         int len)
{
    /* indicates that the API does not require error recovery support */
    SWITCH_ERR_RECOVERY_NOT_NEEDED(unit);
    return soc_led_control_data_write(unit, led_uc, offset, data, len);
}

/*
 * Function:
 *     bcm_common_switch_led_fw_load
 * Purpose:
 *     Load LED firmware binary.
 * Parameters:
 *     unit - (IN) Unit number.
 *     led_uc - (IN) LED microcontroller number.
 *     data - (IN) Firmware binary content.
 *     len - (IN) Length of firmware binary.
 * Returns:
 *     BCM_E_NONE               - Operation successful
 *     BCM_E_PARAM              - Invalid input parameter value
 *     BCM_E_UNAVAIL            - Feature Not supported
 */
int
bcm_common_switch_led_fw_load(int unit,
                              int led_uc,
                              const uint8 *data,
                              int len)
{
    /* indicates that the API does not require error recovery support */
    SWITCH_ERR_RECOVERY_NOT_NEEDED(unit);
    return soc_led_fw_load(unit, led_uc, data, len);
}

/*
 * Function:
 *     bcm_common_switch_led_fw_start_get
 * Purpose:
 *     Get if LED firmware is started or not.
 * Parameters:
 *     unit - (IN) Unit number.
 *     led_uc - (IN) LED microcontroller number.
 *     start - (OUT)  Whether firmware is started(1) or stopped(0).
 * Returns:
 *     BCM_E_NONE               - Operation successful
 *     BCM_E_PARAM              - Invalid input parameter value
 *     BCM_E_UNAVAIL            - Feature Not supported
 */
int
bcm_common_switch_led_fw_start_get(int unit,
                                   int led_uc,
                                   int *start)
{
    /* indicates that the API does not require error recovery support */
    SWITCH_ERR_RECOVERY_NOT_NEEDED(unit);
    return soc_led_fw_start_get(unit, led_uc, start);
}

/*
 * Function:
 *     bcm_common_switch_led_fw_start_set
 * Purpose:
 *     Start/stop LED firmware.
 * Parameters:
 *     unit - (IN) Unit number.
 *     led_uc - (IN) LED microcontroller number.
 *     start - (IN) Value to request firmware to start(1) or stop(0).
 * Returns:
 *     BCM_E_NONE               - Operation successful
 *     BCM_E_PARAM              - Invalid input parameter value
 *     BCM_E_UNAVAIL            - Feature Not supported
 */
int
bcm_common_switch_led_fw_start_set(int unit,
                                   int led_uc,
                                   int start)
{
    /* indicates that the API does not require error recovery support */
    SWITCH_ERR_RECOVERY_NOT_NEEDED(unit);
    return soc_led_fw_start_set(unit, led_uc, start);
}

/*
 * Function:
 *     bcm_common_switch_led_port_to_uc_port_get
 * Purpose:
 *     Map physical port to LED microcontroller number and port index.
 * Parameters:
 *     unit - (IN) Unit number.
 *     port - (IN) Physical port number.
 *     led_uc - (OUT) LED microcontroller number.
 *     led_uc_port - (OUT) Port index controlled by corresponding LED microcontroller.
 * Returns:
 *     BCM_E_NONE               - Operation successful
 *     BCM_E_PARAM              - Invalid input parameter value
 *     BCM_E_UNAVAIL            - Feature Not supported
 */
int
bcm_common_switch_led_port_to_uc_port_get(int unit,
                                          int port,
                                          int *led_uc,
                                          int *led_uc_port)
{
    /* indicates that the API does not require error recovery support */
    SWITCH_ERR_RECOVERY_NOT_NEEDED(unit);
    return soc_led_port_to_uc_port_get(unit, port, led_uc, led_uc_port);
}

/*
 * Function:
 *     bcm_common_switch_led_port_to_uc_port_set
 * Purpose:
 *     Configure the mapping from physical port to LED microcontroller
 *     number and port index.
 * Parameters:
 *     unit - (IN) Unit number.
 *     port - (IN) Physical port number.
 *     led_uc - (IN) LED microcontroller number.
 *     led_uc_port - (IN) Port index controlled by corresponding LED microcontroller.
 * Returns:
 *     BCM_E_NONE               - Operation successful
 *     BCM_E_PARAM              - Invalid input parameter value
 *     BCM_E_UNAVAIL            - Feature Not supported
 */
int
bcm_common_switch_led_port_to_uc_port_set(int unit,
                                          int port,
                                          int led_uc,
                                          int led_uc_port)
{
    /* indicates that the API does not require error recovery support */
    SWITCH_ERR_RECOVERY_NOT_NEEDED(unit);
    return soc_led_port_to_uc_port_set(unit, port, led_uc, led_uc_port);
}
