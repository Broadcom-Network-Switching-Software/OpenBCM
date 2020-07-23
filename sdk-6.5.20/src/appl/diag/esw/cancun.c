/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cih.c
 * Purpose: CANCUN operation handler
 */

#ifndef NO_SAL_APPL

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <soc/drv.h>
#include <soc/esw/cancun.h>

/*
 * Local variable declares
 */
char cmd_cancun_usage[] =
    "Parameters: [stat|list|load|debug|help] <args...>\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "  CANCUN operations including information display and file loading. The\n"
    "  supported operations are:\n"
    "\t stat\t - Display currently loaded CANCUN information\n"
    "\t list\t - List all CANCUN loadable files in current file system\n"
    "\t load\t - Load a particular CANCUN loadable file\n"
    "\t debug\t - Debug commands (TBD)\n"
    "\t help\t - Display this usage page\n"
    "\n"
    "  Arguments for each operation:\n"
    "  - list: [(none)|<filename>]\n"
    "\t (none) - If there is no argument after this operation, it displays all\n"
    "\t the available files and directories under CANCUN file repository\n"
    "\t directory in current file system\n"
    "\t <filename> - If a specific filename is entered, which should include\n"
    "\t the path related to CANCUN file repository directory, this command\n"
    "\t displays the information of this file only\n"
    "\n"
    "  - load: <filename> <file type> <file format>\n"
    "\t <filename> - If there is no argument after this operation, it will \n"
    "\t load all the default CANCUN files. If this file is in packaged format\n"
    "\t , no addition arguments are needed because software can extract them\n"
    "\t from the file itself.\n"
    "\t <file type> - For a non-packaged format file, this argument is needed\n"
    "\t Currently supported file type includes [cih|cmh|cch]\n"
    "\t <file format> - For a non-packaged format file, this argument is needed\n"
    "\t Currently supported file format includes [pio|dma]\n"
#endif
    ;

static const char* soc_cancun_file_type_string[CANCUN_SOC_FILE_TYPE_NUM] =
                   CANCUN_FILE_TYPE_NAMES_INITIALIZER;

static const char* soc_cancun_file_format_string[CANCUN_SOC_FILE_FORMAT_NUM] =
                   CANCUN_FILE_FORMAT_NAMES_INITIALIZER;

static const char* soc_cancun_load_status_string[CANCUN_SOC_FILE_LOAD_STATUS_NUM] =
                   CANCUN_FILE_LOAD_STATUS_INITIALIZER;

/*
 * Local function declares
 */
cmd_result_t _cmd_cancun_status(int unit);
int _cmd_cancun_file_load(int unit, char* filename, uint32 file_type,
                          uint32 file_format, soc_cancun_t *cc);
int _cancun_file_buffer_load(char *filename, uint8 **buf, long *buf_len);
int _cancun_file_info_print(soc_cancun_file_t *ccf, int row_mode);
void _cancun_sdk_version_string_get(char* ver_string, uint32 ver);
void _cancun_version_string_get(char* ver_string, uint32 ver);
void _soc_cancun_default_filename(int unit, char *filename, int file_type);
int _soc_cancun_dest_entry_print(soc_cancun_dest_entry_t *dest_entry);
void _cancun_revision_print(uint32 ver);
STATIC void _cancun_bug_fix_rev_print(int unit);

/*
 * Function:   cmd_cancun
 * Purpose:    Process all the CANCUN file and status operations
 * Parameters: unit - unit
 *             a - args, each of the files to be displayed.
 * Returns:    CMD_OK/CMD_FAIL/CMD_USAGE
 */
cmd_result_t
cmd_cancun(int unit, args_t *a)
{
    char *c;
#ifndef NO_FILEIO
    char filename[CANCUN_FILENAME_SIZE] = {0};
    uint32 file_type = CANCUN_SOC_FILE_TYPE_UNKNOWN;
    uint32 file_format = CANCUN_SOC_FILE_FORMAT_UNKNOWN;
#endif /* NO_FILEIO */

    soc_cancun_t* cc;
    uint32* p = NULL;
    int rv = 0;

    if (!sh_check_attached("cancun", unit)) {
        return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_cancun)) {
        return (CMD_NFND);
    }

    c = ARG_GET(a);
    if(!c) {
        return (CMD_USAGE);
    } else if(!sal_strcasecmp(c, "help")) {
        return (CMD_USAGE);
    }

    rv = soc_cancun_status_get(unit, &cc);
    if(rv != 0) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "can't get CANCUN status. (%s)\n"), soc_errmsg(rv)));
        return(CMD_FAIL);
    }

    /* Operation: stat */
    if (!sal_strcasecmp(c, "stat")) {
        rv = (int)_cmd_cancun_status(unit);

    /* Operation: load */
    } else if (!sal_strcasecmp(c, "load")) {
#ifndef NO_FILEIO
        /* Arguments check */
        c = ARG_GET(a);
        if(!c) {
            /* TBA: Load all default CANCUN files */
            return (CMD_USAGE);
        } else if(sal_strlen(c) > CANCUN_FILENAME_SIZE) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "input filename is longer than limit %d\n"),
                CANCUN_FILENAME_SIZE));
            return (CMD_FAIL);
        } else {
            if (!sal_strcasecmp(c, "cih")) {
                file_type = CANCUN_SOC_FILE_TYPE_CIH;
            } else if (!sal_strcasecmp(c, "cmh")) {
                file_type = CANCUN_SOC_FILE_TYPE_CMH;
            } else if (!sal_strcasecmp(c, "cch")) {
                file_type = CANCUN_SOC_FILE_TYPE_CCH;
            } else if (!sal_strcasecmp(c, "ceh")) {
                file_type = CANCUN_SOC_FILE_TYPE_CEH;
            } else if (!sal_strcasecmp(c, "cfh")) {
                if (soc_feature(unit,soc_feature_flex_flow)) {
                    file_type = CANCUN_SOC_FILE_TYPE_CFH;
                } else {
                    LOG_DEBUG(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                    "Flow initialization mode is disabled for flow module.\n"
                    )));
                    return (CMD_OK);
                }
            } else if (!sal_strcasecmp(c, "ispf")) {
                file_type = CANCUN_SOC_FILE_TYPE_ISPF;
            } else {
                sal_strlcpy(filename, c, CANCUN_FILENAME_SIZE-1);
            }

            c = ARG_GET(a);
            if(c) {
                if (!sal_strcasecmp(c, "pio")) {
                    file_format = CANCUN_SOC_FILE_FORMAT_PIO;
                } else if (!sal_strcasecmp(c, "pack")) {
                    file_format = CANCUN_SOC_FILE_FORMAT_PACK;
                } else {
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                         "%s is not a supported file format\n"), c));
                    return (CMD_FAIL);
                }
            } else if(file_type != CANCUN_SOC_FILE_TYPE_UNKNOWN) {
                LOG_INFO(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                     "Loading default CANCUN %s files\n"),
                     soc_cancun_file_type_string[file_type]));
                _soc_cancun_default_filename(unit, filename, file_type);
            }
        }

        rv = _cmd_cancun_file_load(unit, filename, file_type, file_format, cc);
#else
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "No file system\n")));
#endif /* NO_FILEIO */
    /* Operation: list */
    } else if (!sal_strcasecmp(c, "list")) {
#ifndef NO_FILEIO
        SAL_DIR *dir, *dir2;
        struct sal_dirent *d;
        char *filename;
        uint8 *buf = NULL;
        long buf_len = 0;
        soc_cancun_file_t ccf;

        dir = sal_opendir(".");
        if (dir) {
            cli_out("   %-50s %-7s %-8s %-11s  %s\n", "FILE", "TYPE", "FORMAT",
                    "VER", "VALID");
            while ((d = sal_readdir(dir)) != NULL) {
                filename = d->d_name;
                if (!sal_strcasecmp(filename, ".") ||
                    !sal_strcasecmp(filename, "..")) {
                    continue;
                }

                dir2 = sal_opendir(filename);
                if (dir2) {
                    /* Skip directories */
                    cli_out(" D %s\n", filename);
                    sal_closedir(dir2);
                } else {
                    rv = _cancun_file_buffer_load(filename, &buf, &buf_len);
                    if (rv == SOC_E_NONE && buf != NULL) {
                        rv = soc_cancun_file_info_get(unit, &ccf, filename, buf, buf_len);
                        if (rv == SOC_E_NONE) {
                            _cancun_file_info_print(&ccf, 1);
                        }
                    } else {
                        if(buf != NULL) {
                            sal_free(buf);
                        }
                    }
                }
            }
            sal_closedir(dir);
        }
#else
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "No file system\n")));
        return (CMD_FAIL);
#endif /* NO_FILEIO */
    /* Operation: scan */
    } else if (!sal_strcasecmp(c, "scan")) {
        c = ARG_GET(a);
        if (!c || !sal_strcasecmp(c, "cmh")) {
            rv = soc_cancun_access_scan(unit, CANCUN_SOC_FILE_TYPE_CMH);
            cli_out("CMH access scan %s\n", (rv == SOC_E_NONE)? "PASS": "FAIL");
        }
        if (!c || !sal_strcasecmp(c, "cch")) {
            rv = soc_cancun_access_scan(unit, CANCUN_SOC_FILE_TYPE_CCH);
            cli_out("CCH access scan %s\n", (rv == SOC_E_NONE)? "PASS": "FAIL");
        }

    /* Operation: debug */
    } else if (!sal_strcasecmp(c, "debug")) {

        /* Arguments check */
        c = ARG_GET(a);
        if(!c) {
            /* TBA: Display default debug info */
        } else {
            if (!sal_strcasecmp(c, "cmh")) {
                soc_cancun_cmh_t* cmh = cc->cmh;
                soc_cancun_hash_table_t* hash_table_header;
                uint32 i, entry_num;

                if(cmh == NULL) {
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                         "CANCUN CMH is not initialized\n")));
                    return (CMD_FAIL);
                } else if (cmh->status != SOC_CANCUN_LOAD_STATUS_LOADED) {
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                         "CANCUN CMH is not loaded\n")));
                    return (CMD_FAIL);
                }
                hash_table_header = (soc_cancun_hash_table_t*) cmh->cmh_table;

                cli_out("%s: CMH debug information:\n", ARG_CMD(a));
                cli_out("%s: CMH hash table\n", ARG_CMD(a));
                cli_out("\tpA = %d\n", hash_table_header->pa);
                cli_out("\tpB = %d\n", hash_table_header->pb);
                cli_out("\tpC = %d\n", hash_table_header->pc);
                cli_out("\tpD = %d\n", hash_table_header->pd);
                cli_out("\tsize = %d\n", hash_table_header->table_size);
                cli_out("\tList = \n\n");
                soc_cancun_cmh_list (unit, &p);
                if(p) {
                    entry_num = *p++;
                    cli_out("\t%-40s %-40s %-3s\n", "MEM","FIELD", "APP");
                    for(i = 0; i < entry_num; i++) {
                        cli_out("\t%-40s %-40s %-3d\n",
                                (*p >= NUM_SOC_MEM)? SOC_REG_NAME(unit, *p):
                                                     SOC_MEM_NAME(unit, *p),
                                *(p+1) < NUM_SOC_FIELD ? SOC_FIELD_NAME(unit, *(p+1)): "",
                                *(p+2));
                        p += 3;
                    }
                }
#if 0
                cli_out("\tRaw data = \n");
                p = &hash_table_header->table_entry;
                if(p) {
                    for(i = 0; i < hash_table_header->table_size; i++) {
                        if((i % 10) == 0) {
                            cli_out("\n");
                        }
                        cli_out("%08X ", *p++);
                    }
                }
                cli_out("\n");
#endif
            } else if (!sal_strcasecmp(c, "cch")) {
                soc_cancun_cch_t* cch = cc->cch;
                soc_cancun_hash_table_t* hash_table_header;
                uint32 i, j, mem_num, fld_num, list_buf[CANCUN_LIST_BUF_LEN];

                if(cch == NULL) {
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                         "CANCUN CCH is not initialized\n")));
                    return (CMD_FAIL);
                } else if (cch->status != SOC_CANCUN_LOAD_STATUS_LOADED) {
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                         "CANCUN CCH is not loaded\n")));
                    return (CMD_FAIL);
                }
                hash_table_header = (soc_cancun_hash_table_t*) cch->cch_table;

                cli_out("%s: CCH debug information:\n", ARG_CMD(a));
                cli_out("%s: CCH hash table\n", ARG_CMD(a));
                cli_out("\tpA = %d\n", hash_table_header->pa);
                cli_out("\tpB = %d\n", hash_table_header->pb);
                cli_out("\tpC = %d\n", hash_table_header->pc);
                cli_out("\tpD = %d\n", hash_table_header->pd);
                cli_out("\tsize = %d\n", hash_table_header->table_size);
                cli_out("\tCCH Coverage List = \n");
                soc_cancun_cch_list (unit, list_buf, CANCUN_LIST_BUF_LEN);
                p = list_buf;
                mem_num = *p++;
                for(i = 0; i < mem_num; i++) {
                    cli_out("\t%-20s\n", SOC_REG_NAME(unit, *p++));
                    fld_num = *p++;
                    for(j = 0; j < fld_num; j++) {
                        cli_out("\t |- %-20s\n", SOC_FIELD_NAME(unit, *p++));
                    }
                }
#if 0
                cli_out("\tRaw data = \n");
                p = &hash_table_header->table_entry;
                if(p) {
                    for(i = 0; i < hash_table_header->table_size; i++) {
                        if((i % 10) == 0) {
                            cli_out("\n");
                        }
                        cli_out("%08X ", *p++);
                    }
                }
                cli_out("\n");
#endif
            } else if (!sal_strcasecmp(c, "app")) {
                int src_app = 0;
                uint64 value;
                int rv = 0;

                COMPILER_64_ZERO(value);

                c = ARG_GET(a);
                if (!c || !isint(c)) {
                LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                    "%s is not app\n"), c));
                    return (CMD_FAIL);
                } else {
                    src_app = parse_integer(c);
                }

                c = ARG_GET(a);
                if (!c || !isint(c)) {
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                        "%s is not value\n"), c));
                    return (CMD_FAIL);
                } else {
                    value = parse_uint64(c);
                }

                rv = soc_cancun_cch_app_set(unit, src_app, value);
                if (rv != 0){
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                        "CANCUN CCH APP(%d) CFG failed (%s)\n"), src_app, soc_errmsg(rv)));
                    return (CMD_FAIL);
                };
            } else if (!sal_strcasecmp(c, "branch_id")) {
                int cancun_type = 0;
                int rv = 0;
                soc_cancun_file_branch_id_e branch_id = 0;

                c = ARG_GET(a);
                if (!c || !isint(c)) {
                LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                    "%s is not CANCUN type\n"), c));
                    return (CMD_FAIL);
                } else {
                    cancun_type = parse_integer(c);
                }
                rv = soc_cancun_branch_id_get(unit, cancun_type, &branch_id);
                if (rv != SOC_E_NONE){
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                        "CANCUN (%s) branch_id get failed (%s)\n"),
                        soc_cancun_file_type_string[cancun_type], soc_errmsg(rv)));
                    return (CMD_FAIL);
                } else {
                    cli_out("CANCUN (%s), branch_id(%d)\n",
                        soc_cancun_file_type_string[cancun_type], branch_id);
                };
            } else if (!sal_strcasecmp(c, "udf")) {
                uint8 stg;
                uint32 srcapp = 0;
                uint32 max_stages = 0;
                uint32 stage_idx = 0;
                soc_cancun_udf_stage_info_t stage_info_arr[10];
                uint32 *hfe_prof_ptr_arr[2];
                uint32 hfe_prof_ptr_arr_len[2] = {0};
                int rv = 0;

                c = ARG_GET(a);
                if (!c || !isint(c)) {
                LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                    "%s is not udf srcapp\n"), c));
                    return (CMD_FAIL);
                } else {
                    srcapp = parse_integer(c);
                }
                c = ARG_GET(a);
                if (!c || !isint(c)) {
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                        "%s is not value\n"), c));
                    return (CMD_FAIL);
                } else {
                    max_stages = parse_integer(c);
                }

                rv = soc_cancun_udf_abstr_type_info_get(unit, srcapp & 0xff,
                                                (srcapp >> 16) & 0xff,
                                                0, NULL, NULL,
                                                &max_stages,
                                                hfe_prof_ptr_arr_len);
                if (rv == SOC_E_NONE){
                    for (stg = 0; stg < max_stages; stg++) {
                        hfe_prof_ptr_arr[stg] = NULL;
                        hfe_prof_ptr_arr[stg] =
                             sal_alloc(sizeof(uint32) * hfe_prof_ptr_arr_len[stg],
                                       "UDF Hfe Prof Ptr Alloc.");
                    }

                    rv = soc_cancun_udf_abstr_type_info_get(unit, srcapp & 0xff,
                        (srcapp >> 16) & 0xff, max_stages, hfe_prof_ptr_arr,
                         stage_info_arr, NULL, NULL);
                    if (rv != SOC_E_NONE){
                       for (stg = 0; stg < max_stages; stg++) {
                           sal_free(hfe_prof_ptr_arr[stg]);
                       }
                    }
                }
                if (rv != SOC_E_NONE){
                    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                        "CANCUN udf stage info get failed (%s)\n"),
                        soc_errmsg(rv)));
                    return (CMD_FAIL);
                } else {
                    cli_out("CANCUN udf stage info:\n");
                    cli_out("size,policy_mem,hfe_profile_ptr, start_pos,flags\n");
                    for (stage_idx = 0; stage_idx < max_stages; stage_idx++) {
                        cli_out("%d,%s,%d,%d,%d\n",
                            stage_info_arr[stage_idx].size,
                            SOC_MEM_NAME(unit, stage_info_arr[stage_idx].policy_mem),
                            stage_info_arr[stage_idx].hfe_profile_ptr[0],
                            stage_info_arr[stage_idx].start_pos,
                            stage_info_arr[stage_idx].flags);
                    }

                    for (stg = 0; stg < max_stages; stg++) {
                        sal_free(hfe_prof_ptr_arr[stg]);
                    }
                };
            } else {
                LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                     "%s is not a supported input\n"), c));
                return (CMD_FAIL);
            }
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "Unknown operation %s\n"), c));
        return(CMD_USAGE);
    }

    return (CMD_OK);
}

/*
 * Function:   _cmd_cancun_status
 * Purpose:    Get the CANCUN control structure of current unit and display
 * Parameters: unit - unit
 * Returns:    CMD_OK/CMD_FAIL/CMD_USAGE
 */
cmd_result_t _cmd_cancun_status(int unit) {
    soc_cancun_t *cc;
    char ver_string[CANCUN_VERSION_LEN_MAX];
    char sdk_ver_string[CANCUN_VERSION_LEN_MAX];

    (void)soc_cancun_status_get(unit, &cc);

    if(cc == NULL) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META(
            "CANCUN is not initialized\n")));
        return CMD_FAIL;
    }

    cli_out("UNIT%d CANCUN: ", cc->unit);
    if(cc->flags & SOC_CANCUN_FLAG_VERSIONS_MATCH) {
        cli_out("%08X", cc->version);
    }
    if(cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
        cli_out("\nDebug Mode: \t%s\n",
                (cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE)? "ON": "OFF");
        cli_out("Skip Validity:  %s\n",
                (cc->flags & SOC_CANCUN_FLAG_SKIP_VALIDITY)? "ON": "OFF");
        if (soc_feature(unit,soc_feature_flex_flow)) {
            cli_out("Default Load:   %s%s%s%s%s\n",
                   (cc->flags & SOC_CANCUN_FLAG_LOAD_DEFAULT_CIH)? "CIH ": "",
                   (cc->flags & SOC_CANCUN_FLAG_LOAD_DEFAULT_CMH)? "CMH ": "",
                   (cc->flags & SOC_CANCUN_FLAG_LOAD_DEFAULT_CCH)? "CCH ": "",
                   (cc->flags & SOC_CANCUN_FLAG_LOAD_DEFAULT_CEH)? "CEH ": "",
                   (cc->flags & SOC_CANCUN_FLAG_LOAD_DEFAULT_CFH)? "CFH ": "");
        } else {
            cli_out("Default Load:   %s%s%s%s\n",
                   (cc->flags & SOC_CANCUN_FLAG_LOAD_DEFAULT_CIH)? "CIH ": "",
                   (cc->flags & SOC_CANCUN_FLAG_LOAD_DEFAULT_CMH)? "CMH ": "",
                   (cc->flags & SOC_CANCUN_FLAG_LOAD_DEFAULT_CCH)? "CCH ": "",
                   (cc->flags & SOC_CANCUN_FLAG_LOAD_DEFAULT_CEH)? "CEH ": "");
        }

        cli_out("REGFILE Ver:    %s\n", SOC_ORIGIN(unit));
    }

    if(cc->cih) {
        _cancun_version_string_get(ver_string, cc->cih->version);

        cli_out("\n\tCIH: %s\n",
                soc_cancun_load_status_string[cc->cih->status]);
        if(cc->cih->status != CANCUN_SOC_FILE_LOAD_NONE) {
            cli_out("\tVer: %s\n", ver_string);
            _cancun_revision_print(cc->cih->version);
            _cancun_bug_fix_rev_print(unit);
        }
        if(cc->cih->file.status != CANCUN_SOC_FILE_LOAD_NONE) {
            if(cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                cli_out("\t\t-Last Attempt-\n");
                _cancun_file_info_print(&cc->cih->file, 0);
            }
        }
    }

    if(cc->cmh) {
        _cancun_version_string_get(ver_string, cc->cmh->version);
        _cancun_sdk_version_string_get(sdk_ver_string, cc->cmh->sdk_version);

        cli_out("\n\tCMH: %s\n",
                soc_cancun_load_status_string[cc->cmh->status]);
        if(cc->cmh->status != CANCUN_SOC_FILE_LOAD_NONE) {
            cli_out("\tVer: %s\n", ver_string);
            _cancun_revision_print(cc->cmh->version);
            _cancun_bug_fix_rev_print(unit);
            cli_out("\tSDK Ver: %s\n", sdk_ver_string);
        }
        if(cc->cmh->file.status != CANCUN_SOC_FILE_LOAD_NONE) {
            if(cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                cli_out("\t\t-Last Attempt-\n");
                _cancun_file_info_print(&cc->cmh->file, 0);
            }
        }
    }

    if(cc->cch) {
        _cancun_version_string_get(ver_string, cc->cch->version);
        _cancun_sdk_version_string_get(sdk_ver_string, cc->cch->sdk_version);

        cli_out("\n\tCCH: %s\n",
                soc_cancun_load_status_string[cc->cch->status]);
        if(cc->cch->status != CANCUN_SOC_FILE_LOAD_NONE) {
            cli_out("\tVer: %s\n", ver_string);
            _cancun_revision_print(cc->cch->version);
            _cancun_bug_fix_rev_print(unit);
            cli_out("\tSDK Ver: %s\n", sdk_ver_string);
        }
        if(cc->cch->file.status != CANCUN_SOC_FILE_LOAD_NONE) {
            if(cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                cli_out("\t\t-Last Attempt-\n");
                _cancun_file_info_print(&cc->cch->file, 0);
            }
        }
    }
   
    if(cc->ceh) {
        _cancun_version_string_get(ver_string, cc->ceh->version);
        _cancun_sdk_version_string_get(sdk_ver_string, cc->ceh->sdk_version);

        cli_out("\n\tCEH: %s\n",
                soc_cancun_load_status_string[cc->ceh->status]);
        if(cc->ceh->status != CANCUN_SOC_FILE_LOAD_NONE) {
            cli_out("\tVer: %s\n", ver_string);
            _cancun_revision_print(cc->ceh->version);
            _cancun_bug_fix_rev_print(unit);
            cli_out("\tSDK Ver: %s\n", sdk_ver_string);
        }
        if(cc->ceh->file.status != CANCUN_SOC_FILE_LOAD_NONE) {
            if(cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                cli_out("\t\t-Last Attempt-\n");
                _cancun_file_info_print(&cc->ceh->file, 0);
            }
        }
    }

    if((soc_feature(unit,soc_feature_flex_flow)) &&
       (cc->flow_db)) {
        _cancun_version_string_get(ver_string, cc->flow_db->version);

        cli_out("\n\tCFH: %s\n",
                soc_cancun_load_status_string[cc->flow_db->status]);
        if(cc->flow_db->status != CANCUN_SOC_FILE_LOAD_NONE) {
            cli_out("\tVer: %s\n", ver_string);
            _cancun_revision_print(cc->flow_db->version);
            _cancun_bug_fix_rev_print(unit);
        }
        if(cc->flow_db->file.status != CANCUN_SOC_FILE_LOAD_NONE) {
            if(cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                cli_out("\t\t-Last Attempt-\n");
                _cancun_file_info_print(&cc->flow_db->file, 0);
            }
        }
    }
    /* Display ISPF status only when package file is present and
     * loading the file is attempted */
    if(cc->ispf->file.valid) {
        _cancun_version_string_get(ver_string, cc->ispf->version);

        cli_out("\n\tISPF: %s\n",
                soc_cancun_load_status_string[cc->ispf->status]);
        if(cc->ispf->status != CANCUN_SOC_FILE_LOAD_NONE) {
            cli_out("\tVer: %s\n", ver_string);
            _cancun_revision_print(cc->ispf->version);
            cli_out("\tISPF Ver: %d\n", cc->ispf->ispf_version);
        }
        if(cc->ispf->file.status != CANCUN_SOC_FILE_LOAD_NONE) {
            if(cc->flags & SOC_CANCUN_FLAG_DEBUG_MODE) {
                cli_out("\t\t-Last Attempt-\n");
                _cancun_file_info_print(&cc->ispf->file, 0);
            }
        }
    }

    cli_out("\n");

    return CMD_OK;
}

/*
 * Function:   _cancun_file_info_print
 * Purpose:    Print CANCUN loadable file information
 * Parameters: ccf      - (IN) Pointer to a CANCUN file structure
 *             row_mode - (IN) Option to print information in brief row mode
 * Returns:
 */
int _cancun_file_info_print(soc_cancun_file_t *ccf, int row_mode) {
    char ver_string[CANCUN_VERSION_LEN_MAX];

    if(ccf == NULL) return SOC_E_INTERNAL;

    _cancun_version_string_get(ver_string, ccf->header.version);

    if(row_mode) {
        cli_out("   %-50s %-7s %-8s %-11s  %s\n", ccf->filename,
                soc_cancun_file_type_string[ccf->type],
                soc_cancun_file_format_string[ccf->format],
                ver_string,
                (ccf->valid == 1)? "YES":"");
    } else {
        cli_out("\t\tFile:\t%s\n", ccf->filename);
        cli_out("\t\tVer:\t%s\n", ver_string);
        cli_out("\t\tType:\t%s\n",
                soc_cancun_file_type_string[ccf->type]);
        cli_out("\t\tFormat:\t%s\n",
                soc_cancun_file_format_string[ccf->format]);
        cli_out("\t\tLength:\t%dB\n", ccf->header.file_length * 4);
        cli_out("\t\tValid:\t%s\n", (ccf->valid)? "YES": "NO");
        cli_out("\t\tStatus:\t%s\n", soc_cancun_load_status_string[ccf->status]);
    }

    return SOC_E_NONE;
}

/*
 * Function:   _cmd_cancun_file_load
 * Purpose:    Load a CANCUN loadable file
 * Parameters:
 *             unit        - (IN) Device unit number
 *             filename    - (IN) Pointer to filename string
 *             file_type   - (IN) User input file type
 *             file_format - (IN) User input file format
 *             cc          - (OUT) Pointer to CANCUN control structure
 * Returns:
 */
int _cmd_cancun_file_load(int unit, char* filename, uint32 file_type,
                          uint32 file_format, soc_cancun_t *cc) {
    int rv = SOC_E_NONE;
#ifndef NO_FILEIO
    uint8 *file_buf = NULL;
    long file_len = 0;
    int filename_len;
    soc_cancun_file_t *ccf = NULL;

    /* filename length check */
    filename_len = sal_strlen(filename);
    if(filename_len > CANCUN_FILENAME_SIZE-1) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "input filename is too long (%d)\n"), filename_len));
        return SOC_E_PARAM;
    }

    /* open file and read into buffer */
    rv = _cancun_file_buffer_load(filename, &file_buf, &file_len);
    if(rv != SOC_E_NONE) {
        if(file_buf != NULL) {
            sal_free(file_buf);
        }
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "Fail to open/load file %s. (%s)\n"), filename, soc_errmsg(rv)));
        return (rv);
    }

    /* calling SOC API: soc_cancun_file_load */
    rv = soc_cancun_file_load(unit, file_buf, file_len, &file_type,
                              &file_format);
    if(rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "Fail to load file %s. (%s)\n"), filename, soc_errmsg(rv)));
        goto _end__cmd_cancun_file_load;
    }

    /* can't update filename inside API so we have to take it here */
    if(file_type == CANCUN_SOC_FILE_TYPE_CIH) {
        ccf = &cc->cih->file;
    } else if (file_type == CANCUN_SOC_FILE_TYPE_CMH){
        ccf = &cc->cmh->file;
    } else if (file_type == CANCUN_SOC_FILE_TYPE_CCH){
        ccf = &cc->cch->file;
    } else if (file_type == CANCUN_SOC_FILE_TYPE_CEH){
        ccf = &cc->ceh->file;
    } else if (file_type == CANCUN_SOC_FILE_TYPE_CFH){
        ccf = &cc->flow_db->file;
    } else if (file_type == CANCUN_SOC_FILE_TYPE_ISPF){
        ccf = &cc->ispf->file;
    }

    if(ccf) {
        sal_strlcpy(ccf->filename, filename, sizeof(ccf->filename));
    }

_end__cmd_cancun_file_load:
    if(file_buf) {
        sal_free(file_buf);
    }
#else
    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "No file system\n")));
    return (CMD_FAIL);
#endif /* NO_FILEIO */

    return (rv);
}

/*
 * Function:   _cancun_file_buffer_load
 * Purpose:    Allocate a buffer and load a CANCUN into it
 * Parameters:
 *             filename    - (IN) Pointer to filename string
 *             buf         - (OUT) Allocated buffer to hold file content
 *             buf_len     - (IN) Required buffer length in byte
 * Returns:
 */
int _cancun_file_buffer_load(char *filename, uint8 **buf, long *buf_len) {
    int rv = SOC_E_NONE;

#ifndef NO_FILEIO
    FILE *fp = NULL;
    fp = sal_fopen(filename, "rb");
    if (!fp) {
        rv = SOC_E_PARAM;
        goto _end__cancun_file_buffer_load;
    }

    if(sal_fseek(fp, 0L, SEEK_END) != 0) {
        rv = SOC_E_INTERNAL;
        goto _end__cancun_file_buffer_load;
    }
    *buf_len = ftell(fp);
    if(sal_fseek(fp, 0L, SEEK_SET) != 0) {
        rv = SOC_E_INTERNAL;
        goto _end__cancun_file_buffer_load;
    }

    *buf = (uint8*)sal_alloc(*buf_len, "soc_cancun_file_buf");
    if(*buf != NULL) {
        if(sal_fread(*buf, *buf_len, 1, fp) <= 0) {
            sal_free(buf);
            rv = SOC_E_INTERNAL;
            goto _end__cancun_file_buffer_load;
        }
    } else {
        rv = SOC_E_MEMORY;
    }

_end__cancun_file_buffer_load:
    if(fp) {
        sal_fclose(fp);
    }
#else
    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META("No file system\n")));
    return CMD_FAIL;
#endif /* NO_FILEIO */

    return rv;
}

/*
 * Function:   _cancun_sdk_version_string_get
 * Purpose:    Convert and format SDK release version from uint32 to string
 * Parameters:
 *             sdk_ver_string - (OUT) Pointer to version string, end by '\0'
 *             ver            - (IN) Input CANCUN version number
 * Returns:
 *
 * Note:
 *      Input version format: 0XAABBCCDD
 *      AA: SDK release MAJOR version
 *      BB: SDK release MINOR version
 *      CC: SDK release BUILD version
 *      DD: Reserved
 */
void _cancun_sdk_version_string_get(char* ver_string, uint32 ver) {
    char *p = ver_string;
    char buf_str[10];
    uint32 num;
    
    sal_memset(ver_string, 0, CANCUN_VERSION_LEN_MAX);
    
    num = (ver >> CANCUN_SDK_VERSION_OFFSET_MAJOR) & 0xFF;
    sal_itoa(buf_str, num, 10, 0, 2);
    sal_strcat(p, buf_str);
    sal_strcat(p, ".");
    
    num = (ver >> CANCUN_SDK_VERSION_OFFSET_MINOR) & 0xFF;
    sal_itoa(buf_str, num, 10, 0, 2);
    sal_strcat(p, buf_str);
    sal_strcat(p, ".");
    
    num = (ver >> CANCUN_SDK_VERSION_OFFSET_BUILD) & 0xFF;
    sal_itoa(buf_str, num, 10, 0, 2);
    sal_strcat(p, buf_str);
    
    sal_strcat(p, "\0");
}

/*
 * Function:   _cancun_version_string_get
 * Purpose:    Convert and format CANCUN version from uint32 to string
 * Parameters:
 *             ver_string  - (OUT) Pointer to version string, end by '\0'
 *             ver         - (IN) Input CANCUN version number
 * Returns:
 *
 * Note:
 *      Input version format: 0XAAEBCCDD
 *      AA : branch ID
 *      E  : revision
 *       B : major version
 *      CC:  minor version
 *      DD:  patch version
 */
void _cancun_version_string_get(char* ver_string, uint32 ver) {
    char *p = ver_string;
    char buf_str[10];
    uint32 num;

    sal_memset(ver_string, 0, CANCUN_VERSION_LEN_MAX);

    num = (ver >> CANCUN_VERSION_OFFSET_BRANCH_ID) & 0xF;
    if (num == CANCUN_FILE_BRANCH_ID_DEF) {
        /* disaply nothing for default */
    } else if (num == CANCUN_FILE_BRANCH_ID_HGoE) {
        sal_strcat(p, "HGoE.");
    }
    else if (num == CANCUN_FILE_BRANCH_ID_GSH) {
        sal_strcat(p, "GSH.");
    } else {
        sal_strcat(p, "UNKONWN.");
    }

    num = (ver >> CANCUN_VERSION_OFFSET_MAJOR) & 0xF;
    sal_itoa(buf_str, num, 10, 0, 2);
    sal_strcat(p, buf_str);
    sal_strcat(p, ".");

    num = (ver >> CANCUN_VERSION_OFFSET_MINOR) & 0xFF;
    sal_itoa(buf_str, num, 10, 0, 2);
    sal_strcat(p, buf_str);
    sal_strcat(p, ".");

    num = (ver >> CANCUN_VERSION_OFFSET_PATCH) & 0xFF;
    sal_itoa(buf_str, num, 10, 0, 2);
    sal_strcat(p, buf_str);

    sal_strcat(p, "\0");
}

/*
 * Function:   _cancun_revision_print
 * Purpose:    Print revision based on CANCUN version
 * Parameters:
 *             ver         - (IN) Input CANCUN version number
 * Returns:
 *
 *      Input version format: 0XAAEBCCDD
 *      AA : branch ID
 *      E  : revision
 *       B : major version
 *      CC:  minor version
 *      DD:  patch version

 */
void _cancun_revision_print(uint32 ver) {
    int revision =  (ver >> CANCUN_VERSION_OFFSET_REVISION) & 0xF;

    if (revision) {
        cli_out("\tRev: %d\n", revision);
    }
}

/*
 * Function:   _cancun_bug_fix_rev_print
 * Purpose:    Print CANCUN bug fix version
 * Parameters:
 *             unit         - (IN) unit number
 * Returns:
 */
STATIC void _cancun_bug_fix_rev_print(int unit) {
    uint32    entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    soc_field_t field;
    int bug_rev;
    int rv;

    mem = FLEX_EDITOR_ZONE_0_MATCH_ID_COMMAND_PROFILE_TABLEm;
    field = MHC_CHECKSUM_0_RANGEf;

    if (SOC_MEM_IS_VALID(unit, mem)) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, 127, entry);
        if (BCM_SUCCESS(rv) && soc_mem_field_valid(unit, mem, field)) { 
            bug_rev = soc_mem_field32_get(unit, mem, entry, field);
            if (bug_rev) {
                cli_out("\tBug fix rev: %d\n", bug_rev);
            }
        }
    } 
}

void _soc_cancun_default_filename(int unit, char *filename, int file_type) {
#ifndef NO_FILEIO
#define _SOC_CANCUN_DEFAULT_PATHS_NUM   (7)

    soc_cancun_t* cc;
    char *p = filename;
    int i, filepath_len;
    FILE *fp = NULL;
    char filepath[CANCUN_FILENAME_SIZE] = {0};
    char current_dir[CANCUN_FILENAME_SIZE] = {0};
    char paths[_SOC_CANCUN_DEFAULT_PATHS_NUM][80] = \
                {"../../rc/flex/", "../rc/flex/", "rc/flex/", "flex/",
                 "../../../rc/flex/","../../../../rc/flex/", ""};
    int chipname_len = 0;

    if(soc_cancun_status_get(unit, &cc) != SOC_E_NONE) {
        return;
    }

    sal_snprintf(p, CANCUN_FILENAME_SIZE-1, "%s_%s.pkg",
                 soc_chip_type_names[SOC_DRIVER(unit)->type],
                 soc_cancun_file_type_string[file_type]);
    do {
        *p = sal_tolower(*p);
    } while(*++p);

    filepath_len = sal_strlen(cc->default_path);
    chipname_len = sal_strlen(soc_chip_type_names[SOC_DRIVER(unit)->type]);
    if(filepath_len == 0) {
    /* searching default file path if not available */
        for(i = 0; i < _SOC_CANCUN_DEFAULT_PATHS_NUM; i++) {
            sal_snprintf(filepath, sizeof(filepath),
                         "%s%.*s/default/%s",
                         paths[i], chipname_len, filename, filename);
            fp = sal_fopen(filepath, "rb");
            if (fp) {
                sal_fclose(fp);
                sal_getcwd(current_dir, sizeof(current_dir));
                sal_snprintf(cc->default_path, sizeof(cc->default_path),
                             "%s%s%.*s/default/",
                             current_dir, paths[i], chipname_len, filename);
                break;
            }
        }
    } else {
        p = cc->default_path + filepath_len - 1;
        if(*p != '/') {
            sal_strcat(cc->default_path, "/");
        }
    }

    sal_snprintf(filepath, sizeof(filepath), "%s%s",
                 cc->default_path, filename );
    sal_strlcpy(filename, filepath, CANCUN_FILENAME_SIZE-1);

    return;
#else
    LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "No file system\n")));
#endif /* NO_FILEIO */
}

int _soc_cancun_dest_entry_print(soc_cancun_dest_entry_t *dest_entry) {

    int i, dest_mem;

    if(dest_entry == NULL) return SOC_E_INTERNAL;

    cli_out("CANCUN destination entry:\n");
    cli_out("\tIndex #: %d\n", dest_entry->dest_index_num);
    cli_out("\tField #: %d\n", dest_entry->dest_field_num);

    if(dest_entry->dest_index_num > 0) {
        cli_out("\tDestination Table Indices:\n");
        for(i = 0; i < dest_entry->dest_index_num; i++) {
            dest_mem = dest_entry->dest_mems[i];
            cli_out("\t\t%s: %d\n",
                    (dest_mem >= NUM_SOC_MEM)? SOC_REG_NAME(unit, dest_mem):
                                               SOC_MEM_NAME(unit, dest_mem),
                    dest_entry->dest_values[i]);
        }
    } else {
        cli_out("\tDestination Table, Field Values:\n");
        dest_mem = dest_entry->dest_mems[0];
        cli_out("\t\t%s\n",
                (dest_mem >= NUM_SOC_MEM)? SOC_REG_NAME(unit, dest_mem):
                                           SOC_MEM_NAME(unit, dest_mem));
        for(i = 0; i < dest_entry->dest_field_num; i++) {
            cli_out("\t\t |- %s = %d\n",
                    SOC_FIELD_NAME(unit, dest_entry->dest_fields[0]),
                    dest_entry->dest_values[i]);
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:   soc_cancun_generic_load
 * Purpose:    Load cancun based on type
 * Parameters:
 *             unit        - (IN) Device unit number
 *             cancun_type   - (IN)cancun_file_type
 * Returns:
 */
int soc_cancun_generic_load(int unit, uint32 cancun_type) {
    int rv = SOC_E_NONE;

    if ((cancun_type < CANCUN_SOC_FILE_TYPE_CIH) ||
        (cancun_type > CANCUN_SOC_FILE_TYPE_ISPF)) {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
            "Unsupported cancun type\n")));
        return SOC_E_PARAM;
    }

#ifndef NO_FILEIO
    {
        char filename[CANCUN_FILENAME_SIZE] = {0};
        uint32 file_format = CANCUN_SOC_FILE_FORMAT_UNKNOWN;

        soc_cancun_t* cc;
        rv = soc_cancun_status_get(unit, &cc);
        if(rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit,
                "can't get CANCUN status. (%s)\n"), soc_errmsg(rv)));
            return rv;
        }

        _soc_cancun_default_filename(unit, filename, cancun_type);

        rv = _cmd_cancun_file_load(unit, filename, cancun_type, file_format, cc);
    }
#else
    {
        LOG_ERROR(BSL_LS_SOC_CANCUN, (BSL_META_U(unit, "No file system\n")));

        return SOC_E_UNAVAIL;
    }
#endif

    return rv;
}

#endif /* NO_SAL_APPL */
