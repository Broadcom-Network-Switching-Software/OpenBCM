/** \file scheduler_element.c
 * 
 *
 * e2e scheduler element related functionality for DNX
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

#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <soc/dnx/dbal/dbal.h>
#include "scheduler.h"
#include "scheduler_element.h"
#include "scheduler_flow.h"
#include "scheduler_convert.h"
#include "scheduler_dbal.h"
#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_sch_config_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_sch_config_access.h>

const dnx_scheduler_element_cl_type_info_t dnx_sch_cl_info[] = {
    {
        /** DBAL_ENUM_FVAL_CL_MODE_NONE */
     "dummy",
     0,    /** nof_base_priorities */
     -1,    /** weighted_or_default_priority */
     -1,    /** nof_descrete_priority */
     -1,    /** max_independent_weight */
     0      /** nof_unused_lsb */
     },
    {
        /** DBAL_ENUM_FVAL_CL_MODE_1 */
     "SP1,SP2,SP3,SP4",
     4,    /** nof_base_priorities */
     BCM_COSQ_SP0,    /** weighted_or_default_priority */
     -1,    /** nof_descrete_priority */
     -1,    /** max_independent_weight */
     0      /** nof_unused_lsb */
     },
    {
        /** DBAL_ENUM_FVAL_CL_MODE_2 */
     "SP1,SP2,SP3-WFQ(2)",
     3,    /** nof_base_priorities */
     BCM_COSQ_SP2,    /** weighted_or_default_priority */
     2,    /** nof_descrete_priority */
     -1,    /** max_independent_weight */
     0      /** nof_unused_lsb */
     },
    {
        /** DBAL_ENUM_FVAL_CL_MODE_3 */
     "SP1-WFQ(1:63),SP2 or SP1-WFQ(3),SP2",
     2,    /** nof_base_priorities */
     BCM_COSQ_SP0,    /** weighted_or_default_priority */
     3,    /** nof_descrete_priority */
     64,    /** max_independent_weight */
     2      /** nof_unused_lsb */
     },
    {
        /** DBAL_ENUM_FVAL_CL_MODE_4 */
     "SP1,SP2-WFQ(1:63) or SP1,SP2-WFQ(3)",
     2,    /** nof_base_priorities */
     BCM_COSQ_SP1,    /** weighted_or_default_priority */
     3,    /** nof_descrete_priority */
     64,    /** max_independent_weight */
     2      /** nof_unused_lsb */
     },
    {
        /** DBAL_ENUM_FVAL_CL_MODE_5 */
     "WFQ(1:253) or SP-WFQ(4)",
     1,    /** nof_base_priorities */
     BCM_COSQ_SP0,    /** weighted_or_default_priority */
     4,    /** nof_descrete_priority */
     256,    /** max_independent_weight */
     0       /** nof_unused_lsb */
     }

};

#define DNX_SCH_CL_BASE_TO_REAL_MODE(base_mode, enhanced_mode) \
    (enhanced_mode == DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP ? base_mode + 1 : base_mode)

/** Note: this macro can be used only for regular (not enhanced) mode (priority) */
#define DNX_SCH_CL_REAL_TO_BASE_MODE(mode, enhanced_mode) \
    (enhanced_mode == DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP ? mode - 1 : mode)

#define DNX_SCH_CL_IS_WEIGHTED_MODE(mode, cl_mode, enhanced_mode)     \
    (cl_mode >= DBAL_ENUM_FVAL_CL_MODE_2  && \
     (mode == -1 || DNX_SCH_CL_REAL_TO_BASE_MODE(mode, enhanced_mode) == dnx_sch_cl_info[cl_mode].weighted_or_default_priority))

typedef struct
{
    const char *description;
    int nof_priorities;
    int wfq1_index;
    int wfq2_index;
} dnx_scheduler_element_hr_type_info_t;

static const dnx_scheduler_element_hr_type_info_t dnx_sch_hr_info[] = {
    {
        /** DBAL_ENUM_FVAL_HR_MODE_SINGLE */
     "SP1,SP2,SP3,SP4-WFQ(1:4K),SP5",
     5 /** nof_priorities */ ,
     BCM_COSQ_SP3 /** wfq1_index */ ,
     -1    /** wfq2_index */
     },
    {
        /** DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ */
     "SP1,SP2,SP3,SP4-WFQ(1:4K),SP5-WFQ(1:4K),SP6",
     6 /** nof_priorities */ ,
     BCM_COSQ_SP3 /** wfq1_index */ ,
     BCM_COSQ_SP4    /** wfq2_index */
     },
    {
        /** DBAL_ENUM_FVAL_HR_MODE_ENHANCED */
     "SP1,SP2,SP3,SP4,SP5,SP6,SP7,SP8,SP9,SP10-WFQ(1:4K),SP11",
     11 /** nof_priorities */ ,
     BCM_COSQ_SP9 /** wfq1_index */ ,
     -1    /** wfq2_index */
     },

};

#define DNX_SCH_HR_IS_WEIGHTED_MODE(mode, hr_mode)     \
    ((mode == dnx_sch_hr_info[hr_mode].wfq1_index) ||  \
      (hr_mode == DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ && mode == dnx_sch_hr_info[hr_mode].wfq2_index))

#define DNX_SCH_HR_IS_DUAL_WFQ_MODE(mode, hr_mode)     \
    ((hr_mode == DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ && mode == dnx_sch_hr_info[hr_mode].wfq2_index) || \
     (hr_mode == DBAL_ENUM_FVAL_HR_MODE_ENHANCED && mode == dnx_sch_hr_info[hr_mode].wfq1_index))

static const uint32 dnx_sch_hr_reserved_mapping[] = {

    0 /** DBAL_ENUM_FVAL_COS_HR_INVALID */ ,
    0xce /** DBAL_ENUM_FVAL_COS_HR_EF1 -- 0xFC 0.006835938 146.2857 = 0xCE or A7 (exactly same value) 146.2857 */ ,
    0xcf /** DBAL_ENUM_FVAL_COS_HR_EF2 -- 0xFD 0.007080078 141.2413793 to ~0xCF (0.007324219) 136.5333333   */ ,
    0xcf /** DBAL_ENUM_FVAL_COS_HR_EF3 -- 0xFE 0.007324219 136.5333333 to 0xCF (exactly same value) 136.5333333 */ ,
    0xd0 /** DBAL_ENUM_FVAL_COS_HR_BE  -- 0xFF 0.007568359 132.1290323  to ~0xD0 or A8 (0.0078125) 128 */ ,
    0x08 /** DBAL_ENUM_FVAL_COS_HR_EF4 -- 0x30 1/2^2 = 0x08 */ ,
    0x02 /** DBAL_ENUM_FVAL_COS_HR_EF5 -- 0x70 1/2^4 = 0x02 */ ,
    0x21 /** DBAL_ENUM_FVAL_COS_HR_EF6 -- 0xb0 1/2^6 = 0x21 */ ,
    0x61 /** DBAL_ENUM_FVAL_COS_HR_EF7 -- 0xf0 1/2^8 = 0x61 */ ,
    0xa1 /** DBAL_ENUM_FVAL_COS_HR_EF8 -- 0xe4 1/2^10 = 0xa1 */ ,
    0xe2 /** DBAL_ENUM_FVAL_COS_HR_EF9 -- 0xe1 1/2^12 ~ 1/2^11 0xe2 */
};

/**
 * \brief - 
 * Convert CL WFQ index to index to HW
 */
shr_error_e
dnx_scheduler_element_cl_wfq_indx_to_hw(
    int unit,
    dbal_enum_value_field_cl_mode_e cl_mode,
    int wfq_indx,
    int *hw_indx)
{
    SHR_FUNC_INIT_VARS(unit);

    if (cl_mode == DBAL_ENUM_FVAL_CL_MODE_3)
    {
        /** in Mode 3, WFQ is at index 0 */
        *hw_indx = wfq_indx;
    }
    else
    {
        /** in other modes, WFQ is at last index */
        *hw_indx = wfq_indx + dnx_sch_cl_info[cl_mode].nof_base_priorities - 1;
    }

    SHR_FUNC_EXIT;

}

/**
 * \brief - 
 * Validate CL mode and weight according to the CL configuration
 */
static shr_error_e
dnx_scheduler_element_cl_mode_weight_verify(
    int unit,
    dnx_sch_cl_config_t * cl_config,
    int mode,
    int weight)
{

    int priority_indx, nof_priorities;
    int is_weighted;
    int min_descrete_weight, max_descrete_weight;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate mode
     */

    if (mode != -1)
    {
        /** real mode specified -- need be verified */

        priority_indx = mode - BCM_COSQ_SP0;

        nof_priorities = dnx_sch_cl_info[cl_config->cl_mode].nof_base_priorities;

        if (cl_config->enhanced_mode != DBAL_ENUM_FVAL_CL_ENHANCED_MODE_DISABLED)
        {
            nof_priorities++;
        }

        if (priority_indx < 0 || priority_indx >= nof_priorities)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid mode %d\n", mode);

        }
    }

    /*
     * Validate weight
     */

    is_weighted = DNX_SCH_CL_IS_WEIGHTED_MODE(mode, cl_config->cl_mode, cl_config->enhanced_mode);

    if (is_weighted)
    {
        switch (cl_config->wfq_weight_mode)
        {
            case DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INDEPENDENT_PER_FLOW:
                if (weight < 0 || weight >= dnx_sch_cl_info[cl_config->cl_mode].max_independent_weight)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid weight %d\n", weight);

                }
                break;
            case DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW:
            case DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS:
                /** weight levels defined as negative and the order is reversed */
                max_descrete_weight = BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL0;
                min_descrete_weight = BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL0
                    - dnx_sch_cl_info[cl_config->cl_mode].nof_descrete_priority;
                SHR_RANGE_VERIFY(weight, min_descrete_weight, max_descrete_weight,
                                 _SHR_E_PARAM, "Invalid descrete weight %d\n", weight);

                break;
            default:
                break;
        }

    }
    /** in non weighted mode - no need to verify weight */

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_scheduler_element_cl_independent_weight_get(
    int unit,
    dbal_enum_value_field_cl_mode_e cl_type,
    uint32 weight,
    uint32 *cos_weight)
{
    dnx_sch_ipf_config_mode_e ipf_config_mode;
    uint32 temp_weight, factored_weight, max_weight, empty_exp;
    uint32 ratio;
    int mnt_field_size;
    uint32 first_reserved_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(cosq_config.ipf_config_mode.get(unit, &ipf_config_mode));

    switch (ipf_config_mode)
    {
        case DNX_SCH_IPF_CONFIG_MODE_PROPORTIONAL:
            temp_weight = weight;
            break;
        case DNX_SCH_IPF_CONFIG_MODE_INVERSE:

            /** Use maximal max_weight -- the weight of mode 5 */
            max_weight = dnx_sch_cl_info[DBAL_ENUM_FVAL_CL_MODE_5].max_independent_weight;

            /*
             * Note: we calculate binary representation for weight/max_weight.
             *
             * Since max_weight is factored, factor the weight by the same factor
             */
            ratio = dnx_sch_cl_info[DBAL_ENUM_FVAL_CL_MODE_5].max_independent_weight
                / dnx_sch_cl_info[cl_type].max_independent_weight;
            factored_weight = weight * ratio;

            /** Get number of bits of the CREDIT_SRC_COS field */
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, DBAL_FIELD_WEIGHT,
                                                       0, DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_WFQ_WEIGHT, 0,
                                                       &mnt_field_size));

            /** Get binary representation for weight/max_weight */
            SHR_IF_ERR_EXIT(utilex_abs_val_to_mnt_binary_fraction_exp(factored_weight /** abs_val_numerator */ ,
                                                                      1 /** abs_val_denominator */ ,
                                                                      mnt_field_size /** mnt_nof_bits */ ,
                                                                      0 /** exp_nof_bits */ ,
                                                                      max_weight /** max_val */ ,
                                                                      &temp_weight /** mnt_bin_fraction */ ,
                                                                      &empty_exp));

            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected ipf_config_mode %d\n", ipf_config_mode);
            break;

    }

    /*
     * temp_weight is 8 bits number.
     * In mode 3 and 4, the weight is 6 MSB only
     * shift the weight -- we might get too big result
     */
    temp_weight <<= dnx_sch_cl_info[cl_type].nof_unused_lsb;

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_COS_CL,
                                                  DBAL_ENUM_FVAL_COS_CL_SP_ENHANCED, &first_reserved_value));

    /*
     * if weight is bigger than upper bound, set it to be upper bound.
     * this operation might set unused LSBs to be non zero
     */
    temp_weight = UTILEX_RANGE_BOUND(temp_weight, 1, first_reserved_value - 1);

    /*
     * Two LSB should be zeroed in modes 3, 4 
     * Clear them again
     */
    temp_weight = (temp_weight >> dnx_sch_cl_info[cl_type].nof_unused_lsb) << dnx_sch_cl_info[cl_type].nof_unused_lsb;

    *cos_weight = temp_weight;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Calculate cos value for the specified CL parameters
 */
static shr_error_e
dnx_scheduler_element_cl_cos_calc(
    int unit,
    dnx_sch_cl_config_t * cl_config,
    int mode,
    int weight,
    dnx_sch_flow_credit_source_cos_data_t * cl_cos)
{
    int priority_indx;
    int is_weighted;

    int cl_type = cl_config->cl_mode;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(cl_cos, 0, sizeof(dnx_sch_flow_credit_source_cos_data_t));

    if (mode == -1)
    {
        /** replace -1 with the actual mode */
        mode = dnx_sch_cl_info[cl_type].weighted_or_default_priority;
        if (cl_config->cl_mode >= DBAL_ENUM_FVAL_CL_MODE_2
            && cl_config->enhanced_mode == DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP)
        {
            mode++;
        }
    }

    is_weighted = DNX_SCH_CL_IS_WEIGHTED_MODE(mode, cl_type, cl_config->enhanced_mode);

    /*
     * handle weighted case first
     */
    if (is_weighted && cl_config->cl_mode >= DBAL_ENUM_FVAL_CL_MODE_3 &&
        cl_config->wfq_weight_mode == DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INDEPENDENT_PER_FLOW)
    {
        /** cos should be configured with weight */
        cl_cos->type = DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_WFQ_WEIGHT;
        SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_independent_weight_get(unit, cl_type, weight, &cl_cos->cl_weight));
    }
    else
    {
        cl_cos->type = DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_SIMPLE;

        if (cl_config->cl_mode == DBAL_ENUM_FVAL_CL_MODE_3 &&
            DNX_SCH_CL_REAL_TO_BASE_MODE(mode, cl_config->enhanced_mode) == BCM_COSQ_SP1)
        {
            /** In mode 3, 2 additional SPs are taken by WFQ */
            mode += dnx_sch_cl_info[DBAL_ENUM_FVAL_CL_MODE_3].nof_descrete_priority - 1;
        }

        if (is_weighted)
        {
            /*
             * In descrete CL mode - update the mode according to the weight
             */

            /** weight levels defined as negative and the order is reversed */
            mode += (BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL0 - weight);
        }

        priority_indx = mode - BCM_COSQ_SP0;

        switch (cl_config->enhanced_mode)
        {
            case DBAL_ENUM_FVAL_CL_ENHANCED_MODE_DISABLED:
                /** indx==0 -> SP1 */
                cl_cos->simple_cos = DBAL_ENUM_FVAL_COS_CL_SP1 + priority_indx;
                break;
            case DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_LP:
                /** like in disabled: indx==0 -> SP1 */

                /*
                 * In case of descrete weight, we might get priority_indx == dnx_sch_cl_info[cl_type].nof_base_priorities
                 * but this is not an indication for enhanced mode.
                 * This case should be excluded.
                 */
                cl_cos->simple_cos = (!is_weighted && priority_indx == dnx_sch_cl_info[cl_type].nof_base_priorities) ?
                    DBAL_ENUM_FVAL_COS_CL_SP_ENHANCED : DBAL_ENUM_FVAL_COS_CL_SP1 + priority_indx;
                break;
            case DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP:
                /** indx==1 -> SP1 */
                cl_cos->simple_cos = (priority_indx == 0) ? DBAL_ENUM_FVAL_COS_CL_SP_ENHANCED :
                    DBAL_ENUM_FVAL_COS_CL_SP1 + priority_indx - 1;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected enhanced_mode %d\n", cl_config->enhanced_mode);
                break;
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Get CL configuration
 */
shr_error_e
dnx_scheduler_element_cl_config_get(
    int unit,
    int core_id,
    int cl_id,
    dnx_sch_cl_config_t * cl_config)
{
    int cl_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** get CL class profile */
    SHR_IF_ERR_EXIT(dnx_sch_cl_mapping_get(unit, core_id, cl_id, &cl_profile));

    /** get cl config per profile */
    SHR_IF_ERR_EXIT(dnx_sch_cl_config_get(unit, core_id, cl_profile, cl_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set Color group of the scheduling element.
 * Sets the color on both flows in case of composite flow.
 * Should be called for base flow only.
 */
shr_error_e
dnx_scheduler_element_color_group_set(
    int unit,
    int core_id,
    int flow_id,
    int group)
{
    int se_id;
    int is_composite, composite_flow_id, composite_se_id;

    SHR_FUNC_INIT_VARS(unit);

    /** get SE id */
    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

    /** set the color on SE */
    SHR_IF_ERR_EXIT(dnx_sch_se_color_group_set(unit, core_id, se_id, group));

    /** get is composite */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_get(unit, core_id, flow_id, &is_composite));

    if (is_composite)
    {
        SHR_IF_ERR_EXIT(dnx_sch_flow_composite_id_get(unit, core_id, flow_id, &composite_flow_id));
        SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, composite_flow_id, &composite_se_id));

        /** set the same color on composite se */
        SHR_IF_ERR_EXIT(dnx_sch_se_color_group_set(unit, core_id, composite_se_id, group));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get Color group of the scheduling element.
 */
shr_error_e
dnx_scheduler_element_color_group_get(
    int unit,
    int core_id,
    int flow_id,
    int *group)
{
    int se_id;

    SHR_FUNC_INIT_VARS(unit);

    /** get SE id */
    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

    /** get the color of SE */
    SHR_IF_ERR_EXIT(dnx_sch_se_color_group_get(unit, core_id, se_id, group));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Decide and set Color group of the scheduling element.
 * Sets the color on both flows in case of composite flow.
 * Should be called for base flow only.
 */
shr_error_e
dnx_scheduler_element_color_group_decide_and_set(
    int unit,
    int core_id,
    int child_flow_id,
    int parent_se_id)
{
    int group;

    SHR_FUNC_INIT_VARS(unit);

    /** get parent color */
    SHR_IF_ERR_EXIT(dnx_sch_se_color_group_get(unit, core_id, parent_se_id, &group));

    /** calculate child color */
    group = (group + 1) % dnx_data_sch.se.nof_color_group_get(unit);

    /** set the color on child */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_color_group_set(unit, core_id, child_flow_id, group));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Validate HR mode and weight according to the HR configuration
 */
static shr_error_e
dnx_scheduler_element_hr_mode_weight_verify(
    int unit,
    dbal_enum_value_field_hr_mode_e hr_mode,
    int mode,
    int weight)
{

    int priority_indx, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate mode
     */
    priority_indx = mode - BCM_COSQ_SP0;

    nof_priorities = dnx_sch_hr_info[hr_mode].nof_priorities;

    if (priority_indx < 0 || priority_indx >= nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid mode %d\n", mode);

    }

    /*
     * Validate weight
     */
    if (DNX_SCH_HR_IS_WEIGHTED_MODE(mode, hr_mode))
    {
        if (weight < 0 || weight > dnx_data_sch.se.max_hr_weight_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid weight %d\n", weight);
        }
    }
    /** in non weighted mode no need to verify weight */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Calculate weight cos for HR
 */
shr_error_e
dnx_scheduler_element_hr_cos_weight_calc(
    int unit,
    dbal_enum_value_field_hr_mode_e hr_mode,
    int weight,
    dnx_sch_flow_credit_source_cos_data_t * hr_cos)
{
    int mnt_field_size, exp_field_size;
    uint32 mnt_max_value, exp_max_value;
    uint32 temp_mnt[1], temp_exp[1];

    uint32 temp_cos[1];
    uint32 reserved_value, first_reserved_value;
    dbal_enum_value_field_cos_hr_e reserved_id, last_id_to_check;

    SHR_FUNC_INIT_VARS(unit);

    /** Get number of bits of the mantissa field */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, DBAL_FIELD_WEIGHT_MANT,
                                               0, DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT, 0,
                                               &mnt_field_size));

    /** Get maximal value of the mantissa field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, DBAL_FIELD_WEIGHT_MANT,
                                                          0, DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE, &mnt_max_value));

    /** Get number of bits of the exponent field */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, DBAL_FIELD_WEIGHT_EXP,
                                               0, DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT, 0,
                                               &exp_field_size));

    /** Get maximal value of the exponent field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, DBAL_FIELD_WEIGHT_EXP,
                                                          0, DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE, &exp_max_value));

    /** Get binary representation for weight */
    SHR_IF_ERR_EXIT(utilex_abs_val_to_mnt_binary_fraction_exp(weight /** abs_val_numerator */ ,
                                                              1 /** abs_val_denominator */ ,
                                                              mnt_field_size /** mnt_nof_bits */ ,
                                                              exp_field_size /** exp_nof_bits */ ,
                                                              dnx_data_sch.se.max_hr_weight_get(unit) /** max_val */ ,
                                                              temp_mnt /** mnt_bin_fraction */ ,
                                                              temp_exp));

    /*
     * if temp_mnt is bigger than upper bound, set it to be upper bound.
     */
    temp_mnt[0] = UTILEX_RANGE_BOUND(temp_mnt[0], 1, mnt_max_value);

    /*
     * if temp_exp is bigger than upper bound, set it to be upper bound.
     */
    temp_exp[0] = UTILEX_RANGE_BOUND(temp_exp[0], 0, exp_max_value);

    temp_cos[0] = 0;

    /** set mnt to the temporary cos -- mnt_field_size bits starting at bit 0 */
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(temp_mnt, 0, mnt_field_size, temp_cos));
    /** set exp to the temporary cos -- exp_field_size bits starting at bit mnt_field_size */
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(temp_exp, mnt_field_size, exp_field_size, temp_cos));

    /** Make sure result cos value is not pre-defined value */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_COS_HR, DBAL_ENUM_FVAL_COS_HR_EF1,
                                                  &first_reserved_value));
    if (temp_cos[0] >= first_reserved_value || hr_mode == DBAL_ENUM_FVAL_HR_MODE_ENHANCED)
    {
        last_id_to_check =
            (hr_mode == DBAL_ENUM_FVAL_HR_MODE_ENHANCED ? DBAL_ENUM_FVAL_COS_HR_EF9 : DBAL_ENUM_FVAL_COS_HR_BE);
        for (reserved_id = DBAL_ENUM_FVAL_COS_HR_EF1; reserved_id <= last_id_to_check; reserved_id++)
        {
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_COS_HR, reserved_id, &reserved_value));
            if (temp_cos[0] == reserved_value)
            {
                temp_cos[0] = dnx_sch_hr_reserved_mapping[reserved_id];
                break;
            }
        }
    }

    /** get mnt from the temporary cos -- mnt_field_size bits starting at bit 0 */
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(temp_cos, 0, mnt_field_size, temp_mnt));
    /** get exp from the temporary cos -- exp_field_size bits starting at bit mnt_field_size */
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(temp_cos, mnt_field_size, exp_field_size, temp_exp));

    /** update the result */
    hr_cos->hr_weight_mantissa = temp_mnt[0];
    hr_cos->hr_weight_exp = temp_exp[0];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Calculate cos value for the specified HR parameters
 */
static shr_error_e
dnx_scheduler_element_hr_cos_calc(
    int unit,
    dbal_enum_value_field_hr_mode_e hr_mode,
    int mode,
    int weight,
    dnx_sch_flow_credit_source_cos_data_t * hr_cos)
{
    int priority_indx;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(hr_cos, 0, sizeof(dnx_sch_flow_credit_source_cos_data_t));

    /*
     * handle weighted case first
     */
    if (DNX_SCH_HR_IS_WEIGHTED_MODE(mode, hr_mode))
    {
        /** cos should be configured with weight */
        hr_cos->type = DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT;
        SHR_IF_ERR_EXIT(dnx_scheduler_element_hr_cos_weight_calc(unit, hr_mode, weight, hr_cos));
        hr_cos->hr_sel_dual = DNX_SCH_HR_IS_DUAL_WFQ_MODE(mode, hr_mode);
    }
    else
    {
        hr_cos->type = DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_SIMPLE;

        priority_indx = mode - BCM_COSQ_SP0;

        /** calculate HR cos according to hr_mode */
        if (priority_indx == dnx_sch_hr_info[hr_mode].nof_priorities - 1)
        {
            /** Best Effort -- last priority */
            hr_cos->simple_cos = DBAL_ENUM_FVAL_COS_HR_BE;
        }
        else if (priority_indx < 3)
        {
            /** One of 3 Strict priorities */
            hr_cos->simple_cos = DBAL_ENUM_FVAL_COS_HR_EF1 + priority_indx;
        }
        else if (hr_mode == DBAL_ENUM_FVAL_HR_MODE_ENHANCED && priority_indx >= 3)
        {
            /** One of the additional enhanced priorities */
            hr_cos->simple_cos = DBAL_ENUM_FVAL_COS_HR_EF4 + (priority_indx - 3);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority_indx %d\n", priority_indx);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Calculate cos value for the specified SE
 */
static shr_error_e
dnx_scheduler_element_cos_calc(
    int unit,
    int core_id,
    int parent_se_id,
    int child_mode,
    int child_weight,
    dnx_sch_flow_credit_source_cos_data_t * se_cos)
{

    dnx_sch_cl_config_t cl_config;
    int cl_id, hr_id;
    dbal_enum_value_field_hr_mode_e hr_mode;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(se_cos, 0, sizeof(dnx_sch_flow_credit_source_cos_data_t));

    if (DNX_SCHEDULER_ELEMENT_IS_HR(unit, parent_se_id))
    {
        /*
         * Connect to HR
         */

        hr_id = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, parent_se_id);

        SHR_IF_ERR_EXIT(dnx_sch_hr_mode_get(unit, core_id, hr_id, &hr_mode));

        /** validate mode and weight */
        SHR_INVOKE_VERIFY_DNX(dnx_scheduler_element_hr_mode_weight_verify(unit, hr_mode, child_mode, child_weight));

        /** calculate SE cos */
        SHR_IF_ERR_EXIT(dnx_scheduler_element_hr_cos_calc(unit, hr_mode, child_mode, child_weight, se_cos));

    }
    else if (DNX_SCHEDULER_ELEMENT_IS_CL(unit, parent_se_id))
    {
        /*
         * Connect to CL
         */

        cl_id = DNX_SCHEDULER_ELEMENT_CL_ID_GET(unit, parent_se_id);

        SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_config_get(unit, core_id, cl_id, &cl_config));

        /** validate mode and weight */
        SHR_INVOKE_VERIFY_DNX(dnx_scheduler_element_cl_mode_weight_verify(unit, &cl_config, child_mode, child_weight));

        /** calculate SE cos */
        SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_cos_calc(unit, &cl_config, child_mode, child_weight, se_cos));
    }
    else
    {
        /*
         * Connect to FQ
         */
        se_cos->type = DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_SIMPLE;
        se_cos->simple_cos = DBAL_ENUM_FVAL_COS_HR_EF1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Decode HR weight from weight cos
 */
static shr_error_e
dnx_scheduler_element_hr_cos_weight_decode(
    int unit,
    dnx_sch_flow_credit_source_cos_data_t * hr_cos,
    int *weight)
{
    uint32 weight_numerator, weight_denominator;
    int mnt_field_size, exp_field_size;

    SHR_FUNC_INIT_VARS(unit);

    /** Get number of bits of the mantissa field */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, DBAL_FIELD_WEIGHT_MANT,
                                               0, DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT, 0,
                                               &mnt_field_size));

    /** Get number of bits of the exponent field */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, DBAL_FIELD_WEIGHT_EXP,
                                               0, DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT, 0,
                                               &exp_field_size));

    SHR_IF_ERR_EXIT(utilex_mnt_binary_fraction_exp_to_abs_val(mnt_field_size,
                                                              exp_field_size,
                                                              dnx_data_sch.se.max_hr_weight_get(unit) /** max_val */ ,
                                                              hr_cos->hr_weight_mantissa,
                                                              hr_cos->hr_weight_exp,
                                                              &weight_numerator, &weight_denominator));

    *weight = weight_numerator / weight_denominator;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Calculate cos value for the specified HR parameters
 */
static shr_error_e
dnx_scheduler_element_hr_cos_decode(
    int unit,
    dbal_enum_value_field_hr_mode_e hr_mode,
    dnx_sch_flow_credit_source_cos_data_t * hr_cos,
    int *mode,
    int *weight)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (hr_cos->type)
    {
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_SIMPLE:

            if (hr_cos->simple_cos == DBAL_ENUM_FVAL_COS_HR_BE)
            {
                /** Best Effort -- last priority */
                *mode = BCM_COSQ_SP0 + dnx_sch_hr_info[hr_mode].nof_priorities - 1;
            }
            else if (hr_cos->simple_cos <= DBAL_ENUM_FVAL_COS_HR_EF1 + 2)
            {
                /** One of 3 Strict priorities */
                *mode = BCM_COSQ_SP0 + hr_cos->simple_cos - DBAL_ENUM_FVAL_COS_HR_EF1;
            }
            else if (hr_mode == DBAL_ENUM_FVAL_HR_MODE_ENHANCED && hr_cos->simple_cos >= DBAL_ENUM_FVAL_COS_HR_EF4)
            {
                /** One of the additional enhanced priorities */
                *mode = BCM_COSQ_SP3 + hr_cos->simple_cos - DBAL_ENUM_FVAL_COS_HR_EF4;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected cos value %x\n", hr_cos->simple_cos);
            }
            *weight = 0;
            break;

        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT:
            if (hr_mode == DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ)
            {
                *mode = ((hr_cos->hr_sel_dual == 0) ? BCM_COSQ_SP3 : BCM_COSQ_SP4);
            }
            else
            {
                /** weight mode is one before last */
                *mode = BCM_COSQ_SP0 + dnx_sch_hr_info[hr_mode].nof_priorities - 2;
            }
            SHR_IF_ERR_EXIT(dnx_scheduler_element_hr_cos_weight_decode(unit, hr_cos, weight));

            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected HR cos type %d\n", hr_cos->type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Decode CL independent weight from weight cos
 */
static shr_error_e
dnx_scheduler_element_cl_independent_weight_decode(
    int unit,
    dbal_enum_value_field_cl_mode_e cl_type,
    uint32 cos_weight,
    uint32 *weight)
{
    dnx_sch_ipf_config_mode_e ipf_config_mode;
    uint32 max_weight;
    int mnt_field_size;
    uint32 weight_numerator, weight_denominator;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(cosq_config.ipf_config_mode.get(unit, &ipf_config_mode));

    switch (ipf_config_mode)
    {
        case DNX_SCH_IPF_CONFIG_MODE_PROPORTIONAL:
            *weight = cos_weight >> dnx_sch_cl_info[cl_type].nof_unused_lsb;
            break;
        case DNX_SCH_IPF_CONFIG_MODE_INVERSE:

            /** Use maximal max_weight -- the weight of mode 5 */
            max_weight = dnx_sch_cl_info[cl_type].max_independent_weight;

            /** Get number of bits of the CREDIT_SRC_COS field */
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, DBAL_FIELD_WEIGHT,
                                                       0, DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_WFQ_WEIGHT, 0,
                                                       &mnt_field_size));

            SHR_IF_ERR_EXIT(utilex_mnt_binary_fraction_exp_to_abs_val(mnt_field_size /** mnt_nof_bits */ ,
                                                                      0 /** exp_nof_bits */ ,
                                                                      max_weight /** max_val */ ,
                                                                      cos_weight /** mnt_bin_fraction */ ,
                                                                      0 /** exp */ ,
                                                                      &weight_numerator /** abs_val_numerator */ ,
                                                                      &weight_denominator /** abs_val_denominator */ ));
            *weight = weight_numerator / weight_denominator;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected ipf_config_mode %d\n", ipf_config_mode);
            break;

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Calculate cos value for the specified CL parameters
 */
static shr_error_e
dnx_scheduler_element_cl_cos_decode(
    int unit,
    dnx_sch_cl_config_t * cl_config,
    dnx_sch_flow_credit_source_cos_data_t * cl_cos,
    int *mode,
    int *weight)
{
    int offset, weighted_offset;

    int cl_type = cl_config->cl_mode;

    SHR_FUNC_INIT_VARS(unit);

    switch (cl_cos->type)
    {
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_SIMPLE:

            *weight = 0;
            if (cl_cos->simple_cos == DBAL_ENUM_FVAL_COS_CL_SP_ENHANCED)
            {
                *mode = (cl_config->enhanced_mode == DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP ?
                         BCM_COSQ_SP0 : BCM_COSQ_SP0 + dnx_sch_cl_info[cl_type].nof_base_priorities);
            }
            else
            {
                offset = cl_cos->simple_cos - DBAL_ENUM_FVAL_COS_CL_SP1;
                weighted_offset = dnx_sch_cl_info[cl_type].weighted_or_default_priority - BCM_COSQ_SP0;

                /** detect descrete weight first */
                if ((cl_config->wfq_weight_mode == DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW ||
                     cl_config->wfq_weight_mode == DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS) &&
                    UTILEX_IS_IN_RANGE(offset, weighted_offset,
                                       weighted_offset + dnx_sch_cl_info[cl_type].nof_descrete_priority - 1))
                {
                    *mode = dnx_sch_cl_info[cl_type].weighted_or_default_priority;
                    /** weight levels defined as negative and the order is reversed */
                    *weight = BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL0 - (offset - weighted_offset);
                }
                else
                {
                    *mode = BCM_COSQ_SP0 + cl_cos->simple_cos - DBAL_ENUM_FVAL_COS_CL_SP1;
                    if (cl_config->cl_mode == DBAL_ENUM_FVAL_CL_MODE_3 &&
                        cl_cos->simple_cos == DBAL_ENUM_FVAL_COS_CL_SP4)
                    {
                        *mode = BCM_COSQ_SP1;
                    }
                }
                *mode = DNX_SCH_CL_BASE_TO_REAL_MODE(*mode, cl_config->enhanced_mode);
            }
            break;

        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_WFQ_WEIGHT:
            *mode = dnx_sch_cl_info[cl_type].weighted_or_default_priority;
            *mode = DNX_SCH_CL_BASE_TO_REAL_MODE(*mode, cl_config->enhanced_mode);
            SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_independent_weight_decode(unit, cl_type, cl_cos->cl_weight,
                                                                               (uint32 *) weight));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected cos type %d\n", cl_cos->type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Decode cos value into mode and weight for the specified SE
 */
shr_error_e
dnx_scheduler_element_cos_decode(
    int unit,
    int core_id,
    int parent_se_id,
    dnx_sch_flow_credit_source_cos_data_t * se_cos,
    int *child_mode,
    int *child_weight)
{

    dnx_sch_cl_config_t cl_config;
    int cl_id, hr_id;
    dbal_enum_value_field_hr_mode_e hr_mode;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_SCHEDULER_ELEMENT_IS_HR(unit, parent_se_id))
    {
        /*
         * Connected to HR
         */

        hr_id = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, parent_se_id);

        SHR_IF_ERR_EXIT(dnx_sch_hr_mode_get(unit, core_id, hr_id, &hr_mode));

        /** decode SE cos */
        SHR_IF_ERR_EXIT(dnx_scheduler_element_hr_cos_decode(unit, hr_mode, se_cos, child_mode, child_weight));

    }
    else if (DNX_SCHEDULER_ELEMENT_IS_CL(unit, parent_se_id))
    {
        /*
         * Connected to CL
         */

        cl_id = DNX_SCHEDULER_ELEMENT_CL_ID_GET(unit, parent_se_id);

        SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_config_get(unit, core_id, cl_id, &cl_config));

        /** decode SE cos */
        SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_cos_decode(unit, &cl_config, se_cos, child_mode, child_weight));
    }
    else
    {
        /*
         * Connected to FQ
         */
        *child_weight = 0;
        *child_mode = BCM_COSQ_SP0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
  * \brief -
  * Calculate credit source configuration according to parent configuration
  * and child attach information
  */
shr_error_e
dnx_scheduler_element_credit_src_calc(
    int unit,
    int core_id,
    int parent_se_id,
    int child_mode,
    int child_weight,
    dnx_sch_flow_credit_source_t * credit_source)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(credit_source, 0, sizeof(dnx_sch_flow_credit_source_t));

    credit_source->se_id = parent_se_id;

    /** calculate SE cos according to element configuration and child attach information */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_cos_calc(unit, core_id, parent_se_id, child_mode, child_weight,
                                                   &credit_source->cos));

exit:
    SHR_FUNC_EXIT;
}
