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
#include <phymod/phymod_util.h>
#include <phymod/phymod_config.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/chip/eagle_diagnostics.h>

#define PATTERN_MAX_LENGTH 240

#ifdef PHYMOD_EAGLE_SUPPORT

#include <phymod/chip/eagle_common.h>

#include "../../eagle/tier1/eagle_tsc_enum.h"
#include "../../eagle/tier1/eagle_tsc_common.h"
/* #include "../../eagle/tier1/eagle_tsc_common_enum.h" */
#include "../../eagle/tier1/eagle_cfg_seq.h"
#include "../../eagle/tier1/eagle_tsc_interface.h"
#include "../../eagle/tier1/eagle_tsc_dependencies.h"
#include "../../eagle/tier1/eagle_tsc_debug_functions.h"

#include "../../tsce/tier1/temod_enum_defines.h"

int eagle_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */
    PHYMOD_DEBUG_ERROR(("eagle_phy_rx_slicer_position_set function is NOT SUPPORTED!!\n"));

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */
    PHYMOD_DEBUG_ERROR(("eagle_phy_rx_slicer_position_get function is NOT SUPPORTED!!\n"));

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
        
    /* Place your code here */
    PHYMOD_DEBUG_ERROR(("eagle_phy_rx_slicer_position_max_get function is NOT SUPPORTED!!\n"));

    return PHYMOD_E_NONE;
    
}




int eagle_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
        
    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_prbs_en(&phy->access, enable));
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_prbs_en(&phy->access, enable));
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_prbs_en(&phy->access, enable));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_prbs_en(&phy->access, enable));
    }
    return PHYMOD_E_NONE;

}

int eagle_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    uint32_t enable_tmp;
    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_enable_get(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_enable_get(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_enable_get(&phy->access, &enable_tmp));
        *enable = enable_tmp;
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_enable_get(&phy->access, &enable_tmp));
        *enable &= enable_tmp;
    }

    return PHYMOD_E_NONE;
    
}


int eagle_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
        
    uint8_t status = 0;
    uint32_t prbs_err_count = 0;

    PHYMOD_IF_ERR_RETURN(eagle_tsc_prbs_chk_lock_state(&phy->access, &status));
    if (status) {
        prbs_status->prbs_lock = 1;
        /*next check the lost of lock and error count */
        status = 0;
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_prbs_err_count_state(&phy->access, &prbs_err_count, &status));
        if (status) {
        /*temp lost of lock */
            prbs_status->prbs_lock_loss = 1;
        } else {
            prbs_status->prbs_lock_loss = 0;
            prbs_status->error_count = prbs_err_count;
        }
    } else {
        prbs_status->prbs_lock = 0;
    }

    return PHYMOD_E_NONE;
    
}


int eagle_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
    int i,j = 0, bit;
    char patt[PATTERN_MAX_LENGTH+1];
   
    for (i=0; i< PATTERN_MAX_SIZE; i++)
    {
      for (j=0;j<32 && i*32+j <= PATTERN_MAX_LENGTH; j++)
      {
          if (i*32+j == (int)pattern->pattern_len) {
              break;
          }
          bit = pattern->pattern[i] >> j & 00000001;
          switch (bit) {
          case (1):
              patt[i*32+j] = '1';
              break;
          default:
              patt[i*32+j] = '0';
              break;
          }
      }
      if (i*32+j == (int)pattern->pattern_len && i*32+j <= PATTERN_MAX_LENGTH) {
          /* coverity[overrun-local] */
          patt[i*32+j] = '\0';
          break;
      } 
    }
    PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_shared_tx_pattern_idx_set(&phy->access,
                                  &pattern->pattern_len ));
    PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_shared_tx_pattern (&phy->access,
                                (uint8_t) pattern->pattern_len, (const char *) patt));

    return PHYMOD_E_NONE;
}

int eagle_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_config_shared_tx_pattern_idx_get(&phy->access,
                                  &pattern->pattern_len,
                                  pattern->pattern));
    return PHYMOD_E_NONE;
}


int eagle_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_tx_shared_patt_gen_en(&phy->access, (uint8_t) enable, (uint8_t)pattern->pattern_len));
    return PHYMOD_E_NONE;
}

int eagle_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    uint8_t enable_8;
    PHYMOD_IF_ERR_RETURN
    (eagle_tsc_tx_shared_patt_gen_en_get(&phy->access, &enable_8));
    *enable = (uint32_t) enable_8;
    return PHYMOD_E_NONE;
}


int eagle_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{
        
    
/* NOT SUPPORTED IN EAGLE YET
    signed short temperature;

    eagle_tsc_read_die_temperature(&core->access, &temperature);
    diag->temperature = temperature;
    */

    eagle_tsc_read_pll_range(&core->access, &diag->pll_range);

    return PHYMOD_E_NONE;
    
}


int eagle_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
        
    unsigned char rx_lock;
    int osr_mode;
    phymod_diag_eyescan_t_init(&diag->eyescan);
    phymod_diag_slicer_offset_t_init(&diag->slicer_offset);

    PHYMOD_IF_ERR_RETURN(eagle_tsc_pmd_lock_status(&phy->access, &rx_lock));
    diag->rx_lock = (uint32_t ) rx_lock;
    PHYMOD_IF_ERR_RETURN(eagle_osr_mode_get(&phy->access, &osr_mode));
    PHYMOD_IF_ERR_RETURN(eagle_osr_mode_to_enum(osr_mode, &diag->osr_mode));
    PHYMOD_IF_ERR_RETURN(eagle_tsc_signal_detect(&phy->access, &diag->signal_detect));

    return PHYMOD_E_NONE;
    
}

STATIC err_code_t eagle_phy_meas_lowber_eye (const phymod_access_t *pa, 
                                             const phymod_phy_eyescan_options_t *eyescan_options,
                                             uint32_t *buffer) 
{
    struct eagle_tsc_eyescan_options_st e_options;

    e_options.linerate_in_khz = eyescan_options->linerate_in_khz;
    e_options.timeout_in_milliseconds = eyescan_options->timeout_in_milliseconds;
    e_options.horz_max = eyescan_options->horz_max;
    e_options.horz_min = eyescan_options->horz_min;
    e_options.hstep = eyescan_options->hstep;
    e_options.vert_max = eyescan_options->vert_max;
    e_options.vert_min = eyescan_options->vert_min;
    e_options.vstep = eyescan_options->vstep;
    e_options.mode = eyescan_options->mode;

    return (eagle_tsc_meas_lowber_eye(pa, e_options, buffer));
}

STATIC err_code_t eagle_phy_display_lowber_eye (const phymod_access_t *pa, 
                                                const phymod_phy_eyescan_options_t *eyescan_options,
                                                uint32_t *buffer) 
{
    struct eagle_tsc_eyescan_options_st e_options;

    e_options.linerate_in_khz = eyescan_options->linerate_in_khz;
    e_options.timeout_in_milliseconds = eyescan_options->timeout_in_milliseconds;
    e_options.horz_max = eyescan_options->horz_max;
    e_options.horz_min = eyescan_options->horz_min;
    e_options.hstep = eyescan_options->hstep;
    e_options.vert_max = eyescan_options->vert_max;
    e_options.vert_min = eyescan_options->vert_min;
    e_options.vstep = eyescan_options->vstep;
    e_options.mode = eyescan_options->mode;

    return(eagle_tsc_display_lowber_eye (pa, e_options, buffer)); 
} 


static void  _eagle_diag_uc_reg_dump(const phymod_access_t *pa)
{
    err_code_t errc =  PHYMOD_E_NONE;

    COMPILER_REFERENCE(errc);

    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|    MICRO CODE USR CTRL CONFIGURATION REGISTERS  |\n"));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|    config_word              [0x00]: 0x%04X      |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x0)));
    PHYMOD_DEBUG_ERROR(("|    retune_after_restart     [0x02]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x2)));
    PHYMOD_DEBUG_ERROR(("|    clk90_offset_adjust      [0x03]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x3)));
    PHYMOD_DEBUG_ERROR(("|    clk90_offset_override    [0x04]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x4)));
    PHYMOD_DEBUG_ERROR(("|    lane_event_log_level     [0x05]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x5)));
    PHYMOD_DEBUG_ERROR(("|    disable_startup          [0x06]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x6)));
    PHYMOD_DEBUG_ERROR(("|    disable_startup_dfe      [0x07]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x7)));
    PHYMOD_DEBUG_ERROR(("|    disable_steady_state     [0x08]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x8)));
    PHYMOD_DEBUG_ERROR(("|    disable_steady_state_dfe [0x09]: 0x%04X      |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x9)));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|         MICRO CODE USER STATUS REGISTERS        |\n"));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|    restart_counter           [0x0A]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xa)));
    PHYMOD_DEBUG_ERROR(("|    reset_counter             [0x0B]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xb)));
    PHYMOD_DEBUG_ERROR(("|    pmd_lock_counter          [0x0C]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xc)));
    PHYMOD_DEBUG_ERROR(("|    heye_left                 [0x0D]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xd)));
    PHYMOD_DEBUG_ERROR(("|    heye_left_lsbyte          [0x0E]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xe)));
    PHYMOD_DEBUG_ERROR(("|    heye_right                [0x0F]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0xf)));
    PHYMOD_DEBUG_ERROR(("|    heye_right_lsbyte         [0x10]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x10)));
    PHYMOD_DEBUG_ERROR(("|    veye_upper                [0x11]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x11)));
    PHYMOD_DEBUG_ERROR(("|    veye_upper_lsbyte         [0x12]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x12)));
    PHYMOD_DEBUG_ERROR(("|    veye_lower                [0x13]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x13)));
    PHYMOD_DEBUG_ERROR(("|    veye_lower_lsbyte         [0x14]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x14)));
    PHYMOD_DEBUG_ERROR(("|    micro_stopped             [0x15]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x15)));
    PHYMOD_DEBUG_ERROR(("|    link_time                 [0x16]: 0x%04X     |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x16)));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|            MICRO CODE MISC REGISTERS            |\n"));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));
    PHYMOD_DEBUG_ERROR(("|    usr_diag_status           [0x18]: 0x%04X     |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x18)));
    PHYMOD_DEBUG_ERROR(("|    usr_diag_rd_ptr           [0x1A]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x1a)));
    PHYMOD_DEBUG_ERROR(("|    usr_diag_mode             [0x1B]: 0x%04X     |\n",    eagle_tsc_rdbl_uc_var(pa,&errc,0x1b)));
    PHYMOD_DEBUG_ERROR(("|    usr_var_msb               [0x1C]: 0x%04X     |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x1c)));
    PHYMOD_DEBUG_ERROR(("|    usr_var_lsb               [0x1E]: 0x%04X     |\n",    eagle_tsc_rdwl_uc_var(pa,&errc,0x1e)));
    PHYMOD_DEBUG_ERROR(("+-------------------------------------------------+\n"));

}

STATIC int eagle_diagnostics_eyescan_run_uc(const phymod_phy_access_t* phy, uint32_t flags)   
{
    int                 j;
    int                 ii, rc = PHYMOD_E_NONE;
    uint32_t            stripe[64];
    uint16_t            status;
    phymod_phy_access_t phy_copy;
    

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for(j=0; j< PHYMOD_CONFIG_MAX_LANES_PER_CORE; j++) { /* Loop for all lanes. */
      if ((phy->access.lane_mask & (1<<j))==0) continue;

      phy_copy.access.lane_mask = (phy->access.lane_mask & (1<<j));

      if(PHYMOD_EYESCAN_F_ENABLE_GET(flags)) {
          PHYMOD_IF_ERR_RETURN(eagle_tsc_meas_eye_scan_start(&(phy_copy.access), 0));

          if(!PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_GET(flags)) {
              PHYMOD_USLEEP(100000);
          }
      }

      if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {

          for (ii = 31; ii >= -31; ii--) { 
              if (ii == 31) {
                  PHYMOD_DIAG_OUT(("\n\n\n"));
                  PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                  PHYMOD_DIAG_OUT(("    | EYESCAN Phy: 0x%02x lane_mask 0x%02x                                   |\n", phy_copy.access.addr, phy_copy.access.lane_mask));
                  PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                  /*display eyescan header*/
                  PHYMOD_IF_ERR_RETURN(eagle_tsc_display_eye_scan_header(&(phy_copy.access), 1));
              }

              rc  = eagle_tsc_read_eye_scan_stripe(&(phy_copy.access), stripe, &status);
              if(rc != PHYMOD_E_NONE) {
                _eagle_diag_uc_reg_dump(&(phy_copy.access));
                PHYMOD_IF_ERR_RETURN(rc);
              }
              PHYMOD_IF_ERR_RETURN(eagle_tsc_display_eye_scan_stripe(&(phy_copy.access), ii, &stripe[0]));

              PHYMOD_DIAG_OUT(("\n"));
          }

          if (rc == PHYMOD_E_NONE) {
            PHYMOD_IF_ERR_RETURN(eagle_tsc_display_eye_scan_footer(&(phy_copy.access), 1));
            PHYMOD_DIAG_OUT(("\n"));
          }
      }
    
      if(PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_meas_eye_scan_done(&(phy_copy.access)));
      }
    }

    return PHYMOD_E_NONE;
}

STATIC int eagle_diagnostics_eyescan_run_lowber( 
    const phymod_phy_access_t* phy,
    uint32_t flags,
    const phymod_phy_eyescan_options_t* eyescan_options
    )   
{
    uint32_t *buffer;
    int buffer_size = 64*64;
    int rv = PHYMOD_E_NONE;

    buffer = PHYMOD_MALLOC(sizeof(uint32_t) * buffer_size, "buffer");
    if (NULL == buffer) {
        return PHYMOD_E_MEMORY;
    }
    PHYMOD_MEMSET(buffer, 0, sizeof(uint32_t) * buffer_size);

    /*enable eyescan*/
    if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {

        rv = eagle_phy_meas_lowber_eye (&(phy->access), eyescan_options, buffer);
        if (rv < 0) {
            goto cleanup;
        }
        rv = eagle_phy_display_lowber_eye (&(phy->access), eyescan_options, buffer);
        if (rv < 0) {
            goto cleanup;
        }
    }
    if (buffer != NULL) {
        PHYMOD_FREE(buffer);
    }

    if(PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_pmd_uc_cmd(&(phy->access), CMD_CAPTURE_BER_END, 0, 2000));
    }

    return PHYMOD_E_NONE;

cleanup:
    if (buffer != NULL) {
        PHYMOD_FREE(buffer);
    }

    return rv;
}

STATIC int eagle_diagnostics_eye_margin_proj( const phymod_phy_access_t* phy, uint32_t flags,
                                        const phymod_phy_eyescan_options_t* eyescan_options) {
#ifdef SERDES_API_FLOATING_POINT
  int osr_mode;
  USR_DOUBLE data_rate, data_rate_in_Mhz;
  struct   eagle_tsc_uc_core_config_st core_cfg;
  phymod_phy_access_t phy_copy;
  int start_lane, num_lane, i, found=0;

  if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {
      PHYMOD_IF_ERR_RETURN
          (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
      PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
      for (i = 0; i < num_lane; i++) {
          phy_copy.access.lane_mask = 1 << (start_lane + i);

          if (found == 0) {
              PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_core_config (&(phy_copy.access), &core_cfg));
              PHYMOD_IF_ERR_RETURN(eagle_osr_mode_get(&phy_copy.access, &osr_mode));
              data_rate = (core_cfg.vco_rate_in_Mhz * 1000.0 * 1000.0) / (1<<osr_mode);
              data_rate_in_Mhz = (core_cfg.vco_rate_in_Mhz) / (1<<osr_mode);
              found = 1;
          }
          if(num_lane > 1) {
              PHYMOD_DIAG_OUT((" l=%0d, data rate  = %fMHz\n", i, data_rate_in_Mhz));
          } else {
              PHYMOD_DIAG_OUT((" data rate       = %fMHz\n", data_rate_in_Mhz));
          }
          eagle_tsc_eye_margin_proj(&phy_copy.access, data_rate, eyescan_options->ber_proj_scan_mode,
                                    eyescan_options->ber_proj_timer_cnt, eyescan_options->ber_proj_err_cnt);
      }
  }
#else
      PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("BER Proj is supported with SERDES_API_FLOATING_POINT only\n")));
#endif
  return PHYMOD_E_NONE;
}

int eagle_phy_eyescan_run(const phymod_phy_access_t* phy, 
                           uint32_t flags, 
                           phymod_eyescan_mode_t mode, 
                           const phymod_phy_eyescan_options_t* eyescan_options)
{
    /* If stage isn't set - perform all stages*/
    if(!PHYMOD_EYESCAN_F_ENABLE_GET(flags)
       && !PHYMOD_EYESCAN_F_PROCESS_GET(flags)
       && !PHYMOD_EYESCAN_F_DONE_GET(flags)) 
    {
        PHYMOD_EYESCAN_F_ENABLE_SET(flags);
        PHYMOD_EYESCAN_F_PROCESS_SET(flags);
        PHYMOD_EYESCAN_F_DONE_SET(flags);
    }


    switch(mode) {
        case phymodEyescanModeFast:
            return eagle_diagnostics_eyescan_run_uc(phy, flags);
        case phymodEyescanModeLowBER:
            return eagle_diagnostics_eyescan_run_lowber(phy, flags, eyescan_options);
        case phymodEyescanModeBERProj:
            return eagle_diagnostics_eye_margin_proj(phy, flags, eyescan_options);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }
}

/*phymod, internal enum mappings*/
STATIC
int _eagle_prbs_poly_phymod_to_eagle(phymod_prbs_poly_t phymod_poly, enum srds_prbs_polynomial_enum *eagle_poly)
{
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *eagle_poly = PRBS_7;
        break;
    case phymodPrbsPoly9:
        *eagle_poly = PRBS_9;
        break;
    case phymodPrbsPoly11:
        *eagle_poly = PRBS_11;
        break;
    case phymodPrbsPoly15:
        *eagle_poly = PRBS_15;
        break;
    case phymodPrbsPoly23:
        *eagle_poly = PRBS_23;
        break;
    case phymodPrbsPoly31:
        *eagle_poly = PRBS_31;
        break;
    case phymodPrbsPoly58:
        *eagle_poly = PRBS_58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for tsce %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _eagle_prbs_poly_tsce_to_phymod(eagle_prbs_polynomial_type_t tsce_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch(tsce_poly){
    case EAGLE_PRBS_POLYNOMIAL_7:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case EAGLE_PRBS_POLYNOMIAL_9:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case EAGLE_PRBS_POLYNOMIAL_11:
        *phymod_poly = phymodPrbsPoly11;
        break;
    case EAGLE_PRBS_POLYNOMIAL_15:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case EAGLE_PRBS_POLYNOMIAL_23:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case EAGLE_PRBS_POLYNOMIAL_31:
        *phymod_poly = phymodPrbsPoly31;
        break;
    case EAGLE_PRBS_POLYNOMIAL_58:
        *phymod_poly = phymodPrbsPoly58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown poly %u"), tsce_poly));
    }
    return PHYMOD_E_NONE;
}

int eagle_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int i = 0;
    /*
    int j = 0;
    struct eagle_tsc_detailed_lane_status_st lane_st[4];
    */
    uint32_t cmd_type;
    uint8_t trace_mem[768];

    if (!type) {
        cmd_type = (uint32_t)TEMOD_DIAG_STATE;
    } else if (!PHYMOD_STRCMP(type, "ber")) {
        cmd_type = (uint32_t)TEMOD_DIAG_BER;
    } else if (!PHYMOD_STRCMP(type, "config")) {
        cmd_type = (uint32_t)TEMOD_DIAG_CFG;
    } else if (!PHYMOD_STRCMP(type, "CL72")) {
        cmd_type = (uint32_t)TEMOD_DIAG_CL72;
    } else if (!PHYMOD_STRCMP(type, "debug")) {
        cmd_type = (uint32_t)TEMOD_DIAG_DEBUG;
    } else if (!PHYMOD_STRCMP(type, "STD")) {
        cmd_type = (uint32_t)TEMOD_DIAG_DSC_STD;
    } else {
        cmd_type = (uint32_t)TEMOD_DIAG_STATE;
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);

        switch (cmd_type) {
            case TEMOD_DIAG_CFG:
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_core_config(&phy_copy.access));
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_lane_config(&phy_copy.access));
                 break;

            case TEMOD_DIAG_CL72:
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_cl72_status(&phy_copy.access));
                 break;

            case TEMOD_DIAG_DEBUG:
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_lane_debug_status(&phy_copy.access));
                 break;

            case TEMOD_DIAG_BER:
                 break;

            case TEMOD_DIAG_STATE:
            case TEMOD_DIAG_DSC_STD:
            default:
                PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                PHYMOD_DIAG_OUT(("    | DSC Phy: 0x%03x lane_mask: 0x%02x                                 |\n", phy->access.addr, phy->access.lane_mask));
                PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_core_state(&phy_copy.access));
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_lane_state_hdr(&phy_copy.access));
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_lane_state(&phy_copy.access));
                    /*
                     NOT SUPPORTED YET in EAGle: eagle_tsc_log_full_pmd_state and eagle_tsc_disp_full_pmd_state
                 for (j = 0; j < 4; j++) {
                   PHYMOD_IF_ERR_RETURN
                      (eagle_tsc_log_full_pmd_state(&phy_copy.access, &lane_st[j]));
                 }
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_disp_full_pmd_state(&phy_copy.access, lane_st, 4));
                    */
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_read_event_log((&phy_copy.access), trace_mem, 2));
                 break;
        }
    }

    return PHYMOD_E_NONE;
}


int eagle_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags ,  phymod_prbs_t* prbs)
{
        
    phymod_prbs_t config_tmp;
    eagle_prbs_polynomial_type_t tsce_poly;

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_inv_data_get(&phy->access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_poly_get(&phy->access, &tsce_poly));
        PHYMOD_IF_ERR_RETURN(_eagle_prbs_poly_tsce_to_phymod(tsce_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_inv_data_get(&phy->access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_poly_get(&phy->access, &tsce_poly));
        PHYMOD_IF_ERR_RETURN(_eagle_prbs_poly_tsce_to_phymod(tsce_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_inv_data_get(&phy->access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_poly_get(&phy->access, &tsce_poly));
        PHYMOD_IF_ERR_RETURN(_eagle_prbs_poly_tsce_to_phymod(tsce_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    }
    return PHYMOD_E_NONE;

    
}

int eagle_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
        
    
    enum srds_prbs_polynomial_enum eagle_poly;
    PHYMOD_IF_ERR_RETURN(_eagle_prbs_poly_phymod_to_eagle(prbs->poly, &eagle_poly));
    /*first check which direction */
    if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_rx_prbs(&phy->access, eagle_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
    } else if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_tx_prbs(&phy->access, eagle_poly, prbs->invert));
    } else {
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_rx_prbs(&phy->access, eagle_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_tx_prbs(&phy->access, eagle_poly, prbs->invert));
    }
        
    return PHYMOD_E_NONE;
    
}



#endif /* PHYMOD_EAGLE_SUPPORT */
