/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PRE_COMPILED_XML_PARSER_INTERNAL_H
#define _PRE_COMPILED_XML_PARSER_INTERNAL_H

#include <sal/types.h>
#include <sal/core/libc.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#if defined(BCM_DNX_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT)

/**
 * \brief - Callback to get a pre_compiled string buffer and its size, by given XML file name.
 *
 *   \param [in] filepath - full path, either relative or absolute for XML file,
 *       that will be used to find the correct string buffer.
 *   \param [out] buf - Pointer to store the string buffer.
 *   \param [out] size_p - Size of the string buffer.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
typedef shr_error_e(
    *dbx_pre_compiled_xml_to_buf_cb) (
    char *filePath,
    char **buf,
    long int *size_p);

/**
 * \brief - Get handle for top node inside pre compiled string.
 *
 *   \param [in] filepath - full path, either relative or absolute for XML file,
 *       that will be used to find the correct string buffer.
 *   \param [in] topname - name of top XML node
 *   \param [in] flags - options for file opening, see above CONF_OPEN_*
 *   \param [in] xml_to_buf - callback of the relevant xml_to_buff function.
 *
 * \return
 *   xml_node if XML pre compiled string exists and has top node with this name
 *   NULL for any failure
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void *dbx_xml_top_get_internal(
    char *filepath,
    char *topname,
    int flags,
    dbx_pre_compiled_xml_to_buf_cb xml_to_buf);

/**
 * \brief - Decode a string by given key.
 *
 *   \param [in] key - Key to decode with.
 *   \param [in] input - Pointer to the coded string.
 *   \param [out] output - Pointer to the memory where the decoded string will be saved.
 *   \param [out] size - Size of the decoded string.
 *
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void pre_compiled_xml_to_decoded_buf(
    char *key,
    char *input,
    char **output,
    long int *size);

#ifdef BCM_PETRA_SUPPORT
/**
 * \brief get handle for top node inside XML file
 * \par DIRECT INPUT
 *   \param [in] unit - Unit
 *   \param [in] filepath - full path, either relative or absolute for XML file
 *   \param [in] topname - name of top XML node
 *   \param [in] flags - options for file opening, see above CONF_OPEN_*
 * \par DIRECT OUTPUT:
 *   \retval xml_node if XML file exists and has top node with this name
 *   \retval NULL for any failure
 */
xml_node pre_compiled_dbx_xml_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags);
#endif /* BCM_PETRA_SUPPORT */
#endif /* BCM_DNX_SUPPORT || BCM_PETRA_SUPPORT || BCM_DNXF_SUPPORT */
#endif /* _PRE_COMPILED_XML_PARSER_INTERNAL_H */
