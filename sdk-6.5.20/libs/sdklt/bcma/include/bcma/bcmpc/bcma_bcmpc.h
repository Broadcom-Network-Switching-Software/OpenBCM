/*! \file bcma_bcmpc.h
 *
 * Common functions for bcmpc commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPC_H
#define BCMA_BCMPC_H

#include <bcma/cli/bcma_cli_arg.h>
#include <bcma/cli/bcma_cli_token.h>

#include <bcmpc/bcmpc_phy.h>

#include <phymod/phymod_symbols.h>

/*! Maximum tokens buffer size in BCMPC CLI command. */
#define BCMA_BCMPC_CMD_BUFFER_MAX       256

/*!
 * \name The 'phy' command access flags.
 * \anchor BCMA_BCMPC_PHY_CMD_FLAG_xxx
 */

/*! \{ */

/*! Flag to display raw data only for PHY symbol(s). */
#define BCMA_BCMPC_PHY_CMD_FLAG_SYM_RAW        (1U << 0)

/*! Flag to list symbol information. */
#define BCMA_BCMPC_PHY_CMD_FLAG_SYM_LIST       (1U << 1)

/*! \} */

/*!
 * Structures for handling parsed result of the main symbol ID.
 */
typedef struct bcma_bcmpc_parse_desc_s {

    /*! register or address string. */
    char name[BCMA_BCMPC_CMD_BUFFER_MAX];

    /*! Numerical value of address string (if applicable). */
    uint32_t name32;

    /*! Lane index of the port, starting with 0. */
    uint32_t lane_index;

    /*! Pll index. */
    uint32_t pll_index;

} bcma_bcmpc_parse_desc_t;

/*!
 * \brief Initialize \ref bcma_bcmpc_parse_desc_t structure.
 *
 * \param [in] desc Pointer to the structure to be initialized.
 *
 * \retval 0 No errors
 * \retval -1 On error
 */
extern int
bcma_bcmpc_parse_desc_t_init(bcma_bcmpc_parse_desc_t *desc);

/*!
 * \brief Data conversion between structures.
 *
 * This function will initialize the BCMPC PHY-access override structure
 * \c ovrr first. If the input \c desc is not NULL, this function will then
 * convert contents in \c desc to \c ovrr.
 *
 * \param [in] desc Pointer to the structure to be converted from.
 * \param [in] ovrr Pointer to the structure to be converted to.
 *
 * \retval 0 No errors
 * \retval -1 On error
 */
extern int
bcma_bcmpc_desc2ovrr(bcma_bcmpc_parse_desc_t *desc,
                     bcmpc_phy_acc_ovrr_t *ovrr);

/*!
 * \brief Display error message for command line argument.
 *
 * \param [in] desc Name of command line argument.
 * \param [in] arg Specified value of argument (or NULL if none).
 *
 * \retval Always -1.
 */
extern int
bcma_bcmpc_parse_error(const char *desc, const char *arg);

/*!
 * \brief Parse CLI argument structure to BCMPC command tokens structure.
 *
 * This function parses each argument in \c args, and restore in \c ctoks in
 * format of BCMPC command tokens. If argument string is A=B, token A, B
 * will be tokenized into different arguments in a single token structure.
 *
 * \param [in] args CLI arguments to parse.
 * \param [out] ctoks Array of command token structures to store the result.
 * \param [in] max Maximum array size of command token structure.
 *
 * \retval NULL if all arguments are successfully parsed.
 * \retval Argument string which is failed to be parsed.
 */
extern const char *
bcma_bcmpc_parse_args(bcma_cli_args_t *args, bcma_cli_tokens_t *ctoks, int max);

/*!
 * \brief Parse CLI argument to BCMPC command parse descriptor.
 *
 * This function parse the address or register expression token to
 * \ref bcma_bcmpc_parse_desc_t \c desc.
 *
 * \param [in] str CLI argument to parse
 * \param [out] desc Pointer to BCMPC command parse descriptor.
 *
 * \retval 0 No errors
 * \retval -1 On error
 */
extern int
bcma_bcmpc_parse_desc(const char *str, bcma_bcmpc_parse_desc_t *desc);

/*!
 * \brief Encode field name/value pair into data for register modification.
 *
 * Encode specifed field value for modification of specified symbol to
 * \c and_masks and \c or_masks arrays.
 *
 * \param [in] symbol Symbol structure (single symbol)
 * \param [in] fnames Device-specfic list of all field names
 * \param [in] field Specified field name for modification
 * \param [in] value Specified field value for modification
 * \param [out] and_masks Data masks to be AND'ed
 * \param [out] or_masks Data masks to be OR'ed
 *
 * \retval 0 No errors
 * \retval -1 On error
 */
extern int
bcma_bcmpc_encode_field(const phymod_symbol_t *symbol, const char **fnames,
                        const char *field, const char *value,
                        uint32_t *and_masks, uint32_t *or_masks);

/*!
 * \brief Encode token structures into data for register modification.
 *
 * Parse the command token structures to get the register modification
 * data. The \c and_masks and \c or_masks arrays are ouputted for use of
 * arithemetic AND and OR operations with the orignal register raw data
 * respectively to get the modified result of the register.
 *
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] fnames Device-specfic list of all field names.
 * \param [in] ctoks_num Number of tokens in tokens array.
 * \param [in] ctoks Array of tokens with raw data or field value assignments.
 * \param [in] max Size of masks arrays.
 * \param [out] and_masks Data masks to be AND'ed.
 * \param [out] or_masks Data masks to be OR'ed.
 *
 * \retval 0 No errors.
 * \retval -1 On error.
 */
extern int
bcma_bcmpc_encode_fields_from_tokens(const phymod_symbol_t *symbol,
                                     const char **fnames,
                                     int ctoks_num,
                                     const bcma_cli_tokens_t *ctoks,
                                     int max,
                                     uint32_t *and_masks,
                                     uint32_t *or_masks);

/*!
 * \brief PHY register read/write.
 *
 * This function will perform a read operation if \c data_ptr is NULL,
 * otherwise a write operation will be performed.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical device port.
 * \param [in] desc PHY access command descriptor.
 * \param [in] reg PHY register address to access.
 * \param [in,out] data_ptr Pointer to the data to be written.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_phy_reg(int unit, int port, bcma_bcmpc_parse_desc_t *desc,
                   uint32_t reg, uint32_t *data_ptr);

/*!
 * \brief Read or write PHY register data from a given port macro (PM).
 *
 * This function will perform a read operation if \c data_ptr is NULL,
 * otherwise a write operation will be performed.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_num PM number.
 * \param [in] desc PHY access command descriptor.
 * \param [in] reg PHY register address to access.
 * \param [in,out] data_ptr Pointer to the data to be written.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_phy_raw_reg(int unit, int pm_num, bcma_bcmpc_parse_desc_t *desc,
                       uint32_t reg, uint32_t *data_ptr);

/*!
 * \brief PHY register symbol read/write.
 *
 * This function will perform a read operation if \c ctoks is NULL,
 * otherwise a write operation will be performed.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical device port.
 * \param [in] desc PHY access command descriptor.
 * \param [in] flags PHY access command flags.
 * \param [in] ctoks Specify the fields values for write operation if not NULL.
 * \param [in] ctoks_num Number of \c ctoks.
 *
 * \return SHR_E_xxx return values.
 */
int
bcma_bcmpc_phy_sym(int unit, int port, bcma_bcmpc_parse_desc_t *desc,
                   uint32_t flags, bcma_cli_tokens_t *ctoks, int ctoks_num);

/*!
 * \brief PHY register symbols list.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical device port.
 * \param [in] name Symbol name.
 * \param [in] flags PHY access command flags.
 *
 * \return SHR_E_xxx return values.
 */
extern int
bcma_bcmpc_phy_sym_list(int unit, int port, const char *name, uint32_t flags);

#endif /* BCMA_BCMPC_H */
