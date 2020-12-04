/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/phymod_util.h>
#include "falcon16_v1l4p1/tier1/src/falcon16_v1l4p1_cfg_seq.h"
#include "falcon16_v1l4p1/tier1/include/falcon16_v1l4p1_diag.h"
#include "falcon16_v1l4p1/tier1/include/falcon16_v1l4p1_access.h"
#include "falcon16_v1l4p1/tier1/include/falcon16_v1l4p1_config.h"

#ifdef PHYMOD_FALCON16_V1L4P1_SUPPORT

int falcon16_v1l4p1_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int falcon16_v1l4p1_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int falcon16_v1l4p1_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{        
    unsigned char rx_lock;
    int osr_mode;
    int div2p5 = 0;
    int div2p5_27g = 0;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phymod_diag_eyescan_t_init(&diag->eyescan);
    phymod_diag_slicer_offset_t_init(&diag->slicer_offset);

    PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_pmd_lock_status(&phy_copy.access, &rx_lock));
    diag->rx_lock = (uint32_t ) rx_lock;
    PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_osr_mode_get(&phy_copy.access, &osr_mode));
    switch(osr_mode) {
        case 0:
            PHYMOD_IF_ERR_RETURN
                (falcon16_v1l4p1_10g_clock_enable_get(&phy->access, &div2p5, &div2p5_27g));
            if (div2p5 || div2p5_27g) {
                diag->osr_mode = phymodOversampleMode2P5;
            } else {
                diag->osr_mode = phymodOversampleMode1;
            }
            break;
        case 1: diag->osr_mode = phymodOversampleMode2; break;
        case 2: diag->osr_mode = phymodOversampleMode4; break;
        case 5: diag->osr_mode = phymodOversampleMode8; break;
        case 8: diag->osr_mode = phymodOversampleMode16P5; break;
        case 11: diag->osr_mode = phymodOversampleMode8P25; break;
        case 12: diag->osr_mode = phymodOversampleMode20P625; break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("unsupported OS mode %d"), osr_mode));
    }
    PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_signal_detect(&phy_copy.access, &diag->signal_detect));

    return PHYMOD_E_NONE;
}


int falcon16_v1l4p1_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

static void _falcon16_v1l4p1_diag_uc_reg_dump(const phymod_access_t *pa)
{
    err_code_t errc;

    COMPILER_REFERENCE(errc);

    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    MICRO CODE USR CTRL CONFIGURATION REGISTERS  |\n"));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    config_word              [0x00]: 0x%04X      |\n",    falcon16_v1l4p1_rdwl_uc_var(pa,&errc,0x0)));
    PHYMOD_DIAG_OUT(("|    retune_after_restart     [0x02]: 0x%04X      |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x2)));
    PHYMOD_DIAG_OUT(("|    clk90_offset_adjust      [0x03]: 0x%04X      |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x3)));
    PHYMOD_DIAG_OUT(("|    clk90_offset_override    [0x04]: 0x%04X      |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x4)));
    PHYMOD_DIAG_OUT(("|    lane_event_log_level     [0x05]: 0x%04X      |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x5)));
    PHYMOD_DIAG_OUT(("|    disable_startup          [0x06]: 0x%04X      |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x6)));
    PHYMOD_DIAG_OUT(("|    disable_startup_dfe      [0x07]: 0x%04X      |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x7)));
    PHYMOD_DIAG_OUT(("|    disable_steady_state     [0x08]: 0x%04X      |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x8)));
    PHYMOD_DIAG_OUT(("|    disable_steady_state_dfe [0x09]: 0x%04X      |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x9)));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|         MICRO CODE USER STATUS REGISTERS        |\n"));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    restart_counter           [0x0A]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0xa)));
    PHYMOD_DIAG_OUT(("|    reset_counter             [0x0B]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0xb)));
    PHYMOD_DIAG_OUT(("|    pmd_lock_counter          [0x0C]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0xc)));
    PHYMOD_DIAG_OUT(("|    heye_left                 [0x0D]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0xd)));
    PHYMOD_DIAG_OUT(("|    heye_left_lsbyte          [0x0E]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0xe)));
    PHYMOD_DIAG_OUT(("|    heye_right                [0x0F]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0xf)));
    PHYMOD_DIAG_OUT(("|    heye_right_lsbyte         [0x10]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x10)));
    PHYMOD_DIAG_OUT(("|    veye_upper                [0x11]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x11)));
    PHYMOD_DIAG_OUT(("|    veye_upper_lsbyte         [0x12]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x12)));
    PHYMOD_DIAG_OUT(("|    veye_lower                [0x13]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x13)));
    PHYMOD_DIAG_OUT(("|    veye_lower_lsbyte         [0x14]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x14)));
    PHYMOD_DIAG_OUT(("|    micro_stopped             [0x15]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x15)));
    PHYMOD_DIAG_OUT(("|    link_time                 [0x16]: 0x%04X     |\n",    falcon16_v1l4p1_rdwl_uc_var(pa,&errc,0x16)));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|            MICRO CODE MISC REGISTERS            |\n"));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    usr_diag_status           [0x18]: 0x%04X     |\n",    falcon16_v1l4p1_rdwl_uc_var(pa,&errc,0x18)));
    PHYMOD_DIAG_OUT(("|    usr_diag_rd_ptr           [0x1A]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x1a)));
    PHYMOD_DIAG_OUT(("|    usr_diag_mode             [0x1B]: 0x%04X     |\n",    falcon16_v1l4p1_rdbl_uc_var(pa,&errc,0x1b)));
    PHYMOD_DIAG_OUT(("|    usr_var_msb               [0x1C]: 0x%04X     |\n",    falcon16_v1l4p1_rdwl_uc_var(pa,&errc,0x1c)));
    PHYMOD_DIAG_OUT(("|    usr_var_lsb               [0x1E]: 0x%04X     |\n",    falcon16_v1l4p1_rdwl_uc_var(pa,&errc,0x1e)));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
}

STATIC int falcon16_v1l4p1_diagnostics_eyescan_run_uc(const phymod_phy_access_t* phy, uint32_t flags)
{
    int                 ii, rc = PHYMOD_E_NONE;
    uint32_t            stripe[64];
    uint16_t            status;
    int                 j ;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for(j=0; j< PHYMOD_CONFIG_MAX_LANES_PER_CORE; j++) { /* Loop for all lanes. */
        if ((phy->access.lane_mask & (1<<j))==0) continue;

        phy_copy.access.lane_mask = (phy->access.lane_mask & (1<<j));

        PHYMOD_IF_ERR_RETURN
            (falcon16_v1l4p1_init_falcon16_v1l4p1_info(&phy_copy.access));

        if(PHYMOD_EYESCAN_F_ENABLE_GET(flags)) {
            PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_meas_eye_scan_start(&(phy_copy.access), 0));

            if(!PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_GET(flags)) {
                PHYMOD_USLEEP(100000);
            }
        }

        if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {

            for (ii = 62; ii >= -62; ii=ii-2) {
                if (ii == 62) {
                    PHYMOD_DIAG_OUT(("\n\n\n"));
                    PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                    PHYMOD_DIAG_OUT(("    | EYESCAN Phy: 0x%03x lane_mask: 0x%02x                                 |\n", phy_copy.access.addr, phy_copy.access.lane_mask));
                    PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                    /*display eyescan header*/
                    PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_display_eye_scan_header(1));
                }

                rc  = falcon16_v1l4p1_read_eye_scan_stripe(&(phy_copy.access), stripe, &status);
                if(rc != PHYMOD_E_NONE) {
                    _falcon16_v1l4p1_diag_uc_reg_dump(&(phy_copy.access));
                    PHYMOD_IF_ERR_RETURN(rc);
                }
                PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_display_eye_scan_stripe(&(phy_copy.access), ii, &stripe[0]));

                PHYMOD_DIAG_OUT(("\n"));
            }

            if (rc == PHYMOD_E_NONE) {
                PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_display_eye_scan_footer(1));
                PHYMOD_DIAG_OUT(("\n"));
            }
        }

        if(PHYMOD_EYESCAN_F_DONE_GET(flags)) {
            PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_meas_eye_scan_done(&(phy_copy.access)));
        }
    }
    return PHYMOD_E_NONE;
}

int falcon16_v1l4p1_diagnostics_eye_margin_proj( const phymod_phy_access_t* phy, uint32_t flags,
                                        const phymod_phy_eyescan_options_t* eyescan_options) {
#ifdef SERDES_API_FLOATING_POINT
  int osr_mode;
  USR_DOUBLE data_rate, data_rate_in_Mhz;
  phymod_phy_access_t phy_copy;
  int start_lane, num_lane, i, found=0;

  struct   falcon16_v1l4p1_uc_core_config_st core_cfg;
  if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {
     PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
     PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
     for (i = 0; i < num_lane; i++) {
         phy_copy.access.lane_mask = 1 << (start_lane + i);

         if (found == 0) {
             PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_get_uc_core_config (&(phy_copy.access), &core_cfg));
             PHYMOD_IF_ERR_RETURN(falcon16_v1l4p1_osr_mode_get(&phy_copy.access, &osr_mode));
             data_rate = (core_cfg.vco_rate_in_Mhz * 1000.0 * 1000.0) / (1<<osr_mode);
             data_rate_in_Mhz = (core_cfg.vco_rate_in_Mhz) / (1<<osr_mode);
             found = 1;
         }
         if(num_lane > 1) {
             PHYMOD_DIAG_OUT((" l=%0d, data rate  = %fMHz\n", i, data_rate_in_Mhz));
         } else {
             PHYMOD_DIAG_OUT((" data rate       = %fMHz\n", data_rate_in_Mhz));
         }
         falcon16_v1l4p1_eye_margin_proj(&phy_copy.access, data_rate, eyescan_options->ber_proj_scan_mode,
                                    eyescan_options->ber_proj_timer_cnt, eyescan_options->ber_proj_err_cnt);
     }
  }
#else
  PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("BER Proj is supported with SERDES_API_FLOATING_POINT only\n")));
#endif
  return PHYMOD_E_NONE;
}


int falcon16_v1l4p1_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{        
    
    uint8_t  pmd_rx_lock=0;

    PHYMOD_IF_ERR_RETURN
      (falcon16_v1l4p1_pmd_lock_status(&phy->access, &pmd_rx_lock));
    if(pmd_rx_lock == 0) {
      PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("Can not get eyescan when pmd_rx is not locked\n")));
    }

    /* If stage isn't set - perform all stages*/
    if(!PHYMOD_EYESCAN_F_ENABLE_GET(flags)
       && !PHYMOD_EYESCAN_F_PROCESS_GET(flags)
       && !PHYMOD_EYESCAN_F_DONE_GET(flags))
    {
        PHYMOD_EYESCAN_F_ENABLE_SET(flags);
        PHYMOD_EYESCAN_F_PROCESS_SET(flags);
        PHYMOD_EYESCAN_F_DONE_SET(flags);
    }


    /* mode phymodEyescanModeBERProj gives BER projection */
    switch(mode) {
        case phymodEyescanModeFast:
            return falcon16_v1l4p1_diagnostics_eyescan_run_uc(phy, flags);
       case phymodEyescanModeBERProj:
            return falcon16_v1l4p1_diagnostics_eye_margin_proj(phy, flags, eyescan_options);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }

    return PHYMOD_E_NONE;
}


#endif /* PHYMOD_FALCON16_V1L4P1_SUPPORT */
