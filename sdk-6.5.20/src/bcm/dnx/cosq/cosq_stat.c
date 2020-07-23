/** \file cosq_stat.c
 * 
 *
 * General cosq statistics (CRPS and statistic interface and meters) functionality \n
 * relevant for ingress and egress stats
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
 * Include files.
 * {
 */
#include <bcm/cosq.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/cosq_stat.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION (8)

/*
 * }
 */

/**
* \brief
*     set a mapping table from voq to object_stat_id.
*     the input to the table refer to the base octet voqs, and the mapping inside the eight is 1x1.
*     It means that table do not get the 3 LSB bits of the voq.
*     the output value of the table refer also to the octet base object_id,
*     therfore, the HW add 3 LSB bits to the given value in order to create 1x1 object_stat_id mapping inside the octet.
* \param [in] unit -unit id
* \param [in] gport -cosq gport. only ingress ucast/mcast queues allowed
* \param [in] object_stat_val -the object stat value to set in the table (without the 3 LSB bits) (3 LSB bits must be zero)
* \return
*    shr_error_e
* \remark
*  1.  The object_stat_id (which is the counter pointer that CRPS gets) may be modified in the pipe after the mapping table
*   (only 3 LSB bits) in order to count several queues toghether.
*  2. symetric core configuration
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_stat_object_mapping_set(
    int unit,
    bcm_gport_t gport,
    uint32 object_stat_val)
{
    uint32 base_octet_queue_id;
    uint32 voq;
    uint32 entry_handle_id;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ITM_INT_STAT_PTR_MAP, &entry_handle_id));

    /** Setting key fields */
    voq = BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ?
        BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport) : BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
    /** table get as input the base octet voq shifted right by 3 */
    base_octet_queue_id = (voq - (voq % DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION)) / DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BASE_OCTET_VOQ, base_octet_queue_id);

    /** get core id */
    core_id = BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ?
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport) : BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);

    /** Set Key Fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_PTR, INST_SINGLE,
                                 (object_stat_val / DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION));
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*     get from mapping table the object_stat_id for a given voq.
*     it calc the base of the voq octet, get the hw value and calculate the object_stat_id to the specific voq asked in the octet.
* \param [in] unit -unit id
* \param [in] gport -cosq gport. only ingress ucast/mcast queues allowed
* \param [out] object_stat_val -the object stat value mapped for the give voq
* \return
*    shr_error_e
* \remark
*   1. The object_stat_id (which is the counter pointer that CRPS gets) may be modified in the pipe after the mapping table
*   (only 3 LSB bits) in order to count several queues toghether.
*  2. symetric core configuration
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_stat_object_mapping_get(
    int unit,
    bcm_gport_t gport,
    uint32 *object_stat_val)
{
    uint32 entry_handle_id;
    uint32 voq, base_octet_queue_id;
    uint32 data;
    uint32 core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ITM_INT_STAT_PTR_MAP, &entry_handle_id));

    /** Setting key fields */
    voq = BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ?
        BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport) : BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);

    /** get core id */
    core_id = BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ?
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport) : BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);

    /** table get as input the base octet voq shifted right by 3 */
    base_octet_queue_id = (voq - (voq % DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION)) / DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BASE_OCTET_VOQ, base_octet_queue_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STAT_PTR, INST_SINGLE, &data);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** the hw table hold the value without the 3 LSB bits. therfore, need to multiple in 8 and add the 1x1 mapping according to the specific voq in the octet*/
    *object_stat_val = (data * DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION) + (voq % DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*     verification for the parameters of API bcm_dnx_cosq_stat_obj_map_set
* \param [in] unit -unit id
* \param [in] flags -flags
* \param [in] key -key tructure
* \param [in] object_stat_id -object_stat_id
* \return
*    shr_error_e
* \remark
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_stat_obj_map_set_verify(
    int unit,
    int flags,
    bcm_cosq_obj_map_key_t * key,
    uint32 object_stat_id)
{
    int core_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");

    if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(key->gport) == FALSE) && (BCM_GPORT_IS_MCAST_QUEUE_GROUP(key->gport) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport=%d invalid. Types allowed: ingress ucast/mcast voq", key->gport);
    }
    if ((object_stat_id % DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid object_stat_id=%d. must be devided by %d",
                     object_stat_id, DNX_COSQ_STAT_OBJ_MAPPING_RESOLUTION);
    }

    /** core id range verify */
    core_id = BCM_GPORT_IS_UCAST_QUEUE_GROUP(key->gport) ?
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(key->gport) : BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(key->gport);
    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     verification for the parameters of API bcm_dnx_cosq_stat_obj_map_get
* \param [in] unit -unit id
* \param [in] flags -flags
* \param [in] key -key tructure
* \param [in] object_stat_id -object_stat_id
* \return
*    shr_error_e
* \remark
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_stat_obj_map_get_verify(
    int unit,
    int flags,
    bcm_cosq_obj_map_key_t * key,
    uint32 *object_stat_id)
{
    int core_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(object_stat_id, _SHR_E_PARAM, "object_stat_id");

    if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(key->gport) == FALSE) && (BCM_GPORT_IS_MCAST_QUEUE_GROUP(key->gport) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport=%d invalid. Types allowed: ingress ucast/mcast voq", key->gport);
    }

    /** core id range verify */
    core_id = BCM_GPORT_IS_UCAST_QUEUE_GROUP(key->gport) ?
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(key->gport) : BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(key->gport);
    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   set a list of parameters per command_id, in order to generate statistics pointers toward the CRPS.
* \param [in] unit -unit id
* \param [in] command_id -command_id key to the table
* \param [in] config -strucutre of the relevant parameters that need to configure
* \return
*    shr_error_e
* \remark
*   symetric core configuration
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_stat_itm_config_set(
    int unit,
    int command_id,
    bcm_cosq_stat_info_t * config)
{
    int shift_val;
    uint32 entry_handle_id;
    uint32 temp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ITM_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    /** Setting value fields */
    /** enable/disable queue maping */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ITM_QUEUE_MAPPING_ENABLE, INST_SINGLE,
                                 config->enable_mapping);
    /** set the shift value in order to count several queues together. */
    temp = (config->num_queues_per_entry - 1);
    SHR_BITCOUNT_RANGE(&temp, shift_val, 0, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ITM_COUNTER_POINTER_SHIFT, INST_SINGLE,
                                 (uint32) shift_val);
    /** set the range of the voqs to count */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ITM_QNUM_LOW, INST_SINGLE, config->queue_first);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRPS_ITM_QNUM_HIGH, INST_SINGLE, config->queue_last);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   get a list of parameters per command_id, that generates statistics pointers toward the CRPS.
* \param [in] unit -unit id
* \param [in] command_id -key to the table
* \param [out] config -strucutre of the relevant parameters to get from hw
* \return
*    shr_error_e
* \remark
*   symetric core configuration, therefore, taking the value from core=0
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_stat_itm_config_get(
    int unit,
    int command_id,
    bcm_cosq_stat_info_t * config)
{
    uint32 entry_handle_id;
    uint32 data1, data2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CRPS_ITM_INTERFACE_CFG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID, command_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

    /** Setting pointers value to receive the fields */
    /** get enable/disable queue maping */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ITM_QUEUE_MAPPING_ENABLE, INST_SINGLE, &data2);
    /** get the shift value in order to count several queues together */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ITM_COUNTER_POINTER_SHIFT, INST_SINGLE, &data1);
    /** get the range of the voqs to count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ITM_QNUM_LOW, INST_SINGLE, &config->queue_first);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CRPS_ITM_QNUM_HIGH, INST_SINGLE, &config->queue_last);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

     /* coverity[Out-of-bounds:FALSE]  */
    SHR_BITSET(&config->num_queues_per_entry, data1);
    config->enable_mapping = (int) data2;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_stat_config_init(
    int unit,
    int itm_nof_commands)
{
    bcm_cosq_stat_info_t info;
    int cmd_idx;
    SHR_FUNC_INIT_VARS(unit);

    info.enable_mapping = 0;
    info.num_queues_per_entry = 1;
    info.queue_first = 0;
    /** set the range to the maximum, in order to allow counting all queues */
    info.queue_last = dnx_data_ipq.queues.nof_queues_get(unit) - 1;

    for (cmd_idx = 0; cmd_idx < itm_nof_commands; cmd_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_stat_itm_config_set(unit, cmd_idx, &info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     verification for the parameters of API bcm_cosq_stat_info_set
* \param [in] unit -unit id
* \param [in] flags -NONE
* \param [in] key -key to the table. hold the command_id
* \param [in] config -strucutre of the relevant parameters that need to configure
* \return
*    shr_error_e
* \remark
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_stat_info_set_verify(
    int unit,
    int flags,
    bcm_cosq_stat_info_key_t * key,
    bcm_cosq_stat_info_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    if (config->enable_mapping != TRUE && config->enable_mapping != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "enable_mapping=%d invalid. allowed: TRUE/FALSE", config->enable_mapping);
    }
    if (config->num_queues_per_entry != 1 && config->num_queues_per_entry != 2 &&
        config->num_queues_per_entry != 4 && config->num_queues_per_entry != 8)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "num_queues_per_entry=%d invalid. allowed: 1/2/4/8", config->num_queues_per_entry);
    }
    if (config->queue_first >= dnx_data_ipq.queues.nof_queues_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "queue_first=%d invalid. max allowed: %d",
                     config->queue_first, dnx_data_ipq.queues.nof_queues_get(unit) - 1);
    }
    if (config->queue_last >= dnx_data_ipq.queues.nof_queues_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "queue_last=%d invalid. max allowed: %d",
                     config->queue_last, dnx_data_ipq.queues.nof_queues_get(unit) - 1);
    }
    if (config->queue_last < config->queue_first)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "queue_last=%d smaller than queue_first=%d",
                     config->queue_last, config->queue_first);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     verification for the parameters of API bcm_cosq_stat_info_get
* \param [in] unit -unit id
* \param [in] flags -NONE
* \param [in] key -key to the table. hold the command_id
* \param [in] config -strucutre of the relevant parameters to get from hw
* \return
*    shr_error_e
* \remark
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_stat_info_get_verify(
    int unit,
    int flags,
    bcm_cosq_stat_info_key_t * key,
    bcm_cosq_stat_info_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*     API to configure ingress_VOQ>object_stat_id (counter_pointer) mapping
* \param [in] unit -unit id
* \param [in] flags -NONE
* \param [in] key -hold the gport
* \param [in] object_stat_id -object_stat_id
* \return
*    shr_error_e as int
* \remark
* \see
*   NONE
*/
int
bcm_dnx_cosq_stat_obj_map_set(
    int unit,
    int flags,
    bcm_cosq_obj_map_key_t * key,
    uint32 object_stat_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_stat_obj_map_set_verify(unit, flags, key, object_stat_id));
    SHR_IF_ERR_EXIT(dnx_cosq_stat_object_mapping_set(unit, key->gport, object_stat_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*     API to get the object_stat_id (counter_pointer) for given VOQ from the mapping table
* \param [in] unit -unit id
* \param [in] flags -NONE
* \param [in] key -hold the gport
* \param [out] object_stat_id -object_stat_id
* \return
*    shr_error_e as int
* \remark
* \see
*   NONE
*/
int
bcm_dnx_cosq_stat_obj_map_get(
    int unit,
    int flags,
    bcm_cosq_obj_map_key_t * key,
    uint32 *object_stat_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_stat_obj_map_get_verify(unit, flags, key, object_stat_id));
    SHR_IF_ERR_EXIT(dnx_cosq_stat_object_mapping_get(unit, key->gport, object_stat_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   API configure cosq statistics parameters per command_id, in order to
*  generate the counter pointer toward the counter processor.
* \param [in] unit -unit id
* \param [in] flags -NONE
* \param [in] key -hold the command_id
* \param [in] config -list of parameters need to configure for the ingress queues
* \return
*    shr_error_e as int
* \remark
* \see
*   NONE
*/
int
bcm_dnx_cosq_stat_info_set(
    int unit,
    int flags,
    bcm_cosq_stat_info_key_t * key,
    bcm_cosq_stat_info_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_stat_info_set_verify(unit, flags, key, config));
    SHR_IF_ERR_EXIT(dnx_cosq_stat_itm_config_set(unit, key->command_id, config));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   API get cosq statistics parameters per command_id, in order to
*  generate the counter pointer toward the counter processor.
* \param [in] unit -unit id
* \param [in] flags -NONE
* \param [in] key -hold the command_id
* \param [out] config -list of parameters configured for the ingress queues
* \return
*    shr_error_e as int
* \remark
* \see
*   NONE
*/
int
bcm_dnx_cosq_stat_info_get(
    int unit,
    int flags,
    bcm_cosq_stat_info_key_t * key,
    bcm_cosq_stat_info_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_stat_info_get_verify(unit, flags, key, config));
    SHR_IF_ERR_EXIT(dnx_cosq_stat_itm_config_get(unit, key->command_id, config));

exit:
    SHR_FUNC_EXIT;
}
/** see .h file */
shr_error_e
dnx_cosq_stat_discard_egress_meter_set(
    int unit,
    uint32 flags)
{
    uint32 entry_handle_id;
    uint32 discard_dp[] = { BCM_COSQ_DISCARD_COLOR_GREEN, BCM_COSQ_DISCARD_COLOR_YELLOW,
        BCM_COSQ_DISCARD_COLOR_RED, BCM_COSQ_DISCARD_COLOR_BLACK
    };
    dbal_table_field_info_t field_info;
    int dp_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_EGRESS_DISCARD_ENABLE, DBAL_FIELD_DP,
                                               TRUE, 0, 0, &field_info));

    if (!(flags & BCM_COSQ_DISCARD_ENABLE))
    {
        /** clear the table - disable drop */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGRESS_DISCARD_ENABLE, &entry_handle_id));

        /** go over all DPs and update the flags */
        for (dp_index = 0; dp_index <= field_info.max_value; dp_index++)
        {
            /** Setting key fields */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp_index);
            /** Replace dbal_table_clear with dbal_entry_clear to support ER */
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGRESS_DISCARD_ENABLE, &entry_handle_id));
        /** set the table value to TRUE - enable drop */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, TRUE);
        /** per Dp set the key and commit */
        for (dp_index = 0; dp_index <= field_info.max_value; dp_index++)
        {
            if (flags & discard_dp[dp_index])
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp_index);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_stat_discard_egress_meter_get(
    int unit,
    uint32 *flags)
{
    uint32 entry_handle_id;
    uint32 discard_dp[] = { BCM_COSQ_DISCARD_COLOR_GREEN, BCM_COSQ_DISCARD_COLOR_YELLOW,
        BCM_COSQ_DISCARD_COLOR_RED, BCM_COSQ_DISCARD_COLOR_BLACK
    };
    dbal_table_field_info_t field_info;
    int dp_index;
    uint32 enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_EGRESS_DISCARD_ENABLE, DBAL_FIELD_DP,
                                               TRUE, 0, 0, &field_info));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGRESS_DISCARD_ENABLE, &entry_handle_id));

    /** go over all DPs and update the flags if the current DP is enabled */
    for (dp_index = 0; dp_index <= field_info.max_value; dp_index++)
    {
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp_index);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &enable);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (enable == TRUE)
        {
            (*flags) |= (BCM_COSQ_DISCARD_ENABLE | discard_dp[dp_index]);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
