
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT

#ifdef PORTMOD_SUPPORT

#include <soc/dnxf/cmn/dnxf_lane_map_db.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxc/dnxc_port.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>

shr_error_e
soc_dnxf_lane_map_db_map_input_check(
    int unit,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes)
{
    int sw_state_map_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lane2serdes, _SHR_E_PARAM, "lane2serdes");

    SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.serdes_map.map_size.get(unit, &sw_state_map_size));
    if (map_size != sw_state_map_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid map_size parameter %d. map_size is supposed to be %d\n", map_size,
                     sw_state_map_size);
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_lane_map_db_link_to_fmac_lane_get(
    int unit,
    bcm_port_t link,
    int *fmac_lane)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.link_to_fmac_lane_map.get(unit, link, fmac_lane));

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxf_lane_map_db_fmac_lane_to_link_get(
    int unit,
    int fmac_lane,
    bcm_port_t * link)
{
    int fmac_temp;
    bcm_port_t link_iter;

    SHR_FUNC_INIT_VARS(unit);

    PBMP_SFI_ITER(unit, link_iter)
    {
        SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.link_to_fmac_lane_map.get(unit, link_iter, &fmac_temp));

        if (fmac_temp == fmac_lane)
        {
            *link = link_iter;
            SHR_EXIT();
        }
    }

    *link = -1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_lane_map_db_map_size_align_get(
    int unit,
    int map_size,
    int *map_size_aligned)
{
    int nof_links = dnxf_data_port.general.nof_links_get(unit);
    int max_link;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_port_max_port_get(unit, &max_link));

    *map_size_aligned = map_size * (nof_links / max_link);

exit:
    SHR_FUNC_EXIT;
}

#endif

#undef BSL_LOG_MODULE
