
/**
 * \file diag_sand_prt.h
 *
 * Set of macros and routines to print beautified tabular output. Main purpose to provide
 * standard output format for all tabular data in bcm shell
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_SAND_PRT_H
#define DIAG_SAND_PRT_H

#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>

#define PRT_COLUMN_WIDTH     (RHNAME_MAX_SIZE + 1)
#define PRT_COLUMN_WIDTH_BIG (16 * RHNAME_MAX_SIZE + 1)
#define PRT_TITLE_WIDTH      (2*RHSTRING_MAX_SIZE + 1)
#define PRT_COLUMN_MAX_NUM  256
#define PRT_TITLE_ID       PRT_COLUMN_MAX_NUM
#define PRT_MAX_SHIFT_NUM   5

typedef struct
{
    uint32 width;
    char format[RHKEYWORD_MAX_SIZE];
    char *next_ptr;
    /**
     * sample_str - first non empty string in column, if all cells in the column will be either identical or empty
     *              column will be shown in info instead of actual column when option columns="nosame" was choosen
     */
    char *sample_str;
} prt_format_t;

/**
 * In order to have proper XML output, each column should be assigned XML role
 */
typedef enum
{
    /**
     * No XML output assign, column will be skipped
     */
    PRT_XML_NONE,
    /**
     * Column will be child, parent node should be figured out either through calculation or explicit input
     */
    PRT_XML_CHILD,
    /**
     * Column will be attribute, node which attribute it will be should be either calculated or provided explicitly
     */
    PRT_XML_ATTRIBUTE
} PRT_XML_TYPE;

/**
 * Flex Type - allows PRT to split either cells or headers according to different criteria into multiple rows,
 */
typedef enum
{
    /*
     * No flexibility
     */
    PRT_FLEX_NONE,
    /*
     * Split ASCII string using space, tab, point, comma or underscore (' ', '.', ',', '_', '\t')
     *  '\n' wil always move to the next line
     */
    PRT_FLEX_ASCII,
    /*
     * Split ASCII string using space, tab, point, comma or underscore (' ', '.', ',', '_', '\t')
     *  '\n' will always move to the next line
     *  Difference from previous is that no shrink is performed on the string
     */
    PRT_FLEX_ASCII_ORIG,
    /*
     * Split the string directly into portions of RHNAME_MAX_SIZE characters
     */
    PRT_FLEX_BINARY
} PRT_FLEX_TYPE;

#define         PRT_COLUMN_FLAG_SHOW         0x01
#define         PRT_COLUMN_FLAG_FILTER       0x02

typedef struct
{
    /**
     * Column name - used in table header or as XML node name
     * Established only by COLUMN_ADD
     */
    char name[PRT_COLUMN_WIDTH];
    /**
     * Misc flags - Does not influence XML output
     * PRT_COLUMN_FLAG_SHOW     - if this flag is TRUE show column in screen table
     * PRT_COLUMN_FLAG_FILTER   - If this flag is TRUE use cell from this column in content filtering
     */
    int flags;
    /**
     * Flag marking whether string should be split as binary, ASCII or none
     * Established only by COLUMN_ADD
     */
    PRT_FLEX_TYPE flex_type;
    /**
     * Maximum size for strings in this column
     * Established only by COLUMN_ADD
     */
    int max_width;
    /**
     * Cell offset in the row
     * Established only by COLUMN_ADD
     */
    int cell_offset;
    void *node[PRT_MAX_SHIFT_NUM];
    int cur_shift_id;
    PRT_XML_TYPE type;
    char *parent_name;
    int parent_id;
    int depth;
    int align;
} prt_column_t;

typedef struct
{
    /*
     * Entry used to be queued on info_list
     */
    rhentry_t entry;
    /*
     * Allow info length be the same as title one
     */
    char content[PRT_TITLE_WIDTH];
} prt_info_t;

typedef struct
{
    /*
     * Assigned once when we now the sum of all column widths by the first row added
     */
    rhlist_t *list;
    /*
     * Unit id
     */
    int unit;
    /*
     * Modified by PRT_COLUMN_ADD, first PRT_ROW_ADD blocks the option for column changes
     */
    int col_num;
    /*
     * Sum of the all column max widths, modified only by PRT_COLUMN_ADD
     */
    int row_width;
    /**
     * Desirable display width, set around in PRT_TITLE_SET or left undefined which prompts 64 character per column
     */
    int display_width;
    /*
     * Separate place for title to allow bigger string
     */
    char title[PRT_TITLE_WIDTH];
    /*
     * Contain per column info - keep + 1 - it is for title node
     */
    prt_column_t *columns[PRT_COLUMN_MAX_NUM + 1];
    /*
     * Dynamic parameter - used to keep current entry
     */
    void *row_cur;
    /*
     * Dynamic parameter - current column in the row
     */
    int col_cur;
    /*
     * Per table information list
     */
    rhlist_t *info_list;
} prt_control_t;

/**
 * \brief Get the width of the terminal to best output fit
 * \par DIRECT OUTPUT
 * \retval - terminal width
 */
int diag_sand_prt_width(
    void);

/**
 * \brief Print number of identical characters
 * \par DIRECT INPUT
 *  \param num [in] number of characters to print
 *  \param ch [in] character to print
 * \par DIRECT OUTPUT
 * \retval - terminal width
 */
void diag_sand_prt_char(
    int num,
    char ch);

/**
 * \brief Actual output of prepared data either to display or XML file
 * \param prt_ctr [in/out] pointer to the control list of table print
 * \param sand_control [in] pointer to control block of shell command. NULL means that command was invocated by framework
 * \param prt_filename [in] explicit XML filename to write the table to
 * \remark
 *      OUTPUT: printing or formatted output to the standard output/XML File
 */
shr_error_e diag_sand_prt(
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control,
    char *prt_filename);

/**
 * \brief Actual output of prepared data to display (multiple tables)
 * \param prt_ctr [in/out] pointer to the control list of tables to print
 * \param sand_control [in] pointer to control block of shell command. NULL means that command was invocated by framework
 * \param table_count [in] Table count.
 * \remark
 *      OUTPUT: printing or formatted output to the standard output.
 */
shr_error_e diag_sand_prt_multiple(
    prt_control_t ** prt_ctr,
    sh_sand_control_t * sand_control,
    uint32 table_count);

/**
 * \brief Add new column to the table
 * \param prt_ctr [in/out]      - pointer to the control list of table print
 * \param flag [in]             - mark different properties of column
 * \param width [in]            - column width
 * \param prt_column_p [out]    - pointer to column pointer
 * \remark
 *    Should not be used outside the diag_sand_prt MACROs
 */
shr_error_e diag_sand_prt_column_add(
    prt_control_t * prt_ctr,
    int flag,
    int width,
    prt_column_t ** prt_column_p);

/**
 * \brief Sanitize XML gold result file - like remove timestamp and may be other things to consider
 * \param unit     [in]      - device id
 * \param flags     [in]     - flags used for gold result path.
 * \param command  [in]      - command string for the file to be sanitized
 * \param filename [in]      - filename build for this command
 * \remark
 *    should be kept as internal routine
 */
shr_error_e diag_sand_prt_gold_sanitize(
    int unit,
    int flags,
    char *command,
    char *filename);

/*
 * Modes for list entries
 */
typedef enum
{
    /**
     * No separation line
     */
    PRT_ROW_SEP_NONE,
    /**
     * Underscore(__________) line after the row
     */
    PRT_ROW_SEP_UNDERSCORE,
    /**
     * Underscore(__________) line before the row
     */
    PRT_ROW_SEP_UNDERSCORE_BEFORE,
    /**
     * Equal(==========) line after  the row
     */
    PRT_ROW_SEP_EQUAL,
    /**
     * Equal(==========) line before  the row
     */
    PRT_ROW_SEP_EQUAL_BEFORE,

} PRT_ROW_SEP_MODE;

/*
 * Internal Facilities for pretty print not to be used outside this include file
 * {
 */
#define _PRT_FORMAT(mc_ptr, mc_length, mc_format,...) {                                                                \
                                       sal_snprintf(mc_ptr, mc_length, mc_format "%s", __VA_ARGS__);                   \
                                     }
#define PRT_FORMAT(mc_ptr, mc_length,...) {                                                                            \
                                       _PRT_FORMAT(mc_ptr, mc_length, __VA_ARGS__, "");                                \
                                     }

#define PRT_COLUMN_ITERATOR(k_mc, prt_column_mc, prt_ctr_mc)                                                           \
        for (k_mc = 0, prt_column_mc = prt_ctr->columns[0]; k_mc < prt_ctr_mc->col_num;                                \
                                                           k_mc++, prt_column_mc = prt_ctr->columns[k_mc])             \
        if(prt_column_mc->flags & PRT_COLUMN_FLAG_SHOW)

#define PRT_FILTER_COLUMN_ITERATOR(k_mc, prt_column_mc, prt_ctr_mc)                                                    \
        for (k_mc = 0, prt_column_mc = prt_ctr->columns[0]; k_mc < prt_ctr_mc->col_num;                                \
                                                           k_mc++, prt_column_mc = prt_ctr->columns[k_mc])             \
        if(prt_column_mc->flags & PRT_COLUMN_FLAG_FILTER)

#define PRT_HIDDEN_COLUMN_ITERATOR(k_mc, prt_column_mc, prt_ctr_mc)                                                    \
        for (k_mc = 0, prt_column_mc = prt_ctr->columns[0]; k_mc < prt_ctr_mc->col_num;                                \
                                                           k_mc++, prt_column_mc = prt_ctr->columns[k_mc])             \
        if(!(prt_column_mc->flags & PRT_COLUMN_FLAG_SHOW))

#define PRT_ALL_COLUMN_ITERATOR(k_mc, prt_column_mc, prt_ctr_mc)                                                       \
        for (k_mc = 0, prt_column_mc = prt_ctr->columns[0]; k_mc < prt_ctr_mc->col_num;                                \
                                                           k_mc++, prt_column_mc = prt_ctr->columns[k_mc])

/**
 * \brief Free control structure allocated by _SET_TITLE
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \remark
 *     Should be always used at the end(usually after exit: label) of routine using pretty printing to catch all errors
 */
#define _PRT_FREE(prt_ctr_mc)                                                                                          \
    if(prt_ctr_mc != NULL)                                                                                             \
    {                                                                                                                  \
        int i_col;                                                                                                     \
        for(i_col = 0; i_col < prt_ctr_mc->col_num; i_col++)                                                           \
        {                                                                                                              \
            if(prt_ctr_mc->columns[i_col] != NULL)                                                                     \
                sal_free(prt_ctr_mc->columns[i_col]);                                                                  \
        }                                                                                                              \
        if(prt_ctr_mc->columns[PRT_TITLE_ID] != NULL)                                                                  \
            sal_free(prt_ctr_mc->columns[PRT_TITLE_ID]);                                                               \
        if(prt_ctr_mc->list != NULL)                                                                                   \
            utilex_rhlist_free_all(prt_ctr_mc->list);                                                                  \
        if(prt_ctr_mc->info_list != NULL)                                                                              \
            utilex_rhlist_free_all(prt_ctr_mc->info_list);                                                             \
        sal_free(prt_ctr_mc);                                                                                          \
        prt_ctr_mc = NULL;                                                                                             \
    }

/**
 * \brief Actual printing of prepared data to be used only in the context of shell framework
 * \param prt_ctr_mc [in] - pointer to the control structure
 * \remark
 *    Requires presence of sand_control, which is implicitly provided by the shell command invocation parameters, and
 *    should be transfered to underlying routines in the case of PRT usage down stack
 *    Frees the list
 */
#define _PRT_COMMITX(prt_ctr_mc) {                                                                                     \
        SHR_CLI_EXIT_IF_ERR(diag_sand_prt(prt_ctr_mc, sand_control, NULL), "");                                        \
        _PRT_FREE(prt_ctr_mc);                                                                                         \
    }

/**
 * \brief Actual printing of prepared data, like PRT_COMMITX, but with on option to use explicit XML output file name
 * \par DIRECT INPUT
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \param [in] filename_str - XML file to be used for data export, if NULL the one from sand_control may be used
 * \par INDIRECT INPUT: prt_ctr_mc      - control structure that provides all the info on the table
 *                      sand_control - control structure that provides all command related info
 * \par INDIRECT OUTPUT: printing or formatted output to the standard output
 * \remark automatically frees the list
 */
#define _PRT_COMMITF(prt_ctr_mc, filename_str) {                                                                       \
        SHR_CLI_EXIT_IF_ERR(diag_sand_prt(prt_ctr_mc, sand_control, filename_str), "");                                \
        _PRT_FREE(prt_ctr_mc);                                                                                         \
    }

/**
 * \brief Same as PRT_COMMITX, the difference being to be used when no shell framework sand_control is now available
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \par INDIRECT OUTPUT: printing or formatted output to the standard output
 * \remark automatically frees the list
 */
#define _PRT_COMMIT(prt_ctr_mc)                                                                                        \
{                                                                                                                      \
    SHR_CLI_EXIT_IF_ERR(diag_sand_prt(prt_ctr_mc, NULL, NULL), "");                                                    \
    _PRT_FREE(prt_ctr_mc);                                                                                             \
}

/**
 * \brief Same as PRT_COMMITX, the difference being to be is that it is printing several tables as one.
 * \param prt_ctr_mc [in] - pointer to the control structure
 * \param mc_num [in] - number of tables to print.
 * \remark
 *    Requires presence of sand_control, which is implicitly provided by the shell command invocation parameters, and
 *    should be transfered to underlying routines in the case of PRT usage down stack
 *    Does NOT frees the list, as we may want to print part of the set separately
 */
#define _PRT_COMMITX_COMBINE(prt_ctr_mc, mc_num)                                                                      \
{                                                                                                                      \
        SHR_CLI_EXIT_IF_ERR(diag_sand_prt_multiple(prt_ctr_mc, sand_control, mc_num), "");                               \
}

/**
 * \brief Verifies validity if row/column position
 * \param [in] prt_ctr_mc - pointer to the control structure
 */
#define _PRT_CELL_VERIFY(prt_ctr_mc) \
            if(prt_ctr_mc->row_cur == NULL)                                                                            \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "First row for the table was not allocated\n");                          \
            }                                                                                                          \
            if(prt_ctr_mc->col_cur >= prt_ctr_mc->col_num)                                                             \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Cell setting column:%d number of columns:%d\n",                         \
                                                                 prt_ctr_mc->col_cur + 1, prt_ctr_mc->col_num);        \
            }                                                                                                          \
            if(prt_ctr_mc->columns[prt_ctr_mc->col_cur] == NULL)                                                       \
            {                                                                                                          \
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Column:%d was not allocated\n", prt_ctr_mc->col_cur);                   \
            }

/**
 * \brief Fill cell with formatted data
 * \param prt_ctr_mc [in] - pointer to the control structure
 * \par INDIRECT OUTPUT prt_ctr_mc->col_cur is updated, so to reflect that cell was filled
 */
#define _PRT_CELL_SET(prt_ctr_mc, ...) \
{                                                                                                                     \
    char *prt_row_offset;                                                                                             \
    prt_column_t *prt_column_mc;                                                                                      \
    _PRT_CELL_VERIFY(prt_ctr_mc)                                                                                      \
    prt_column_mc = prt_ctr_mc->columns[prt_ctr_mc->col_cur];                                                         \
    prt_row_offset = (char *)prt_ctr_mc->row_cur + sizeof(rhentry_t) + prt_column_mc->cell_offset;                    \
    PRT_FORMAT(prt_row_offset, prt_column_mc->max_width, __VA_ARGS__);                                                \
    if (prt_column_mc->flex_type == PRT_FLEX_ASCII)                                                                   \
    {                                                                                                                 \
        utilex_str_shrink(prt_row_offset);                                                                            \
    }                                                                                                                 \
    prt_row_offset[prt_column_mc->max_width - 1] = 0;                                                                 \
    prt_ctr_mc->col_cur++;                                                                                            \
}

/**
 * \brief Fill cell with formatted data
 * \par DIRECT INPUT variadic variables
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \param [in] prt_n - number of shift in the cell, each shift is 2 spaces
 * \par INDIRECT OUTPUT prt_ctr_mc->col_cur is updated, so to reflect that cell was filled
 */
#define _PRT_CELL_SET_SHIFT(prt_ctr_mc, prt_n, ...) \
{                                                                                                                      \
    char *prt_row_offset;                                                                                              \
    prt_column_t *prt_column_mc;                                                                                       \
    _PRT_CELL_VERIFY(prt_ctr_mc)                                                                                       \
    prt_column_mc = prt_ctr_mc->columns[prt_ctr_mc->col_cur];                                                          \
    prt_row_offset = (char *)prt_ctr_mc->row_cur + sizeof(rhentry_t) + prt_column_mc->cell_offset;                     \
    {                                                                                                                  \
        int shift_num  = prt_n;                                                                                        \
        while(shift_num--)                                                                                             \
        {                                                                                                              \
            PRT_FORMAT(prt_row_offset + strlen(prt_row_offset), prt_column_mc->max_width, "  ");                       \
        }                                                                                                              \
    }                                                                                                                  \
    PRT_FORMAT(prt_row_offset + strlen(prt_row_offset), prt_column_mc->max_width, __VA_ARGS__);                        \
    prt_row_offset[prt_column_mc->max_width - 1] = 0;                                                                  \
    prt_ctr_mc->col_cur++;                                                                                             \
}

/**
 * \brief When filling table allows to skip cell
 * \par DIRECT INPUT
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \param [in] mc_skip_num - number of cells to be skipped
 * \par INDIRECT OUTPUT prt_ctr_mc->col_cur is updated, so to reflect that cell was filled
 */
#define _PRT_CELL_SKIP(prt_ctr_mc, mc_skip_num)                                                                        \
{                                                                                                                      \
    _PRT_CELL_VERIFY(prt_ctr_mc)                                                                                       \
    prt_ctr_mc->col_cur += mc_skip_num;                                                                                \
}

/**
 * \brief Add Info Line after the title and before actual table starts
 * \par DIRECT INPUT variadic variables
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \par INDIRECT INPUT prt_ctr_mc - Control structure containing information on table to be printed
 * \remark MACRO limits the size to RHNAME_MAX_SIZE
 */
#define _PRT_INFO_ADD(prt_ctr_mc, ...) \
{                                                                                                                      \
    rhhandle_t entry;                                                                                                  \
    if(prt_ctr_mc == NULL)                                                                                             \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Title was not defined\n");                                                      \
    }                                                                                                                  \
    if(prt_ctr_mc->info_list == NULL)                                                                                  \
    {                                                                                                                  \
        if((prt_ctr_mc->info_list = utilex_rhlist_create("prt_print", sizeof(prt_info_t), 0)) == NULL)                 \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_MEMORY,"info_list create %s", "failed\n");                                             \
        }                                                                                                              \
    }                                                                                                                  \
    if(utilex_rhlist_entry_add_tail(prt_ctr_mc->info_list, NULL, RHID_TO_BE_GENERATED, &entry) == _SHR_E_MEMORY)       \
    {                                                                                                                  \
         SHR_CLI_EXIT(_SHR_E_MEMORY,"add entry to info_list  %s", "failed\n");                                         \
    }                                                                                                                  \
    ((rhentry_t *)entry)->mode = PRT_ROW_SEP_NONE;                                                               \
    PRT_FORMAT(((prt_info_t *)entry)->content, PRT_TITLE_WIDTH - 1, __VA_ARGS__);                                                       \
}

/**
 * \brief Set separator for the last info - how it will be separated from previous line
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \par DIRECT INPUT
 * \param [in] mc_mode Type of separator from previous line. See PRT_ROW_SEP_MODE
 */
#define _PRT_INFO_SET_MODE(prt_ctr_mc, mc_mode)                                                                        \
{                                                                                                                      \
    rhentry_t *entry;                                                                                                  \
    if((entry = utilex_rhlist_entry_get_last(prt_ctr_mc->info_list)) == NULL)                                          \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_MEMORY,"Setting info mode while no info added\n");                                         \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        entry->mode = mc_mode;                                                                                         \
    }                                                                                                                  \
}

/**
 * \brief Low Level Add column routine
 * \par DIRECT INPUT variadic variables
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \param [in] flag - points to the FLEX type of the column to be created, either BINARY or ASCII, see PRT_FLEX_TYPE
 * \param [in] width - maximum width for cell in the column
 * \par INDIRECT INPUT prt_ctr_mc - Control structure containing information on table to be printed
 * \remark MACRO verifies the number of column does not exceed MAX
 */
#define _PRT_COLUMN_ADD(prt_ctr_mc, flag, width, ...)                                                                  \
{                                                                                                                      \
    prt_column_t *prt_column;                                                                                          \
    SHR_CLI_EXIT_IF_ERR(diag_sand_prt_column_add(prt_ctr_mc, flag, width, &prt_column),"");                            \
    PRT_FORMAT(prt_column->name, prt_column->max_width, __VA_ARGS__);                                                  \
}

#define _PRT_COLUMN_ALIGN(prt_ctr_mc)                                                                                  \
    if((prt_ctr_mc->col_num == 0) || (prt_ctr_mc->columns[prt_ctr_mc->col_num - 1] == NULL))                           \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Align for non-allocated column\n");                                               \
    }                                                                                                                  \
    prt_ctr_mc->columns[prt_ctr_mc->col_num - 1]->align = TRUE

/**
 * \brief Add column
 * \par DIRECT INPUT variadic variables
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \param [in] prt_type - type of XML node to be created, may be either PRT_XML_CHILD, PRT_XML_ATTRUBUTE or PRT_XML_NONE
 * \param [in] prt_parent_id - id for parent XML node
 * \par DIRECT OUTPUT
 * \param [out] prt_my_id_ptr - pointer for id of this column
 * \par INDIRECT INPUT prt_ctr_mc - Control structure containing information on table to be printed
 * \remark MACRO verifies the number of column does not exceed MAX
 */
#define _PRT_COLUMN_ADDX(prt_ctr_mc, prt_type, prt_parent_id, prt_my_id_ptr, ...)                                      \
{                                                                                                                      \
    _PRT_COLUMN_ADD(prt_ctr_mc, PRT_FLEX_NONE, PRT_COLUMN_WIDTH, __VA_ARGS__)                                          \
    {                                                                                                                  \
        int *loc_my_id_ptr = prt_my_id_ptr;                                                                            \
        prt_ctr_mc->columns[prt_ctr_mc->col_num - 1]->type      = prt_type;                                            \
        prt_ctr_mc->columns[prt_ctr_mc->col_num - 1]->parent_id = prt_parent_id;                                       \
        if(loc_my_id_ptr != NULL)                                                                                      \
            *loc_my_id_ptr = prt_ctr_mc->col_num - 1;                                                                  \
    }                                                                                                                  \
}

/**
 * \brief Add column
 * \par DIRECT INPUT variadic variables
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \param [in] flag - points to the FLEX type of the column to be created, either BINARY or ASCII, see PRT_FLEX_TYPE
 * \param [in] prt_type - type of XML node to be created, may be either PRT_XML_CHILD, PRT_XML_ATTRUBUTE or PRT_XML_NONE
 * \param [in] prt_parent_id - id for parent XML node
 * \par DIRECT OUTPUT
 * \param [out] prt_my_id_ptr - pointer for id of this column
 * \par INDIRECT INPUT prt_ctr_mc - Control structure containing information on table to be printed
 * \remark MACRO verifies the number of column does not exceed MAX
 */
#define _PRT_COLUMN_ADDX_FLEX(prt_ctr_mc, flag, prt_type, prt_parent_id, prt_my_id_ptr, ...)                           \
{                                                                                                                      \
    _PRT_COLUMN_ADD(prt_ctr_mc, flag, PRT_COLUMN_WIDTH_BIG, __VA_ARGS__)                                               \
    {                                                                                                                  \
        int *loc_my_id_ptr = prt_my_id_ptr;                                                                            \
        prt_ctr_mc->columns[prt_ctr_mc->col_num - 1]->type      = prt_type;                                            \
        prt_ctr_mc->columns[prt_ctr_mc->col_num - 1]->parent_id = prt_parent_id;                                       \
        if(loc_my_id_ptr != NULL)                                                                                      \
            *loc_my_id_ptr = prt_ctr_mc->col_num - 1;                                                                  \
    }                                                                                                                  \
}

/**
 * \brief Set Title String for tabular output, it may appear as most upper string on display and top XML node
 * \par DIRECT INPUT variadic variables
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \par INDIRECT OUTPUT
 *         prt_ctr_mc->columns[PRT_TITLE_ID] - title prt column is updated
 */
#define _PRT_TITLE_SET(prt_ctr_mc, ...)                                                                                \
{                                                                                                                      \
    if(prt_ctr_mc != NULL)                                                                                             \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Title may be defined only once\n");                                             \
    }                                                                                                                  \
    if((prt_ctr_mc = sal_alloc(sizeof(prt_control_t), "prt_control")) == NULL)                                         \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_MEMORY, "No memory for PRT table\n");                                                      \
    }                                                                                                                  \
    sal_memset(prt_ctr_mc, 0, sizeof(prt_control_t));                                                                  \
    if((prt_ctr_mc->columns[PRT_TITLE_ID] = sal_alloc(sizeof(*(prt_ctr_mc->columns[PRT_TITLE_ID])), "prt_title")) == NULL)  \
    {                                                                                                                  \
        SHR_CLI_EXIT(_SHR_E_MEMORY, "No memory for PRT title\n");                                                      \
    }                                                                                                                  \
    sal_memset(prt_ctr_mc->columns[PRT_TITLE_ID], 0, sizeof(*(prt_ctr_mc->columns[PRT_TITLE_ID])) );                    \
    prt_ctr_mc->unit = unit;                                                                                           \
    prt_ctr_mc->columns[PRT_TITLE_ID]->depth = 1;                                                                      \
    PRT_FORMAT(prt_ctr_mc->title, PRT_TITLE_WIDTH, __VA_ARGS__);                                                       \
    PRT_FORMAT(prt_ctr_mc->columns[PRT_TITLE_ID]->name, PRT_COLUMN_WIDTH, __VA_ARGS__);                                \
}

/**
 * \brief Check if the print title is set.
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \par DIRECT OUTPUT
 * \retval - return 1 if the title is already set, 0 if it is not.
 */
#define _PRT_TITLE_IS_SET(prt_ctr_mc) ((prt_ctr_mc != NULL) ? 1 : 0)

/**
 * \brief Return the total number of columns in the table Used to derive number of columns in the table
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \par DIRECT OUTPUT
 * \retval - number of columns
 */
#define _PRT_COLUMN_NUM(prt_ctr_mc)               prt_ctr_mc->col_num

/**
 * \brief Add new row in the table
 * \par DIRECT INPUT
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \param [in] mc_mode Points to the type of separator from previous line. See PRT_ROW_SEP_MODE
 * \par INDIRECT INPUT prt_list - pointer to the list of rows, that this one will be added to
 *                     prt_row_offset - current place in the row to be written (cell pointer)
 *                     prt_row_start - pointer to the beginning of row
 * \par INDIRECT OUTPUT prt_list - new row will be added to this list
 *                      prt_row_start - pointer to the beginning of new row
 *                      prt_row_offset - set to the beginning of new row skipping control info
 * \remark First row filled followed by ROW_COLUMN_SET is considered of being header one
 */
#define _PRT_ROW_ADD(prt_ctr_mc, mc_mode) \
{                                                                                                                      \
    prt_ctr_mc->col_cur = 0;                                                                                           \
    if(prt_ctr_mc->list == NULL)                                                                                       \
    {                                                                                                                  \
        if((prt_ctr_mc->list = utilex_rhlist_create("prt_print", sizeof(rhentry_t) + prt_ctr_mc->row_width, 0)) == NULL)     \
        {                                                                                                              \
            SHR_CLI_EXIT(_SHR_E_MEMORY,"prt_list create %s", "failed\n");                                              \
        }                                                                                                              \
    }                                                                                                                  \
    if(utilex_rhlist_entry_add_tail(prt_ctr_mc->list, NULL, RHID_TO_BE_GENERATED, &(prt_ctr_mc->row_cur)) == _SHR_E_MEMORY)    \
    {                                                                                                                  \
         SHR_CLI_EXIT(_SHR_E_MEMORY,"add entry to prt_list  %s", "failed\n");                                          \
    }                                                                                                                  \
    ((rhentry_t *)(prt_ctr_mc->row_cur))->mode = mc_mode;                                                              \
}

/**
 * \brief Delete Last Row in the table
 * \par DIRECT INPUT
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \remark Last Roe will be deleted, used when in process of PRT we decide to skip certain row
 */
#define _PRT_ROW_DELETE(prt_ctr_mc) \
{                                                                                                                      \
    prt_ctr_mc->col_cur = 0;                                                                                           \
    if(prt_ctr_mc->list != NULL)                                                                                       \
    {                                                                                                                  \
        rhhandle_t *rhentry = utilex_rhlist_entry_get_last(prt_ctr_mc->list);                                          \
        utilex_rhlist_entry_del_free(prt_ctr_mc->list, rhentry);                                                       \
    }                                                                                                                  \
}

/**
 * \brief Return number of rows in the table
 * \param [in] prt_ctr_mc - pointer to the control structure
 */
#define _PRT_ROW_NUM(prt_ctr_mc)     ((prt_ctr_mc->list != NULL) ? RHLNUM(prt_ctr_mc->list) : 0)

/**
 * \brief Set mode for the last row
 * \param [in] prt_ctr_mc - pointer to the control structure
 * \par DIRECT INPUT
 * \param [in] mc_mode Type of separator from previous line. See PRT_ROW_SEP_MODE
 */
#define _PRT_ROW_SET_MODE(prt_ctr_mc, mc_mode)                                                                         \
{                                                                                                                      \
    if(prt_ctr_mc->row_cur == NULL)                                                                                    \
    {   /* If there are no rows yet - add one to assign mode to */                                                     \
        _PRT_ROW_ADD(prt_ctr_mc, mc_mode);                                                                             \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        ((rhentry_t *)(prt_ctr_mc->row_cur))->mode = mc_mode;                                                          \
    }                                                                                                                  \
}
/*
 * End of internal pretty printing facilities
 * }
 */

/*
 * Below MACROs are for external usage when only 1 table is required
 * {
 */
/**
 * \brief
 *    1. Declares pointer to control structure used by pretty printing
 *    2. Invocation should be before any code start
 *    3. Pointer declared here may be passed down the line in order to have an ability to add cells to the table over
 *       multiple routines
 *    4. The requirement is to invoke PRT_TITLE_SET, PRT_COMMIT and PRT_FREE only from the routine with PRT_INIT_VARS
 */
#define PRT_INIT_VARS                prt_control_t *prt_ctr = NULL

/**
 * \brief
 *    1. Should be set once per table
 *    2. Accepts regular printf format
 * \par DIRECT INPUT variadic variables
 */
#define PRT_TITLE_SET(...)          _PRT_TITLE_SET(prt_ctr, __VA_ARGS__)
#define PRT_TITLE_IS_SET            _PRT_TITLE_IS_SET(prt_ctr)
/**
 * \brief Add Info
 * \par DIRECT INPUT variadic variables
 * \par INDIRECT INPUT prt_ctr_mc - Control structure containing information on table to be printed
 * \remark
 *    1. MACRO limits the size to RHNAME_MAX_SIZE - 1
 *    2. In XML output all infos will be added to special node at the beginning of the file
 */
#define PRT_INFO_ADD(...)           _PRT_INFO_ADD(prt_ctr, __VA_ARGS__)

/**
 * \brief Set mode for the last row
 * \par DIRECT INPUT
 * \param [in] mc_mode Type of separator from previous line. See PRT_ROW_SEP_MODE
 * \remark
 *    Used when the mode could be figured only after it is filled, e.g. when we understand the the row we printed is the
 *    last for certain logic
 */
#define PRT_INFO_SET_MODE(mc_mode)       _PRT_INFO_SET_MODE(prt_ctr, mc_mode)

/**
 * \brief Add column to the table with text provided in regular printf format
 * \par DIRECT INPUT variadic variables
 * \par INDIRECT INPUT prt_ctr_mc - Control structure containing information on table to be printed
 * \remark
 *    MACRO verifies the number of column does not exceed MAX
 */
#define PRT_COLUMN_ADD(...) _PRT_COLUMN_ADD(prt_ctr, PRT_FLEX_NONE, PRT_COLUMN_WIDTH, __VA_ARGS__)

/**
 * \brief
 *    1. Allows to have cells with size over RHNAME_MAX_SIZE, where cell content is split over multiple rows in the
 *       same column up to PRT_COLUMN_WIDTH_BIG - 1
 *    2. When column header is bigger than max cell size in the column it will be split over the multiple rows inside
 *       header line
 * \par DIRECT INPUT variadic variables
 * \param [in] flag - points to the FLEX type of the column to be created, either BINARY or ASCII, see PRT_FLEX_TYPE
 * \par INDIRECT INPUT prt_ctr_mc - Control structure containing information on table to be printed
 * \remark
 *    1. MACRO verifies the number of column does not exceed MAX
 *    2. Use binary for long numbers presentation
 *    3. Use ASCII for string output
 */
#define PRT_COLUMN_ADD_FLEX(flag, ...)  _PRT_COLUMN_ADD(prt_ctr, flag, PRT_COLUMN_WIDTH_BIG, __VA_ARGS__)

/**
 * \brief Add column with specific instructions on XML location
 * \par DIRECT INPUT variadic variables
 * \param [in] prt_type - type of XML node to be created, may be either PRT_XML_CHILD, PRT_XML_ATTRUBUTE or PRT_XML_NONE
 * \param [in] prt_parent_id - id for parent XML node
 * \param [out] prt_my_id_ptr - pointer for id of this column
 * \par INDIRECT INPUT prt_ctr_mc - Control structure containing information on table to be printed
 * \remark
 *    1. Used in the cases where automatic XML placement does not work, so you need to state explicitly which type do
 *       you want this column to be. See PRT_XML_TYPE for options
 *    2. MACRO verifies the number of column does not exceed MAX
 */
#define PRT_COLUMN_ADDX(prt_type, prt_parent_id, prt_my_id_ptr, ...) \
                    _PRT_COLUMN_ADDX(prt_ctr, prt_type, prt_parent_id, prt_my_id_ptr, __VA_ARGS__)
/**
 * \brief
 *    Combination of PRT_COLUMN_ADDX with flexibility option, see above
 * \param [in] flag - points to the FLEX type of the column to be created, either BINARY or ASCII, see PRT_FLEX_TYPE
 * \param [in] prt_type - type of XML node to be created, may be either PRT_XML_CHILD, PRT_XML_ATTRUBUTE or PRT_XML_NONE
 * \param [in] prt_parent_id - id for parent XML node
 * \param [out] prt_my_id_ptr - pointer for id of this column
 */
#define PRT_COLUMN_ADDX_FLEX(flag, prt_type, prt_parent_id, prt_my_id_ptr, ...)                                       \
                    _PRT_COLUMN_ADDX_FLEX(prt_ctr, flag, prt_type, prt_parent_id, prt_my_id_ptr, __VA_ARGS__)

/**
 * \brief
 *    Aligns current column content to the right. By default all columns are aligned to the left
 *    Should be called right after COLUMN_ADD
 */
#define PRT_COLUMN_ALIGN                _PRT_COLUMN_ALIGN(prt_ctr)

/**
 * \brief
 *    Returns current column number, is useful to calculate how many columns should be skipped when dealing with table,
 *    where number an order of columns is parameter dependent
 */
#define PRT_COLUMN_NUM                  _PRT_COLUMN_NUM(prt_ctr)

/**
 * \brief Set mode for the last row
 * \par DIRECT INPUT
 * \param [in] mc_mode Type of separator from previous line. See PRT_ROW_SEP_MODE
 * \remark
 *    Used when the mode could be figured only after it is filled, e.g. when we understand the the row we printed is the
 *    last for certain logic
 */
#define PRT_ROW_SET_MODE(mc_mode)       _PRT_ROW_SET_MODE(prt_ctr, mc_mode)

/**
 * \brief Add new row in the table
 * \par DIRECT INPUT
 * \param [in] mc_mode Points to the type of separator from previous line. See PRT_ROW_SEP_MODE
 * \par INDIRECT INPUT prt_list - pointer to the list of rows, that this one will be added to
 *                     prt_row_offset - current place in the row to be written (cell pointer)
 *                     prt_row_start - pointer to the beginning of row
 * \par INDIRECT OUTPUT prt_list - new row will be added to this list
 *                      prt_row_start - pointer to the beginning of new row
 *                      prt_row_offset - set to the beginning of new row skipping control info
 * \remark First row filled followed by ROW_COLUMN_SET is considered of being header one
 */
#define PRT_ROW_ADD(mc_mode)            _PRT_ROW_ADD(prt_ctr, mc_mode)

/**
 * \brief Add new row in the table
 * \remark Last row in the table to be deleted due to lack of interest in its content
 */
#define PRT_ROW_DELETE()            _PRT_ROW_DELETE(prt_ctr)

/**
 * \brief
 *    Returns current row number, is useful to figure out if there were rows at all and how many of them
 */
#define PRT_ROW_NUM()               _PRT_ROW_NUM(prt_ctr)

/**
 * \brief Fill cell with formatted data
 * \par INDIRECT OUTPUT prt_ctr_mc->col_cur is updated, so to reflect that cell was filled
 * \remark
 *    1. Maximum size of regular cell is PRT_COLUMN_WIDTH - 1
 *    2. In the case of flexible column size may be up to PRT_COLUMN_WIDTH_BIG - 1
 *    3. So cell with length between these 2 values will be split either accordingly - see PRT_COLUMN_ADD_FLEX
 */
#define PRT_CELL_SET(...)               _PRT_CELL_SET(prt_ctr, __VA_ARGS__)
/**
 * \brief same as PRT_CELL_SET with only difference being shift inside the cell for indentation purposes
 * \param [in] prt_n - number of shift in the cell, each shift is 2 spaces
 * \par INDIRECT OUTPUT prt_ctr_mc->col_cur is updated, so to reflect that cell was filled
 * \remark
 *    Used when we don't want to add additional column, but rather indent certain places in the same column
 */
#define PRT_CELL_SET_SHIFT(prt_n, ...)  _PRT_CELL_SET_SHIFT(prt_ctr, prt_n, __VA_ARGS__)

/**
 * \brief When filling table allows to skip cell
 * \param [in] mc_skip_num - number of cells to be skipped
 * \par INDIRECT OUTPUT prt_ctr_mc->col_cur is updated, so to reflect that cell was filled
 * \remark
 *     No need to do SKIP at the end of ROW, anyway new row will start from column
 */
#define PRT_CELL_SKIP(mc_skip_num)      _PRT_CELL_SKIP(prt_ctr, mc_skip_num)

/**
 * \brief Actual printing of prepared data, may be used inside or outside framework
 * \remark
 *    Does not support XML output, see PRT_COMMITF for XML output outside framework
 */
#define PRT_COMMIT                      _PRT_COMMIT(prt_ctr)

/**
 * \brief Actual printing of prepared data to be used only in the context of shell framework
 * \remark
 *    Requires presence of sand_control, which is implicitly provided by the shell command invocation parameters, and
 *    should be transfered to underlying routines in the case of PRT usage down stack.
 *    This macro allows to transfer implicitly cli options to PRT - e.g XML file output
 *    Frees the list
 */
#define PRT_COMMITX                     _PRT_COMMITX(prt_ctr)

/**
 * \brief Actual printing of prepared data to be used, when outside shell framework XML file output may be requested
 * \param [in] filename_str - XML file to be used for data export, if NULL the one from sand_control may be used
 * \remark
 *    Requires presence of sand_control, which is implicitly provided by the shell command invocation parameters, and
 *    should be transfered to underlying routines in the case of PRT usage down stack.
 *    This macro allows to transfer implicitly cli options to PRT - e.g XML file output
 *    Frees the list
 */
#define PRT_COMMITF(filename_str)       _PRT_COMMITF(prt_ctr, filename_str)

/**
 * \brief Free control structure allocated by SET_TITLE
 * \remark
 *     Should be always used at the end(usually after exit: label) of routine using pretty printing to catch all errors
 */
#define PRT_FREE                        _PRT_FREE(prt_ctr)
/*
 * End of facilities for external usage when only 1 table is required
 * }
 */

/*
 * Below MACROs are for external usage when only Simultaneous Multiple Table are required
 * All MACROs below will have _PL (plural) as a mark for this purpose
 * All MACROS are exactly the same as regular ones with the only difference being additional parameter of table id
 * {
 */
/**
 * \brief Declares pointer to control structure used by pretty printing
 * \param [in] mc_num - number of tables required for simultaneous filling
 */
#define PRT_INIT_VARS_PL(mc_num)            prt_control_t *prt_ctr[mc_num] = {NULL}

/**
 * All MACROS are exactly the same as regular ones with the only difference being additional parameter of table mc_id
 * See one table macros for description
 */
#define PRT_TITLE_SET_PL(mc_id, ...)             _PRT_TITLE_SET(prt_ctr[mc_id], __VA_ARGS__)
#define PRT_INFO_ADD_PL(mc_id, ...)              _PRT_INFO_ADD(prt_ctr[mc_id], __VA_ARGS__)
#define PRT_COLUMN_ADD_PL(mc_id, ...)                                                                                  \
                        _PRT_COLUMN_ADD(prt_ctr[mc_id], PRT_FLEX_NONE, PRT_COLUMN_WIDTH, __VA_ARGS__)
#define PRT_COLUMN_ADD_FLEX_PL(mc_id, flag, ...)                                                                       \
                        _PRT_COLUMN_ADD(prt_ctr[mc_id], flag, PRT_COLUMN_WIDTH_BIG, __VA_ARGS__)
#define PRT_COLUMN_ADDX_PL(mc_id, prt_type, prt_parent_id, prt_my_id_ptr, ...) \
                        _PRT_COLUMN_ADDX(prt_ctr[mc_id], prt_type, prt_parent_id, prt_my_id_ptr, __VA_ARGS__)
#define PRT_COLUMN_ADDX_FLEX_PL(mc_id, flag, prt_type, prt_parent_id, prt_my_id_ptr, ...)                              \
                        _PRT_COLUMN_ADDX_FLEX(prt_ctr[mc_id], flag, prt_type, prt_parent_id, prt_my_id_ptr, __VA_ARGS__)
#define PRT_COLUMN_NUM_PL(mc_id)                 _PRT_COLUMN_NUM(prt_ctr[mc_id])
#define PRT_ROW_SET_MODE_PL(mc_id, mc_mode)      _PRT_ROW_SET_MODE(prt_ctr[mc_id], mc_mode)
#define PRT_ROW_ADD_PL(mc_id, mc_mode)           _PRT_ROW_ADD(prt_ctr[mc_id], mc_mode)
#define PRT_ROW_NUM_PL(mc_id)                    _PRT_ROW_NUM(prt_ctr[mc_id])
#define PRT_CELL_SET_PL(mc_id, ...)              _PRT_CELL_SET(prt_ctr[mc_id], __VA_ARGS__)
#define PRT_CELL_SET_SHIFT_PL(mc_id, prt_n, ...) _PRT_CELL_SET_SHIFT(prt_ctr[mc_id], prt_n, __VA_ARGS__)
#define PRT_CELL_SKIP_PL(mc_id, mc_skip_num)     _PRT_CELL_SKIP(prt_ctr[mc_id], mc_skip_num)
#define PRT_COMMIT_PL(mc_id)                     _PRT_COMMIT(prt_ctr[mc_id])
#define PRT_COMMITX_PL(mc_id)                    _PRT_COMMITX(prt_ctr[mc_id])
#define PRT_COMMITF_PL(mc_id, filename_str)      _PRT_COMMITF(prt_ctr[mc_id], filename_str)
#define PRT_FREE_PL(mc_id)                       _PRT_FREE(prt_ctr[mc_id])

#define PRT_COMMITX_COMBINE_PL(mc_num)         _PRT_COMMITX_COMBINE(prt_ctr, mc_num)
/*
 * End of facilities for external usage for multiple tables
 * }
 */

#endif /* DIAG_SAND_PRT_H */
