/** \file compensation.c
 * 
 *
 * implementation of compensation related APIs
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

#include <shared/shrextend/shrextend_debug.h>
#include <shared/gport.h>
#include <bcm/types.h>
#include <bcm/stat.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ingress_compensation_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>

/*
 * Set ranges for ingress compensation
 */
#define DNX_COSQ_INGRESS_COMPENSATION_DELTA_VALUE_MIN (-128)
#define DNX_COSQ_INGRESS_COMPENSATION_DELTA_VALUE_MAX (127)

/**
* \brief
* Configure compensation delta to HW.
* This function is used to configure the following tables:
*     * INGRESS_COMPENSATION_PER_IN_PORT
*     * INGRESS_COMPENSATION_PER_VOQ
*     * INGRESS_COMPENSATION_PER_HEADER_APPEND_PTR
 */
static shr_error_e
dnx_cosq_ingress_compensation_delta_hw_set(
    int unit,
    bcm_core_t core_id,
    dbal_tables_e table_id,
    dbal_fields_e key_field_id,
    int key,
    int delta)
{
    uint32 fld_delta;
    int field_size;

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_id, key);

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, DBAL_FIELD_HEADER_DELTA, 0, 0, 0, &field_size));

    fld_delta = UTILEX_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, field_size);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_DELTA, INST_SINGLE, fld_delta);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get compensation delta from HW.
* This function is used to get configuration for the following tables:
*     * INGRESS_COMPENSATION_PER_IN_PORT
*     * INGRESS_COMPENSATION_PER_VOQ
*     * INGRESS_COMPENSATION_PER_HEADER_APPEND_PTR
 */
static shr_error_e
dnx_cosq_ingress_compensation_delta_hw_get(
    int unit,
    bcm_core_t core_id,
    dbal_tables_e table_id,
    dbal_fields_e key_field_id,
    int key,
    int *delta)
{
    uint32 fld_delta;
    int field_size;

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_id, key);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HEADER_DELTA, INST_SINGLE, &fld_delta);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, DBAL_FIELD_HEADER_DELTA, 0, 0, 0, &field_size));

    *delta = UTILEX_TWO_COMPLEMENT_INTO_SIGNED_NUM(fld_delta, field_size);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_ingress_compensation_voq_compesation_profile_hw_set(
    int unit,
    int core_id,
    uint32 voq,
    uint32 credit_class)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_INGRESS_COMPENSATION_VOQ_TO_CREDIT_CLASS_PROFILE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, voq);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMPENSATION_PROFILE, INST_SINGLE, credit_class);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_ingress_compensation_voq_compesation_profile_hw_get(
    int unit,
    int core_id,
    uint32 voq,
    uint32 *credit_class)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_INGRESS_COMPENSATION_VOQ_TO_CREDIT_CLASS_PROFILE_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, voq);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COMPENSATION_PROFILE, INST_SINGLE, credit_class);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Configure per VOQ compensation delta to HW.
 */
static shr_error_e
dnx_cosq_ingress_compensation_voq_delta_hw_set(
    int unit,
    int core,
    int compensation_profile,
    int delta)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_delta_hw_set(unit, core,
                                                               DBAL_TABLE_INGRESS_COMPENSATION_PER_VOQ,
                                                               DBAL_FIELD_COMPENSATION_PROFILE, compensation_profile,
                                                               delta));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get compensation delta per VOQ from HW.
*/
static shr_error_e
dnx_cosq_ingress_compensation_voq_delta_hw_get(
    int unit,
    int core,
    int compensation_profile,
    int *delta)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_delta_hw_get(unit, core,
                                                               DBAL_TABLE_INGRESS_COMPENSATION_PER_VOQ,
                                                               DBAL_FIELD_COMPENSATION_PROFILE, compensation_profile,
                                                               delta));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Configure compensation delta per Header Append Pointer to HW.
*/
static shr_error_e
dnx_cosq_ingress_compensation_hdr_append_ptr_delta_hw_set(
    int unit,
    int append_pointer,
    int delta)
{
    uint32 max_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_INGRESS_COMPENSATION_PER_HEADER_APPEND_PTR,
                                                          DBAL_FIELD_HEADER_APPEND_PTR /** field_id */ ,
                                                          1 /** is_key */ , 0, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE /** max value of the field */ ,
                                                          &max_value));

    if (append_pointer > max_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid append pointer id %d", append_pointer);
    }

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_delta_hw_set(unit, DBAL_CORE_ALL,
                                                               DBAL_TABLE_INGRESS_COMPENSATION_PER_HEADER_APPEND_PTR,
                                                               DBAL_FIELD_HEADER_APPEND_PTR, append_pointer, delta));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get HW compensation delta per Header Append Pointer from HW.
*/
static shr_error_e
dnx_cosq_ingress_compensation_hdr_append_ptr_delta_hw_get(
    int unit,
    int append_pointer,
    int *delta)
{
    uint32 max_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_INGRESS_COMPENSATION_PER_HEADER_APPEND_PTR,
                                                          DBAL_FIELD_HEADER_APPEND_PTR /** field_id */ ,
                                                          1 /** is_key */ , 0, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE /** max value of the field */ ,
                                                          &max_value));

    if (append_pointer > max_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid append pointer id %d", append_pointer);
    }

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_delta_hw_get(unit, DBAL_CORE_DEFAULT,
                                                               DBAL_TABLE_INGRESS_COMPENSATION_PER_HEADER_APPEND_PTR,
                                                               DBAL_FIELD_HEADER_APPEND_PTR, append_pointer, delta));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*      for dbal table INGRESS_COMPENSATION_PER_IN_PORT
*      set field IPP_HEADER_DELTA
* \param [in] unit -unit id
* \param [in] core_id - core_id
* \param [in] pp_port - pp_port
* \param [in] delta - delta value to be set
* \return
*      shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_ingress_compensation_in_port_delta_hw_set(
    int unit,
    bcm_core_t core_id,
    int pp_port,
    int delta)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_delta_hw_set(unit, core_id,
                                                               DBAL_TABLE_INGRESS_COMPENSATION_PER_IN_PORT,
                                                               DBAL_FIELD_PP_PORT, pp_port, delta));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*      for dbal table INGRESS_COMPENSATION_PER_IN_PORT get field
*      IPP_HEADER_DELTA
* \param [in] unit -unit id
* \param [in] core_id - core_id
* \param [in] pp_port - pp_port
* \param [out] delta - delta value to be set
* \return
*      shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_ingress_compensation_in_port_delta_hw_get(
    int unit,
    bcm_core_t core_id,
    int pp_port,
    int *delta)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_delta_hw_get(unit, core_id,
                                                               DBAL_TABLE_INGRESS_COMPENSATION_PER_IN_PORT,
                                                               DBAL_FIELD_PP_PORT, pp_port, delta));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set input port component of ingress compensation
 */
int
dnx_cosq_ingress_compensation_in_port_delta_set(
    int unit,
    bcm_port_t src_port,
    int delta)
{
    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF], temp_port_indx;
    int nof_cgm_entries;
    int core_id;

    shr_error_e rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the relevant core ID from the src_port */
    rc = dnx_algo_port_core_get(unit, src_port, &core_id);
    SHR_IF_ERR_EXIT(rc);

    rc = dnx_cosq_cgm_in_port_get(unit, src_port, &nof_cgm_entries, cgm_input_port);
    SHR_IF_ERR_EXIT(rc);

    for (temp_port_indx = 0; temp_port_indx < nof_cgm_entries; temp_port_indx++)
    {
        /** configure compensation in all lines matching the required local port */
        SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_in_port_delta_hw_set(unit, core_id,
                                                                           cgm_input_port[temp_port_indx], delta));
    }

    /** Last -- mark the port to have unchangable priority */
    SHR_IF_ERR_EXIT(dnx_cosq_port_fixed_priority_set(unit, src_port, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get input port component of ingress compensation
 */
int
dnx_cosq_ingress_compensation_in_port_delta_get(
    int unit,
    bcm_port_t src_port,
    int *delta)
{
    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF];
    int nof_cgm_entries;
    int core_id;

    shr_error_e rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the relevant core ID from the src_port */
    rc = dnx_algo_port_core_get(unit, src_port, &core_id);
    SHR_IF_ERR_EXIT(rc);

    rc = dnx_cosq_cgm_in_port_get(unit, src_port, &nof_cgm_entries, cgm_input_port);
    SHR_IF_ERR_EXIT(rc);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_in_port_delta_hw_get(unit, core_id, cgm_input_port[0], delta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set VOQ component of ingress compensation
 */
int
dnx_cosq_ingress_compensation_voq_delta_set(
    int unit,
    int core,
    int voq,
    int delta)
{
    uint32 old_comp_profile;
    int new_comp_profile;
    uint8 is_first, is_last;
    int core_idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        /** get old profile */
        SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_compesation_profile_hw_get
                        (unit, core_idx, voq, &old_comp_profile));

        /** exchange to a profile with the requested delta */
        SHR_IF_ERR_EXIT(dnx_ingress_compensation_db.
                        compensation.exchange(unit, core_idx, 0, &delta, old_comp_profile, NULL, &new_comp_profile,
                                              &is_first, &is_last));

        if (is_first)
        {
            /** configure compensation in HW */
            SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_delta_hw_set(unit, core_idx, new_comp_profile, delta));
        }
        if (is_last && new_comp_profile != old_comp_profile)
        {
            /** clear old profile compensation */
            SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_delta_hw_set(unit, core_idx, old_comp_profile, 0));
        }

        /** Set mapping to new profile */
        SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_compesation_profile_hw_set
                        (unit, core_idx, voq, new_comp_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get VOQ component of ingress compensation
 */
int
dnx_cosq_ingress_compensation_voq_delta_get(
    int unit,
    int core,
    int voq,
    int *delta)
{
    uint32 compensation_profile;
    SHR_FUNC_INIT_VARS(unit);

    /** get the profile mapped to the VOQ */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_compesation_profile_hw_get
                    (unit, core, voq, &compensation_profile));

    /** get compensation from HW */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_delta_hw_get(unit, core, compensation_profile, delta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set header append pointer component of ingress compensation
 */
int
dnx_cosq_ingress_compensation_hdr_append_ptr_delta_set(
    int unit,
    int append_ptr,
    int delta)
{
    SHR_FUNC_INIT_VARS(unit);

    /** configure compensation */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_hdr_append_ptr_delta_hw_set(unit, append_ptr, delta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get header append pointer component of ingress compensation
 */
int
dnx_cosq_ingress_compensation_hdr_append_ptr_delta_get(
    int unit,
    int append_ptr,
    int *delta)
{
    SHR_FUNC_INIT_VARS(unit);

    /** configure compensation */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_hdr_append_ptr_delta_hw_get(unit, append_ptr, delta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure negate for automatic compensation
 *        components. Configure all cores.
 *
 * @param unit
 *
 * @return shr_error_e
 */
shr_error_e
dnx_cosq_ingress_compensation_negate_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_COMPENSATION_DELTA_SIGNS, &entry_handle_id));

    /** Enable credit compensation negate */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_DELTA_NEG, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_DELTA_NEG, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_HEADER_TRUNCATE_SIZE_NEG, INST_SINGLE, 1);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  HW init of Scheduler Compensation
 */
shr_error_e
dnx_cosq_ingress_compensation_scheduler_hw_defaults_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Enable all compensation types apart of Per In Port
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_COMPENSATION_SCHEDULER_ENABLERS, &entry_handle_id));

    /** Set Range Key Fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CR_PROFILE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /** Setting value fields */
    /** Enable all compensation components apart of Per In Port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRUNCATE_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HDR_APPEND_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_ENABLE, INST_SINGLE, 0);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT | DBAL_COMMIT_OVERRUN));

    /*
     * Other HW Initializations
     */

    /*
     * Configure negate for automatic compensation components
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_negate_init(unit));
    /*
     * Configure truncate and Header Append modes
     */

    

#ifdef LEGACY_TRUNCATE_AND_NEGATE_CODE
    /** this is the legacy code which is used in QAX */
/* *INDENT-OFF* */

    /*
     * Set Header-Truncate & Header-Append-Ptr compensation modes to get
     * information about system headers and IRPP editing from PP */
    /*
       -   Nwk-Header-Truncate(8) = (Cfg-Truncate-Mode == 0) ? 0 :
          (Cfg-Truncate-Mode == 1) ? User-Header-1[23:16] :
          PPH.FWD_HEADER_OFFSET

          -   Nwk-Header-Append(8) =   (Cfg-Append-Mode == 0) ? 0 :
          (Cfg-Append-Mode == 1) ? User-Header-1[15:8] :  {3'b0, Out-LIF[17:13]}
    */
    COMPILER_64_ZERO(data64);
    SHR_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, &data64));
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &data64, NWK_HDR_TRUNCATE_MODEf, 0x2);
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &data64, NWK_HDR_APPEND_MODEf, 0x2);
    SHR_IF_ERR_EXIT(WRITE_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, data64));
/* *INDENT-ON* */
#endif

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Set per-in-port enable for Scheduler Compensation
*/
static shr_error_e
dnx_cosq_ingress_compensation_scheduler_per_in_port_enable_hw_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Enable Per In Port compensation type
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_COMPENSATION_SCHEDULER_ENABLERS, &entry_handle_id));

    /** Set Range Key Fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CR_PROFILE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PORT_ENABLE, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get per-in-port enable for Scheduler Compensation
*/
static shr_error_e
dnx_cosq_ingress_compensation_scheduler_per_in_port_enable_hw_get(
    int unit,
    int *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Enable Per In Port compensation
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_COMPENSATION_SCHEDULER_ENABLERS, &entry_handle_id));

    /** Set Key Field to 0 -- all entries configured to the same value */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CR_PROFILE_ID, 0);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_PORT_ENABLE, INST_SINGLE, (uint32 *) enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify dnx_cosq_ingress_compensation_scheduler_per_port_enable set/get parameters
*/
shr_error_e
dnx_cosq_ingress_compensation_scheduler_per_port_enable_set_verify(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(enable, 0, 1, _SHR_E_PARAM, "enable should be 0 or 1");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Set per-in-port enable for Scheduler Compensation
*/
shr_error_e
dnx_cosq_ingress_compensation_scheduler_per_port_enable_set(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_compensation_scheduler_per_port_enable_set_verify(unit, enable));

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_scheduler_per_in_port_enable_hw_set(unit, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get per-in-port enable for Scheduler Compensation
*/
shr_error_e
dnx_cosq_ingress_compensation_scheduler_per_port_enable_get(
    int unit,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_scheduler_per_in_port_enable_hw_get(unit, enable));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
void
dnx_cosq_ingress_compensation_profile_print_cb(
    int unit,
    const void *data)
{
    int *delta = (int *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Compensation Delta", *delta, NULL, NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief - Create template manager for VOQ compensation
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_cosq_ingress_compensation_profile_init(
    int unit)
{
    int default_compensation = dnx_data_ingr_congestion.voq.default_compensation_get(unit);
    int default_profile = 0;
    dnx_algo_template_create_data_t create_data;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));

    /** Fill the create data for the template manager */
    create_data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    create_data.first_profile = 0;
    create_data.max_references = dnx_data_ipq.queues.nof_queues_get(unit);
    create_data.data_size = sizeof(int);
    create_data.default_data = &default_compensation;
    create_data.default_profile = default_profile;
    create_data.nof_profiles = dnx_data_ingr_congestion.voq.nof_compensation_profiles_get(unit);
    sal_strncpy(create_data.name, "VOQ map Ingress Compensation profile", DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_ingress_compensation_db.compensation.alloc(unit));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        /** call the template manager create function */
        SHR_IF_ERR_EXIT(dnx_ingress_compensation_db.compensation.create(unit, core_idx, &create_data, NULL));
    }

    /** configure default compensation in HW */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_delta_hw_set
                    (unit, BCM_CORE_ALL, default_profile, default_compensation));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Init of Scheduler Compensation submodule
 */
shr_error_e
dnx_cosq_ingress_compensation_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Scheduler compensation init
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_scheduler_hw_defaults_init(unit));

    /*
     * init SW state DB
     */
    SHR_IF_ERR_EXIT(dnx_ingress_compensation_db.init(unit));

    /*
     * VOQ compensation template manager init
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_profile_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Deinit of Scheduler Compensation submodule
 */
shr_error_e
dnx_cosq_ingress_compensation_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** nothing to do */

    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify structure bcm_cosq_pkt_size_adjust_info_t
 * \param [in] unit -  Unit-ID
 * \param [in] adjust_info - gport and source info
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_cosq_gport_pkt_size_adjust_info_verify(
    int unit,
    bcm_cosq_pkt_size_adjust_info_t * adjust_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(adjust_info, _SHR_E_PARAM, "adjust_info");

    /** check for valid flags value */
    SHR_MASK_VERIFY(adjust_info->flags, 0, _SHR_E_PARAM, "flags are not supported.\n");

    /** check for valid source */
    if (adjust_info->source_info.source_type != bcmCosqPktSizeAdjustSourceGlobal)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     " Incorrect source=%d\n,not supported for this API - only Global is supported",
                     adjust_info->source_info.source_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify parameters for API
 * bcm_cosq_gport_pkt_size_adjust_set
 * \param [in] unit -  Unit-ID
 * \param [in] adjust_info - gport and source info
 * \param [in] delta - delta to be set
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_cosq_gport_pkt_size_adjust_set_verify(
    int unit,
    bcm_cosq_pkt_size_adjust_info_t * adjust_info,
    int delta)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify the adjust_info is valid */
    SHR_IF_ERR_EXIT(dnx_cosq_gport_pkt_size_adjust_info_verify(unit, adjust_info));

    /** verify delta value */
    if (delta < DNX_COSQ_INGRESS_COMPENSATION_DELTA_VALUE_MIN || delta > DNX_COSQ_INGRESS_COMPENSATION_DELTA_VALUE_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect delta=%d\n, must be in range min=%d max=%d", delta,
                     DNX_COSQ_INGRESS_COMPENSATION_DELTA_VALUE_MIN, DNX_COSQ_INGRESS_COMPENSATION_DELTA_VALUE_MAX);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * packet size compensation set
 * \param [in] unit -  Unit-ID
 * \param [in] adjust_info - gport and source info
 * \param [in] delta - delta to be set
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_pkt_size_adjust_set(
    int unit,
    bcm_cosq_pkt_size_adjust_info_t * adjust_info,
    int delta)
{
    int append_ptr, voq, core;

    bcm_port_t src_local_port;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_pkt_size_adjust_set_verify(unit, adjust_info, delta));

    switch (adjust_info->source_info.source_type)
    {
        case bcmCosqPktSizeAdjustSourceGlobal:

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(adjust_info->gport)
                || BCM_GPORT_IS_MCAST_QUEUE_GROUP(adjust_info->gport))
            {
                /** get VOQ from Gport */
                SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get
                                (unit, adjust_info->gport, adjust_info->cosq, FALSE, &core, &voq));
                /** configure compensation delta */
                SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_delta_set(unit, core, voq, delta));

            }
            else if (BCM_GPORT_IS_PROFILE(adjust_info->gport))
            {
                /** append size ptr compensation (per OutLif profile)*/
                append_ptr = BCM_GPORT_PROFILE_GET(adjust_info->gport);

                /** configure compensation delta */
                SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_hdr_append_ptr_delta_set(unit, append_ptr, delta));
            }
            else if (BCM_GPORT_IS_LOCAL(adjust_info->gport) == TRUE)
            {
                src_local_port = BCM_GPORT_LOCAL_GET(adjust_info->gport);

                /** update the HW */
                SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_in_port_delta_set(unit, src_local_port, delta));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid gport (0x%x)", (adjust_info->gport));
            }
            break;

        case bcmCosqPktSizeAdjustSourceScheduler:
        case bcmCosqPktSizeAdjustSourceCrpsInPP:
        case bcmCosqPktSizeAdjustSourceCrpsInTM:
        case bcmCosqPktSizeAdjustSourceStatReportIn:
        case bcmCosqPktSizeAdjustSourceStatReportOut:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Type %d is not supported on this device, Please use bcmCosqPktSizeAdjustSourceGlobal instead",
                         adjust_info->source_info.source_type);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_FAIL, "bcm_cosq_gport_pkt_size_adjust_set called with invalid source_info enum value");
            break;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * packet size compensation get
 * \param [in] unit -  Unit-ID
 * \param [in] adjust_info - gport and source info
 * \param [out] delta - get delta
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_gport_pkt_size_adjust_get(
    int unit,
    bcm_cosq_pkt_size_adjust_info_t * adjust_info,
    int *delta)
{
    bcm_port_t src_local_port;
    int append_ptr, voq, core;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify adjust_info */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_pkt_size_adjust_info_verify(unit, adjust_info));
    SHR_NULL_CHECK(delta, _SHR_E_PARAM, "delta");

    switch (adjust_info->source_info.source_type)
    {
        case bcmCosqPktSizeAdjustSourceGlobal:

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(adjust_info->gport)
                || BCM_GPORT_IS_MCAST_QUEUE_GROUP(adjust_info->gport))
            {
                /** get VOQ from Gport */
                SHR_IF_ERR_EXIT(dnx_cosq_gport_to_voq_get
                                (unit, adjust_info->gport, adjust_info->cosq, FALSE, &core, &voq));
                /** get compensation delta */
                SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_delta_get(unit, core, voq, delta));
            }
            else if (BCM_GPORT_IS_PROFILE(adjust_info->gport))
            {
                /** append size ptr compensation (per OutLif profile)*/
                append_ptr = BCM_GPORT_PROFILE_GET(adjust_info->gport);

                /** get compensation delta */
                SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_hdr_append_ptr_delta_get(unit, append_ptr, delta));
            }
            else if (BCM_GPORT_IS_LOCAL(adjust_info->gport) == TRUE)
            {
                src_local_port = BCM_GPORT_LOCAL_GET(adjust_info->gport);

                /** update the HW */
                SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_in_port_delta_get(unit, src_local_port, delta));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid gport (0x%x)", (adjust_info->gport));
            }
            break;

        case bcmCosqPktSizeAdjustSourceScheduler:
        case bcmCosqPktSizeAdjustSourceCrpsInPP:
        case bcmCosqPktSizeAdjustSourceCrpsInTM:
        case bcmCosqPktSizeAdjustSourceStatReportIn:
        case bcmCosqPktSizeAdjustSourceStatReportOut:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Type %d is not supported on this device, Please use bcmCosqPktSizeAdjustSourceGlobal instead",
                         adjust_info->source_info.source_type);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_FAIL, "bcm_cosq_gport_pkt_size_adjust_get called with invalid source_info enum value");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
