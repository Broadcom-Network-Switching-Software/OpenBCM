
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_EM_LAYOUT_H__
#define __MDB_EM_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int mdb_em_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_MDB_EM_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(MDB_EM_MODULE_ID, 0),
    DNX_SW_STATE_MDB_EM_DB__SHADOW_EM_DB,
    DNX_SW_STATE_MDB_EM_DB__VMV_INFO,
    DNX_SW_STATE_MDB_EM_DB__MACT_STAMP,
    DNX_SW_STATE_MDB_EM_DB__VMV_INFO__VALUE,
    DNX_SW_STATE_MDB_EM_DB__VMV_INFO__SIZE,
    DNX_SW_STATE_MDB_EM_DB__VMV_INFO__ENCODING_MAP,
    DNX_SW_STATE_MDB_EM_DB__VMV_INFO__APP_ID_SIZE,
    DNX_SW_STATE_MDB_EM_DB__VMV_INFO__MAX_PAYLOAD_SIZE,
    DNX_SW_STATE_MDB_EM_DB__VMV_INFO__ENCODING_MAP__ENCODING,
    DNX_SW_STATE_MDB_EM_DB__VMV_INFO__ENCODING_MAP__SIZE,
    
    DNX_SW_STATE_MDB_EM_DB_NOF_PARAMS,
} dnxc_sw_state_layout_mdb_em_node_id_e;

#endif 
