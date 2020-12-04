/** \file utilex_str.h
 * Purpose:    Misc. routines for string/buffer handling
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_STR_H_INCLUDED
#define UTILEX_STR_H_INCLUDED

#include <sal/core/alloc.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/error.h>
#include <shared/shrextend/shrextend_error.h>

/*
 * Misc. defines
 */
#define  RHFILE_MAX_SIZE      384
#define  RHNAME_MAX_SIZE      64
#define  RHSTRING_MAX_SIZE    128
#define  RHKEYWORD_MAX_SIZE   12

/**
 * \brief Check the pointer and the content. If it is not NULL and the not empty return true
 * \par DIRECT INPUT
 *   \param [in] name_macro pointer to the string to be checked
 */
#define ISEMPTY(name_macro)     ((name_macro == NULL) || (name_macro[0] == 0))

/**
 * \brief Make string empty one
 * \par DIRECT INPUT
 *   \param [in] name_macro pointer to the string to be empty
 */
#define SET_EMPTY(name_macro)   (name_macro[0] = 0)

/**
 * \brief Return uint32 value hidden in the string
 * \par DIRECT INPUT
 *   \param [in] value_string pointer to the uint32 value
 */
#define VALUE(value_string)     (*((uint32 *)value_string))

/**
 * \brief
 * Structure used when scanning XML attributes in free style to create name/value pairs to be understood/analyzed by calling rotuine
 */
typedef struct
{
    /**
     * Attribute name
     */
    char name[RHNAME_MAX_SIZE];
    /*
     * Attribute value as string, calling procedure should know how to handle it
     */
    char value[RHNAME_MAX_SIZE];
} attribute_param_t;

void utilex_escape_quote(
    char *source,
    char *target,
    int target_size);

/**
 * \brief Append string to existing one, if it is not empty insert space and copy after space, otherwise just copy from the beginning
 * \par DIRECT INPUT
 *   \param [in] base pointer to the base string, may be empty, but should not be NULL
 *   \param [in] add pointer to the string to be appended, may be empty, but should not be NULL
 */
void utilex_str_append(
    char *base,
    char *add);

/**
 * \brief Fill certain number of characters in string with character
 * \par DIRECT INPUT
 *   \param [in,out] string pointer to the start of string to be filled
 *   \param [in] num number of bytes to be filled
 *   \param [in] ch character that will be put into the string
 */
void utilex_str_fill(
    char *string,
    int num,
    char ch);

/**
 * \brief Split input string into tokens - multiple strings separated by certain character
 * \par DIRECT INPUT
 * \param [in] string input string for splitting
 * \param [in] delim  delimiter character for splitting
 * \param [in] maxtokens maximum number of expected token
 * \param [in,out] realtokens  pointer to the variable that discovered count of tokens will be filled by
 * \par DIRECT OUTPUT
 *   \retval pointer to string array containing list of tokens
 * \par INDIRECT OUTPUT
 *   *realtokens - number of tokens found in the string
 * \remark
 *   String array should be freed once not needed through utilex_str_split_free
 */
char **utilex_str_split(
    char *string,
    char *delim,
    uint32 maxtokens,
    uint32 *realtokens);

/**
 * \brief Free array of tokens allocated by utilex_str_split
 * \par DIRECT INPUT
 *   \param [in] tokens pointer to token array
 *   \param [in] token_num delim  delimiter character for splitting
 */
void utilex_str_split_free(
    char **tokens,
    uint32 token_num);

/**
 * \brief Swap bytes inside uint32
 * \par DIRECT INPUT
 *   \param [in] buffer pointer to buffer
 *   \param [in] uint32_num number of words to be swapped
 * \par INDIRECT OUTPUT
 *   *buffer buffer content will present bytes in reversed order
 */
void utilex_str_swap_long(
    uint8 *buffer,
    int uint32_num);

/**
 * \brief Replace all 'from' characters by 'to' in source itself
 *   It is assumed that neither 'from' nor 'to' is NULL
 * \par DIRECT INPUT
 *   \param [in] str string to search for occurences of charactre from
 *   \param [in] from character to be replaced by to
 *   \param [in] to character to replace from in str
 */
void utilex_str_replace(
    char *str,
    char from,
    char to);

/**
 * \brief Replace special charaters ( ',' ' ' '#' '&' '$' '(' ')' )  by escape one ('replacement')
 *   (usually underscore) on the source string itself.
 *   It is assumed that 'replacement' is not NULL
 * \par DIRECT INPUT
 *   \param [in] str string to search for occurrences of character from
 *   \param [in] replacement character to replace all special characters in source
 */
void utilex_str_escape(
    char *str,
    char replacement);

/**
 * \brief Replace special charaters ( utilex_special_full[] ) by escape one ('replacement')
 *   (usually underscore) on the source string itself.
 *   It is assumed that 'replacement' is not NULL
 *   Routine oriented to be used by XML file builder to avoid problematic object names
 * \param [in] str string to search for occurrences of character from
 * \param [in] replacement character to replace all special characters in source
 */
void utilex_str_escape_xml(
    char *str,
    char replacement);

/**
 * \brief Verify string versus special characters list
 * \param [in] str string to search for occurrences of character from the list
 */
shr_error_e utilex_str_verify(
    char *str);

/**
 * \brief Replace white space by underscore in source and put it in destination
 * \par DIRECT INPUT
 *   \param [in] dest buffer where result will be placed
 *   \param [in] source pointer to string with white spaces
 * \par INDIRECT OUTPUT
 *   *dest  buffer where result of replace operation will be placed
 */
void utilex_str_replace_whitespace(
    char *dest,
    char *source);

/**
 * \brief Remove all white spaces and tabs from the end of the string and
 *        return pointer to first non white space character
 * \param [in,out] str_p - pointer to string for process, pointer to first non white space character will be assigned
 */
void utilex_str_clean(
    char **str_p);

/**
 * \brief Return TRUE if the character is digit, otherwise FALSE
 * \param [in] mychar - character to check for digit
 */
int utilex_char_is_digit(
    unsigned char mychar);

/**
 * \brief Return upper case character
 * \param [in] mychar - lower-case character will be return as upper, any other will be returned as is
 */
unsigned char utilex_char_to_upper(
    unsigned char mychar);

/**
 * \brief Return lower case character
 * \param [in] mychar - upper-case character will be return as lower, any other will be returned as is
 */
unsigned char utilex_char_to_lower(
    unsigned char mychar);

/**
 * \brief Replace end of line by white space
 * \par DIRECT INPUT
 *   \param [in,out] str string in which EOL will be replaced
 *   \param [in] replacement The character to replace EOL with
 * \par INDIRECT OUTPUT
 *   *str  buffer where result of replace operation will be placed
 */
void utilex_str_replace_eol(
    char *str,
    char replacement);

/**
 * \brief Convert uppercase letters to lowercase letters
 * \param [in,out] str string in which the uppercase letters will be converted into lowercase letters
 */
void utilex_str_to_lower(
    char *str);

/**
 * \brief Convert lowercase letters to uppercase letters
 * \param [in,out] str string in which the lowercase letters will be converted into uppercase letters
 */
void utilex_str_to_upper(
    char *str);

/**
 * \brief Replace multiple white spaces and/or tabs to 1 white space and remove them from the end
 * \par DIRECT INPUT
 *   \param [in,out] str string in which replacement will be take place
 * \par INDIRECT OUTPUT
 *   *str  buffer where result of replace operation will be placed
 */
void utilex_str_shrink(
    char *str);

/**
 * \brief Remove all whitespaces from a given string
 * \par DIRECT INPUT
 *   \param [in,out] str string in which replacement will be take place
 * \par INDIRECT OUTPUT
 *   *str  buffer where result of replace operation will be placed
 */
void utilex_str_white_spaces_remove(
    char *str);

int utilex_str_get_shift(
    char *string,
    int token_size);

/**
 * \brief Checks if  string is decimal number
 * \param [in] str pointer to the string to be checked
 * \retval TRUE  - if string is decimal number
 * \retval FALSE - otherwise
 */
int utilex_str_is_decimal(
    char *str);

/**
 * \brief Checks if  string is decimal or hex number
 * \param [in] str pointer to the string to be checked
 * \retval TRUE  - if string is number
 * \retval FALSE - otherwise
 */
int utilex_str_is_number(
    char *str);

/**
 * \brief Convert string to unsigned int
 * \par DIRECT INPUT
 *   \param [in] str pointer to the string to be converted
 *   \param [in] value_p pointer to the value
 * \par INDIRECT OUTPUT
 *   *value_p value obtained from the string
 */
uint32 utilex_str_stoul(
    char *str,
    uint32 *value_p);

/**
 * \brief Convert a hexadecimal string (without the 0x prefix) to unsigned int
 * \par DIRECT INPUT
 *   \param [in] str pointer to the string to be converted
 *   \param [in] value_p pointer to the value
 * \par INDIRECT OUTPUT
 *   *value_p value obtained from the string
 */
uint32 utilex_str_xstoul(
    char *str,
    uint32 *value_p);

/**
 * \brief Convert long string to unsigned int array, string more than 1 uint should be prepended by 0x
 * \par DIRECT INPUT
 *   \param [in] str pointer to the string to be converted
 *   \param [in] value_p pointer to the value
 *   \param [in] nval - size of memory pointed by value_p in uint32 words
 * \par INDIRECT OUTPUT
 *   *value_p value obtained from the string
 */
shr_error_e utilex_str_long_stoul(
    char *str,
    uint32 *value_p,
    int nval);
/**
 * \brief
 *   Given an input array of N pairs of ascii characters, each representing
 *   a hex digit, convert into array of N binary bytes, each made out of a
 *   pair of hex digits. Note that input array ends with NULL so that
 *   the number of its elements may be smaller than 'max_sizeof_array_of_hex_digits'.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] array_of_hex_digits_p -
 *   uint8 *. Array of 'sizeof_array_of_hex_digits - 1' pairs of hex
 *   digits in ASCII representation. Last element must be NULL.
 * \param [in] max_sizeof_array_of_hex_digits -
 *   uint32. Maximal number of elements on 'array_of_hex_digits', including
 *   ending NULL. Note that the number of elements may be smaller but not larger
 *   than 'max_sizeof_array_of_hex_digits' and that it should be odd.
 * \param [out] array_of_binary_values_p -
 *   uint8 *. Array of bytes as converted from 'array_of_hex_digits'.
 *   Caller MUST make sure that the number of elements, on this array, is,
 *   exactly, the number of pairs on max_sizeof_array_of_hex_digits
 *   (i.e., (max_sizeof_array_of_hex_digits - 1)/2).
 * \return
 *   Error code (as per 'shr_error_e'.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_hex_char_array_to_num(
    int unit,
    uint8 *array_of_hex_digits_p,
    uint32 max_sizeof_array_of_hex_digits,
    uint8 *array_of_binary_values_p);

void *utilex_alloc(
    unsigned int size);

void utilex_free(
    void *mem);

int utilex_pow(
    int n,
    int k);

uint8 utilex_char_to_num(
    uint8 digit);

void utilex_sub_string_replace(
    char *str,
    const char *in_sub_str,
    const char *out_sub_str);

#endif /* UTILEX_STR_H_INCLUDED */
