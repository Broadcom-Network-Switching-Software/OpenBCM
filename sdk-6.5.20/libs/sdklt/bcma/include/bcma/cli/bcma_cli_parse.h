/*! \file bcma_cli_parse.h
 *
 * Definitions for CLI parsing functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_PARSE_H
#define BCMA_CLI_PARSE_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_token.h>


/*!
 * \brief Parse CLI command statement into command tokens.
 *
 * The optional s_ret parameter can be used to parse separate command
 * statements within a longer command line. Each statement is
 * typically terminated by a semicolon (or a NUL character for the
 * last statement of the input string.) If the s_ret pointer is
 * provided, then it will be updated to point to the next statement or
 * set to NULL if no more statements are available.
 *
 * \param[in] cli CLI object
 * \param[in] str Command line from input device
 * \param[out] s_ret Pointer to the next statement (may be NULL)
 * \param[out] args Structure where to store parsed command tokens
 *
 * \retval 0 No errors
 * \retval -1 Bad CLI object
 */
extern int
bcma_cli_parse_args(bcma_cli_t *cli, const char *str,
                    const char **s_ret, bcma_cli_args_t *args);

/*!
 * \brief Parse CLI argument structure to command tokens structure.
 *
 * This function parses each argument in \c args, and restore in \c ctoks in
 * format of command tokens. If argument string is A=B[,C,D], token
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
bcma_cli_parse_tokens(bcma_cli_args_t *args, bcma_cli_tokens_t *ctoks, int max);

/*!
 * \brief Parse a string into an integer.
 *
 * \param[in] str String to parse
 * \param[out] val Parsed integer value
 *
 * \retval 0 No errors
 * \retval -1 String is not a valid integer
 */
extern int
bcma_cli_parse_int(const char *str, int *val);

/*!
 * \brief Parse a string into an unsigned integer.
 *
 * \param[in] str String to parse
 * \param[out] val Parsed unsigned integer value
 *
 * \retval 0 No errors
 * \retval -1 String is not a valid integer
 */
extern int
bcma_cli_parse_uint32(const char *str, uint32_t *val);

/*!
 * \brief Parse a string into an unsigned long integer.
 *
 * \param[in] str String to parse
 * \param[out] val Parsed unsigned long integer value
 *
 * \retval 0 No errors
 * \retval -1 String is not a valid integer
 */
extern int
bcma_cli_parse_uint64(const char *str, uint64_t *val);

/*!
 * \brief Check if string contains a valid integer.
 *
 * Input string may be decimal or hexadecimal (starting with 0x).
 *
 * \param [in] str String to check
 *
 * \retval TRUE String is a valid integer
 * \retval FALSE String is not a valid integer
 */
extern int
bcma_cli_parse_is_int(const char *str);

/*!
 * \brief Parse a string into 8-bit data array.
 *
 * There is no length limitation of the input string \c str as long as
 * the size of the data array is large enough. The string is expected
 * to start with 0x or 0X in hex format, otherwise it will be parsed as a
 * single 8-bit scalar.
 *
 * The parsing might be destructive as any underscores in the input
 * string \c str will be removed and NUL characters may be inserted.
 *
 * \param[in] str String to parse.
 * \param[in] data_max Maximum data array size.
 * \param[out] data 8-bit data array to store the parsed data.
 *
 * \return Required data array size or
 *         -1 on error for invalid string or insufficient array size.
 */
extern int
bcma_cli_parse_array_uint8(char *str, int data_max, uint8_t *data);

/*!
 * \brief Parse a string into 32-bit data array.
 *
 * There is no length limitation of the input string \c str as long as
 * the size of the data array is large enough. The string is expected
 * to start with 0x or 0X in hex format, otherwise it will be parsed as a
 * single 32-bit scalar.
 *
 * The parsing might be destructive as any underscores in the input
 * string \c str will be removed and NUL characters may be inserted.
 *
 * \param[in] str String to parse.
 * \param[in] data_max Maximum data array size.
 * \param[out] data 32-bit data array to store the parsed data.
 *
 * \return Required data array size or
 *         -1 on error for invalid string or insufficient array size.
 */
extern int
bcma_cli_parse_array_uint32(char *str, int data_max, uint32_t *data);

/*!
 * \brief Parse a string into 64-bit data array.
 *
 * There is no length limitation of the input string \c str as long as
 * the size of the data array is large enough. The string is expected
 * to start with 0x or 0X in hex format, otherwise it will be parsed as a
 * single 64-bit scalar.
 *
 * The parsing might be destructive as any underscores in the input
 * string \c str will be removed and NUL characters may be inserted.
 *
 * \param[in] str String to parse.
 * \param[in] data_max Maximum data array size.
 * \param[out] data 64-bit data array to store the parsed data.
 *
 * \return Required data array size or
 *         -1 on error for invalid string or insufficient array size.
 */
extern int
bcma_cli_parse_array_uint64(char *str, int data_max, uint64_t *data);

/*!
 * \brief Compare string to argument/option name.
 *
 * This function checks a string against a reference according to the
 * following rules:
 *
 * Comparison in case-insensitive, e.g. input string "myarg" matches
 * reference string "MyArg".
 *
 * Sub-string matches, e.g. input string "mya" matches reference
 * string "MyArg".
 *
 * Short-form matches, e.g. input string "ma" matches reference
 * "MyArg". Short-form is defined as the uppercase letters in
 * reference string.
 *
 * \param [in] ref Reference string to check against
 * \param [in] str Input string to check
 * \param [in] term Character which terminates input string
 *
 * \retval TRUE Input string matches reference
 * \retval FALSE Input string does not match reference
 */
extern int
bcma_cli_parse_cmp(const char *ref, const char *str, const char term);

/*!
 * \brief Compare string to the shorthand of the argument/option name.
 *
 * This function checks a string against a reference according to
 * the "short-form matches" rule, \ref bcma_cli_parse_cmp.
 *
 * \param [in] ref Reference string to check against
 * \param [in] str Input string to check
 * \param [in] term Character which terminates input string
 *
 * \retval TRUE Input string matches reference
 * \retval FALSE Input string does not match reference
 */
extern int
bcma_cli_parse_abbr_cmp(const char *ref, const char *str, const char term);

/*!
 * \brief Display error message for command line argument.
 *
 * \param [in] desc Name of command line argument.
 * \param [in] arg Specified value of argument (or NULL if none).
 *
 * \retval Always -1.
 */
extern int
bcma_cli_parse_error(const char *desc, const char *arg);

/*!
 * \brief CLI parse table data type.
 *
 * The parse table data types are small drivers for handling CLI
 * command options of a particular type such as int, string, boolean,
 * enum, etc.
 *
 * A number of default parse data drivers are available by default,
 * but an application can extend this support dynamically.
 */
typedef struct bcma_cli_parse_data_s {

    /*! Argument type */
    const char *arg_type;

    /*! Function to parse argument string into a data object */
    int (*parse_arg)(const char *argstr, void *data, void **option);

    /*! Function to print current argument value */
    char *(*format_arg)(const void *data, const void *option,
                        char *buf, int bufsz);

    /*! Next data type in list (internal) */
    struct bcma_cli_parse_data_s *next;

} bcma_cli_parse_data_t;

/*!
 * \brief CLI parse data type for "enum".
 *
 * The structure is used as the "option" information by parse data
 * type "enum".
 */
typedef shr_enum_map_t bcma_cli_parse_enum_t;

/*!
 * \brief Initialize list of parse data drivers.
 *
 * The parse data drivers are used by the parse table APIs in order to
 * parse (and format) different argument types (int, string, boolean,
 * enum, etc.)
 *
 * \retval 0 No errors
 */
extern int
bcma_cli_parse_data_init(void);

/*!
 * \brief Add argument data type to list of supported argument types.
 *
 * \param [in] pd New argument type for use with parse tables
 *
 * \retval 0 No errors
 * \retval -1 Parse data type already exists
 */
extern int
bcma_cli_parse_data_add(bcma_cli_parse_data_t *pd);

/*!
 * \brief Add default set of parse data drivers.
 *
 * If any parse drivers have been added already, then this function
 * will just return success without doing anything.
 *
 * The default parse data drivers are added in function
 * \ref bcma_cli_parse_table_init, the default supported data types are
 * "bool", "int", "hex", "str", and "enum".
 *
 * \retval 0 No errors
 */
extern int
bcma_cli_parse_data_add_default(void);

/*!
 * \brief Add parse data drivers for net object.
 *
 * The supports data types for net objects are :
 *
 * - Data type "mac" for mac address parsing and formatting
 *
 *   The MAC parser would support two kinds of MAC address formats :
 *   1. A string of the form H:H:H:H:H:H, where H is one or two hex digits.
 *   2. A string of the from 0xN, where N consist of up to 12 hex digits.
 *
 * - Data type "ipv4" for IPv4 addrees parsing and formatting
 *
 *   The IPv4 parser would support two kinds of IPv4 formats :
 *   1. A string of the form D.D.D.D, where D is decimal number between 0 ~ 255.
 *   2. A string of the numerical form (in hex, octal or decimal).
 *
 * - Data type "ipv6" for IPv6 address parsing and formatting
 *
 *   The IPv6 parser would support the IPv6 formats :
 *   1. H:H:H:H:H:H:H:H, where H is up to 4 hex digits.
 *   2. H:H:H:H:H:H.D.D.D.D, where H is up to 4 hex digits and
 *      D is a decimal number between 0~ 255.
 *   Note the the above hex digits H could be compressed in :: for consecutive 0's
 *   with only one occurrence.
 *
 * \retval 0 No errors
 */
extern int
bcma_cli_parse_data_add_net(void);

/*!
 * \brief Get parse data driver for a data type.
 *
 * If any parse drivers have been added already, then this function
 * will just return success without doing anything.
 *
 * \return Pointer to parse data driber or NULL if none found
 */
extern bcma_cli_parse_data_t *
bcma_cli_parse_data_lookup(const char *arg_type);

/*!
 * \brief Wrapper for parse_arg driver funtion.
 *
 * Error checking wrapper around parse_arg function in \ref
 * bcma_cli_parse_data_t.
 *
 * \param [in] pd Parse data driver
 * \param [in] argstr Argument string (from command line)
 * \param [out] data Object to hold parsed data
 * \param [in,out] option Optional parsing information
 *
 * \retval 0 No errors
 * \retval -1 Malformed argument
 */
extern int
bcma_cli_parse_data_parse_arg(bcma_cli_parse_data_t *pd,
                              const char *argstr, void *data, void **option);

/*!
 * \brief Wrapper for format_arg driver funtion.
 *
 * Error checking wrapper around format_arg function in \ref
 * bcma_cli_parse_data_t.
 *
 * \param [in] pd Parse data driver
 * \param [in] data Data object to format
 * \param [in] option Optional object to format
 * \param [in] buf Output buffer
 * \param [in] bufsz Size of output buffer
 *
 * \return Data object string (or error string)
 */
extern char *
bcma_cli_parse_data_format_arg(bcma_cli_parse_data_t *pd,
                               const void *data, const void *option,
                               char *buf, int bufsz);


/*!
 * \brief CLI parse table entry.
 *
 * A parse table entry contains the type and value of a CLI command
 * argument.
 */
typedef struct bcma_cli_parse_entry_s {

    /*! Name of CLI command argument */
    char *name;

    /*! Type of CLI parse data */
    bcma_cli_parse_data_t *pd;

    /*! Actual value of parameter */
    void *value;

    /*! Optional information (depends on argument type) */
    void *option;

} bcma_cli_parse_entry_t;

/*!
 * \brief CLI parse table.
 *
 * The parse table is populated with the valid arguments for a
 * particular CLI command, and then each argument from the actual
 * command line is checked against the table.
 */
typedef struct bcma_cli_parse_table_s {

    /*! CLI object */
    bcma_cli_t *cli;

    /*! Number of valid entries in parse table */
    int num_valid;

    /*! Number of allocated entries in parse table */
    int num_alloc;

    /*! Actual entries in parse table */
    bcma_cli_parse_entry_t *entries;

} bcma_cli_parse_table_t;

/*!
 * \brief Initialize parse table.
 *
 * Must be called before entries can be added.
 *
 * \param [in] cli CLI object
 * \param [in] pt Parse table
 */
extern void
bcma_cli_parse_table_init(bcma_cli_t *cli, bcma_cli_parse_table_t *pt);

/*!
 * \brief Add argument to parse table.
 *
 * For most argument types, the \c value parameter should point to a data
 * object containing the default value for this CLI argument. One
 * exception is arguments of type "str", where the data object should
 * be a string pointer and not the string itself.
 *
 * For parse driver type "bool", "int", and "hex", the \c value parameter
 * should point to object type of int. Only the parse result of "bool"
 * will be TRUE or FALSE.
 *
 * Note that \c arg_name is copied to parse table and maintained internally
 * in parse table. User does not need to keep the \c arg_name after calling
 * this function.
 *
 * \param [in] pt Parse table
 * \param [in] arg_name Argument name
 * \param [in] arg_type Argument type
 * \param [in] value Current/default value of argument
 * \param [in] option Optional argument information
 *
 * \retval 0 No errors
 * \retval -1 Insufficient resources to add entry or unsupported parse data type
 */
extern int
bcma_cli_parse_table_add(bcma_cli_parse_table_t *pt,
                         const char *arg_name, const char *arg_type,
                         void *value, void *option);

/*!
 * \brief Parse argument list against parse table entries.
 *
 * Input string may be decimal or hexadecimal (starting with 0x).
 *
 * \param [in] pt Parse table
 * \param [in] args Argument list
 *
 * \retval 0 No errors
 * \retval -1 Unrecognized or malformed argument encountered.
 */
extern int
bcma_cli_parse_table_do_args(bcma_cli_parse_table_t *pt,
                             bcma_cli_args_t *args);

/*!
 * \brief Free parse table resources.
 *
 * Free any resources allocated during parse table init or args
 * processing.
 *
 * \param [in] pt Parse table
 */
extern void
bcma_cli_parse_table_done(bcma_cli_parse_table_t *pt);

/*!
 * \brief Display current values of arguments in parse table.
 *
 * Display list of arguments and their current value. If no prefix
 * string is specified (prefix=NULL), then a TAB charater will be used
 * as the prefix string.
 *
 * \param [in] pt Parse table
 * \param [in] prefix Optional string to be displayed in front of each argument
 */
extern void
bcma_cli_parse_table_show(bcma_cli_parse_table_t *pt, const char *prefix);

/*!
 * \brief Format data of MAC address to string with MAC address format.
 *
 * \param [in] mac_addr MAC address data to be formatted.
 * \param [out] buf Buffer to store the transformed string.
 * \param [in] bufsize Maximum size of \c buf.
 *
 * \return Pointer to \c buf if no error, otherwise a pointer to the
 *         string "<overrun>".
 */
extern char *
bcma_cli_format_mac_addr(shr_mac_t mac_addr, char *buf, int bufsize);

/*!
 * \brief Parse string with MAC address format to MAC address data.
 *
 * The supported MAC address format strings are:
 * 1. A string of the form H:H:H:H:H:H, where H is one or two hex digits.
 * 2. A string of the from 0xN, where N consist of up to 12 hex digits.
 *
 * \param [in] str String to be parsed to MAC address data
 * \param [out] mac_addr MAC address data that stores the parsed result
 *
 * \retval 0 No errors
 * \retval -1 Failed to parse the string to MAC address data.
 */
extern int
bcma_cli_parse_mac_addr(char *str, shr_mac_t mac_addr);

/*!
 * \brief Format IPv4 data to string with IPv4 format.
 *
 * \param [in] ipv4_addr IPv4 data to be formatted.
 * \param [out] buf Buffer to store the transformed string.
 * \param [in] bufsize Maximum size of \c buf.
 *
 * \return Pointer to \c buf if no error, otherwise a pointer to the
 *         string "<overrun>".
 */
extern char *
bcma_cli_format_ipv4_addr(shr_ip_t ipv4_addr, char *buf, int bufsize);

/*!
 * \brief Parse string of IPv4 format to IPv4 data.
 *
 * The IPv4 parser would support two kinds of IPv4 formats:
 * 1. A string of the form D.D.D.D, where D is decimal number between 0 ~ 255.
 * 2. A string of the numerical form (in hex, octal or decimal).
 *
 * \param [in] str String to be parsed to IPv4 data
 * \param [in,out] ipv4_addr Pointer of IPv4 data that stores the parsed result
 *
 * \retval 0 No errors
 * \retval -1 Fail to parse the string to IPv4 data.
 */
extern int
bcma_cli_parse_ipv4_addr(char *str, shr_ip_t *ipv4_addr);

/*!
 * \brief Format IPv6 data to string with IPv6 format.
 *
 * \param [in] ipv6_addr IPv6 data to be formatted.
 * \param [out] buf Buffer to store the transformed string.
 * \param [in] bufsize Maximum size of \c buf.
 *
 * \return Pointer to \c buf if no error, otherwise a pointer to the
 *         string "<overrun>".
 */
extern char *
bcma_cli_format_ipv6_addr(shr_ip6_t ipv6_addr, char *buf, int bufsize);

/*!
 * \brief Parse string of IPv6 format to IPv6 data.
 *
 * The supported IPv6 format strings are:
 * 1. H:H:H:H:H:H:H:H, where H is up to 4 hex digits.
 * 2. H:H:H:H:H:H.D.D.D.D, where H is up to 4 hex digits and D is
 *    a decimal number between 0~ 255.
 * Note the the above hex digits H could be compressed in :: for
 * consecutive 0's with only one occurrence.
 *
 * \param [in] str String to be parsed to IPv6 data
 * \param [out] ipv6_addr IPv6 data that stores the parsed result
 *
 * \retval 0 No errors
 * \retval -1 Fail to parse the string to IPv6 data.
 */
extern int
bcma_cli_parse_ipv6_addr(char *str, shr_ip6_t ipv6_addr);

/*!
 * \brief Parse string of decimal integer range format to integers.
 *
 * The supported decimal interger range formats are:
 * 1. Two decimal values joined by a hyphen (-): 10-100
 * 2. One single decimal value: 10
 *
 * The returned \c start and \c end values represents the two decimal values
 * separated by the hyphen (-). If the string format is one single value,
 * the returned value of \c start and \c end will be identical.
 *
 * \param [in] str String to be parsed to integers.
 * \param [out] start Integer value the range starts from.
 * \param [out] end Integer value the range ends up.
 *
 * \retval 0 No errors
 * \retval -1 Fail to parse the string to a range of integers.
 */
extern int
bcma_cli_parse_int_range(const char *str, int *start, int *end);

/*!
 * \brief Parse a bit number string into a bit array.
 *
 * The bit number string may contain commas to separate bit numbers
 * and hyphens to indicate ranges of bits.
 *
 * Examples: "2" "2,5" "2,5,7-13,43"
 *
 * \param [in] str String to be parsed.
 * \param [in] max_bits Maximum number of bits in the bit array.
 * \param [out] bit_array Bit array representation of input string.
 *
 * \retval 0 No errors
 * \retval -1 Fail to parse the string to a bit array.
 */
extern int
bcma_cli_parse_bit_list(const char *str, int max_bits, SHR_BITDCL *bit_array);


#endif /* BCMA_CLI_PARSE_H */
