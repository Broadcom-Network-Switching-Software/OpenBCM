/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/phymod_util.h>

#ifdef PHYMOD_TSCBH_SUB10G_SUPPORT

#include <phymod/chip/tscbh_sub10g.h>
#include <phymod/chip/tscbh_sub10g_diagnostics.h>
#include <tscbh_sub10g/tier1/tbhmod_sub10g.h>

int tscbh_sub10g_phy_pcs_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN (temod2pll_diag_disp(&phy_copy.access, type));
    return PHYMOD_E_NONE;
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_fec_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    PHYMOD_IF_ERR_RETURN(temod2pll_fec_correctable_counter_get(&phy->access, count));

    return PHYMOD_E_NONE;
}



int tscbh_sub10g_phy_fec_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    /* Place your code here */
    PHYMOD_IF_ERR_RETURN(temod2pll_fec_uncorrectable_counter_get(&phy->access, count));

    return PHYMOD_E_NONE;

}

int tscbh_sub10g_phy_fec_error_inject_set(const phymod_phy_access_t* phy, uint16_t error_control_map, phymod_fec_error_mask_t bit_error_mask)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    uint16_t valid_error_mask = 0xf;
    uint32_t fec_enable = 0;
    temod2pll_fec_error_mask_t fec_error_mask;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = TSCBH_SUB10G_LANE_MASK_LOWER(phy->access.lane_mask) ?
                                TSCBH_SUB10G_LANE_MASK_LOWER(phy->access.lane_mask) :
                                TSCBH_SUB10G_LANE_MASK_UPPER(phy->access.lane_mask);
    fec_error_mask.error_mask_bit_31_0 = bit_error_mask.error_mask_bit_31_0;
    fec_error_mask.error_mask_bit_63_32 = bit_error_mask.error_mask_bit_63_32;
    fec_error_mask.error_mask_bit_79_64 = bit_error_mask.error_mask_bit_79_64;

    PHYMOD_IF_ERR_RETURN(temod2pll_fecmode_get(&phy->access, &fec_enable));
    if (!fec_enable) {
        PHYMOD_DEBUG_ERROR((" %s:%d fec is NOT enabled!\n", __func__, __LINE__));
        return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy_copy.access, &start_lane, &num_lane));

    /* valid error_control_map(bit mask):
     * single lane port     :  [0:0]
     * dual lane port       :  [1:0]
     * 4-lane port          :  [3:0]
     * all lanes on the port:  0xff
     *
     * If the error_control_map is not 0xff, needs to translate it to lane_mask.
     * Otherwise, use the lane_mask directly.
     */
    if (error_control_map != 0xff) {
        switch (num_lane) {
            case 0x1:
                valid_error_mask = 0x1;
                break;
            case 0x2:
                valid_error_mask = 0x3;
                phy_copy.access.lane_mask = (error_control_map & 0x3) << start_lane;
                break;
            case 0x4:
                valid_error_mask = phy->access.lane_mask;
                phy_copy.access.lane_mask = error_control_map & 0xf;
                break;
            default:
                return PHYMOD_E_CONFIG;
        }
        if (error_control_map & (~valid_error_mask)) {
            PHYMOD_DEBUG_ERROR((" %s:%d error_control_map is wrong! valid mask range 0x%x\n", __func__, __LINE__, valid_error_mask));
            return PHYMOD_E_CONFIG;
        }
    }

    PHYMOD_IF_ERR_RETURN(temod2pll_fec_error_inject_set(&phy_copy.access, error_control_map, fec_error_mask));

    return PHYMOD_E_NONE;
}



int tscbh_sub10g_phy_fec_error_inject_get(const phymod_phy_access_t* phy, uint16_t* error_control_map, phymod_fec_error_mask_t* bit_error_mask)
{
    temod2pll_fec_error_mask_t fec_error_mask;
    uint16_t control_map = 0;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = TSCBH_SUB10G_LANE_MASK_LOWER(phy->access.lane_mask) ?
                                TSCBH_SUB10G_LANE_MASK_LOWER(phy->access.lane_mask) :
                                TSCBH_SUB10G_LANE_MASK_UPPER(phy->access.lane_mask);
    PHYMOD_IF_ERR_RETURN(temod2pll_fec_error_inject_get(&phy_copy.access, &control_map, &fec_error_mask));

    bit_error_mask->error_mask_bit_31_0 = fec_error_mask.error_mask_bit_31_0;
    bit_error_mask->error_mask_bit_63_32 = fec_error_mask.error_mask_bit_63_32;
    bit_error_mask->error_mask_bit_79_64 = fec_error_mask.error_mask_bit_79_64;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy_copy.access, &start_lane, &num_lane));

    switch (num_lane) {
        case 0x1:
            *error_control_map = (control_map >> start_lane) & 0x1;
            break;
        case 0x2:
            *error_control_map = (control_map >> start_lane) & 0x3;
            break;
        case 0x4:
            *error_control_map = control_map & 0xf;
            break;
        default:
            return PHYMOD_E_CONFIG;
    }

    return PHYMOD_E_NONE;
}


#endif /* PHYMOD_TSCBH_SUB10G_SUPPORT */
