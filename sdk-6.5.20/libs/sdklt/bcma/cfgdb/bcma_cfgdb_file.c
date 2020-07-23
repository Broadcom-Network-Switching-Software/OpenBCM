/*! \file bcma_cfgdb_file.c
 *
 * Configuration data manager for file I/O operation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/io/bcma_io_file.h>

#include <bcma/cfgdb/bcma_cfgdb.h>

#include "cfgdb_internal.h"

/*
 * Utility function to strip the trailing newline or whitespaces of string.
 */
static void
str_strip_trailing_whitespace(char *str)
{
    char *ptr;

    /* Strip trailing newline/blanks */
    ptr = str + sal_strlen(str) - 1;
    while (ptr >= str && sal_isspace(*ptr)) {
        ptr--;
    };
    *(ptr + 1) = '\0';
}

/*
 * Parsing the token of name before  '=' on string 's' and
 * the token of value after '='. String 's' would be split into 2 strings
 * if '=' is found. 'name' and 'value' would be pointed to the tokens
 * before and after '=' respectively if it's not NULL.
 * Return FALSE if no '=' is found on string 's'.
 */
static int
parse_eq_string(char *s, char **name, char **value)
{
    char *ptr;

    if ((ptr = sal_strchr(s, '=')) == NULL) {
        return FALSE;
    }

    *ptr = '\0';

    /* Get the token after '=' */
    if (value) {
        ptr++;

        /* Remove spaces after '=' */
        while (sal_isspace(*ptr)) {
            ptr++;
        }
        *value = ptr;

        while (*ptr != '\0' && !sal_isspace(*ptr)) {
            ptr++;
        }
        *ptr = '\0';
    }

    /* Get the token before '=' */
    if (name) {
        int len;

        /* Remove the whitespaces before '=' */
        str_strip_trailing_whitespace(s);

        len = sal_strlen(s);
        if (len == 0) {
            ptr = s;
        } else {
            ptr = s + len - 1;

            /* Find a valid token for var name */
            do {
                if (*ptr == '#' || sal_isspace(*ptr)) {
                    ptr++;
                    break;
                }
                if (ptr == s) {
                    break;
                }
                ptr--;
            } while (1);
        }

        *name = ptr;
    }

    return TRUE;
}

/*
 * Cookie for bcma_cfgdb_var_traverse call-back function var_file_write.
 */
struct trvs_file_wr {

    /*! File handle to write to */
    bcma_io_file_handle_t fh;

    /*! Wildcard string(string including '*') to be compared */
    const char *wildcard_str;
};

/*
 * Call-back function of bcma_cfgdb_var_traverse to write variable to file if
 * variable name matches wildcard string.
 */
static int
var_file_write(void *cookie, const char *name, const char *value,
               uint32_t *tag)
{
    struct trvs_file_wr *wr = (struct trvs_file_wr *)cookie;
    bcma_io_file_handle_t fh = wr->fh;
    const char *wc_str = wr->wildcard_str;

    COMPILER_REFERENCE(tag);

    if (bcma_cfgdb_wildcard_match(name, wc_str)) {
        bcma_io_file_printf(fh, "%s=%s\n", name, value);
    }

    return 0;
}

/*
 * Cookie for bcma_cfgdb_var_traverse call-back function var_fline_write.
 */
struct trvs_fline_wr {

    /*! File handle to write to */
    bcma_io_file_handle_t fh;

    /*! Line string read from file that variables loaded from */
    char *buf;

    /*! Candidate variable name in line string */
    char *cand_name;

    /*! Flag to indicate if line string has been written out */
    int done;
};

/*
 * Call-back function of bcma_cfgdb_var_traverse to check variables read from
 * config file is a valid variable or not then flush the line to file.
 */
static int
var_fline_write(void *cookie, const char *name, const char *value,
                uint32_t *tag)
{
    struct trvs_fline_wr *fwr = (struct trvs_fline_wr *)cookie;
    bcma_io_file_handle_t fh = fwr->fh;
    char *buf = fwr->buf;
    char *cand_name = fwr->cand_name;

    /* Stop traversing if the buf has been written out. */
    if (fwr->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(name, cand_name) == 0) {
        if ((*tag & BCMA_CFGDB_TAG_FLAG_FLUSHED) == 0) {
            /* Write out the variable value to file. */
            bcma_io_file_printf(fh, "%s=%s\n", name, value);

            /* Mark the variable as flushed. */
            *tag |= BCMA_CFGDB_TAG_FLAG_FLUSHED;
        } else {
            /*
             * If variable has been written to file in previous line,
             * make sure the line is commented.
             */
            if (buf[0] != '#') {
                bcma_io_file_printf(fh, "#");
            }
            bcma_io_file_printf(fh, "%s", buf);
        }
        fwr->done = TRUE;
    }

    return 0;
}

/*
 * Cookie for bcma_cfgdb_var_traverse call-back function var_tag_import_set/get.
 */
struct trvs_tag_import_info {

    /*! Variable name */
    const char *name;

    /*! File line number where variable is loaded from */
    int line_num;

    /*! Flag to indicate if the variable line number has been set */
    int done;
};

/*
 * Call-back function of bcma_cfgdb_var_traverse to get import information
 * of variable loaded from file.
 */
static int
var_tag_import_get(void *cookie, const char *name, const char *value,
                   uint32_t *tag)
{
    struct trvs_tag_import_info *ti = (struct trvs_tag_import_info *)cookie;

    COMPILER_REFERENCE(value);

    /* Stop traversing if the line number has been written. */
    if (ti->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(ti->name, name) == 0) {
        /* Get loaded line number from variable tag. */
        ti->line_num = BCMA_CFGDB_TAG_LINE_NUM_GET(*tag);

        ti->done = TRUE;
    }

    return 0;
}

/*
 * Call-back function of bcma_cfgdb_var_traverse to write import information
 * of variable loaded from file to variable tag.
 */
static int
var_tag_import_set(void *cookie, const char *name, const char *value,
                   uint32_t *tag)
{
    struct trvs_tag_import_info *ti = (struct trvs_tag_import_info *)cookie;

    COMPILER_REFERENCE(value);

    /* Stop traversing if the line number has been written. */
    if (ti->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(ti->name, name) == 0) {
        /* Update variable tag with line number */
        BCMA_CFGDB_TAG_LINE_NUM_SET(*tag, ti->line_num);

        /* Mark with imported flag */
        *tag |= BCMA_CFGDB_TAG_FLAG_IMPORTED;

        ti->done = TRUE;
    }

    return 0;
}

/*
 * Call-back function of bcma_cfgdb_var_traverse to write unflushed variables to file
 */
static int
var_unflushed_file_write(void *cookie, const char *name, const char *value,
                         uint32_t *tag)
{
    bcma_io_file_handle_t fh = (bcma_io_file_handle_t)cookie;

    /* Write unflushed variable to file */
    if ((*tag & BCMA_CFGDB_TAG_FLAG_FLUSHED) == 0) {
        bcma_io_file_printf(fh, "%s=%s\n", name, value);
        *tag |= BCMA_CFGDB_TAG_FLAG_FLUSHED;
    }

    return 0;
}

int
bcma_cfgdb_load(void)
{
    bcma_io_file_handle_t fh;
    int line = 0;
    char buf[256], *name, *value;
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return -1;
    }

    if (cfgdb->fname == NULL) {
        return 0;
    }

    fh = bcma_io_file_open(cfgdb->fname, "r");
    if (fh == NULL) {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META("Fail to open file %s.\n"), cfgdb->fname));
        return 0;
    }

    while (bcma_io_file_gets(fh, buf, sizeof(buf))) {
        line++;

        /* Skip comment lines */
        if (buf[0] == '#') {
            continue;
        }

        /* Strip trailing newline/whitespaces */
        str_strip_trailing_whitespace(buf);

        /* Skip blank lines */
        if (buf[0] == 0) {
            continue;
        }

        /* Check if line string contains '=' and get name and value tokens. */
        if (parse_eq_string(buf, &name, &value)) {
            struct trvs_tag_import_info ti;

            if (name[0] == '\0' || value[0] == '\0') {
                LOG_WARN(BSL_LS_APPL_SHELL,
                         (BSL_META("%s(%d): format error (ignored).\n"),
                          cfgdb->fname, line));
                continue;
            }

            /* Check if variable has been loaded. */
            if (bcma_cfgdb_var_get(name) != NULL) {
                ti.name = name;
                ti.line_num = 0;
                ti.done = FALSE;

                bcma_cfgdb_var_traverse(var_tag_import_get, &ti);

                if (ti.line_num != 0) {
                    LOG_WARN(BSL_LS_APPL_SHELL,
                             (BSL_META("%s(%d): ignoring duplicate entry "
                                       "\"%s\" (first defined at line %d)\n"),
                              cfgdb->fname, line, name, ti.line_num));
                    continue;
                }
            }

            /* Set <name, value> to cfgdb vairable. */
            if (bcma_cfgdb_var_set(name, value) == 0) {
                ti.name = name;
                ti.line_num = line;
                ti.done = FALSE;

                /* Store file import information to variable tag */
                bcma_cfgdb_var_traverse(var_tag_import_set, &ti);
            }
        }
    }

    bcma_io_file_close(fh);

    return 0;
}

int
bcma_cfgdb_save(const char *filename, const char *wildcard_str, int append)
{
    bcma_io_file_handle_t fh;
    struct trvs_file_wr fwr;

    if (filename == NULL || sal_strlen(filename) == 0) {
        return -1;
    }

    if (append) {
        fh = bcma_io_file_open(filename, "a");
    } else {
        fh = bcma_io_file_open(filename, "w");
    }

    if (fh == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Fail to open file %s to save.\n"),
                   filename));
        return -1;
    }

    fwr.fh = fh;
    fwr.wildcard_str = wildcard_str;

    /* Write variables to file if variable name matches wildcard string. */
    bcma_cfgdb_var_traverse(var_file_write, &fwr);

    bcma_io_file_close(fh);

    return 0;
}

int
bcma_cfgdb_flush(void)
{
    bcma_io_file_handle_t fh;
    bcma_io_file_handle_t tmp_fh;
    int rv = 0;
    char buf[256], tmp_buf[256];
    char *name;
    struct trvs_tag_flag_op tfop;
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return -1;
    }

    if (cfgdb->fname == NULL || cfgdb->tmp_fname == NULL) {
        return 0;
    }

    /* Open the configuration file to read from. */
    fh = bcma_io_file_open(cfgdb->fname, "r");

    /* Open the temporary file to write to. */
    tmp_fh = bcma_io_file_open(cfgdb->tmp_fname, "w");
    if (tmp_fh == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META("Fail to open file %s to write.\n"),
                   cfgdb->tmp_fname));
        bcma_io_file_close(fh);
        return -1;
    }

    tfop.name = NULL;
    tfop.flags = BCMA_CFGDB_TAG_FLAG_FLUSHED;

    /* Clear flushed flag for all variables */
    bcma_cfgdb_var_traverse(bcma_cfgdb_var_tag_flags_clear, &tfop);

    /* Process per line string from config file and write to temporary file. */
    while (fh && bcma_io_file_gets(fh, buf, sizeof(buf))) {
        sal_strcpy(tmp_buf, buf);

        /* Check if line string contains '=' and get name token. */
        if (parse_eq_string(tmp_buf, &name, NULL) == FALSE) {
            /* Leave the lines without '=' remain unchanged. */
            bcma_io_file_printf(tmp_fh, "%s", buf);
        } else {
            struct trvs_fline_wr flw;

            flw.fh = tmp_fh;
            flw.buf = buf;
            flw.cand_name = name;
            flw.done = FALSE;

            /*
             * Check if the name is a valid variable and write line string to
             * temporary file.
             */
            bcma_cfgdb_var_traverse(var_fline_write, &flw);

            /*
             * If name from config file is not a valid variable name, make sure
             * the line is commented.mark the line as commented.
             */
            if (!flw.done) {
                if (buf[0] != '#') {
                    bcma_io_file_printf(tmp_fh, "#");
                }
                bcma_io_file_printf(tmp_fh, "%s", buf);
            }
        }
    }

    /* Append the unflushed variables to the end of temporary file */
    bcma_cfgdb_var_traverse(var_unflushed_file_write, tmp_fh);

    if (fh) {
        bcma_io_file_close(fh);
    }
    bcma_io_file_close(tmp_fh);

    rv = bcma_io_file_move(cfgdb->tmp_fname, cfgdb->fname);

    /* Remove the temporary file if rename is fail. */
    if (rv != 0) {
        rv = bcma_io_file_delete(cfgdb->tmp_fname);
    }

    return rv;
}
