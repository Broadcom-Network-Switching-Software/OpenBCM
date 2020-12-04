/** \file dbx_file.h
 * Purpose:    Routines used when working with data IO
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DBX_FILE_H_INCLUDED
#define DBX_FILE_H_INCLUDED

#include <shared/shrextend/shrextend_error.h>
#include <shared/dbx/dbx_xml.h>

typedef enum
{
  /**
   *  No .ext for in filename
   */
    DBX_FILE_NONE,
  /**
   * There is an extension but not the one known
   */
    DBX_FILE_UKNOWN,
  /**
   * Comma separated format
   */
    DBX_FILE_CSV,
  /**
   * XML format
   */
    DBX_FILE_XML,
  /**
   * Verilog format
   */
    DBX_FILE_VERILOG,
  /**
   * Must come last
   */
    DBX_FILE_MAX
} DBX_FILE_TYPE_E;

/**
 * \brief Return file type according to extension.
 * String after last dot is compared to predefined set of extensions
 * \par DIRECT INPUT
 *   \param [in] filename - string representing filename to be checked
 * \par DIRECT OUTPUT:
 *   \retval file type as per DBX_FILE_TYPE_E
 */
DBX_FILE_TYPE_E dbx_file_get_type(
    char *filename);

/**
 * \brief Check directory existence. Return TRUE if exists, FALSE if not
 * \param [in] unit - BCM unit id
 * \param [in] filename name of the file, includes relative path inside DB
 * \param [in] flags - flags pointing to the character of filepath
 * \param [out] filepath pointer to string where resulting file path should be copied
 * \return
 *   \retval _SHR_E_NONE if path was successfully created
 *   \retval _SHR_E_PARAM if there was some problem with parameters
 * \remarks
 * .If there is no CONF_OPEN_CREATE or CONF_OPEN_OWERWRITE flags - file existence will be verified
 *  If there is CONF_OPEN_PER_DEVICE - device db will be added between global DB path and filename
 *  If there is CONF_OPEN_ALTER_LOC - alternative internal folder relatively to filename will be checked for
 *                                      file presence
 */
shr_error_e dbx_file_get_file_path(
    int unit,
    char *filename,
    int flags,
    char *filepath);

/**
 * \brief Create list of all xml file's names in a directory and its sub directories
 * \par DIRECT INPUT
 *   \param [in] chip_db - chip name
 *   \param [in] dirpath directory path - relative or absolute
 *   \param [in,out] files_list - ptr to strings array
 * \par INDIRECT OUTPUT:
 *   See files_list above - will be updated with the file's names
 * \par DIRECT OUTPUT:
 *   \retval number of files in list
 */
int dbx_file_get_xml_files_from_dir(
    char *chip_db,
    char *dirpath,
    char **files_list);

/**
 * \brief Check file existence. Return TRUE if exists, FALSE if not
 * Oriented for xml files only
 * \par DIRECT INPUT
 *   \param [in] filename name of the files which existence should be checked
 * \par DIRECT OUTPUT:
 *   \retval FALSE if file do not exists
 *   \retval TRUE if file exists
 * \remark
 *   If filename has no xml extension, it is added before existence check
 */
int dbx_file_exists(
    char *filename);

/**
 * \brief Checks file writability and removes it preparing places for new
 *
 * \param [in] filename name of the files which existence should be checked
 * \retval _SHR_E_NONE if file was writable and was deleted successfully
 * \retval Error per regular list
 */
shr_error_e dbx_file_remove(
    char *filename);

/**
 * \brief Creates folder if it not exists
 *
 * \param [in] dirpath - folder to be created
 * \retval 0 - if folder not exists
 * \retval 1 - if folder exists
 */

int dbx_file_dir_exists(
    char *dirpath);

/**
 * \brief Creates folder if it not exists
 *
 * \param [in] dir - folder to be created
 * \retval 0  - if folder existed or was successfully created
 * \retval -1 - if error on folder creation
 */
int dbx_file_dir_create(
    char *dir);

/**
 * \brief Get full path for SDK file
 * \par DIRECT INPUT
 *   \param [out] sdk_dir pointer to string where resulting SDK
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE if path was successfully created
 *   \retval _SHR_E_NOT_FOUND if sdk directory were not found.
 */
shr_error_e dbx_file_find_sdk_path(
    char *sdk_dir);

/**
 * \brief Get full path for SDK file
 * \par DIRECT INPUT
 *   \param [in] unit unit id
 *   \param [in] filename only file name
 *   \param [in] location relative location inside $SDK, this directory must exist
 *   \param [in,out] filepath pointer to string where resulting file path should be copied
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE if path was successfully created
 *   \retval _SHR_E_PARAM if there was some problem with parameters
 *   \retval _SHR_E_NOT_FOUND if the file or directory were not found
 *   \retval _SHR_E_INTERNAL if file or directory names exceed the allowed limit
 * \remark
 *   File existence is not important here, call may be made for file creation, reading or just checking path validity
 */
shr_error_e dbx_file_get_sdk_path(
    int unit,
    char *filename,
    char *location,
    char *filepath);

/**
 * \brief Get handle to the top node of XML file
 * \par DIRECT INPUT
 *   \param [in] unit unit id
 *   \param [in] filename of XML to be opened
 *   \param [in] topname name of top node inside XML file
 *   \param [in] flags options for XML file opening
 * \par DIRECT OUTPUT:
 *   \retval handle to the node if operation succeeds
 *   \retval NULL on failure
 * \remark
 *   Failure may be either file does not exist or top name is wrong
 */
xml_node dbx_file_get_xml_top(
    int unit,
    char *filename,
    char *topname,
    int flags);

/**
 * \brief Saves XML file
 * \par DIRECT INPUT
 *   \param [in] unit unit id
 *   \param [in] filename of XML to be opened
 *   \param [in] top XML node of top node inside XML file
 *   \param [in] flags options for XML file opening
 * \par DIRECT OUTPUT:
 *   \retval shr_errro_e
 */
shr_error_e dbx_file_save_xml(
    int unit,
    char *filename,
    void *top,
    int flags);
/**
 * \brief Open file for overwrite, previous content will be deleted on close
 * \par DIRECT INPUT
 *   \param [in] filepath - full path either relative or absolute for file to be opened
 * \par DIRECT OUTPUT:
 *   \retval NULL if there was some problem with the path
 *   \retval FILE io handle
 * \remark
 *   May be used for any files
 */
void *dbx_file_open(
    char *filepath);

/**
 * \brief Close file by its handle
 * \par DIRECT INPUT
 *   \param [in] in - file handle obtained through dbx_file_close
 */
void dbx_file_close(
    void *in);

/**
 * \brief Write buffer to file
 * \par DIRECT INPUT
 *   \param [in] in - file handle obtained through dbx_file_close
 *   \param [in] buffer - buffer to be written to file
 * \par DIRECT OUTPUT:
 *   \retval number - number of bytes actually written, if less then the original buffer or 0 - error
 */
int dbx_file_write(
    void *in,
    char *buffer);

/**
* \brief
*   Add device specific suffix to given string buffer.
*    \param [in] unit - Relevant unit.
*    \param [in] buffer - String buffer. Important note: the buffer will not be initialized in this function, the caller should do it.
*    \param [in] create_dir - Create the directory tree if this flag is TRUE.
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    None
*/
shr_error_e dbx_file_add_device_specific_suffix(
    int unit,
    char *buffer,
    uint8 create_dir);

shr_error_e dbx_file_get_db(
    int unit,
    char *db_name,
    char *base_db_name);

#endif /* DBX_FILE_H_INCLUDED */
