/* 
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <phymod/phymod_util.h>
#include <phymod/acc/phymod_tsc_iblk.h>


#define BLACKHAWK_DEVICE_MODEL 0x26
#define PMD_DEVICE_MODE_MASK   0x3f

int phymod_util_lane_config_get(const phymod_access_t *phys, int *start_lane, int *num_of_lane)
{
    int i ;
    uint32_t val, is_bh = 0;

    /* first read PMD device id */
    PHYMOD_IF_ERR_RETURN
        (phymod_tscbh_iblk_read(phys, 0x1d100, &val));
    if ((val & PMD_DEVICE_MODE_MASK) == BLACKHAWK_DEVICE_MODEL) {
        is_bh = 1;
    }

    switch (phys->lane_mask) {
        case 0x1:
            *start_lane = 0;
            *num_of_lane = 1;
            break;
        case 0x2:
            *start_lane = 1;
            *num_of_lane = 1;
            break;
        case 0x4:
            *start_lane = 2;
            *num_of_lane = 1;
            break;
        case 0x8:
            *start_lane = 3;
            *num_of_lane = 1;
            break;
        case 0x3:
            *start_lane = 0;
            *num_of_lane = 2;
            break;
        case 0xc:
            *start_lane = 2;
            *num_of_lane = 2;
            break;
        case 0x7:
            *start_lane = 0;
            *num_of_lane = 3;
            break;
        case 0xf:
            *start_lane = 0;
            *num_of_lane = 4;
            break;
        case 0x10:
            *start_lane = 4;
            *num_of_lane = 1;
            break;
        case 0x20:
            *start_lane = 5;
            *num_of_lane = 1;
            break;
        case 0x40:
            *start_lane = 6;
            *num_of_lane = 1;
            break;
        case 0x80:
            *start_lane = 7;
            *num_of_lane = 1;
            break;
        case 0x30:
            *start_lane = 4;
            *num_of_lane = 2;
            break;
        case 0xc0:
            *start_lane = 6;
            *num_of_lane = 2;
            break;
        case 0xf0:
            *start_lane = 4;
            *num_of_lane = 4;
            break;
        case 0xff:
            *start_lane = 0;
            *num_of_lane = 8;
            break;
        case 0x100:
            *start_lane = 8;
            *num_of_lane = 1;
            break;
        case 0x200:
            *start_lane = 9;
            *num_of_lane = 1;
            break;
        case 0x400:
            *start_lane = 10;
            *num_of_lane = 1;
            break;
        case 0x800:
            *start_lane = 11;
            *num_of_lane = 1;
            break;
        case 0x1000:
            *start_lane = 12;
            *num_of_lane = 1;
            break;
        case 0x2000:
            *start_lane = 13;
            *num_of_lane = 1;
            break;
        case 0x4000:
            *start_lane = 14;
            *num_of_lane = 1;
            break;
        case 0x8000:
            *start_lane = 15;
            *num_of_lane = 1;
            break;
        default:
            /*Support non-consecutive lanes*/
            for(i = 0; i < 16; i++)
            {
                if(phys->lane_mask & (1 << i))
                {
                    *start_lane = i;
                    break;
                }
            }
            if (is_bh) {
                *num_of_lane = 8;
            } else {
                *num_of_lane = 4;
            }
    }
    return PHYMOD_E_NONE;
}

int phymod_core_name_get(const phymod_core_access_t *core,  uint32_t serdes_id, char *core_name, phymod_core_info_t *info)
{
    char* rev_char[4]={"A", "B", "C", "D"};
    char rev_num[]="7";
    uint8_t rev_letter, rev_number;

    info->serdes_id = serdes_id;
    rev_letter = (serdes_id & 0xc000) >> 14;
    rev_number = (serdes_id & 0x3800) >> 11;
    PHYMOD_STRCAT(core_name, rev_char[rev_letter]);
    PHYMOD_SNPRINTF(rev_num, 2, "%d", rev_number);
    PHYMOD_STRCAT(core_name, rev_num);
    PHYMOD_STRNCPY(info->name, core_name, PHYMOD_STRLEN(core_name)+1);

    return PHYMOD_E_NONE;
}

int phymod_swap_bit(uint16_t original_value, uint16_t *swapped_val)
{
    uint16_t new_val, temp_val;
    uint16_t i;

    new_val = 0;
    for (i = 0; i < 16; i++) {
        temp_val = 0x0;
        temp_val = (original_value & (0x1 << (15 - i))) >> (15 -i);
        temp_val = (temp_val << i) & (0x1 << i);
        new_val |= temp_val;
   }
   *swapped_val = new_val;
    return PHYMOD_E_NONE;
}

int phymod_util_lane_mask_get(int start_lane, int num_of_lane, uint32_t *lane_mask)
{
    int i ;
    uint32_t temp_mask = 0;

    for (i = 0; i <num_of_lane; i++) {
        temp_mask |= 1 << (start_lane + i);
    }

    *lane_mask = temp_mask;

    return PHYMOD_E_NONE;
}
