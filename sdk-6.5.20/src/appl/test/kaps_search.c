/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Test for KAPS search interface
 */

#include <soc/defs.h>
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_diag.h>
#include <sal/types.h>
#include <appl/diag/test.h>
#include <appl/diag/shell.h>

int
kaps_search_test(int u, args_t *a, void *p)
{
    int rv = 0;
    parse_table_t pt;
    uint32 add_entries = 1, search_entries = 1, delete_entries = 1, loop_test = 1, cache_test = 0;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"add"         ,PQ_INT , (void *) (1)             , &add_entries     , NULL);
        parse_table_add(&pt,"search"      ,PQ_INT , (void *) (1)             , &search_entries  , NULL);
        parse_table_add(&pt,"delete"      ,PQ_INT , (void *) (1)             , &delete_entries  , NULL);
        parse_table_add(&pt,"loop"        ,PQ_INT , (void *) (1)             , &loop_test  , NULL);
        parse_table_add(&pt,"cache"        ,PQ_INT , (void *) (0)             , &cache_test  , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }

    rv = jer_pp_kaps_search_test(u, add_entries, search_entries, delete_entries, loop_test, cache_test);

    if (rv < 0)
    {
        test_error(u, "KAPS SEARCH TEST Failed!!!\n");
        return rv;
    }

    return rv;
}

#endif

