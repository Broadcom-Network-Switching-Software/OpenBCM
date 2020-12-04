/*! \file bcma_bcmlt_enum_map.h
 *
 * Functions related to string mapping to enum in BCMLT module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLT_ENUM_MAP_H
#define BCMA_BCMLT_ENUM_MAP_H

#include <bcma/cli/bcma_cli_parse.h>

#include <bcmlt/bcmlt.h>

/*!
 * \brief Get string mapping for logical or physical table operation codes.
 *
 * \param [in] physical True if for physical table operation codes.
 *
 * \return Table operation codes enum map structure.
 */
extern const bcma_cli_parse_enum_t *
bcma_bcmlt_opcode_enum_get(bool physical);

/*!
 * \brief Get string mapping for entry attributes.
 *
 * \return Entry attributes enum map structure.
 */
extern const bcma_cli_parse_enum_t *
bcma_bcmlt_ent_attr_enum_get(void);

/*!
 * \brief Get string mapping for transaction type.
 *
 * \return Transaction types enum map structure.
 */
extern const bcma_cli_parse_enum_t *
bcma_bcmlt_trans_type_enum_get(void);

/*!
 * \brief Get string mapping for priority level.
 *
 * \return Priority level enum map structure.
 */
extern const bcma_cli_parse_enum_t *
bcma_bcmlt_commit_pri_enum_get(void);

/*!
 * \brief Get string mapping for asynchronous notification.
 *
 * \return Asynchronous notification enum map structure.
 */
extern const bcma_cli_parse_enum_t *
bcma_bcmlt_notify_opt_enum_get(void);

/*!
 * \brief Format table operation code to string.
 *
 * \param [in] opc Logical or physical table operation code.
 * \param [in] physical True if \c opc is physical table operation code.
 *
 * \return String of the specified physical table operation.
 */
extern const char *
bcma_bcmlt_opcode_format(int opc, bool physical);

/*!
 * \brief Format table operation code from BCMLT entry information.
 *
 * This function can retrieve the entry operation code and entry table type
 * (logical or physical) from BCMLT entry information.
 *
 * \param [in] bcmlt_ei Entry information returned from BCMLT module.
 * \param [out] opcode Logical or physical operation code if not NULL.
 * \param [out] physical True if the entry is a physical table if not NULL.
 *
 * \return String of logical or physical table operation.
 */
extern const char *
bcma_bcmlt_opcode_format_by_entry_info(bcmlt_entry_info_t *bcmlt_ei,
                                       int *opcode, bool *physical);

/*!
 * \brief Format asynchronous notification to string.
 *
 * \param [in] ntf Notification code.
 *
 * \return String of the specified notification code.
 */
extern const char *
bcma_bcmlt_notification_format(bcmlt_notif_option_t ntf);

/*!
 * \brief Format priority level to string.
 *
 * \param [in] priority Priority level.
 *
 * \return String of the specified priority level.
 */
extern const char *
bcma_bcmlt_priority_format(bcmlt_priority_level_t priority);

/*!
 * \brief Format transaction type to string.
 *
 * \param [in] type Transaction type.
 *
 * \return String of the specified transaction type.
 */
extern const char *
bcma_bcmlt_trans_type_format(bcmlt_trans_type_t type);

#endif /* BCMA_BCMLT_ENUM_MAP_H */
