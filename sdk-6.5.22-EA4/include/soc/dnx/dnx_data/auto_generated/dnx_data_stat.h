/** \file dnx_data_stat.h
 * 
 * MODULE DATA INTERFACE - 
 * 
 * Device Data
 * SW component that maintains per device data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 *        
 *     
 * 
 * AUTO-GENERATED BY AUTOCODER!
 * DO NOT EDIT THIS FILE!
 */
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_STAT_H_
/*{*/
#define _DNX_DATA_STAT_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/mcm/allenum.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_stat.h>
/*
 * }
 */

/*!
* \brief This file is only used by DNX (JR2 family). Including it by
* software that is not specific to DNX is an error.
*/
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MODULE FUNCTIONS:
 * {
 */
/**
 * \brief Init default data structure - dnx_data_if_stat
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     rv - see 'shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnx_data_if_stat_init(
    int unit);

/*
 * }
 */

/*
 * SUBMODULE  - STAT_PP:
 * general stat data
 * {
 */
/*
 * Table value structure
 */
/*
 * Feature enum
 */
/**
 * \brief 
 * Submodule features
 */
typedef enum
{
    /**
     * Full stat support is present
     */
    dnx_data_stat_stat_pp_full_stat_support,

    /**
     * Must be last one!
     */
    _dnx_data_stat_stat_pp_feature_nof
} dnx_data_stat_stat_pp_feature_e;

/*
 * Submodule functions typedefs
 */
/**
 * \brief get submodule feature - disabled/enabled
 * 
 * \param [in] unit - Unit #
 * \param [in] feature - feature to get (see enum definition for more details
 * 
 * \return
 *     is_supported - returns 1 if and only if supported and 0 otherwise
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef int(
    *dnx_data_stat_stat_pp_feature_get_f) (
    int unit,
    dnx_data_stat_stat_pp_feature_e feature);

/**
 * \brief returns define data of max_irpp_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'max_irpp_profile_value'
 * max irpp profile size
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_irpp_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_max_irpp_profile_value_get_f) (
    int unit);

/**
 * \brief returns define data of max_etpp_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'max_etpp_profile_value'
 * max etpp profile size
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_etpp_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_max_etpp_profile_value_get_f) (
    int unit);

/**
 * \brief returns define data of max_etpp_counting_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'max_etpp_counting_profile_value'
 * max etpp counting profile size
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_etpp_counting_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_max_etpp_counting_profile_value_get_f) (
    int unit);

/**
 * \brief returns define data of max_etpp_metering_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'max_etpp_metering_profile_value'
 * max etpp metering profile size
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_etpp_metering_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_max_etpp_metering_profile_value_get_f) (
    int unit);

/**
 * \brief returns define data of max_erpp_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'max_erpp_profile_value'
 * max erpp profile size
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_erpp_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_max_erpp_profile_value_get_f) (
    int unit);

/**
 * \brief returns define data of etpp_reversed_stat_cmd
 * Module - 'stat', Submodule - 'stat_pp', data - 'etpp_reversed_stat_cmd'
 * ETPP statistics are built wrong in FWD, ENCAP
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     etpp_reversed_stat_cmd - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_etpp_reversed_stat_cmd_get_f) (
    int unit);

/**
 * \brief returns define data of etpp_enc_stat_cmd_is_reversed
 * Module - 'stat', Submodule - 'stat_pp', data - 'etpp_enc_stat_cmd_is_reversed'
 * ETPP statistics are built wrong in FWD, ENCAP (some devices reverse it themselves for the trap)
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     etpp_enc_stat_cmd_is_reversed - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_etpp_enc_stat_cmd_is_reversed_get_f) (
    int unit);

/**
 * \brief returns define data of etpp_egress_vsi_always_pushed
 * Module - 'stat', Submodule - 'stat_pp', data - 'etpp_egress_vsi_always_pushed'
 * ETPP ENCAP stages push VSI stats entry regardless of use_vsd enabler
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     etpp_egress_vsi_always_pushed - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_etpp_egress_vsi_always_pushed_get_f) (
    int unit);

/**
 * \brief returns define data of irpp_start_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'irpp_start_profile_value'
 * irpp profile first index
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     irpp_start_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_irpp_start_profile_value_get_f) (
    int unit);

/**
 * \brief returns define data of erpp_start_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'erpp_start_profile_value'
 * erpp profile first index
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     erpp_start_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_erpp_start_profile_value_get_f) (
    int unit);

/**
 * \brief returns define data of etpp_start_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'etpp_start_profile_value'
 * etpp profile first index
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     etpp_start_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_etpp_start_profile_value_get_f) (
    int unit);

/**
 * \brief returns define data of etpp_counting_start_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'etpp_counting_start_profile_value'
 * etpp counting profile first index
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     etpp_counting_start_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_etpp_counting_start_profile_value_get_f) (
    int unit);

/**
 * \brief returns define data of etpp_metering_start_profile_value
 * Module - 'stat', Submodule - 'stat_pp', data - 'etpp_metering_start_profile_value'
 * etpp metering profile first index
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     etpp_metering_start_profile_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_stat_pp_etpp_metering_start_profile_value_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_STAT - STAT_PP:
 * {
 */
/**
 * \brief Interface for stat stat_pp data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_stat_stat_pp_feature_get_f feature_get;
    /**
     * returns define data of max_irpp_profile_value
     */
    dnx_data_stat_stat_pp_max_irpp_profile_value_get_f max_irpp_profile_value_get;
    /**
     * returns define data of max_etpp_profile_value
     */
    dnx_data_stat_stat_pp_max_etpp_profile_value_get_f max_etpp_profile_value_get;
    /**
     * returns define data of max_etpp_counting_profile_value
     */
    dnx_data_stat_stat_pp_max_etpp_counting_profile_value_get_f max_etpp_counting_profile_value_get;
    /**
     * returns define data of max_etpp_metering_profile_value
     */
    dnx_data_stat_stat_pp_max_etpp_metering_profile_value_get_f max_etpp_metering_profile_value_get;
    /**
     * returns define data of max_erpp_profile_value
     */
    dnx_data_stat_stat_pp_max_erpp_profile_value_get_f max_erpp_profile_value_get;
    /**
     * returns define data of etpp_reversed_stat_cmd
     */
    dnx_data_stat_stat_pp_etpp_reversed_stat_cmd_get_f etpp_reversed_stat_cmd_get;
    /**
     * returns define data of etpp_enc_stat_cmd_is_reversed
     */
    dnx_data_stat_stat_pp_etpp_enc_stat_cmd_is_reversed_get_f etpp_enc_stat_cmd_is_reversed_get;
    /**
     * returns define data of etpp_egress_vsi_always_pushed
     */
    dnx_data_stat_stat_pp_etpp_egress_vsi_always_pushed_get_f etpp_egress_vsi_always_pushed_get;
    /**
     * returns define data of irpp_start_profile_value
     */
    dnx_data_stat_stat_pp_irpp_start_profile_value_get_f irpp_start_profile_value_get;
    /**
     * returns define data of erpp_start_profile_value
     */
    dnx_data_stat_stat_pp_erpp_start_profile_value_get_f erpp_start_profile_value_get;
    /**
     * returns define data of etpp_start_profile_value
     */
    dnx_data_stat_stat_pp_etpp_start_profile_value_get_f etpp_start_profile_value_get;
    /**
     * returns define data of etpp_counting_start_profile_value
     */
    dnx_data_stat_stat_pp_etpp_counting_start_profile_value_get_f etpp_counting_start_profile_value_get;
    /**
     * returns define data of etpp_metering_start_profile_value
     */
    dnx_data_stat_stat_pp_etpp_metering_start_profile_value_get_f etpp_metering_start_profile_value_get;
} dnx_data_if_stat_stat_pp_t;

/*
 * }
 */

/*
 * SUBMODULE  - DIAG_COUNTER:
 * diag counter related infomation.
 * {
 */
/*
 * Table value structure
 */
/**
 * \brief Holds values of submodule diag_counter table ovf_info
 * Table info:
 * irregular overfield infomation
 */
typedef struct
{
    /**
     * register name for counter
     */
    soc_reg_t reg;
    /**
     * cnt field name.
     */
    soc_field_t cnt_field;
    /**
     * overflow field name
     */
    soc_field_t ovf_field;
} dnx_data_stat_diag_counter_ovf_info_t;

/*
 * Feature enum
 */
/**
 * \brief 
 * Submodule features
 */
typedef enum
{

    /**
     * Must be last one!
     */
    _dnx_data_stat_diag_counter_feature_nof
} dnx_data_stat_diag_counter_feature_e;

/*
 * Submodule functions typedefs
 */
/**
 * \brief get submodule feature - disabled/enabled
 * 
 * \param [in] unit - Unit #
 * \param [in] feature - feature to get (see enum definition for more details
 * 
 * \return
 *     is_supported - returns 1 if and only if supported and 0 otherwise
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef int(
    *dnx_data_stat_diag_counter_feature_get_f) (
    int unit,
    dnx_data_stat_diag_counter_feature_e feature);

/**
 * \brief get table ovf_info entry 
 * irregular overfield infomation
 * 
 * \param [in] unit - unit #
 * \param [in] index - overflow field index
 * 
 * \return
 *     ovf_info - returns the relevant entry values grouped in struct - see dnx_data_stat_diag_counter_ovf_info_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_stat_diag_counter_ovf_info_t *(
    *dnx_data_stat_diag_counter_ovf_info_get_f) (
    int unit,
    int index);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_STAT - DIAG_COUNTER:
 * {
 */
/**
 * \brief Interface for stat diag_counter data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_stat_diag_counter_feature_get_f feature_get;
    /**
     * get table ovf_info entry 
     */
    dnx_data_stat_diag_counter_ovf_info_get_f ovf_info_get;
    /**
     * get general info table about table (for example key size)ovf_info info
     */
    dnxc_data_table_info_get_f ovf_info_info_get;
} dnx_data_if_stat_diag_counter_t;

/*
 * }
 */

/*
 * SUBMODULE  - DROP_REASONS:
 * tm drop reasons
 * {
 */
/*
 * Table value structure
 */
/**
 * \brief Holds values of submodule drop_reasons table group_drop_reason_index
 * Table info:
 * mapping drop reasons group enum to hw bit index
 */
typedef struct
{
    /**
     * hw group index used in dbal
     */
    uint32 index;
} dnx_data_stat_drop_reasons_group_drop_reason_index_t;

/**
 * \brief Holds values of submodule drop_reasons table drop_reason_index
 * Table info:
 * mapping drop reasons bit enum to hw bit index
 */
typedef struct
{
    /**
     * hw group index used in dbal
     */
    uint32 index;
} dnx_data_stat_drop_reasons_drop_reason_index_t;

/**
 * \brief Holds values of submodule drop_reasons table drop_reasons_groups
 * Table info:
 * default drop reasons groups
 */
typedef struct
{
    /**
     * default drop reasons
     */
    bcm_cosq_drop_reason_t drop_reasons[DNX_DATA_MAX_STAT_DROP_REASONS_NOF_DROP_REASONS];
} dnx_data_stat_drop_reasons_drop_reasons_groups_t;

/*
 * Feature enum
 */
/**
 * \brief 
 * Submodule features
 */
typedef enum
{

    /**
     * Must be last one!
     */
    _dnx_data_stat_drop_reasons_feature_nof
} dnx_data_stat_drop_reasons_feature_e;

/*
 * Submodule functions typedefs
 */
/**
 * \brief get submodule feature - disabled/enabled
 * 
 * \param [in] unit - Unit #
 * \param [in] feature - feature to get (see enum definition for more details
 * 
 * \return
 *     is_supported - returns 1 if and only if supported and 0 otherwise
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef int(
    *dnx_data_stat_drop_reasons_feature_get_f) (
    int unit,
    dnx_data_stat_drop_reasons_feature_e feature);

/**
 * \brief returns define data of nof_drop_reasons
 * Module - 'stat', Submodule - 'drop_reasons', data - 'nof_drop_reasons'
 * number of drop reasons
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_drop_reasons - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_stat_drop_reasons_nof_drop_reasons_get_f) (
    int unit);

/**
 * \brief get table group_drop_reason_index entry 
 * mapping drop reasons group enum to hw bit index
 * 
 * \param [in] unit - unit #
 * \param [in] group - group enum value
 * 
 * \return
 *     group_drop_reason_index - returns the relevant entry values grouped in struct - see dnx_data_stat_drop_reasons_group_drop_reason_index_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_stat_drop_reasons_group_drop_reason_index_t *(
    *dnx_data_stat_drop_reasons_group_drop_reason_index_get_f) (
    int unit,
    int group);

/**
 * \brief get table drop_reason_index entry 
 * mapping drop reasons bit enum to hw bit index
 * 
 * \param [in] unit - unit #
 * \param [in] drop_reason_bit - drop reason bit representative enum
 * 
 * \return
 *     drop_reason_index - returns the relevant entry values grouped in struct - see dnx_data_stat_drop_reasons_drop_reason_index_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_stat_drop_reasons_drop_reason_index_t *(
    *dnx_data_stat_drop_reasons_drop_reason_index_get_f) (
    int unit,
    int drop_reason_bit);

/**
 * \brief get table drop_reasons_groups entry 
 * default drop reasons groups
 * 
 * \param [in] unit - unit #
 * \param [in] name - drop reasons group name
 * 
 * \return
 *     drop_reasons_groups - returns the relevant entry values grouped in struct - see dnx_data_stat_drop_reasons_drop_reasons_groups_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_stat_drop_reasons_drop_reasons_groups_t *(
    *dnx_data_stat_drop_reasons_drop_reasons_groups_get_f) (
    int unit,
    int name);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_STAT - DROP_REASONS:
 * {
 */
/**
 * \brief Interface for stat drop_reasons data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_stat_drop_reasons_feature_get_f feature_get;
    /**
     * returns define data of nof_drop_reasons
     */
    dnx_data_stat_drop_reasons_nof_drop_reasons_get_f nof_drop_reasons_get;
    /**
     * get table group_drop_reason_index entry 
     */
    dnx_data_stat_drop_reasons_group_drop_reason_index_get_f group_drop_reason_index_get;
    /**
     * get general info table about table (for example key size)group_drop_reason_index info
     */
    dnxc_data_table_info_get_f group_drop_reason_index_info_get;
    /**
     * get table drop_reason_index entry 
     */
    dnx_data_stat_drop_reasons_drop_reason_index_get_f drop_reason_index_get;
    /**
     * get general info table about table (for example key size)drop_reason_index info
     */
    dnxc_data_table_info_get_f drop_reason_index_info_get;
    /**
     * get table drop_reasons_groups entry 
     */
    dnx_data_stat_drop_reasons_drop_reasons_groups_get_f drop_reasons_groups_get;
    /**
     * get general info table about table (for example key size)drop_reasons_groups info
     */
    dnxc_data_table_info_get_f drop_reasons_groups_info_get;
} dnx_data_if_stat_drop_reasons_t;

/*
 * }
 */

/*
 * SUBMODULE  - FEATURE:
 * 
 * {
 */
/*
 * Table value structure
 */
/*
 * Feature enum
 */
/**
 * \brief 
 * Submodule features
 */
typedef enum
{
    dnx_data_stat_feature_itpp_cud_count_always_stamp,

    /**
     * Must be last one!
     */
    _dnx_data_stat_feature_feature_nof
} dnx_data_stat_feature_feature_e;

/*
 * Submodule functions typedefs
 */
/**
 * \brief get submodule feature - disabled/enabled
 * 
 * \param [in] unit - Unit #
 * \param [in] feature - feature to get (see enum definition for more details
 * 
 * \return
 *     is_supported - returns 1 if and only if supported and 0 otherwise
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef int(
    *dnx_data_stat_feature_feature_get_f) (
    int unit,
    dnx_data_stat_feature_feature_e feature);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_STAT - FEATURE:
 * {
 */
/**
 * \brief Interface for stat feature data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_stat_feature_feature_get_f feature_get;
} dnx_data_if_stat_feature_t;

/*
 * }
 */

/*
 * MODULE INTERFACE DNX_DATA_IF_STAT:
 * {
 */
/**
 * \brief Interface for stat data
 */
typedef struct
{
    /**
     * Interface for stat stat_pp data
     */
    dnx_data_if_stat_stat_pp_t stat_pp;
    /**
     * Interface for stat diag_counter data
     */
    dnx_data_if_stat_diag_counter_t diag_counter;
    /**
     * Interface for stat drop_reasons data
     */
    dnx_data_if_stat_drop_reasons_t drop_reasons;
    /**
     * Interface for stat feature data
     */
    dnx_data_if_stat_feature_t feature;
} dnx_data_if_stat_t;

/*
 * }
 */
/*
 * Extern:
 * {
 */
/**
 * \brief
 * Module structured interface - used to get the required data stored in dnx data
 */
extern dnx_data_if_stat_t dnx_data_stat;
/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_STAT_H_*/
/* *INDENT-ON* */
