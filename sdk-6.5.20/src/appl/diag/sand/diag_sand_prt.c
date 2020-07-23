/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    diag_sand_prt.c
 * Purpose:    Beautified cli output routine
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <soc/drv.h>
#include <soc/sand/sand_aux_access.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include "diag_sand_framework_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#define prt_printf  cli_out

#define     PRT_COLUMN_CONTROL_ALL      0
#define     PRT_COLUMN_CONTROL_NOSAME   1
#define     PRT_COLUMN_CONTROL_NOZERO   2
#define     PRT_COLUMN_CONTROL_FILTERED 3

/*
 * Maximum number of tokens to filter rows in PRT tables having these strings as part of the content
 * It does seem that we'll need more than that - but it can be enlarged
 */
#define     PRT_CELL_FILTER_MAX         16

int
diag_sand_get_shift(
    char *str)
{
    int i_shift;
    for (i_shift = 0; str[i_shift] == ' '; i_shift++)
    {
    }
    /*
     * Each shift has 2 spaces
     */
    return i_shift / 2;
}

shr_error_e
diag_sand_prt_column_add(
    prt_control_t * prt_ctr,
    int flag,
    int width,
    prt_column_t ** prt_column_p)
{
    prt_column_t *prt_column;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    if (prt_ctr == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Title was not defined\n");
    }
    if (prt_ctr->col_num >= PRT_COLUMN_MAX_NUM)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Number of columns exceeded maximum:%d\n", PRT_COLUMN_MAX_NUM);
    }
    if ((prt_ctr->columns[prt_ctr->col_num] = sal_alloc(sizeof(prt_control_t), "prt_column")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "No memory for PRT title\n");
    }
    prt_column = prt_ctr->columns[prt_ctr->col_num];

    sal_memset(prt_column, 0, sizeof(prt_column_t));

    prt_column->cell_offset = prt_ctr->row_width;
    prt_column->max_width = width;
    prt_column->flex_type = flag;
    prt_ctr->row_width += prt_column->max_width;
    prt_ctr->col_num++;

    *prt_column_p = prt_column;
exit:
    SHR_FUNC_EXIT;
}

void
diag_sand_prt_char(
    int num,
    char ch)
{
    while (num--)
        prt_printf("%c", ch);
    return;
}

static void
diag_sand_prt_row(
    int num,
    char ch)
{
    while (num--)
        prt_printf("%c", ch);
    prt_printf("\n");
    return;
}

static int
prt_token_max_size(
    char *prt_str)
{
    int prt_size = sal_strlen(prt_str);
    int i_ch, max_size = 0, cur_size = 0;

    for (i_ch = 0; i_ch < prt_size; i_ch++)
    {
        /*
         * Take next row on new line
         */
        cur_size++;
        if (prt_str[i_ch] == '\n')
        {
            if (cur_size > max_size)
                max_size = cur_size;
            cur_size = 0;
        }
    }

    if (max_size == 0)
        max_size = prt_size;

    return max_size;
}

static int
prt_token_has_eol(
    char *prt_str)
{
    int prt_size = sal_strlen(prt_str);
    int i_ch;

    for (i_ch = 0; i_ch < prt_size; i_ch++)
    {
        if (prt_str[i_ch] == '\n')
            return TRUE;
    }

    return FALSE;
}

static int
prt_get_token_size(
    char *prt_str,
    int column_width,
    int *next_shift_p)
{
    int token_size = -1, prt_size = sal_strlen(prt_str);
    int i_ch;
    int next_shift = 0;

    for (i_ch = 0; i_ch < prt_size; i_ch++)
    {
        /*
         * Take next row on new line or tab
         */
        if ((prt_str[i_ch] == '\n') || (prt_str[i_ch] == '\t'))
        {
            /*
             * If it is still inside MAX column size, take this offset as last, otherwise use previous and leave \n for
             * next round
             */
            if (i_ch < column_width)
            {
                token_size = i_ch;
                prt_str[i_ch] = ' ';
                next_shift = 1;
            }
            break;
        }

        if (prt_str[i_ch] == ' ')
        {
            if (i_ch <= column_width)
            {   /* Keep last character that we may break the line on it */
                token_size = i_ch;
                next_shift = 1;
            }
            else
            {
                break;
            }
        }
        if ((prt_str[i_ch] == ',') || (prt_str[i_ch] == '.') || (prt_str[i_ch] == '_'))
        {
            if (i_ch <= column_width)
            {   /* Keep last character that we may break the line on it */
                token_size = i_ch + 1;
                next_shift = 0;
            }
            else
            {
                break;
            }
        }
    }

    if (token_size == -1)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("Single token is longer than:%d\n"), column_width));
        token_size = column_width;
    }
    if (next_shift_p != NULL)
    {
        *next_shift_p = next_shift;
    }
    return token_size;
}

static int
prt_get_token_max_size(
    char *prt_str)
{
    int token_size = 0, prt_size = sal_strlen(prt_str);
    int i_ch, i_tok = 0;

    for (i_ch = 0; i_ch < prt_size; i_ch++)
    {
        if ((prt_str[i_ch] == ' ') || (prt_str[i_ch] == '\t') || (prt_str[i_ch] == '\n'))
        {
            /*
             * If the number of regular characters more than current token size update token_size
             * Last empty character will be extinguished
             */
            token_size = i_tok > token_size ? i_tok : token_size;
            /*
             * And start the count from the beginning
             */
            i_tok = 0;
        }
        else if ((prt_str[i_ch] == ',') || (prt_str[i_ch] == '.') || (prt_str[i_ch] == '_'))
        {
            /*
             * If the number of regular characters more than current token size update token_size
             * Do not extinguish last character
             */
            token_size = (i_tok + 1) > token_size ? (i_tok + 1) : token_size;
            /*
             * And start the count from the beginning
             */
            i_tok = 0;
        }
        else
        {
            i_tok++;
        }
    }
    /*
     * Take last token as well
     */
    token_size = i_tok > token_size ? i_tok : token_size;

    if (token_size == 0)
    {
        /*
         * Special characters not found, string is one token
         */
        token_size = prt_size;
    }
    return token_size;
}

static int
prt_token_is_zero(
    char *prt_str)
{
    int prt_size = sal_strlen(prt_str);
    int i_ch;

    for (i_ch = 0; i_ch < prt_size; i_ch++)
    {
        switch (prt_str[i_ch])
        {
            case ' ':
            case '\n':
            case '\t':
            case '0':
            case ':':
            case '.':
            case ',':
                break;
            case 'x':
            case 'X':
                if ((i_ch == 0) || (prt_str[i_ch] != '0'))
                {
                    return FALSE;
                }
                break;
            default:
                return FALSE;
                break;
        }
    }
    return TRUE;
}

#if !defined(NO_FILEIO)

shr_error_e
diag_sand_prt_gold_sanitize(
    int unit,
    int flags,
    char *command,
    char *filename)
{
    xml_node top_node = NULL, cmd_node, cur_node;
    int cmd_id;

    char output_file[RHFILE_MAX_SIZE];
    char *sand_name;
    char **cmd_tokens = NULL;
    uint32 cmd_num = 0;

    SHR_FUNC_INIT_VARS(unit);

    sand_name = shr_access_get_sand_name(unit);

    SHR_CLI_EXIT_IF_ERR(sh_sand_gold_path(unit, flags, filename, output_file),
                        "Gold result file:'%s' cannot be obtained\n", filename);

    if ((top_node = dbx_file_get_xml_top(unit, output_file, sand_name, CONF_OPEN_CURRENT_LOC)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "failed to open:%s\n", output_file);
    }

    /*
     * Obtain the array of command to navigate in XML structure
     */
    if ((cmd_tokens = utilex_str_split(command, " ", SH_SAND_MAX_COMMAND_DEPTH, &cmd_num)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Problem parsing input command:%s for XML gold sanitize\n", command);
    }

    cmd_node = top_node;

    for (cmd_id = 0; cmd_id < cmd_num; cmd_id++)
    {
        /*
         * If first command is equal to sand_name, skip it on node creation, was created above
         */
        if ((cmd_id == 0) && !sal_strcasecmp(cmd_tokens[cmd_id], sand_name))
            continue;
        /*
         * If this is last keyword in command there should be timestamp
         */
        if (cmd_id == (cmd_num - 1))
        {
            /*
             * Go through all appearances of command and remove timestamp property
             */
            RHDATA_ITERATOR(cur_node, cmd_node, cmd_tokens[cmd_id])
            {
                RHDATA_DELETE(cur_node, "timestamp");
            }
        }
        else
        {       /* For not last command just check such node exists */
            if ((cmd_node = dbx_xml_child_get_first(cmd_node, cmd_tokens[cmd_id])) == NULL)
            {   /* Not supposed to happen - just for sanity */
                break;
            }
        }
    }

    SHR_IF_ERR_EXIT(dbx_file_save_xml(unit, output_file, top_node, CONF_OPEN_CURRENT_LOC));

exit:
    if (cmd_tokens != NULL)
    {
        utilex_str_split_free(cmd_tokens, cmd_num);
    }
    dbx_xml_top_close(top_node);
    SHR_FUNC_EXIT;
}

static char *
diag_sand_which_CPU(
    void)
{
#if defined(__DUNE_GTO_BCM_CPU__)
    return "GTO";
#elif defined(__DUNE_WRX_BCM_CPU__)
    return "WRX";
#elif defined(__DUNE_SLK_BCM_CPU__)
    return "SLK";
#elif defined(__DUNE_GTS_BCM_CPU__)
    return "GTS";
#else
    return "not recognize";
#endif
}

static shr_error_e
diag_sand_prt_xml(
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control,
    char *filename)
{
    xml_node top_node, cmd_node, table_node;
    int k, k_cl;
    rhhandle_t prt_row_start;
    char *prt_row_offset;
    void *parent_node;
    int parent_id = PRT_TITLE_ID;
    int shift_n;
    prt_info_t *info_entry;
    int depth_base = 0;
    char tmp_str[PRT_COLUMN_WIDTH];
    char *sand_name;
    char **cmd_tokens = NULL;
    uint32 cmd_num = 0;
    char **xpath_tokens = NULL;
    uint32 xpath_parts_nof;
    int file_flags;
    int enable;
    char output_file[RHFILE_MAX_SIZE];
    int _flags = 0;
    prt_column_t *prt_column;

    SHR_FUNC_INIT_VARS(prt_ctr->unit);

    /** Verify and/or try to fix columns names - should be valid as XML object names */
    PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
    {
        if (ISEMPTY(prt_column->name))
        {
            sal_snprintf(prt_column->name, PRT_COLUMN_WIDTH - 1, "column_%d", k);
        }
        else
        {
            int k_cur;
            prt_column_t *prt_column_cur;
            /** replace all illegal for XML characters by '_' */
            utilex_str_escape_xml(prt_column->name, '_');
            /** Check that name is not pure number */
            if (utilex_str_is_number(prt_column->name) == TRUE)
            {
                /** if it is add column_ prefix */
                sal_snprintf(tmp_str, PRT_COLUMN_WIDTH - 1, "column_%s", prt_column->name);
                sal_strncpy(prt_column->name, tmp_str, PRT_COLUMN_WIDTH - 1);
            }
            /** Check if there are 2 identical names and append column number to avoid same XML object names*/
            for (k_cur = k + 1; k_cur < prt_ctr->col_num; k_cur++)
            {
                prt_column_cur = prt_ctr->columns[k_cur];
                if (!sal_strcasecmp(prt_column->name, prt_column_cur->name))
                {
                    /** If the same name found - modify all that are similar to this one, leaving the 1st one as is */
                    sal_snprintf(tmp_str, PRT_COLUMN_WIDTH - 1, "%s_%d", prt_column->name, k_cur);
                    sal_strncpy(prt_column_cur->name, tmp_str, PRT_COLUMN_WIDTH - 1);
                }
            }
        }
    }

    /*
     * Check if advanced table creation was used
     */
    if ((prt_ctr->columns[0] != NULL) && (prt_ctr->columns[0]->parent_id == 0))
    {   /* First column having parent_id 0 is signal than no advanced XMl formatting was used */
        PRT_XML_TYPE xml_type = PRT_XML_CHILD;
        /*
         * Always assign title id to first column parent and make it child
         */
        PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
        {
            prt_column->parent_id = parent_id;
            prt_column->type = xml_type;
            if (xml_type == PRT_XML_ATTRIBUTE)
            {   /* From this column and forward all columns will be attribute of the first column without empty cells */
                continue;
            }
            RHITERATOR(prt_row_start, prt_ctr->list)
            {
                prt_row_offset = ((char *) prt_row_start) + sizeof(rhentry_t) + prt_column->cell_offset;
                if (ISEMPTY(prt_row_offset))
                {
                    break;
                }
            }
            /*
             *  If all rows have non empty strings on specific column then next column may be its attribute
             *  If there are holes, next column will be child
             */
            parent_id = k;
            if (prt_row_start == NULL)
            {   /* End of list reached, no empty cells discovered, make all next columns attributes */
                xml_type = PRT_XML_ATTRIBUTE;
            }
            else
            {   /* empty cells were found, so next column will be child and not attribute */
                xml_type = PRT_XML_CHILD;
            }
        }
    }

    sand_name = shr_access_get_sand_name(prt_ctr->unit);

    SH_SAND_GET_BOOL("new", enable);
    /*
     * Always create and find the absolute path when writing prt output
     */
    file_flags = CONF_OPEN_CREATE | CONF_OPEN_CURRENT_LOC;
    if (enable == TRUE)
    {
        file_flags |= CONF_OPEN_OVERWRITE;
    }
    SH_SAND_GET_BOOL("gold", enable);
    if (enable == FALSE)
    {
        sal_strncpy(output_file, filename, RHFILE_MAX_SIZE - 1);
    }
    else
    {
        SH_SAND_IS_PRESENT("custom_verify", enable);
        if (enable == TRUE)
        {
            _flags |= SH_CMD_CUSTOM_XML_VERIFY;
        }
        SHR_CLI_EXIT_IF_ERR(sh_sand_gold_path(prt_ctr->unit, _flags, filename, output_file),
                            "Gold result file:'%s' cannot be obtained\n", filename);
    }
    if ((top_node = dbx_file_get_xml_top(prt_ctr->unit, output_file, sand_name, file_flags)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "failed to create:%s\n", output_file);
    }

    /*
     * Check if device was already recorded on XML top node, if not record all params
     */
    if ((dbx_xml_property_get_str
         (top_node, "device__not_compared_to_gold_only_for_debugging", tmp_str, RHNAME_MAX_SIZE)) != _SHR_E_NONE)
    {
        RHDATA_SET_INT(top_node, "unit", prt_ctr->unit);
        RHDATA_SET_STR(top_node, "device__not_compared_to_gold_only_for_debugging",
                       (char *) soc_dev_name(prt_ctr->unit));
        RHDATA_SET_STR(top_node, "family", SOC_CHIP_STRING(prt_ctr->unit));
        RHDATA_SET_STR(top_node, "CPU__not_compared_to_gold_only_for_debugging", diag_sand_which_CPU());
#ifdef __OPTIMIZE__
        RHDATA_SET_STR(top_node, "Opt__not_compared_to_gold_only_for_debugging", "1");
#else /* __OPTIMIZE__ */
        RHDATA_SET_STR(top_node, "Opt__not_compared_to_gold_only_for_debugging", "0");
#endif /* __OPTIMIZE__ */
    }
    else
    {
        if (sal_strcasecmp(tmp_str, (char *) soc_dev_name(prt_ctr->unit)))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "File was created for:%s this one is:%s\n", tmp_str,
                         (char *) soc_dev_name(prt_ctr->unit));
        }
    }

    /*
     *  Header comes from special ID
     */
    if ((sand_control != NULL) && (sand_control->sh_sand_cmd != NULL))
    {
        void *cur_node;
        int cmd_id;
        char *xpath = NULL;

        cmd_node = top_node;

        SH_SAND_GET_STR("xpath", xpath);
        if (!ISEMPTY(xpath))
        {
            /*
             * If xpath was supplied to record command output under, search the xpath. If it already
             * exists, use it. Else, create it.
             */
            if ((xpath_tokens = utilex_str_split(xpath, "/", SH_SAND_MAX_COMMAND_DEPTH, &xpath_parts_nof)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "Problem parsing input xpath for XML output\n");
            }
            for (cmd_id = 0; cmd_id < xpath_parts_nof; cmd_id++)
            {
                /*
                 * If first part is equal to sand_name, skip it on node creation, was created above
                 */
                if ((cmd_id == 0) && !sal_strncasecmp(xpath_tokens[cmd_id], sand_name, RHNAME_MAX_SIZE))
                    continue;
                /*
                 * Always look for existing node for the path
                 */
                depth_base++;
                /*
                 * check if node exists
                 */
                cur_node = dbx_xml_child_get_first(cmd_node, xpath_tokens[cmd_id]);
                /*
                 * Create node if none was found
                 */
                if (cur_node == NULL)
                {
                    if ((cur_node = dbx_xml_child_add(cmd_node, xpath_tokens[cmd_id], depth_base)) == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add:%s\n", xpath_tokens[cmd_id]);
                    }
                }
                cmd_node = cur_node;
            }
        }
        /*
         * Now add the command structure under the xpath final node.
         */
        if ((cmd_tokens =
             utilex_str_split(sand_control->sh_sand_cmd->cmd_only, " ", SH_SAND_MAX_COMMAND_DEPTH, &cmd_num)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Problem parsing input command for XML output\n");
        }

        for (cmd_id = 0; cmd_id < cmd_num; cmd_id++)
        {
            /*
             * If first command is equal to sand_name, skip it on node creation, was created above
             */
            if ((cmd_id == 0) && !sal_strcasecmp(cmd_tokens[cmd_id], sand_name))
                continue;
            /*
             * Always look for existing node of it is not last Create new node for last command in the list to
             * differentiate multiple running of the same command
             */
            depth_base++;
            /*
             * If this is last keyword in command look for matching timestamp
             */
            if (cmd_id == (cmd_num - 1))
            {
                char *options_str;
                /*
                 * Replace local by dynamic to decrease stack usage
                 */
                if ((options_str = sal_alloc(SH_SAND_MAX_TOKEN_SIZE, "OptionStr")) == NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate to read options on:%s\n", cmd_tokens[cmd_id]);
                }
                RHDATA_ITERATOR(cur_node, cmd_node, cmd_tokens[cmd_id])
                {
                    RHDATA_GET_STR_CONT_EXT(cur_node, "timestamp", tmp_str, SH_SAND_MAX_TIME_SIZE);
                    RHDATA_GET_STR_CONT_EXT(cur_node, "options", options_str, SH_SAND_MAX_TOKEN_SIZE);
                    if (!sal_strcasecmp(sand_control->time_str, tmp_str)
                        && !sal_strcasecmp(sand_control->options_str, options_str))
                        break;
                }
                sal_free(options_str);
            }
            else
            {   /* For not last command just check such node exists */
                cur_node = dbx_xml_child_get_first(cmd_node, cmd_tokens[cmd_id]);
            }
            /*
             * Create node if none was found
             */
            if (cur_node == NULL)
            {
                if ((cur_node = dbx_xml_child_add(cmd_node, cmd_tokens[cmd_id], depth_base)) == NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add:%s\n", cmd_tokens[cmd_id]);
                }
                if (cmd_id == (cmd_num - 1))
                {
                    RHDATA_SET_STR(cur_node, "options", sand_control->options_str);
                    RHDATA_SET_STR(cur_node, "timestamp", sand_control->time_str);
                }
            }
            cmd_node = cur_node;
        }
    }
    else
    {
        depth_base++;
        if ((cmd_node = dbx_xml_child_add(top_node, "command", depth_base)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add:%s\n", prt_ctr->title);
        }
    }
    /*
     * Increase indent for table node to come
     */
    depth_base++;
    /*
     * Create node of table inside leaf, there may be multiple tables inside the leaf
     */
    if ((table_node = dbx_xml_child_add(cmd_node, "table", depth_base)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add table node\n");
    }
    /*
     * Increase indent for content group nodes(infos, headers, entries) node to come
     */
    depth_base++;

    RHDATA_SET_STR(table_node, "title", prt_ctr->title);

    /*
     * Add Info List to table node
     */
    {
        void *infos_node;
        if ((infos_node = dbx_xml_child_add(table_node, "infos", depth_base)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add infos node to:%s\n", prt_ctr->title);
        }
        RHITERATOR(info_entry, prt_ctr->info_list)
        {
            void *info_node;
            if ((info_node = dbx_xml_child_add(infos_node, "info", depth_base + 1)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add info node to:%s\n", prt_ctr->title);
            }
            RHDATA_SET_STR(info_node, "text", info_entry->content);
        }
        dbx_xml_node_end(infos_node, depth_base);
    }
    /*
     * Fill Header Information as Headers Node as list
     */
    {
        void *headers_node, *node;
        if ((headers_node = dbx_xml_child_add(table_node, "headers", depth_base)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add headers node to:%s\n", prt_ctr->title);
        }
        PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
        {
            if ((node = dbx_xml_child_add(headers_node, "header", depth_base + 1)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add header node\n");
            }
            utilex_str_replace(prt_column->name, ' ', '_');
            RHDATA_SET_INT(node, "id", k);
            RHDATA_SET_STR(node, "name", prt_column->name);
            RHDATA_SET_INT(node, "type", prt_column->type);
        }
        dbx_xml_node_end(headers_node, depth_base);
    }
    /*
     * Add Entries Group node
     */
    if ((prt_ctr->columns[PRT_TITLE_ID]->node[0] = dbx_xml_child_add(table_node, "entries", depth_base)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "failed to add headers node to:%s\n", prt_ctr->title);
    }
    prt_ctr->columns[PRT_TITLE_ID]->depth = depth_base;
    /*
     * Now we'll iterate on all the row and all the cells inside
     */
    RHITERATOR(prt_row_start, prt_ctr->list)
    {
        PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
        {
            prt_row_offset = ((char *) prt_row_start) + sizeof(rhentry_t) + prt_column->cell_offset;
            /*
             * No use for empty cells
             */
            if (ISEMPTY(prt_row_offset))
                continue;
            /*
             * Check if it is shifted and by how many
             */
            shift_n = diag_sand_get_shift(prt_row_offset);
            /*
             * Eliminate white spaces
             */
            prt_row_offset += shift_n * 2;
            /*
             * Figure out who is the parent
             */
            if ((shift_n != 0) && (prt_column->type == PRT_XML_CHILD))
            {   /* 
                 * We have shift and this column is child, we make shifted cell child to its non-shifted or less shifted
                 * brother instead of parent
                 * If shifted cell is attribute it will be assigned to its parent column anyway
                 */
                parent_node = prt_column->node[shift_n - 1];
            }
            else
            {
                int parent_id = prt_column->parent_id;
                /*
                 * If parent node is NULL look for its parent and so on until title
                 */
                do
                {
                    prt_column_t *prt_column_parent = prt_ctr->columns[parent_id];

                    shift_n = prt_column_parent->cur_shift_id;
                    parent_node = prt_column_parent->node[shift_n];
                    parent_id = prt_column_parent->parent_id;
                }
                while ((parent_node == NULL) && (parent_id != PRT_TITLE_ID));
            }

            if (parent_node == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Inconsistency in inheritance Parent:%d for:%s is not node\n",
                             prt_column->parent_id, prt_column->name);
            }

            switch (prt_column->type)
            {
                case PRT_XML_CHILD:
                {       /* 
                         * If the place is occupied it should be ended together with all underlying shifted nodes
                         * and further columns
                         */
                    int i_sh;
                    prt_column_t *prt_column_cl;
                    for (k_cl = k; k_cl < prt_ctr->col_num; k_cl++)
                    {
                        prt_column_cl = prt_ctr->columns[k_cl];
                        if (prt_column_cl->type == PRT_XML_CHILD)
                        {
                            for (i_sh = shift_n; i_sh < PRT_MAX_SHIFT_NUM; i_sh++)
                            {
                                if (prt_column_cl->node[i_sh] != NULL)
                                {
                                    dbx_xml_node_end(prt_column_cl->node[i_sh], prt_column_cl->depth + i_sh);
                                    prt_column_cl->node[i_sh] = NULL;
                                    prt_column_cl->cur_shift_id = (i_sh != 0) ? (i_sh - 1) : 0;
                                }
                            }
                        }
                    }
                }
                    prt_column->depth = prt_ctr->columns[prt_column->parent_id]->depth + 1;
                    /*
                     * Make sure shift id is pointing to right place
                     */
                    prt_column->cur_shift_id = shift_n;
                    prt_column->node[shift_n] = dbx_xml_child_add(parent_node, prt_column->name,
                                                                  prt_column->depth + shift_n);
                    RHDATA_SET_STR(prt_column->node[shift_n], "node_content", prt_row_offset);
                    break;
                case PRT_XML_ATTRIBUTE:
                    RHDATA_SET_STR(parent_node, prt_column->name, prt_row_offset);
                    break;
                case PRT_XML_NONE:
                default:
                    break;
            }
        }
    }

    PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
    {
        for (shift_n = 0; shift_n < PRT_MAX_SHIFT_NUM; shift_n++)
        {
            if (prt_column->node[shift_n] != NULL)
            {
                dbx_xml_node_end(prt_column->node[shift_n], prt_column->depth + shift_n);
            }
        }
    }

    /*
     * Close "entries" node, depth_base son level of entries
     */
    dbx_xml_node_end(prt_ctr->columns[PRT_TITLE_ID]->node[0], prt_ctr->columns[PRT_TITLE_ID]->depth);
    depth_base--;
    dbx_xml_node_end(table_node, depth_base);
    depth_base--;
    dbx_xml_node_end(cmd_node, depth_base);

    SHR_IF_ERR_EXIT(dbx_file_save_xml(prt_ctr->unit, output_file, top_node, file_flags));
    dbx_xml_top_close(top_node);

exit:
    if (cmd_tokens != NULL)
    {
        utilex_str_split_free(cmd_tokens, cmd_num);
    }
    if (xpath_tokens != NULL)
    {
        utilex_str_split_free(xpath_tokens, xpath_parts_nof);
    }
    SHR_FUNC_EXIT;
}
#endif /* #if !defined(NO_FILEIO) */

static shr_error_e
diag_sand_prt_column_mark(
    prt_control_t * prt_ctr,
    char *columns_str,
    int flag)
{
    uint32 realtokens = 0;
    char **column_nums = NULL;
    uint32 range_realtokens = 0;
    char **range_column_nums = NULL;
    int i_col;

    SHR_FUNC_INIT_VARS(prt_ctr->unit);

    /*
     * Assume string represents set of comma separated substrings, numbers or range of columns
     */

    if ((column_nums = utilex_str_split(columns_str, ",", prt_ctr->col_num + 1, &realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to parse columns list:%s\n", columns_str);
    }

    if (realtokens > prt_ctr->col_num)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Too much tokens in columns list:%s\n", columns_str);
    }
    for (i_col = 0; i_col < realtokens; i_col++)
    {
        /*
         * Skip empty - usually last or double comma
         */
        if (ISEMPTY(column_nums[i_col]))
            continue;
        /*
         * Test on range
         */
        if ((range_column_nums =
             utilex_str_split(column_nums[i_col], "-", prt_ctr->col_num + 1, &range_realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to parse column list token:%s\n", column_nums[i_col]);
        }
        if (range_realtokens == 2)
        {
            uint32 show_col_id_start, show_col_id_end, i_ran;

            if ((utilex_str_stoul(range_column_nums[0], &show_col_id_start) != _SHR_E_NONE) ||
                (show_col_id_start > prt_ctr->col_num) || (show_col_id_start <= 0))
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Element:\"%s\" in the column list is not a valid column number\n",
                             range_column_nums[0]);
            }
            if ((utilex_str_stoul(range_column_nums[1], &show_col_id_end) != _SHR_E_NONE) ||
                (show_col_id_end > prt_ctr->col_num) || (show_col_id_end <= 0))
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Element:\"%s\" in the column list is not a valid column number\n",
                             range_column_nums[1]);
            }
            if (show_col_id_start > show_col_id_end)
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Start column id:%d is greater than end column id:%d\n",
                             show_col_id_start, show_col_id_end);
            }
            for (i_ran = show_col_id_start; i_ran <= show_col_id_end; i_ran++)
            {
                prt_ctr->columns[i_ran - 1]->flags |= flag;
            }
            /*
             * We have range indeed
             */
        }
        else if (range_realtokens != 1)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error - there may not more than 1 '-' inside 1 range:%s\n",
                         column_nums[i_col]);
        }
        else
        {
            uint32 show_col_id;
            /*
             * No range sign '-'inside
             * Go through all numbers in list and assign flag to matching occurences
             */
            if (utilex_str_stoul(column_nums[i_col], &show_col_id) != _SHR_E_NONE)
            {
                int k;
                prt_column_t *prt_column;
                /** Token is not a number lookup as substring in header names */
                PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
                {
                    if (sal_strcasestr(prt_column->name, column_nums[i_col]) != NULL)
                    {
                        prt_column->flags |= flag;
                    }
                }
            }
            else if ((show_col_id > prt_ctr->col_num) || (show_col_id <= 0))
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Element:\"%s\" in the column list is not a valid column number\n",
                             column_nums[i_col]);
            }
            else
            {
                prt_ctr->columns[show_col_id - 1]->flags |= flag;
            }
        }
        /*
         * In case there will be more than one range
         */
        utilex_str_split_free(range_column_nums, range_realtokens);
        range_column_nums = NULL;
    }

exit:
    utilex_str_split_free(column_nums, realtokens);
    utilex_str_split_free(range_column_nums, range_realtokens);
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_prt_column_show(
    prt_control_t * prt_ctr,
    char *columns_str,
    int *column_control_p)
{
    int column_control;

    SHR_FUNC_INIT_VARS(prt_ctr->unit);

    if (ISEMPTY(columns_str) || !sal_strcasecmp(columns_str, "all"))
    {
        column_control = PRT_COLUMN_CONTROL_ALL;
    }
    else if (!sal_strcasecmp(columns_str, "nosame"))
    {
        /*
         * First column will be always visible even if matching all other criteria
         */
        prt_ctr->columns[0]->flags |= PRT_COLUMN_FLAG_SHOW;
        column_control = PRT_COLUMN_CONTROL_NOSAME;
    }
    else if (!sal_strcasecmp(columns_str, "nozero"))
    {
        /*
         * First column will be always visible even if matching all other criteria
         */
        prt_ctr->columns[0]->flags |= PRT_COLUMN_FLAG_SHOW;
        column_control = PRT_COLUMN_CONTROL_NOZERO;
    }
    else
    {
        /*
         * Assume string represents set of comma separated or range of columns
         */
        SHR_CLI_EXIT_IF_ERR(diag_sand_prt_column_mark(prt_ctr, columns_str, PRT_COLUMN_FLAG_SHOW), "");
        column_control = PRT_COLUMN_CONTROL_FILTERED;
    }

    *column_control_p = column_control;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_prt_cell_filter(
    prt_control_t * prt_ctr,
    char *filter_str)
{
    uint32 tokens_num = 0;
    char **tokens = NULL;
    char *cell_filter;
    uint32 cell_filters_num = 0;
    char **cell_filters = NULL;
    int i_col, k_str;
    prt_column_t *prt_column;
    rhhandle_t prt_row_start;
    char *prt_row_offset;

    SHR_FUNC_INIT_VARS(prt_ctr->unit);

    /*
     * If filter is empty - nothing happens
     */
    if (ISEMPTY(filter_str))
    {
        SHR_EXIT();
    }
    /*
     * Split filter into columns identification and actual cell filter
     */
    if ((tokens = utilex_str_split(filter_str, ":", 2, &tokens_num)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "No Memory for parsing:%s\n", filter_str);
    }

    if (tokens_num == 1)
    {
        /*
         * Look in all columns for the cell with this content
         */
        PRT_ALL_COLUMN_ITERATOR(i_col, prt_column, prt_ctr)
        {
            prt_column->flags |= PRT_COLUMN_FLAG_FILTER;
        }
        cell_filter = tokens[0];
    }
    else if (tokens_num == 2)
    {
        /*
         * Assume tokens[0] string represents set of comma separated or range of columns
         */
        cell_filter = tokens[1];
        if (ISEMPTY(cell_filter))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Empty cell content in criterion:%s\n", filter_str);
        }
        SHR_CLI_EXIT_IF_ERR(diag_sand_prt_column_mark(prt_ctr, tokens[0], PRT_COLUMN_FLAG_FILTER), "");
    }
    else
    {
        /*
         * Cannot happen - keeping it for logical reasons
         */
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Wrong number of tokens on:%s\n", filter_str);
    }
    /*
     * Now we need to parse proper cell_filter - it may be list of comma separated strings up to MAX number
     */
    if ((cell_filters = utilex_str_split(cell_filter, ",", PRT_CELL_FILTER_MAX, &cell_filters_num)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "No Memory for parsing:%s\n", cell_filter);
    }
    else
    {
        /**
         * No go through rows and figure one which one match the filtering criteria
         */
        RHSAFE_ITERATOR(prt_row_start, prt_ctr->list)
        {
            int row_match = FALSE;
            /*
             * Use here ALL_COLUMN because we may mark some of the columns as invisible,
             * but here we may probably need to make them visible
             */
            PRT_FILTER_COLUMN_ITERATOR(i_col, prt_column, prt_ctr)
            {
                prt_row_offset = ((char *) prt_row_start) + sizeof(rhentry_t) + prt_column->cell_offset;
                for (k_str = 0; k_str < cell_filters_num; k_str++)
                {
                    /*
                     * Compare content of the cell to input filter string, if there is no match left the sign
                     * Break if match - No need to keep comparing if there is already match in one of columns
                     */
                    if (sal_strcasestr(prt_row_offset, cell_filters[k_str]) != NULL)
                    {
                        row_match = TRUE;
                        break;
                    }
                }
                if (row_match == TRUE)
                    break;
            }
            /*
             * No match in anyone of filters anyone of eligible columns - delete the row
             */
            if (row_match == FALSE)
            {
                utilex_rhlist_entry_del_free(prt_ctr->list, prt_row_start);
            }
        }
    }

exit:
    utilex_str_split_free(tokens, tokens_num);
    utilex_str_split_free(cell_filters, cell_filters_num);
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_prt_get_column_info(
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control,
    prt_format_t * column,
    int *total_width,
    int *padding_last_column)
{

    int k, n;

    int column_width;
    rhhandle_t prt_row_start;
    char *prt_row_offset;
    int column_control = PRT_COLUMN_CONTROL_ALL;
    prt_column_t *prt_column;
    int silence = FALSE;

    SHR_FUNC_INIT_VARS(prt_ctr->unit);

    if (sand_control != NULL)
    {
        char *arg_str;
        SH_SAND_GET_BOOL("silence", silence);
        SH_SAND_GET_STR("columns", arg_str);
        SHR_IF_ERR_EXIT(diag_sand_prt_column_show(prt_ctr, arg_str, &column_control));
        SH_SAND_GET_STR("cell", arg_str);
        SHR_IF_ERR_EXIT(diag_sand_prt_cell_filter(prt_ctr, arg_str));
    }

    /*
     * ALL control means any event that we are not filtering the columns - showing them all
     */
    if (column_control == PRT_COLUMN_CONTROL_ALL)
    {
        PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
        {
            prt_column->flags |= PRT_COLUMN_FLAG_SHOW;
        }
    }

    if ((prt_ctr->list == NULL) && (prt_ctr->info_list == NULL))
    {
        if (silence == FALSE)
        {
            SHR_CLI_EXIT(_SHR_E_NONE, "No content for table:\"%s\"\n", prt_ctr->title);
        }
        else
        {
            /*
             * Leave Peacefully
             */
            SHR_CLI_EXIT(_SHR_E_NONE, "");
        }
    }

    /**
     * Step 1 - Figure out longest cells in each column and figure out if there is a need to replace column by info
     */
    RHITERATOR(prt_row_start, prt_ctr->list)
    {
        /*
         * Use here ALL_COLUMN because we may mark some of the columns as invisible,
         * but here we may probably need to
         * make them visible
         */
        PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
        {
            prt_row_offset = ((char *) prt_row_start) + sizeof(rhentry_t) + prt_column->cell_offset;
            /*
             * remove trailing white spaces
             */
            for (n = sal_strlen(prt_row_offset) - 1; n > 1; n--)
            {
                if (prt_row_offset[n] == ' ')
                    prt_row_offset[n] = 0;
                else
                    break;
            }
            /*
             * Make width maximum between all cells in column
             */
            if ((column_width = prt_token_max_size(prt_row_offset)) > column[k].width)
                column[k].width = column_width;
            /*
             * Compare content of the cell to invisible string, if there is no match left the sign
             * No need to keep comparing if there is already no match
             */
            if (column_control == PRT_COLUMN_CONTROL_NOSAME)
            {
                /*
                 * Always fill sample string with first non empty string in the column
                 */
                if (ISEMPTY(column[k].sample_str))
                {
                    /*
                     * If sample is still empty fill it only if content itself is not empty
                     */
                    if (!ISEMPTY(prt_row_offset))
                    {
                        column[k].sample_str = prt_row_offset;
                    }
                }
                else
                {
                    /*
                     * No need to check is column is already visible or cell is empty
                     * Otherwise if current does not match sample or empty column will be visible
                     */
                    if (!(prt_column->flags & PRT_COLUMN_FLAG_SHOW)
                        && !ISEMPTY(prt_row_offset) && sal_strcasecmp(prt_row_offset, column[k].sample_str))
                    {
                        prt_column->flags |= PRT_COLUMN_FLAG_SHOW;
                    }
                }
            }
            else if ((column_control == PRT_COLUMN_CONTROL_NOZERO))
            {
                /*
                 * Check if the token is actually full of zeroes in different formats
                 */
                if (!(prt_column->flags & PRT_COLUMN_FLAG_SHOW) && (prt_token_is_zero(prt_row_offset) == FALSE))
                {
                    prt_column->flags |= PRT_COLUMN_FLAG_SHOW;
                }
            }
        }
    }

    /**
     * Step 2 - Create format per column and count total line width only in case where there are any entries
     */
    if (prt_ctr->list != NULL)
    {
        PRT_COLUMN_ITERATOR(k, prt_column, prt_ctr)
        {
            int col_name_size = sal_strlen(prt_column->name);
            int max_token_size;
            /*
             * Found MAX Token size in column header. If col_name_size is equal to max_token_size - no tokens inside
             */
            max_token_size = prt_get_token_max_size(prt_column->name);
            /*
             * Sync with Header Cell(actually column name) width
             */
            /*
             * For columns defined as flex we'll make sure that column name will be also split to match minimal width
             */
            if ((prt_column->flex_type == 0) || (col_name_size <= column[k].width) ||
                ((col_name_size > column[k].width) && (col_name_size == max_token_size)))
            {
                column[k].width = (col_name_size > column[k].width) ? col_name_size : column[k].width;
            }
            else
            {
                column[k].width = (max_token_size > column[k].width) ? max_token_size : column[k].width;
            }
            /**
             * Cannot be more that PRT_COLUMN_WIDTH
             */
            if (column[k].width >= PRT_COLUMN_WIDTH)
                column[k].width = PRT_COLUMN_WIDTH - 1;
            /*
             * aggregate total row size
             */
            if (column[k].width == 0)
            {
                /*
                 * Just add | without spaces - it is intended for special separation
                 */
                *total_width += column[k].width + 1;    /* for | between fields and spaces before and after */
                /*
                 * create format string for each column;
                 */
                sal_snprintf(column[k].format, RHKEYWORD_MAX_SIZE - 1, "|%ss", "%");
            }
            else
            {
                *total_width += column[k].width + 3;    /* for | between fields and spaces before and after */
                /*
                 * create format string for each column;
                 */
                if (prt_column->align == TRUE)
                    sal_snprintf(column[k].format, RHKEYWORD_MAX_SIZE - 1, "|%s%ds ", "%", column[k].width + 1);
                else
                    sal_snprintf(column[k].format, RHKEYWORD_MAX_SIZE - 1, "| %s-%ds", "%", column[k].width + 1);
            }
        }
    }

    /*
     * Step 3- Print title surrounded by '=' lines and inside info
     */
    if (!ISEMPTY(prt_ctr->title))
    {
        int tmp_width;
        int title_width;
        int header_name_width = 0, sample_str_width = 0;
        prt_info_t *info_entry;
        char *preamble = "Hidden columns";
        int preamble_width = sal_strlen(preamble);

        /*
         * In case title is longer than table width - make header separation line same size as title
         */
        title_width = sal_strlen(prt_ctr->title);
        /*
         * Make line according to longest info line. Pay attention that info line has 2 extra spaces before
         */
        RHITERATOR(info_entry, prt_ctr->info_list)
        {
            if ((tmp_width = sal_strlen(info_entry->content)) > (title_width - 2))
                title_width = tmp_width + 2;
        }

        PRT_HIDDEN_COLUMN_ITERATOR(k, prt_column, prt_ctr)
        {
            if ((tmp_width = sal_strlen(prt_column->name)) > header_name_width)
                header_name_width = tmp_width;
            if (column[k].sample_str == NULL)
                column[k].sample_str = "";
            if ((tmp_width = sal_strlen(column[k].sample_str)) > sample_str_width)
                sample_str_width = tmp_width;
        }
        /*
         * Hidden headers are represent by sum of max name max value and 3 characters in between " | "
         * If there are none (both widths are 0)title width is assumed to be always more than 3 so no harm
         */

        if ((preamble_width + header_name_width + sample_str_width + 6) > title_width)
            title_width = preamble_width + header_name_width + sample_str_width + 6;
        /*
         * "- 4" is '| ' before and ' |' after
         * In case title_width is more than total_width, update total_width and padding_last_column
         * (represent the padding that the last column should have before '|')
         */
        if (title_width > (*total_width - 4))
        {
            *padding_last_column = title_width + 4 - *total_width;
            *total_width = title_width + 4;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_prt_internal(
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control,
    char *prt_filename,
    int max_line_width)
{
    int i_rows, k;

    prt_format_t *column = NULL;
    int total_width = 1;
    int tmp_total_width = 1;
    int padding_last_column = 0;
    rhhandle_t prt_row_start;
    rhentry_t *entry;
    char *tmp_str = NULL;
    int silence = FALSE;
    int column_control = PRT_COLUMN_CONTROL_ALL;
    prt_column_t *prt_column;
    int print_end = 0;
    char *title_padding = NULL;

    SHR_FUNC_INIT_VARS(prt_ctr->unit);

    if (sand_control != NULL)
    {
        char *arg_str;
        SH_SAND_GET_BOOL("silence", silence);
        SH_SAND_GET_STR("columns", arg_str);
        SHR_IF_ERR_EXIT(diag_sand_prt_column_show(prt_ctr, arg_str, &column_control));
        if (prt_filename == NULL)
        {
            SH_SAND_GET_STR("file", prt_filename);
        }
        SH_SAND_GET_STR("cell", arg_str);
        SHR_IF_ERR_EXIT(diag_sand_prt_cell_filter(prt_ctr, arg_str));
    }

    /*
     * ALL control means any event that we are not filtering the columns - showing them all
     */
    if (column_control == PRT_COLUMN_CONTROL_ALL)
    {
        PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
        {
            prt_column->flags |= PRT_COLUMN_FLAG_SHOW;
        }
    }

    if ((prt_ctr->list == NULL) && (prt_ctr->info_list == NULL))
    {
        if (silence == FALSE)
        {
            SHR_CLI_EXIT(_SHR_E_NONE, "No content for table:\"%s\"\n", prt_ctr->title);
        }
        else
        {
            /*
             * Leave Peacefully
             */
            SHR_CLI_EXIT(_SHR_E_NONE, "");
        }
    }

#if !defined(NO_FILEIO)
    if (!ISEMPTY(prt_filename))
    {
        if (dbx_file_get_type(prt_filename) != DBX_FILE_XML)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Output file must have .xml suffix. file was:\"%s\"\n", prt_filename);
        }

        SHR_CLI_EXIT_IF_ERR(diag_sand_prt_xml(prt_ctr, sand_control, prt_filename), "");
        SHR_EXIT();
    }
#endif /* #if !defined(NO_FILEIO) */

    if (prt_ctr->col_num != 0)
    {
        column = sal_alloc(sizeof(prt_format_t) * prt_ctr->col_num, "column format");
        sal_memset(column, 0, sizeof(prt_format_t) * prt_ctr->col_num);
    }

    if (max_line_width == 0 || !ISEMPTY(prt_filename))
    {
        print_end = 1;
    }

    /*
     * Step 1- Figure out longest cells in each column and
     * create format per column and count total line width only in case where there are any entries
     */

    SHR_IF_ERR_EXIT(diag_sand_prt_get_column_info(prt_ctr, sand_control, column, &tmp_total_width,
                                                  &padding_last_column));

    if (tmp_total_width < max_line_width)
    {
        total_width = max_line_width;
    }
    else
    {
        total_width = tmp_total_width;
    }

    /*
     * Step 2- Print title surrounded by '=' lines and inside info
     */
    if (!ISEMPTY(prt_ctr->title))
    {
        int tmp_width;
        int title_width;
        char title_format[RHNAME_MAX_SIZE];
        int header_name_width = 0, sample_str_width = 0;
        prt_info_t *info_entry;
        char *preamble = "Hidden columns";
        int preamble_width = sal_strlen(preamble);
        int first_hidden;

        /*
         * In case title is longer than table width - make header separation line same size as title
         */
        title_width = sal_strlen(prt_ctr->title);
        /*
         * Make line according to longest info line. Pay attention that info line has 2 extra spaces before
         */
        RHITERATOR(info_entry, prt_ctr->info_list)
        {
            if ((tmp_width = sal_strlen(info_entry->content)) > (title_width - 2))
                title_width = tmp_width + 2;
        }

        PRT_HIDDEN_COLUMN_ITERATOR(k, prt_column, prt_ctr)
        {
            if ((tmp_width = sal_strlen(prt_column->name)) > header_name_width)
                header_name_width = tmp_width;
            if (column[k].sample_str == NULL)
                column[k].sample_str = "";
            if ((tmp_width = sal_strlen(column[k].sample_str)) > sample_str_width)
                sample_str_width = tmp_width;
        }
        /*
         * Hidden headers are represent by sum of max name max value and 3 characters in between " | "
         * If there are none (both widths are 0)title width is assumed to be always more than 3 so no harm
         */

        if ((preamble_width + header_name_width + sample_str_width + 6) > title_width)
            title_width = preamble_width + header_name_width + sample_str_width + 6;

        if (title_width < (total_width - 4))
        {

            int nof_spaces = (total_width - 4) - title_width;
            int front_title_padding = (nof_spaces / 2);
            int end_title_padding = (nof_spaces / 2) + (nof_spaces % 2);

            sal_snprintf(title_format, RHNAME_MAX_SIZE - 1, "| %s%ds ", "%", front_title_padding + title_width);
            title_padding = sal_alloc(end_title_padding, "title padding");
            sal_memset(title_padding, 0, end_title_padding);
            sal_memset(title_padding, ' ', end_title_padding - 1);
            sal_strncat(title_format, title_padding, RHNAME_MAX_SIZE - sal_strnlen(title_format, RHNAME_MAX_SIZE) - 5);
            sal_strncat(title_format, " |\n", 4);
        }
        else
        {
            sal_snprintf(title_format, RHNAME_MAX_SIZE - 1, "| %s-%ds |\n", "%", total_width - 4);
        }

        /*
         * "- 4" is '| ' before and ' |' after
         * In case title_width is more than total_width, update total_width and last column format
         */
        if (padding_last_column > 0 || (tmp_total_width < total_width))
        {
            int last_visible_col_id = 0;
            int offset_to_add = 0;
            if (padding_last_column > 0)
            {
                offset_to_add = padding_last_column;
            }
            else
            {
                offset_to_add = total_width - tmp_total_width;
            }

            PRT_COLUMN_ITERATOR(k, prt_column, prt_ctr)
            {
                last_visible_col_id = k;
            }
            if (column != NULL)
            {
                sal_snprintf(column[last_visible_col_id].format, RHKEYWORD_MAX_SIZE - 1, "| %s-%ds", "%",
                             column[last_visible_col_id].width + 1 + offset_to_add);
            }
        }

        diag_sand_prt_row(total_width, '=');
        prt_printf(title_format, prt_ctr->title);
        /*
         * Info format is the same as title with 2 extra spaces before
         */
        sal_snprintf(title_format, RHNAME_MAX_SIZE - 1, "|   %s-%ds |\n", "%", total_width - 6);
        RHITERATOR(info_entry, prt_ctr->info_list)
        {
            if (info_entry->entry.mode != PRT_ROW_SEP_NONE)
            {
                diag_sand_prt_row(total_width, '-');
            }
            prt_printf(title_format, info_entry->content);
        }
        /*
         * Print all hidden columns with sample string only if there are any lines
         */
        if (prt_ctr->list != NULL)
        {
            sal_snprintf(title_format, RHNAME_MAX_SIZE - 1, "| %s%ds | %s-%ds | %s-%ds |\n",
                         "%", total_width - header_name_width - sample_str_width - 10, "%", header_name_width, "%",
                         sample_str_width);
            first_hidden = TRUE;
            PRT_HIDDEN_COLUMN_ITERATOR(k, prt_column, prt_ctr)
            {
                if (first_hidden == TRUE)
                {
                    diag_sand_prt_row(total_width, '-');
                    first_hidden = FALSE;
                    prt_printf(title_format, preamble, prt_column->name, column[k].sample_str);
                }
                else
                {
                    prt_printf(title_format, "", prt_column->name, column[k].sample_str);
                }
            }
            /*
             * Check if there is at least one column chosen, if not - just print '=' line and exit
             */
            PRT_ALL_COLUMN_ITERATOR(k, prt_column, prt_ctr)
            {
                if (prt_column->flags & PRT_COLUMN_FLAG_SHOW)
                    break;
            }
            if (k == prt_ctr->col_num)
            {
                diag_sand_prt_row(total_width, '=');
                SHR_EXIT();
            }
        }
    }
    /*
     * Always separate general part from columns by '=' line, if the prt_ctr list is not empty
     */
    if (prt_ctr->list != NULL || print_end)
    {
        diag_sand_prt_row(total_width, '=');
    }

    /*
     * Step 3 - Print Header Line from prt_ctr->col only if there is list of rows, otherwise skip,
     *          probably we have table info only
     */
    tmp_str = sal_alloc(PRT_COLUMN_WIDTH_BIG, "temp");
    if (prt_ctr->list != NULL)
    {
        int cells_to_print;
        char *to_ptr;
        int token_size;
        int next_shift = 0;
        int i_line = 0;
        do
        {
            /**
             * For each line zero cell_to_print, as if there will be no more lines in the row
             */
            cells_to_print = 0;
            if (i_line++ == 0)
            {
                /*
                 * Initialize next pointer for the first line in the row
                 */
                PRT_COLUMN_ITERATOR(k, prt_column, prt_ctr)
                {
                    column[k].next_ptr = prt_column->name;
                }
            }
            else
            {   /* 
                 * If the line is not first, issue EOL
                 */
                prt_printf("|\n");
            }

            PRT_COLUMN_ITERATOR(k, prt_column, prt_ctr)
            {
                token_size = sal_strlen(column[k].next_ptr);
                if ((token_size > column[k].width) || prt_token_has_eol(column[k].next_ptr))
                {   /**
                     * Split the string to print portions fitting into the width
                     * Adjust token_size
                     */

                    switch (prt_column->flex_type)
                    {
                            /*
                             * Never mind how regular cells were split, header will be split always ASCII
                             */
                        case PRT_FLEX_BINARY:
                        case PRT_FLEX_ASCII:
                        case PRT_FLEX_ASCII_ORIG:
                            token_size = prt_get_token_size(column[k].next_ptr, column[k].width, &next_shift);
                            break;
                        case PRT_FLEX_NONE:
                            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Small column width:%d for the string:%d\n", column[k].width,
                                         token_size);
                            break;
                    }
                    memcpy(tmp_str, column[k].next_ptr, token_size);
                    cells_to_print++;
                    /*
                     * Make token NULL ended
                     */
                    tmp_str[token_size] = 0;
                    to_ptr = tmp_str;
                }
                else
                {
                    to_ptr = column[k].next_ptr;
                }
                prt_printf(column[k].format, to_ptr);
                column[k].next_ptr += (token_size + next_shift);
            }
        }
        while (cells_to_print != 0);
        prt_printf("|\n");
        diag_sand_prt_row(total_width, '=');
    }

    /*
     * Step 4 - Print cells, if there are any rows
     */
    i_rows = 0;
    RHITERATOR(prt_row_start, prt_ctr->list)
    {
        int cells_to_print;
        char *to_ptr;
        int token_size;
        int next_shift = 0;
        /**
         * Row is the one allocated by PRT user, may have multiple printed lines inside due to long cells
         */
        int i_line = 0;
        entry = (rhentry_t *) prt_row_start;
        if ((i_rows != 0) && (entry->mode == PRT_ROW_SEP_UNDERSCORE_BEFORE))
        {
            diag_sand_prt_row(total_width, '-');
        }
        else if ((i_rows != 0) && (entry->mode == PRT_ROW_SEP_EQUAL_BEFORE))
        {
            diag_sand_prt_row(total_width, '=');
        }
        do
        {
            /**
             * For each line zero cell_to_print, as if there will be no more lines in the row
             */
            cells_to_print = 0;
            if (i_line++ == 0)
            {
                /*
                 * Initialize next pointer for the first line in the row
                 */
                PRT_COLUMN_ITERATOR(k, prt_column, prt_ctr)
                {
                    column[k].next_ptr = ((char *) prt_row_start) + sizeof(rhentry_t) + prt_column->cell_offset;
                }
            }
            else
            {   /* 
                 * If the line is not first, issue EOL
                 */
                prt_printf("|\n");
            }

            PRT_COLUMN_ITERATOR(k, prt_column, prt_ctr)
            {
                token_size = sal_strlen(column[k].next_ptr);
                if ((token_size > column[k].width) || prt_token_has_eol(column[k].next_ptr))
                {   /**
                     * Split the string to print portions fitting into the width
                     * Adjust token_size
                     */

                    switch (prt_column->flex_type)
                    {
                        case PRT_FLEX_BINARY:
                            token_size = column[k].width;
                            break;
                        case PRT_FLEX_ASCII:
                        case PRT_FLEX_ASCII_ORIG:
                            token_size = prt_get_token_size(column[k].next_ptr, column[k].width, &next_shift);
                            break;
                        case PRT_FLEX_NONE:
                            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Small column width:%d for the string:%d\n", column[k].width,
                                         token_size);
                            break;
                    }
                    memcpy(tmp_str, column[k].next_ptr, token_size);
                    cells_to_print++;
                    /*
                     * Make token NULL ended
                     */
                    tmp_str[token_size] = 0;
                    to_ptr = tmp_str;
                }
                else
                {
                    to_ptr = column[k].next_ptr;
                }
                prt_printf(column[k].format, to_ptr);
                column[k].next_ptr += (token_size + next_shift);
            }
        }
        while (cells_to_print != 0);

        prt_printf("|\n");
        if (((i_rows == RHLNUM(prt_ctr->list) - 1) && print_end) || (entry->mode == PRT_ROW_SEP_EQUAL))
        {
            diag_sand_prt_row(total_width, '=');
        }
        else if (entry->mode == PRT_ROW_SEP_UNDERSCORE)
        {
            diag_sand_prt_row(total_width, '-');
        }
        i_rows++;
    }

exit:
    if (tmp_str != NULL)
    {
        sal_free(tmp_str);
    }
    if (column != NULL)
    {
        sal_free(column);
    }
    if (title_padding != NULL)
    {
        sal_free(title_padding);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_prt_multiple(
    prt_control_t ** prt_ctr,
    sh_sand_control_t * sand_control,
    uint32 table_count)
{
    uint32 table_index = 0;
    int max_line_width = 1;
    int total_width = 1;
    int padding_last_column = 0;
    prt_format_t *column = NULL;
    uint8 file_is_present = FALSE;
    SHR_FUNC_INIT_VARS(prt_ctr[table_index]->unit);

    SH_SAND_IS_PRESENT("file", file_is_present);

    for (table_index = 0; table_index < table_count; table_index++)
    {
        /** skip tables that are NULL */
        if ((prt_ctr[table_index]) && (prt_ctr[table_index]->col_num != 0))
        {
            column = sal_alloc(sizeof(prt_format_t) * prt_ctr[table_index]->col_num, "column format");
            sal_memset(column, 0, sizeof(prt_format_t) * prt_ctr[table_index]->col_num);

            SHR_IF_ERR_EXIT(diag_sand_prt_get_column_info(prt_ctr[table_index], sand_control, column, &total_width,
                                                          &padding_last_column));
            SHR_FREE(column);

            if (total_width > max_line_width)
            {
                max_line_width = total_width;
            }

            total_width = 1;
        }
    }

    for (table_index = 0; table_index < table_count; table_index++)
    {
        /** skip tables that are NULL */
        if (prt_ctr[table_index])
        {
            SHR_IF_ERR_EXIT(diag_sand_prt_internal(prt_ctr[table_index], sand_control, NULL, max_line_width));
        }
    }

    if (file_is_present == FALSE)
    {
        diag_sand_prt_row(max_line_width, '=');
    }

exit:
    if (column != NULL)
    {
        sal_free(column);
    }
    SHR_FUNC_EXIT;
}
shr_error_e
diag_sand_prt(
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control,
    char *prt_filename)
{
    int max_line_width = 0;
    SHR_FUNC_INIT_VARS(prt_ctr->unit);

    SHR_IF_ERR_EXIT(diag_sand_prt_internal(prt_ctr, sand_control, prt_filename, max_line_width));

exit:
    SHR_FUNC_EXIT;
}
