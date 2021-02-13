/** \file dnxf_data_internal_intr.h
 * 
 * MODULE DEVICE DATA - DNXF_DATA
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
#ifndef _DNXF_DATA_INTERNAL_INTR_H_
/*{*/
#define _DNXF_DATA_INTERNAL_INTR_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>
/*
 * }
 */

/*!
* \brief This file is only used by DNX (JR2 family). Including it by
* software that is not specific to DNX is an error.
*/
#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * TYPEDEFS:
 * {
 */
/**
 * \brief 
 * Submodule enum
 */
typedef enum
{
    dnxf_data_intr_submodule_general,
    dnxf_data_intr_submodule_ser,

    /**
     * Must be last one!
     */
    _dnxf_data_intr_submodule_nof
} dnxf_data_intr_submodule_e;

/*
 * }
 */

/*
 * SUBMODULE GENERAL:
 * {
 */
/*
 * Features
 */
/* Feature enum - placed in if header (should be exposed to component user */
/* Get Data */
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
int dnxf_data_intr_general_feature_get(
    int unit,
    dnxf_data_intr_general_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnxf_data_intr_general_define_nof_interrupts,
    dnxf_data_intr_general_define_polled_irq_mode,

    /**
     * Must be last one!
     */
    _dnxf_data_intr_general_define_nof
} dnxf_data_intr_general_define_e;

/* Get Data */
/**
 * \brief returns numeric data of nof_interrupts
 * Module - 'intr', Submodule - 'general', data - 'nof_interrupts'
 * Number of interrupts in the device.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_interrupts - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnxf_data_intr_general_nof_interrupts_get(
    int unit);

/**
 * \brief returns numeric data of polled_irq_mode
 * Module - 'intr', Submodule - 'general', data - 'polled_irq_mode'
 * Are interrupts supported polling mode.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     polled_irq_mode - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnxf_data_intr_general_polled_irq_mode_get(
    int unit);

/*
 * Tables
 */
/**
 * \brief 
 * Table enum
 */
typedef enum
{
    dnxf_data_intr_general_table_active_on_init,
    dnxf_data_intr_general_table_disable_on_init,
    dnxf_data_intr_general_table_disable_print_on_init,

    /**
     * Must be last one!
     */
    _dnxf_data_intr_general_table_nof
} dnxf_data_intr_general_table_e;

/* Get Data */
/**
 * \brief get table active_on_init entry 
 * interrupt active on init
 * 
 * \param [in] unit - unit #
 * \param [in] offset - active interrupt offset
 * 
 * \return
 *     active_on_init - returns the relevant entry values grouped in struct - see dnxf_data_intr_general_active_on_init_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxf_data_intr_general_active_on_init_t * dnxf_data_intr_general_active_on_init_get(
    int unit,
    int offset);

/**
 * \brief get table disable_on_init entry 
 * interrupt disabled on init
 * 
 * \param [in] unit - unit #
 * \param [in] offset - disable interrupt offset
 * 
 * \return
 *     disable_on_init - returns the relevant entry values grouped in struct - see dnxf_data_intr_general_disable_on_init_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxf_data_intr_general_disable_on_init_t * dnxf_data_intr_general_disable_on_init_get(
    int unit,
    int offset);

/**
 * \brief get table disable_print_on_init entry 
 * interrupt disable print on init
 * 
 * \param [in] unit - unit #
 * \param [in] offset - disable print on init offset
 * 
 * \return
 *     disable_print_on_init - returns the relevant entry values grouped in struct - see dnxf_data_intr_general_disable_print_on_init_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxf_data_intr_general_disable_print_on_init_t * dnxf_data_intr_general_disable_print_on_init_get(
    int unit,
    int offset);

/* Get Value Str */
/**
 * \brief Get table value as string
 * Module - 'intr', Submodule - 'general', table - 'active_on_init'
 * interrupt active on init
 * 
 * \param [in] unit - Unit #
 * \param [out] buffer - buffer for the value
 * \param [in] key0 - key #0 index - 0 if not used
 * \param [in] key1 - key #1 index - 0 if not used
 * \param [in] value_index - value index to get - the index is alligned with values array in table structure
 * 
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnxf_data_intr_general_active_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/**
 * \brief Get table value as string
 * Module - 'intr', Submodule - 'general', table - 'disable_on_init'
 * interrupt disabled on init
 * 
 * \param [in] unit - Unit #
 * \param [out] buffer - buffer for the value
 * \param [in] key0 - key #0 index - 0 if not used
 * \param [in] key1 - key #1 index - 0 if not used
 * \param [in] value_index - value index to get - the index is alligned with values array in table structure
 * 
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnxf_data_intr_general_disable_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/**
 * \brief Get table value as string
 * Module - 'intr', Submodule - 'general', table - 'disable_print_on_init'
 * interrupt disable print on init
 * 
 * \param [in] unit - Unit #
 * \param [out] buffer - buffer for the value
 * \param [in] key0 - key #0 index - 0 if not used
 * \param [in] key1 - key #1 index - 0 if not used
 * \param [in] value_index - value index to get - the index is alligned with values array in table structure
 * 
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnxf_data_intr_general_disable_print_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/* Get Table info */
/**
 * \brief get general info table about table (for example key size)active_on_init info
 * interrupt active on init
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     active_on_init - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnxf_data_intr_general_active_on_init_info_get(
    int unit);

/**
 * \brief get general info table about table (for example key size)disable_on_init info
 * interrupt disabled on init
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     disable_on_init - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnxf_data_intr_general_disable_on_init_info_get(
    int unit);

/**
 * \brief get general info table about table (for example key size)disable_print_on_init info
 * interrupt disable print on init
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     disable_print_on_init - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnxf_data_intr_general_disable_print_on_init_info_get(
    int unit);

/*
 * SUBMODULE SER:
 * {
 */
/*
 * Features
 */
/* Feature enum - placed in if header (should be exposed to component user */
/* Get Data */
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
int dnxf_data_intr_ser_feature_get(
    int unit,
    dnxf_data_intr_ser_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{

    /**
     * Must be last one!
     */
    _dnxf_data_intr_ser_define_nof
} dnxf_data_intr_ser_define_e;

/* Get Data */
/*
 * Tables
 */
/**
 * \brief 
 * Table enum
 */
typedef enum
{
    dnxf_data_intr_ser_table_mem_mask,
    dnxf_data_intr_ser_table_xor_mem,

    /**
     * Must be last one!
     */
    _dnxf_data_intr_ser_table_nof
} dnxf_data_intr_ser_table_e;

/* Get Data */
/**
 * \brief get table mem_mask entry 
 * memory need masked
 * 
 * \param [in] unit - unit #
 * \param [in] index - memory masked index
 * 
 * \return
 *     mem_mask - returns the relevant entry values grouped in struct - see dnxf_data_intr_ser_mem_mask_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxf_data_intr_ser_mem_mask_t * dnxf_data_intr_ser_mem_mask_get(
    int unit,
    int index);

/**
 * \brief get table xor_mem entry 
 * XOR memory information
 * 
 * \param [in] unit - unit #
 * \param [in] index - XOR memory index
 * 
 * \return
 *     xor_mem - returns the relevant entry values grouped in struct - see dnxf_data_intr_ser_xor_mem_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxf_data_intr_ser_xor_mem_t * dnxf_data_intr_ser_xor_mem_get(
    int unit,
    int index);

/* Get Value Str */
/**
 * \brief Get table value as string
 * Module - 'intr', Submodule - 'ser', table - 'mem_mask'
 * memory need masked
 * 
 * \param [in] unit - Unit #
 * \param [out] buffer - buffer for the value
 * \param [in] key0 - key #0 index - 0 if not used
 * \param [in] key1 - key #1 index - 0 if not used
 * \param [in] value_index - value index to get - the index is alligned with values array in table structure
 * 
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnxf_data_intr_ser_mem_mask_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/**
 * \brief Get table value as string
 * Module - 'intr', Submodule - 'ser', table - 'xor_mem'
 * XOR memory information
 * 
 * \param [in] unit - Unit #
 * \param [out] buffer - buffer for the value
 * \param [in] key0 - key #0 index - 0 if not used
 * \param [in] key1 - key #1 index - 0 if not used
 * \param [in] value_index - value index to get - the index is alligned with values array in table structure
 * 
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnxf_data_intr_ser_xor_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/* Get Table info */
/**
 * \brief get general info table about table (for example key size)mem_mask info
 * memory need masked
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     mem_mask - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnxf_data_intr_ser_mem_mask_info_get(
    int unit);

/**
 * \brief get general info table about table (for example key size)xor_mem info
 * XOR memory information
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     xor_mem - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnxf_data_intr_ser_xor_mem_info_get(
    int unit);

/*
 * FUNCTIONS:
 * {
 */
/**
 * \brief Init module
 * 
 * \param [in] unit - Unit #
 * \param [out] module_data - pointer to module data
 * 
 * \return
 *     rv - see 'shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnxf_data_intr_init(
    int unit,
    dnxc_data_module_t *module_data);

/*
 * }
 */
/*}*/
#endif /*_DNXF_DATA_INTERNAL_INTR_H_*/
/* *INDENT-ON* */
