/*! \file bcma_testutil_common.h
 *
 * Common utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_COMMON_H
#define BCMA_TESTUTIL_COMMON_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_port.h>
#include <bcmdrd/bcmdrd_types.h>

/*! Double-words in port bit maps. */
#define BCMA_TESTUTIL_PBMP_DWORD_MAX (((BCMDRD_PBMP_WORD_MAX - 1) >> 1) + 1)

/*! Number of bits in a double-word integer. */
#define BCMA_TESTUTIL_DWORD_WIDTH 64

/*! The min index of table. */
#define BCMA_TESTUTIL_TABLE_MIN_INDEX (-1)

/*! The max index of table. */
#define BCMA_TESTUTIL_TABLE_MAX_INDEX (-2)

/*! The middle index of table. */
#define BCMA_TESTUTIL_TABLE_MID_INDEX (-3)

/*!
 * \brief Get name by id.
 *
 * \param [in] name_id_map Array of name id map.
 * \param [in] map_len Array length.
 * \param [in] id Identifier to look for.
 *
 * \return The pointer to name and NULL otherwise.
 */
extern const char*
bcma_testutil_id2name(const shr_enum_map_t *name_id_map, int map_len, int id);

/*!
 * \brief Get id by name.
 *
 * \param [in] name_id_map Array of name id map.
 * \param [in] map_len Array length.
 * \param [in] name Name to look for.
 *
 * \return identifier on success and -1 otherwise.
 */
extern int
bcma_testutil_name2id(const shr_enum_map_t *name_id_map, int map_len,
                      const char *name);

/*!
 * \brief Parse port bitmap from string.
 *
 * \param [in] str String to be passed.
 * \param [out] pbmp port bitmap after parsing.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_parse_port_bmp(const char *str, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Parse port list from string.
 *
 * \param [in] str String to be passed.
 * \param [out] port_list port list after parsing.
 * \param [in] list_size array size of port_list
 * \param [out] actual_count actual port number after parsing.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_parse_port_list(const char *str, uint32_t *port_list,
                              uint32_t list_size, uint32_t *actual_count);

/*!
 * \brief Parse table index from string.
 *
 * \param [in] str String to be passed.
 * \param [out] tbl_idx Table index after parsing.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_parse_table_index(const char* str, int *tbl_idx);

/*!
 * \brief Get default port bitmap.
 *
 * \param [in] unit Unit number.
 * \param [in] port_type_bmp The bitmap of port type (see BCMDRD_PORT_TYPE_XXX).
 * \param [out] pbmp Default port bitmap.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_default_port_bmp_get(int unit, uint32_t port_type_bmp,
                                   bcmdrd_pbmp_t *pbmp);


/*!
 * \brief Get default port list.
 *
 * \param [in] unit Unit number.
 * \param [in] port_type_bmp The bitmap of port type (see BCMDRD_PORT_TYPE_XXX).
 * \param [out] port_list Default port list.
 * \param [in] list_size array size of port_list
 * \param [out] actual_count actual port number in the list.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_default_port_list_get(int unit, uint32_t port_type_bmp,
                                    uint32_t *port_list, uint32_t list_size,
                                    uint32_t *actual_count);


/*!
 * \brief Get the depth of logical table field.
 *
 * \param [in] unit Unit number.
 * \param [in] tab Logical table name.
 * \param [in] field Field name.
 * \param [out] depth Number of elements in the field array.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_lt_field_depth_get(int unit, const char *tab, const char *field,
                                 int *depth);

/*!
 * \brief Get default port speed list.
 *
 * \param [in] unit Unit number.
 * \param [in] list_size Size of speed_list.
 * \param [out] speed_list Port speed list.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_default_port_speed_list_get(int unit, uint32_t list_size,
                                          uint32_t *speed_list);

/*!
 * \brief Get management port speed list.
 *
 * \param [in] unit Unit number.
 * \param [in] list_size Size of speed_list.
 * \param [out] speed_list Management port speed list, array index is pipe no.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_mgmt_port_speed_list_get(int unit, uint32_t list_size,
                                       uint32_t *speed_list);

/*!
 * \brief Get core clock frequency.
 *
 * \param [in] unit Unit number.
 * \param [in] str_len String length of core_clk_freq_str.
 * \param [out] core_clk_freq_str Ptr to core clock frequency string.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_core_clk_freq_get(int unit, uint32_t str_len,
                                char *core_clk_freq_str);

/*!
 * \brief Get pp clock frequency.
 *
 * \param [in] unit Unit number.
 * \param [in] str_len String length of pp_clk_freq_str.
 * \param [out] pp_clk_freq_str Ptr to pp clock frequency string.
 *
 * \return SHR_E_NONE on success and error for otherwise.
 */
extern int
bcma_testutil_pp_clk_freq_get(int unit, uint32_t str_len,
                              char *pp_clk_freq_str);

#endif /* BCMA_TESTUTIL_COMMON_H */
