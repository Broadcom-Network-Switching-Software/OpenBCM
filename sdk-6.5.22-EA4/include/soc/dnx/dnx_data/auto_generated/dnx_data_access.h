/** \file dnx_data_access.h
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
#ifndef _DNX_DATA_ACCESS_H_
/*{*/
#define _DNX_DATA_ACCESS_H_
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
#include <bcm/port.h>
#include <soc/chip.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_access.h>
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
 * \brief Init default data structure - dnx_data_if_access
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
shr_error_e dnx_data_if_access_init(
    int unit);

/*
 * }
 */

/*
 * SUBMODULE  - MDIO:
 * mdio related data
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
     * Must be last one!
     */
    _dnx_data_access_mdio_feature_nof
} dnx_data_access_mdio_feature_e;

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
    *dnx_data_access_mdio_feature_get_f) (
    int unit,
    dnx_data_access_mdio_feature_e feature);

/**
 * \brief returns define data of nof_pms_per_ring
 * Module - 'access', Submodule - 'mdio', data - 'nof_pms_per_ring'
 * Number of Fabric PMs per MDIO ring.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_pms_per_ring - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_access_mdio_nof_pms_per_ring_get_f) (
    int unit);

/**
 * \brief returns define data of ring_offset
 * Module - 'access', Submodule - 'mdio', data - 'ring_offset'
 * Offset for first fabric ring.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     ring_offset - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_access_mdio_ring_offset_get_f) (
    int unit);

/**
 * \brief returns numeric data of ext_divisor
 * Module - 'access', Submodule - 'mdio', data - 'ext_divisor'
 * external MDIO clock rate divisor
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     ext_divisor - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_access_mdio_ext_divisor_get_f) (
    int unit);

/**
 * \brief returns numeric data of int_divisor
 * Module - 'access', Submodule - 'mdio', data - 'int_divisor'
 * internal MDIO clock rate divisor
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     int_divisor - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_access_mdio_int_divisor_get_f) (
    int unit);

/**
 * \brief returns numeric data of int_div_out_delay
 * Module - 'access', Submodule - 'mdio', data - 'int_div_out_delay'
 * number of clock delay between the rising edge of MDC and the starting data of MDIO for internal divisor
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     int_div_out_delay - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_access_mdio_int_div_out_delay_get_f) (
    int unit);

/**
 * \brief returns numeric data of ext_div_out_delay
 * Module - 'access', Submodule - 'mdio', data - 'ext_div_out_delay'
 * number of clock delay between the rising edge of MDC and the starting data of MDIO for external divisor
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     ext_div_out_delay - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_access_mdio_ext_div_out_delay_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_ACCESS - MDIO:
 * {
 */
/**
 * \brief Interface for access mdio data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_access_mdio_feature_get_f feature_get;
    /**
     * returns define data of nof_pms_per_ring
     */
    dnx_data_access_mdio_nof_pms_per_ring_get_f nof_pms_per_ring_get;
    /**
     * returns define data of ring_offset
     */
    dnx_data_access_mdio_ring_offset_get_f ring_offset_get;
    /**
     * returns numeric data of ext_divisor
     */
    dnx_data_access_mdio_ext_divisor_get_f ext_divisor_get;
    /**
     * returns numeric data of int_divisor
     */
    dnx_data_access_mdio_int_divisor_get_f int_divisor_get;
    /**
     * returns numeric data of int_div_out_delay
     */
    dnx_data_access_mdio_int_div_out_delay_get_f int_div_out_delay_get;
    /**
     * returns numeric data of ext_div_out_delay
     */
    dnx_data_access_mdio_ext_div_out_delay_get_f ext_div_out_delay_get;
} dnx_data_if_access_mdio_t;

/*
 * }
 */

/*
 * SUBMODULE  - RCPU:
 * rcpu related data
 * {
 */
/*
 * Table value structure
 */
/**
 * \brief Holds values of submodule rcpu table rx
 * Table info:
 * general data about rx
 */
typedef struct
{
    /**
     * valid ports on which RCPU packets can be received by slave device
     */
    bcm_pbmp_t pbmp;
} dnx_data_access_rcpu_rx_t;

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
    _dnx_data_access_rcpu_feature_nof
} dnx_data_access_rcpu_feature_e;

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
    *dnx_data_access_rcpu_feature_get_f) (
    int unit,
    dnx_data_access_rcpu_feature_e feature);

/**
 * \brief get table rx entry 
 * general data about rx
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     rx - returns the relevant entry values grouped in struct - see dnx_data_access_rcpu_rx_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_access_rcpu_rx_t *(
    *dnx_data_access_rcpu_rx_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_ACCESS - RCPU:
 * {
 */
/**
 * \brief Interface for access rcpu data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_access_rcpu_feature_get_f feature_get;
    /**
     * get table rx entry 
     */
    dnx_data_access_rcpu_rx_get_f rx_get;
    /**
     * get general info table about table (for example key size)rx info
     */
    dnxc_data_table_info_get_f rx_info_get;
} dnx_data_if_access_rcpu_t;

/*
 * }
 */

/*
 * SUBMODULE  - BLOCKS:
 * HW blocks related data
 * {
 */
/*
 * Table value structure
 */
/**
 * \brief Holds values of submodule blocks table override
 * Table info:
 * blocks enable/disable override
 */
typedef struct
{
    /**
     * See soc_block_type_t
     */
    char *block_type;
    /**
     * instance ID within block type
     */
    int block_instance;
    /**
     * The value to override
     */
    int value;
} dnx_data_access_blocks_override_t;

/**
 * \brief Holds values of submodule blocks table soft_init
 * Table info:
 * soft_init info
 */
typedef struct
{
    /**
     * Value to be written to initialize soft init
     */
    soc_pbmp_t trigger_value;
} dnx_data_access_blocks_soft_init_t;

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
    _dnx_data_access_blocks_feature_nof
} dnx_data_access_blocks_feature_e;

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
    *dnx_data_access_blocks_feature_get_f) (
    int unit,
    dnx_data_access_blocks_feature_e feature);

/**
 * \brief get table override entry 
 * blocks enable/disable override
 * 
 * \param [in] unit - unit #
 * \param [in] index - running index
 * 
 * \return
 *     override - returns the relevant entry values grouped in struct - see dnx_data_access_blocks_override_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_access_blocks_override_t *(
    *dnx_data_access_blocks_override_get_f) (
    int unit,
    int index);

/**
 * \brief get table soft_init entry 
 * soft_init info
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     soft_init - returns the relevant entry values grouped in struct - see dnx_data_access_blocks_soft_init_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_access_blocks_soft_init_t *(
    *dnx_data_access_blocks_soft_init_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_ACCESS - BLOCKS:
 * {
 */
/**
 * \brief Interface for access blocks data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_access_blocks_feature_get_f feature_get;
    /**
     * get table override entry 
     */
    dnx_data_access_blocks_override_get_f override_get;
    /**
     * get general info table about table (for example key size)override info
     */
    dnxc_data_table_info_get_f override_info_get;
    /**
     * get table soft_init entry 
     */
    dnx_data_access_blocks_soft_init_get_f soft_init_get;
    /**
     * get general info table about table (for example key size)soft_init info
     */
    dnxc_data_table_info_get_f soft_init_info_get;
} dnx_data_if_access_blocks_t;

/*
 * }
 */

/*
 * MODULE INTERFACE DNX_DATA_IF_ACCESS:
 * {
 */
/**
 * \brief Interface for access data
 */
typedef struct
{
    /**
     * Interface for access mdio data
     */
    dnx_data_if_access_mdio_t mdio;
    /**
     * Interface for access rcpu data
     */
    dnx_data_if_access_rcpu_t rcpu;
    /**
     * Interface for access blocks data
     */
    dnx_data_if_access_blocks_t blocks;
} dnx_data_if_access_t;

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
extern dnx_data_if_access_t dnx_data_access;
/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_ACCESS_H_*/
/* *INDENT-ON* */
