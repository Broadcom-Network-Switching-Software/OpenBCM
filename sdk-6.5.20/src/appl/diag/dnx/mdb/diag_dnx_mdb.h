/*
 * ! \file mdb_diag.h Contains all of the MDB diag&test declarations for external usage
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_MDB_H_INCLUDED
#define DIAG_DNX_MDB_H_INCLUDED

#include <appl/diag/sand/diag_sand_framework.h>

extern sh_sand_man_t sh_dnx_mdb_man;
extern sh_sand_cmd_t sh_dnx_mdb_cmds[];

/**
 * \brief
 * Verify the KBPSDK version
 */
shr_error_e mdb_diag_verify_kbpsdk(
    int unit);

#endif /* DIAG_DNX_MDB_H_INCLUDED */
