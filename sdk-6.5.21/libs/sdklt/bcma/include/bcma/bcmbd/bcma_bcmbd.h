/*! \file bcma_bcmbd.h
 *
 * Common functions for bcmbd commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBD_H
#define BCMA_BCMBD_H

#include <shr/shr_pb.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_cli_token.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_ser.h>

/*! Maximum tokens buffer size in BCMBD CLI command. */
#define BCMA_BCMBD_CMD_BUFFER_MAX       256

/*! Maximum block types in BCMBD CLI command. */
#define BCMA_BCMBD_MAX_BLKTYPES_PER_SYM  BCMDRD_SYM_INFO_MAX_BLKTYPES

/*! Vectors for parsing sub-commands */
typedef struct bcma_bcmbd_vect_s {

    /*! Sub-command name */
    const char *id;

    /*! dispatched function of the sub-command */
    int (*v)(bcma_cli_args_t *args);

} bcma_bcmbd_vect_t;

/*!
 * Basic structure for handling parsed result of the main symbol ID, i.e.
 * a register/memory name and its instances, indices, port range, etc.
 */
typedef struct bcma_bcmbd_basic_id_s {

    /*! Structure is valid. */
    int valid;

    /*! Original source string. */
    char id[BCMA_BCMBD_CMD_BUFFER_MAX];

    /*! First argument string. */
    char name[BCMA_BCMBD_CMD_BUFFER_MAX];

    /*! Numerical value of first argument string (if applicable). */
    uint32_t name32;

    /*! Optional address extension. */
    uint32_t ext32;

    /*! First index (if given). */
    int start;

    /*! Last index (if given). */
    int end;

} bcma_bcmbd_basic_id_t;

/*!
 * \name Output formatting flags.
 * \anchor BCMA_BCMBD_CMD_IDF_xxx
 */

/*! \{ */

/*! Do not decode fields. */
#define BCMA_BCMBD_CMD_IDF_RAW       0x1

/*! Only show contents if non-zero. */
#define BCMA_BCMBD_CMD_IDF_NONZERO   0x2

/*! Display fields in compact mode. */
#define BCMA_BCMBD_CMD_IDF_COMPACT   0x4

/*! Only show contents if the data is different from the default value. */
#define BCMA_BCMBD_CMD_IDF_DIFF      0x8

/*! } */

/*!
 * Structures for handling parsed result of the main symbol ID.
 */
typedef struct bcma_bcmbd_id_s {

    /*! Original source string. */
    char id[BCMA_BCMBD_CMD_BUFFER_MAX];

    /*! Symbol address. */
    bcma_bcmbd_basic_id_t addr;

    /*! Block info (if applicable). */
    bcma_bcmbd_basic_id_t block;

    /*! Unique pipe in access type (if applicable). */
    bcma_bcmbd_basic_id_t pipe;

    /*! Port range (if applicable). */
    bcma_bcmbd_basic_id_t port;

    /*! Output flags. */
    uint32_t flags;

    /*! Optional symbol output filter function. */
    bcmdrd_symbol_custom_filter_cb_f ccb;

    /*! Context for the symbol output filter function. */
    void *ccb_cookie;

} bcma_bcmbd_id_t;

/*!
 * Information to be parsed from HMI access commands.
 */
typedef struct bcma_bcmbd_parse_info_s {

    /*! If non-zero, this is where field/symbol data starts. */
    uint32_t data_start;

    /*! Flags for output processing (\ref BCMA_BCMBD_CMD_IDF_xxx). */
    uint32_t cmd_flags;

    /*! Symbol flags which must be present. */
    uint32_t sym_pflags;

    /*! Symbol flags which must be absent. */
    uint32_t sym_aflags;

    /*! Field flags which must be present. */
    uint32_t fld_pflags;

    /*! Field flags which must be absent. */
    uint32_t fld_aflags;

    /*! String to indentify a symbol instance. */
    const char *identifier;

    /*! Optional view name for multi-view symbols. */
    const char *view;

    /*! Optional list of field names to show. */
    const bcma_cli_tokens_t *fld_names;

} bcma_bcmbd_parse_info_t;

/*!
 * \brief Parse HMI access command line.
 *
 * This function is used to parse arguments for set, get, etc.
 *
 * \param [in] unit Unit number.
 * \param [in] ctoks Array of command line tokens.
 * \param [in] ctoks_num Number of command line tokens.
 * \param [out] pi Parsed information will be placed here.
 *
 * \retval 0 No errors.
 * \retval -1 Parse error.
 */
extern int
bcma_bcmbd_parse_cmd(int unit, bcma_cli_tokens_t *ctoks, int ctoks_num,
                     bcma_bcmbd_parse_info_t *pi);

/*!
 * \brief Get device-specific block type from DRD symbol info.
 *
 * Decode device-specific block type from DRD symbol info.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Index of block type to be decoded from DRD symbol info.
 * \param [in] info DRD symbol info.
 *
 * \retval Device-specific block type, otherwise -1.
 */
extern int
bcma_bcmbd_info_block_type_get(int unit, int idx, uint32_t info);

/*!
 * \brief Block type validation in DRD symbol.
 *
 * Check if the specified block type is valid for in the specified
 * DRD symbol info.
 *
 * \param [in] unit Unit number.
 * \param [in] blktype Device-specific block type.
 * \param [out] info DRD symbol info.
 *
 * \retval 1 if block type is valid for the symbol info, otherwise 0.
 */
extern int
bcma_bcmbd_info_block_type_valid(int unit, int blktype, uint32_t info);

/*!
 * \brief Convert value from bcmbd command tokens.
 *
 * This function will convert flags information from token structure
 * into pflags/aflags or pinfo/ainfo respectively.
 *
 * The token array in \c ctok must be the argument name followed by
 * the list of flag names.
 *
 * If the token argument starts with a exclamation mark(!), the converted
 * value will added to absent information \c aflags or \c ainfo.
 * Otherwise, the converted flag will be added to present information
 * \c pflags or \c pinfo.
 *
 * \param [in] unit Unit number.
 * \param [in] ctok Command token to convert.
 * \param [out] pflags Converted flags with "set" attribute.
 * \param [out] aflags Converted flags with "not set" attribute.
 *
 * \retval 0 convert the token to flags successfully.
 * \retval -1 Failed to convert the token to flags.
 */
extern int
bcma_bcmbd_tok2flags(int unit, const bcma_cli_tokens_t *ctok,
                     uint32_t *pflags, uint32_t *aflags);

/*!
 * \brief Convert value from bcmbd command tokens.
 *
 * This function will convert field flags from token structure into
 * pflags or aflags.
 *
 * The token array in \c ctok must be the argument name followed by
 * the list of flag names.
 *
 * If the token argument starts with a exclamation mark(!), the converted
 * value will be added to absent information \c aflags.
 * Otherwise, the converted flag will be added to present information
 * \c pflags.
 *
 * \param [in] unit Unit number.
 * \param [in] ctok Command token to convert.
 * \param [out] pflags Converted flags with "set" attribute.
 * \param [out] aflags Converted flags with "not set" attribute.
 *
 * \retval 0 convert the token to field flags successfully.
 * \retval -1 Failed to convert the token to field flags.
 */
int
bcma_bcmbd_tok2fieldflags(int unit, const bcma_cli_tokens_t *ctok,
                          uint32_t *pflags, uint32_t *aflags);

/*!
 * \brief Get block name from block type.
 *
 * Convert device-specified block type to device-specified block name.
 *
 * \param [in] unit Unit number.
 * \param [in] blktype Block type to convert.
 *
 * \retval Device-specified block name on success.
 * \retval NULL on failure.
 */
extern const char *
bcma_bcmbd_block_type2name(int unit, int blktype);

/*!
 * \brief Get block type from block name.
 *
 * Convert device-specified block name to device-specified block type.
 *
 * \param [in] unit Unit number.
 * \param [in] name Block name to convert.
 *
 * \retval Device-specified block type on success.
 * \retval -1 on failure.
 */
extern int
bcma_bcmbd_block_name2type(int unit, const char *name);

/*!
 * \brief Format block name string from block number.
 *
 * This function will output block name string in format of
 * \<block name\>\<block instance\> according to the
 * device-specified block number.
 *
 * \param [in] unit Unit number.
 * \param [in] blknum Block number to convert.
 * \param [out] dst Buffer to output the formatted string.
 * \param [in] dstsz Maximum size of the output buffer.
 *
 * \return \c dst on success, otherwise "*BADBLK*".
 */
extern char *
bcma_bcmbd_block_name(int unit, int blknum, char *dst, int dstsz);

/*!
 * \brief Convert symbol flag value to symbol flag name.
 *
 * \param [in] flag Symbol flag value(BCMDRD_SYMBOL_FLAG_XXX) to convert.
 *
 * \retval String of symbol flag name if \c flag is valid.
 * \retval NULL if flag is invalid.
 */
extern const char *
bcma_bcmbd_symflag_type2name(uint32_t flag);

/*!
 * \brief Convert symbol flag name to symbol flag value.
 *
 * The supported symbol flag names are "register", "port", "counter",
 * "memory", "r64", "big-endian", "soft-port", and "memmapped".
 *
 * \param [in] name Symbol flag name to convert.
 *
 * \retval BCMDRD_SYMBOL_FLAG* if input \c name is valid.
 * \retval 0 if no matching name is found.
 */
extern uint32_t
bcma_bcmbd_symflag_name2type(const char *name);

/*!
 * \brief Convert field flag value to field flag name.
 *
 * \param [in] flag field flag value(BCMDRD_SYMBOL_FIELD_FLAG_XXX) to convert.
 *
 * \retval String of field flag name if \c flag is valid.
 * \retval NULL if flag is invalid.
 */
extern const char *
bcma_bcmbd_fieldflag_type2name(uint32_t flag);

/*!
 * \brief Convert field flag name to field flag value.
 *
 * The supported field flag names are "counter", "key", and "mask".
 *
 * \param [in] name Fieldl flag name to convert.
 *
 * \retval BCMDRD_SYMBOL_FIELD_FLAG* if input \c name is valid.
 * \retval 0 if no matching name is found.
 */
extern uint32_t
bcma_bcmbd_fieldflag_name2type(const char *name);

/*!
 * \brief Parse and dispatch sub-command.
 *
 * Find \c arg's first argument in the vector table \c sv and call its handler.
 *
 * \param [in] args Argument for the sub-command.
 * \param [in] sv Vector-table of sub-commands.
 * \param [out] rc Return value of sub-command handler if not NULL.
 *
 * \retval 0 Sub-command is found in vector table.
 * \retval -1 No sub-commands are matched in vector table.
 */
extern int
bcma_bcmbd_parse_vect(bcma_cli_args_t *args, bcma_bcmbd_vect_t *sv, int *rc);

/*!
 * \brief Parse string to command id structure.
 *
 * This function is mainly to parse the command input argument string
 * into \ref bcma_bcmbd_id_t structure \c sid, which contains
 * address, unique-pipe access type, block, and port information if any.
 *
 * \param [in] str String to parse (will not be modified).
 * \param [out] sid Command id structure to store the parsed result.
 * \param [in] intaddr Indicate address name is in integer format if 1,
 *                     otherwise 0.
 *
 * \retval 0 on success.
 * \retval -1 on failure.
 */
extern int
bcma_bcmbd_parse_id(const char *str, bcma_bcmbd_id_t *sid, int intaddr);

/*!
 * \brief Parse CLI argument structure to BCMBD command tokens structure.
 *
 * This function parses each argument in \c args, and restore in \c ctoks in
 * format of BCMBD command tokens. If argument string is A=B[,C,D], token
 * A, B(, C and D) will be tokenized into different arguments in a single token
 * structure.
 *
 * \param [in] args CLI arguments to parse.
 * \param [out] ctoks Array of command token structures to store the result.
 * \param [in] max Maximum array size of command token structure.
 *
 * \retval NULL if all arguments are successfully parsed.
 * \retval Argument string which is failed to be parsed.
 */
extern const char *
bcma_bcmbd_parse_args(bcma_cli_args_t *args, bcma_cli_tokens_t *ctoks, int max);

/*!
 * \brief Encode field name/value pair into data for register/memory modification.
 *
 * Encode specified field value for modification of specified symbol to
 * \c and_masks and \c or_masks arrays.
 *
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] fnames Device-specific list of all field names.
 * \param [in] field Specified field name for modification.
 * \param [in] value Specified field value for modification.
 * \param [out] and_masks Data masks to be AND'ed.
 * \param [out] or_masks Data masks to be OR'ed.
 *
 * \retval 0 No errors.
 * \retval -1 On error.
 */
extern int
bcma_bcmbd_encode_field(const bcmdrd_symbol_t *symbol, const char **fnames,
                        const char *field, const char *value,
                        uint32_t *and_masks, uint32_t *or_masks);

/*!
 * \brief Encode token structures into data for register/memory modification.
 *
 * Parse the command token structures to get the register/memory modification
 * data. The \c and_masks and \c or_masks arrays are outputted for use of
 * arithmetic AND and OR operations with the original register/memory raw data
 * respectively to get the modified result of the register/memory.
 *
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] fnames Device-specific list of all field names.
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
bcma_bcmbd_encode_fields_from_tokens(const bcmdrd_symbol_t *symbol,
                                     const char **fnames,
                                     int ctoks_num,
                                     const bcma_cli_tokens_t *ctoks,
                                     int max,
                                     uint32_t *and_masks,
                                     uint32_t *or_masks);

/*!
 * \brief Display fields of a register/memory.
 *
 * This function will output fields and values of a register/memory.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] data Symbol data of one or more 32-bit words.
 * \param [in] oflags Output flags (\ref BCMA_BCMBD_CMD_IDF_xxx).
 * \param [in] ccb Customized call-back function for filtering fields.
 * \param [in] ccb_cookie Context for the customized call-back function.
 *
 * \retval Always 0.
 */
extern int
bcma_bcmbd_show_fields(int unit, const bcmdrd_symbol_t *symbol,
                       uint32_t *data, uint32_t oflags,
                       bcmdrd_symbol_custom_filter_cb_f ccb,
                       void *ccb_cookie);

/*!
 * \brief Dump fields of a register/memory to the print buffer.
 *
 * This function will dump fields and values of a register/memory to the
 * print buffer.
 *
 * \param [in] pb Print buffer.
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] data Symbol data of one or more 32-bit words.
 * \param [in] oflags Output flags (\ref BCMA_BCMBD_CMD_IDF_xxx).
 * \param [in] ccb Customized call-back function for filtering fields.
 * \param [in] ccb_cookie Context for the customized call-back function.
 *
 * \retval Always 0.
 */
extern int
bcma_bcmbd_dump_fields(shr_pb_t *pb, int unit, const bcmdrd_symbol_t *symbol,
                       uint32_t *data, uint32_t oflags,
                       bcmdrd_symbol_custom_filter_cb_f ccb,
                       void *ccb_cookie);

/*!
 * \brief Create bit range string.
 *
 * This function will create bit range string for a given port bitmap.
 *
 * \param [out] buf Output buffer for bit range string.
 * \param [in] size Size of output buffer.
 * \param [in] pbmp Port bitmap for output buffer.
 * \param [in] mask Port bitmap that determines how many words to print.
 *
 * \retval Always 0.
 */
extern int
bcma_bcmbd_port_bitmap(char *buf, int size, const bcmdrd_pbmp_t *pbmp,
                       const bcmdrd_pbmp_t *mask);

/*!
 * \brief Get device valid ports bitmap of a given block number and symbol.
 *
 * If the enum_val is -1, the returned bitmap will be the valid ports bitmap
 * of the specified block number.
 *
 * \param [in] unit Unit number.
 * \param [in] enum_val Symbol enum value.
 * \param [in] blknum Block number.
 * \param [out] pbmp Valid Ports bitmap.
 *
 * \retval 0 No errors.
 * \retval -1 On error.
 */
int
bcma_bcmbd_symbol_block_valid_ports_get(int unit, int enum_val, int blknum,
                                        bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Convert SER type value to string name.
 *
 * \param [in] ser_type SER type value(BCMDRD_SER_TYPE_xxx).
 *
 * \retval String of SER type name.
 */
extern const char *
bcma_bcmbd_ser_type2name(bcmdrd_ser_type_t ser_type);

/*!
 * \brief Convert SER response value to string name.
 *
 * \param [in] ser_resp_type SER response type value(BCMDRD_SER_RESP_xxx).
 *
 * \retval String of SER response type name.
 */
extern const char *
bcma_bcmbd_ser_resp_type2name(bcmdrd_ser_resp_t ser_resp_type);

/*!
 * \brief Convert SER enable type value to string name.
 *
 * \param [in] ser_en_type SER enable type value(BCMDRD_SER_EN_TYPE_xxx).
 *
 * \retval String of SER enable type name.
 */
extern const char *
bcma_bcmbd_ser_en_type2name(bcmdrd_ser_en_type_t ser_en_type);

/*!
 * \brief Compare symbol reset value from DRD symbol info.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] data_wsz Word number of the data.
 * \param [in] data Data to compare with the reset value.
 *
 * \retval 0 The input data is equal to the reset value.
 * \retval -1 The input data is different from the reset value.
 */
int
bcma_bcmbd_resetval_cmp(int unit, const bcmdrd_symbol_t *symbol,
                        uint32_t data_wsz, uint32_t *data);

#endif /* BCMA_BCMBD_H */
