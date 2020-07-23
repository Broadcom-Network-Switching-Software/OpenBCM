/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    dbx_file.c
 * Purpose:    Misc. routines used by export/import/show facilities
 */
#include <soc/drv.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <sys/stat.h>
#include <shared/bsl.h>

#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_pre_compiled_xml_parser.h>
#endif /* BCM_PETRA_SUPPORT */
#if defined(BCM_DNX_SUPPORT) || defined(BCM_PETRA_SUPPORT)
#include <shared/dbx/dbx_pre_compiled_xml_parser_internal.h>
#endif /* BCM_DNX_SUPPORT || BCM_PETRA_SUPPORT */
#include <soc/sand/auto_generated/dbx_pre_compiled_common.h>

#include <sal/appl/config.h>
#include <shared/shrextend/shrextend_error.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

DBX_FILE_TYPE_E
dbx_file_get_type(
    char *filename)
{
    char **tokens;
    uint32 realtokens = 0;
    DBX_FILE_TYPE_E ret = DBX_FILE_MAX;
    char *extension;

    if ((tokens = utilex_str_split(filename, ".", 10, &realtokens)) == NULL)
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(0, "Failed to split:%s\n"), filename));
        return ret;
    }

    if ((realtokens == 1) || (realtokens == 0))
    {
        ret = DBX_FILE_NONE;
    }
    else
    {
        extension = tokens[realtokens - 1];
        if (!sal_strcmp(extension, "csv") || !sal_strcmp(extension, "txt"))
            ret = DBX_FILE_CSV;
        else if (!sal_strcmp(extension, "xml"))
            ret = DBX_FILE_XML;
        else if (!sal_strcmp(extension, "v"))
            ret = DBX_FILE_VERILOG;
        else
            ret = DBX_FILE_UKNOWN;
    }

    utilex_str_split_free(tokens, realtokens);
    return ret;
}

#if !defined(NO_FILEIO)
#include <stdlib.h>
/*
 * {
 */
int
dbx_file_dir_exists(
    char *dirpath)
{
    SAL_DIR *dir;

    dir = sal_opendir(dirpath);
    if (!dir)
    {
        /*
         * No such directory
         */
        return FALSE;
    }

    sal_closedir(dir);
    return TRUE;
}

int
dbx_file_dir_create(
    char *dir)
{
    int rv = 0;
    if (dbx_file_dir_exists(dir) == FALSE)
    {
        mode_t prev_mask;
        /*
         * Set mask to 0
         */
        prev_mask = sal_umask(0);
        if ((rv = sal_mkdir(dir)) != 0)
        {
            char pwd[256];

            if (sal_getcwd(pwd, sizeof(pwd)) == NULL)
            {
                LOG_ERROR(BSL_LOG_MODULE, ("Unable to determine pwd on folder:%s create\n", dir));
            }
            else
            {
                LOG_ERROR(BSL_LOG_MODULE, ("Unable to create:%s folder in %s location\n", dir, pwd));
            }
        }
        sal_umask(prev_mask);
    }
    return rv;
}

static void
dbx_file_add_dir_to_file_path(
    char *file_name,
    char *dir)
{
    char full_path[RHFILE_MAX_SIZE];

    sal_strncpy(full_path, dir, sizeof(full_path) - 1);
    full_path[sizeof(full_path) - 1] = '\0';
    sal_strncat_s(full_path, "/", sizeof(full_path));
    sal_strncat_s(full_path, file_name, sizeof(full_path));
    sal_strcpy(file_name, full_path);
}

int
dbx_file_get_xml_files_from_dir(
    char *chip_db,
    char *dirpath,
    char **files_list)
{
    int idx = 0;
    SAL_DIR *dir;
    struct sal_dirent *dirent;
    int unit = 0;

    char *db_dir = NULL;
    char db_path[RHFILE_MAX_SIZE];
    char sub_dir[RHFILE_MAX_SIZE];
    char last_sub_dir[RHFILE_MAX_SIZE];

    /*
     * check if main directory exists, check property and env variable
     */
    db_dir = sal_config_get("dpp_db_path");
    if (ISEMPTY(db_dir) || (dbx_file_dir_exists(db_dir) == FALSE))
    {
        db_dir = getenv("DPP_DB_PATH");
        if (ISEMPTY(db_dir) || (dbx_file_dir_exists(db_dir) == FALSE))
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "DB_PATH was not found in %s\n"), db_dir));
            db_dir = "./db";
        }
    }

    sal_strncpy(db_path, db_dir, sizeof(db_path) - 1);
    db_path[sizeof(db_path) - 1] = '\0';
    sal_strncat_s(db_path, "/", sizeof(db_path));
    sal_strncat_s(db_path, chip_db, sizeof(db_path));
    sal_strncat_s(db_path, "/", sizeof(db_path));
    sal_strncat_s(db_path, dirpath, sizeof(db_path));

    if (!dbx_file_dir_exists(db_path))
    {
        return 0;
    }

    dir = sal_opendir(db_path);

    while ((dirent = sal_readdir(dir)))
    {
        if (sal_strcmp(dirent->d_name, ".") == 0 || sal_strcmp(dirent->d_name, "..") == 0)
        {
            continue;
        }

        sal_strncpy(sub_dir, db_path, sizeof(sub_dir) - 1);
        sub_dir[sizeof(sub_dir) - 1] = '\0';
        sal_strncat_s(sub_dir, "/", sizeof(sub_dir));
        sal_strncat_s(sub_dir, dirent->d_name, sizeof(sub_dir));
        if (dbx_file_dir_exists(sub_dir))
        {
            /*
             * directory - recursive call
             */
            int nof_xml_in_sub;
            int ii;

            sal_strncpy(sub_dir, dirpath, sizeof(sub_dir) - 1);
            sub_dir[sizeof(sub_dir) - 1] = '\0';
            sal_strncat_s(sub_dir, "/", sizeof(sub_dir));
            sal_strncat_s(sub_dir, dirent->d_name, sizeof(sub_dir));
            sal_strncpy(last_sub_dir, dirent->d_name, sizeof(last_sub_dir) - 1);
            last_sub_dir[sizeof(last_sub_dir) - 1] = '\0';

            nof_xml_in_sub = dbx_file_get_xml_files_from_dir(chip_db, sub_dir, &(files_list[idx]));
            for (ii = 0; ii < nof_xml_in_sub; ii++)
            {
                dbx_file_add_dir_to_file_path(files_list[idx + ii], last_sub_dir);
            }
            idx += nof_xml_in_sub;
        }
        else if (dbx_file_get_type(dirent->d_name) == DBX_FILE_XML)
        {
            /*
             * xml file - add to files list
             */
            sal_strcpy(files_list[idx++], dirent->d_name);
        }
    }

    sal_closedir(dir);
    return idx;
}

int
dbx_file_exists(
    char *filepath)
{

    FILE *in;

    in = sal_fopen(filepath, "r");
    if (!in)
    {
#if defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA)
        /*
         * Check if we have it pre-compiled.
         */
        if (!pre_compiled_xml_file_exists(filepath))
        {
            return FALSE;
        }
#endif /* defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA) */
        /*
         * No such file - not an error - just warning
         */
        return FALSE;
    }
    sal_fclose(in);
    return TRUE;
}

shr_error_e
dbx_file_remove(
    char *filepath)
{
    FILE *in;
    int unit = 0;

    SHR_FUNC_INIT_VARS(unit);

    if ((in = sal_fopen(filepath, "w")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not open file %s for writing\n", filepath);
    }
    else
    {
        sal_fclose(in);
    }

    if (sal_remove(filepath) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not remove file %s\n", filepath);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Get full path for file in DB folder
 * \par DIRECT INPUT
 *   \param [in] chip_db directory inside DB which contains files relevant to the chip
 *   \param [in] filename name of the file
 *   \param [in,out] filepath pointer to string where resulting file path should be copied
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE if path was successfully created
 *   \retval _SHR_E_PARAM if there was some problem with parameters
 *   \retval _SHR_E_NOT_FOUND if the file or directory were not found
 *   \retval _SHR_E_INTERNAL if file or directory names exceed the allowed limit
 */
static shr_error_e
dbx_file_get_db_location(
    char *chip_db,
    char *filename,
    char *filepath)
{
    static char sdk_db_dir[RHFILE_MAX_SIZE] = { 0 };
    int unit = 0;
    int descr_size;
    uint8 dir_override = TRUE;

    /*
     * Filename cannot be empty - this is clear error
     */
    if (ISEMPTY(filename))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DB Resource not defined\n")));
        return _SHR_E_PARAM;
    }
    /*
     * Check static variable - may be it was already initialized
     */
    if (ISEMPTY(sdk_db_dir))
    {
        /**Check for soc property and env variable*/
        char *db_dir = sal_config_get("dpp_db_path");
        if (ISEMPTY(db_dir) || (dbx_file_dir_exists(db_dir) == FALSE))
        {
            db_dir = getenv("DPP_DB_PATH");
            /*
             * No explicit DB path - try via local
             */
            if (ISEMPTY(db_dir) || (dbx_file_dir_exists(db_dir) == FALSE))
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "DB was not found in %s\n"), db_dir));
                dir_override = FALSE;
                /*
                 * If environment variables do not exist or DB inside does not exist, try local one
                 */
                db_dir = "./db";
                if (dbx_file_dir_exists(db_dir) == FALSE)
                {
                    /*
                     * DB does not exists and cannot be found in any place
                     */
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "DB was not found in %s\n"), db_dir));
                    return _SHR_E_NOT_FOUND;
                }
                sal_getcwd(sdk_db_dir, RHFILE_MAX_SIZE);

                sal_strncat_s(sdk_db_dir, "db", sizeof(sdk_db_dir));
            }
        }
        if (dir_override)
        {
            sal_strncpy(sdk_db_dir, db_dir, RHFILE_MAX_SIZE - 1);
        }
    }

    if (!ISEMPTY(chip_db))
    {
        descr_size = sal_strlen(chip_db);
    }
    else
    {
        descr_size = 0;
    }

    if ((sal_strlen(sdk_db_dir) + descr_size + sal_strlen(filename)) + 3 > RHFILE_MAX_SIZE)
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "PATH %s is too long\n"), sdk_db_dir));
        return _SHR_E_INTERNAL;
    }

    if (ISEMPTY(chip_db))
    {
        sal_snprintf(filepath, RHFILE_MAX_SIZE - 1, "%s/%s", sdk_db_dir, filename);
    }
    else
    {
        sal_snprintf(filepath, RHFILE_MAX_SIZE - 1, "%s/%s/%s", sdk_db_dir, chip_db, filename);
    }

    return _SHR_E_NONE;
}

shr_error_e
dbx_file_find_sdk_path(
    char *sdk_dir)
{
    static char tmp_sdk_dir[RHFILE_MAX_SIZE];
    char *pos;
    int loc;
    char pwd[RHFILE_MAX_SIZE];
    char *relatives_dir[3] = { "/regress/bcm/tests/dnx/", "/tests/dnx/", "/include/soc/drv.h" };

    if (ISEMPTY(tmp_sdk_dir))
    {
        for (int i = 0; i < 3; i++)
        {
            sal_getcwd(pwd, RHFILE_MAX_SIZE);
            while ((pos = sal_strrchr(pwd, '/')) != NULL)
            {
                loc = pos - pwd + 1;
                pwd[loc - 1] = '\0';
                sal_strncpy(tmp_sdk_dir, pwd, loc);
                sal_strncat(tmp_sdk_dir, relatives_dir[i], RHFILE_MAX_SIZE - (sal_strlen(tmp_sdk_dir) + 1));
                if (dbx_file_dir_exists(tmp_sdk_dir) || dbx_file_exists(tmp_sdk_dir))
                {
                    sal_strncpy(sdk_dir, pwd, loc);
                    sal_strncpy(tmp_sdk_dir, sdk_dir, RHFILE_MAX_SIZE - 1);
                    return _SHR_E_NONE;
                }
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(0, "SDK variable not found\n")));
        return _SHR_E_NOT_FOUND;
    }
    sal_strncpy(sdk_dir, tmp_sdk_dir, RHFILE_MAX_SIZE - 1);
    return _SHR_E_NONE;
}

shr_error_e
dbx_file_get_sdk_path(
    int unit,
    char *filename,
    char *location,
    char *filepath)
{
    char *sdk_dir;
    char sdk_dir_found[RHFILE_MAX_SIZE];
    char pwd[RHFILE_MAX_SIZE];
    uint8 should_free_sdk_dir = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (ISEMPTY(filename))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Filename not defined\n");
    }

    sal_getcwd(pwd, RHFILE_MAX_SIZE);

    sdk_dir = getenv("SDK");

    if (ISEMPTY(sdk_dir))
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(0, "SDK variable not defined\n")));
        SHR_IF_ERR_EXIT(dbx_file_find_sdk_path(sdk_dir_found));
        sdk_dir = sal_alloc(RHFILE_MAX_SIZE * sizeof(char), "SDK directory");
        should_free_sdk_dir = 1;
        sal_strncpy(sdk_dir, sdk_dir_found, RHFILE_MAX_SIZE);
    }

    if (dbx_file_dir_exists(sdk_dir) == FALSE)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(0, "SDK directory:%s is unavailable\n"), sdk_dir));
        SHR_IF_ERR_EXIT(dbx_file_find_sdk_path(sdk_dir_found));
        sal_strncpy(sdk_dir, sdk_dir_found, RHFILE_MAX_SIZE);
    }

    sal_sprintf(filepath, "%s%s", sdk_dir, location);
    if (dbx_file_dir_exists(filepath) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Directory:%s is unavailable\n", filepath);
    }

    if ((sal_strlen(sdk_dir) + sal_strlen(location) + sal_strlen(filename)) >= RHFILE_MAX_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "PATH %s is too long\n", sdk_dir);
    }

    sal_sprintf(filepath, "%s%s%s", sdk_dir, location, filename);

exit:
    if (should_free_sdk_dir)
    {
        sal_free(sdk_dir);
    }
    SHR_FUNC_EXIT;

}

shr_error_e
dbx_file_get_db(
    int unit,
    char *db_name,
    char *base_db_name)
{
    char device_n[RHNAME_MAX_SIZE];
    xml_node curTop = NULL, curSubTop, cur;

    SHR_FUNC_INIT_VARS(unit);

    if ((curTop = dbx_pre_compiled_common_top_get(unit, "DNX-Devices.xml", "top", 0)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    if ((curSubTop = dbx_xml_child_get_first(curTop, "devices")) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    /** First check presence of device specific name in the list of supported devices */
    RHDATA_ITERATOR(cur, curSubTop, "device")
    {
        RHDATA_GET_STR_CONT(cur, "chip", device_n);
        if (!sal_strcasecmp(device_n, soc_dev_name(unit)))
        {
            RHDATA_GET_STR_CONT(cur, "db_name", db_name);
            RHDATA_GET_STR_DEF_NULL(cur, "base_db_name", base_db_name);
            SHR_EXIT();
        }
    }
    /** Not found in device name list, look into the driver  */
    RHDATA_ITERATOR(cur, curSubTop, "device")
    {
        RHDATA_GET_STR_CONT(cur, "chip", device_n);
        if (!sal_strcasecmp(device_n, SOC_CHIP_NAME(SOC_CONTROL(unit)->chip_driver->type)))
        {
            RHDATA_GET_STR_CONT(cur, "db_name", db_name);
            RHDATA_GET_STR_DEF_NULL(cur, "base_db_name", base_db_name);
            SHR_EXIT();
        }
    }
    /** Not found in device name list, look into the family list */
    RHDATA_ITERATOR(cur, curSubTop, "device")
    {
        RHDATA_GET_STR_CONT(cur, "chip", device_n);
        if (!sal_strcasecmp(device_n, SOC_CHIP_STRING(unit)))
        {
            RHDATA_GET_STR_CONT(cur, "db_name", db_name);
            RHDATA_GET_STR_DEF_NULL(cur, "base_db_name", base_db_name);
            SHR_EXIT();
        }
    }

exit:
    if (curTop != NULL)
    {
        dbx_xml_top_close(curTop);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dbx_file_get_base_db(
    int unit,
    char *db_name,
    char *base_db_name)
{
    char cur_db_name[RHNAME_MAX_SIZE];
    xml_node curTop = NULL, curSubTop, cur;

    SHR_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

    if ((curTop = dbx_pre_compiled_common_top_get(unit, "DNX-Devices.xml", "top", 0)) == NULL)
    {
        SHR_EXIT();
    }

    if ((curSubTop = dbx_xml_child_get_first(curTop, "devices")) == NULL)
    {
        SHR_EXIT();
    }
    /** First check presence of device specific name in the list of supported devices */
    RHDATA_ITERATOR(cur, curSubTop, "device")
    {
        RHDATA_GET_STR_CONT(cur, "db_name", cur_db_name);
        if (!sal_strcasecmp(cur_db_name, db_name))
        {
            RHDATA_GET_STR_DEF_NULL(cur, "base_db_name", base_db_name);
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            SHR_EXIT();
        }
    }

exit:
    if (curTop != NULL)
    {
        dbx_xml_top_close(curTop);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dbx_file_get_dev_path(
    int unit,
    char *db_name,
    char *filename,
    int flags,
    char *filepath)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Files opened with CONF_OPEN_ALTER_LOC must exits, so me need to check the presence
     * Also CONF_OPEN_CREATE or CONF_OPEN_OVERWRITE cannot be present
     */
    if (flags & CONF_OPEN_ALTER_LOC)
    {
        char alt_file_loc[RHNAME_MAX_SIZE];
        if (flags & (CONF_OPEN_CREATE | CONF_OPEN_OVERWRITE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid combination of flags on:\"%s\"\n", filename);
        }
        sal_snprintf(alt_file_loc, RHNAME_MAX_SIZE - 1, "internal/%s", filename);
        if ((dbx_file_get_db_location(db_name, alt_file_loc, filepath) == _SHR_E_NONE) && dbx_file_exists(filepath))
        {       /** File found in alternative location */
            SHR_EXIT();
        }
    }

    SHR_SET_CURRENT_ERR(dbx_file_get_db_location(db_name, filename, filepath));
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
    {
        /** Check existence on files with READ intention only*/
        if (!(flags & (CONF_OPEN_CREATE | CONF_OPEN_OVERWRITE)))
        {
            if (!dbx_file_exists(filepath))
            {
                SHR_SET_CURRENT_ERR(_SHR_E_EXISTS);
                SHR_EXIT();
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbx_file_get_file_path(
    int unit,
    char *filename,
    int flags,
    char *filepath)
{
    char db_name[RHNAME_MAX_SIZE] = { 0 };
    char base_db_name[RHNAME_MAX_SIZE] = { 0 };
    char **tokens = NULL;
    uint32 realtokens = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & CONF_OPEN_CURRENT_LOC)
    {
        /*
         * DO not go look for DB - only filename relatively to current location
         */
        sal_strncpy(filepath, filename, RHFILE_MAX_SIZE - 1);
        SHR_EXIT();
    }

    /** PER_DEVICE flag means that specific file is under device db */
    if (flags & CONF_OPEN_PER_DEVICE)
    {
        if (dbx_file_get_db(unit, db_name, base_db_name) != _SHR_E_NONE)
            SHR_EXIT();
    }

    SHR_SET_CURRENT_ERR(dbx_file_get_dev_path(unit, db_name, filename, flags, filepath));
    /** If we have not found the file in base and base db is not empty look in the base */
    if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
    {
        if (!ISEMPTY(base_db_name))
        {
            SHR_SET_CURRENT_ERR(dbx_file_get_dev_path(unit, base_db_name, filename, flags, filepath));
        }
        else
        {
            /** May be upper folder in filename path is actually db name, in this case, look for its base */
            if (((tokens = utilex_str_split(filename, "/", 2, &realtokens)) == NULL) || (realtokens == 1))
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(0, "No folders in:%s\n"), filename));
            }
            else
            {
                SHR_SET_CURRENT_ERR(dbx_file_get_base_db(unit, tokens[0], base_db_name));
                /** Base not found - nothing to do - leave with NOT_FOUND error */
                if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
                {
                    SHR_EXIT();
                }
                SHR_SET_CURRENT_ERR(dbx_file_get_dev_path(unit, base_db_name, tokens[1], flags, filepath));
            }
        }
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

xml_node
dbx_file_get_xml_top(
    int unit,
    char *filename,
    char *topname,
    int flags)
{
    void *curTop = NULL;
    char filepath[RHFILE_MAX_SIZE];
    char local_filename[RHFILE_MAX_SIZE];
    DBX_FILE_TYPE_E file_type = dbx_file_get_type(filename);

    /*
     * If no known extension recognized assume that intension was for XML file
     */
    sal_strncpy(local_filename, filename, RHFILE_MAX_SIZE - 1);
    if (file_type == DBX_FILE_NONE)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(0, "xml extension added to %s\n"), filename));
        sal_strncat_s(local_filename, ".xml", sizeof(local_filename));
    }
    else if (file_type != DBX_FILE_XML)
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(0, "Filename:%s is not XML one\n"), filename));
        SHR_EXIT();
    }

    if (dbx_file_get_file_path(unit, local_filename, flags, filepath) != _SHR_E_NONE)
    {
        if (!(flags & CONF_OPEN_NO_ERROR_REPORT))
        {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "%s was not found\n"), filepath));
        }
        SHR_EXIT();
    }

    if ((curTop = dbx_xml_top_get(filepath, topname, flags)) == NULL)
    {
#if defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA)
        /*
         * We do not need the file path, only the name.
         */
        if ((curTop = pre_compiled_dbx_xml_top_get(unit, filename, topname, flags)) == NULL)
        {
#endif /* defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA) */
            if (!(flags & CONF_OPEN_NO_ERROR_REPORT))
            {
                LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "%s was not found in %s\n"), topname, filepath));
            }
            SHR_EXIT();
#if defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA)
        }
#endif /* defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA) */
    }

exit:
    return curTop;
}

shr_error_e
dbx_file_save_xml(
    int unit,
    char *filename,
    void *top,
    int flags)
{
    char filepath[RHFILE_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbx_file_get_file_path(unit, filename, flags, filepath));

    dbx_xml_top_save(top, filepath);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbx_file_get_ucode_location(
    int unit,
    char *filename,
    char *filepath)
{
    static char sdk_bcm_user_dir[RHFILE_MAX_SIZE] = { 0 };
    static char db_name[RHNAME_MAX_SIZE] = { 0 };
    char base_db_name[RHNAME_MAX_SIZE] = { 0 };

    /*
     * Filename cannot be empty - this is clear error
     */
    if (ISEMPTY(filename))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Resource not defined\n")));
        return _SHR_E_PARAM;
    }

    if (ISEMPTY(db_name))
    {
        if (dbx_file_get_db(unit, db_name, base_db_name) != _SHR_E_NONE)
            return _SHR_E_NOT_FOUND;
    }

    /*
     * Check static variable - may be it was already initialized
     */
    if (ISEMPTY(sdk_bcm_user_dir))
    {
        sal_getcwd(sdk_bcm_user_dir, RHFILE_MAX_SIZE);
    }

    sal_snprintf(filepath, RHFILE_MAX_SIZE - 1, "%spemla/%s/%s", sdk_bcm_user_dir, db_name, filename);

    return _SHR_E_NONE;
}

void *
dbx_file_open(
    char *filename)
{
    FILE *in;

    if ((in = sal_fopen(filename, "w+")) == NULL)
    {   /* Could not open the file, usually means that path was illegal */
        return NULL;
    }

    return (void *) in;
}

void
dbx_file_close(
    void *in)
{
    sal_fclose((FILE *) in);
    return;
}

int
dbx_file_write(
    void *in,
    char *buffer)
{
    return sal_fwrite(buffer, strlen(buffer), 1, in);
}

shr_error_e
dbx_file_add_device_specific_suffix(
    int unit,
    char *buffer,
    uint8 create_dir)
{
    char db_name[RHNAME_MAX_SIZE] = { 0 };
    char base_db_name[RHNAME_MAX_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (dbx_file_get_db(unit, db_name, base_db_name) != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    sal_strncat(buffer, db_name, RHFILE_MAX_SIZE);
    if (create_dir)
    {
        SHR_IF_ERR_EXIT(dbx_file_dir_create(buffer));
    }

#ifdef ADAPTER_SERVER_MODE
    sal_strncat(buffer, "/adapter", RHFILE_MAX_SIZE);
    if (create_dir)
    {
        SHR_IF_ERR_EXIT(dbx_file_dir_create(buffer));
    }
#endif /* ADAPTER_SERVER_MODE */

exit:
    SHR_FUNC_EXIT;
}

#else
xml_node
dbx_file_get_xml_top(
    int unit,
    char *filename,
    char *topname,
    int flags)
{
    return NULL;
}
/*
 * }
 */
#endif /* !defined(NO_FILEIO) */
