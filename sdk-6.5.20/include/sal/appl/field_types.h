/*! \file field_types.h
 * Purpose: Scan and print different field types
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef INCLUDE_SAL_APPL_FIELD_TYPES_H_
#define INCLUDE_SAL_APPL_FIELD_TYPES_H_

/*!
 * \brief enum field types, field type used for scan input /print output\n
 *  this enum also used to indicate the field type for getting field
 *  when adding values to this enum need also to add corresponding string to: SAL_FIELD_TYPE_STRINGS
 */
typedef enum
  {
    /**
     * None used to mark uncertain or undefined one
     * Printed NONE
     */
    SAL_FIELD_TYPE_NONE,
    /**
     * Default value needs to be defined, "true" or "false"
     * Printed always as TRUE/FALSE
     */
    SAL_FIELD_TYPE_BOOL,
    /**
     * For fields that may have negative values
     * Input should be strictly in decimal format
     */
    SAL_FIELD_TYPE_INT32,
    /**
     * Fields that are strictly zero or positive
     * Input may be in HEX form
     */
    SAL_FIELD_TYPE_UINT32,
    /**
     * Input and Output should be in: x.x.x.x format
     */
    SAL_FIELD_TYPE_IP4,
    /**
     * Input and Output should be in: x:x::x format
     */
    SAL_FIELD_TYPE_IP6,
    /**
     * field type is char array, print format plain string
 */
    SAL_FIELD_TYPE_STR,
    /** field type is short array, format AA:BB:CC:DD:EE:FF
     */
    SAL_FIELD_TYPE_MAC,
    /** field type is uint32 array, print format 0xaabbccdd 0xaabbccdd
     *  XML indication: ARRAY32 */
    SAL_FIELD_TYPE_ARRAY32,
    /** print format 11010110...
     *  XML indication: BITMAP */
    SAL_FIELD_TYPE_BITMAP,
    /** print enum format prints string for each valid value
     *  XML indication: ENUM  */
    SAL_FIELD_TYPE_ENUM,
    /**
     * Get port or bitmaps of ports as an input:
     * The supported options might be:
     * * getting a port as simple integer (i.e. '5')
     * * getting a bitmap of ports as logical port type (i.e. 'tm', 'fabric', 'nif' ...)
     *   See 'dnx_algo_port_logicals_type_e'
     * * Getting bitmaps of ports using port name (i.e. 'il', 'sfi256', 'xe2-xe4')
     * The result will be added to var type 'bcm_pbmp_t'
     */
    SAL_FIELD_TYPE_PORT,

    SAL_FIELD_TYPE_MAX
  } sal_field_type_e;

#define SAL_FIELD_TYPE_STRINGS \
{ \
    "none",\
    "boolean",\
    "int32",\
    "uint32",\
    "ip4",\
    "ip6",\
    "string",\
    "mac",\
    "array32",\
    "bitmap",\
    "enum",\
    "port"\
}

/**
 * \brief Parses input string and return value and field type
 * \par DIRECT INPUT
 *   \param [in] source - string representing field value
 *   \param [in] in_field_type - supposed field type to be matched to actual one, SAL_FIELD_TYPE_NONE means expecting any
 *   \param [in] value - pointer to buffer for field value
 *   \param [in] value_size - buffer size in bytes
 * \par DIRECT OUTPUT:
 *   \retval SAL_FIELD_TYPE_NONE if type was not recognized or contradicted in_field_type
 *   \retval real field type, supposed to be the same as in_field_type if it was non SAL_FIELD_TYPE_NONE
 * \par INDIRECT OUTPUT
 *   \param value - place for field value
 */
sal_field_type_e
sal_field_scan_str(char *source, sal_field_type_e in_field_type, void *value, int value_size);

/**
 * \brief Convert value to string
 * \par DIRECT INPUT
 *   \param [in] target - string where formatted value will be placed
 *   \param [in] target_size - maximum size of target string, output will be limited to this size
 *   \param [in] out_field_type - convert will be performed according to this field type
 *   \param [in] value - pointer to buffer holding value
 *   \param [in] value_size - value size in bytes
 * \par INDIRECT OUTPUT
 *   \param target - place for field value
 */
void
sal_field_format_str(char *target, int target_size, sal_field_type_e out_field_type, void *value, int value_size);

/**
 * \brief Returns string describing specified field type
 * \par DIRECT INPUT
 *   \param [in] field_type - field type which name is requested
 * \par DIRECT OUTPUT
 *   \retval name for field type
 *   \retval NULL if field type is illegal
 */
char*
sal_field_type_str(sal_field_type_e field_type);

/**
 * \brief Returns field type id
 * \par DIRECT INPUT
 *   \param [in] field_type_name - field type which name is requested
 * \par DIRECT OUTPUT
 *   \retval field type id
 *   \retval SAL_FIELD_TYPE_MAX if field type with such name does is illegal
 */
sal_field_type_e
sal_field_type_by_name(char *field_type_name);

#endif /* INCLUDE_SAL_APPL_FIELD_TYPES_H_ */
